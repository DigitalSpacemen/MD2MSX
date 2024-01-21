/* Copyright (C) 2024  Walter van Niftrik, Kris McAulay
 *
 * This file is part of MD2MSX.
 * 
 * MD2MSX is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * MD2MSX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MD2MSX. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <digitalWriteFast.h>
#include "delay.h"

#if !defined(PCB_VER_MAJOR) || !defined(PCB_VER_MINOR)
#error "PCB version not specified"
#endif

#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
public:
	Controller();
	void init() const;
	void go();
	void debug();

private:
	enum Type {
		None, ThreeButton, SixButton
	};

	enum Button {
		Up, Down, Left, Right, A, B, C, Start,
		X, Y, Z, Mode,
		Home, // 8BitDo-only
		Count
	};

	void resetMSXTimer() const;
	bool isMSXTimerDone() const;
	void resetControllerTimer() const;
	bool isControllerTimerDone() const;

	void readControllerButtons();

	template <byte CYCLENR>
	inline void writeMSXButtons() const __attribute__((always_inline));

	template <byte CYCLENR>
	inline bool hasMSXCycleChanged() const __attribute__((always_inline));

	template <byte CYCLENR>
	inline bool handleMSXCycle() __attribute__((always_inline));

	Type getControllerType() const;
	void printState() const;
	bool getButton(Button button) const;
	void printButton(Button button, const char *name) const;

	static constexpr int kCycles = 8;

	// Shorthand for checking PCB version
	static constexpr bool pcbVersion(byte major, byte minor) {
		return PCB_VER_MAJOR == major && PCB_VER_MINOR == minor;
	};

	// Maps controller pin to Arduino pin
	static constexpr byte mapControllerPin(byte cPin) {
		if (pcbVersion(1, 2)) {
			// PCB version 1.2
			constexpr byte pinMap[] = { 0, 2, 4, 5, 6, 0, 3, 8, 0, 7 };
			return pinMap[cPin];
		} else if (pcbVersion(1, 1)) {
			// PCB version 1.1
			constexpr byte pinMap[] = { 0, 3, 5, 7, 8, 0, 4, 6, 0, 9 };
			return pinMap[cPin];
		} else {
			// PCB version 1.0
			constexpr byte pinMap[] = { 0, 2, 4, 6, 7, 0, 3, 5, 0, 8 };
			return pinMap[cPin];
		}
	};

	// Maps MSX pin to Arduino pin
	static constexpr byte mapMSXPin(byte mPin) {
		if (pcbVersion(1, 2)) {
			// PCB version 1.2
			constexpr byte pinMap[] = { 0, 14, 16, 17, 18, 0, 15, 19, 13, 0 };
			return pinMap[mPin];
		} else if (pcbVersion(1, 1)) {
			// PCB version 1.1
			constexpr byte pinMap[] = { 0, 19, 17, 15, 14, 0, 18, 16, 2, 0 };
			return pinMap[mPin];
		} else {
			// PCB version 1.0
			constexpr byte pinMap[] = { 0, 9, 19, 16, 14, 0, 18, 17, 15, 0 };
			return pinMap[mPin];
		}
	};

	// Bitmask for a single button in a cycle
	static constexpr byte buttonBit(Button b) {
		// Maps button to DE-9 input pin
		constexpr byte buttonToPin[Button::Count] = {
			1, 2, 3, 4, 6, 6, 9, 9,
			3, 2, 1, 4,
			1
		};

		// Pins are right-shifted two bits in _cycles
		return bit(mapControllerPin(buttonToPin[b]) - 2);
	};

	// Bitmask for all buttons in a cycle
	static constexpr byte buttonMask() {
		byte mask = 0;

		for (byte b = 0; b < Button::Count; ++b)
			mask |= buttonBit(static_cast<Button>(b));

		return mask;
	};

	// Cache for cycles read from the controller
	byte _cycles[kCycles];
	// Next cycle to read
	byte _controllerCycle;
};

bool Controller::getButton(Controller::Button button) const {
	// Maps button to input read cycle
	constexpr byte buttonCycle[Button::Count] = {
		0, 0, 0, 0, 3, 0, 0, 3,
		6, 6, 6, 6,
		7
	};

	return _cycles[buttonCycle[button]] & buttonBit(button);
}

Controller::Controller() :
		_controllerCycle(0) {
	pinModeFast(LED_BUILTIN, OUTPUT);

	pinModeFast(mapControllerPin(1), INPUT_PULLUP);
	pinModeFast(mapControllerPin(2), INPUT_PULLUP);
	pinModeFast(mapControllerPin(3), INPUT_PULLUP);
	pinModeFast(mapControllerPin(4), INPUT_PULLUP);
	pinModeFast(mapControllerPin(6), INPUT_PULLUP);
	pinModeFast(mapControllerPin(9), INPUT_PULLUP);

	pinModeFast(mapControllerPin(7), INPUT_PULLUP);
	pinModeFast(mapControllerPin(7), OUTPUT);

	pinModeFast(mapMSXPin(1), INPUT_PULLUP);
	pinModeFast(mapMSXPin(1), OUTPUT);
	pinModeFast(mapMSXPin(2), INPUT_PULLUP);
	pinModeFast(mapMSXPin(2), OUTPUT);
	pinModeFast(mapMSXPin(3), INPUT_PULLUP);
	pinModeFast(mapMSXPin(3), OUTPUT);
	pinModeFast(mapMSXPin(4), INPUT_PULLUP);
	pinModeFast(mapMSXPin(4), OUTPUT);
	pinModeFast(mapMSXPin(6), INPUT_PULLUP);
	pinModeFast(mapMSXPin(6), OUTPUT);
	pinModeFast(mapMSXPin(7), INPUT_PULLUP);
	pinModeFast(mapMSXPin(7), OUTPUT);

	pinModeFast(mapMSXPin(8), INPUT_PULLUP);

	for (byte &c : _cycles)
		c = UINT8_MAX;
};

void Controller::init() const {
	// Initialize Timer1
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// Disable all Timer1 interrupts
	TIMSK1 = 0;

	// Reset Timer1 compare A flags
	TIFR1 |= _BV(OCF1A);

	// Set Timer1 compare A to 1.5ms
	OCR1A = 15 * F_CPU / 10000;

	// Start Timer1 without prescaling
	TCCR1B = _BV(CS10);

	// Initialize Timer2
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;

	// Disable all Timer2 interrupts
	TIMSK2 = 0;

	// Reset Timer2 flags
	TIFR2 |= _BV(OCF2A);

	// Set Timer2 compare A to 3ms
	OCR2A = 3 * F_CPU / 1000 / 256;

	// Start Timer2 with 256 prescaling
	TCCR2B = _BV(CS22) | _BV(CS21);
}

void Controller::readControllerButtons() {
	if (pcbVersion(1, 2))
		_cycles[_controllerCycle] = PIND >> 2;
	else
		_cycles[_controllerCycle] = ((PINB << 8) | PIND) >> 2;

	_controllerCycle = (_controllerCycle + 1) & 7;
	digitalToggleFast(mapControllerPin(7));
}

template <byte CYCLENR>
void Controller::writeMSXButtons() const {
	const byte cycle = _cycles[CYCLENR];

	if (pcbVersion(1, 2)) {
		PORTC = cycle;
	} else {
		digitalWriteFast(mapMSXPin(1), cycle & bit(mapControllerPin(1) - 2));
		digitalWriteFast(mapMSXPin(2), cycle & bit(mapControllerPin(2) - 2));
		digitalWriteFast(mapMSXPin(4), cycle & bit(mapControllerPin(4) - 2));
		digitalWriteFast(mapMSXPin(6), cycle & bit(mapControllerPin(6) - 2));
		digitalWriteFast(mapMSXPin(7), cycle & bit(mapControllerPin(9) - 2));

		// FIXME: Put back
		digitalWriteFast(mapMSXPin(3), cycle & bit(mapControllerPin(3) - 2));
	}
}

template <byte CYCLENR>
bool Controller::hasMSXCycleChanged() const {
	return (CYCLENR & 1) != digitalReadFast(mapMSXPin(8));
}

template <byte CYCLENR>
bool Controller::handleMSXCycle() {
	while (!hasMSXCycleChanged<CYCLENR>()) {
		writeMSXButtons<CYCLENR & 7>();

		if (_controllerCycle == 0) {
			while (!isControllerTimerDone()) {
				if (hasMSXCycleChanged<CYCLENR>())
					return false;
				if (CYCLENR >= 2 && isMSXTimerDone())
					return true;
			}
			resetControllerTimer();
		}

		if (hasMSXCycleChanged<CYCLENR>())
			return false;

		readControllerButtons();
	}

	return false;
}

void Controller::resetMSXTimer() const {
	// Set timer to 0
	TCNT1 = 0;
	// Clear compare A flag
	TIFR1 |= bit(OCF1A);
}

bool Controller::isMSXTimerDone() const {
	// Check compare A flag
	return TIFR1 & _BV(OCF1A);
}

void Controller::resetControllerTimer() const {
	// Set timer to 0
	TCNT2 = 0;
	// Clear compare A flag
	TIFR2 |= bit(OCF1A);
}

bool Controller::isControllerTimerDone() const {
	// Check compare A flag
	return TIFR2 & _BV(OCF2A);
}

void Controller::go() {
#ifdef DEBUG
	printState();
#endif

	handleMSXCycle<0>();
	handleMSXCycle<1>();

	// Restart the timer
	resetMSXTimer();

	if (handleMSXCycle<2>())
		return;

	if (handleMSXCycle<3>())
		return;

	if (handleMSXCycle<4>())
		return;

	if (handleMSXCycle<5>())
		return;

	if (handleMSXCycle<6>())
		return;

	if (handleMSXCycle<7>())
		return;

	for (;;) {
		if (handleMSXCycle<8>())
			return;

		if (handleMSXCycle<9>())
			return;
	}
}

Controller::Type Controller::getControllerType() const {
	if ((_cycles[7] & buttonBit(Button::Left)) && (_cycles[7] & buttonBit(Button::Right))) {
		if (!(_cycles[5] & buttonBit(Button::Left)) && !(_cycles[5] & buttonBit(Button::Right)))
			return SixButton;
		else
			return None;
	}

	return ThreeButton;
}

void Controller::printButton(Button button, const char *name) const {
	if (!getButton(button)) {
		Serial.print(name);
		Serial.print(' ');
	}
}

void Controller::printState() const {
	Serial.println();

	for (byte c = 0; c < kCycles; ++c) {
		Serial.print(c);
		Serial.print(": ");
		Serial.println(_cycles[c], BIN);
	}

	Type tp = getControllerType();

	if (tp == Type::ThreeButton)
		Serial.print("3 Button Controller: ");
	else if (tp == Type::SixButton)
		Serial.print("6 Button Controller: ");
	else {
		Serial.println("No Controller");
		return;
	}

	printButton(Button::Up, "Up");
	printButton(Button::Down, "Down");
	printButton(Button::Left, "Left");
	printButton(Button::Right, "Right");

	printButton(Button::A, "A");
	printButton(Button::B, "B");
	printButton(Button::C, "C");
	printButton(Button::Start, "Start");

	if (tp == Type::SixButton) {
		printButton(Button::X, "X");
		printButton(Button::Y, "Y");
		printButton(Button::Z, "Z");
		printButton(Button::Mode, "Mode");
		printButton(Button::Home, "Home"); // 8BitDo-only
	}

	Serial.println();
}

void Controller::debug() {
	Serial.setTimeout(10000);

	Serial.println("\n--- Controller Tester ---\n");

	double cycleDelayMicro = 3.0;
	long seqDelay = 10000;

	Serial.print("Delay between cycles in us (3.00): ");
	String str = Serial.readStringUntil('\r');

	if (str.length() > 0 && str.charAt(str.length() - 1) != '\n')
		cycleDelayMicro = str.toDouble();

	Serial.println(cycleDelayMicro);

	word cycleDelay = cycleDelayMicro * 4;

	Serial.print("Delay between sequences in us (10000): ");
	str = Serial.readStringUntil('\r');

	if (str.length() > 0 && str.charAt(str.length() - 1) != '\n')
		seqDelay = str.toInt();

	Serial.println(seqDelay);

	byte oldCycles[kCycles] = { 0 };

	for (;;) {
		if (seqDelay)
			delayMicroseconds(seqDelay);

		noInterrupts();

		for (byte c = 0; c < kCycles; ++c) {
			_cycles[c] = ((PINB << 8) | PIND) >> 2;
			digitalToggleFast(mapControllerPin(7));
			if (cycleDelay)
				delay_qus(cycleDelay);
		}

		interrupts();

		for (byte &c : _cycles)
			c |= ~buttonMask();

	for (byte c = 0; c < kCycles; ++c) {
			if (oldCycles[c] != _cycles[c]) {
				printState();
				break;
			}
		}

		for (byte c = 0; c < kCycles; ++c)
			oldCycles[c] = _cycles[c];
	}
}

#endif // CONTROLLER_H
