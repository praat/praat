/* EMA_def.h
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

#define ooSTRUCT EMA
oo_DEFINE_CLASS (EMA, Function)
	oo_INTEGER (numberOfSensors)
	oo_STRING_VECTOR (sensorNames, numberOfSensors)
	oo_COLLECTION_OF (OrderedOf, sensors, Sensor, 0)

	#if oo_DECLARING
		void v1_info ()
			override;
		int v_domainQuantity () const
			override { return MelderQuantity_TIME_SECONDS; }
	#endif

oo_END_CLASS (EMA)
#undef ooSTRUCT

#define ooSTRUCT EMAamp
oo_DEFINE_CLASS (EMAamp, EMA)
	oo_INTEGER (numberOfTransmitters)
	oo_MAT (sensorCalibrations, numberOfSensors, numberOfTransmitters)

	#if oo_DECLARING
		void v1_info ()
			override;
	#endif

oo_END_CLASS (EMAamp)
#undef ooSTRUCT

/* End of file EMA_def.h */
