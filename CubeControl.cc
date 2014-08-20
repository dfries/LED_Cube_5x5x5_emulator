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
#include <QDebug>

static Cube cube;

QVariantList SetCubePattern(int pattern)
{
	Emu_ClearIntensity();
	cube.Clear();
	int layer = pattern % DIM;
	qDebug() << "layer" << layer;
	for(int i=0; i<DIM; ++i)
		cube.ByPosition[layer][i] = 0b11111;
	/*
	int layer = (pattern / DIM ) % DIM;
	int column = pattern % DIM;
	qDebug() << "set led at" << layer << column;
	cube.ByPosition[layer][column] = 1;
	*/
	/*
	cube.Run(1);
	cube.ByPosition[0][0] = 0x55;
	cube.ByPosition[2][0] = 0x55;
	cube.ByPosition[4][0] = 0x55;
	*/
	cube.Run(1);
	return Emu_GetCubeIntensity();
}
