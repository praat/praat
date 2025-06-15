/* Pitch_Intensity.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2005/03/08
 */

#ifndef _Pitch_h_
	#include "Pitch.h"
#endif
#ifndef _Intensity_h_
	#include "Intensity.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

void Pitch_Intensity_draw (Pitch pitch, Intensity intensity, Graphics g,
	double f1, double f2, double s1, double s2, int garnish, int connect);

/* End of file Pitch_Intensity.h */
