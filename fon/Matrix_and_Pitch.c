/* Matrix_and_Pitch.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 */

#include "Matrix_and_Pitch.h"

Matrix Pitch_to_Matrix (Pitch me) {
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1, 1, 1, 1, 1);
	long i;
	if (thee) for (i = 1; i <= my nx; i ++)
		thy z [1] [i] = my frame [i]. candidate [1]. frequency;
	return thee;
}

Pitch Matrix_to_Pitch (Matrix me) {
	Pitch thee = Pitch_create (my xmin, my xmax, my nx, my dx, my x1, 5000, 2);
	long i;
	if (! thee) goto error;
	for (i = 1; i <= my nx; i ++) {
		Pitch_Frame frame = & thy frame [i];
		if (my z [1] [i] == 0.0) {
			if (! Pitch_Frame_init (frame, 1)) goto error;
			frame->candidate[1].frequency = 0.0;   /* Voiceless candidate always present. */
			frame->candidate[1].strength = 0.4;
		} else {
			if (! Pitch_Frame_init (frame, 2)) goto error;
			frame->intensity = 1;
			frame->candidate[1].frequency = my z [1] [i];
			frame->candidate[1].strength = 0.9;
			frame->candidate[2].frequency = 0.0;   /* Voiceless candidate always present. */
			frame->candidate[2].strength = 0.4;
		}
	}
	return thee;
error:
	forget (thee);
	return Melder_errorp ("(Matrix_to_Pitch:) Not performed.");
}

int Pitch_formula (Pitch me, const wchar_t *formula) {
	Matrix m = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		1, my maxnCandidates, my maxnCandidates, 1, 1);
	long iframe, icand;
	if (! m) return 0;
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Pitch_Frame frame = & my frame [iframe];
		for (icand = 1; icand <= frame -> nCandidates; icand ++)
			m -> z [icand] [iframe] = frame -> candidate [icand]. frequency;
	}
	if (! Matrix_formula (m, formula, 0)) {
		forget (m);
		return Melder_error1 (L"(Pitch_formula:) Not performed.");
	}
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Pitch_Frame frame = & my frame [iframe];
		for (icand = 1; icand <= frame -> nCandidates; icand ++)
			frame -> candidate [icand]. frequency = m -> z [icand] [iframe];
	}
	forget (m);
	return 1;
}

/* End of file Matrix_and_Pitch.c */
