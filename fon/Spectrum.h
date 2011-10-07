#ifndef _Spectrum_h_
#define _Spectrum_h_
/* Spectrum.h
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

/* Complex spectrum. */
/* If it comes from a sound (expressed in Pa), the values are expressed in Pa/Hz. */

#include "Matrix.h"
#include "Graphics.h"

#include "Spectrum_def.h"
oo_CLASS_CREATE (Spectrum, Matrix);

/*
	xmin            // lowest frequency (Hz)
	xmax            // highest frequency (Hz)
	nx              // number of frequencies
	dx              // frequency step (Hz)
	x1              // first frequency (Hz)
	ymin = 1        // first row: real part
	ymax = 2        // second row: imaginary part
	ny = 2          // two rows
	dy = 1; y1 = 1  // y is row number
*/

Spectrum Spectrum_create (double fmax, long nf);
/* Preconditions:
		fmax > 0.0;
		nf >= 2;
	Postconditions:
		my xmin == 0.0;
		my xmax == fmax;
		my nx == nf;
		my dx == fmax / (nx - 1);
		my x1 == 0.0;
		my ymin == 1;
		my ymax == 2;
		my ny == 2;
		my dy == 1;
		my y1 == 1;
		my z [1..ny] [1..nx] == 0.0;
*/
		
int Spectrum_getPowerDensityRange (Spectrum me, double *minimum, double *maximum);   /* Return 0 if all zeroes. */
double Spectrum_getBandDensity (Spectrum me, double fmin, double fmax);   /* Pa2 / Hz2 */
double Spectrum_getBandEnergy (Spectrum me, double fmin, double fmax);   /* Pa2 sec */
double Spectrum_getBandDensityDifference (Spectrum me,
	double lowBandMin, double lowBandMax, double highBandMin, double HighBandMax);
double Spectrum_getBandEnergyDifference (Spectrum me,
	double lowBandMin, double lowBandMax, double highBandMin, double highBandMax);

/*
	Spectral moments.
*/
double Spectrum_getCentreOfGravity (Spectrum me, double power);
double Spectrum_getCentralMoment (Spectrum me, double moment, double power);
double Spectrum_getStandardDeviation (Spectrum me, double power);
double Spectrum_getSkewness (Spectrum me, double power);
double Spectrum_getKurtosis (Spectrum me, double power);

void Spectrum_drawInside (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum);
void Spectrum_draw (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish);
/*
	Function:
		draw a Spectrum into a Graphics.
	Preconditions:
		maximum > minimum;
	Arguments:
		[fmin, fmax]: frequencies in Hz; x domain of drawing;
		Autowindowing: if fmax <= fmin, x domain of drawing is [my xmin, my xmax].
		[minimum, maximum]: power in dB/Hz; y range of drawing.
*/
void Spectrum_drawLogFreq (Spectrum me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish);

Table Spectrum_downto_Table (Spectrum me, bool includeBinNumbers, bool includeFrequency,
	bool includeRealPart, bool includeImaginaryPart, bool includeEnergyDensity, bool includePowerDensity);
void Spectrum_list (Spectrum me, bool includeBinNumbers, bool includeFrequency,
	bool includeRealPart, bool includeImaginaryPart, bool includeEnergyDensity, bool includePowerDensity);

Spectrum Matrix_to_Spectrum (Matrix me);

Matrix Spectrum_to_Matrix (Spectrum me);

Spectrum Spectrum_cepstralSmoothing (Spectrum me, double bandWidth);

void Spectrum_passHannBand (Spectrum me, double fmin, double fmax, double smooth);
void Spectrum_stopHannBand (Spectrum me, double fmin, double fmax, double smooth);

void Spectrum_getNearestMaximum (Spectrum me, double frequency, double *frequencyOfMaximum, double *heightOfMaximum);

/* End of file Spectrum.h */
#endif
