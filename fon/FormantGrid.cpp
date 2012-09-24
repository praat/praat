/* FormantGrid.cpp
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

Thing_implement (FormantGrid, Function, 0);

double structFormantGrid :: v_getVector (long irow, long icol) {
	RealTier tier = (RealTier) formants -> item [irow];
	return RealTier_getValueAtIndex (tier, icol);
}

double structFormantGrid :: v_getFunction1 (long irow, double x) {
	RealTier tier = (RealTier) formants -> item [irow];
	return RealTier_getValueAtTime (tier, x);
}

void structFormantGrid :: v_shiftX (double xfrom, double xto) {
	FormantGrid_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= formants -> size; i ++) {
		RealTier tier = (RealTier) formants -> item [i];
		tier -> v_shiftX (xfrom, xto);
	}
	for (long i = 1; i <= bandwidths -> size; i ++) {
		RealTier tier = (RealTier) bandwidths -> item [i];
		tier -> v_shiftX (xfrom, xto);
	}
}

void structFormantGrid :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	FormantGrid_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= formants -> size; i ++) {
		RealTier tier = (RealTier) formants -> item [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
	for (long i = 1; i <= bandwidths -> size; i ++) {
		RealTier tier = (RealTier) bandwidths -> item [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

void FormantGrid_init (I, double tmin, double tmax, long numberOfFormants) {
	iam (FormantGrid);
	my formants = Ordered_create ();
	my bandwidths = Ordered_create ();
	for (long iformant = 1; iformant <= numberOfFormants; iformant ++) {
		RealTier formant = RealTier_create (tmin, tmax);
		Collection_addItem (my formants, formant);
		RealTier bandwidth = RealTier_create (tmin, tmax);
		Collection_addItem (my bandwidths, bandwidth);
	}
	my xmin = tmin;
	my xmax = tmax;
}

FormantGrid FormantGrid_createEmpty (double tmin, double tmax, long numberOfFormants) {
	try {
		autoFormantGrid me = Thing_new (FormantGrid);
		FormantGrid_init (me.peek(), tmin, tmax, numberOfFormants);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Empty FormantGrid not created.");
	}
}

FormantGrid FormantGrid_create (double tmin, double tmax, long numberOfFormants,
	double initialFirstFormant, double initialFormantSpacing,
	double initialFirstBandwidth, double initialBandwidthSpacing)
{
	try {
		autoFormantGrid me = FormantGrid_createEmpty (tmin, tmax, numberOfFormants);
		for (long iformant = 1; iformant <= numberOfFormants; iformant ++) {
			FormantGrid_addFormantPoint (me.peek(), iformant, 0.5 * (tmin + tmax),
				initialFirstFormant + (iformant - 1) * initialFormantSpacing);
			FormantGrid_addBandwidthPoint (me.peek(), iformant, 0.5 * (tmin + tmax),
				initialFirstBandwidth + (iformant - 1) * initialBandwidthSpacing);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FormantGrid not created.");
	}
}

void FormantGrid_addFormantPoint (FormantGrid me, long iformant, double t, double value) {
	try {
		if (iformant < 1 || iformant > my formants -> size)
			Melder_throw ("No such formant number.");
		RealTier formantTier = (RealTier) my formants -> item [iformant];
		RealTier_addPoint (formantTier, t, value);
	} catch (MelderError) {
		Melder_throw (me, ": formant point not added.");
	}
}

void FormantGrid_addBandwidthPoint (FormantGrid me, long iformant, double t, double value) {
	try {
		if (iformant < 1 || iformant > my formants -> size)
			Melder_throw ("No such formant number.");
		RealTier bandwidthTier = (RealTier) my bandwidths -> item [iformant];
		RealTier_addPoint (bandwidthTier, t, value);
	} catch (MelderError) {
		Melder_throw (me, ": bandwidth point not added.");
	}
}

double FormantGrid_getFormantAtTime (FormantGrid me, long iformant, double t) {
	if (iformant < 1 || iformant > my formants -> size) return NUMundefined;
	return RealTier_getValueAtTime ((RealTier) my formants -> item [iformant], t);
}

double FormantGrid_getBandwidthAtTime (FormantGrid me, long iformant, double t) {
	if (iformant < 1 || iformant > my bandwidths -> size) return NUMundefined;
	return RealTier_getValueAtTime ((RealTier) my bandwidths -> item [iformant], t);
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
		RealTier formantTier = (RealTier) formantGrid -> formants -> item [iformant];
		RealTier bandwidthTier = (RealTier) formantGrid -> bandwidths -> item [iformant];
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
	try {
		autoSound thee = Data_copy (me);
		Sound_FormantGrid_filter_inline (thee.peek(), formantGrid);
		Vector_scale (thee.peek(), 0.99);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered with ", formantGrid, ".");
	}
}

Sound Sound_FormantGrid_filter_noscale (Sound me, FormantGrid formantGrid) {
	try {
		autoSound thee = Data_copy (me);
		Sound_FormantGrid_filter_inline (thee.peek(), formantGrid);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered with ", formantGrid, ".");
	}
}

Sound FormantGrid_to_Sound (FormantGrid me, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2)
{
	try {
		autoPitchTier pitch = PitchTier_create (my xmin, my xmax);
		RealTier_addPoint (pitch.peek(), my xmin + tStart * (my xmax - my xmin), f0Start);
		RealTier_addPoint (pitch.peek(), my xmin + tMid * (my xmax - my xmin), f0Mid);
		RealTier_addPoint (pitch.peek(), my xmax - (1.0 - tEnd) * (my xmax - my xmin), f0End);
		autoSound thee = PitchTier_to_Sound_phonation (pitch.peek(), samplingFrequency,
			adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2, false);
		Sound_FormantGrid_filter_inline (thee.peek(), me);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

void FormantGrid_playPart (FormantGrid me, double tmin, double tmax, double samplingFrequency,
	double tStart, double f0Start, double tMid, double f0Mid, double tEnd, double f0End,
	double adaptFactor, double maximumPeriod, double openPhase, double collisionPhase, double power1, double power2,
	int (*playCallback) (void *playClosure, int phase, double tmin, double tmax, double t), void *playClosure)
{
	try {
		autoSound sound = FormantGrid_to_Sound (me, samplingFrequency,
			tStart, f0Start, tMid, f0Mid, tEnd, f0End,
			adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2);
		Vector_scale (sound.peek(), 0.99);
		Sound_playPart (sound.peek(), tmin, tmax, playCallback, playClosure);
	} catch (MelderError) {
		Melder_throw (me, ": not played.");
	}
}

void FormantGrid_formula_bandwidths (FormantGrid me, const wchar_t *expression, Interpreter interpreter, FormantGrid thee) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		if (thee == NULL) thee = me;
		for (long irow = 1; irow <= my formants -> size; irow ++) {
			RealTier bandwidth = (RealTier) thy bandwidths -> item [irow];
			for (long icol = 1; icol <= bandwidth -> points -> size; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult == NUMundefined)
					Melder_throw ("Cannot put an undefined value into the tier.\nFormula not finished.");
				((RealPoint) bandwidth -> points -> item [icol]) -> value = result. result.numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": bandwidth formula not completed.");
	}
}

void FormantGrid_formula_frequencies (FormantGrid me, const wchar_t *expression, Interpreter interpreter, FormantGrid thee) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		if (thee == NULL) thee = me;
		for (long irow = 1; irow <= my formants -> size; irow ++) {
			RealTier formant = (RealTier) thy formants -> item [irow];
			for (long icol = 1; icol <= formant -> points -> size; icol ++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult == NUMundefined)
					Melder_throw ("Cannot put an undefined value into the tier.\nFormula not finished.");
				((RealPoint) formant -> points -> item [icol]) -> value = result. result.numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": frequency formula not completed.");
	}
}

FormantGrid Formant_downto_FormantGrid (Formant me) {
	try {
		autoFormantGrid thee = FormantGrid_createEmpty (my xmin, my xmax, my maxnFormants);
		for (long iframe = 1; iframe <= my nx; iframe ++) {
			Formant_Frame frame = & my d_frames [iframe];
			double t = Sampled_indexToX (me, iframe);
			for (long iformant = 1; iformant <= frame -> nFormants; iformant ++) {
				Formant_Formant pair = & frame -> formant [iformant];
				FormantGrid_addFormantPoint (thee.peek(), iformant, t, pair -> frequency);
				FormantGrid_addBandwidthPoint (thee.peek(), iformant, t, pair -> bandwidth);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to FormantGrid.");
	}
}

Formant FormantGrid_to_Formant (FormantGrid me, double dt, double intensity) {
	try {
		Melder_assert (dt > 0.0);
		Melder_assert (intensity >= 0.0);
		long nt = (long) floor ((my xmax - my xmin) / dt) + 1;
		double t1 = 0.5 * (my xmin + my xmax - (nt - 1) * dt);
		autoFormant thee = Formant_create (my xmin, my xmax, nt, dt, t1, my formants -> size);
		for (long iframe = 1; iframe <= nt; iframe ++) {
			Formant_Frame frame = & thy d_frames [iframe];
			frame -> intensity = intensity;
			frame -> nFormants = my formants -> size;
			frame -> formant = NUMvector <structFormant_Formant> (1, my formants -> size);
			double t = t1 + (iframe - 1) * dt;
			for (long iformant = 1; iformant <= my formants -> size; iformant ++) {
				Formant_Formant formant = & frame -> formant [iformant];
				formant -> frequency = RealTier_getValueAtTime ((RealTier) my formants -> item [iformant], t);
				formant -> bandwidth = RealTier_getValueAtTime ((RealTier) my bandwidths -> item [iformant], t);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Formant.");
	}
}

Sound Sound_Formant_filter (Sound me, Formant formant) {
	try {
		autoFormantGrid grid = Formant_downto_FormantGrid (formant);
		autoSound thee = Sound_FormantGrid_filter (me, grid.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered with ", formant, ".");
	}
}

Sound Sound_Formant_filter_noscale (Sound me, Formant formant) {
	try {
		autoFormantGrid grid = Formant_downto_FormantGrid (formant);
		autoSound thee = Sound_FormantGrid_filter_noscale (me, grid.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered with ", formant, ".");
	}
}

/* End of file FormantGrid.cpp */
