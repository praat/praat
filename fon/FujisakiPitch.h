#ifndef _FujisakiPitch_h_
#define _FujisakiPitch_h_
/* FujisakiPitch.h
 *
 * Copyright (C) 2002-2011,2015 Paul Boersma
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
#include "Collection.h"

#include "FujisakiPitch_def.h"

autoFujisakiCommand FujisakiCommand_create (double tmin, double tmax, double amplitude);

autoFujisakiPitch FujisakiPitch_create (double tmin, double tmax,
	double baseFrequency, double alpha, double beta, double gamma);

autoFujisakiPitch Pitch_to_FujisakiPitch (Pitch me, double gamma, double timeResolution,
	autoFujisakiPitch *intermediate1, autoFujisakiPitch *intermediate2, autoFujisakiPitch *intermediate3);

/* End of file FujisakiPitch.h */
#endif
