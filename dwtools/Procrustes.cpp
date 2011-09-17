/* Procrustes.cpp
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 2001
 djmw 20020813 GPL header
 djmw 20040117 Corrected bug in classProcrustes_transform: scale (s) was not used.
 djmw 20050406 Renamed Procrustus Procrustes
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
  djmw 20110304 Thing_new
*/

#include "Procrustes.h"

#include "oo_DESTROY.h"
#include "Procrustes_def.h"
#include "oo_COPY.h"
#include "Procrustes_def.h"
#include "oo_EQUAL.h"
#include "Procrustes_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Procrustes_def.h"
#include "oo_WRITE_TEXT.h"
#include "Procrustes_def.h"
#include "oo_WRITE_BINARY.h"
#include "Procrustes_def.h"
#include "oo_READ_TEXT.h"
#include "Procrustes_def.h"
#include "oo_READ_BINARY.h"
#include "Procrustes_def.h"
#include "oo_DESCRIPTION.h"
#include "Procrustes_def.h"

Thing_implement (Procrustes, AffineTransform, 0);

void structProcrustes :: v_transform (double **in, long nrows, double **out) {
	for (long i = 1; i <= nrows; i++) {
		for (long j = 1; j <= n; j++) {
			double tmp = 0;
			for (long k = 1; k <= n; k++) {
				tmp += in[i][k] * r[k][j];
			}
			out[i][j] = s * tmp + t[j];
		}
	}
}

Any structProcrustes :: v_invert () {
	autoProcrustes thee = Data_copy (this);
	/*
		R is symmetric rotation matrix -->
		inverse is transpose!
	*/

	thy s = s == 0 ? 1 : 1 / s;

	for (long i = 1; i <= n; i++) {
		for (long j = i + 1; j <= n; j++) {
			thy r[i][j] = r[j][i];
			thy r[j][i] = r[i][j];
		}
		thy t[i] = 0;
		/*
		for (j = 1; j <= thy n; j++)
		{
			thy t[i] -= thy r[i][j] * t[j];
		}
		*/
		for (long j = 1; j <= thy n; j++) {
			thy t[i] -= thy r[j][i] * t[j];
		}

		thy t[i] *= thy s;
	}
	return thee.transfer();
}

static void Procrustes_setDefaults (Procrustes me) {
	my s = 1;
	for (long i = 1; i <= my n; i++) {
		my t[i] = 0;
		my r[i][i] = 1;
		for (long j = i + 1; j <= my n; j++) {
			my r[i][j] = my r[j][i] = 0;
		}
	}
}

Procrustes Procrustes_create (long n) {
	try {
		autoProcrustes me = (Procrustes) Thing_new (Procrustes);
		AffineTransform_init (me.peek(), n);
		Procrustes_setDefaults (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Procrustes not created.");
	}
}


/* End of file Procrustes.c */
