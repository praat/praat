/* Formant.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
 * pb 2011/05/24 C++
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

Thing_implement (Formant, Sampled, 2);   // version 1 = with intensity, 2 = double

void structFormant :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"Time sampling:");
	MelderInfo_writeLine (L"   Number of frames: ", Melder_integer (nx));
	MelderInfo_writeLine (L"   Time step: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"   First frame centred at: ", Melder_double (x1), L" seconds");
}

double structFormant :: v_getValueAtSample (long iframe, long which, int units) {
	Formant_Frame frame = & d_frames [iframe];
	long iformant = which >> 1;
	if (iformant < 1 || iformant > frame -> nFormants) return NUMundefined;
	double frequency = frame -> formant [iformant]. frequency;
	if ((which & 1) == 0) {
		return units ? NUMhertzToBark (frequency) : frequency;
	} else {
		double bandwidth = frame -> formant [iformant]. bandwidth;
		if (units) {
			double fleft = frequency - 0.5 * bandwidth, fright = frequency + 0.5 * bandwidth;
			fleft = fleft <= 0 ? 0 : NUMhertzToBark (fleft);   // prevent NUMundefined
			fright = NUMhertzToBark (fright);
			return fright - fleft;
		}
		return bandwidth;
	}
	return NUMundefined;
}

Formant Formant_create (double tmin, double tmax, long nt, double dt, double t1,
	int maxnFormants)
{
	try {
		autoFormant me = Thing_new (Formant);
		Sampled_init (me.peek(), tmin, tmax, nt, dt, t1);
		my d_frames = NUMvector <structFormant_Frame> (1, nt);
		my maxnFormants = maxnFormants;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Formant object not created.");
	}
}

long Formant_getMinNumFormants (Formant me) {
	long minNumFormants = 100000000;
	for (long iframe = 1; iframe <= my nx; iframe ++)
		if (my d_frames [iframe]. nFormants < minNumFormants)
			minNumFormants = my d_frames [iframe]. nFormants;
	return minNumFormants;
}

long Formant_getMaxNumFormants (Formant me) {
	long maxNumFormants = 0;
	for (long iframe = 1; iframe <= my nx; iframe ++)
		if (my d_frames [iframe]. nFormants > maxNumFormants)
			maxNumFormants = my d_frames [iframe]. nFormants;
	return maxNumFormants;
}

void Formant_drawTracks (Formant me, Graphics g, double tmin, double tmax, double fmax, int garnish) {
	long itmin, itmax, ntrack = Formant_getMinNumFormants (me);
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, 0.0, fmax);
	for (long itrack = 1; itrack <= ntrack; itrack ++) {
		for (long iframe = itmin; iframe < itmax; iframe ++) {
			Formant_Frame curFrame = & my d_frames [iframe], nextFrame = & my d_frames [iframe + 1];
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
	long itmin, itmax;
	double maximumIntensity = 0.0, minimumIntensity;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);

	for (long iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my d_frames [iframe];
		if (frame -> intensity > maximumIntensity)
			maximumIntensity = frame -> intensity;
	}
	if (maximumIntensity == 0.0 || suppress_dB <= 0.0)
		minimumIntensity = 0.0;   /* Ignore. */
	else
		minimumIntensity = maximumIntensity / pow (10.0, suppress_dB / 10.0);

	for (long iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my d_frames [iframe];
		double x = Sampled_indexToX (me, iframe);
		if (frame -> intensity < minimumIntensity) continue;
		for (long iformant = 1; iformant <= frame -> nFormants; iformant ++) {
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

void Formant_formula_bandwidths (Formant me, const wchar_t *formula, Interpreter interpreter) {
	try {
		long nrow = Formant_getMaxNumFormants (me);
		if (nrow < 1)
			Melder_throw (L"No formants available.");
		autoMatrix mat = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, nrow + 0.5, nrow, 1, 1);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			for (long iformant = 1; iformant <= frame -> nFormants; iformant ++)
				mat -> z [iformant] [iframe] = frame -> formant [iformant]. bandwidth;
		}
		Matrix_formula (mat.peek(), formula, interpreter, NULL);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			for (long iformant = 1; iformant <= frame -> nFormants; iformant ++)
				frame -> formant [iformant]. bandwidth = mat -> z [iformant] [iframe];
		}
	} catch (MelderError) {
		Melder_throw (me, ": bandwidth formula not executed.");
	}
}

void Formant_formula_frequencies (Formant me, const wchar_t *formula, Interpreter interpreter) {
	try {
		long nrow = Formant_getMaxNumFormants (me);
		if (nrow < 1)
			Melder_throw ("No formants available.");
		autoMatrix mat = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, nrow + 0.5, nrow, 1, 1);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			for (long iformant = 1; iformant <= frame -> nFormants; iformant ++)
				mat -> z [iformant] [iframe] = frame -> formant [iformant]. frequency;
		}
		Matrix_formula (mat.peek(), formula, interpreter, NULL);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			for (long iformant = 1; iformant <= frame -> nFormants; iformant ++)
				frame -> formant [iformant]. frequency = mat -> z [iformant] [iframe];
		}
	} catch (MelderError) {
		Melder_throw (me, ": frequency formula not executed.");
	}
}

void Formant_getExtrema (Formant me, int iformant, double tmin, double tmax, double *fmin, double *fmax) {
	long itmin, itmax, iframe;
	if (fmin) *fmin = 0.0;
	if (fmax) *fmax = 0.0;
	if (iformant < 1) return;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	for (iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my d_frames [iframe];
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
	if (iformant < 1 || tmin == NUMundefined || tmax == NUMundefined) return NUMundefined;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	long itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return NUMundefined;
	double mean = Formant_getMean (me, iformant, tmin, tmax, bark);
	double sum = 0.0;
	long n = 0;
	for (long iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my d_frames [iframe];
		if (iformant > frame -> nFormants) continue;
		double f = frame -> formant [iformant]. frequency;
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
	if (iformant1 < 1 || iformant2 < 1) return;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	long itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) return;
	if (fmax1 == fmin1)
		Formant_getExtrema (me, iformant1, tmin, tmax, & fmin1, & fmax1);
	if (fmax1 == fmin1) return;
	if (fmax2 == fmin2)
		Formant_getExtrema (me, iformant2, tmin, tmax, & fmin2, & fmax2);
	if (fmax2 == fmin2) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, fmin1, fmax1, fmin2, fmax2);
	for (long iframe = itmin; iframe <= itmax; iframe ++) {
		Formant_Frame frame = & my d_frames [iframe];
		if (iformant1 > frame -> nFormants || iformant2 > frame -> nFormants) continue;
		double x = frame -> formant [iformant1]. frequency;
		double y = frame -> formant [iformant2]. frequency;
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
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			thy z [1] [iframe] = iformant <= frame -> nFormants ?
				frame -> formant [iformant]. frequency : 0.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": frequencies of formant ", iformant, " not converted to Matrix.");
	}
}

Matrix Formant_to_Matrix_bandwidths (Formant me, int iformant) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			thy z [1] [iframe] = iformant <= frame -> nFormants ?
				frame -> formant [iformant]. bandwidth : 0.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": bandwidths of formant ", iformant, " not converted to Matrix.");
	}
}

/*** Viterbi methods. ***/

struct fparm { Formant me, thee; double dfCost, bfCost, octaveJumpCost, refF [1 + 5]; };

static double getLocalCost (long iframe, long icand, int itrack, void *closure) {
	struct fparm *me = (struct fparm *) closure;
	Formant_Frame frame = & my my d_frames [iframe];
	Formant_Formant candidate;
	if (icand > frame -> nFormants) return 1e30;
	candidate = & frame -> formant [icand];
	/*if (candidate -> frequency <= 0.0) candidate -> frequency = 0.001;
		/*Melder_fatal ("Weird formant frequency %ls Hz.", Melder_double (candidate -> frequency))*/;
	Melder_assert (candidate -> bandwidth > 0.0);
	Melder_assert (itrack > 0 && itrack <= 5);
	return my dfCost * fabs (candidate -> frequency - my refF [itrack]) +
		my bfCost * candidate -> bandwidth / candidate -> frequency;
}
static double getTransitionCost (long iframe, long icand1, long icand2, int itrack, void *closure) {
	struct fparm *me = (struct fparm *) closure;
	Formant_Frame prevFrame = & my my d_frames [iframe - 1], curFrame = & my my d_frames [iframe];
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
	struct fparm *me = (struct fparm *) closure;
	Melder_assert (iframe > 0 && iframe <= my my nx);
	Melder_assert (itrack > 0 && itrack <= 5);
	Melder_assert (place > 0);
	Melder_assert (place <= my my d_frames [iframe]. nFormants);
	my thy d_frames [iframe]. formant [itrack] = my my d_frames [iframe]. formant [place];
}

Formant Formant_tracker (Formant me, int ntrack,
	double refF1, double refF2, double refF3, double refF4, double refF5,
	double dfCost,   /* Per kHz. */
	double bfCost, double octaveJumpCost)
{
	try {
		long nformmin = Formant_getMinNumFormants (me);
		struct fparm parm;
		if (ntrack > nformmin) Melder_throw ("Number of tracks (", ntrack, ") should not exceed minimum number of formants (", nformmin, ").");
		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, ntrack);
		for (long iframe = 1; iframe <= thy nx; iframe ++) {
			thy d_frames [iframe]. formant = NUMvector <structFormant_Formant> (1, ntrack);
			thy d_frames [iframe]. nFormants = ntrack;
			thy d_frames [iframe]. intensity = my d_frames [iframe]. intensity;
		}
		/* BUG: limit costs to 1e10 or so */
		parm.me = me;
		parm.thee = thee.peek();
		parm.dfCost = dfCost / 1000.0;   // per Hz
		parm.bfCost = bfCost;
		parm.octaveJumpCost = octaveJumpCost;
		parm.refF [1] = refF1;
		parm.refF [2] = refF2;
		parm.refF [3] = refF3;
		parm.refF [4] = refF4;
		parm.refF [5] = refF5;
		NUM_viterbi_multi (my nx, my maxnFormants, ntrack,
			getLocalCost, getTransitionCost, putResult, & parm);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not tracked.");
	}
}

Table Formant_downto_Table (Formant me, bool includeFrameNumbers,
	bool includeTimes, int timeDecimals,
	bool includeIntensity, int intensityDecimals,
	bool includeNumberOfFormants, int frequencyDecimals,
	bool includeBandwidths)
{
	try {
		autoTable thee = Table_createWithoutColumnNames (my nx, includeFrameNumbers + includeTimes + includeIntensity +
			includeNumberOfFormants + my maxnFormants * (1 + includeBandwidths));
		long icol = 0;
		if (includeFrameNumbers)     Table_setColumnLabel (thee.peek(), ++ icol, L"frame");
		if (includeTimes)            Table_setColumnLabel (thee.peek(), ++ icol, L"time(s)");
		if (includeIntensity)        Table_setColumnLabel (thee.peek(), ++ icol, L"intensity");
		if (includeNumberOfFormants) Table_setColumnLabel (thee.peek(), ++ icol, L"nformants");
		for (long iformant = 1; iformant <= my maxnFormants; iformant ++) {
			Table_setColumnLabel (thee.peek(), ++ icol, Melder_wcscat (L"F", Melder_integer (iformant), L"(Hz)"));
			if (includeBandwidths) { Table_setColumnLabel (thee.peek(), ++ icol, Melder_wcscat (L"B", Melder_integer (iformant), L"(Hz)")); }
		}
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			icol = 0;
			if (includeFrameNumbers)
				Table_setNumericValue (thee.peek(), iframe, ++ icol, iframe);
			if (includeTimes)
				Table_setStringValue (thee.peek(), iframe, ++ icol, Melder_fixed (my x1 + (iframe - 1) * my dx, timeDecimals));
			Formant_Frame frame = & my d_frames [iframe];
			if (includeIntensity)
				Table_setStringValue (thee.peek(), iframe, ++ icol, Melder_fixed (frame -> intensity, intensityDecimals));
			if (includeNumberOfFormants)
				Table_setNumericValue (thee.peek(), iframe, ++ icol, frame -> nFormants);
			for (long iformant = 1; iformant <= frame -> nFormants; iformant ++) {
				Formant_Formant formant = & frame -> formant [iformant];
				Table_setStringValue (thee.peek(), iframe, ++ icol, Melder_fixed (formant -> frequency, frequencyDecimals));
				if (includeBandwidths)
					Table_setStringValue (thee.peek(), iframe, ++ icol, Melder_fixed (formant -> bandwidth, frequencyDecimals));
			}
			for (long iformant = frame -> nFormants + 1; iformant <= my maxnFormants; iformant ++) {
				Table_setNumericValue (thee.peek(), iframe, ++ icol, NUMundefined);
				if (includeBandwidths)
					Table_setNumericValue (thee.peek(), iframe, ++ icol, NUMundefined);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Table.");
	}
}

void Formant_list (Formant me, bool includeFrameNumbers,
	bool includeTimes, int timeDecimals,
	bool includeIntensity, int intensityDecimals,
	bool includeNumberOfFormants, int frequencyDecimals,
	bool includeBandwidths)
{
	try {
		autoTable table = Formant_downto_Table (me, includeFrameNumbers, includeTimes, timeDecimals,
			includeIntensity, intensityDecimals,
			includeNumberOfFormants, frequencyDecimals, includeBandwidths);
		Table_list (table.peek(), false);
	} catch (MelderError) {
		Melder_throw (me, ": not listed.");
	}
}

/* End of file Formant.cpp */
