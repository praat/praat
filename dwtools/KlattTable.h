#ifndef _KlattTable_h_
#define _KlattTable_h_
/* KlattTable.h
 *
 * Copyright (C) 2008-2020 David Weenink
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

/*
 djmw 20081018 Initial version
 djmw 20110306 Latest modification
*/

#include "TableOfReal.h"
#include "Sound.h"
#include "Table.h"

Thing_define (KlattTable, Table) {
};

autoKlattTable KlattTable_create (double frameDuration, double totalDuration);
/*
f0  (08)	This is the fundamental frequency (pitch) of the utterance.
		In this case it is specified in steps of 0.1 Hz, hence 100Hz
		will be represented by a value of 1000.
av  (08)	Amplitude of voicing for the cascade branch of the
		synthesizer in dB0. Range 0-70, value usually 60 for a vowel sound.
f1  (08)	First formant frequency in 200-1300 Hz.
b1  (08)	Cascade branch bandwidth of first formant in the range 40-1000 Hz.
f2  (08)	Second formant frequency in the range 550 - 3000 Hz.
b2  (08)	Cascade branch bandwidth of second  formant in the range 40-1000 Hz.
f3  (08)	Third formant frequency in the range 1200-4999 Hz.
b3  (08)	Cascade branch bandwidth of third formant in the range 40-1000 Hz.
f4  (08)	Fourth formant frequency in 1200-4999 Hz.
b4  (08)	Cascade branch bandwidth of fourth formant in the range 40-1000 Hz.
f5  (08)	Fifth formant frequency in the range 1200-4999 Hz.
b5  (08)	Cascade branch bandwidth of fifth formant in the range 40-1000 Hz.
f6  (08)	Sixth formant frequency in the range 1200-4999 Hz.
b6  (08)	Cascade branch bandwidth of sixth formant in the range 40-2000 Hz.
fnz (08)	Frequency of the nasal zero in the range 248-528 Hz.
			(cascade branch only)
bnz (08)	Bandwidth of the nasal zero in the range 40-1000 Hz
			(cascade branch only)
fnp (08)	Frequency of the nasal pole in the range 248-528 Hz
bnp (08)	Bandwidth of the nasal pole in the range 40-1000 Hz
ah  (08)	Amplitude of aspiration 0-70 dB.
kopen (-8) 	Open quotient of voicing waveform, range 0-60, usually 30.
			Will influence the gravelly or smooth quality of the voice.
			Only works with impulse and antural simulations. For the
			sampled glottal excitation waveform the open quotient is fixed.
aturb 	Amplitude of turbulence 0-80 dB. A value of 40 is useful. Can be
			used to simulate "breathy" voice quality.
tltdb (-8) 	Spectral tilt in dB, range 0-24. Tilts down the output spectrum.
			The value refers to dB down at 3Khz. Increasing the value emphasizes
			the low frequency content of the speech and attenuates the high
			frequency content.
af 	(08)	Amplitude of frication in dB, range 0-80 (parallel branch)

skew  	Spectral Skew - skewness of alternate periods, range 0-40

a1  (08)	Amplitude of first formant in the parallel branch, in 0-80 dB.
b1p (-8)	Bandwidth of the first formant in the parallel branch, in Hz.
a2  (08)	Amplitude of parallel branch second formant.
b2p (-8)	Bandwidth of parallel branch second formant.
a3  (08)	Amplitude of parallel branch third formant.
b3p (-8)	Bandwidth of parallel branch third formant.
a4  (08)	Amplitude of parallel branch fourth formant.
b4p (-8)	Bandwidth of parallel branch fourth formant.
a5  (08)	Amplitude of parallel branch fifth formant.
b5p (-8)	Bandwidth of parallel branch fifth formant.
a6  (08)	Amplitude of parallel branch sixth formant.
b6p (-8)	Bandwidth of parallel branch sixth formant.

anp   	Amplitude of the parallel branch nasal formant.

ab  (08)	Amplitude of bypass frication in dB. 0-80.

avp		Amplitude of voicing for the parallel branch, 0-70 dB.

gain  	Overall gain in dB range 0-80.

*/

autoSound KlattTable_to_Sound (KlattTable me, double samplingFrequency, int synthesisModel, int numberOfFormants, double frameDuration, int voicing, double flutter, int outputType);

autoSound KlattTable_Sound_to_Sound (KlattTable me, Sound thee);

autoKlattTable KlattTable_createExample ();

autoKlattTable Table_to_KlattTable (Table me);

autoTable KlattTable_to_Table (KlattTable me);

autoKlattTable KlattTable_readFromRawTextFile (MelderFile fs);

#endif /* _KlattTable_h_ */
