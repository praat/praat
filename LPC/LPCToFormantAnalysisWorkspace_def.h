/* LPCToFormantAnalysisWorkspace_def.h
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

#define ooSTRUCT LPCToFormantAnalysisWorkspace
oo_DEFINE_CLASS (LPCToFormantAnalysisWorkspace, LPCAnalysisWorkspace)

	oo_DOUBLE (margin)
	oo_STRUCT (Formant_Frame, formantFrame)
	oo_OBJECT (Polynomial, 0, p)
	oo_OBJECT (Roots, 0, roots)

	#if oo_DECLARING

		void getInputFrame (integer iframe) override;
		void allocateOutputFrames () override;
		bool inputFrameToOutputFrame () override;
		void saveOutputFrame () override;
		
	#endif

oo_END_CLASS (LPCToFormantAnalysisWorkspace)
#undef ooSTRUCT

/* End of file LPCToFormantAnalysisWorkspace_def.h */

