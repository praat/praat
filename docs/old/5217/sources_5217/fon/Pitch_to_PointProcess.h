/* Pitch_to_PointProcess.h
 *
 * Copyright (C) 1992-2003 Paul Boersma
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
 * pb 1996/04/26
 * pb 2002/07/16 GPL
 * pb 2003/02/26
 */

#ifndef _Pitch_h_
	#include "Pitch.h"
#endif
#ifndef _PointProcess_h_
	#include "PointProcess.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

PointProcess Pitch_to_PointProcess (Pitch pitch);

PointProcess Sound_Pitch_to_PointProcess_cc (Sound sound, Pitch pitch);

PointProcess Sound_Pitch_to_PointProcess_peaks (Sound sound, Pitch pitch, int includeMaxima, int includeMinima);

/* End of file Pitch_to_PointProcess.h */
