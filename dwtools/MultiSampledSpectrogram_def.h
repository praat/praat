/* MultiSampledSpectrogram_def.h
 * 
 * Copyright (C) 2021 David Weenink
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

#define ooSTRUCT FrequencyBin
oo_DEFINE_CLASS (FrequencyBin, Sampled)

	oo_MAT (z, 2, nx) // re & im 

oo_END_CLASS (FrequencyBin)
#undef ooSTRUCT

#define ooSTRUCT  MultiSampledSpectrogram
oo_DEFINE_CLASS (MultiSampledSpectrogram, Sampled)

	oo_COLLECTION_OF (OrderedOf, frequencyBins, FrequencyBin, 0) // invariant frequencyBins.size == my nx
	
	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (MultiSampledSpectrogram)
#undef ooSTRUCT

/* End of file MultiSampledSpectrogram_def.h */
