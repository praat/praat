/* SampledIntoSampledStatus_def.h
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

#define ooSTRUCT SampledIntoSampledStatus
oo_DEFINE_CLASS (SampledIntoSampledStatus, Daata)

	oo_INTEGER (numberOfFrames)
	oo_INTVEC (frameIntoFrameInfo, numberOfFrames)

	#if oo_DECLARING
		virtual void showStatus ();
	#endif

oo_END_CLASS (SampledIntoSampledStatus)	
#undef ooSTRUCT

#define ooSTRUCT SoundIntoSampledStatus
oo_DEFINE_CLASS (SoundIntoSampledStatus, SampledIntoSampledStatus)

	oo_INTVEC (soundFrameBegins, numberOfFrames)
	#if oo_DECLARING
		void showStatus () override;
	#endif
	
oo_END_CLASS (SoundIntoSampledStatus)	
#undef ooSTRUCT


/* End of file SampledIntoSampledStatus_def.h */	
