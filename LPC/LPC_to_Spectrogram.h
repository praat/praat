#ifndef _LPC_to_Spectrogram_h_
#define _LPC_to_Spectrogram_h_
/* LPC_to_Spectrogram.h
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
 djmw 19980322
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/

#include "LPC.h"
#include "Spectrogram.h"
#include "LPC_to_Spectrum.h"

autoSpectrogram LPC_to_Spectrogram (LPC me, double dfMin, double bandwidthReduction, double deEmphasisFrequency);
/* if(dfMin >= 0) df <= dfMin else df = NyquistFrequency / 512 */

#endif /* _LPC_and_Spectrogram_h_ */
