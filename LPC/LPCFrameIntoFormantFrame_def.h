/* LPCFrameIntoFormantFrame_def.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

#define ooSTRUCT LPCFrameIntoFormantFrame
oo_DEFINE_CLASS (LPCFrameIntoFormantFrame, LPCFrameIntoSampledFrame)

	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Formant, formant)
	oo_DOUBLE (margin)
	
	oo_OBJECT (Polynomial, 0, p)	// for the coefficients
	oo_OBJECT (Roots, 0, roots)		// the roots of the polynomial
	oo_INTEGER (bufferSize)
	oo_VEC (buffer, bufferSize)
	
	#if oo_DECLARING
	
		void getInputFrame (void) override;
		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;
		void saveLocalOutputFrames (void) {};
	#endif

oo_END_CLASS (LPCFrameIntoFormantFrame)
#undef ooSTRUCT

/* End of file LPCFrameIntoFormantFrame_def.h */

