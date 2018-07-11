#ifndef _TextGrid_and_DurationTier_h_
#define _TextGrid_and_DurationTier_h_
/* TextGrid_and_DurationTier.h
 *
 * Copyright (C) 2017 David Weenink
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

#include "TextGrid.h"
#include "DurationTier.h"

/* Scale the times of the TextGrid with the DurationTier */
void IntervalTier_DurationTier_scaleTimes (IntervalTier me, DurationTier thee);

void TextTier_DurationTier_scaleTimes (TextTier me, DurationTier thee);

autoTextGrid TextGrid_DurationTier_scaleTimes (TextGrid me, DurationTier thee);

autoDurationTier TextGrid_to_DurationTier (TextGrid me, integer tierNumber,
	double timeScalefactor, double leftTransitionDuration, double rightTransitionDuration, kMelder_string which, conststring32 criterion);

#endif
