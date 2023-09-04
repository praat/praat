#ifndef _Sensor_h_
#define _Sensor_h_
/* Sensor.h
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

#include "Vector.h"

#include "Sensor_def.h"

autoSensor Sensor_create (integer numberOfChannels, double xmin, double xmax, integer nx, double dx, double x1);

void Sensor_setChannelName (Sensor me, integer channel, conststring32 newName);

integer Sensor_getChannelNumber (Sensor me, conststring32 channelName);

#endif /* _Sensor_h_ */
 
