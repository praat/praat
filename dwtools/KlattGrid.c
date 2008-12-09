/* KlattGrid.c
 *
 * Copyright (C) 2008 David Weenink
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
*/

#include "KlattGrid.h"
#include "KlattTable.h"
#include "Resonator.h"
#include "PitchTier_to_Sound.h"
#include "PitchTier_to_PointProcess.h"
#include "NUM2.h"
#include "gsl_poly.h"

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

PointProcess PitchTier_to_PointProcess_flutter (PitchTier f0, RealTier flutter, double maximumPeriod);

int PhonationGrid_replacePitchTier (PhonationGrid me, PitchTier thee);

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, synthesisParams params);
Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, synthesisParams params, RealTier *glottisOpenDurations);
Sound PhonationGrid_to_Sound (PhonationGrid me, synthesisParams params, RealTier *glottisOpenDurations);
Sound PhonationGrid_Sound_to_Sound (PhonationGrid me, Sound thee, int soundType, synthesisParams params); // type (0) voiced or (1) aspiration
void Sound_PhonationGrid_spectralTilt_inline (Sound thee, PhonationGrid me);

int Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant);
int Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant);

Sound Sound_FormantGridP_filter (Sound me, FormantGridP thee, long iformantb, long iformante, int alternatingSign);
int Sound_FormantGridP_filterWithOneFormant_inline (Sound me, FormantGridP thee, long iformant);

int FormantGridP_addAmplitudePoint (FormantGridP me, long iformant, double t, double value);
void FormantGridP_removeAmplitudePointsBetween (FormantGridP me, long iformant, double tmin, double tmax);
double FormantGridP_getAmplitudeAtTime (FormantGridP me, long iformant, double t);
FormantGrid FormantGridP_downto_FormantGrid (FormantGridP me);
int FormantGrid_DeltaFormantGrid_updateOpenPhases (I, DeltaFormantGrid delta, RealTier glottisOpenDurations, double openglottis_fadeFraction);

Sound Sound_KlattGrid_filter_laryngial (Sound me, KlattGrid thee, RealTier glottisOpenDurations, synthesisParams p);
Sound Sound_KlattGrid_filter_laryngial_cascade (Sound me, KlattGrid thee, RealTier glottisOpenDurations, synthesisParams params);
Sound Sound_KlattGrid_filter_laryngial_parallel (Sound me, KlattGrid thee, RealTier glottisOpenDurations, synthesisParams params);
Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee, synthesisParams params);

int KlattGrid_replaceFormantGrid (KlattGrid me, thou);
Sound KlattGrid_to_Sound_frication (KlattGrid me, synthesisParams params);
Sound KlattGrid_to_Sound_aspiration (KlattGrid me, synthesisParams p);
	
	
/*	Amplitude scaling: maximum amplitude (-1,+1) corresponds to 91 dB */


static double NUMinterpolateLinear (double x1, double y1, double x2, double y2, double x)
{
	if (y1 == y2) return y1;
	if (x1 == x2) return NUMundefined;
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
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

static RealTier RealTier_updateWithDelta (RealTier me, RealTier delta, RealTier glottisOpenDurations, double openglottis_fadeFraction)
{
	RealPoint mypoint;
	long myindex = 0;
	double mytime = my xmin, myvalue, lasttime;
	long numberOfValues = my points -> size;
	RealTier thee = RealTier_create (my xmin, my xmax);
	if (thee == NULL) return NULL;

	if (openglottis_fadeFraction <= 0) openglottis_fadeFraction = 0.0001;
	if (openglottis_fadeFraction >= 0.5) openglottis_fadeFraction = 0.4999;
	
	for (long ipoint = 1; ipoint <= glottisOpenDurations -> points -> size; ipoint++)
	{
		UPDATE_TIER
		RealPoint point = glottisOpenDurations -> points -> item [ipoint];
		double t4 = point -> time; // glottis closing
		double openDuration = point -> value;
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

static void synthesisParams_setDefault (synthesisParams p, KlattGrid thee)
{	// Glottis part
	p -> samplingFrequency = 44100; p -> maximumPeriod = 0.05;
	p -> voicing = p -> aspiration = p -> spectralTilt = 1;
	// Vocal tract filter part
	p -> filterModel = 0;
	p -> endFormant = thy formants -> formants -> size;
	p -> startFormant = 1;
	p -> endNasalFormant = thy nasal_formants -> formants -> size;
	p -> startNasalFormant = 1;
	p -> endTrachealFormant = thy tracheal_formants -> formants -> size;
	p -> startTrachealFormant = 1;
	p -> endNasalAntiFormant = thy nasal_antiformants -> formants -> size;
	p -> startNasalAntiFormant = 1;
	p -> endTrachealAntiFormant = thy tracheal_antiformants -> formants -> size;
	p -> startTrachealAntiFormant = 1;
	// Frication part
	p -> endFricationFormant = thy frication_formants -> formants -> size;
	p -> startFricationFormant = 2;
	p -> fricationBypass = 1;
	// Coupling characteristic
	p -> openglottis_fadeFraction = 0.1;
	p -> kg = &thee; 
}

// TODO glottisOpenDurations verwerken en delta_f en delta_b tiers

static struct synthesisParams synthesisParams_createDefault (KlattGrid me)
{
	struct synthesisParams p;
	synthesisParams_setDefault (&p, me);
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

// Maximum amplitue (-1,1) at 91 dB
#define DB_to_A(x) (pow (10.0, x / 20.0) * 2.0e-5)

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

static int PhonationGrid_setDefaults (PhonationGrid me)
{
	double tmid = (my xmax + my xmin) / 2;
	if (! RealTier_addPoint (my power1, tmid, 2) ||
	    ! RealTier_addPoint (my power2, tmid, 3) ||
	    ! RealTier_addPoint (my voicingAmplitude, tmid, 60) ||
	    ! RealTier_addPoint (my openPhase, tmid, 0.3) ||
	    ! RealTier_addPoint (my collisionPhase, tmid, 0.03) ||
	    ! RealTier_addPoint (my flutter, tmid, 0) ||
	    ! RealTier_addPoint (my doublePulsing, tmid, 0) ||
	    ! RealTier_addPoint (my f0, my xmin, 100)) goto end;
end:
	if (Melder_hasError ()) return 0;
	return 1;
}

PhonationGrid PhonationGrid_create (double tmin, double tmax)
{
	PhonationGrid me = new (PhonationGrid);

	if (me == NULL) return NULL;
	my xmin = tmin; my xmax = tmax;
	if (((my f0 = PitchTier_create (tmin, tmax)) == NULL) ||
		((my voicingAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my openPhase = RealTier_create (tmin, tmax)) == NULL) ||
		((my collisionPhase = RealTier_create (tmin, tmax)) == NULL) ||
		((my power1 = RealTier_create (tmin, tmax)) == NULL) ||
		((my power2 = RealTier_create (tmin, tmax)) == NULL) ||
		((my flutter = RealTier_create (tmin, tmax)) == NULL) ||
		((my doublePulsing = RealTier_create (tmin, tmax)) == NULL) ||
		((my spectralTilt = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my aspirationAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my breathynessAmplitude = IntensityTier_create (tmin, tmax)) == NULL)) forget (me);
	return me;
}

#define PhonationGrid_GET_PHONATIONAMPLITUDETIER  \
	RealTier tier = phonationAmplitudeTier == 1 ? (RealTier) my voicingAmplitude : \
	phonationAmplitudeTier == 2 ? (RealTier) my aspirationAmplitude : \
	phonationAmplitudeTier == 3 ? (RealTier) my breathynessAmplitude : NULL;

/**** Query ********/

double PhonationGrid_getCollisionPhaseAtTime (PhonationGrid me, double t)
{
	return RealTier_getValueAtTime (my collisionPhase, t);
}

double PhonationGrid_getOpenPhaseAtTime (PhonationGrid me, double t)
{
	return RealTier_getValueAtTime (my openPhase, t);
}

double PhonationGrid_getFlowFunctionAtTime (PhonationGrid me, long powerTerm, double t)
{	
	return RealTier_getValueAtTime (powerTerm == 1 ? my power1 : my power2, t);
}

double PhonationGrid_getPitchAtTime (PhonationGrid me, double t)
{
	return RealTier_getValueAtTime (my f0, t);
}

double PhonationGrid_getFlutterAtTime (PhonationGrid me, double t)
{
	return RealTier_getValueAtTime (my flutter, t);
}

double PhonationGrid_getPhonationAmplitudeAtTime (PhonationGrid me, int phonationAmplitudeTier, double t)
{
	PhonationGrid_GET_PHONATIONAMPLITUDETIER
	return RealTier_getValueAtTime (tier, t);
}

double PhonationGrid_getSpectralTiltAtTime (PhonationGrid me, double t)
{
	return RealTier_getValueAtTime (my spectralTilt, t);
}

/*** Modify ******/

int PhonationGrid_addCollisionPhasePoint (PhonationGrid me, double t, double value)
{
	return RealTier_addPoint (my collisionPhase, t, value);
}

void PhonationGrid_removeCollisionPhasePointsBetween (PhonationGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my collisionPhase, t1, t2);
}

int PhonationGrid_addSpectralTiltPoint (PhonationGrid me, double t, double value)
{
	return RealTier_addPoint (my spectralTilt, t, value);
}

void PhonationGrid_removeSpectralTiltPointsBetween (PhonationGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my spectralTilt, t1, t2);
}

int PhonationGrid_addOpenPhasePoint (PhonationGrid me, double t, double value)
{
	return RealTier_addPoint (my openPhase, t, value);
}

void PhonationGrid_removeOpenPhasePointsBetween (PhonationGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my openPhase, t1, t2);
}
int PhonationGrid_addFlowFunctionPoint (PhonationGrid me, long powerTerm, double t, double value)
{
	return RealTier_addPoint (powerTerm == 1 ? my power1 : my power2, t, value);
}

void PhonationGrid_removeFlowFunctionPointsBetween (PhonationGrid me, long powerTerm, double t1, double t2)
{
	AnyTier_removePointsBetween (powerTerm == 1 ? my power1 : my power2, t1, t2);
}

int PhonationGrid_addPitchPoint (PhonationGrid me, double t, double value)
{	
	return RealTier_addPoint (my f0, t, value);
}

void PhonationGrid_removePitchPointsBetween (PhonationGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my f0, t1, t2);
}

int PhonationGrid_addFlutterPoint (PhonationGrid me, double t, double value)
{	
	return RealTier_addPoint (my flutter, t, value);
}

void PhonationGrid_removeFlutterPointsBetween (PhonationGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my flutter, t1, t2);
}
	
int PhonationGrid_addPhonationAmplitudePoint (PhonationGrid me, int phonationAmplitudeTier, double t, double value)
{
	PhonationGrid_GET_PHONATIONAMPLITUDETIER
	return tier != NULL && RealTier_addPoint (tier, t, value);
}

void PhonationGrid_removePhonationAmplitudePointsBetween (PhonationGrid me, int phonationAmplitudeTier, double t1, double t2)
{
	PhonationGrid_GET_PHONATIONAMPLITUDETIER
	AnyTier_removePointsBetween (tier, t1, t2);
}

PointProcess PitchTier_to_PointProcess_flutter (PitchTier f0, RealTier flutter, double maximumPeriod)
{
	PointProcess thee = PitchTier_to_PointProcess (f0);
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

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, synthesisParams p)
{
	double lastval = 0;
	
	if (my aspirationAmplitude -> points -> size == 0) return Melder_errorp1 (L"Aspiration amplitude tier is empty.");
	
	Sound thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
	if (thee == NULL) return NULL;
	
	// Noise spectrum is tilted down by soft low-pass filter having a pole near
	// the origin in the z-plane, i.e. y[n] = x[n] + (0.75 * y[n-1])

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
	return thee;
}

void Sound_PhonationGrid_spectralTilt_inline (Sound thee, PhonationGrid me)
{
	if (my spectralTilt -> points -> size > 0)
	{
		/* Spectral tilt
			Filter y[n] = a * x[n] + b * y[n] => H(z) = a / (1 - bz^()-1)).
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

int PhonationGrid_replacePitchTier (PhonationGrid me, PitchTier thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal.");
	PitchTier pt = Data_copy (thee);
	if (pt == NULL) return 0;
	forget (my f0);
	my f0 = pt;
	return 1;
}

struct nrfunction_struct { double n; double m; double a;};
static void nrfunction (double x, double *fx, double *dfx, void *closure)
{
	struct nrfunction_struct *nrfs = (struct nrfunction_struct *) closure;
	double mplusax = nrfs -> m + nrfs -> a * x;
	double mminn = nrfs -> m - nrfs -> n;
	*fx = pow (x, mminn) - (nrfs -> n + nrfs -> a * x) / mplusax;
	*dfx = mminn * pow (x,mminn-1) - nrfs -> a * mminn / (mplusax * mplusax);
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

Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, synthesisParams p, RealTier *glottisOpenDurations)
{
	Sound thee = NULL, breathy = NULL;
	long i, diplophonicPulseIndex = 0;
	double openPhase_default = 0.7, power1_default = 2;

	if (my voicingAmplitude -> points -> size == 0) return Melder_errorp1 (L"Amplitude of voicing tier is empty.");
	if (glottisOpenDurations != NULL)
	{
		*glottisOpenDurations = RealTier_create (my xmin, my xmax);
		if (*glottisOpenDurations == NULL) return NULL;
	}
	
	if (my f0 -> points -> size == 0) return Melder_errorp1 (L"Pitch tier is empty.");
	PointProcess  point = PitchTier_to_PointProcess_flutter (my f0, my flutter, p -> maximumPeriod);
	if (point == NULL) goto end;
	
	thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
	if (thee == NULL) goto end;
	
	if (my breathynessAmplitude -> points -> size > 0)
	{
		breathy = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
		if (breathy == NULL) goto end;
	}

	/*
		Cycle through the points of the point PointProcess. Each will become a period.
		We assume that the planning for the pitch period occurs approximately at a time T before the glottal closure.
		For each point t[i]:
			Determine the f0 -> period T[i]
			Determine time t[i]-T[i] the open quotient, power1, power2, collisionphase etc.
			Generate the period.
	 */
	double *sound = thy z[1];
	for (long it = 1; it <= point -> nt; it++)
	{
		double t = point -> t[it];		// the glottis "closing" point
		double period = NUMundefined; // duration of the current period
		double pulseDelay = 0;        // For alternate pulses in case of diplophonia
		double pulseScale = 1;        // For alternate pulses in case of diplophonia
		double phase;                 // 0..1
		double flow;

		// Determine the period: first look left (because that's where the open phase is), then right.
		
		period = PointProcess_getPeriodAtIndex (point, it, p -> maximumPeriod);
		if (period == NUMundefined)
		{
			period = 0.5 * p -> maximumPeriod; // Some default value
		}
		
		// Calculate the point where the exponential decay starts: 
		// Query tiers where period starts .
		
		double periodStart = t - period; // point where period starts:
		double collisionPhase, power1, power2, re;
		
		if (p -> klatt80)
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

		re *= openPhase; // relative duration before exponential decay

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
				double nextPeriod = PointProcess_getPeriodAtIndex (point, it+1, p -> maximumPeriod);
				if (nextPeriod == NUMundefined) nextPeriod = period;
				openPhase = openPhase_default;
				if (my openPhase -> points -> size > 0) openPhase = RealTier_getValueAtTime (my openPhase, t);
				double maxDelay = period * openPhase;
				pulseDelay = maxDelay * doublePulsing;
				pulseScale *= (1 - doublePulsing);
			}
		}
		else
		{
			diplophonicPulseIndex = 0;
		}		

		t += pulseDelay;

		//- double amplitude = pulseScale * (power1 + power2 + 1.0) / (power2 - power1);
		//- amplitude /= period * openPhase;
		/*
		  Maximum of U(x) = x^n - x^m is where the derivative U'(x) = n x^(n-1) - m x^(m-1) == 0,
			i.e. (n/m) = x^(m-n), so xmax = (n/m)^(1/(m-n))
			U(xmax) = x^n (1-x^(m-n)) = (n/m)^(n/(m-n))(1-n/m)
		*/

		double amplitude = pulseScale / (pow (power1/power2, 1 / (power2/power1 - 1)) * (1 - power1 / power2));
		
		// Fill in the samples to the left of the current point.

		long midSample = Sampled_xToLowIndex (thee, t), beginSample;
		double te = re * period;
		beginSample = midSample - floor (te / thy dx);
		if (beginSample < 1) beginSample = 1;
		for (i = beginSample; i <= midSample; i++)
		{
			double tsamp = thy x1 + (i - 1) * thy dx;
			phase = (tsamp - (t - te)) / (period * openPhase);
			if (phase > 0.0)
			{
				flow = amplitude * (pow (phase, power1) - pow (phase, power2));
				sound[i] += flow;

				// Breathyness only during open part modulated by the flow
				if (breathy)
				{
					double val = flow * NUMrandomUniform (-1, 1);
					double a = RealTier_getValueAtTime (my breathynessAmplitude, t);
					breathy -> z[1][i] += val * DB_to_A (a);
				}
			}
		}

		if (*glottisOpenDurations && ! RealTier_addPoint (*glottisOpenDurations, t, te)) goto end;

		// Determine the signal parameters at the current point.

		phase = te / (period * openPhase);
		
		//- double flow = amplitude * (period * openPhase) * (pow (phase, power1) - pow (phase, power2));

		flow = amplitude * (pow (phase, power1) - pow (phase, power2));

		// Fill in the samples to the right of the current point.

		if (flow > 0.0)
		{
			double ta = collisionPhase * (period * openPhase);
			double factorPerSample = exp (- thy dx / ta);
			double value = flow * exp (-(thy x1 + midSample * thy dx - t) / ta);
			long endSample = midSample + floor (20 * ta / thy dx);
			if (endSample > thy nx) endSample = thy nx;
			for (i = midSample + 1; i <= endSample; i++)
			{
				sound[i] += value;
				value *= factorPerSample;
			}
		}
	}

	// Scale voiced part and add breathyness during open phase
	
	for (i = 1; i <= thy nx; i++)
	{
		double t = thy x1 + (i - 1) * thy dx;
		thy z[1][i] *= DB_to_A (RealTier_getValueAtTime (my voicingAmplitude, t));
		if (breathy) thy z[1][i] += breathy -> z[1][i];
	}
	
end:
	forget (point); forget (breathy);
	if (Melder_hasError ())	forget (thee);
	return thee;
}

Sound PhonationGrid_to_Sound (PhonationGrid me, synthesisParams p, RealTier *glottisOpenDurations)
{
	Sound thee = NULL;
	if (p -> voicing)
	{
		thee = PhonationGrid_to_Sound_voiced (me, p, glottisOpenDurations);
		if (thee == NULL) return NULL;
		if (p -> spectralTilt) Sound_PhonationGrid_spectralTilt_inline (thee, me);
	}
	if (p -> aspiration)
	{
		Sound aspiration = PhonationGrid_to_Sound_aspiration (me, p);
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

/********************** DeltaFormantGrid *************************************/

class_methods (DeltaFormantGrid, FormantGrid)
{
	class_method_local (DeltaFormantGrid, destroy)
	class_method_local (DeltaFormantGrid, copy)
	class_method_local (DeltaFormantGrid, equal)
	class_method_local (DeltaFormantGrid, canWriteAsEncoding)
	class_method_local (DeltaFormantGrid, writeText)
	class_method_local (DeltaFormantGrid, readText)
	class_method_local (DeltaFormantGrid, writeBinary)
	class_method_local (DeltaFormantGrid, readBinary)
	class_method_local (DeltaFormantGrid, description)
	class_methods_end
}

DeltaFormantGrid DeltaFormantGrid_create (double tmin, double tmax, long numberOfFormants)
{
	DeltaFormantGrid me = new (DeltaFormantGrid);
	if (me == NULL || ! FormantGrid_init (me, tmin, tmax, numberOfFormants)) return NULL;
	return me;
}

double DeltaFormantGrid_getDeltaFormantAtTime (DeltaFormantGrid me, long iformant, double t)
{
	if (iformant < 1 || iformant > my formants -> size) return NUMundefined;
	return RealTier_getValueAtTime (my formants -> item[iformant], t);
}

double DeltaFormantGrid_getDeltaBandwidthAtTime (DeltaFormantGrid me, long iformant, double t)
{
	if (iformant < 1 || iformant > my bandwidths -> size) return NUMundefined;
	return RealTier_getValueAtTime (my bandwidths -> item[iformant], t);
}

int FormantGrid_DeltaFormantGrid_updateOpenPhases (I, DeltaFormantGrid delta, RealTier glottisOpenDurations, double fadeFraction)
{
	iam (FormantGrid);
	RealTier deltaTier;
	for (long itier = 1; itier <= delta -> formants -> size; itier++)
	{
		deltaTier = delta -> formants -> item[itier];
		if (itier <= my formants -> size)
		{
			RealTier tier = delta -> formants -> item[itier];
			if (tier -> points -> size > 0)
			{
				RealTier rt = RealTier_updateWithDelta (my formants -> item[itier], deltaTier, glottisOpenDurations, fadeFraction);
				if (rt == NULL) goto end;
				forget (my formants -> item[itier]);
				my formants -> item[itier] = rt;
			}
		}
		deltaTier = delta -> bandwidths -> item[itier];
		if (itier <= my bandwidths -> size)
		{
			RealTier tier = delta -> bandwidths -> item[itier];
			if (tier -> points -> size > 0)
			{
				RealTier rt = RealTier_updateWithDelta (my bandwidths -> item[itier], deltaTier, glottisOpenDurations, fadeFraction);
				if (rt == NULL) goto end;
				forget (my bandwidths -> item[itier]);
				my bandwidths -> item[itier] = rt;
			}
		}
	}	
end:
	return ! Melder_hasError ();
}

/**************************** FormantGridP ****************************/

class_methods (FormantGridP, FormantGrid)
{
	class_method_local (FormantGridP, destroy)
	class_method_local (FormantGridP, copy)
	class_method_local (FormantGridP, equal)
	class_method_local (FormantGridP, canWriteAsEncoding)
	class_method_local (FormantGridP, writeText)
	class_method_local (FormantGridP, readText)
	class_method_local (FormantGridP, writeBinary)
	class_method_local (FormantGridP, readBinary)
	class_method_local (FormantGridP, description)
	class_methods_end
}

FormantGridP FormantGridP_create (double tmin, double tmax, long numberOfFormants)
{
	FormantGridP me = new (FormantGridP);
	if (me == NULL || ! FormantGrid_init (me, tmin, tmax, numberOfFormants)) goto end;
	my amplitudes = Ordered_create ();
	if (my amplitudes == NULL) goto end;
	for (long iformant = 1; iformant <= numberOfFormants; iformant++)
	{
		RealTier amplitude = RealTier_create (tmin, tmax);
		if (amplitude == NULL ||
			! Collection_addItem (my amplitudes, amplitude) ) goto end;
	}

end:
	if (Melder_hasError ()) forget (me);
	return me;
}

double FormantGridP_getAmplitudeAtTime (FormantGridP me, long iformant, double t)
{
	if (iformant < 1 || iformant > my amplitudes -> size) return NUMundefined;
	return RealTier_getValueAtTime (my amplitudes -> item[iformant], t);
}

int FormantGridP_addAmplitudePoint (FormantGridP me, long iformant, double t, double value)
{
	if (iformant < 1 || iformant > my amplitudes -> size) return Melder_error1 (L"No such formant number.");
	RealTier tier = my amplitudes -> item[iformant];
	if (! RealTier_addPoint (tier, t, value)) return 0;
	return 1;
}

void FormantGridP_removeAmplitudePointsBetween (FormantGridP me, long iformant, double tmin, double tmax)
{
	if (iformant < 1 || iformant > my amplitudes -> size) return;
	AnyTier_removePointsBetween (my amplitudes -> item[iformant], tmin, tmax);
}

FormantGrid FormantGridP_downto_FormantGrid (FormantGridP me)
{
	FormantGrid thee = new (FormantGrid);
	if (thee == NULL) return NULL;
	thy formants = Data_copy (my formants);
	if (thy formants == NULL) goto end;
	thy bandwidths = Data_copy (my bandwidths);
	thy xmin = my xmin; thy xmax = my xmax;
end:
	if (Melder_hasError ()) forget (thee);
	return thee; 
}

/************************ Sound and FormantGrid(P) *********************************************/

static int _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, thou, long iformant, int antiformant)
{
	thouart (FormantGrid);
	if (iformant < 1 || iformant > thy formants -> size)
	{
		Melder_warning3 (L"Formant ", Melder_integer (iformant), L" does not exist.");
		return 1;
	}
	
	RealTier ftier = thy formants -> item[iformant];
	RealTier btier = thy bandwidths -> item[iformant];
	if (ftier -> points -> size == 0 || btier -> points -> size == 0)
	{
		Melder_warning3 (L"Formant ", Melder_integer (iformant), L" is empty.");
		return 1;
	}
	
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

int Sound_FormantGridP_filterWithOneFormant_inline (Sound me, FormantGridP thee, long iformant)
{
	if (iformant < 1 || iformant > thy formants -> size) return Melder_error3 (L"Formant ", Melder_integer (iformant), L" not defined. \nThis formant will not be used.");
	double nyquist = 0.5 / my dx;
	
	RealTier ftier = thy formants -> item[iformant];
	RealTier btier = thy bandwidths -> item[iformant];
	RealTier atier = thy amplitudes -> item[iformant];
	
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

Sound Sound_FormantGridP_filter (Sound me, FormantGridP thee, long iformantb, long iformante, int alternatingSign)
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
		if (Sound_FormantGridP_filterWithOneFormant_inline (tmp, thee, iformant))
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

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants, long numberOfNasalFormants, long numberOfTrachealFormants, long numberOfFricationFormants, long numberOfDeltaFormants) 
{
	KlattGrid me = new (KlattGrid);
	
	if (me == NULL || ! Function_init (me, tmin, tmax)) goto end;

	// If the user doesn't want any formant, allocate at least some space for it
	numberOfFormants = numberOfFormants <= 0 ? 1 : numberOfFormants;
	numberOfNasalFormants = numberOfNasalFormants <= 0 ? 1 : numberOfNasalFormants;
	numberOfTrachealFormants = numberOfTrachealFormants <= 0 ? 1 : numberOfTrachealFormants;
	numberOfFricationFormants = numberOfFricationFormants <= 0 ? 1 : numberOfFricationFormants;
	numberOfDeltaFormants = numberOfDeltaFormants <= 0 ? 1 : numberOfDeltaFormants;
	
	// Source
	my source = PhonationGrid_create (tmin, tmax);
	if (my source == NULL) goto end;
	
	// Filter
	if (((my formants = FormantGridP_create (tmin, tmax, numberOfFormants)) == NULL) ||
		((my nasal_formants = FormantGridP_create (tmin, tmax, numberOfNasalFormants)) == NULL) ||
		((my nasal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalFormants)) == NULL) ||
		((my tracheal_formants = FormantGridP_create (tmin, tmax, numberOfTrachealFormants)) == NULL) ||
		((my tracheal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealFormants)) == NULL)) goto end;
		
	// Coupling
	if	((my open_glottis_delta = DeltaFormantGrid_create (tmin, tmax, numberOfDeltaFormants)) == NULL) goto end;
	
	// Frication
	if (((my fricationAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my frication_formants = FormantGridP_create (tmin, tmax, numberOfFricationFormants)) == NULL) ||
		((my bypassAmplitude = IntensityTier_create (tmin, tmax)) == NULL)) goto end;
		
	// Output
	if	((my gain = IntensityTier_create (tmin, tmax)) == NULL) goto end;
	
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

int KlattGrid_replaceFormantGrid (KlattGrid me, thou)
{
	thouart (FormantGrid);
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal.");
	Ordered fc = Data_copy (thy formants);
	if (fc == NULL) return 0;
	
	Ordered bc = Data_copy (thy formants);
	if (bc != NULL)
	{
		forget (my formants -> formants);
		forget (my formants -> bandwidths);
		my formants -> formants = fc;
		my formants -> bandwidths = bc;
	}
	else
	{
		forget (fc);
		return 0;
	}
	return 1;
}

#define KlattGrid_GET_FORMANTGRID FormantGrid fg = gridType == 1 ? (FormantGrid) my formants : gridType == 2 ? (FormantGrid) my nasal_formants : gridType == 3 ? (FormantGrid) my tracheal_formants : gridType == 4 ? (FormantGrid) my frication_formants : gridType == 5 ? (FormantGrid) my nasal_antiformants : gridType == 6 ? my tracheal_antiformants : NULL;

#define KlattGrid_GET_FORMANTGRIDP FormantGridP fg = gridType == 1 ? my formants : gridType == 2 ? my nasal_formants : gridType == 3 ? my tracheal_formants : gridType == 4 ? my frication_formants : NULL;

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t)
{
	return RealTier_getValueAtTime (my bypassAmplitude, t);
}

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t)
{
	return RealTier_getValueAtTime (my fricationAmplitude, t);
}

double KlattGrid_getFormantAtTime (KlattGrid me, int gridType, long formantNumber, double t)
{
	KlattGrid_GET_FORMANTGRID	
	return fg != NULL ? FormantGrid_getFormantAtTime (fg, formantNumber, t) : NUMundefined;
}

double KlattGrid_getBandwidthAtTime (KlattGrid me, int gridType, long formantNumber, double t)
{
	KlattGrid_GET_FORMANTGRID	
	return fg != NULL ? FormantGrid_getBandwidthAtTime (fg, formantNumber, t) : NUMundefined;
}

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int gridType, long formantNumber, double t)
{
	KlattGrid_GET_FORMANTGRID	
	return fg != NULL ? FormantGridP_getAmplitudeAtTime ((FormantGridP)fg, formantNumber, t) : NUMundefined;
}

int KlattGrid_addBandwidthPoint (KlattGrid me, int gridType, long formantNumber, double t, double value)
{
	KlattGrid_GET_FORMANTGRID	
	return fg != NULL && FormantGrid_addBandwidthPoint (fg, formantNumber, t, value);
}

void KlattGrid_removeBandwidthPointsBetween (KlattGrid me, long gridType, long iformant, double t1, double t2)
{
	KlattGrid_GET_FORMANTGRID
	FormantGrid_removeBandwidthPointsBetween (fg, iformant, t1, t2);
}

int KlattGrid_addFormantPoint (KlattGrid me, int gridType, long formantNumber, double t, double value)
{
	KlattGrid_GET_FORMANTGRID	
	return fg != NULL && FormantGrid_addFormantPoint (fg, formantNumber, t, value);
}

void KlattGrid_removeFormantPointsBetween (KlattGrid me, long gridType, long iformant, double t1, double t2)
{
	KlattGrid_GET_FORMANTGRID
	FormantGrid_removeFormantPointsBetween (fg, iformant, t1, t2);
}

int KlattGrid_addDeltaBandwidthPoint (KlattGrid me, long formantNumber, double t, double value)
{	
	return FormantGrid_addBandwidthPoint ((FormantGrid)(my open_glottis_delta), formantNumber, t, value);
}

void KlattGrid_removeDeltaBandwidthPointsBetween (KlattGrid me, long iformant, double t1, double t2)
{
	FormantGrid_removeBandwidthPointsBetween ((FormantGrid)(my open_glottis_delta), iformant, t1, t2);
}

int KlattGrid_addDeltaFormantPoint (KlattGrid me, long formantNumber, double t, double value)
{	
	return FormantGrid_addFormantPoint ((FormantGrid)(my open_glottis_delta), formantNumber, t, value);
}

void KlattGrid_removeDeltaFormantPointsBetween (KlattGrid me, long iformant, double t1, double t2)
{
	FormantGrid_removeFormantPointsBetween ((FormantGrid)(my open_glottis_delta), iformant, t1, t2);
}

int KlattGrid_addAmplitudePoint (KlattGrid me, int gridType, long formantNumber, double t, double value)
{
	KlattGrid_GET_FORMANTGRIDP	
	return fg != NULL && FormantGridP_addAmplitudePoint (fg, formantNumber, t, value);
}

void KlattGrid_removeAmplitudePointsBetween (KlattGrid me, long gridType, long iformant, double t1, double t2)
{
	KlattGrid_GET_FORMANTGRIDP
	FormantGridP_removeAmplitudePointsBetween (fg, iformant, t1, t2);
}

int KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value)
{
	return RealTier_addPoint (my bypassAmplitude, t, value);
}

void KlattGrid_removeFricationBypassPointsBetween (KlattGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my bypassAmplitude, t1, t2);
}

int KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value)
{
	return RealTier_addPoint (my fricationAmplitude, t, value);
}

void KlattGrid_removeFricationAmplitudePointsBetween (KlattGrid me, double t1, double t2)
{
	AnyTier_removePointsBetween (my bypassAmplitude, t1, t2);
}

Sound KlattGrid_to_Sound_aspiration (KlattGrid me, synthesisParams p)
{
	return PhonationGrid_to_Sound_aspiration (my source, p);
}

Sound KlattGrid_to_Sound_frication (KlattGrid me, synthesisParams p)
{	
	Sound thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
	if (thee == NULL) return NULL;

	double lastval = 0;
	for (long i = 1; i <= thy nx; i++)
	{
		double t = thy x1 + (i - 1) * thy dx;
		double val = NUMrandomUniform (-1, 1);
		double a = 0;
		if (my fricationAmplitude -> points -> size > 0)
		{
			double dba = RealTier_getValueAtTime (my fricationAmplitude, t);
			a = dba == NUMundefined ? 0 : DB_to_A (dba);
		}
		lastval = (val += 0.75 * lastval); // soft low-pass
		thy z[1][i] = val * a;
	}

	Sound him = Sound_KlattGrid_filter_frication (thee, me, p);
	forget (thee);
	return him;	
}

Sound KlattGrid_to_Sound (KlattGrid me, synthesisParams p)
{
	Sound thee = NULL, frication = NULL;
	RealTier glottisOpenDurations;
	
	if (p -> aspiration || p -> voicing) // No need for filtering if no glottal source signal present
	{
		Sound source = PhonationGrid_to_Sound (my source, p, &glottisOpenDurations);
		if (source == NULL) goto end;
		
		thee = Sound_KlattGrid_filter_laryngial (source, me, glottisOpenDurations, p);
		
		forget (source);
		
		if (thee == NULL) goto end;
	}

	if (p -> endFricationFormant > 0 || p -> fricationBypass)
	{
		frication = KlattGrid_to_Sound_frication (me, p);
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
	if (Melder_hasError ()) forget (thee);
	return thee;	
}

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, int parallel)
{
	struct synthesisParams p;
	synthesisParams_setDefault (&p, me);
	p.filterModel = parallel ? 1 : 0;
	p.samplingFrequency = samplingFrequency;
	return KlattGrid_to_Sound (me, &p);
}

/************************* Sound(s) & KlattGrid **************************************************/

Sound Sound_KlattGrid_filter_laryngial (Sound me, KlattGrid thee, RealTier glottisOpenDurations, synthesisParams p)
{
		return p -> filterModel == 1 ? Sound_KlattGrid_filter_laryngial_parallel (me, thee, glottisOpenDurations, p) : Sound_KlattGrid_filter_laryngial_cascade (me, thee, glottisOpenDurations, p);
}

Sound Sound_KlattGrid_filter_laryngial_parallel (Sound me, KlattGrid thee, RealTier glottisOpenDurations, synthesisParams params)
{
	Sound f1 = NULL, vocalTract = NULL, trachea = NULL, nasal = 0, him = NULL;
	struct synthesisParams p; if (params == NULL) synthesisParams_setDefault (&p, thee); else p = *params;
	FormantGridP formantsp = thy formants;
	int alternatingSign = 0; // 0: no alternating signs in parallel adding of filter outputs, 1/-1 start sign
	long numberOfFormants = p.klatt80 ? 4 : thy formants -> formants -> size;
	long numberOfNasalFormants = p.klatt80 ? 1 : thy nasal_formants -> formants -> size;
	long numberOfTrachealFormants = p.klatt80 ? 1 : thy tracheal_formants -> formants -> size;
	
	check_formants (numberOfFormants, &(p.startFormant), &(p.endFormant));
	check_formants (numberOfNasalFormants, &(p.startNasalFormant), &(p.endNasalFormant));
	check_formants (numberOfTrachealFormants, &(p.startTrachealFormant), &(p.endTrachealFormant));

	if (glottisOpenDurations)
	{
		formantsp = Data_copy (thy formants);
		if (formantsp == NULL || ! FormantGrid_DeltaFormantGrid_updateOpenPhases (formantsp, thy open_glottis_delta, glottisOpenDurations,
			p.openglottis_fadeFraction)) goto end;
	}
	
	if (p.endFormant > 0)
	{
		if (p.startFormant == 1)
		{
			him = Data_copy (me);
			if (him == NULL) return NULL;
			if (formantsp -> formants -> size > 0 &&
				! Sound_FormantGridP_filterWithOneFormant_inline (him, formantsp, 1)) goto end;
		}

		// Formants 2 and up, with alternating signs.
		// We perform the pre-emphasis by differentiating afterwards in the summation step.
	
		if (p.endFormant >= 2) 
		{
			long startFormant2 = p.startFormant > 2 ? p.startFormant : 2;
			alternatingSign = startFormant2 % 2 == 0 ? -1 : 1; // 2 starts with negative sign
			if (startFormant2 <= formantsp -> formants -> size)
			{
				vocalTract = Sound_FormantGridP_filter (me, formantsp, startFormant2, p.endFormant, alternatingSign);
				if (vocalTract == NULL) goto end;
			
				if (him == NULL)
				{
					him = Data_copy (vocalTract);
					if (him == NULL) goto end;
				}
				else _Sounds_addDifferentiated_inline (him, vocalTract);
			}
		}
	}

	if (p.endTrachealFormant > 0) // Tracheal formants
	{
		alternatingSign = 0;
		trachea =  Sound_FormantGridP_filter (me, thy tracheal_formants, p.startTrachealFormant, p.endTrachealFormant, alternatingSign);
		if (trachea == NULL) goto end;

		if (him == NULL)
		{
			him = Data_copy (trachea);
			if (him == NULL) goto end;
		}
		else _Sounds_addDifferentiated_inline (him, trachea);
	}
	
	if (p.endNasalFormant > 0)
	{
		alternatingSign = 0;
		nasal =  Sound_FormantGridP_filter (me, thy nasal_formants, p.startNasalFormant, p.endNasalFormant, alternatingSign);
		if (nasal == NULL) goto end;

		if (him == NULL)
		{
			him = Data_copy (nasal);
			if (him == NULL) goto end;
		}
		else _Sounds_addDifferentiated_inline (him, nasal);
	}

	if (him == NULL) him = Data_copy (me);
	
end:
	forget (vocalTract); forget (f1); forget (trachea); forget (nasal);
	if (glottisOpenDurations) forget (formantsp);
	if (Melder_hasError ()) forget (him);
	return him;	
}

Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee, synthesisParams params)
{
	Sound him = NULL;
	struct synthesisParams p; if (params == NULL) synthesisParams_setDefault (&p, thee); else p = *params;
	FormantGridP fgridp = thy frication_formants;
	long numberOfFricationFormants = p.klatt80 ? (fgridp -> formants -> size > 6 ? 6 : fgridp -> formants -> size) : fgridp -> formants -> size;
	
	check_formants (numberOfFricationFormants, &(p.startFricationFormant), &(p.endFricationFormant));

	if (p.endFricationFormant > 1)
	{
		long startFricationFormant2 = p.startFricationFormant > 2 ? p.startFricationFormant : 2;
		int alternatingSign = startFricationFormant2 % 2 == 0 ? 1 : -1; // 2 starts with positive sign
		him = Sound_FormantGridP_filter (me, fgridp, startFricationFormant2, p.endFricationFormant, alternatingSign);
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
			if (thy bypassAmplitude -> points -> size > 0)
			{
				double val = RealTier_getValueAtTime (thy bypassAmplitude, t);
				ab = val == NUMundefined ? 0 : DB_to_A (val);
			}
			his z[1][is] += my z[1][is] * ab;
		}
	}
	
	return him;
}

Sound Sound_KlattGrid_filter_laryngial_cascade (Sound me, KlattGrid thee, RealTier glottisOpenDurations, synthesisParams params)
{
	struct synthesisParams p; if (params == NULL) synthesisParams_setDefault (&p, thee); else p = *params;
	FormantGrid formantsp = (FormantGrid) thy formants;
	FormantGrid nasal_formants = (FormantGrid) thy nasal_formants;
	FormantGrid nasal_antiformants = thy nasal_antiformants;
	FormantGrid tracheal_formants = (FormantGrid) thy tracheal_formants;
	FormantGrid tracheal_antiformants = thy tracheal_antiformants;
	
	long iformant; int antiformants = 0;
	long numberOfFormants = p.klatt80 ? 5 : thy formants -> formants -> size;
	long numberOfTrachealFormants = p.klatt80 ? 1 : thy tracheal_formants -> formants -> size;
	long numberOfTrachealAntiFormants = p.klatt80 ? 1 : thy tracheal_antiformants -> formants -> size;
	long numberOfNasalFormants = p.klatt80 ? 1 : thy nasal_formants -> formants -> size;
	long numberOfNasalAntiFormants = p.klatt80 ? 1 : thy nasal_antiformants -> formants -> size;
	
	check_formants (numberOfFormants, & p.startFormant, & p.endFormant);
	check_formants (numberOfNasalFormants, &p.startNasalFormant, & p.endNasalFormant);
	check_formants (numberOfTrachealFormants, & p.startTrachealFormant, & p.endTrachealFormant);
	check_formants (numberOfNasalAntiFormants, & p.startNasalAntiFormant, & p.endNasalAntiFormant);
	check_formants (numberOfTrachealAntiFormants, & p.startTrachealAntiFormant, & p.endTrachealAntiFormant);

	Sound him = Data_copy (me);
	if (him == NULL) return NULL;
	
	if (glottisOpenDurations)
	{
		formantsp = Data_copy (thy formants);
		if (formantsp == NULL || ! FormantGrid_DeltaFormantGrid_updateOpenPhases (formantsp, thy open_glottis_delta, glottisOpenDurations,
			p.openglottis_fadeFraction)) goto end;
	}
	
	if (p.endNasalFormant > 0)  // Nasal formants
	{
		antiformants = 0;
		for (iformant = p.startNasalFormant; iformant <= p.endNasalFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, nasal_formants, iformant, antiformants);
//			if (Melder_hasError ()) Melder_clearError (); // formant does not exist, no problem
		}
	}
	
	if (p.endNasalAntiFormant > 0)  // Nasal anti formants
	{
		antiformants = 1;
		for (iformant = p.startNasalAntiFormant; iformant <= p.endNasalAntiFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, nasal_antiformants, iformant, antiformants);
		}
	}

	if (p.endTrachealFormant > 0)  // Tracheal formants
	{
		antiformants = 0;
		for (iformant = p.startTrachealFormant; iformant <= p.endTrachealFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_formants, iformant, antiformants);
		}
	}
	
	if (p.endTrachealAntiFormant > 0)  // Tracheal anti formants
	{
		antiformants = 1;
		for (iformant = p.startTrachealAntiFormant; iformant <= p.endTrachealAntiFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_antiformants, iformant, antiformants);
		}
	}
	
	if (p.endFormant > 0)  // Normal formants
	{
		antiformants = 0;
		for (iformant = p.startFormant; iformant <= p.endFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, formantsp, iformant, antiformants);
		}
	}
end:	
//	if (Melder_hasError ()) Melder_clearError (); // formant does not exist, no problem
	return him;
}

Sound Sounds_KlattGrid_filter_allSources (Sound phonation, Sound frication, KlattGrid me, RealTier glottisOpenDurations, synthesisParams p)
{
	Sound thee = NULL;

	if (phonation)
	{
		thee = Sound_KlattGrid_filter_laryngial (phonation, me, glottisOpenDurations, p);
		if (thee == NULL) return NULL;
	}
	if (frication)
	{
//		if (! (p -> startFricationFormant == 0 && p -> noFricationBypass))
		if (! (p -> endFricationFormant == 0 && ! p -> fricationBypass))
		{
			Sound ff = Sound_KlattGrid_filter_frication (frication, me, p);
			if (ff == NULL) goto end;
	
			if (thee != NULL) thee = ff;
			else
			{
				 _Sounds_add_inline (thee, ff);
				 forget (ff);
			}
		}
	}
	if (thee == NULL) thee = Sound_createEmptyMono (my xmin, my xmax, p -> samplingFrequency);
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

/*********************** KlattGrid & Tier ************************/

int KlattGrid_replacePitchTier (KlattGrid me, PitchTier thee)
{
	return PhonationGrid_replacePitchTier (my source, thee);
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
	long numberOfTrachealFormants = 1;
	long numberOfFricationFormants = 6;
	long numberOfDeltaFormants = 1;
	
	KlattGrid thee = KlattGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants,
		 numberOfTrachealFormants, numberOfFricationFormants, numberOfDeltaFormants);
	if (thee == NULL) return NULL;
	
	for (long irow = 1; irow <= nrows; irow++)
	{
		double t = (irow - 1) * frameDuration;
		double val, f0, fk;
		long icol = 1;
		
		f0 = val = Table_getNumericValue (kt, irow, icol) / 10; // F0hz10
		RealTier_addPoint (thy source -> f0, t, f0);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AVdb
		// dB values below 13 were put to zero in the DBtoLIN function
		val -= 7;
		if (val < 13) val = dBNul;
//		RealTier_addPoint (thy source -> voicingAmplitude, t, val);
		for (long kf = 1; kf <= 6; kf++)
		{
			icol++; fk = val = Table_getNumericValue (kt, irow, icol); // Fhz
			RealTier_addPoint (thy formants -> formants -> item[kf], t, val);
			RealTier_addPoint (thy frication_formants -> formants -> item[kf], t, val); // only amplitudes and bandwidths in frication section
			icol++; val = Table_getNumericValue (kt, irow, icol); // Bhz
			if (val <= 0) val = fk / 10;
			RealTier_addPoint (thy formants -> bandwidths -> item[kf], t, val);
		}
		icol++; val = Table_getNumericValue (kt, irow, icol); // FNZhz
		RealTier_addPoint (thy nasal_antiformants -> formants -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // BNZhz
		RealTier_addPoint (thy nasal_antiformants -> bandwidths -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // FNPhz
		RealTier_addPoint (thy nasal_formants -> formants -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // BNPhz
		RealTier_addPoint (thy nasal_formants -> bandwidths -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // ah
		if (val < 13) val = dBNul; else val += 20 * log10 (0.05) + dB_offset_noise;
		RealTier_addPoint (thy source -> aspirationAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Kopen
		double openPhase = f0 > 0 ? (val / 16000) * f0 : 0.7;
		RealTier_addPoint (thy source -> openPhase, t, openPhase);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Aturb breathynessAmplitude during voicing (max is 8192)
		if (val < 13) val = dBNul; else val += 20 * log10 (0.1) + dB_offset_noise;
		RealTier_addPoint (thy source -> breathynessAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // TLTdb
		RealTier_addPoint (thy source -> spectralTilt, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AF
		if (val < 13) val = dBNul; else val += 20 * log10 (0.25) + dB_offset_noise;
		RealTier_addPoint (thy fricationAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Kskew ???
		//RealTier_addPoint (, t, val);
		for (long kf = 1; kf <= 6; kf++)
		{
			icol++; val = Table_getNumericValue (kt, irow, icol); // Ap
			if (val < 13) val = dBNul; else val += 20 * log10 (ap[kf]) + dB_offset;
			RealTier_addPoint (thy formants -> amplitudes -> item[kf], t, val);
			RealTier_addPoint (thy frication_formants -> amplitudes -> item[kf], t, val);
			icol++; val = Table_getNumericValue (kt, irow, icol); // Bhz
			RealTier_addPoint (thy frication_formants -> bandwidths -> item[kf], t, val);
		}
		
		icol++; val = Table_getNumericValue (kt, irow, icol); // ANP
		if (val < 13) val = dBNul; else val += 20 * log10 (0.6) + dB_offset;
		RealTier_addPoint (thy nasal_formants -> amplitudes -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AB
		if (val < 13) val = dBNul; else val += 20 * log10 (0.05) + dB_offset_noise;
		RealTier_addPoint (thy bypassAmplitude, t, val);
		
		icol++; val = Table_getNumericValue (kt, irow, icol); // AVpdb
		RealTier_addPoint (thy source -> voicingAmplitude, t, val + dB_offset_voicing);
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

/* End of file KlattGrid.c */
