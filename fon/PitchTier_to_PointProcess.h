/* PitchTier_to_PointProcess.h
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

#include "PitchTier.h"
#include "PointProcess.h"
#include "Pitch.h"

PointProcess PitchTier_to_PointProcess (PitchTier me);

PointProcess PitchTier_Pitch_to_PointProcess (PitchTier me, Pitch vuv);
/* Keeps only the parts that are voiced according to 'vuv'. */
/* Voiced means: inside voiced frame of 'pitch'. */

PointProcess PitchTier_Point_to_PointProcess (PitchTier me, PointProcess vuv, double maxT);
/* Keeps only the parts that are voiced according to 'vuv'. */
/* Voiced means: within an interval no longer than 'maxT', */
/* or within half an adjacent short-enough interval from any pulse. */

PitchTier PointProcess_to_PitchTier (PointProcess me, double maximumInterval);
PitchTier Pitch_PointProcess_to_PitchTier (Pitch me, PointProcess pp);
PitchTier PitchTier_PointProcess_to_PitchTier (PitchTier me, PointProcess pp);
TableOfReal PitchTier_downto_TableOfReal (PitchTier me, int useSemitones);

/* End of file PitchTier_to_PointProcess.h */
