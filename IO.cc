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
#include "IO_emu.h"
#include "Cube.h"

// emulation implementation of the IO routines required for the Cube
// Each intersection of layer and decoder (or pin) is seen as on and will
// contribute to the intensity of the final LED at that location, if either
// is off the intensity doesn't change at that location.

const int DECODER_BITS = 8;
// array of decoder, layer, LED intensity
// This is the LEDS connected to each decoder arrangement.
// The hardware works by a pulse with modulation and fast refresh,
// for the emulator each Set call will increase the intensity of the
// given LED to accmulate an intensity which will need to periodically
// be cleared.
static int ByDecoder[DecoderCount][DIM][DECODER_BITS];
// bit field of currently enabled layers
static uint8_t LayersEnabled;
// bit field of currently enabled decoders
static uint8_t DecodersEnabled;
// last set value for each decoder
static uint8_t DecoderValue[DecoderCount];

// increase the intensity at any intersection of layer and decoder
// enabled LEDs, call at each pin transition not just on,
// because any other intersection that is powered will be getting brighter
static void BumpIntensity()
{
	// easy out, nothing will light
	if(!LayersEnabled || !DecodersEnabled)
		return;
	for(uint8_t d=0; d<DecoderCount; ++d)
	{
		uint8_t decoder_bit = 1<<d;
		if(!(DecodersEnabled & decoder_bit))
			continue;
		uint8_t value = DecoderValue[d];
		for(uint8_t l=0; l<DIM; ++l)
		{
			uint8_t layer_bit = 1<<l;
			if(!(LayersEnabled & layer_bit))
				continue;

			++ByDecoder[d][l][value];
		}
	}
}

// zero's the intensity, all LEDs will appear off until something triggers
// them to be set again
void Emu_ClearIntensity()
{
	for(uint8_t d=0; d<DecoderCount; ++d)
	for(uint8_t l=0; l<DIM; ++l)
	for(uint8_t i=0; i<DECODER_BITS; ++i)
	{
		ByDecoder[d][l][i] = 0;
	}
	// If they really wanted it dark they would turn everything off first,
	// just a reminder to not leave anything on when you don't want
	// the LED on.
	BumpIntensity();
}

// for the single line of LEDs directly connected to one pin
void IO_SetPin(uint8_t /*pin*/, uint8_t value)
{
	// The single pin is treated specially, to work like the other
	// decoders it functions as if the decoder input is always 0,
	// causing output pin 0 to always be 1 based on enable, and the
	// pin value controls enable.
	SetDecoderEnable(3, value);
	BumpIntensity();
}

// no delay for the emulator, except to contribute to intensity
void IO_Delay(uint8_t /*delay*/)
{
	BumpIntensity();
}

void SetDecoderValue(uint8_t decoder, uint8_t value)
{
	DecoderValue[decoder] = value;
	BumpIntensity();
}

void SetDecoderEnable(uint8_t decoder, uint8_t enable)
{
	uint8_t decoder_bit = 1<<decoder;
	if(enable)
		DecodersEnabled |= decoder_bit;
	else
		DecodersEnabled &= ~decoder_bit;
	BumpIntensity();
}

void SetLayerEnable(uint8_t layer, uint8_t enable)
{
	uint8_t layer_bit = 1<<layer;
	if(enable)
		LayersEnabled |= layer_bit;
	else
		LayersEnabled &= ~layer_bit;
	BumpIntensity();
}

QVariantList Emu_GetCubeIntensity()
{
	QVariantList layer;
	for(uint8_t l=0; l<DIM; ++l)
	{
		QVariantList column;
		QVariantList row;

		row.push_back(ByDecoder[0][l][0]);
		row.push_back(ByDecoder[0][l][1]);
		row.push_back(ByDecoder[0][l][2]);
		row.push_back(ByDecoder[0][l][3]);
		row.push_back(ByDecoder[0][l][4]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[0][l][5]);
		row.push_back(ByDecoder[0][l][6]);
		row.push_back(ByDecoder[0][l][7]);
		row.push_back(ByDecoder[1][l][0]);
		row.push_back(ByDecoder[1][l][1]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[1][l][2]);
		row.push_back(ByDecoder[1][l][3]);
		row.push_back(ByDecoder[1][l][4]);
		row.push_back(ByDecoder[1][l][5]);
		row.push_back(ByDecoder[1][l][6]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[1][l][7]);
		row.push_back(ByDecoder[2][l][0]);
		row.push_back(ByDecoder[2][l][1]);
		row.push_back(ByDecoder[2][l][2]);
		row.push_back(ByDecoder[2][l][3]);
		column.push_back(row);
		row.clear();

		row.push_back(ByDecoder[2][l][4]);
		row.push_back(ByDecoder[2][l][5]);
		row.push_back(ByDecoder[2][l][6]);
		row.push_back(ByDecoder[2][l][7]);
		row.push_back(ByDecoder[3][l][0]);
		column.push_back(row);

		layer.push_back(column);
	}
	return layer;
}
