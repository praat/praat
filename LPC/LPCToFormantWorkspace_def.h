/* LPCToFormantWorkspace_def.h
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

#define ooSTRUCT LPCToFormantWorkspace
oo_DEFINE_CLASS (LPCToFormantWorkspace, LPCToSampledWorkspace)

	oo_DOUBLE (margin)
	oo_INTEGER (maxnFormants) // for initialisation of the Formant_Frame if output object is missing
	oo_STRUCT (Formant_Frame, formantFrame)
	
	oo_OBJECT (Polynomial, 0, p)	// for the coefficients
	oo_OBJECT (Roots, 0, roots)		// the roots of the polynomial

	#if oo_DECLARING
	
		Formant_Frame formantFrameRef = & formantFrame;

		void getInputFrame () override;
		void allocateOutputFrames () override;
		bool inputFrameToOutputFrame () override;
		void saveOutputFrame () override;
		
	#endif
		
	#if oo_COPYING
	
		thy formantFrameRef = & thy formantFrame;

	#endif

oo_END_CLASS (LPCToFormantWorkspace)
#undef ooSTRUCT

/* End of file LPCToFormantWorkspace_def.h */

