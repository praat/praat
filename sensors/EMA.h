#ifndef _EMA_h_
#define _EMA_h_
/* EMA.h
 *
 * Copyright (C) 2023 David Weenink
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

#include "melder.h"
#include "Sampled.h"

#include "EMA_def.h"

autoEMA EMA_readFromCarstensEMA50xPosFile (MelderFile file);

autoEMA EMA_create (double tmin, double tmax, integer numberOfSensors, integer numberOfFrames, double dt, double x1);

void CarstensEMA_processHeader (MelderFile file, integer *out_version, integer *out_headerSizeBytes, 
	integer *out_numberOfSensors, integer *out_samplingFrequencyHz, integer *out_numberOfBytesInLines1234);
#endif /* _EMA_h_ */
