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

#include <stdlib.h>

static Cube cube;

static void LayerInc(int sequence)
{
	cube.Clear();
	int layer = sequence % DIM;
	for(int i=0; i<DIM; ++i)
		cube.ByPosition[layer][i] = 0b11111;
	cube.Run(64);
}

static void InOrder(int sequence)
{
	cube.Clear();
	const int total = DIM * DIM * DIM;
	int num = sequence % total;
	cube.SetLED(num, 1);
	cube.Run(64);
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
	cube.Run(64);
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
		cube.Run(64);
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
	cube.Run(64);
}

static void drawLetter(char c)
{
	switch(c)
	{
	// Specify them mirror image left to right, as the most significant
	// bit (MSB) is the higher value and on the right.
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
	case 0:
	case ' ' :
		break;
	default:
		cube.ByPosition[4][0] = 0b01110;
		cube.ByPosition[3][0] = 0b01010;
		cube.ByPosition[2][0] = 0b01000;
		cube.ByPosition[1][0] = 0b00100;
		cube.ByPosition[0][0] = 0b00100;
		break;
	}
}

static void Fade(int sequence)
{
	cube.Clear();
	const char *word = "ABIGAIL";
	const int total = 20;
	int step = sequence % total;
	char letter = word[sequence/total%sizeof(word)];
	drawLetter(letter);
	if(step <= 4)
	{
		cube.Run(64);
		return;
	}
	if(step == 5)
	{
		cube.Run(32);
		cube.Shift(2, false);
		cube.Run(64);
		return;
	}

	for(int i=0; i<step-6; ++i)
		cube.Shift(2, false);

	cube.Run(16);
	cube.Shift(2, false);
	cube.Run(32);
	cube.Shift(2, false);
	cube.Run(64);

	if(step < 9)
		return;

	drawLetter(letter);
	for(int i=0; i<4; ++i)
		cube.Shift(2, false);
	cube.Shift(-1, false);
	if(step == 9)
	{
		cube.Run(64);
		return;
	}
	if(step == 10)
	{
		cube.Run(32);
		cube.Shift(-1, false);
		cube.Run(64);
		return;
	}

	for(int i=0; i<step-11; ++i)
		cube.Shift(-1, false);

	cube.Run(16);
	cube.Shift(-1, false);
	cube.Run(32);
	cube.Shift(-1, false);
	cube.Run(64);
}

static void Pong(int sequence)
{
	cube.Clear();
	cube.ByPosition[1][2] = 1;
	cube.ByPosition[2][1] = 1;
	cube.ByPosition[2][2] = 1;
	cube.ByPosition[2][3] = 1;
	cube.ByPosition[3][2] = 1;
	static int x = 0, y = 0;
	if(sequence == 100000)
		++y;
	else if(sequence == 200000)
		--y;
	else if(sequence == 300000)
		--x;
	else if(sequence == 400000)
		++x;

	if(y>3)
		y=3;
	else if(y<-3)
		y=-3;
	if(x>3)
		x=3;
	else if(x<-3)
		x=-3;

	int sign = 1;
	if(y<0)
		sign = -1;
	for(int i=0, stop=abs(y); i<stop; ++i)
		cube.Shift(sign * 1, false);

	sign = -1;
	if(x<0)
		sign = 1;
	for(int i=0, stop=abs(x); i<stop; ++i)
		cube.Shift(sign * 2, false);

	cube.Run(64);
}

void SetCubePattern(int pattern, int sequence)
{
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
	case 5:
		Pong(sequence);
		break;
	default:
		// light of the corners
		cube.Clear();
		cube.ByPosition[0][0] = 0b10001;
		cube.ByPosition[0][4] = 0b10001;
		cube.ByPosition[4][0] = 0b10001;
		cube.ByPosition[4][4] = 0b10001;
		cube.Run(64);
	}
}
