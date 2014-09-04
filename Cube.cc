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
#include <string.h>

Cube::Cube() :
	SequenceCount(0),
	LED_Delay(0)
{
	Clear();
}

void Cube::SetLED(uint8_t num, bool enable)
{
	uint8_t layer = num / 25;
	uint8_t mod = num % 25;
	uint8_t row = mod / 5;
	uint8_t column = mod % 5;
	uint8_t bit = 1<<column;
	if(enable)
		ByPosition[layer][row] |= bit;
	else
		ByPosition[layer][row] &= ~bit;
}

void Cube::Shift(char axis, bool wrap)
{
	uint8_t layer[DIM];
	if(axis == 1)
	{
		// store off the top layer
		if(wrap)
			memcpy(layer, ByPosition[4], sizeof(layer));
		// move everything up
		for(int i=3; i>=0; --i)
			memcpy(ByPosition[i+1], ByPosition[i],
				sizeof(ByPosition[i+1]));
		// restore or clear the bottom layer
		if(wrap)
			memcpy(ByPosition[0], layer, sizeof(*ByPosition));
		else
			memset(ByPosition[0], 0, sizeof(*ByPosition));
	}
	else if(axis == -1)
	{
		// store off the bottom layer
		if(wrap)
			memcpy(layer, ByPosition[0], sizeof(layer));
		// move everything down
		for(int i=1; i<DIM; ++i)
			memcpy(ByPosition[i-1], ByPosition[i],
				sizeof(*ByPosition));
		// restore or clear the top layer
		if(wrap)
			memcpy(ByPosition[4], layer, sizeof(*ByPosition));
		else
			memset(ByPosition[4], 0, sizeof(*ByPosition));
	}
	else if(axis == 2)
	{
		for(int i=0; i<DIM; ++i)
		{
			uint8_t tmp = ByPosition[i][4];
			for(int j=3; j>=0; --j)
				ByPosition[i][j+1] = ByPosition[i][j];
			if(wrap)
				ByPosition[i][0] = tmp;
			else
				ByPosition[i][0] = 0;
		}
	}
	else if(axis == -2)
	{
		for(int i=0; i<DIM; ++i)
		{
			uint8_t tmp = ByPosition[i][0];
			for(int j=1; j<DIM; ++j)
				ByPosition[i][j-1] = ByPosition[i][j];
			if(wrap)
				ByPosition[i][4] = tmp;
			else
				ByPosition[i][4] = 0;
		}
	}
	else if(axis == 3)
	{
		for(int i=0; i<DIM; ++i)
		{
			for(int j=0; j<DIM; ++j)
			{
				uint8_t bits = ByPosition[i][j];
				bits <<= 1;
				if(wrap)
					bits |= bits >> 5;
				bits &= 0b11111;
				ByPosition[i][j] = bits;
			}
		}
	}
	else if(axis == -3)
	{
		for(int i=0; i<DIM; ++i)
		{
			for(int j=0; j<DIM; ++j)
			{
				uint8_t bits = ByPosition[i][j];
				uint8_t tmp = bits & 1;
				bits >>= 1;
				if(wrap)
					bits |= tmp << 4;
				ByPosition[i][j] = bits;
			}
		}
	}
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

void Cube::Setup(OrganizeDisplay organize)
{
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
	// displaying by layer reads directly out of the ByDecoder array
	if(organize == BY_LAYER)
		return;

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

void Cube::ExecuteDecoder(uint8_t iterations, uint8_t ex_delay)
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

void Cube::ExecuteLayer(uint8_t iterations, uint8_t ex_delay)
{
	// For each layer the LEDs are enabled in sequence for each decoder
	// until no decoder has any LEDs left to turn on, then the next layer
	// is looked at.  The optimization is to skip over LEDs that are off on
	// a given decoder.
	for(uint8_t i=0; i<iterations; ++i)
	{
		// for each layer
		for(uint8_t l=0; l<DIM; ++l)
		{
			SetLayerEnable(l, 1);
			uint8_t decoder_led[DecoderCount] = {0, 0, 0, 0};
			// while an LED needs to be on
			for(;;)
			{
				bool needed = false;
				// for each decoder
				for(uint8_t d=0; d<DecoderCount; ++d)
				{
					uint8_t value = ByDecoder[d][l];
					// for each LED (skip off positions)
					for(;;)
					{
						if(decoder_led[d] >= 8)
						{
							InternalSetDecoderEnable(d, 0);
							break;
						}

						uint8_t led = decoder_led[d];
						uint8_t bit = 1<<led;
						++decoder_led[d];
						if(value & bit)
						{
							needed = true;
							InternalSetDecoderValue(d, led);
							InternalSetDecoderEnable(d, 1);
							break;
						}
					}
				}
				if(!needed)
					break;

				IO_Delay(LED_Delay);
			}
			SetLayerEnable(l, 0);
		}

		for(uint8_t d=0; d<DecoderCount; ++d)
			InternalSetDecoderEnable(d, 0);
	}
}

uint8_t Cube::Run(int iterations, OrganizeDisplay organize, uint8_t ex_delay)
{
	Reset();
	Setup(organize);
	switch(organize)
	{
	case BY_DECODER:
		ExecuteDecoder(iterations, ex_delay);
		break;
	case BY_LAYER:
		ExecuteLayer(iterations, ex_delay);
		break;
	}
	return GetCount();
}
