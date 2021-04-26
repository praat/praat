/* ConstantQLogFSpectrogram_def.h
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


#define ooSTRUCT ConstantQLogFSpectrogram
oo_DEFINE_CLASS (ConstantQLogFSpectrogram, MultiSampledSpectrogram)

	oo_DOUBLE (frequencyResolutionInBins) // invariant frequencyBins.size == my nx
	
	#if oo_DECLARING
		void v_info ()
			override;
		double v_getValueAtSample (integer /* ifreq */, integer /* iframe */ , int /* unit */)
			override;
		double v_myFrequencyToHertz (double /* log2_f */)
			override;
		double v_hertzToMyFrequency (double /* f_hz */)
			override;
	#endif

oo_END_CLASS (ConstantQLogFSpectrogram)
#undef ooSTRUCT

/* End of file ConstantQLogFSpectrogram_def.h */
