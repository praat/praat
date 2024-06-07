/* SoundToFormantAnalysisWorkspace_def.h
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
oo_DEFINE_CLASS (LPCToFormantAnalysisWorkspace, SampledAnalysisWorkspace)

	void (*LPC_Frame_into_Formant_Frame) (LPCToFormantAnalysisWorkspace me, LPC_Frame in, Formant_Frame out);
	
	#if oo_DECLARING
	
		void allocateSampledFrames (SoundAnalysisWorkspace me) override {
			Melder_assert (my result != nullptr);
			Formant thee = reinterpret_cast<Formant> (my result);
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				const Formant_Frame formantFrame = & thy frames [iframe];
				Formant_Frame_init (formantFrame, thy maxnFormants);
			}
		}
		
	#endif
	#if oo_COPYING
		thy LPC_Frame_into_Formant_Frame = our LPC_Frame_into_Formant_Frame;
	#endif

oo_END_CLASS (LPCToFormantAnalysisWorkspace)
#undef ooSTRUCT

#define ooSTRUCT SoundToFormantAnalysisWorkspace
oo_DEFINE_CLASS (SoundToFormantAnalysisWorkspace, SampledAnalysisWorkspace)

	oo_OBJECT (SoundToLPCAnalysisWorkspace, soundToLPC)
	oo_OBJECT (LPCToFormantAnalysisWorkspace, lpcToFormant)

oo_END_CLASS (SoundToFormantAnalysisWorkspace)
#undef ooSTRUCT


/* End of file SoundToFormantAnalysisWorkspace_def.h */
 
