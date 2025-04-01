/* LPCFrameIntoSampledFrame_def.h
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

#define ooSTRUCT LPCFrameIntoSampledFrame
oo_DEFINE_CLASS (LPCFrameIntoSampledFrame, SampledFrameIntoSampledFrame)

	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (LPC, inputlpc)
	oo_INTEGER (order) // for conveniance inputlpc -> maxnCoefficients

oo_END_CLASS (LPCFrameIntoSampledFrame)
#undef ooSTRUCT

/* End of file LPCFrameIntoSampledFrame_def.h */

