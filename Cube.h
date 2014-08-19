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
 */
class Cube
{
public:
	Cube();

	// Set all LEDs to 0
	void Clear();

	// create the sequence
	void Setup();
	// execute the sequence
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
		// bit field for the rows that are enabled
		uint8_t rows_enabled;
	};
	// current sequence, worst case number if each decoder has a
	// different pattern at each level
	Step Sequence[DecoderCount * DIM];
	uint8_t SequenceCount;
};

#endif // __CUBE_H__
