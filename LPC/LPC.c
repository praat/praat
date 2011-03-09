/* LPC.c
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020812 GPL header
 djmw 20030612 Removed LPC_Frame_free
 djmw 20060510 LPC_drawPoles error cleared if something goes wrong.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071017 oo_CAN_WRITE_AS_ENCODING.h
 djmw 20080122 float -> double
 djmw 20081223 Corrected a bug in Matrix LPC_to_Matrix (last coefficient was not copied)
 djmw 20110304 Thing_new
*/

#include "LPC_and_Polynomial.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "LPC_def.h"
#include "oo_COPY.h"
#include "LPC_def.h"
#include "oo_EQUAL.h"
#include "LPC_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPC_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPC_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPC_def.h"
#include "oo_READ_TEXT.h"
#include "LPC_def.h"
#include "oo_READ_BINARY.h"
#include "LPC_def.h"
#include "oo_DESCRIPTION.h"
#include "LPC_def.h"

static void info (I)
{
	iam (LPC);
	classData -> info (me);
	MelderInfo_writeLine5 (L"Time domain: ", Melder_double (my xmin), L" to ", Melder_double (my xmax),
		L" (s).");
	MelderInfo_writeLine2 (L"Prediction order: ", Melder_integer (my maxnCoefficients));
	MelderInfo_writeLine2 (L"Number of frames: ", Melder_integer (my nx));
	MelderInfo_writeLine3 (L"Time step: ", Melder_double (my dx), L" (s).");
	MelderInfo_writeLine3 (L"First frame at: ", Melder_double (my x1), L" (s).");
}

class_methods (LPC, Sampled)
	us -> version = 1;
	class_method_local (LPC, destroy)
	class_method_local (LPC, equal)
	class_method_local (LPC, copy)
	class_method_local (LPC, canWriteAsEncoding)
	class_method_local (LPC, readText)
	class_method_local (LPC, readBinary)
	class_method_local (LPC, writeText)
	class_method_local (LPC, writeBinary)
	class_method_local (LPC, description)
	class_method (info)
class_methods_end

int LPC_Frame_init (LPC_Frame me, int nCoefficients)
{
	if (nCoefficients != 0 &&
		((my a = NUMdvector (1, nCoefficients)) == NULL)) return 0;
	my nCoefficients = nCoefficients;
	return 1;
}

int LPC_init (LPC me, double tmin, double tmax, long nt, double dt, double t1,
	int predictionOrder, double samplingPeriod)
{
	my samplingPeriod = samplingPeriod;
	my maxnCoefficients = predictionOrder;
	return Sampled_init (me, tmin, tmax, nt, dt, t1) &&
		(my frame = NUMstructvector (LPC_Frame, 1, nt));
}

Any LPC_create (double tmin, double tmax, long nt, double dt, double t1,
	int predictionOrder, double samplingPeriod)
{
	LPC me = Thing_new (LPC);
	if (! me || ! LPC_init (me, tmin, tmax, nt, dt, t1,
		predictionOrder, samplingPeriod)) forget (me);
	return me;
}

void LPC_drawGain (LPC me, Graphics g, double tmin, double tmax, double gmin, double gmax, int garnish)
{
	long itmin, itmax, iframe; double *gain;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	if ((gain = NUMdvector (itmin, itmax)) == NULL) return;
	for (iframe=itmin; iframe <= itmax; iframe++) gain[iframe] = my frame[iframe].gain;
	if (gmax <= gmin) NUMdvector_extrema (gain, itmin, itmax, & gmin, & gmax);
	if (gmax == gmin) { gmin = 0; gmax += 0.5; }
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, gmin, gmax);
	for (iframe=itmin; iframe <= itmax; iframe++)
	{
		double x = Sampled_indexToX (me, iframe);
		Graphics_fillCircle_mm (g, x, gain[iframe], 1.0);
	}
	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (seconds)");
		Graphics_textLeft (g, 1, L"Gain");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
	NUMdvector_free (gain, itmin);
}

void LPC_drawPoles (LPC me, Graphics g, double time, int garnish)
{
	Polynomial p = LPC_to_Polynomial (me, time);

	if (p != NULL)
	{
		Roots r = Polynomial_to_Roots (p);
		if (r != NULL)
		{
			Roots_draw (r, g, -1, 1, -1, 1, L"+", 12, garnish);
			forget (r);
		}
		forget (p);
	}
	Melder_clearError ();
}


Matrix LPC_to_Matrix (LPC me)
{
	Matrix thee;
	long i, j;

	thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 
		1, my maxnCoefficients, my maxnCoefficients, 1, 1);
	if (thee == NULL) return NULL;

	for (i = 1; i <= my nx; i++)
	{
		LPC_Frame frame = & my frame[i];
		for (j = 1; j <= frame -> nCoefficients; j++)
		{ 
			thy z[j][i] = frame -> a[j];
		}
	}
	return thee;
}


/* End of file LPC.c */
