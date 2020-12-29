/* FormantTier.cpp
 *
 * Copyright (C) 1992-2007,2011,2012,2014-2020 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2006/07/21 made Sound_FormantTier_filter_inplace () accurate for higher numbers of formants
 * pb 2007/01/27 made compatible with stereo sounds
 * pb 2007/03/17 domain quantity
 * pb 2007/10/01 can write as encoding
 * pb 2011/03/01 moved Formant filtering to FormantGrid (reimplemented)
 * pb 2011/05/26 C++
 */

#include "FormantTier.h"
#include "AnyTier.h"

#include "oo_DESTROY.h"
#include "FormantTier_def.h"
#include "oo_COPY.h"
#include "FormantTier_def.h"
#include "oo_EQUAL.h"
#include "FormantTier_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantTier_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantTier_def.h"
#include "oo_READ_TEXT.h"
#include "FormantTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantTier_def.h"
#include "oo_READ_BINARY.h"
#include "FormantTier_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantTier_def.h"

Thing_implement (FormantPoint, Daata, 0);

autoFormantPoint FormantPoint_create (double time, integer numberOfFormants) {
	try {
		autoFormantPoint me = Thing_new (FormantPoint);
		my number = time;
		my numberOfFormants = numberOfFormants;
		my formant = zero_VEC (numberOfFormants);
		my bandwidth = zero_VEC (numberOfFormants);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Formant point not created.");
	}
}

Thing_implement (FormantTier, AnyTier, 0);

autoFormantTier FormantTier_create (double tmin, double tmax) {
	try {
		autoFormantTier me = Thing_new (FormantTier);
		my xmin = tmin;
		my xmax = tmax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantTier not created.");
	}
}

double FormantTier_getValueAtTime (FormantTier me, integer iformant, double t) {
	integer n = my points.size;
	if (n == 0 || iformant < 1)
		return undefined;
	FormantPoint pointRight = my points.at [1];
	if (t <= pointRight -> number) {
		if (iformant > pointRight -> numberOfFormants)
			return undefined;
		return pointRight -> formant [iformant];   // constant extrapolation
	}
	FormantPoint pointLeft = my points.at [n];
	if (t >= pointLeft -> number) {
		if (iformant > pointLeft -> numberOfFormants)
			return undefined;
		return pointLeft -> formant [iformant];   // constant extrapolation
	}
	Melder_assert (n >= 2);
	integer ileft = AnyTier_timeToLowIndex (me->asAnyTier(), t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	pointLeft = my points.at [ileft];
	pointRight = my points.at [iright];
	const double tleft = pointLeft -> number;
	const double fleft = ( iformant > pointLeft -> numberOfFormants ? undefined : pointLeft -> formant [iformant] );
	const double tright = pointRight -> number;
	const double fright = ( iformant > pointRight -> numberOfFormants ? undefined : pointRight -> formant [iformant] );
	return isundef (fleft) ? ( isundef (fright) ? undefined : fright )
		: isundef (fright) ? fleft
		: t == tright ? fright   // be very accurate
		: tleft == tright ? 0.5 * (fleft + fright)   // unusual, but possible; no preference
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   // linear interpolation
}

double FormantTier_getBandwidthAtTime (FormantTier me, integer iformant, double t) {
	const integer n = my points.size;
	if (n == 0 || iformant < 1)
		return undefined;
	FormantPoint pointRight = my points.at [1];
	if (t <= pointRight -> number) {
		if (iformant > pointRight -> numberOfFormants)
			return undefined;
		return pointRight -> bandwidth [iformant];   // constant extrapolation
	}
	FormantPoint pointLeft = my points.at [n];
	if (t >= pointLeft -> number) {
		if (iformant > pointLeft -> numberOfFormants)
			return undefined;
		return pointLeft -> bandwidth [iformant];   // constant extrapolation
	}
	Melder_assert (n >= 2);
	const integer ileft = AnyTier_timeToLowIndex (me->asAnyTier(), t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	pointLeft = my points.at [ileft];
	pointRight = my points.at [iright];
	const double tleft = pointLeft -> number;
	const double fleft = iformant > pointLeft -> numberOfFormants ? undefined : pointLeft -> bandwidth [iformant];
	const double tright = pointRight -> number;
	const double fright = iformant > pointRight -> numberOfFormants ? undefined : pointRight -> bandwidth [iformant];
	return isundef (fleft) ? ( isundef (fright) ? undefined : fright )
		: isundef (fright) ? fleft
		: t == tright ? fright   // be very accurate
		: tleft == tright ? 0.5 * (fleft + fright)   // unusual, but possible; no preference
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   // linear interpolation
}

void FormantTier_speckle (FormantTier me, Graphics g, double tmin, double tmax, double fmax, bool garnish) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	Graphics_setWindow (g, tmin, tmax, 0.0, fmax);
	Graphics_setInner (g);
	const integer imin = AnyTier_timeToHighIndex (me->asAnyTier(), tmin);
	const integer imax = AnyTier_timeToLowIndex (me->asAnyTier(), tmax);
	if (imin > 0) for (integer i = imin; i <= imax; i ++) {
		FormantPoint point = my points.at [i];
		const double t = point -> number;
		for (integer j = 1; j <= point -> numberOfFormants; j ++) {
			const double f = point -> formant [j];
			if (f <= fmax)
				Graphics_speckle (g, t, f);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Frequency (Hz)");
	}
}

autoFormantTier Formant_downto_FormantTier (Formant me) {
	try {
		autoFormantTier thee = FormantTier_create (my xmin, my xmax);
		for (integer i = 1; i <= my nx; i ++) {
			const Formant_Frame frame = & my frames [i];
			/* mutable move */ autoFormantPoint point = FormantPoint_create (Sampled_indexToX (me, i), frame -> numberOfFormants);
			for (integer j = 1; j <= frame -> numberOfFormants; j ++) {
				Formant_Formant pair = & frame -> formant [j];
				point -> formant [j] = pair -> frequency;
				point -> bandwidth [j] = pair -> bandwidth;
			}
			thy points. addItem_move (point.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to FormantTier.");
	}
}

autoFormantTier Formant_PointProcess_to_FormantTier (Formant me, PointProcess pp) {
	try {
		const integer maximumNumberOfFormants = Formant_getMaxNumFormants (me);
		const autoFormantTier temp = Formant_downto_FormantTier (me);
		/* mutable return */ autoFormantTier thee = FormantTier_create (pp -> xmin, pp -> xmax);
		for (integer ipoint = 1; ipoint <= pp -> nt; ipoint ++) {
			const double time = pp -> t [ipoint];
			/* mutable move */ autoFormantPoint point = FormantPoint_create (time, maximumNumberOfFormants);
			/* mutable loop */ integer iformant = 1;
			for (; iformant <= maximumNumberOfFormants; iformant ++) {
				const double frequency = FormantTier_getValueAtTime (temp.get(), iformant, time);
				if (isundef (frequency))
					break;
				point -> formant [iformant] = frequency;
				const double bandwidth = FormantTier_getBandwidthAtTime (temp.get(), iformant, time);
				Melder_assert (isdefined (bandwidth));
				point -> bandwidth [iformant] = bandwidth;
			}
			point -> numberOfFormants = iformant - 1;
			point -> formant. resize (point -> numberOfFormants);   // maintain invariant
			point -> bandwidth. resize (point -> numberOfFormants);   // maintain invariant
			thy points. addItem_move (point.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", pp, U": not converted to FormantTier.");
	}
}

integer FormantTier_getMinNumFormants (FormantTier me) {
	integer minNumFormants = INTEGER_MAX;
	for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
		const FormantPoint point = my points.at [ipoint];
		if (point -> numberOfFormants < minNumFormants)
			minNumFormants = point -> numberOfFormants;
	}
	return minNumFormants;
}

integer FormantTier_getMaxNumFormants (FormantTier me) {
	integer maxNumFormants = 0;
	for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
		const FormantPoint point = my points.at [ipoint];
		if (point -> numberOfFormants > maxNumFormants)
			maxNumFormants = point -> numberOfFormants;
	}
	return maxNumFormants;
}
	
autoTableOfReal FormantTier_downto_TableOfReal (FormantTier me, bool includeFormants, bool includeBandwidths) {
	try {
		const integer maximumNumberOfFormants = FormantTier_getMaxNumFormants (me);
		/* mutable return */ autoTableOfReal thee = TableOfReal_create (my points.size, 1 +
			( includeFormants ? maximumNumberOfFormants : 0 ) +
			( includeBandwidths ? maximumNumberOfFormants : 0 ));
		TableOfReal_setColumnLabel (thee.get(), 1, U"Time");
		for (integer icol = 1, iformant = 1; iformant <= maximumNumberOfFormants; iformant ++) {
			char32 label [4];
			if (includeFormants) {
				Melder_sprint (label,4, U"F", iformant);
				TableOfReal_setColumnLabel (thee.get(), ++ icol, label);
			}
			if (includeBandwidths) {
				Melder_sprint (label,4, U"B", iformant);
				TableOfReal_setColumnLabel (thee.get(), ++ icol, label);
			}
		}
		for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
			const FormantPoint point = my points.at [ipoint];
			thy data [ipoint] [1] = point -> number;
			for (integer icol = 1, iformant = 1; iformant <= maximumNumberOfFormants; iformant ++) {
				if (includeFormants)
					thy data [ipoint] [++ icol] = point -> formant [iformant];
				if (includeBandwidths)
					thy data [ipoint] [++ icol] = point -> bandwidth [iformant];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

void Sound_FormantTier_filter_inplace (Sound me, FormantTier formantTier) {
	const double dt = my dx;
	const integer maximumNumberOfFormants = FormantTier_getMaxNumFormants (formantTier);
	if (formantTier -> points.size) for (integer iformant = 1; iformant <= maximumNumberOfFormants; iformant ++) {
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			const double t = my x1 + (isamp - 1) * my dx;
			/*
				Compute LP coefficients.
			*/
			const double formant = FormantTier_getValueAtTime (formantTier, iformant, t);
			const double bandwidth = FormantTier_getBandwidthAtTime (formantTier, iformant, t);
			if (isdefined (formant) && isdefined (bandwidth)) {
				const double cosomdt = cos (2.0 * NUMpi * formant * dt);
				const double r = exp (- NUMpi * bandwidth * dt);
				/* Formants at 0 Hz or the Nyquist are single poles; others are double poles. */
				if (fabs (cosomdt) > 0.999999) {   // allow for round-off errors
					/* single pole: D(z) = 1 - r z^-1 */
					for (integer channel = 1; channel <= my ny; channel ++)
						if (isamp > 1)
							my z [channel] [isamp] += r * my z [channel] [isamp - 1];
				} else {
					/* double pole: D(z) = 1 + p z^-1 + q z^-2 */
					const double p = - 2.0 * r * cosomdt;
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

autoSound Sound_FormantTier_filter (Sound me, FormantTier formantTier) {
	try {
		autoSound thee = Data_copy (me);
		Sound_FormantTier_filter_inplace (thee.get(), formantTier);
		Vector_scale (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with ", formantTier, U".");
	}
}

autoSound Sound_FormantTier_filter_noscale (Sound me, FormantTier formantTier) {
	try {
		autoSound thee = Data_copy (me);
		Sound_FormantTier_filter_inplace (thee.get(), formantTier);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with ", formantTier, U".");
	}
}

/* End of file FormantTier.cpp */
