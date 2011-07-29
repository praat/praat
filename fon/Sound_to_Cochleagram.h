/* Sound_to_Cochleagram.h
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

#include "Sound.h"
#include "Cochleagram.h"

Cochleagram Sound_to_Cochleagram (Sound me, double dt, double df, double windowLength, double forwardMaskingTime);

Cochleagram Sound_to_Cochleagram_edb
	(Sound me, double dtime, double dfreq, int hasSynapse, double replenishmentRate,
	 double lossRate, double returnRate, double reprocessingRate);

/* End of file Sound_to_Cochleagram.h */
