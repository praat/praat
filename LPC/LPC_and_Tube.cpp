/* LPC_and_Tube.cpp
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20020612 GPL header
 djmw 20041020 struct Tube_Frame -> struct structTube_Frame; struct LPC_Frame -> struct structLPC_Frame;
 	struct Formant_Frame->struct structFormant_Frame
 djmw 20051005 Always make a VocalTract with length 0.01 m when wakita_length==NUMundefined.
*/

#include "LPC_and_Tube.h"
#include "LPC_and_Formant.h"

void LPC_Frame_into_Tube_Frame_rc (LPC_Frame me, Tube_Frame thee)
{
	try {
		long p = my nCoefficients;	
		Melder_assert (p <= thy nSegments); //TODO 
	
		autoNUMvector<double> b (1, p);
		autoNUMvector<double> a (1, p);
	
		for (long i = 1; i <= p; i++) a[i] = my a[i];
	
		double *rc = thy c;
		for (long m = p; m > 0; m--)
		{
			rc[m] = a[m];
			if (fabs (rc[m]) > 1) Melder_throw ("rc larger 1."); // TODO kan er geen Tube worden gemaakt?
			for (long i = 1; i < m; i++) b[i] = a[i];
			for (long i = 1; i < m; i++)
			{
				a[i] = (b[i] - rc[m] * b[m-i]) / (1.0 - rc[m] * rc[m]);
			}
		}
	} catch (MelderError) { rethrow; }
}

void LPC_Frame_into_Tube_Frame_area (LPC_Frame me, Tube_Frame thee)
{
	struct structTube_Frame rc_struct = { 0 };
	Tube_Frame rc = & rc_struct;
	try {
		Tube_Frame_init (rc, my nCoefficients, thy length); therror
		LPC_Frame_into_Tube_Frame_rc (me, rc); therror
		Tube_Frames_rc_into_area (rc, thee); therror
		Tube_Frame_destroy (rc);
	} catch (MelderError) { Tube_Frame_destroy (rc); rethrow; }
}

double LPC_Frame_getVTL_wakita (LPC_Frame me, double samplingPeriod, double refLength)
{
	struct structLPC_Frame lpc_struct;
	LPC_Frame lpc = & lpc_struct;
	struct structFormant_Frame f_struct;
	Formant_Frame f = & f_struct;
	struct structTube_Frame rc_struct, af_struct;
	Tube_Frame rc = & rc_struct, af = & af_struct;
	try {
		long m = my nCoefficients;
		double length, dlength = 0.001, wakita_length = NUMundefined; 
		double var, varMin = 1e38, logSum;
		double fscale = 1;
	
		memset(& lpc_struct, 0, sizeof(lpc_struct));
		memset(& f_struct, 0, sizeof(f_struct));
		memset(& rc_struct, 0, sizeof(rc_struct));
		memset(& af_struct, 0, sizeof(af_struct));
	
	
		LPC_Frame_init  (lpc, m); 
		Tube_Frame_init (rc, m, refLength);
		Tube_Frame_init (af, m, refLength);
	
		// Step 2
		
		LPC_Frame_into_Formant_Frame (me, f, samplingPeriod, 0);
		
		// LPC_Frame_into_Formant_Frame performs the Formant_Frame_init !! 
		
		if (f -> nFormants < 1) rethrowzero;
	
		double *area = af -> c;
		double lmin = length = 0.10;
		double plength = refLength;	
		while (length <= 0.25)
		{
			// Step 3 
		
			double fscale = plength / length;
			for (long i = 1; i <= f -> nFormants; i++)
			{
				f -> formant[i].frequency *= fscale;
				f -> formant[i].bandwidth *= fscale; 
			}
		
			/*
			20000125: Bovenstaande schaling van f1/b1 kan ook gedaan worden door
			MGfb_to_a (f, b, nf, samplingFrequency*length/refLength, a1)
			De berekening is extreem gevoelig voor de samplefrequentie: een zelfde
			stel f,b waardes geven andere lengtes afhankelijk van Fs. Ook het
			weglaten van een hogere formant heeft consekwenties.
			De refLength zou eigenlijk vast moeten liggen op
			refLength=c*aantalFormanten/Fs waarbij c=340 m/s (geluidssnelheid).
			Bij Fs=10000 zou aantalFormanten=5 zijn en refLength -> 0.17 m
			*/
		
			// step 4 
		
			Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod); therror
		
			// step 5 
		
			rc -> length = length;
			LPC_Frame_into_Tube_Frame_rc (lpc, rc); therror
		
			// step 6.1
			
			Tube_Frames_rc_into_area (rc, af); therror
		
			// step 6.2 Log(areas) 
		
			double logSum = 0;
			for (long i = 1; i <= af -> nSegments; i++)
			{
				area[i] = log (area[i]);
				logSum += area[i];
			}
		
			// step 6.3 and 7
			double var = 0;
			for (long i = 1; i <= af -> nSegments; i++)
			{
				double delta = area[i] - logSum / af -> nSegments;
				var += delta * delta;
			}
		
			if (var < varMin)
			{
				lmin = length; varMin = var;
			}
			plength = length;
			length += dlength;
		}
	
		wakita_length = lmin;
		Formant_Frame_destroy (f); 
		LPC_Frame_destroy (lpc);
		Tube_Frame_destroy (rc);
		Tube_Frame_destroy (af);
		return wakita_length;
	} catch (MelderError) {
		Formant_Frame_destroy (f); 
		LPC_Frame_destroy (lpc);
		Tube_Frame_destroy (rc);
		Tube_Frame_destroy (af);
		return NUMundefined;
	}
}

int Tube_Frame_into_LPC_Frame_area (Tube_Frame me, LPC_Frame thee)
{
	(void) me;
	(void) thee;
	return 0;
}

int Tube_Frame_into_LPC_Frame_rc (Tube_Frame me, LPC_Frame thee)
{
	(void) me;
	(void) thee;
	return 0;
}


VocalTract LPC_to_VocalTract (LPC me, double time, double length, int wakita)
{
	autoVocalTract thee = 0;
	struct structTube_Frame area_struct;
	Tube_Frame area = & area_struct;
	try {
		long iframe = Sampled_xToIndex (me, time);
	
		if (iframe < 1) iframe = 1;
		if (iframe > my nx) iframe = my nx;
	
		memset (& area_struct, 0, sizeof(area_struct));
	
		LPC_Frame lpc = & my frame[iframe];
		long m = lpc -> nCoefficients;
	
		Tube_Frame_init (area, m, length); therror
		LPC_Frame_into_Tube_Frame_area (lpc, area); therror
		
		thee.reset(VocalTract_create (m, area -> length / m));

		// area[lips..glottis] (m^2) to VocalTract[glottis..lips] (m^2)
	
		for (long i = 1; i <= m; i++)
		{
			thy z[1][i] = area -> c[m + 1 - i];
		}
		if (wakita) double wakita_length =  LPC_Frame_getVTL_wakita (lpc, my samplingPeriod, length);
		Tube_Frame_destroy (area);
		return thee.transfer();
	} catch (MelderError) { 
		thy xmax = thy x1 = thy dx = NUMundefined;
		Tube_Frame_destroy (area); 
		rethrowmzero ("VocalTract not created."); }
}

/* End of file LPC_and_Tube.cpp */
