/* Sound_to_Cochleagram.h
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
 * pb 1998/01/05
 * pb 2002/07/16 GPL
 */

#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Cochleagram_h_
	#include "Cochleagram.h"
#endif

Cochleagram Sound_to_Cochleagram (I, double dt, double df, double windowLength, double forwardMaskingTime);

Cochleagram Sound_to_Cochleagram_edb
	(I, double dtime, double dfreq, int hasSynapse, double replenishmentRate,
	 double lossRate, double returnRate, double reprocessingRate);

/* End of file Sound_to_Cochleagram.h */
