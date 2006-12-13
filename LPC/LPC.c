/* LPC.c
 *
 * Copyright (C) 1994-2006 David Weenink
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
*/

#include "LPC_and_Polynomial.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "LPC_def.h"
#include "oo_COPY.h"
#include "LPC_def.h"
#include "oo_EQUAL.h"
#include "LPC_def.h"
#include "oo_WRITE_ASCII.h"
#include "LPC_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPC_def.h"
#include "oo_READ_ASCII.h"
#include "LPC_def.h"
#include "oo_READ_BINARY.h"
#include "LPC_def.h"
#include "oo_DESCRIPTION.h"
#include "LPC_def.h"

static void info (I)
{
	iam (LPC);
	classData -> info (me);
	MelderInfo_writeLine5 ("Time domain: ", Melder_double (my xmin), " to ", Melder_double (my xmax),
		" (s).");
	MelderInfo_writeLine2 ("Prediction order: ", Melder_integer (my maxnCoefficients));
	MelderInfo_writeLine2 ("Number of frames: ", Melder_integer (my nx));
	MelderInfo_writeLine3 ("Time step: ", Melder_double (my dx), " (s).");
	MelderInfo_writeLine3 ("First frame at: ", Melder_double (my x1), " (s).");
}

class_methods (LPC, Sampled)
	class_method_local (LPC, destroy)
	class_method_local (LPC, equal)
	class_method_local (LPC, copy)
	class_method (info)
	class_method_local (LPC, readAscii)
	class_method_local (LPC, readBinary)
	class_method_local (LPC, writeAscii)
	class_method_local (LPC, writeBinary)
	class_method_local (LPC, description)
class_methods_end

int LPC_Frame_init (LPC_Frame me, int nCoefficients)
{
	if (nCoefficients !=0 &&
		((my a = NUMfvector (1, nCoefficients)) == NULL)) return 0;
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
	LPC me = new (LPC);
	if (! me || ! LPC_init (me, tmin, tmax, nt, dt, t1,
		predictionOrder, samplingPeriod)) forget (me);
	return me;
}

void LPC_drawGain (LPC me, Graphics g, double tmin, double tmax, double gmin, double gmax, int garnish)
{
	long itmin, itmax, iframe; float *gain;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	if ((gain = NUMfvector (itmin, itmax)) == NULL) return;
	for (iframe=itmin; iframe <= itmax; iframe++) gain[iframe] = my frame[iframe].gain;
	if (gmax <= gmin) NUMfvector_extrema (gain, itmin, itmax, & gmin, & gmax);
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
		Graphics_textBottom (g, 1, "Time (seconds)");
		Graphics_textLeft (g, 1, "Gain");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
	NUMfvector_free (gain, itmin);
}

void LPC_drawPoles (LPC me, Graphics g, double time, int garnish)
{
	Polynomial p = LPC_to_Polynomial (me, time);

	if (p != NULL)
	{
		Roots r = Polynomial_to_Roots (p);
		if (r != NULL)
		{
			Roots_draw (r, g, -1, 1, -1, 1, "+", 12, garnish);
			forget (r);
		}
		forget (p);
	}
	Melder_clearError ();
}

/*
	distance = ln (a'Ra' / aRa);
	a=[1,a1,a2,...ap];
	R is (p+1)x(p+1) autocorrelation Toeplitz matrix (first row: [r0,r1,..rp])
	aRa = Sum (i=0; i=p; a[i] * Sum (j=0; j=p; t[i][j]*a[j]))
		= Sum (i=0; i=p; a[i] * Sum (j=0; j=p; r[|i-j|]*a[j]))
	    = Sum (i=0; i=p; r[0]*a[i]*a[i] + 2*Sum (j=i+1; j=p; r[|i-j|]*a[i]*a[j]))
*/
static double LPC_Frame_difference_Itakura (LPC_Frame me, LPC_Frame thee)
{
	int i, j, n = my nCoefficients < thy nCoefficients ? my nCoefficients : thy nCoefficients;
	float *r = NULL, *a = NULL; double num = 0, denum = 0;
	
	if ((r = NUMfvector (0, n)) == NULL) return 1e38;
	NUMfvector_copyElements (my a, a+1, 1, n);
	/* from a to autocorrelation coefficients ...*/
	for (i=0; i <= n; i++)
	{
		float ai = i==0 ? 1 : my a[i], api = i==0 ? 1 : thy a[i];
		num += r[0] * api * api; denum = r[0] * ai * ai;
		for (j=i+1; j <= n; j++)
		{
			num += 2 * r[abs(i-j)] * api * thy a[j];
			denum += 2 * r[abs(i-j)] * ai * my a[j];
		}
	}
	NUMfvector_free (r, 0);
	return log (num / denum); 
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
		for (j = 1; j < frame -> nCoefficients; j++)
		{ 
			thy z[j][i] = frame -> a[j];
		}
	}
	return thee;
}


/* End of file LPC.c */
