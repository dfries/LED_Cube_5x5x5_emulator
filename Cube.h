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
#ifndef __CUBE_H__
#define __CUBE_H__

#include <stdint.h>

#include "IO.h"

// setup for a 5x5x5 cube
const uint8_t DIM = 5;
// in reality 3 decoder chip and one pin, but just think of that one as a
// degenerate decoder case with only one input
const uint8_t DecoderCount = 4;

/* Holds a Cube's worth of LEDs along with various way to manipulate the
 * arrangement in addition to direct access to alter an individual LED.
 * Once an arrangement is set, generate a sequence of steps required to
 * display the cube arrangement with the set of decoders available,
 * then output that sequence.
 *
 * Depending on the pattern different sequences are more optimal.
 * Currently this is optimized for the same pattern on different layers,
 * so as to enable all layers, then cycle one decoder through all the
 * patterns before going on to the next layer or decoder.  This can have
 * up to five LEDs on at a time.
 * Alternatively one layer could be enabled, update all decoders to the
 * first LED for that layer, delay, go on to the next LED, and then
 * the next layer.  This can have up to 4 LEDs on at one time, although
 * as one LED is directly connected to a pin it will mostly only have
 * three LEDs on at one time.
 * Depending on the pattern more symmetry could be found with up to 20
 * LEDs on at a time.
 */
class Cube
{
public:
	Cube();

	void SetLED(uint8_t num, bool enable);

	// axis, 1 is layer, 2 row, 3 column, -1, -2, -3 for the opposite
	// direction, if wrap is true it will wrap around otherwise they
	// are set to off
	void Shift(char axis, bool wrap);

	// allows
	// cube[layer][row][column] = true
	// bool value = cube[layer][row][column];
	// if(cube[layer][row][column] == true )
	class BracketColumn
	{
	public:
		BracketColumn(Cube &cube, uint8_t layer, uint8_t row,
			uint8_t column) :
			CubeObj(cube), Layer(layer), Row(row), Bit(1<<column) {}
		BracketColumn& operator=(bool on)
		{
			if(on)
				CubeObj.ByPosition[Layer][Row] |= Bit;
			else
				CubeObj.ByPosition[Layer][Row] &= ~Bit;
			return *this;
		}
		bool operator==(bool rhs)
		{
			bool on = CubeObj.ByPosition[Layer][Row] & Bit;
			return on == rhs;
		}
		bool operator!=(bool rhs)
		{
			return !(*this == rhs);
		}
		operator bool() const
		{
			return CubeObj.ByPosition[Layer][Row] & Bit;
		}
	private:
		Cube &CubeObj;
		// Bit is the column value already turned into a bit field.
		uint8_t Layer, Row, Bit;
	};
	class BracketRow
	{
	public:
		BracketRow(Cube &cube, uint8_t layer, uint8_t row) :
			CubeObj(cube), Layer(layer), Row(row) {}
		BracketColumn operator[](uint8_t column)
		{
			return BracketColumn(CubeObj, Layer, Row, column);
		}
	private:
		Cube &CubeObj;
		uint8_t Layer, Row;
	};
	class BracketLayer
	{
	public:
		BracketLayer(Cube &cube, uint8_t layer) : CubeObj(cube),
			Layer(layer) {}
		BracketRow operator[](uint8_t row)
		{
			return BracketRow(CubeObj, Layer, row);
		}
	private:
		Cube &CubeObj;
		uint8_t Layer;
	};
	BracketLayer operator[](uint8_t layer)
	{
		return BracketLayer(*this, layer);
	}

	void SetLED_OnDelay(uint8_t delay) { LED_Delay = delay; }

	// Set all LEDs to 0 in ByPosition
	void Clear();

	// Clear() is for the future pattern, Reset() is the current pin output
	void Reset();
	// create the sequence
	void Setup();
	// execute the sequence, add extra ex_delay at the end of each iteration
	void Execute(uint8_t iterations, uint8_t ex_delay = 0);
	// find out how many steps are in the sequence
	uint8_t GetCount() const { return SequenceCount; }

	// one call when the individual steps don't need to be broken up
	// the individual steps allows finding out how long a sequence
	// is to add additional delays to better keep the intensity the same
	// from one pattern to the next
	uint8_t Run(int iterations, uint8_t ex_delay = 0);

	// array of layer, row, with the lower 5 bits being the column
	// This is a layer, row, column physical arrangement of the LEDs.
	uint8_t ByPosition[DIM][DIM];

private:
	// holds the data required for one combination of decoder and
	// levels enabled for that decoder
	struct Step
	{
		// the one decoder to modify
		uint8_t decoder;
		// bit field for the LEDs that are on
		uint8_t value;
		// bit field for the layers that are enabled
		uint8_t layers_enabled;
	};
	// current sequence, worst case number if each decoder has a
	// different pattern at each level
	Step Sequence[DecoderCount * DIM];
	uint8_t SequenceCount;
	// time to hold each output before going to the next pattern
	uint8_t LED_Delay;

	// deal with the 4th decoder being a direct pin connection
	static void InternalSetDecoderEnable(uint8_t decoder, uint8_t enable)
	{
		if(decoder == 3)
		{
			IO_SetPin(25, enable);
		}
		else
		{
			SetDecoderEnable(decoder, enable);
		}
	}
	static void InternalSetDecoderValue(uint8_t decoder, uint8_t value)
	{
		// only a value of 0 is valid for decoder 3 when treating it
		// as a one pin decoder
		if(decoder == 3)
			return;
		SetDecoderValue(decoder, value);
	}
};

#endif // __CUBE_H__
