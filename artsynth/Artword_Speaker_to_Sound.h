/* Artword_Speaker_to_Sound.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "Artword.h"
#include "Speaker.h"
#include "Sound.h"

autoSound Artword_Speaker_to_Sound (Artword artword, Speaker speaker,
   double samplingFrequency, int oversampling,
   autoSound *w1, int iw1, autoSound *w2, int iw2, autoSound *w3, int iw3,
   autoSound *p1, int ip1, autoSound *p2, int ip2, autoSound *p3, int ip3,
   autoSound *v1, int iv1, autoSound *v2, int iv2, autoSound *v3, int iv3);

/* End of file Artword_Speaker_to_Sound.h */
