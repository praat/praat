/* KlattTable.cpp
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

/* The README file that accompanied the version 3.03 software:

Klatt Cascade-Parallel Formant Synthesizer v 3.03 (April 1994)
--------------------------------------------------------------

Jon Iles           j.p.iles@cs.bham.ac.uk
Nick Ing-Simmons   nicki@lobby.ti.com

History
-------

This file contains a version of the Klatt Cascade-Parallel Formant
Speech Synthesizer. The software for this synthesizer was originally
described in (1) and an updated version of the software
was described in (2). The most recent version of the software
synthesizer as described in (2) is commercially available from
Sensimetrics Inc. (3)

The code contained within this directory is a translation of the
original Fortran, into C, by Dennis Klatt. In terms of the two
articles referred to above, this version seems to be the mid point of the
development between the two systems described.


Modifications
-------------

The main part of the code in this directory was posted to comp.speech
in early 1993 as part of a crude text to speech conversion system. The
code taken from comp.speech seemed to have been modified considerably
from the original, and for use of the synthesizer in research it was
necessary to "fix" the changes that had been made. The major changes
that have been made are:

1. Re-introduced the parallel-only / cascade-parallel switch. This
allows choice of synthesis method, either using both branches, or just
using the parallel branch.

2. Correct use of bandwidth parameters. One of the cascade bandwidth
parameters was being wrongly used in the parallel branch of the
synthesizer.

3. Modified operation of natural voicing source. The amplitude of the
natural voicing source was very much smaller than the amplitude of the
impulse source, making it difficult to swap between them to evaluate
the differences.

4. Removed the software synthesizer from the context of a text to
speech system. The synthesizer is now a stand-alone program, accepting
input as a set of parameters from a file, and allowing output to a
file or to stdout.

5. Increased the number of parameters available for use in the input
file. The original comp.speech software made assumptions about a
number of the control parameters. To provide the greatest flexibility,
these parameters have been made specific in the input file. It is
important to note that the input file format is NOT compatible with
that used by the software originally posted to the comp.speech news group.

6. Added command line options to control the parameters that remain
constant during synthesis.

7. Added F0 flutter control, as described in (2).

8. Subsequently the code in parwave was re-written by Nick to improve
efficiency, and add a more acceptable ANSI style, and generally make
an elegant implementation.

9. Further re-writes have been carried out to remove all global references.
All parameters are passed around in structures.

10. The facility to use a sampled natural excitation waveform has been
implemented. Naturalness of the resulting synthetic speech can be greatly
improved by using the glottal excitation waveform from a natural speaker,
especially if it is the speaker on whose voice the synthesis is actually
based. This may be obtained indirectly by inverse-filtering a vowel.

11. This synthesizer appears in modified form as part of Nick's
rsynth text-to-speech system. This may be found at svr-ftp.eng.cam.ac.uk
in comp.speech/sources.

12. Fixed bug to the antiresonator code that caused overflow problems
when compiling on a PC

13. Various minor modifications to ensure correct compilation using
Microsoft C 7.0 (tested) and Borland C (untested).

14. Modified random number generation for noise production as
previously it was dependent on the size of the "long" type.

Input File Format
-----------------

The input file consists of a series of parameter frames. Each frame of
parameters (usually) represents 10ms of audio output, although this
figure can be adjusted down to 1ms per frame. The parameters in each
frame are described below. To avoid confusion, note that the cascade
and parallel branch of the synthesizer duplicate some of the control
parameters.

f0	This is the fundamental frequency (pitch) of the utterance
	in this case it is specified in steps of 0.1 Hz, hence 100Hz
	will be represented by a value of 1000.

av    	Amplitude of voicing for the cascade branch of the
	synthesizer in dB0. Range 0-70, value usually 60 for a vowel sound.

f1    	First formant frequency in 200-1300 Hz.

b1    	Cascade branch bandwidth of first formant in the range 40-1000 Hz.

f2     	Second formant frequency in the range 550 - 3000 Hz.

b2      Cascade branch bandwidth of second  formant in the range 40-1000 Hz.

f3      Third formant frequency in the range 1200-4999 Hz.

b3      Cascade branch bandwidth of third formant in the range 40-1000 Hz.

f4      Fourth formant frequency in 1200-4999 Hz.

b4      Cascade branch bandwidth of fourth formant in the range 40-1000 Hz.

f5      Fifth formant frequency in the range 1200-4999 Hz.

b5      Cascade branch bandwidth of fifth formant in the range 40-1000 Hz.

f6      Sixth formant frequency in the range 1200-4999 Hz.

b6      Cascade branch bandwidth of sixth formant in the range 40-2000 Hz.

fnz  	Frequency of the nasal zero in the range 248-528 Hz.
	(cascade branch only) 	An implementation of a Klatt cascade-parallel formant synthesizer.
	A re-implementation in C of Dennis Klatt's Fortran code, by:

		Jon Iles (j.p.iles@cs.bham.ac.uk)
		Nick Ing-Simmons (nicki@lobby.ti.com)

	This code is a slightly modified version of the code of


bnz   	Bandwidth of the nasal zero in the range 40-1000 Hz
	(cascade branch only)

fnp   	Frequency of the nasal pole in the range 248-528 Hz

bnp   	Bandwidth of the nasal pole in the range 40-1000 Hz

ah    	Amplitude of aspiration 0-70 dB.

kopen 	Open quotient of voicing waveform, range 0-60, usually 30.
	Will influence the gravelly or smooth quality of the voice.
	Only works with impulse and antural simulations. For the
	sampled glottal excitation waveform the open quotient is fixed.

aturb 	Amplitude of turbulence 0-80 dB. A value of 40 is useful. Can be
	used to simulate "breathy" voice quality.

tilt  	Spectral tilt in dB, range 0-24. Tilts down the output spectrum.
	The value refers to dB down at 3Khz. Increasing the value emphasizes
	the low frequency content of the speech and attenuates the high
	frequency content.

af    	Amplitude of frication in dB, range 0-80 (parallel branch)

skew  	Spectral Skew - skewness of alternate periods, range 0-40

a1    	Amplitude of first formant in the parallel branch, in 0-80 dB.

b1p  	Bandwidth of the first formant in the parallel branch, in Hz.

a2    	Amplitude of parallel branch second formant.

b2p   	Bandwidth of parallel branch second formant.

a3    	Amplitude of parallel branch third formant.

b3p	Bandwidth of parallel branch third formant.

a4    	Amplitude of parallel branch fourth formant.

b4p   	Bandwidth of parallel branch fourth formant.

a5	Amplitude of parallel branch fifth formant.

b5p   	Bandwidth of parallel branch fifth formant.

a6	Amplitude of parallel branch sixth formant.

b6p   	Bandwidth of parallel branch sixth formant.

anp   	Amplitude of the parallel branch nasal formant.

ab    	Amplitude of bypass frication in dB. 0-80.

avp	Amplitude of voicing for the parallel branch, 0-70 dB.

gain  	Overall gain in dB range 0-80.



Command Line Options
--------------------

-h Displays a help message.

-i <filename> sets input filename.

-o <outfile> sets output filename.
   If output filename not specified, stdout is used.

-q quiet - print no messages.

-t <n> select output waveform (RTFC !)

-c select cascade-parallel configuration.
   Parallel only configuration is default.

-n <number> Number of formants in cascade branch.
   Default is 5.

-s <n> set sample rate
   Default is 10Khz.

-f <n> set number of milliseconds per frame.
   Default is 10ms per frame

-v <n> Specifies that the voicing source to be used.
   1 = impulse train.
   2 = natural simulation.
   3 = natural samples.
   Default is natural voicing

-V <filename> Specifies the filename for a sampled natural excitation
   waveform. See man page for format details.

-r <n> Output raw binary samples, rather than ASCII integer samples.
   1 = high byte, low byte arrangement.
   2 = low byte, high byte arrangement.

-F <percent> percentage of f0 flutter
   Default is 0


Example Parameter File
----------------------

Some example parameter files for a short segments of speech are included in
this distribution. e.g. file called example1.par. Use the following
to produce the output waveforms:

	klatt -i example1.par -o example1.dat -f 5 -v 2
	klatt -i example2.par -o example2.dat -f 5 -s 16000 -v 2

The '-r' option can be used to produce raw binary output, which can
then be converted to many different formats using utilities such as
'sox' (sound exchange) which are available from major ftp sites.

An example is given below of conversion to the ulaw encoded format
used by Sun Sparc SLC's

sox -r 16000 -s -w example.raw -r 8000 -b -U example.au

Beware of the byte ordering of your machine - if the above procedure
produces distored rubbish, try using -r 2 instead of -r 1. This just
reverses the byte ordering in the raw binary output file. It is also
worth noting that the above example reduces the quality of the output,
as the sampling rate is being halved and the number of bits per sample
is being halved. Ideally output should be at 16kHz with 16 bits per
sample.


Notes
-----

This code is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

This code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this work; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Bug reports, comments etc. to j.p.iles@cs.bham.ac.uk


References
----------

(1) @article{klatt1980,
     AUTHOR = {Klatt,D.H.},
     JOURNAL = {Journal of the Acoustic Society of America},
     PAGES = {971--995},
     TITLE = {Software for a cascade/parallel formant synthesizer},
     VOLUME = {67},
     NUMBER = {3},
     MONTH = {March},
     YEAR =  1980}


(2) @Article{klatt1990,
  author = 	 "Klatt,D.H. and Klatt, L.C.",
  title = 	 "Analysis, synthesis and perception of voice quality
		  variations among female and male talkers.",
  journal = 	 "Journal of the Acoustical Society of America",
  year = 	 "1990",
  volume = 	 "87",
  number = 	 "2",
  pages = 	 "820--857",
  month = 	 "February"}

(3) Dr. David Williams  at

Sensimetrics Corporation,
64 Sidney Street,
Cambridge,
MA  02139.
Fax: (617) 225-0470
Tel: (617) 225-2442
e-mail sensimetrics@sens.com
*/

/*
  djmw 20081019 first implementation.
  djmw 20081128 Parallel section: rnp filters dif(source)+frication instead of source only.
  djmw 20090708 +Table_to_KlattTable, KlattTable_to_Table
  djmw 20110308 Thing_new + struct KlattFrame -> struct structKlattFrame
  djmw 20110329 Table_get(Numeric|String)Value is now Table_get(Numeric|String)Value_Assert
*/

#include "KlattTable.h"
#include "Resonator.h"

Thing_implement (KlattTable, Table, 0);

#define CASCADE_PARALLEL 1         /* Type of synthesis model */
#define ALL_PARALLEL     2
#define KlattTable_NPAR	40        /* Number of control parameters */
#define MAX_SAM          20000     /* Maximum sample rate */
#define IMPULSIVE        1         /* Type of voicing source */
#define NATURAL          2
#define SAMPLED          3

#define NUMBER_OF_SAMPLES 100
#define SAMPLE_FACTOR 0.00001

/* Structure for Klatt Parameters */
typedef struct structKlattFrame {
	integer F0hz10;	/* Voicing fund freq in units of 0.1 Hz */
	integer AVdb;	/* Amp of voicing in dB, 0 to 70 */
	integer Fhz [9];	/* Formant freqs in Hz */
	integer Bhz [9];	/* bandwidths in Hz */
	integer Bphz [7];	/* Bandwidths, parallel branch */
	integer A [7];	/* Formant amplitudes parallel branch */
	integer FNZhz;	/* Nasal zero freq in Hz,  248 to  528     */
	integer BNZhz;	/* Nasal zero bw in Hz,             40 to 1000 */
	integer FNPhz;	/* Nasal pole freq in Hz,           248 to  528 */
	integer BNPhz;	/* Nasal pole bw in Hz,             40 to 1000 */
	integer ah;	/* Amp of aspiration in dB,         0 to   70 */
	integer Kopen;	/* # of samples in open period,     10 to   65 */
	integer Aturb;	/* Breathiness in voicing,          0 to   80 */
	integer TLTdb;	/* Voicing spectral tilt in dB,     0 to   24 */
	integer AF;	/* Amp of frication in dB,          0 to   80 */
	integer Kskew;	/* Skewness of alternate periods,   0 to   40 in sample#/2 */
	integer ANP;	/* Amp of par nasal pole in dB,     0 to   80 */
	integer AB;	/* Amp of bypass fric. in dB,       0 to   80 */
	integer AVpdb;	/* Amp of voicing,  par in dB,      0 to   70 */
	integer Gain0;	/* Overall gain, 60 dB is unity,    0 to   60 */
} *KlattFrame;

static autoSTRVEC theColumnNames { U"f0", U"av", U"f1", U"b1", U"f2", U"b2", U"f3", U"b3", U"f4", U"b4", U"f5", U"b5", U"f6", U"b6",
	U"fnz", U"bnz", U"fnp", U"bnp", U"ah", U"kopen", U"aturb", U"tilt", U"af", U"skew",
	U"a1", U"b1p", U"a2", U"b2p", U"a3", U"b3p", U"a4", U"b4p", U"a5", U"b5p", U"a6", U"b6p", U"anp", U"ab", U"avp", U"gain" };

static double DBtoLIN (integer dB) {
	static const double amptable [88] = {
		0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 13.0, 14.0,
		16.0, 18.0, 20.0, 22.0, 25.0, 28.0, 32.0, 35.0, 40.0, 45.0,
		51.0, 57.0, 64.0, 71.0, 80.0, 90.0, 101.0, 114.0, 128.0, 142.0,
		159.0, 179.0, 202.0, 227.0, 256.0, 284.0, 318.0, 359.0, 405.0, 455.0,
		512.0, 568.0, 638.0, 719.0, 811.0, 911.0, 1024.0, 1137.0, 1276.0, 1438.0,
		1622.0, 1823.0, 2048.0, 2273.0, 2552.0, 2875.0, 3244.0, 3645.0, 4096.0, 4547.0,
		5104.0, 5751.0, 6488.0, 7291.0, 8192.0, 9093.0, 10207.0, 11502.0, 12976.0, 14582.0,
		16384.0, 18350.0, 20644.0, 23429.0, 26214.0, 29491.0, 32767
	};
	return ( (dB < 0) || (dB > 87) ? 0 : amptable [dB] * .001 );
}

typedef struct structKlattGlobal {
	int synthesis_model; /* cascade-parallel or all-parallel */
	int outsl;       	/* Output waveform selector                      */
	integer samrate;     /* Number of output samples per second           */
	integer FLPhz ;      /* Frequeny of glottal downsample low-pass filter */
	integer BLPhz ;      /* Bandwidth of glottal downsample low-pass filter */
	integer nfcascade;   /* Number of formants in cascade vocal tract    */
	int glsource;    	/* Type of glottal source */
	int f0_flutter;   	/* Percentage of f0 flutter 0-100 */
	integer nspfr;       /* number of samples per frame */
	integer nper;        /* Counter for number of samples in a pitch period */
	integer ns;
	integer T0;          /* Fundamental period in output samples times 4 */
	integer nopen;       /* Number of samples in open phase of period    */
	integer nmod;        /* Position in period to begin noise amp. modul */
	integer Kopen;       /* # of samples in open period,     10 to   65 */
	integer Kskew;       /* Skewness of alternate periods,   0 to   40 in sample#/2 */
	double TLTdb;       /* Voicing spectral tilt in dB,     0 to   24 */
	integer nrand;       /* Varible used by random number generator      */
	double pulse_shape_a;  /* Makes waveshape of glottal pulse when open   */
	double pulse_shape_b;  /* Makes waveshape of glottal pulse when open   */
	double onemd;
	double decay;
	double amp_bypas; /* AB converted to linear gain              */
	double AVdb; /* Amp of voicing in dB, 0 to 70 */
	double amp_voice; /* AVdb converted to linear gain            */
	double par_amp_voice; /* AVpdb converted to linear gain       */
	double amp_aspir; /* AP converted to linear gain              */
	double amp_frica; /* AF converted to linear gain              */
	double Aturb;	/* Breathiness in voicing,          0 to   80 */
	double amp_breth; /* ATURB converted to linear gain           */
	double amp_gain0; /* G0 converted to linear gain              */
	integer num_samples; /* number of glottal samples */
	double sample_factor; /* multiplication factor for glottal samples */
	short *natural_samples; /* pointer to an array of glottal samples */
	integer F0hz10; /* Voicing fund freq in units of 0.1 Hz */
	integer original_f0; /* original value of f0 not modified by flutter (kanweg) */
	autoResonator rp [7], rc [9], rnpp, rnpc, rgl, rlp, rout;
	autoAntiResonator rnz;
} *KlattGlobal;

autoKlattTable KlattTable_readFromRawTextFile (MelderFile fs) {
	try {
		autoMatrix thee = Matrix_readFromRawTextFile (fs);
		Melder_require (thy nx == KlattTable_NPAR,
			U"A KlattTable needs ",  KlattTable_NPAR, U" columns.");

		autoKlattTable me = Thing_new (KlattTable);
		Table_initWithColumnNames (me.get(), thy ny, theColumnNames.get());
		for (integer irow = 1; irow <= thy ny; irow ++) {
			for (integer jcol = 1; jcol <= KlattTable_NPAR; jcol ++) {
				double val = thy z [irow] [jcol];
				if (jcol > 3 && jcol < 13 && (jcol % 2 == 0) && val <= 0) // bw == 0?
					val = thy z [irow] [jcol - 1] / 10;
				Table_setNumericValue (me.get(), irow, jcol, val);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattTable not read from file.");
	}
}

static void KlattGlobal_free (KlattGlobal me) {
	for (integer i = 1; i <= 8; i ++) {
		my rc [i]. reset();
		if (i <= 6)
			my rp [i]. reset();
	}
	my rnpp. reset();
	my rnpc. reset();
	my rnz. reset();
	my rgl. reset();
	my rlp. reset();
	my rout. reset();
	Melder_free (me);
}

static KlattGlobal KlattGlobal_create (double samplingFrequency) {
	KlattGlobal me = nullptr;
	try {
		me = Melder_calloc (structKlattGlobal, 1);

		my samrate = Melder_ifloor (samplingFrequency);
		const double dT = 1.0 / my samrate;

		for (integer i = 1; i <= 8; i ++) {
			my rc [i] = Resonator_create (dT, true);
			if (i <= 6)
				my rp [i] = Resonator_create (dT, true);
		}
		my rnpp = Resonator_create (dT, true);
		my rnpc = Resonator_create (dT, true);
		my rgl = Resonator_create (dT, true);
		my rlp = Resonator_create (dT, true);
		my rout = Resonator_create (dT, true);
		my rnz = AntiResonator_create (dT);
		return me;
	} catch (MelderError) {
		KlattGlobal_free (me);
		Melder_throw (U"KlattGlobal not created.");
	}
}

static void KlattGlobal_init (KlattGlobal me, int synthesisModel, int numberOfFormants, int glottalSource, double frameDuration, integer flutter, int outputType) {
	static short natural_samples [NUMBER_OF_SAMPLES] = {
		-310, -400, 530, 356, 224, 89, 23, -10, -58, -16, 461, 599, 536, 701, 770,
		605, 497, 461, 560, 404, 110, 224, 131, 104, -97, 155, 278, -154, -1165,
		-598, 737, 125, -592, 41, 11, -247, -10, 65, 92, 80, -304, 71, 167, -1, 122,
		233, 161, -43, 278, 479, 485, 407, 266, 650, 134, 80, 236, 68, 260, 269, 179,
		53, 140, 275, 293, 296, 104, 257, 152, 311, 182, 263, 245, 125, 314, 140, 44,
		203, 230, -235, -286, 23, 107, 92, -91, 38, 464, 443, 176, 98, -784, -2449,
		-1891, -1045, -1600, -1462, -1384, -1261, -949, -730
	};

	my nspfr = Melder_ifloor (my samrate * frameDuration); /* average number of samples per frame */
	my synthesis_model = synthesisModel;
	my nfcascade = numberOfFormants;
	my glsource = glottalSource;
	my natural_samples = natural_samples;
	my num_samples = NUMBER_OF_SAMPLES;
	my sample_factor = (float) SAMPLE_FACTOR;
	my outsl = outputType;
	my f0_flutter = flutter;

	my FLPhz = Melder_ifloor (0.0950 * my samrate); // depends on samplingFrequency ????
	my BLPhz = Melder_ifloor (0.0630 * my samrate);
	Filter_setCoefficients (my rlp.get(), my FLPhz, my BLPhz);
}

static KlattFrame KlattFrame_create () {
	return Melder_malloc (structKlattFrame, 1);
}

static void KlattFrame_free (KlattFrame me) {
	Melder_free (me);
}

autoKlattTable KlattTable_create (double frameDuration, double totalDuration) {
	try {
		autoKlattTable me = Thing_new (KlattTable);
		const integer nrows = Melder_ifloor (totalDuration / frameDuration) + 1;
		Table_initWithColumnNames (me.get(), nrows, theColumnNames.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"KlattTable not created.");
	}
}

//static void frame_init(KlattGlobal_ptr globals, KlattFrame_ptr frame)
static void KlattGlobal_getFrame (KlattGlobal me, KlattFrame thee) {
	const double amp_parF [7] = {0, 0.4, 0.15, 0.06, 0.04, 0.022, 0.03};

	my F0hz10 = thy F0hz10;
	my original_f0 = my F0hz10 / 10;
	my Kopen = thy Kopen;
	my Kskew = thy Kskew;
	my TLTdb = thy TLTdb;

	my Aturb = thy Aturb;
	my AVdb = thy AVdb;
	my AVdb -= 7;
	if (my AVdb < 0)
		my AVdb = 0;

	my amp_aspir = DBtoLIN (thy ah) * 0.05;
	my amp_frica = DBtoLIN (thy AF) * 0.25;
	my par_amp_voice = DBtoLIN (thy AVpdb);
	const double amp_parFNP = DBtoLIN (thy ANP) * 0.6;
	my amp_bypas = DBtoLIN (thy AB) * 0.05;
	thy Gain0 -= 3;
	if (thy Gain0 <= 0)
		thy Gain0 = 57;

	my amp_gain0 = DBtoLIN (thy Gain0);
	/*
		Set coefficients of variable cascade resonators
	*/
	for (integer i = 8; i > 0; i--)
		if (my nfcascade >= i)
			Filter_setCoefficients (my rc [i].get(), thy Fhz [i], thy Bhz [i]);
	/*
		Set coefficients of nasal resonator and zero antiresonator
	*/
	Filter_setCoefficients (my rnpc.get(), thy FNPhz, thy BNPhz);
	Filter_setCoefficients (my rnz.get(), thy FNZhz, thy BNZhz);
	/*
		Set coefficients of parallel resonators, and amplitude of outputs
	*/
	for (integer i = 1; i <= 6; i ++) {
		Filter_setCoefficients (my rp [i].get(), thy Fhz [i], thy Bphz [i]);
		my rp [i] -> a *= amp_parF [i] * DBtoLIN (thy A [i]);
	}

	Filter_setCoefficients (my rnpp.get(), thy FNPhz, thy BNPhz);
	my rnpp -> a *= amp_parFNP;
	/*
		Output low-pass filter
	*/
	Filter_setCoefficients (my rout.get(), 0, (integer) (my samrate / 2));
}

/*
	This function adds F0 flutter, as specified in:

	"Analysis, synthesis and perception of voice quality variations among
	female and male talkers", D.H. Klatt and L.C. Klatt, JASA 87(2), February 1990.

	Flutter is added by applying a quasi-random element constructed from three
	slowly varying sine waves.
*/

static void KlattFrame_flutter (KlattGlobal me) {
	static integer time_count = 0;
	const double fla = (double) my f0_flutter / 50.0;
	const double flb = (double) my original_f0 / 100.0;
	const double flc = sin (2 * NUMpi * 12.7 * time_count);
	const double fld = sin (2 * NUMpi * 7.1 * time_count);
	const double fle = sin (2 * NUMpi * 4.7 * time_count);
	const double delta_f0 =  fla * flb * (flc + fld + fle) * 10.0;
	my F0hz10 += (integer) delta_f0;
	time_count ++;
}

/*
	Random number generator (return a number between -8191 and +8191)
	Noise spectrum is tilted down by soft low-pass filter having a pole near
	the origin in the Z-plane, i.e. output = input + (0.75 * lastoutput)
*/
static double KlattGlobal_gen_noise (KlattGlobal me) {
	static double nlast = 0.0;

	my nrand = ( (rand() % (int) ( ( (8191) + 1) - (-8191))) + (-8191));
	double noise = my nrand + (0.75 * nlast);
	nlast = noise;

	return noise;
}

/*
Generate a low pass filtered train of impulses as an approximation of
a natural excitation waveform. Low-pass filter the differentiated impulse
with a critically-damped second-order filter, time constant proportional
to Kopen.
*/
static double KlattGlobal_impulsive_source (KlattGlobal me) {   // ppgb: dit was een float; kan niet goed zijn
	constexpr static double doublet [] = {0.0, 13000000.0, -13000000.0};
	static double vwave = ( my nper < 3 ? doublet [my nper] : 0.0 );

	return Filter_getOutput (my rgl.get(), vwave);
}

/*
Vwave is the differentiated glottal flow waveform, there is a weak
spectral zero around 800 Hz, magic constants a,b reset pitch synchronously.
*/
static double KlattGlobal_natural_source (KlattGlobal me) {   // ppgb: dit was een float; kan niet goed zijn
	static double vwave = 0.0;
	double lgtemp = 0.0;

	if (my nper < my nopen) {
		my pulse_shape_a -= my pulse_shape_b;
		vwave += my pulse_shape_a;
		lgtemp = vwave * 0.028;
	} else {
		vwave = 0.0;
	}
	return lgtemp;
}

/* Allows the use of a glottal excitation waveform sampled from a real voice. */
static double KlattGlobal_sampled_source (KlattGlobal me) {   // ppgb: dit was een float; kan niet goed zijn
	double result = 0.0;

	if (my T0 != 0) {
		double ftemp = my nper;
		ftemp *= my num_samples / my T0;
		const integer itemp = Melder_ifloor (ftemp);

		const double temp_diff = ftemp - itemp;

		const integer current_value = my natural_samples [itemp];
		const integer  next_value = my natural_samples [itemp + 1];

		double diff_value = next_value - current_value;
		diff_value = diff_value * temp_diff;

		result = my natural_samples [itemp] + diff_value;
		result = result * my sample_factor;
	}

	return result;
}

/*
  Reset selected parameters pitch-synchronously.


  Constant B0 controls shape of glottal pulse as a function
	of desired duration of open phase N0
	(Note that N0 is specified in terms of 40,000 samples/sec of speech)

  Assume voicing waveform V(t) has form: k1 t**2 - k2 t**3

  If the radiation characterivative, a temporal derivative
  is folded in, and we go from continuous time to discrete
  integers n:  dV/dt = vwave [n]
                        = sum over i=1,2,...,n of { a - (i * b) }
                        = a n  -  b/2 n**2

  where the  constants a and b control the detailed shape
  and amplitude of the voicing waveform over the open
  portion of the voicing cycle "nopen".

  Let integral of dV/dt have no net dc flow --> a = (b * nopen) / 3

  Let maximum of dUg(n)/dn be constant --> b = gain / (nopen * nopen)
  meaning as nopen gets bigger, V has bigger peak proportional to n

  Thus, to generate the table below for 40 <= nopen <= 263:

  B0 [nopen - 40] = 1920000 / (nopen * nopen)

  Modified calculation by djmw 20081127
  Given a and b as above (which are wrong, see below) V' [n]= b*N/3*n - b/2*n^2.
  V' [N]=b/3*N^2-b/2*N^2=b/6*N^2
  Given the table B0 for N=40, b=1200 and V' [N]=1200/6*(40)^2=320000
  Also b=G/N^2 then V' [n]=-G/6 (=1920000/6=320000)!


  (We have not implemented the following correct calculations in the code) N=nopen
  dV/dt = vwave [n]=sum(i=1, n, a-i*b) = a*n-b*1/2*n*(n+1)=(a-b/2)*n-b/2 * n^2
  (Here they forgot that sum(i=1,N,i)=1/2*n*(n+1) ! )
  We want the sum(i=1,N, dV/dt [n])==0, therefore (a-b/2)*1/2*N*(N+1)-b*1/2*1/6*N*(N+1)*(2*N+1)==0.
  It follows that (a-b/2) - b*1/6*(2*N+1)==0 => a = b*(N+2)/3
  we can rewrite with only b: V' [n] = b/2 {(2N+1)/3*n - n^2}
  The maximum of V' [n] is where (2N+1)/3-2n==0 => for n=(2N+1)/6
  This maximum is b/2{(2N+1)/3*(2N+1)/(2*3)-((2N+1)/(2*3))^2} = 1/72*b*(2N+1)^2
  The minimum is at n=N and equals V' [N]=b/6 {N-N^2}.
  This minimum has larger amplitude than the maximum.
  b = 6*gain/(N^2-N), b approx 6*gain/N^2. With a maximum gain of 32767 we arrive at
  b= 196602 / (N^2-N) Their value is 20*log10(1920000/196602) = 19.79 dB too high!
  The noise is in the range [-8192,8192],
*/

static void KlattGlobal_pitch_synch_par_reset (KlattGlobal me) {
	static integer skew;
	const static short B0 [224] = {
		1200, 1142, 1088, 1038, 991, 948, 907, 869, 833, 799, 768, 738, 710, 683, 658,
		634, 612, 590, 570, 551, 533, 515, 499, 483, 468, 454, 440, 427, 415, 403,
		391, 380, 370, 360, 350, 341, 332, 323, 315, 307, 300, 292, 285, 278, 272,
		265, 259, 253, 247, 242, 237, 231, 226, 221, 217, 212, 208, 204, 199, 195,
		192, 188, 184, 180, 177, 174, 170, 167, 164, 161, 158, 155, 153, 150, 147,
		145, 142, 140, 137, 135, 133, 131, 128, 126, 124, 122, 120, 119, 117, 115,
		113, 111, 110, 108, 106, 105, 103, 102, 100, 99, 97, 96, 95, 93, 92, 91, 90,
		88, 87, 86, 85, 84, 83, 82, 80, 79, 78, 77, 76, 75, 75, 74, 73, 72, 71,
		70, 69, 68, 68, 67, 66, 65, 64, 64, 63, 62, 61, 61, 60, 59, 59, 58, 57,
		57, 56, 56, 55, 55, 54, 54, 53, 53, 52, 52, 51, 51, 50, 50, 49, 49, 48, 48,
		47, 47, 46, 46, 45, 45, 44, 44, 43, 43, 42, 42, 41, 41, 41, 41, 40, 40,
		39, 39, 38, 38, 38, 38, 37, 37, 36, 36, 36, 36, 35, 35, 35, 35, 34, 34, 33,
		33, 33, 33, 32, 32, 32, 32, 31, 31, 31, 31, 30, 30, 30, 30, 29, 29, 29, 29,
		28, 28, 28, 28, 27, 27
	};

	if (my F0hz10 > 0) {
		/*
			T0 is 4* the number of samples in one pitch period
		*/
		my T0 = (40 * my samrate) / my F0hz10;
		my amp_voice = DBtoLIN (Melder_ifloor (my AVdb));
		/*
			Duration of period before amplitude modulation
		*/
		my nmod = my T0;
		if (my AVdb > 0)
			my nmod >>= 1;
		/*
			Breathiness of voicing waveform
		*/
		my amp_breth = DBtoLIN (Melder_ifloor (my Aturb)) * 0.1;
		/*
			Set open phase of glottal period where  40 <= open phase <= 263
		*/
		my nopen = 4 * my Kopen;

		if ( (my glsource == IMPULSIVE) && (my nopen > 263))
			my nopen = 263;

		if (my nopen >= (my T0 - 1)) {
			my nopen = my T0 - 2;
			Melder_warning (U"Glottal open period cannot exceed T0, truncated");
		}

		if (my nopen < 40) {
			/* F0 max = 1000 Hz */
			my nopen = 40;
			Melder_warning (U"Warning: minimum glottal open period is 10 samples.\n"
			                U"truncated, nopen = ", my nopen);
		}
		/*
			Reset a & b, which determine shape of "natural" glottal waveform
		*/
		my pulse_shape_b = B0 [my nopen - 40];
		my pulse_shape_a = (my pulse_shape_b * my nopen) * 0.333;
		/*
			Reset width of "impulsive" glottal pulse
		*/
		integer temp = my samrate / my nopen;

		Filter_setCoefficients (my rgl.get(), 0, temp); // Only used for impulsive source.
		/*
			Make gain at F1 about constant
		*/
		const double temp1 = my nopen * 0.00833;
		my rgl -> a *= temp1 * temp1;
		/*
			Truncate skewness so as not to exceed duration of closed phase of glottal period.
		*/
		temp = my T0 - my nopen;
		if (my Kskew > temp) {
			Melder_information (U"Kskew duration=", my Kskew, U" > glottal closed period=",
				my T0 - my nopen, U" truncate");
			my Kskew = temp;
		}
		if (skew >= 0)
			skew = my Kskew;
		else
			skew = - my Kskew;
		/*
			Add skewness to closed portion of voicing period
		*/
		my T0 = my T0 + skew;
		skew = - skew;
	} else {
		my T0 = 4;                     // Default for f0 undefined
		my amp_voice = 0.0;
		my nmod = my T0;
		my amp_breth = 0.0;
		my pulse_shape_a = 0.0;
		my pulse_shape_b = 0.0;
	}
	/*
		Reset these pars pitch synchronously or at update rate if f0=0
	*/
	if ( (my T0 != 4) || (my ns == 0)) {
		/*
			Set one-pole low-pass filter that tilts glottal source
		*/
		my decay = (0.033 * my TLTdb);
		my onemd = ( my decay > 0.0 ? 1.0 - my decay : 1.0 );
	}
}

// This is Klatt80 with improved source model.
static void KlattGlobal_synthesizeFrame (KlattGlobal me, vector<int16> const& output) {
	double out, frics, glotout, aspiration, par_glotout, noise, sourc, voice = 0;
	static double vlast = 0, glotlast = 0;

	KlattFrame_flutter (me);
	/*
		MAIN LOOP, for each output sample of current frame:
	*/
	for (integer isamp = 1; isamp <= my nspfr; isamp ++) {
		/*
			Get low-passed random number for aspiration and frication noise
		*/
		noise = KlattGlobal_gen_noise (me);
		/*
			Amplitude modulate noise (reduce noise amplitude during
			second half of glottal period) if voicing simultaneously present.
		*/
		if (my nper > my nmod)
			noise *= 0.5;
		/*
			Compute frication noise
		*/
		frics = my amp_frica * noise;
		/*
			Compute voicing waveform. Run glottal source simulation at 4
			times normal sample rate to minimize quantization noise in
			period of female voice.
		*/
		for (integer n4 = 0; n4 < 4; n4 ++) {
			switch (my glsource) {
				case IMPULSIVE:
					voice = KlattGlobal_impulsive_source (me);
					break;
				case NATURAL:
					voice = KlattGlobal_natural_source (me);
					break;
				case SAMPLED:
					voice = KlattGlobal_sampled_source (me);
					break;
			}
			/*
				Reset period when counter 'nper' reaches T0
			*/
			if (my nper >= my T0) {
				my nper = 0;
				KlattGlobal_pitch_synch_par_reset (me);
			}
			/*
				Low-pass filter voicing waveform before downsampling from 4*samrate
				to samrate samples/sec.  Resonator f=.09*samrate, bw=.06*samrate
			*/
			voice = Filter_getOutput (my rlp.get(), voice);
			/*
				Increment counter that keeps track of 4*samrate samples per sec
			*/
			my nper ++;
		}
		/*
			Tilt spectrum of voicing source down by soft low-pass filtering, amount
			of tilt determined by TLTdb
		*/
		voice = (voice * my onemd) + (vlast * my decay);
		vlast = voice;
		/*
			Add breathiness during glottal open phase. Amount of breathiness
			determined by parameter Aturb Use nrand rather than noise because
			noise is low-passed.
		*/
		if (my nper < my nopen)
			voice += my amp_breth * my nrand;
		/*
			Set voicing amplitude
		*/
		glotout = my amp_voice * voice;
		par_glotout = my par_amp_voice * voice;
		/*
			Compute aspiration amplitude and add to voicing source
		*/
		aspiration = my amp_aspir * noise;
		glotout += aspiration;
		par_glotout += aspiration;
		/*
			Cascade vocal tract, excited by laryngeal sources.
			Nasal antiresonator, then formants FNP, F5, F4, F3, F2, F1
		*/
		if (my synthesis_model != ALL_PARALLEL) {
			out = Filter_getOutput (my rnz.get(), glotout); /* anti resonator */
			out = Filter_getOutput (my rnpc.get(), out);

			for (integer i = 8; i > 0; i--)
				if (my nfcascade >= i)
					out = Filter_getOutput (my rc [i].get(), out);
		} else {
			/*
				we are not using the cascade tract, set out to zero
			*/
			out = 0;
		}
		/*
			Excite parallel F1 and FNP by voicing waveform
		*/
		sourc = par_glotout;        // Source is voicing plus aspiration
		/*
			Standard parallel vocal tract Formants F6,F5,F4,F3,F2,
			outputs added with alternating sign. Sound sourc for other
			parallel resonators is frication plus first difference of
			voicing waveform.

			In Klatt80:
				source: through r1,
				diff(source)+frication: through filters rnp, r2, r3, r4
				frication: through r5, r6 and ab.
			In the original code of Iles and Ing it was
				source: through r1 and rnp
				diff(source)+frication: r2, r3, r4 , r5, r6, ab

			Problem: The source signal is already v' [n], and we are differentiating here again ???
		*/
		out += Filter_getOutput (my rp [1].get(), sourc);
		sourc = frics + par_glotout - glotlast; // diff
		glotlast = par_glotout;

		out += Filter_getOutput (my rnpp.get(), sourc);

		for (integer i = 6; i >= 2; i--)
			if (my nfcascade >= i)
				out = Filter_getOutput (my rp [i].get(), sourc) - out;

		const double outbypas = my amp_bypas * sourc;
		out = outbypas - out;

		if (my outsl != 0) {
			switch (my outsl) {
				case 1:
					out = voice;
					break;
				case 2:
					out = aspiration;
					break;
				case 3:
					out = frics;
					break;
				case 4:
					out = glotout;
					break;
				case 5:
					out = par_glotout;
					break;
				case 6:
					out = outbypas;
					break;
				case 7:
					out = sourc;
					break;
			}
		}

		out = Filter_getOutput (my rout.get(), out);
		double temp = out * my amp_gain0;  // Convert back to integer

		if (temp < -32768.0) {
			temp = -32768.0;
		}
		if (temp >	32767.0) {
			temp =  32767.0;
		}
		output [isamp] = temp;   // ppgb: truncatie naar 0, dus compressie; is dat de bedoeling?
	}
	my ns = my nspfr;
}

static int KlattTable_checkLimits (KlattTable me) {
	integer nviolations_upper [KlattTable_NPAR + 1] = { 0 }, nviolations_lower [KlattTable_NPAR + 1] = { 0 };
	const integer lower [KlattTable_NPAR + 1] = { 0, // dummy
		10, 0,  // f0, av
		200, 40, 550, 40, 1200, 40, 1200, 40, 1200, 40, 1200, 40, // f1,b1 -- f6,b6
		248, 40, 248, 40, // fnz, bnz, fnp, bnp
		0, 0, 0, 0, 0, 0, // ah, kopen, aturb, tilt, af, skew
		0, 40, 0, 40, 0, 40, 0, 40, 0, 40, 0, 40, // a1,b1p -- a6,b6p
		0, 0, 0, 0 // anp, ab, avp, gain
	};
	const integer upper [KlattTable_NPAR + 1] = { 0, // dummy
		10000, 70,   // f0, av
		1300, 1000, 3000, 1000, 4999, 1000, 4999, 1000, 6999, 1000, 7000, 1000,  // f1,b1 -- f6,b6
		528, 1000, 528, 1000, // fnz, bnz, fnp, bnp
		70, 60, 80, 24, 80, 40,  // ah, kopen, aturb, tilt, af, skew
		80, 1000, 80, 1000, 80, 1000, 80, 1000, 80, 1000, 80, 1000, // a1,b1p -- a6,b6p
		80, 80, 70, 80  // anp, ab, avp, gain
	};

	integer nv = 0;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		for (integer j = 1; j <= KlattTable_NPAR; j ++) {
			const integer val = Table_getNumericValue_a (me, irow, j);   // ppgb: truncatie? kan dat kloppen?
			if (val < lower [j]) {
				nviolations_lower [j] ++;
				nv ++;
			} else if (val > upper [j]) {
				nviolations_upper [j] ++;
				nv ++;
			}
		}
	}
	if (nv > 0) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"Diagnostics for KlattTable \"", Thing_getName (me), U"\":");
		MelderInfo_writeLine (U"Number of frames: ", my rows.size);
		for (integer j = 1; j <= KlattTable_NPAR; j ++) {
			if (nviolations_lower [j] > 0) {
				if (nviolations_upper [j] > 0)
					MelderInfo_writeLine (theColumnNames [j].get(), U": ", nviolations_lower [j], U" frame(s) < min = ",
						nviolations_lower [j], U"; ", nviolations_upper [j], U" frame(s) > max = ", upper [j]);
				else
					MelderInfo_writeLine (theColumnNames [j].get(), U": ", nviolations_lower [j], U" frame(s) < min = ", lower [j]);
			} else if (nviolations_upper [j] > 0) {
				MelderInfo_writeLine (theColumnNames [j].get(), U": ", nviolations_upper [j], U" frame(s) > max = ", upper [j]);
			}
		}
		MelderInfo_close ();
		return 0;
	}
	return 1;
}

autoSound KlattTable_to_Sound (KlattTable me, double samplingFrequency, int synthesisModel, int numberOfFormants, double frameDuration, int glottalSource, double flutter, int outputType) {
	KlattGlobal thee = 0;
	KlattFrame frame = 0;
	try {
		integer numberOfSamples = 1, par [KlattTable_NPAR + 1];

		if (! KlattTable_checkLimits (me))
			Melder_warning (U"Some values in the KlattTable are outside the limits; the resulting sound may sound weird.");
		thee = KlattGlobal_create (samplingFrequency);
		frame = KlattFrame_create ();
		autovector <short> iwave = newvectorzero<short> (MAX_SAM);
		thy samrate = Melder_ifloor (samplingFrequency);

		KlattGlobal_init (thee, synthesisModel, numberOfFormants, glottalSource, frameDuration, Melder_ifloor (flutter), outputType);

		autoSound him = Sound_createSimple (1, frameDuration * my rows.size, samplingFrequency);

		for (integer irow = 1 ; irow <= my rows.size; irow ++) {
			for (integer col = 1; col <= KlattTable_NPAR; col ++)
				par [col] = Table_getNumericValue_a (me, irow, col);   // ppgb: truncatie?
			integer jcol = 1;
			frame -> F0hz10 = par [jcol ++];
			frame -> AVdb = par [jcol ++];
			frame -> Fhz [1] = par [jcol ++];
			frame -> Bhz [1] = par [jcol ++];
			frame -> Fhz [2] = par [jcol ++];
			frame -> Bhz [2] = par [jcol ++];
			frame -> Fhz [3] = par [jcol ++];
			frame -> Bhz [3] = par [jcol ++];
			frame -> Fhz [4] = par [jcol ++];
			frame -> Bhz [4] = par [jcol ++];
			frame -> Fhz [5] = par [jcol ++];
			frame -> Bhz [5] = par [jcol ++];
			frame -> Fhz [6] = par [jcol ++];
			frame -> Bhz [6] = par [jcol ++];
			frame -> FNZhz = par [jcol ++];
			frame -> BNZhz = par [jcol ++];
			frame -> FNPhz = par [jcol ++];
			frame -> BNPhz = par [jcol ++];
			frame -> ah = par [jcol ++];
			frame -> Kopen = par [jcol ++];
			frame -> Aturb = par [jcol ++];
			frame -> TLTdb = par [jcol ++];
			frame -> AF = par [jcol ++];
			frame -> Kskew = par [jcol ++];
			frame -> A [1] = par [jcol ++];
			frame -> Bphz [1] = par [jcol ++];
			frame -> A [2] = par [jcol ++];
			frame -> Bphz [2] = par [jcol ++];
			frame -> A [3] = par [jcol ++];
			frame -> Bphz [3] = par [jcol ++];
			frame -> A [4] = par [jcol ++];
			frame -> Bphz [4] = par [jcol ++];
			frame -> A [5] = par [jcol ++];
			frame -> Bphz [5] = par [jcol ++];
			frame -> A [6] = par [jcol ++];
			frame -> Bphz [6] = par [jcol ++];
			frame -> ANP = par [jcol ++];
			frame -> AB = par [jcol ++];
			frame -> AVpdb = par [jcol ++];
			frame -> Gain0 = par [jcol ++];;
			frame -> Fhz [7] = 6500;
			frame -> Bhz [7] = 600;
			frame -> Fhz [8] = 7500;
			frame -> Bhz [8] = 600;

			KlattGlobal_getFrame (thee, frame);

			KlattGlobal_synthesizeFrame (thee, iwave.get());

			for (integer isam = 1; isam <= thy nspfr; isam ++)
				his z [1] [numberOfSamples ++] = iwave [isam] / 32768.0;
		}
		KlattGlobal_free (thee);
		KlattFrame_free (frame);
		return him;
	} catch (MelderError) {
		KlattGlobal_free (thee);
		KlattFrame_free (frame);
		Melder_throw (me, U": no Sound created.");
	}
}

autoKlattTable KlattTable_createExample () {
	const integer nrows = 1376;
	const struct klatt_params {
		short p [40];
	} klatt_data [1376] = {
		{{ 1000, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1000, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1010, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1020, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1030, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1040, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1050, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1060, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1070, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1080, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1090, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1100, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1110, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1120, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1130, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1140, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1150, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1160, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1170, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1180, 0, 542, 0, 1372, 0, 2634, 0, 3737, 0, 5740, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 52, 0, 56, 0, 71, 0, 66, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1190, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 28, 60 }},
		{{ 1200, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 31, 60 }},
		{{ 1207, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 34, 60 }},
		{{ 1214, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 36, 60 }},
		{{ 1221, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 39, 60 }},
		{{ 1228, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 42, 60 }},
		{{ 1234, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 42, 60 }},
		{{ 1241, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 42, 60 }},
		{{ 1248, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 42, 60 }},
		{{ 1255, 0, 506, 0, 752, 0, 3029, 0, 3545, 0, 6301, 0, 7523, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 41, 73, 79, 56, 108, 53, 65, 43, 80, 37, 80, 0, 0, 42, 60 }},
		{{ 1262, 0, 510, 0, 765, 0, 3006, 0, 3547, 0, 6283, 0, 7484, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 42, 72, 78, 56, 106, 53, 65, 43, 80, 37, 80, 0, 0, 42, 60 }},
		{{ 1269, 0, 518, 0, 792, 0, 2961, 0, 3552, 0, 6248, 0, 7407, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 38, 43, 71, 75, 56, 102, 53, 64, 43, 80, 37, 80, 0, 0, 44, 60 }},
		{{ 1276, 0, 530, 0, 833, 0, 2899, 0, 3560, 0, 6197, 0, 7302, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 45, 70, 71, 56, 98, 52, 63, 43, 80, 38, 80, 0, 0, 45, 60 }},
		{{ 1282, 0, 543, 0, 892, 0, 2825, 0, 3572, 0, 6132, 0, 7176, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 41, 47, 67, 66, 55, 92, 51, 62, 43, 80, 38, 80, 0, 0, 46, 60 }},
		{{ 1289, 0, 556, 0, 969, 0, 2749, 0, 3592, 0, 6055, 0, 7046, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 42, 50, 65, 62, 55, 86, 50, 61, 43, 80, 39, 80, 0, 0, 48, 60 }},
		{{ 1296, 0, 567, 0, 1068, 0, 2680, 0, 3622, 0, 5969, 0, 6926, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 43, 52, 62, 58, 55, 81, 50, 61, 44, 80, 39, 80, 0, 0, 50, 60 }},
		{{ 1303, 0, 573, 0, 1156, 0, 2642, 0, 3651, 0, 5903, 0, 6858, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 54, 61, 57, 56, 78, 50, 63, 44, 80, 39, 80, 0, 0, 51, 60 }},
		{{ 1310, 0, 576, 0, 1230, 0, 2624, 0, 3677, 0, 5854, 0, 6823, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 59, 56, 56, 76, 50, 64, 44, 80, 39, 80, 0, 0, 52, 60 }},
		{{ 1322, 0, 576, 0, 1287, 0, 2617, 0, 3699, 0, 5819, 0, 6809, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 59, 57, 56, 76, 50, 66, 44, 80, 39, 80, 0, 0, 54, 60 }},
		{{ 1335, 0, 576, 0, 1326, 0, 2616, 0, 3714, 0, 5797, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 58, 57, 57, 75, 50, 67, 44, 80, 39, 80, 0, 0, 56, 60 }},
		{{ 1348, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 57, 60 }},
		{{ 1360, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 58, 60 }},
		{{ 1372, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1385, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 1, 0, 60, 60 }},
		{{ 1398, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 3, 0, 59, 60 }},
		{{ 1410, 0, 585, 0, 1342, 0, 2619, 0, 3715, 0, 5810, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 59, 58, 57, 79, 50, 68, 44, 80, 39, 80, 4, 0, 58, 60 }},
		{{ 1422, 0, 603, 0, 1337, 0, 2628, 0, 3706, 0, 5849, 0, 6803, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 60, 61, 58, 85, 51, 67, 44, 80, 40, 80, 6, 0, 56, 60 }},
		{{ 1435, 0, 624, 0, 1341, 0, 2649, 0, 3705, 0, 5883, 0, 6805, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 61, 65, 60, 95, 52, 65, 45, 80, 40, 80, 9, 0, 54, 60 }},
		{{ 1448, 0, 643, 0, 1367, 0, 2692, 0, 3727, 0, 5885, 0, 6814, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 62, 72, 62, 110, 54, 61, 47, 80, 39, 80, 12, 0, 51, 60 }},
		{{ 1460, 0, 650, 0, 1417, 0, 2754, 0, 3776, 0, 5837, 0, 6833, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 80, 65, 125, 56, 56, 49, 80, 39, 80, 16, 0, 49, 60 }},
		{{ 1460, 0, 649, 0, 1470, 0, 2811, 0, 3830, 0, 5769, 0, 6852, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 87, 68, 137, 58, 51, 52, 80, 38, 80, 19, 0, 47, 60 }},
		{{ 1460, 0, 644, 0, 1513, 0, 2855, 0, 3874, 0, 5707, 0, 6867, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 92, 70, 145, 60, 47, 54, 80, 37, 80, 23, 0, 46, 60 }},
		{{ 1460, 0, 641, 0, 1537, 0, 2879, 0, 3899, 0, 5672, 0, 6876, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 94, 71, 149, 60, 45, 55, 80, 37, 80, 26, 0, 45, 60 }},
		{{ 1460, 0, 641, 0, 1537, 0, 2879, 0, 3899, 0, 5672, 0, 6876, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 94, 71, 149, 60, 45, 55, 80, 37, 80, 29, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 31, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 32, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 34, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 640, 0, 1540, 0, 2882, 0, 3903, 0, 5668, 0, 6876, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 94, 71, 149, 61, 45, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 641, 0, 1532, 0, 2876, 0, 3899, 0, 5677, 0, 6872, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 94, 70, 148, 60, 45, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 642, 0, 1522, 0, 2868, 0, 3894, 0, 5691, 0, 6867, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 92, 70, 146, 60, 46, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 643, 0, 1509, 0, 2856, 0, 3886, 0, 5709, 0, 6861, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 91, 69, 143, 60, 47, 54, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 644, 0, 1493, 0, 2843, 0, 3877, 0, 5730, 0, 6855, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 89, 69, 140, 59, 48, 53, 80, 38, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 644, 0, 1477, 0, 2827, 0, 3866, 0, 5753, 0, 6849, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 87, 68, 136, 59, 50, 52, 80, 38, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 643, 0, 1461, 0, 2809, 0, 3853, 0, 5779, 0, 6846, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 84, 67, 132, 58, 51, 51, 80, 38, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 641, 0, 1447, 0, 2789, 0, 3839, 0, 5804, 0, 6846, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 62, 81, 66, 127, 57, 53, 50, 80, 39, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 636, 0, 1435, 0, 2769, 0, 3824, 0, 5829, 0, 6850, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 62, 79, 65, 121, 57, 55, 49, 80, 39, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 629, 0, 1428, 0, 2747, 0, 3808, 0, 5852, 0, 6860, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 61, 76, 64, 116, 56, 57, 48, 80, 39, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 618, 0, 1428, 0, 2725, 0, 3791, 0, 5870, 0, 6879, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 61, 73, 63, 110, 55, 58, 47, 80, 39, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 603, 0, 1436, 0, 2703, 0, 3773, 0, 5884, 0, 6908, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 60, 70, 62, 103, 55, 60, 47, 80, 39, 80, 35, 0, 45, 60 }},
		{{ 1460, 0, 583, 0, 1454, 0, 2682, 0, 3755, 0, 5890, 0, 6950, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 561, 0, 1481, 0, 2665, 0, 3740, 0, 5888, 0, 6997, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 538, 0, 1513, 0, 2651, 0, 3726, 0, 5879, 0, 7049, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 514, 0, 1548, 0, 2640, 0, 3715, 0, 5866, 0, 7102, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 491, 0, 1586, 0, 2632, 0, 3705, 0, 5849, 0, 7155, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 468, 0, 1623, 0, 2625, 0, 3697, 0, 5831, 0, 7206, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 448, 0, 1659, 0, 2620, 0, 3690, 0, 5813, 0, 7254, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 429, 0, 1692, 0, 2617, 0, 3684, 0, 5795, 0, 7298, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 413, 0, 1722, 0, 2614, 0, 3680, 0, 5778, 0, 7336, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 399, 0, 1746, 0, 2612, 0, 3676, 0, 5764, 0, 7368, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 389, 0, 1765, 0, 2611, 0, 3674, 0, 5753, 0, 7392, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 382, 0, 1779, 0, 2610, 0, 3672, 0, 5746, 0, 7409, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 46, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 35, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 43, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 56, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 58, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1460, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 72, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 0, 60 }},
		{{ 1460, 0, 304, 0, 1265, 0, 2343, 0, 3343, 0, 6085, 0, 7150, 0, 0, 0, 200, 30, 72, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 0, 60 }},
		{{ 1460, 0, 384, 0, 1107, 0, 2418, 0, 3392, 0, 6038, 0, 7106, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 0, 60 }},
		{{ 1460, 0, 491, 0, 864, 0, 2707, 0, 3511, 0, 6110, 0, 7147, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 0, 60 }},
		{{ 1475, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 0, 60 }},
		{{ 1490, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 0, 60 }},
		{{ 1505, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 58, 60 }},
		{{ 1520, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 58, 60 }},
		{{ 1519, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 59, 60 }},
		{{ 1517, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1516, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1514, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1513, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1511, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 1, 0, 60, 60 }},
		{{ 1510, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 2, 0, 60, 60 }},
		{{ 1509, 0, 518, 0, 779, 0, 2921, 0, 3582, 0, 6230, 0, 7231, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 48, 68, 63, 53, 87, 50, 61, 42, 80, 38, 80, 5, 0, 59, 60 }},
		{{ 1507, 0, 540, 0, 841, 0, 2890, 0, 3588, 0, 6186, 0, 7205, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 47, 68, 66, 55, 93, 51, 62, 43, 80, 38, 80, 8, 0, 56, 60 }},
		{{ 1506, 0, 578, 0, 984, 0, 2848, 0, 3623, 0, 6083, 0, 7144, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 42, 68, 73, 58, 105, 53, 62, 45, 80, 39, 80, 12, 0, 53, 60 }},
		{{ 1504, 0, 619, 0, 1239, 0, 2836, 0, 3731, 0, 5894, 0, 7026, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 28, 66, 84, 64, 126, 57, 57, 49, 80, 38, 80, 17, 0, 49, 60 }},
		{{ 1503, 0, 635, 0, 1436, 0, 2862, 0, 3840, 0, 5745, 0, 6931, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 13, 64, 91, 68, 142, 59, 49, 53, 80, 37, 80, 22, 0, 47, 60 }},
		{{ 1501, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 27, 0, 45, 60 }},
		{{ 1500, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 30, 0, 45, 60 }},
		{{ 1497, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 32, 0, 45, 60 }},
		{{ 1493, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 34, 0, 45, 60 }},
		{{ 1490, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1487, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1483, 0, 642, 0, 1530, 0, 2871, 0, 3891, 0, 5683, 0, 6873, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 93, 70, 148, 60, 46, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1480, 0, 645, 0, 1504, 0, 2845, 0, 3864, 0, 5722, 0, 6864, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 91, 69, 143, 59, 48, 54, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1477, 0, 649, 0, 1468, 0, 2809, 0, 3827, 0, 5772, 0, 6851, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 86, 68, 136, 58, 51, 52, 80, 38, 80, 35, 0, 46, 60 }},
		{{ 1473, 0, 650, 0, 1427, 0, 2765, 0, 3786, 0, 5825, 0, 6836, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 81, 66, 127, 57, 55, 50, 80, 39, 80, 35, 0, 49, 60 }},
		{{ 1470, 0, 647, 0, 1387, 0, 2718, 0, 3746, 0, 5870, 0, 6822, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 75, 63, 117, 55, 59, 48, 80, 39, 80, 35, 0, 52, 60 }},
		{{ 1467, 0, 637, 0, 1354, 0, 2673, 0, 3715, 0, 5890, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 62, 69, 61, 104, 53, 63, 46, 80, 40, 80, 35, 0, 56, 60 }},
		{{ 1463, 0, 622, 0, 1339, 0, 2646, 0, 3704, 0, 5880, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 52, 61, 65, 59, 94, 52, 66, 45, 80, 40, 80, 0, 0, 58, 60 }},
		{{ 1460, 0, 606, 0, 1337, 0, 2630, 0, 3705, 0, 5854, 0, 6803, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 54, 60, 61, 58, 86, 51, 67, 44, 80, 40, 80, 0, 0, 60, 60 }},
		{{ 1455, 0, 592, 0, 1339, 0, 2622, 0, 3711, 0, 5825, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 59, 59, 57, 81, 51, 67, 44, 80, 40, 80, 0, 0, 60, 60 }},
		{{ 1450, 0, 581, 0, 1344, 0, 2618, 0, 3718, 0, 5800, 0, 6805, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 58, 57, 77, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1445, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1440, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1435, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1430, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1425, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1420, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1415, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 54, 60 }},
		{{ 1410, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 54, 60 }},
		{{ 1405, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1400, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1383, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1367, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1350, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1333, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1317, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1283, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1267, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1250, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1233, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 56, 0, 57, 0, 75, 0, 68, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1217, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 64, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 65, 100, 75, 100, 65, 100, 0, 0, 0, 60 }},
		{{ 1200, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 64, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 65, 100, 75, 100, 65, 100, 0, 0, 0, 60 }},
		{{ 1150, 0, 517, 0, 2038, 0, 2845, 0, 3850, 0, 5703, 0, 7608, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1100, 0, 528, 0, 1991, 0, 2822, 0, 3840, 0, 5691, 0, 7551, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1090, 0, 543, 0, 1921, 0, 2791, 0, 3827, 0, 5674, 0, 7470, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1080, 0, 562, 0, 1831, 0, 2757, 0, 3812, 0, 5655, 0, 7370, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1070, 0, 583, 0, 1720, 0, 2724, 0, 3796, 0, 5637, 0, 7260, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1060, 0, 605, 0, 1592, 0, 2701, 0, 3782, 0, 5621, 0, 7149, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1050, 0, 624, 0, 1450, 0, 2696, 0, 3773, 0, 5612, 0, 7048, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1040, 0, 637, 0, 1332, 0, 2711, 0, 3772, 0, 5613, 0, 6986, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1030, 0, 644, 0, 1239, 0, 2736, 0, 3774, 0, 5619, 0, 6952, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1020, 0, 648, 0, 1166, 0, 2765, 0, 3779, 0, 5628, 0, 6936, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1010, 0, 650, 0, 1113, 0, 2791, 0, 3785, 0, 5637, 0, 6931, 0, 0, 0, 200, 30, 0, 60, 0, 0, 35, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 1000, 0, 651, 0, 1078, 0, 2811, 0, 3789, 0, 5644, 0, 6931, 0, 0, 0, 200, 30, 0, 60, 0, 0, 32, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 990, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 32, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 980, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 100, 50, 100, 60, 100, 65, 100, 70, 100, 55, 100, 0, 0, 0, 60 }},
		{{ 970, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 57, 60 }},
		{{ 960, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 58, 60 }},
		{{ 958, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 58, 60 }},
		{{ 957, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 59, 60 }},
		{{ 955, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 59, 60 }},
		{{ 953, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 952, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 950, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 948, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 947, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 945, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 943, 0, 651, 0, 1061, 0, 2822, 0, 3791, 0, 5648, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 942, 0, 650, 0, 1068, 0, 2817, 0, 3790, 0, 5651, 0, 6931, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 52, 55, 68, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 940, 0, 647, 0, 1082, 0, 2808, 0, 3788, 0, 5658, 0, 6930, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 44, 59, 53, 55, 69, 52, 64, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 938, 0, 644, 0, 1102, 0, 2794, 0, 3785, 0, 5667, 0, 6929, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 45, 58, 53, 56, 69, 52, 65, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 937, 0, 639, 0, 1130, 0, 2777, 0, 3782, 0, 5678, 0, 6928, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 46, 58, 53, 56, 70, 52, 65, 45, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 935, 0, 632, 0, 1166, 0, 2757, 0, 3778, 0, 5692, 0, 6928, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 46, 58, 54, 56, 72, 52, 65, 44, 80, 41, 80, 0, 0, 60, 60 }},
		{{ 933, 0, 623, 0, 1209, 0, 2735, 0, 3773, 0, 5708, 0, 6929, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 46, 47, 58, 55, 56, 73, 52, 66, 44, 80, 40, 80, 0, 0, 60, 60 }},
		{{ 932, 0, 612, 0, 1260, 0, 2713, 0, 3769, 0, 5725, 0, 6933, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 45, 48, 58, 56, 57, 74, 52, 67, 44, 80, 40, 80, 0, 0, 60, 60 }},
		{{ 930, 0, 597, 0, 1321, 0, 2691, 0, 3764, 0, 5743, 0, 6941, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 45, 49, 58, 58, 57, 76, 52, 67, 44, 80, 40, 80, 0, 0, 60, 60 }},
		{{ 928, 0, 580, 0, 1390, 0, 2672, 0, 3761, 0, 5761, 0, 6953, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 57, 59, 58, 78, 52, 68, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 927, 0, 559, 0, 1470, 0, 2656, 0, 3758, 0, 5778, 0, 6972, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 57, 61, 58, 80, 53, 69, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 925, 0, 533, 0, 1560, 0, 2647, 0, 3757, 0, 5792, 0, 6998, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 64, 59, 81, 53, 70, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 923, 0, 502, 0, 1662, 0, 2646, 0, 3757, 0, 5805, 0, 7033, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 52, 56, 67, 60, 83, 53, 70, 45, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 922, 0, 471, 0, 1758, 0, 2654, 0, 3760, 0, 5812, 0, 7071, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 52, 56, 70, 61, 85, 54, 71, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 920, 0, 440, 0, 1848, 0, 2668, 0, 3764, 0, 5816, 0, 7112, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 52, 55, 73, 62, 86, 54, 72, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 919, 0, 411, 0, 1930, 0, 2686, 0, 3768, 0, 5816, 0, 7152, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 52, 55, 76, 63, 87, 55, 72, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 918, 0, 384, 0, 2006, 0, 2706, 0, 3773, 0, 5815, 0, 7192, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 52, 55, 79, 64, 87, 55, 73, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 916, 0, 359, 0, 2073, 0, 2728, 0, 3778, 0, 5812, 0, 7229, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 51, 54, 81, 64, 88, 56, 73, 48, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 915, 0, 337, 0, 2131, 0, 2749, 0, 3784, 0, 5808, 0, 7262, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 51, 54, 83, 65, 88, 56, 73, 48, 80, 33, 80, 0, 0, 60, 60 }},
		{{ 914, 0, 318, 0, 2181, 0, 2768, 0, 3788, 0, 5804, 0, 7292, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 50, 54, 85, 66, 88, 57, 73, 49, 80, 33, 80, 0, 0, 60, 60 }},
		{{ 912, 0, 302, 0, 2220, 0, 2785, 0, 3792, 0, 5801, 0, 7316, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 50, 54, 87, 66, 89, 57, 74, 49, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 911, 0, 290, 0, 2251, 0, 2798, 0, 3795, 0, 5798, 0, 7335, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 49, 54, 88, 66, 89, 57, 74, 49, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 910, 0, 282, 0, 2271, 0, 2807, 0, 3797, 0, 5796, 0, 7348, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 54, 89, 67, 89, 57, 74, 49, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 909, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 908, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 906, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 905, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 1, 0, 60, 60 }},
		{{ 904, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 2, 0, 60, 60 }},
		{{ 902, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 3, 0, 60, 60 }},
		{{ 901, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 5, 0, 60, 60 }},
		{{ 900, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 7, 0, 59, 60 }},
		{{ 899, 0, 284, 0, 2238, 0, 2783, 0, 3772, 0, 5820, 0, 7318, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 49, 53, 87, 66, 90, 57, 72, 49, 80, 32, 80, 9, 0, 57, 60 }},
		{{ 898, 0, 294, 0, 2148, 0, 2732, 0, 3724, 0, 5864, 0, 7253, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 50, 53, 82, 65, 93, 57, 69, 48, 80, 33, 80, 12, 0, 55, 60 }},
		{{ 896, 0, 308, 0, 2006, 0, 2673, 0, 3661, 0, 5910, 0, 7171, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 52, 52, 75, 63, 95, 56, 65, 46, 80, 34, 80, 15, 0, 53, 60 }},
		{{ 895, 0, 325, 0, 1801, 0, 2629, 0, 3595, 0, 5935, 0, 7095, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 55, 53, 67, 61, 96, 56, 60, 44, 80, 35, 80, 18, 0, 50, 60 }},
		{{ 894, 0, 337, 0, 1579, 0, 2623, 0, 3549, 0, 5918, 0, 7055, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 60, 54, 62, 60, 93, 55, 57, 43, 80, 37, 80, 21, 0, 48, 60 }},
		{{ 892, 0, 345, 0, 1402, 0, 2644, 0, 3529, 0, 5877, 0, 7049, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 64, 55, 58, 59, 90, 55, 56, 42, 80, 38, 80, 23, 0, 47, 60 }},
		{{ 891, 0, 348, 0, 1279, 0, 2670, 0, 3522, 0, 5837, 0, 7056, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 56, 57, 58, 86, 55, 56, 41, 80, 39, 80, 25, 0, 46, 60 }},
		{{ 890, 0, 350, 0, 1216, 0, 2687, 0, 3520, 0, 5813, 0, 7063, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 56, 56, 58, 84, 55, 56, 41, 80, 39, 80, 27, 0, 45, 60 }},
		{{ 894, 0, 350, 0, 1216, 0, 2687, 0, 3520, 0, 5813, 0, 7063, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 56, 56, 58, 84, 55, 56, 41, 80, 39, 80, 28, 0, 45, 60 }},
		{{ 897, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 29, 0, 45, 60 }},
		{{ 901, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 904, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 908, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 911, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 915, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 919, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 922, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 926, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 929, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 933, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 936, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 30, 0, 45, 60 }},
		{{ 940, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 941, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 942, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 943, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 944, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 945, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 946, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 947, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 948, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 949, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 950, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 951, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 952, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 953, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 954, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 955, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 956, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 957, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 958, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 959, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 960, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 961, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 962, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 963, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 964, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 965, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 966, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 967, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 968, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 969, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 970, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 971, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 972, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 973, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 974, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 975, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 976, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 977, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 978, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 0, 60 }},
		{{ 979, 0, 350, 0, 1200, 0, 2691, 0, 3520, 0, 5807, 0, 7065, 0, 0, 0, 200, 30, 0, 60, 0, 0, 38, 0, 0, 50, 57, 56, 58, 83, 55, 56, 41, 80, 39, 80, 0, 0, 43, 60 }},
		{{ 980, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 51, 60 }},
		{{ 989, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 51, 60 }},
		{{ 998, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 51, 60 }},
		{{ 1006, 0, 481, 0, 1539, 0, 2809, 0, 3806, 0, 5770, 0, 6975, 0, 0, 0, 200, 30, 0, 60, 0, 0, 34, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 51, 60 }},
		{{ 1015, 0, 500, 0, 1570, 0, 2785, 0, 3803, 0, 5839, 0, 6997, 0, 0, 0, 200, 30, 0, 60, 0, 0, 11, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 52, 60 }},
		{{ 1024, 0, 528, 0, 1617, 0, 2761, 0, 3805, 0, 5917, 0, 7031, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 54, 60 }},
		{{ 1032, 0, 566, 0, 1680, 0, 2752, 0, 3819, 0, 5978, 0, 7080, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 55, 60 }},
		{{ 1041, 0, 602, 0, 1744, 0, 2770, 0, 3846, 0, 5988, 0, 7130, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 47, 59, 78, 64, 111, 57, 71, 46, 80, 38, 80, 0, 0, 57, 60 }},
		{{ 1050, 0, 630, 0, 1793, 0, 2800, 0, 3875, 0, 5962, 0, 7170, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 48, 59, 80, 64, 112, 58, 80, 45, 80, 38, 80, 0, 0, 59, 60 }},
		{{ 1048, 0, 648, 0, 1825, 0, 2828, 0, 3898, 0, 5928, 0, 7198, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 49, 59, 82, 65, 114, 58, 88, 44, 80, 39, 80, 0, 0, 59, 60 }},
		{{ 1046, 0, 657, 0, 1842, 0, 2844, 0, 3911, 0, 5907, 0, 7212, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 49, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1044, 0, 657, 0, 1842, 0, 2844, 0, 3911, 0, 5907, 0, 7212, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 49, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1042, 0, 647, 0, 1829, 0, 2837, 0, 3911, 0, 5885, 0, 7175, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 82, 65, 113, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1040, 0, 624, 0, 1796, 0, 2809, 0, 3897, 0, 5855, 0, 7101, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 79, 64, 108, 57, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1038, 0, 591, 0, 1751, 0, 2751, 0, 3857, 0, 5818, 0, 7004, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 54, 58, 75, 63, 100, 55, 82, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1036, 0, 552, 0, 1698, 0, 2646, 0, 3769, 0, 5783, 0, 6904, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 70, 61, 90, 53, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1034, 0, 511, 0, 1643, 0, 2469, 0, 3603, 0, 5762, 0, 6825, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 64, 59, 77, 51, 67, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1032, 0, 483, 0, 1607, 0, 2292, 0, 3425, 0, 5763, 0, 6798, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 59, 56, 67, 49, 60, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1030, 0, 466, 0, 1586, 0, 2138, 0, 3266, 0, 5775, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 56, 54, 60, 47, 55, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1026, 0, 456, 0, 1574, 0, 2025, 0, 3147, 0, 5788, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 58, 54, 52, 55, 46, 52, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1022, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1018, 0, 457, 0, 1544, 0, 1983, 0, 3092, 0, 5812, 0, 6824, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 54, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1014, 0, 466, 0, 1494, 0, 2019, 0, 3109, 0, 5845, 0, 6838, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 59, 53, 51, 58, 46, 52, 46, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1010, 0, 479, 0, 1421, 0, 2078, 0, 3137, 0, 5892, 0, 6864, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 60, 54, 51, 64, 46, 54, 45, 80, 35, 80, 0, 0, 57, 60 }},
		{{ 1006, 0, 494, 0, 1325, 0, 2168, 0, 3178, 0, 5954, 0, 6910, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 61, 56, 52, 71, 47, 56, 44, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 1002, 0, 507, 0, 1208, 0, 2296, 0, 3235, 0, 6028, 0, 6986, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 49, 63, 58, 52, 79, 48, 58, 44, 80, 36, 80, 0, 0, 54, 60 }},
		{{ 998, 0, 516, 0, 1072, 0, 2473, 0, 3312, 0, 6112, 0, 7103, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 66, 63, 53, 88, 49, 60, 43, 80, 36, 80, 0, 0, 52, 60 }},
		{{ 994, 0, 518, 0, 962, 0, 2641, 0, 3383, 0, 6178, 0, 7223, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 41, 46, 68, 67, 54, 95, 50, 62, 43, 80, 37, 80, 0, 0, 51, 60 }},
		{{ 990, 0, 516, 0, 876, 0, 2787, 0, 3445, 0, 6229, 0, 7334, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 44, 70, 72, 55, 101, 52, 63, 43, 80, 37, 80, 0, 0, 50, 60 }},
		{{ 995, 0, 512, 0, 820, 0, 2892, 0, 3488, 0, 6261, 0, 7414, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 38, 43, 71, 75, 56, 104, 52, 64, 43, 80, 37, 80, 0, 0, 48, 60 }},
		{{ 1000, 0, 513, 0, 834, 0, 2865, 0, 3477, 0, 6253, 0, 7394, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 38, 43, 71, 74, 55, 103, 52, 64, 43, 80, 37, 80, 0, 0, 47, 60 }},
		{{ 1005, 0, 515, 0, 855, 0, 2827, 0, 3461, 0, 6241, 0, 7364, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 43, 71, 73, 55, 102, 52, 64, 43, 80, 37, 80, 0, 0, 48, 60 }},
		{{ 1010, 0, 516, 0, 883, 0, 2776, 0, 3440, 0, 6225, 0, 7325, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 44, 70, 71, 55, 100, 51, 63, 43, 80, 37, 80, 0, 0, 49, 60 }},
		{{ 1015, 0, 517, 0, 918, 0, 2714, 0, 3414, 0, 6204, 0, 7278, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 40, 45, 69, 70, 54, 98, 51, 63, 43, 80, 37, 80, 0, 0, 49, 60 }},
		{{ 1020, 0, 518, 0, 960, 0, 2643, 0, 3384, 0, 6179, 0, 7225, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 41, 46, 68, 67, 54, 95, 50, 62, 43, 80, 37, 80, 0, 0, 50, 60 }},
		{{ 1025, 0, 518, 0, 1011, 0, 2563, 0, 3350, 0, 6149, 0, 7167, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 41, 47, 67, 65, 54, 92, 50, 61, 43, 80, 37, 80, 0, 0, 51, 60 }},
		{{ 1030, 0, 516, 0, 1069, 0, 2477, 0, 3313, 0, 6113, 0, 7106, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 42, 47, 66, 63, 53, 88, 49, 60, 43, 80, 36, 80, 0, 0, 51, 60 }},
		{{ 1035, 0, 513, 0, 1136, 0, 2386, 0, 3274, 0, 6072, 0, 7044, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 43, 48, 65, 61, 53, 84, 48, 59, 43, 80, 36, 80, 0, 0, 52, 60 }},
		{{ 1040, 0, 507, 0, 1211, 0, 2292, 0, 3233, 0, 6026, 0, 6983, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 43, 49, 63, 58, 52, 79, 48, 58, 44, 80, 36, 80, 0, 0, 53, 60 }},
		{{ 1045, 0, 499, 0, 1280, 0, 2214, 0, 3198, 0, 5982, 0, 6936, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 50, 62, 57, 52, 74, 47, 56, 44, 80, 36, 80, 0, 0, 53, 60 }},
		{{ 1050, 0, 491, 0, 1343, 0, 2150, 0, 3170, 0, 5942, 0, 6900, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 61, 55, 52, 69, 47, 55, 45, 80, 35, 80, 0, 0, 54, 60 }},
		{{ 1052, 0, 483, 0, 1398, 0, 2098, 0, 3146, 0, 5907, 0, 6874, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 60, 54, 51, 65, 46, 54, 45, 80, 35, 80, 0, 0, 55, 60 }},
		{{ 1055, 0, 475, 0, 1446, 0, 2057, 0, 3127, 0, 5876, 0, 6854, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 59, 54, 51, 62, 46, 53, 46, 80, 35, 80, 0, 0, 55, 60 }},
		{{ 1058, 0, 468, 0, 1487, 0, 2025, 0, 3112, 0, 5850, 0, 6840, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 59, 53, 51, 59, 46, 52, 46, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 1060, 0, 462, 0, 1519, 0, 2000, 0, 3100, 0, 5828, 0, 6831, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 56, 46, 52, 46, 80, 34, 80, 0, 0, 57, 60 }},
		{{ 1062, 0, 457, 0, 1544, 0, 1983, 0, 3092, 0, 5812, 0, 6824, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 54, 46, 51, 47, 80, 34, 80, 0, 0, 57, 60 }},
		{{ 1065, 0, 453, 0, 1560, 0, 1971, 0, 3087, 0, 5801, 0, 6821, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 53, 45, 51, 47, 80, 34, 80, 0, 0, 58, 60 }},
		{{ 1068, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 59, 60 }},
		{{ 1070, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 59, 60 }},
		{{ 1072, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1075, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 56, 60 }},
		{{ 1078, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 53, 60 }},
		{{ 1080, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 49, 60 }},
		{{ 1082, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 46, 60 }},
		{{ 1085, 0, 451, 0, 1570, 0, 2013, 0, 3118, 0, 5791, 0, 6824, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 43, 50, 58, 54, 52, 57, 46, 52, 46, 80, 34, 80, 0, 0, 42, 60 }},
		{{ 1088, 0, 450, 0, 1573, 0, 2105, 0, 3185, 0, 5782, 0, 6835, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 43, 50, 58, 56, 53, 66, 48, 55, 46, 80, 35, 80, 0, 0, 38, 60 }},
		{{ 1090, 0, 450, 0, 1574, 0, 2234, 0, 3282, 0, 5769, 0, 6851, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 42, 50, 57, 60, 56, 79, 50, 59, 45, 80, 36, 80, 0, 0, 35, 60 }},
		{{ 1092, 0, 451, 0, 1572, 0, 2391, 0, 3406, 0, 5755, 0, 6873, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 41, 50, 57, 64, 58, 93, 52, 62, 45, 80, 36, 80, 0, 0, 31, 60 }},
		{{ 1095, 0, 455, 0, 1563, 0, 2562, 0, 3551, 0, 5742, 0, 6902, 0, 0, 0, 200, 30, 52, 60, 0, 0, 0, 0, 40, 48, 57, 68, 61, 106, 55, 63, 45, 80, 37, 80, 0, 0, 28, 60 }},
		{{ 1098, 0, 460, 0, 1551, 0, 2680, 0, 3659, 0, 5736, 0, 6926, 0, 0, 0, 200, 30, 52, 60, 0, 0, 40, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 24, 60 }},
		{{ 1100, 0, 465, 0, 1539, 0, 2756, 0, 3736, 0, 5733, 0, 6945, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 469, 0, 1529, 0, 2802, 0, 3785, 0, 5731, 0, 6958, 0, 0, 0, 200, 30, 0, 60, 0, 0, 43, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 45, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 46, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 471, 0, 1524, 0, 2823, 0, 3809, 0, 5731, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 469, 0, 1519, 0, 2817, 0, 3799, 0, 5719, 0, 6958, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 464, 0, 1511, 0, 2805, 0, 3778, 0, 5695, 0, 6947, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 457, 0, 1500, 0, 2783, 0, 3742, 0, 5661, 0, 6934, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 448, 0, 1487, 0, 2748, 0, 3687, 0, 5622, 0, 6923, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 439, 0, 1474, 0, 2694, 0, 3607, 0, 5580, 0, 6920, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 429, 0, 1461, 0, 2617, 0, 3495, 0, 5541, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 420, 0, 1453, 0, 2508, 0, 3340, 0, 5512, 0, 6967, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 413, 0, 1451, 0, 2358, 0, 3133, 0, 5499, 0, 7036, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 410, 0, 1455, 0, 2205, 0, 2925, 0, 5505, 0, 7121, 0, 0, 0, 200, 30, 0, 60, 0, 0, 48, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 408, 0, 1464, 0, 2057, 0, 2727, 0, 5525, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 47, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 409, 0, 1475, 0, 1923, 0, 2548, 0, 5551, 0, 7302, 0, 0, 0, 200, 30, 0, 60, 0, 0, 46, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 410, 0, 1486, 0, 1808, 0, 2396, 0, 5577, 0, 7382, 0, 0, 0, 200, 30, 0, 60, 0, 0, 43, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 411, 0, 1495, 0, 1718, 0, 2276, 0, 5601, 0, 7447, 0, 0, 0, 200, 30, 0, 60, 0, 0, 41, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 413, 0, 1502, 0, 1656, 0, 2194, 0, 5619, 0, 7492, 0, 0, 0, 200, 30, 0, 60, 0, 0, 39, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 413, 0, 1506, 0, 1624, 0, 2152, 0, 5628, 0, 7516, 0, 0, 0, 200, 30, 0, 60, 0, 0, 39, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 413, 0, 1506, 0, 1624, 0, 2152, 0, 5628, 0, 7516, 0, 0, 0, 200, 30, 0, 60, 0, 0, 39, 0, 0, 10, 0, 10, 55, 100, 0, 10, 60, 200, 0, 10, 0, 0, 0, 60 }},
		{{ 1100, 0, 413, 0, 1506, 0, 1624, 0, 2152, 0, 5628, 0, 7516, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 12, 69, 75, 48, 127, 52, 85, 39, 80, 31, 80, 0, 0, 36, 60 }},
		{{ 1100, 0, 413, 0, 1506, 0, 1624, 0, 2152, 0, 5628, 0, 7516, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 12, 69, 75, 48, 127, 52, 85, 39, 80, 31, 80, 0, 0, 39, 60 }},
		{{ 1101, 0, 413, 0, 1506, 0, 1624, 0, 2152, 0, 5628, 0, 7516, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 12, 69, 75, 48, 127, 52, 85, 39, 80, 31, 80, 0, 0, 42, 60 }},
		{{ 1103, 0, 413, 0, 1506, 0, 1624, 0, 2152, 0, 5628, 0, 7516, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 12, 69, 75, 48, 127, 52, 85, 39, 80, 31, 80, 0, 0, 45, 60 }},
		{{ 1104, 0, 413, 0, 1510, 0, 1634, 0, 2170, 0, 5615, 0, 7497, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 13, 68, 75, 48, 127, 52, 85, 39, 80, 31, 80, 0, 0, 48, 60 }},
		{{ 1106, 0, 411, 0, 1518, 0, 1655, 0, 2206, 0, 5589, 0, 7460, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 14, 68, 74, 48, 126, 53, 85, 39, 80, 31, 80, 0, 0, 49, 60 }},
		{{ 1107, 0, 408, 0, 1530, 0, 1687, 0, 2259, 0, 5552, 0, 7406, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 15, 67, 73, 49, 126, 53, 84, 39, 80, 31, 80, 0, 0, 49, 60 }},
		{{ 1109, 0, 405, 0, 1547, 0, 1729, 0, 2329, 0, 5505, 0, 7337, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 39, 17, 66, 72, 50, 125, 53, 84, 39, 80, 32, 80, 0, 0, 50, 60 }},
		{{ 1110, 0, 400, 0, 1570, 0, 1784, 0, 2416, 0, 5451, 0, 7256, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 38, 19, 64, 71, 51, 125, 53, 83, 40, 80, 32, 80, 0, 0, 50, 60 }},
		{{ 1111, 0, 394, 0, 1599, 0, 1850, 0, 2517, 0, 5393, 0, 7166, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 38, 22, 62, 70, 52, 125, 53, 82, 40, 80, 33, 80, 0, 0, 51, 60 }},
		{{ 1113, 0, 386, 0, 1635, 0, 1929, 0, 2633, 0, 5334, 0, 7071, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 38, 24, 61, 69, 54, 125, 54, 82, 40, 80, 34, 80, 0, 0, 52, 60 }},
		{{ 1114, 0, 377, 0, 1680, 0, 2021, 0, 2762, 0, 5279, 0, 6975, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 27, 59, 69, 55, 125, 55, 81, 41, 80, 34, 80, 0, 0, 52, 60 }},
		{{ 1116, 0, 366, 0, 1735, 0, 2128, 0, 2902, 0, 5233, 0, 6885, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 29, 57, 68, 57, 127, 55, 80, 42, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 1117, 0, 353, 0, 1802, 0, 2251, 0, 3053, 0, 5201, 0, 6805, 0, 0, 0, 200, 30, 16, 60, 0, 0, 0, 0, 37, 32, 54, 69, 60, 130, 56, 79, 42, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 1119, 0, 340, 0, 1866, 0, 2364, 0, 3182, 0, 5189, 0, 6753, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 33, 53, 70, 62, 134, 57, 78, 43, 80, 36, 80, 0, 0, 54, 60 }},
		{{ 1120, 0, 327, 0, 1927, 0, 2467, 0, 3294, 0, 5192, 0, 6721, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 34, 51, 71, 63, 137, 58, 77, 43, 80, 36, 80, 0, 0, 55, 60 }},
		{{ 1122, 0, 316, 0, 1984, 0, 2559, 0, 3388, 0, 5206, 0, 6704, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 34, 50, 73, 65, 141, 59, 77, 44, 80, 36, 80, 0, 0, 55, 60 }},
		{{ 1123, 0, 306, 0, 2034, 0, 2640, 0, 3467, 0, 5225, 0, 6699, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 35, 49, 74, 66, 145, 60, 76, 44, 80, 36, 80, 0, 0, 56, 60 }},
		{{ 1125, 0, 297, 0, 2078, 0, 2708, 0, 3530, 0, 5247, 0, 6701, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 35, 49, 76, 68, 148, 61, 76, 44, 80, 36, 80, 0, 0, 56, 60 }},
		{{ 1127, 0, 290, 0, 2114, 0, 2763, 0, 3580, 0, 5268, 0, 6706, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 77, 69, 151, 61, 75, 45, 80, 36, 80, 0, 0, 57, 60 }},
		{{ 1128, 0, 284, 0, 2142, 0, 2805, 0, 3617, 0, 5285, 0, 6712, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 78, 69, 154, 62, 75, 45, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1130, 0, 281, 0, 2160, 0, 2833, 0, 3641, 0, 5298, 0, 6717, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 79, 70, 155, 62, 75, 45, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1132, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 1133, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 1135, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1137, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1138, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1140, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1142, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 1143, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1145, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1147, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 57, 60 }},
		{{ 1148, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 1150, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 54, 60 }},
		{{ 1152, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 1153, 0, 282, 0, 2160, 0, 2843, 0, 3653, 0, 5313, 0, 6724, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 79, 70, 155, 62, 75, 45, 80, 35, 80, 0, 0, 51, 60 }},
		{{ 1155, 0, 289, 0, 2142, 0, 2835, 0, 3654, 0, 5329, 0, 6730, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 35, 48, 79, 70, 154, 62, 74, 45, 80, 35, 80, 0, 0, 49, 60 }},
		{{ 1157, 0, 299, 0, 2114, 0, 2824, 0, 3654, 0, 5353, 0, 6740, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 35, 48, 79, 69, 152, 62, 73, 45, 80, 35, 80, 0, 0, 48, 60 }},
		{{ 1158, 0, 312, 0, 2077, 0, 2809, 0, 3655, 0, 5383, 0, 6753, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 36, 49, 78, 69, 150, 61, 72, 45, 80, 36, 80, 0, 0, 45, 60 }},
		{{ 1160, 0, 328, 0, 2032, 0, 2792, 0, 3657, 0, 5420, 0, 6769, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 37, 50, 78, 68, 146, 61, 71, 45, 80, 36, 80, 0, 0, 43, 60 }},
		{{ 1162, 0, 347, 0, 1979, 0, 2774, 0, 3659, 0, 5463, 0, 6787, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 38, 51, 77, 68, 143, 60, 70, 45, 80, 36, 80, 0, 0, 41, 60 }},
		{{ 1163, 0, 369, 0, 1918, 0, 2754, 0, 3662, 0, 5511, 0, 6808, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 39, 52, 77, 67, 139, 60, 68, 46, 80, 36, 80, 0, 0, 39, 60 }},
		{{ 1165, 0, 393, 0, 1850, 0, 2735, 0, 3666, 0, 5561, 0, 6830, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 40, 53, 76, 66, 134, 59, 66, 46, 80, 36, 80, 0, 0, 36, 60 }},
		{{ 1167, 0, 419, 0, 1776, 0, 2717, 0, 3671, 0, 5615, 0, 6853, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 41, 54, 75, 66, 129, 58, 64, 46, 80, 37, 80, 0, 0, 35, 60 }},
		{{ 1168, 0, 447, 0, 1696, 0, 2703, 0, 3679, 0, 5668, 0, 6876, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 43, 56, 74, 65, 124, 58, 63, 46, 80, 37, 80, 0, 0, 33, 60 }},
		{{ 1170, 0, 473, 0, 1619, 0, 2693, 0, 3687, 0, 5717, 0, 6898, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 44, 57, 73, 64, 119, 57, 61, 46, 80, 37, 80, 0, 0, 31, 60 }},
		{{ 1172, 0, 496, 0, 1552, 0, 2689, 0, 3696, 0, 5756, 0, 6915, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 44, 58, 72, 63, 114, 56, 60, 46, 80, 38, 80, 0, 0, 30, 60 }},
		{{ 1173, 0, 515, 0, 1494, 0, 2689, 0, 3705, 0, 5787, 0, 6929, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 45, 59, 71, 63, 111, 56, 59, 46, 80, 38, 80, 0, 0, 28, 60 }},
		{{ 1175, 0, 531, 0, 1445, 0, 2692, 0, 3713, 0, 5811, 0, 6940, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 46, 59, 71, 62, 108, 56, 58, 47, 80, 38, 80, 0, 0, 27, 60 }},
		{{ 1177, 0, 545, 0, 1405, 0, 2696, 0, 3720, 0, 5830, 0, 6949, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 46, 60, 70, 62, 105, 55, 57, 47, 80, 38, 80, 0, 0, 26, 60 }},
		{{ 1178, 0, 555, 0, 1371, 0, 2701, 0, 3727, 0, 5844, 0, 6956, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 60, 70, 62, 103, 55, 57, 47, 80, 38, 80, 0, 0, 26, 60 }},
		{{ 1180, 0, 564, 0, 1345, 0, 2706, 0, 3732, 0, 5854, 0, 6961, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 61, 69, 61, 101, 55, 57, 47, 80, 39, 80, 0, 0, 25, 60 }},
		{{ 1182, 0, 570, 0, 1326, 0, 2710, 0, 3737, 0, 5861, 0, 6964, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 61, 69, 61, 100, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1183, 0, 574, 0, 1313, 0, 2713, 0, 3739, 0, 5866, 0, 6967, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1185, 0, 576, 0, 1307, 0, 2715, 0, 3741, 0, 5868, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1187, 0, 576, 0, 1307, 0, 2715, 0, 3741, 0, 5868, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1188, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1190, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1184, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1177, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1171, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1164, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1158, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1151, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1145, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1139, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1132, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1126, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1119, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1113, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1106, 0, 462, 0, 1513, 0, 2553, 0, 3535, 0, 5746, 0, 6910, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1100, 0, 466, 0, 1520, 0, 2560, 0, 3550, 0, 5757, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1075, 0, 474, 0, 1530, 0, 2570, 0, 3573, 0, 5773, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 49, 58, 68, 61, 104, 54, 62, 45, 80, 37, 80, 0, 0, 50, 60 }},
		{{ 1050, 0, 484, 0, 1544, 0, 2585, 0, 3603, 0, 5793, 0, 6911, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 103, 54, 62, 45, 80, 37, 80, 0, 0, 51, 60 }},
		{{ 1048, 0, 496, 0, 1563, 0, 2603, 0, 3638, 0, 5816, 0, 6916, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 102, 54, 62, 45, 80, 37, 80, 0, 0, 53, 60 }},
		{{ 1045, 0, 512, 0, 1587, 0, 2626, 0, 3679, 0, 5840, 0, 6927, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 50, 58, 69, 62, 101, 55, 63, 46, 80, 37, 80, 0, 0, 55, 60 }},
		{{ 1043, 0, 530, 0, 1616, 0, 2654, 0, 3722, 0, 5864, 0, 6945, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 70, 62, 101, 55, 65, 46, 80, 38, 80, 0, 0, 57, 60 }},
		{{ 1041, 0, 552, 0, 1652, 0, 2686, 0, 3767, 0, 5885, 0, 6975, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 72, 62, 102, 55, 67, 46, 80, 38, 80, 0, 0, 58, 60 }},
		{{ 1038, 0, 575, 0, 1691, 0, 2720, 0, 3808, 0, 5900, 0, 7012, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 74, 63, 103, 56, 71, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1036, 0, 595, 0, 1726, 0, 2750, 0, 3840, 0, 5907, 0, 7053, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 76, 63, 105, 56, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1034, 0, 612, 0, 1758, 0, 2777, 0, 3864, 0, 5910, 0, 7092, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 50, 59, 78, 64, 108, 57, 79, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1031, 0, 627, 0, 1786, 0, 2800, 0, 3882, 0, 5910, 0, 7129, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 79, 64, 110, 57, 83, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1029, 0, 640, 0, 1809, 0, 2819, 0, 3896, 0, 5908, 0, 7161, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 81, 64, 112, 58, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1027, 0, 649, 0, 1826, 0, 2833, 0, 3905, 0, 5905, 0, 7186, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 82, 65, 113, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1024, 0, 655, 0, 1838, 0, 2843, 0, 3911, 0, 5903, 0, 7204, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1022, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5901, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1020, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5901, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1017, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5899, 0, 7211, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1015, 0, 655, 0, 1840, 0, 2845, 0, 3913, 0, 5895, 0, 7202, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 114, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1013, 0, 651, 0, 1834, 0, 2841, 0, 3912, 0, 5890, 0, 7188, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 83, 65, 114, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1010, 0, 646, 0, 1827, 0, 2836, 0, 3910, 0, 5883, 0, 7171, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 82, 65, 112, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1008, 0, 639, 0, 1818, 0, 2829, 0, 3907, 0, 5874, 0, 7149, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 81, 64, 111, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1006, 0, 631, 0, 1807, 0, 2819, 0, 3903, 0, 5864, 0, 7124, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 80, 64, 109, 57, 88, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1003, 0, 622, 0, 1795, 0, 2807, 0, 3896, 0, 5853, 0, 7097, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 79, 64, 107, 57, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1001, 0, 612, 0, 1781, 0, 2791, 0, 3886, 0, 5841, 0, 7066, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 53, 58, 78, 64, 105, 56, 86, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 999, 0, 602, 0, 1766, 0, 2772, 0, 3873, 0, 5829, 0, 7034, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 53, 58, 76, 63, 102, 56, 84, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 996, 0, 590, 0, 1750, 0, 2748, 0, 3855, 0, 5816, 0, 7001, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 54, 58, 75, 63, 100, 55, 82, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 994, 0, 578, 0, 1733, 0, 2719, 0, 3832, 0, 5804, 0, 6967, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 58, 73, 62, 97, 55, 80, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 992, 0, 565, 0, 1715, 0, 2684, 0, 3802, 0, 5792, 0, 6934, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 71, 62, 93, 54, 77, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 989, 0, 551, 0, 1697, 0, 2642, 0, 3766, 0, 5782, 0, 6902, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 70, 61, 89, 53, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 987, 0, 538, 0, 1678, 0, 2592, 0, 3720, 0, 5773, 0, 6872, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 68, 60, 86, 52, 72, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 985, 0, 524, 0, 1660, 0, 2533, 0, 3665, 0, 5766, 0, 6846, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 66, 60, 81, 52, 69, 46, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 982, 0, 512, 0, 1644, 0, 2473, 0, 3607, 0, 5762, 0, 6826, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 64, 59, 77, 51, 67, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 980, 0, 501, 0, 1630, 0, 2413, 0, 3548, 0, 5760, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 62, 58, 74, 50, 64, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 977, 0, 492, 0, 1618, 0, 2354, 0, 3489, 0, 5761, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 60, 57, 70, 49, 62, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 974, 0, 484, 0, 1608, 0, 2297, 0, 3430, 0, 5763, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 59, 56, 67, 49, 60, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 971, 0, 477, 0, 1599, 0, 2242, 0, 3374, 0, 5766, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 58, 55, 65, 48, 58, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 968, 0, 471, 0, 1592, 0, 2191, 0, 3321, 0, 5770, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 57, 54, 62, 48, 57, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 965, 0, 467, 0, 1586, 0, 2144, 0, 3272, 0, 5774, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 56, 54, 60, 47, 55, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 962, 0, 463, 0, 1581, 0, 2101, 0, 3227, 0, 5779, 0, 6802, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 55, 53, 58, 47, 54, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 959, 0, 459, 0, 1578, 0, 2063, 0, 3188, 0, 5783, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 54, 53, 56, 46, 53, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 956, 0, 457, 0, 1574, 0, 2031, 0, 3154, 0, 5787, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 58, 54, 52, 55, 46, 52, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 953, 0, 455, 0, 1572, 0, 2005, 0, 3126, 0, 5790, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 54, 46, 52, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 950, 0, 453, 0, 1570, 0, 1986, 0, 3105, 0, 5793, 0, 6816, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 947, 0, 452, 0, 1569, 0, 1972, 0, 3091, 0, 5795, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 944, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 941, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 938, 0, 451, 0, 1568, 0, 1971, 0, 3090, 0, 5797, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 935, 0, 450, 0, 1566, 0, 1983, 0, 3103, 0, 5799, 0, 6820, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 51, 52, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 932, 0, 449, 0, 1564, 0, 2000, 0, 3122, 0, 5802, 0, 6822, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 929, 0, 447, 0, 1561, 0, 2022, 0, 3146, 0, 5805, 0, 6824, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 58, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 59, 60 }},
		{{ 926, 0, 445, 0, 1558, 0, 2050, 0, 3176, 0, 5810, 0, 6828, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 59, 60 }},
		{{ 923, 0, 442, 0, 1554, 0, 2082, 0, 3211, 0, 5815, 0, 6832, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 53, 52, 54, 46, 52, 47, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 920, 0, 439, 0, 1551, 0, 2118, 0, 3250, 0, 5820, 0, 6839, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 54, 57, 54, 53, 54, 46, 52, 47, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 917, 0, 436, 0, 1549, 0, 2157, 0, 3293, 0, 5825, 0, 6847, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 55, 57, 54, 53, 55, 47, 53, 47, 80, 35, 80, 0, 0, 57, 60 }},
		{{ 914, 0, 432, 0, 1547, 0, 2200, 0, 3338, 0, 5830, 0, 6858, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 56, 57, 54, 54, 55, 47, 53, 46, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 911, 0, 428, 0, 1546, 0, 2245, 0, 3385, 0, 5834, 0, 6873, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 56, 56, 55, 54, 56, 47, 54, 46, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 908, 0, 424, 0, 1548, 0, 2291, 0, 3433, 0, 5838, 0, 6891, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 57, 56, 55, 55, 57, 48, 54, 46, 80, 35, 80, 0, 0, 55, 60 }},
		{{ 905, 0, 419, 0, 1552, 0, 2338, 0, 3481, 0, 5841, 0, 6913, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 57, 56, 56, 55, 58, 48, 55, 46, 80, 35, 80, 0, 0, 54, 60 }},
		{{ 902, 0, 415, 0, 1558, 0, 2385, 0, 3527, 0, 5842, 0, 6940, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 57, 56, 57, 55, 59, 49, 56, 46, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 899, 0, 410, 0, 1569, 0, 2431, 0, 3571, 0, 5840, 0, 6973, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 55, 57, 56, 60, 49, 57, 46, 80, 35, 80, 0, 0, 52, 60 }},
		{{ 896, 0, 405, 0, 1583, 0, 2474, 0, 3610, 0, 5837, 0, 7013, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 55, 58, 56, 61, 50, 58, 46, 80, 35, 80, 0, 0, 50, 60 }},
		{{ 893, 0, 401, 0, 1600, 0, 2508, 0, 3639, 0, 5831, 0, 7054, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 49, 60 }},
		{{ 890, 0, 397, 0, 1618, 0, 2536, 0, 3661, 0, 5824, 0, 7096, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 48, 60 }},
		{{ 890, 0, 394, 0, 1637, 0, 2558, 0, 3676, 0, 5816, 0, 7138, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 47, 60 }},
		{{ 891, 0, 391, 0, 1657, 0, 2574, 0, 3686, 0, 5807, 0, 7178, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 46, 60 }},
		{{ 891, 0, 388, 0, 1677, 0, 2587, 0, 3691, 0, 5797, 0, 7217, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 46, 60 }},
		{{ 891, 0, 386, 0, 1696, 0, 2596, 0, 3692, 0, 5788, 0, 7254, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 45, 60 }},
		{{ 892, 0, 384, 0, 1714, 0, 2602, 0, 3691, 0, 5779, 0, 7288, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 44, 60 }},
		{{ 892, 0, 383, 0, 1731, 0, 2606, 0, 3689, 0, 5771, 0, 7319, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 44, 60 }},
		{{ 892, 0, 382, 0, 1745, 0, 2608, 0, 3685, 0, 5763, 0, 7346, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 43, 60 }},
		{{ 893, 0, 380, 0, 1758, 0, 2609, 0, 3682, 0, 5756, 0, 7369, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 43, 60 }},
		{{ 893, 0, 380, 0, 1769, 0, 2610, 0, 3678, 0, 5751, 0, 7388, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 893, 0, 379, 0, 1777, 0, 2610, 0, 3675, 0, 5746, 0, 7402, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 894, 0, 379, 0, 1782, 0, 2610, 0, 3673, 0, 5743, 0, 7412, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 894, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 894, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 895, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 895, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 895, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 42, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 42, 60 }},
		{{ 896, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 41, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 41, 60 }},
		{{ 896, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 39, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 39, 60 }},
		{{ 896, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 35, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 35, 60 }},
		{{ 897, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 30, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 30, 60 }},
		{{ 897, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 25, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 25, 60 }},
		{{ 897, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 18, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 18, 60 }},
		{{ 898, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 1, 60, 0, 0, 12, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 12, 60 }},
		{{ 898, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 1, 60, 0, 0, 7, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 7, 60 }},
		{{ 898, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 1, 60, 0, 0, 4, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 4, 60 }},
		{{ 899, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 1, 60, 0, 0, 2, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 2, 60 }},
		{{ 899, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 1, 60, 0, 0, 1, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 1, 60 }},
		{{ 899, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 1, 60, 0, 0, 1, 0, 0, 40, 35, 40, 50, 50, 65, 100, 75, 200, 65, 100, 0, 0, 1, 60 }},
		{{ 900, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 901, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 901, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 902, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 902, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 902, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 903, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 903, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 904, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 904, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 904, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 905, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 905, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 906, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 906, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 906, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 907, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 907, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 908, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 908, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 908, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 909, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 909, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 910, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 910, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 910, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 911, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 911, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 912, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 912, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 912, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 913, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 913, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 914, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 914, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 914, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 915, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 915, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 916, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 916, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 916, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 917, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 917, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 918, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 918, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 918, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 919, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 919, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 920, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 920, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 920, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 921, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 921, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 922, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 922, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 922, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 923, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 923, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 924, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 924, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 924, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 925, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 925, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 926, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 926, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 926, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 927, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 927, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 928, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 928, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 928, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 929, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 929, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 930, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 930, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 930, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 931, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 931, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 932, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 932, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 932, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 933, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 933, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 934, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 934, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 934, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 935, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 935, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 936, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 936, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 936, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 937, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 937, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 938, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 938, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 938, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 939, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 939, 0, 379, 0, 1785, 0, 2610, 0, 3672, 0, 5742, 0, 7417, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 64, 0, 67, 0, 63, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 940, 0, 591, 0, 1313, 0, 2691, 0, 3779, 0, 5717, 0, 6857, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 39, 60 }},
		{{ 940, 0, 591, 0, 1313, 0, 2691, 0, 3779, 0, 5717, 0, 6857, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 42, 60 }},
		{{ 945, 0, 591, 0, 1313, 0, 2691, 0, 3779, 0, 5717, 0, 6857, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 45, 60 }},
		{{ 950, 0, 591, 0, 1313, 0, 2691, 0, 3779, 0, 5717, 0, 6857, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 45, 60 }},
		{{ 955, 0, 591, 0, 1313, 0, 2691, 0, 3779, 0, 5717, 0, 6857, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 45, 60 }},
		{{ 960, 0, 590, 0, 1314, 0, 2690, 0, 3779, 0, 5720, 0, 6856, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 45, 60 }},
		{{ 965, 0, 589, 0, 1315, 0, 2688, 0, 3777, 0, 5724, 0, 6853, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 70, 44, 80, 40, 80, 30, 0, 45, 60 }},
		{{ 970, 0, 588, 0, 1317, 0, 2683, 0, 3774, 0, 5730, 0, 6850, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 69, 44, 80, 40, 80, 30, 0, 46, 60 }},
		{{ 975, 0, 587, 0, 1319, 0, 2678, 0, 3770, 0, 5737, 0, 6844, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 57, 75, 51, 69, 44, 80, 40, 80, 30, 0, 49, 60 }},
		{{ 980, 0, 585, 0, 1323, 0, 2670, 0, 3764, 0, 5746, 0, 6838, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 58, 57, 57, 75, 51, 69, 44, 80, 40, 80, 30, 0, 52, 60 }},
		{{ 985, 0, 583, 0, 1327, 0, 2660, 0, 3757, 0, 5756, 0, 6831, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 54, 58, 57, 57, 75, 51, 69, 44, 80, 40, 80, 0, 0, 56, 60 }},
		{{ 990, 0, 580, 0, 1332, 0, 2649, 0, 3748, 0, 5766, 0, 6824, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 58, 57, 57, 75, 50, 69, 44, 80, 40, 80, 0, 0, 58, 60 }},
		{{ 994, 0, 579, 0, 1337, 0, 2639, 0, 3740, 0, 5773, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 998, 0, 577, 0, 1340, 0, 2631, 0, 3733, 0, 5778, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1001, 0, 576, 0, 1343, 0, 2624, 0, 3728, 0, 5782, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 55, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1005, 0, 576, 0, 1345, 0, 2620, 0, 3724, 0, 5785, 0, 6807, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1009, 0, 575, 0, 1346, 0, 2618, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1012, 0, 575, 0, 1346, 0, 2618, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1016, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 1, 0, 60, 60 }},
		{{ 1020, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 2, 0, 60, 60 }},
		{{ 1024, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 3, 0, 60, 60 }},
		{{ 1028, 0, 575, 0, 1346, 0, 2617, 0, 3722, 0, 5786, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 56, 58, 57, 57, 75, 50, 68, 44, 80, 39, 80, 5, 0, 59, 60 }},
		{{ 1031, 0, 557, 0, 1344, 0, 2602, 0, 3692, 0, 5820, 0, 6861, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 55, 58, 58, 57, 76, 51, 65, 44, 80, 39, 80, 8, 0, 58, 60 }},
		{{ 1035, 0, 519, 0, 1335, 0, 2584, 0, 3639, 0, 5871, 0, 6953, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 54, 58, 59, 57, 78, 51, 60, 45, 80, 38, 80, 11, 0, 56, 60 }},
		{{ 1039, 0, 464, 0, 1308, 0, 2584, 0, 3578, 0, 5904, 0, 7046, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 56, 58, 59, 57, 80, 53, 56, 44, 80, 38, 80, 15, 0, 53, 60 }},
		{{ 1042, 0, 409, 0, 1265, 0, 2617, 0, 3538, 0, 5885, 0, 7085, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 61, 58, 58, 58, 82, 54, 54, 43, 80, 38, 80, 19, 0, 50, 60 }},
		{{ 1046, 0, 373, 0, 1227, 0, 2658, 0, 3524, 0, 5844, 0, 7080, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 57, 58, 83, 55, 55, 42, 80, 39, 80, 22, 0, 47, 60 }},
		{{ 1050, 0, 355, 0, 1205, 0, 2684, 0, 3520, 0, 5815, 0, 7069, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 55, 41, 80, 39, 80, 25, 0, 46, 60 }},
		{{ 1055, 0, 355, 0, 1205, 0, 2684, 0, 3520, 0, 5815, 0, 7069, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 57, 56, 58, 83, 55, 55, 41, 80, 39, 80, 27, 0, 45, 60 }},
		{{ 1060, 0, 267, 0, 1676, 0, 2414, 0, 3530, 0, 5966, 0, 7193, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 55, 55, 46, 50, 47, 47, 80, 33, 80, 28, 0, 45, 60 }},
		{{ 1065, 0, 267, 0, 1676, 0, 2414, 0, 3530, 0, 5966, 0, 7193, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 55, 55, 46, 50, 47, 47, 80, 33, 80, 29, 0, 45, 60 }},
		{{ 1070, 0, 267, 0, 1676, 0, 2414, 0, 3530, 0, 5966, 0, 7193, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 55, 55, 46, 50, 47, 47, 80, 33, 80, 30, 0, 45, 60 }},
		{{ 1075, 0, 267, 0, 1676, 0, 2414, 0, 3530, 0, 5966, 0, 7193, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 55, 55, 46, 50, 47, 47, 80, 33, 80, 30, 0, 45, 60 }},
		{{ 1080, 0, 268, 0, 1679, 0, 2416, 0, 3528, 0, 5969, 0, 7190, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 55, 55, 47, 50, 47, 47, 80, 33, 80, 30, 0, 45, 60 }},
		{{ 1085, 0, 271, 0, 1685, 0, 2420, 0, 3523, 0, 5974, 0, 7184, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 56, 55, 50, 50, 48, 47, 80, 33, 80, 30, 0, 45, 60 }},
		{{ 1090, 0, 275, 0, 1695, 0, 2427, 0, 3517, 0, 5978, 0, 7174, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 57, 56, 53, 50, 49, 46, 80, 33, 80, 30, 0, 46, 60 }},
		{{ 1095, 0, 279, 0, 1709, 0, 2438, 0, 3510, 0, 5980, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 58, 56, 58, 51, 51, 46, 80, 33, 80, 30, 0, 47, 60 }},
		{{ 1100, 0, 284, 0, 1730, 0, 2454, 0, 3504, 0, 5974, 0, 7139, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 59, 57, 64, 52, 53, 45, 80, 34, 80, 30, 0, 49, 60 }},
		{{ 1105, 0, 289, 0, 1759, 0, 2477, 0, 3501, 0, 5956, 0, 7110, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 61, 58, 72, 52, 55, 45, 80, 34, 80, 30, 0, 51, 60 }},
		{{ 1110, 0, 293, 0, 1797, 0, 2509, 0, 3503, 0, 5919, 0, 7073, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 52, 63, 59, 82, 54, 58, 45, 80, 34, 80, 30, 0, 54, 60 }},
		{{ 1115, 0, 295, 0, 1848, 0, 2553, 0, 3513, 0, 5857, 0, 7024, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 56, 52, 66, 61, 94, 55, 61, 44, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 1120, 0, 295, 0, 1906, 0, 2605, 0, 3530, 0, 5773, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 53, 51, 69, 63, 106, 56, 64, 44, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1122, 0, 294, 0, 1963, 0, 2656, 0, 3552, 0, 5682, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 49, 51, 71, 64, 118, 58, 67, 44, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 1123, 0, 291, 0, 2016, 0, 2705, 0, 3575, 0, 5591, 0, 6864, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 46, 50, 74, 66, 128, 59, 69, 44, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1124, 0, 288, 0, 2064, 0, 2748, 0, 3598, 0, 5506, 0, 6820, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 42, 49, 75, 67, 137, 60, 71, 44, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1126, 0, 285, 0, 2104, 0, 2786, 0, 3618, 0, 5432, 0, 6782, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 39, 49, 77, 68, 144, 61, 73, 44, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1128, 0, 282, 0, 2135, 0, 2815, 0, 3634, 0, 5373, 0, 6753, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 37, 48, 78, 69, 150, 61, 74, 45, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 1129, 0, 280, 0, 2156, 0, 2834, 0, 3646, 0, 5332, 0, 6733, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 35, 48, 79, 70, 154, 62, 74, 45, 80, 35, 80, 0, 0, 58, 60 }},
		{{ 1131, 0, 279, 0, 2167, 0, 2845, 0, 3652, 0, 5310, 0, 6723, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 79, 70, 155, 62, 75, 45, 80, 35, 80, 0, 0, 56, 60 }},
		{{ 1132, 0, 279, 0, 2167, 0, 2845, 0, 3652, 0, 5310, 0, 6723, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 79, 70, 155, 62, 75, 45, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 1134, 0, 294, 0, 2129, 0, 2830, 0, 3654, 0, 5340, 0, 6735, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 35, 48, 79, 69, 153, 62, 74, 45, 80, 35, 80, 0, 0, 49, 60 }},
		{{ 1135, 0, 323, 0, 2048, 0, 2798, 0, 3656, 0, 5407, 0, 6764, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 37, 50, 78, 69, 147, 61, 71, 45, 80, 36, 80, 0, 0, 45, 60 }},
		{{ 1137, 0, 364, 0, 1931, 0, 2758, 0, 3661, 0, 5500, 0, 6803, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 39, 52, 77, 67, 139, 60, 68, 46, 80, 36, 80, 0, 0, 39, 60 }},
		{{ 1138, 0, 417, 0, 1783, 0, 2719, 0, 3671, 0, 5609, 0, 6850, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 41, 54, 75, 66, 129, 58, 65, 46, 80, 37, 80, 0, 0, 35, 60 }},
		{{ 1140, 0, 476, 0, 1610, 0, 2693, 0, 3688, 0, 5722, 0, 6900, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 44, 57, 73, 64, 118, 57, 61, 46, 80, 37, 80, 0, 0, 31, 60 }},
		{{ 1141, 0, 519, 0, 1482, 0, 2690, 0, 3707, 0, 5793, 0, 6932, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 45, 59, 71, 63, 110, 56, 59, 46, 80, 38, 80, 0, 0, 28, 60 }},
		{{ 1143, 0, 549, 0, 1391, 0, 2698, 0, 3723, 0, 5836, 0, 6952, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 46, 60, 70, 62, 104, 55, 57, 47, 80, 38, 80, 0, 0, 26, 60 }},
		{{ 1144, 0, 568, 0, 1333, 0, 2708, 0, 3735, 0, 5859, 0, 6963, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 61, 69, 61, 101, 55, 57, 47, 80, 39, 80, 0, 0, 25, 60 }},
		{{ 1146, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1147, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1149, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1150, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1154, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1157, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1161, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1164, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1168, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1171, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1175, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1179, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1182, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1186, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1189, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1193, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1196, 0, 462, 0, 1513, 0, 2553, 0, 3535, 0, 5746, 0, 6910, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1200, 0, 466, 0, 1520, 0, 2560, 0, 3550, 0, 5757, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1235, 0, 474, 0, 1530, 0, 2570, 0, 3573, 0, 5773, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 49, 58, 68, 61, 104, 54, 62, 45, 80, 37, 80, 0, 0, 50, 60 }},
		{{ 1270, 0, 484, 0, 1544, 0, 2585, 0, 3603, 0, 5793, 0, 6911, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 103, 54, 62, 45, 80, 37, 80, 0, 0, 51, 60 }},
		{{ 1276, 0, 496, 0, 1563, 0, 2603, 0, 3638, 0, 5816, 0, 6916, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 102, 54, 62, 45, 80, 37, 80, 0, 0, 53, 60 }},
		{{ 1282, 0, 512, 0, 1587, 0, 2626, 0, 3679, 0, 5840, 0, 6927, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 50, 58, 69, 62, 101, 55, 63, 46, 80, 37, 80, 0, 0, 55, 60 }},
		{{ 1288, 0, 530, 0, 1616, 0, 2654, 0, 3722, 0, 5864, 0, 6945, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 70, 62, 101, 55, 65, 46, 80, 38, 80, 0, 0, 57, 60 }},
		{{ 1294, 0, 552, 0, 1652, 0, 2686, 0, 3767, 0, 5885, 0, 6975, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 72, 62, 102, 55, 67, 46, 80, 38, 80, 0, 0, 58, 60 }},
		{{ 1300, 0, 575, 0, 1691, 0, 2720, 0, 3808, 0, 5900, 0, 7012, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 74, 63, 103, 56, 71, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1306, 0, 595, 0, 1726, 0, 2750, 0, 3840, 0, 5907, 0, 7053, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 76, 63, 105, 56, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1312, 0, 612, 0, 1758, 0, 2777, 0, 3864, 0, 5910, 0, 7092, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 50, 59, 78, 64, 108, 57, 79, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1318, 0, 627, 0, 1786, 0, 2800, 0, 3882, 0, 5910, 0, 7129, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 79, 64, 110, 57, 83, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1324, 0, 640, 0, 1809, 0, 2819, 0, 3896, 0, 5908, 0, 7161, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 81, 64, 112, 58, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1330, 0, 649, 0, 1826, 0, 2833, 0, 3905, 0, 5905, 0, 7186, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 82, 65, 113, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1336, 0, 655, 0, 1838, 0, 2843, 0, 3911, 0, 5903, 0, 7204, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1342, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5901, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1348, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5901, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1354, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5899, 0, 7211, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1360, 0, 655, 0, 1840, 0, 2845, 0, 3913, 0, 5895, 0, 7202, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 114, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1366, 0, 651, 0, 1834, 0, 2841, 0, 3912, 0, 5890, 0, 7188, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 83, 65, 114, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1372, 0, 646, 0, 1827, 0, 2836, 0, 3910, 0, 5883, 0, 7171, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 82, 65, 112, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1378, 0, 639, 0, 1818, 0, 2829, 0, 3907, 0, 5874, 0, 7149, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 81, 64, 111, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1384, 0, 631, 0, 1807, 0, 2819, 0, 3903, 0, 5864, 0, 7124, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 80, 64, 109, 57, 88, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1390, 0, 622, 0, 1795, 0, 2807, 0, 3896, 0, 5853, 0, 7097, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 79, 64, 107, 57, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1396, 0, 612, 0, 1781, 0, 2791, 0, 3886, 0, 5841, 0, 7066, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 53, 58, 78, 64, 105, 56, 86, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1402, 0, 602, 0, 1766, 0, 2772, 0, 3873, 0, 5829, 0, 7034, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 53, 58, 76, 63, 102, 56, 84, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1408, 0, 590, 0, 1750, 0, 2748, 0, 3855, 0, 5816, 0, 7001, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 54, 58, 75, 63, 100, 55, 82, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1414, 0, 578, 0, 1733, 0, 2719, 0, 3832, 0, 5804, 0, 6967, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 58, 73, 62, 97, 55, 80, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1420, 0, 565, 0, 1715, 0, 2684, 0, 3802, 0, 5792, 0, 6934, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 71, 62, 93, 54, 77, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1426, 0, 551, 0, 1697, 0, 2642, 0, 3766, 0, 5782, 0, 6902, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 70, 61, 89, 53, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1432, 0, 538, 0, 1678, 0, 2592, 0, 3720, 0, 5773, 0, 6872, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 68, 60, 86, 52, 72, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1438, 0, 524, 0, 1660, 0, 2533, 0, 3665, 0, 5766, 0, 6846, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 66, 60, 81, 52, 69, 46, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1444, 0, 512, 0, 1644, 0, 2473, 0, 3607, 0, 5762, 0, 6826, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 64, 59, 77, 51, 67, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1450, 0, 501, 0, 1630, 0, 2413, 0, 3548, 0, 5760, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 62, 58, 74, 50, 64, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1456, 0, 492, 0, 1618, 0, 2354, 0, 3489, 0, 5761, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 60, 57, 70, 49, 62, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1462, 0, 484, 0, 1608, 0, 2297, 0, 3430, 0, 5763, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 59, 56, 67, 49, 60, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1468, 0, 477, 0, 1599, 0, 2242, 0, 3374, 0, 5766, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 58, 55, 65, 48, 58, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1474, 0, 471, 0, 1592, 0, 2191, 0, 3321, 0, 5770, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 57, 54, 62, 48, 57, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1480, 0, 467, 0, 1586, 0, 2144, 0, 3272, 0, 5774, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 56, 54, 60, 47, 55, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1486, 0, 463, 0, 1581, 0, 2101, 0, 3227, 0, 5779, 0, 6802, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 55, 53, 58, 47, 54, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1492, 0, 459, 0, 1578, 0, 2063, 0, 3188, 0, 5783, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 54, 53, 56, 46, 53, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1498, 0, 457, 0, 1574, 0, 2031, 0, 3154, 0, 5787, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 58, 54, 52, 55, 46, 52, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1504, 0, 455, 0, 1572, 0, 2005, 0, 3126, 0, 5790, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 54, 46, 52, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1510, 0, 453, 0, 1570, 0, 1986, 0, 3105, 0, 5793, 0, 6816, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1516, 0, 452, 0, 1569, 0, 1972, 0, 3091, 0, 5795, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1522, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1528, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1534, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1540, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1546, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1552, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1558, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1564, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1570, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1576, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 53, 60 }},
		{{ 1582, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 47, 60 }},
		{{ 1588, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 40, 60 }},
		{{ 1594, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 33, 60 }},
		{{ 1600, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 27, 60 }},
		{{ 1606, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 20, 60 }},
		{{ 1612, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 14, 60 }},
		{{ 1618, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 7, 60 }},
		{{ 1624, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1630, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1626, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1622, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1618, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1613, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1609, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1605, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1601, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1597, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1593, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1589, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1585, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1580, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1576, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1572, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1568, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1564, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1560, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1556, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1552, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1547, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1543, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1539, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1535, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1531, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1527, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1523, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1519, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1514, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1510, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1506, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1502, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1498, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1494, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1490, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1486, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1481, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1477, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1473, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1469, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1465, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1461, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1457, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1453, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1448, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1444, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1440, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1436, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1432, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1428, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1424, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1420, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1415, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1411, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1407, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1403, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1399, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1395, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1391, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1387, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1382, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1378, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1374, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1370, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1366, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1362, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1358, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1354, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1349, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1345, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1341, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1337, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1333, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1329, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1325, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1321, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1316, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1312, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1308, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1304, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1285, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1270, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1255, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1240, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1225, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1210, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1195, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1180, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1165, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1150, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1135, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1120, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1105, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1090, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1075, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1060, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1045, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1030, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1015, 0, 523, 0, 2043, 0, 2848, 0, 3848, 0, 5725, 0, 7619, 0, 0, 0, 200, 30, 0, 60, 0, 0, 36, 0, 0, 100, 55, 100, 70, 100, 70, 100, 55, 100, 35, 100, 0, 0, 48, 60 }},
		{{ 1000, 0, 545, 0, 2007, 0, 2830, 0, 3836, 0, 5758, 0, 7582, 0, 0, 0, 200, 30, 0, 60, 0, 0, 36, 0, 0, 100, 55, 100, 70, 100, 70, 100, 55, 100, 35, 100, 0, 0, 48, 60 }},
		{{ 975, 0, 576, 0, 1954, 0, 2806, 0, 3820, 0, 5807, 0, 7529, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 34, 57, 84, 63, 109, 60, 92, 44, 80, 36, 80, 0, 0, 50, 60 }},
		{{ 950, 0, 616, 0, 1885, 0, 2776, 0, 3804, 0, 5869, 0, 7464, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 36, 58, 82, 63, 109, 59, 88, 44, 80, 37, 80, 0, 0, 52, 60 }},
		{{ 950, 0, 662, 0, 1802, 0, 2744, 0, 3793, 0, 5943, 0, 7392, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 45, 38, 59, 80, 62, 109, 59, 82, 45, 80, 38, 80, 0, 0, 54, 60 }},
		{{ 951, 0, 712, 0, 1705, 0, 2713, 0, 3792, 0, 6027, 0, 7318, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 47, 39, 60, 78, 62, 107, 58, 74, 45, 80, 39, 80, 0, 0, 56, 60 }},
		{{ 951, 0, 763, 0, 1598, 0, 2689, 0, 3809, 0, 6118, 0, 7249, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 49, 39, 62, 76, 62, 104, 58, 63, 47, 80, 39, 80, 0, 0, 58, 60 }},
		{{ 952, 0, 800, 0, 1511, 0, 2677, 0, 3839, 0, 6190, 0, 7206, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 50, 37, 63, 76, 63, 100, 57, 52, 49, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 952, 0, 827, 0, 1442, 0, 2674, 0, 3874, 0, 6246, 0, 7180, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 51, 35, 64, 76, 63, 96, 57, 42, 51, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 952, 0, 845, 0, 1389, 0, 2676, 0, 3909, 0, 6288, 0, 7167, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 33, 65, 77, 63, 93, 57, 33, 53, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 953, 0, 858, 0, 1350, 0, 2680, 0, 3940, 0, 6318, 0, 7161, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 31, 65, 77, 64, 90, 57, 26, 55, 80, 39, 80, 0, 0, 59, 60 }},
		{{ 953, 0, 865, 0, 1325, 0, 2683, 0, 3962, 0, 6337, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 30, 66, 78, 64, 88, 57, 21, 56, 80, 39, 80, 0, 0, 57, 60 }},
		{{ 954, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 55, 60 }},
		{{ 954, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 53, 60 }},
		{{ 955, 0, 864, 0, 1330, 0, 2683, 0, 3958, 0, 6334, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 30, 65, 77, 64, 88, 57, 22, 56, 80, 39, 80, 0, 0, 50, 60 }},
		{{ 955, 0, 853, 0, 1364, 0, 2678, 0, 3928, 0, 6307, 0, 7163, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 32, 65, 77, 64, 91, 57, 28, 54, 80, 39, 80, 0, 0, 46, 60 }},
		{{ 955, 0, 836, 0, 1418, 0, 2674, 0, 3889, 0, 6266, 0, 7173, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 51, 34, 64, 76, 63, 95, 57, 38, 52, 80, 39, 80, 0, 0, 42, 60 }},
		{{ 956, 0, 808, 0, 1492, 0, 2676, 0, 3848, 0, 6206, 0, 7198, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 51, 37, 63, 76, 63, 99, 57, 49, 50, 80, 39, 80, 0, 0, 38, 60 }},
		{{ 956, 0, 767, 0, 1590, 0, 2687, 0, 3811, 0, 6125, 0, 7245, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 49, 39, 62, 76, 62, 104, 58, 62, 47, 80, 39, 80, 0, 0, 35, 60 }},
		{{ 957, 0, 707, 0, 1715, 0, 2716, 0, 3792, 0, 6019, 0, 7324, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 47, 39, 60, 78, 62, 107, 58, 75, 45, 80, 38, 80, 0, 0, 32, 60 }},
		{{ 957, 0, 649, 0, 1825, 0, 2752, 0, 3795, 0, 5923, 0, 7411, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 45, 38, 59, 80, 63, 109, 59, 84, 45, 80, 38, 80, 0, 0, 29, 60 }},
		{{ 957, 0, 598, 0, 1917, 0, 2789, 0, 3810, 0, 5840, 0, 7493, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 35, 58, 83, 63, 109, 60, 90, 44, 80, 37, 80, 0, 0, 27, 60 }},
		{{ 958, 0, 556, 0, 1988, 0, 2821, 0, 3829, 0, 5776, 0, 7562, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 33, 57, 85, 63, 109, 60, 93, 45, 80, 36, 80, 0, 0, 26, 60 }},
		{{ 958, 0, 527, 0, 2037, 0, 2845, 0, 3846, 0, 5731, 0, 7612, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 31, 56, 87, 64, 109, 61, 95, 45, 80, 35, 80, 0, 0, 25, 60 }},
		{{ 959, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 30, 56, 88, 64, 109, 61, 96, 45, 80, 35, 80, 0, 0, 24, 60 }},
		{{ 959, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 30, 56, 88, 64, 109, 61, 96, 45, 80, 35, 80, 0, 0, 24, 60 }},
		{{ 960, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 960, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 961, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 962, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 964, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 965, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 966, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 968, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 29, 0, 78, 0, 86, 0, 18, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 969, 0, 857, 0, 1328, 0, 2698, 0, 3979, 0, 6330, 0, 7151, 0, 0, 0, 200, 30, 0, 60, 0, 0, 36, 0, 0, 100, 55, 100, 70, 100, 70, 100, 55, 100, 35, 100, 0, 0, 48, 60 }},
		{{ 970, 0, 833, 0, 1358, 0, 2723, 0, 3989, 0, 6297, 0, 7136, 0, 0, 0, 200, 30, 0, 60, 0, 0, 36, 0, 0, 100, 55, 100, 70, 100, 70, 100, 55, 100, 35, 100, 0, 0, 48, 60 }},
		{{ 980, 0, 797, 0, 1404, 0, 2756, 0, 4000, 0, 6245, 0, 7113, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 49, 30, 64, 80, 66, 100, 58, 26, 55, 80, 38, 80, 0, 0, 50, 60 }},
		{{ 990, 0, 749, 0, 1467, 0, 2795, 0, 4008, 0, 6174, 0, 7081, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 47, 31, 63, 81, 67, 109, 58, 31, 55, 80, 38, 80, 0, 0, 52, 60 }},
		{{ 990, 0, 689, 0, 1548, 0, 2835, 0, 4006, 0, 6080, 0, 7041, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 45, 31, 61, 83, 68, 118, 59, 37, 53, 80, 38, 80, 0, 0, 54, 60 }},
		{{ 990, 0, 617, 0, 1648, 0, 2870, 0, 3986, 0, 5960, 0, 6989, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 32, 59, 83, 69, 128, 60, 44, 52, 80, 37, 80, 0, 0, 56, 60 }},
		{{ 990, 0, 533, 0, 1771, 0, 2893, 0, 3941, 0, 5811, 0, 6927, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 33, 57, 84, 69, 138, 60, 53, 50, 80, 37, 80, 0, 0, 58, 60 }},
		{{ 990, 0, 460, 0, 1880, 0, 2898, 0, 3882, 0, 5676, 0, 6871, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 33, 54, 83, 70, 145, 61, 59, 49, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 990, 0, 400, 0, 1974, 0, 2891, 0, 3818, 0, 5557, 0, 6822, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 34, 52, 82, 70, 150, 61, 65, 47, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 990, 0, 351, 0, 2051, 0, 2878, 0, 3758, 0, 5459, 0, 6782, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 34, 50, 81, 70, 153, 62, 69, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 990, 0, 315, 0, 2110, 0, 2864, 0, 3708, 0, 5383, 0, 6752, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 34, 49, 80, 70, 154, 62, 72, 46, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 990, 0, 291, 0, 2150, 0, 2853, 0, 3672, 0, 5331, 0, 6731, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 48, 80, 70, 156, 62, 74, 45, 80, 35, 80, 0, 0, 57, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 55, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 990, 0, 289, 0, 2141, 0, 2835, 0, 3654, 0, 5330, 0, 6731, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 35, 48, 79, 70, 154, 62, 74, 45, 80, 35, 80, 0, 0, 50, 60 }},
		{{ 990, 0, 309, 0, 2085, 0, 2812, 0, 3655, 0, 5377, 0, 6751, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 36, 49, 79, 69, 150, 61, 72, 45, 80, 36, 80, 0, 0, 46, 60 }},
		{{ 990, 0, 339, 0, 2002, 0, 2782, 0, 3658, 0, 5445, 0, 6780, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 37, 51, 78, 68, 144, 60, 70, 45, 80, 36, 80, 0, 0, 42, 60 }},
		{{ 990, 0, 377, 0, 1896, 0, 2748, 0, 3663, 0, 5528, 0, 6815, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 39, 52, 76, 67, 137, 59, 67, 46, 80, 36, 80, 0, 0, 38, 60 }},
		{{ 990, 0, 422, 0, 1768, 0, 2716, 0, 3672, 0, 5620, 0, 6855, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 41, 54, 75, 66, 128, 58, 64, 46, 80, 37, 80, 0, 0, 35, 60 }},
		{{ 990, 0, 471, 0, 1624, 0, 2694, 0, 3687, 0, 5714, 0, 6896, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 44, 57, 73, 64, 119, 57, 61, 46, 80, 37, 80, 0, 0, 32, 60 }},
		{{ 990, 0, 509, 0, 1511, 0, 2689, 0, 3702, 0, 5778, 0, 6925, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 45, 58, 72, 63, 112, 56, 59, 46, 80, 38, 80, 0, 0, 29, 60 }},
		{{ 990, 0, 538, 0, 1426, 0, 2694, 0, 3716, 0, 5820, 0, 6945, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 46, 60, 71, 62, 106, 55, 58, 47, 80, 38, 80, 0, 0, 27, 60 }},
		{{ 990, 0, 558, 0, 1364, 0, 2702, 0, 3728, 0, 5847, 0, 6957, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 60, 70, 62, 103, 55, 57, 47, 80, 39, 80, 0, 0, 26, 60 }},
		{{ 990, 0, 570, 0, 1325, 0, 2710, 0, 3737, 0, 5862, 0, 6965, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 61, 69, 61, 100, 55, 56, 47, 80, 39, 80, 0, 0, 25, 60 }},
		{{ 990, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 990, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 990, 0, 461, 0, 1512, 0, 2551, 0, 3531, 0, 5744, 0, 6910, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 990, 0, 463, 0, 1516, 0, 2555, 0, 3541, 0, 5750, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 995, 0, 468, 0, 1522, 0, 2562, 0, 3555, 0, 5760, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 48, 58, 67, 61, 104, 54, 62, 45, 80, 37, 80, 0, 0, 49, 60 }},
		{{ 1000, 0, 474, 0, 1530, 0, 2570, 0, 3573, 0, 5773, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 49, 58, 68, 61, 104, 54, 62, 45, 80, 37, 80, 0, 0, 50, 60 }},
		{{ 1002, 0, 481, 0, 1540, 0, 2581, 0, 3595, 0, 5788, 0, 6910, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 103, 54, 62, 45, 80, 37, 80, 0, 0, 52, 60 }},
		{{ 1005, 0, 490, 0, 1554, 0, 2594, 0, 3621, 0, 5805, 0, 6913, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 102, 54, 62, 45, 80, 37, 80, 0, 0, 53, 60 }},
		{{ 1008, 0, 501, 0, 1570, 0, 2610, 0, 3650, 0, 5823, 0, 6919, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 102, 54, 62, 46, 80, 37, 80, 0, 0, 54, 60 }},
		{{ 1010, 0, 513, 0, 1589, 0, 2628, 0, 3682, 0, 5842, 0, 6928, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 69, 62, 101, 55, 63, 46, 80, 38, 80, 0, 0, 55, 60 }},
		{{ 1012, 0, 527, 0, 1611, 0, 2649, 0, 3715, 0, 5860, 0, 6942, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 70, 62, 101, 55, 64, 46, 80, 38, 80, 0, 0, 56, 60 }},
		{{ 1015, 0, 543, 0, 1638, 0, 2673, 0, 3750, 0, 5877, 0, 6962, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 71, 62, 102, 55, 66, 46, 80, 38, 80, 0, 0, 58, 60 }},
		{{ 1018, 0, 562, 0, 1668, 0, 2700, 0, 3785, 0, 5892, 0, 6990, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 72, 63, 102, 55, 69, 46, 80, 38, 80, 0, 0, 59, 60 }},
		{{ 1020, 0, 578, 0, 1698, 0, 2726, 0, 3815, 0, 5902, 0, 7020, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 74, 63, 104, 56, 72, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1022, 0, 594, 0, 1725, 0, 2749, 0, 3839, 0, 5907, 0, 7051, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 76, 63, 105, 56, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1025, 0, 608, 0, 1750, 0, 2771, 0, 3858, 0, 5910, 0, 7082, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 50, 58, 77, 64, 107, 57, 78, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1028, 0, 621, 0, 1773, 0, 2790, 0, 3874, 0, 5910, 0, 7112, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 79, 64, 109, 57, 82, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1030, 0, 631, 0, 1793, 0, 2806, 0, 3887, 0, 5909, 0, 7139, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 80, 64, 110, 57, 84, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1032, 0, 640, 0, 1810, 0, 2820, 0, 3897, 0, 5907, 0, 7163, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 81, 64, 112, 58, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1035, 0, 648, 0, 1824, 0, 2832, 0, 3904, 0, 5905, 0, 7183, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 82, 65, 113, 58, 89, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1038, 0, 653, 0, 1835, 0, 2840, 0, 3909, 0, 5903, 0, 7199, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 83, 65, 114, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1040, 0, 657, 0, 1842, 0, 2846, 0, 3913, 0, 5902, 0, 7210, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1042, 0, 659, 0, 1846, 0, 2849, 0, 3914, 0, 5901, 0, 7215, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 49, 60, 83, 65, 115, 58, 93, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1045, 0, 659, 0, 1846, 0, 2849, 0, 3914, 0, 5901, 0, 7215, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 49, 60, 83, 65, 115, 58, 93, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1048, 0, 658, 0, 1845, 0, 2848, 0, 3914, 0, 5900, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 93, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1050, 0, 657, 0, 1843, 0, 2847, 0, 3914, 0, 5898, 0, 7207, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1052, 0, 654, 0, 1839, 0, 2845, 0, 3913, 0, 5895, 0, 7200, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 114, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1055, 0, 651, 0, 1835, 0, 2842, 0, 3912, 0, 5891, 0, 7190, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 83, 65, 114, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1058, 0, 648, 0, 1830, 0, 2838, 0, 3911, 0, 5886, 0, 7177, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 82, 65, 113, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1060, 0, 643, 0, 1824, 0, 2833, 0, 3909, 0, 5880, 0, 7163, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 82, 65, 112, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1062, 0, 638, 0, 1816, 0, 2828, 0, 3907, 0, 5873, 0, 7146, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 81, 64, 111, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1065, 0, 632, 0, 1808, 0, 2820, 0, 3903, 0, 5865, 0, 7128, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 80, 64, 110, 57, 89, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1068, 0, 626, 0, 1799, 0, 2812, 0, 3898, 0, 5857, 0, 7107, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 79, 64, 108, 57, 88, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1070, 0, 619, 0, 1789, 0, 2801, 0, 3892, 0, 5849, 0, 7085, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 79, 64, 106, 57, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1072, 0, 611, 0, 1779, 0, 2789, 0, 3884, 0, 5840, 0, 7062, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 53, 58, 78, 64, 105, 56, 85, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1075, 0, 603, 0, 1768, 0, 2775, 0, 3874, 0, 5831, 0, 7038, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 53, 58, 77, 63, 103, 56, 84, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1078, 0, 594, 0, 1756, 0, 2758, 0, 3862, 0, 5821, 0, 7014, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 54, 58, 75, 63, 101, 56, 83, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1080, 0, 585, 0, 1743, 0, 2738, 0, 3847, 0, 5812, 0, 6988, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 54, 58, 74, 63, 99, 55, 81, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1082, 0, 576, 0, 1730, 0, 2715, 0, 3828, 0, 5803, 0, 6963, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 58, 73, 62, 96, 55, 79, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1085, 0, 566, 0, 1717, 0, 2689, 0, 3806, 0, 5794, 0, 6938, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 72, 62, 94, 54, 78, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1088, 0, 556, 0, 1703, 0, 2659, 0, 3780, 0, 5786, 0, 6914, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 70, 61, 91, 54, 76, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1090, 0, 546, 0, 1690, 0, 2624, 0, 3749, 0, 5778, 0, 6890, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 69, 61, 88, 53, 74, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1092, 0, 536, 0, 1676, 0, 2585, 0, 3714, 0, 5772, 0, 6869, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 67, 60, 85, 52, 72, 46, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1095, 0, 525, 0, 1662, 0, 2541, 0, 3672, 0, 5766, 0, 6849, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 66, 60, 82, 52, 70, 46, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 1098, 0, 516, 0, 1649, 0, 2496, 0, 3629, 0, 5763, 0, 6833, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 64, 59, 79, 51, 68, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1100, 0, 507, 0, 1638, 0, 2451, 0, 3585, 0, 5761, 0, 6821, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 63, 58, 76, 50, 66, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1104, 0, 500, 0, 1628, 0, 2406, 0, 3541, 0, 5760, 0, 6811, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 62, 58, 73, 50, 64, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1107, 0, 493, 0, 1619, 0, 2362, 0, 3496, 0, 5761, 0, 6805, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 61, 57, 71, 49, 62, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 1110, 0, 487, 0, 1612, 0, 2318, 0, 3452, 0, 5762, 0, 6800, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 60, 56, 68, 49, 61, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1114, 0, 481, 0, 1605, 0, 2276, 0, 3409, 0, 5764, 0, 6798, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 59, 56, 66, 48, 60, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1117, 0, 476, 0, 1599, 0, 2236, 0, 3368, 0, 5766, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 58, 55, 64, 48, 58, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1121, 0, 472, 0, 1593, 0, 2198, 0, 3328, 0, 5769, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 57, 55, 62, 48, 57, 47, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1124, 0, 468, 0, 1588, 0, 2161, 0, 3290, 0, 5773, 0, 6798, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 56, 54, 61, 47, 56, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1128, 0, 465, 0, 1584, 0, 2127, 0, 3255, 0, 5776, 0, 6800, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 55, 54, 59, 47, 55, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1131, 0, 462, 0, 1581, 0, 2096, 0, 3222, 0, 5779, 0, 6803, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 55, 53, 58, 47, 54, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1135, 0, 460, 0, 1578, 0, 2068, 0, 3193, 0, 5783, 0, 6805, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 54, 53, 56, 46, 53, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1138, 0, 458, 0, 1576, 0, 2043, 0, 3166, 0, 5786, 0, 6808, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 58, 54, 52, 55, 46, 53, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1142, 0, 456, 0, 1573, 0, 2021, 0, 3143, 0, 5788, 0, 6811, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 58, 54, 52, 54, 46, 52, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1145, 0, 454, 0, 1572, 0, 2003, 0, 3124, 0, 5791, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 54, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1149, 0, 453, 0, 1571, 0, 1988, 0, 3108, 0, 5793, 0, 6815, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1152, 0, 452, 0, 1570, 0, 1977, 0, 3096, 0, 5794, 0, 6817, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 53, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1156, 0, 452, 0, 1569, 0, 1969, 0, 3088, 0, 5795, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1159, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1163, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1166, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1170, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1173, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1177, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1180, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1184, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1187, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1191, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1194, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1198, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 55, 60 }},
		{{ 1201, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 50, 60 }},
		{{ 1205, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 45, 60 }},
		{{ 1208, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 40, 60 }},
		{{ 1212, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 35, 60 }},
		{{ 1215, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 30, 60 }},
		{{ 1219, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 25, 60 }},
		{{ 1222, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 20, 60 }},
		{{ 1226, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 15, 60 }},
		{{ 1229, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 10, 60 }},
		{{ 1233, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 5, 60 }},
		{{ 1236, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1240, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1244, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1249, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1253, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1257, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1262, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1266, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1270, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1275, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1279, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1283, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1288, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1292, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1296, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1301, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1305, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1309, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1314, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1318, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1322, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1327, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1331, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1335, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1340, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1344, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1348, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1353, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1357, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1361, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1366, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1370, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1374, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1379, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1383, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1387, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1392, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1396, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1400, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1405, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1409, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1413, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1418, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1422, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1426, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1431, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1435, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1439, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1444, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1448, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1452, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1457, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1461, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1465, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1470, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1474, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1478, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1483, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1487, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1491, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1496, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 39, 60 }},
		{{ 1500, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 42, 60 }},
		{{ 1498, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 45, 60 }},
		{{ 1495, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 48, 60 }},
		{{ 1492, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 51, 60 }},
		{{ 1490, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 54, 60 }},
		{{ 1488, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 57, 60 }},
		{{ 1485, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1482, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1480, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1478, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 1475, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 1, 0, 60, 60 }},
		{{ 1472, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 2, 0, 60, 60 }},
		{{ 1470, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 4, 0, 60, 60 }},
		{{ 1468, 0, 868, 0, 1313, 0, 2686, 0, 3974, 0, 6346, 0, 7159, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 52, 29, 66, 78, 64, 86, 56, 18, 57, 80, 39, 80, 5, 0, 60, 60 }},
		{{ 1465, 0, 858, 0, 1312, 0, 2712, 0, 3995, 0, 6326, 0, 7131, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 51, 29, 66, 78, 65, 89, 57, 19, 57, 80, 39, 80, 7, 0, 59, 60 }},
		{{ 1462, 0, 837, 0, 1312, 0, 2761, 0, 4033, 0, 6282, 0, 7081, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 50, 30, 65, 79, 66, 94, 57, 21, 57, 80, 39, 80, 10, 0, 58, 60 }},
		{{ 1460, 0, 806, 0, 1319, 0, 2823, 0, 4073, 0, 6213, 0, 7015, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 48, 30, 65, 81, 67, 101, 57, 24, 56, 80, 39, 80, 13, 0, 56, 60 }},
		{{ 1458, 0, 767, 0, 1340, 0, 2883, 0, 4099, 0, 6114, 0, 6946, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 45, 28, 65, 84, 68, 111, 58, 28, 56, 80, 39, 80, 16, 0, 54, 60 }},
		{{ 1455, 0, 721, 0, 1386, 0, 2922, 0, 4085, 0, 5977, 0, 6890, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 23, 64, 87, 70, 124, 59, 33, 56, 80, 38, 80, 19, 0, 51, 60 }},
		{{ 1452, 0, 687, 0, 1439, 0, 2926, 0, 4038, 0, 5858, 0, 6869, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 64, 90, 70, 134, 59, 37, 56, 80, 38, 80, 22, 0, 49, 60 }},
		{{ 1450, 0, 663, 0, 1487, 0, 2913, 0, 3981, 0, 5763, 0, 6868, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 64, 92, 71, 142, 60, 41, 56, 80, 37, 80, 25, 0, 48, 60 }},
		{{ 1448, 0, 647, 0, 1524, 0, 2896, 0, 3932, 0, 5697, 0, 6873, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 94, 71, 147, 60, 43, 56, 80, 37, 80, 28, 0, 46, 60 }},
		{{ 1445, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 30, 0, 45, 60 }},
		{{ 1442, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 31, 0, 45, 60 }},
		{{ 1440, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 33, 0, 45, 60 }},
		{{ 1438, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 34, 0, 45, 60 }},
		{{ 1435, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1432, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1430, 0, 640, 0, 1543, 0, 2885, 0, 3905, 0, 5663, 0, 6878, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 56, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1428, 0, 640, 0, 1542, 0, 2883, 0, 3905, 0, 5666, 0, 6876, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 95, 71, 150, 61, 44, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1425, 0, 642, 0, 1539, 0, 2879, 0, 3903, 0, 5674, 0, 6872, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 94, 71, 149, 60, 45, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1422, 0, 644, 0, 1535, 0, 2874, 0, 3902, 0, 5685, 0, 6865, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 93, 70, 148, 60, 45, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1420, 0, 646, 0, 1529, 0, 2867, 0, 3899, 0, 5699, 0, 6857, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 92, 70, 146, 60, 46, 55, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1418, 0, 649, 0, 1523, 0, 2858, 0, 3897, 0, 5716, 0, 6848, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 91, 70, 144, 60, 46, 54, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1415, 0, 653, 0, 1516, 0, 2848, 0, 3894, 0, 5736, 0, 6838, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 90, 69, 142, 59, 47, 54, 80, 37, 80, 35, 0, 45, 60 }},
		{{ 1412, 0, 656, 0, 1509, 0, 2837, 0, 3890, 0, 5757, 0, 6827, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 88, 69, 139, 59, 48, 53, 80, 38, 80, 35, 0, 45, 60 }},
		{{ 1410, 0, 660, 0, 1503, 0, 2825, 0, 3886, 0, 5781, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 63, 86, 68, 136, 59, 49, 53, 80, 38, 80, 35, 0, 45, 60 }},
		{{ 1408, 0, 663, 0, 1498, 0, 2813, 0, 3882, 0, 5805, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 62, 85, 68, 133, 58, 50, 52, 80, 38, 80, 35, 0, 45, 60 }},
		{{ 1405, 0, 667, 0, 1494, 0, 2800, 0, 3878, 0, 5830, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 62, 83, 67, 130, 58, 51, 51, 80, 39, 80, 35, 0, 45, 60 }},
		{{ 1402, 0, 669, 0, 1492, 0, 2788, 0, 3873, 0, 5855, 0, 6801, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 32, 0, 81, 0, 126, 0, 53, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1400, 0, 671, 0, 1494, 0, 2776, 0, 3869, 0, 5878, 0, 6803, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 36, 0, 79, 0, 123, 0, 55, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1398, 0, 671, 0, 1499, 0, 2765, 0, 3864, 0, 5899, 0, 6811, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 39, 0, 78, 0, 120, 0, 57, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1396, 0, 670, 0, 1509, 0, 2755, 0, 3860, 0, 5918, 0, 6826, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 42, 0, 76, 0, 116, 0, 59, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1394, 0, 667, 0, 1525, 0, 2748, 0, 3856, 0, 5932, 0, 6849, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 45, 0, 75, 0, 113, 0, 61, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1392, 0, 662, 0, 1547, 0, 2743, 0, 3852, 0, 5941, 0, 6882, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 74, 0, 111, 0, 64, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1390, 0, 655, 0, 1578, 0, 2741, 0, 3849, 0, 5944, 0, 6926, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 74, 0, 108, 0, 67, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1388, 0, 646, 0, 1613, 0, 2743, 0, 3847, 0, 5940, 0, 6977, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 74, 0, 106, 0, 70, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1385, 0, 635, 0, 1651, 0, 2748, 0, 3846, 0, 5930, 0, 7033, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 74, 0, 105, 0, 73, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1383, 0, 624, 0, 1691, 0, 2754, 0, 3846, 0, 5917, 0, 7092, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 75, 0, 104, 0, 76, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1381, 0, 612, 0, 1732, 0, 2763, 0, 3845, 0, 5899, 0, 7153, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 48, 0, 76, 0, 104, 0, 79, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1379, 0, 599, 0, 1773, 0, 2773, 0, 3846, 0, 5880, 0, 7214, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 46, 0, 77, 0, 104, 0, 81, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1377, 0, 587, 0, 1815, 0, 2783, 0, 3846, 0, 5859, 0, 7274, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 44, 0, 78, 0, 104, 0, 84, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1375, 0, 575, 0, 1854, 0, 2794, 0, 3847, 0, 5837, 0, 7333, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 42, 0, 80, 0, 105, 0, 86, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1373, 0, 564, 0, 1892, 0, 2804, 0, 3848, 0, 5815, 0, 7388, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 40, 0, 81, 0, 105, 0, 88, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1371, 0, 553, 0, 1927, 0, 2815, 0, 3850, 0, 5795, 0, 7440, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 38, 0, 82, 0, 106, 0, 90, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1369, 0, 543, 0, 1959, 0, 2825, 0, 3851, 0, 5775, 0, 7487, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 37, 0, 84, 0, 106, 0, 91, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1367, 0, 535, 0, 1987, 0, 2833, 0, 3852, 0, 5757, 0, 7528, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 35, 0, 85, 0, 107, 0, 93, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1365, 0, 527, 0, 2011, 0, 2841, 0, 3853, 0, 5742, 0, 7564, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 33, 0, 86, 0, 108, 0, 94, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1363, 0, 521, 0, 2031, 0, 2847, 0, 3854, 0, 5729, 0, 7593, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 32, 0, 86, 0, 108, 0, 95, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1360, 0, 516, 0, 2046, 0, 2852, 0, 3854, 0, 5719, 0, 7615, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 31, 0, 87, 0, 108, 0, 95, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1358, 0, 513, 0, 2056, 0, 2856, 0, 3855, 0, 5712, 0, 7630, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 31, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1356, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1354, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 30, 0, 88, 0, 109, 0, 96, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1352, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 36, 0, 0, 100, 55, 100, 70, 100, 70, 100, 55, 100, 35, 100, 0, 0, 48, 60 }},
		{{ 1350, 0, 512, 0, 2061, 0, 2857, 0, 3855, 0, 5709, 0, 7638, 0, 0, 0, 200, 30, 0, 60, 0, 0, 36, 0, 0, 100, 55, 100, 70, 100, 70, 100, 55, 100, 35, 100, 0, 0, 48, 60 }},
		{{ 1325, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 47, 0, 69, 0, 99, 0, 56, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1300, 0, 463, 0, 1825, 0, 2082, 0, 2944, 0, 5259, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1300, 0, 465, 0, 1830, 0, 2122, 0, 2989, 0, 5250, 0, 6805, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1285, 0, 468, 0, 1839, 0, 2181, 0, 3055, 0, 5240, 0, 6787, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 16, 57, 74, 60, 131, 56, 72, 46, 80, 35, 80, 0, 0, 50, 60 }},
		{{ 1270, 0, 472, 0, 1850, 0, 2257, 0, 3141, 0, 5229, 0, 6765, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 16, 57, 75, 61, 135, 56, 72, 47, 80, 35, 80, 0, 0, 53, 60 }},
		{{ 1266, 0, 478, 0, 1863, 0, 2348, 0, 3246, 0, 5221, 0, 6741, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 16, 56, 77, 63, 140, 57, 71, 47, 80, 35, 80, 0, 0, 55, 60 }},
		{{ 1263, 0, 485, 0, 1879, 0, 2451, 0, 3367, 0, 5219, 0, 6716, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 15, 56, 80, 65, 145, 58, 70, 48, 80, 36, 80, 0, 0, 58, 60 }},
		{{ 1260, 0, 492, 0, 1893, 0, 2533, 0, 3464, 0, 5224, 0, 6699, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 15, 56, 82, 66, 149, 59, 69, 49, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1256, 0, 498, 0, 1903, 0, 2595, 0, 3540, 0, 5232, 0, 6688, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 14, 55, 83, 68, 152, 60, 68, 49, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1253, 0, 503, 0, 1911, 0, 2639, 0, 3595, 0, 5241, 0, 6681, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 13, 55, 84, 68, 155, 60, 66, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1249, 0, 506, 0, 1916, 0, 2668, 0, 3631, 0, 5248, 0, 6677, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 13, 55, 85, 69, 156, 60, 66, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1246, 0, 508, 0, 1919, 0, 2683, 0, 3649, 0, 5252, 0, 6675, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 13, 55, 86, 69, 157, 61, 65, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1242, 0, 508, 0, 1919, 0, 2683, 0, 3649, 0, 5252, 0, 6675, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 13, 55, 86, 69, 157, 61, 65, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1239, 0, 508, 0, 1920, 0, 2679, 0, 3647, 0, 5261, 0, 6676, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 14, 55, 85, 69, 156, 60, 65, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1235, 0, 507, 0, 1922, 0, 2673, 0, 3642, 0, 5281, 0, 6676, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 15, 55, 85, 69, 154, 60, 66, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1232, 0, 505, 0, 1924, 0, 2664, 0, 3637, 0, 5309, 0, 6679, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 18, 55, 84, 69, 152, 60, 66, 50, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1228, 0, 502, 0, 1929, 0, 2653, 0, 3630, 0, 5346, 0, 6684, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 21, 55, 83, 68, 149, 60, 66, 49, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1225, 0, 497, 0, 1937, 0, 2642, 0, 3623, 0, 5389, 0, 6693, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 24, 55, 81, 68, 144, 59, 67, 49, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1221, 0, 491, 0, 1947, 0, 2631, 0, 3617, 0, 5439, 0, 6708, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 28, 54, 80, 67, 140, 59, 67, 48, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1218, 0, 481, 0, 1962, 0, 2623, 0, 3614, 0, 5493, 0, 6731, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 32, 54, 79, 67, 134, 58, 68, 48, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1214, 0, 468, 0, 1983, 0, 2620, 0, 3615, 0, 5549, 0, 6767, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 36, 54, 78, 66, 128, 58, 69, 47, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1211, 0, 451, 0, 2010, 0, 2623, 0, 3622, 0, 5605, 0, 6816, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 40, 54, 78, 66, 122, 57, 70, 47, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 1207, 0, 428, 0, 2047, 0, 2636, 0, 3637, 0, 5660, 0, 6884, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 44, 54, 78, 65, 115, 57, 71, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1204, 0, 404, 0, 2084, 0, 2656, 0, 3657, 0, 5701, 0, 6957, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 46, 54, 79, 65, 109, 57, 71, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 1200, 0, 380, 0, 2121, 0, 2680, 0, 3680, 0, 5732, 0, 7030, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 48, 54, 81, 65, 104, 57, 72, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1198, 0, 357, 0, 2157, 0, 2706, 0, 3704, 0, 5754, 0, 7101, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 49, 54, 82, 66, 100, 57, 72, 48, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 1196, 0, 337, 0, 2190, 0, 2732, 0, 3727, 0, 5769, 0, 7168, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 49, 53, 84, 66, 97, 57, 73, 48, 80, 33, 80, 0, 0, 60, 60 }},
		{{ 1194, 0, 318, 0, 2219, 0, 2756, 0, 3749, 0, 5780, 0, 7227, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 49, 53, 86, 66, 94, 57, 73, 49, 80, 33, 80, 0, 0, 60, 60 }},
		{{ 1192, 0, 303, 0, 2243, 0, 2777, 0, 3768, 0, 5787, 0, 7276, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 49, 53, 87, 66, 92, 57, 73, 49, 80, 32, 80, 0, 0, 60, 60 }},
		{{ 1190, 0, 291, 0, 2262, 0, 2794, 0, 3783, 0, 5791, 0, 7315, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 49, 53, 88, 66, 90, 57, 74, 49, 80, 32, 80, 0, 0, 59, 60 }},
		{{ 1188, 0, 282, 0, 2274, 0, 2806, 0, 3793, 0, 5793, 0, 7341, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 49, 53, 89, 67, 89, 57, 74, 49, 80, 32, 80, 0, 0, 58, 60 }},
		{{ 1186, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 0, 0, 56, 60 }},
		{{ 1184, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 32, 49, 53, 89, 67, 89, 57, 74, 50, 80, 32, 80, 0, 0, 53, 60 }},
		{{ 1182, 0, 296, 0, 2239, 0, 2788, 0, 3785, 0, 5799, 0, 7313, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 33, 49, 54, 87, 66, 90, 57, 73, 49, 80, 32, 80, 0, 0, 49, 60 }},
		{{ 1180, 0, 331, 0, 2156, 0, 2745, 0, 3761, 0, 5809, 0, 7235, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 50, 54, 84, 65, 92, 57, 73, 48, 80, 33, 80, 0, 0, 45, 60 }},
		{{ 1178, 0, 379, 0, 2033, 0, 2693, 0, 3731, 0, 5821, 0, 7136, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 51, 54, 79, 64, 94, 56, 71, 47, 80, 34, 80, 0, 0, 39, 60 }},
		{{ 1176, 0, 436, 0, 1873, 0, 2648, 0, 3705, 0, 5836, 0, 7035, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 39, 51, 55, 74, 63, 97, 55, 69, 46, 80, 36, 80, 0, 0, 35, 60 }},
		{{ 1174, 0, 496, 0, 1678, 0, 2629, 0, 3693, 0, 5851, 0, 6958, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 51, 57, 70, 62, 99, 55, 65, 46, 80, 37, 80, 0, 0, 31, 60 }},
		{{ 1172, 0, 534, 0, 1526, 0, 2643, 0, 3701, 0, 5860, 0, 6935, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 68, 61, 99, 54, 62, 46, 80, 38, 80, 0, 0, 28, 60 }},
		{{ 1170, 0, 558, 0, 1414, 0, 2672, 0, 3717, 0, 5865, 0, 6942, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 48, 60, 68, 61, 99, 54, 59, 46, 80, 38, 80, 0, 0, 26, 60 }},
		{{ 1168, 0, 571, 0, 1341, 0, 2699, 0, 3732, 0, 5868, 0, 6957, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 60, 68, 61, 99, 55, 57, 46, 80, 39, 80, 0, 0, 25, 60 }},
		{{ 1166, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1164, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 1162, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1160, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1149, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1138, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1127, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1116, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1105, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1094, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1083, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1072, 0, 278, 0, 2281, 0, 2812, 0, 3799, 0, 5795, 0, 7354, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 49, 0, 89, 0, 89, 0, 74, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 1061, 0, 235, 0, 1987, 0, 2058, 0, 2877, 0, 5298, 0, 6768, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 1050, 0, 238, 0, 2002, 0, 2134, 0, 2950, 0, 5285, 0, 6756, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 985, 0, 241, 0, 2025, 0, 2244, 0, 3056, 0, 5269, 0, 6741, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 37, 43, 49, 63, 60, 113, 55, 74, 42, 80, 34, 80, 0, 0, 51, 60 }},
		{{ 920, 0, 247, 0, 2056, 0, 2385, 0, 3193, 0, 5258, 0, 6725, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 41, 48, 66, 62, 123, 57, 76, 42, 80, 35, 80, 0, 0, 54, 60 }},
		{{ 919, 0, 256, 0, 2094, 0, 2551, 0, 3356, 0, 5257, 0, 6714, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 39, 48, 71, 65, 135, 59, 76, 43, 80, 35, 80, 0, 0, 57, 60 }},
		{{ 918, 0, 264, 0, 2125, 0, 2675, 0, 3479, 0, 5269, 0, 6712, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 37, 48, 74, 67, 144, 60, 76, 44, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 916, 0, 271, 0, 2147, 0, 2763, 0, 3568, 0, 5284, 0, 6715, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 36, 48, 77, 68, 150, 61, 76, 44, 80, 35, 80, 0, 0, 59, 60 }},
		{{ 915, 0, 276, 0, 2162, 0, 2820, 0, 3625, 0, 5297, 0, 6718, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 35, 47, 79, 69, 154, 62, 75, 45, 80, 35, 80, 0, 0, 57, 60 }},
		{{ 914, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 54, 60 }},
		{{ 912, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 34, 34, 47, 79, 70, 156, 62, 75, 45, 80, 35, 80, 0, 0, 50, 60 }},
		{{ 911, 0, 302, 0, 2106, 0, 2821, 0, 3654, 0, 5360, 0, 6743, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 35, 35, 49, 79, 69, 151, 61, 73, 45, 80, 35, 80, 0, 0, 45, 60 }},
		{{ 910, 0, 346, 0, 1982, 0, 2775, 0, 3659, 0, 5461, 0, 6786, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 36, 38, 51, 77, 68, 143, 60, 70, 45, 80, 36, 80, 0, 0, 39, 60 }},
		{{ 909, 0, 409, 0, 1806, 0, 2724, 0, 3669, 0, 5594, 0, 6843, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 38, 41, 54, 75, 66, 131, 59, 65, 46, 80, 36, 80, 0, 0, 34, 60 }},
		{{ 908, 0, 483, 0, 1590, 0, 2691, 0, 3691, 0, 5734, 0, 6905, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 44, 57, 73, 64, 117, 57, 60, 46, 80, 37, 80, 0, 0, 30, 60 }},
		{{ 906, 0, 533, 0, 1441, 0, 2692, 0, 3713, 0, 5813, 0, 6941, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 46, 59, 71, 62, 107, 56, 58, 47, 80, 38, 80, 0, 0, 27, 60 }},
		{{ 905, 0, 562, 0, 1349, 0, 2705, 0, 3731, 0, 5853, 0, 6960, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 46, 60, 69, 61, 102, 55, 57, 47, 80, 39, 80, 0, 0, 25, 60 }},
		{{ 904, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 902, 0, 576, 0, 1305, 0, 2715, 0, 3741, 0, 5869, 0, 6968, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 47, 61, 69, 61, 99, 55, 56, 47, 80, 39, 80, 0, 0, 24, 60 }},
		{{ 901, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 279, 0, 2170, 0, 2847, 0, 3653, 0, 5305, 0, 6720, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 34, 0, 79, 0, 156, 0, 75, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 900, 0, 462, 0, 1513, 0, 2553, 0, 3535, 0, 5746, 0, 6910, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 900, 0, 466, 0, 1520, 0, 2560, 0, 3550, 0, 5757, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 100, 50, 100, 50, 100, 50, 100, 35, 100, 0, 100, 0, 0, 60, 60 }},
		{{ 870, 0, 474, 0, 1530, 0, 2570, 0, 3573, 0, 5773, 0, 6909, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 40, 49, 58, 68, 61, 104, 54, 62, 45, 80, 37, 80, 0, 0, 50, 60 }},
		{{ 840, 0, 484, 0, 1544, 0, 2585, 0, 3603, 0, 5793, 0, 6911, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 103, 54, 62, 45, 80, 37, 80, 0, 0, 51, 60 }},
		{{ 840, 0, 496, 0, 1563, 0, 2603, 0, 3638, 0, 5816, 0, 6916, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 49, 58, 68, 61, 102, 54, 62, 45, 80, 37, 80, 0, 0, 53, 60 }},
		{{ 839, 0, 512, 0, 1587, 0, 2626, 0, 3679, 0, 5840, 0, 6927, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 50, 58, 69, 62, 101, 55, 63, 46, 80, 37, 80, 0, 0, 55, 60 }},
		{{ 839, 0, 530, 0, 1616, 0, 2654, 0, 3722, 0, 5864, 0, 6945, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 70, 62, 101, 55, 65, 46, 80, 38, 80, 0, 0, 57, 60 }},
		{{ 839, 0, 552, 0, 1652, 0, 2686, 0, 3767, 0, 5885, 0, 6975, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 72, 62, 102, 55, 67, 46, 80, 38, 80, 0, 0, 58, 60 }},
		{{ 838, 0, 575, 0, 1691, 0, 2720, 0, 3808, 0, 5900, 0, 7012, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 74, 63, 103, 56, 71, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 838, 0, 595, 0, 1726, 0, 2750, 0, 3840, 0, 5907, 0, 7053, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 58, 76, 63, 105, 56, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 838, 0, 612, 0, 1758, 0, 2777, 0, 3864, 0, 5910, 0, 7092, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 50, 59, 78, 64, 108, 57, 79, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 837, 0, 627, 0, 1786, 0, 2800, 0, 3882, 0, 5910, 0, 7129, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 79, 64, 110, 57, 83, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 837, 0, 640, 0, 1809, 0, 2819, 0, 3896, 0, 5908, 0, 7161, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 81, 64, 112, 58, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 837, 0, 649, 0, 1826, 0, 2833, 0, 3905, 0, 5905, 0, 7186, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 82, 65, 113, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 836, 0, 655, 0, 1838, 0, 2843, 0, 3911, 0, 5903, 0, 7204, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 836, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5901, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 836, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5901, 0, 7213, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 835, 0, 658, 0, 1844, 0, 2848, 0, 3914, 0, 5899, 0, 7211, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 115, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 835, 0, 655, 0, 1840, 0, 2845, 0, 3913, 0, 5895, 0, 7202, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 60, 83, 65, 114, 58, 92, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 835, 0, 651, 0, 1834, 0, 2841, 0, 3912, 0, 5890, 0, 7188, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 50, 59, 83, 65, 114, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 834, 0, 646, 0, 1827, 0, 2836, 0, 3910, 0, 5883, 0, 7171, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 82, 65, 112, 58, 91, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 834, 0, 639, 0, 1818, 0, 2829, 0, 3907, 0, 5874, 0, 7149, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 51, 59, 81, 64, 111, 58, 90, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 834, 0, 631, 0, 1807, 0, 2819, 0, 3903, 0, 5864, 0, 7124, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 80, 64, 109, 57, 88, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 833, 0, 622, 0, 1795, 0, 2807, 0, 3896, 0, 5853, 0, 7097, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 52, 59, 79, 64, 107, 57, 87, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 833, 0, 612, 0, 1781, 0, 2791, 0, 3886, 0, 5841, 0, 7066, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 53, 58, 78, 64, 105, 56, 86, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 833, 0, 602, 0, 1766, 0, 2772, 0, 3873, 0, 5829, 0, 7034, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 53, 58, 76, 63, 102, 56, 84, 44, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 832, 0, 590, 0, 1750, 0, 2748, 0, 3855, 0, 5816, 0, 7001, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 54, 58, 75, 63, 100, 55, 82, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 832, 0, 578, 0, 1733, 0, 2719, 0, 3832, 0, 5804, 0, 6967, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 58, 73, 62, 97, 55, 80, 45, 80, 39, 80, 0, 0, 60, 60 }},
		{{ 832, 0, 565, 0, 1715, 0, 2684, 0, 3802, 0, 5792, 0, 6934, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 71, 62, 93, 54, 77, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 831, 0, 551, 0, 1697, 0, 2642, 0, 3766, 0, 5782, 0, 6902, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 55, 57, 70, 61, 89, 53, 75, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 831, 0, 538, 0, 1678, 0, 2592, 0, 3720, 0, 5773, 0, 6872, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 68, 60, 86, 52, 72, 45, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 831, 0, 524, 0, 1660, 0, 2533, 0, 3665, 0, 5766, 0, 6846, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 66, 60, 81, 52, 69, 46, 80, 38, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 512, 0, 1644, 0, 2473, 0, 3607, 0, 5762, 0, 6826, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 41, 56, 57, 64, 59, 77, 51, 67, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 501, 0, 1630, 0, 2413, 0, 3548, 0, 5760, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 62, 58, 74, 50, 64, 46, 80, 37, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 492, 0, 1618, 0, 2354, 0, 3489, 0, 5761, 0, 6804, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 55, 57, 60, 57, 70, 49, 62, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 484, 0, 1608, 0, 2297, 0, 3430, 0, 5763, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 59, 56, 67, 49, 60, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 477, 0, 1599, 0, 2242, 0, 3374, 0, 5766, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 42, 54, 57, 58, 55, 65, 48, 58, 46, 80, 36, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 471, 0, 1592, 0, 2191, 0, 3321, 0, 5770, 0, 6797, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 57, 54, 62, 48, 57, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 467, 0, 1586, 0, 2144, 0, 3272, 0, 5774, 0, 6799, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 53, 57, 56, 54, 60, 47, 55, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 463, 0, 1581, 0, 2101, 0, 3227, 0, 5779, 0, 6802, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 55, 53, 58, 47, 54, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 459, 0, 1578, 0, 2063, 0, 3188, 0, 5783, 0, 6806, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 52, 57, 54, 53, 56, 46, 53, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 457, 0, 1574, 0, 2031, 0, 3154, 0, 5787, 0, 6810, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 43, 51, 58, 54, 52, 55, 46, 52, 47, 80, 35, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 455, 0, 1572, 0, 2005, 0, 3126, 0, 5790, 0, 6813, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 54, 46, 52, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 453, 0, 1570, 0, 1986, 0, 3105, 0, 5793, 0, 6816, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 51, 58, 53, 52, 53, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1972, 0, 3091, 0, 5795, 0, 6818, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 46, 51, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 60, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 53, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 47, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 40, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 33, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 27, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 20, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 14, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 44, 50, 58, 53, 51, 52, 45, 50, 47, 80, 34, 80, 0, 0, 7, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 452, 0, 1569, 0, 1966, 0, 3084, 0, 5796, 0, 6819, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 50, 0, 53, 0, 52, 0, 50, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }},
		{{ 830, 0, 920, 0, 1445, 0, 2804, 0, 3915, 0, 5969, 0, 6256, 0, 0, 0, 200, 30, 0, 60, 0, 0, 0, 0, 0, 87, 0, 62, 0, 103, 0, 105, 0, 80, 0, 80, 0, 0, 0, 60 }}
	};
	try {
		autoKlattTable me = Thing_new (KlattTable);
		Table_initWithColumnNames (me.get(), nrows, theColumnNames.get());
		Melder_assert (theColumnNames.size == KlattTable_NPAR);
		for (integer irow = 1; irow <= nrows; irow ++) {
			for (integer jcol = 1; jcol <= KlattTable_NPAR; jcol ++) {
				double val = klatt_data [irow - 1].p [jcol - 1];
				if (jcol > 3 && jcol < 13 && (jcol % 2 == 0) && val <= 0) // bw == 0?
					val = klatt_data [irow - 1].p [jcol] / 10;
				Table_setNumericValue ( (Table) me.get(), irow, jcol, val);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U" KlattTable example not created.");
	}
}

autoKlattTable Table_to_KlattTable (Table me) {
	try {
		Melder_require (my numberOfColumns == KlattTable_NPAR,
			U"A KlattTable needs ", KlattTable_NPAR, U" columns.");
		
		autoKlattTable thee = Thing_new (KlattTable);
		my structTable :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"KlattTable not created from Table.");
	}
}

autoTable KlattTable_to_Table (KlattTable me) {
	try {
		autoTable thee = Thing_new (Table);
		my structTable :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Table not created from KlattTable.");
	}
}

/* End of file KlattTable.cpp */
