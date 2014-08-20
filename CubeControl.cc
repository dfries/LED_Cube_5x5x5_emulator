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

#include "CubeControl.h"
#include "Cube.h"
#include "IO.h"

#include <QVariant>

static Cube cube;

QVariantList SetCubePattern()
{
	Emu_ClearIntensity();
	cube.Clear();
	cube.Run(1);
	cube.ByPosition[0][0] = 0x55;
	cube.ByPosition[2][0] = 0x55;
	cube.ByPosition[4][0] = 0x55;
	cube.Run(1);
	return Emu_GetCubeIntensity();
}
