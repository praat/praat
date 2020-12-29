/* SoundSet.cpp
 *
 * Copyright (C) 2019,2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SoundSet.h"

Thing_implement (SoundSet, Ordered, 0);

integer SoundSet_getMinimumNumberOfSamples (SoundSet me) {
	integer result = INTEGER_MAX;
	for (integer isound = 1; isound <= my size; isound ++)
		if (my at [isound] -> nx < result)
			result = my at [isound] -> nx;
	return result;
}

autoMAT SoundSet_getRandomizedPatterns (SoundSet me, integer numberOfPatterns, integer patternSize) {
	try {
		integer minimumNumberOfSamples = SoundSet_getMinimumNumberOfSamples (me);
		Melder_require (patternSize <= minimumNumberOfSamples,
			U"The pattern size cannot be ", patternSize, U", because there is a Sound that is only ", minimumNumberOfSamples, U" samples long.");
		autoMAT result = zero_MAT (numberOfPatterns, patternSize);
		for (integer ipattern = 1; ipattern <= numberOfPatterns; ipattern ++) {
			integer soundNumber = NUMrandomInteger (1, my size);
			Sound sound = my at [soundNumber];
			integer numberOfSamples = sound -> nx;
			integer endSample = NUMrandomInteger (patternSize, numberOfSamples);
			integer startSample = endSample - (patternSize - 1);
			Melder_assert (startSample >= 1);
			constVEC const samples = sound -> z.row (1);
			result.row (ipattern)  <<=  samples. part (startSample, endSample);
		}
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": no randomize patterns gotten.");
	}
}

void SoundSet_Table_getRandomizedPatterns (SoundSet me, Table thee, conststring32 columnName, integer numberOfPatterns, integer inputSize, integer outputSize,
	autoPatternList *out_inputs, autoPatternList *out_outputs)
{
	try {
		Melder_require (thy rows.size == my size,
			U"The number of rows of ", thee, U" should be equal to the number of elements of ", me);
		integer columnNumber = Table_getColumnIndexFromColumnLabel (thee, columnName);
		integer minimumNumberOfSamples = SoundSet_getMinimumNumberOfSamples (me);
		Melder_require (inputSize <= minimumNumberOfSamples,
			U"The input size cannot be ", inputSize, U", because there is a Sound that is only ", minimumNumberOfSamples, U" samples long.");
		Table_numericize_Assert (thee, columnNumber);
		autoPatternList inputs = PatternList_create (numberOfPatterns, inputSize);
		autoPatternList outputs = PatternList_create (numberOfPatterns, outputSize);
		for (integer ipattern = 1; ipattern <= numberOfPatterns; ipattern ++) {
			integer soundNumber = NUMrandomInteger (1, my size);
			Sound sound = my at [soundNumber];
			integer numberOfSamples = sound -> nx;
			integer endSample = NUMrandomInteger (inputSize, numberOfSamples);
			integer startSample = endSample - (inputSize - 1);
			Melder_assert (startSample >= 1);
			constVEC const samples = sound -> z.row (1);
			inputs -> z.row (ipattern)  <<=  samples. part (startSample, endSample);
			integer classNumber = Melder_iround (thy rows.at [soundNumber] -> cells [columnNumber]. number);
			Melder_require (classNumber >= 1 && classNumber <= outputSize,
				U"The class number should be between 1 and ", outputSize, U", not ", classNumber, U".");
			outputs -> z [ipattern] [classNumber] = 1.0;
		}
		if (out_inputs) *out_inputs = inputs.move();
		if (out_outputs) *out_outputs = outputs.move();
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U": no randomized patterns gotten.");
	}
}

/* End of file SoundSet.cpp */
