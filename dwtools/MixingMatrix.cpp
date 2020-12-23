/* MixingMatrix.cpp
 *
 * Copyright (C) 2010-2017 David Weenink
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

#include "Interpreter.h"
#include "MixingMatrix.h"

Thing_implement (MixingMatrix, TableOfReal, 0);

autoMixingMatrix MixingMatrix_create (integer numberOfOutputChannels, integer numberOfInputChannels) {
	try {
		autoMixingMatrix me = Thing_new (MixingMatrix);
		TableOfReal_init (me.get(), numberOfOutputChannels, numberOfInputChannels);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MixingMatrix not created.");
	}
}

autoMixingMatrix MixingMatrix_createSimple (integer numberOfOutputChannels, integer numberOfInputChannels, conststring32 elements_string) {
	try {
		autoSTRVEC elements = splitByWhitespace_STRVEC (elements_string);
		
		const integer numberOfCells = numberOfInputChannels * numberOfOutputChannels;
		
		Melder_require (elements.size == numberOfCells,
			U"The number of mixing coefficients (", elements.size, U") should equal the number of cells (", numberOfCells, U") in the mixing matrix.");
		
		autoMixingMatrix me = MixingMatrix_create (numberOfOutputChannels, numberOfInputChannels);

		/*
			Construct the full matrix from the elements
		*/
		for (integer inum = 1; inum <= numberOfCells; inum ++) {
			double number;
			Interpreter_numericExpression (0, elements [inum].get(), & number);
			const integer irow = (inum - 1) / numberOfInputChannels + 1;
			const integer icol = (inum - 1) % numberOfInputChannels + 1;
			my data [irow] [icol] = number;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"MixingMatrix not created.");
	}
}

void MixingMatrix_setRandomGauss (MixingMatrix me, double mean, double stdev) {
	for (integer i = 1; i <= my numberOfRows; i++) {
		for (integer j = 1; j <= my numberOfColumns; j++) {
			my data[i][j] = NUMrandomGauss (mean, stdev);
		}
	}
}

void MixingMatrix_multiplyInputChannel (MixingMatrix me, integer inputChannel, double value) {
	if (inputChannel >= 1 && inputChannel <= my numberOfColumns) {
		for (integer i = 1; i <= my numberOfRows; i ++) {
			my data [i] [inputChannel] *= value;
		}
	}
}

// https://developer.mozilla.org/en-US/docs/Web/API/AudioNode/channelInterpretation
void MixingMatrix_setStandardChannelInterpretation (MixingMatrix me) {
	for (integer i = 1; i <= my numberOfRows; i++) {
		for (integer j = 1; j <= my numberOfColumns; j++) {
			my data [i][j] = 0;
		}
	}
	bool dimensionsCovered = true;
	if (my numberOfColumns == 1) { // mono input
		if (my numberOfRows == 2) { // up-mix to stereo
			/*
				The M input channel is used for both output channels (L and R).
					output.L = input.M
					output.R = input.M
			*/
			my data [1][1] = my data [2][1] = 1.0;
		} else if (my numberOfRows == 4) { // up-mix to quad
			/*
				The M input channel is used for non-surround output channels (L and R).
				Surround output channels (SL and SR) are silent.
					output.L = input.M
					output.R = input.M
					output.SL = 0
					output.SR = 0
			*/
			my data [1][1] = my data [2][1] = 1.0;
		} else if (my numberOfRows == 6) { // up-mix to to 5.1
			/*
				The M input channel is used for the center output channel (C). 
				All the others (L, R, LFE, SL, and SR) are silent.
					output.L = 0
					output.R = 0
					output.C = input.M
					output.LFE = 0
					output.SL = 0
					output.SR = 0
			*/
			my data [3][1] = 1.0;
		} else {
			dimensionsCovered = false;
		}
	} else if (my numberOfColumns == 2) { // stereo input
		if (my numberOfRows == 1) { // down-mix to mono
			/*
				Both input channels (L and R) are equally combined to produce the unique output channel (M).
					output.M = 0.5 * (input.L + input.R)
			*/
			my data [1][1] = my data [1][2] = 0.5;
		} else if (my numberOfRows == 2) { // to stereo
			/*
				output.L = input.L
				output.R = input.R
			*/
			my data [1][1] = my data [2][2] = 1.0;
		} else if (my numberOfRows == 4) { // up-mix to quad
			/*
				The L and R input channels are used for their non-surround respective output channels (L and R). 
				Surround output channels (SL and SR) are silent.
					output.L = input.L
					output.R = input.R
					output.SL = 0
					output.SR = 0
			*/
			my data [1][1] = my data [2][2] = 1.0;
		} else if (my numberOfRows == 6) { // up-mix to 5.1
			/*
				The L and R input channels are used for their non-surround respective output channels (L and R). 
				Surround output channels (SL and SR), as well as the center (C) and subwoofer (LFE) channels, are left silent.
					output.L = input.L
					output.R = input.R
					output.C = 0
					output.LFE = 0
					output.SL = 0
					output.SR = 0
			*/
			my data [1][1] = my data [2][2] = 1.0;
		} else { // up-mix 
			dimensionsCovered = false;
		}
	} else if (my numberOfColumns == 4) { // quad input
		if (my numberOfRows == 1) { // down-mix to mono
			/*
				All four input channels (L, R, SL, and SR) are equally combined to produce the unique output 
				channel (M).
				output.M = 0.25 * (input.L + input.R + input.SL + input.SR)
			*/
			my data [1][1] = my data [1][2] = my data [1][3] = my data [1][4] = 0.25;
		} else if (my numberOfRows == 2) { // down-mix to stereo
			/*
				Both left input channels (L and SL) are equally combined to produce the unique left output channel (L). 
				And similarly, both right input channels (R and SR) are equally combined to produce the unique right output channel (R).
					output.L = 0.5 * (input.L + input.SL)
					output.R = 0.5 * (input.R + input.SR)
			*/
			my data [1][1] = my data [1][3] = 0.5;
			my data [1][2] = my data [1][4] = 0.5;
		} else if (my numberOfRows == 6) { // up-mix to 5.1
			/*
				The L, R, SL, and SR input channels are used for their respective output channels (L and R). 
				Center (C) and subwoofer (LFE) channels are left silent.
					output.L = input.L
					output.R = input.R
					output.C = 0
					output.LFE = 0
					output.SL = input.SL
					output.SR = input.SR
			*/
			my data [1][1] = my data [2][2] = my data [5][3] = my data [6][4] = 1.0;
		} else {
			dimensionsCovered = false;
		}
	} else if (my numberOfColumns == 6) { // 5.1 input)
		if (my numberOfRows == 1) { // down-mix to mono
			/*
				The left (L and SL), right (R and SR) and central channels are all mixed together.
				The surround channels are slightly attenuated and the regular lateral channels are power-compensated to make them count as a single channel by multiplying by √2/2.
				The subwoofer (LFE) channel is lost.
					output.M = 0.7071 * (input.L + input.R) + input.C + 0.5 * (input.SL + input.SR)
			*/
			my data [1][3] = 1.0;
			my data [1][1] = my data [1][2] = 0.5; // NUMsqrt1_2 is not safe if the channels are equal;
			my data [1][5] = my data [1][6] = 0.5;
		} else if (my numberOfRows == 2)  { // down-mix to stereo
			/*
				The central channel (C) is summed with each lateral surround channel (SL or SR) and mixed to each lateral channel. 
				As it is mixed down to two channels, it is mixed at a lower power: in each case it is multiplied by √2/2. 
				The subwoofer (LFE) channel is lost.
					output.L = input.L + 0.7071 * (input.C + input.SL)
					output.R = input.R + 0.7071 * (input.C + input.SR)
			*/
			my data [1][1] = my data [2][2] = 1.0;
			my data [1][3] = my data [1][5] = 0.5; // NUMsqrt1_2;
			my data [2][4] = my data [2][6] = 0.5; // NUMsqrt1_2;
		} else if (my numberOfRows == 4)  { // down-mix to quad
			/*
				The central (C) is mixed with the lateral non-surround channels (L and R). 
				As it is mixed down to two channels, it is mixed at a lower power: in each case it is multiplied by √2/2. 
				The surround channels are passed unchanged. The subwoofer (LFE) channel is lost.
					output.L = input.L + 0.7071 * input.C
					output.R = input.R + 0.7071 * input.C
					output.SL = input.SL
					output.SR = input.SR
			*/
			my data [1][1] = my data [2][2] = my data [3][5] = my data [4][6] = 1.0;
			my data [1][3] = my data [2][3] = NUMsqrt1_2;
		} else {
			dimensionsCovered = false;
		}
	} else {
		dimensionsCovered = false;
	}
	if (! dimensionsCovered) {
		/*
			Fill each output channel with its input counterpart, that is the input channel with the same index. 
			Channels with no corresponding input channels are left silent.
		*/
		const integer lowerDimension = std::min (my numberOfRows, my numberOfColumns);
		for (integer i = 1; i <= lowerDimension; i++)
			my data [i][i] = 1.0;
	}
}

void MixingMatrix_muteAndActivateChannels (MixingMatrix me, constBOOLVEC const& muteChannels) {
	Melder_assert (muteChannels.size == my numberOfColumns);
	integer numberOfMuteChannels = 0;
	for (integer ichan = 1; ichan <= muteChannels.size; ichan ++)
		if (muteChannels [ichan])
			numberOfMuteChannels ++;
	/*
		Set all mute channels to 0 and all other channels to 1.
		To prevent overflow, scale by the number of channels that are on.
	*/
	const double coefficient = ( my numberOfColumns > numberOfMuteChannels ? 1.0 / (my numberOfColumns - numberOfMuteChannels) : 0.0 );
	for (integer ichan = 1; ichan <= my numberOfColumns; ichan ++) {
		const double channelScaling = ( muteChannels [ichan] ? 0.0 : coefficient );
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			my data [irow] [ichan] = channelScaling;
	}
}

/* End of file MixingMatrix.cpp */
