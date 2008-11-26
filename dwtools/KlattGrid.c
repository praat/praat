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

/*
	Amplitude scaling: maximum amplitude (-1,+1) corresponds to 91 dB

*/

typedef struct synthesisParams {
	double samplingFrequency;
	double maximumPeriod;
	int filterModel; // 1: parallel, 2: cascade
	long startFormant, endFormant;
	long startNasalFormant, endNasalFormant;
	long startTrachealFormant, endTrachealFormant;
	long startNasalAntiFormant, endNasalAntiFormant;
	long startTrachealAntiFormant, endTrachealAntiFormant;
	int noFrication, noVoicing, noAspiration;

} *synthesisParams;

static void synthesisParams_setDefault (synthesisParams p, KlattGrid thee)
{
	p -> samplingFrequency = 44100; p -> maximumPeriod = 0.05; p -> filterModel = 1;
	p -> endFormant = thy formants -> formants -> size;
	p -> startFormant = p -> endFormant >= 1 ? 1 : 0;
	p -> endNasalFormant = thy nasal_formants -> formants -> size;
	p -> startNasalFormant = p -> endNasalFormant > 0 ? 1 : 0;
	p -> endTrachealFormant = thy tracheal_formants -> formants -> size;
	p -> startTrachealFormant = p -> endTrachealFormant > 0 ? 1 : 0;
	p -> endNasalAntiFormant = thy nasal_antiformants -> formants -> size;
	p -> startNasalAntiFormant = p -> endNasalAntiFormant > 0 ? 1 : 0;
	p -> endTrachealAntiFormant = thy tracheal_antiformants -> formants -> size;
	p -> startTrachealAntiFormant = p -> endTrachealAntiFormant > 0 ? 1 : 0;
	p -> noFrication =  p -> noVoicing = p -> noAspiration = 0;
}

static struct synthesisParams synthesisParams_createDefault (KlattGrid me)
{
	struct synthesisParams p;
	synthesisParams_setDefault (&p, me);
	return p;
}

int RealTier_updateMicroIntonationInterval (RealTier me, RealTier thee, double tb, double te, double fadeFraction);

double PointProcess_getPeriodAtIndex (PointProcess me, long it, double maximumPeriod);

PitchTier PhonationGrid_to_PitchTier_flutter (PhonationGrid me, double deltaT);
Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency);
Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, double samplingFrequency, double maximumPeriod, RealTier df1, RealTier db1, RealTier *df1_micro, RealTier *db1_micro, int klatt88);

int RealTier_updateMicroIntonationInterval (RealTier me, RealTier thee, double tb, double te, double transitionFraction);
RealTier RealTier_copyInterval (I, double t1, double t2, int interpolateBorders);

int Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant);
int Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant);

Sound Sound_FormantGridP_filter (Sound me, FormantGridP thee, long iformantb, long iformante, int alternatingSign);
int Sound_FormantGridP_filterWithOneFormant_inline (Sound me, FormantGridP thee, long iformant, RealTier dftier, RealTier dbtier);

int FormantGridP_addAmplitudePoint (FormantGridP me, long iformant, double t, double value);
void FormantGridP_removeAmplitudePointsBetween (FormantGridP me, long iformant, double tmin, double tmax);
double FormantGridP_getAmplitudeAtTime (FormantGridP me, long iformant, double t);
FormantGrid FormantGridP_downto_FormantGrid (FormantGridP me);

Sound Sound_KlattGrid_filter_cascade_laryngial (Sound me, KlattGrid thee, synthesisParams params);
Sound Sound_KlattGrid_filter_parallel_laryngial (Sound me, KlattGrid thee, synthesisParams params);
Sound Sound_KlattGrid_filter_parallel_frication (Sound me, KlattGrid thee, synthesisParams params);

int KlattGrid_replaceFormantGrid (KlattGrid me, FormantGrid thee);
Sound KlattGrid_to_Sound_frication (KlattGrid me, synthesisParams params);

// Maximum amplitue (-1,1) at 91 dB
#define DB_to_A(x) (pow (10.0, x / 20.0) * 2.0e-5)

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

static int PhonationGrid_setDefaults (PhonationGrid me)
{
	double tmid = (my xmax + my xmin) / 2;
	if (! RealTier_addPoint (my power1, tmid, 2) ||
	    ! RealTier_addPoint (my power2, tmid, 3) ||
	    ! RealTier_addPoint (my amplitudeOfVoicing, tmid, 60) ||
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
		((my amplitudeOfVoicing = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my openPhase = RealTier_create (tmin, tmax)) == NULL) ||
		((my collisionPhase = RealTier_create (tmin, tmax)) == NULL) ||
		((my power1 = RealTier_create (tmin, tmax)) == NULL) ||
		((my power2 = RealTier_create (tmin, tmax)) == NULL) ||
		((my flutter = RealTier_create (tmin, tmax)) == NULL) ||
		((my doublePulsing = RealTier_create (tmin, tmax)) == NULL) ||
		((my extraSpectralTilt = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my aspirationAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my breathyness = IntensityTier_create (tmin, tmax)) == NULL)) forget (me);
	return me;
}

FormantGridP FormantGridP_create (double tmin, double tmax, long numberOfFormants)
{
	FormantGridP me = new (FormantGridP);
	if (! FormantGrid_init (me, tmin, tmax, numberOfFormants)) goto end;
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


KlattGrid KlattGrid_create (double tmin, double tmax)
{
	long numberOfFormants = 6;
	long numberOfNasalFormants = 1;
	long numberOfTrachealFormants = 1;
	long numberOfFricationFormants = 6;
	KlattGrid me = new (KlattGrid);
	
	if (me == NULL) return NULL;

	if (! Function_init (me, tmin, tmax) ||
		((my formants = FormantGridP_create (tmin, tmax, numberOfFormants)) == NULL) ||
		((my nasal_formants = FormantGridP_create (tmin, tmax, numberOfNasalFormants)) == NULL) ||
		((my nasal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalFormants)) == NULL) ||
		((my tracheal_formants = FormantGridP_create (tmin, tmax, numberOfTrachealFormants)) == NULL) ||
		((my tracheal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealFormants)) == NULL)) goto end;
	if ((my open_glottis_delta_f1 = RealTier_create (tmin, tmax)) == NULL) goto end;
	if ((my open_glottis_delta_b1 = RealTier_create (tmin, tmax)) == NULL) goto end;

	my source = PhonationGrid_create (tmin, tmax);
	if (my source == NULL) goto end;
	
	if (((my fricationAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my frication_formants = FormantGridP_create (tmin, tmax, numberOfFricationFormants)) == NULL) ||
		((my bypassAmplitude = IntensityTier_create (tmin, tmax)) == NULL) ||
		((my gain = IntensityTier_create (tmin, tmax)) == NULL)) goto end;
end:
	if (Melder_hasError ()) forget (me);
	return me;	
}


/*
	if numberOfFormants == 0 || ifb == 0 then no formant is used
	if ibe < ifb then all formants are used
	if ife > size then ife = size
	if ifb > numberOfFormants then no formant is used
	The indicator of no formants is ifb == 0!
*/
static void check_formants (long numberOfFormants, long *ifb, long *ife)
{
	if (numberOfFormants <= 0 || *ifb < 0 || *ifb > numberOfFormants) *ifb = 0;
	if (*ifb == 0) return;
	if (*ife < *ifb)
	{
		*ifb = 1; *ife = numberOfFormants;
	}
	if (*ife > numberOfFormants) *ife = numberOfFormants;
	return;
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

int KlattGrid_replaceFormantGrid (KlattGrid me, FormantGrid thee)
{
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_error1 (L"Domains must be equal.");
	Ordered fc = Data_copy (thy formants);
	if (fc == NULL) return 0;
	Ordered bc = Data_copy (thy bandwidths);
	if (bc == NULL)
	{
		forget (fc);
		return 0;
	}
	my formants -> formants = fc;
	my formants -> bandwidths = bc;
	return 1;
}

static double NUMinterpolateLinear (double x1, double y1, double x2, double y2, double x)
{
	if (y1 == y2) return y1;
	if (x1 == x2) return NUMundefined;
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}

RealTier RealTier_copyInterval (I, double t1, double t2, int interpolateBorders)
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

PitchTier PhonationGrid_to_PitchTier_flutter (PhonationGrid me, double deltaT)
{
	PitchTier thee = PitchTier_create (my xmin, my xmax);
	double t = my xmin;
	
	if (thee == NULL) return NULL;
	if (deltaT == 0) deltaT = 0.001; // 1 ms interpolation
	while (t <= my xmax)
	{
		double val = RealTier_getValueAtTime (my flutter, t); // 0..1
		if (val == NUMundefined) val = 0;
		double f0 = RealTier_getValueAtTime (my f0, t);
		double newF0 = f0 * (1 + (val / 50) * (sin (2*NUMpi*12.7*t) + sin (2*NUMpi*7.1*t) + sin (2*NUMpi*4.7*t)));
		if (! RealTier_addPoint (thee, t, newF0)) goto end;
		t += deltaT;
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

struct nrfunction_struct { double n; double m; double a;};
static void nrfunction (double x, double *fx, double *dfx, void *closure)
{
	struct nrfunction_struct *nrfs = (struct nrfunction_struct *) closure;
	double mplusax = nrfs->m + nrfs->a * x;
	double mminn = nrfs->m - nrfs->n;
	*fx = pow(x, mminn) - (nrfs->n + nrfs->a * x) / mplusax;
	*dfx = mminn * pow(x,mminn-1) - nrfs->a * mminn / (mplusax * mplusax);
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

double PointProcess_getPeriodAtIndex (PointProcess me, long it, double maximumPeriod)
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

Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, double samplingFrequency)
{
	long i, nt = 1 + floor ((my xmax - my xmin) * samplingFrequency);
	double dt = 1.0 / samplingFrequency;
	double tmid = (my xmin + my xmax) / 2;
	double t1 = tmid - 0.5 * (nt - 1) * dt;
	double val, lastval = 0;
	
	Sound thee = Sound_create (1, my xmin, my xmax, nt, dt, t1);
	if (thee == NULL) return NULL;
	
	// Noise spectrum is tilted down by soft low-pass filter having a pole near
	// the origin in the z-plane, i.e. y[n] = x[n] + (0.75 * y[n-1])

	for (i = 1; i <= nt; i++)
	{
		double t = thy x1 + (i - 1) * dt;
		val = NUMrandomUniform (-1, 1) * DB_to_A (RealTier_getValueAtTime (my aspirationAmplitude, t));
		thy z[1][i] = lastval = val + 0.75 * lastval;
	}
	return thee;
}

int RealTier_updateMicroIntonationInterval (RealTier me, RealTier thee, double tb, double te, double fadeFraction)
{
	double t2 = tb + fadeFraction * (te -tb);
	double t3 = te - fadeFraction * (te -tb) ;

	if (te <= tb) return 0;
	
	// TODO: also update points that happen to lie in interval (t2, t3)
	double df_t2 = RealTier_getValueAtTime (me, t2);
	if (df_t2 != NUMundefined)
	{
		double df_t3 = RealTier_getValueAtTime (me, t3);
		if (df_t3 != NUMundefined)
		{
			RealTier_addPoint (thee, tb, 0);
			RealTier_addPoint (thee, t2, df_t2);
			RealTier_addPoint (thee, t3, df_t3);
			RealTier_addPoint (thee, te, 0);
		}
	}
	return ! Melder_hasError ();
}

// Generate the source signal and optionally the microlevel df1 and df2 structure
Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, double samplingFrequency, double maximumPeriod, RealTier df1, RealTier db1, RealTier *df1_micro, RealTier *db1_micro, int klatt88)
{
	Sound thee = NULL, breathy = NULL;
	double deltaT = 0.001;
	double transitionFraction = 0.1; // 10% of open phase for formant frequency to increase, 10% to decrease
	long diplophonicPulseIndex = 0;
	double openPhase_default = 0.7, power1_default = 2;

	int get_df1 = df1 != NULL && df1_micro != NULL;
	int get_db1 = db1 != NULL && db1_micro != NULL;
	
	PitchTier pitch = PhonationGrid_to_PitchTier_flutter (me, deltaT);
	if (pitch == NULL) goto end;
	
	PointProcess point = PitchTier_to_PointProcess (pitch);
	if (point == NULL) goto end;
	
	long i, npoints = point -> nt;
	long sound_nt = ceil ((my xmax - my xmin) * samplingFrequency);
	double dt = 1.0 / samplingFrequency;
	double tmid = (my xmin + my xmax) / 2;
	double sound_t1 = tmid - 0.5 * (sound_nt - 1) * dt;
	
	thee = Sound_create (1, my xmin, my xmax, sound_nt, dt, sound_t1);
	if (thee == NULL) goto end;
	if (my breathyness -> points -> size > 0)
	{
		breathy = Sound_create (1, my xmin, my xmax, sound_nt, dt, sound_t1);
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
	for (long it = 1; it <= npoints; it++)
	{
		double t = point -> t[it];		// the glottis "closing" point
		double period = NUMundefined; // duration of the current period
		double pulseDelay = 0;        // For alternate pulses in case of diplophonia
		double pulseScale = 1;        // For alternate pulses in case of diplophonia
		double phase;                 // 0..1
		double flow;

		// Determine the period: first look left (because that's where the open phase is), then right.
		
		period = PointProcess_getPeriodAtIndex (point, it, maximumPeriod);
		if (period == NUMundefined)
		{
			period = 0.5 * maximumPeriod; // Some default value
		}
		
		// Calculate the point where the exponential decay starts: 
		// Query tiers where period starts .
		
		double periodStart = t - period; // point where period starts:
		double collisionPhase, power1, power2, re;
		
		if (klatt88)
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
				double nextPeriod = PointProcess_getPeriodAtIndex (point, it+1, maximumPeriod);
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

		double amplitude = 1 / (pow (power1/power2, 1 / (power2/power1 - 1)) * (1 - power1 / power2));
		
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
					double a = RealTier_getValueAtTime (my breathyness, t);
					breathy -> z[1][i] += val * DB_to_A (a);
				}
			}
		}
		
		// Modify the delta tiers during the open phase interval (t - te, t)

		if (get_df1 & ! RealTier_updateMicroIntonationInterval (df1, *df1_micro, t - te, t, transitionFraction)) goto end;
		if (get_db1 & ! RealTier_updateMicroIntonationInterval (db1, *db1_micro, t - te, t, transitionFraction)) goto end;

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
		thy z[1][i] *= DB_to_A (RealTier_getValueAtTime (my amplitudeOfVoicing, t));
		if (breathy) thy z[1][i] += breathy -> z[1][i];
	}
		
	if (my extraSpectralTilt -> points -> size > 0)
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

		for (i = 1; i <= thy nx; i++)
		{
			double t = thy x1 + (i - 1) * thy dx;
			double tilt_db = RealTier_getValueAtTime (my extraSpectralTilt, t);
		
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
	
end:
	forget (pitch);
	forget (point); forget (breathy);
	if (Melder_hasError ())	forget (thee);
	return thee;
}

static int _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant, int antiformant)
{
	if (iformant < 1 || iformant > thy formants -> size) return Melder_error1 (L"No such formant number.");
	double nyquist = 0.5 / my dx;
	Filter r =  antiformant != 0 ? (Filter) AntiResonator_create (my dx) : (Filter) Resonator_create (my dx);
	if (r == NULL) return 0;
	RealTier ftier = thy formants -> item[iformant];
	RealTier btier = thy bandwidths -> item[iformant];

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

int Sound_FormantGridP_filterWithOneFormant_inline (Sound me, FormantGridP thee, long iformant, RealTier dftier, RealTier dbtier)
{
	if (iformant < 1 || iformant > thy formants -> size) return Melder_error1 (L"No such formant number.");
	double nyquist = 0.5 / my dx;
	
	RealTier ftier = thy formants -> item[iformant];
	RealTier btier = thy bandwidths -> item[iformant];
	RealTier atier = thy amplitudes -> item[iformant];
	
	if (ftier -> points -> size == 0) return Melder_error3 (L"Frequencies of formant ", Melder_integer (iformant), L" not defined.");
	if (btier -> points -> size == 0) return Melder_error3 (L"Bandwidths of formant ", Melder_integer (iformant), L" not defined.");
	if (atier -> points -> size == 0) Melder_warning3 (L"Amplitudes of formant ", Melder_integer (iformant), L" not defined (amplitudes unmodified).");
	
	Resonator r = Resonator_create (my dx);
	if (r == NULL) return 0;

	for (long is = 1; is <= my nx; is++)
	{
		double t = my x1 + (is - 1) * my dx;
		double f = RealTier_getValueAtTime (ftier, t);
		double b = RealTier_getValueAtTime (btier, t);
		if (f <= nyquist && b != NUMundefined)
		{
			if (dftier && dftier -> points -> size > 0)
			{
				double df = RealTier_getValueAtTime (dftier, t);
				if (df == NUMundefined) df = 0;
				f += df;
			}
			if (dbtier && dbtier -> points -> size > 0)
			{
				double db = RealTier_getValueAtTime (dbtier, t);
				if (db == NUMundefined) db = 0;
				b += db;
			}
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
		if (! Sound_FormantGridP_filterWithOneFormant_inline (tmp, thee, iformant, NULL, NULL)) goto end;
		for (long is = 1; is <= my nx; is++)
		{
			his z[1][is] += alternatingSign >= 0 ? tmp -> z[1][is] : - tmp -> z[1][is];
		}
		forget (tmp);
		if (alternatingSign != 0) alternatingSign = -alternatingSign;
	}
end:
	if (Melder_hasError ()) forget (him);	
	return him;
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
	double pval = 0;
	for (long i = 1; i <= my nx; i++)
	{
		double val =  thy z[1][i];
		my z[1][i] += val - pval;
		pval = val;
	}
}

Sound Sound_KlattGrid_filter_parallel_laryngial (Sound me, KlattGrid thee, synthesisParams params)
{
	Sound f1 = NULL, vocalTract = NULL, trachea = NULL, nasal = 0, him = NULL;
	int alternatingSign = 0; // 0: no alternating signs in parallel adding of filter outputs, 1/-1 start sign
	long numberOfFormants = thy klatt88 ? 4 : thy formants -> formants -> size;
	long numberOfNasalFormants = thy klatt88 ? 1 : thy nasal_formants -> formants -> size;
	long numberOfTrachealFormants = thy klatt88 ? 1 : thy tracheal_formants -> formants -> size;
	struct synthesisParams p = params == NULL ? synthesisParams_createDefault (thee) : *params;
	synthesisParams pp = &p;
	
	check_formants (numberOfFormants, &(pp -> startFormant), &(pp -> endFormant));
	check_formants (numberOfNasalFormants, &(pp -> startNasalFormant), &(pp -> endNasalFormant));
	check_formants (numberOfTrachealFormants, &(pp -> startTrachealFormant), &(pp -> endTrachealFormant));

	// TODO: df and db tiers ???
	if (p.startFormant > 0)
	{
		if (p.startFormant == 1)
		{
			him = Data_copy (me);
			if (him == NULL) return NULL;
			if (thy formants -> formants -> size > 0 &&
				! Sound_FormantGridP_filterWithOneFormant_inline (him, thy formants, 1, NULL, NULL)) goto end;
		}

		// Formants 2 and up, with alternating signs.
		// We perform the pre-emphasis by differentiating afterwards in the summation step.
	
		if (p.endFormant >= 2) 
		{
			long startFormant2 = p.startFormant > 2 ? p.startFormant : 2;
			alternatingSign = startFormant2 % 2 == 0 ? -1 : 1; // 2 starts with negative sign
			if (startFormant2 <= thy formants -> formants -> size)
			{
				vocalTract = Sound_FormantGridP_filter (me, thy formants, startFormant2, p.endFormant, alternatingSign);
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

	if (p.startTrachealFormant > 0) // Tracheal formants
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
	
	if (p.startNasalFormant > 0)
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
	if (Melder_hasError ()) forget (him);
	return him;	
}

Sound Sound_KlattGrid_filter_parallel_frication (Sound me, KlattGrid thee, synthesisParams params)
{
	Sound him = NULL;
	FormantGridP fgp = thy frication_formants;
	long numberOfFormants = thy klatt88 ? (fgp -> formants -> size > 6 ? 6 : fgp -> formants -> size) : fgp -> formants -> size;
	struct synthesisParams p = params == NULL ? synthesisParams_createDefault (thee) : *params;
	synthesisParams pp = &p;
	
	check_formants (numberOfFormants, &(pp -> startFormant), &(pp -> endFormant));

	if (p.startFormant > 0)
	{
		long startFormant2 = p.startFormant > 2 ? p.startFormant : 2;
		int alternatingSign = startFormant2 % 2 == 0 ? 1 : -1; // 2 starts with positive sign
		Sound him = Sound_FormantGridP_filter (me, fgp, startFormant2, p.endFormant, alternatingSign);
		if (him == NULL) return NULL;
	}
	
	if (him == NULL) him = Data_copy (me);
	if (him == NULL) return NULL;
	
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

	return him;
}

Sound Sound_KlattGrid_filter_cascade_laryngial (Sound me, KlattGrid thee, synthesisParams params)
{
	FormantGrid formants = (FormantGrid) thy formants;
	FormantGrid nasal_formants = (FormantGrid) thy nasal_formants;
	FormantGrid nasal_antiformants = thy nasal_antiformants;
	FormantGrid tracheal_formants = (FormantGrid) thy tracheal_formants;
	FormantGrid tracheal_antiformants = thy tracheal_antiformants;
	
	long iformant; int antiformants = 0;
	long numberOfFormants = thy klatt88 ? 5 : thy formants -> formants -> size;
	long numberOfTrachealFormants = thy klatt88 ? 1 : thy tracheal_formants -> formants -> size;
	long numberOfTrachealAntiFormants = thy klatt88 ? 1 : thy tracheal_antiformants -> formants -> size;
	long numberOfNasalFormants = thy klatt88 ? 1 : thy nasal_formants -> formants -> size;
	long numberOfNasalAntiFormants = thy klatt88 ? 1 : thy nasal_antiformants -> formants -> size;

	struct synthesisParams p = params == NULL ? synthesisParams_createDefault (thee) : *params;
	synthesisParams pp = &p;
	
	check_formants (numberOfFormants, &(pp -> startFormant), &(pp -> endFormant));
	check_formants (numberOfNasalFormants, &(pp -> startNasalFormant), &(pp -> endNasalFormant));
	check_formants (numberOfTrachealFormants, &(pp -> startTrachealFormant), &(pp -> endTrachealFormant));
	check_formants (numberOfNasalAntiFormants, &(pp -> startNasalAntiFormant), &(pp -> endNasalAntiFormant));
	check_formants (numberOfTrachealAntiFormants, &(pp -> startTrachealAntiFormant), &(pp -> endTrachealAntiFormant));

	Sound him = Data_copy (me);
	if (him == NULL) return NULL;
	
	if (p.startNasalFormant > 0)  // Nasal formants
	{
		antiformants = 0;
		for (iformant = p.startNasalFormant; iformant <= p.endNasalFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, nasal_formants, iformant, antiformants);
		}
	}
	
	if (p.startNasalAntiFormant > 0)  // Nasal anti formants
	{
		antiformants = 1;
		for (iformant = p.startNasalAntiFormant; iformant <= p.endNasalAntiFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, nasal_antiformants, iformant, antiformants);
		}
	}

	if (p.startTrachealFormant > 0)  // Tracheal formants
	{
		antiformants = 0;
		for (iformant = p.startTrachealFormant; iformant <= p.endTrachealFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_formants, iformant, antiformants);
		}
	}
	
	if (p.startTrachealAntiFormant > 0)  // Tracheal anti formants
	{
		antiformants = 1;
		for (iformant = p.startTrachealAntiFormant; iformant <= p.endTrachealAntiFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_antiformants, iformant, antiformants);
		}
	}
	
	if (p.startFormant > 0)  // Normal formants
	{
		antiformants = 0;
		for (iformant = p.startFormant; iformant <= p.endFormant; iformant++)
		{
			_Sound_FormantGrid_filterWithOneFormant_inline (him, formants, iformant, antiformants);
		}
	}
	
	return him;
}

Sound KlattGrid_to_Sound_frication (KlattGrid me, synthesisParams p)
{
	long i, nt = ceil ((my xmax - my xmin) * p -> samplingFrequency);
	double dt = 1.0 / p -> samplingFrequency;
	double tmid = (my xmin + my xmax) / 2;
	double t1 = tmid - 0.5 * (nt - 1) * dt;
	
	Sound thee = Sound_create (1, my xmin, my xmax, nt, dt, t1);
	if (thee == NULL) return NULL;

	for (i = 1; i <= nt; i++)
	{
		double t = thy x1 + (i - 1) * dt;
		double val = NUMrandomUniform (-1, 1);
		double a = 0;
		if (my fricationAmplitude -> points -> size > 0)
		{
			double val = RealTier_getValueAtTime (my fricationAmplitude, t);
			a = val == NUMundefined ? 0 : DB_to_A (val);
		}
		thy z[1][i] = val * a;
	}

	Sound him = Sound_KlattGrid_filter_parallel_frication (thee, me, p);
	forget (thee);
	return him;	
}

Sound KlattGrid_to_Sound (KlattGrid me, double samplingFrequency, double maximumPeriod, int filterModel,
	long startFormant, long endFormant, long startNasalFormant, long endNasalFormant,
	long startTrachealFormant, long endTrachealFormant,
	long startNasalAntiFormant, long endNasalAntiFormant, long startTrachealAntiFormant,
	long endTrachealAntiFormant, int noVoicing, int noAspiration, int noFrication)
{
	Sound thee = NULL, voicing = NULL, aspiration = NULL, frication = NULL;
	struct synthesisParams p; synthesisParams pp = &p;
	
	p.samplingFrequency = samplingFrequency;
	p.maximumPeriod = maximumPeriod; p.filterModel = filterModel;
	p.startFormant = startFormant; p.endFormant = endFormant;
	p.startNasalFormant = startNasalFormant; p.endNasalFormant = endNasalFormant;
	p.startTrachealFormant = startTrachealFormant; p.endTrachealFormant = endTrachealFormant;
	p.startNasalAntiFormant = startNasalAntiFormant; p.endNasalAntiFormant = endNasalAntiFormant;
	p.startTrachealAntiFormant = startTrachealAntiFormant; p.endTrachealAntiFormant = endTrachealAntiFormant;
	p.noVoicing = noVoicing; p.noAspiration = noAspiration; p.noFrication = noFrication;
	
	check_formants (my formants -> formants -> size, &(pp -> startFormant), &(pp -> endFormant));
	check_formants (my nasal_formants -> formants -> size, &(pp -> startNasalFormant), &(pp -> endNasalFormant));
	check_formants (my tracheal_formants -> formants -> size, &(pp -> startTrachealFormant), &(pp -> endTrachealFormant));
	check_formants (my nasal_antiformants -> formants -> size, &(pp -> startNasalAntiFormant), &(pp -> endNasalAntiFormant));
	check_formants (my tracheal_antiformants -> formants -> size, &(pp -> startTrachealAntiFormant), &(pp -> endTrachealAntiFormant));
	
	RealTier df1_micro = RealTier_create (my xmin, my xmax);
	RealTier db1_micro = RealTier_create (my xmin, my xmax);

	// source section
	if (! p.noAspiration)
	{
		aspiration = PhonationGrid_to_Sound_aspiration (my source, samplingFrequency);
		if (aspiration == NULL) goto end;
	}
	if (! p.noVoicing)
	{
		voicing = PhonationGrid_to_Sound_voiced (my source, samplingFrequency, maximumPeriod, my open_glottis_delta_f1, my open_glottis_delta_b1, &df1_micro, &db1_micro, my klatt88);
		if (voicing == NULL) goto end;
		
		if (aspiration != NULL) _Sounds_add_inline (voicing, aspiration);
	}

	if (! (p.noAspiration && p.noVoicing)) // No need for filtering if no source signal present
	{
		Sound source = voicing == NULL ? aspiration : voicing;
		thee = filterModel == 1 ? Sound_KlattGrid_filter_parallel_laryngial (source, me, pp) : Sound_KlattGrid_filter_cascade_laryngial (source, me, pp);
	
		if (thee == NULL) goto end;
	}

	if (! p.noFrication)
	{
		frication = KlattGrid_to_Sound_frication (me, pp);
		if (frication == NULL) goto end;
	
		_Sounds_add_inline (thee, frication);
	}

	if (thee == NULL)
	{
		if (aspiration != NULL)
		{
			thee = aspiration;
			aspiration = NULL;
		}
		if (thee == NULL)
		{
			(void) Melder_error1 (L"No sound sources (phonation, aspiration or frication). At least one of these must be present.");
			goto end;
		}
	}
	
end:
	forget (df1_micro); forget (db1_micro);
	forget (aspiration); forget (voicing); forget (frication);
	if (Melder_hasError ()) forget (thee);
	return thee;	
}

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, double maximumPeriod, int parallel)
{
	return KlattGrid_to_Sound (me, samplingFrequency, maximumPeriod, parallel,
	1, my formants -> formants -> size, 1, my nasal_formants -> formants -> size, 1, my tracheal_formants -> formants -> size,
	1, my nasal_antiformants -> formants -> size, 1, my tracheal_antiformants -> formants -> size, 0, 0, 0);
}

KlattGrid KlattGrid_test (double tmin, double tmax, double f0b, double f0e, double flutterb, double fluttere, double openphb, double openphe, double p1b, double p1e, double p2b, double p2e, double colphb, double colphe, double dpulsb, double dpulse, double avdbb, double avdbe, double aspNoisedBb, double aspNoisedBe, double breathyb, double breathye, double tiltb, double tilte,
double f1b, double f1e, double b1b, double b1e, double df1b, double df1e, double f2b, double f2e, double b2b, double b2e)
{	
	KlattGrid me = KlattGrid_create (0, 1);
	if (me == NULL) return NULL;
	
	PitchTier f0 = my source -> f0;
	RealTier openPhase = my source -> openPhase;
	RealTier power1 = my source -> power1;
	RealTier power2 = my source -> power2;
	RealTier collisionPhase = my source -> collisionPhase;
	RealTier doublePulsing = my source -> doublePulsing;
	RealTier flutter = my source -> flutter;
	IntensityTier breathyness = my source -> breathyness;
	IntensityTier amplitudeOfVoicing = my source -> amplitudeOfVoicing;
	IntensityTier aspirationAmplitude = my source -> aspirationAmplitude;
	IntensityTier tilt = my source -> extraSpectralTilt;
	RealTier f1 = my formants -> formants -> item[1];
	RealTier b1 = my formants -> bandwidths -> item[1];
	RealTier f2 = my formants -> formants -> item[2];
	RealTier b2 = my formants -> bandwidths -> item[2];
	
	if (! RealTier_addPoint (f0, tmin, f0b) || ! RealTier_addPoint (f0, tmax, f0e) ||
	    ! RealTier_addPoint (openPhase, tmin, openphb) || ! RealTier_addPoint (openPhase, tmax, openphe) ||
	    ! RealTier_addPoint (power1, tmin, p1b) || ! RealTier_addPoint (power1, tmax, p1e) ||
	    ! RealTier_addPoint (power2, tmin, p2b) || ! RealTier_addPoint (power2, tmax, p2e) ||
	    ! RealTier_addPoint (collisionPhase, tmin, colphb) || ! RealTier_addPoint (collisionPhase, tmax, colphe) ||
	    ! RealTier_addPoint (doublePulsing, tmin, dpulsb) || ! RealTier_addPoint (doublePulsing, tmax, dpulse) ||
	    ! RealTier_addPoint (flutter, tmin, flutterb) || ! RealTier_addPoint (flutter, tmax, fluttere) ||
	    ! RealTier_addPoint (amplitudeOfVoicing, tmin, avdbb) || ! RealTier_addPoint (amplitudeOfVoicing, tmax, avdbe) ||
	    ! RealTier_addPoint (aspirationAmplitude, tmin, aspNoisedBb) || ! RealTier_addPoint (aspirationAmplitude, tmax, aspNoisedBe) ||
	    ! RealTier_addPoint (breathyness, tmin, breathyb) || ! RealTier_addPoint (breathyness, tmax, breathye) ||
	    ! RealTier_addPoint (tilt, tmin, tiltb) || ! RealTier_addPoint (tilt, tmax, tilte) ||
		! RealTier_addPoint (f1, tmin, f1b) || ! RealTier_addPoint (f1, tmax, f1e) ||
		! RealTier_addPoint (b1, tmin, b1b) || ! RealTier_addPoint (b1, tmax, b1e) ||
		! RealTier_addPoint (my open_glottis_delta_f1, tmin, df1b) || ! RealTier_addPoint (my open_glottis_delta_f1, tmax, df1e) ||
		! RealTier_addPoint (f2, tmin, f2b) || ! RealTier_addPoint (f2, tmax, f2e) ||
		! RealTier_addPoint (b2, tmin, b2b) || ! RealTier_addPoint (b2, tmax, b2e)
		) forget (me);
	return me;
}

KlattGrid KlattGrid_createExample (void)
{
	KlattGrid me = NULL;
	Table kt = (Table) KlattTable_createExample ();
	if (kt == NULL) return NULL;

	long nrows = kt -> rows -> size;
	double frameDuration = 0.005, dBNul = -300;
	// KlattTable maximum in DB_to_LIN is at 87 dB : 32767 and afterwards
	// reference 2.10^-5 
	double dB_offset = 20 * log10 (32.767) - (100 - 20 * log10 (2)) + 87; // in KlattTable maximum in DB_to_LIN is at 87 dB : 32767
	double tmin = 0, tmax = nrows * frameDuration;
	double ap[7] = {0, 0.4, 0.15, 0.06, 0.04, 0.022, 0.03 };

	me = KlattGrid_create (tmin, tmax);
	if (me == NULL) goto end;
	my klatt88 = 1;
	for (long irow = 1; irow <= nrows; irow++)
	{
		double t = (irow - 1) * frameDuration;
		double val, f0, fk;
		long icol = 1;
		
		f0 = val = Table_getNumericValue (kt, irow, icol) / 10; // F0hz10
		RealTier_addPoint (my source -> f0, t, f0);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AVdb
		// dB values below 13 were put to zero in the DBtoLIN function
		val -= 7;
		if (val < 13) val = dBNul;
//		RealTier_addPoint (my source -> amplitudeOfVoicing, t, val);
		for (long kf = 1; kf <= 6; kf++)
		{
			icol++; fk = val = Table_getNumericValue (kt, irow, icol); // Fhz
			RealTier_addPoint (my formants -> formants -> item[kf], t, val);
			RealTier_addPoint (my frication_formants -> formants -> item[kf], t, val); // only amplitudes and bandwidths in frication section
			icol++; val = Table_getNumericValue (kt, irow, icol); // Bhz
			if (val <= 0) val = fk / 10;
			RealTier_addPoint (my formants -> bandwidths -> item[kf], t, val);
		}
		icol++; val = Table_getNumericValue (kt, irow, icol); // FNZhz
		RealTier_addPoint (my nasal_antiformants -> formants -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // BNZhz
		RealTier_addPoint (my nasal_antiformants -> bandwidths -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // FNPhz
		RealTier_addPoint (my nasal_formants -> formants -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // BNPhz
		RealTier_addPoint (my nasal_formants -> bandwidths -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // ah
		if (val < 13) val = dBNul; else val += 20 * log (0.05) + dB_offset;
		RealTier_addPoint (my source -> aspirationAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Kopen
		double openPhase = f0 > 0 ? (val / 16000) * f0 : 0.7;
		RealTier_addPoint (my source -> openPhase, t, openPhase);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Aturb breathyness during voicing
		if (val < 13) val = dBNul; else val += 20 * log (0.1) + dB_offset;
		RealTier_addPoint (my source -> breathyness, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // TLTdb
		RealTier_addPoint (my source -> extraSpectralTilt, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AF
		if (val < 13) val = dBNul; else val += 20 * log (0.25) + dB_offset;
		RealTier_addPoint (my fricationAmplitude, t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Kskew ???
		//RealTier_addPoint (, t, val);
		for (long kf = 1; kf <= 6; kf++)
		{
			icol++; val = Table_getNumericValue (kt, irow, icol); // Ap
			if (val < 13) val = dBNul; else val += 20 * log (ap[kf]) + dB_offset;
			RealTier_addPoint (my formants -> amplitudes -> item[kf], t, val);
			icol++; val = Table_getNumericValue (kt, irow, icol); // Bhz
			RealTier_addPoint (my frication_formants -> bandwidths -> item[kf], t, val);
		}
		
		icol++; val = Table_getNumericValue (kt, irow, icol); // ANP
		if (val < 13) val = dBNul; else val += 20 * log (0.6) + dB_offset;
		RealTier_addPoint (my nasal_formants -> amplitudes -> item[1], t, val);
		icol++; val = Table_getNumericValue (kt, irow, icol); // AB
		if (val < 13) val = dBNul; else val += 20 * log (0.05) + dB_offset;
		RealTier_addPoint (my bypassAmplitude, t, val);
		
		icol++; val = Table_getNumericValue (kt, irow, icol); // AVpdb
		RealTier_addPoint (my source -> amplitudeOfVoicing, t, val + dB_offset);
		icol++; val = Table_getNumericValue (kt, irow, icol); // Gain0
		val -= 3; if (val <= 0) val = 57;
		RealTier_addPoint (my gain, t, val + dB_offset);
	}
	// We don't need the following low-pass: we do not use oversampling !!
	//RealTier_addPoint (my tracheal_formants -> formants -> item[1], 0.5*(tmin+tmax), 0.095*samplingFrequency);
	//RealTier_addPoint (my tracheal_formants -> bandwidths -> item[1], 0.5*(tmin+tmax), 0.063*samplingFrequency);
	
end:
	forget (kt);
	if (Melder_hasError ()) forget (me);
	return me;
}

/* End of file KlattGrid.c */
