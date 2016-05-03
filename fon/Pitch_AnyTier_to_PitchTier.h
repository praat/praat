/* Pitch_AnyTier_to_PitchTier.h
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

#include "Pitch.h"
#include "AnyTier.h"
#include "PitchTier.h"

autoPitchTier Pitch_AnyTier_to_PitchTier (Pitch pitch, AnyTier tier, int checkMethod);
autoPitchTier PitchTier_AnyTier_to_PitchTier (PitchTier pitch, AnyTier tier);

/* End of file Pitch_AnyTier_to_PitchTier.h */
