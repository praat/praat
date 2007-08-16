/* Cepstrumc.c
 *
 * Copyright (C) 1994-2002 David Weenink
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
 djmw 20061218 Changed info to Melder_writeLine<x> format.
*/

#include "Cepstrumc.h"
#include "DTW.h"

#include "oo_DESTROY.h"
#include "Cepstrumc_def.h"
#include "oo_COPY.h"
#include "Cepstrumc_def.h"
#include "oo_EQUAL.h"
#include "Cepstrumc_def.h"
#include "oo_WRITE_TEXT.h"
#include "Cepstrumc_def.h"
#include "oo_WRITE_BINARY.h"
#include "Cepstrumc_def.h"
#include "oo_READ_TEXT.h"
#include "Cepstrumc_def.h"
#include "oo_READ_BINARY.h"
#include "Cepstrumc_def.h"
#include "oo_DESCRIPTION.h"
#include "Cepstrumc_def.h"

static void info (I)
{
	iam (Cepstrumc);
	classData -> info (me);
	MelderInfo_writeLine2 (L"  Start time: ", Melder_double (my xmin));
	MelderInfo_writeLine2 (L"  End time: ", Melder_double (my xmax));
	MelderInfo_writeLine2 (L"  Number of frames: ", Melder_integer (my nx));
	MelderInfo_writeLine2 (L"  Time step: ", Melder_double (my dx));
	MelderInfo_writeLine2 (L"  First frame at: ", Melder_double (my x1));
	MelderInfo_writeLine2 (L"  Number of coefficients: ", Melder_integer (my maxnCoefficients));
}

class_methods (Cepstrumc, Sampled)
	class_method_local (Cepstrumc, destroy)
	class_method_local (Cepstrumc, equal)
	class_method_local (Cepstrumc, copy)
	class_method ( info)
	class_method_local (Cepstrumc, readText)
	class_method_local (Cepstrumc, readBinary)
	class_method_local (Cepstrumc, writeText)
	class_method_local (Cepstrumc, writeBinary)
	class_method_local (Cepstrumc, description)
class_methods_end

int Cepstrumc_Frame_init (Cepstrumc_Frame me, int nCoefficients)
{
	if ((my c = NUMfvector (0, nCoefficients)) == NULL) return 0;
	my nCoefficients = nCoefficients;
	return 1;
}

int Cepstrumc_init (Cepstrumc me, double tmin, double tmax, long nt, double dt, double t1,
	int nCoefficients, double samplingFrequency)
{
	my samplingFrequency = samplingFrequency;
	my maxnCoefficients = nCoefficients;
	return Sampled_init (me, tmin, tmax, nt, dt, t1) &&
		(my frame = NUMstructvector (Cepstrumc_Frame, 1, nt));
}
 
Cepstrumc Cepstrumc_create (double tmin, double tmax, long nt, double dt, double t1,
	int nCoefficients, double samplingFrequency)
{
	Cepstrumc me = new (Cepstrumc);
	if (! me || ! Cepstrumc_init (me, tmin, tmax, nt, dt, t1, nCoefficients, samplingFrequency)) forget (me);
	return me;
}

static void regression (Cepstrumc me, long frame, float r[], long nr)
{
	long i, j, nc = 1e6; double sumsq = 0;
	for (i=0; i <= my maxnCoefficients; i++) r[i] = 0;
	if (frame <= nr/2 || frame >= my nx - nr/2) return;
	for (j=-nr/2; j <= nr/2; j++)
	{
		Cepstrumc_Frame f = & my frame[frame+j];
		if (f->nCoefficients < nc) nc = f->nCoefficients;
		sumsq += j * j;
	}
	for (i=0; i <= nc; i++)
	{
		for (j=-nr/2; j <= nr/2; j++)
		{
			Cepstrumc_Frame f = & my frame[frame+j];
			r[i] += f->c[i] * j / sumsq / my dx;
		}
	}
}

DTW Cepstrumc_to_DTW ( Cepstrumc me, Cepstrumc thee, double wc, double wle,
	double wr, double wer, double dtr, int matchStart, int matchEnd, int constraint)
{
	DTW him = NULL; float *ri = NULL, *rj = NULL;
	long i, j, nr = dtr / my dx;
	
	if (my maxnCoefficients != thy maxnCoefficients) return Melder_errorp(
		"Cepstrumc_difference: Cepstrumc orders must be equal.");
	if (wr != 0 && nr < 2) return Melder_errorp ("Cepstrumc_difference: "
		"time window for regression coefficients too small.");
	if (nr % 2 == 0) nr++;
	if (wr != 0) Melder_casual ("Cepstrumc_difference: #frames used for regression coefficients %ld", nr); 

	if (((him = DTW_create (my xmin, my xmax, my nx, my dx, my x1,
			thy xmin, thy xmax, thy nx, thy dx, thy x1)) == NULL) ||
		((ri = NUMfvector (0, my maxnCoefficients)) == NULL) ||
		((rj = NUMfvector (0, my maxnCoefficients)) == NULL)) { forget (him); goto end; }

	/*
		Calculate distance matrix
	*/
	
	Melder_progress (0.0, "");
	for (i=1; i <= my nx; i++)
	{
		Cepstrumc_Frame fi = & my frame[i];
		regression (me, i, ri, nr);
		for (j=1; j <= thy nx; j++)
		{
			Cepstrumc_Frame fj = & thy frame[j]; long k;
			double d, dist = 0, distr = 0;
			if (wc != 0) /* cepstral distance */
			{
				for (k=1; k <= fj->nCoefficients; k++)
				{
					d = fi->c[k] - fj->c[k]; dist += d * d;
				}
				dist *= wc;
			}
			/* log energy distance */
			d = fi->c[0] - fj->c[0];
			dist += wle * d * d;
			if (wr != 0) /* regression distance */
			{
				regression (thee, j, rj, nr);
				for (k=1; k <= fj->nCoefficients; k++)
				{
					d = ri[k] - rj[k]; distr += d * d;
				}
				dist += wr * distr;
			}
			if (wer != 0) /* regression on c[0]: log(energy) */
			{
				if (wr == 0) regression (thee, j, rj, nr);
				d = ri[0] - rj[0]; dist += wer * d * d;
			}
			dist /= wc + wle + wr + wer;
			his z[i][j] = sqrt (dist);	/* prototype along y-direction */
		}
		if (! Melder_progress ((double)i / my nx, 
			"Calculate distances: frame %ld from %ld", i, my nx)) { forget(him); goto end; }
	}
	Melder_progress (1.0, NULL);
	DTW_findPath (him, matchStart, matchEnd, constraint);
end:
	NUMfvector_free (ri, 0);
	NUMfvector_free (rj, 0); 	
	return him;
}

Matrix Cepstrumc_to_Matrix (Cepstrumc me)
{
	Matrix thee = NULL; long i, j;
	
	if ((thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		0, my maxnCoefficients, my maxnCoefficients+1, 1, 0)) == NULL) return thee;

	for (i=1; i <= my nx; i++)
	{
		Cepstrumc_Frame him = & my frame[i];
		for (j=1; j <= his nCoefficients+1; j++) thy z[j][i] = his c[j-1];
	}
	return thee;
}

/* End of file Cepstrumc.c */
