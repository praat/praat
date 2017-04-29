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

autoMixingMatrix MixingMatrix_create (long numberOfOutputChannels, long numberOfInputChannels) {
	try {
		autoMixingMatrix me = Thing_new (MixingMatrix);
		TableOfReal_init (me.get(), numberOfOutputChannels, numberOfInputChannels);
		MixingMatrix_initializeRandom (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"MixingMatrix not created.");
	}
}

autoMixingMatrix MixingMatrix_createSimple (long numberOfOutputChannels, long numberOfInputChannels, char32 *elements) {
	try {
		long inum = 1, ntokens = Melder_countTokens (elements);
		if (ntokens == 0) {
			Melder_throw (U"No matrix elements.");
		}
		long numberOfCells = numberOfInputChannels * numberOfOutputChannels;

		autoMixingMatrix me = MixingMatrix_create (numberOfOutputChannels, numberOfInputChannels);

		/*
			Construct the full matrix from the elements
		*/
		double number;
		for (char32 *token = Melder_firstToken (elements); token && inum <= ntokens; token = Melder_nextToken (), inum ++) {
			long irow = (inum - 1) / numberOfInputChannels + 1;
			long icol = (inum - 1) % numberOfInputChannels + 1;
			Interpreter_numericExpression (0, token, &number);

			my data [irow] [icol] = number;
		}
		if (ntokens < numberOfCells) {
			for (long i = inum; i <= numberOfCells; i ++) {
				long irow = (inum - 1) / numberOfInputChannels + 1;
				long icol = (inum - 1) % numberOfInputChannels + 1;
				my data [irow] [icol] = number; // repeat the last number given!
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"MixingMatrix not created.");
	}
}

void MixingMatrix_initializeRandom (MixingMatrix me) {
	for (long i = 1; i <= my numberOfRows; i ++) {
		for (long j = 1; j <= my numberOfColumns; j ++) {
			my data [i] [j] = NUMrandomGauss (0.0, 1.0);
		}
	}
}

/* End of file MixingMatrix.cpp */
