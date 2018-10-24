/* SPINET_def.h
 *
 * Copyright (C) 1997 David Weenink
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

#define ooSTRUCT SPINET
oo_DEFINE_CLASS (SPINET, SampledXY)

	oo_INTEGER (gamma)						/* filter order */
	oo_DOUBLE (excitationErbProportion)	/* excitatory bandwidth proportionality factor*/
	oo_DOUBLE (inhibitionErbProportion)	/* inhibitatory bandwidth proportionality factor*/
	oo_MAT (y, ny, nx) /* short term average energy spectrum */
	/* spectrum after on-center/off-surround and rectification */
	oo_MAT (s, ny, nx)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (SPINET)
#undef ooSTRUCT

/* End of file SPINET_def.h */
