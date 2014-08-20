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

static void LayerInc(int sequence)
{
	cube.Clear();
	int layer = sequence % DIM;
	qDebug() << "layer" << layer;
	for(int i=0; i<DIM; ++i)
		cube.ByPosition[layer][i] = 0b11111;
}

static void InOrder(int sequence)
{
	cube.Clear();
	const int total = DIM * DIM * DIM;
	int num = sequence % total;
	cube.SetLED(num, 1);
}

static void Perimeter(int sequence)
{
	static uint8_t layer, row, column;
	static bool enable = true;
	if(sequence == 0)
	{
		layer = row = column = 0;
		enable = true;
		cube.Clear();
	}
	cube[layer][row][column] = enable;
	if(row == 0 || row == 4)
	{
		++column;
	}
	else
	{
		column += 4;
	}
	if(column > 4)
	{
		++row;
		column = 0;
	}
	if(row > 4)
	{
		++layer;
		row = 0;
		column = 0;
		if(layer > 4)
		{
			layer = 0;
			enable = !enable;
		}
	}
}

static void Shift(int sequence)
{
	if(sequence == 0)
	{
		cube.Clear();
		// + in the center
		for(int i=0; i<DIM; ++i)
			cube.ByPosition[2][i] = 0b00100;
		cube.ByPosition[2][2] = 0b11111;
		return;
	}
	int index = (sequence-1) % 120;
	if(index < 20)
		cube.Shift(1, true);
	else if(index < 40)
		cube.Shift(-1, true);
	else if(index < 60)
		cube.Shift(2, true);
	else if(index < 80)
		cube.Shift(-2, true);
	else if(index < 100)
		cube.Shift(3, true);
	else if(index < 120)
		cube.Shift(-3, true);
}

static void Fade(int sequence)
{
	cube.Clear();
	const char *word = "ABIGAIL";
	switch(word[sequence%7])
	{
	case 'A':
		cube.ByPosition[4][0] = 0b01110;
		cube.ByPosition[3][0] = 0b01010;
		cube.ByPosition[2][0] = 0b01110;
		cube.ByPosition[1][0] = 0b01010;
		cube.ByPosition[0][0] = 0b01010;
		break;
	case 'B':
		cube.ByPosition[4][0] = 0b01110;
		cube.ByPosition[3][0] = 0b01010;
		cube.ByPosition[2][0] = 0b01110;
		cube.ByPosition[1][0] = 0b10010;
		cube.ByPosition[0][0] = 0b01110;
		break;
	case 'G':
		cube.ByPosition[4][0] = 0b00110;
		cube.ByPosition[3][0] = 0b00001;
		cube.ByPosition[2][0] = 0b11001;
		cube.ByPosition[1][0] = 0b10001;
		cube.ByPosition[0][0] = 0b01110;
		break;
	case 'I':
		cube.ByPosition[4][0] = 0b01110;
		cube.ByPosition[3][0] = 0b00100;
		cube.ByPosition[2][0] = 0b00100;
		cube.ByPosition[1][0] = 0b00100;
		cube.ByPosition[0][0] = 0b01110;
		break;
	case 'L':
		cube.ByPosition[4][0] = 0b00010;
		cube.ByPosition[3][0] = 0b00010;
		cube.ByPosition[2][0] = 0b00010;
		cube.ByPosition[1][0] = 0b00010;
		cube.ByPosition[0][0] = 0b01110;
		break;
	}
}

QVariantList SetCubePattern(int pattern, int sequence)
{
	Emu_ClearIntensity();
	switch(pattern)
	{
	case 0:
		LayerInc(sequence);
		break;
	case 1:
		InOrder(sequence);
		break;
	case 2:
		Perimeter(sequence);
		break;
	case 3:
		Shift(sequence);
		break;
	case 4:
		Fade(sequence);
		break;
	default:
		// light of the corners
		cube.Clear();
		cube.ByPosition[0][0] = 0b10001;
		cube.ByPosition[0][4] = 0b10001;
		cube.ByPosition[4][0] = 0b10001;
		cube.ByPosition[4][4] = 0b10001;
	}
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
