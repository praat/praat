#ifndef _Cepstrum_and_Spectrum_h_
#define _Cepstrum_and_Spectrum_h_
/* Cepstrum_and_Spectrum.h
 *
 * Copyright (C) 1994-2020 David Weenink
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

#include "Spectrum.h"
#include "Cepstrum.h"
#include "PowerCepstrum.h"

autoCepstrum Spectrum_to_Cepstrum (Spectrum me);

autoSpectrum Cepstrum_to_Spectrum (Cepstrum me);

autoPowerCepstrum Spectrum_to_PowerCepstrum (Spectrum me);

autoCepstrum Spectrum_to_Cepstrum_hillenbrand (Spectrum me);

#endif /* _Cepstrum_and_Spectrum_h_ */
