/* TextTier_Pitch.h
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
 * pb 1999/10/23
 * pb 2002/07/16 GPL
 */

#ifndef _TextGrid_h_
	#include "TextGrid.h"
#endif
#ifndef _Pitch_h_
	#include "Pitch.h"
#endif

void TextTier_Pitch_draw (TextTier label, Pitch pitch, Graphics g,
	double tmin, double tmax, double fmin, double fmax,
	double fontSize, int useTextStyles, int garnish, int speckle, int yscale);

/* End of file TextTier_Pitch.h */
