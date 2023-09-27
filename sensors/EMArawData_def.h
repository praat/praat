/* EMArawData_def.h
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

#define ooSTRUCT EMATransmitter_Frame
oo_DEFINE_STRUCT (EMATransmitter_Frame)
	oo_INTEGER (numberOfTransmitters)
	oo_VEC (amplitudes, numberOfTransmitters)
oo_END_STRUCT (EMATransmitter_Frame)
#undef ooSTRUCT

#define ooSTRUCT EMArawData_Frame
oo_DEFINE_STRUCT (EMArawData_Frame)
	oo_INTEGER (numberOfSensors)
	oo_STRUCTVEC (EMATransmitter_Frame, transmitterFrames, numberOfSensors)
oo_END_STRUCT (EMArawData_Frame)
#undef ooSTRUCT

#define ooSTRUCT EMArawData
oo_DEFINE_CLASS (EMArawData, Sampled)
	oo_INTEGER (numberOfSensors)
	oo_INTEGER (numberOfTransmitters)
	oo_STRUCTVEC (EMArawData_Frame, emaRawDataFrames, nx)
	oo_MAT (sensorCalibrations, numberOfSensors, numberOfTransmitters)
	#if oo_DECLARING
		void v1_info ()
			override;
		int v_domainQuantity () const
			override { return MelderQuantity_TIME_SECONDS; }
	#endif

oo_END_CLASS (EMArawData)
#undef ooSTRUCT

/* End of file EMArawData_def.h */
