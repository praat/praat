/* Sound_to_Harmonicity.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 2004/10/29 corrected 
 */

#include <math.h>
#include "Sound_to_Pitch.h"
#include "Sound_to_Harmonicity.h"

Harmonicity Sound_to_Harmonicity_ac (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow)
{
	Harmonicity thee;
	long i;
	Pitch pitch = Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, 15, 1,
		silenceThreshold, 0, 0, 0, 0, 0.5 / my dx);
	if (! pitch) return NULL;
	thee = Harmonicity_create (my xmin, my xmax, pitch -> nx,
		pitch -> dx, pitch -> x1);
	if (thee)
		for (i = 1; i <= thy nx; i ++)
			if (pitch -> frame [i]. candidate [1]. frequency == 0)
				thy z [1] [i] = -200;
			else {
				double r = pitch -> frame [i]. candidate [1]. strength;
				thy z [1] [i] = r <= 1e-15 ? -150 : r > 1 - 1e-15 ? 150 : 10 * log10 (r / (1 - r));
			}
	forget (pitch);
	return thee;
}

Harmonicity Sound_to_Harmonicity_cc (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow)
{
	Harmonicity thee;
	long i;
	Pitch pitch = Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, 15, 3,
		silenceThreshold, 0, 0, 0, 0, 0.5 / my dx);
	if (! pitch) return NULL;
	thee = Harmonicity_create (my xmin, my xmax, pitch -> nx,
		pitch -> dx, pitch -> x1);
	if (thee)
		for (i = 1; i <= thy nx; i ++)
			if (pitch -> frame [i]. candidate [1]. frequency == 0)
				thy z [1] [i] = -200;
			else {
				double r = pitch -> frame [i]. candidate [1]. strength;
				thy z [1] [i] = r <= 1e-15 ? -150 : r > 1 - 1e-15 ? 150 : 10 * log10 (r / (1 - r));
			}
	forget (pitch);
	return thee;
}

/* End of file Sound_to_Harmonicity.c */
