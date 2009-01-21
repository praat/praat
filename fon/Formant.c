/* Formant.c
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
 * pb 2004/10/05 allow reverse axes in scatter plot (and remove special routine for those)
 * pb 2005/12/08 Formant_getQuantileOfBandwidth
 * pb 2007/03/17 domain quantity
 * pb 2007/10/01 can write as encoding
 * pb 2008/01/19 version 2
 * pb 2008/01/19 don't draw undefined lines
 * pb 2008/06/01 Formant_downto_Table, Formant_list
 * pb 2009/01/18 Interpreter argument to formula
 */

#include "Formant.h"

#include "oo_DESTROY.h"
#include "Formant_def.h"
#include "oo_COPY.h"
#include "Formant_def.h"
#include "oo_EQUAL.h"
#include "Formant_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Formant_def.h"
#include "oo_READ_TEXT.h"
#include "Formant_def.h"
#include "oo_WRITE_TEXT.h"
#include "Formant_def.h"
#include "oo_READ_BINARY.h"
#include "Formant_def.h"
#include "oo_WRITE_BINARY.h"
#include "Formant_def.h"
#include "oo_DESCRIPTION.h"
#include "Formant_def.h"

static void info (I) {
	iam (Formant);
	classData -> info (me);
	MelderInfo_writeLine1 (L"Time domain:");
	MelderInfo_writeLine3 (L"   Start time: ", Melder_double (my xmin), L" seconds");
	MelderInfo_writeLine3 (L"   End time: ", Melder_double (my xmax), L" seconds");
	MelderInfo_writeLine3 (L"   Total duration: ", Melder_double (my xmax - my xmin), L" seconds");
	MelderInfo_writeLine1 (L"Time sampling:");
	MelderInfo_writeLine2 (L"   Number of frames: ", Melder_integer (my nx));
	MelderInfo_writeLine3 (L"   Time step: ", Melder_double (my dx), L" seconds");
	MelderInfo_writeLine3 (L"   First frame centred at: ", Melder_double (my x1), L" seconds");
}

static double getValueAtSample (I, long iframe, long which, int units) {
	iam (Formant);
	Formant_Frame frame = & my frame [iframe];
	double frequency;
	long iformant = which >> 1;
	if (iformant < 1 || iformant > frame -> nFormants) return NUMundefined;
	frequency = frame -> formant [iformant]. frequency;
	if ((which & 1) == 0) {
		return units ? NUMhertzToBark (frequency) : frequency;
	} else {
		double bandwidth = frame -> formant [iformant]. bandwidth;
		if (units) {
			double fleft = frequency - 0.5 * bandwidth, fright = frequency + 0.5 * bandwidth;
			fleft = fleft <= 0 ? 0 : NUMhertzToBark (fleft);   /* Prevent NUMundefined. */
			fright = NUMhertzToBark (fright);
			return fright - fleft;
		}
		return bandwidth;
	}
	return NUMundefined;
}

class_methods (Formant, Sampled) {
	us -> version = 2;   /* With intensity, and double. */
	class_method_local (Formant, destroy)
	class_method_local (Formant, description)
	class_method_local (Formant, copy)
	class_method_local (Formant, equal)
	class_method_local (Formant, canWriteAsEncoding)
	class_method_local (Formant, writeText)
	class_method_local (Formant, readText)
	class_method_local (Formant, writeBinary)
	class_method_local (Formant, readBinary)
	class_method (info)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_method (getValueAtSample)
	class_methods_end
}

Formant Formant_create (double tmin, double tmax, long nt, double dt, double t1,
	int maxnFormants)
{
	Formant me = new (Formant);
	if (! me || ! Sampled_init (me, tmin, tmax, nt, dt, t1) ||
		! (my frame = NUMstructvector (Formant_Frame, 1, nt))) goto error;
	my maxnFormants = maxnFormants;
	return me;
error:
	forget (me);
	return NULL;
}

long Formant_getMinNumFormants (Formant me) {
	long minNumFormants = 100000000, iframe;
	for (iframe = 1; iframe <= my nx; iframe ++)
		if (my frame [iframe]. nFormants < minNumFormants)
			minNumFormants = my frame [iframe]. nFormants;
	return minNumFormants;
}

long Formant_getMaxNumFormants (Formant me) {
	long maxNumFormants = 0, iframe;
	for (iframe = 1; iframe <= my nx; iframe ++)
		if (my frame [iframe]. nFormants > maxNumFormants)
			maxNumFormants = my frame [iframe]. nFormants;
	return maxNumFormants;
}

void Formant_drawTracks (Formant me, Graphics g, double tmin, double tmax, double fmax, int garnish) {
	long itmin, itmax, iframe, itrack, ntrack = Formant_getMinNumFormants (me);
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, 0.0, fmax);
	for (itrack = 1; itrack <= ntrack; itrack ++) {
		for (iframe = itmin; iframe < itmax; iframe ++) {
			Formant_Frame curFrame = & my frame [iframe], nextFrame = & my frame [iframe + 1];
			double x1 = Sampled_indexToX (me, iframe), x2 = Sampled_indexToX (me, iframe + 1);
			double f1 = curFrame -> formant [itrack]. frequency;
			double f2 = nextFrame -> formant [itrack]. frequency;
			if (NUMdefined (x1) && NUMdefined (f1) && NUMdefined (x2) && NUMdefined (f2))
				Graphics_line (g, x1, f1, x2, f2);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_textLeft (g, 1, L"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, 1, 1, 1);
	}
}

void Formant_drawSpeckles_inside (Formant me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double suppress_dB, double dotSize)
{
	long itmin, itmax, iframe, iformant;
	double maximumIntensity = 0.0, minimumIntensity;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);

	for (iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		if (frame -> intensity > maximumIntensity)
			maximumIntensity = frame -> intensity;
	}
	if (maximumIntensity == 0.0 || suppress_dB <= 0.0)
		minimumIntensity = 0.0;   /* Ignore. */
	else
		minimumIntensity = maximumIntensity / pow (10, suppress_dB / 10);

	for (iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		double x = Sampled_indexToX (me, iframe);
		if (frame -> intensity < minimumIntensity) continue;
		for (iformant = 1; iformant <= frame -> nFormants; iformant ++) {
			double frequency = frame -> formant [iformant]. frequency;
			if (frequency >= fmin && frequency <= fmax)
				Graphics_fillCircle_mm (g, x, frequency, dotSize);
		}
	}
}

void Formant_drawSpeckles (Formant me, Graphics g, double tmin, double tmax, double fmax, double suppress_dB,
	int garnish)
{
	Graphics_setInner (g);
	Formant_drawSpeckles_inside (me, g, tmin, tmax, 0.0, fmax, suppress_dB, 1.0);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_textLeft (g, 1, L"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, 1, 1, 1);
	}
}

int Formant_formula_bandwidths (Formant me, const wchar_t *formula, Interpreter interpreter) {
	long iframe, iformant, nrow = Formant_getMaxNumFormants (me);
	Matrix mat = NULL;
	if (nrow < 1) return Melder_error1 (L"(Formant_formula_bandwidths:) No formants available.");
	mat = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, nrow + 0.5, nrow, 1, 1); cherror
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		for (iformant = 1; iformant <= frame -> nFormants; iformant ++)
			mat -> z [iformant] [iframe] = frame -> formant [iformant]. bandwidth;
	}
	Matrix_formula (mat, formula, interpreter, NULL); cherror
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		for (iformant = 1; iformant <= frame -> nFormants; iformant ++)
			frame -> formant [iformant]. bandwidth = mat -> z [iformant] [iframe];
	}
end:
	forget (mat);
	iferror return 0;
	return 1;
}

int Formant_formula_frequencies (Formant me, const wchar_t *formula, Interpreter interpreter) {
	long iframe, iformant, nrow = Formant_getMaxNumFormants (me);
	Matrix mat = NULL;
	if (nrow < 1) return Melder_error1 (L"(Formant_formula_frequencies:) No formants available.");
	mat = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, nrow + 0.5, nrow, 1, 1); cherror
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		for (iformant = 1; iformant <= frame -> nFormants; iformant ++)
			mat -> z [iformant] [iframe] = frame -> formant [iformant]. frequency;
	}
	Matrix_formula (mat, formula, interpreter, NULL); cherror
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		for (iformant = 1; iformant <= frame -> nFormants; iformant ++)
			frame -> formant [iformant]. frequency = mat -> z [iformant] [iframe];
	}
end:
	forget (mat);
	iferror return 0;
	return 1;
}

void Formant_getExtrema (Formant me, int iformant, double tmin, double tmax, double *fmin, double *fmax) {
	long itmin, itmax, iframe;
	if (fmin) *fmin = 0.0;
	if (fmax) *fmax = 0.0;
	if (iformant < 1) return;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	for (iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		double f;
		if (iformant > frame -> nFormants) continue;
		f = frame -> formant [iformant]. frequency;
		if (! f) continue;
		if (fmin) if (f < *fmin || *fmin == 0.0) *fmin = f;
		if (fmax) if (f > *fmax) *fmax = f;
	}
}

void Formant_getMinimumAndTime (Formant me, int iformant, double tmin, double tmax, int bark, int interpolate,
	double *return_minimum, double *return_timeOfMinimum)
{
	Sampled_getMinimumAndX (me, tmin, tmax, iformant << 1, bark, interpolate, return_minimum, return_timeOfMinimum);
	if (return_minimum && *return_minimum <= 0.0) *return_minimum = NUMundefined;
}

double Formant_getMinimum (Formant me, int iformant, double tmin, double tmax, int bark, int interpolate) {
	double minimum;
	Formant_getMinimumAndTime (me, iformant, tmin, tmax, bark, interpolate, & minimum, NULL);
	return minimum;
}

double Formant_getTimeOfMinimum (Formant me, int iformant, double tmin, double tmax, int bark, int interpolate) {
	double time;
	Formant_getMinimumAndTime (me, iformant, tmin, tmax, bark, interpolate, NULL, & time);
	return time;
}

void Formant_getMaximumAndTime (Formant me, int iformant, double tmin, double tmax, int bark, int interpolate,
	double *return_maximum, double *return_timeOfMaximum)
{
	Sampled_getMaximumAndX (me, tmin, tmax, iformant << 1, bark, interpolate, return_maximum, return_timeOfMaximum);
	if (return_maximum && *return_maximum <= 0.0) *return_maximum = NUMundefined;   /* Unlikely. */
}

double Formant_getMaximum (Formant me, int iformant, double tmin, double tmax, int bark, int interpolate) {
	double maximum;
	Formant_getMaximumAndTime (me, iformant, tmin, tmax, bark, interpolate, & maximum, NULL);
	return maximum;
}

double Formant_getTimeOfMaximum (Formant me, int iformant, double tmin, double tmax, int bark, int interpolate) {
	double time;
	Formant_getMaximumAndTime (me, iformant, tmin, tmax, bark, interpolate, NULL, & time);
	return time;
}

double Formant_getQuantile (Formant me, int iformant, double quantile, double tmin, double tmax, int bark) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, iformant << 1, bark);
}

double Formant_getMean (Formant me, int iformant, double tmin, double tmax, int bark) {
	return Sampled_getMean (me, tmin, tmax, iformant << 1, bark, TRUE);
}

double Formant_getStandardDeviation (Formant me, int iformant, double tmin, double tmax, int bark) {
	long itmin, itmax, iframe, n = 0;
	double mean, sum = 0.0;
	if (iformant < 1 || tmin == NUMundefined || tmax == NUMundefined) return NUMundefined;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return NUMundefined;
	mean = Formant_getMean (me, iformant, tmin, tmax, bark);
	for (iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		double f;
		if (iformant > frame -> nFormants) continue;
		f = frame -> formant [iformant]. frequency;
		if (! f) continue;
		if (bark) f = NUMhertzToBark (f);
		n += 1;
		sum += (f - mean) * (f - mean);
	}
	if (n > 1) return sqrt (sum / (n - 1));
	return NUMundefined;
}

double Formant_getValueAtTime (Formant me, int iformant, double time, int bark) {
	return Sampled_getValueAtX (me, time, iformant << 1, bark, TRUE);
}

double Formant_getBandwidthAtTime (Formant me, int iformant, double time, int bark) {
	return Sampled_getValueAtX (me, time, (iformant << 1) + 1, bark, TRUE);
}

double Formant_getQuantileOfBandwidth (Formant me, int iformant, double quantile, double tmin, double tmax, int bark) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, (iformant << 1) + 1, bark);
}

void Formant_scatterPlot (Formant me, Graphics g, double tmin, double tmax,
	int iformant1, double fmin1, double fmax1, int iformant2, double fmin2, double fmax2,
	double size_mm, const wchar_t *mark, int garnish)
{
	long itmin, itmax, iframe;
	if (iformant1 < 1 || iformant2 < 1) return;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	if (fmax1 == fmin1)
		Formant_getExtrema (me, iformant1, tmin, tmax, & fmin1, & fmax1);
	if (fmax1 == fmin1) return;
	if (fmax2 == fmin2)
		Formant_getExtrema (me, iformant2, tmin, tmax, & fmin2, & fmax2);
	if (fmax2 == fmin2) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, fmin1, fmax1, fmin2, fmax2);
	for (iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		double x, y;
		if (iformant1 > frame -> nFormants || iformant2 > frame -> nFormants) continue;
		x = frame -> formant [iformant1]. frequency;
		y = frame -> formant [iformant2]. frequency;
		if (x == 0.0 || y == 0.0) continue;
		Graphics_mark (g, x, y, size_mm, mark);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		wchar_t text [100];
		Graphics_drawInnerBox (g);
		swprintf (text, 100, L"%%F_%d (Hz)", iformant1);
		Graphics_textBottom (g, 1, text);
		swprintf (text, 100, L"%%F_%d (Hz)", iformant2);
		Graphics_textLeft (g, 1, text);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

Matrix Formant_to_Matrix (Formant me, int iformant) {
	long iframe;
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		1, 1, 1, 1, 1);
	if (! thee) return NULL;
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		thy z [1] [iframe] = iformant <= frame -> nFormants ?
			frame -> formant [iformant]. frequency : 0.0;
	}
	return thee;
}

Matrix Formant_to_Matrix_bandwidths (Formant me, int iformant) {
	long iframe;
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		1, 1, 1, 1, 1);
	if (! thee) return NULL;
	for (iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		thy z [1] [iframe] = iformant <= frame -> nFormants ?
			frame -> formant [iformant]. bandwidth : 0.0;
	}
	return thee;
}

/*** Viterbi methods. ***/

struct fparm { Formant me, thee; double dfCost, bfCost, octaveJumpCost, refF [1 + 5]; };

static double getLocalCost (long iframe, long icand, int itrack, void *closure) {
	struct fparm *me = closure;
	Formant_Frame frame = & my my frame [iframe];
	Formant_Formant candidate;
	if (icand > frame -> nFormants) return 1e30;
	candidate = & frame -> formant [icand];
	/*if (candidate -> frequency <= 0.0) candidate -> frequency = 0.001;
		/*Melder_fatal ("Weird formant frequency %ls Hertz.", Melder_double (candidate -> frequency))*/;
	Melder_assert (candidate -> bandwidth > 0.0);
	Melder_assert (itrack > 0 && itrack <= 5);
	return my dfCost * fabs (candidate -> frequency - my refF [itrack]) +
		my bfCost * candidate -> bandwidth / candidate -> frequency;
}
static double getTransitionCost (long iframe, long icand1, long icand2, int itrack, void *closure) {
	struct fparm *me = closure;
	Formant_Frame prevFrame = & my my frame [iframe - 1], curFrame = & my my frame [iframe];
	double f1, f2;
	(void) itrack;
	if (icand1 > prevFrame -> nFormants || icand2 > curFrame -> nFormants) return 1e30;
	f1 = prevFrame -> formant [icand1]. frequency;
	f2 = curFrame -> formant [icand2]. frequency;
	/*Melder_assert (f1 > 0.0);*/
	/*Melder_assert (f2 > 0.0);*/
	return my octaveJumpCost * fabs (NUMlog2 (f1 / f2));
}
static void putResult (long iframe, long place, int itrack, void *closure) {
	struct fparm *me = closure;
	Melder_assert (iframe > 0 && iframe <= my my nx);
	Melder_assert (itrack > 0 && itrack <= 5);
	Melder_assert (place > 0);
	Melder_assert (place <= my my frame [iframe]. nFormants);
	my thy frame [iframe]. formant [itrack] = my my frame [iframe]. formant [place];
}

Formant Formant_tracker (Formant me, int ntrack,
	double refF1, double refF2, double refF3, double refF4, double refF5,
	double dfCost,   /* Per kHz. */
	double bfCost, double octaveJumpCost)
{
	Formant thee;
	long iframe, nformmin = Formant_getMinNumFormants (me);
	struct fparm parm;
	if (ntrack > nformmin) return Melder_errorp ("(Formant_tracker:) "
		"Number of tracks (%d) should not exceed minimum number of formants (%ld)", ntrack, nformmin);
	if (! (thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, ntrack))) return NULL;
	for (iframe = 1; iframe <= thy nx; iframe ++) {
		thy frame [iframe]. formant = NUMstructvector (Formant_Formant, 1, ntrack);
		thy frame [iframe]. nFormants = ntrack;
		thy frame [iframe]. intensity = my frame [iframe]. intensity;
	}
	/* BUG: limit costs to 1e10 or so */
	parm.me = me;
	parm.thee = thee;
	parm.dfCost = dfCost / 1000;   /* Per Hz. */
	parm.bfCost = bfCost;
	parm.octaveJumpCost = octaveJumpCost;
	parm.refF [1] = refF1;
	parm.refF [2] = refF2;
	parm.refF [3] = refF3;
	parm.refF [4] = refF4;
	parm.refF [5] = refF5;
	if (! NUM_viterbi_multi (my nx, my maxnFormants, ntrack,
		getLocalCost, getTransitionCost, putResult, & parm)) { forget (thee); return NULL; }
	return thee;
}

Table Formant_downto_Table (Formant me, bool includeFrameNumbers,
	bool includeTimes, int timeDecimals,
	bool includeIntensity, int intensityDecimals,
	bool includeNumberOfFormants, int frequencyDecimals,
	bool includeBandwidths)
{
	Table thee = Table_createWithoutColumnNames (my nx, includeFrameNumbers + includeTimes + includeIntensity +
		includeNumberOfFormants + my maxnFormants * (1 + includeBandwidths)); cherror
	long icol = 0;
	if (includeFrameNumbers) { Table_setColumnLabel (thee, ++ icol, L"frame"); cherror }
	if (includeTimes) { Table_setColumnLabel (thee, ++ icol, L"time(s)"); cherror }
	if (includeIntensity) { Table_setColumnLabel (thee, ++ icol, L"intensity"); cherror }
	if (includeNumberOfFormants) { Table_setColumnLabel (thee, ++ icol, L"nformants"); cherror }
	for (long iformant = 1; iformant <= my maxnFormants; iformant ++) {
		Table_setColumnLabel (thee, ++ icol, Melder_wcscat3 (L"F", Melder_integer (iformant), L"(Hz)")); cherror
		if (includeBandwidths) { Table_setColumnLabel (thee, ++ icol, Melder_wcscat3 (L"B", Melder_integer (iformant), L"(Hz)")); cherror }
	}
	for (long iframe = 1; iframe <= my nx; iframe ++) {
		icol = 0;
		if (includeFrameNumbers) { Table_setNumericValue (thee, iframe, ++ icol,
			iframe); cherror }
		if (includeTimes) { Table_setStringValue (thee, iframe, ++ icol,
			Melder_fixed (my x1 + (iframe - 1) * my dx, timeDecimals)); cherror }
		Formant_Frame frame = & my frame [iframe];
		if (includeIntensity) { Table_setStringValue (thee, iframe, ++ icol,
			Melder_fixed (frame -> intensity, intensityDecimals)); cherror }
		if (includeNumberOfFormants) { Table_setNumericValue (thee, iframe, ++ icol,
			frame -> nFormants); cherror }
		for (long iformant = 1; iformant <= frame -> nFormants; iformant ++) {
			Formant_Formant formant = & frame -> formant [iformant];
			Table_setStringValue (thee, iframe, ++ icol,
				Melder_fixed (formant -> frequency, frequencyDecimals)); cherror
			if (includeBandwidths) { Table_setStringValue (thee, iframe, ++ icol,
				Melder_fixed (formant -> bandwidth, frequencyDecimals)); cherror }
		}
		for (long iformant = frame -> nFormants + 1; iformant <= my maxnFormants; iformant ++) {
			Table_setNumericValue (thee, iframe, ++ icol, NUMundefined); cherror
			if (includeBandwidths) { Table_setNumericValue (thee, iframe, ++ icol, NUMundefined); cherror }
		}
	}
end:
	iferror forget (thee);
	return thee;
}

void Formant_list (Formant me, bool includeFrameNumbers,
	bool includeTimes, int timeDecimals,
	bool includeIntensity, int intensityDecimals,
	bool includeNumberOfFormants, int frequencyDecimals,
	bool includeBandwidths)
{
	Table table = Formant_downto_Table (me, includeFrameNumbers, includeTimes, timeDecimals,
		includeIntensity, intensityDecimals,
		includeNumberOfFormants, frequencyDecimals, includeBandwidths); cherror
	Table_list (table, false);
end:
	iferror { Melder_clearError (); Melder_information1 (L"Nothing to list."); }
	forget (table);
}

/* End of file Formant.c */
