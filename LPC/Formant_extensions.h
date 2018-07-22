#ifndef _Formant_extensions_h_
#define _Formant_extensions_h_
/* Formant_extensions.h
 *
 * Copyright (C) 2012-2017 David Weenink
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
 djmw 20121214 Initial version
*/


#include "Formant.h"
#include "IntensityTier.h"
#include "Spectrogram.h"


void Formant_formula (Formant me, double tmin, double tmax, integer formantmin, integer formantmax, Interpreter interpreter, conststring32 expression);
autoIntensityTier Formant_Spectrogram_to_IntensityTier (Formant me, Spectrogram thee, integer iformant);

#endif /* _Formant_extensions_h_ */
