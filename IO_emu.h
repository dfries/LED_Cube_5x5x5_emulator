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

#ifndef __IO_EMU_H_
#define __IO_EMU_H_

#include <stdint.h>
#include <QVariant>

// Emulation routines to clear the intensity and get the output values
// after the pattern is displayed.  This file and routines would not be
// included in a microcontroller project.

void Emu_ClearIntensity();
QVariantList Emu_GetCubeIntensity();

#endif // __IO_EMU_H_
