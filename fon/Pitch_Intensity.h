/* Pitch_Intensity.h
 *
 * Copyright (C) 1992-2011,2016 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Pitch.h"
#include "Intensity.h"
#include "Graphics.h"

void Pitch_Intensity_draw (Pitch pitch, Intensity intensity, Graphics g,
	double f1, double f2, double s1, double s2, bool garnish, int connect);

double Pitch_Intensity_getMean (Pitch thee, Intensity me);

double Pitch_Intensity_getMeanAbsoluteSlope (Pitch pitch, Intensity intensity);

/* End of file Pitch_Intensity.h */
