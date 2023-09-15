/* Sensor_def.h
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

#define ooSTRUCT Sensor
oo_DEFINE_CLASS (Sensor, Vector) // or Sound??

	oo_STRING_VECTOR (channelNames, ny)
	
	#if oo_DECLARING
		void v1_info ()
			override;
		int v_domainQuantity () const
			override { return MelderQuantity_TIME_SECONDS; }
	#endif
	
oo_END_CLASS (Sensor)
#undef ooSTRUCT

/* End of file Sensor_def.h */
