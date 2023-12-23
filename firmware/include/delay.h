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

#ifndef DELAY_H
#define DELAY_H

void inline delay_qus(word qus) {
	if (qus == 0)
		return;

	// From delayMicroseconds
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (qus) : "0" (qus) // 2 cycles
	);
}

#endif // DELAY_H
