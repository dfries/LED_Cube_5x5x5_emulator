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

Cube::Cube() :
	SequenceCount(0),
	LED_Delay(0)
{
	Clear();
}

void Cube::Clear()
{
	for(uint8_t l=0; l<DIM; ++l)
	for(uint8_t r=0; r<DIM; ++r)
	{
		ByPosition[l][r] = 0;
	}
}

void Cube::Reset()
{
	// disable layers
	for(uint8_t l=0; l<DIM; ++l)
	{
		SetLayerEnable(l, 0);
	}
	// disable decoders
	for(uint8_t d=0; d<DecoderCount; ++d)
	{
		InternalSetDecoderEnable(d, 0);
	}
}

void Cube::Setup()
{
	// array of decoder, layer, with a bit per LED
	// This is the LEDS connected to each decoder arrangement
	uint8_t ByDecoder[DecoderCount][DIM];

	// convert LED positions into per decoder values
	/* Decoder and LED arrangement per level.
	 * Decoder <decoder number> : LED number
	 * D3:0 could be thought of as a one bit decoder
	 *
	 * D2:4 D2:5 D2:6 D2:7 D3:0
	 * D1:7 D2:0 D2:1 D2:2 D2:3
	 * D1:2 D1:3 D1:4 D1:5 D1:6
	 * D0:5 D0:6 D0:7 D1:0 D1:1
	 * D0:0 D0:1 D0:2 D0:3 D0:4
	 */
	for(uint8_t l=0; l<DIM; ++l)
	{
		uint8_t c0 = ByPosition[l][0];
		uint8_t c1 = ByPosition[l][1];
		uint8_t c2 = ByPosition[l][2];
		uint8_t c3 = ByPosition[l][3];
		uint8_t c4 = ByPosition[l][4];
		// 5 LEDs from the first column, 3 from the next shifted up
		ByDecoder[0][l] = c0 | (c1 << 5);
		// remaining 2 LEDs shifted down, 5 LEDs shifted, 1 LED shifted
		ByDecoder[1][l] = (c1 >> 3) | (c2 << 2) | (c3 << 7);
		// 4 LEDs shifted, 4 LEDs shifted
		ByDecoder[2][l] = (c3 >> 1) | (c4 << 4);
		ByDecoder[3][l] = c4 >> 4;
	}

	// generate a sequence of steps to display this pattern
	// for each decoder:
	// layers that are all 0's are ignored
	// layers that have the same value will be consolidated into one step
	SequenceCount = 0;
	for(uint8_t d=0; d<DecoderCount; ++d)
	{
		// bit field of the layers used
		// once a layers is used it is part of Step pattern already
		// and doesn't need to be looked at again
		uint8_t used = 0;
		for(uint8_t l=0; l<DIM; ++l)
		{
			uint8_t layer_bit = 1<<l;
			if(used & layer_bit)
				continue;
			Step &step = Sequence[SequenceCount];
			step.value = ByDecoder[d][l];
			if(!step.value)
				continue;
			// some LEDs are on, this step will be used
			++SequenceCount;
			step.decoder = d;
			step.layers_enabled = layer_bit;
			// see if any of the next layers have the same pattern
			for(uint8_t n=l+1; n<DIM; ++n)
			{
				layer_bit = 1<<n;
				if(used & layer_bit)
					continue;
				if(ByDecoder[d][n] == step.value)
				{
					step.layers_enabled |= layer_bit;
					used |= layer_bit;
				}
			}
		}
	}
}

void Cube::Execute(uint8_t iterations, uint8_t ex_delay)
{
	for(uint8_t i=0; i<iterations; ++i)
	{
		for(uint8_t s=0; s<SequenceCount; ++s)
		{
			Step &step = Sequence[s];
			// set layers for this pattern
			for(uint8_t l=0; l<DIM; ++l)
			{
				uint8_t layer_bit = 1<<l;
				SetLayerEnable(l,
					step.layers_enabled & layer_bit);
			}
			// wait to enable until the desired value is written,
			// then keep it on until all patterns for this layer
			// and decoder combination are sent
			bool first = true;
			for(uint8_t v=0; v<8; ++v)
			{
				uint8_t select = 1<<v;
				if(!(step.value & select))
					continue;

				InternalSetDecoderValue(step.decoder, v);
				if(first)
				{
					InternalSetDecoderEnable(step.decoder,
						1);
					first = false;
				}
				IO_Delay(LED_Delay);
			}
			InternalSetDecoderEnable(step.decoder, 0);
		}
		for(uint8_t j=0; j<ex_delay; ++j) 
			IO_Delay(LED_Delay);
	}
}

uint8_t Cube::Run(int iterations, uint8_t ex_delay)
{
	Reset();
	Setup();
	Execute(iterations, ex_delay);
	return GetCount();
}
