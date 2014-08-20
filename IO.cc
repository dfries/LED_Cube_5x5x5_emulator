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

#include "IO.h"
#include "Cube.h"

#include <QDebug>

// emulation implementation of the IO routines required for the Cube

// array of decoder, layer, LED intensity
// This is the LEDS connected to each decoder arrangement.
// The hardware works by a pulse with modulation and fast refresh,
// for the emulator each Set call will increase the intensity of the
// given LED to accmulate an intensity which will need to periodically
// be cleared.
static int ByDecoder[DecoderCount][DIM][8];
// bit field of currently enabled layers
static uint8_t LayersEnabled;
// bit field of currently enabled decoders
static uint8_t DecodersEnabled;

void Emu_ClearIntensity()
{
	for(uint8_t d=0; d<DecoderCount; ++d)
	for(uint8_t l=0; l<DIM; ++l)
	for(uint8_t i=0; i<DIM; ++i)
	{
		ByDecoder[d][l][i] = 0;
	}
}

void IO_SetPin(uint8_t /*pin*/, uint8_t value)
{
	// off means the intensity doesn't increase
	if(!value)
		return;

	// one LED per layer, on or off
	for(uint8_t l=0; l<DIM; ++l)
	{
		uint8_t layer_bit = 1<<l;
		bool enabled = LayersEnabled & layer_bit;
		if(!enabled)
			continue;

		++ByDecoder[3][l][0];
	}
}

void IO_Delay(uint8_t /*delay*/)
{
}

void SetDecoderValue(uint8_t decoder, uint8_t value)
{
	// off means the intensity doesn't increase
	if(!value)
		return;

	for(uint8_t l=0; l<DIM; ++l)
	{
		uint8_t layer_bit = 1<<l;
		bool enabled = LayersEnabled & layer_bit;
		if(!enabled)
			continue;

		++ByDecoder[decoder][l][value];
	}
}

void SetDecoderEnable(uint8_t decoder, uint8_t enable)
{
	uint8_t decoer_bit = 1<<decoder;
	if(enable)
		DecodersEnabled |= decoer_bit;
	else
		DecodersEnabled &= ~decoer_bit;
}

void SetLayerEnable(uint8_t layer, uint8_t enable)
{
	uint8_t layer_bit = 1<<layer;
	if(enable)
		LayersEnabled |= layer_bit;
	else
		LayersEnabled &= ~layer_bit;
}

QVariantList Emu_GetCubeIntensity()
{
	QVariantList layer;
	for(uint8_t l=0; l<DIM; ++l)
	{
		QVariantList column;
		QVariantList row;

		row.push_back(ByDecoder[l][0][0]);
		row.push_back(ByDecoder[l][0][1]);
		row.push_back(ByDecoder[l][0][2]);
		row.push_back(ByDecoder[l][0][3]);
		row.push_back(ByDecoder[l][0][4]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[l][0][5]);
		row.push_back(ByDecoder[l][0][6]);
		row.push_back(ByDecoder[l][0][7]);
		row.push_back(ByDecoder[l][1][0]);
		row.push_back(ByDecoder[l][1][1]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[l][1][2]);
		row.push_back(ByDecoder[l][1][3]);
		row.push_back(ByDecoder[l][1][4]);
		row.push_back(ByDecoder[l][1][5]);
		row.push_back(ByDecoder[l][1][6]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[l][1][7]);
		row.push_back(ByDecoder[l][2][0]);
		row.push_back(ByDecoder[l][2][1]);
		row.push_back(ByDecoder[l][2][2]);
		row.push_back(ByDecoder[l][2][3]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[l][2][4]);
		row.push_back(ByDecoder[l][2][5]);
		row.push_back(ByDecoder[l][2][6]);
		row.push_back(ByDecoder[l][2][7]);
		row.push_back(ByDecoder[l][3][0]);
		column.push_back(row);

		layer.push_back(column);
	}
	return layer;
}
