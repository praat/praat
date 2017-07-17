/* Spectrum_and_Spectrogram.h
 *
 * Copyright (C) 1992-2011,2015 David Weenink & Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Spectrum.h"
#include "Spectrogram.h"

autoSpectrum Spectrogram_to_Spectrum (Spectrogram me, double time);
/*
	Function:
		Create a time slice from the Spectrogram at the time nearest to 'time'.
	Postconditions:
		result -> xmin == my ymin;   // Lowest frequency; often 0.
		result -> xmax == my ymax;   // Highest frequency.
		result -> nx == my ny;   // Number of frequency bands.
		result -> dx == my dy;   // Frequency step.
		result -> x1 == my y1;   // Centre of first frequency band.
		for (iy = 1; iy <= my ny; iy ++) {  
			result -> z [1] [i] == sqrt (my z [i] ['time']);
			result -> z [2] [i] == 0.0;
		}
*/
 
autoSpectrogram Spectrum_to_Spectrogram (Spectrum me);
/*
	Function:
		Create a Spectrogram with one time slice from the Spectrum.
	Postconditions:
		thy xmin = 0.0;			thy ymin == my xmin;
		thy xmax = 1.0;		thy ymax == my xmax;
		thy nx == 1;			thy ny == my nx;
		thy dx == 1.0;			thy dy == my dx;
		thy x1 == 0.5;			thy y1 == my x1;
		for (i = 1; i <= my nx; i ++) 
			thy z [i] [1] == (my z [1] [i]) ^ 2 + (my z [2] [i]) ^ 2;
*/

/* End of file Spectrum_and_Spectrogram.h */
