#ifndef _Sound_and_PCA_h_
#define _Sound_and_PCA_h_
/* Sound_and_PCA.h
 *
 * Copyright (C) 2012-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PCA.h"
#include "Sound.h"

autoPCA Sound_to_PCA_channels (Sound me, double startTime, double endTime);

autoSound Sound_PCA_to_Sound_pc_selectedChannels (Sound me, PCA thee, integer numberOfComponents, constINTVEC const& channels);

autoSound Sound_PCA_principalComponents (Sound me, PCA thee, integer numberOfComponents);

autoSound Sound_PCA_whitenSelectedChannels (Sound me, PCA thee, integer numberOfComponents, constINTVEC const& channels);

autoSound Sound_PCA_whitenChannels (Sound me, PCA thee, integer numberOfComponents);

/* End of file Sound_and_PCA.h */
#endif
