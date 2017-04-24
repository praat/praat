#ifndef Sound_and_MixingMatrix_h_
#define Sound_and_MixingMatrix_h_

/* Sound_and_MixingMatrix.h
 *
 * Copyright (C) 2010-2017 David Weenink
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

#include "MixingMatrix.h"
#include "Sound.h"

autoSound Sound_and_MixingMatrix_mixPart (Sound me, MixingMatrix thee, double t1, double t2);

autoSound Sound_and_MixingMatrix_mix (Sound me, MixingMatrix thee);

autoSound Sound_and_MixingMatrix_unmix (Sound me, MixingMatrix thee); 

void Sound_and_MixingMatrix_playPart (Sound me, MixingMatrix thee, double fromTime, double toTime, Sound_PlayCallback callback, Thing boss);

void Sound_and_MixingMatrix_play (Sound me, MixingMatrix thee, Sound_PlayCallback callback, Thing boss);

#endif
/* End of file Sound_and_MixingMatrix.h */
