/* MultiSampledSpectrogram_def.h
 * 
 * Copyright (C) 2021-2022 David Weenink
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

#define ooSTRUCT  MultiSampledSpectrogram
oo_DEFINE_CLASS (MultiSampledSpectrogram, Sampled)

	oo_DOUBLE (tmin) // invariant: frequencyBin [i] -> xmin = my tmin for all i
	oo_DOUBLE (tmax) // invariant: frequencyBin [i] -> xmax = my tmax for all i
	oo_DOUBLE (frequencyResolutionInBins)
	
	oo_INTEGER (numberOfSpectralValues) // needed for resythesis
	oo_VEC (frequencyAmplifications, numberOfSpectralValues) // keep track of individual frequencies for resynthesis)
	
	oo_COLLECTION_OF (OrderedOf, frequencyBins, FrequencyBin, 0) // invariant frequencyBins.size == my nx
	oo_OBJECT (FrequencyBin, 0, zeroBin)	// needs special treatment 														 
	oo_OBJECT (FrequencyBin, 0, nyquistBin)	// needs special treatment
	
	#if oo_DECLARING
		void v1_info ()
			override;
		double v_getValueAtSample (integer ifreq, integer iframe, int unit) const
			override;
		virtual double v_myFrequencyUnitToHertz (double x) const = 0;
		virtual double v_hertzToMyFrequencyUnit (double f_hz) const = 0;
	#endif

oo_END_CLASS (MultiSampledSpectrogram)
#undef ooSTRUCT

/* End of file MultiSampledSpectrogram_def.h */
