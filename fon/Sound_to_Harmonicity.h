/* Sound_to_Harmonicity.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Sound.h"
#include "Harmonicity.h"

Harmonicity Sound_to_Harmonicity_ac (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow);

Harmonicity Sound_to_Harmonicity_cc (Sound me, double dt, double minimumPitch,
	double silenceThreshold, double periodsPerWindow);

Matrix Sound_to_Harmonicity_GNE (Sound me,
	double fmin,   /* 500 Hz */
	double fmax,   /* 4500 Hz */
	double bandwidth,  /* 1000 Hz */
	double step);   /* 80 Hz */

/* End of file Sound_to_Harmonicity.h */
