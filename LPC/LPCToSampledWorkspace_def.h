/* LPCToSampledWorkspace_def.h
 *
 * Copyright (C) 2024 David Weenink
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

#define ooSTRUCT LPCToSampledWorkspace
oo_DEFINE_CLASS (LPCToSampledWorkspace, SampledToSampledWorkspace)

	oo_DOUBLE (samplingPeriod)
	oo_INTEGER (maxnCoefficients)
	oo_STRUCT (LPC_Frame, lpcFrame)
	
	#if oo_DECLARING

		LPC_Frame lpcFrameRef = & lpcFrame;

	#endif

	#if oo_COPYING

		thy lpcFrameRef = & thy lpcFrame;

	#endif

oo_END_CLASS (LPCToSampledWorkspace)
#undef ooSTRUCT

/* End of file LPCToSampledWorkspace_def.h */

