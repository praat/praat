/* Formant.cpp
 *
 * Copyright (C) 1992-2009,2011,2012,2014-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
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

#include "enums_getText.h"
#include "Formant_enums.h"
#include "enums_getValue.h"
#include "Formant_enums.h"

Thing_implement (Formant, Sampled, 2);   // version 1 = with intensity, 2 = double

void structFormant :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", nx);
	MelderInfo_writeLine (U"   Time step: ", dx, U" seconds");
	MelderInfo_writeLine (U"   First frame centred at: ", x1, U" seconds");
}

double structFormant :: v_getValueAtSample (integer iframe, integer which, int units) {
	const Formant_Frame frame = & our frames [iframe];
	const integer iformant = which >> 1;
	if (iformant < 1 || iformant > frame -> numberOfFormants)
		return undefined;
	const double frequency = frame -> formant [iformant]. frequency;
	if ((which & 1) == 0) {
		return ( units ? NUMhertzToBark (frequency) : frequency );
	} else {
		const double bandwidth = frame -> formant [iformant]. bandwidth;
		if (units) {
			double fleft = frequency - 0.5 * bandwidth, fright = frequency + 0.5 * bandwidth;
			fleft = ( fleft <= 0.0 ? 0.0 : NUMhertzToBark (fleft) );   // prevent undefined
			fright = NUMhertzToBark (fright);
			return fright - fleft;
		}
		return bandwidth;
	}
	return undefined;
}

autoFormant Formant_create (double tmin, double tmax, integer nt, double dt, double t1,
	integer maximumNumberOfFormants)
{
	try {
		autoFormant me = Thing_new (Formant);
		Sampled_init (me.get(), tmin, tmax, nt, dt, t1);
		my frames = newvectorzero <structFormant_Frame> (nt);
		my maxnFormants = maximumNumberOfFormants;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Formant object not created.");
	}
}

integer Formant_getMinNumFormants (Formant me) {
	integer minNumFormants = 100000000;
	for (integer iframe = 1; iframe <= my nx; iframe ++)
		if (my frames [iframe]. numberOfFormants < minNumFormants)
			minNumFormants = my frames [iframe]. numberOfFormants;
	return minNumFormants;
}

integer Formant_getMaxNumFormants (Formant me) {
	integer maxNumFormants = 0;
	for (integer iframe = 1; iframe <= my nx; iframe ++)
		if (my frames [iframe]. numberOfFormants > maxNumFormants)
			maxNumFormants = my frames [iframe]. numberOfFormants;
	return maxNumFormants;
}

void Formant_drawTracks (Formant me, Graphics g, double tmin, double tmax, double fmax, bool garnish) {
	const integer ntrack = Formant_getMinNumFormants (me);
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, 0.0, fmax);
	for (integer itrack = 1; itrack <= ntrack; itrack ++) {
		for (integer iframe = itmin; iframe < itmax; iframe ++) {
			const Formant_Frame curFrame = & my frames [iframe], nextFrame = & my frames [iframe + 1];
			const double x1 = Sampled_indexToX (me, iframe), x2 = Sampled_indexToX (me, iframe + 1);
			const double f1 = curFrame -> formant [itrack]. frequency;
			const double f2 = nextFrame -> formant [itrack]. frequency;
			if (isdefined (x1) && isdefined (f1) && isdefined (x2) && isdefined (f2))
				Graphics_line (g, x1, f1, x2, f2);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

void Formant_drawSpeckles_inside (Formant me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double suppress_dB, MelderColour oddColour, MelderColour evenColour, bool drawWithContrast)
{
	double maximumIntensity = 0.0, minimumIntensity;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		if (frame -> intensity > maximumIntensity)
			maximumIntensity = frame -> intensity;
	}
	if (maximumIntensity == 0.0 || suppress_dB <= 0.0)
		minimumIntensity = 0.0;   // ignore
	else
		minimumIntensity = maximumIntensity / pow (10.0, suppress_dB / 10.0);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		const double x = Sampled_indexToX (me, iframe);
		if (frame -> intensity < minimumIntensity)
			continue;
		for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++) {
			const double frequency = frame -> formant [iformant]. frequency;
			if (frequency >= fmin && frequency <= fmax) {
				if (drawWithContrast) {
					#if macintosh
						const double original_speckleSize = Graphics_inqSpeckleSize (g);
						Graphics_setSpeckleSize (g, 1.111 * original_speckleSize);
						Graphics_setColour (g, iformant % 2 == 1 ? evenColour : oddColour);
						Graphics_speckle (g, x, frequency);
						Graphics_setSpeckleSize (g, 0.900 * original_speckleSize);
						Graphics_setColour (g, iformant % 2 == 1 ? oddColour : evenColour);
						Graphics_speckle (g, x, frequency);
						Graphics_setSpeckleSize (g, original_speckleSize);
					#else
						const double original_lineWidth = Graphics_inqLineWidth (g);
						const bool even = ( iformant % 2 == 0 );
						const double speckleSize = Graphics_inqSpeckleSize (g);
						Graphics_setLineWidth (g, 1.0);
						if (even) {
							Graphics_setColour (g, evenColour);
							Graphics_fillCircle_mm (g, x, frequency, speckleSize * 1.4);
							Graphics_setColour (g, oddColour);
							Graphics_circle_mm (g, x, frequency, speckleSize * 1.4);
						} else {
							Graphics_setColour (g, oddColour);
							Graphics_fillCircle_mm (g, x, frequency, speckleSize * 1.15);
							Graphics_setColour (g, evenColour);
							Graphics_circle_mm (g, x, frequency, speckleSize * 1.15);
						}
						Graphics_setLineWidth (g, original_lineWidth);
					#endif
				} else {
					Graphics_setColour (g, iformant % 2 == 1 ? oddColour : evenColour);
					Graphics_speckle (g, x, frequency);
				}
			}
		}
	}
}

void Formant_drawSpeckles_inside (Formant me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double suppress_dB)
{
	Formant_drawSpeckles_inside (me, g, tmin, tmax, fmin, fmax, suppress_dB, Graphics_inqColour (g), Graphics_inqColour (g), false);
}

void Formant_drawSpeckles (Formant me, Graphics g, double tmin, double tmax, double fmax, double suppress_dB,
	bool garnish)
{
	Graphics_setInner (g);
	Formant_drawSpeckles_inside (me, g, tmin, tmax, 0.0, fmax, suppress_dB);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

void Formant_formula_bandwidths (Formant me, conststring32 formula, Interpreter interpreter) {
	try {
		const integer nrow = Formant_getMaxNumFormants (me);
		if (nrow < 1)
			Melder_throw (U"No formants available.");
		autoMatrix mat = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, nrow + 0.5, nrow, 1.0, 1.0);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++)
				mat -> z [iformant] [iframe] = frame -> formant [iformant]. bandwidth;
		}
		Matrix_formula (mat.get(), formula, interpreter, nullptr);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++)
				frame -> formant [iformant]. bandwidth = mat -> z [iformant] [iframe];
		}
	} catch (MelderError) {
		Melder_throw (me, U": bandwidth formula not executed.");
	}
}

void Formant_formula_frequencies (Formant me, conststring32 formula, Interpreter interpreter) {
	try {
		const integer nrow = Formant_getMaxNumFormants (me);
		if (nrow < 1)
			Melder_throw (U"No formants available.");
		autoMatrix mat = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, nrow + 0.5, nrow, 1.0, 1.0);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++)
				mat -> z [iformant] [iframe] = frame -> formant [iformant]. frequency;
		}
		Matrix_formula (mat.get(), formula, interpreter, nullptr);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++)
				frame -> formant [iformant]. frequency = mat -> z [iformant] [iframe];
		}
	} catch (MelderError) {
		Melder_throw (me, U": frequency formula not executed.");
	}
}

void Formant_getExtrema (Formant me, integer iformant, double tmin, double tmax, double *fmin, double *fmax) {
	if (fmin)
		*fmin = 0.0;
	if (fmax)
		*fmax = 0.0;
	if (iformant < 1)
		return;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		if (iformant > frame -> numberOfFormants)
			continue;
		const double f = frame -> formant [iformant]. frequency;
		if (f == 0.0)
			continue;
		if (fmin && (f < *fmin || *fmin == 0.0))
			*fmin = f;
		if (fmax && f > *fmax)
			*fmax = f;
	}
}

void Formant_getMinimumAndTime (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit, int interpolate,
	double *return_minimum, double *return_timeOfMinimum)
{
	Sampled_getMinimumAndX (me, tmin, tmax, iformant << 1, (int) unit, interpolate, return_minimum, return_timeOfMinimum);
	if (return_minimum && *return_minimum <= 0.0)
		*return_minimum = undefined;
}

double Formant_getMinimum (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit, int interpolate) {
	double minimum;
	Formant_getMinimumAndTime (me, iformant, tmin, tmax, unit, interpolate, & minimum, nullptr);
	return minimum;
}

double Formant_getTimeOfMinimum (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit, int interpolate) {
	double time;
	Formant_getMinimumAndTime (me, iformant, tmin, tmax, unit, interpolate, nullptr, & time);
	return time;
}

void Formant_getMaximumAndTime (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit, int interpolate,
	double *return_maximum, double *return_timeOfMaximum)
{
	Sampled_getMaximumAndX (me, tmin, tmax, iformant << 1, (int) unit, interpolate, return_maximum, return_timeOfMaximum);
	if (return_maximum && *return_maximum <= 0.0)
		*return_maximum = undefined;   // unlikely
}

double Formant_getMaximum (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit, int interpolate) {
	double maximum;
	Formant_getMaximumAndTime (me, iformant, tmin, tmax, unit, interpolate, & maximum, nullptr);
	return maximum;
}

double Formant_getTimeOfMaximum (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit, int interpolate) {
	double time;
	Formant_getMaximumAndTime (me, iformant, tmin, tmax, unit, interpolate, nullptr, & time);
	return time;
}

double Formant_getQuantile (Formant me, integer iformant, double quantile, double tmin, double tmax, kFormant_unit unit) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, iformant << 1, (int) unit);
}

double Formant_getMean (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit) {
	return Sampled_getMean (me, tmin, tmax, iformant << 1, (int) unit, true);
}

double Formant_getStandardDeviation (Formant me, integer iformant, double tmin, double tmax, kFormant_unit unit) {
	if (iformant < 1 || isundef (tmin) || isundef (tmax))
		return undefined;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return undefined;
	const double mean = Formant_getMean (me, iformant, tmin, tmax, unit);
	longdouble sum = 0.0;
	integer n = 0;
	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		if (iformant > frame -> numberOfFormants)
			continue;
		double f = frame -> formant [iformant]. frequency;
		if (f == 0.0)
			continue;
		if (unit == kFormant_unit::BARK)
			f = NUMhertzToBark (f);
		n += 1;
		sum += (f - mean) * (f - mean);
	}
	if (n > 1)
		return sqrt (double (sum) / (n - 1));
	return undefined;
}

double Formant_getValueAtTime (Formant me, integer iformant, double time, kFormant_unit unit) {
	return Sampled_getValueAtX (me, time, iformant << 1, (int) unit, true);
}

double Formant_getBandwidthAtTime (Formant me, integer iformant, double time, kFormant_unit unit) {
	return Sampled_getValueAtX (me, time, (iformant << 1) + 1, (int) unit, true);
}

double Formant_getQuantileOfBandwidth (Formant me, integer iformant, double quantile, double tmin, double tmax, kFormant_unit unit) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, (iformant << 1) + 1, (int) unit);
}

void Formant_scatterPlot (Formant me, Graphics g, double tmin, double tmax,
	integer iformant1, double fmin1, double fmax1, integer iformant2, double fmin2, double fmax2,
	double size_mm, conststring32 mark, bool garnish)
{
	if (iformant1 < 1 || iformant2 < 1)
		return;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	if (fmax1 == fmin1)
		Formant_getExtrema (me, iformant1, tmin, tmax, & fmin1, & fmax1);
	if (fmax1 == fmin1)
		return;
	if (fmax2 == fmin2)
		Formant_getExtrema (me, iformant2, tmin, tmax, & fmin2, & fmax2);
	if (fmax2 == fmin2)
		return;
	Graphics_setInner (g);
	Graphics_setWindow (g, fmin1, fmax1, fmin2, fmax2);
	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		if (iformant1 > frame -> numberOfFormants || iformant2 > frame -> numberOfFormants)
			continue;
		double x = frame -> formant [iformant1]. frequency;
		double y = frame -> formant [iformant2]. frequency;
		if (x == 0.0 || y == 0.0)
			continue;
		Graphics_mark (g, x, y, size_mm, mark);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, Melder_cat (U"%%F_", iformant1, U" (Hz)"));
		Graphics_textLeft (g, true, Melder_cat (U"%%F_", iformant2, U" (Hz)"));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoMatrix Formant_to_Matrix (Formant me, integer iformant) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			thy z [1] [iframe] = ( iformant <= frame -> numberOfFormants ? frame -> formant [iformant]. frequency : 0.0 );
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": frequencies of formant ", iformant, U" not converted to Matrix.");
	}
}

autoMatrix Formant_to_Matrix_bandwidths (Formant me, integer iformant) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			thy z [1] [iframe] = ( iformant <= frame -> numberOfFormants ? frame -> formant [iformant]. bandwidth : 0.0 );
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": bandwidths of formant ", iformant, U" not converted to Matrix.");
	}
}

/*** Viterbi methods. ***/

struct fparm { Formant me, thee; double dfCost, bfCost, octaveJumpCost, refF [1 + 5]; };

static double getLocalCost (integer iframe, integer icand, integer itrack, void *closure) {
	struct fparm *me = (struct fparm *) closure;
	const Formant_Frame frame = & my my frames [iframe];
	if (icand > frame -> numberOfFormants)
		return 1e30;
	Formant_Formant candidate = & frame -> formant [icand];
	/*if (candidate -> frequency <= 0.0) candidate -> frequency = 0.001;
		Melder_fatal (U"Weird formant frequency ", candidate -> frequency, U" Hz.")*/;
	Melder_assert (candidate -> bandwidth > 0.0);
	Melder_assert (itrack > 0 && itrack <= 5);
	return my dfCost * fabs (candidate -> frequency - my refF [itrack]) +
		my bfCost * candidate -> bandwidth / candidate -> frequency;
}
static double getTransitionCost (integer iframe, integer icand1, integer icand2, integer /* itrack */, void *closure) {
	struct fparm *me = (struct fparm *) closure;
	const Formant_Frame prevFrame = & my my frames [iframe - 1], curFrame = & my my frames [iframe];
	if (icand1 > prevFrame -> numberOfFormants || icand2 > curFrame -> numberOfFormants)
		return 1e30;
	const double f1 = prevFrame -> formant [icand1]. frequency;
	const double f2 = curFrame -> formant [icand2]. frequency;
	/*Melder_assert (f1 > 0.0);*/
	/*Melder_assert (f2 > 0.0);*/
	return my octaveJumpCost * fabs (NUMlog2 (f1 / f2));
}
static void putResult (integer iframe, integer place, integer itrack, void *closure) {
	struct fparm *me = (struct fparm *) closure;
	Melder_assert (iframe > 0 && iframe <= my my nx);
	Melder_assert (itrack > 0 && itrack <= 5);
	Melder_assert (place > 0);
	Melder_assert (place <= my my frames [iframe]. numberOfFormants);
	my thy frames [iframe]. formant [itrack] = my my frames [iframe]. formant [place];
}

autoFormant Formant_tracker (Formant me, integer ntrack,
	double refF1, double refF2, double refF3, double refF4, double refF5,
	double dfCost,   /* Per kHz. */
	double bfCost, double octaveJumpCost)
{
	try {
		integer nformmin = Formant_getMinNumFormants (me);
		struct fparm parm;
		if (ntrack > nformmin) Melder_throw (U"Number of tracks (", ntrack, U") should not exceed minimum number of formants (", nformmin, U").");
		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, ntrack);
		for (integer iframe = 1; iframe <= thy nx; iframe ++) {
			thy frames [iframe]. formant = newvectorzero <structFormant_Formant> (ntrack);
			thy frames [iframe]. numberOfFormants = ntrack;   // maintain invariant
			thy frames [iframe]. intensity = my frames [iframe]. intensity;
		}
		/* BUG: limit costs to 1e10 or so */
		parm.me = me;
		parm.thee = thee.get();
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
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not tracked.");
	}
}

autoTable Formant_downto_Table (Formant me, bool includeFrameNumbers,
	bool includeTimes, integer timeDecimals,
	bool includeIntensity, integer intensityDecimals,
	bool includeNumberOfFormants, integer frequencyDecimals,
	bool includeBandwidths)
{
	try {
		autoTable thee = Table_createWithoutColumnNames (my nx, includeFrameNumbers + includeTimes + includeIntensity +
			includeNumberOfFormants + my maxnFormants * (1 + includeBandwidths));
		integer icol = 0;
		if (includeFrameNumbers)     Table_setColumnLabel (thee.get(), ++ icol, U"frame");
		if (includeTimes)            Table_setColumnLabel (thee.get(), ++ icol, U"time(s)");
		if (includeIntensity)        Table_setColumnLabel (thee.get(), ++ icol, U"intensity");
		if (includeNumberOfFormants) Table_setColumnLabel (thee.get(), ++ icol, U"nformants");
		for (integer iformant = 1; iformant <= my maxnFormants; iformant ++) {
			Table_setColumnLabel (thee.get(), ++ icol, Melder_cat (U"F", iformant, U"(Hz)"));
			if (includeBandwidths) { Table_setColumnLabel (thee.get(), ++ icol, Melder_cat (U"B", iformant, U"(Hz)")); }
		}
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			icol = 0;
			if (includeFrameNumbers)
				Table_setNumericValue (thee.get(), iframe, ++ icol, iframe);
			if (includeTimes)
				Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (my x1 + (iframe - 1) * my dx, timeDecimals));
			const Formant_Frame frame = & my frames [iframe];
			if (includeIntensity)
				Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (frame -> intensity, intensityDecimals));
			if (includeNumberOfFormants)
				Table_setNumericValue (thee.get(), iframe, ++ icol, frame -> numberOfFormants);
			for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++) {
				const Formant_Formant formant = & frame -> formant [iformant];
				Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (formant -> frequency, frequencyDecimals));
				if (includeBandwidths)
					Table_setStringValue (thee.get(), iframe, ++ icol, Melder_fixed (formant -> bandwidth, frequencyDecimals));
			}
			for (integer iformant = frame -> numberOfFormants + 1; iformant <= my maxnFormants; iformant ++) {
				Table_setNumericValue (thee.get(), iframe, ++ icol, undefined);
				if (includeBandwidths)
					Table_setNumericValue (thee.get(), iframe, ++ icol, undefined);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

void Formant_list (Formant me, bool includeFrameNumbers,
	bool includeTimes, integer timeDecimals,
	bool includeIntensity, integer intensityDecimals,
	bool includeNumberOfFormants, integer frequencyDecimals,
	bool includeBandwidths)
{
	try {
		autoTable table = Formant_downto_Table (me, includeFrameNumbers, includeTimes, timeDecimals,
				includeIntensity, intensityDecimals,
				includeNumberOfFormants, frequencyDecimals, includeBandwidths);
		Table_list (table.get(), false);
	} catch (MelderError) {
		Melder_throw (me, U": not listed.");
	}
}

/* End of file Formant.cpp */
