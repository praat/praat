#ifndef _Sound_and_PCA_h_
#define _Sound_and_PCA_h_
/* Sound_and_PCA.h
 *
 * Copyright (C) 2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20121001
*/

#include "PCA.h"
#include "Sound.h"

PCA Sound_to_PCA_channels (Sound me, double startTime, double endTime);

Sound Sound_and_PCA_to_Sound_pc_selectedChannels (Sound me, PCA thee, long numberOfComponents, long *channels, long numberOfChannels);

Sound Sound_and_PCA_principalComponents (Sound me, PCA thee, long numberOfComponents);

Sound Sound_and_PCA_whitenSelectedChannels (Sound me, PCA thee, long numberOfComponents, long *channels, long numberOfChannels);

Sound Sound_and_PCA_whitenChannels (Sound me, PCA thee, long numberOfComponents);

#endif /* _Sound_and_PCA_h_ */
