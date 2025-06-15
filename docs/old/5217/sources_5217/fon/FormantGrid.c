/* FormantGrid.c
 *
 * Copyright (C) 2008-2011 Paul Boersma & David Weenink
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
 * pb 2008/04/24 created
 * pb 2008/04/25 playPart
 * pb 2008/04/27 removePointsBetween, filter_noscale
 * pb 2008/09/23 shiftX, scaleX
 * pb 2008/10/28 FormantGrid_to_Formant, FormantGrid_formula
 * pb 2008/11/16 FormantGrid_init
 * pb 2009/01/18 Interpreter argument to formula
 * pb 2011/03/01 moved Formant filtering here, from FormantTier (reimplemented)
 */

#include "FormantGrid.h"
#include "PitchTier_to_Sound.h"
#include "Formula.h"

#include "oo_DESTROY.h"
#include "FormantGrid_def.h"
#include "oo_COPY.h"
#include "FormantGrid_def.h"
#include "oo_EQUAL.h"
#include "FormantGrid_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantGrid_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantGrid_def.h"
#include "oo_READ_TEXT.h"
#include "FormantGrid_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantGrid_def.h"
#include "oo_READ_BINARY.h"
#include "FormantGrid_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantGrid_def.h"

static double getVector (I, long irow, long icol) {
	iam (FormantGrid);
	RealTier tier = my formants -> item [irow];
	return RealTier_getValueAtIndex (tier, icol);
}

static double getFunction1 (I, long irow, double x) {
	iam (FormantGrid);
	RealTier tier = my formants -> item [irow];
	return RealTier_getValueAtTime (tier, x);
}

static const wchar_t * getUnitText (I, long ilevel, int unit, unsigned long flags) {
	(void) void_me;
	(void) ilevel;
	(void) unit;
	(void) flags;
	return ilevel & 1 ? L"Formant (Hz)" : L"Bandwidth (Hz)";
}

static void shiftX (I, double xfrom, double xto) {
	iam (FormantGrid);
	inherited (FormantGrid) shiftX (me, xfrom, xto);
	for (long i = 1; i <= my formants -> size; i ++) {
		RealTier tier = my formants -> item [i];
		tier -> methods -> shiftX (tier, xfrom, xto);
	}
	for (long i = 1; i <= my bandwidths -> size; i ++) {
		RealTier tier = my bandwidths -> item [i];
		tier -> methods -> shiftX (tier, xfrom, xto);
	}
}

static void scaleX (I, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	iam (FormantGrid);
	inherited (FormantGrid) scaleX (me, xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= my formants -> size; i ++) {
		RealTier tier = my formants -> item [i];
		tier -> methods -> scaleX (tier, xminfrom, xmaxfrom, xminto, xmaxto);
	}
	for (long i = 1; i <= my bandwidths -> size; i ++) {
		RealTier tier = my bandwidths -> item [i];
		tier -> methods -> scaleX (tier, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

class_methods (FormantGrid, Function) {
	class_method_local (FormantGrid, destroy)
	class_method_local (FormantGrid, copy)
	class_method_local (FormantGrid, equal)
	class_method_local (FormantGrid, canWriteAsEncoding)
	class_method_local (FormantGrid, writeText)
	class_method_local (FormantGrid, readText)
	class_method_local (FormantGrid, writeBinary)
	class_method_local (FormantGrid, readBinary)
	class_method_local (FormantGrid, description)
	class_method (getVector)
	class_method (getFunction1)
	class_method (getUnitText)
	class_method (shiftX)
	class_method (scaleX)
	class_methods_end
}

int FormantGrid_init (I, double tmin, double tmax, long numberOfFormants) {
	iam (FormantGrid);
	my formants = Ordered_create (); cherror
	my bandwidths = Ordered_create (); cherror
	for (long iformant = 1; iformant <= numberOfFormants; iformant ++) {
		RealTier formant = RealTier_create (tmin, tmax); cherror
		Collection_addItem (my formants, formant); cherror
		RealTier bandwidth = RealTier_create (tmin, tmax); cherror
		Collection_addItem (my bandwidths, bandwidth); cherror
	}
	my xmin = tmin;
	my xmax = tmax;
end:
	iferror return 0;
	return 1;
}

FormantGrid FormantGrid_createEmpty (double tmin, double tmax, long numberOfFormants) {
	FormantGrid me = new (FormantGrid); cherror
	FormantGrid_init (me, tmin, tmax, numberOfFormants); cherror
end:
	iferror forget (me);
	return me;
}

FormantGrid FormantGrid_create (double tmin, double tmax, long numberOfFormants,
	double initialFirstFormant, double initialFormantSpacing,
	double initialFirstBandwidth, double initialBandwidthSpacing)
{
	FormantGrid me = FormantGrid_createEmpty (tmin, tmax, numberOfFormants); cherror
	for (long iformant = 1; iformant <= numberOfFormants; iformant ++) {
		FormantGrid_addFormantPoint (me, iformant, 0.5 * (tmin + tmax),
			initialFirstFormant + (iformant - 1) * initialFormantSpacing); cherror
		FormantGrid_addBandwidthPoint (me, iformant, 0.5 * (tmin + tmax),
			initialFirstBandwidth + (iformant - 1) * initialBandwidthSpacing); cherror
	}
end:
	iferror forget (me);
	return me;
}

int FormantGrid_addFormantPoint (FormantGrid me, long iformant, double t, double value) {
	if (iformant < 1 || iformant > my formants -> size) error1 (L"No such formant number.");
	RealTier formantTier = my formants -> item [iformant];
	RealTier_addPoint (formantTier, t, value);
end:
	iferror return 0;
	return 1;
}

int FormantGrid_addBandwidthPoint (FormantGrid me, long iformant, double t, double value) {
	if (iformant < 1 || iformant > my formants -> size) error1 (L"No such formant number.");
	RealTier bandwidthTier = my bandwidths -> item [iformant];
	RealTier_addPoint (bandwidthTier, t, value);
end:
	iferror return 0;
	return 1;
}

double FormantGrid_getFormantAtTime (FormantGrid me, long iformant, double t) {
	if (iformant < 1 || iformant > my formants -> size) return NUMundefined;
	return RealTier_getValueAtTime (my formants -> item [iformant], t);
}

double FormantGrid_getBandwidthAtTime (FormantGrid me, long iformant, double t) {
	if (iformant < 1 || iformant > my bandwidths -> size) return NUMundefined;
	return RealTier_getValueAtTime (my bandwidths -> item [iformant], t);
}

void FormantGrid_removeFormantPointsBetween (FormantGrid me, long iformant, double tmin, double tmax) {
	if (iformant < 1 || iformant > my formants -> size) return;
	AnyTier_removePointsBetween (my formants -> item [iformant], tmin, tmax);
}

void FormantGrid_removeBandwidthPointsBetween (FormantGrid me, long iformant, double tmin, double tmax) {
	if (iformant < 1 || iformant > my bandwidths -> size) return;
	AnyTier_removePointsBetween (my bandwidths -> item [iformant], tmin, tmax);
}

void Sound_FormantGrid_filter_inline (Sound me, FormantGrid formantGrid) {
	double dt = my dx;
	if (formantGrid -> formants -> size && formantGrid -> bandwidths -> size)
	for (long iformant = 1; iformant <= formantGrid -> formants -> size; iformant ++) {
		RealTier formantTier = formantGrid -> formants -> item [iformant];
		RealTier bandwidthTier = formantGrid -> bandwidths -> item [iformant];
		for (long isamp = 1; isamp <= my nx; isamp ++) {
			double t = my x1 + (isamp - 1) * my dx;
			/*
			 * Compute LP coefficients.
			 */
			double formant, bandwidth;
			formant = RealTier_getValueAtTime (formantTier, t);
			bandwidth = RealTier_getValueAtTime (bandwidthTier, t);
			if (NUMdefined (formant) && NUMdefined (bandwidth)) {
				double cosomdt = cos (2 * NUMpi * formant * dt);
				double r = exp (- NUMpi * bandwidth * dt);
				/* Formants at 0 Hz or the Nyquist are single poles, others are double poles. */
				if (fabs (cosomdt) > 0.999999) {   /* Allow for round-off errors. */
					/* single pole: D(z) = 1 - r z^-1 */
					for (long channel = 1; channel <= my ny; channel ++) {
						if (isamp > 1) my z [channel] [isamp] += r * my z [channel] [isamp - 1];
					}
				} else {
					/* double pole: D(z) = 1 + p z^-1 + q z^-2 */
					double p = - 2 * r * cosomdt;
					double q = r * r;
					for (long channel = 1; channel <= my ny; channel ++) {
						if (isamp > 1) my z [channel] [isamp] -= p * my z [channel] [isamp - 1];
						if (isamp > 2) my z [channel] [isamp] -= q * my z [channel] [isamp - 2];
					}
				}
			}
		}
	}
}

Sound Sound_FormantGrid_filter (Sound me, FormantGrid formantGrid) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_FormantGrid_filter_inline (thee, formantGrid);
	Vector_scale (thee, 0.99);
	return thee;
}

Sound Sound_FormantGrid_filter_noscale (Sound me, FormantGrid formantGrid) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_FormantGrid_filter_inline (thee, formantGrid);
	return thee;
}

Sound FormantGrid_to_Sound (FormantGrid me, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2)
{
	PitchTier pitch = NULL;
	Sound thee = NULL;

	pitch = PitchTier_create (my xmin, my xmax); cherror
	RealTier_addPoint (pitch, my xmin + tStart * (my xmax - my xmin), f0Start); cherror
	RealTier_addPoint (pitch, my xmin + tMid * (my xmax - my xmin), f0Mid); cherror
	RealTier_addPoint (pitch, my xmax - (1.0 - tEnd) * (my xmax - my xmin), f0End); cherror
	thee = PitchTier_to_Sound_phonation (pitch, samplingFrequency,
		adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2, false); cherror
	Sound_FormantGrid_filter_inline (thee, me);
end:
	forget (pitch);
	iferror forget (thee);
	return thee;
}

int FormantGrid_playPart (FormantGrid me, double tmin, double tmax, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure)
{
	Sound sound = FormantGrid_to_Sound (me, samplingFrequency,
		tStart, f0Start, tMid, f0Mid, tEnd, f0End,
		adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2); cherror
	Vector_scale (sound, 0.99);
	Sound_playPart (sound, tmin, tmax, playCallback, playClosure);
end:
	forget (sound);
	iferror return 0;
	return 1;
}

int FormantGrid_formula_bandwidths (I, const wchar_t *expression, Interpreter interpreter, thou) {
	iam (FormantGrid);
	thouart (FormantGrid);
	Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	if (thee == NULL) thee = me;
	for (long irow = 1; irow <= my formants -> size; irow ++) {
		RealTier bandwidth = thy bandwidths -> item [irow];
		for (long icol = 1; icol <= bandwidth -> points -> size; icol ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result); cherror
			if (result. result.numericResult == NUMundefined)
				error1 (L"Cannot put an undefined value into the tier.\nFormula not finished.")
			((RealPoint) bandwidth -> points -> item [icol]) -> value = result. result.numericResult;
		}
	}
end:
	iferror return 0;
	return 1;
}

int FormantGrid_formula_frequencies (I, const wchar_t *expression, Interpreter interpreter, thou) {
	iam (FormantGrid);
	thouart (FormantGrid);
	Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	if (thee == NULL) thee = me;
	for (long irow = 1; irow <= my formants -> size; irow ++) {
		RealTier formant = thy formants -> item [irow];
		for (long icol = 1; icol <= formant -> points -> size; icol ++) {
			struct Formula_Result result;
			Formula_run (irow, icol, & result); cherror
			if (result. result.numericResult == NUMundefined)
				error1 (L"Cannot put an undefined value into the tier.\nFormula not finished.")
			((RealPoint) formant -> points -> item [icol]) -> value = result. result.numericResult;
		}
	}
end:
	iferror return 0;
	return 1;
}

FormantGrid Formant_downto_FormantGrid (Formant me) {
	FormantGrid thee = FormantGrid_createEmpty (my xmin, my xmax, my maxnFormants); cherror
	for (long iframe = 1; iframe <= my nx; iframe ++) {
		Formant_Frame frame = & my frame [iframe];
		double t = Sampled_indexToX (me, iframe);
		for (long iformant = 1; iformant <= frame -> nFormants; iformant ++) {
			Formant_Formant pair = & frame -> formant [iformant];
			FormantGrid_addFormantPoint (thee, iformant, t, pair -> frequency); cherror
			FormantGrid_addBandwidthPoint (thee, iformant, t, pair -> bandwidth); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

Formant FormantGrid_to_Formant (FormantGrid me, double dt, double intensity) {
	Melder_assert (dt > 0.0);
	Melder_assert (intensity >= 0.0);
	long nt = (long) floor ((my xmax - my xmin) / dt) + 1;
	double t1 = 0.5 * (my xmin + my xmax - (nt - 1) * dt);
	Formant thee = Formant_create (my xmin, my xmax, nt, dt, t1, my formants -> size);
	for (long iframe = 1; iframe <= nt; iframe ++) {
		Formant_Frame frame = & thy frame [iframe];
		frame -> intensity = intensity;
		frame -> nFormants = my formants -> size;
		frame -> formant = NUMstructvector (Formant_Formant, 1, my formants -> size);
		double t = t1 + (iframe - 1) * dt;
		for (long iformant = 1; iformant <= my formants -> size; iformant ++) {
			Formant_Formant formant = & frame -> formant [iformant];
			formant -> frequency = RealTier_getValueAtTime (my formants -> item [iformant], t);
			formant -> bandwidth = RealTier_getValueAtTime (my bandwidths -> item [iformant], t);
		}
	}
end:
	iferror forget (thee);
	return thee;
}

Sound Sound_Formant_filter (Sound me, Formant formant) {
	Sound thee = NULL;
	FormantGrid grid = NULL;
//start:
	grid = Formant_downto_FormantGrid (formant); cherror
	thee = Sound_FormantGrid_filter (me, grid); cherror
end:
	forget (grid);
	return thee;
}

Sound Sound_Formant_filter_noscale (Sound me, Formant formant) {
	Sound thee = NULL;
	FormantGrid grid = NULL;
//start:
	grid = Formant_downto_FormantGrid (formant); cherror
	thee = Sound_FormantGrid_filter_noscale (me, grid); cherror
end:
	forget (grid);
	iferror forget (thee);
	return thee;
}

#if 0
Sound Sound_Formant_filter_noscale (Sound me, Formant formant) {
	Sound thee = NULL;
	try {
		automatic (FormantGrid) grid = Formant_downto_FormantGrid (formant);
		thee = Sound_FormantGrid_filter_noscale (me, grid);
	} catch (...) {
		forget (thee);
		throw;
	}
	return thee;
}
#endif

/* End of file FormantGrid.c */
