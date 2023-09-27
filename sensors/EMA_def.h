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

#define ooSTRUCT EMAsensor_Frame
oo_DEFINE_STRUCT (EMAsensor_Frame)
	oo_DOUBLE (x)
	oo_DOUBLE (y)
	oo_DOUBLE (z)
	oo_DOUBLE (phi)
	oo_DOUBLE (theta)
oo_END_STRUCT (EMAsensor_Frame)
#undef ooSTRUCT

#define ooSTRUCT EMA_Frame
oo_DEFINE_STRUCT (EMA_Frame)
	oo_INTEGER (numberOfSensors)
	oo_STRUCTVEC (EMAsensor_Frame, sensorFrames, numberOfSensors)
oo_END_STRUCT (EMA_Frame)
#undef ooSTRUCT

#define ooSTRUCT EMA
oo_DEFINE_CLASS (EMA, Sampled)
	oo_INTEGER (numberOfSensors)
	oo_STRING_VECTOR (sensorNames, numberOfSensors)
	oo_STRUCTVEC (EMA_Frame, emaFrames, nx)

	#if oo_DECLARING
		void v1_info ()
			override;
		int v_domainQuantity () const
			override { return MelderQuantity_TIME_SECONDS; }
	#endif

oo_END_CLASS (EMA)
#undef ooSTRUCT

/* End of file EMA_def.h */
