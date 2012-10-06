/* KlattGrid.cpp
 *
 * Copyright (C) 2008-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

// Prototypes

PointProcess PitchTier_to_PointProcess_flutter (PitchTier pitch, RealTier flutter, double maximumPeriod);

void _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, thou, long iformant, int antiformant);

Sound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling);

Sound PhonationGrid_PhonationTier_to_Sound_voiced (PhonationGrid me, PhonationTier thee, double samplingFrequency);

Sound KlattGrid_to_Sound_aspiration (KlattGrid me, double samplingFrequency);

#undef MIN
#undef MAX
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define KlattGrid_OPENPHASE_DEFAULT 0.7
#define KlattGrid_POWER1_DEFAULT 3
#define KlattGrid_POWER2_DEFAULT (KlattGrid_POWER1_DEFAULT+1)

/*	Amplitude scaling: maximum amplitude (-1,+1) corresponds to 91 dB */

/*static double NUMinterpolateLinear (double x1, double y1, double x2, double y2, double x)
{
	if (y1 == y2) return y1;
	if (x1 == x2) return NUMundefined;
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}*/

static void rel_to_abs (double *w, double *ws, long n, double d) {
	double sum = 0;
	for (long i = 1; i <= n; i++) { // relative
		sum += w[i];
	}
	if (sum != 0) {
		double dw = d / sum;
		sum = 0;
		for (long i = 1; i <= n; i++) { // to absolute
			w[i] *= dw;
			sum += w[i];
			ws[i] = sum;
		}
	}
}

static bool RealTier_valuesInRange (I, double min, double max) {
	iam (RealTier);
	for (long i = 1; i <= my points -> size; i++) {
		RealPoint p = (RealPoint) my points -> item[i];
		if (NUMdefined (min) && p -> value < min) {
			return false;
		}
		if (NUMdefined (max) && p -> value < max) {
			return false;
		}
	}
	return true;
}

static double PointProcess_getPeriodAtIndex (PointProcess me, long it, double maximumPeriod) {
	double period = NUMundefined;
	if (it >= 2) {
		period = my t[it] - my t[it - 1];
		if (period > maximumPeriod) {
			period = NUMundefined;
		}
	}
	if (period == NUMundefined) {
		if (it < my nt) {
			period = my t[it + 1] - my t[it];
			if (period > maximumPeriod) {
				period = NUMundefined;
			}
		}
	}
	// NUMundefined can only occur for a single isolated pulse.
	return period;
}

#define UPDATE_TIER RealTier_addPoint (thee.peek(), mytime, myvalue); \
	lasttime=mytime; myindex++; \
	if (myindex <= numberOfValues)\
	{\
	mypoint = (RealPoint) my points -> item [myindex];\
	mytime = mypoint -> number; myvalue = mypoint -> value;\
	}\
	else mytime = my xmax;\

static RealTier RealTier_updateWithDelta (RealTier me, RealTier delta, PhonationTier glottis, double openglottis_fadeFraction) {
	try {
		long myindex = 1;
		RealPoint mypoint = (RealPoint) my points -> item [myindex];
		long numberOfValues = my points -> size;
		double mytime = mypoint -> number;
		double myvalue = mypoint -> value;
		double lasttime = my xmin - 0.001; // sometime before xmin
		autoRealTier thee = RealTier_create (my xmin, my xmax);


		if (openglottis_fadeFraction <= 0) {
			openglottis_fadeFraction = 0.0001;
		}
		if (openglottis_fadeFraction >= 0.5) {
			openglottis_fadeFraction = 0.4999;
		}

		for (long ipoint = 1; ipoint <= glottis -> points -> size; ipoint++) {
			PhonationPoint point = (PhonationPoint) glottis -> points -> item [ipoint];
			double t4 = point -> time; // glottis closing
			double openDuration = point -> te ;
			double t1 = t4 - openDuration;
			double t2 = t1 + openglottis_fadeFraction * openDuration;
			double t3 = t4 - openglottis_fadeFraction * openDuration;

			// Add my points that lie before t1 and after previous t4
			while (mytime > lasttime && mytime < t1) {
				UPDATE_TIER
			}

			if (t2 > t1) {
				// Set new value at t1
				double myvalue1 = RealTier_getValueAtTime (me, t1);
				RealTier_addPoint (thee.peek(), t1, myvalue1);
				// Add my points between t1 and t2
				while (mytime > lasttime && mytime < t2) {
					double dvalue = RealTier_getValueAtTime (delta, mytime);
					if (NUMdefined (dvalue)) {
						double fraction = (mytime - t1) / (openglottis_fadeFraction * openDuration);
						myvalue += dvalue * fraction;
					}
					UPDATE_TIER
				}
			}

			double myvalue2 = RealTier_getValueAtTime (me, t2);
			double dvalue = RealTier_getValueAtTime (delta, t2);
			if (NUMdefined (dvalue)) {
				myvalue2 += dvalue;
			}
			RealTier_addPoint (thee.peek(), t2, myvalue2);

			// Add points between t2 and t3

			while (mytime > lasttime && mytime < t3) {
				dvalue = RealTier_getValueAtTime (delta, mytime);
				if (NUMdefined (dvalue)) {
					myvalue += dvalue;
				}
				UPDATE_TIER
			}

			// set new value at t3

			double myvalue3 = RealTier_getValueAtTime (me, t3);
			dvalue = RealTier_getValueAtTime (delta, t3);
			if (NUMdefined (dvalue)) {
				myvalue3 += dvalue;
			}
			RealTier_addPoint (thee.peek(), t3, myvalue3);

			if (t4 > t3) {
				// Add my points between t3 and t4
				while (mytime > lasttime && mytime < t4) {
					dvalue = RealTier_getValueAtTime (delta, mytime);
					if (NUMdefined (dvalue)) {
						double fraction = 1 - (mytime - t3) / (openglottis_fadeFraction * openDuration);
						myvalue += dvalue * fraction;
					}
					UPDATE_TIER
				}

				// Set new value at t4
				double myvalue4 = RealTier_getValueAtTime (me, t4);
				RealTier_addPoint (thee.peek(), t4, myvalue4);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not updated with delta.");
	}
}

static bool FormantGrid_isFormantDefined (FormantGrid me, long iformant) {
	RealTier ftier = (RealTier) my formants -> item[iformant];
	RealTier btier = (RealTier) my bandwidths -> item[iformant];
	return ftier -> points -> size != 0 and btier -> points -> size != 0;
}

static bool FormantGrid_Intensities_isFormantDefined (FormantGrid me, Ordered thee, long iformant) {
	RealTier ftier = (RealTier) my formants -> item[iformant];
	RealTier btier = (RealTier) my bandwidths -> item[iformant];
	RealTier atier = (RealTier) thy item[iformant];
	return ftier -> points -> size != 0 and btier -> points -> size != 0 and atier -> points -> size != 0;
}

static void check_formants (long numberOfFormants, long *ifb, long *ife) {
	if (numberOfFormants <= 0 || *ifb > numberOfFormants || *ife < *ifb || *ife < 1) {
		*ife = 0;  // overrules everything *ifb value is a don't care now
		return;
	}
	if (*ifb <= 1) {
		*ifb = 1;
	}
	if (*ife > numberOfFormants) {
		*ife = numberOfFormants;
	}
}

static Sound Sound_createEmptyMono (double xmin, double xmax, double samplingFrequency) {
	long nt = ceil ( (xmax - xmin) * samplingFrequency);
	double dt = 1.0 / samplingFrequency;
	double tmid = (xmin + xmax) / 2;
	double t1 = tmid - 0.5 * (nt - 1) * dt;

	return Sound_create (1, xmin, xmax, nt, dt, t1);
}

static void _Sounds_add_inline (Sound me, Sound thee) {
	for (long i = 1; i <= my nx; i++) {
		my z[1][i] += thy z[1][i];
	}
}

static Sound _Sound_diff (Sound me, int scale) {
	try {
		autoSound thee = Data_copy (me);

		// extremum
		double amax1 = -1.0e34, amax2 = amax1, val, pval = 0;
		if (scale) {
			for (long i = 1; i <= thy nx; i++) {
				val = fabs (thy z[1][i]);
				if (val > amax1) {
					amax1 = val;
				}
			}
		}
		// x[n]-x[n-1]
		for (long i = 1; i <= thy nx; i++) {
			val =  thy z[1][i];
			thy z[1][i] -=  pval;
			pval = val;
		}
		if (scale) {
			for (long i = 1; i <= thy nx; i++) {
				val = fabs (thy z[1][i]);
				if (val > amax2) {
					amax2 = val;
				}
			}
			// scale
			for (long i = 1; i <= thy nx; i++) {
				thy z[1][i] *= amax1 / amax2;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not differenced.");
	}
}

/*static void _Sounds_addDifferentiated_inline (Sound me, Sound thee)
{
	double pval = 0, dx = my dx;
	for (long i = 1; i <= my nx; i++)
	{
		double val =  thy z[1][i];
		my z[1][i] += (val - pval) / dx; // dx makes amplitude of dz/dt independent of sampling.
		pval = val;
	}
}*/

typedef struct structconnections {
	long numberOfConnections;
	double *x, *y;
} *connections;

static void connections_free (connections me) {
	if (me == NULL) {
		return;
	}
	NUMvector_free (my x, 1);
	NUMvector_free (my y, 1);
	Melder_free (me);
}

static connections connections_create (long numberOfConnections) {
	connections me = 0;
	try {
		me = (connections) Melder_malloc (structconnections, 1);
		my numberOfConnections = numberOfConnections;
		my x = NUMvector<double> (1, numberOfConnections);
		my y = NUMvector<double> (1, numberOfConnections);
		return me;
	} catch (MelderError) {
		connections_free (me);
		Melder_throw ("Connections not created.");
	}
}

// Calculates the intersection point (xi,yi) of a line with a circle.
// The line starts at the origin and P (xp, yp) is on that line.
static void NUMcircle_radial_intersection_sq (double x, double y, double r, double xp, double yp, double *xi, double *yi) {
	double dx = xp - x, dy = yp - y;
	double d = sqrt (dx * dx + dy * dy);
	if (d > 0) {
		*xi = x + dx * r / d;
		*yi = y + dy * r / d;
	} else {
		*xi = *yi = NUMundefined;
	}
}

static void summer_draw (Graphics g, double x, double y, double r, int alternating) {
	Graphics_setLineWidth (g, 2);
	Graphics_circle (g, x, y, r);
	double dy = 3 * r / 4;
	// + symbol
	if (alternating) {
		y += r / 4;
	}
	Graphics_line (g, x, y + r / 2, x, y - r / 2);
	Graphics_line (g, x - r / 2, y, x + r / 2, y);
	if (alternating) {
		Graphics_line (g, x - r / 2, y - dy , x + r / 2, y - dy);
	}
}

static void _summer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, int alternating, double horizontalFraction) {
	summer_draw (g, x, y, r, alternating);

	for (long i = 1; i <= thy numberOfConnections; i++) {
		double xto, yto, xp = thy x[i], yp = thy y[i];
		if (horizontalFraction > 0) {
			double dx = x - xp;
			if (dx > 0) {
				xp += horizontalFraction * dx;
				Graphics_line (g, thy x[i], yp, xp, yp);
			}
		}
		NUMcircle_radial_intersection_sq (x, y, r, xp, yp, &xto, &yto);
		if (xto == NUMundefined || yto == NUMundefined) {
			continue;
		}
		if (arrow) {
			Graphics_arrow (g, xp, yp, xto, yto);
		} else {
			Graphics_line (g, xp, yp, xto, yto);
		}
	}
}

static void summer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, double horizontalFraction) {
	_summer_drawConnections (g, x, y, r, thee, arrow, 0, horizontalFraction);
}

static void alternatingSummer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, double horizontalFraction) {
	_summer_drawConnections (g, x, y, r, thee, arrow, 1, horizontalFraction);
}

static void draw_oneSection (Graphics g, double xmin, double xmax, double ymin, double ymax,
                             const wchar_t *line1, const wchar_t *line2, const wchar_t *line3) {
	long numberOfTextLines = 0, iline = 0;
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	if (line1 != 0) {
		numberOfTextLines++;
	}
	if (line2 != 0) {
		numberOfTextLines++;
	}
	if (line3 != 0) {
		numberOfTextLines++;
	}
	double y = ymax, dy = (ymax - ymin) / (numberOfTextLines + 1), ddy = dy / 10;
	double x = (xmax + xmin) / 2;
	if (line1 != 0) {
		iline++;
		y -= dy - (numberOfTextLines == 2 ? ddy : 0); // extra spacing for two lines
		Graphics_text1 (g, x, y, line1);
	}
	if (line2 != 0) {
		iline++;
		y -= dy - (numberOfTextLines == 2 ? (iline == 1 ? ddy : -iline * ddy) : 0);
		Graphics_text1 (g, x, y, line2);
	}
	if (line3 != 0) {
		iline++;
		y -= dy - (numberOfTextLines == 2 ? -iline * ddy : 0);
		Graphics_text1 (g, x, y, line3);
	}
}

// Maximum amplitue (-1,1) at 93.97940008672037 dB
#define DBSPL_to_A(x) (pow (10.0, x / 20.0) * 2.0e-5)
// Normal dB's
#define DB_to_A(x) (pow (10.0, x / 20.0))

/********************* PhonationTier ************************/

Thing_implement (PhonationPoint, Data, 0);

PhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
                                      double power1, double power2, double pulseScale) {
	try {
		autoPhonationPoint me = Thing_new (PhonationPoint);
		my time = time; my period = period;
		my openPhase = openPhase;
		my collisionPhase = collisionPhase;	my te = te;
		my power1 = power1; my power2 = power2;
		my pulseScale = pulseScale;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PhonationPoint not created.");
	}
}

Thing_implement (PhonationTier, Function, 0);

PhonationTier PhonationTier_create (double tmin, double tmax) {
	try {
		autoPhonationTier me = Thing_new (PhonationTier);
		Function_init (me.peek(), tmin, tmax);
		my points = SortedSetOfDouble_create ();
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PhonationTier not created.");
	}
}

PointProcess PhonationTier_to_PointProcess_closures (PhonationTier me) {
	try {
		long nt = my points -> size;
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, nt);
		for (long ip = 1; ip <= nt; ip++) {
			PhonationPoint fp = (PhonationPoint) my points -> item[ip];
			PointProcess_addPoint (thee.peek(), fp -> time);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PointProcess with closure times created.");
	}
}

/********************** PhonationGridPlayOptions **********************/

Thing_implement (PhonationGridPlayOptions, Data, 0);

static void PhonationGridPlayOptions_setDefaults (PhonationGridPlayOptions me) {
	my flowDerivative = my voicing = 1;
	my aspiration = my breathiness = 1;
	my flutter = my doublePulsing = 1;
	my collisionPhase = my spectralTilt = 1;
	my flowFunction = 1; // User defined flow tiers (power1 & power2)
	my maximumPeriod = 0;
}

PhonationGridPlayOptions PhonationGridPlayOptions_create () {
	try {
		autoPhonationGridPlayOptions me = Thing_new (PhonationGridPlayOptions);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PhonationGridPlayOptions not created.");
	}
}

/********************** PhonationGrid **********************/


Thing_implement (PhonationGrid, Function, 0);

void structPhonationGrid :: v_info () {
	structData :: v_info ();
	const wchar_t *in1 = L"  ", *in2 = L"    ";
	MelderInfo_writeLine (in1, L"Time domain:");
	MelderInfo_writeLine (in2, L"Start time:     ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (in2, L"End time:       ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (in2, L"Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (in1, L"\nNumber of points in the PHONATION tiers:");
	MelderInfo_writeLine (in2, L"pitch:               ", Melder_integer (pitch -> points -> size));
	MelderInfo_writeLine (in2, L"voicingAmplitude:    ", Melder_integer (voicingAmplitude -> points -> size));
	MelderInfo_writeLine (in2, L"openPhase:           ", Melder_integer (openPhase -> points -> size));
	MelderInfo_writeLine (in2, L"collisionPhase:      ", Melder_integer (collisionPhase -> points -> size));
	MelderInfo_writeLine (in2, L"power1:              ", Melder_integer (power1 -> points -> size));
	MelderInfo_writeLine (in2, L"power2:              ", Melder_integer (power2 -> points -> size));
	MelderInfo_writeLine (in2, L"flutter:             ", Melder_integer (flutter -> points -> size));
	MelderInfo_writeLine (in2, L"doublePulsing:       ", Melder_integer (doublePulsing -> points -> size));
	MelderInfo_writeLine (in2, L"spectralTilt:        ", Melder_integer (spectralTilt -> points -> size));
	MelderInfo_writeLine (in2, L"aspirationAmplitude: ", Melder_integer (aspirationAmplitude -> points -> size));
	MelderInfo_writeLine (in2, L"breathinessAmplitude:", Melder_integer (breathinessAmplitude -> points -> size));
}

void PhonationGrid_setNames (PhonationGrid me) {
	Thing_setName (my pitch, L"pitch");
	Thing_setName (my voicingAmplitude, L"voicingAmplitude");
	Thing_setName (my openPhase, L"openPhase");
	Thing_setName (my collisionPhase, L"collisionPhase");
	Thing_setName (my power1, L"power1");
	Thing_setName (my power2, L"power2");
	Thing_setName (my flutter, L"flutter");
	Thing_setName (my doublePulsing, L"doublePulsing");
	Thing_setName (my spectralTilt, L"spectralTilt");
	Thing_setName (my aspirationAmplitude, L"aspirationAmplitude");
	Thing_setName (my breathinessAmplitude, L"breathinessAmplitude");
}

PhonationGrid PhonationGrid_create (double tmin, double tmax) {
	try {
		autoPhonationGrid me = Thing_new (PhonationGrid);
		Function_init (me.peek(), tmin, tmax);
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
		PhonationGrid_setNames (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PhonationGrid not created.");
	}
}

static void PhonationGrid_checkFlowFunction (PhonationGrid me) {
	int hasPower1Points = my power1 -> points -> size > 0;
	int hasPower2Points = my power2 -> points -> size > 0;

	long ipoint = 1;
	do {
		double time = hasPower1Points ? ( (RealPoint) (my power1 -> points -> item[ipoint])) -> number : 0.5 * (my xmin + my xmax);
		double power1 = RealTier_getValueAtIndex (my power1, ipoint);
		if (power1 == NUMundefined) {
			power1 = KlattGrid_POWER1_DEFAULT;
		}
		if (power1 <= 0) {
			Melder_throw ("All power1 values must greater than zero.");
		}
		double power2 = RealTier_getValueAtTime (my power2, time);
		if (power2 == NUMundefined) {
			power2 = KlattGrid_POWER2_DEFAULT;
		}
		if (power2 <= power1) {
			Melder_throw ("At all times a power1 value must be smaller than the corresponding power2 value.");
		}
	} while (++ipoint < my power1 -> points -> size);

	// Now check power2 values. This is necessary to catch situations where power2 has a valley:
	// power1(0) = 3; power2(1)= 4; power2(1)= 4; power2(0.5) = 3;

	ipoint = 1;
	do {
		double time = hasPower2Points ? ( (RealPoint) (my power2 -> points -> item[ipoint])) -> number : 0.5 * (my xmin + my xmax);
		double power2 = RealTier_getValueAtIndex (my power2, ipoint);
		if (power2 == NUMundefined) {
			power2 = KlattGrid_POWER2_DEFAULT;
		}
		double power1 = RealTier_getValueAtTime (my power1, time);
		if (power1 == NUMundefined) {
			power1 = KlattGrid_POWER1_DEFAULT;
		}
		if (power2 <= power1) {
			Melder_throw ("At all times the power2 value must be greater than the corresponding power1 value.");
		}
	} while (++ipoint < my power2 -> points -> size);
}

static void PhonationGrid_draw_inside (PhonationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout) {
	// dum voicing conn tilt conn summer
	(void) me;
	double xw[6] = { 0, 1, 0.5, 1, 0.5, 0.5 }, xws[6];

	connections thee = connections_create (2);

	rel_to_abs (xw, xws, 5, xmax - xmin);

	dy = (ymax - ymin) / (1 + (dy < 0 ? 0 : dy) + 1);

	double x1 = xmin, x2 = x1 + xw[1];
	double y2 = ymax, y1 = y2 - dy;
	draw_oneSection (g, x1, x2, y1, y2, NULL, L"Voicing", 0);

	x1 = x2; x2 = x1 + xw[2];
	double ymid = (y1 + y2) / 2;
	Graphics_line (g, x1, ymid, x2, ymid);

	x1 = x2; x2 = x1 + xw[3];
	draw_oneSection (g, x1, x2, y1, y2, NULL, L"Tilt", 0);

	thy x[1] = x2; thy y[1] = ymid;

	y2 = y1 - 0.5 * dy; y1 = y2 - dy; ymid = (y1 + y2) / 2;
	x2 = xmin + xws[3]; x1 = x2 - 1.5 * xw[3]; // some extra space
	draw_oneSection (g, x1, x2, y1, y2, NULL, L"Aspiration", 0);

	thy x[2] = x2; thy y[2] = ymid;

	double r = xw[5] / 2;
	double xs = xmax - r, ys = (ymax + ymin) / 2;
	int arrow = 1;

	summer_drawConnections (g, xs, ys, r, thee, arrow, 0.4);
	connections_free (thee);

	if (yout != 0) {
		*yout = ys;
	}
}

void PhonationGrid_draw (PhonationGrid me, Graphics g) {
	double xmin = 0, xmax2 = 0.9, xmax = 1, ymin = 0, ymax = 1, dy = 0.5, yout;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	PhonationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, &yout);

	Graphics_arrow (g, xmax2, yout, xmax, yout);
	Graphics_unsetInner (g);
}

double PhonationGrid_getMaximumPeriod (PhonationGrid me) {
	double minimumPitch = RealTier_getMinimumValue (my pitch);
	return 2 / ( (minimumPitch == NUMundefined || minimumPitch == 0) ? (my xmax - my xmin) : minimumPitch);
}

PointProcess PitchTier_to_PointProcess_flutter (PitchTier pitch, RealTier flutter, double maximumPeriod) {
	try {
		autoPointProcess thee = PitchTier_to_PointProcess (pitch);
		if (flutter == 0) {
			return thee.transfer();
		}
		double tsum = 0;
		for (long it = 2; it <= thy nt; it++) {
			double t = thy t[it - 1];
			double period = thy t[it] - thy t[it - 1];
			if (period < maximumPeriod && flutter -> points -> size > 0) {
				double fltr = RealTier_getValueAtTime (flutter, t);
				if (NUMdefined (fltr)) {
					// newF0 = f0 * (1 + (val / 50) * (sin ... + ...));
					double newPeriod = period / (1 + (fltr / 50) * (sin (2 * NUMpi * 12.7 * t) + sin (2 * NUMpi * 7.1 * t) + sin (2 * NUMpi * 4.7 * t)));
					tsum += newPeriod - period;
				}
			}
			thy t[it] += tsum;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (pitch, ": no flutter PointProcess created.");
	}
}

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency) {
	try {
		autoSound thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);

		// Noise spectrum is tilted down by soft low-pass filter having a pole near
		// the origin in the z-plane, i.e. y[n] = x[n] + (0.75 * y[n-1])
		double lastval = 0;
		if (my aspirationAmplitude -> points -> size > 0) {
			for (long i = 1; i <= thy nx; i++) {
				double t = thy x1 + (i - 1) * thy dx;
				double val = NUMrandomUniform (-1, 1);
				double a = DBSPL_to_A (RealTier_getValueAtTime (my aspirationAmplitude, t));
				if (NUMdefined (a)) {
					thy z[1][i] = lastval = val + 0.75 * lastval;
					lastval = (val += 0.75 * lastval); // soft low-pass
					thy z[1][i] = val * a;
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no aspiration Sound created.");
	}
}

static void Sound_PhonationGrid_spectralTilt_inline (Sound thee, PhonationGrid me) {
	if (my spectralTilt -> points -> size > 0) {
		/* Spectral tilt
			Filter y[n] = a * x[n] + b * y[n-1] => H(z) = a / (1 - bz^(-1)).
			We need attenuation, i.e. low-pass. Therefore 0 <= b <= 1.
			|H(f)| = a / sqrt (1 - 2*b*cos(2*pi*f*T) + b^2),
			|H(0)|= a /(1 - b) => if |H(0)| == 1, then a = 1 - b.
			Now solve 20 log|H(F)|= -c (at F=3 kHz and c > 0)
			Solution: if q = (1 - D * cos(2*pi*F*T)) / (1 - D), with D = 10^(-c/10)
				then b = q -sqrt(q^2 - 1)
		*/

		double cosf = cos (2 * NUMpi * 3000 * thy dx), ynm1 = 0;  // samplingFrequency > 6000 !

		for (long i = 1; i <= thy nx; i++) {
			double t = thy x1 + (i - 1) * thy dx;
			double tilt_db = RealTier_getValueAtTime (my spectralTilt, t);

			if (tilt_db > 0) {
				double d = pow (10, -tilt_db / 10);
				double q = (1 - d * cosf) / (1 - d);
				double b = q - sqrt (q * q - 1);
				double a = 1 - b;
				thy z[1][i] = a * thy z[1][i] + b * ynm1;
				ynm1 = thy z[1][i];
			}
		}
	}
}

struct nrfunction_struct {
	double n;
	double m;
	double a;
};
static void nrfunction (double x, double *fx, double *dfx, void *closure) {
	struct nrfunction_struct *nrfs = (struct nrfunction_struct *) closure;
	double mplusax = nrfs -> m + nrfs -> a * x;
	double mminn = nrfs -> m - nrfs -> n;
	*fx = pow (x, mminn) - (nrfs -> n + nrfs -> a * x) / mplusax;
	*dfx = mminn * pow (x, mminn - 1) - nrfs -> a * mminn / (mplusax * mplusax);
}

static double get_collisionPoint_x (double n, double m, double collisionPhase) {
	double y = NUMundefined;
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
	if (collisionPhase <= 0) {
		return 1;
	}
	double a = 1 / collisionPhase;
	if (m - n == 1) {
		double b = m - a;
		double c = - n, y1, y2;
		long nroots = NUMsolveQuadraticEquation (a, b, c, &y1, &y2);
		if (nroots == 2) {
			y = y2;
		} else if (nroots == 1) {
			y = y1;
		}
	} else { // Newton-Raphson
		// search in the interval from where the flow is a maximum to 1
		struct nrfunction_struct nrfs = {n, m, a};
		double root, xmaxFlow = pow (n / m, 1.0 / (m - n));
		NUMnrbis (&nrfunction, xmaxFlow, 1, &nrfs, &root); y = root;
	}
	return y;
}

PhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me) {
	try {
		long diplophonicPulseIndex = 0;
		PhonationGridPlayOptions pp = my options;

		PhonationGrid_checkFlowFunction (me);
		if (my pitch -> points -> size == 0) {
			Melder_throw ("Pitch tier is empty.");
		}

		if (pp -> maximumPeriod == 0) {
			pp -> maximumPeriod = PhonationGrid_getMaximumPeriod (me);
		}

		autoPointProcess point = PitchTier_to_PointProcess_flutter (my pitch, (pp -> flutter ? my flutter : 0), pp -> maximumPeriod);

		autoPhonationTier thee = PhonationTier_create (my xmin, my xmax);

		/*
		Cycle through the points of the point PointProcess. Each will become a period.
		We assume that the planning for the pitch period occurs approximately at a time T before the glottal closure.
		For each point t[i]:
			Determine the f0 -> period T[i]
			Determine time t[i]-T[i] the open quotient, power1, power2, collisionphase etc.
			Generate the period.
		*/

		for (long it = 1; it <= point -> nt; it++) {
			double re, t = point -> t[it];		// the glottis "closing" point
			double pulseDelay = 0;        // For alternate pulses in case of diplophonia
			double pulseScale = 1;        // For alternate pulses in case of diplophonia

			double period = PointProcess_getPeriodAtIndex (point.peek(), it, pp -> maximumPeriod);
			if (period == NUMundefined) {
				period = 0.5 * pp -> maximumPeriod; // Some default value
			}

			// Calculate the point where the exponential decay starts:
			// Query tiers where period starts .

			double periodStart = t - period; // point where period starts:

			double collisionPhase = pp -> collisionPhase ? RealTier_getValueAtTime (my collisionPhase, periodStart) : 0;
			if (collisionPhase == NUMundefined) {
				collisionPhase = 0;
			}
			double power1 = pp -> flowFunction == 1 ? RealTier_getValueAtTime (my power1, periodStart) : pp -> flowFunction;
			if (power1 == NUMundefined) {
				power1 = KlattGrid_POWER1_DEFAULT;
			}
			double power2 = pp -> flowFunction == 1 ? RealTier_getValueAtTime (my power2, periodStart) : pp -> flowFunction + 1;
			if (power2 == NUMundefined) {
				power2 = KlattGrid_POWER2_DEFAULT;
			}
			try {
				re = get_collisionPoint_x (power1, power2, collisionPhase);
			} catch (MelderError) {
				Melder_warning (L"Illegal collision point at t = ", Melder_double (t), L" (power1=", Melder_double (power1), L", power2=", Melder_double (power2), L"colPhase=", Melder_double (collisionPhase), L")");
			}

			double openPhase = RealTier_getValueAtTime (my openPhase, periodStart);
			if (openPhase == NUMundefined) {
				openPhase = KlattGrid_OPENPHASE_DEFAULT;
			}

			double te = re * period * openPhase;

			// In case of diplophonia alternate pulses get modified.
			// A modified puls is delayed in time and its amplitude attenuated.
			// This delay scales to maximally equal the closed phase of the next period.
			// The doublePulsing scales the amplitudes as well as the delay linearly.

			double doublePulsing = pp -> doublePulsing ? RealTier_getValueAtTime (my doublePulsing, periodStart) : 0;
			if (doublePulsing == NUMundefined) {
				doublePulsing = 0;
			}

			if (doublePulsing > 0) {
				diplophonicPulseIndex++;
				if (diplophonicPulseIndex % 2 == 1) { // the odd one
					double nextPeriod = PointProcess_getPeriodAtIndex (point.peek(), it + 1, pp -> maximumPeriod);
					if (nextPeriod == NUMundefined) {
						nextPeriod = period;
					}
					double openPhase2 = KlattGrid_OPENPHASE_DEFAULT;
					if (my openPhase -> points -> size > 0) {
						openPhase2 = RealTier_getValueAtTime (my openPhase, t);
					}
					double maxDelay = period * (1 - openPhase2);
					pulseDelay = maxDelay * doublePulsing;
					pulseScale *= (1 - doublePulsing);
				}
			} else {
				diplophonicPulseIndex = 0;
			}

			t += pulseDelay;
			autoPhonationPoint phonationPoint = PhonationPoint_create (t, period, openPhase, collisionPhase, te, power1, power2, pulseScale);
			AnyTier_addPoint (thee.peek(), phonationPoint.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PhonationTier created.");
	}
}

Sound PhonationGrid_PhonationTier_to_Sound_voiced (PhonationGrid me, PhonationTier thee, double samplingFrequency) {
	try {
		PhonationGridPlayOptions p = my options;
		double lastVal = NUMundefined;

		if (my voicingAmplitude -> points -> size == 0) {
			Melder_throw ("Voicing amplitude tier is empty.");
		}

		autoSound him = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);
		autoSound breathy = 0;
		if (p -> breathiness && my breathinessAmplitude -> points -> size > 0) {
			breathy.reset (Sound_createEmptyMono (my xmin, my xmax, samplingFrequency));
		}
		/*
			Cycle through the points of the PhonationTier. Each will become a period.
			We assume that the planning for the pitch period occurs approximately at a time T before the glottal closure.
			For each point t[i]:
				Determine the f0 -> period T[i]
				Determine time t[i]-T[i] the open quotient, power1, power2, collisionphase etc.
				Generate the period.
		*/
		double *sound = his z[1];
		for (long it = 1; it <= thy points -> size; it++) {
			PhonationPoint point = (PhonationPoint) thy points -> item[it];
			double t = point -> time;		// the glottis "closing" point
			double te = point -> te;
			double period = point -> period; // duration of the current period
			double openPhase = point -> openPhase;
			double collisionPhase = point -> collisionPhase;
			double pulseScale = point -> pulseScale;        // For alternate pulses in case of diplophonia
			double power1 = point -> power1, power2 = point -> power2;
			double phase;                 // 0..1
			double flow;

			//- double amplitude = pulseScale * (power1 + power2 + 1.0) / (power2 - power1);
			//- amplitude /= period * openPhase;

			// Maximum of U(x) = x^n - x^m is where the derivative U'(x) = n x^(n-1) - m x^(m-1) == 0,
			//	i.e. (n/m) = x^(m-n), so xmax = (n/m)^(1/(m-n))
			//	U(xmax) = x^n (1-x^(m-n)) = (n/m)^(n/(m-n))(1-n/m)

			double amplitude = pulseScale / (pow (power1 / power2, 1 / (power2 / power1 - 1)) * (1 - power1 / power2));

			// Fill in the samples to the left of the current point.

			long midSample = Sampled_xToLowIndex (him.peek(), t), beginSample;
			beginSample = midSample - floor (te / his dx);
			if (beginSample < 1) {
				beginSample = 0;
			}
			if (midSample > his nx) {
				midSample = his nx;
			}
			for (long i = beginSample; i <= midSample; i++) {
				double tsamp = his x1 + (i - 1) * his dx;
				phase = (tsamp - (t - te)) / (period * openPhase);
				if (phase > 0.0) {
					flow = amplitude * (pow (phase, power1) - pow (phase, power2));
					if (i == 0) {
						lastVal = flow;    // For the derivative
						continue;
					}
					sound[i] += flow;

					// Breathiness only during open part modulated by the flow
					if (breathy.peek() != 0) {
						double val = flow * NUMrandomUniform (-1, 1);
						double a = RealTier_getValueAtTime (my breathinessAmplitude, t);
						breathy -> z[1][i] += val * DBSPL_to_A (a);
					}
				}
			}

			// Determine the signal parameters at the current point.

			phase = te / (period * openPhase);

			//- double flow = amplitude * (period * openPhase) * (pow (phase, power1) - pow (phase, power2));

			flow = amplitude * (pow (phase, power1) - pow (phase, power2));

			// Fill in the samples to the right of the current point.

			if (flow > 0.0) {
				double ta = collisionPhase * (period * openPhase);
				double factorPerSample = exp (- his dx / ta);
				double value = flow * exp (- (his x1 + midSample * his dx - t) / ta);
				long endSample = midSample + floor (20 * ta / his dx);
				if (endSample > his nx) {
					endSample = his nx;
				}
				for (long i = midSample + 1; i <= endSample; i++) {
					sound[i] += value;
					value *= factorPerSample;
				}
			}
		}

		// Scale voiced part and add breathiness during open phase
		if (p -> flowDerivative) {
			double extremum = Vector_getAbsoluteExtremum (him.peek(), 0, 0, Vector_VALUE_INTERPOLATION_CUBIC);
			if (! NUMdefined (lastVal)) {
				lastVal = 0;
			}
			for (long i = 1; i <= his nx; i++) {
				double val = his z[1][i];
				his z[1][i] -= lastVal;
				lastVal = val;
			}
			Vector_scale (him.peek(), extremum);
		}

		for (long i = 1; i <= his nx; i++) {
			double t = his x1 + (i - 1) * his dx;
			his z[1][i] *= DBSPL_to_A (RealTier_getValueAtTime (my voicingAmplitude, t));
			if (breathy.peek() != 0) {
				his z[1][i] += breathy -> z[1][i];
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Sound created.");
	}
}

static Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, double samplingFrequency) {
	try {
		autoPhonationTier thee = PhonationGrid_to_PhonationTier (me);
		autoSound him = PhonationGrid_PhonationTier_to_Sound_voiced (me, thee.peek(), samplingFrequency);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no voiced Sound created.");
	}
}

static Sound PhonationGrid_to_Sound (PhonationGrid me, CouplingGrid him, double samplingFrequency) {
	try {
		PhonationGridPlayOptions pp = my options;
		autoSound thee = 0;
		if (pp -> voicing) {
			if (him != 0 && his glottis -> points -> size > 0) {
				thee.reset (PhonationGrid_PhonationTier_to_Sound_voiced (me, his glottis, samplingFrequency));
			} else {
				thee.reset (PhonationGrid_to_Sound_voiced (me, samplingFrequency));
			}
			if (pp -> spectralTilt) {
				Sound_PhonationGrid_spectralTilt_inline (thee.peek(), me);
			}
		}
		if (pp -> aspiration) {
			autoSound aspiration = PhonationGrid_to_Sound_aspiration (me, samplingFrequency);
			if (thee.peek() == 0) {
				thee.reset (aspiration.transfer());
			} else {
				_Sounds_add_inline (thee.peek(), aspiration.peek());
			}
		}
		if (thee.peek() == 0) {
			thee.reset (Sound_createEmptyMono (my xmin, my xmax, samplingFrequency));
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Sound created.");
	}
}

static Ordered formantsAmplitudes_create (double tmin, double tmax, long numberOfFormants) {
	try {
		autoOrdered me = Ordered_create ();
		for (long i = 1; i <= numberOfFormants; i++) {
			autoIntensityTier a = IntensityTier_create (tmin, tmax);
			Collection_addItem (me.peek(), a.transfer());
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("No formants amplitudes created.");
	};
}

/********************** VocalTractGridPlayOptions **********************/

Thing_implement (VocalTractGridPlayOptions, Data, 0);

static void VocalTractGridPlayOptions_setDefaults (VocalTractGridPlayOptions me, VocalTractGrid thee) {
	my filterModel = KlattGrid_FILTER_CASCADE;
	my endOralFormant = MIN (thy oral_formants -> formants -> size, thy oral_formants -> bandwidths -> size);
	my startOralFormant = 1;
	my endNasalFormant = MIN (thy nasal_formants -> formants -> size, thy nasal_formants -> bandwidths -> size);
	my startNasalFormant = 1;
	my endNasalAntiFormant = MIN (thy nasal_antiformants -> formants -> size, thy nasal_antiformants -> bandwidths -> size);
	my startNasalAntiFormant = 1;
}

VocalTractGridPlayOptions VocalTractGridPlayOptions_create () {
	try {
		VocalTractGridPlayOptions me = Thing_new (VocalTractGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw ("VocalTractGridPlayOptions not created.");
	}
}

/********************** VocalTractGrid ***************************************/

static long FormantGrid_getNumberOfFormantPoints (FormantGrid me, long iformant) {
	if (iformant < 1 || iformant > my formants -> size) {
		return -1;
	}
	RealTier f = (RealTier) my formants -> item[iformant];
	return f -> points -> size;
}

static long FormantGrid_getNumberOfBandwidthPoints (FormantGrid me, long iformant) {
	if (iformant < 1 || iformant > my bandwidths -> size) {
		return -1;
	}
	RealTier b = (RealTier) my bandwidths -> item[iformant];
	return b -> points -> size;
}

static long Ordered_getNumberOfAmplitudePoints (Ordered me, long iformant) {
	if (me == 0 || iformant < 1 || iformant > my size) {
		return -1;
	}
	RealTier t = (RealTier) my item[iformant];
	return t -> points -> size;
}

static void FormantGrid_info (FormantGrid me, Ordered amplitudes, const wchar_t *in1, const wchar_t *in2) {
	long nformants = my formants -> size;
	long namplitudes = amplitudes != NULL ? amplitudes -> size : 0;
	long nmax = MAX (nformants, namplitudes);

	for (long iformant = 1; iformant <= nmax; iformant++) {
		MelderInfo_writeLine (in1, L"Formant ", Melder_integer (iformant), L":");
		if (iformant <= my formants -> size) {
			long nfp = FormantGrid_getNumberOfFormantPoints (me, iformant);
			long nbp = FormantGrid_getNumberOfBandwidthPoints (me, iformant);
			MelderInfo_writeLine (in2, L"formants:   ", (nfp >= 0 ? Melder_integer (nfp) : L"-- undefined --"));
			MelderInfo_writeLine (in2, L"bandwidths: ", (nbp >= 0 ? Melder_integer (nbp) : L"-- undefined --"));
		}
		if (amplitudes != NULL) {
			long nap = Ordered_getNumberOfAmplitudePoints (amplitudes, iformant);
			MelderInfo_writeLine (in2, L"amplitudes: ", (nap >= 0 ? Melder_integer (nap) : L"-- undefined --"));
		}
	}
}

void structVocalTractGrid :: v_info () {
	structData :: v_info ();
	const wchar_t *in1 = L"  ", *in2 = L"    ", *in3 = L"      ";
	MelderInfo_writeLine (in1, L"Time domain:");
	MelderInfo_writeLine (in2, L"Start time:     ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (in2, L"End time:       ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (in2, L"Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (in1, L"\nNumber of points in the ORAL FORMANT tiers:");
	FormantGrid_info (oral_formants, oral_formants_amplitudes, in2, in3);
	MelderInfo_writeLine (in1, L"\nNumber of points in the NASAL FORMANT tiers:");
	FormantGrid_info (nasal_formants, nasal_formants_amplitudes, in2, in3);
	MelderInfo_writeLine (in1, L"\nNumber of points in the NASAL ANTIFORMANT tiers:");
	FormantGrid_info (nasal_antiformants, NULL, in2, in3);
}

Thing_implement (VocalTractGrid, Function, 0);

void VocalTractGrid_setNames (VocalTractGrid me) {
	Thing_setName (my oral_formants, L"oral_formants");
	Thing_setName (my nasal_formants, L"nasal_formants");
	Thing_setName (my nasal_antiformants, L"nasal_antiformants");
	Thing_setName (my oral_formants_amplitudes, L"oral_formants_amplitudes");
	Thing_setName (my nasal_formants_amplitudes, L"nasal_formants_amplitudes");
}

VocalTractGrid VocalTractGrid_create (double tmin, double tmax, long numberOfFormants,
                                      long numberOfNasalFormants,	long numberOfNasalAntiFormants) {
	try {
		autoVocalTractGrid me = Thing_new (VocalTractGrid);
		Function_init (me.peek(), tmin, tmax);
		my oral_formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants);
		my nasal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalFormants);
		my nasal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalAntiFormants);
		my oral_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfFormants);
		my nasal_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfNasalFormants);
		my options = VocalTractGridPlayOptions_create ();
		VocalTractGrid_setNames (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("VocalTractGrid not created.");
	}
}

static void VocalTractGrid_CouplingGrid_drawCascade_inline (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double *yin, double *yout) {
	long numberOfOralFormants = my oral_formants -> formants -> size;
	long numberOfNasalFormants = my nasal_formants -> formants -> size;
	long numberOfNasalAntiFormants = my nasal_antiformants -> formants -> size;
	long numberOfTrachealFormants = thee != NULL ? thy tracheal_formants -> formants -> size : 0;
	long numberOfTrachealAntiFormants = thee != NULL ? thy tracheal_antiformants -> formants -> size : 0;
	double x1, y1 = ymin, x2, y2 = ymax, dx, ddx = 0.2, ymid = (y1 + y2) / 2;
	const wchar_t *text[6] = { 0, L"TF", L"TAF", L"NF", L"NAF", L""};
	long nf[6] = {0, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfNasalFormants, numberOfNasalAntiFormants, numberOfOralFormants};
	long numberOfXSections = 5, nsx = 0;
	MelderString ff = { 0 }, fb = { 0 };

	long numberOfFilters = numberOfNasalFormants + numberOfNasalAntiFormants + numberOfTrachealFormants + numberOfTrachealAntiFormants + numberOfOralFormants;

	if (numberOfFilters == 0) {
		x2 = xmax;
		Graphics_line (g, xmin, ymid, x2, ymid);
		goto end;
	}

	for (long isection = 1; isection <= numberOfXSections; isection++) if (nf[isection] > 0) {
			nsx++;
		}
	dx = (xmax - xmin) / (numberOfFilters  + (nsx - 1) * ddx);

	x1 = xmin;
	for (long isection = 1; isection <= numberOfXSections; isection++) {
		long numberOfFormants = nf[isection];

		if (numberOfFormants == 0) {
			continue;
		}

		x2 = x1 + dx;
		for (long i = 1; i <= numberOfFormants; i++) {
			MelderString_append (&ff, L"F", Melder_integer (i));
			MelderString_append (&fb, L"B", Melder_integer (i));
			draw_oneSection (g, x1, x2, y1, y2, text[isection], ff.string, fb.string);
			if (i < numberOfFormants) {
				x1 = x2;
				x2 = x1 + dx;
			}
			MelderString_empty (&ff); MelderString_empty (&fb);
		}

		if (isection < numberOfXSections) {
			x1 = x2; x2 = x1 + ddx * dx;
			Graphics_line (g, x1, ymid, x2, ymid);
			x1 = x2;
		}
	}
end:
	if (yin != 0) {
		*yin = ymid;
	}
	if (yout != 0) {
		*yout = ymid;
	}

	MelderString_free (&ff); MelderString_free (&fb);
}

static void VocalTractGrid_CouplingGrid_drawParallel_inline (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yin, double *yout) {
	// (0: filler) (1: hor. line to split) (2: split to diff) (3: diff) (4: diff to split)
	// (5: split to filter) (6: filters) (7: conn to summer) (8: summer)
	double xw[9] = { 0, 0.3, 0.2, 1.5, 0.5, 0.5, 1, 0.5, 0.5 }, xws[9];
	long numberOfXSections = 8, ic = 0, numberOfYSections = 4;
	long numberOfNasalFormants = my nasal_formants -> formants -> size;
	long numberOfOralFormants = my oral_formants -> formants -> size;
	long numberOfTrachealFormants = thee != NULL ? thy tracheal_formants -> formants -> size : 0;
	long numberOfFormants = numberOfNasalFormants + numberOfOralFormants + numberOfTrachealFormants;
	long numberOfUpperPartFormants = numberOfNasalFormants + (numberOfOralFormants > 0 ? 1 : 0);
	long numberOfLowerPartFormants = numberOfFormants - numberOfUpperPartFormants;
	double ddy = dy < 0 ? 0 : dy, x1, y1, x2, y2, x3, r, ymid;
	const wchar_t *text[5] = { 0, L"Nasal", L"", L"", L"Tracheal"};
	long nffrom[5] = {0, 1, 1, 2, 1};
	long nfto[5] = {0, numberOfNasalFormants, (numberOfOralFormants > 0 ? 1 : 0), numberOfOralFormants, numberOfTrachealFormants};
	autoMelderString fba;

	rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

	if (numberOfFormants == 0) {
		y1 = y2 = (ymin + ymax) / 2;
		Graphics_line (g, xmin, y1, xmax, y1);
		if (yin != NULL) {
			*yin = y1;
		}
		if (yout != NULL) {
			*yout = y2;
		}
		return;
	}

	dy = (ymax - ymin) / (numberOfFormants * (1 + ddy) - ddy);

	connections local_in = connections_create (numberOfFormants);
	connections local_out = connections_create (numberOfFormants);

	// parallel section
	x1 = xmin + xws[5]; x2 = x1 + xw[6]; y2 = ymax;
	x3 = xmin + xws[4];
	for (long isection = 1; isection <= numberOfYSections; isection++) {
		long ifrom = nffrom[isection], ito = nfto[isection];
		if (ito < ifrom) {
			continue;
		}
		for (long i = ifrom; i <= ito; i++) {
			y1 = y2 - dy; ymid = (y1 + y2) / 2;
			const wchar_t *fi = Melder_integer (i);
			MelderString_append (&fba, L"A", fi, L" F", fi, L" B", fi);
			draw_oneSection (g, x1, x2, y1, y2, text[isection], fba.string, NULL);
			Graphics_line (g, x3, ymid, x1, ymid); // to the left
			ic++;
			local_in -> x[ic] = x3; local_out -> x[ic] = x2;
			local_in -> y[ic] = local_out -> y[ic] = ymid;
			y2 = y1 - 0.5 * dy;
			MelderString_empty (&fba);
		}
	}

	ic = 0;
	x1 = local_in -> y[1];
	if (numberOfUpperPartFormants > 0) {
		x1 = local_in -> x[numberOfUpperPartFormants]; y1 = local_in -> y[numberOfUpperPartFormants];
		if (numberOfUpperPartFormants > 1) {
			Graphics_line (g, x1, y1, local_in -> x[1], local_in -> y[1]);    // vertical
		}
		x2 = xmin;
		if (numberOfLowerPartFormants > 0) {
			x2 += xw[1];
		}
		Graphics_line (g, x1, y1, x2, y1); // done
	}
	if (numberOfLowerPartFormants > 0) {
		long ifrom = numberOfUpperPartFormants + 1;
		x1 = local_in -> x[ifrom]; y1 = local_in -> y[ifrom]; // at the split
		if (numberOfLowerPartFormants > 1) {
			Graphics_line (g, x1, y1, local_in -> x[numberOfFormants], local_in -> y[numberOfFormants]);    // vertical
		}
		x2 = xmin + xws[3]; // right of diff
		Graphics_line (g, x1, y1, x2, y1); // from vertical to diff
		x1 = xmin + xws[2]; // left of diff
		draw_oneSection (g, x1, x2, y1 + 0.5 * dy, y1 - 0.5 * dy, L"Pre-emphasis", NULL, NULL);
		x2 = x1;
		if (numberOfUpperPartFormants > 0) {
			x2 = xmin + xw[1]; y2 = y1; // at split
			Graphics_line (g, x1, y1, x2, y1); // to split
			y1 += (1 + ddy) * dy;
			Graphics_line (g, x2, y2, x2, y1); // vertical
			y1 -= 0.5 * (1 + ddy) * dy;
		}
		Graphics_line (g, xmin, y1, x2, y1);
	}

	r = xw[8] / 2;
	x2 = xmax - r; y2 = (ymin + ymax) / 2;

	alternatingSummer_drawConnections (g, x2, y2, r, local_out, 1, 0.4);

	connections_free (local_out); connections_free (local_in);

	if (yin != NULL) {
		*yin = y1;
	}
	if (yout != NULL) {
		*yout = y2;
	}
}

static void VocalTractGrid_CouplingGrid_draw_inside (VocalTractGrid me, CouplingGrid thee, Graphics g, int filterModel, double xmin, double xmax, double ymin, double ymax, double dy, double *yin, double *yout) {
	filterModel == KlattGrid_FILTER_CASCADE ?
	VocalTractGrid_CouplingGrid_drawCascade_inline (me, thee, g, xmin, xmax, ymin, ymax, yin, yout) :
	VocalTractGrid_CouplingGrid_drawParallel_inline (me, thee, g, xmin, xmax, ymin, ymax, dy, yin, yout);
}

static void VocalTractGrid_CouplingGrid_draw (VocalTractGrid me, CouplingGrid thee, Graphics g, int filterModel) {
	double xmin = 0, xmin1 = 0.05, xmax1 = 0.95, xmax = 1, ymin = 0, ymax = 1, dy = 0.5, yin, yout;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);
	VocalTractGrid_CouplingGrid_draw_inside (me, thee, g, filterModel, xmin1, xmax1, ymin, ymax, dy, &yin, &yout);
	Graphics_line (g, xmin, yin, xmin1, yin);
	Graphics_arrow (g, xmax1, yout, xmax, yout);
	Graphics_unsetInner (g);
}

static Sound Sound_VocalTractGrid_CouplingGrid_filter_cascade (Sound me, VocalTractGrid thee, CouplingGrid coupling) {
	try {
		VocalTractGridPlayOptions pv = thy options;
		CouplingGridPlayOptions pc = coupling -> options;
		int useOpenGlottisInfo = pc -> openglottis && coupling && coupling -> glottis && coupling -> glottis -> points -> size > 0;
		FormantGrid oral_formants = thy oral_formants;
		FormantGrid nasal_formants = thy nasal_formants;
		FormantGrid nasal_antiformants = thy nasal_antiformants;
		FormantGrid tracheal_formants = coupling -> tracheal_formants;
		FormantGrid tracheal_antiformants = coupling -> tracheal_antiformants;

		int antiformants = 0;
		long numberOfFormants = oral_formants -> formants -> size;
		long numberOfTrachealFormants = tracheal_formants -> formants -> size;
		long numberOfTrachealAntiFormants = tracheal_antiformants -> formants -> size;
		long numberOfNasalFormants = nasal_formants -> formants -> size;
		long numberOfNasalAntiFormants = nasal_antiformants -> formants -> size;
		check_formants (numberOfFormants, & (pv -> startOralFormant), & (pv -> endOralFormant));
		check_formants (numberOfNasalFormants, & (pv -> startNasalFormant), & (pv -> endNasalFormant));
		check_formants (numberOfTrachealFormants, & (pc -> startTrachealFormant), & (pc -> endTrachealFormant));
		check_formants (numberOfNasalAntiFormants, & (pv -> startNasalAntiFormant), & (pv -> endNasalAntiFormant));
		check_formants (numberOfTrachealAntiFormants, & (pc -> startTrachealAntiFormant), & (pc -> endTrachealAntiFormant));

		autoSound him = Data_copy (me);

		autoFormantGrid formants = 0;
		if (useOpenGlottisInfo) {
			formants.reset (Data_copy (thy oral_formants));
			FormantGrid_CouplingGrid_updateOpenPhases (formants.peek(), coupling);
		}

		long nasal_formant_warning = 0, any_warning = 0;
		if (pv -> endNasalFormant > 0) { // Nasal formants
			antiformants = 0;
			for (long iformant = pv -> startNasalFormant; iformant <= pv -> endNasalFormant; iformant++) {
				if (FormantGrid_isFormantDefined (thy nasal_formants, iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inline (him.peek(), thy nasal_formants, iformant, antiformants);
				} else {
					// Melder_warning ("Nasal formant", iformant, ": frequency and/or bandwidth missing.");
					nasal_formant_warning++; any_warning++;
				}
			}
		}

		long nasal_antiformant_warning = 0;
		if (pv -> endNasalAntiFormant > 0) { // Nasal anti formants
			antiformants = 1;
			for (long iformant = pv -> startNasalAntiFormant; iformant <= pv -> endNasalAntiFormant; iformant++) {
				if (FormantGrid_isFormantDefined (thy nasal_antiformants, iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inline (him.peek(), thy nasal_antiformants, iformant, antiformants);
				} else {
					// Melder_warning ("Nasal antiformant", iformant, ": frequency and/or bandwidth missing.");
					nasal_antiformant_warning++; any_warning++;
				}
			}
		}

		long tracheal_formant_warning = 0;
		if (pc -> endTrachealFormant > 0) { // Tracheal formants
			antiformants = 0;
			for (long iformant = pc -> startTrachealFormant; iformant <= pc -> endTrachealFormant; iformant++) {
				if (FormantGrid_isFormantDefined (tracheal_formants, iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inline (him.peek(), tracheal_formants, iformant, antiformants);
				} else {
					// Melder_warning ("Tracheal formant", iformant, ": frequency and/or bandwidth missing.");
					tracheal_formant_warning++; any_warning++;
				}
			}
		}

		long tracheal_antiformant_warning = 0;
		if (pc -> endTrachealAntiFormant > 0) { // Tracheal anti formants
			antiformants = 1;
			for (long iformant = pc -> startTrachealAntiFormant; iformant <= pc -> endTrachealAntiFormant; iformant++) {
				if (FormantGrid_isFormantDefined (tracheal_antiformants, iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inline (him.peek(), tracheal_antiformants, iformant, antiformants);
				} else {
					// Melder_warning ("Tracheal antiformant", iformant, ": frequency and/or bandwidth missing.");
					tracheal_antiformant_warning++; any_warning++;
				}
			}
		}

		long oral_formant_warning = 0;
		if (pv -> endOralFormant > 0) { // Oral formants
			antiformants = 0;
			if (formants.peek() == 0) {
				formants.reset (thy oral_formants);
			}
			for (long iformant = pv -> startOralFormant; iformant <= pv -> endOralFormant; iformant++) {
				if (FormantGrid_isFormantDefined (formants.peek(), iformant)) {
					_Sound_FormantGrid_filterWithOneFormant_inline (him.peek(), formants.peek(), iformant, antiformants);
				} else {
					// Melder_warning ("Oral formant", iformant, ": frequency and/or bandwidth missing.");
					oral_formant_warning++; any_warning++;
				}
			}
		}
		if (any_warning > 0)
		{
			autoMelderString warning;
			if (nasal_formant_warning > 0) {
				MelderString_append (&warning, L"\tNasal formants: one or more are missing.\n");
			}
			if (nasal_antiformant_warning) {
				MelderString_append (&warning, L"\tNasal antiformants: one or more are missing.\n");
			}
			if (tracheal_formant_warning) {
				MelderString_append (&warning, L"\tTracheal formants: one or more are missing.\n");
			}
			if (tracheal_antiformant_warning) {
				MelderString_append (&warning, L"\tTracheal antiformants: one or more are missing.\n");
			}
			if (oral_formant_warning) {
				MelderString_append (&warning, L"\tOral formants: one or more are missing.\n");
			}
			MelderInfo_open();
            MelderInfo_writeLine (L"Warning:");
			MelderInfo_writeLine (warning.string);
            MelderInfo_close();
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered by vocaltract and coupling grid.");
	}
}

Sound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling) {
	try {
		VocalTractGridPlayOptions pv = thy options;
		CouplingGridPlayOptions pc = coupling -> options;
		autoSound him = 0;
		FormantGrid oral_formants = thy oral_formants; autoFormantGrid aof = 0;
		int alternatingSign = 0; // 0: no alternating signs in parallel adding of filter outputs, 1/-1 start sign
		int useOpenGlottisInfo = pc -> openglottis && coupling -> glottis && coupling -> glottis -> points -> size > 0;
		int scale = 1;
		long numberOfFormants = thy oral_formants -> formants -> size;
		long numberOfNasalFormants = thy nasal_formants -> formants -> size;
		long numberOfTrachealFormants = coupling -> tracheal_formants -> formants -> size;

		check_formants (numberOfFormants, & (pv -> startOralFormant), & (pv -> endOralFormant));
		check_formants (numberOfNasalFormants, & (pv -> startNasalFormant), & (pv -> endNasalFormant));
		check_formants (numberOfTrachealFormants, & (pc -> startTrachealFormant), & (pc -> endTrachealFormant));

		if (useOpenGlottisInfo) {
			aof.reset (Data_copy (thy oral_formants)); oral_formants = aof.peek();
			FormantGrid_CouplingGrid_updateOpenPhases (oral_formants, coupling);
		}

		if (pv -> endOralFormant > 0) {
			if (pv -> startOralFormant == 1) {
				him.reset (Data_copy (me));
				if (oral_formants -> formants -> size > 0) {
					Sound_FormantGrid_Intensities_filterWithOneFormant_inline (him.peek(), oral_formants, thy oral_formants_amplitudes, 1);
				}
			}
		}

		if (pv -> endNasalFormant > 0) {
			alternatingSign = 0;
			autoSound nasal =  Sound_FormantGrid_Intensities_filter (me, thy nasal_formants, thy nasal_formants_amplitudes, pv -> startNasalFormant, pv -> endNasalFormant, alternatingSign);

			if (him.peek() == 0) {
				him.reset (Data_copy (nasal.peek()));
			} else {
				_Sounds_add_inline (him.peek(), nasal.peek());
			}
		}

		// Formants 2 and up, with alternating signs.
		// We perform pre-emphasis by differentiating.
		// Klatt (1980) states that a first difference for the higher formants is necessary to remove low-frequency
		// energy from them. This energy would otherwise distort the spectrum in the region of F1 during the synthesis
		// of some vowels.

		autoSound me_diff = _Sound_diff (me, scale);

		if (pv -> endOralFormant >= 2) {
			long startOralFormant2 = pv -> startOralFormant > 2 ? pv -> startOralFormant : 2;
			alternatingSign = startOralFormant2 % 2 == 0 ? -1 : 1; // 2 starts with negative sign
			if (startOralFormant2 <= oral_formants -> formants -> size) {
				autoSound vocalTract = Sound_FormantGrid_Intensities_filter (me_diff.peek(), oral_formants, thy oral_formants_amplitudes, startOralFormant2, pv -> endOralFormant, alternatingSign);

				if (him.peek() == 0) {
					him.reset (Data_copy (vocalTract.peek()));
				} else {
					_Sounds_add_inline (him.peek(), vocalTract.peek());
				}
			}
		}

		if (pc -> endTrachealFormant > 0) { // Tracheal formants
			alternatingSign = 0;
			autoSound trachea =  Sound_FormantGrid_Intensities_filter (me_diff.peek(), coupling -> tracheal_formants, coupling -> tracheal_formants_amplitudes,
			                     pc -> startTrachealFormant, pc -> endTrachealFormant, alternatingSign);

			if (him.peek() == 0) {
				him.reset (Data_copy (trachea.peek()));
			} else {
				_Sounds_add_inline (him.peek(), trachea.peek());
			}
		}

		if (him.peek() == 0) {
			him.reset (Data_copy (me));
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered in parallel.");
	}
}

Sound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling) {
	return thy options -> filterModel == KlattGrid_FILTER_CASCADE ?
	       Sound_VocalTractGrid_CouplingGrid_filter_cascade (me, thee, coupling) :
	       Sound_VocalTractGrid_CouplingGrid_filter_parallel (me, thee, coupling);
}

/********************** CouplingGridPlayOptions **********************/

Thing_implement (CouplingGridPlayOptions, Data, 0);

static void CouplingGridPlayOptions_setDefaults (CouplingGridPlayOptions me, CouplingGrid thee) {
	my fadeFraction = 0.1;
	my openglottis = 1;
	my endTrachealFormant = MIN (thy tracheal_formants -> formants -> size, thy tracheal_formants -> bandwidths -> size);
	my startTrachealFormant = 1;
	my endTrachealAntiFormant = MIN (thy tracheal_antiformants -> formants -> size, thy tracheal_antiformants -> bandwidths -> size);
	my startTrachealAntiFormant = 1;
	my startDeltaFormant = 1;
	my endDeltaFormant = thy delta_formants -> formants -> size;
	my startDeltaBandwidth = 1;
	my endDeltaBandwidth = thy delta_formants -> bandwidths -> size;
}

CouplingGridPlayOptions CouplingGridPlayOptions_create () {
	try {
		CouplingGridPlayOptions me = Thing_new (CouplingGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw ("CouplingGridPlayOptions not created.");
	}
}

/********************** CouplingGrid *************************************/

Thing_implement (CouplingGrid, Function, 0);

void structCouplingGrid :: v_info () {
	structData :: v_info ();
	const wchar_t *in1 = L"  ", *in2 = L"    ", *in3 = L"      ";
	MelderInfo_writeLine (in1, L"Time domain:");
	MelderInfo_writeLine (in2, L"Start time:     ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (in2, L"End time:       ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (in2, L"Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (in1, L"\nNumber of points in the TRACHEAL FORMANT tiers:");
	FormantGrid_info (tracheal_formants, tracheal_formants_amplitudes, in2, in3);
	MelderInfo_writeLine (in1, L"\nNumber of points in the TRACHEAL ANTIFORMANT tiers:");
	FormantGrid_info (tracheal_antiformants, NULL, in2, in3);
	MelderInfo_writeLine (in1, L"\nNumber of points in the DELTA FORMANT tiers:");
	FormantGrid_info (delta_formants, NULL, in2, in3);
}

void CouplingGrid_setNames (CouplingGrid me) {
	Thing_setName (my tracheal_formants, L"tracheal_formants");
	Thing_setName (my tracheal_antiformants, L"tracheal_antiformants");
	Thing_setName (my tracheal_formants_amplitudes, L"tracheal_formants_amplitudes");
	Thing_setName (my delta_formants, L"delta_formants");
	Thing_setName (my glottis, L"glottis");
}

CouplingGrid CouplingGrid_create (double tmin, double tmax, long numberOfTrachealFormants, long numberOfTrachealAntiFormants, long numberOfDeltaFormants) {
	try {
		autoCouplingGrid me = Thing_new (CouplingGrid);
		Function_init (me.peek(), tmin, tmax);
		my tracheal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealFormants);
		my tracheal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealAntiFormants);
		my tracheal_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfTrachealFormants);
		my delta_formants = FormantGrid_createEmpty (tmin, tmax, numberOfDeltaFormants);
		my glottis = PhonationTier_create (tmin, tmax);
		my options = CouplingGridPlayOptions_create ();
		CouplingGrid_setNames (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("CouplingGrid not created.");
	}
}

/********************** FormantGrid & CouplingGrid *************************************/

void FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee) {
	try {
		CouplingGridPlayOptions pc = thy options;
		for (long itier = 1; itier <= thy delta_formants -> formants -> size; itier++) {
			RealTier delta = (RealTier) thy delta_formants -> formants -> item[itier];
			if (itier <= my formants -> size) {
				if (delta -> points -> size > 0) {
					autoRealTier rt = RealTier_updateWithDelta ( (RealTier) my formants -> item[itier], delta, thy glottis, pc -> fadeFraction);
					if (! RealTier_valuesInRange (rt.peek(), 0, NUMundefined)) {
						Melder_throw ("Formant ",  itier, " coupling gives negative values.");
					}
					forget ( ( (Thing *) my formants -> item) [itier]);
					my formants -> item[itier] = rt.transfer();
				}
			}
			delta = (RealTier) thy delta_formants -> bandwidths -> item[itier];
			if (itier <= my bandwidths -> size) {
				if (delta -> points -> size > 0) {
					autoRealTier rt = RealTier_updateWithDelta ( (RealTier) my bandwidths -> item[itier], delta, thy glottis, pc -> fadeFraction);
					if (! RealTier_valuesInRange (rt.peek(), 0, NUMundefined)) {
						Melder_throw ("Bandwidth ", itier, " coupling gives negative values.");
					}
					forget ( ( (Thing *) my bandwidths -> item) [itier]);
					my bandwidths -> item[itier] = rt.transfer();
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not updated with open hase information.");
	}
}

/************************ Sound & FormantGrid *********************************************/

void _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, thou, long iformant, int antiformant) {
	thouart (FormantGrid);
	if (iformant < 1 || iformant > thy formants -> size) {
		Melder_warning (L"Formant ", Melder_integer (iformant), L" does not exist.");
		return;
	}

	RealTier ftier = (RealTier) thy formants -> item[iformant];
	RealTier btier = (RealTier) thy bandwidths -> item[iformant];

	if (ftier -> points -> size == 0 && btier -> points -> size == 0) {
		return;
	} else if (ftier -> points -> size == 0 || btier -> points -> size == 0) {
		Melder_throw ("Empty tier");
	}

	double nyquist = 0.5 / my dx;
	autoFilter r =  antiformant != 0 ? (Filter) AntiResonator_create (my dx) : (Filter) Resonator_create (my dx, Resonator_NORMALISATION_H0);

	for (long is = 1; is <= my nx; is++) {
		double t = my x1 + (is - 1) * my dx;
		double f = RealTier_getValueAtTime (ftier, t);
		double b = RealTier_getValueAtTime (btier, t);
		if (f <= nyquist && NUMdefined (b)) {
			Filter_setFB (r.peek(), f, b);
		}
		my z[1][is] = Filter_getOutput (r.peek(), my z[1][is]);
	}
}

void Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant) {
	_Sound_FormantGrid_filterWithOneFormant_inline (me, thee, iformant, 1);
}

void Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant) {
	_Sound_FormantGrid_filterWithOneFormant_inline (me, thee, iformant, 0);
}

void Sound_FormantGrid_Intensities_filterWithOneFormant_inline (Sound me, FormantGrid thee, Ordered amplitudes, long iformant) {
	try {
		if (iformant < 1 || iformant > thy formants -> size) {
			Melder_throw ("Formant ",  iformant, " not defined. \nThis formant will not be used.");
		}
		double nyquist = 0.5 / my dx;

		RealTier ftier = (RealTier) thy formants -> item[iformant];
		RealTier btier = (RealTier) thy bandwidths -> item[iformant];
		RealTier atier = (RealTier) amplitudes -> item[iformant];

		if (ftier -> points -> size == 0 || btier -> points -> size == 0 || atier -> points -> size == 0) {
			return;    // nothing to do
		}

		autoResonator r = Resonator_create (my dx, Resonator_NORMALISATION_HMAX);

		for (long is = 1; is <= my nx; is++) {
			double t = my x1 + (is - 1) * my dx;
			double f = RealTier_getValueAtTime (ftier, t);
			double b = RealTier_getValueAtTime (btier, t);
			double a;
			if (f <= nyquist && NUMdefined (b)) {
				Filter_setFB (r.peek(), f, b);
				a = RealTier_getValueAtTime (atier, t);
				if (NUMdefined (a)) {
					r -> a *= DB_to_A (a);
				}
			}
			my z[1][is] = Filter_getOutput (r.peek(), my z[1][is]);
		}
	} catch (MelderError) {
		Melder_throw (me, ": not filtered with one formant filter.");
	}
}

Sound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, Ordered amplitudes, long iformantb, long iformante, int alternatingSign) {
	try {
		if (iformantb > iformante) {
			iformantb = 1;
			iformante = thy formants -> size;
		}
		if (iformantb < 1 || iformantb > thy formants -> size ||
		        iformante < 1 || iformante > thy formants -> size) {
			Melder_throw ("No such formant number.");
		}

		autoSound him = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);

		for (long iformant = iformantb; iformant <= iformante; iformant++) {
			if (FormantGrid_Intensities_isFormantDefined (thee, amplitudes, iformant)) {
				autoSound tmp = Data_copy (me);
				Sound_FormantGrid_Intensities_filterWithOneFormant_inline (tmp.peek(), thee, amplitudes, iformant);
				for (long is = 1; is <= my nx; is++) {
					his z[1][is] += alternatingSign >= 0 ? tmp -> z[1][is] : - tmp -> z[1][is];
				}
				if (alternatingSign != 0) {
					alternatingSign = -alternatingSign;
				}
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered.");
	}
}

/********************** FricationGridPlayOptions **********************/

Thing_implement (FricationGridPlayOptions, Data, 0);

static void FricationGridPlayOptions_setDefaults (FricationGridPlayOptions me, FricationGrid thee) {
	my endFricationFormant = MIN (thy frication_formants -> formants -> size, thy frication_formants -> bandwidths -> size);
	my startFricationFormant = 2;
	my bypass = 1;
}

FricationGridPlayOptions FricationGridPlayOptions_create () {
	try {
		FricationGridPlayOptions me = Thing_new (FricationGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw ("FricationGridPlayOptions not created.");
	}
}

/************************ FricationGrid (& Sound) *********************************************/

void structFricationGrid :: v_info () {
	structData :: v_info ();
	const static wchar_t *in1 = L"  ", *in2 = L"    ", *in3 = L"      ";
	MelderInfo_writeLine (in1, L"Time domain:");
	MelderInfo_writeLine (in2, L"Start time:     ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (in2, L"End time:       ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (in2, L"Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (in1, L"\nNumber of points in the FRICATION tiers:");
	MelderInfo_writeLine (in2, L"fricationAmplitude:  ", Melder_integer (fricationAmplitude -> points -> size));
	MelderInfo_writeLine (in2, L"bypass:              ", Melder_integer (bypass -> points -> size));
	MelderInfo_writeLine (in1, L"\nNumber of points in the FRICATION FORMANT tiers:");
	FormantGrid_info (frication_formants, frication_formants_amplitudes, in2, in3);
}

Thing_implement (FricationGrid, Function, 0);

void FricationGrid_setNames (FricationGrid me) {
	Thing_setName (my fricationAmplitude, L"fricationAmplitude");
	Thing_setName (my frication_formants, L"frication_formants");
	Thing_setName (my bypass, L"bypass");
	Thing_setName (my frication_formants_amplitudes, L"frication_formants_amplitudes");
}

FricationGrid FricationGrid_create (double tmin, double tmax, long numberOfFormants) {
	try {
		autoFricationGrid me = Thing_new (FricationGrid);
		Function_init (me.peek(), tmin, tmax);
		my fricationAmplitude = IntensityTier_create (tmin, tmax);
		my frication_formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants);
		my bypass = IntensityTier_create (tmin, tmax);
		my frication_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfFormants);
		my options = FricationGridPlayOptions_create ();
		FricationGrid_setNames (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FricationGrid not created.");
	}
}

static void FricationGrid_draw_inside (FricationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout) {
	long numberOfXSections = 5;
	long numberOfFormants = my frication_formants -> formants -> size;
	long numberOfParts = numberOfFormants + (numberOfFormants > 1 ? 0 : 1) ; // 2..number + bypass
	// dum noise, connections, filter, connections, adder
	double xw[6] = { 0, 2, 0.6, 1.5, 0.6, 0.5 }, xws[6];
	double r, x1, y1, x2, y2, x3, xs, ys, ymid = (ymin + ymax) / 2;

	rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

	dy = dy < 0 ? 0 : dy;
	dy = (ymax - ymin) / (numberOfParts * (1 + dy) - dy);

	connections cp = connections_create (numberOfParts);
	if (cp == 0) {
		return;
	}

	// section 1
	x1 = xmin; x2 = x1 + xw[1]; y1 = ymid - 0.5 * dy; y2 = y1 + dy;
	draw_oneSection (g, x1, x2, y1, y2, L"Frication", L"noise", NULL);

	// section 2, horizontal line halfway, vertical line
	x1 = x2; x2 = x1 + xw[2] / 2;
	Graphics_line (g, x1, ymid, x2, ymid);
	Graphics_line (g, x2, ymax - dy / 2, x2, ymin + dy / 2);
	x3 = x2;
	// final connection to section 2 , filters , connections to adder
	x1 = xmin + xws[2]; x2 = x1 + xw[3]; y2 = ymax;
	MelderString fba = { 0 };
	for (long i = 1; i <= numberOfParts; i++) {
		const wchar_t *fi = Melder_integer (i + 1);
		y1 = y2 - dy;
		if (i < numberOfParts) {
			MelderString_append (&fba, L"A", fi, L" F", fi, L" B", fi);
		} else {
			MelderString_append (&fba,  L"Bypass");
		}
		draw_oneSection (g, x1, x2, y1, y2, NULL, fba.string, NULL);
		double ymidi = (y1 + y2) / 2;
		Graphics_line (g, x3, ymidi, x1, ymidi); // from noise to filter
		cp -> x[i] = x2; cp -> y[i] = ymidi;
		y2 = y1 - 0.5 * dy;
		MelderString_empty (&fba);
	}

	r = xw[5] / 2;
	xs = xmax - r; ys = ymid;

	if (numberOfParts > 1) {
		alternatingSummer_drawConnections (g, xs, ys, r, cp, 1, 0.4);
	} else {
		Graphics_line (g, cp -> x[1], cp -> y[1], xs + r, ys);
	}

	connections_free (cp);

	if (yout != NULL) {
		*yout = ys;
	}
	MelderString_free (&fba);
}

void FricationGrid_draw (FricationGrid me, Graphics g) {
	double xmin = 0, xmax = 1, xmax2 = 0.9, ymin = 0, ymax = 1, dy = 0.5, yout;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);

	FricationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, &yout);

	Graphics_arrow (g, xmax2, yout, xmax, yout);
	Graphics_unsetInner (g);
}

Sound FricationGrid_to_Sound (FricationGrid me, double samplingFrequency) {
	try {
		autoSound thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);

		double lastval = 0;
		for (long i = 1; i <= thy nx; i++) {
			double t = thy x1 + (i - 1) * thy dx;
			double val = NUMrandomUniform (-1, 1);
			double a = 0;
			if (my fricationAmplitude -> points -> size > 0) {
				double dba = RealTier_getValueAtTime (my fricationAmplitude, t);
				a = dba == NUMundefined ? 0 : DBSPL_to_A (dba);
			}
			lastval = (val += 0.75 * lastval); // TODO: soft low-pass coefficient must be Fs dependent!
			thy z[1][i] = val * a;
		}

		autoSound him = Sound_FricationGrid_filter (thee.peek(), me);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no frication Sound created.");
	}
}

/************************ Sound & FricationGrid *********************************************/

Sound Sound_FricationGrid_filter (Sound me, FricationGrid thee) {
	try {
		FricationGridPlayOptions pf = thy options;
		autoSound him = 0;
		long numberOfFricationFormants = thy frication_formants -> formants -> size;

		check_formants (numberOfFricationFormants, & (pf -> startFricationFormant), & (pf -> endFricationFormant));

		if (pf -> endFricationFormant > 1) {
			long startFricationFormant2 = pf -> startFricationFormant > 2 ? pf -> startFricationFormant : 2;
			int alternatingSign = startFricationFormant2 % 2 == 0 ? 1 : -1; // 2 starts with positive sign
			him.reset (Sound_FormantGrid_Intensities_filter (me, thy frication_formants, thy frication_formants_amplitudes, startFricationFormant2, pf -> endFricationFormant, alternatingSign));
		}

		if (him.peek() == 0) {
			him.reset (Data_copy (me));
		}

		if (pf -> bypass) {
			for (long is = 1; is <= his nx; is++) {	// Bypass
				double t = his x1 + (is - 1) * his dx;
				double ab = 0;
				if (thy bypass -> points -> size > 0) {
					double val = RealTier_getValueAtTime (thy bypass, t);
					ab = val == NUMundefined ? 0 : DB_to_A (val);
				}
				his z[1][is] += my z[1][is] * ab;
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered by frication filter.");
	}
}

/********************** KlattGridPlayOptions **********************/

Thing_implement (KlattGridPlayOptions, Data, 0);

static void KlattGridPlayOptions_setDefaults (KlattGridPlayOptions me, KlattGrid thee) {
	my samplingFrequency = 44100;
	my scalePeak = 1;
	my xmin = thy xmin;
	my xmax = thy xmax;
}

KlattGridPlayOptions KlattGridPlayOptions_create () {
	try {
		KlattGridPlayOptions me = Thing_new (KlattGridPlayOptions);
		return me;
	} catch (MelderError) {
		Melder_throw ("KlattGridPlayOptions not created.");
	}
}

void KlattGrid_setDefaultPlayOptions (KlattGrid me) {
	KlattGridPlayOptions_setDefaults (my options, me);
	PhonationGridPlayOptions_setDefaults (my phonation -> options);
	VocalTractGridPlayOptions_setDefaults (my vocalTract -> options, my vocalTract);
	CouplingGridPlayOptions_setDefaults (my coupling -> options, my coupling);
	FricationGridPlayOptions_setDefaults (my frication -> options, my frication);
}

/************************ KlattGrid *********************************************/

Thing_implement (KlattGrid, Function, 0);

void structKlattGrid :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time:     ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time:       ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"\n--- PhonationGrid ---\n");
	phonation -> v_info ();
	MelderInfo_writeLine (L"\n--- VocalTractGrid ---\n");
	vocalTract -> v_info ();
	MelderInfo_writeLine (L"\n--- CouplingGrid ---\n");
	coupling -> v_info ();
	MelderInfo_writeLine (L"\n--- FricationgGrid ---\n");
	frication -> v_info ();
}

void KlattGrid_setNames (KlattGrid me) {
	Thing_setName (my phonation, L"phonation");
	Thing_setName (my vocalTract, L"vocalTract");
	Thing_setName (my coupling, L"coupling");
	Thing_setName (my frication, L"frication");
	Thing_setName (my gain, L"gain");
}

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants,
                            long numberOfNasalFormants,	long numberOfNasalAntiFormants,
                            long numberOfTrachealFormants, long numberOfTrachealAntiFormants,
                            long numberOfFricationFormants, long numberOfDeltaFormants) {
	try {
		autoKlattGrid me = Thing_new (KlattGrid);
		Function_init (me.peek(), tmin, tmax);
		my phonation = PhonationGrid_create (tmin, tmax);
		my vocalTract = VocalTractGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants, numberOfNasalAntiFormants);
		my coupling = CouplingGrid_create (tmin, tmax, numberOfTrachealFormants,  numberOfTrachealAntiFormants, numberOfDeltaFormants);
		my frication = FricationGrid_create (tmin, tmax, numberOfFricationFormants);
		my gain = IntensityTier_create (tmin, tmax);
		my options = KlattGridPlayOptions_create ();

		KlattGrid_setDefaultPlayOptions (me.peek());
		KlattGrid_setNames (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid not created.");
	}
}

KlattGrid KlattGrid_createExample () {
	try {
		autoKlattTable thee = KlattTable_createExample ();
		autoKlattGrid me = KlattTable_to_KlattGrid (thee.peek(), 0.005);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("KlattGrid example not created.");
	};
}

// y is the heigth in units of the height of one section,
// y1 is the heigth from the top to the split between the uppper, non-diffed, and lower diffed part
static void _KlattGrid_queryParallelSplit (KlattGrid me, double dy, double *y, double *y1) {
	long ny = my vocalTract -> nasal_formants -> formants -> size + my vocalTract -> oral_formants -> formants -> size + my coupling -> tracheal_formants -> formants -> size;
	long n1 = my vocalTract -> nasal_formants -> formants -> size + (my vocalTract -> oral_formants -> formants -> size > 0 ? 1 : 0);

	long n2 = ny - n1;
	if (ny == 0) {
		*y = 0;
		*y1 = 0;
		return;
	}

	*y = ny + (ny - 1) * dy;

	if (n1 == 0) {
		*y1 = 0.5;
	} else if (n2 == 0) {
		*y1 = *y - 0.5;
	} else {
		*y1 = n1 + (n1 - 1) * dy + 0.5 * dy;
	}
	return;
}

static void getYpositions (double h1, double h2, double h3, double h4, double h5, double fractionOverlap, double *dy, double *ymin1, double *ymax1, double *ymin2, double *ymax2, double *ymin3, double *ymax3) {
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
	double h13 = h1 > h3 ? h1 : h3; // baselines are now equal
	if (h2 >= h4) {
		h = h13 + h2 + h5;
	} else { // h2 < h4
		double maximumOverlap3 = fractionOverlap * h5;
		if (maximumOverlap3 < (h1 + h2)) {
			maximumOverlap3 = 0;
		} else if (maximumOverlap3 > (h4 - h2)) {
			maximumOverlap3 = h4 - h2;
		}
		h = h13 + h4 + h5 - maximumOverlap3;
	}
	*dy = 1 / (1.1 * h);
	*ymin1 = 1 - (h13 + h2) * *dy; *ymax1 = *ymin1 + (h1 + h2) * *dy;
	*ymin2 = 1 - (h13 + h4) * *dy; *ymax2 = *ymin2 + (h3 + h4) * *dy;
	*ymin3 = 0;  *ymax3 = h5 * *dy;
}

void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, int filterModel, int withTrachea) {
	VocalTractGrid_CouplingGrid_draw (my vocalTract, withTrachea ? my coupling : NULL, g, filterModel);
}

void KlattGrid_draw (KlattGrid me, Graphics g, int filterModel) {
	double xs1, xs2, ys1, ys2, xf1, xf2, yf1, yf2;
	double xp1, xp2, yp1, yp2, xc1, xc2, yc1, yc2;
	double dy, r, xs, ys;
	double xmin = 0, xmax2 = 0.90, xmax3 = 0.95, xmax = 1, ymin = 0, ymax = 1;
	double xws[6];
	double height_phonation = 0.3;
	double dy_phonation = 0.5, dy_vocalTract_p = 0.5, dy_frication = 0.5;

	connections tf = connections_create (2);
	if (tf == 0) {
		return;
	}

	Graphics_setInner (g);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);

	long nff = my frication -> frication_formants -> formants -> size - 1 + 1;
	double yh_frication = nff > 0 ? nff + (nff - 1) * dy_frication : 1;
	double yh_phonation = 1 + dy_phonation + 1;
	double yout_phonation, yout_frication;
	dy = height_phonation / yh_phonation; // 1 vertical unit in source section height units

	if (filterModel == KlattGrid_FILTER_CASCADE) { // Cascade section
		// source connection tract connection, out
		//     frication
		double xw[6] = {0, 1.75, 0.125, 3, 0.25, 0.125 };
		double yin_vocalTract_c, yout_vocalTract_c;

		rel_to_abs (xw, xws, 5, xmax2 - xmin);

		// limit height of frication unit dy !

		height_phonation = yh_phonation / (yh_phonation + yh_frication);
		if (height_phonation < 0.3) {
			height_phonation = 0.3;
		}
		dy = height_phonation / yh_phonation;

		xs1 = xmin; xs2 = xs1 + xw[1]; ys2 = ymax; ys1 = ys2 - height_phonation;
		PhonationGrid_draw_inside (my phonation, g, xs1, xs2, ys1, ys2, dy_phonation, &yout_phonation);

		// units in cascade have same heigth as units in source part.

		xc1 = xmin + xws[2]; xc2 = xc1 + xw[3];
		yc2 = yout_phonation + dy / 2; yc1 = yc2 - dy;
		VocalTractGrid_CouplingGrid_drawCascade_inline (my vocalTract, my coupling, g, xc1, xc2, yc1, yc2, &yin_vocalTract_c, &yout_vocalTract_c);

		tf -> x[1] = xc2; tf -> y[1] = yout_vocalTract_c;

		Graphics_line (g, xs2, yout_phonation, xc1, yin_vocalTract_c);

		xf1 = xmin + xws[2]; xf2 = xf1 + xw[3]; yf2 = ymax - height_phonation; yf1 = 0;

		FricationGrid_draw_inside (my frication, g, xf1, xf2, yf1, yf2, dy_frication, &yout_frication);
	} else { // Parallel
		// source connection tract connection, out
		//     frication
		double yf_parallel, yh_parallel, yh_overlap = 0.3, yin_vocalTract_p, yout_vocalTract_p;
		double xw[6] = { 0, 1.75, 0.125, 3, 0.25, 0.125 };

		rel_to_abs (xw, xws, 5, xmax2 - xmin);

		// optimize the vertical space for source, parallel and frication
		// source part is relatively fixed. let the number of vertical section units be the divisor
		// connector line from source to parallel has to be horizontal
		// determine y's of source and parallel section
		_KlattGrid_queryParallelSplit (me, dy_vocalTract_p, &yh_parallel, &yf_parallel);
		if (yh_parallel == 0) {
			yh_parallel = yh_phonation;
			yf_parallel = yh_parallel / 2;
			yh_overlap = -0.1;
		}

		height_phonation = yh_phonation / (yh_phonation + yh_frication);
		if (height_phonation < 0.3) {
			height_phonation = 0.3;
		}
		double yunit = (ymax - ymin) / (yh_parallel + (1 - yh_overlap) * yh_frication); // some overlap

		double ycs = ymax - 0.5 * height_phonation; // source output connector
		double ycp = ymax - yf_parallel * yunit; // parallel input connector
		double ytrans_phonation = ycs > ycp ? ycp - ycs : 0;
		double ytrans_parallel = ycp > ycs ? ycs - ycp : 0;

		// source, tract, frication
		xs1 = xmin; xs2 = xs1 + xw[1];

		double h1 = yh_phonation / 2, h2 = h1, h3 = yf_parallel, h4 = yh_parallel - h3, h5 = yh_frication;
		getYpositions (h1, h2, h3, h4, h5, yh_overlap, &dy, &ys1, &ys2, &yp1, &yp2, &yf1, &yf2);

		PhonationGrid_draw_inside (my phonation, g, xs1, xs2, ys1, ys2, dy_phonation, &yout_phonation);

		xp1 = xmin + xws[2]; xp2 = xp1 + xw[3];
		VocalTractGrid_CouplingGrid_drawParallel_inline (my vocalTract, my coupling, g, xp1, xp2, yp1, yp2, dy_vocalTract_p, &yin_vocalTract_p, &yout_vocalTract_p);

		tf -> x[1] = xp2; tf -> y[1] = yout_vocalTract_p;

		Graphics_line (g, xs2, yout_phonation, xp1, yin_vocalTract_p);

		xf1 = xmin /*+ 0.5 * xws[1]*/; xf2 = xf1 + 0.55 * (xw[2] + xws[3]);

		FricationGrid_draw_inside (my frication, g, xf1, xf2, yf1, yf2, dy_frication, &yout_frication);
	}

	tf -> x[2] = xf2; tf -> y[2] = yout_frication;
	r = (xmax3 - xmax2) / 2; xs = xmax2 + r / 2; ys = (ymax - ymin) / 2;

	summer_drawConnections (g, xs, ys, r, tf, 1, 0.6);

	Graphics_arrow (g, xs + r, ys, xmax, ys);

	Graphics_unsetInner (g);
	connections_free (tf);
}

/**** Query, Add, Remove, Extract Replace ********/

#define PhonationGrid_QUERY_ADD_REMOVE_EXTRACT_REPLACE(Name,name,tierType) \
double KlattGrid_get##Name##AtTime (KlattGrid me, double t) \
{ return RealTier_getValueAtTime ((RealTier) my phonation -> name, t); } \
void KlattGrid_add##Name##Point (KlattGrid me, double t, double value) \
{ RealTier_addPoint ((RealTier) my phonation -> name, t, value);} \
void KlattGrid_remove##Name##Points (KlattGrid me, double t1, double t2) \
{ AnyTier_removePointsBetween ((RealTier) my phonation -> name, t1, t2); } \
tierType KlattGrid_extract##Name##Tier (KlattGrid me) \
{ return Data_copy ((tierType) my phonation -> name); } \
void KlattGrid_replace##Name##Tier (KlattGrid me, tierType thee) \
{ try {\
	if (my xmin != thy xmin || my xmax != thy xmax) Melder_throw ("Domains must be equal"); \
	auto##tierType any = Data_copy ((tierType) thee); \
	forget (my phonation -> name); \
	my phonation -> name = any.transfer(); \
	} catch (MelderError) { Melder_throw (me, ": tier not replaced."); } \
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

FormantGrid *KlattGrid_getAddressOfFormantGrid (KlattGrid me, int formantType) {
	return formantType == KlattGrid_ORAL_FORMANTS ? & (my vocalTract -> oral_formants) :
	       formantType == KlattGrid_NASAL_FORMANTS ? & (my vocalTract -> nasal_formants) :
	       formantType == KlattGrid_FRICATION_FORMANTS ? & (my frication -> frication_formants) :
	       formantType == KlattGrid_TRACHEAL_FORMANTS ? & (my coupling -> tracheal_formants) :
	       formantType == KlattGrid_NASAL_ANTIFORMANTS ? & (my vocalTract -> nasal_antiformants) :
	       formantType == KlattGrid_TRACHEAL_ANTIFORMANTS ? & (my coupling -> tracheal_antiformants) :
	       formantType == KlattGrid_DELTA_FORMANTS ? & (my coupling -> delta_formants) : 0;
}

Ordered *KlattGrid_getAddressOfAmplitudes (KlattGrid me, int formantType) {
	return formantType == KlattGrid_ORAL_FORMANTS ? & (my vocalTract -> oral_formants_amplitudes) :
	       formantType == KlattGrid_NASAL_FORMANTS ? & (my vocalTract -> nasal_formants_amplitudes) :
	       formantType == KlattGrid_FRICATION_FORMANTS ? & (my frication -> frication_formants_amplitudes) :
	       formantType == KlattGrid_TRACHEAL_FORMANTS ? & (my coupling -> tracheal_formants_amplitudes) : NULL;
}

#define KlattGrid_QUERY_ADD_REMOVE(Name) \
double KlattGrid_get##Name##AtTime (KlattGrid me, int formantType, long iformant, double t) \
{ \
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	return FormantGrid_get##Name##AtTime ((FormantGrid) *fg, iformant, t); \
} \
void KlattGrid_add##Name##Point (KlattGrid me, int formantType, long iformant, double t, double value) \
{ \
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	FormantGrid_add##Name##Point ((FormantGrid) *fg, iformant, t, value); \
} \
void KlattGrid_remove##Name##Points (KlattGrid me, int formantType, long iformant, double t1, double t2) \
{ \
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	FormantGrid_remove##Name##PointsBetween ((FormantGrid) *fg, iformant, t1, t2); \
}

// 6 functions
KlattGrid_QUERY_ADD_REMOVE (Formant)
KlattGrid_QUERY_ADD_REMOVE (Bandwidth)

void KlattGrid_formula_frequencies (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter) {
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_formula_frequencies (*fg, expression, interpreter, 0);
}

void KlattGrid_formula_bandwidths (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter) {
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_formula_bandwidths (*fg, expression, interpreter, 0);
}

void KlattGrid_formula_amplitudes (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter) {
	try {
		Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		Formula_compile (interpreter, *ordered, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		for (long irow = 1; irow <= (*ordered) -> size; irow++) {
			RealTier amplitudes = (RealTier) (*ordered) -> item[irow];
			for (long icol = 1; icol <= amplitudes -> points -> size; icol++) {
				struct Formula_Result result;
				Formula_run (irow, icol, & result);
				if (result. result.numericResult == NUMundefined) {
					Melder_throw ("Cannot put an undefined value into the tier.\nFormula not finished.");
				}
				( (RealPoint) amplitudes -> points -> item [icol]) -> value = result. result.numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": formula not finished on amplitudes.");
	}
}

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int formantType, long iformant, double t) {
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) -> size) {
		return NUMundefined;
	}
	return RealTier_getValueAtTime ( (RealTier) (*ordered) -> item[iformant], t);
}

void KlattGrid_addAmplitudePoint (KlattGrid me, int formantType, long iformant, double t, double value) {
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) -> size) {
		Melder_throw (L"Formant does not exist.");
	}
	RealTier_addPoint ( (RealTier) (*ordered) -> item[iformant], t, value);
}

void KlattGrid_removeAmplitudePoints (KlattGrid me, int formantType, long iformant, double t1, double t2) {
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) ->size) {
		return;
	}
	AnyTier_removePointsBetween ( (*ordered) -> item[iformant], t1, t2);
}

IntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, int formantType, long iformant) {
	try {
		Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		if (iformant < 0 || iformant > (*ordered) ->size) {
			Melder_throw ("Formant does not exist.");
		}
		autoIntensityTier thee = Data_copy ( (IntensityTier) (*ordered) -> item[iformant]);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no IntensityTier extracted.");
	}
}

void KlattGrid_replaceAmplitudeTier (KlattGrid me, int formantType, long iformant, IntensityTier thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Domains must be equal");
		}
		Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		if (iformant < 0 || iformant > (*ordered) -> size) {
			Melder_throw ("Formant does not exist.");
		}
		autoIntensityTier any = Data_copy (thee);
		forget ( ( (Thing *) (*ordered) -> item) [iformant]);
		(*ordered) -> item[iformant] = any.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no ampitude tier replaced.");
	}
}

FormantGrid KlattGrid_extractFormantGrid (KlattGrid me, int formantType) {
	try {
		FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
		autoFormantGrid thee = Data_copy (*fg);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no FormantGrid extracted.");
	}
}

void KlattGrid_replaceFormantGrid (KlattGrid me, int formantType, FormantGrid thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Domains must be equal");
		}
		FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
		autoFormantGrid any = Data_copy (thee);
		forget (*fg);
		*fg = any.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no FormantGrid replaced.");
	}
}

void KlattGrid_addFormant (KlattGrid me, int formantType, long position) {
	try {
		FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
		if (*fg == 0) {
			Melder_throw (L"Formant type ",  formantType, " does not exist.");
		}

		long nof = (*fg) -> formants -> size;
		if (position > nof || position < 1) {
			position = nof + 1;
		}

		if (formantType == KlattGrid_NASAL_ANTIFORMANTS || formantType == KlattGrid_TRACHEAL_ANTIFORMANTS ||
		        formantType == KlattGrid_DELTA_FORMANTS) {
			FormantGrid_addFormantAndBandwidthTiers (*fg, position);
			return;
		}

		Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
		long noa = (*ordered) -> size;
		if (nof != noa) {
			Melder_throw ("The number of formants (",  nof, ") and the number of amplitudes (", noa, ") must be equal.");
		}
		FormantGrid_addFormantAndBandwidthTiers (*fg, position);
		try {
			autoIntensityTier it = IntensityTier_create (my xmin, my xmax);
			Ordered_addItemPos ( (*ordered), it.transfer(), position);
		} catch (MelderError) { // restore original
			FormantGrid_removeFormantAndBandwidthTiers (*fg, position);
		}
	} catch (MelderError) {
		Melder_throw (me, ": no formant added.");
	}
}

void KlattGrid_removeFormant (KlattGrid me, int formantType, long position) {
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);

	if (position < 1 || position > (*fg) -> formants -> size) {
		return;
	}
	FormantGrid_removeFormantAndBandwidthTiers (*fg, position);
	if (formantType == KlattGrid_NASAL_ANTIFORMANTS || formantType == KlattGrid_TRACHEAL_ANTIFORMANTS ||
	        formantType == KlattGrid_DELTA_FORMANTS) {
		return;    // Done, no amplitudes
	}
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	Collection_removeItem (*ordered, position);
}

void KlattGrid_addFormantAndBandwidthTier (KlattGrid me, int formantType, long position) {
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_addFormantAndBandwidthTiers (*fg, position);
}

void KlattGrid_removeFormantAndBandwidthTier (KlattGrid me, int formantType, long position) {
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
	FormantGrid_removeFormantAndBandwidthTiers (*fg, position);
}

double KlattGrid_getDeltaFormantAtTime (KlattGrid me, long iformant, double t) {
	return FormantGrid_getFormantAtTime (my coupling -> delta_formants, iformant, t);
}
void KlattGrid_addDeltaFormantPoint (KlattGrid me, long iformant, double t, double value) {
	FormantGrid_addFormantPoint (my coupling -> delta_formants, iformant, t, value);
}
void KlattGrid_removeDeltaFormantPoints (KlattGrid me, long iformant, double t1, double t2) {
	FormantGrid_removeFormantPointsBetween (my coupling -> delta_formants, iformant, t1, t2);
}
double KlattGrid_getDeltaBandwidthAtTime (KlattGrid me, long iformant, double t) {
	return FormantGrid_getBandwidthAtTime (my coupling -> delta_formants, iformant, t);
}
void KlattGrid_addDeltaBandwidthPoint (KlattGrid me, long iformant, double t, double value) {
	FormantGrid_addBandwidthPoint (my coupling -> delta_formants, iformant, t, value);
}
void KlattGrid_removeDeltaBandwidthPoints (KlattGrid me, long iformant, double t1, double t2) {
	FormantGrid_removeBandwidthPointsBetween (my coupling -> delta_formants, iformant, t1, t2);
}

FormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me) {
	try {
		FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, KlattGrid_DELTA_FORMANTS);
		autoFormantGrid thee = Data_copy (*fg);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no delta FormantGrid extracted.");
	}
}

void KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Domains must be equal");
		}
		FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, KlattGrid_DELTA_FORMANTS);
		autoFormantGrid him = Data_copy (thee);
		forget (*fg);
		*fg = him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no delta FormantGrid replaced.");
	}
}

FormantGrid KlattGrid_to_oralFormantGrid_openPhases (KlattGrid me, double fadeFraction) {
	try {
		if (my vocalTract -> oral_formants -> formants -> size == 0 && my vocalTract -> oral_formants -> bandwidths -> size == 0) {
			Melder_throw ("Formant grid is empty.");
		}
		if (fadeFraction < 0) {
			fadeFraction = 0;
		}
		if (fadeFraction >= 0.5) {
			Melder_throw ("Fade fraction must be smaller than 0.5");
		}
		my coupling -> options -> fadeFraction = fadeFraction;
		autoFormantGrid thee = Data_copy ( (FormantGrid) my vocalTract -> oral_formants);
		KlattGrid_setGlottisCoupling (me);
		FormantGrid_CouplingGrid_updateOpenPhases (thee.peek(), my coupling);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no \"open phase\" oral FormantGrid created.");
	}
}

PointProcess KlattGrid_extractPointProcess_glottalClosures (KlattGrid me) {
	try {
		// Update PhonationTier
		autoPhonationTier pt = PhonationGrid_to_PhonationTier (my phonation);
		autoPointProcess thee = PhonationTier_to_PointProcess_closures (pt.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no glottal closure points extracted.");
	}
}

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t) {
	return RealTier_getValueAtTime (my frication -> fricationAmplitude, t);
}

void KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value) {
	RealTier_addPoint (my frication -> fricationAmplitude, t, value);
}

void KlattGrid_removeFricationAmplitudePoints (KlattGrid me, double t1, double t2) {
	AnyTier_removePointsBetween (my frication -> fricationAmplitude, t1, t2);
}

IntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me) {
	return Data_copy (my frication -> fricationAmplitude);
}

void KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Domains must be equal");
		}
		autoIntensityTier him = Data_copy (thee);
		forget (my frication -> fricationAmplitude);
		my frication -> fricationAmplitude = him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no frication amplitude tier replaced.");
	}
}

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t) {
	return RealTier_getValueAtTime (my frication -> bypass, t);
}

void KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value) {
	RealTier_addPoint (my frication -> bypass, t, value);
}

void KlattGrid_removeFricationBypassPoints (KlattGrid me, double t1, double t2) {
	AnyTier_removePointsBetween (my frication -> bypass, t1, t2);
}

IntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me) {
	return Data_copy (my frication -> bypass);
}

void KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Domains must be equal");
		}
		autoIntensityTier him = Data_copy (thee);
		forget (my frication -> bypass);
		my frication -> bypass = him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no frication bypass tier replaced.");
	}
}

void KlattGrid_setGlottisCoupling (KlattGrid me) {
	try {
		forget (my coupling -> glottis);
		my coupling -> glottis = PhonationGrid_to_PhonationTier (my phonation);
		if (my coupling -> glottis == 0) {
			Melder_throw ("Empty phonation tier.");
		}
	} catch (MelderError) {
		Melder_throw (me, ": no coupling could be set.");
	}
}

Sound KlattGrid_to_Sound_aspiration (KlattGrid me, double samplingFrequency) {
	return PhonationGrid_to_Sound_aspiration (my phonation, samplingFrequency);
}

Sound KlattGrid_to_Sound_phonation (KlattGrid me) {
	return PhonationGrid_to_Sound (my phonation, 0, my options -> samplingFrequency);
}

Sound KlattGrid_to_Sound (KlattGrid me) {
	try {
		autoSound thee = 0;
		PhonationGridPlayOptions pp = my phonation -> options;
		FricationGridPlayOptions pf = my frication -> options;
		double samplingFrequency = my options -> samplingFrequency;

		if (pp -> voicing) {
			KlattGrid_setGlottisCoupling (me);
		}

		if (pp -> aspiration || pp -> voicing) { // No vocal tract filtering if no glottal source signal present
			autoSound source = PhonationGrid_to_Sound (my phonation, my coupling, samplingFrequency);

			thee.reset (Sound_VocalTractGrid_CouplingGrid_filter (source.peek(), my vocalTract, my coupling));
		}

		if (pf -> endFricationFormant > 0 || pf -> bypass) {
			autoSound frication = FricationGrid_to_Sound (my frication, samplingFrequency);
			if (thee.peek() != 0) {
				_Sounds_add_inline (thee.peek(), frication.peek());
			} else {
				thee.reset (frication.transfer());
			}
		}

		if (thee.peek() == 0) {
			thee.reset (Sound_createEmptyMono (my xmin, my xmax, samplingFrequency));
		} else if (my options -> scalePeak) {
			Vector_scale (thee.peek(), 0.99);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Sound created.");
	}
}

void KlattGrid_playSpecial (KlattGrid me) {
	try {
		autoSound thee = KlattGrid_to_Sound (me);
		KlattGridPlayOptions him = my options;
		if (his scalePeak) {
			Vector_scale (thee.peek(), 0.99);
		}
		if (his xmin == 0 && his xmax == 0) {
			his xmin = my xmin;
			his xmax = my xmax;
		}
		Sound_playPart (thee.peek(), his xmin, his xmax, 0, 0);
	} catch (MelderError) {
		Melder_throw (me, ": not played.");
	}
}

void KlattGrid_play (KlattGrid me) {
	KlattGrid_setDefaultPlayOptions (me);
	KlattGrid_playSpecial (me);
}

/************************* Sound(s) & KlattGrid **************************************************/

Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee) {
	return Sound_FricationGrid_filter (me, thy frication);
}

Sound Sound_KlattGrid_filterByVocalTract (Sound me, KlattGrid thee, int filterModel) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Domains must be equal.");
		}
		KlattGrid_setDefaultPlayOptions (thee);
		thy coupling -> options -> openglottis = 0; // don't trust openglottis info!
		thy vocalTract -> options -> filterModel = filterModel;
		autoSound him = Sound_VocalTractGrid_CouplingGrid_filter (me, thy vocalTract, thy coupling);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not filtered by KlattGrid.");
	}
}

/******************* KlattTable to KlattGrid *********************/

KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration) {
	try {
		Table kt = (Table) me;

		long nrows = my rows -> size;
		double tmin = 0, tmax = nrows * frameDuration;
		double dBNul = -300;
		double dB_offset = -20 * log10 (2.0e-5) - 87; // in KlattTable maximum in DB_to_LIN is at 87 dB : 32767
		double dB_offset_voicing = 20 * log10 (320000 / 32767); // V'[n] in range (-320000,32000)
		double dB_offset_noise = -20 * log10 (32.767 / 8.192); // noise in range (-8192,8192)
		//	double dB_offset_noise = -20 * log10 (320000/32767)  - 20 * log10 (32.767 / 8.192);
		double ap[7] = {0, 0.4, 0.15, 0.06, 0.04, 0.022, 0.03 };
		long numberOfFormants = 6;
		long numberOfNasalFormants = 1;
		long numberOfNasalAntiFormants = numberOfNasalFormants;
		long numberOfTrachealFormants = 0;
		long numberOfTrachealAntiFormants = numberOfTrachealFormants;
		long numberOfFricationFormants = 6;
		long numberOfDeltaFormants = 1;

		autoKlattGrid thee = KlattGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants,
		                                       numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants,
		                                       numberOfFricationFormants, numberOfDeltaFormants);
		for (long irow = 1; irow <= nrows; irow++) {
			double t = (irow - 1) * frameDuration;
			long icol = 1;
			double val = Table_getNumericValue_Assert (kt, irow, icol) / 10; // F0hz10
			double f0 = val;
			RealTier_addPoint (thy phonation -> pitch, t, f0);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // AVdb
			// dB values below 13 were put to zero in the DBtoLIN function
			val -= 7;
			if (val < 13) {
				val = dBNul;
			}
			// RealTier_addPoint (thy source -> voicingAmplitude, t, val);
			for (long kf = 1; kf <= 6; kf++) {
				icol++;
				double fk = val = Table_getNumericValue_Assert (kt, irow, icol); // Fhz
				RealTier_addPoint ( (RealTier) thy vocalTract -> oral_formants -> formants -> item[kf], t, val);
				RealTier_addPoint ( (RealTier) thy frication -> frication_formants -> formants -> item[kf], t, val); // only amplitudes and bandwidths in frication section
				icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Bhz
				if (val <= 0) {
					val = fk / 10;
				}
				RealTier_addPoint ( (RealTier) thy vocalTract -> oral_formants -> bandwidths -> item[kf], t, val);
			}
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // FNZhz
			RealTier_addPoint ( (RealTier) thy vocalTract -> nasal_antiformants -> formants -> item[1], t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // BNZhz
			RealTier_addPoint ( (RealTier) thy vocalTract -> nasal_antiformants -> bandwidths -> item[1], t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // FNPhz
			RealTier_addPoint ( (RealTier) thy vocalTract -> nasal_formants -> formants -> item[1], t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // BNPhz
			RealTier_addPoint ( (RealTier) thy vocalTract -> nasal_formants -> bandwidths -> item[1], t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // ah
			if (val < 13) {
				val = dBNul;
			} else {
				val += 20 * log10 (0.05) + dB_offset_noise;
			}
			RealTier_addPoint (thy phonation -> aspirationAmplitude, t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Kopen
			double openPhase = f0 > 0 ? (val / 16000) * f0 : 0.7;
			RealTier_addPoint (thy phonation -> openPhase, t, openPhase);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Aturb breathinessAmplitude during voicing (max is 8192)
			if (val < 13) {
				val = dBNul;
			} else {
				val += 20 * log10 (0.1) + dB_offset_noise;
			}
			RealTier_addPoint (thy phonation -> breathinessAmplitude, t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // TLTdb
			RealTier_addPoint (thy phonation -> spectralTilt, t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // AF
			if (val < 13) {
				val = dBNul;
			} else {
				val += 20 * log10 (0.25) + dB_offset_noise;
			}
			RealTier_addPoint (thy frication -> fricationAmplitude, t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Kskew ???
			//RealTier_addPoint (, t, val);
			for (long kf = 1; kf <= 6; kf++) {
				icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Ap
				if (val < 13) {
					val = dBNul;
				} else {
					val += 20 * log10 (ap[kf]) + dB_offset;
				}
				RealTier_addPoint ( (RealTier) thy vocalTract -> oral_formants_amplitudes -> item[kf], t, val);
				RealTier_addPoint ( (RealTier) thy frication -> frication_formants_amplitudes -> item[kf], t, val);
				icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Bhz
				RealTier_addPoint ( (RealTier) thy frication -> frication_formants -> bandwidths -> item[kf], t, val);
			}

			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // ANP
			if (val < 13) {
				val = dBNul;
			} else {
				val += 20 * log10 (0.6) + dB_offset;
			}
			RealTier_addPoint ( (RealTier) thy vocalTract -> nasal_formants_amplitudes -> item[1], t, val);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // AB
			if (val < 13) {
				val = dBNul;
			} else {
				val += 20 * log10 (0.05) + dB_offset_noise;
			}
			RealTier_addPoint (thy frication -> bypass, t, val);

			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // AVpdb
			RealTier_addPoint (thy phonation -> voicingAmplitude, t, val + dB_offset_voicing);
			icol++; val = Table_getNumericValue_Assert (kt, irow, icol); // Gain0
			val -= 3; if (val <= 0) {
				val = 57;
			}
			RealTier_addPoint (thy gain, t, val + dB_offset);
		}
		// We don't need the following low-pass: we do not use oversampling !!
		//RealTier_addPoint (thy tracheal_formants -> formants -> item[1], 0.5*(tmin+tmax), 0.095*samplingFrequency);
		//RealTier_addPoint (thy tracheal_formants -> bandwidths -> item[1], 0.5*(tmin+tmax), 0.063*samplingFrequency);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no KlattGrid created.");
	}
}

KlattGrid Sound_to_KlattGrid_simple (Sound me, double timeStep, long maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preEmphasisFrequency, double minimumPitch, double maximumPitch, double pitchFloorIntensity, int subtractMean) {
	try {
		long numberOfFormants = maximumNumberOfFormants;
		long numberOfNasalFormants = 1;
		long numberOfNasalAntiFormants = numberOfNasalFormants;
		long numberOfTrachealFormants = 1;
		long numberOfTrachealAntiFormants = numberOfTrachealFormants;
		long numberOfFricationFormants =  maximumNumberOfFormants;
		long numberOfDeltaFormants = 1;
		autoSound sound = Data_copy (me);
		Vector_subtractMean (sound.peek());
		autoFormant f = Sound_to_Formant_burg (sound.peek(), timeStep, maximumNumberOfFormants,
		                                       maximumFormantFrequency, windowLength, preEmphasisFrequency);
		autoFormantGrid fgrid = Formant_downto_FormantGrid (f.peek());
		autoPitch p = Sound_to_Pitch (sound.peek(), timeStep, minimumPitch, maximumPitch);
		autoPitchTier ptier = Pitch_to_PitchTier (p.peek());
		autoIntensity i = Sound_to_Intensity (sound.peek(), pitchFloorIntensity, timeStep, subtractMean);
		autoIntensityTier itier = Intensity_downto_IntensityTier (i.peek());
		autoKlattGrid thee = KlattGrid_create (my xmin, my xmax, numberOfFormants, numberOfNasalFormants,
		                                       numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants,
		                                       numberOfFricationFormants, numberOfDeltaFormants);
		KlattGrid_replacePitchTier (thee.peek(), ptier.peek());
		KlattGrid_replaceFormantGrid (thee.peek(), KlattGrid_ORAL_FORMANTS, fgrid.peek());
		KlattGrid_replaceVoicingAmplitudeTier (thee.peek(), itier.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no simple KlattGrid created.");
	}
}

/* End of file KlattGrid.cpp */
