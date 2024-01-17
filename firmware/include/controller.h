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
#include "config.h"
#include "delay.h"

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

	void resetOutTimer() const;
	bool isOutTimerDone() const;
	void resetInTimer() const;
	bool isInTimerDone() const;

	void readCycle();

	template <byte CYCLENR>
	inline void writeCycle() const __attribute__((always_inline));

	template <byte CYCLENR>
	inline bool hasCycleChanged() const __attribute__((always_inline));

	template <byte CYCLENR>
	inline bool outputCycle() __attribute__((always_inline));

	Type getControllerType() const;
	Type getType() const;
	void printState() const;

	template <Button B>
	bool getButton() const;

	template <Button B>
	void printButton(const char *name) const;

	static constexpr int kCycles = 8;

	// Maps button to input read cycle
	static constexpr byte kButtonCycle[Button::Count] = {
		0, 0, 0, 0, 3, 0, 0, 3,
		6, 6, 6, 6,
		7
	};

	// Maps button to DE-9 input pin
	static constexpr byte kButtonInputPin[Button::Count] = {
		1, 2, 3, 4, 6, 6, 9, 9,
		3, 2, 1, 4,
		1
	};

	// Maps DE-9 input pin to Arduino pin
	static constexpr byte kInPinMap[] = {
#ifdef BOARD_REV_1_1
		0, 3, 5, 7, 8, 0, 4, 6, 0, 9
#else
		0, 2, 4, 6, 7, 0, 3, 5, 0, 8
#endif
	};

	// Maps DE-9 output pin to Arduino pin
	static constexpr byte kOutPinMap[] = {
#ifdef BOARD_REV_1_1
		0, 19, 17, 15, 14, 0, 18, 16, 2, 0
#else
		0, 9, 19, 16, 14, 0, 18, 17, 15, 0
#endif
	};

	constexpr byte buttonBit(Button b) const {
		return bit(kInPinMap[kButtonInputPin[b]] - 2);
	};

	constexpr byte buttonMask() const {
		byte retval = 0;

		for (byte b = 0; b < Button::Count; ++b)
			retval |= buttonBit(static_cast<Button>(b));

		return retval;
	};

	byte _inCycles[kCycles];
	byte _curInCycle;
};

template <Controller::Button B>
bool Controller::getButton() const {
	return _inCycles[kButtonCycle[B]] & buttonBit(B);
}

Controller::Controller() :
		_curInCycle(0) {
	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(kInPinMap[1], INPUT_PULLUP);
	pinMode(kInPinMap[2], INPUT_PULLUP);
	pinMode(kInPinMap[3], INPUT_PULLUP);
	pinMode(kInPinMap[4], INPUT_PULLUP);
	pinMode(kInPinMap[6], INPUT_PULLUP);
	pinMode(kInPinMap[9], INPUT_PULLUP);

	pinMode(kInPinMap[7], INPUT_PULLUP);
	pinMode(kInPinMap[7], OUTPUT);

	pinMode(kOutPinMap[1], INPUT_PULLUP);
	pinMode(kOutPinMap[1], OUTPUT);
	pinMode(kOutPinMap[2], INPUT_PULLUP);
	pinMode(kOutPinMap[2], OUTPUT);
	pinMode(kOutPinMap[3], INPUT_PULLUP);
	pinMode(kOutPinMap[3], OUTPUT);
	pinMode(kOutPinMap[4], INPUT_PULLUP);
	pinMode(kOutPinMap[4], OUTPUT);
	pinMode(kOutPinMap[6], INPUT_PULLUP);
	pinMode(kOutPinMap[6], OUTPUT);
	pinMode(kOutPinMap[7], INPUT_PULLUP);
	pinMode(kOutPinMap[7], OUTPUT);

	pinMode(kOutPinMap[8], INPUT_PULLUP);

	for (byte &c : _inCycles)
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

void Controller::readCycle() {
	_inCycles[_curInCycle] = ((PINB << 8) | PIND) >> 2;
	_curInCycle = (_curInCycle + 1) & 7;
	digitalToggleFast(kInPinMap[7]);
}

template <byte CYCLENR>
void Controller::writeCycle() const {
	const byte cycle = _inCycles[CYCLENR];

	digitalWriteFast(kOutPinMap[1], cycle & bit(kInPinMap[1] - 2));
	digitalWriteFast(kOutPinMap[2], cycle & bit(kInPinMap[2] - 2));
	digitalWriteFast(kOutPinMap[4], cycle & bit(kInPinMap[4] - 2));
	digitalWriteFast(kOutPinMap[6], cycle & bit(kInPinMap[6] - 2));
	digitalWriteFast(kOutPinMap[7], cycle & bit(kInPinMap[9] - 2));

	// FIXME: Put back
	digitalWriteFast(kOutPinMap[3], cycle & bit(kInPinMap[3] - 2));
}

template <byte CYCLENR>
bool Controller::hasCycleChanged() const {
	return (CYCLENR & 1) != digitalReadFast(kOutPinMap[8]);
}

template <byte CYCLENR>
bool Controller::outputCycle() {
	while (!hasCycleChanged<CYCLENR>()) {
		writeCycle<CYCLENR & 7>();

		if (!_curInCycle) {
			while (!isInTimerDone()) {
				if (hasCycleChanged<CYCLENR>())
					return false;
				if (CYCLENR >= 2 && isOutTimerDone())
					return true;
			}
			resetInTimer();
		}

		if (hasCycleChanged<CYCLENR>())
			return false;

		readCycle();
	}

	return false;
}

void Controller::resetOutTimer() const {
	// Set timer to 0
	TCNT1 = 0;
	// Clear compare A flag
	TIFR1 |= bit(OCF1A);
}

bool Controller::isOutTimerDone() const {
	// Check compare A flag
	return TIFR1 & _BV(OCF1A);
}

void Controller::resetInTimer() const {
	// Set timer to 0
	TCNT2 = 0;
	// Clear compare A flag
	TIFR2 |= bit(OCF1A);
}

bool Controller::isInTimerDone() const {
	// Check compare A flag
	return TIFR2 & _BV(OCF2A);
}

void Controller::go() {
	if (DEBUG)
		printState();

	outputCycle<0>();
	outputCycle<1>();

	// Restart the timer
	resetOutTimer();

	if (outputCycle<2>())
		return;

	if (outputCycle<3>())
		return;

	if (outputCycle<4>())
		return;

	if (outputCycle<5>())
		return;

	if (outputCycle<6>())
		return;

	if (outputCycle<7>())
		return;

	for (;;) {
		if (outputCycle<8>())
			return;

		if (outputCycle<9>())
			return;
	}
}

Controller::Type Controller::getType() const {
	if ((_inCycles[7] & buttonBit(Button::Left)) && (_inCycles[7] & buttonBit(Button::Right))) {
		if (!(_inCycles[5] & buttonBit(Button::Left)) && !(_inCycles[5] & buttonBit(Button::Right)))
			return SixButton;
		else
			return None;
	}

	return ThreeButton;
}

template <Controller::Button B>
void Controller::printButton(const char *name) const {
	if (!getButton<B>()) {
		Serial.print(name);
		Serial.print(' ');
	}
}

void Controller::printState() const {
	Serial.println();

	for (byte c = 0; c < kCycles; ++c) {
		Serial.print(c);
		Serial.print(": ");
		Serial.println(_inCycles[c], BIN);
	}

	Type tp = getType();

	if (tp == Type::ThreeButton)
		Serial.print("3 Button Controller: ");
	else if (tp == Type::SixButton)
		Serial.print("6 Button Controller: ");
	else {
		Serial.println("No Controller");
		return;
	}

	printButton<Button::Up>("Up");
	printButton<Button::Down>("Down");
	printButton<Button::Left>("Left");
	printButton<Button::Right>("Right");

	printButton<Button::A>("A");
	printButton<Button::B>("B");
	printButton<Button::C>("C");
	printButton<Button::Start>("Start");

	if (tp == Type::SixButton) {
		printButton<Button::X>("X");
		printButton<Button::Y>("Y");
		printButton<Button::Z>("Z");
		printButton<Button::Mode>("Mode");
		printButton<Button::Home>("Home"); // 8BitDo-only
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
			_inCycles[c] = ((PINB << 8) | PIND) >> 2;
			digitalToggleFast(kInPinMap[7]);
			if (cycleDelay)
				delay_qus(cycleDelay);
		}

		interrupts();

		for (byte &c : _inCycles)
			c |= ~buttonMask();

	for (byte c = 0; c < kCycles; ++c) {
			if (oldCycles[c] != _inCycles[c]) {
				printState();
				break;
			}
		}

		for (byte c = 0; c < kCycles; ++c)
			oldCycles[c] = _inCycles[c];
	}
}

#endif // CONTROLLER_H
