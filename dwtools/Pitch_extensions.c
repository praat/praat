/* Pitch_extensions.c
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 1997
 djmw 20030217 Latest modification
*/

#include "Pitch_extensions.h"

void Pitch_Frame_addPitch (Pitch_Frame me, double f, double strength, int maxnCandidates)
{
	int i, pos = 0;	double weakest = 1e38;
	if (my nCandidates < maxnCandidates) pos = ++ my nCandidates;
	else
	{
		/* Find weakest candidate so far (skip the unvoiced one) */
		for (i=1; i <= maxnCandidates; i++) if (my candidate[i].strength < weakest &&
			my candidate[i].frequency > 0) { weakest = my candidate[i].strength; pos = i; }
		if (strength < weakest) pos = 0;
	}
	if (pos)
	{
		my candidate[pos].frequency = f;
		my candidate[pos].strength = strength;
	}
}

void Pitch_Frame_getPitch (Pitch_Frame me, double *f, double *strength)
{
	int i, pos = 1;
	*strength = -1;
	for (i=1; i <= my nCandidates; i++) if (my candidate[i].strength > *strength && 
		my candidate[i].frequency > 0) { *strength = my candidate[i].strength; pos = i; }
	*f = my candidate[pos].frequency;
}

void Pitch_Frame_resizeStrengths (Pitch_Frame me, double maxStrength,
	double unvoicedCriterium)
{
	int i, pos = 1; double strongest = my candidate[1].strength;
	for (i=2; i <= my nCandidates; i++) if (my candidate[i].strength > strongest)
		{ strongest = my candidate[i].strength; pos = i; }
 	if (strongest != 0) for (i=1; i <= my nCandidates; i++)
 		my candidate[i].strength *= maxStrength / strongest;		
	if (maxStrength < unvoicedCriterium) for (i=1; i <= my nCandidates; i++)
		if (my candidate[i].frequency == 0) { pos = i; break; }
	if (pos != 1)
	{
		double tmp = my candidate[1].frequency;
		my candidate[1].frequency = my candidate[pos].frequency;
		my candidate[pos].frequency = tmp;
		tmp = my candidate[1].strength;
		my candidate[1].strength = my candidate[pos].strength;
		my candidate[pos].strength = tmp;
	}
}

Pitch Pitch_scaleTime (Pitch me, double scaleFactor)
{
	Pitch thee = NULL;
	long i;
	double dx = my dx, x1 = my x1, xmax = my xmax;

	if (scaleFactor != 1)
	{
		dx = my dx * scaleFactor;
		x1 = my xmin + 0.5 * dx;
		xmax = my xmin + my nx * dx;
	}	
	thee = Pitch_create (my xmin, xmax, my nx, dx, x1, my ceiling, 2);
	if ( thee == NULL) return NULL;
	for (i = 1; i <= my nx; i++)
	{
		double f = my frame[i].candidate[1].frequency;
		thy frame[i].candidate[1].strength = my frame[i].candidate[1].strength;
		f /= scaleFactor;
		if (f < my ceiling) thy frame[i].candidate[1].frequency = f; 
	}
	return thee;
}

void PitchTier_modifyRange (PitchTier me, double tmin, double tmax, double fmin,
	double factor, double fmid)
{
	long i;

	for (i = 1; i <= my points -> size; i ++)
	{
		RealPoint point = my points -> item [i];
		double f = point -> value;
		if (point -> time < tmin || point -> time > tmax) continue;
		f = fmid + (f - fmid) * factor;
		point -> value = f < fmin ? fmin : f;
	}
}

/* End of file Pitch_extensions.c */
