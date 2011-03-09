/* LPC_and_Tube.c
 *
 * Copyright (C) 1993-2004 David Weenink
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

int LPC_Frame_into_Tube_Frame_rc (LPC_Frame me, Tube_Frame thee)
{
	double *b = NULL, *a = NULL, *rc = thy c;
	long m, i, p = my nCoefficients;
	int status = 0;
	
	if (p > thy nSegments) return 0;
	
	b = NUMdvector (1, p + p);
	if (b == NULL) return 0;
	
	a = & b[p];
	
	for (i = 1; i <= p; i++) a[i] = my a[i];
	
	for (m = p; m > 0; m--)
	{
		rc[m] = a[m];
		if (fabs (rc[m]) > 1) goto end;
		for (i = 1; i < m; i++) b[i] = a[i];
		for (i = 1; i < m; i++)
		{
			a[i] = (b[i] - rc[m] * b[m-i]) / (1.0 - rc[m] * rc[m]);
		}
	}
	
	status = 1;
	
end:

	NUMdvector_free (b, 1);
	return status;	
}

int LPC_Frame_into_Tube_Frame_area (LPC_Frame me, Tube_Frame thee)
{
	struct structTube_Frame rc_struct = { 0 };
	Tube_Frame rc = & rc_struct;
	int status;
	memset (&  rc_struct, 0, sizeof(rc_struct));
	status = Tube_Frame_init (rc, my nCoefficients, thy length) &&
		LPC_Frame_into_Tube_Frame_rc (me, rc) && 
		Tube_Frames_rc_into_area (rc, thee);
		
	Tube_Frame_destroy (rc);
	
	return status;
}

double LPC_Frame_getVTL_wakita (LPC_Frame me, double samplingPeriod, double refLength)
{
	struct structLPC_Frame lpc_struct;
	LPC_Frame lpc = & lpc_struct;
	struct structFormant_Frame f_struct;
	Formant_Frame f = & f_struct;
	struct structTube_Frame rc_struct, af_struct;
	Tube_Frame rc = & rc_struct, af = & af_struct;
	long i, m = my nCoefficients;
	double lmin, length, dlength = 0.001, plength, wakita_length = NUMundefined; 
	double *area, var, varMin = 1e38, logSum;
	double fscale = 1;
	
	memset(& lpc_struct, 0, sizeof(lpc_struct));
	memset(& f_struct, 0, sizeof(f_struct));
	memset(& rc_struct, 0, sizeof(rc_struct));
	memset(& af_struct, 0, sizeof(af_struct));
	
	
	if (! LPC_Frame_init  (lpc, m) || 
		! Tube_Frame_init (rc, m, refLength) ||
		! Tube_Frame_init (af, m, refLength) ||
	
		/*
			Step 2
		*/
		
		! LPC_Frame_into_Formant_Frame (me, f, samplingPeriod, 0) ||
		
		/*
			LPC_Frame_into_Formant_Frame performs the Formant_Frame_init !! 
		*/
		
		f -> nFormants < 1) goto end;
	
	area = af -> c;
	lmin = length = 0.10;
	plength = refLength;	
	while (length <= 0.25)
	{
		/* Step 3 */
		
		fscale = plength / length;
		for (i = 1; i <= f -> nFormants; i++)
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
		
		/* step 4 */
		
		if (! Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod)) goto end;
		
		/* step 5 */
		
		rc -> length = length;
		if (! LPC_Frame_into_Tube_Frame_rc (lpc, rc)) goto end;
		
		/* step 6.1 */
			
		if (! Tube_Frames_rc_into_area (rc, af)) goto end;
		
		/* step 6.2 Log(areas) */
		
		for (logSum = 0, i = 1; i <= af -> nSegments; i++)
		{
			area[i] = log (area[i]);
			logSum += area[i];
		}
		
		/* step 6.3 and 7*/
		
		for (var = 0, i = 1; i <= af -> nSegments; i++)
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
	
end:

	Formant_Frame_destroy (f); 
	LPC_Frame_destroy (lpc);
	Tube_Frame_destroy (rc);
	Tube_Frame_destroy (af);
	
	return wakita_length;
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
	VocalTract thee = NULL;
	struct structTube_Frame area_struct;
	Tube_Frame area = & area_struct;
	LPC_Frame lpc; 
	long i, m, iframe = Sampled_xToIndex (me, time);
	
	if (iframe < 1) iframe = 1;
	if (iframe > my nx) iframe = my nx;
	
	memset (& area_struct, 0, sizeof(area_struct));
	
	lpc = & my frame[iframe];
	m = lpc -> nCoefficients;
	
	if (! Tube_Frame_init (area, m, length) ||
		! LPC_Frame_into_Tube_Frame_area (lpc, area)) goto end;
		
	thee = VocalTract_create (m, area -> length / m);
	if (thee == NULL) goto end;
	
	/* 
		area[lips..glottis] (m^2) to VocalTract[glottis..lips] (m^2)
	*/
	
	for (i = 1; i <= m; i++)
	{
		thy z[1][i] = area -> c[m + 1 - i];
	}
	if (wakita)
	{
		double wakita_length =  LPC_Frame_getVTL_wakita (lpc, my samplingPeriod, length);
		if (wakita_length == NUMundefined)
		{
			Melder_warning1 (L"Vocal tract length could not be calculated.\nRelevant tract dimensions will be undefined.");
			thy xmax = thy x1 = thy dx = NUMundefined;
		}
	}
end:

	Tube_Frame_destroy (area);
	return thee;
}

/* End of file LPC_and_Tube.c */
