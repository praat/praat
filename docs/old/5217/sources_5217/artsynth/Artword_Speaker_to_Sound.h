/* Artword_Speaker_to_Sound.h
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
 * pb 1995/04/06
 * pb 2002/07/16 GPL
 */

#ifndef _Artword_h_
	#include "Artword.h"
#endif
#ifndef _Speaker_h_
	#include "Speaker.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

Sound Artword_Speaker_to_Sound (Artword artword, Speaker speaker,
   double samplingFrequency, int oversampling,
   Sound *w1, int iw1, Sound *w2, int iw2, Sound *w3, int iw3,
   Sound *p1, int ip1, Sound *p2, int ip2, Sound *p3, int ip3,
   Sound *v1, int iv1, Sound *v2, int iv2, Sound *v3, int iv3);

/* End of file Artword_Speaker_to_Sound.h */
