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

#include <avr/io.h>

#ifndef PIN_H
#define PIN_H

inline constexpr volatile uint8_t &pinToPortReg(uint8_t pin) {
	if (pin < 8)
		return PORTD;
	if (pin < 14)
		return PORTB;
	return PORTC;
}

inline constexpr volatile uint8_t &pinToPinReg(uint8_t pin) {
	if (pin < 8)
		return PIND;
	if (pin < 14)
		return PINB;
	return PINC;
}

inline constexpr volatile uint8_t &pinToDDRReg(uint8_t pin) {
	if (pin < 8)
		return DDRD;
	if (pin < 14)
		return DDRB;
	return DDRC;
}

inline constexpr uint8_t pinToBitMask(uint8_t pin) {
	if (pin < 8)
		return _BV(pin);
	if (pin < 14)
		return _BV(pin - 8);
	return _BV(pin - 14);
}

inline void pinSet(uint8_t pin) {
	pinToPortReg(pin) |= pinToBitMask(pin);
}

inline void pinClear(uint8_t pin) {
	pinToPortReg(pin) &= ~pinToBitMask(pin);
}

inline void pinToggle(uint8_t pin) {
	pinToPinReg(pin) |= pinToBitMask(pin);
}

inline void pinWrite(uint8_t pin, bool value) {
	if (value)
		pinSet(pin);
	else
		pinClear(pin);
}

inline bool pinRead(uint8_t pin) {
	return pinToPinReg(pin) & pinToBitMask(pin);
}

inline void pinModeOutput(uint8_t pin) {
	pinToDDRReg(pin) |= pinToBitMask(pin);
}

inline void pinModeInput(uint8_t pin) {
	pinToDDRReg(pin) &= ~pinToBitMask(pin);
}

#endif // PIN_H
