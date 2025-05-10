/* PowerCepstrogramFrameIntoMatrixFrame_def.h
 *
 * Copyright (C) 2025 David Weenink
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

#define ooSTRUCT PowerCepstrogramFrameIntoMatrixFrame
oo_DEFINE_CLASS (PowerCepstrogramFrameIntoMatrixFrame, SampledFrameIntoMatrixFrame)

	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (PowerCepstrogram, powercepstrogram)
	
	oo_OBJECT (PowerCepstrum, 2, powerCepstrum) // copy of a column of the PowerCepstrogram
	oo_OBJECT (PowerCepstrumWorkspace, 0, powerCepstrumWs)
	oo_BOOLEAN (getPeakAndPosition)
	oo_BOOLEAN (getSlopeAndIntercept)
	oo_BOOLEAN (subtractTrend)
	oo_BOOLEAN (trendSubtracted) // TODO ?? needed
	
	#if oo_DECLARING

		void getInputFrame () override;

		bool inputFrameToOutputFrame (void) override;
		
		void saveOutputFrame () override;

		void saveLocalOutputFrames (void) override;

	#endif
oo_END_CLASS (PowerCepstrogramFrameIntoMatrixFrame)
#undef ooSTRUCT

/* End of file PowerCepstrogramFrameIntoMatrixFrame_def.h */
