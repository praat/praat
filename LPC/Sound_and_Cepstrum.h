#ifndef _Sound_and_Cepstrum_h_
#define _Sound_and_Cepstrum_h_
/* Sound_and_Cepstrum.h
 *
 * Copyright (C) 1994-2011, 2015 David Weenink
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

/*
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/

#include "Sound.h"
#include "Cepstrum.h"

autoCepstrum Sound_to_Cepstrum (Sound me);

autoCepstrum Sound_to_Cepstrum_bw (Sound me);

autoSound Cepstrum_to_Sound (Cepstrum me);

#endif /* _Sound_and_Cepstrum_h_ */
