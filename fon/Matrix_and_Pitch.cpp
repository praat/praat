/* Matrix_and_Pitch.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2007/08/12 wchar_t
 * pb 2009/01/18 Interpreter argument to formula
 * pb 2009/04/04 corrected voiceless frames in Pitch_to_Matrix
 * pb 2011/06/04 C++
 */

#include "Matrix_and_Pitch.h"

Matrix Pitch_to_Matrix (Pitch me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, 1, 1, 1, 1);
		for (long i = 1; i <= my nx; i ++) {
			double value = my frame [i]. candidate [1]. frequency;
			thy z [1] [i] = value > 0.0 && value < my ceiling ? my frame [i]. candidate [1]. frequency : 0.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

Pitch Matrix_to_Pitch (Matrix me) {
	try {
		autoPitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, 5000, 2);
		for (long i = 1; i <= my nx; i ++) {
			Pitch_Frame frame = & thy frame [i];
			if (my z [1] [i] == 0.0) {
				Pitch_Frame_init (frame, 1);
				frame->candidate[1].frequency = 0.0;   // voiceless candidate always present
				frame->candidate[1].strength = 0.4;
			} else {
				Pitch_Frame_init (frame, 2);
				frame->intensity = 1;
				frame->candidate[1].frequency = my z [1] [i];
				frame->candidate[1].strength = 0.9;
				frame->candidate[2].frequency = 0.0;   // voiceless candidate always present
				frame->candidate[2].strength = 0.4;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Pitch.");
	}
}

void Pitch_formula (Pitch me, const wchar_t *formula, Interpreter interpreter) {
	try {
		autoMatrix m = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, my maxnCandidates, my maxnCandidates, 1, 1);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Pitch_Frame frame = & my frame [iframe];
			for (long icand = 1; icand <= frame -> nCandidates; icand ++)
				m -> z [icand] [iframe] = frame -> candidate [icand]. frequency;
		}
		Matrix_formula (m.peek(), formula, interpreter, NULL);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Pitch_Frame frame = & my frame [iframe];
			for (long icand = 1; icand <= frame -> nCandidates; icand ++)
				frame -> candidate [icand]. frequency = m -> z [icand] [iframe];
		}
	} catch (MelderError) {
		Melder_throw (me, ": formula not completed.");
	}
}

/* End of file Matrix_and_Pitch.cpp */
