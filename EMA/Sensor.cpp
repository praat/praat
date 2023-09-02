/* Sensor.cpp
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

#include "Sensor.h"

#include "oo_DESTROY.h"
#include "Sensor_def.h"
#include "oo_COPY.h"
#include "Sensor_def.h"
#include "oo_EQUAL.h"
#include "Sensor_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Sensor_def.h"
#include "oo_WRITE_TEXT.h"
#include "Sensor_def.h"
#include "oo_WRITE_BINARY.h"
#include "Sensor_def.h"
#include "oo_READ_TEXT.h"
#include "Sensor_def.h"
#include "oo_READ_BINARY.h"
#include "Sensor_def.h"
#include "oo_DESCRIPTION.h"
#include "Sensor_def.h"

Thing_implement (Sensor, Vector, 2);

void structSensor :: v1_info () {
	structDaata:: v1_info();
}

autoSensor Sensor_create (integer numberOfChannels, double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoSensor me = Thing_new (Sensor);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1, numberOfChannels, numberOfChannels, 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create Sensor object.");
	}
}

void Sensor_setChannelName (Sensor me, integer channel, conststring32 newName);

integer Sensor_getChannelNumber (Sensor me, conststring32 channelName);

 /* End of file Sensor.cpp */
 
