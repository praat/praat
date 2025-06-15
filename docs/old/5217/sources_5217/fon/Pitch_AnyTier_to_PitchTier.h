/* Pitch_AnyTier_to_PitchTier.h
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
 * pb 1997/04/07
 * pb 2002/07/16 GPL
 */

#ifndef _Pitch_h_
	#include "Pitch.h"
#endif
#ifndef _AnyTier_h_
	#include "AnyTier.h"
#endif
#ifndef _PitchTier_h_
	#include "PitchTier.h"
#endif

PitchTier Pitch_AnyTier_to_PitchTier (Pitch pitch, AnyTier tier, int checkMethod);
PitchTier PitchTier_AnyTier_to_PitchTier (PitchTier pitch, AnyTier tier);

/* End of file Pitch_AnyTier_to_PitchTier.h */
