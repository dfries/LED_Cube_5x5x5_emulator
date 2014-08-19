/*
    Copyright (C) 2014 David Fries <David@Fries.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __IO_H_
#define __IO_H_

#include <stdint.h>

void IO_SetPin(uint8_t pin, uint8_t value);
void IO_Delay(uint8_t delay);
void SetDecoderValue(uint8_t decoder, uint8_t value);
void SetDecoderEnable(uint8_t decoder, uint8_t enable);
void SetLayerEnable(uint8_t layer, uint8_t enable);

void Emu_ClearIntensity();

#endif // __IO_H_
