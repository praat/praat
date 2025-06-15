/* Matrix_and_Pitch.c
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 */

#include "Matrix_and_Pitch.h"

Matrix Pitch_to_Matrix (Pitch me) {
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, 1, 1, 1, 1); cherror
	for (long i = 1; i <= my nx; i ++) {
		double value = my frame [i]. candidate [1]. frequency;
		thy z [1] [i] = value > 0.0 && value < my ceiling ? my frame [i]. candidate [1]. frequency : 0.0;
	}
end:
	iferror forget (thee);
	return thee;
}

Pitch Matrix_to_Pitch (Matrix me) {
	Pitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, 5000, 2); cherror
	for (long i = 1; i <= my nx; i ++) {
		Pitch_Frame frame = & thy frame [i];
		if (my z [1] [i] == 0.0) {
			Pitch_Frame_init (frame, 1); cherror
			frame->candidate[1].frequency = 0.0;   /* Voiceless candidate always present. */
			frame->candidate[1].strength = 0.4;
		} else {
			Pitch_Frame_init (frame, 2); cherror
			frame->intensity = 1;
			frame->candidate[1].frequency = my z [1] [i];
			frame->candidate[1].strength = 0.9;
			frame->candidate[2].frequency = 0.0;   /* Voiceless candidate always present. */
			frame->candidate[2].strength = 0.4;
		}
	}
end:
	iferror forget (thee);
	return thee;
}

int Pitch_formula (Pitch me, const wchar_t *formula, Interpreter interpreter) {
	Matrix m = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, my maxnCandidates, my maxnCandidates, 1, 1); cherror
	for (long iframe = 1; iframe <= my nx; iframe ++) {
		Pitch_Frame frame = & my frame [iframe];
		for (long icand = 1; icand <= frame -> nCandidates; icand ++)
			m -> z [icand] [iframe] = frame -> candidate [icand]. frequency;
	}
	Matrix_formula (m, formula, interpreter, NULL); cherror
	for (long iframe = 1; iframe <= my nx; iframe ++) {
		Pitch_Frame frame = & my frame [iframe];
		for (long icand = 1; icand <= frame -> nCandidates; icand ++)
			frame -> candidate [icand]. frequency = m -> z [icand] [iframe];
	}
end:
	forget (m);
	iferror return Melder_error1 (L"(Pitch_formula:) Not performed.");
	return 1;
}

/* End of file Matrix_and_Pitch.c */
