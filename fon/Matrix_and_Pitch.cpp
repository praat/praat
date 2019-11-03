/* Matrix_and_Pitch.cpp
 *
 * Copyright (C) 1992-2005,2011,2012,2015-2019 Paul Boersma
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

#include "Matrix_and_Pitch.h"

autoMatrix Pitch_to_Matrix (Pitch me) {
	try {
		autoMatrix you = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer i = 1; i <= my nx; i ++) {
			const double value = my frames [i]. candidates [1]. frequency;
			your z [1] [i] =
				Pitch_util_frequencyIsVoiced (value, my ceiling) ? my frames [i]. candidates [1]. frequency : 0.0;
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoPitch Matrix_to_Pitch (Matrix me) {
	try {
		autoPitch you = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, 5000.0, 2);
		for (integer i = 1; i <= my nx; i ++) {
			const Pitch_Frame frame = & your frames [i];
			if (my z [1] [i] == 0.0) {
				Pitch_Frame_init (frame, 1);
				frame -> candidates [1]. frequency = 0.0;   // voiceless candidate always present
				frame -> candidates [1]. strength = 0.4;
			} else {
				Pitch_Frame_init (frame, 2);
				frame -> intensity = 1;
				frame -> candidates [1]. frequency = my z [1] [i];
				frame -> candidates [1]. strength = 0.9;
				frame -> candidates [2]. frequency = 0.0;   // voiceless candidate always present
				frame -> candidates [2]. strength = 0.4;
			}
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Pitch.");
	}
}

void Pitch_formula (Pitch me, conststring32 formula, Interpreter interpreter) {
	try {
		autoMatrix m = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, my maxnCandidates, my maxnCandidates, 1.0, 1.0);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Pitch_Frame frame = & my frames [iframe];
			for (integer icand = 1; icand <= frame -> nCandidates; icand ++)
				m -> z [icand] [iframe] = frame -> candidates [icand]. frequency;
		}
		Matrix_formula (m.get(), formula, interpreter, nullptr);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Pitch_Frame frame = & my frames [iframe];
			for (integer icand = 1; icand <= frame -> nCandidates; icand ++)
				frame -> candidates [icand]. frequency = m -> z [icand] [iframe];
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

/* End of file Matrix_and_Pitch.cpp */
