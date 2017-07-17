/* PitchTier_to_PointProcess.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "PitchTier.h"
#include "PointProcess.h"
#include "Pitch.h"

autoPointProcess PitchTier_to_PointProcess (PitchTier me);

autoPointProcess PitchTier_Pitch_to_PointProcess (PitchTier me, Pitch vuv);
/* Keeps only the parts that are voiced according to 'vuv'. */
/* Voiced means: inside voiced frame of 'pitch'. */

autoPointProcess PitchTier_Point_to_PointProcess (PitchTier me, PointProcess vuv, double maxT);
/* Keeps only the parts that are voiced according to 'vuv'. */
/* Voiced means: within an interval no longer than 'maxT', */
/* or within half an adjacent short-enough interval from any pulse. */

autoPitchTier PointProcess_to_PitchTier (PointProcess me, double maximumInterval);
autoPitchTier Pitch_PointProcess_to_PitchTier (Pitch me, PointProcess pp);
autoPitchTier PitchTier_PointProcess_to_PitchTier (PitchTier me, PointProcess pp);
autoTableOfReal PitchTier_downto_TableOfReal (PitchTier me, int useSemitones);

/* End of file PitchTier_to_PointProcess.h */
