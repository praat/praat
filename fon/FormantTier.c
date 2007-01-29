/* FormantTier.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2006/07/21 made Sound_FormantTier_filter_inline () accurate for higher numbers of formants
 * pb 2007/01/27 made compatible with stereo sounds
 */

#include "FormantTier.h"

#include "oo_DESTROY.h"
#include "FormantTier_def.h"
#include "oo_COPY.h"
#include "FormantTier_def.h"
#include "oo_EQUAL.h"
#include "FormantTier_def.h"
#include "oo_WRITE_ASCII.h"
#include "FormantTier_def.h"
#include "oo_READ_ASCII.h"
#include "FormantTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantTier_def.h"
#include "oo_READ_BINARY.h"
#include "FormantTier_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantTier_def.h"

class_methods (FormantPoint, Data)
	class_method_local (FormantPoint, destroy)
	class_method_local (FormantPoint, copy)
	class_method_local (FormantPoint, equal)
	class_method_local (FormantPoint, writeAscii)
	class_method_local (FormantPoint, readAscii)
	class_method_local (FormantPoint, writeBinary)
	class_method_local (FormantPoint, readBinary)
	class_method_local (FormantPoint, description)
class_methods_end

FormantPoint FormantPoint_create (double time) {
	FormantPoint me = new (FormantPoint);
	if (! me) return NULL;
	my time = time;
	return me;
}

class_methods (FormantTier, Function)
	class_method_local (FormantTier, destroy)
	class_method_local (FormantTier, copy)
	class_method_local (FormantTier, equal)
	class_method_local (FormantTier, writeAscii)
	class_method_local (FormantTier, readAscii)
	class_method_local (FormantTier, writeBinary)
	class_method_local (FormantTier, readBinary)
	class_method_local (FormantTier, description)
class_methods_end

FormantTier FormantTier_create (double tmin, double tmax) {
	FormantTier me = new (FormantTier);
	if (! me || ! (my points = SortedSetOfDouble_create ()))
		{ forget (me); return NULL; }
	my xmin = tmin;
	my xmax = tmax;
	return me;
}

double FormantTier_getValueAtTime (FormantTier me, int iformant, double t) {
	long n = my points -> size, ileft, iright;
	double tleft, tright, fleft, fright;
	FormantPoint pointLeft, pointRight;
	if (n == 0 || iformant < 1) return NUMundefined;
	pointRight = my points -> item [1];
	if (t <= pointRight -> time) {
		if (iformant > pointRight -> numberOfFormants) return NUMundefined;
		return pointRight -> formant [iformant-1];   /* Constant extrapolation. */
	}
	pointLeft = my points -> item [n];
	if (t >= pointLeft -> time) {
		if (iformant > pointLeft -> numberOfFormants) return NUMundefined;
		return pointLeft -> formant [iformant-1];   /* Constant extrapolation. */
	}
	Melder_assert (n >= 2);
	ileft = AnyTier_timeToLowIndex (me, t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	pointLeft = my points -> item [ileft];
	pointRight = my points -> item [iright];
	tleft = pointLeft -> time;
	fleft = iformant > pointLeft -> numberOfFormants ? NUMundefined : pointLeft -> formant [iformant-1];
	tright = pointRight -> time;
	fright = iformant > pointRight -> numberOfFormants ? NUMundefined : pointRight -> formant [iformant-1];
	return fleft == NUMundefined ? fright == NUMundefined ? NUMundefined : fright
		: fright == NUMundefined ? fleft
		: t == tright ? fright   /* Be very accurate. */
		: tleft == tright ? 0.5 * (fleft + fright)   /* Unusual, but possible; no preference. */
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   /* Linear interpolation. */
}

double FormantTier_getBandwidthAtTime (FormantTier me, int iformant, double t) {
	long n = my points -> size, ileft, iright;
	double tleft, tright, fleft, fright;
	FormantPoint pointLeft, pointRight;
	if (n == 0) return 0.0;
	pointRight = my points -> item [1];
	if (t <= pointRight -> time) {
		if (iformant > pointRight -> numberOfFormants) return NUMundefined;
		return pointRight -> bandwidth [iformant-1];   /* Constant extrapolation. */
	}
	pointLeft = my points -> item [n];
	if (t >= pointLeft -> time) {
		if (iformant > pointLeft -> numberOfFormants) return NUMundefined;
		return pointLeft -> bandwidth [iformant-1];   /* Constant extrapolation. */
	}
	Melder_assert (n >= 2);
	ileft = AnyTier_timeToLowIndex (me, t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	pointLeft = my points -> item [ileft];
	pointRight = my points -> item [iright];
	tleft = pointLeft -> time;
	fleft = iformant > pointLeft -> numberOfFormants ? NUMundefined : pointLeft -> bandwidth [iformant-1];
	tright = pointRight -> time;
	fright = iformant > pointRight -> numberOfFormants ? NUMundefined : pointRight -> bandwidth [iformant-1];
	return fleft == NUMundefined ? fright == NUMundefined ? NUMundefined : fright
		: fright == NUMundefined ? fleft
		: t == tright ? fright   /* Be very accurate. */
		: tleft == tright ? 0.5 * (fleft + fright)   /* Unusual, but possible; no preference. */
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   /* Linear interpolation. */
}

void FormantTier_speckle (FormantTier me, Graphics g, double tmin, double tmax, double fmax, int garnish) {
	long imin, imax, i, j;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	Graphics_setWindow (g, tmin, tmax, 0.0, fmax);
	Graphics_setInner (g);
	imin = AnyTier_timeToHighIndex (me, tmin);
	imax = AnyTier_timeToLowIndex (me, tmax);
	if (imin > 0) for (i = imin; i <= imax; i ++) {
		FormantPoint point = my points -> item [i];
		double t = point -> time;
		for (j = 1; j <= point -> numberOfFormants; j ++) {
			double f = point -> formant [j-1];
			if (f <= fmax) Graphics_fillCircle_mm (g, t, f, 1.0);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, TRUE, "Time (s)");
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
		Graphics_textLeft (g, TRUE, "Frequency (Hz)");
	}
}

FormantTier Formant_downto_FormantTier (Formant me) {
	FormantTier thee = FormantTier_create (my xmin, my xmax);
	long i, j;
	if (! thee) return NULL;
	for (i = 1; i <= my nx; i ++) {
		Formant_Frame frame = & my frame [i];
		FormantPoint point = FormantPoint_create (Sampled_indexToX (me, i));
		if (! point) { forget (thee); return NULL; }
		point -> numberOfFormants = frame -> nFormants > 10 ? 10 : frame -> nFormants;
		for (j = 1; j <= point -> numberOfFormants; j ++) {
			Formant_Formant pair = & frame -> formant [j];
			point -> formant [j-1] = pair -> frequency;
			point -> bandwidth [j-1] = pair -> bandwidth;
		}
		if (! Collection_addItem (thy points, point)) { forget (thee); return NULL; }
	}
	return thee;
}

FormantTier Formant_PointProcess_to_FormantTier (Formant me, PointProcess pp) {
	long ipoint, iformant;
	FormantTier temp = Formant_downto_FormantTier (me), thee;

	temp = Formant_downto_FormantTier (me); cherror
	thee = FormantTier_create (pp -> xmin, pp -> xmax); cherror
	for (ipoint = 1; ipoint <= pp -> nt; ipoint ++) {
		double time = pp -> t [ipoint];
		FormantPoint point = FormantPoint_create (time); cherror
		for (iformant = 1; iformant <= 10; iformant ++) {
			double value = FormantTier_getValueAtTime (temp, iformant, time);
			if (value == NUMundefined) break;
			point -> formant [iformant-1] = value;
			value = FormantTier_getBandwidthAtTime (temp, iformant, time);
			Melder_assert (value != NUMundefined);
			point -> bandwidth [iformant-1] = value;
		}
		point -> numberOfFormants = iformant - 1;
		Collection_addItem (thy points, point); cherror
	}
end:
	forget (temp);
	iferror { forget (thee); Melder_error ("(Formant_PointProcess_to_FormantTier:) Not performed."); }
	return thee;
}

int FormantTier_getMinNumFormants (FormantTier me) {
	int minNumFormants = 10, ipoint;
	for (ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		FormantPoint point = my points -> item [ipoint];
		if (point -> numberOfFormants < minNumFormants)
			minNumFormants = point -> numberOfFormants;
	}
	return minNumFormants;
}

int FormantTier_getMaxNumFormants (FormantTier me) {
	int maxNumFormants = 0, ipoint;
	for (ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		FormantPoint point = my points -> item [ipoint];
		if (point -> numberOfFormants > maxNumFormants)
			maxNumFormants = point -> numberOfFormants;
	}
	return maxNumFormants;
}
	
TableOfReal FormantTier_downto_TableOfReal (FormantTier me, int includeFormants, int includeBandwidths) {
	TableOfReal thee = NULL;
	int maximumNumberOfFormants = FormantTier_getMaxNumFormants (me), iformant, icol;
	long ipoint;

	thee = TableOfReal_create (my points -> size, 1 +
		( includeFormants ? maximumNumberOfFormants : 0 ) +
		( includeBandwidths ? maximumNumberOfFormants : 0 )); cherror
	TableOfReal_setColumnLabel (thee, 1, "Time");
	for (icol = 1, iformant = 1; iformant <= maximumNumberOfFormants; iformant ++) {
		char label [4];
		if (includeFormants) {
			sprintf (label, "F%d", iformant);
			TableOfReal_setColumnLabel (thee, ++ icol, label); cherror
		}
		if (includeBandwidths) {
			sprintf (label, "B%d", iformant);
			TableOfReal_setColumnLabel (thee, ++ icol, label); cherror
		}
	}
	for (ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		FormantPoint point = my points -> item [ipoint];
		thy data [ipoint] [1] = point -> time;
		for (icol = 1, iformant = 1; iformant <= maximumNumberOfFormants; iformant ++) {
			if (includeFormants) thy data [ipoint] [++ icol] = point -> formant [iformant-1];
			if (includeBandwidths) thy data [ipoint] [++ icol] = point -> bandwidth [iformant-1];
		}
	}
end:
	iferror forget (thee);
	return thee;
}

void Sound_FormantTier_filter_inline (Sound me, FormantTier formantTier) {
	long isamp, iformant;
	double dt = my dx;
	if (formantTier -> points -> size) for (iformant = 1; iformant <= 10; iformant ++) {
		for (isamp = 1; isamp <= my nx; isamp ++) {
			double t = my x1 + (isamp - 1) * my dx;
			/*
			 * Compute LP coefficients.
			 */
			double formant, bandwidth, cosomdt, r;
			formant = FormantTier_getValueAtTime (formantTier, iformant, t);
			if (NUMdefined (formant)) {
				bandwidth = FormantTier_getBandwidthAtTime (formantTier, iformant, t);
				Melder_assert (bandwidth != NUMundefined);
				cosomdt = cos (2 * NUMpi * formant * dt);
				r = exp (- NUMpi * bandwidth * dt);
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

Sound Sound_FormantTier_filter (Sound me, FormantTier formantTier) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_FormantTier_filter_inline (thee, formantTier);
	Vector_scale (thee, 0.99);
	return thee;
}

Sound Sound_FormantTier_filter_noscale (Sound me, FormantTier formantTier) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_FormantTier_filter_inline (thee, formantTier);
	return thee;
}

Sound Sound_Formant_filter (Sound me, Formant formant) {
	Sound thee = NULL;
	FormantTier tier = Formant_downto_FormantTier (formant); cherror
	thee = Sound_FormantTier_filter (me, tier); cherror
end:
	forget (tier);
	return thee;
}

Sound Sound_Formant_filter_noscale (Sound me, Formant formant) {
	Sound thee = NULL;
	FormantTier tier = Formant_downto_FormantTier (formant); cherror
	thee = Sound_FormantTier_filter_noscale (me, tier); cherror
end:
	forget (tier);
	return thee;
}

/* End of file FormantTier.c */
