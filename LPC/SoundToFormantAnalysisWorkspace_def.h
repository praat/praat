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

#define ooSTRUCT SoundToFormantAnalysisWorkspace
oo_DEFINE_CLASS (SoundToFormantAnalysisWorkspace, SoundAnalysisWorkspace)

	oo_OBJECT (SoundToLPCAnalysisWorkspace, 0, soundToLPC)
	oo_OBJECT (LPCToFormantAnalysisWorkspace, 0, lpcToFormant)

	#if oo_DECLARING

		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;

	#endif

oo_END_CLASS (SoundToFormantAnalysisWorkspace)
#undef ooSTRUCT

/* End of file SoundToFormantAnalysisWorkspace_def.h */
 
