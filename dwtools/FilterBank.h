#ifndef _FilterBank_h_
#define _FilterBank_h_
/* FilterBank.h
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
 djmw 20010609
 djmw 20020813 GPL header
 djmw 20040702 Corrected MELTOBARK formula.
 djmw 20110306 Latest modification.
*/

#include "TableOfReal.h"
#include "Spectrum.h"
#include "MFCC.h"
#include "Intensity.h"
#include "PCA.h"

#define HZTOBARK(x) NUMhertzToBark2(x)
#define HZTOMEL(x)	NUMhertzToMel2(x)
#define BARKTOHZ(x) NUMbarkToHertz2(x)
#define MELTOHZ(x)	NUMmelToHertz2(x)

#define BARKTOMEL(x) HZTOMEL(BARKTOHZ(x))
#define MELTOBARK(x) HZTOBARK(MELTOHZ(x))

#define FilterBank_DBREF 4e-10
#define FilterBank_DBFAC 1
#define FilterBank_DBFLOOR -20

#define FilterBank_HERTZ 1
#define FilterBank_BARK  2
#define FilterBank_MEL   3

Thing_define (FilterBank, Matrix) {
	// new methods:
	public:
		virtual int v_getFrequencyScale () { return FilterBank_HERTZ; }
};

Thing_define (BarkFilter, FilterBank) {
	// overridden methods:
	protected:
		virtual int v_getFrequencyScale () { return FilterBank_BARK; }
};

/*
Interpretation:
	xmin, xmax, x1, dx, nx like Sampled.
	ymin, ymax lowest and highest frequencies in Barks.
    y1 mid of first filter (bark).
    dy distance between filters (bark).
    ny the number of filters.
 */

double FilterBank_getFrequencyInHertz (I, double f, int scale_from);
double FilterBank_getFrequencyInBark (I, double f, int scale_from);
double FilterBank_getFrequencyInMel (I, double f, int scale_from);

int FilterBank_getFrequencyScale (I);

void FilterBank_drawFrequencyScales (I, Graphics g, int horizontalScale, double xmin,
	double xmax, int verticalScale, double ymin, double ymax, int garnish);

void BarkFilter_drawSekeyHansonFilterFunctions (BarkFilter me, Graphics g,
	int freqScale,	int fromFilter, int toFilter, double zmin, double zmax,
	int dbScale, double ymin, double ymax, int garnish);

void FilterBank_drawTimeSlice (I, Graphics g, double t, double fmin, double fmax,
	double min, double max, const wchar_t *xlabel, int garnish);

BarkFilter BarkFilter_create (double tmin, double tmax, long nt, double dt,
	double t1, double fmin, double fmax, long nf, double df, long f1);

BarkFilter Matrix_to_BarkFilter (I);

Thing_define (MelFilter, FilterBank) {
	// overridden methods:
	protected:
		virtual int v_getFrequencyScale () { return FilterBank_MEL; }
};

/*
Interpretation:
	xmin, xmax, x1, dx, nx like Sampled.
	ymin, ymax lowest and highest frequencies in mels.
    y1 mid of first filter (mels).
    dy distance between filters (mel).
    ny the number of filters.
 */

MelFilter MelFilter_create (double tmin, double tmax, long nt, double dt,
	double t1, double fmin, double fmax, long nf, double df, double f1);

MelFilter Matrix_to_MelFilter (I);

void MelFilter_drawFilterFunctions (MelFilter me, Graphics g,
	int freqScale, int fromFilter, int toFilter, double zmin, double zmax,
	int dbScale, double ymin, double ymax, int garnish);

MFCC MelFilter_to_MFCC (MelFilter me, long numberOfCoefficients);

Thing_define (FormantFilter, FilterBank) {
	// overridden methods:
	protected:
		//virtual int v_getFrequencyScale ();   // David, is dit correct? ja
	// new methods:
	public:
		//virtual void v_drawFilterFunction (int from, int to, void *dwrawclosure);   // David, is dit correct? ja
};

FormantFilter FormantFilter_create (double tmin, double tmax, long nt,
	double dt, double t1, double fmin, double fmax, long nf, double df,
	double f1);

void FormantFilter_drawFilterFunctions (FormantFilter me, Graphics g, double bandwidth,
	int freqScale, int fromFilter, int toFilter, double zmin, double zmax,
	int dbScale, double ymin, double ymax, int garnish);

FormantFilter Matrix_to_FormantFilter (I);

void FilterBank_equalizeIntensities (I, double intensity_db);

Matrix FilterBank_to_Matrix (I);
/* Generieke ...Filters_to_Matrix */

Spectrum FormantFilter_to_Spectrum_slice (FormantFilter me, double t);
/*
	Calculate amplitude spectrum at time t.
	power[i] = ref * 10 ^ (my z[i][t] / 10)
	spec->z[1][i] = sqrt(power[i]) = sqrt(ref) * 10 ^ (my z[i][t] / (2*10))
	spec->z[2][i] = 0
*/

Intensity FilterBank_to_Intensity (I);

void FilterBank_and_PCA_drawComponent (I, PCA thee, Graphics g, long component, double dblevel,
	double frequencyOffset, double scale, double tmin, double tmax, double fmin, double fmax);

#endif /* _FilterBank_h_ */
