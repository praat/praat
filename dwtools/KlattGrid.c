/* KlattGrid.c
 *
 * Copyright (C) 2008-2009 David Weenink
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
*/

#include "KlattGrid.h"
#include "KlattTable.h"
#include "Resonator.h"
#include "Pitch_to_PitchTier.h"
#include "PitchTier_to_Sound.h"
#include "PitchTier_to_PointProcess.h"
#include "NUM2.h"
#include "gsl_poly.h"
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

int _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, thou, long iformant, int antiformant);

Sound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling, KlattGrid_synthesisParams params);

Sound PhonationGrid_PhonationTier_to_Sound_voiced (PhonationGrid me, PhonationTier thee, double samplingFrequency, int sourceIsFlowDerivative, int breathiness);

Sound KlattGrid_to_Sound_aspiration (KlattGrid me, double samplingFrequency);
	
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
	
/*	Amplitude scaling: maximum amplitude (-1,+1) corresponds to 91 dB */

static double NUMinterpolateLinear (double x1, double y1, double x2, double y2, double x)
{
	if (y1 == y2) return y1;
	if (x1 == x2) return NUMundefined;
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}

static void rel_to_abs (double *w, double *ws, long n, double d)
{
	long i; double sum;
	
	for (sum = 0, i = 1; i <= n; i++) // relative
	{
		sum += w[i];
	}
	if (sum != 0)
	{
		double dw = d / sum;
		for (sum = 0, i = 1; i <= n; i++) // to absolute
		{
			w[i] *= dw;
			sum += w[i];
			ws[i] = sum;
		}
	}
}

static RealTier RealTier_copyInterval (I, double t1, double t2, int interpolateBorders)
{
	iam (RealTier);
	long i = 1, i1, npoints = my points -> size;
	RealPoint p1;
	
	if (t1 >= t2 || t1 < my xmin || t2 > my xmax) return NULL;
	
	RealTier thee = RealTier_create (t1, t2);
	if (thee == NULL) return NULL;
	
	while (i <= npoints && (p1 = my points -> item[i], p1 -> time <= t1)) { i++; }
	i1 = i;
	
	while (i <= npoints && (p1 = my points -> item[i], p1 -> time <= t2))
	{
		if (! RealTier_addPoint (thee, p1 -> time, p1 -> value)) goto end;
		i++;
	}
	if (interpolateBorders)
	{
		RealPoint p2;
		double value;
		if (i1 > 1 && i > i1)
		{
			p1 = my points -> item[i1 - 1]; p2 = my points -> item[i1];
			value = NUMinterpolateLinear (p1 -> time, p1 -> value, p2 -> time, p2 -> value, t1);
			RealTier_addPoint (thee, t1, value);
		}
		if (i <= npoints)
		{
			p1 = my points -> item[i - 1]; p2 = my points -> item[i];
			value = NUMinterpolateLinear (p1 -> time, p1 -> value, p2 -> time, p2 -> value, t2);
			RealTier_addPoint (thee, t2, value);
		}
		
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

static double PointProcess_getPeriodAtIndex (PointProcess me, long it, double maximumPeriod)
{
	double period = NUMundefined;
	if (it >= 2)
	{
		period = my t[it] - my t[it - 1];
		if (period > maximumPeriod) { period = NUMundefined; }
	}
	if (period == NUMundefined)
	{
		if (it < my nt)
		{
			period = my t[it + 1] - my t[it];
			if (period > maximumPeriod) { period = NUMundefined; }
		}
	}
	// NUMundefined can only occur for a single isolated pulse.
	return period;
}

#define UPDATE_TIER RealTier_addPoint (thee, mytime, myvalue); \
	lasttime=mytime; myindex++; \
	if (myindex <= numberOfValues)\
	{\
	mypoint = my points -> item [myindex];\
	mytime = mypoint -> time; myvalue = mypoint -> value;\
	}\
	else mytime = my xmax;\

static RealTier RealTier_updateWithDelta (RealTier me, RealTier delta, PhonationTier glottis, double openglottis_fadeFraction)
{
	long myindex = 1;
	RealPoint mypoint = my points -> item [myindex];
	long numberOfValues = my points -> size;
	double mytime = mypoint -> time;
	double myvalue = mypoint -> value;
	double lasttime = my xmin - 0.001; // sometime before xmin
	RealTier thee = RealTier_create (my xmin, my xmax);
	if (thee == NULL) return NULL;

	if (openglottis_fadeFraction <= 0) openglottis_fadeFraction = 0.0001;
	if (openglottis_fadeFraction >= 0.5) openglottis_fadeFraction = 0.4999;
	
	for (long ipoint = 1; ipoint <= glottis -> points -> size; ipoint++)
	{
		PhonationPoint point = glottis -> points -> item [ipoint];
		double t4 = point -> time; // glottis closing
		double openDuration = point -> te ;
		double t1 = t4 - openDuration;
		double t2 = t1 + openglottis_fadeFraction * openDuration;
		double t3 = t4 - openglottis_fadeFraction * openDuration;
		
		// Add my points that lie before t1 and after previous t4
		while (mytime > lasttime && mytime < t1)
		{
			UPDATE_TIER
		}

		if (t2 > t1)
		{
			// Set new value at t1
			double myvalue1 = RealTier_getValueAtTime (me, t1);
			RealTier_addPoint (thee, t1, myvalue1);
			// Add my points between t1 and t2
			while (mytime > lasttime && mytime < t2)
			{
				double dvalue = RealTier_getValueAtTime (delta, mytime);
				if (dvalue != NUMundefined)
				{
					double fraction = (mytime - t1) / (openglottis_fadeFraction * openDuration);
					myvalue += dvalue * fraction;
				}
				UPDATE_TIER
			}
		}
		
		double myvalue2 = RealTier_getValueAtTime (me, t2);
		double dvalue = RealTier_getValueAtTime (delta, t2);
		if (dvalue != NUMundefined) myvalue2 += dvalue;
		RealTier_addPoint (thee, t2, myvalue2);

		// Add points between t2 and t3

		while (mytime > lasttime && mytime < t3)
		{
			dvalue = RealTier_getValueAtTime (delta, mytime);
			if (dvalue != NUMundefined) myvalue += dvalue;
			UPDATE_TIER
		}

		// set new value at t3
		
		double myvalue3 = RealTier_getValueAtTime (me, t3);
		dvalue = RealTier_getValueAtTime (delta, t3);
		if (dvalue != NUMundefined) myvalue3 += dvalue;
		RealTier_addPoint (thee, t3, myvalue3);

		if (t4 > t3)
		{
			// Add my points between t3 and t4
			while (mytime > lasttime && mytime < t4)
			{
				dvalue = RealTier_getValueAtTime (delta, mytime);
				if (dvalue != NUMundefined)
				{
					double fraction = 1 - (mytime - t3) / (openglottis_fadeFraction * openDuration);
					myvalue += dvalue * fraction;
				}
				UPDATE_TIER
			}
			
			// Set new value at t4
			double myvalue4 = RealTier_getValueAtTime (me, t4);
			RealTier_addPoint (thee, t4, myvalue4);
		}
	}
	if (Melder_hasError ()) forget (thee);
	return thee;
}

static void check_formants (long numberOfFormants, long *ifb, long *ife)
{
	if (numberOfFormants <= 0 || *ifb > numberOfFormants || *ife < *ifb || *ife < 1)
	{
		*ife = 0;  // overrules everything *ifb value is a don't care now
		return;
	}
	if (*ifb <= 1) *ifb = 1;
	if (*ife > numberOfFormants) *ife = numberOfFormants;
	return;
}

static void synthesisParams_setCommonDefaults (KlattGrid_synthesisParams p)
{
	p -> samplingFrequency = 44100;
}

static void synthesisParams_setDefaultsFricationGrid (KlattGrid_synthesisParams p, FricationGrid thee)
{
	p -> endFricationFormant = MIN (thy formants -> formants -> size, thy formants -> bandwidths -> size);
	p -> startFricationFormant = 2;
	p -> fricationBypass = 1;
}

static void synthesisParams_setDefaultsCouplingGrid (KlattGrid_synthesisParams p, CouplingGrid thee)
{
	p -> openglottis_fadeFraction = 0.1; p -> openglottis = 1;
	p -> endTrachealFormant = MIN (thy tracheal_formants -> formants -> size, thy tracheal_formants -> bandwidths -> size);
	p -> startTrachealFormant = 1;
	p -> endTrachealAntiFormant = MIN (thy tracheal_antiformants -> formants -> size, thy tracheal_antiformants -> bandwidths -> size);
	p -> startTrachealAntiFormant = 1;
}

static void synthesisParams_setDefaultsPhonationGrid (KlattGrid_synthesisParams p)
{
	p -> maximumPeriod = 0; p -> sourceIsFlowDerivative = 1;
	p -> voicing = p -> aspiration = p -> breathiness = p -> spectralTilt = 1;
}

static void synthesisParams_setDefaultsVocalTractGrid (KlattGrid_synthesisParams p, VocalTractGrid thee)
{
	p -> filterModel = 0;
	p -> endFormant = MIN (thy formants -> formants -> size, thy formants -> bandwidths -> size);
	p -> startFormant = 1;
	p -> endNasalFormant = MIN (thy nasal_formants -> formants -> size, thy nasal_formants -> bandwidths -> size);
	p -> startNasalFormant = 1;
	p -> endNasalAntiFormant = MIN (thy nasal_antiformants -> formants -> size, thy nasal_antiformants -> bandwidths -> size);
	p -> startNasalAntiFormant = 1;
}

void KlattGrid_synthesisParams_setDefault (KlattGrid_synthesisParams p, KlattGrid thee)
{	// Glottis part
	synthesisParams_setCommonDefaults (p);
	synthesisParams_setDefaultsPhonationGrid (p);
	// Vocal tract filter part
	synthesisParams_setDefaultsVocalTractGrid (p, thy vocalTract);
	// Frication part
	synthesisParams_setDefaultsFricationGrid (p, thy frication);
	// Coupling characteristic
	synthesisParams_setDefaultsCouplingGrid (p, thy coupling);
	p -> kg = &thee; 
}

struct KlattGrid_synthesisParams KlattGrid_synthesisParams_createDefault (KlattGrid me)
{
	struct KlattGrid_synthesisParams p = { 0 };
	KlattGrid_synthesisParams_setDefault (&p, me);
	return p;
}

static Sound Sound_createEmptyMono (double xmin, double xmax, double samplingFrequency)
{
	long nt = ceil ((xmax - xmin) * samplingFrequency);
	double dt = 1.0 / samplingFrequency;
	double tmid = (xmin + xmax) / 2;
	double t1 = tmid - 0.5 * (nt - 1) * dt;
	
	return Sound_create (1, xmin, xmax, nt, dt, t1);
}

static void _Sounds_add_inline (Sound me, Sound thee)
{
	for (long i = 1; i <= my nx; i++)
	{
		my z[1][i] += thy z[1][i];
	}
}

static void _Sounds_addDifferentiated_inline (Sound me, Sound thee)
{
	double pval = 0, dx = my dx;
	for (long i = 1; i <= my nx; i++)
	{
		double val =  thy z[1][i];
		my z[1][i] += (val - pval) / dx; // dx makes amplitude of dz/dt independent of sampling.
		pval = val;
	}
}

typedef struct connections { long numberOfConnections; double *x, *y;} *connections;

static void connections_free (connections me)
{
	if (me == NULL) return;
	NUMdvector_free (my x, 1);
	NUMdvector_free (my y, 1);
	Melder_free (me);
}

static connections connections_create (long numberOfConnections)
{
	connections me = (connections) _Melder_malloc (sizeof (struct connections));
	if (me == NULL) return NULL;
	my numberOfConnections = numberOfConnections;
	my x = NUMdvector (1, numberOfConnections);
	if (my x == NULL) goto end;
	my y = NUMdvector (1, numberOfConnections);
end:
	if (Melder_hasError ()) connections_free (me);
	return me;
}

static connections connections_createSimple (double x, double y)
{
	connections me = connections_create (1);
	if (me == NULL) return NULL;
	my x[1] = x;
	my y[1] = y;
	return me;
}

// Calculates the intersection point (xi,yi) of a line with a circle.
// The line starts at the origin and P (xp, yp) is on that line.
static void NUMcircle_radial_intersection_sq (double x, double y, double r, double xp, double yp, double *xi, double *yi)
{
	double dx = xp - x, dy = yp - y;
	double d = sqrt (dx * dx + dy * dy);
	if (d > 0)
	{
		*xi = x + dx * r / d;
		*yi = y + dy * r / d;
	}
	else { *xi = *yi = NUMundefined; }
}

static void summer_draw (Graphics g, double x, double y, double r, int alternating)
{
	Graphics_setLineWidth (g, 2);
	Graphics_circle (g, x, y, r);
	double dy = 3 * r / 4;
	// + symbol
	if (alternating) y += r / 4;
	Graphics_line (g, x, y + r / 2, x, y - r / 2);
	Graphics_line (g, x - r / 2, y, x + r / 2, y);
	if (alternating) Graphics_line (g, x - r / 2, y - dy , x + r / 2, y - dy);
}

static void _summer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, int alternating, double horizontalFraction)
{	
	summer_draw (g, x, y, r, alternating);
	
	for (long i = 1; i <= thy numberOfConnections; i++)
	{
		double xto, yto, xp = thy x[i], yp = thy y[i];
		if (horizontalFraction > 0)
		{
			double dx = x - xp;
			if (dx > 0)
			{
				xp += horizontalFraction * dx;
				Graphics_line (g, thy x[i], yp, xp, yp);
			}
		}
		NUMcircle_radial_intersection_sq (x, y, r, xp, yp, &xto, &yto);
		if (xto == NUMundefined || yto == NUMundefined) continue;
		if (arrow) Graphics_arrow (g, xp, yp, xto, yto);
		else Graphics_line (g, xp, yp, xto, yto);
	}	
}

static void summer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, double horizontalFraction)
{
	_summer_drawConnections (g, x, y, r, thee, arrow, 0, horizontalFraction);
}

static void alternatingSummer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, double horizontalFraction)
{
	_summer_drawConnections (g, x, y, r, thee, arrow, 1, horizontalFraction);
}

static void draw_oneSection (Graphics g, double xmin, double xmax, double ymin, double ymax, wchar_t *line1, wchar_t *line2, wchar_t *line3)
{
	long numberOfTextLines = 0;
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	if (line1 != NULL) numberOfTextLines++;
	if (line2 != NULL) numberOfTextLines++;
	if (line3 != NULL) numberOfTextLines++;
	double dy = (ymax - ymin) / (numberOfTextLines + 1);
	double x = (xmax + xmin) / 2;
	double y = ymax - dy;
	if (line1 != NULL)
	{
		Graphics_text1 (g, x, y, line1);
		y -= dy;
	}
	if (line2 != NULL)
	{
		Graphics_text1 (g, x, y, line2);
		y -= dy;
	}
	if (line3 != NULL)
	{
		Graphics_text1 (g, x, y, line3);
	}	
}

// Maximum amplitue (-1,1) at 91 dB
#define DB_to_A(x) (pow (10.0, x / 20.0) * 2.0e-5)

/********************* PhonationTier ************************/

class_methods (PhonationPoint, Data)
{
	class_method_local (PhonationPoint, destroy)
	class_method_local (PhonationPoint, copy)
	class_method_local (PhonationPoint, equal)
	class_method_local (PhonationPoint, canWriteAsEncoding)
	class_method_local (PhonationPoint, writeText)
	class_method_local (PhonationPoint, readText)
	class_method_local (PhonationPoint, writeBinary)
	class_method_local (PhonationPoint, readBinary)
	class_method_local (PhonationPoint, description)
	class_methods_end
}

PhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
	double power1, double power2, double pulseScale)
{
	PhonationPoint me = new (PhonationPoint);
	if (me == NULL) return NULL;
	my time = time; my period = period;
	my openPhase = openPhase;
	my collisionPhase = collisionPhase;	my te = te;
	my power1 = power1; my power2 = power2;
	my pulseScale = pulseScale;
	return me;
}

class_methods (PhonationTier, Function)
{
	class_method_local (PhonationTier, destroy)
	class_method_local (PhonationTier, copy)
	class_method_local (PhonationTier, equal)
	class_method_local (PhonationTier, canWriteAsEncoding)
	class_method_local (PhonationTier, writeText)
	class_method_local (PhonationTier, readText)
	class_method_local (PhonationTier, writeBinary)
	class_method_local (PhonationTier, readBinary)
	class_method_local (PhonationTier, description)
	class_methods_end
}

PhonationTier PhonationTier_create (double tmin, double tmax)
{
	PhonationTier me = new (PhonationTier);
	if (me == NULL || ! Function_init (me, tmin, tmax) ||
		((my points = SortedSetOfDouble_create ()) == NULL)) forget (me);
	return me;
}

/********************** PhonationGrid **********************/

class_methods (PhonationGrid, Function)
{
	class_method_local (PhonationGrid, destroy)
	class_method_local (PhonationGrid, copy)
	class_method_local (PhonationGrid, equal)
	class_method_local (PhonationGrid, canWriteAsEncoding)
	class_method_local (PhonationGrid, writeText)
	class_method_local (PhonationGrid, readText)
	class_method_local (PhonationGrid, writeBinary)
	class_method_local (PhonationGrid, readBinary)
	class_method_local (PhonationGrid, description)
	class_methods_end
}

PhonationGrid PhonationGrid_create (double tmin, double tmax)
{
	PhonationGrid me = new (PhonationGrid);

	if (me == NULL || ! Function_init (me, tmin, tmax)) goto end;

	if (((my pitch = PitchTier_create (tmin, tmax)) == NULL) ||
		((my voicingAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my openPhase = RealTier_create (tmin, tmax)) == NULL) ||
		((my collisionPhase = RealTier_create (tmin, tmax)) == NULL) ||
		((my power1 = RealTier_create (tmin, tmax)) == NULL) ||
		((my power2 = RealTier_create (tmin, tmax)) == NULL) ||
		((my flutter = RealTier_create (tmin, tmax)) == NULL) ||
		((my doublePulsing = RealTier_create (tmin, tmax)) == NULL) ||
		((my spectralTilt = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my aspirationAmplitude = IntensityTier_create (tmin, tmax)) == NULL)) goto end;
	my breathinessAmplitude = IntensityTier_create (tmin, tmax);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

static void PhonationGrid_draw_inside (PhonationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout)
{
	// dum voicing conn tilt conn summer
	(void) me;
	double xw[6] = { 0, 1, 0.5, 1, 0.5, 0.5 }, xws[6];
	double x1, y1, x2, y2, xs, ys, ymid, r;
	int arrow = 1;

	connections thee = connections_create (2);
	if (thee == NULL) return;
	
	rel_to_abs (xw, xws, 5, xmax - xmin);
	
	dy = (ymax - ymin) / (1 + (dy < 0 ? 0 : dy) + 1);

	x1 = xmin; x2 = x1 + xw[1];
	y2 = ymax; y1 = y2 - dy;
	draw_oneSection (g, x1, x2, y1, y2, NULL, L"Voicing", NULL);
	
	x1 = x2; x2 = x1 + xw[2]; ymid = (y1 + y2) / 2;
	Graphics_line (g, x1, ymid, x2, ymid);
	
	x1 = x2; x2 = x1 + xw[3];
	draw_oneSection (g, x1, x2, y1, y2, NULL, L"Tilt", NULL);
	
	thy x[1] = x2; thy y[1] = ymid;
	
	y2 = y1 - 0.5 * dy; y1 = y2 - dy; ymid = (y1 + y2) / 2;
	x2 = xmin + xws[3]; x1 = x2 - 1.5 * xw[3]; // some extra space
	draw_oneSection (g, x1, x2, y1, y2, NULL, L"Aspiration", NULL);

	thy x[2] = x2; thy y[2] = ymid;

	r = xw[5] / 2;
	xs = xmax - r; ys = (ymax + ymin) / 2;
	
	summer_drawConnections (g, xs, ys, r, thee, arrow, 0.4);
	connections_free (thee);
	
	if (yout != NULL) *yout = ys;
}

void PhonationGrid_draw (PhonationGrid me, Graphics g)
{
	double xmin = 0, xmax2 = 0.9, xmax = 1, ymin = 0, ymax = 1, dy = 0.5, yout;
	
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	PhonationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, &yout);

	Graphics_arrow (g, xmax2, yout, xmax, yout);
	Graphics_unsetInner (g);
}

double PhonationGrid_getMaximumPeriod (PhonationGrid me)
{
	double minimumPitch = RealTier_getMinimumValue (my pitch);
	return 2 / ((minimumPitch == NUMundefined || minimumPitch == 0) ? (my xmax - my xmin) : minimumPitch);
}

PointProcess PitchTier_to_PointProcess_flutter (PitchTier pitch, RealTier flutter, double maximumPeriod)
{
	PointProcess thee = PitchTier_to_PointProcess (pitch);
	if (thee == NULL) return NULL;
	double tsum = 0;
	for (long it = 2; it <= thy nt; it++)
	{
		double t = thy t[it - 1];
		double period = thy t[it] - thy t[it - 1];
		if (period < maximumPeriod && flutter -> points -> size > 0)
		{
			double fltr = RealTier_getValueAtTime (flutter, t);
			if (fltr != NUMundefined)
			{
				// newF0 = f0 * (1 + (val / 50) * (sin ... + ...));
				double newPeriod = period / (1 + (fltr / 50) * (sin (2*NUMpi*12.7*t) + sin (2*NUMpi*7.1*t) + sin (2*NUMpi*4.7*t)));
				tsum += newPeriod - period;
			}
		}
		thy t[it] += tsum;
	}
	return thee;
}

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency)
{
	double lastval = 0;
	
	Sound thee = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);
	if (thee == NULL) return NULL;
	
	// Noise spectrum is tilted down by soft low-pass filter having a pole near
	// the origin in the z-plane, i.e. y[n] = x[n] + (0.75 * y[n-1])
	if (my aspirationAmplitude -> points -> size > 0)
	{
		for (long i = 1; i <= thy nx; i++)
		{
			double t = thy x1 + (i - 1) * thy dx;
			double val = NUMrandomUniform (-1, 1);
			double a = DB_to_A (RealTier_getValueAtTime (my aspirationAmplitude, t));
			if (a != NUMundefined)
			{
				thy z[1][i] = lastval = val + 0.75 * lastval;
				lastval = (val += 0.75 * lastval); // soft low-pass
				thy z[1][i] = val * a;
			}
		}
	}
	return thee;
}

static void Sound_PhonationGrid_spectralTilt_inline (Sound thee, PhonationGrid me)
{
	if (my spectralTilt -> points -> size > 0)
	{
		/* Spectral tilt
			Filter y[n] = a * x[n] + b * y[n] => H(z) = a / (1 - bz^(-1)).
			We need attenuation, i.e. low-pass. Therefore 0 <= b <= 1.
			|H(f)| = a / sqrt (1 - 2*b*cos(2*pi*f*T) + b^2),
			|H(0)|= a /(1 - b) => if |H(0)| == 1, then a = 1 - b.
			Now solve 20 log|H(F)|= -c (at F=3 kHz and c > 0)
			Solution: if q = (1 - D * cos(2*pi*F*T)) / (1 - D), with D = 10^(-c/10)
				then b = q -sqrt(q^2 - 1)
		*/
		
		double cosf = cos (2 * NUMpi * 3000 * thy dx), ynm1 = 0;  // samplingFrequency > 6000 !

		for (long i = 1; i <= thy nx; i++)
		{
			double t = thy x1 + (i - 1) * thy dx;
			double tilt_db = RealTier_getValueAtTime (my spectralTilt, t);
		
			if (tilt_db > 0)
			{
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

struct nrfunction_struct { double n; double m; double a;};
static void nrfunction (double x, double *fx, double *dfx, void *closure)
{
	struct nrfunction_struct *nrfs = (struct nrfunction_struct *) closure;
	double mplusax = nrfs -> m + nrfs -> a * x;
	double mminn = nrfs -> m - nrfs -> n;
	*fx = pow (x, mminn) - (nrfs -> n + nrfs -> a * x) / mplusax;
	*dfx = mminn * pow (x, mminn - 1) - nrfs -> a * mminn / (mplusax * mplusax);
}

static double get_collisionPoint_x (double n, double m, double collisionPhase)
{
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
	if (collisionPhase <= 0) return 1;
	double a = 1 / collisionPhase;
	if (m - n == 1)
	{
		double b = m - a;
		double c = - n, y1, y2;
		int nroots = gsl_poly_solve_quadratic (a, b, c, &y1, &y2);
		if (nroots == 2) y = y2;
		else if (nroots == 1) y = y1;
	}
	else // Newton-Raphson
	{
		// search in the interval from where the flow is a maximum to 1
		struct nrfunction_struct nrfs = {n, m, a};
		double root, xmaxFlow = pow (n / m, 1.0 / (m - n));
		if (NUMnrbis (&nrfunction, xmaxFlow, 1, &nrfs, &root)) y = root;
	}
	return y;
}

PhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me, double maximumPeriod, int klatt80)
{
	long diplophonicPulseIndex = 0;
	double openPhase_default = 0.7, power1_default = 2;
	
	if (my pitch -> points -> size == 0) return Melder_errorp1 (L"Pitch tier is empty.");
	
	if (maximumPeriod == 0) maximumPeriod = PhonationGrid_getMaximumPeriod (me);
	
	PointProcess point = PitchTier_to_PointProcess_flutter (my pitch, my flutter, maximumPeriod);
	if (point == NULL) goto end;

	PhonationTier thee = PhonationTier_create (my xmin, my xmax);
	if (thee == NULL) goto end;
	
	/*
		Cycle through the points of the point PointProcess. Each will become a period.
		We assume that the planning for the pitch period occurs approximately at a time T before the glottal closure.
		For each point t[i]:
			Determine the f0 -> period T[i]
			Determine time t[i]-T[i] the open quotient, power1, power2, collisionphase etc.
			Generate the period.
	 */
	
	for (long it = 1; it <= point -> nt; it++)
	{
		double te, t = point -> t[it];		// the glottis "closing" point
		double period = NUMundefined; // duration of the current period
		double pulseDelay = 0;        // For alternate pulses in case of diplophonia
		double pulseScale = 1;        // For alternate pulses in case of diplophonia
		
		period = PointProcess_getPeriodAtIndex (point, it, maximumPeriod);
		if (period == NUMundefined)
		{
			period = 0.5 * maximumPeriod; // Some default value
		}
		
		// Calculate the point where the exponential decay starts: 
		// Query tiers where period starts .
		
		double periodStart = t - period; // point where period starts:
		double collisionPhase, power1, power2, re;
		
		if (klatt80)
		{
			collisionPhase = 0;
			power1 = 2;
			power2 = 3;
			re = 1;
		}
		else
		{
			collisionPhase = RealTier_getValueAtTime (my collisionPhase, periodStart);
			if (collisionPhase == NUMundefined) collisionPhase = 0;
			power1 = RealTier_getValueAtTime (my power1, periodStart);
			if (power1 == NUMundefined) power1 = power1_default;
			power2 = RealTier_getValueAtTime (my power2, periodStart);
			if (power2 == NUMundefined) power2 = power1_default + 1;
		
			re = get_collisionPoint_x (power1, power2, collisionPhase);
			if (re == NUMundefined)
			{
				Melder_warning9 (L"Illegal collision point at t = ", Melder_double (t), L" (power1=", Melder_double(power1), L", power2=", Melder_double(power2), L"colPhase=", Melder_double(collisionPhase), L")");
			}
		}
		
		double openPhase = RealTier_getValueAtTime (my openPhase, periodStart);
		if (openPhase == NUMundefined) openPhase = openPhase_default;
		
		te = re * period * openPhase;

		/*
			In case of diplophonia alternate pulses get modified.
			A modified puls is delayed in time and its amplitude attenuated.
			This delay scales to maximally equal the closed phase of the next period.
			The doublePulsing scales the amplitudes as well as the delay linearly. 
		*/
		
		double doublePulsing = RealTier_getValueAtTime (my doublePulsing, periodStart);
		if (doublePulsing == NUMundefined) doublePulsing = 0;
		
		if (doublePulsing > 0)
		{
			diplophonicPulseIndex++;
			if (diplophonicPulseIndex % 2 == 1) // the odd one
			{
				double nextPeriod = PointProcess_getPeriodAtIndex (point, it+1, maximumPeriod);
				if (nextPeriod == NUMundefined) nextPeriod = period;
				double openPhase2 = openPhase_default;
				if (my openPhase -> points -> size > 0) openPhase2 = RealTier_getValueAtTime (my openPhase, t);
				double maxDelay = period * openPhase2;
				pulseDelay = maxDelay * doublePulsing;
				pulseScale *= (1 - doublePulsing);
			}
		}
		else
		{
			diplophonicPulseIndex = 0;
		}		

		t += pulseDelay;
		PhonationPoint phonationPoint = PhonationPoint_create (t, period, openPhase, collisionPhase, te, power1, power2, pulseScale);
		if (phonationPoint == NULL || ! AnyTier_addPoint (thee, phonationPoint)) goto end;
	}
	
end:
	forget (point);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Sound PhonationGrid_PhonationTier_to_Sound_voiced (PhonationGrid me, PhonationTier thee, double samplingFrequency, int sourceIsFlowDerivative, int breathiness)
{
	Sound him = NULL, breathy = NULL;
	long i;
	double lastVal = NUMundefined;
	
	if (my voicingAmplitude -> points -> size == 0) return Melder_errorp1 (L"Voicing amplitude tier is empty.");
	
	him = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);
	if (him == NULL) goto end;
	
	if (breathiness && my breathinessAmplitude -> points -> size > 0)
	{
		breathy = Sound_createEmptyMono (my xmin, my xmax, samplingFrequency);
		if (breathy == NULL) goto end;
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
	for (long it = 1; it <= thy points -> size; it++)
	{
		PhonationPoint point = thy points -> item[it];
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
		/*
		  Maximum of U(x) = x^n - x^m is where the derivative U'(x) = n x^(n-1) - m x^(m-1) == 0,
			i.e. (n/m) = x^(m-n), so xmax = (n/m)^(1/(m-n))
			U(xmax) = x^n (1-x^(m-n)) = (n/m)^(n/(m-n))(1-n/m)
		*/

		double amplitude = pulseScale / (pow (power1 / power2, 1 / (power2 / power1 - 1)) * (1 - power1 / power2));
		
		// Fill in the samples to the left of the current point.

		long midSample = Sampled_xToLowIndex (him, t), beginSample;
		beginSample = midSample - floor (te / his dx);
		if (beginSample < 1) beginSample = 0;
		for (i = beginSample; i <= midSample; i++)
		{
			double tsamp = his x1 + (i - 1) * his dx;
			phase = (tsamp - (t - te)) / (period * openPhase);
			if (phase > 0.0)
			{
				flow = amplitude * (pow (phase, power1) - pow (phase, power2));
				if (i == 0) { lastVal = flow; continue; } // For the derivative
				sound[i] += flow;

				// Breathiness only during open part modulated by the flow
				if (breathy)
				{
					double val = flow * NUMrandomUniform (-1, 1);
					double a = RealTier_getValueAtTime (my breathinessAmplitude, t);
					breathy -> z[1][i] += val * DB_to_A (a);
				}
			}
		}

		// Determine the signal parameters at the current point.

		phase = te / (period * openPhase);
		
		//- double flow = amplitude * (period * openPhase) * (pow (phase, power1) - pow (phase, power2));

		flow = amplitude * (pow (phase, power1) - pow (phase, power2));

		// Fill in the samples to the right of the current point.

		if (flow > 0.0)
		{
			double ta = collisionPhase * (period * openPhase);
			double factorPerSample = exp (- his dx / ta);
			double value = flow * exp (-(his x1 + midSample * his dx - t) / ta);
			long endSample = midSample + floor (20 * ta / his dx);
			if (endSample > his nx) endSample = his nx;
			for (i = midSample + 1; i <= endSample; i++)
			{
				sound[i] += value;
				value *= factorPerSample;
			}
		}
	}

	// Scale voiced part and add breathiness during open phase
	if (sourceIsFlowDerivative)
	{
		double extremum = Vector_getAbsoluteExtremum (him, 0, 0, Vector_VALUE_INTERPOLATION_CUBIC);
		if (! NUMdefined (lastVal)) lastVal = 0;
		for (i = 1; i <= his nx; i++)
		{
			double val = his z[1][i];
			his z[1][i] -= lastVal;
			lastVal = val;
		}
		Vector_scale (him, extremum);
	}
	
	for (i = 1; i <= his nx; i++)
	{
		double t = his x1 + (i - 1) * his dx;
		his z[1][i] *= DB_to_A (RealTier_getValueAtTime (my voicingAmplitude, t));
		if (breathy) his z[1][i] += breathy -> z[1][i];
	}
	
end:
	forget (breathy);
	if (Melder_hasError ()) forget (him);
	return him;
}

static Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, KlattGrid_synthesisParams p)
{
	PhonationTier thee = PhonationGrid_to_PhonationTier (me, p -> maximumPeriod, p -> klatt80);
	if (thee == NULL) return NULL;
	
	Sound him = PhonationGrid_PhonationTier_to_Sound_voiced (me, thee, p -> samplingFrequency, p -> sourceIsFlowDerivative, p -> breathiness);
	forget (thee);
	return him;
}

static Sound PhonationGrid_to_Sound (PhonationGrid me, KlattGrid_synthesisParams p, CouplingGrid him)
{
	Sound thee = NULL;
	if (p -> voicing)
	{
		if (him != NULL && his glottis -> points -> size > 0)
			thee = PhonationGrid_PhonationTier_to_Sound_voiced (me, his glottis, p -> samplingFrequency, p -> sourceIsFlowDerivative, p -> breathiness);
		else
			thee = PhonationGrid_to_Sound_voiced (me, p);
		if (thee == NULL) return NULL;
		if (p -> spectralTilt) Sound_PhonationGrid_spectralTilt_inline (thee, me);
	}
	if (p -> aspiration)
	{
		Sound aspiration = PhonationGrid_to_Sound_aspiration (me, p -> samplingFrequency);
		if (aspiration == NULL) goto end;
		if (thee == NULL) thee = aspiration;
		else
		{
			_Sounds_add_inline (thee, aspiration);
			forget (aspiration);
		}
	}
	if (thee == NULL) thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
end:	
	if (Melder_hasError ()) forget (thee);
	return thee;
}

static Ordered formantsAmplitudes_create (double tmin, double tmax, long numberOfFormants)
{
	Ordered me = Ordered_create ();
	if (me == NULL) return NULL;
	for (long i = 1; i <= numberOfFormants; i++)
	{
		IntensityTier a = IntensityTier_create (tmin, tmax);
		if (a == NULL || ! Collection_addItem (me, a)) goto end;
	}
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

/********************** VocalTractGrid ***************************************/

class_methods (VocalTractGrid, Function)
{
	class_method_local (VocalTractGrid, destroy)
	class_method_local (VocalTractGrid, copy)
	class_method_local (VocalTractGrid, equal)
	class_method_local (VocalTractGrid, canWriteAsEncoding)
	class_method_local (VocalTractGrid, writeText)
	class_method_local (VocalTractGrid, readText)
	class_method_local (VocalTractGrid, writeBinary)
	class_method_local (VocalTractGrid, readBinary)
	class_method_local (VocalTractGrid, description)
	class_methods_end
}

VocalTractGrid VocalTractGrid_create (double tmin, double tmax, long numberOfFormants,
	long numberOfNasalFormants,	long numberOfNasalAntiFormants)
{
	VocalTractGrid me = new (VocalTractGrid);
	
	if (me == NULL || ! Function_init (me, tmin, tmax)) goto end;
	
	if (((my formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants)) == NULL) ||
		((my nasal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalFormants)) == NULL) ||
		((my nasal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalAntiFormants)) == NULL)) goto end;
		
	if ((my formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfFormants)) == NULL) goto end;
	
	my nasal_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfNasalFormants);
	
end:

	if (Melder_hasError ()) forget (me);
	return me;
}

static void VocalTractGrid_CouplingGrid_drawCascade_inline (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double *yin, double *yout)
{
	long numberOfNormalFormants = my formants -> formants -> size;
	long numberOfNasalFormants = my nasal_formants -> formants -> size;
	long numberOfNasalAntiFormants = my nasal_antiformants -> formants -> size;
	long numberOfTrachealFormants = thee != NULL ? thy tracheal_formants -> formants -> size : 0;
	long numberOfTrachealAntiFormants = thee != NULL ? thy tracheal_antiformants -> formants -> size : 0;
 	double x1, y1 = ymin, x2, y2 = ymax, dx, ddx = 0.2, ymid = (y1 + y2) / 2;
 	wchar_t *text[6] = { 0, L"TF", L"TAF", L"NF", L"NAF", L""};
 	long nf[6] = {0, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfNasalFormants, numberOfNasalAntiFormants, numberOfNormalFormants};
	long numberOfFilters, numberOfXSections = 5, nsx = 0, isection, i;
	MelderString ff = { 0 }, fb = { 0 };
	
	numberOfFilters = numberOfNasalFormants + numberOfNasalAntiFormants + numberOfTrachealFormants + numberOfTrachealAntiFormants + numberOfNormalFormants;
		
	if (numberOfFilters == 0)
	{
		x2 = xmax;
		Graphics_line (g, xmin, ymid, x2, ymid);
		goto end;
	}
	
	for (isection = 1; isection <= numberOfXSections; isection++) if (nf[isection] > 0) nsx++;
	dx = (xmax - xmin) / (numberOfFilters  + (nsx - 1) * ddx);
	
	x1 = xmin;
	for (isection = 1; isection <= numberOfXSections; isection++)
	{
		long numberOfFormants = nf[isection];
		
		if (numberOfFormants == 0) continue;

		x2 = x1 + dx;
		for (i = 1; i <= numberOfFormants; i++)
		{
			MelderString_append2 (&ff, L"F", Melder_integer (i));
			MelderString_append2 (&fb, L"B", Melder_integer (i));
			draw_oneSection (g, x1, x2, y1, y2, text[isection], ff.string, fb.string);
			if (i < numberOfFormants) { x1 = x2; x2 = x1 + dx; }
			MelderString_empty (&ff); MelderString_empty (&fb);
		}

		if (isection < numberOfXSections)
		{
			x1 = x2; x2 = x1 + ddx * dx;
			Graphics_line (g, x1, ymid, x2, ymid);
			x1 = x2;
		}
	}
end:
	if (yin != NULL) *yin = ymid;
	if (yout != NULL) *yout = ymid;
	
	MelderString_free (&ff); MelderString_free (&fb);
}

static void VocalTractGrid_CouplingGrid_drawParallel_inline (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yin, double *yout)
{
	// (0: filler) (1: hor. line to split) (2: split to diff) (3: diff) (4: diff to split)
	// (5: split to filter) (6: filters) (7: conn to summer) (8: summer)
	double xw[9] = { 0, 0.3, 0.2, 1, 0.5, 0.5, 1, 0.5, 0.5 }, xws[9];
	long i, isection, numberOfXSections = 8, ic = 0, numberOfYSections = 4;
	long numberOfNasalFormants = my nasal_formants -> formants -> size;
	long numberOfNormalFormants = my formants -> formants -> size;
	long numberOfTrachealFormants = thee != NULL ? thy tracheal_formants -> formants -> size : 0;
	long numberOfFormants = numberOfNasalFormants + numberOfNormalFormants + numberOfTrachealFormants;
	long numberOfUpperPartFormants = numberOfNasalFormants + (numberOfNormalFormants > 0 ? 1 : 0);
	long numberOfLowerPartFormants = numberOfFormants - numberOfUpperPartFormants;
	double ddy = dy < 0 ? 0 : dy, x1, y1, x2, y2, x3, r, ymid;
 	wchar_t *text[5] = { 0, L"Nasal", L"", L"", L"Tracheal"};
 	long nffrom[5] = {0, 1, 1, 2, 1};
 	long nfto[5] = {0, numberOfNasalFormants, (numberOfNormalFormants > 0 ? 1 : 0), numberOfNormalFormants, numberOfTrachealFormants};
	MelderString fba = { 0 };
	
	rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

	if (numberOfFormants == 0)
	{
		y1 = y2 = (ymin + ymax) / 2;
		Graphics_line (g, xmin, y1, xmax, y1);
		goto end;
	}
	
	dy = (ymax - ymin) / (numberOfFormants * (1 + ddy) - ddy);
	
	connections local_in = connections_create (numberOfFormants);
	if (local_in == NULL) return;
	connections local_out = connections_create (numberOfFormants);
	if (local_out == NULL) goto end;

	// parallel section
	x1 = xmin + xws[5]; x2 = x1 + xw[6]; y2 = ymax;
	x3 = xmin + xws[4];
	for (isection = 1; isection <= numberOfYSections; isection++)
	{
		long ifrom = nffrom[isection], ito = nfto[isection];
		if (ito < ifrom) continue;
		for (i = ifrom; i <= ito; i++)
		{
			y1 = y2 - dy; ymid = (y1 + y2) / 2;
			const wchar_t *fi = Melder_integer (i);
			MelderString_append6 (&fba, L"A", fi, L" F", fi, L" B", fi);
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
	if (numberOfUpperPartFormants > 0)
	{
		x1 = local_in -> x[numberOfUpperPartFormants]; y1 = local_in -> y[numberOfUpperPartFormants];
		if (numberOfUpperPartFormants > 1) Graphics_line (g, x1, y1, local_in -> x[1], local_in -> y[1]); // vertical
		x2 = xmin;
		if (numberOfLowerPartFormants > 0) { x2 += xw[1]; }
		Graphics_line (g, x1, y1, x2, y1); // done
	}
	if (numberOfLowerPartFormants > 0)
	{
		long ifrom = numberOfUpperPartFormants + 1;
		x1 = local_in -> x[ifrom]; y1 = local_in -> y[ifrom]; // at the split
		if (numberOfLowerPartFormants > 1) Graphics_line (g, x1, y1, local_in -> x[numberOfFormants], local_in -> y[numberOfFormants]); // vertical
		x2 = xmin + xws[3]; // right of diff
		Graphics_line (g, x1, y1, x2, y1); // from vertical to diff
		x1 = xmin + xws[2]; // left of diff
		draw_oneSection (g, x1, x2, y1 + 0.5 * dy, y1 - 0.5 * dy, L"Pre-emphasis", NULL, NULL);
		x2 = x1;
		if (numberOfUpperPartFormants > 0)
		{
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
	
end:

	connections_free (local_out); connections_free (local_in);

	if (yin != NULL) *yin = y1;
	if (yout != NULL) *yout = y2;
	
	MelderString_free (&fba);
}

static void VocalTractGrid_CouplingGrid_draw_inside (VocalTractGrid me, CouplingGrid thee, Graphics g, int filterModel, double xmin, double xmax, double ymin, double ymax, double dy, double *yin, double *yout)
{
	filterModel == 0 ? VocalTractGrid_CouplingGrid_drawCascade_inline (me, thee, g, xmin, xmax, ymin, ymax, yin, yout) :
		VocalTractGrid_CouplingGrid_drawParallel_inline (me, thee, g, xmin, xmax, ymin, ymax, dy, yin, yout);
}

static void VocalTractGrid_CouplingGrid_draw (VocalTractGrid me, CouplingGrid thee, Graphics g, int filterModel)
{
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

static Sound Sound_VocalTractGrid_CouplingGrid_filter_cascade (Sound me, VocalTractGrid thee, CouplingGrid coupling, KlattGrid_synthesisParams params)
{
	struct KlattGrid_synthesisParams p = { 0 }; if (params == NULL) { synthesisParams_setDefaultsVocalTractGrid (&p, thee); synthesisParams_setDefaultsCouplingGrid (&p, coupling);} else p = *params;
	int useOpenGlottisInfo = p.openglottis && coupling && coupling -> glottis && coupling -> glottis -> points -> size > 0;
	FormantGrid formants = NULL;
	FormantGrid normal_formants = thy formants;
	FormantGrid nasal_formants = thy nasal_formants;
	FormantGrid nasal_antiformants = thy nasal_antiformants;
	FormantGrid tracheal_formants = coupling -> tracheal_formants;
	FormantGrid tracheal_antiformants = coupling -> tracheal_antiformants;
	
	long iformant; int antiformants = 0;
	long numberOfFormants = p.klatt80 ? MIN (5, normal_formants -> formants -> size) : normal_formants -> formants -> size;
	long numberOfTrachealFormants = p.klatt80 ? MIN (1, tracheal_formants -> formants -> size) : tracheal_formants -> formants -> size;
	long numberOfTrachealAntiFormants = p.klatt80 ? MIN (1, tracheal_antiformants -> formants -> size) : tracheal_antiformants -> formants -> size;
	long numberOfNasalFormants = p.klatt80 ? MIN (1, nasal_formants -> formants -> size) : nasal_formants -> formants -> size;
	long numberOfNasalAntiFormants = p.klatt80 ? MIN (1, nasal_antiformants -> formants -> size) : nasal_antiformants -> formants -> size;
	check_formants (numberOfFormants, & p.startFormant, & p.endFormant);
	check_formants (numberOfNasalFormants, &p.startNasalFormant, & p.endNasalFormant);
	check_formants (numberOfTrachealFormants, & p.startTrachealFormant, & p.endTrachealFormant);
	check_formants (numberOfNasalAntiFormants, & p.startNasalAntiFormant, & p.endNasalAntiFormant);
	check_formants (numberOfTrachealAntiFormants, & p.startTrachealAntiFormant, & p.endTrachealAntiFormant);

	Sound him = Data_copy (me);
	if (him == NULL) return NULL;
	
	if (useOpenGlottisInfo)
	{
		formants = Data_copy (thy formants);
		if (formants == NULL || ! FormantGrid_CouplingGrid_updateOpenPhases (formants, coupling, p.openglottis_fadeFraction)) goto end;
	}
	
	if (p.endNasalFormant > 0)  // Nasal formants
	{
		antiformants = 0;
		for (iformant = p.startNasalFormant; iformant <= p.endNasalFormant; iformant++)
		{
			if (! _Sound_FormantGrid_filterWithOneFormant_inline (him, thy nasal_formants, iformant, antiformants))
			{
				(void) Melder_error3 (L"Frequency or bandwidth missing for nasal formant ", Melder_integer (iformant), L".");
				goto end;
			}
		}
	}
	
	if (p.endNasalAntiFormant > 0)  // Nasal anti formants
	{
		antiformants = 1;
		for (iformant = p.startNasalAntiFormant; iformant <= p.endNasalAntiFormant; iformant++)
		{
			if (! _Sound_FormantGrid_filterWithOneFormant_inline (him, thy nasal_antiformants, iformant, antiformants))
			{
				(void) Melder_error3 (L"Frequency or bandwidth missing for nasal anti formant ", Melder_integer (iformant), L".");
				goto end;
			}
		}
	}

	if (p.endTrachealFormant > 0)  // Tracheal formants
	{
		antiformants = 0;
		for (iformant = p.startTrachealFormant; iformant <= p.endTrachealFormant; iformant++)
		{
			if (! _Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_formants, iformant, antiformants))
			{
				(void) Melder_error3 (L"Frequency or bandwidth missing for tracheal formant ", Melder_integer (iformant), L".");
				goto end;
			}
		}
	}
	
	if (p.endTrachealAntiFormant > 0)  // Tracheal anti formants
	{
		antiformants = 1;
		for (iformant = p.startTrachealAntiFormant; iformant <= p.endTrachealAntiFormant; iformant++)
		{
			if (! _Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_antiformants, iformant, antiformants))
			{
				(void) Melder_error3 (L"Frequency or bandwidth missing for tracheal anti formant ", Melder_integer (iformant), L".");
				goto end;
			}
		}
	}
	
	if (p.endFormant > 0)  // Normal formants
	{
		antiformants = 0;
		if (formants == NULL) formants = thy formants;
		for (iformant = p.startFormant; iformant <= p.endFormant; iformant++)
		{
			if (! _Sound_FormantGrid_filterWithOneFormant_inline (him, formants, iformant, antiformants))
			{
				(void) Melder_error3 (L"Frequency or bandwidth missing for normal formant ", Melder_integer (iformant), L".");
				goto end;
			}
		}
	}
	if (useOpenGlottisInfo) forget (formants);
end:
	if (Melder_hasError ()) forget (him);
	return him;
}

Sound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling, KlattGrid_synthesisParams params)
{
	Sound f1 = NULL, vocalTract = NULL, trachea = NULL, nasal = NULL, him = NULL;
	struct KlattGrid_synthesisParams p; if (params == NULL) { synthesisParams_setDefaultsVocalTractGrid (&p, thee); synthesisParams_setDefaultsCouplingGrid (&p, coupling);} else p = *params;
	FormantGrid formants = thy formants;
	int alternatingSign = 0; // 0: no alternating signs in parallel adding of filter outputs, 1/-1 start sign
	int useOpenGlottisInfo = p.openglottis && coupling -> glottis && coupling -> glottis -> points -> size > 0;
	long numberOfFormants = p.klatt80 ? MIN (4, thy formants -> formants -> size) : thy formants -> formants -> size;
	long numberOfNasalFormants = p.klatt80 ? MIN (1, thy nasal_formants -> formants -> size) : thy nasal_formants -> formants -> size;
	long numberOfTrachealFormants = p.klatt80 ? MIN (1, coupling -> tracheal_formants -> formants -> size) : coupling ->tracheal_formants -> formants -> size;
	
	check_formants (numberOfFormants, &(p.startFormant), &(p.endFormant));
	check_formants (numberOfNasalFormants, &(p.startNasalFormant), &(p.endNasalFormant));
	check_formants (numberOfTrachealFormants, &(p.startTrachealFormant), &(p.endTrachealFormant));

	if (useOpenGlottisInfo)
	{
		formants = Data_copy (thy formants);
		if (formants == NULL || ! FormantGrid_CouplingGrid_updateOpenPhases (formants, coupling, p.openglottis_fadeFraction)) goto end;
	}
	
	if (p.endFormant > 0)
	{
		if (p.startFormant == 1)
		{
			him = Data_copy (me);
			if (him == NULL) return NULL;
			if (formants -> formants -> size > 0 &&
				! Sound_FormantGrid_Intensities_filterWithOneFormant_inline (him, formants, thy formants_amplitudes, 1)) goto end;
		}
	}
	
	if (p.endNasalFormant > 0)
	{
		alternatingSign = 0;
		nasal =  Sound_FormantGrid_Intensities_filter (me, thy nasal_formants, thy nasal_formants_amplitudes, p.startNasalFormant, p.endNasalFormant, alternatingSign);
		if (nasal == NULL) goto end;

		if (him == NULL)
		{
			him = Data_copy (nasal);
			if (him == NULL) goto end;
		}
		else _Sounds_add_inline (him, nasal);
	}

		/*
			Formants 2 and up, with alternating signs.
			We perform the pre-emphasis by differentiating afterwards in the summation step.
			Klatt (1980) states that a first difference for the higher formants is necessary to remove low-frequency
			energy from them. This energy would otherwise distort the spectrum in the region of F1 during the synthesis
			of some vowels.
			We perform the pre-emphasis by differentiating afterwards in the summation step
			(this is allowed because of linearity).
		*/
		
	if (p.endFormant >= 2)
	{
		long startFormant2 = p.startFormant > 2 ? p.startFormant : 2;
		alternatingSign = startFormant2 % 2 == 0 ? -1 : 1; // 2 starts with negative sign
		if (startFormant2 <= formants -> formants -> size)
		{
			vocalTract = Sound_FormantGrid_Intensities_filter (me, formants, thy formants_amplitudes, startFormant2, p.endFormant, alternatingSign);
			if (vocalTract == NULL) goto end;
			
			if (him == NULL)
			{
				him = Data_copy (vocalTract);
				if (him == NULL) goto end;
			}
			else _Sounds_addDifferentiated_inline (him, vocalTract);
		}
	}
	
	if (p.endTrachealFormant > 0) // Tracheal formants
	{
		alternatingSign = 0;
		trachea =  Sound_FormantGrid_Intensities_filter (me, coupling -> tracheal_formants, coupling -> tracheal_formants_amplitudes,
			 p.startTrachealFormant, p.endTrachealFormant, alternatingSign);
		if (trachea == NULL) goto end;

		if (him == NULL)
		{
			him = Data_copy (trachea);
			if (him == NULL) goto end;
		}
		else _Sounds_addDifferentiated_inline (him, trachea);
	}

	if (him == NULL) him = Data_copy (me);
	
end:
	forget (vocalTract); forget (f1); forget (trachea); forget (nasal);
	if (useOpenGlottisInfo) forget (formants);
	if (Melder_hasError ()) forget (him);
	return him;	
}

Sound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling, KlattGrid_synthesisParams p)
{
	return  p -> filterModel == 0 ? Sound_VocalTractGrid_CouplingGrid_filter_cascade (me, thee, coupling, p) :
		Sound_VocalTractGrid_CouplingGrid_filter_parallel (me, thee, coupling, p);
}

/********************** CouplingGrid *************************************/

class_methods (CouplingGrid, Function)
{
	class_method_local (CouplingGrid, destroy)
	class_method_local (CouplingGrid, copy)
	class_method_local (CouplingGrid, equal)
	class_method_local (CouplingGrid, canWriteAsEncoding)
	class_method_local (CouplingGrid, writeText)
	class_method_local (CouplingGrid, readText)
	class_method_local (CouplingGrid, writeBinary)
	class_method_local (CouplingGrid, readBinary)
	class_method_local (CouplingGrid, description)
	class_methods_end
}

CouplingGrid CouplingGrid_create (double tmin, double tmax, long numberOfTrachealFormants, long numberOfTrachealAntiFormants, long numberOfDeltaFormants)
{
	CouplingGrid me = new (CouplingGrid);
	if (me == NULL || ! Function_init (me, tmin, tmax)) goto end;
	if (((my tracheal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealFormants)) == NULL) ||
		((my tracheal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealAntiFormants)) == NULL) ||
		((my tracheal_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfTrachealFormants)) == NULL) ||
		((my delta_formants = FormantGrid_createEmpty (tmin, tmax, numberOfDeltaFormants)) == NULL))  goto end;

	my glottis = PhonationTier_create (tmin, tmax);
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

/********************** FormantGrid & CouplingGrid *************************************/

int FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee, double fadeFraction)
{
	RealTier delta;
	for (long itier = 1; itier <= thy delta_formants -> formants -> size; itier++)
	{
		delta = thy delta_formants -> formants -> item[itier];
		if (itier <= my formants -> size)
		{
			if (delta -> points -> size > 0)
			{
				RealTier rt = RealTier_updateWithDelta (my formants -> item[itier], delta, thy glottis, fadeFraction);
				if (rt == NULL) goto end;
				forget (my formants -> item[itier]);
				my formants -> item[itier] = rt;
			}
		}
		delta = thy delta_formants -> bandwidths -> item[itier];
		if (itier <= my bandwidths -> size)
		{
			if (delta -> points -> size > 0)
			{
				RealTier rt = RealTier_updateWithDelta (my bandwidths -> item[itier], delta, thy glottis, fadeFraction);
				if (rt == NULL) goto end;
				forget (my bandwidths -> item[itier]);
				my bandwidths -> item[itier] = rt;
			}
		}
	}	
end:
	return ! Melder_hasError ();
}

/************************ Sound & FormantGrid *********************************************/

int _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, thou, long iformant, int antiformant)
{
	thouart (FormantGrid);
	if (iformant < 1 || iformant > thy formants -> size)
	{
		Melder_warning3 (L"Formant ", Melder_integer (iformant), L" does not exist.");
		return 1;
	}
	
	RealTier ftier = thy formants -> item[iformant];
	RealTier btier = thy bandwidths -> item[iformant];
	
	if (ftier -> points -> size == 0 && btier -> points -> size == 0) return 1;
	else if (ftier -> points -> size == 0 || btier -> points -> size == 0) return 0;
	
	double nyquist = 0.5 / my dx;
	Filter r =  antiformant != 0 ? (Filter) AntiResonator_create (my dx) : (Filter) Resonator_create (my dx);
	if (r == NULL) return 0;

	for (long is = 1; is <= my nx; is++)
	{
		double t = my x1 + (is - 1) * my dx;
		double f = RealTier_getValueAtTime (ftier, t);
		double b = RealTier_getValueAtTime (btier, t);
		if (f <= nyquist && b != NUMundefined) Filter_setFB (r, f, b);
		my z[1][is] = Filter_getOutput (r, my z[1][is]);
	}

	forget (r);
	return 1;
}

int Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant)
{
	return _Sound_FormantGrid_filterWithOneFormant_inline (me, thee, iformant, 1);
}

int Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant)
{
	return _Sound_FormantGrid_filterWithOneFormant_inline (me, thee, iformant, 0);
}

int Sound_FormantGrid_Intensities_filterWithOneFormant_inline (Sound me, FormantGrid thee, Ordered amplitudes, long iformant)
{
	if (iformant < 1 || iformant > thy formants -> size) return Melder_error3 (L"Formant ", Melder_integer (iformant), L" not defined. \nThis formant will not be used.");
	double nyquist = 0.5 / my dx;
	
	RealTier ftier = thy formants -> item[iformant];
	RealTier btier = thy bandwidths -> item[iformant];
	RealTier atier = amplitudes -> item[iformant];
	
	if (ftier -> points -> size == 0 || btier -> points -> size == 0 || atier -> points -> size == 0) return Melder_error3 (L"Frequencies or bandwidths or amplitudes of formant ", Melder_integer (iformant), L" not defined.\nThis formant will not be used.");
	
	Resonator r = Resonator_create (my dx);
	if (r == NULL) return 0;

	for (long is = 1; is <= my nx; is++)
	{
		double t = my x1 + (is - 1) * my dx;
		double f = RealTier_getValueAtTime (ftier, t);
		double b = RealTier_getValueAtTime (btier, t);
		if (f <= nyquist && b != NUMundefined)
		{
			Filter_setFB (r, f, b);
			if (atier -> points -> size > 0)
			{
				double a = RealTier_getValueAtTime (atier, t);
				if (a != NUMundefined) r -> a *= DB_to_A (a);
			}
		}
		my z[1][is] = Filter_getOutput (r, my z[1][is]);
	}

	forget (r);
	return 1;
}

Sound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, Ordered amplitudes, long iformantb, long iformante, int alternatingSign)
{
	if (iformantb > iformante) { iformantb = 1; iformante = thy formants -> size; }
	if (iformantb < 1 || iformantb > thy formants -> size ||
		iformante < 1 || iformante > thy formants -> size) return Melder_errorp1 (L"No such formant number.");

	Sound him = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
	if (him == NULL) return NULL;
	
	for (long iformant = iformantb; iformant <= iformante; iformant++)
	{
		Sound tmp = Data_copy (me);
		if (tmp == NULL) goto end;
		if (Sound_FormantGrid_Intensities_filterWithOneFormant_inline (tmp, thee, amplitudes, iformant))
		{
			for (long is = 1; is <= my nx; is++)
			{
				his z[1][is] += alternatingSign >= 0 ? tmp -> z[1][is] : - tmp -> z[1][is];
			}
			forget (tmp);
			if (alternatingSign != 0) alternatingSign = -alternatingSign;
		}
		else Melder_clearError (); // We just skip this formant
	}
end:
	if (Melder_hasError ()) forget (him);
	return him;
}

/************************ FricationGrid (& Sound) *********************************************/

class_methods (FricationGrid, Function)
{
	class_method_local (FricationGrid, destroy)
	class_method_local (FricationGrid, copy)
	class_method_local (FricationGrid, equal)
	class_method_local (FricationGrid, canWriteAsEncoding)
	class_method_local (FricationGrid, writeText)
	class_method_local (FricationGrid, readText)
	class_method_local (FricationGrid, writeBinary)
	class_method_local (FricationGrid, readBinary)
	class_method_local (FricationGrid, description)
	class_methods_end
}

FricationGrid FricationGrid_create (double tmin, double tmax, long numberOfFormants)
{
	FricationGrid me = new (FricationGrid);
	
	if (me == NULL || ! Function_init (me, tmin, tmax)) goto end;
	if (((my noise_amplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants)) == NULL) ||
		((my bypass = IntensityTier_create (tmin, tmax)) == NULL)) goto end;

	my formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfFormants);

end:
	if (Melder_hasError ()) forget (me);
	return me;
}

static void FricationGrid_draw_inside (FricationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout)
{
	long numberOfXSections = 5;
	long numberOfFormants = my formants -> formants -> size;
	long numberOfParts = numberOfFormants + (numberOfFormants > 1 ? 0 : 1) ; // 2..number + bypass
	// dum noise, connections, filter, connections, adder
	double xw[6] = { 0, 1.5, 1, 1, 1, 0.5 }, xws[6];
	double r, x1, y1, x2, y2, x3, xs, ys, ymid = (ymin + ymax) / 2;
	
	rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

	dy = dy < 0 ? 0 : dy;
	dy = (ymax - ymin) / (numberOfParts * (1 + dy) - dy);
	
	connections cp = connections_create (numberOfParts);
	if (cp == NULL) return;
	
	// section 1
	x1 = xmin; x2 = x1 + xw[1]; y1 = ymid - 0.5 * dy; y2 = y1 + dy;
	draw_oneSection (g, x1, x2, y1, y2, L"Frication", L"noise", L"generator");
	
	// section 2, horizontal line halfway, vertical line
	x1 = x2; x2 = x1 + xw[2] / 2; 
	Graphics_line (g, x1, ymid, x2, ymid);
	Graphics_line (g, x2, ymax - dy / 2, x2, ymin + dy / 2);
	x3 = x2;
	// final connection to section 2 , filters , connections to adder
	x1 = xmin + xws[2]; x2 = x1 + xw[3]; y2 = ymax;
	MelderString fba = { 0 };
	for (long i = 1; i <= numberOfParts; i++)
	{
		const wchar_t *fi = Melder_integer (i+1);
		y1 = y2 - dy;
		if (i < numberOfParts) { MelderString_append6 (&fba, L"A", fi, L" F", fi, L" B", fi); }
		else { MelderString_append1 (&fba,  L"Bypass"); }
		draw_oneSection (g, x1, x2, y1, y2, NULL, fba.string, NULL);
		double ymidi = (y1 + y2) / 2;
		Graphics_line (g, x3, ymidi, x1, ymidi); // from noise to filter
		cp -> x[i] = x2; cp -> y[i] = ymidi;
		y2 = y1 - 0.5 * dy;
		MelderString_empty (&fba);
	}

	r = xw[5] / 2;
	xs = xmax - r; ys = ymid;

	if (numberOfParts > 1) alternatingSummer_drawConnections (g, xs, ys, r, cp, 1, 0.4);
	else Graphics_line (g, cp -> x[1], cp -> y[1], xs + r, ys);
	
	connections_free (cp);

	if (yout != NULL) *yout = ys;
	MelderString_free (&fba);
}

void FricationGrid_draw (FricationGrid me, Graphics g)
{
	double xmin = 0, xmax = 1, xmax2 = 0.9, ymin = 0, ymax = 1, dy = 0.5, yout;

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);
	
	FricationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, &yout);
	
	Graphics_arrow (g, xmax2, yout, xmax, yout);
	Graphics_unsetInner (g);
}

Sound FricationGrid_to_Sound (FricationGrid me, KlattGrid_synthesisParams p)
{	
	Sound thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
	if (thee == NULL) return NULL;

	double lastval = 0;
	for (long i = 1; i <= thy nx; i++)
	{
		double t = thy x1 + (i - 1) * thy dx;
		double val = NUMrandomUniform (-1, 1);
		double a = 0;
		if (my noise_amplitude -> points -> size > 0)
		{
			double dba = RealTier_getValueAtTime (my noise_amplitude, t);
			a = dba == NUMundefined ? 0 : DB_to_A (dba);
		}
		lastval = (val += 0.75 * lastval); // soft low-pass
		thy z[1][i] = val * a;
	}

	Sound him = Sound_FricationGrid_filter (thee, me, p);
	forget (thee);
	return him;	
}

/************************ Sound & FricationGrid *********************************************/

Sound Sound_FricationGrid_filter (Sound me, FricationGrid thee, KlattGrid_synthesisParams params)
{
	Sound him = NULL;
	struct KlattGrid_synthesisParams p; if (params == NULL) synthesisParams_setDefaultsFricationGrid (&p, thee); else p = *params;
	long numberOfFricationFormants = p.klatt80 ? MIN (6, thy formants -> formants -> size) : thy formants -> formants -> size;
	
	check_formants (numberOfFricationFormants, &(p.startFricationFormant), &(p.endFricationFormant));

	if (p.endFricationFormant > 1)
	{
		long startFricationFormant2 = p.startFricationFormant > 2 ? p.startFricationFormant : 2;
		int alternatingSign = startFricationFormant2 % 2 == 0 ? 1 : -1; // 2 starts with positive sign
		him = Sound_FormantGrid_Intensities_filter (me, thy formants, thy formants_amplitudes, startFricationFormant2, p.endFricationFormant, alternatingSign);
		if (him == NULL) return NULL;
	}
	
	if (him == NULL) him = Data_copy (me);
	if (him == NULL) return NULL;

	if (p.fricationBypass)
	{
		for (long is = 1; is <= his nx; is++)	// Bypass
		{
			double t = his x1 + (is - 1) * his dx;
			double ab = 0;
			if (thy bypass -> points -> size > 0)
			{
				double val = RealTier_getValueAtTime (thy bypass, t);
				ab = val == NUMundefined ? 0 : DB_to_A (val);
			}
			his z[1][is] += my z[1][is] * ab;
		}
	}
	return him;
}

/************************ KlattGrid *********************************************/

class_methods (KlattGrid, Function)
{
	class_method_local (KlattGrid, destroy)
	class_method_local (KlattGrid, copy)
	class_method_local (KlattGrid, equal)
	class_method_local (KlattGrid, canWriteAsEncoding)
	class_method_local (KlattGrid, writeText)
	class_method_local (KlattGrid, readText)
	class_method_local (KlattGrid, writeBinary)
	class_method_local (KlattGrid, readBinary)
	class_method_local (KlattGrid, description)
	class_methods_end
}

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants,
	long numberOfNasalFormants,	long numberOfNasalAntiFormants,
	long numberOfTrachealFormants, long numberOfTrachealAntiFormants,
	long numberOfFricationFormants, long numberOfDeltaFormants)
{
	KlattGrid me = new (KlattGrid);
	
	if (me == NULL || ! Function_init (me, tmin, tmax)) goto end;
	
	if ((my phonation = PhonationGrid_create (tmin, tmax)) == NULL) goto end;
	
	if ((my vocalTract = VocalTractGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants, numberOfNasalAntiFormants)) == NULL) goto end;
	
	if ((my coupling = CouplingGrid_create (tmin, tmax, numberOfTrachealFormants,  numberOfTrachealAntiFormants, numberOfDeltaFormants)) == NULL) goto end;

	if ((my frication = FricationGrid_create (tmin, tmax, numberOfFricationFormants)) == NULL) goto end;

	if ((my gain = IntensityTier_create (tmin, tmax)) == NULL) goto end;

end:
	if (Melder_hasError ()) forget (me);
	return me;	
}

KlattGrid KlattGrid_createExample (void)
{
	KlattTable thee = KlattTable_createExample ();
	if (thee == NULL) return NULL;

	KlattGrid me = KlattTable_to_KlattGrid (thee, 0.005);
	forget (thee);
	return me;
}

// y is the heigth in units of the height of one section,
// y1 is the heigth from the top to the split between the uppper, non-diffed, and lower diffed part
static void _KlattGrid_queryParallelSplit (KlattGrid me, double dy, double *y, double *y1)
{
	long ny = my vocalTract -> nasal_formants -> formants -> size + my vocalTract -> formants -> formants -> size + my coupling -> tracheal_formants -> formants -> size;
	long n1 = my vocalTract -> nasal_formants -> formants -> size + (my vocalTract -> formants -> formants -> size > 0 ? 1 : 0);
	
	long n2 = ny - n1;
	if (ny == 0) { *y = 0; *y1 = 0; return; }
	
	*y = ny + (ny - 1) * dy;
	
	if (n1 == 0) { *y1 = 0.5; }
	else if (n2 == 0) { *y1 = *y - 0.5; }
	else { *y1 = n1 + (n1 - 1) * dy + 0.5 * dy; }
	return;
}

static void getYpositions (double h1, double h2, double h3, double h4, double h5, double fractionOverlap, double *dy, double *ymin1, double *ymax1, double *ymin2, double *ymax2, double *ymin3, double *ymax3)
{
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
	if (h2 >= h4)
	{
		h = h13 + h2 + h5;
	}
	else // h2 < h4
	{
		double maximumOverlap3 = fractionOverlap * h5;
		if (maximumOverlap3 < (h1 + h2)) maximumOverlap3 = 0;
		else if (maximumOverlap3 > (h4 - h2)) maximumOverlap3 = h4 - h2;
		h = h13 + h4 + h5 - maximumOverlap3;
	}
	*dy = 1 / (1.1 * h);
	*ymin1 = 1 - (h13 + h2) * *dy; *ymax1 = *ymin1 + (h1 + h2) * *dy;
	*ymin2 = 1 - (h13 + h4) * *dy; *ymax2 = *ymin2 + (h3 + h4) * *dy;
	*ymin3 = 0;  *ymax3 = h5 * *dy;
}

void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, int filterModel, int withTrachea)
{
	VocalTractGrid_CouplingGrid_draw (my vocalTract, withTrachea ? my coupling : NULL, g, filterModel);
}

void KlattGrid_draw (KlattGrid me, Graphics g, int filterModel)
{
 	double xs1, xs2, ys1, ys2, xf1, xf2, yf1, yf2;
 	double xp1, xp2, yp1, yp2, xc1, xc2, yc1, yc2;
 	double dy, r, xs, ys;
 	double xmin = 0, xmax2 = 0.90, xmax3 = 0.95, xmax = 1, ymin = 0, ymax = 1;
	double xws[6];
	double height_phonation = 0.3;
	double dy_phonation = 0.5, dy_vocalTract_p = 0.5, dy_frication = 0.5;
		
	connections tf = connections_create (2);
	if (tf == NULL) return;
	
	Graphics_setInner (g);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setLineWidth (g, 2);

	long nff = my frication -> formants -> formants -> size - 1 + 1;
	double yh_frication = nff > 0 ? nff + (nff - 1) * dy_frication : 1;
	double yh_phonation = 1 + dy_phonation + 1;
	double yout_phonation, yout_frication;
	dy = height_phonation / yh_phonation; // 1 vertical unit in source section height units

	if (filterModel == 0) // Cascade section
	{
		// source connection tract connection, out
		//     frication
		double xw[6] = {0, 1, 0.125, 3, 0.25, 0.125 };
		double yin_vocalTract_c, yout_vocalTract_c;
		
		rel_to_abs (xw, xws, 5, xmax2 - xmin);

		// limit height of frication unit dy !
		
		height_phonation = yh_phonation / (yh_phonation + yh_frication);
		if (height_phonation < 0.3) height_phonation = 0.3;
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
	}
	else // Parallel
	{	
		// source connection tract connection, out
		//     frication
		double yf_parallel, yh_parallel, ytrans_phonation, ytrans_parallel, yh_overlap = 0.3, yin_vocalTract_p, yout_vocalTract_p;
		double xw[6] = { 0, 1, 0.125, 3, 0.25, 0.125 };
		
		rel_to_abs (xw, xws, 5, xmax2 - xmin);

		// optimize the vertical space for source, parallel and frication
		// source part is relatively fixed. let the number of vertical section units be the divisor
		// connector line from source to parallel has to be horizontal
		// determine y's of source and parallel section
		_KlattGrid_queryParallelSplit (me, dy_vocalTract_p, &yh_parallel, &yf_parallel);
		if (yh_parallel == 0) { yh_parallel = yh_phonation; yf_parallel = yh_parallel / 2; yh_overlap = -0.1; }
		
		height_phonation = yh_phonation / (yh_phonation + yh_frication);
		if (height_phonation < 0.3) height_phonation = 0.3;
		double yunit = (ymax - ymin) / (yh_parallel + (1 - yh_overlap) * yh_frication); // some overlap

		double ycs = ymax - 0.5 * height_phonation; // source output connector
		double ycp = ymax - yf_parallel * yunit; // parallel input connector
		ytrans_phonation = ycs > ycp ? ycp - ycs : 0;
		ytrans_parallel = ycp > ycs ? ycs - ycp : 0;

		// source, tract, frication
		xs1 = xmin; xs2 = xs1 + xw[1];
		
		double h1 = yh_phonation / 2, h2 = h1, h3 = yf_parallel, h4 = yh_parallel - h3, h5 = yh_frication;
		getYpositions (h1, h2, h3, h4, h5, yh_overlap, &dy, &ys1, &ys2, &yp1, &yp2, &yf1, &yf2);

		PhonationGrid_draw_inside (my phonation, g, xs1, xs2, ys1, ys2, dy_phonation, &yout_phonation);
		
		xp1 = xmin + xws[2]; xp2 = xp1 + xw[3];
		VocalTractGrid_CouplingGrid_drawParallel_inline (my vocalTract, my coupling, g, xp1, xp2, yp1, yp2, dy_vocalTract_p, &yin_vocalTract_p, &yout_vocalTract_p);

		tf -> x[1] = xp2; tf -> y[1] = yout_vocalTract_p;
		
		Graphics_line (g, xs2, yout_phonation, xp1, yin_vocalTract_p);

		xf1 = xmin /*+ 0.5 * xws[1]*/; xf2 = xf1 + 0.45 * (xw[2] + xws[3]);
		
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
{ return RealTier_getValueAtTime (my phonation -> name, t); } \
int KlattGrid_add##Name##Point (KlattGrid me, double t, double value) \
{ return RealTier_addPoint (my phonation -> name, t, value);} \
void KlattGrid_remove##Name##PointsBetween (KlattGrid me, double t1, double t2) \
{ AnyTier_removePointsBetween (my phonation -> name, t1, t2); } \
tierType KlattGrid_extract##Name##Tier (KlattGrid me) \
{ return Data_copy (my phonation -> name); } \
int KlattGrid_replace##Name##Tier (KlattGrid me, tierType thee) \
{\
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal"); \
	Any any = Data_copy (thee); \
	if (any == NULL) return 0; \
	forget (my phonation -> name); \
	my phonation -> name = any; \
	return 1; \
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

Any KlattGrid_getAddressOfFormantGrid (KlattGrid me, int formantType)
{
	return formantType == KlattGrid_NORMAL_FORMANTS ? &(my vocalTract -> formants) :
		formantType == KlattGrid_NASAL_FORMANTS ? &(my vocalTract -> nasal_formants) :
		formantType == KlattGrid_FRICATION_FORMANTS ? &(my frication -> formants) :
		formantType == KlattGrid_TRACHEAL_FORMANTS ? &(my coupling -> tracheal_formants) :
		formantType == KlattGrid_NASAL_ANTIFORMANTS ? &(my vocalTract -> nasal_antiformants) :
		formantType == KlattGrid_TRACHEAL_ANTIFORMANTS ? &(my coupling -> tracheal_antiformants) :
		formantType == KlattGrid_DELTA_FORMANTS ? &(my coupling -> delta_formants) : NULL;
}

Any KlattGrid_getAddressOfAmplitudes (KlattGrid me, int formantType)
{
	return formantType == KlattGrid_NORMAL_FORMANTS ? &(my vocalTract -> formants_amplitudes) :
		formantType == KlattGrid_NASAL_FORMANTS ? &(my vocalTract -> nasal_formants_amplitudes) :
		formantType == KlattGrid_FRICATION_FORMANTS ? &(my frication -> formants_amplitudes) :
		formantType == KlattGrid_TRACHEAL_FORMANTS ? &(my coupling -> tracheal_formants_amplitudes) : NULL;
}

#define KlattGrid_QUERY_ADD_REMOVE(Name) \
double KlattGrid_get##Name##AtTime (KlattGrid me, int formantType, long iformant, double t) \
{ \
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	return FormantGrid_get##Name##AtTime (*fg, iformant, t); \
} \
int KlattGrid_add##Name##Point (KlattGrid me, int formantType, long iformant, double t, double value) \
{ \
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	return FormantGrid_add##Name##Point (*fg, iformant, t, value); \
} \
void KlattGrid_remove##Name##PointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2) \
{ \
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType); \
	FormantGrid_remove##Name##PointsBetween (*fg, iformant, t1, t2); \
}

// 6 functions
KlattGrid_QUERY_ADD_REMOVE(Formant)
KlattGrid_QUERY_ADD_REMOVE(Bandwidth)

int KlattGrid_formula_frequencies (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter)
{
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	return FormantGrid_formula_frequencies (*fg, expression, interpreter, NULL);
}

int KlattGrid_formula_bandwidths (KlattGrid me, int formantType, const wchar_t *expression, Interpreter interpreter)
{
	FormantGrid *fg = KlattGrid_getAddressOfFormantGrid (me, formantType);
	return FormantGrid_formula_bandwidths (*fg, expression, interpreter, NULL);
}

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int formantType, long iformant, double t)
{
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) -> size) return NUMundefined;
	return RealTier_getValueAtTime ((*ordered) -> item[iformant], t);
}
int KlattGrid_addAmplitudePoint (KlattGrid me, int formantType, long iformant, double t, double value)
{
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) -> size) return Melder_error1 (L"Formant does not exist.");
	return RealTier_addPoint ((*ordered) -> item[iformant], t, value);
}
void KlattGrid_removeAmplitudePointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2)
{
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) ->size) return;
	AnyTier_removePointsBetween ((*ordered) -> item[iformant], t1, t2);
}
IntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, int formantType, long iformant)
{
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) ->size) return Melder_errorp1 (L"Formant does not exist.");
	return Data_copy ((*ordered) -> item[iformant]);
}

int KlattGrid_replaceAmplitudeTier (KlattGrid me, int formantType, long iformant, IntensityTier thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal");
	Ordered *ordered = KlattGrid_getAddressOfAmplitudes (me, formantType);
	if (iformant < 0 || iformant > (*ordered) -> size) return Melder_error1 (L"Formant does not exist.");
	IntensityTier  any = Data_copy (thee);
	if (any == NULL) return 0;
	forget ((*ordered) -> item[iformant]);
	(*ordered) -> item[iformant] = any;
	return 1;
}

FormantGrid KlattGrid_extractFormantGrid (KlattGrid me, int formantType)
{
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
	return Data_copy (*fg);
}

int KlattGrid_replaceFormantGrid (KlattGrid me, int formantType, FormantGrid thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal");
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, formantType);
	Any any = Data_copy (thee);
	if (any == NULL) return 0;
	forget (*fg);
	*fg = any;
	return 1;
}

double KlattGrid_getDeltaFormantAtTime (KlattGrid me, long iformant, double t)
	{ return FormantGrid_getFormantAtTime (my coupling -> delta_formants, iformant, t); }
int KlattGrid_addDeltaFormantPoint (KlattGrid me, long iformant, double t, double value)
	{ return FormantGrid_addFormantPoint (my coupling -> delta_formants, iformant, t, value); }
void KlattGrid_removeDeltaFormantPointsBetween (KlattGrid me, long iformant, double t1, double t2)
	{ FormantGrid_removeFormantPointsBetween (my coupling -> delta_formants, iformant, t1, t2); }
double KlattGrid_getDeltaBandwidthAtTime (KlattGrid me, long iformant, double t)
	{ return FormantGrid_getBandwidthAtTime (my coupling -> delta_formants, iformant, t); }
int KlattGrid_addDeltaBandwidthPoint (KlattGrid me, long iformant, double t, double value)
	{ return FormantGrid_addBandwidthPoint (my coupling -> delta_formants, iformant, t, value); }
void KlattGrid_removeDeltaBandwidthPointsBetween (KlattGrid me, long iformant, double t1, double t2)
	{ FormantGrid_removeBandwidthPointsBetween (my coupling -> delta_formants, iformant, t1, t2); }

FormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me)
{
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, KlattGrid_DELTA_FORMANTS);
	return Data_copy (*fg);
}

int KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal");
	FormantGrid *fg =  KlattGrid_getAddressOfFormantGrid (me, KlattGrid_DELTA_FORMANTS);
	Any any = Data_copy (thee);
	if (any == NULL) return 0;
	forget (*fg);
	*fg = any;
	return 1;
}

FormantGrid KlattGrid_to_FormantGrid_openPhases (KlattGrid me, double fadeFraction)
{
	if (my vocalTract -> formants -> formants -> size == 0 && my vocalTract -> formants -> bandwidths -> size == 0)
		return Melder_errorp1 (L"Formant grid is empty.");
		
	FormantGrid thee = Data_copy (my vocalTract -> formants);
	
	if (thee == NULL || ! KlattGrid_setGlottisCoupling (me, 0, 0) ||
		! FormantGrid_CouplingGrid_updateOpenPhases (thee, my coupling, fadeFraction)) forget (thee);

	return thee;
}

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t)
{
	return RealTier_getValueAtTime (my frication -> noise_amplitude, t);
}

int KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value)
{
	return RealTier_addPoint (my frication -> noise_amplitude, t, value);
}

void KlattGrid_removeFricationAmplitudePointsBetween (KlattGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my frication -> noise_amplitude, t1, t2);
}

IntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me)
{
	return Data_copy (my frication -> noise_amplitude);
}

int KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal");
	Any any = Data_copy (thee);
	if (any == NULL) return 0;
	forget (my frication -> noise_amplitude);
	my frication -> noise_amplitude = any;
	return 1;
}

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t)
{
	return RealTier_getValueAtTime (my frication -> bypass, t);
}

int KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value)
{
	return RealTier_addPoint (my frication -> bypass, t, value);
}

void KlattGrid_removeFricationBypassPointsBetween (KlattGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my frication -> bypass, t1, t2);
}

IntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me)
{
	return Data_copy (my frication -> bypass);
}

int KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal");
	Any any = Data_copy (thee);
	if (any == NULL) return 0;
	forget (my frication -> bypass);
	my frication -> bypass = any;
	return 1;
}

int KlattGrid_setGlottisCoupling (KlattGrid me, double maximumPeriod, int klatt80)
{
	forget (my coupling -> glottis);
	my coupling -> glottis = PhonationGrid_to_PhonationTier (my phonation, maximumPeriod, klatt80);
	return my coupling -> glottis != NULL ? 1 : 0;
}

Sound KlattGrid_to_Sound_aspiration (KlattGrid me, double samplingFrequency)
{
	return PhonationGrid_to_Sound_aspiration (my phonation, samplingFrequency);
}

Sound KlattGrid_to_Sound_phonation (KlattGrid me, double samplingFrequency, int voicing, int spectralTilt, int sourceIsFlowDerivative, int breathiness, int aspiration)
{
	struct KlattGrid_synthesisParams p = KlattGrid_synthesisParams_createDefault (me);
	p.samplingFrequency = samplingFrequency;
	p.voicing = voicing;
	p.spectralTilt = spectralTilt;
	p.sourceIsFlowDerivative = sourceIsFlowDerivative;
	p.breathiness = breathiness;
	p.aspiration = aspiration;
	return PhonationGrid_to_Sound (my phonation, &p, NULL);
}

Sound KlattGrid_to_Sound (KlattGrid me, KlattGrid_synthesisParams p)
{
	Sound thee = NULL, source = NULL, frication = NULL;
	
	if (p -> voicing && ! KlattGrid_setGlottisCoupling (me, p -> maximumPeriod, p -> klatt80)) return NULL;

	if (p -> aspiration || p -> voicing) // No need for vocal tract filtering if no glottal source signal present
	{
		Sound source = PhonationGrid_to_Sound (my phonation, p, my coupling);
		if (source == NULL) goto end;
		
		thee = Sound_VocalTractGrid_CouplingGrid_filter (source, my vocalTract, my coupling, p);
		if (thee == NULL) goto end;
	}

	if (p -> endFricationFormant > 0 || p -> fricationBypass)
	{
		frication = FricationGrid_to_Sound (my frication, p);
		if (frication == NULL) goto end;
	
		if (thee != NULL)
		{
			_Sounds_add_inline (thee, frication);
			forget (frication);
		}
		else thee = frication;
	}
	
	if (thee == NULL) thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
	
end:
	forget (source);
	if (Melder_hasError ()) forget (thee);
	return thee;	
}

int KlattGrid_play (KlattGrid me)
{
	struct KlattGrid_synthesisParams p = KlattGrid_synthesisParams_createDefault (me);
	Sound thee = KlattGrid_to_Sound (me, &p);
	if (thee == NULL) return 0;
	Vector_scale (thee, 0.99);
	(void) Sound_play (thee, NULL, NULL);
	forget (thee);
	return 1;
}

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, int parallel)
{
	struct KlattGrid_synthesisParams p = { 0 };
	KlattGrid_synthesisParams_setDefault (&p, me);
	p.filterModel = parallel ? 1 : 0;
	p.samplingFrequency = samplingFrequency;
	return KlattGrid_to_Sound (me, &p);
}

/************************* Sound(s) & KlattGrid **************************************************/

Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee, KlattGrid_synthesisParams params)
{
	return Sound_FricationGrid_filter (me, thy frication, params);
}

Sound Sound_KlattGrid_filterByVocalTract (Sound me, KlattGrid thee, int filterModel)
{
	struct KlattGrid_synthesisParams p = { 0 };
	KlattGrid_synthesisParams_setDefault (&p, thee);
	p.openglottis = 0; // don't trust openglottis info!
	p.filterModel = filterModel;
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_errorp1 (L"Domains of sound and KlattGrid must be equal.");
	return Sound_VocalTractGrid_CouplingGrid_filter (me, thy vocalTract, thy coupling, &p);
}

/******************* KlattTable to KlattGrid *********************/

KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration)
{
	Table kt = (Table) me;

	long nrows = my rows -> size;
	double tmin = 0, tmax = nrows * frameDuration;
	double dBNul = -300;
	double dB_offset = -20 * log10 (2.0e-5) - 87; // in KlattTable maximum in DB_to_LIN is at 87 dB : 32767
	double dB_offset_voicing = 20 * log10 (320000/32767); // V'[n] in range (-320000,32000)
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
	
	KlattGrid thee = KlattGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants,
		numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants,
		numberOfFricationFormants, numberOfDeltaFormants);
	if (thee == NULL) return NULL;
	for (long irow = 1; irow <= nrows; irow++)
	{
		double t = (irow - 1) * frameDuration;
		double val, f0, fk;
		long icol = 1;
		
		f0 = val = Table_getNumericValue (kt, irow, icol) / 10; // F0hz10
		RealTier_addPoint (thy phonation -> pitch, t, f0);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AVdb
		// dB values below 13 were put to zero in the DBtoLIN function
		val -= 7;
		if (val < 13) val = dBNul;
//		RealTier_addPoint (thy source -> voicingAmplitude, t, val);
		for (long kf = 1; kf <= 6; kf++)
		{
			icol++; fk = val = Table_getNumericValue (kt, irow, icol); // Fhz
			RealTier_addPoint (thy vocalTract -> formants -> formants -> item[kf], t, val);
			RealTier_addPoint (thy frication -> formants -> formants -> item[kf], t, val); // only amplitudes and bandwidths in frication section
			icol++; val = Table_getNumericValue (kt, irow, icol); // Bhz
			if (val <= 0) val = fk / 10;
			RealTier_addPoint (thy vocalTract -> formants -> bandwidths -> item[kf], t, val);
		}
		icol++; val = Table_getNumericValue (kt, irow, icol); // FNZhz
		RealTier_addPoint (thy vocalTract -> nasal_antiformants -> formants -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // BNZhz
		RealTier_addPoint (thy vocalTract -> nasal_antiformants -> bandwidths -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // FNPhz
		RealTier_addPoint (thy vocalTract -> nasal_formants -> formants -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // BNPhz
		RealTier_addPoint (thy vocalTract -> nasal_formants -> bandwidths -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // ah
		if (val < 13) val = dBNul; else val += 20 * log10 (0.05) + dB_offset_noise;
		RealTier_addPoint (thy phonation -> aspirationAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Kopen
		double openPhase = f0 > 0 ? (val / 16000) * f0 : 0.7;
		RealTier_addPoint (thy phonation -> openPhase, t, openPhase);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Aturb breathinessAmplitude during voicing (max is 8192)
		if (val < 13) val = dBNul; else val += 20 * log10 (0.1) + dB_offset_noise;
		RealTier_addPoint (thy phonation -> breathinessAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // TLTdb
		RealTier_addPoint (thy phonation -> spectralTilt, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AF
		if (val < 13) val = dBNul; else val += 20 * log10 (0.25) + dB_offset_noise;
		RealTier_addPoint (thy frication -> noise_amplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Kskew ???
		//RealTier_addPoint (, t, val);
		for (long kf = 1; kf <= 6; kf++)
		{
			icol++; val = Table_getNumericValue (kt, irow, icol); // Ap
			if (val < 13) val = dBNul; else val += 20 * log10 (ap[kf]) + dB_offset;
			RealTier_addPoint (thy vocalTract -> formants_amplitudes -> item[kf], t, val);
			RealTier_addPoint (thy frication -> formants_amplitudes -> item[kf], t, val);
			icol++; val = Table_getNumericValue (kt, irow, icol); // Bhz
			RealTier_addPoint (thy frication -> formants -> bandwidths -> item[kf], t, val);
		}
		
		icol++; val = Table_getNumericValue (kt, irow, icol); // ANP
		if (val < 13) val = dBNul; else val += 20 * log10 (0.6) + dB_offset;
		RealTier_addPoint (thy vocalTract -> nasal_formants_amplitudes -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AB
		if (val < 13) val = dBNul; else val += 20 * log10 (0.05) + dB_offset_noise;
		RealTier_addPoint (thy frication -> bypass, t, val);
		
		icol++; val = Table_getNumericValue (kt, irow, icol); // AVpdb
		RealTier_addPoint (thy phonation -> voicingAmplitude, t, val + dB_offset_voicing);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Gain0
		val -= 3; if (val <= 0) val = 57;
		RealTier_addPoint (thy gain, t, val + dB_offset);
	}
	// We don't need the following low-pass: we do not use oversampling !!
	//RealTier_addPoint (thy tracheal_formants -> formants -> item[1], 0.5*(tmin+tmax), 0.095*samplingFrequency);
	//RealTier_addPoint (thy tracheal_formants -> bandwidths -> item[1], 0.5*(tmin+tmax), 0.063*samplingFrequency);

	if (Melder_hasError()) forget (thee);
	return thee;
}

KlattGrid Sound_to_KlattGrid_simple (Sound me, double timeStep, long maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preEmphasisFrequency, double minimumPitch, double maximumPitch, double pitchFloorIntensity, int subtractMean)
{
	long numberOfFormants = maximumNumberOfFormants;
	long numberOfNasalFormants = 1;
	long numberOfNasalAntiFormants = numberOfNasalFormants;
	long numberOfTrachealFormants = 1;
	long numberOfTrachealAntiFormants = numberOfTrachealFormants;
	long numberOfFricationFormants =  maximumNumberOfFormants;
	long numberOfDeltaFormants = 1;
	Sound sound = Data_copy (me);
	if (sound == NULL) goto end;
	Vector_subtractMean (sound);
	Formant f = Sound_to_Formant_burg (sound, timeStep, maximumNumberOfFormants, maximumFormantFrequency, windowLength, preEmphasisFrequency);
	if (f == NULL) goto end;
	FormantGrid fgrid = Formant_downto_FormantGrid (f);
	if (fgrid == NULL) goto end;
	Pitch p = Sound_to_Pitch (sound, timeStep, minimumPitch, maximumPitch);
	if (p == NULL) goto end;
	PitchTier ptier = Pitch_to_PitchTier (p);
	if (ptier == NULL) goto end;
	Intensity i = Sound_to_Intensity (sound, pitchFloorIntensity, timeStep, subtractMean);
	if (i == NULL) goto end;
	IntensityTier itier = Intensity_downto_IntensityTier (i);
	if (itier == NULL) goto end;

	KlattGrid thee = KlattGrid_create (my xmin, my xmax, numberOfFormants, numberOfNasalFormants, numberOfNasalAntiFormants,
		numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfFricationFormants, numberOfDeltaFormants);
	if (thee == NULL) goto end;
	if (! KlattGrid_replacePitchTier (thee, ptier) || ! KlattGrid_replaceFormantGrid (thee, KlattGrid_NORMAL_FORMANTS, fgrid) ||
		! KlattGrid_replaceVoicingAmplitudeTier (thee, itier)) goto end;

end:
	forget (f); forget (fgrid);
	forget (p); forget (ptier);
	forget (i); forget (itier);
	forget (sound);
	if (Melder_hasError ()) forget (thee);
	return thee;
}
/* End of file KlattGrid.c */
