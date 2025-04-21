/* FormantGrid.cpp
 *
 * Copyright (C) 2008,2009,2011,2012,2014-2020,2023,2025 Paul Boersma & David Weenink
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

double structFormantGrid :: v_getVector (
	const integer irow,
	const integer icol
) const {
	const constRealTier tier = our formants.at [irow];
	return RealTier_getValueAtIndex (tier, icol);
}

double structFormantGrid :: v_getFunction1 (
	const integer irow,
	const double x
) const {
	const constRealTier tier = our formants.at [irow];
	return RealTier_getValueAtTime (tier, x);
}

void structFormantGrid :: v_shiftX (
	const double xfrom,
	const double xto
) /* mutable */ {
	FormantGrid_Parent :: v_shiftX (xfrom, xto);
	for (integer i = 1; i <= our formants.size; i ++) {
		const mutableRealTier tier = our formants.at [i];
		tier -> v_shiftX (xfrom, xto);
	}
	for (integer i = 1; i <= our bandwidths.size; i ++) {
		const mutableRealTier tier = our bandwidths.at [i];
		tier -> v_shiftX (xfrom, xto);
	}
}

void structFormantGrid :: v_scaleX (
	const double xminfrom,
	const double xmaxfrom,
	const double xminto,
	const double xmaxto
) /* mutable */ {
	FormantGrid_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (integer i = 1; i <= our formants.size; i ++) {
		const mutableRealTier tier = our formants.at [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
	for (integer i = 1; i <= our bandwidths.size; i ++) {
		const mutableRealTier tier = our bandwidths.at [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

void FormantGrid_init (
	const mutableFormantGrid me,
	const double tmin,
	const double tmax,
	const integer numberOfFormants
) {
	for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
		autoRealTier formant = RealTier_create (tmin, tmax);
		my formants. addItem_move (formant.move());
		autoRealTier bandwidth = RealTier_create (tmin, tmax);
		my bandwidths. addItem_move (bandwidth.move());
	}
	my xmin = tmin;
	my xmax = tmax;
}

autoFormantGrid FormantGrid_createEmpty (
	const double tmin,
	const double tmax,
	const integer numberOfFormants
) {
	try {
		autoFormantGrid me = Thing_new (FormantGrid);
		FormantGrid_init (me.get(), tmin, tmax, numberOfFormants);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Empty FormantGrid not created.");
	}
}

autoFormantGrid FormantGrid_create (
	const double tmin,
	const double tmax,
	const integer numberOfFormants,
	const double initialFirstFormant,
	const double initialFormantSpacing,
	const double initialFirstBandwidth,
	const double initialBandwidthSpacing
) {
	try {
		autoFormantGrid me = FormantGrid_createEmpty (tmin, tmax, numberOfFormants);
		for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
			FormantGrid_addFormantPoint (me.get(), iformant, 0.5 * (tmin + tmax),
					initialFirstFormant + (iformant - 1) * initialFormantSpacing);
			FormantGrid_addBandwidthPoint (me.get(), iformant, 0.5 * (tmin + tmax),
					initialFirstBandwidth + (iformant - 1) * initialBandwidthSpacing);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantGrid not created.");
	}
}

void FormantGrid_addFormantPoint (
	const mutableFormantGrid me,
	const integer iformant,
	const double time,
	const double value
) {
	try {
		if (iformant < 1 || iformant > my formants.size)
			Melder_throw (U"No such formant number.");
		RealTier formantTier = my formants.at [iformant];
		RealTier_addPoint (formantTier, time, value);
	} catch (MelderError) {
		Melder_throw (me, U": formant point not added.");
	}
}

void FormantGrid_addBandwidthPoint (
	const mutableFormantGrid me,
	const integer iformant,
	const double time,
	const double value
) {
	try {
		if (iformant < 1 || iformant > my formants.size)
			Melder_throw (U"No such formant number.");
		RealTier bandwidthTier = my bandwidths.at [iformant];
		RealTier_addPoint (bandwidthTier, time, value);
	} catch (MelderError) {
		Melder_throw (me, U": bandwidth point not added.");
	}
}

double FormantGrid_getFormantAtTime (
	const constFormantGrid me,
	const integer iformant,
	const double time
) {
	if (iformant < 1 || iformant > my formants.size)
		return undefined;
	return RealTier_getValueAtTime (my formants.at [iformant], time);
}

double FormantGrid_getBandwidthAtTime (
	const constFormantGrid me,
	const integer iformant,
	const double time
) {
	if (iformant < 1 || iformant > my bandwidths.size)
		return undefined;
	return RealTier_getValueAtTime (my bandwidths.at [iformant], time);
}

void FormantGrid_removeFormantPointsBetween (
	const mutableFormantGrid me,
	const integer iformant,
	const double tmin,
	const double tmax
) {
	if (iformant < 1 || iformant > my formants.size)
		return;
	AnyTier_removePointsBetween (my formants.at [iformant]->asAnyTier(), tmin, tmax);
}

void FormantGrid_removeBandwidthPointsBetween (
	const mutableFormantGrid me,
	const integer iformant,
	const double tmin,
	const double tmax
) {
	if (iformant < 1 || iformant > my bandwidths.size)
		return;
	AnyTier_removePointsBetween (my bandwidths.at [iformant]->asAnyTier(), tmin, tmax);
}

void Sound_FormantGrid_filter_inplace (
	const mutableSound me,
	const constFormantGrid formantGrid
) {
	const double dt = my dx;
	if (formantGrid -> formants.size > 0 && formantGrid -> bandwidths.size > 0) {
		for (integer iformant = 1; iformant <= formantGrid -> formants.size; iformant ++) {
			const constRealTier formantTier = formantGrid -> formants.at [iformant];
			const constRealTier bandwidthTier = formantGrid -> bandwidths.at [iformant];
			for (integer isamp = 1; isamp <= my nx; isamp ++) {
				const double t = my x1 + (isamp - 1) * my dx;
				/*
					Compute LP coefficients.
				*/
				const double formant = RealTier_getValueAtTime (formantTier, t);
				const double bandwidth = RealTier_getValueAtTime (bandwidthTier, t);
				if (isdefined (formant) && isdefined (bandwidth)) {
					const double cosomdt = cos (2 * NUMpi * formant * dt);
					const double r = exp (- NUMpi * bandwidth * dt);
					/* Formants at 0 Hz or the Nyquist are single poles, others are double poles. */
					if (fabs (cosomdt) > 0.999999) {   /* Allow for round-off errors. */
						/* single pole: D(z) = 1 - r z^-1 */
						for (integer channel = 1; channel <= my ny; channel ++)
							if (isamp > 1)
								my z [channel] [isamp] += r * my z [channel] [isamp - 1];
					} else {
						/* double pole: D(z) = 1 + p z^-1 + q z^-2 */
						const double p = - 2 * r * cosomdt;
						const double q = r * r;
						for (integer channel = 1; channel <= my ny; channel ++) {
							if (isamp > 1)
								my z [channel] [isamp] -= p * my z [channel] [isamp - 1];
							if (isamp > 2)
								my z [channel] [isamp] -= q * my z [channel] [isamp - 2];
						}
					}
				}
			}
		}
	}
}

autoSound Sound_FormantGrid_filter (
	const constSound me,
	const constFormantGrid formantGrid
) {
	try {
		autoSound thee = Data_copy (me);
		Sound_FormantGrid_filter_inplace (thee.get(), formantGrid);
		Vector_scale (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with ", formantGrid, U".");
	}
}

autoSound Sound_FormantGrid_filter_noscale (
	const constSound me,
	const constFormantGrid formantGrid
) {
	try {
		autoSound thee = Data_copy (me);
		Sound_FormantGrid_filter_inplace (thee.get(), formantGrid);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with ", formantGrid, U".");
	}
}

autoSound FormantGrid_to_Sound (
	const constFormantGrid me,
	const double samplingFrequency,
	const double tStart, const double f0Start,
	const double tMid, const double f0Mid,
	const double tEnd, const double f0End,
	const double adaptFactor, const double maximumPeriod,
	const double openPhase, const double collisionPhase, const double power1, const double power2
) {
	try {
		autoPitchTier pitch = PitchTier_create (my xmin, my xmax);
		RealTier_addPoint (pitch.get(), my xmin + tStart * (my xmax - my xmin), f0Start);
		RealTier_addPoint (pitch.get(), my xmin + tMid * (my xmax - my xmin), f0Mid);
		RealTier_addPoint (pitch.get(), my xmax - (1.0 - tEnd) * (my xmax - my xmin), f0End);
		autoSound thee = PitchTier_to_Sound_phonation (pitch.get(), samplingFrequency,
				adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2, false);
		Sound_FormantGrid_filter_inplace (thee.get(), me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

void FormantGrid_playPart (
	const constFormantGrid me,
	const double tmin, const double tmax,
	const double samplingFrequency,
	const double tStart, const double f0Start,
	const double tMid, const double f0Mid,
	const double tEnd, const double f0End,
	const double adaptFactor, const double maximumPeriod,
	const double openPhase, const double collisionPhase, const double power1, const double power2,
	const Sound_PlayCallback playCallback, const mutableThing playBoss
) {
	try {
		autoSound sound = FormantGrid_to_Sound (me, samplingFrequency,
			tStart, f0Start, tMid, f0Mid, tEnd, f0End,
			adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2
		);
		Vector_scale (sound.get(), 0.99);
		Sound_playPart (sound.get(), tmin, tmax, playCallback, playBoss);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void FormantGrid_formula_frequencies (
	const mutableFormantGrid me,
	const conststring32 expression,
	const Interpreter interpreter,
	/* mutable assign if null */ mutableFormantGrid thee
) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		if (! thee)
			thee = me;
		for (integer irow = 1; irow <= my formants.size; irow ++) {
			const RealTier formant = thy formants.at [irow];
			for (integer icol = 1; icol <= formant -> points.size; icol ++) {
				Formula_run (irow, icol, & result);
				if (isundef (result. numericResult))
					Melder_throw (U"Cannot put an undefined value into the tier.\nFormula not finished.");
				formant -> points.at [icol] -> value = result. numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": frequency formula not completed.");
	}
}

void FormantGrid_formula_bandwidths (
	const mutableFormantGrid me,
	const conststring32 expression,
	const Interpreter interpreter,
	/* mutable assign if null */ mutableFormantGrid thee
) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		if (! thee)
			thee = me;
		for (integer irow = 1; irow <= my formants.size; irow ++) {
			const RealTier bandwidth = thy bandwidths.at [irow];
			for (integer icol = 1; icol <= bandwidth -> points.size; icol ++) {
				Formula_run (irow, icol, & result);
				if (isundef (result. numericResult))
					Melder_throw (U"Cannot put an undefined value into the tier.\nFormula not finished.");
				bandwidth -> points.at [icol] -> value = result. numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": bandwidth formula not completed.");
	}
}

autoFormantGrid Formant_downto_FormantGrid (const constFormant me) {
	try {
		autoFormantGrid thee = FormantGrid_createEmpty (my xmin, my xmax, my maxnFormants);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			const double t = Sampled_indexToX (me, iframe);
			for (integer iformant = 1; iformant <= frame -> numberOfFormants; iformant ++) {
				Formant_Formant pair = & frame -> formant [iformant];
				FormantGrid_addFormantPoint (thee.get(), iformant, t, pair -> frequency);
				FormantGrid_addBandwidthPoint (thee.get(), iformant, t, pair -> bandwidth);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to FormantGrid.");
	}
}

autoFormant FormantGrid_to_Formant (const constFormantGrid me, const double dt, const double intensity) {
	try {
		Melder_assert (dt > 0.0);
		Melder_assert (intensity >= 0.0);
		const integer nt = Melder_ifloor ((my xmax - my xmin) / dt) + 1;
		const double t1 = 0.5 * (my xmin + my xmax - (nt - 1) * dt);
		autoFormant thee = Formant_create (my xmin, my xmax, nt, dt, t1, my formants.size);
		for (integer iframe = 1; iframe <= nt; iframe ++) {
			const Formant_Frame frame = & thy frames [iframe];
			frame -> intensity = intensity;
			frame -> numberOfFormants = my formants.size;
			frame -> formant = newvectorzero <structFormant_Formant> (my formants.size);
			const double time = t1 + (iframe - 1) * dt;
			for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
				const Formant_Formant formant = & frame -> formant [iformant];
				formant -> frequency = RealTier_getValueAtTime (my formants.at [iformant], time);
				formant -> bandwidth = RealTier_getValueAtTime (my bandwidths.at [iformant], time);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Formant.");
	}
}

autoSound Sound_Formant_filter (const constSound me, const constFormant formant) {
	try {
		autoFormantGrid grid = Formant_downto_FormantGrid (formant);
		autoSound thee = Sound_FormantGrid_filter (me, grid.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with ", formant, U".");
	}
}

autoSound Sound_Formant_filter_noscale (const constSound me, const constFormant formant) {
	try {
		autoFormantGrid grid = Formant_downto_FormantGrid (formant);
		autoSound thee = Sound_FormantGrid_filter_noscale (me, grid.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with ", formant, U".");
	}
}

/* End of file FormantGrid.cpp */
