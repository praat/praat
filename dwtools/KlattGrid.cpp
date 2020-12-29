/* KlattGrid.cpp
 *
 * Copyright (C) 2008-2020 David Weenink, 2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */
/*
  djmw 20080917 Initial version
  djmw 20090109 Add formulas for formant frequencies and bandwidths.
  djmw 20090123 Add PlayOptions.
  djmw 20090129 KlattGrid_draw text in boxes displays better
  djmw 20090311 Add RealTier_valuesInRange
  djmw 20090312 Add klattGrid_addFormantAndBandwidthTier
  djmw 20090326 Changed DBSPL_to_A into DB_to_A for bypass and formant amplitudes.
  djmw 20100223 Removed gsl dependency
  djmw 20110304 Thing_new
  djmw 20110308 struc connections -> struct structconnections
  djmw 20110329 Table_get(Numeric|String)Value is now Table_get(Numeric|String)Value_Assert
  djmw 20111011 Sound_VocalTractGrid_CouplingGrid_filter_cascade: group warnings
*/

#include "FormantGrid_extensions.h"
#include "Formula.h"
#include "KlattGrid.h"
#include "KlattTable.h"
#include "Resonator.h"
#include "Pitch_to_PitchTier.h"
#include "PitchTier_to_Sound.h"
#include "PitchTier_to_PointProcess.h"
#include "NUM2.h"
#include "Sound_to_Formant.h"
#include "Sound_to_Intensity.h"
#include "Sound_to_Pitch.h"

#include "oo_DESTROY.h"
#include "KlattGrid_def.h"
#include "oo_COPY.h"
#include "KlattGrid_def.h"
#include "oo_EQUAL.h"
#include "KlattGrid_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "KlattGrid_def.h"
#include "oo_WRITE_TEXT.h"
#include "KlattGrid_def.h"
#include "oo_WRITE_BINARY.h"
#include "KlattGrid_def.h"
#include "oo_READ_TEXT.h"
#include "KlattGrid_def.h"
#include "oo_READ_BINARY.h"
#include "KlattGrid_def.h"
#include "oo_DESCRIPTION.h"
#include "KlattGrid_def.h"


#include "enums_getText.h"
#include "KlattGrid_enums.h"
#include "enums_getValue.h"
#include "KlattGrid_enums.h"

/*
 * A KlattGrid consists of a great many tiers that can be independently modified.
 * 
 * For any particular formant, the formant frequency tier and the formant bandwidth tier can only be added or removed jointly
 * because they are part of a FormantGrid object. There will always be an equal number of formant frequency tiers and
 * formant bandwidth tiers.
 * 
 * For parallel synthesis we also need, besides the frequency and bandwidth tier, an additional amplitude tier for each formant.
 * It is not necessary that there are an equal number of formant frequency tiers (nf) and amplitude tiers (na).
 * During parallel synthesis we simply synthesize with min(nf,na) number of formants.
 * These numbers nf and na can get out of sync because of the following (add, remove, replace) actions:
 * 	- we replace a FormantGrid that has not the same number of tiers as the corresponding number of amplitude tiers
 * 	- we remove/add a formant tier and a bandwidth tier together and not the corresponding amplitude tier
 * 	- we remove/add an amplitude tier and not the corresponding formant&bandwidth tiers
 * 
 * As of 20130113 the KlattGrid_addFormant (/remove) which also added automatically an amplitude tier has been split into two explicit actions
 *	KlattGrid_addFormantFrequencyAndBandwidth (/remove)
 *	KlattGrid_addFormantAmplitudeTier (/remove)
 * 
 */

conststring32 KlattGrid_getFormantName (kKlattGridFormantType formantType) {
	conststring32 result;
	if (formantType == kKlattGridFormantType::ORAL)
		result = U"Oral formant";
	else if (formantType == kKlattGridFormantType::NASAL)
		result = U"Nasal formant";
	else if (formantType == kKlattGridFormantType::FRICATION)
		result = U"Frication Formant";
	else if (formantType == kKlattGridFormantType::TRACHEAL)
		result = U"Tracheal formant";
	else if (formantType == kKlattGridFormantType::NASAL_ANTI)
		result = U"Nasal Antiformant";
	else if (formantType == kKlattGridFormantType::TRACHEALANTI)
		result = U"Tracheal antiformant";
	else if (formantType == kKlattGridFormantType::DELTA)
		result = U"Delta formant";
	else
		result = U"Unknown formant";
	return result;
}

static const conststring32 formant_names[] = { U"", U"oral ", U"nasal ", U"frication ", U"tracheal ", U"nasal anti", U"tracheal anti", U"delta "};

#define KlattGrid_OPENPHASE_DEFAULT 0.7
#define KlattGrid_POWER1_DEFAULT 3
#define KlattGrid_POWER2_DEFAULT (KlattGrid_POWER1_DEFAULT+1)

/*	Amplitude scaling: maximum amplitude (-1,+1) corresponds to 91 dB */

/*static double NUMinterpolateLinear (double x1, double y1, double x2, double y2, double x)
{
	if (y1 == y2) return y1;
	if (x1 == x2) return undefined;
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}*/

static void rel_to_abs (double *w, double *ws, integer n, double d) {
	double sum = 0.0;
	for (integer i = 1; i <= n; i ++) { // relative
		sum += w [i];
	}
	if (sum != 0.0) {
		double dw = d / sum;
		sum = 0.0;
		for (integer i = 1; i <= n; i ++) { // to absolute
			w [i] *= dw;
			sum += w [i];
			ws [i] = sum;
		}
	}
}

static bool RealTier_valuesInRange (RealTier me, double min, double max) {
	for (integer i = 1; i <= my points.size; i ++) {
		const RealPoint p = my points.at [i];
		if (isdefined (min) && p -> value < min)
			return false;
		if (isdefined (max) && p -> value < max)
			return false;
	}
	return true;
}

static double PointProcess_getPeriodAtIndex (PointProcess me, integer it, double maximumPeriod) {
	double period = undefined;
	if (it >= 2) {
		period = my t [it] - my t [it - 1];
		if (period > maximumPeriod)
			period = undefined;
	}
	if (isundef (period)) {
		if (it < my nt) {
			period = my t [it + 1] - my t [it];
			if (period > maximumPeriod)
				period = undefined;
		}
	}
	// undefined can only occur for a single isolated pulse.
	return period;
}

#define UPDATE_TIER \
	RealTier_addPoint (thee.get(), mytime, myvalue); \
	lasttime = mytime; \
	myindex ++; \
	if (myindex <= numberOfValues) { \
		mypoint = my points.at [myindex]; \
		mytime = mypoint -> number; \
		myvalue = mypoint -> value;\
	} else { \
		mytime = my xmax; \
	}

static autoRealTier RealTier_updateWithDelta (RealTier me, RealTier delta, PhonationTier glottis, double openglottis_fadeFraction) {
	try {
		integer myindex = 1;
		RealPoint mypoint = my points.at [myindex];
		const integer numberOfValues = my points.size;
		double mytime = mypoint -> number;
		double myvalue = mypoint -> value;
		double lasttime = my xmin - 0.001;   // sometime before xmin
		autoRealTier thee = RealTier_create (my xmin, my xmax);

		if (openglottis_fadeFraction <= 0.0)
			openglottis_fadeFraction = 0.0001;
		if (openglottis_fadeFraction >= 0.5)
			openglottis_fadeFraction = 0.4999;
		for (integer ipoint = 1; ipoint <= glottis -> points.size; ipoint ++) {
			const PhonationPoint point = glottis -> points.at [ipoint];
			const double t4 = point -> number;   // glottis closing
			const double openDuration = point -> te;
			const double t1 = t4 - openDuration;
			const double t2 = t1 + openglottis_fadeFraction * openDuration;
			const double t3 = t4 - openglottis_fadeFraction * openDuration;
			/*
				Add my points that lie before t1 and after previous t4.
			*/
			while (mytime > lasttime && mytime < t1) {
				UPDATE_TIER
			}
			if (t2 > t1) {
				// Set new value at t1
				const double myvalue1 = RealTier_getValueAtTime (me, t1);
				RealTier_addPoint (thee.get(), t1, myvalue1);
				// Add my points between t1 and t2
				while (mytime > lasttime && mytime < t2) {
					const double dvalue = RealTier_getValueAtTime (delta, mytime);
					if (isdefined (dvalue)) {
						const double fraction = (mytime - t1) / (openglottis_fadeFraction * openDuration);
						myvalue += dvalue * fraction;
					}
					UPDATE_TIER
				}
			}

			double myvalue2 = RealTier_getValueAtTime (me, t2);
			double dvalue = RealTier_getValueAtTime (delta, t2);
			if (isdefined (dvalue))
				myvalue2 += dvalue;
			RealTier_addPoint (thee.get(), t2, myvalue2);

			// Add points between t2 and t3

			while (mytime > lasttime && mytime < t3) {
				dvalue = RealTier_getValueAtTime (delta, mytime);
				if (isdefined (dvalue))
					myvalue += dvalue;
				UPDATE_TIER
			}

			// set new value at t3

			double myvalue3 = RealTier_getValueAtTime (me, t3);
			dvalue = RealTier_getValueAtTime (delta, t3);
			if (isdefined (dvalue)) {
				myvalue3 += dvalue;
			}
			RealTier_addPoint (thee.get(), t3, myvalue3);

			if (t4 > t3) {
				// Add my points between t3 and t4
				while (mytime > lasttime && mytime < t4) {
					dvalue = RealTier_getValueAtTime (delta, mytime);
					if (isdefined (dvalue)) {
						const double fraction = 1 - (mytime - t3) / (openglottis_fadeFraction * openDuration);
						myvalue += dvalue * fraction;
					}
					UPDATE_TIER
				}

				// Set new value at t4
				const double myvalue4 = RealTier_getValueAtTime (me, t4);
				RealTier_addPoint (thee.get(), t4, myvalue4);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not updated with delta.");
	}
}

static bool FormantGrid_isFormantDefined (FormantGrid me, integer iformant) {
	// formant and bandwidth are always in sync
	const RealTier ftier = my formants.at [iformant];
	const RealTier btier = my bandwidths.at [iformant];
	return ftier -> points.size > 0 and btier -> points.size > 0;
}

static bool FormantGrid_Intensities_isFormantDefined (FormantGrid me, OrderedOf<structIntensityTier>* thee, integer iformant) {
	bool exists = false;
	if (iformant <= my formants.size && iformant <= my bandwidths.size && iformant <= thee->size) {
		const RealTier ftier = my formants.at [iformant];
		const RealTier btier = my bandwidths.at [iformant];
		const IntensityTier atier = thy at [iformant];
		exists = ( ftier -> points.size > 0 && btier -> points.size > 0 && atier -> points.size > 0 );
	}
	return exists;
}

static void check_formants (integer numberOfFormants, integer *ifb, integer *ife) {
	if (numberOfFormants <= 0 || *ifb > numberOfFormants || *ife < *ifb || *ife < 1) {
		*ife = 0;  // overrules everything *ifb value is a don't care now
		return;
	}
	if (*ifb <= 1)
		*ifb = 1;
	if (*ife > numberOfFormants)
		*ife = numberOfFormants;
}

static autoSound Sound_createEmptyMono (double xmin, double xmax, double samplingFrequency) {
	const integer nt = Melder_iceiling ((xmax - xmin) * samplingFrequency);
	const double dt = 1.0 / samplingFrequency;
	const double tmid = (xmin + xmax) / 2.0;
	const double t1 = tmid - 0.5 * (nt - 1) * dt;

	return Sound_create (1, xmin, xmax, nt, dt, t1);
}

static void _Sounds_add_inplace (Sound me, Sound thee) {
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] += thy z [1] [i];
}

static autoSound _Sound_diff (Sound me, int scale) {
	try {
		autoSound thee = Data_copy (me);

		// extremum
		double amax1 = -1.0e34, amax2 = amax1, val, pval = 0.0;
		if (scale) {
			for (integer i = 1; i <= thy nx; i ++) {
				val = fabs (thy z [1] [i]);
				if (val > amax1)
					amax1 = val;
			}
		}
		// x [n]-x [n-1]
		for (integer i = 1; i <= thy nx; i ++) {
			val = thy z [1] [i];
			thy z [1] [i] -=  pval;
			pval = val;
		}
		if (scale) {
			for (integer i = 1; i <= thy nx; i ++) {
				val = fabs (thy z [1] [i]);
				if (val > amax2)
					amax2 = val;
			}
			// scale
			for (integer i = 1; i <= thy nx; i ++)
				thy z [1] [i] *= amax1 / amax2;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not differenced.");
	}
}

/*static void _Sounds_addDifferentiated_inplace (Sound me, Sound thee)
{
	double pval = 0, dx = my dx;
	for (integer i = 1; i <= my nx; i ++)
	{
		const double val =  thy z [1] [i];
		my z [1] [i] += (val - pval) / dx; // dx makes amplitude of dz/dt independent of sampling.
		pval = val;
	}
}*/

typedef struct structconnections {
	integer numberOfConnections;
	autoVEC x, y;
} *connections;

static void connections_free (connections me) {
	if (! me)
		return;
	Melder_free (me);
}

static connections connections_create (integer numberOfConnections) {
	connections me = 0;
	try {
		me = (connections) Melder_calloc (structconnections, 1);
		my numberOfConnections = numberOfConnections;
		my x = zero_VEC (numberOfConnections);
		my y = zero_VEC (numberOfConnections);
		return me;
	} catch (MelderError) {
		connections_free (me);
		Melder_throw (U"Connections not created.");
	}
}

// Calculates the intersection point (xi,yi) of a line with a circle.
// The line starts at the origin and P (xp, yp) is on that line.
static void NUMcircle_radial_intersection_sq (double x, double y, double r, double xp, double yp, double *xi, double *yi) {
	double dx = xp - x, dy = yp - y;
	const double d = sqrt (dx * dx + dy * dy);
	if (d > 0) {
		*xi = x + dx * r / d;
		*yi = y + dy * r / d;
	} else {
		*xi = *yi = undefined;
	}
}

static void summer_draw (Graphics g, double x, double y, double r, bool alternating) {
	Graphics_setLineWidth (g, 2);
	Graphics_circle (g, x, y, r);
	const double dy = 3.0 * r / 4.0;
	// + symbol
	if (alternating)
		y += r / 4.0;
	Graphics_line (g, x, y + r / 2.0, x, y - r / 2.0);
	Graphics_line (g, x - r / 2.0, y, x + r / 2.0, y);
	if (alternating)
		Graphics_line (g, x - r / 2.0, y - dy , x + r / 2.0, y - dy);
}

static void _summer_drawConnections (Graphics g, double x, double y, double r, connections thee, bool arrow, bool alternating, double horizontalFraction) {
	summer_draw (g, x, y, r, alternating);

	for (integer i = 1; i <= thy numberOfConnections; i ++) {
		const double yp = thy y [i];
		double xto, yto, xp = thy x [i];
		if (horizontalFraction > 0) {
			const double dx = x - xp;
			if (dx > 0) {
				xp += horizontalFraction * dx;
				Graphics_line (g, thy x [i], yp, xp, yp);
			}
		}
		NUMcircle_radial_intersection_sq (x, y, r, xp, yp, & xto, & yto);
		if (isundef (xto) || isundef (yto))
			continue;
		if (arrow)
			Graphics_arrow (g, xp, yp, xto, yto);
		else
			Graphics_line (g, xp, yp, xto, yto);
	}
}

static void summer_drawConnections (Graphics g, double x, double y, double r, connections thee, bool arrow, double horizontalFraction) {
	_summer_drawConnections (g, x, y, r, thee, arrow, false, horizontalFraction);
}

static void alternatingSummer_drawConnections (Graphics g, double x, double y, double r, connections thee, bool arrow, double horizontalFraction) {
	_summer_drawConnections (g, x, y, r, thee, arrow, true, horizontalFraction);
}

static void draw_oneSection (Graphics g, double xmin, double xmax, double ymin, double ymax,
	conststring32 line1, conststring32 line2, conststring32 line3)
{
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	integer numberOfTextLines = 0;
	if (line1)
		numberOfTextLines ++;
	if (line2)
		numberOfTextLines ++;
	if (line3)
		numberOfTextLines ++;
	const double dy = (ymax - ymin) / (numberOfTextLines + 1), ddy = dy / 10.0;
	const double x = (xmax + xmin) / 2.0;
	double y = ymax;
	integer iline = 0;
	if (line1) {
		iline ++;
		y -= dy - ( numberOfTextLines == 2 ? ddy : 0.0 ); // extra spacing for two lines
		Graphics_text (g, x, y, line1);
	}
	if (line2) {
		iline ++;
		y -= dy - ( numberOfTextLines == 2 ? ( iline == 1 ? ddy : -iline * ddy ) : 0.0 );
		Graphics_text (g, x, y, line2);
	}
	if (line3) {
		iline ++;
		y -= dy - ( numberOfTextLines == 2 ? -iline * ddy : 0.0 );
		Graphics_text (g, x, y, line3);
	}
}

// Maximum amplitue (-1,1) at 93.97940008672037 dB
#define DBSPL_to_A(x) (pow (10.0, x / 20.0) * 2.0e-5)
// Normal dB's
#define DB_to_A(x) (pow (10.0, x / 20.0))

/************************ Sound & FormantGrid *********************************************/

static void _Sound_FormantGrid_filterWithOneFormant_inplace (Sound me, FormantGrid thee, integer iformant, bool antiformant) {
	if (iformant < 1 || iformant > thy formants.size) {
		Melder_warning (U"Formant ", iformant, U" does not exist.");
		return;
	}
	const RealTier ftier = thy formants.at [iformant];
	const RealTier btier = thy bandwidths.at [iformant];
	if (ftier -> points.size == 0 && btier -> points.size == 0)
		return;
	Melder_require (ftier -> points.size != 0 && btier -> points.size != 0,
		U"Tier should not be empty,");
	const double nyquist = 0.5 / my dx;
	autoFilter r;
	if (antiformant)
		r = AntiResonator_create (my dx);
	else
		r = Resonator_create (my dx, true);

	for (integer is = 1; is <= my nx; is ++) {
		const double t = my x1 + (is - 1) * my dx;
		const double f = RealTier_getValueAtTime (ftier, t);
		const double b = RealTier_getValueAtTime (btier, t);
		if (f <= nyquist && isdefined (b))
			Filter_setCoefficients (r.get(), f, b);
		my z [1] [is] = Filter_getOutput (r.get(), my z [1] [is]);
	}
}

void Sound_FormantGrid_filterWithOneAntiFormant_inplace (Sound me, FormantGrid thee, integer iformant) {
	_Sound_FormantGrid_filterWithOneFormant_inplace (me, thee, iformant, true);
}

void Sound_FormantGrid_filterWithOneFormant_inplace (Sound me, FormantGrid thee, integer iformant) {
	_Sound_FormantGrid_filterWithOneFormant_inplace (me, thee, iformant, false);
}

void Sound_FormantGrid_Intensities_filterWithOneFormant_inplace (Sound me, FormantGrid thee, OrderedOf<structIntensityTier>* amplitudes, integer iformant) {
	try {
		Melder_require (iformant > 0 && iformant <= thy formants.size, U"Formant ", iformant, U" not defined.");
		
		const double nyquist = 0.5 / my dx;

		const RealTier ftier = thy formants.at [iformant];
		const RealTier btier = thy bandwidths.at [iformant];
		const IntensityTier atier = amplitudes->at [iformant];

		if (ftier -> points.size == 0 || btier -> points.size == 0 || atier -> points.size == 0)
			return;    // nothing to do
		autoResonator r = Resonator_create (my dx, false);
		for (integer is = 1; is <= my nx; is ++) {
			const double t = my x1 + (is - 1) * my dx;
			const double f = RealTier_getValueAtTime (ftier, t);
			const double b = RealTier_getValueAtTime (btier, t);
			if (f <= nyquist && isdefined (b)) {
				Filter_setCoefficients (r.get(), f, b);
				const double a = RealTier_getValueAtTime (atier, t);
				if (isdefined (a))
					r -> a *= DB_to_A (a);
			}
			my z [1] [is] = Filter_getOutput (r.get(), my z [1] [is]);
		}
	} catch (MelderError) {
		Melder_throw (me, U": not filtered with one formant filter.");
	}
}

autoSound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, OrderedOf<structIntensityTier>* amplitudes, integer iformantb, integer iformante, int alternatingSign) {
	try {
		if (iformantb > iformante) {
			iformantb = 1;
			iformante = thy formants.size;
		}
		Melder_require (iformantb > 0 && iformantb <= thy formants.size ,
			U"From formant ", iformantb, U" not defined.");
		Melder_require (iformante > 0 && iformante <= thy formants.size ,
			U"To formant ", iformante, U" not defined.");

		autoSound him = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);

		for (integer iformant = iformantb; iformant <= iformante; iformant ++) {
			if (FormantGrid_Intensities_isFormantDefined (thee, amplitudes, iformant)) {
				autoSound tmp = Data_copy (me);
				Sound_FormantGrid_Intensities_filterWithOneFormant_inplace (tmp.get(), thee, amplitudes, iformant);
				for (integer is = 1; is <= my nx; is ++)
					his z [1] [is] += ( alternatingSign >= 0 ? tmp -> z [1] [is] : - tmp -> z [1] [is] );
				if (alternatingSign != 0)
					alternatingSign = - alternatingSign;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered.");
	}
}

/********************* PhonationTier ************************/

Thing_implement (PhonationPoint, AnyPoint, 0);

autoPhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te, double power1, double power2, double pulseScale) {
	try {
		autoPhonationPoint me = Thing_new (PhonationPoint);
		my number = time;
		my period = period;
		my openPhase = openPhase;
		my collisionPhase = collisionPhase;
		my te = te;
		my power1 = power1;
		my power2 = power2;
		my pulseScale = pulseScale;
		return me;
	} catch (MelderError) {
		Melder_throw (U"PhonationPoint not created.");
	}
}

Thing_implement (PhonationTier, AnyTier, 0);

autoPhonationTier PhonationTier_create (double tmin, double tmax) {
	try {
		autoPhonationTier me = Thing_new (PhonationTier);
		Function_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PhonationTier not created.");
	}
}

autoPointProcess PhonationTier_to_PointProcess_closures (PhonationTier me) {
	try {
		const integer nt = my points.size;
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, nt);
		for (integer ip = 1; ip <= nt; ip ++) {
			const PhonationPoint fp = my points.at [ip];
			PointProcess_addPoint (thee.get(), fp -> number);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PointProcess with closure times created.");
	}
}

/********************** PhonationGridPlayOptions **********************/

Thing_implement (PhonationGridPlayOptions, Daata, 0);

static void PhonationGridPlayOptions_setDefaults (PhonationGridPlayOptions me) {
	my flowDerivative = my voicing = 1;
	my aspiration = my breathiness = 1;
	my flutter = my doublePulsing = 1;
	my collisionPhase = my spectralTilt = 1;
	my flowFunction = 1; // User defined flow tiers (power1 & power2)
	my maximumPeriod = 0;
}

autoPhonationGridPlayOptions PhonationGridPlayOptions_create () {
	try {
		autoPhonationGridPlayOptions me = Thing_new (PhonationGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PhonationGridPlayOptions not created.");
	}
}

/********************** PhonationGrid **********************/


Thing_implement (PhonationGrid, Function, 0);

void structPhonationGrid :: v_info () {
	structDaata :: v_info ();
	conststring32 in1 = U"  ", in2 = U"    ";
	MelderInfo_writeLine (in1, U"Time domain:");
	MelderInfo_writeLine (in2, U"Start time:     ", xmin, U" seconds");
	MelderInfo_writeLine (in2, U"End time:       ", xmax, U" seconds");
	MelderInfo_writeLine (in2, U"Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (in1, U"\nNumber of points in the PHONATION tiers:");
	MelderInfo_writeLine (in2, U"pitch:               ", pitch -> points.size);
	MelderInfo_writeLine (in2, U"voicingAmplitude:    ", voicingAmplitude -> points.size);
	MelderInfo_writeLine (in2, U"openPhase:           ", openPhase -> points.size);
	MelderInfo_writeLine (in2, U"collisionPhase:      ", collisionPhase -> points.size);
	MelderInfo_writeLine (in2, U"power1:              ", power1 -> points.size);
	MelderInfo_writeLine (in2, U"power2:              ", power2 -> points.size);
	MelderInfo_writeLine (in2, U"flutter:             ", flutter -> points.size);
	MelderInfo_writeLine (in2, U"doublePulsing:       ", doublePulsing -> points.size);
	MelderInfo_writeLine (in2, U"spectralTilt:        ", spectralTilt -> points.size);
	MelderInfo_writeLine (in2, U"aspirationAmplitude: ", aspirationAmplitude -> points.size);
	MelderInfo_writeLine (in2, U"breathinessAmplitude:", breathinessAmplitude -> points.size);
}

void PhonationGrid_setNames (PhonationGrid me) {
	Thing_setName (my pitch.get(), U"pitch");
	Thing_setName (my voicingAmplitude.get(), U"voicingAmplitude");
	Thing_setName (my openPhase.get(), U"openPhase");
	Thing_setName (my collisionPhase.get(), U"collisionPhase");
	Thing_setName (my power1.get(), U"power1");
	Thing_setName (my power2.get(), U"power2");
	Thing_setName (my flutter.get(), U"flutter");
	Thing_setName (my doublePulsing.get(), U"doublePulsing");
	Thing_setName (my spectralTilt.get(), U"spectralTilt");
	Thing_setName (my aspirationAmplitude.get(), U"aspirationAmplitude");
	Thing_setName (my breathinessAmplitude.get(), U"breathinessAmplitude");
}

autoPhonationGrid PhonationGrid_create (double tmin, double tmax) {
	try {
		autoPhonationGrid me = Thing_new (PhonationGrid);
		Function_init (me.get(), tmin, tmax);
		my pitch = PitchTier_create (tmin, tmax);
		my voicingAmplitude = IntensityTier_create (tmin, tmax);
		my openPhase = RealTier_create (tmin, tmax);
		my collisionPhase = RealTier_create (tmin, tmax);
		my power1 = RealTier_create (tmin, tmax);
		my power2 = RealTier_create (tmin, tmax);
		my flutter = RealTier_create (tmin, tmax);
		my doublePulsing = RealTier_create (tmin, tmax);
		my spectralTilt = IntensityTier_create (tmin, tmax);
		my aspirationAmplitude = IntensityTier_create (tmin, tmax);
		my breathinessAmplitude = IntensityTier_create (tmin, tmax);
		my options = PhonationGridPlayOptions_create ();
		PhonationGrid_setNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"PhonationGrid not created.");
	}
}

static void PhonationGrid_checkFlowFunction (PhonationGrid me) {
	int hasPower1Points = my power1 -> points.size > 0;
	int hasPower2Points = my power2 -> points.size > 0;

	integer ipoint = 1;
	do {
		const double time = ( hasPower1Points ? my power1 -> points.at [ipoint] -> number : 0.5 * (my xmin + my xmax) );
		double power1 = RealTier_getValueAtIndex (my power1.get(), ipoint);
		if (isundef (power1))
			power1 = KlattGrid_POWER1_DEFAULT;
		Melder_require (power1 > 0.0,
			U"All power1 values should greater than zero.");

		double power2 = RealTier_getValueAtTime (my power2.get(), time);
		if (isundef (power2))
			power2 = KlattGrid_POWER2_DEFAULT;
		Melder_require (power1 < power2,
			U"At all times a power1 value should be smaller than the corresponding power2 value.");
		
	} while ( ++ ipoint < my power1 -> points.size);

	// Now check power2 values. This is necessary to catch situations where power2 has a valley:
	// power1(0) = 3; power2(1)= 4; power2(1)= 4; power2(0.5) = 3;

	ipoint = 1;
	do {
		const double time = ( hasPower2Points ? my power2 -> points.at [ipoint] -> number : 0.5 * (my xmin + my xmax) );
		double power2 = RealTier_getValueAtIndex (my power2.get(), ipoint);
		if (isundef (power2))
			power2 = KlattGrid_POWER2_DEFAULT;
		double power1 = RealTier_getValueAtTime (my power1.get(), time);
		if (isundef (power1))
			power1 = KlattGrid_POWER1_DEFAULT;
		Melder_require (power1 < power2,
			U"At all times a power1 value should be smaller than the corresponding power2 value.");
		
	} while ( ++ ipoint < my power2 -> points.size);
}

static void PhonationGrid_draw_inside (PhonationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *out_y) {
	// dum voicing conn tilt conn summer
	(void) me;
	double xw [6] = { 0.0, 1.0, 0.5, 1.0, 0.5, 0.5 }, xws [6];

	connections thee = connections_create (2);

	rel_to_abs (xw, xws, 5, xmax - xmin);

	dy = (ymax - ymin) / (1.0 + ( dy < 0.0 ? 0.0 : dy ) + 1.0);

	double x1 = xmin, x2 = x1 + xw [1];
	double y2 = ymax, y1 = y2 - dy;
	draw_oneSection (g, x1, x2, y1, y2, nullptr, U"Voicing", nullptr);

	x1 = x2;
	x2 = x1 + xw [2];
	double ymid = (y1 + y2) / 2.0;
	Graphics_line (g, x1, ymid, x2, ymid);

	x1 = x2;
	x2 = x1 + xw [3];
	draw_oneSection (g, x1, x2, y1, y2, nullptr, U"Tilt", nullptr);

	thy x [1] = x2;
	thy y [1] = ymid;

	y2 = y1 - 0.5 * dy;
	y1 = y2 - dy;
	ymid = (y1 + y2) / 2.0;
	x2 = xmin + xws [3];
	x1 = x2 - 1.5 * xw [3]; // some extra space
	draw_oneSection (g, x1, x2, y1, y2, nullptr, U"Aspiration", nullptr);

	thy x [2] = x2;
	thy y [2] = ymid;

	const double r = xw [5] / 2.0;
	const double xs = xmax - r, ys = (ymax + ymin) / 2.0;
	const bool arrow = true;

	summer_drawConnections (g, xs, ys, r, thee, arrow, 0.4);
	connections_free (thee);

	if (out_y)
		*out_y = ys;
}

void PhonationGrid_draw (PhonationGrid me, Graphics g) {
	const double xmin = 0.0, xmax2 = 0.9, xmax = 1.0, ymin = 0.0, ymax = 1.0, dy = 0.5;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	double yout;
	PhonationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, & yout);

	Graphics_arrow (g, xmax2, yout, xmax, yout);
	Graphics_unsetInner (g);
}

double PhonationGrid_getMaximumPeriod (PhonationGrid me) {
	const double minimumPitch = RealTier_getMinimumValue (my pitch.get());
	return 2.0 / ( isdefined (minimumPitch) && minimumPitch != 0.0 ? minimumPitch : my xmax - my xmin );
}

static autoPointProcess PitchTier_to_PointProcess_flutter (PitchTier pitch, RealTier flutter, double maximumPeriod) {
	try {
		autoPointProcess thee = PitchTier_to_PointProcess (pitch);
		if (! flutter)
			return thee;
		double tsum = 0;
		for (integer it = 2; it <= thy nt; it ++) {
			const double t = thy t [it - 1];
			const double period = thy t [it] - thy t [it - 1];
			if (period < maximumPeriod && flutter -> points.size > 0) {
				const double fltr = RealTier_getValueAtTime (flutter, t);
				if (isdefined (fltr)) {
					// newF0 = f0 * (1 + (val / 50) * (sin ... + ...));
					const double newPeriod = period / (1.0 + (fltr / 50.0) * (sin (NUM2pi * 12.7 * t) + sin (NUM2pi * 7.1 * t) + sin (NUM2pi * 4.7 * t)));
					tsum += newPeriod - period;
				}
			}
			thy t [it] += tsum;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (pitch, U": no flutter PointProcess created.");
	}
}

autoSound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency) {
	try {
		autoSound thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);

		// Noise spectrum is tilted down by soft low-pass filter having a pole near
		// the origin in the z-plane, i.e. y [n] = x [n] + (0.75 * y [n-1])
		double lastval = 0.0;
		if (my aspirationAmplitude -> points.size > 0) {
			for (integer i = 1; i <= thy nx; i ++) {
				const double t = thy x1 + (i - 1) * thy dx;
				double val = NUMrandomUniform (-1.0, 1.0);
				const double a = DBSPL_to_A (RealTier_getValueAtTime (my aspirationAmplitude.get(), t));
				if (isdefined (a)) {
					thy z [1] [i] = lastval = val + 0.75 * lastval;
					lastval = (val += 0.75 * lastval); // soft low-pass
					thy z [1] [i] = val * a;
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no aspiration Sound created.");
	}
}

static void Sound_PhonationGrid_spectralTilt_inplace (Sound thee, PhonationGrid me) {
	if (my spectralTilt -> points.size > 0) {
		/* Spectral tilt
			Filter y [n] = a * x [n] + b * y [n-1] => H(z) = a / (1 - bz^(-1)).
			We need attenuation, i.e. low-pass. Therefore 0 <= b <= 1.
			|H(f)| = a / sqrt (1 - 2*b*cos(2*pi*f*T) + b^2),
			|H(0)|= a /(1 - b) => if |H(0)| == 1, then a = 1 - b.
			Now solve 20 log|H(F)|= -c (at F=3 kHz and c > 0)
			Solution: if q = (1 - D * cos(2*pi*F*T)) / (1 - D), with D = 10^(-c/10)
				then b = q -sqrt(q^2 - 1)
		*/

		const double cosf = cos (NUM2pi * 3000.0 * thy dx); // samplingFrequency > 6000.0 !
		double ynm1 = 0.0;

		for (integer i = 1; i <= thy nx; i ++) {
			const double t = thy x1 + (i - 1) * thy dx;
			const double tilt_db = RealTier_getValueAtTime (my spectralTilt.get(), t);

			if (tilt_db > 0) {
				const double d = pow (10.0, -tilt_db / 10.0);
				const double q = (1.0 - d * cosf) / (1.0 - d);
				const double b = q - sqrt (q * q - 1.0);
				const double a = 1.0 - b;
				thy z [1] [i] = a * thy z [1] [i] + b * ynm1;
				ynm1 = thy z [1] [i];
			}
		}
	}
}

struct nrfunction_struct {
	double n;
	double m;
	double a;
};

static double nrfunction (double x, double *dfx, void *closure) {
	const struct nrfunction_struct *nrfs = (struct nrfunction_struct *) closure;
	const double mplusax = nrfs -> m + nrfs -> a * x;
	const double mminn = nrfs -> m - nrfs -> n;
	const double fx = pow (x, mminn) - (nrfs -> n + nrfs -> a * x) / mplusax;
	*dfx = mminn * pow (x, mminn - 1) - nrfs -> a * mminn / (mplusax * mplusax);
	return fx;
}

static double get_collisionPoint_x (double n, double m, double collisionPhase) {
	double y = undefined;
	/*
	Domain [0,1]:
	The glottal flow is given by:
	U(y) = y^n - y^m               0<= y <= x, and m > n
			(x^n - x^m)exp(-a(y-x))  y >= x, where a = 1 / collisionPhase
	The x where this occurs is the point where the amplitudes as well as the derivatives are equal.
	I.e. the x where n x^(n-1) - m x^(m-1) = (x^n-x^m)(-a).
	This can be simplified: find x in (0,1) where f(x) = x^(m-n) - (n+ax)/(m+ax) == 0.
		For m - n == 1, f(x) is a second order equation f(x)= a x^2 + (m-a) x - n == 0.
	In all other cases we solve with Newton-Raphson. For these cases we also need the derivative:
		f'(x)= (m - n)x^(m - n - 1)- a(m - n) / (m + a x)^2
	*/
	if (collisionPhase <= 0.0)
		return 1.0;
	double a = 1.0 / collisionPhase;
	if (m - n == 1.0) {
		const double b = m - a;
		const double c = - n;
		double y1, y2;
		integer nroots = NUMsolveQuadraticEquation (a, b, c, &y1, &y2);
		if (nroots == 2)
			y = y2;
		else if (nroots == 1)
			y = y1;
	} else { // Newton-Raphson
		// search in the interval from where the flow is a maximum to 1
		const double xmaxFlow = pow (n / m, 1.0 / (m - n));
		struct nrfunction_struct nrfs = {n, m, a};
		double root = NUMnrbis (& nrfunction, xmaxFlow, 1.0, & nrfs);
		y = root;
	}
	return y;
}

autoPhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me) {
	try {
		integer diplophonicPulseIndex = 0;
		const PhonationGridPlayOptions pp = my options.get();

		PhonationGrid_checkFlowFunction (me);
		Melder_require (my pitch -> points.size > 0,
			U"Pitch tier should not be empty.");

		if (pp -> maximumPeriod == 0.0)
			pp -> maximumPeriod = PhonationGrid_getMaximumPeriod (me);

		autoPointProcess point = PitchTier_to_PointProcess_flutter (my pitch.get(), ( pp -> flutter ? my flutter.get() : nullptr ), pp -> maximumPeriod);

		autoPhonationTier thee = PhonationTier_create (my xmin, my xmax);

		/*
		Cycle through the points of the point PointProcess. Each will become a period.
		We assume that the planning for the pitch period occurs approximately at a time T before the glottal closure.
		For each point t [i]:
			Determine the f0 -> period T [i]
			Determine time t [i]-T [i] the open quotient, power1, power2, collisionphase etc.
			Generate the period.
		*/

		for (integer it = 1; it <= point -> nt; it ++) {
			double re = 0.0, t = point -> t [it];		// the glottis "closing" point
			double pulseDelay = 0.0;        // For alternate pulses in case of diplophonia
			double pulseScale = 1.0;        // For alternate pulses in case of diplophonia

			double period = PointProcess_getPeriodAtIndex (point.get(), it, pp -> maximumPeriod);
			if (isundef (period))
				period = 0.5 * pp -> maximumPeriod;   // some default value

			// Calculate the point where the exponential decay starts:
			// Query tiers where period starts .

			double periodStart = t - period;
			double collisionPhase = ( pp -> collisionPhase ? RealTier_getValueAtTime (my collisionPhase.get(), periodStart) : 0.0 );
			if (isundef (collisionPhase))
				collisionPhase = 0.0;
			double power1 = ( pp -> flowFunction == 1 ? RealTier_getValueAtTime (my power1.get(), periodStart) : pp -> flowFunction );
			if (isundef (power1))
				power1 = KlattGrid_POWER1_DEFAULT;
			double power2 = ( pp -> flowFunction == 1 ? RealTier_getValueAtTime (my power2.get(), periodStart) : pp -> flowFunction + 1 );
			if (isundef (power2))
				power2 = KlattGrid_POWER2_DEFAULT;
			try {
				re = get_collisionPoint_x (power1, power2, collisionPhase);
			} catch (MelderError) {
				Melder_warning (U"Illegal collision point at t = ", t, U" (power1=", power1, U", power2=", power2, U"colPhase=", collisionPhase, U")");
			}

			double openPhase = RealTier_getValueAtTime (my openPhase.get(), periodStart);
			if (isundef (openPhase))
				openPhase = KlattGrid_OPENPHASE_DEFAULT;

			const double te = re * period * openPhase;

			// In case of diplophonia alternate pulses get modified.
			// A modified puls is delayed in time and its amplitude attenuated.
			// This delay scales to maximally equal the closed phase of the next period.
			// The doublePulsing scales the amplitudes as well as the delay linearly.

			double doublePulsing = ( pp -> doublePulsing ? RealTier_getValueAtTime (my doublePulsing.get(), periodStart) : 0.0 );
			if (isundef (doublePulsing))
				doublePulsing = 0.0;
			if (doublePulsing > 0.0) {
				diplophonicPulseIndex ++;
				if (diplophonicPulseIndex % 2 == 1) {   // the odd-numbered one
					double nextPeriod = PointProcess_getPeriodAtIndex (point.get(), it + 1, pp -> maximumPeriod);
					if (isundef (nextPeriod))
						nextPeriod = period;
					double openPhase2 = KlattGrid_OPENPHASE_DEFAULT;
					if (my openPhase -> points.size > 0)
						openPhase2 = RealTier_getValueAtTime (my openPhase.get(), t);
					const double maxDelay = period * (1.0 - openPhase2);
					pulseDelay = maxDelay * doublePulsing;
					pulseScale *= 1.0 - doublePulsing;
				}
			} else
				diplophonicPulseIndex = 0;

			t += pulseDelay;
			autoPhonationPoint phonationPoint = PhonationPoint_create (t, period, openPhase, collisionPhase, te, power1, power2, pulseScale);
			AnyTier_addPoint_move (thee.get()->asAnyTier(), phonationPoint.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PhonationTier created.");
	}
}

static autoSound PhonationGrid_PhonationTier_to_Sound_voiced (PhonationGrid me, PhonationTier thee, double samplingFrequency) {
	try {
		const PhonationGridPlayOptions p = my options.get();
		double lastVal = undefined;

		Melder_require (my voicingAmplitude -> points.size > 0,
			U"Voicing amplitude tier should not be empty.");

		autoSound him = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);
		autoSound breathy;
		if (p -> breathiness && my breathinessAmplitude -> points.size > 0)
			breathy = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);

		/*
			Cycle through the points of the PhonationTier. Each will become a period.
			We assume that the planning for the pitch period occurs approximately at a time T before the glottal closure.
			For each point t [i]:
				Determine the f0 -> period T [i]
				Determine time t [i]-T [i] the open quotient, power1, power2, collisionphase etc.
				Generate the period.
		*/
		VEC sound = his z.row (1);
		for (integer it = 1; it <= thy points.size; it ++) {
			PhonationPoint point = thy points.at [it];
			const double t = point -> number;		// the glottis "closing" point
			const double te = point -> te;
			const double period = point -> period; // duration of the current period
			const double openPhase = point -> openPhase;
			const double collisionPhase = point -> collisionPhase;
			const double pulseScale = point -> pulseScale;        // For alternate pulses in case of diplophonia
			const double power1 = point -> power1, power2 = point -> power2;
			double phase;                 // 0..1
			double flow;

			//- double amplitude = pulseScale * (power1 + power2 + 1.0) / (power2 - power1);
			//- amplitude /= period * openPhase;

			// Maximum of U(x) = x^n - x^m is where the derivative U'(x) = n x^(n-1) - m x^(m-1) == 0,
			//	i.e. (n/m) = x^(m-n), so xmax = (n/m)^(1/(m-n))
			//	U(xmax) = x^n (1-x^(m-n)) = (n/m)^(n/(m-n))(1-n/m)

			const double amplitude = pulseScale / (pow (power1 / power2, 1.0 / (power2 / power1 - 1.0)) * (1.0 - power1 / power2));

			// Fill in the samples to the left of the current point.

			integer midSample = Sampled_xToLowIndex (him.get(), t), beginSample;
			beginSample = midSample - Melder_ifloor (te / his dx);
			if (beginSample < 1)
				beginSample = 0;
			if (midSample > his nx)
				midSample = his nx;
			for (integer i = beginSample; i <= midSample; i ++) {
				const double tsamp = his x1 + (i - 1) * his dx;
				phase = (tsamp - (t - te)) / (period * openPhase);
				if (phase > 0.0) {
					flow = amplitude * (pow (phase, power1) - pow (phase, power2));
					if (i == 0) {
						lastVal = flow;    // For the derivative
						continue;
					}
					sound [i] += flow;

					// Breathiness only during open part modulated by the flow
					if (breathy) {
						double val = flow * NUMrandomUniform (-1.0, 1.0);
						double a = RealTier_getValueAtTime (my breathinessAmplitude.get(), t);
						breathy -> z [1] [i] += val * DBSPL_to_A (a);
					}
				}
			}

			// Determine the signal parameters at the current point.

			phase = te / (period * openPhase);

			//- double flow = amplitude * (period * openPhase) * (pow (phase, power1) - pow (phase, power2));

			flow = amplitude * (pow (phase, power1) - pow (phase, power2));

			// Fill in the samples to the right of the current point.

			if (flow > 0.0) {
				const double ta = collisionPhase * (period * openPhase);
				const double factorPerSample = exp (- his dx / ta);
				double value = flow * exp (- (his x1 + midSample * his dx - t) / ta);
				integer endSample = midSample + Melder_ifloor (20.0 * ta / his dx);
				if (endSample > his nx)
					endSample = his nx;
				for (integer i = midSample + 1; i <= endSample; i ++) {
					sound [i] += value;
					value *= factorPerSample;
				}
			}
		}

		// Scale voiced part and add breathiness during open phase
		if (p -> flowDerivative) {
			const double extremum = Vector_getAbsoluteExtremum (him.get(), 0.0, 0.0, kVector_peakInterpolation :: CUBIC);
			if (isundef (lastVal))
				lastVal = 0.0;
			for (integer i = 1; i <= his nx; i ++) {
				const double val = his z [1] [i];
				his z [1] [i] -= lastVal;
				lastVal = val;
			}
			Vector_scale (him.get(), extremum);
		}

		for (integer i = 1; i <= his nx; i ++) {
			const double t = his x1 + (i - 1) * his dx;
			his z [1] [i] *= DBSPL_to_A (RealTier_getValueAtTime (my voicingAmplitude.get(), t));
			if (breathy)
				his z [1] [i] += breathy -> z [1] [i];
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}

static autoSound PhonationGrid_to_Sound_voiced (PhonationGrid me, double samplingFrequency) {
	try {
		autoPhonationTier thee = PhonationGrid_to_PhonationTier (me);
		return PhonationGrid_PhonationTier_to_Sound_voiced (me, thee.get(), samplingFrequency);
	} catch (MelderError) {
		Melder_throw (me, U": no voiced Sound created.");
	}
}

static autoSound PhonationGrid_to_Sound (PhonationGrid me, CouplingGrid him, double samplingFrequency) {
	try {
		PhonationGridPlayOptions pp = my options.get();
		autoSound thee;
		if (pp -> voicing) {
			if (him && his glottis -> points.size > 0)
				thee = PhonationGrid_PhonationTier_to_Sound_voiced (me, his glottis.get(), samplingFrequency);
			else
				thee = PhonationGrid_to_Sound_voiced (me, samplingFrequency);
			if (pp -> spectralTilt)
				Sound_PhonationGrid_spectralTilt_inplace (thee.get(), me);
		}
		if (pp -> aspiration) {
			autoSound aspiration = PhonationGrid_to_Sound_aspiration (me, samplingFrequency);
			if (thee)
				_Sounds_add_inplace (thee.get(), aspiration.get());
			else
				thee = aspiration.move();
		}
		if (! thee)
			thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}

static void formantsAmplitudes_create (OrderedOf<structIntensityTier>* me, double tmin, double tmax, integer numberOfFormants) {
	try {
		for (integer i = 1; i <= numberOfFormants; i ++) {
			autoIntensityTier a = IntensityTier_create (tmin, tmax);
			me -> addItem_move (a.move());
		}
	} catch (MelderError) {
		Melder_throw (U"No formants amplitudes created.");
	};
}

/********************** VocalTractGridPlayOptions **********************/

Thing_implement (VocalTractGridPlayOptions, Daata, 0);

static void VocalTractGridPlayOptions_setDefaults (VocalTractGridPlayOptions me, VocalTractGrid thee) {
	my filterModel = kKlattGridFilterModel::CASCADE;
	my endOralFormant = std::min (thy oral_formants -> formants.size, thy oral_formants -> bandwidths.size);
	my startOralFormant = 1;
	my endNasalFormant = std::min (thy nasal_formants -> formants.size, thy nasal_formants -> bandwidths.size);
	my startNasalFormant = 1;
	my endNasalAntiFormant = std::min (thy nasal_antiformants -> formants.size, thy nasal_antiformants -> bandwidths.size);
	my startNasalAntiFormant = 1;
}

autoVocalTractGridPlayOptions VocalTractGridPlayOptions_create () {
	try {
		autoVocalTractGridPlayOptions me = Thing_new (VocalTractGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw (U"VocalTractGridPlayOptions not created.");
	}
}

/********************** VocalTractGrid ***************************************/

static integer FormantGrid_getNumberOfFormantPoints (FormantGrid me, integer iformant) {
	if (iformant < 1 || iformant > my formants.size)
		return -1;
	const RealTier f = my formants.at [iformant];
	return f -> points.size;
}

static integer FormantGrid_getNumberOfBandwidthPoints (FormantGrid me, integer iformant) {
	if (iformant < 1 || iformant > my bandwidths.size)
		return -1;
	const RealTier b = my bandwidths.at [iformant];
	return b -> points.size;
}

static integer Ordered_getNumberOfAmplitudePoints (OrderedOf<structIntensityTier>* me, integer iformant) {
	if (! me || iformant < 1 || iformant > my size)
		return -1;
	const RealTier t = my at [iformant];
	return t -> points.size;
}

static void FormantGrid_info (FormantGrid me, OrderedOf<structIntensityTier>* amplitudes, conststring32 in1, conststring32 in2) {
	const integer nformants = my formants.size;
	const integer namplitudes = ( amplitudes ? amplitudes->size : 0 );
	const integer nmax = std::max (nformants, namplitudes);

	for (integer iformant = 1; iformant <= nmax; iformant ++) {
		MelderInfo_writeLine (in1, U"Formant ", iformant, U":");
		if (iformant <= my formants.size) {
			const integer nfp = FormantGrid_getNumberOfFormantPoints (me, iformant);
			const integer nbp = FormantGrid_getNumberOfBandwidthPoints (me, iformant);
			MelderInfo_writeLine (in2, U"formants:   ", ( nfp >= 0 ? Melder_integer (nfp) : U"--undefined--" ));
			MelderInfo_writeLine (in2, U"bandwidths: ", ( nbp >= 0 ? Melder_integer (nbp) : U"--undefined--" ));
		}
		if (amplitudes) {
			const integer nap = Ordered_getNumberOfAmplitudePoints (amplitudes, iformant);
			MelderInfo_writeLine (in2, U"amplitudes: ", ( nap >= 0 ? Melder_integer (nap) : U"--undefined--" ));
		}
	}
}

void structVocalTractGrid :: v_info () {
	our structDaata :: v_info ();
	const conststring32 in1 = U"  ", in2 = U"    ", in3 = U"      ";
	MelderInfo_writeLine (in1, U"Time domain:");
	MelderInfo_writeLine (in2, U"Start time:     ", our xmin, U" seconds");
	MelderInfo_writeLine (in2, U"End time:       ", our xmax, U" seconds");
	MelderInfo_writeLine (in2, U"Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (in1, U"\nNumber of points in the ORAL FORMANT tiers:");
	FormantGrid_info (our oral_formants.get(), & our oral_formants_amplitudes, in2, in3);
	MelderInfo_writeLine (in1, U"\nNumber of points in the NASAL FORMANT tiers:");
	FormantGrid_info (our nasal_formants.get(), & our nasal_formants_amplitudes, in2, in3);
	MelderInfo_writeLine (in1, U"\nNumber of points in the NASAL ANTIFORMANT tiers:");
	FormantGrid_info (our nasal_antiformants.get(), nullptr, in2, in3);
}

Thing_implement (VocalTractGrid, Function, 0);

void VocalTractGrid_setNames (VocalTractGrid me) {
	Thing_setName (my oral_formants.get(), U"oral_formants");
	Thing_setName (my nasal_formants.get(), U"nasal_formants");
	Thing_setName (my nasal_antiformants.get(), U"nasal_antiformants");
	//Thing_setName (my oral_formants_amplitudes.get(), U"oral_formants_amplitudes");
	//Thing_setName (my nasal_formants_amplitudes.get(), U"nasal_formants_amplitudes");
}

autoVocalTractGrid VocalTractGrid_create (double tmin, double tmax, integer numberOfFormants, integer numberOfNasalFormants, integer numberOfNasalAntiFormants) {
	try {
		autoVocalTractGrid me = Thing_new (VocalTractGrid);
		Function_init (me.get(), tmin, tmax);
		my oral_formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants);
		my nasal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalFormants);
		my nasal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalAntiFormants);
		formantsAmplitudes_create (& my oral_formants_amplitudes, tmin, tmax, numberOfFormants);
		formantsAmplitudes_create (& my nasal_formants_amplitudes, tmin, tmax, numberOfNasalFormants);
		my options = VocalTractGridPlayOptions_create ();
		VocalTractGrid_setNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"VocalTractGrid not created.");
	}
}

static void VocalTractGrid_CouplingGrid_drawCascade_inplace (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double *out_yin, double *out_yout) {
	const integer numberOfOralFormants = my oral_formants -> formants.size;
	const integer numberOfNasalFormants = my nasal_formants -> formants.size;
	const integer numberOfNasalAntiFormants = my nasal_antiformants -> formants.size;
	const integer numberOfTrachealFormants = ( thee ? thy tracheal_formants -> formants.size : 0 );
	const integer numberOfTrachealAntiFormants = ( thee ? thy tracheal_antiformants -> formants.size : 0 );
	const double y1 = ymin, y2 = ymax, ddx = 0.2, ymid = (y1 + y2) / 2.0;
	const conststring32 text [6] = { 0, U"TF", U"TAF", U"NF", U"NAF", U""};
	const integer nf [6] = { 0, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfNasalFormants, numberOfNasalAntiFormants, numberOfOralFormants };
	constexpr integer numberOfXSections = 5;
	integer nsx = 0;
	autoMelderString ff, fb;

	const integer numberOfFilters = numberOfNasalFormants + numberOfNasalAntiFormants + numberOfTrachealFormants + numberOfTrachealAntiFormants + numberOfOralFormants;
	const double dx = (xmax - xmin) / (numberOfFilters + (nsx - 1) * ddx);

	double x1, x2;
	if (numberOfFilters == 0) {
		x2 = xmax;
		Graphics_line (g, xmin, ymid, x2, ymid);
		goto end;
	}

	for (integer isection = 1; isection <= numberOfXSections; isection ++)
		if (nf [isection] > 0)
			nsx ++;

	x1 = xmin;
	for (integer isection = 1; isection <= numberOfXSections; isection ++) {
		const integer numberOfFormants = nf [isection];

		if (numberOfFormants == 0)
			continue;

		x2 = x1 + dx;
		for (integer i = 1; i <= numberOfFormants; i ++) {
			MelderString_copy (& ff, U"F", i);
			MelderString_copy (& fb, U"B", i);
			draw_oneSection (g, x1, x2, y1, y2, text [isection], ff.string, fb.string);

			if (i < numberOfFormants) {
				x1 = x2;
				x2 = x1 + dx;
			}
		}

		if (isection < numberOfXSections) {
			x1 = x2;
			x2 = x1 + ddx * dx;
			Graphics_line (g, x1, ymid, x2, ymid);
			x1 = x2;
		}
	}
end:
	if (out_yin)
		*out_yin = ymid;
	if (out_yout)
		*out_yout = ymid;
}

static void VocalTractGrid_CouplingGrid_drawParallel_inplace (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *out_yin, double *out_yout) {
	// (0: filler) (1: hor. line to split) (2: split to diff) (3: diff) (4: diff to split)
	// (5: split to filter) (6: filters) (7: conn to summer) (8: summer)
	double xw [9] = { 0.0, 0.3, 0.2, 1.5, 0.5, 0.5, 1.0, 0.5, 0.5 };
	const integer numberOfXSections = 8, numberOfYSections = 4;
	const integer numberOfNasalFormants = my nasal_formants -> formants.size;
	const integer numberOfOralFormants = my oral_formants -> formants.size;
	const integer numberOfTrachealFormants = ( thee ? thy tracheal_formants -> formants.size : 0 );
	const integer numberOfFormants = numberOfNasalFormants + numberOfOralFormants + numberOfTrachealFormants;
	const integer numberOfUpperPartFormants = numberOfNasalFormants + ( numberOfOralFormants > 0 ? 1 : 0 );
	const integer numberOfLowerPartFormants = numberOfFormants - numberOfUpperPartFormants;
	const conststring32 text [5] = { nullptr, U"Nasal", U"", U"", U"Tracheal" };
	const integer nffrom [5] = { 0, 1, 1, 2, 1 };
	const integer nfto [5] = { 0, numberOfNasalFormants, ( numberOfOralFormants > 0 ? 1 : 0 ), numberOfOralFormants, numberOfTrachealFormants };
	autoMelderString fba;
	double xws [9];
	rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

	double y1, y2;
	if (numberOfFormants == 0) {
		y1 = y2 = (ymin + ymax) / 2.0;
		Graphics_line (g, xmin, y1, xmax, y1);
		if (out_yin)
			*out_yin = y1;
		if (out_yout)
			*out_yout = y2;
		return;
	}

	const double ddy = ( dy < 0 ? 0.0 : dy);
	dy = (ymax - ymin) / (numberOfFormants * (1.0 + ddy) - ddy);

	const connections local_in = connections_create (numberOfFormants);
	const connections local_out = connections_create (numberOfFormants);

	// parallel section
	double x1 = xmin + xws [5];
	double x2 = x1 + xw [6];
	y2 = ymax;
	double x3 = xmin + xws [4];
	integer ic = 0;
	for (integer isection = 1; isection <= numberOfYSections; isection ++) {
		const integer ifrom = nffrom [isection], ito = nfto [isection];
		if (ito < ifrom)
			continue;
		for (integer i = ifrom; i <= ito; i ++) {
			y1 = y2 - dy;
			const double ymid = (y1 + y2) / 2.0;
			const conststring32 fi = Melder_integer (i);
			MelderString_copy (& fba, U"A", fi, U" F", fi, U" B", fi);
			draw_oneSection (g, x1, x2, y1, y2, text [isection], fba.string, nullptr);
			Graphics_line (g, x3, ymid, x1, ymid); // to the left
			ic ++;
			local_in -> x [ic] = x3;
			local_out -> x [ic] = x2;
			local_in -> y [ic] = local_out -> y [ic] = ymid;
			y2 = y1 - 0.5 * dy;
		}
	}

	ic = 0;
	x1 = local_in -> y [1];
	if (numberOfUpperPartFormants > 0) {
		x1 = local_in -> x [numberOfUpperPartFormants];
		y1 = local_in -> y [numberOfUpperPartFormants];
		if (numberOfUpperPartFormants > 1)
			Graphics_line (g, x1, y1, local_in -> x [1], local_in -> y [1]);    // vertical
		x2 = xmin;
		if (numberOfLowerPartFormants > 0)
			x2 += xw [1];
		Graphics_line (g, x1, y1, x2, y1); // done
	}
	if (numberOfLowerPartFormants > 0) {
		const integer ifrom = numberOfUpperPartFormants + 1;
		x1 = local_in -> x [ifrom];
		y1 = local_in -> y [ifrom];   // at the split
		if (numberOfLowerPartFormants > 1)
			Graphics_line (g, x1, y1, local_in -> x [numberOfFormants], local_in -> y [numberOfFormants]);    // vertical
		x2 = xmin + xws [3]; // right of diff
		Graphics_line (g, x1, y1, x2, y1); // from vertical to diff
		x1 = xmin + xws [2]; // left of diff
		draw_oneSection (g, x1, x2, y1 + 0.5 * dy, y1 - 0.5 * dy, U"Pre-emphasis", nullptr, nullptr);
		x2 = x1;
		if (numberOfUpperPartFormants > 0) {
			x2 = xmin + xw [1];
			y2 = y1;   // at split
			Graphics_line (g, x1, y1, x2, y1); // to split
			y1 += (1 + ddy) * dy;
			Graphics_line (g, x2, y2, x2, y1); // vertical
			y1 -= 0.5 * (1.0 + ddy) * dy;
		}
		Graphics_line (g, xmin, y1, x2, y1);
	}

	const double r = xw [8] / 2.0;
	x2 = xmax - r;
	y2 = (ymin + ymax) / 2.0;

	alternatingSummer_drawConnections (g, x2, y2, r, local_out, true, 0.4);

	connections_free (local_out);
	connections_free (local_in);

	if (out_yin)
		*out_yin = y1;
	if (out_yout)
		*out_yout = y2;
}

static void VocalTractGrid_CouplingGrid_draw_inside (VocalTractGrid me, CouplingGrid thee, Graphics g, kKlattGridFilterModel filterModel, double xmin, double xmax, double ymin, double ymax, double dy, double *out_yin, double *out_yout) {
	if (filterModel == kKlattGridFilterModel::CASCADE)
		VocalTractGrid_CouplingGrid_drawCascade_inplace (me, thee, g, xmin, xmax, ymin, ymax, out_yin, out_yout);
	else if (filterModel == kKlattGridFilterModel::PARALLEL)
		VocalTractGrid_CouplingGrid_drawParallel_inplace (me, thee, g, xmin, xmax, ymin, ymax, dy, out_yin, out_yout);
	else
		;// Not valid
}

static void VocalTractGrid_CouplingGrid_draw (VocalTractGrid me, CouplingGrid thee, Graphics g, kKlattGridFilterModel filterModel) {
	const double xmin = 0.0, xmin1 = 0.05, xmax1 = 0.95, xmax = 1.0, ymin = 0.0, ymax = 1.0, dy = 0.5;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);
	double yin, yout;
	VocalTractGrid_CouplingGrid_draw_inside (me, thee, g, filterModel, xmin1, xmax1, ymin, ymax, dy, & yin, & yout);
	Graphics_line (g, xmin, yin, xmin1, yin);
	Graphics_arrow (g, xmax1, yout, xmax, yout);
	Graphics_unsetInner (g);
}

static autoSound Sound_VocalTractGrid_CouplingGrid_filter_cascade (Sound me, VocalTractGrid thee, CouplingGrid coupling) {
	try {
		const VocalTractGridPlayOptions pv = thy options.get();
		const CouplingGridPlayOptions pc = coupling -> options.get();
		const bool useOpenGlottisInfo = pc -> openglottis && coupling && coupling -> glottis && coupling -> glottis -> points.size > 0;
		const FormantGrid oral_formants = thy oral_formants.get();
		const FormantGrid nasal_formants = thy nasal_formants.get();
		const FormantGrid nasal_antiformants = thy nasal_antiformants.get();
		const FormantGrid tracheal_formants = coupling -> tracheal_formants.get();
		const FormantGrid tracheal_antiformants = coupling -> tracheal_antiformants.get();

		const integer numberOfFormants = oral_formants -> formants.size;
		const integer numberOfTrachealFormants = tracheal_formants -> formants.size;
		const integer numberOfTrachealAntiFormants = tracheal_antiformants -> formants.size;
		const integer numberOfNasalFormants = nasal_formants -> formants.size;
		const integer numberOfNasalAntiFormants = nasal_antiformants -> formants.size;
		check_formants (numberOfFormants, & pv -> startOralFormant, & pv -> endOralFormant);
		check_formants (numberOfNasalFormants, & pv -> startNasalFormant, & pv -> endNasalFormant);
		check_formants (numberOfTrachealFormants, & pc -> startTrachealFormant, & pc -> endTrachealFormant);
		check_formants (numberOfNasalAntiFormants, & pv -> startNasalAntiFormant, & pv -> endNasalAntiFormant);
		check_formants (numberOfTrachealAntiFormants, & pc -> startTrachealAntiFormant, & pc -> endTrachealAntiFormant);

		autoSound him = Data_copy (me);

		autoFormantGrid formants;
		if (useOpenGlottisInfo) {
			formants = Data_copy (thy oral_formants.get());
			FormantGrid_CouplingGrid_updateOpenPhases (formants.get(), coupling);
		}

		integer nasal_formant_warning = 0, any_warning = 0;
		if (pv -> endNasalFormant > 0) {   // nasal formants
			for (integer iformant = pv -> startNasalFormant; iformant <= pv -> endNasalFormant; iformant ++) {
				if (FormantGrid_isFormantDefined (thy nasal_formants.get(), iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inplace (him.get(), thy nasal_formants.get(), iformant, false);
				} else {
					// Melder_warning ("Nasal formant", iformant, ": frequency and/or bandwidth missing.");
					nasal_formant_warning ++;
					any_warning ++;
				}
			}
		}

		integer nasal_antiformant_warning = 0;
		if (pv -> endNasalAntiFormant > 0) {   // nasal antiformants
			for (integer iformant = pv -> startNasalAntiFormant; iformant <= pv -> endNasalAntiFormant; iformant ++) {
				if (FormantGrid_isFormantDefined (thy nasal_antiformants.get(), iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inplace (him.get(), thy nasal_antiformants.get(), iformant, true);
				} else {
					// Melder_warning ("Nasal antiformant", iformant, ": frequency and/or bandwidth missing.");
					nasal_antiformant_warning ++;
					any_warning ++;
				}
			}
		}

		integer tracheal_formant_warning = 0;
		if (pc -> endTrachealFormant > 0) {   // tracheal formants
			for (integer iformant = pc -> startTrachealFormant; iformant <= pc -> endTrachealFormant; iformant ++) {
				if (FormantGrid_isFormantDefined (tracheal_formants, iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inplace (him.get(), tracheal_formants, iformant, false);
				} else {
					// Melder_warning ("Tracheal formant", iformant, ": frequency and/or bandwidth missing.");
					tracheal_formant_warning ++;
					any_warning ++;
				}
			}
		}

		integer tracheal_antiformant_warning = 0;
		if (pc -> endTrachealAntiFormant > 0) {   // tracheal antiformants
			for (integer iformant = pc -> startTrachealAntiFormant; iformant <= pc -> endTrachealAntiFormant; iformant ++) {
				if (FormantGrid_isFormantDefined (tracheal_antiformants, iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inplace (him.get(), tracheal_antiformants, iformant, true);
				} else {
					// Melder_warning ("Tracheal antiformant", iformant, ": frequency and/or bandwidth missing.");
					tracheal_antiformant_warning ++;
					any_warning ++;
				}
			}
		}

		integer oral_formant_warning = 0;
		if (pv -> endOralFormant > 0) {   // oral formants
			if (! formants)
				formants = Data_copy (thy oral_formants.get());

			for (integer iformant = pv -> startOralFormant; iformant <= pv -> endOralFormant; iformant ++) {
				if (FormantGrid_isFormantDefined (formants.get(), iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inplace (him.get(), formants.get(), iformant, false);
				} else {
					// Melder_warning ("Oral formant", iformant, ": frequency and/or bandwidth missing.");
					oral_formant_warning ++;
					any_warning ++;
				}
			}
		}
		if (any_warning > 0)
		{
			autoMelderString warning;
			if (nasal_formant_warning > 0)
				MelderString_append (& warning, U"\tNasal formants: one or more are missing.\n");
			if (nasal_antiformant_warning)
				MelderString_append (& warning, U"\tNasal antiformants: one or more are missing.\n");
			if (tracheal_formant_warning)
				MelderString_append (& warning, U"\tTracheal formants: one or more are missing.\n");
			if (tracheal_antiformant_warning)
				MelderString_append (& warning, U"\tTracheal antiformants: one or more are missing.\n");
			if (oral_formant_warning)
				MelderString_append (& warning, U"\tOral formants: one or more are missing.\n");
			MelderInfo_write (U"\nWarning:\n", warning.string);
			MelderInfo_drain ();
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered by vocaltract and coupling grid.");
	}
}

static autoSound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling) {
	try {
		const VocalTractGridPlayOptions pv = thy options.get();
		const CouplingGridPlayOptions pc = coupling -> options.get();
		autoSound him;
		FormantGrid oral_formants = thy oral_formants.get();
		autoFormantGrid aof;
		int alternatingSign = 0; // 0: no alternating signs in parallel adding of filter outputs, 1/-1 start sign
		const bool useOpenGlottisInfo = pc -> openglottis && coupling -> glottis && coupling -> glottis -> points.size > 0;
		int scale = 1;
		const integer numberOfFormants = thy oral_formants -> formants.size;
		const integer numberOfNasalFormants = thy nasal_formants -> formants.size;
		const integer numberOfTrachealFormants = coupling -> tracheal_formants -> formants.size;

		check_formants (numberOfFormants, & (pv -> startOralFormant), & (pv -> endOralFormant));
		check_formants (numberOfNasalFormants, & (pv -> startNasalFormant), & (pv -> endNasalFormant));
		check_formants (numberOfTrachealFormants, & (pc -> startTrachealFormant), & (pc -> endTrachealFormant));

		if (useOpenGlottisInfo) {
			aof = Data_copy (thy oral_formants.get());
			oral_formants = aof.get();
			FormantGrid_CouplingGrid_updateOpenPhases (oral_formants, coupling);
		}

		if (pv -> endOralFormant > 0) {
			if (pv -> startOralFormant == 1) {
				him = Data_copy (me);
				if (oral_formants -> formants.size > 0)
					Sound_FormantGrid_Intensities_filterWithOneFormant_inplace (him.get(), oral_formants, & thy oral_formants_amplitudes, 1);
			}
		}

		if (pv -> endNasalFormant > 0) {
			alternatingSign = 0;
			autoSound nasal = Sound_FormantGrid_Intensities_filter (me, thy nasal_formants.get(), & thy nasal_formants_amplitudes, pv -> startNasalFormant, pv -> endNasalFormant, alternatingSign);

			if (! him)
				him = Data_copy (nasal.get());
			else
				_Sounds_add_inplace (him.get(), nasal.get());
		}

		// Formants 2 and up, with alternating signs.
		// We perform pre-emphasis by differentiating.
		// Klatt (1980) states that a first difference for the higher formants is necessary to remove low-frequency
		// energy from them. This energy would otherwise distort the spectrum in the region of F1 during the synthesis
		// of some vowels.

		autoSound me_diff = _Sound_diff (me, scale);

		if (pv -> endOralFormant >= 2) {
			const integer startOralFormant2 = ( pv -> startOralFormant > 2 ? pv -> startOralFormant : 2 );
			alternatingSign = ( startOralFormant2 % 2 == 0 ? -1 : 1 );   // 2 starts with negative sign
			if (startOralFormant2 <= oral_formants -> formants.size) {
				autoSound vocalTract = Sound_FormantGrid_Intensities_filter (me_diff.get(), oral_formants, & thy oral_formants_amplitudes, startOralFormant2, pv -> endOralFormant, alternatingSign);

				if (! him)
					him = Data_copy (vocalTract.get());
				else
					_Sounds_add_inplace (him.get(), vocalTract.get());
			}
		}

		if (pc -> endTrachealFormant > 0) {   // tracheal formants
			alternatingSign = 0;
			autoSound trachea = Sound_FormantGrid_Intensities_filter (me_diff.get(), coupling -> tracheal_formants.get(),
				& coupling -> tracheal_formants_amplitudes,
				pc -> startTrachealFormant, pc -> endTrachealFormant, alternatingSign);

			if (! him)
				him = Data_copy (trachea.get());
			else
				_Sounds_add_inplace (him.get(), trachea.get());
		}

		if (! him)
			him = Data_copy (me);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered in parallel.");
	}
}

autoSound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling) {
	return thy options -> filterModel == kKlattGridFilterModel::CASCADE ?
	       Sound_VocalTractGrid_CouplingGrid_filter_cascade (me, thee, coupling) :
	       Sound_VocalTractGrid_CouplingGrid_filter_parallel (me, thee, coupling);
}

/********************** CouplingGridPlayOptions **********************/

Thing_implement (CouplingGridPlayOptions, Daata, 0);

static void CouplingGridPlayOptions_setDefaults (CouplingGridPlayOptions me, CouplingGrid thee) {
	my fadeFraction = 0.1;
	my openglottis = 1;
	my endTrachealFormant = std::min (thy tracheal_formants -> formants.size, thy tracheal_formants -> bandwidths.size);
	my startTrachealFormant = 1;
	my endTrachealAntiFormant = std::min (thy tracheal_antiformants -> formants.size, thy tracheal_antiformants -> bandwidths.size);
	my startTrachealAntiFormant = 1;
	my startDeltaFormant = 1;
	my endDeltaFormant = thy delta_formants -> formants.size;
	my startDeltaBandwidth = 1;
	my endDeltaBandwidth = thy delta_formants -> bandwidths.size;
}

autoCouplingGridPlayOptions CouplingGridPlayOptions_create () {
	try {
		autoCouplingGridPlayOptions me = Thing_new (CouplingGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CouplingGridPlayOptions not created.");
	}
}

/********************** CouplingGrid *************************************/

Thing_implement (CouplingGrid, Function, 0);

void structCouplingGrid :: v_info () {
	structDaata :: v_info ();
	conststring32 in1 = U"  ", in2 = U"    ", in3 = U"      ";
	MelderInfo_writeLine (in1, U"Time domain:");
	MelderInfo_writeLine (in2, U"Start time:     ", xmin, U" seconds");
	MelderInfo_writeLine (in2, U"End time:       ", xmax, U" seconds");
	MelderInfo_writeLine (in2, U"Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (in1, U"\nNumber of points in the TRACHEAL FORMANT tiers:");
	FormantGrid_info (our tracheal_formants.get(), & tracheal_formants_amplitudes, in2, in3);
	MelderInfo_writeLine (in1, U"\nNumber of points in the TRACHEAL ANTIFORMANT tiers:");
	FormantGrid_info (our tracheal_antiformants.get(), nullptr, in2, in3);
	MelderInfo_writeLine (in1, U"\nNumber of points in the DELTA FORMANT tiers:");
	FormantGrid_info (our delta_formants.get(), nullptr, in2, in3);
}

void CouplingGrid_setNames (CouplingGrid me) {
	Thing_setName (my tracheal_formants.get(), U"tracheal_formants");
	Thing_setName (my tracheal_antiformants.get(), U"tracheal_antiformants");
	//Thing_setName (my tracheal_formants_amplitudes.get(), U"tracheal_formants_amplitudes");
	Thing_setName (my delta_formants.get(), U"delta_formants");
	Thing_setName (my glottis.get(), U"glottis");
}

autoCouplingGrid CouplingGrid_create (double tmin, double tmax, integer numberOfTrachealFormants, integer numberOfTrachealAntiFormants, integer numberOfDeltaFormants) {
	try {
		autoCouplingGrid me = Thing_new (CouplingGrid);
		Function_init (me.get(), tmin, tmax);
		my tracheal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealFormants);
		my tracheal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealAntiFormants);
		formantsAmplitudes_create (& my tracheal_formants_amplitudes, tmin, tmax, numberOfTrachealFormants);
		my delta_formants = FormantGrid_createEmpty (tmin, tmax, numberOfDeltaFormants);
		my glottis = PhonationTier_create (tmin, tmax);
		my options = CouplingGridPlayOptions_create ();
		CouplingGrid_setNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"CouplingGrid not created.");
	}
}

/********************** FormantGrid & CouplingGrid *************************************/

void FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee) {
	try {
		CouplingGridPlayOptions pc = thy options.get();
		for (integer itier = 1; itier <= thy delta_formants -> formants.size; itier ++) {
			RealTier delta = thy delta_formants -> formants.at [itier];
			if (itier <= my formants.size) {
				if (delta -> points.size > 0) {
					autoRealTier rt = RealTier_updateWithDelta (my formants.at [itier], delta, thy glottis.get(), pc -> fadeFraction);
					Melder_require (RealTier_valuesInRange (rt.get(), 0, undefined),
						U"Formant ", itier, U" coupling should not give negative values.");
					
					my formants. replaceItem_move (rt.move(), itier);
				}
			}
			delta = thy delta_formants -> bandwidths.at [itier];
			if (itier <= my bandwidths.size) {
				if (delta -> points.size > 0) {
					autoRealTier rt = RealTier_updateWithDelta (my bandwidths.at [itier], delta, thy glottis.get(), pc -> fadeFraction);
					Melder_require (RealTier_valuesInRange (rt.get(), 0, undefined),
						U"Bandwidth ", itier, U" coupling gives negative values.");
					my bandwidths. replaceItem_move (rt.move(), itier);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not updated with open hase information.");
	}
}

/********************** FricationGridPlayOptions **********************/

Thing_implement (FricationGridPlayOptions, Daata, 0);

static void FricationGridPlayOptions_setDefaults (FricationGridPlayOptions me, FricationGrid thee) {
	my endFricationFormant = std::min (thy frication_formants -> formants.size, thy frication_formants -> bandwidths.size);
	my startFricationFormant = 2;
	my bypass = 1;
}

autoFricationGridPlayOptions FricationGridPlayOptions_create () {
	try {
		autoFricationGridPlayOptions me = Thing_new (FricationGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FricationGridPlayOptions not created.");
	}
}

/************************ FricationGrid (& Sound) *********************************************/

void structFricationGrid :: v_info () {
	structDaata :: v_info ();
	const static char32 *in1 = U"  ", *in2 = U"    ", *in3 = U"      ";
	MelderInfo_writeLine (in1, U"Time domain:");
	MelderInfo_writeLine (in2, U"Start time:     ", xmin, U" seconds");
	MelderInfo_writeLine (in2, U"End time:       ", xmax, U" seconds");
	MelderInfo_writeLine (in2, U"Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (in1, U"\nNumber of points in the FRICATION tiers:");
	MelderInfo_writeLine (in2, U"fricationAmplitude:  ", fricationAmplitude -> points.size);
	MelderInfo_writeLine (in2, U"bypass:              ", bypass -> points.size);
	MelderInfo_writeLine (in1, U"\nNumber of points in the FRICATION FORMANT tiers:");
	FormantGrid_info (our frication_formants.get(), & our frication_formants_amplitudes, in2, in3);
}

Thing_implement (FricationGrid, Function, 0);

void FricationGrid_setNames (FricationGrid me) {
	Thing_setName (my fricationAmplitude.get(), U"fricationAmplitude");
	Thing_setName (my frication_formants.get(), U"frication_formants");
	Thing_setName (my bypass.get(), U"bypass");
	//Thing_setName (my frication_formants_amplitudes.get(), U"frication_formants_amplitudes");
}

autoFricationGrid FricationGrid_create (double tmin, double tmax, integer numberOfFormants) {
	try {
		autoFricationGrid me = Thing_new (FricationGrid);
		Function_init (me.get(), tmin, tmax);
		my fricationAmplitude = IntensityTier_create (tmin, tmax);
		my frication_formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants);
		my bypass = IntensityTier_create (tmin, tmax);
		formantsAmplitudes_create (& my frication_formants_amplitudes, tmin, tmax, numberOfFormants);
		my options = FricationGridPlayOptions_create ();
		FricationGrid_setNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"FricationGrid not created.");
	}
}

static void FricationGrid_draw_inside (FricationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout) {
	constexpr integer numberOfXSections = 5;
	const integer numberOfFormants = my frication_formants -> formants.size;
	const integer numberOfParts = numberOfFormants + ( numberOfFormants > 1 ? 0 : 1 ) ;   // 2..number + bypass
	// dum noise, connections, filter, connections, adder
	double xw [6] = { 0.0, 2, 0.6, 1.5, 0.6, 0.5 }, xws [6];
	double r, x1, y1, x2, y2, x3, xs, ys, ymid = (ymin + ymax) / 2.0;

	rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

	dy = std::max (dy, 0.0);
	dy = (ymax - ymin) / (numberOfParts * (1.0 + dy) - dy);

	connections cp = connections_create (numberOfParts);
	if (cp == 0)
		return;

	// section 1
	x1 = xmin;
	x2 = x1 + xw [1];
	y1 = ymid - 0.5 * dy;
	y2 = y1 + dy;
	draw_oneSection (g, x1, x2, y1, y2, U"Frication", U"noise", nullptr);

	// section 2, horizontal line halfway, vertical line
	x1 = x2;
	x2 = x1 + xw [2] / 2.0;
	Graphics_line (g, x1, ymid, x2, ymid);
	Graphics_line (g, x2, ymax - dy / 2, x2, ymin + dy / 2.0);
	x3 = x2;
	// final connection to section 2 , filters , connections to adder
	x1 = xmin + xws [2];
	x2 = x1 + xw [3];
	y2 = ymax;
	autoMelderString fba;
	for (integer i = 1; i <= numberOfParts; i ++) {
		conststring32 fi = Melder_integer (i + 1);
		y1 = y2 - dy;
		if (i < numberOfParts)
			MelderString_copy (& fba, U"A", fi, U" F", fi, U" B", fi);
		else
			MelderString_copy (& fba,  U"Bypass");
		draw_oneSection (g, x1, x2, y1, y2, nullptr, fba.string, nullptr);
		double ymidi = (y1 + y2) / 2.0;
		Graphics_line (g, x3, ymidi, x1, ymidi); // from noise to filter
		cp -> x [i] = x2;
		cp -> y [i] = ymidi;
		y2 = y1 - 0.5 * dy;
	}

	r = xw [5] / 2.0;
	xs = xmax - r;
	ys = ymid;

	if (numberOfParts > 1)
		alternatingSummer_drawConnections (g, xs, ys, r, cp, 1, 0.4);
	else
		Graphics_line (g, cp -> x [1], cp -> y [1], xs + r, ys);

	connections_free (cp);

	if (yout)
		*yout = ys;
}

void FricationGrid_draw (FricationGrid me, Graphics g) {
	const double xmin = 0.0, xmax = 1.0, xmax2 = 0.9, ymin = 0.0, ymax = 1.0, dy = 0.5;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);

	double yout;
	FricationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, & yout);

	Graphics_arrow (g, xmax2, yout, xmax, yout);
	Graphics_unsetInner (g);
}

autoSound FricationGrid_to_Sound (FricationGrid me, double samplingFrequency) {
	try {
		autoSound thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);

		double lastval = 0.0;
		for (integer i = 1; i <= thy nx; i ++) {
			const double t = thy x1 + (i - 1) * thy dx;
			double val = NUMrandomUniform (-1.0, 1.0);
			double a = 0.0;
			if (my fricationAmplitude -> points.size > 0) {
				const double dba = RealTier_getValueAtTime (my fricationAmplitude.get(), t);
				a = ( isdefined (dba) ? DBSPL_to_A (dba) : 0.0 );
			}
			lastval = (val += 0.75 * lastval); // TODO: soft low-pass coefficient should be Fs dependent!
			thy z [1] [i] = val * a;
		}

		autoSound him = Sound_FricationGrid_filter (thee.get(), me);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no frication Sound created.");
	}
}

/************************ Sound & FricationGrid *********************************************/

autoSound Sound_FricationGrid_filter (Sound me, FricationGrid thee) {
	try {
		const FricationGridPlayOptions pf = thy options.get();
		autoSound him;
		const integer numberOfFricationFormants = thy frication_formants -> formants.size;

		check_formants (numberOfFricationFormants, & (pf -> startFricationFormant), & (pf -> endFricationFormant));

		if (pf -> endFricationFormant > 1) {
			const integer startFricationFormant2 = pf -> startFricationFormant > 2 ? pf -> startFricationFormant : 2;
			int alternatingSign = ( startFricationFormant2 % 2 == 0 ? 1 : -1 ); // 2 starts with positive sign
			him = Sound_FormantGrid_Intensities_filter (me, thy frication_formants.get(), & thy frication_formants_amplitudes, startFricationFormant2, pf -> endFricationFormant, alternatingSign);
		}

		if (! him)
			him = Data_copy (me);

		if (pf -> bypass) {
			for (integer is = 1; is <= his nx; is ++) {	// Bypass
				const double t = his x1 + (is - 1) * his dx;
				double ab = 0.0;
				if (thy bypass -> points.size > 0) {
					const double val = RealTier_getValueAtTime (thy bypass.get(), t);
					ab = ( isundef (val) ? 0.0 : DB_to_A (val) );
				}
				his z [1] [is] += my z [1] [is] * ab;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered by frication filter.");
	}
}

/********************** KlattGridPlayOptions **********************/

Thing_implement (KlattGridPlayOptions, Daata, 0);

static void KlattGridPlayOptions_setDefaults (KlattGridPlayOptions me, KlattGrid thee) {
	my samplingFrequency = 44100.0;
	my scalePeak = 1;
	my xmin = thy xmin;
	my xmax = thy xmax;
}

autoKlattGridPlayOptions KlattGridPlayOptions_create () {
	try {
		autoKlattGridPlayOptions me = Thing_new (KlattGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGridPlayOptions not created.");
	}
}

void KlattGrid_setDefaultPlayOptions (KlattGrid me) {
	KlattGridPlayOptions_setDefaults (my options.get(), me);
	PhonationGridPlayOptions_setDefaults (my phonation -> options.get());
	VocalTractGridPlayOptions_setDefaults (my vocalTract -> options.get(), my vocalTract.get());
	CouplingGridPlayOptions_setDefaults (my coupling -> options.get(), my coupling.get());
	FricationGridPlayOptions_setDefaults (my frication -> options.get(), my frication.get());
}

/************************ KlattGrid *********************************************/

Thing_implement (KlattGrid, Function, 0);

void structKlattGrid :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time:     ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time:       ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"\n--- PhonationGrid ---\n");
	phonation -> v_info ();
	MelderInfo_writeLine (U"\n--- VocalTractGrid ---\n");
	vocalTract -> v_info ();
	MelderInfo_writeLine (U"\n--- CouplingGrid ---\n");
	coupling -> v_info ();
	MelderInfo_writeLine (U"\n--- FricationGrid ---\n");
	frication -> v_info ();
}

void KlattGrid_setNames (KlattGrid me) {
	Thing_setName (my phonation.get(), U"phonation");
	Thing_setName (my vocalTract.get(), U"vocalTract");
	Thing_setName (my coupling.get(), U"coupling");
	Thing_setName (my frication.get(), U"frication");
	Thing_setName (my gain.get(), U"gain");
}

autoKlattGrid KlattGrid_create (double tmin, double tmax, integer numberOfFormants, integer numberOfNasalFormants, integer numberOfNasalAntiFormants, integer numberOfTrachealFormants, integer numberOfTrachealAntiFormants, integer numberOfFricationFormants, integer numberOfDeltaFormants) {
	try {
		autoKlattGrid me = Thing_new (KlattGrid);
		Function_init (me.get(), tmin, tmax);
		my phonation = PhonationGrid_create (tmin, tmax);
		my vocalTract = VocalTractGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants, numberOfNasalAntiFormants);
		my coupling = CouplingGrid_create (tmin, tmax, numberOfTrachealFormants,  numberOfTrachealAntiFormants, numberOfDeltaFormants);
		my frication = FricationGrid_create (tmin, tmax, numberOfFricationFormants);
		my gain = IntensityTier_create (tmin, tmax);
		my options = KlattGridPlayOptions_create ();

		KlattGrid_setDefaultPlayOptions (me.get());
		KlattGrid_setNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid not created.");
	}
}

autoKlattGrid KlattGrid_createExample () {
	try {
		autoKlattTable thee = KlattTable_createExample ();
		autoKlattGrid me = KlattTable_to_KlattGrid (thee.get(), 0.005);
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattGrid example not created.");
	};
}

// y is the height in units of the height of one section,
// y1 is the height from the top to the split between the uppper, non-diffed, and lower diffed part
static void _KlattGrid_queryParallelSplit (KlattGrid me, double dy, double *out_y, double *out_y1) {
	const integer ny = my vocalTract -> nasal_formants -> formants.size +
		my vocalTract -> oral_formants -> formants.size + my coupling -> tracheal_formants -> formants.size;
	const integer n1 = my vocalTract -> nasal_formants -> formants.size +
		( my vocalTract -> oral_formants -> formants.size > 0 ? 1 : 0 );

	const integer n2 = ny - n1;
	double y = 0.0, y1 = 0.0;
	if (ny != 0) {
		y = ny + (ny - 1) * dy;

		if (n1 == 0)
			y1 = 0.5;
		else if (n2 == 0)
			y1 = y - 0.5;
		else
			y1 = n1 + (n1 - 1) * dy + 0.5 * dy;
	}
	if (out_y)
		*out_y = y;
	if (out_y1)
		*out_y1 = y1;
	return;
}

static void getYpositions (double h1, double h2, double h3, double h4, double h5, double fractionOverlap, double *out_dy, double *out_ymin1, double *out_ymax1, double *out_ymin2, double *out_ymax2, double *out_ymin3, double *out_ymax3) {
	// Given: five 'blocks' with relative heights h1..h5 in arbitrary units.
	// Problem: scale all h1..h5 such that:
	// 1. blocks h1 and h2 form one unit, with h1 on top of h2, the quotient h1/h2 is fixed
	// 2. blocks h3 and h4 form one unit, with h3 on top of h4, the quotient h3/h4 is fixed
	// 3. blocks h1 and h3 have the same baseline.
	// 4. h5 is always underneath (h1,h2) but may partially overlap (0.45) with h4.
	// 5. After scaling the new h1+h2 >= 0.3
	// 6. Optimally use the vertical space from 0.. 1, i.e the top of h1 or h3 is at 1,
	// the bottom of h5 is at 0. Preferably scale all blocks with the same factor, if not possible than
	// scale h3,h4 and h5 the same
	//
	// h1  h3
	// h2  h4
	//  h5
	/* Cases:
	              x             x       ^
	     x      x x    x      x x       |
	  h1 x x    x x    x x    x x h3    | h13
	     -----------------------------------------------------------
	  h2 x x    x x    x x    x x h4
	     x      x      x x    x x
	                     x      x
	     x      x      x x    x x
	  h5 x      x      x      x
	     x      x      x      x
	*/
	double h; // h12_min = 0.3; not yet
	const double h13 = ( h1 > h3 ? h1 : h3 ); // baselines are now equal
	if (h2 >= h4) {
		h = h13 + h2 + h5;
	} else { // h2 < h4
		double maximumOverlap3 = fractionOverlap * h5;
		if (maximumOverlap3 < h1 + h2)
			maximumOverlap3 = 0.0;
		else if (maximumOverlap3 > h4 - h2)
			maximumOverlap3 = h4 - h2;
		h = h13 + h4 + h5 - maximumOverlap3;
	}
	const double dy = 1.0 / (1.1 * h);
	const double ymin1 = 1.0 - (h13 + h2) * dy;
	const double ymax1 = ymin1 + (h1 + h2) * dy;
	const double ymin2 = 1.0 - (h13 + h4) * dy;
	const double ymax2 = ymin2 + (h3 + h4) * dy;
	const double ymin3 = 0.0;
	const double ymax3 = h5 * dy;
	if (out_dy)
		*out_dy = dy;
	if (out_ymin1)
		*out_ymin1 = ymin1;
	if (out_ymax1)
		*out_ymax1 = ymax1;
	if (out_ymin2)
		*out_ymin2 = ymin2;
	if (out_ymax2)
		*out_ymax2 = ymax2;
	if (out_ymin3)
		*out_ymin3 = ymin3;
	if (out_ymax3)
		*out_ymax3 = ymax3;	
}

void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, kKlattGridFilterModel filterModel, int withTrachea) {
	VocalTractGrid_CouplingGrid_draw (my vocalTract.get(), withTrachea ? my coupling.get() : nullptr, g, filterModel);
}

void KlattGrid_draw (KlattGrid me, Graphics g, kKlattGridFilterModel filterModel) {
	const double xmin = 0.0, xmax2 = 0.90, xmax3 = 0.95, xmax = 1.0, ymin = 0.0, ymax = 1.0;
	const double dy_phonation = 0.5, dy_vocalTract_p = 0.5, dy_frication = 0.5;

	connections tf;
	try {
		tf = connections_create (2);
	} catch (MelderError) {
		Melder_clearError ();
		return;
	}

	Graphics_setInner (g);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);

	const integer nff = my frication -> frication_formants -> formants.size - 1 + 1;
	const double yh_frication = ( nff > 0 ? nff + (nff - 1) * dy_frication : 1.0 );
	const double yh_phonation = 1.0 + dy_phonation + 1.0;
	double yout_phonation, yout_frication;
	double height_phonation = 0.3;
	double dy = height_phonation / yh_phonation; // 1 vertical unit in source section height units

	double xs1, xs2, ys1, ys2, xf1, xf2, yf1, yf2;
	double xp1, xp2, yp1, yp2, xc1, xc2, yc1, yc2;
	double xws [6];
	double xw [6] = {0, 1.75, 0.125, 3.0, 0.25, 0.125 };
	rel_to_abs (xw, xws, 5, xmax2 - xmin);
	
	if (filterModel == kKlattGridFilterModel::CASCADE) { // Cascade section
		/*
			limit height of frication unit dy !
		*/
		height_phonation = yh_phonation / (yh_phonation + yh_frication);
		if (height_phonation < 0.3)
			height_phonation = 0.3;
		dy = height_phonation / yh_phonation;

		xs1 = xmin;
		xs2 = xs1 + xw [1];
		ys2 = ymax;
		ys1 = ys2 - height_phonation;
		PhonationGrid_draw_inside (my phonation.get(), g, xs1, xs2, ys1, ys2, dy_phonation, & yout_phonation);

		// units in cascade have same heigth as units in source part.

		xc1 = xmin + xws [2];
		xc2 = xc1 + xw [3];
		yc2 = yout_phonation + dy / 2.0;
		yc1 = yc2 - dy;
		double yin_vocalTract_c, yout_vocalTract_c;
		VocalTractGrid_CouplingGrid_drawCascade_inplace (my vocalTract.get(), my coupling.get(), g, xc1, xc2, yc1, yc2, & yin_vocalTract_c, & yout_vocalTract_c);

		tf -> x [1] = xc2;
		tf -> y [1] = yout_vocalTract_c;

		Graphics_line (g, xs2, yout_phonation, xc1, yin_vocalTract_c);

		xf1 = xmin + xws [2];
		xf2 = xf1 + xw [3];
		yf2 = ymax - height_phonation;
		yf1 = 0.0;

		FricationGrid_draw_inside (my frication.get(), g, xf1, xf2, yf1, yf2, dy_frication, &yout_frication);
	} else { // Parallel
		/*
			optimize the vertical space for source, parallel and frication
			source part is relatively fixed. let the number of vertical section units be the divisor
			connector line from source to parallel has to be horizontal
			determine y's of source and parallel section
		*/
		double yf_parallel, yh_parallel, yh_overlap = 0.3, yin_vocalTract_p, yout_vocalTract_p;
		_KlattGrid_queryParallelSplit (me, dy_vocalTract_p, &yh_parallel, & yf_parallel);
		if (yh_parallel == 0.0) {
			yh_parallel = yh_phonation;
			yf_parallel = yh_parallel / 2.0;
			yh_overlap = -0.1;
		}

		height_phonation = yh_phonation / (yh_phonation + yh_frication);
		if (height_phonation < 0.3) {
			height_phonation = 0.3;
		}
		//double yunit = (ymax - ymin) / (yh_parallel + (1 - yh_overlap) * yh_frication); // some overlap

		//double ycs = ymax - 0.5 * height_phonation; // source output connector
		//double ycp = ymax - yf_parallel * yunit; // parallel input connector
		//double ytrans_phonation = ycs > ycp ? ycp - ycs : 0;
		//double ytrans_parallel = ycp > ycs ? ycs - ycp : 0;

		// source, tract, frication
		xs1 = xmin;
		xs2 = xs1 + xw [1];

		const double h1 = yh_phonation / 2.0, h2 = h1, h3 = yf_parallel, h4 = yh_parallel - h3, h5 = yh_frication;
		getYpositions (h1, h2, h3, h4, h5, yh_overlap, & dy, & ys1, & ys2, & yp1, & yp2, & yf1, & yf2);

		PhonationGrid_draw_inside (my phonation.get(), g, xs1, xs2, ys1, ys2, dy_phonation, & yout_phonation);

		xp1 = xmin + xws [2];
		xp2 = xp1 + xw [3];
		VocalTractGrid_CouplingGrid_drawParallel_inplace (my vocalTract.get(), my coupling.get(), g, xp1, xp2, yp1, yp2, dy_vocalTract_p, &yin_vocalTract_p, &yout_vocalTract_p);

		tf -> x [1] = xp2;
		tf -> y [1] = yout_vocalTract_p;

		Graphics_line (g, xs2, yout_phonation, xp1, yin_vocalTract_p);

		xf1 = xmin /* + 0.5 * xws [1] */;
		xf2 = xf1 + 0.55 * (xw [2] + xws [3]);

		FricationGrid_draw_inside (my frication.get(), g, xf1, xf2, yf1, yf2, dy_frication, &yout_frication);
	}

	tf -> x [2] = xf2;
	tf -> y [2] = yout_frication;
	const double r = (xmax3 - xmax2) / 2.0;
	const double xs = xmax2 + r / 2.0;
	const double ys = (ymax - ymin) / 2.0;

	summer_drawConnections (g, xs, ys, r, tf, true, 0.6);

	Graphics_arrow (g, xs + r, ys, xmax, ys);

	Graphics_unsetInner (g);
	connections_free (tf);
}

/**** Query, Add, Remove, Extract Replace ********/

#define PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE(Name,name,tierType) \
double KlattGrid_get##Name##AtTime (KlattGrid me, double t) \
{ return RealTier_getValueAtTime (my phonation -> name.get(), t); } \
void KlattGrid_add##Name##Point (KlattGrid me, double t, double value) \
{ RealTier_addPoint (my phonation -> name.get(), t, value);} \
void KlattGrid_remove##Name##Points (KlattGrid me, double t1, double t2) \
{ AnyTier_removePointsBetween (my phonation -> name.get()->asAnyTier(), t1, t2); } \
auto##tierType KlattGrid_extract##Name##Tier (KlattGrid me) \
{ return Data_copy (my phonation -> name.get()); } \
void KlattGrid_replace##Name##Tier (KlattGrid me, tierType thee) \
{ try {\
	Melder_require (my xmin == thy xmin && my xmax == thy xmax, U"Domains should be equal"); \
	auto##tierType any = Data_copy (thee); \
	my phonation -> name = any.move(); \
	} catch (MelderError) { Melder_throw (me, U": tier not replaced."); } \
}

// Generate 55 functions
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (Pitch, pitch, PitchTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (VoicingAmplitude, voicingAmplitude, IntensityTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (Flutter, flutter, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (Power1, power1, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (Power2, power2, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (OpenPhase, openPhase, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (CollisionPhase, collisionPhase, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (DoublePulsing, doublePulsing, RealTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (SpectralTilt, spectralTilt, IntensityTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (AspirationAmplitude, aspirationAmplitude, IntensityTier)
PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE (BreathinessAmplitude, breathinessAmplitude, IntensityTier)

autoFormantGrid* KlattGrid_getAddressOfFormantGrid (KlattGrid me, kKlattGridFormantType formantType) {
	return formantType == kKlattGridFormantType::ORAL ? & my vocalTract -> oral_formants :
	       formantType == kKlattGridFormantType::NASAL ? & my vocalTract -> nasal_formants :
	       formantType == kKlattGridFormantType::FRICATION ? & my frication -> frication_formants :
	       formantType == kKlattGridFormantType::TRACHEAL ? & my coupling -> tracheal_formants :
	       formantType == kKlattGridFormantType::NASAL_ANTI ? & my vocalTract -> nasal_antiformants :
	       formantType == kKlattGridFormantType::TRACHEALANTI ? & my coupling -> tracheal_antiformants :
		   & my coupling -> delta_formants; // kKlattGridFormantType::Delta
}

OrderedOf<structIntensityTier>* KlattGrid_getAddressOfAmplitudes (KlattGrid me, kKlattGridFormantType formantType) {
	return formantType == kKlattGridFormantType::ORAL ? & my vocalTract -> oral_formants_amplitudes :
	       formantType == kKlattGridFormantType::NASAL ? & my vocalTract -> nasal_formants_amplitudes :
	       formantType == kKlattGridFormantType::FRICATION ? & my frication -> frication_formants_amplitudes :
	       formantType == kKlattGridFormantType::TRACHEAL ? & my coupling -> tracheal_formants_amplitudes : nullptr;
}

#define KlattGrid_QUERY_ADD_REMOVE(Name) \
double KlattGrid_get##Name##AtTime (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t) \
{ \
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	return FormantGrid_get##Name##AtTime (fg->get(), iformant, t); \
} \
void KlattGrid_add##Name##Point (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t, double value) \
{ \
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	FormantGrid_add##Name##Point (fg->get(), iformant, t, value); \
} \
void KlattGrid_remove##Name##Points (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t1, double t2) \
{ \
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	FormantGrid_remove##Name##PointsBetween (fg->get(), iformant, t1, t2); \
}

// 6 functions
KlattGrid_QUERY_ADD_REMOVE (Formant)
KlattGrid_QUERY_ADD_REMOVE (Bandwidth)

void KlattGrid_formula_frequencies (KlattGrid me, kKlattGridFormantType formantType, conststring32 expression, Interpreter interpreter) {
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_formula_frequencies (fg->get(), expression, interpreter, nullptr);
}

void KlattGrid_formula_bandwidths (KlattGrid me, kKlattGridFormantType formantType, conststring32 expression, Interpreter interpreter) {
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	
	FormantGrid_formula_bandwidths (fg->get(), expression, interpreter, nullptr);
}

void KlattGrid_formula_amplitudes (KlattGrid me, kKlattGridFormantType formantType, conststring32 expression, Interpreter interpreter) {
	try {
		const OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		for (integer irow = 1; irow <= ordered->size; irow ++) {
			const IntensityTier amplitudes = ordered->at [irow];
			Formula_compile (interpreter, amplitudes, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
			Formula_Result result;
			for (integer icol = 1; icol <= amplitudes -> points.size; icol ++) {
				Formula_run (irow, icol, & result);
				Melder_require (isdefined (result. numericResult),
					U"Cannot put an undefined value into the tier.\nFormula not finished.");
				amplitudes -> points.at [icol] -> value = result. numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not finished on amplitudes.");
	}
}

double KlattGrid_getAmplitudeAtTime (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t) {
	const OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 1 || iformant > ordered->size)
		return undefined;
	return RealTier_getValueAtTime (ordered->at [iformant], t);
}

void KlattGrid_addAmplitudePoint (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t, double value) {
	const OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	Melder_require (iformant > 0 && iformant <= ordered -> size,
		U"Formant amplitude tier ", iformant, U"does not exist.");
	RealTier_addPoint (ordered->at [iformant], t, value);
}

void KlattGrid_removeAmplitudePoints (KlattGrid me, kKlattGridFormantType formantType, integer iformant, double t1, double t2) {
	const OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (ordered && iformant > 0 && iformant <= ordered->size) {
		AnyTier_removePointsBetween (ordered->at [iformant]->asAnyTier(), t1, t2);
	}
}

autoIntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer iformant) {
	try {
		const OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		Melder_require (ordered != nullptr,
			U"This amplitude tier does not exist.");
		Melder_require (iformant > 0 && iformant <= ordered -> size,
			U"Formant amplitude tier ", iformant, U"does not exist.");
		autoIntensityTier thee = Data_copy (ordered->at [iformant]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no IntensityTier extracted.");
	}
}

void KlattGrid_replaceAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer iformant, IntensityTier thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"Domains should be equal.");
		OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		Melder_require (ordered != nullptr,
			U"This amplitude tier does not exist.");
		Melder_require (iformant > 0 && iformant <= ordered -> size,
			U"Formant amplitude tier ", iformant, U" does not exist.");
		autoIntensityTier any = Data_copy (thee);
		ordered -> replaceItem_move (any.move(), iformant);
	} catch (MelderError) {
		Melder_throw (me, U": no ampitude tier replaced.");
	}
}

autoFormantGrid KlattGrid_extractFormantGrid (KlattGrid me, kKlattGridFormantType formantType) {
	try {
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
		Melder_require ((*fg) -> formants . size > 0,
			KlattGrid_getFormantName (formantType), U"s are not defined.");
		autoFormantGrid thee = Data_copy (fg->get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no FormantGrid extracted.");
	}
}

void KlattGrid_replaceFormantGrid (KlattGrid me, kKlattGridFormantType formantType, FormantGrid thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"Domains should be equal");
		autoFormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
		*fg = Data_copy (thee);
	} catch (MelderError) {
		Melder_throw (me, U": no FormantGrid replaced.");
	}
}

void KlattGrid_addFormantAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer position) {
	try {
		Melder_require (formantType != kKlattGridFormantType::NASAL_ANTI && formantType != kKlattGridFormantType::TRACHEALANTI && formantType != kKlattGridFormantType::DELTA,
			U"Cannot add amplitude tier to this formant type.");
		OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		const integer noa = ordered->size;
		if (position > noa || position < 1)
			position = noa + 1;
		autoIntensityTier tier = IntensityTier_create (my xmin, my xmax);
		ordered -> addItemAtPosition_move (tier.move(), position);
	} catch (MelderError) {
		Melder_throw (me, U": no formant amplitude tier added.");
	}
}

void KlattGrid_removeFormantAmplitudeTier (KlattGrid me, kKlattGridFormantType formantType, integer position) {
	try {
		Melder_require (formantType != kKlattGridFormantType::NASAL_ANTI && formantType != kKlattGridFormantType::TRACHEALANTI && formantType != kKlattGridFormantType::DELTA, 
			U"Cannot remove amplitude tier from this formant type.");
		OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		if (position > 0 && position <= ordered->size) {
			ordered -> removeItem (position);
		}
	} catch (MelderError) {
		Melder_throw (me, U": no formant amplitude tier removed.");
	}
}

// The following two routines are deprecated.
// We do this in two separate steps now
void KlattGrid_addFormant (KlattGrid me, kKlattGridFormantType formantType, integer position) {
	try {
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
		
		const integer nof = (*fg) -> formants.size;
		if (position > nof || position < 1) {
			position = nof + 1;
		}

		if (formantType == kKlattGridFormantType::NASAL_ANTI || formantType == kKlattGridFormantType::TRACHEALANTI ||
			formantType == kKlattGridFormantType::DELTA) {
			FormantGrid_addFormantAndBandwidthTiers (fg->get(), position);
			return;
		}

		OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		const integer noa = ordered->size;
		Melder_require (nof == noa,
			U"The number of formants (",  nof, U") and the number of amplitudes (", noa, U") should be equal.");
		
		FormantGrid_addFormantAndBandwidthTiers (fg->get(), position);
		try {
			autoIntensityTier tier = IntensityTier_create (my xmin, my xmax);
			ordered -> addItemAtPosition_move (tier.move(), position);
		} catch (MelderError) { // restore original
			FormantGrid_removeFormantAndBandwidthTiers (fg->get(), position);
		}
	} catch (MelderError) {
		Melder_throw (me, U": no formant added.");
	}
}

void KlattGrid_removeFormant (KlattGrid me, kKlattGridFormantType formantType, integer position) {
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	const integer nof = (*fg) -> formants.size;
	if (formantType == kKlattGridFormantType::NASAL_ANTI || formantType == kKlattGridFormantType::TRACHEALANTI ||
        formantType == kKlattGridFormantType::DELTA) {
		if (position < 1 || position > nof) {
			return;
		}
		FormantGrid_removeFormantAndBandwidthTiers (fg->get(), position);
	} else { 
		// oral & nasal & tracheal formants can have amplitudes
		// only remove a formant and its amplitude tier if number of formants and amplitudes are the same
		OrderedOf<structIntensityTier>* ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		const integer noa = ordered->size;
		if (position < 1 || position > nof || position > noa) {
			if (nof != noa) {
				Melder_warning (U"The number of formant tiers (", nof, U") and the number of amplitude tiers (",
					noa, U") don't match. Nothing removed.");
			}
			return;
		}
		FormantGrid_removeFormantAndBandwidthTiers (fg->get(), position);
		ordered -> removeItem (position);
	}
}

void KlattGrid_addFormantFrequencyAndBandwidthTiers (KlattGrid me, kKlattGridFormantType formantType, integer position) {
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_addFormantAndBandwidthTiers (fg->get(), position);
}

void KlattGrid_removeFormantFrequencyAndBandwidthTiers (KlattGrid me, kKlattGridFormantType formantType, integer position) {
	autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_removeFormantAndBandwidthTiers (fg->get(), position);
}

double KlattGrid_getDeltaFormantAtTime (KlattGrid me, integer iformant, double t) {
	return FormantGrid_getFormantAtTime (my coupling -> delta_formants.get(), iformant, t);
}
void KlattGrid_addDeltaFormantPoint (KlattGrid me, integer iformant, double t, double value) {
	FormantGrid_addFormantPoint (my coupling -> delta_formants.get(), iformant, t, value);
}
void KlattGrid_removeDeltaFormantPoints (KlattGrid me, integer iformant, double t1, double t2) {
	FormantGrid_removeFormantPointsBetween (my coupling -> delta_formants.get(), iformant, t1, t2);
}
double KlattGrid_getDeltaBandwidthAtTime (KlattGrid me, integer iformant, double t) {
	return FormantGrid_getBandwidthAtTime (my coupling -> delta_formants.get(), iformant, t);
}
void KlattGrid_addDeltaBandwidthPoint (KlattGrid me, integer iformant, double t, double value) {
	FormantGrid_addBandwidthPoint (my coupling -> delta_formants.get(), iformant, t, value);
}
void KlattGrid_removeDeltaBandwidthPoints (KlattGrid me, integer iformant, double t1, double t2) {
	FormantGrid_removeBandwidthPointsBetween (my coupling -> delta_formants.get(), iformant, t1, t2);
}

autoFormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me) {
	try {
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, kKlattGridFormantType::DELTA);
		autoFormantGrid thee = Data_copy (fg->get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no delta FormantGrid extracted.");
	}
}

void KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"Domains should be equal");
		autoFormantGrid* fg = KlattGrid_getAddressOfFormantGrid (me, kKlattGridFormantType::DELTA);
		autoFormantGrid him = Data_copy (thee);
		*fg = him.move();
	} catch (MelderError) {
		Melder_throw (me, U": no delta FormantGrid replaced.");
	}
}

autoFormantGrid KlattGrid_to_oralFormantGrid_openPhases (KlattGrid me, double fadeFraction) {
	try {
		Melder_require (my vocalTract -> oral_formants -> formants.size > 0 || my vocalTract -> oral_formants -> bandwidths.size > 0,
			U"Formant grid should not be empty.");
		
		if (fadeFraction < 0.0)
			fadeFraction = 0.0;
		Melder_require (fadeFraction < 0.5,
			U"Fade fraction should be smaller than 0.5");
		
		my coupling -> options -> fadeFraction = fadeFraction;
		autoFormantGrid thee = Data_copy ( (FormantGrid) my vocalTract -> oral_formants.get());
		KlattGrid_setGlottisCoupling (me);
		FormantGrid_CouplingGrid_updateOpenPhases (thee.get(), my coupling.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no \"open phase\" oral FormantGrid created.");
	}
}

autoPointProcess KlattGrid_extractPointProcess_glottalClosures (KlattGrid me) {
	try {
		// Update PhonationTier
		autoPhonationTier pt = PhonationGrid_to_PhonationTier (my phonation.get());
		autoPointProcess thee = PhonationTier_to_PointProcess_closures (pt.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no glottal closure points extracted.");
	}
}

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t) {
	return RealTier_getValueAtTime (my frication -> fricationAmplitude.get(), t);
}

void KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value) {
	RealTier_addPoint (my frication -> fricationAmplitude.get(), t, value);
}

void KlattGrid_removeFricationAmplitudePoints (KlattGrid me, double t1, double t2) {
	AnyTier_removePointsBetween (my frication -> fricationAmplitude.get()->asAnyTier(), t1, t2);
}

autoIntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me) {
	return Data_copy (my frication -> fricationAmplitude.get());
}

void KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"Domains should be equal");
		my frication -> fricationAmplitude = Data_copy (thee);
	} catch (MelderError) {
		Melder_throw (me, U": no frication amplitude tier replaced.");
	}
}

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t) {
	return RealTier_getValueAtTime (my frication -> bypass.get(), t);
}

void KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value) {
	RealTier_addPoint (my frication -> bypass.get(), t, value);
}

void KlattGrid_removeFricationBypassPoints (KlattGrid me, double t1, double t2) {
	AnyTier_removePointsBetween (my frication -> bypass.get()->asAnyTier(), t1, t2);
}

autoIntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me) {
	return Data_copy (my frication -> bypass.get());
}

void KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"Domains should be equal");
		my frication -> bypass = Data_copy (thee);
	} catch (MelderError) {
		Melder_throw (me, U": no frication bypass tier replaced.");
	}
}

void KlattGrid_setGlottisCoupling (KlattGrid me) {
	try {
		my coupling -> glottis = PhonationGrid_to_PhonationTier (my phonation.get());
		Melder_require (my coupling -> glottis,
			U"Phonation tier should not be empty.");
	} catch (MelderError) {
		Melder_throw (me, U": no coupling could be set.");
	}
}

#if 0
static autoSound KlattGrid_to_Sound_aspiration (KlattGrid me, double samplingFrequency) {
	return PhonationGrid_to_Sound_aspiration (my phonation.get(), samplingFrequency);
}
#endif

autoSound KlattGrid_to_Sound_phonation (KlattGrid me) {
	return PhonationGrid_to_Sound (my phonation.get(), 0, my options -> samplingFrequency);
}

autoSound KlattGrid_to_Sound (KlattGrid me) {
	try {
		autoSound thee;
		const PhonationGridPlayOptions pp = my phonation -> options.get();
		const FricationGridPlayOptions pf = my frication -> options.get();
		const double samplingFrequency = my options -> samplingFrequency;

		if (pp -> voicing)
			KlattGrid_setGlottisCoupling (me);

		if (pp -> aspiration || pp -> voicing) { // No vocal tract filtering if no glottal source signal present
			autoSound source = PhonationGrid_to_Sound (my phonation.get(), my coupling.get(), samplingFrequency);
			thee = Sound_VocalTractGrid_CouplingGrid_filter (source.get(), my vocalTract.get(), my coupling.get());
		}

		if (pf -> endFricationFormant > 0 || pf -> bypass) {
			autoSound frication = FricationGrid_to_Sound (my frication.get(), samplingFrequency);
			if (thee)
				_Sounds_add_inplace (thee.get(), frication.get());
			else
				thee = frication.move();
		}

		if (! thee)
			thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);

		if (my options -> scalePeak)
			Vector_scale (thee.get(), 0.99);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}

void KlattGrid_playSpecial (KlattGrid me) {
	try {
		autoSound thee = KlattGrid_to_Sound (me);
		const KlattGridPlayOptions him = my options.get();
		if (his scalePeak)
			Vector_scale (thee.get(), 0.99);
		if (his xmin == 0.0 && his xmax == 0.0) {
			his xmin = my xmin;
			his xmax = my xmax;
		}
		Sound_playPart (thee.get(), his xmin, his xmax, nullptr, nullptr);
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void KlattGrid_play (KlattGrid me) {
	KlattGrid_setDefaultPlayOptions (me);
	KlattGrid_playSpecial (me);
}

/************************* Sound(s) & KlattGrid **************************************************/

autoSound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee) {
	return Sound_FricationGrid_filter (me, thy frication.get());
}

autoSound Sound_KlattGrid_filterByVocalTract (Sound me, KlattGrid thee, kKlattGridFilterModel filterModel) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
						U"Domains should be equal.");
		KlattGrid_setDefaultPlayOptions (thee);
		thy coupling -> options -> openglottis = 0; // don't trust openglottis info!
		thy vocalTract -> options -> filterModel = filterModel;
		return Sound_VocalTractGrid_CouplingGrid_filter (me, thy vocalTract.get(), thy coupling.get());
	} catch (MelderError) {
		Melder_throw (me, U": not filtered by KlattGrid.");
	}
}

/******************* KlattTable to KlattGrid *********************/

autoKlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration) {
	try {
		Table kt = (Table) me;

		const integer numberOfRows = my rows.size;
		const double tmin = 0, tmax = numberOfRows * frameDuration;
		constexpr double dBNul = -300;
		const double dB_offset = -20.0 * log10 (2.0e-5) - 87.0; // in KlattTable maximum in DB_to_LIN is at 87 dB : 32767
		const double dB_offset_voicing = 20.0 * log10 (320000 / 32767); // V' [n] in range (-320000,32000)
		const double dB_offset_noise = -20.0 * log10 (32.767 / 8.192); // noise in range (-8192,8192)
		//	double dB_offset_noise = -20 * log10 (320000/32767)  - 20 * log10 (32.767 / 8.192);
		const double ap [7] = {0, 0.4, 0.15, 0.06, 0.04, 0.022, 0.03 };
		const integer numberOfFormants = 6;
		const integer numberOfNasalFormants = 1;
		const integer numberOfNasalAntiFormants = numberOfNasalFormants;
		const integer numberOfTrachealFormants = 0;
		const integer numberOfTrachealAntiFormants = numberOfTrachealFormants;
		const integer numberOfFricationFormants = 6;
		const integer numberOfDeltaFormants = 1;

		autoKlattGrid thee = KlattGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants,
			numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants,
			numberOfFricationFormants, numberOfDeltaFormants);
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			const double t = (irow - 1) * frameDuration;

			integer icol = 1;
			double val = Table_getNumericValue_Assert (kt, irow, icol) / 10.0;   // F0hz10
			const double f0 = val;
			RealTier_addPoint (thy phonation -> pitch.get(), t, f0);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol); // AVdb
			// dB values below 13 were put to zero in the DBtoLIN function
			val -= 7.0;
			if (val < 13.0)
				val = dBNul;

			// RealTier_addPoint (thy source -> voicingAmplitude, t, val);

			for (integer kf = 1; kf <= 6; kf ++) {
				const double fk = val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // Fhz
				RealTier_addPoint (thy vocalTract -> oral_formants -> formants.at [kf], t, val);
				RealTier_addPoint (thy frication -> frication_formants -> formants.at [kf], t, val);   // only amplitudes and bandwidths in frication section
				val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // Bhz
				if (val <= 0.0)
					val = fk / 10.0;
				RealTier_addPoint (thy vocalTract -> oral_formants -> bandwidths.at [kf], t, val);
			}

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // FNZhz
			RealTier_addPoint (thy vocalTract -> nasal_antiformants -> formants.at [1], t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // BNZhz
			RealTier_addPoint (thy vocalTract -> nasal_antiformants -> bandwidths.at [1], t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // FNPhz
			RealTier_addPoint (thy vocalTract -> nasal_formants -> formants.at [1], t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // BNPhz
			RealTier_addPoint (thy vocalTract -> nasal_formants -> bandwidths.at [1], t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // ah
			if (val < 13.0)
				val = dBNul;
			else
				val += 20.0 * log10 (0.05) + dB_offset_noise;

			RealTier_addPoint (thy phonation -> aspirationAmplitude.get(), t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // Kopen
			const double openPhase = ( f0 > 0.0 ? (val / 16000.0) * f0 : 0.7 );
			RealTier_addPoint (thy phonation -> openPhase.get(), t, openPhase);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // Aturb breathinessAmplitude during voicing (max is 8192)
			if (val < 13.0)
				val = dBNul;
			else
				val += 20.0 * log10 (0.1) + dB_offset_noise;

			RealTier_addPoint (thy phonation -> breathinessAmplitude.get(), t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // TLTdb
			RealTier_addPoint (thy phonation -> spectralTilt.get(), t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // AF
			if (val < 13.0)
				val = dBNul;
			else
				val += 20.0 * log10 (0.25) + dB_offset_noise;

			RealTier_addPoint (thy frication -> fricationAmplitude.get(), t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // Kskew ???
			//RealTier_addPoint (, t, val);

			for (integer kf = 1; kf <= 6; kf ++) {
				val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // Ap
				if (val < 13.0)
					val = dBNul;
				else
					val += 20.0 * log10 (ap [kf]) + dB_offset;

				RealTier_addPoint (thy vocalTract -> oral_formants_amplitudes.at [kf], t, val);
				RealTier_addPoint (thy frication -> frication_formants_amplitudes.at [kf], t, val);
				val = Table_getNumericValue_Assert (kt, irow, ++ icol); // Bhz
				RealTier_addPoint (thy frication -> frication_formants -> bandwidths.at [kf], t, val);
			}

			val = Table_getNumericValue_Assert (kt, irow, ++ icol);   // ANP
			if (val < 13.0)
				val = dBNul;
			else
				val += 20.0 * log10 (0.6) + dB_offset;

			RealTier_addPoint (thy vocalTract -> nasal_formants_amplitudes.at [1], t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol); // AB
			if (val < 13.0)
				val = dBNul;
			else
				val += 20.0 * log10 (0.05) + dB_offset_noise;

			RealTier_addPoint (thy frication -> bypass.get(), t, val);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol); // AVpdb
			RealTier_addPoint (thy phonation -> voicingAmplitude.get(), t, val + dB_offset_voicing);

			val = Table_getNumericValue_Assert (kt, irow, ++ icol); // Gain0
			val -= 3.0;
			if (val <= 0.0)
				val = 57.0;
			RealTier_addPoint (thy gain.get(), t, val + dB_offset);
		}
		// We don't need the following low-pass: we do not use oversampling !!
		//RealTier_addPoint (thy tracheal_formants -> formants->at [1], 0.5*(tmin+tmax), 0.095*samplingFrequency);
		//RealTier_addPoint (thy tracheal_formants -> bandwidths->at [1], 0.5*(tmin+tmax), 0.063*samplingFrequency);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no KlattGrid created.");
	}
}

autoKlattGrid Sound_to_KlattGrid_simple (Sound me, double timeStep, integer maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preEmphasisFrequency, double minimumPitch, double maximumPitch, double pitchFloorIntensity, int subtractMean) {
	try {
		const integer numberOfFormants = maximumNumberOfFormants;
		const integer numberOfNasalFormants = 1;
		const integer numberOfNasalAntiFormants = numberOfNasalFormants;
		const integer numberOfTrachealFormants = 1;
		const integer numberOfTrachealAntiFormants = numberOfTrachealFormants;
		const integer numberOfFricationFormants =  maximumNumberOfFormants;
		const integer numberOfDeltaFormants = 1;
		autoSound sound = Data_copy (me);
		Vector_subtractMean (sound.get());
		autoFormant f = Sound_to_Formant_burg (sound.get(), timeStep, maximumNumberOfFormants,
		                                       maximumFormantFrequency, windowLength, preEmphasisFrequency);
		autoFormantGrid fgrid = Formant_downto_FormantGrid (f.get());
		autoPitch p = Sound_to_Pitch (sound.get(), timeStep, minimumPitch, maximumPitch);
		autoPitchTier ptier = Pitch_to_PitchTier (p.get());
		autoIntensity i = Sound_to_Intensity (sound.get(), pitchFloorIntensity, timeStep, subtractMean);
		autoIntensityTier itier = Intensity_downto_IntensityTier (i.get());
		autoKlattGrid thee = KlattGrid_create (my xmin, my xmax, numberOfFormants, numberOfNasalFormants,                            numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfFricationFormants, numberOfDeltaFormants);
		KlattGrid_replacePitchTier (thee.get(), ptier.get());
		KlattGrid_replaceFormantGrid (thee.get(), kKlattGridFormantType::ORAL, fgrid.get());
		KlattGrid_replaceVoicingAmplitudeTier (thee.get(), itier.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no simple KlattGrid created.");
	}
}

autoKlattGrid KlattGrid_createFromVowel (double duration, double f0start, double f1, double b1, double f2, double b2, double f3, double b3, double f4, double bandWidthFraction, double formantFrequencyInterval) {
	const integer numberOfOralFormants = 15;
	const double tstart = 0.0;
	autoKlattGrid me = KlattGrid_create (0.0, duration, numberOfOralFormants, 0, 0, 0, 0, 0, 0);
	KlattGrid_addPitchPoint (me.get(), tstart, f0start);
	KlattGrid_addVoicingAmplitudePoint (me.get(), tstart, 90.0);
	if (f1 > 0.0) {
		KlattGrid_addFormantPoint (me.get(), kKlattGridFormantType::ORAL, 1, tstart, f1);
		KlattGrid_addBandwidthPoint (me.get(), kKlattGridFormantType::ORAL, 1, tstart, b1);
	}
	if (f2 > 0.0) {
		KlattGrid_addFormantPoint (me.get(), kKlattGridFormantType::ORAL, 2, tstart, f2);
		KlattGrid_addBandwidthPoint (me.get(), kKlattGridFormantType::ORAL, 2, tstart, b2);
	}
	if (f3 > 0) {
		KlattGrid_addFormantPoint (me.get(), kKlattGridFormantType::ORAL, 3, tstart, f3);
		KlattGrid_addBandwidthPoint (me.get(), kKlattGridFormantType::ORAL, 3, tstart, b3);
	}
	if (f4 > 0) {
		KlattGrid_addFormantPoint (me.get(), kKlattGridFormantType::ORAL, 4, tstart, f4);
		KlattGrid_addBandwidthPoint (me.get(), kKlattGridFormantType::ORAL, 4, tstart, f4 * bandWidthFraction);
	}
	if (formantFrequencyInterval > 0) {
		double startFrequency = std::max (std::max (f1, f2), std::max (f3, f4));
		for (integer iformant = 5; iformant <= numberOfOralFormants; iformant ++) {
			double frequency =  startFrequency + (iformant - 4) * formantFrequencyInterval;
			KlattGrid_addFormantPoint (me.get(), kKlattGridFormantType::ORAL, iformant, tstart, frequency);
			KlattGrid_addBandwidthPoint (me.get(), kKlattGridFormantType::ORAL, iformant, tstart, frequency * bandWidthFraction);
		}
	}
	return me;
}

/* End of file KlattGrid.cpp */
