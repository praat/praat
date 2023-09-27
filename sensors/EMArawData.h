#ifndef _EMArawData_h_
#define _EMArawData_h_
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

#include "Sampled.h"

#include "EMArawData_def.h"

autoEMArawData EMArawData_readFromCarstensEMA50xAmpFile (MelderFile file);

autoEMArawData EMArawData_create (double tmin, double tmax, integer numberOfSensors, integer numberOfTransmittersPerSensor, integer numberOfFrames, double dt, double x1);


#endif /* _EMArawData_h_ */
