#ifndef _Sound_to_DTW_h_
#define _Sounds_to_DTW_h_
/* Sounds_to_DTW.h
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
 djmw 20120221
*/

#include "DTW.h"
#include "Sound.h"


DTW Sounds_to_DTW (Sound me, Sound thee, double analysisWidth, double dt, double band, int slope);

#endif /* _Sounds_to_DTW_h_ */
