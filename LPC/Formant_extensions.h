#ifndef _Formant_extensions_h_
#define _Formant_extensions_h_
/* Formant_extensions.h
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
 djmw 20121214 Initial version
*/


#include "Formant.h"
#include "IntensityTier.h"
#include "Spectrogram.h"


void Formant_formula (Formant me, double tmin, double tmax, long formantmin, long formantmax, Interpreter interpreter, wchar_t *expression);
IntensityTier Formant_and_Spectrogram_to_IntensityTier (Formant me, Spectrogram thee, long iformant);

#endif /* _Formant_extensions_h_ */
