/* SoundFrameIntoLPCFrame_def.h
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

#define ooSTRUCT SoundFrameIntoLPCFrame
oo_DEFINE_CLASS (SoundFrameIntoLPCFrame, SoundFrameIntoSampledFrame)

	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (LPC, outputlpc)
	oo_INTEGER (order)
	oo_INTEGER (currentOrder)
	oo_INTEGER (orderp1) 	// convenience order+1
	oo_VEC (a, orderp1)		// common work vector

	#if oo_DECLARING
	
		void allocateOutputFrames (void) override;
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;
		void saveLocalOutputFrames (void) {};
	#endif

oo_END_CLASS (SoundFrameIntoLPCFrame)
#undef ooSTRUCT

#define ooSTRUCT SoundFrameIntoLPCFrameAuto
oo_DEFINE_CLASS (SoundFrameIntoLPCFrameAuto, SoundFrameIntoLPCFrame)
	oo_VEC (r, orderp1)
	oo_VEC (rc, order)
	
	#if oo_DECLARING
		bool inputFrameToOutputFrame (void) override;
	#endif
		
oo_END_CLASS (SoundFrameIntoLPCFrameAuto)
#undef ooSTRUCT

#define ooSTRUCT SoundFrameIntoLPCFrameCovar
oo_DEFINE_CLASS (SoundFrameIntoLPCFrameCovar, SoundFrameIntoLPCFrame)
	oo_INTEGER (order2) // order*(order+1)/2
	oo_VEC (b, order2)
	oo_VEC (grc, order)
	oo_VEC (beta, order)
	oo_VEC (cc, orderp1)
	
	#if oo_DECLARING
		bool inputFrameToOutputFrame (void) override;
	#endif
		
oo_END_CLASS (SoundFrameIntoLPCFrameCovar)
#undef ooSTRUCT

#define ooSTRUCT SoundFrameIntoLPCFrameBurg
oo_DEFINE_CLASS (SoundFrameIntoLPCFrameBurg, SoundFrameIntoLPCFrame)
	oo_VEC (b1, soundFrameSize)
	oo_VEC (b2, soundFrameSize)
	oo_VEC (aa, order)
	
	#if oo_DECLARING
		bool inputFrameToOutputFrame (void) override;
	#endif
		
oo_END_CLASS (SoundFrameIntoLPCFrameBurg)
#undef ooSTRUCT

#define ooSTRUCT SoundFrameIntoLPCFrameMarple
oo_DEFINE_CLASS (SoundFrameIntoLPCFrameMarple, SoundFrameIntoLPCFrame)
	oo_DOUBLE (tol2)
	oo_VEC (c, orderp1)
	oo_VEC (d, orderp1)
	oo_VEC (r, orderp1)
	
	#if oo_DECLARING
		bool inputFrameToOutputFrame (void) override;
	#endif
		
oo_END_CLASS (SoundFrameIntoLPCFrameMarple)
#undef ooSTRUCT

#define ooSTRUCT LPCAndSoundFramesIntoLPCFrameRobust
oo_DEFINE_CLASS (LPCAndSoundFramesIntoLPCFrameRobust, SoundFrameIntoLPCFrame)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, inputlpc)	// TODO rename as otherInputLPC ?
	oo_STRUCT (LPC_Frame, otherInputLPCFrame)
	
	oo_INTEGER (currentPredictionOrder)
	oo_DOUBLE (tol2)
	oo_DOUBLE (k_stdev)
	oo_INTEGER (iter)
	oo_INTEGER (itermax)
	oo_INTEGER (huber_iterations) // = 5;
	oo_BOOLEAN (wantlocation)
	oo_BOOLEAN (wantscale)
	oo_DOUBLE (location)
	oo_DOUBLE (scale)
	oo_VEC (error, soundFrameSize)
	oo_VEC (sampleWeights, soundFrameSize)
	oo_VEC (coefficients, inputlpc -> maxnCoefficients)
	oo_VEC (covariancesw, inputlpc -> maxnCoefficients)
	oo_MAT (covarmatrixw, inputlpc -> maxnCoefficients, inputlpc -> maxnCoefficients)
	oo_INTEGER (computedSVDworksize)
	oo_OBJECT (SVD, 1, svd)
	oo_VEC (svdwork1, computedSVDworksize)
	oo_VEC (svdwork2, order)
	oo_VEC (filterMemory, order)
	oo_VEC (huberwork, soundFrameSize)
	
	#if oo_DECLARING
		bool inputFrameToOutputFrame (void) override;
	#endif

oo_END_CLASS (LPCAndSoundFramesIntoLPCFrameRobust)
#undef ooSTRUCT

#define ooSTRUCT SoundFrameIntoLPCFrameRobust
oo_DEFINE_CLASS (SoundFrameIntoLPCFrameRobust, SoundFrameIntoLPCFrame)

	oo_OBJECT (SoundFrameIntoLPCFrame, 0, soundIntoLPC)
	oo_OBJECT (LPCAndSoundFramesIntoLPCFrameRobust, 0, lpcAndSoundIntoLPC)

	#if oo_DECLARING
		bool inputFrameToOutputFrame (void) override;
		void saveOutputFrame (void) override;
	#endif

oo_END_CLASS (SoundFrameIntoLPCFrameRobust)
#undef ooSTRUCT

/* End of file SoundFrameIntoLPCFrame_def.h */
