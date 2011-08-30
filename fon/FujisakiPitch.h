#ifndef _FujisakiPitch_h_
#define _FujisakiPitch_h_
/* FujisakiPitch.h
 *
 * Copyright (C) 2002-2011 Paul Boersma
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

#include "Pitch.h"
#include "Collection.h"

#include "FujisakiPitch_def.h"
oo_CLASS_CREATE (FujisakiCommand, Function);
oo_CLASS_CREATE (FujisakiPitch, Function);

FujisakiCommand FujisakiCommand_create (double tmin, double tmax, double amplitude);

FujisakiPitch FujisakiPitch_create (double tmin, double tmax,
	double baseFrequency, double alpha, double beta, double gamma);

FujisakiPitch Pitch_to_FujisakiPitch (Pitch me, double gamma, double timeResolution,
	FujisakiPitch *intermediate1, FujisakiPitch *intermediate2, FujisakiPitch *intermediate3);

/* End of file FujisakiPitch.h */
#endif
