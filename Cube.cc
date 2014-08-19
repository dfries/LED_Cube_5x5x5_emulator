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

#include "Cube.h"

Cube::Cube()
{
}

void Cube::Clear()
{
	for(uint8_t l=0; l<DIM; ++l)
	for(uint8_t r=0; r<DIM; ++r)
		ByPosition[l][r] = 0;
}

void Cube::Setup()
{
}

void Cube::Execute(uint8_t iterations, uint8_t ex_delay)
{
}

uint8_t Cube::Run(int iterations, uint8_t ex_delay)
{
	Setup();
	Execute(iterations, ex_delay);
	return GetCount();
}
