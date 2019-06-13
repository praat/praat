/* Sound_extensions.cpp
 *
 * Copyright (C) 1993-2019 David Weenink, 2017 Paul Boersma
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
 djmw 20001109 Sound_scale->Vector_scale
 djmw 20020516 GPL header
 djmw 20020523 Removed Sound_read/writeWAVAudioFile
 djmw 20030926 Sound_changeGender
 djmw 20040405 Renamed: Sound_overrideSamplingFrequency
 djmw 20041124 Changed call to Sound_to_Spectrum & Spectrum_to_Sound.
 djmw 20050620 Changed Pitch_HERTZ to Pitch_UNIT_HERTZ
 djmw 20050628 New and extended Sound_createShepardToneComplex that corrects incorrect amplitudes of tones in complex.
 		(amplitudes were on linear instead of log scale)
 djmw 20060921 Added Sound_to_IntervalTier_detectSilence
 djmw 20061010 Removed crashing bug in Sound_to_IntervalTier_detectSilence.
 djmw 20061201 Interface change: removed minimumPitch parameter from Sound_Pitch_changeGender.
 djmw 20061214 Sound_Pitch_changeSpeaker removed warning.
 djmw 20070103 Sound interface changes
 djmw 20070129 Warning added in changeGender_old.
 djmw 20071022 Possible (bug?) correction in Sound_createShepardToneComplex
 djmw 20071030 Sound_preEmphasis: no pre-emphasis above the Nyquist frequency.
 djmw 20071202 Melder_warning<n>
 djmw 20080122 float -> double
 djmw 20080320 +Sound_fade.
 djmw 20080530 +Sound_localAverage
 pb 20090926 Correction in Sound_Pitch_changeGender_old
 djmw 20091023 Added Sound_drawIntervals
 djmw 20091028 Sound_drawIntervals -> Sound_drawParts + Graphics_function
 djmw 20091126 Sound_drawParts -> Sound_drawWheres
 djmw 20091211 Sound_fade: removed erroneous warning
 djmw 20100318 Cross-correlation, convolution and autocorrelation
 djmw 20100325 -Cross-correlation, convolution and autocorrelation
 djmw 20111227 Sound_trimSilencesAtStartAndEnd and Sound_getStartAndEndTimesOfSounding
 djmw 20120616 Change 0.8 to 1.25 in Sound_Point_Pitch_Duration_to_Sound
*/

#include <ctype.h>
#include "Formula.h"
#include "Intensity_extensions.h"
#include "Sound_extensions.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"
#include "Sound_to_Intensity.h"
#include "Sound_to_Pitch.h"
#include "Vector.h"
#include "Pitch_extensions.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "Polygon_extensions.h"
#include "TextGrid_extensions.h"
#include "DurationTier.h"
#include "Ltas.h"
#include "Manipulation.h"
#include "NUMcomplex.h"

#include "enums_getText.h"
#include "Sound_extensions_enums.h"
#include "enums_getValue.h"
#include "Sound_extensions_enums.h"

#define MAX_T  0.02000000001   /* Maximum interval between two voice pulses (otherwise voiceless). */

static void PitchTier_modifyExcursionRange (PitchTier me, double tmin, double tmax, double multiplier, double fref_Hz) {
	if (fref_Hz <= 0)
		return;
	double fref_st = 12.0 * log (fref_Hz / 100.0) / NUMln2;
	for (integer i = 1; i <= my points.size; i ++) {
		RealPoint point = my points.at [i];
		double f = point -> value;
		if (point -> number < tmin || point -> number > tmax)
			continue;
		if (f > 0.0) {
			double f_st = fref_st + 12.0 * log2 (f / fref_Hz) * multiplier;
			point -> value = 100.0 * exp (f_st * (NUMln2 / 12.0));
		}
	}
}

static void Pitch_scaleDuration (Pitch me, double multiplier) {
	if (multiplier != 1.0) {
		// keep xmin at the same value
		my dx *= multiplier;
		my x1 = my xmin + (my x1 - my xmin) * multiplier;
		my xmax = my xmin + (my xmax - my xmin) * multiplier;
	}
}

static void Pitch_scalePitch (Pitch me, double multiplier) {
	for (integer i = 1; i <= my nx; i ++) {
		double f = my frame [i].candidate [1].frequency;
		f *= multiplier;
		if (f < my ceiling)
			my frame [i].candidate [1].frequency = f;
	}
}

static void i1write (Sound me, FILE *f, integer *nClip) {
	double min = -128.0, max = 127.0;
	*nClip = 0;
	for (integer i = 1; i <= my nx; i ++) {
		double sample = round (my z [1] [i] * 128.0);
		if (sample > max) {
			sample = max;
			(*nClip) ++;
		} else if (sample < min) {
			sample = min;
			(*nClip) ++;
		}
		binputi8 ((int) sample, f);
	}
}

static void i1read (Sound me, FILE *f) {
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = bingeti8 (f) / 128.0;
}

static void u1write (Sound me, FILE *f, integer *nClip) {
	double min = 0.0, max = 255.0;
	*nClip = 0;
	for (integer i = 1; i <= my nx; i ++) {
		double sample = round ((my z [1] [i] + 1.0) * 255.0 / 2.0);
		if (sample > max) {
			sample = max;
			(*nClip) ++;
		} else if (sample < min) {
			sample = min;
			(*nClip) ++;
		}
		binputu8 ((unsigned int) sample, f);
	}
}

static void u1read (Sound me, FILE *f) {
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = bingetu8 (f) / 128.0 - 1.0;
}

static void i2write (Sound me, FILE *f, bool littleEndian, integer *nClip) {
	double min = -32768.0, max = 32767.0;
	void (*put) (int16, FILE *) = littleEndian ? binputi16LE : binputi16;
	*nClip = 0;
	for (integer i = 1; i <= my nx; i ++) {
		double sample = round (my z [1] [i] * 32768.0);
		if (sample > max) {
			sample = max;
			(*nClip) ++;
		} else if (sample < min) {
			sample = min;
			(*nClip) ++;
		}
		put ((int16) sample, f);
	}
}

static void i2read (Sound me, FILE *f, bool littleEndian) {
	int16 (*get) (FILE *) = littleEndian ? bingeti16LE : bingeti16;
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = get (f) / 32768.0;
}

static void u2write (Sound me, FILE *f, bool littleEndian, integer *nClip) {
	double min = 0.0, max = 65535.0;
	void (*put) (uint16, FILE *) = littleEndian ? binputu16LE : binputu16;
	*nClip = 0;
	for (integer i = 1; i <= my nx; i ++) {
		double sample = round ((my z [1] [i] + 1.0) * 65535.0 / 2.0);
		if (sample > max) {
			sample = max;
			(*nClip) ++;
		} else if (sample < min) {
			sample = min;
			(*nClip) ++;
		}
		put ((uint16) sample, f);
	}
}

static void u2read (Sound me, FILE *f, bool littleEndian) {
	uint16 (*get) (FILE *) = littleEndian ? bingetu16LE : bingetu16;
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = get (f) / 32768.0 - 1.0;
}

static void i4write (Sound me, FILE *f, bool littleEndian, integer *nClip) {
	double min = -2147483648.0, max = 2147483647.0;
	void (*put) (int32, FILE *) = littleEndian ? binputi32LE : binputi32;
	*nClip = 0;
	for (integer i = 1; i <= my nx; i ++) {
		double sample = round (my z [1] [i] * 2147483648.0);
		if (sample > max) {
			sample = max;
			(*nClip) ++;
		} else if (sample < min) {
			sample = min;
			(*nClip) ++;
		}
		put ((int32) sample, f);
	}
}

static void i4read (Sound me, FILE *f, bool littleEndian) {
	int32 (*get) (FILE *) = littleEndian ? bingeti32LE : bingeti32;
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = get (f) / 2147483648.0;
}


static void u4write (Sound me, FILE *f, bool littleEndian, integer *nClip) {
	double min = 0.0, max = 4294967295.0;
	void (*put) (uint32, FILE *) = littleEndian ? binputu32LE : binputu32;
	*nClip = 0;
	for (integer i = 1; i <= my nx; i ++) {
		double sample = Melder_round_tieUp (my z [1] [i] * 4294967295.0);
		if (sample > max) {
			sample = max;
			(*nClip) ++;
		} else if (sample < min) {
			sample = min;
			(*nClip) ++;
		}
		put ((uint32) sample, f);
	}
}

static void u4read (Sound me, FILE *f, bool littleEndian) {
	int32 (*get) (FILE *) = littleEndian ? bingeti32LE : bingeti32;
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = get (f) / 2147483648.0 - 1.0;
}


static void r4write (Sound me, FILE *f) {
	for (integer i = 1; i <= my nx; i ++)
		binputr32 (my z [1] [i], f);
}

static void r4read (Sound me, FILE *f) {
	for (integer i = 1; i <= my nx; i ++)
		my z [1] [i] = bingetr32 (f);
}

/* Old TIMIT sound-file format */
autoSound Sound_readFromCmuAudioFile (MelderFile file) {
	try {
		bool littleEndian = true;
		autofile f = Melder_fopen (file, "rb");
		Melder_require (bingeti16LE (f) == 6, U"Incorrect header size.");
		
		bingeti16LE (f);
		short nChannels = bingeti16LE (f);
		Melder_require (nChannels == 1, U"Incorrect number of channels.");
		Melder_require (bingeti16LE (f) > 0, U"Incorrect sampling frequency.");
		
		integer nSamples = bingeti32LE (f);
		Melder_require (nSamples > 0, U"Incorrect number of samples.");
		
		autoSound me = Sound_createSimple (1, nSamples / 16000.0, 16000);
		i2read (me.get(), f, littleEndian);
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from CMU audio file ", MelderFile_messageName (file), U".");
	}
}

autoSound Sound_readFromRawFile (MelderFile file, const char *format, int nBitsCoding, bool littleEndian, bool unSigned, integer skipNBytes, double samplingFrequency) {
	try {
		autofile f = Melder_fopen (file, "rb");
		if (! format) {
			format = "integer";
		}
		if (nBitsCoding <= 0) {
			nBitsCoding = 16;
		}
		integer nBytesPerSample = (nBitsCoding + 7) / 8;
		if (strequ (format, "float")) {
			nBytesPerSample = 4;
		}
		Melder_require (! (nBytesPerSample == 3), U"Number of bytes per sample should be 1, 2 or 4.");
		
		if (skipNBytes <= 0) {
			skipNBytes = 0;
		}
		integer nSamples = (MelderFile_length (file) - skipNBytes) / nBytesPerSample;
		Melder_require (nSamples > 0, U"No samples left to read.");
		
		autoSound me = Sound_createSimple (1, nSamples / samplingFrequency, samplingFrequency);
		fseek (f, skipNBytes, SEEK_SET);
		if (nBytesPerSample == 1 && unSigned) {
			u1read (me.get(), f);
		} else if (nBytesPerSample == 1 && ! unSigned) {
			i1read (me.get(), f);
		} else if (nBytesPerSample == 2 && unSigned) {
			u2read (me.get(), f, littleEndian);
		} else if (nBytesPerSample == 2 && ! unSigned) {
			i2read (me.get(), f, littleEndian);
		} else if (nBytesPerSample == 4 && unSigned) {
			u4read (me.get(), f, littleEndian);
		} else if (nBytesPerSample == 4 && ! unSigned) {
			i4read (me.get(), f, littleEndian);
		} else if (nBytesPerSample == 4 && strequ (format, "float")) {
			r4read (me.get(), f);
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from raw audio file ", MelderFile_messageName (file), U".");
	}
}

void Sound_writeToRawFile (Sound me, MelderFile file, const char *format, bool littleEndian, int nBitsCoding, bool unSigned) {
	try {
		integer nClip = 0;
		autofile f = Melder_fopen (file, "wb");
		if (! format) {
			format = "integer";
		}
		if (nBitsCoding <= 0) {
			nBitsCoding = 16;
		}
		integer nBytesPerSample = (nBitsCoding + 7) / 8;
		if (strequ (format, "float")) {
			nBytesPerSample = 4;
		}
		Melder_require (! (nBytesPerSample == 3), U"number of bytes per sample should be 1, 2 or 4.");
		
		if (nBytesPerSample == 1 && unSigned) {
			u1write (me, f, & nClip);
		} else if (nBytesPerSample == 1 && ! unSigned) {
			i1write (me, f, & nClip);
		} else if (nBytesPerSample == 2 && unSigned) {
			u2write (me, f, littleEndian, & nClip);
		} else if (nBytesPerSample == 2 && ! unSigned) {
			i2write (me, f, littleEndian, & nClip);
		} else if (nBytesPerSample == 4 && unSigned) {
			u4write (me, f, littleEndian, & nClip);
		} else if (nBytesPerSample == 4 && ! unSigned) {
			i4write (me, f, littleEndian, & nClip);
		} else if (nBytesPerSample == 4 && strequ (format, "float")) {
			r4write (me, f);
		}
		if (nClip > 0) {
			Melder_warning (nClip, U" from ", my nx, U" samples have been clipped.\nAdvice: you could scale the amplitudes or save as a binary file.");
		}
		
		Melder_require (feof ((FILE *) f) == 0 && ferror ((FILE *) f) == 0, U"Sound_writeToRawFile: not completed");
		
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": saving as raw file not performed.");
	}
}

struct dialogic_adpcm {
	char code;
	short last, index;
	short step_size [49];
	short adjust [8];
};

static void dialogic_adpcm_init (struct dialogic_adpcm *adpcm) {
	short step_size [49] = {
		16, 17, 19, 21, 23, 25, 28, 31, 34, 37,
		41, 45, 50, 55, 60, 66, 73, 80, 88, 97,
		107, 118, 130, 143, 157, 173, 190, 209, 230, 253,
		279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
		724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552
	};
	short adjust [8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

	adpcm -> last = 0;
	adpcm -> index = 0;
	for (integer i = 0; i < 49; i ++) {
		adpcm -> step_size [i] = step_size [i];
	}
	for (integer i = 0; i <  8; i ++) {
		adpcm -> adjust [i] = adjust [i];
	}
}

/*
	The code is adapted from:
	Bob Edgar (), "PC Telephony - The complete guide to designing,
		building and programming systems using Dialogic and Related
		Hardware", 272-276.
*/
static float dialogic_adpcm_decode (struct dialogic_adpcm *adpcm) {
	float scale = 32767.0 / 32768.0 / 2048.0;

	// nibble = B3 B2 B1 B0 (4 lower bits)
	// d(n) = ss(n)*B2 + ss(n)/2 *B1 + ss(n)/4*B0 + ss(n)/8

	short ss = adpcm -> step_size [adpcm -> index];
	short e = ss / 8;
	if (adpcm -> code & 0x01) {
		e += ss / 4;
	}
	if (adpcm -> code & 0x02) {
		e += ss / 2;
	}
	if (adpcm -> code & 0x04) {
		e += ss;
	}

	// If B3==1 then d(n) = -d(n);

	short diff = (adpcm -> code & 0x08) ? -e : e;

	// x(n) = x(n-1)+d(n)

	short s = adpcm -> last + diff;
	if (s > 2048) {
		s = 2048;
	}
	if (s < -2048) {
		s = -2048;
	}
	adpcm -> last = s;

	// ss(n+1) = ss(n) * 1.1*M(L(n)) via lookup table

	adpcm -> index += adpcm -> adjust [adpcm -> code & 0x07];
	if (adpcm -> index <  0) {
		adpcm -> index = 0;
	}
	if (adpcm -> index > 48) {
		adpcm -> index = 48;
	}
	return scale * s;
}

autoSound Sound_readFromDialogicADPCMFile (MelderFile file, double sampleRate) {
	try {
		autofile f = Melder_fopen (file, "rb");

		integer filelength = MelderFile_length (file);
		
		Melder_require (filelength > 0, U"File should not be empty.");
		
		// Two samples in each byte

		integer numberOfSamples = 2 * filelength;
		
		autoSound me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate);

		// Read all bytes and decode

		struct dialogic_adpcm adpcm;
		dialogic_adpcm_init (& adpcm);

		integer n = 1;
		for (integer i = 1; i <= filelength; i ++) {
			unsigned char sc;
			integer nread = fread (& sc, 1, 1, f);
			Melder_require (nread == 1, U"Error: trying to read byte number ", i, U".");
			adpcm.code = (char) ((sc >> 4) & 0x0f);
			my z [1] [n ++] = dialogic_adpcm_decode (& adpcm);
			adpcm.code = (char) (sc & 0x0f);
			my z [1] [n ++] = dialogic_adpcm_decode (& adpcm);
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from Dialogic ADPCM file ", MelderFile_messageName (file), U".");
	}
}

void Sound_preEmphasis (Sound me, double preEmphasisFrequency) {
	if (preEmphasisFrequency >= 0.5 / my dx)
		return;    // above Nyquist?
	double preEmphasis = exp (- 2.0 * NUMpi * preEmphasisFrequency * my dx);
	for (integer channel = 1; channel <= my ny; channel ++) {
		VEC s = my z.row (channel);
		for (integer i = my nx; i >= 2; i --)
			s [i] -= preEmphasis * s [i - 1];
	}
}

void Sound_deEmphasis (Sound me, double deEmphasisFrequency) {
	double deEmphasis = exp (- 2.0 * NUMpi * deEmphasisFrequency * my dx);
	for (integer channel = 1; channel <= my ny; channel ++) {
		VEC s = my z.row (channel);
		for (integer i = 2; i <= my nx; i ++) {
			s [i] += deEmphasis * s [i - 1];
		}
	}
}

autoSound Sound_createGaussian (double windowDuration, double samplingFrequency) {
	try {
		autoSound me = Sound_createSimple (1, windowDuration, samplingFrequency);
		VEC s = my z.row (1);
		double imid = 0.5 * (my nx + 1), edge = exp (-12.0);
		for (integer i = 1; i <= my nx; i ++) {
			s [i] = (exp (-48.0 * (i - imid) * (i - imid) / (my nx + 1) / (my nx + 1)) - edge) / (1 - edge);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Gaussian function.");
	}
}

autoSound Sound_createHamming (double windowDuration, double samplingFrequency) {
	try {
		autoSound me = Sound_createSimple (1, windowDuration, samplingFrequency);
		double p = 2.0 * NUMpi / (my nx - 1);
		for (integer i = 1; i <= my nx; i ++) {
			my z [1] [i] = 0.54 - 0.46 * cos ((i - 1) * p);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Hamming function.");
	};
}

static autoSound Sound_create2 (double minimumTime, double maximumTime, double samplingFrequency) {
	return Sound_create (1, minimumTime, maximumTime, Melder_iround ( (maximumTime - minimumTime) * samplingFrequency),
		1.0 / samplingFrequency, minimumTime + 0.5 / samplingFrequency);
}

/*
	Trig functions whose arguments form a linear sequence x = x1 + n.dx,
	for n=0,1,2,... are efficiently calculated by the following recurrence:
		cos(a+dx) = cos(a) - (alpha . cos(a) + beta . sin(a))
		sin(a+dx) = sin(a) - (alpha . sin(a) - beta . sin(a))
	where alpha and beta are precomputed coefficients
		alpha = 2 sin^2(dx/2) and beta = sin(dx)
	In this way aplha and beta do not lose significance if the increment
	dx is small.
*/

static autoSound Sound_createToneComplex (double minimumTime, double maximumTime, double samplingFrequency, double firstFrequency, integer numberOfComponents, double frequencyDistance, integer mistunedComponent, double mistuningFraction, bool scaleAmplitudes) {
	try {
		autoSound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
		for (integer j = 1; j <= numberOfComponents; j ++) {
			double fraction = j == mistunedComponent ? mistuningFraction : 0;
			double w = 2 * NUMpi * (firstFrequency + (j - 1 + fraction) * frequencyDistance);
			double delta = w * my dx;
			double alpha = 2 * sin (delta / 2) * sin (delta / 2);
			double beta = sin (delta);
			double sint = sin (w * my x1);
			double cost = cos (w * my x1);
			my z [1] [1] += sint;
			for (integer i = 2; i <= my nx; i ++) {
				double costd = cost - (alpha * cost + beta * sint);
				double sintd = sint - (alpha * sint - beta * cost);
				my z [1] [i] += sintd;
				cost = costd; sint = sintd;
			}
		}
		if (scaleAmplitudes) {
			Vector_scale (me.get(), 0.99996948);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from tone complex.");
	}
}


autoSound Sound_createSimpleToneComplex (double minimumTime, double maximumTime, double samplingFrequency, double firstFrequency, integer numberOfComponents, double frequencyDistance, bool scaleAmplitudes) {
	if (firstFrequency + (numberOfComponents - 1) * frequencyDistance > samplingFrequency / 2) {
		Melder_warning (U"Sound_createSimpleToneComplex: frequency of (some) components too high.");
		numberOfComponents = Melder_ifloor (1.0 + (0.5 * samplingFrequency - firstFrequency) / frequencyDistance);
	}
	return Sound_createToneComplex (minimumTime, maximumTime, samplingFrequency,
	                                firstFrequency, numberOfComponents, frequencyDistance, 0, 0, scaleAmplitudes);
}

autoSound Sound_createMistunedHarmonicComplex (double minimumTime, double maximumTime, double samplingFrequency, double firstFrequency, integer numberOfComponents, integer mistunedComponent, double mistuningFraction, bool scaleAmplitudes) {
	if (firstFrequency + (numberOfComponents - 1) * firstFrequency > samplingFrequency / 2) {
		Melder_warning (U"Sound_createMistunedHarmonicComplex: frequency of (some) components too high.");
		numberOfComponents = Melder_ifloor (1.0 + (0.5 * samplingFrequency - firstFrequency) / firstFrequency);
	}
	if (mistunedComponent > numberOfComponents) {
		Melder_warning (U"Sound_createMistunedHarmonicComplex: mistuned component too high.");
	}
	return Sound_createToneComplex (minimumTime, maximumTime, samplingFrequency, firstFrequency, numberOfComponents, firstFrequency, mistunedComponent, mistuningFraction, scaleAmplitudes);
}

/*
	The gammachirp is a "chirp tone" with a  gamma-function envelope:
	f(t) = t^(n-1) exp (-2 pi b t) cos (2 pi f0 t + c ln (t) + p0)
	     = t^(n-1) exp (-2 pi b t) cos (phi(t))
	Instantaneous frequency f is defined as f = d phi(t) / dt / (2 pi)
	and so: f = f0 + c /(2 pi t)
	Irino: bandwidth = (frequency * (6.23e-6 * frequency + 93.39e-3) + 28.52)
*/
autoSound Sound_createGammaTone (double minimumTime, double maximumTime, double samplingFrequency, double gamma, double frequency, double bandwidth, double initialPhase, double addition, bool scaleAmplitudes) {
	try {
		autoSound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
		for (integer i = 1; i <= my nx; i ++) {
			double t = (i - 0.5) * my dx;
			double f = frequency + addition / (NUM2pi * t);
			if (f > 0 && f < samplingFrequency / 2) {
				my z [1] [i] = pow (t, gamma - 1.0) * exp (- NUM2pi * bandwidth * t) * 
					cos (NUM2pi * frequency * t + addition * log (t) + initialPhase);
			}
		}
		if (scaleAmplitudes) {
			Vector_scale (me.get(), 0.99996948);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from gammatone function.");
	}
}

#if 0
// This routine is unstable for small values of f and large b. Better to use cross-correlation of gammatone with sound.
static void NUMgammatoneFilter4 (double *x, double *y, integer n, double centre_frequency, double bandwidth, double samplingFrequency) {
	double a [5], b [9], dt = 1.0 / samplingFrequency, wt = NUMpi * centre_frequency * dt;
	double bt = 2 * NUMpi * bandwidth * dt, dt2 = dt * dt, dt4 = dt2 * dt2;

	Melder_assert (n > 0 && centre_frequency > 0 && bandwidth >= 0 && samplingFrequency > 0);

	/*
		The filter function is:
			H(z) = sum (i=0..4, a [i] z^-i) / sum (j=0..4, b [j] z^-j)
		Coefficients a & b according to:
		Slaney (1993), An efficient implementation of the Patterson-Holdsworth
		auditory filterbank, Apple Computer Technical Report 35, 41 pages.
		For the a's we have left out an overal scale factor of dt^4.
		This makes a [0] = 1.
	*/

	a [0] = dt4;
	a [1] = -4 * dt4 * cos (2 * wt) * exp (-    bt);
	a [2] =  6 * dt4 * cos (4 * wt) * exp (-2 * bt);
	a [3] = -4 * dt4 * cos (6 * wt) * exp (-3 * bt);
	a [4] =      dt4 * cos (8 * wt) * exp (-4 * bt);

	b [0] = 1;
	b [1] = -8 * cos (2 * wt)                           * exp (-    bt);
	b [2] = (16 + 12 * cos (4 * wt))                    * exp (-2 * bt);
	b [3] = (-48 * cos (2 * wt) - 8 * cos (6 * wt))     * exp (-3 * bt);
	b [4] = (36 + 32 * cos (4 * wt) + 2 * cos (8 * wt)) * exp (-4 * bt);
	b [5] = (-48 * cos (2 * wt) - 8 * cos (6 * wt))     * exp (-5 * bt);
	b [6] = (16 + 12 * cos (4 * wt))                    * exp (-6 * bt);
	b [7] = -8 * cos (2 * wt)                           * exp (-7 * bt);
	b [8] =                                               exp (-8 * bt);

	// Calculate gain (= Abs (H(z); f=fc) and scale a [0-4] with it.

	double zr =  cos (2 * wt), zi = -sin (2 * wt);
	double dr = a [4], di = 0, tr, ti, nr, ni;

	for (integer j = 1; j <= 4; j ++) {
		tr = a [4 - j] + zr * dr - zi * di;
		ti = zi * dr + zr * di;
		dr = tr; di = ti;
	}

	dr = b [8];
	di = 0;
	for (integer j = 1; j <= 8; j ++) {
		nr = b [8 - j] + zr * dr - zi * di;
		ni = zi * dr + zr * di;
		dr = nr; di = ni;
	}

	double n2 = nr * nr + ni * ni;
	double gr = tr * nr + ti * ni;
	double gi = ti * nr - tr * ni;
	double gain = sqrt (gr * gr + gi * gi) / n2;

	for (integer j = 0; j <= 4; j ++) {
		a [j] /= gain;
	}

	if (Melder_debug == -1) {
		Melder_casual (
			U" --gammatonefilter4 --\nF = ", centre_frequency,
			U", B = ", bandwidth,
			U", T = ", dt,
			U"\nGain = ", gain
		);
		for (integer i = 0; i <= 4; i ++) {
			Melder_casual (U"a [", i, U"] = ", a [i]);
		}
		for (integer i = 0; i <= 8; i ++) {
			Melder_casual (U"b [", i, U"] = ", b [i]);
		}
	}

	/* Perform the filtering. For the first 8 samples we must do some extra work.
	y [1] = a [0] * x [1];
	if (n > 1) {
		y [2] = a [0] * x [2];
		y [2] += a [1] * x [1] - b [1] * y [1];
	}
	if (n > 2) {
		y [2] = a [0] * x [2];
		y [2] += a [2] * x [i - 2] - b [2] * y [i - 2];
	}
	*/
	integer n8 = n < 8 ? n : 8;
	for (integer i = 1; i <= n8; i ++) {
		y [i] = a [0] * x [i];
		if (i > 1) {
			y [i] += a [1] * x [i - 1] - b [1] * y [i - 1];
		} else {
			continue;
		}
		if (i > 2) {
			y [i] += a [2] * x [i - 2] - b [2] * y [i - 2];
		} else {
			continue;
		}
		if (i > 3) {
			y [i] += a [3] * x [i - 3] - b [3] * y [i - 3];
		} else {
			continue;
		}
		if (i > 4) {
			y [i] += a [4] * x [i - 4] - b [4] * y [i - 4];
		} else {
			continue;
		}
		if (i > 5) {
			y [i] -= b [5] * y [i - 5];
		} else {
			continue;
		}
		if (i > 6) {
			y [i] -= b [6] * y [i - 6];
		} else {
			continue;
		}
		if (i > 7) {
			y [i] -= b [7] * y [i - 7];
		}
	}

	for (integer i = n8 + 1; i <= n; i ++) {
		// y [i]  = a [0] * x [i];
		// y [i] += a [1] * x [i-1] + a [2] * x [i-2] + a [3] * x [i-3] + a [4] * x [i-4];
		// y [i] -= b [1] * y [i-1] + b [2] * y [i-2] + b [3] * y [i-3] + b [4] * y [i-4];
		// y [i] -= b [5] * y [i-5] + b [6] * y [i-6] + b [7] * y [i-7] + b [8] * y [i-8];
		y [i] = a [0] * x [i] + a [1] * x [i - 1] + a [2] * x [i - 2] + a [3] * x [i - 3] + a [4] * x [i - 4]
		       - b [1] * y [i - 1] - b [2] * y [i - 2] - b [3] * y [i - 3] - b [4] * y [i - 4]
		       - b [5] * y [i - 5] - b [6] * y [i - 6] - b [7] * y [i - 7] - b [8] * y [i - 8];
	}
}

autoSound Sound_filterByGammaToneFilter4 (Sound me, double centre_frequency, double bandwidth) {
	try {
		Meldr_require (centre_frequency > 0, U"Centre frequency should be positive.");
		Melder_require (bandwidth > 0, U"Bandwidth should be positive.");

		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		autoNUMvector<double> y (1, my nx);
		autoNUMvector<double> x (1, my nx);

		double fs = 1 / my dx;
		for (integer channel = 1; channel <= my ny; channel ++) {
			for (integer i = 1; i <= my nx; i ++) {
				x [i] = my z [channel] [i];
			}

			NUMgammatoneFilter4 (x.peek(), y.peek(), my nx, centre_frequency, bandwidth, fs);

			for (integer i = 1; i <= my nx; i ++) {
				thy z [channel] [i] = y [i];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not filtered by gammatone filter4.");
	}
}
#endif


autoSound Sound_filterByGammaToneFilter4 (Sound me, double centre_frequency, double bandwidth) {
	return Sound_filterByGammaToneFilter (me, centre_frequency, bandwidth, 4.0, 0.0);
}

autoSound Sound_filterByGammaToneFilter (Sound me, double centre_frequency, double bandwidth, double gamma, double initialPhase) {
	try {
		autoSound gammaTone = Sound_createGammaTone (my xmin, my xmax, 1.0 / my dx, gamma, centre_frequency, bandwidth, initialPhase, 0.0, 0);
		// kSounds_convolve_scaling_INTEGRAL, SUM, NORMALIZE, PEAK_099
		autoSound thee = Sounds_convolve (me, gammaTone.get(), kSounds_convolve_scaling::INTEGRAL, kSounds_convolve_signalOutsideTimeDomain::ZERO);
		
		dcomplex r = gammaToneFilterResponseAtCentreFrequency (centre_frequency, bandwidth, gamma, initialPhase, my xmax - my xmin);
		
		double scale = 1.0 / sqrt (r.re * r.re + r.im * r.im);
		for (integer i = 1; i <= thy nx; i ++) {
			thy z [1] [i] *= scale;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not filtered by gammatone filter4.");
	}
}


/*
Sound Sound_createShepardTone (double minimumTime, double maximumTime, double samplingFrequency,
	double baseFrequency, double frequencyShiftFraction, double maximumFrequency, double amplitudeRange)
{
	Sound me; integer i, j, nComponents = 1 + log2 (maximumFrequency / 2 / baseFrequency);
	double lmin = pow (10, - amplitudeRange / 10);
	double twoPi = 2.0 * NUMpi, f = baseFrequency * (1 + frequencyShiftFraction);
	if (nComponents < 2) Melder_warning (U"Sound_createShepardTone: only 1 component.");
	Melder_casual (U"Sound_createShepardTone: ", nComponents, U" components.");
	if (! (me = Sound_create2 (minimumTime, maximumTime, samplingFrequency))) return nullptr;

	for (j=1; j <= nComponents; j ++)
	{
		double fj = f * pow (2, j-1), wj = twoPi * fj;
		double amplitude = lmin + (1 - lmin) *
			(1 - cos (twoPi * log (fj + 1) / log (maximumFrequency + 1))) / 2;
		for (i=1; i <= my nx; i ++)
		{
			my z [1] [i] += amplitude * sin (wj * (i - 0.5) * my dx);
		}
	}
	Vector_scale (me, 0.99996948);
	return me;
}
*/

autoSound Sound_createShepardToneComplex (double minimumTime, double maximumTime, double samplingFrequency, double lowestFrequency, integer numberOfComponents, double frequencyChange_st, double amplitudeRange, double octaveShiftFraction) {
	try {
		double highestFrequency = lowestFrequency * pow (2, numberOfComponents);
		double lmax_db = 0, lmin_db = lmax_db - fabs (amplitudeRange);

		Melder_require (highestFrequency <= samplingFrequency / 2.0,U"The highest frequency you want to generate is "
				U"above the Nyquist frequency. Choose a larger value for \"Sampling frequency\", or lower values for "
				U"\"Number of components\" or \"Lowest frequency\".");
		Melder_require (octaveShiftFraction >= 0.0 && octaveShiftFraction < 1.0, U"Octave offset fraction should be greater or equal zero and smaller than one.");
	
		double octaveTime, sweeptime;
		if (frequencyChange_st != 0.0) {
			octaveTime = 12.0 / fabs (frequencyChange_st);
			sweeptime = numberOfComponents * octaveTime;
		} else {
			octaveTime = sweeptime = 1e308;
		}
		autoSound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);

		double a = frequencyChange_st / 12.0;
		for (integer i = 1; i <= numberOfComponents; i ++) {
			double tswitch;
			double freqi = lowestFrequency * pow (2.0, i - 1 + octaveShiftFraction);
			double b1, b2;
			double phase1 = 0, phasejm1 = 0;

			/*
				The frequency is f(t) = lowestFrequency * 2^tone(t)
				The tone is parametrized with a straight line: tone(t) = a * t + b
				where a = frequencyChange_st / 12 and b depends on the component
				If frequencyChange_st >=0
					The tone rises until highest frequency at t=tswich, then falls to lowest and starts rising again.
					The slope is always the same. The offsets are b1 and b2 respectively.
					We count octaveShiftFraction as distance from tone base
				else if frequencyChange_st < 0
					The tone falls until the lowest frequency at t=tswich, then jumps to highest and starts falling again
					All tones start one octave higher as in rising case.
					We also count octaveShiftFraction down from this tone base.
				else
					No changes in frequency of the components.
				endif
			*/
			if (frequencyChange_st >= 0) {
				b1 = i - 1 + octaveShiftFraction;
				b2 = 0.0;
				tswitch = (numberOfComponents - b1) * octaveTime;
			} else {
				freqi *= 2;
				b1 = i - octaveShiftFraction;
				b2 = numberOfComponents;
				tswitch = b1 * octaveTime;
			}
			for (integer j = 1; j <= my nx; j ++) {
				double t = Sampled_indexToX (me.get(), j);
				double tmod = fmod (t, sweeptime);
				double tone = tmod <= tswitch ? b1 + a * tmod : b2 + a * (tmod - tswitch);
				double f = lowestFrequency * pow (2, tone);
				/* double theta = 2 * NUMpi * log2 (f / lowestFrequency) / numberOfComponents; */
				double theta = 2 * NUMpi * tone / numberOfComponents;
				double level = pow (10, (lmin_db + (lmax_db - lmin_db) * (1 - cos (theta)) / 2) / 20);
				double phasej = phasejm1 + 2 * NUMpi * f * my dx; /* Integrate 2*pi*f(t) */

				if (j == 1) {
					phase1 = phasej;    // phase1 = j == 1 ? phasej : phase1;
				}
				my z [1] [j] += level * sin (phasej - phase1); // si
				phasejm1 = phasej;
			}
		}
		Vector_scale (me.get(), 0.99996948);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Shepard tone complex.");
	}
}

/* can be implemented more efficiently with sin recurrence? */
/* amplitude(f) = min + (1-min)*(1-cos(2*pi*(ln(f/f1) / ln(fn/f1)))/2 */
autoSound Sound_createShepardTone (double minimumTime, double maximumTime, double samplingFrequency, double lowestFrequency, integer numberOfComponents, double frequencyChange_st, double amplitudeRange) {
	try {
		double scale = pow (2.0, numberOfComponents);
		double maximumFrequency = lowestFrequency * scale;
		double lmin = pow (10.0, - amplitudeRange / 10.0), twoPi = 2.0 * NUMpi;
		double ln2t0 = log (2.0) * frequencyChange_st / 12.0;
		double lnf1 = log (lowestFrequency + 1.0);
		double amplarg = twoPi / log ((maximumFrequency + 1.0) / (lowestFrequency + 1.0));

		Melder_require (lowestFrequency <= 0.5 * samplingFrequency, U"Sound_createShepardTone: lowest frequency too high.");
		Melder_require (maximumFrequency <= 0.5 * samplingFrequency, U"Sound_createShepardTone: frequency of highest component too high.");
		autoSound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);

		for (integer i = 1; i <= my nx; i ++) {
			double argt, t = (i - 0.5) * my dx, ft = lowestFrequency;
			if (frequencyChange_st != 0.0) {
				double expt = exp (ln2t0 * t);
				argt = twoPi * lowestFrequency * (expt - 1.0) / ln2t0;
				ft *= expt;
			} else {
				argt = twoPi * ft * t;
			}
			for (integer j = 1; j <= numberOfComponents; j ++) {
				while (ft >= maximumFrequency) {
					ft /= scale;
					argt /= scale;
				}
				//amplitude = lmin + (1 - lmin) * (1 - cos (twoPi * log (ft + 1) / log (maximumFrequency + 1))) / 2;
				double amplitude = lmin + (1 - lmin) * (1 - cos (amplarg * (log (ft + 1) - lnf1))) / 2.0;
				my z [1] [i] += amplitude * sin (argt);
				ft *= 2.0; argt *= 2.0;
			}
		}
		Vector_scale (me.get(), 0.99996948);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Shepard tone.");
	}
}

autoSound Sound_createPattersonWightmanTone (double minimumTime, double maximumTime, double samplingFrequency, double baseFrequency, double frequencyShiftRatio, integer numberOfComponents) {
	try {
		Melder_require ((numberOfComponents - 1 + frequencyShiftRatio) * baseFrequency <=  samplingFrequency / 2.0,
			U"Frequency of one or more components too large.");
		autoSound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
		double w0 = NUM2pi * baseFrequency;
		for (integer i = 1; i <= my nx; i ++) {
			double a = 0, t = (i - 0.5) * my dx;
			for (integer j = 1; j <= numberOfComponents; j ++) {
				a += sin ( (j + frequencyShiftRatio) * w0 * t);
			}
			my z [1] [i] = a;
		}
		Vector_scale (me.get(), 0.99996948);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Patterson Wightman tone.");
	}
}

autoSound Sound_createPlompTone (double minimumTime, double maximumTime, double samplingFrequency, double baseFrequency, double frequencyFraction, integer m) {
	try {
		Melder_require (12.0 * (1.0 + frequencyFraction) * baseFrequency <=  samplingFrequency / 2.0,
			U"Sound_createPlompTone: frequency of one or more components too large.");
		
		double w1 = NUM2pi * (1.0 - frequencyFraction) * baseFrequency;
		double w2 = NUM2pi * (1.0 + frequencyFraction) * baseFrequency;
		autoSound me = Sound_create2 (minimumTime, maximumTime, samplingFrequency);
		for (integer i = 1; i <= my nx; i ++) {
			double a = 0, t = (i - 0.5) * my dx;
			for (integer j = 1; j <= m; j ++) {
				a += sin (j * w1 * t);
			}
			for (integer j  = m + 1; j <= 12; j ++) {
				a += sin (j * w2 * t);
			}
			my z [1] [i] = a;
		}
		Vector_scale (me.get(), 0.99996948);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Plomp tone.");
	}
}

void Sounds_multiply (Sound me, Sound thee) {
	integer n = std::min (my nx, thy nx );
	my z.row(1).part(1, n)  *=  thy z.row(1).part(1, n);
}


double Sound_power (Sound me) {
	double e = 0.0;
	constVEC s = my z.row (1);
	for (integer i = 1; i <= my nx; i ++)
		e += s [i] * s [i];
	return sqrt (e) * my dx / (my xmax - my xmin);
}

double Sound_correlateParts (Sound me, double tx, double ty, double duration) {
	if (ty < tx)
		std::swap (tx, ty);
	integer nbx = Sampled_xToNearestIndex (me, tx);
	integer nby = Sampled_xToNearestIndex (me, ty);
	integer ney = Sampled_xToNearestIndex (me, ty + duration);

	integer increment = 0, decrement = 0;
	if (nbx < 1)
		increment = 1 - nbx;
	if (ney > my nx)
		decrement = ney - my nx;

	integer ns = Melder_ifloor (duration / my dx) - increment - decrement;
	if (ns < 1)
		return 0.0;

	double *x = & my z [1] [nbx + increment - 1];
	double *y = & my z [1] [nby + increment - 1];
	double xm = 0.0, ym = 0.0, sxx = 0.0, syy = 0.0, sxy = 0.0;
	for (integer i = 1; i <= ns; i ++) {
		xm += x [i];
		ym += y [i];
	}
	xm /= ns;
	ym /= ns;
	for (integer i = 1; i <= ns; i ++) {
		double xt = x [i] - xm, yt = y [i] - ym;
		sxx += xt * xt;
		syy += yt * yt;
		sxy += xt * yt;
	}
	double denum = sxx * syy;
	double rxy = denum > 0.0 ? sxy / sqrt (denum) : 0.0;
	return rxy;
}

double Sound_localMean (Sound me, double fromTime, double toTime) {
	integer n1 = Sampled_xToNearestIndex (me, fromTime);
	integer n2 = Sampled_xToNearestIndex (me, toTime);
	double mean = undefined;
	if (fromTime <= toTime) {
		if (n1 < 1)
			n1 = 1;
		if (n2 > my nx)
			n2 = my nx;
		Melder_assert (n1 <= n2);
		mean = NUMmean (constVEC (& my z [1] [n1], n2 - n1 + 1));
	}
	return mean;
}

static double interpolate (Sound me, integer i1, integer channel, double level)
/* Precondition: my z [1] [i1] != my z [1] [i1 + 1]; */
{
	integer i2 = i1 + 1;
	double x1 = Sampled_indexToX (me, i1), x2 = Sampled_indexToX (me, i2);
	double y1 = my z [channel] [i1], y2 = my z [channel] [i2];
	return x1 + (x2 - x1) * (y1 - level) / (y1 - y2);   // linear
}

double Sound_getNearestLevelCrossing (Sound me, integer channel, double position, double level, kSoundSearchDirection searchDirection) {
	double *amplitude = & my z [channel] [0];
	integer leftSample = Sampled_xToLowIndex (me, position);
	if (leftSample > my nx)
		return undefined;
	integer rightSample = leftSample + 1;
	integer ileft, iright;
	double leftCrossing, rightCrossing;
	/*
		Are we already at a level crossing?
	*/
	if (leftSample >= 1 && rightSample <= my nx &&
		(amplitude [leftSample] >= level) != (amplitude [rightSample] >= level)) {
		double crossing = interpolate (me, leftSample, channel, level);
		return searchDirection == kSoundSearchDirection::Left ?
			( crossing <= position ? crossing : undefined ) :
			( crossing >= position ? crossing : undefined );
	}
	
	if (searchDirection == kSoundSearchDirection::Left || 
		searchDirection == kSoundSearchDirection::Nearest) {
		for (ileft = leftSample - 1; ileft >= 1; ileft --)
			if ((amplitude [ileft] >= level) != (amplitude [ileft + 1] >= level))
				break;
		leftCrossing = interpolate (me, ileft, channel, level);
		if (searchDirection == kSoundSearchDirection::Left)
			return ileft < 1 ? undefined: leftCrossing;
	}
	
	if (rightSample < 1)
		return undefined;
	if (searchDirection == kSoundSearchDirection::Right || 
		searchDirection == kSoundSearchDirection::Nearest) {
		for (iright = rightSample + 1; iright <= my nx; iright ++)
			if ((amplitude [iright] >= level) != (amplitude [iright - 1] >= level))
				break;
		rightCrossing = interpolate (me, iright - 1, channel, level);
		if (searchDirection == kSoundSearchDirection::Right)
			return iright > my nx ? undefined : rightCrossing;
	}
	
	if (ileft < 1 && iright > my nx) return undefined;
	return ileft < 1 ? rightCrossing : ( iright > my nx ? leftCrossing :
		( position - leftCrossing < rightCrossing - position ? leftCrossing : rightCrossing ) );
}

double Sound_localPeak (Sound me, double fromTime, double toTime, double reference) {
	integer n1 = Sampled_xToNearestIndex (me, fromTime);
	integer n2 = Sampled_xToNearestIndex (me, toTime);
	double *s = & my z [1] [0], peak = -1e308;
	if (fromTime <= toTime) {
		if (n1 < 1)
			n1 = 1;
		if (n2 > my nx)
			n2 = my nx;
		for (integer i = n1; i <= n2; i ++) {
			double ds = fabs (s [i] - reference);
			if (ds > peak) {
				peak = ds;
			}
		}
	}
	return peak;
}

void Sound_into_Sound (Sound me, Sound to, double startTime) {
	integer index = Sampled_xToNearestIndex (me, startTime);
	for (integer i = 1; i <= to -> nx; i ++) {
		integer j = index - 1 + i;
		to -> z [1] [i] = j < 1 || j > my nx ? 0.0 : my z [1] [j];
	}
}

/*
IntervalTier Sound_PointProcess_to_IntervalTier (Sound me, PointProcess thee, double window);
IntervalTier Sound_PointProcess_to_IntervalTier (Sound me, PointProcess thee, double window)
{
	double window2 = window / 2;
	double t1 = thy t [1] - window2;
	if (t1 < my xmin) t1 = my xmin;
	double t2 = t1 + window2;
	if (t2 > my xmax) t2 = my xmax;
	autoIntervalTier him = IntervalTier_create (my xmin, my xmax);
	autoTextInterval interval = TextInterval_create (t1, t2, "yes");
	his intervals -> addItem_move (interval.move());

	for (integer i = 2; i <= thy nt; i ++)
	{
		double t =  thy t [i];

		if (t <= t2)
		{
			integer index = his points.size;
			RealPoint point = his points->at [index];
			t2 = t + window2;
			if (t2 > my xmax) t2 = my xmax;
			point -> value = t2;
		}
		else
		{
			t2 = t + window2;
			if (t2 > my xmax) t2 = my xmax;
			RealTier_addPoint (him, t, t2);
		}
	}
	return him;
}
*/


/*
	First approximation on the basis of differences in the sampled signal.
	The underlying analog signal still could have jumps undetected by this algorithm.
	We could get a better approximation by first upsampling the signal.
*/
autoPointProcess Sound_to_PointProcess_getJumps (Sound me, double minimumJump, double dt) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		integer i = 1, dtn = Melder_ifloor (dt / my dx);   // ppgb: what does dtn mean?
		if (dtn < 1)
			dtn = 1;
		constVEC s = my z.row (1);   // ppgb: what does s mean?
		while (i < my nx) {   // ppgb: what does i mean?
			integer j = i + 1, step = 1;
			while (j <= i + dtn && j <= my nx) {
				if (fabs (s [i] - s [j]) > minimumJump) {
					double t = Sampled_indexToX (me, i);
					PointProcess_addPoint (thee.get(), t);
					step = j - i + 1;
					break;
				}
				j ++;
			}
			i += step;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PointProcess created.");
	}
}

/* Internal pitch representation in semitones */
autoSound Sound_Pitch_changeSpeaker (Sound me, Pitch him, double formantMultiplier, double pitchMultiplier, double pitchRangeMultiplier, double durationMultiplier) {
	try {
		double samplingFrequency_old = 1.0 / my dx;

		Melder_require (my xmin == his xmin && my xmax == his xmax, U"The Pitch and the Sound object should have the same domain.");
		
		autoSound sound = Data_copy (me);
		Vector_subtractMean (sound.get());

		if (formantMultiplier != 1.0) {
			// Shift all frequencies (inclusive pitch!) */
			Sound_overrideSamplingFrequency (sound.get(), samplingFrequency_old * formantMultiplier);
		}

		autoPitch pitch = Data_copy (him);
		Pitch_scaleDuration (pitch.get(), 1.0 / formantMultiplier); //
		Pitch_scalePitch (pitch.get(), formantMultiplier);

		autoPointProcess pulses = Sound_Pitch_to_PointProcess_cc (sound.get(), pitch.get());
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch.get());

		double median = Pitch_getQuantile (pitch.get(), 0.0, 0.0, 0.5, kPitch_unit::HERTZ);
		if (isdefined (median) && median != 0.0) {
			/* Incorporate pitch shift from overriding the sampling frequency */
			PitchTier_multiplyFrequencies (pitchTier.get(), sound -> xmin, sound -> xmax, pitchMultiplier / formantMultiplier);
			PitchTier_modifyExcursionRange (pitchTier.get(), sound -> xmin, sound -> xmax, pitchRangeMultiplier, median);
		} else if (pitchMultiplier != 1) {
			Melder_warning (U"Pitch has not been changed because the sound was entirely voiceless.");
		}
		autoDurationTier duration = DurationTier_create (my xmin, my xmax);
		RealTier_addPoint (duration.get(), (my xmin + my xmax) / 2, formantMultiplier * durationMultiplier);

		autoSound thee = Sound_Point_Pitch_Duration_to_Sound (sound.get(), pulses.get(), pitchTier.get(), duration.get(), MAX_T);

		// Resample to the original sampling frequency

		if (formantMultiplier != 1.0) {
			thee = Sound_resample (thee.get(), samplingFrequency_old, 10);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Pitch & Sound.");
	}
}

autoSound Sound_changeSpeaker (Sound me, double pitchMin, double pitchMax, double formantMultiplier, double pitchMultiplier,  double pitchRangeMultiplier, double durationMultiplier) { // > 0
	try {
		autoPitch pitch = Sound_to_Pitch (me, 0.8 / pitchMin, pitchMin, pitchMax);
		autoSound thee = Sound_Pitch_changeSpeaker (me, pitch.get(), formantMultiplier, pitchMultiplier, pitchRangeMultiplier, durationMultiplier);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": speaker not changed.");
	}
}

autoTextGrid Sound_to_TextGrid_detectSilences (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration,
	conststring32 silentLabel, conststring32 soundingLabel) {
	try {
		bool subtractMeanPressure = true;
		autoSound filtered = Sound_filter_passHannBand (me, 80.0, 8000.0, 80.0);
		autoIntensity thee = Sound_to_Intensity (filtered.get(), minPitch, timeStep, subtractMeanPressure);
		autoTextGrid him = Intensity_to_TextGrid_detectSilences (thee.get(), silenceThreshold, minSilenceDuration, minSoundingDuration, silentLabel, soundingLabel);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no TextGrid with silences created.");
	}
}

void Sound_getStartAndEndTimesOfSounding (Sound me, double minPitch, double timeStep, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double *t1, double *t2) {
	try {
		conststring32 silentLabel = U"-", soundingLabel = U"+";
		autoTextGrid dbs = Sound_to_TextGrid_detectSilences (me, minPitch, timeStep, silenceThreshold, minSilenceDuration, minSoundingDuration, silentLabel, soundingLabel);
		IntervalTier tier = (IntervalTier) dbs -> tiers->at [1];
		Melder_assert (tier -> intervals.size > 0);
		TextInterval interval = tier -> intervals.at [1];
		if (t1) {
			*t1 = my xmin;
			if (Melder_equ (interval -> text.get(), silentLabel)) {
				*t1 = interval -> xmax;
			}
		}
		if (t2) {
			*t2 = my xmax;
			interval = tier -> intervals.at [tier -> intervals.size];
			if (Melder_equ (interval -> text.get(), silentLabel)) {
				*t2 = interval -> xmin;
			}
		}
	} catch (MelderError) {
		Melder_throw (U"Sounding times not found.");
	}
}

autoSound Sound_IntervalTier_cutPartsMatchingLabel (Sound me, IntervalTier thee, conststring32 match) {
    try {
        // count samples of the trimmed sound
        integer ixmin, ixmax, numberOfSamples = 0, previous_ixmax = 0;
		double xmin = my xmin; // start time of output sound is start time of input sound
        for (integer iint = 1; iint <= thy intervals.size; iint ++) {
            TextInterval interval = thy intervals.at [iint];
            if (! Melder_equ (interval -> text.get(), match)) {
                numberOfSamples += Sampled_getWindowSamples (me, interval -> xmin, interval -> xmax, & ixmin, & ixmax);
                // if two contiguous intervals have to be copied then the last sample of previous interval
                // and first sample of current interval might sometimes be equal
				if (ixmin == previous_ixmax) {
					-- numberOfSamples;
				}
				previous_ixmax = ixmax;
			} else { // matches label
				if (iint == 1) { // Start time of output sound is end time of first interval
					xmin = interval -> xmax;
				}
            }
        }
        // Now copy the parts. The output sound starts at xmin
        autoSound him = Sound_create (my ny, xmin, xmin + numberOfSamples * my dx, numberOfSamples, my dx, xmin + 0.5 * my dx);
        numberOfSamples = 0;
		previous_ixmax = 0;
        for (integer iint = 1; iint <= thy intervals.size; iint ++) {
            TextInterval interval = thy intervals.at [iint];
            if (! Melder_equ (interval -> text.get(), match)) {
                Sampled_getWindowSamples (me, interval -> xmin, interval -> xmax, & ixmin, & ixmax);
				if (ixmin == previous_ixmax) {
					ixmin ++;
				}
				integer ipos = 0; // to prevent compiler warning -Wmaybe-uninitialized
				previous_ixmax = ixmax;
                for (integer ichan = 1; ichan <= my ny; ichan ++) {
                    ipos = numberOfSamples + 1;
                    for (integer i = ixmin; i <= ixmax; i ++, ipos ++) {
                        his z [ichan] [ipos] = my z [ichan] [i];
                    }
                }
                numberOfSamples = -- ipos;
            }
        }
        Melder_assert (numberOfSamples == his nx);
        return him;
    } catch (MelderError) {
        Melder_throw (me, U": intervals not trimmed.");
    }
}

autoSound Sound_trimSilences (Sound me, double trimDuration, bool onlyAtStartAndEnd, double minPitch, double timeStep, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, autoTextGrid *p_tg, conststring32 trimLabel) {
    try {
		Melder_require (my ny == 1, U"The sound should be a mono sound.");
		
        conststring32 silentLabel = U"silent", soundingLabel = U"sounding";
        conststring32 copyLabel = U"";
        autoTextGrid tg = Sound_to_TextGrid_detectSilences (me, minPitch, timeStep, silenceThreshold, minSilenceDuration, minSoundingDuration, silentLabel, soundingLabel);
        autoIntervalTier itg = Data_copy ((IntervalTier) tg -> tiers->at [1]);
        IntervalTier tier = (IntervalTier) tg -> tiers->at [1];
        for (integer iint = 1; iint <= tier -> intervals.size; iint ++) {
            TextInterval ti = tier -> intervals.at [iint];
            TextInterval ati = itg -> intervals.at [iint];
            double duration = ti -> xmax - ti -> xmin;
            if (duration > trimDuration && Melder_equ (ti -> text.get(), silentLabel)) {   // silent
				conststring32 label = trimLabel;
                if (iint == 1) { // first is special
                    double trim_t = ti -> xmax - trimDuration;
                    IntervalTier_moveBoundary (itg.get(), iint, false, trim_t);
                } else if (iint == tier -> intervals.size) {   // last is special
                    double trim_t = ti -> xmin + trimDuration;
                    IntervalTier_moveBoundary (itg.get(), iint, true, trim_t);
                } else {
					if (onlyAtStartAndEnd) {
						label = ati -> text.get();
					} else {
                    	double trim_t = ti -> xmin + 0.5 * trimDuration;
						IntervalTier_moveBoundary (itg.get(), iint, true, trim_t);
                    	trim_t = ti -> xmax - 0.5 * trimDuration;
                    	IntervalTier_moveBoundary (itg.get(), iint, false, trim_t);
					}
                }
                TextInterval_setText (ati, label);
            } else {   // sounding
                TextInterval_setText (ati, copyLabel);
            }
        }
        autoSound thee = Sound_IntervalTier_cutPartsMatchingLabel (me, itg.get(), trimLabel);
        if (p_tg) {
			TextGrid_addTier_copy (tg.get(), itg.get());
            *p_tg = tg.move();
        }
        return thee;
    } catch (MelderError) {
        Melder_throw (me, U": silences not trimmed.");
    }
}

autoSound Sound_trimSilencesAtStartAndEnd (Sound me, double trimDuration, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double *startTimeOfSounding, double *endTimeOfSounding)
{
	try {
		autoTextGrid tg;
		autoSound thee = Sound_trimSilences (me, trimDuration, true, minPitch, timeStep, silenceThreshold, 
			minSilenceDuration, minSoundingDuration, & tg, U"trimmed");
		IntervalTier trim = (IntervalTier) tg -> tiers->at [2];
		TextInterval ti1 = trim -> intervals.at [1];
		if (startTimeOfSounding) {
			*startTimeOfSounding = my xmin;
			if (Melder_equ (ti1 -> text.get(), U"trimmed"))	*startTimeOfSounding = ti1 -> xmax;
		}
		TextInterval ti2 = trim -> intervals.at [trim -> intervals.size];
		if (endTimeOfSounding) {
			*endTimeOfSounding = my xmax;
			if (Melder_equ (ti2 -> text.get(), U"trimmed")) *endTimeOfSounding = ti2 -> xmin;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": silences not trimmed.");
	}
}

/*  Compatibility with old Sound(&pitch)_changeGender  ***********************************/

static void PitchTier_modifyRange_old (PitchTier me, double tmin, double tmax, double factor, double fmid) {
	for (integer i = 1; i <= my points.size; i ++) {
		RealPoint point = my points.at [i];
		double f = point -> value;
		if (point -> number < tmin || point -> number > tmax) {
			continue;
		}
		f = fmid + (f - fmid) * factor;
		point -> value = f < 0.0 ? 0.0 : f;
	}
}

static autoPitch Pitch_scaleTime_old (Pitch me, double scaleFactor) {
	try {
		double dx = my dx, x1 = my x1, xmax = my xmax;
		if (scaleFactor != 1.0) {
			dx = my dx * scaleFactor;
			x1 = my xmin + 0.5 * dx;
			xmax = my xmin + my nx * dx;
		}
		autoPitch thee = Pitch_create (my xmin, xmax, my nx, dx, x1, my ceiling, 2);

		for (integer i = 1; i <= my nx; i ++) {
			double f = my frame [i].candidate [1].frequency;
			thy frame [i].candidate [1].strength = my frame [i].candidate [1].strength;
			f /= scaleFactor;
			if (f < my ceiling) {
				thy frame [i]. candidate [1]. frequency = f;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Pitch not scaled.");
	}
}

autoSound Sound_Pitch_changeGender_old (Sound me, Pitch him, double formantRatio, double new_pitch, double pitchRangeFactor, double durationFactor) {
	try {
		double samplingFrequency_old = 1 / my dx;

		Melder_require (my ny == 1, U"Change Gender works only on mono sounds.");
		Melder_require (my xmin == his xmin && my xmax == his xmax, U"The Pitch and the Sound object should have the same domain.");
		Melder_require (new_pitch >= 0, U"The new pitch median should not be negative."); 

		autoSound sound = Data_copy (me);
		Vector_subtractMean (sound.get());

		if (formantRatio != 1.0) {
			// Shift all frequencies (inclusive pitch!)
			Sound_overrideSamplingFrequency (sound.get(), samplingFrequency_old * formantRatio);
		}

		autoPitch pitch = Pitch_scaleTime_old (him, 1 / formantRatio);
		autoPointProcess pulses = Sound_Pitch_to_PointProcess_cc (sound.get(), pitch.get());
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch.get());

		double median = Pitch_getQuantile (pitch.get(), 0, 0, 0.5, kPitch_unit::HERTZ);
		if (isdefined (median) && median != 0.0) {
			// Incorporate pitch shift from overriding the sampling frequency
			if (new_pitch == 0.0) {
				new_pitch = median / formantRatio;
			}
			double factor = new_pitch / median;
			PitchTier_multiplyFrequencies (pitchTier.get(), sound -> xmin, sound -> xmax, factor);
			PitchTier_modifyRange_old (pitchTier.get(), sound -> xmin, sound -> xmax, pitchRangeFactor, new_pitch);
		} else {
			Melder_warning (U"There were no voiced segments found.");
		}
		autoDurationTier duration = DurationTier_create (my xmin, my xmax);
		RealTier_addPoint (duration.get(), (my xmin + my xmax) / 2, formantRatio * durationFactor);

		autoSound thee = Sound_Point_Pitch_Duration_to_Sound (sound.get(), pulses.get(), pitchTier.get(),
			duration.get(), 1.25 / Pitch_getMinimum (pitch.get(), 0.0, 0.0, kPitch_unit::HERTZ, false));

		// Resample to the original sampling frequency

		if (formantRatio != 1.0) {
			thee = Sound_resample (thee.get(), samplingFrequency_old, 10);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from Pitch & Sound.");
	}
}

autoSound Sound_changeGender_old (Sound me, double fmin, double fmax, double formantRatio, double new_pitch, double pitchRangeFactor, double durationFactor) {
	try {
		autoPitch pitch = Sound_to_Pitch (me, 0.8 / fmin, fmin, fmax);
		autoSound thee = Sound_Pitch_changeGender_old (me, pitch.get(), formantRatio, new_pitch, pitchRangeFactor, durationFactor);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created for gender change.");
	}
}

/*  End of compatibility with Sound_changeGender and Sound_Pitch_changeGender ***********************************/

/* Draw a sound vertically, from bottom to top */
void Sound_draw_btlr (Sound me, Graphics g, double tmin, double tmax, double amin, double amax, kSoundDrawingDirection drawingDirection, bool garnish) {
	double xmin, xmax, ymin, ymax;

	if (tmin == tmax) {
		tmin = my xmin;
		tmax = my xmax;
	}
	integer itmin, itmax;
	Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
	if (amin == amax) {
		Matrix_getWindowExtrema (me, itmin, itmax, 1, my ny, & amin, & amax);
		if (amin == amax) {
			amin -= 1.0;
			amax += 1.0;
		}
	}
	/* In bottom-to-top-drawing the maximum amplitude is on the left, minimum on the right */
	if (drawingDirection == kSoundDrawingDirection::BottomToTop) {
		xmin = amax;
		xmax = amin;
		ymin = tmin;
		ymax = tmax;
	} else if (drawingDirection == kSoundDrawingDirection::TopToBottom) {
		xmin = amin;
		xmax = amax;
		ymin = tmax;
		ymax = tmin;
	} else if (drawingDirection == kSoundDrawingDirection::RightToLeft) {
		xmin = tmax;
		xmax = tmin;
		ymin = amin;
		ymax = amax;
	} else { //if (drawingDirection == kSoundDrawingDirection::LeftToRight)
		xmin = tmin;
		xmax = tmax;
		ymin = amin;
		ymax = amax;
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double a1 = my z [1] [itmin];
	double t1 = Sampled_indexToX (me, itmin);
	for (integer it = itmin + 1; it <= itmax; it ++) {
		double t2 = Sampled_indexToX (me, it);
		double a2 = my z [1] [it];
		if (drawingDirection == kSoundDrawingDirection::BottomToTop ||
			drawingDirection == kSoundDrawingDirection::TopToBottom) {
			Graphics_line (g, a1, t1, a2, t2);
		} else {
			Graphics_line (g, t1, a1, t2, a2);
		}
		a1 = a2;
		t1 = t2;
	}
	if (garnish) {
		if (drawingDirection == kSoundDrawingDirection::BottomToTop) {
			if (amin * amax < 0.0)
				Graphics_markBottom (g, 0.0, false, true, true, nullptr);
		} else if (drawingDirection == kSoundDrawingDirection::TopToBottom) {
			if (amin * amax < 0.0)
				Graphics_markTop (g, 0.0, false, true, true, nullptr);
		} else if (drawingDirection == kSoundDrawingDirection::RightToLeft) {
			if (amin * amax < 0.0)
				Graphics_markRight (g, 0.0, false, true, true, nullptr);
		} else { //if (drawingDirection == kSoundDrawingDirection::LeftToRight)
			if (amin * amax < 0.0)
				Graphics_markLeft (g, 0.0, false, true, true, nullptr);
		}
		Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	}
}

void Sound_fade (Sound me, int channel, double t, double fadeTime, int inout, bool fadeGlobal) {
	integer numberOfSamples = Melder_ifloor (fabs (fadeTime) / my dx);
	double t1 = t, t2 = t1 + fadeTime;
	conststring32 fade_inout = inout > 0 ? U"out" : U"in";
	
	Melder_require (channel >= 0 && channel <= my ny, U"Invalid channel number: ", channel, U".");
	
	if (t > my xmax) {
		t = my xmax;
		if (inout <= 0) { // fade in
			Melder_warning (U"The start time of the fade-in is after the end time of the sound. The fade-in will not happen.");
			return;
		}
	} else if (t < my xmin) {
		t = my xmin;
		if (inout > 0) { // fade out
			Melder_warning (U"The start time of the fade-out is before the start time of the sound. The fade-out will not happen.");
			return;
		}
	}
	if (fadeTime < 0.0) {
		t1 = t + fadeTime;
		t2 = t;
	} else if (fadeTime > 0.0) {
		t1 = t;
		t2 = t + fadeTime;
	} else {
		Melder_warning (U"You have given a \"Fade time\" of zero seconds. The fade-", fade_inout, U" will not happen.");
		return;
	}
	integer i0 = 0, iystart, iyend;
	if (channel == 0) { // all
		iystart = 1; iyend = my ny;
	} else {
		iystart = iyend = channel;
	}

	integer istart = Sampled_xToNearestIndex (me, t1);
	if (istart < 1) {
		istart = 1;
	}
	if (istart >= my nx) {
		Melder_warning (U"The part to fade ", fade_inout, U" lies after the end time of the sound. The fade-",  fade_inout, U" will not happen.");
		return;
	}
	integer iend = Sampled_xToNearestIndex (me, t2);
	if (iend <= 1) {
		Melder_warning (U"The part to fade ", fade_inout, U" lies before the start time of the sound. Fade-", fade_inout, U" will be incomplete.");
		return;
	}
	if (iend > my nx) {
		iend = my nx;
	}
	if (iend - istart + 1 >= numberOfSamples) {
		numberOfSamples = iend - istart + 1;
	} else {
		// If the start of the fade is before xmin, arrange starting phase.
		// The end of the fade after xmax presents no problems (i0 = 0).
		if (fadeTime < 0) {
			i0 = numberOfSamples - (iend - istart + 1);
		}
		Melder_warning (U"The fade time is larger than the part of the sound to fade ", fade_inout, U". Fade-", fade_inout, U" will be incomplete.");
	}
	for (integer ichannel = iystart; ichannel <= iyend; ichannel ++) {
		for (integer i = istart; i <= iend; i ++) {
			double cosp = cos (NUMpi * (i0 + i - istart) / (numberOfSamples - 1));
			if (inout <= 0) {
				cosp = -cosp;    // fade-in
			}
			my z [ichannel] [i] *= 0.5 * (1.0 + cosp);
		}
		if (fadeGlobal) {
			if (inout <= 0) {
				for (integer i = 1; i < istart; i ++) {
					my z [ichannel] [i] = 0.0;
				}
			} else {
				for (integer i = iend; i < my nx; i ++) {
					my z [ichannel] [i] = 0.0;
				}
			}
		}
	}
}

/* 1; rect 2:hamming 3: bartlet 4: welch 5: hanning 6:gaussian */
autoSound Sound_createFromWindowFunction (double windowDuration, double samplingFrequency, int windowType) {
	try {
		autoSound me = Sound_createSimple (1, windowDuration, samplingFrequency);

		for (integer i = 1; i <= my nx; i ++) {
			double phase = (my x1 + (i - 1) * my dx) / windowDuration;
			double value = 1.0;
			switch (windowType) {
				case 1:
					value = 1.0;
					break;
				case 2: /* Hamming */
					value = 0.54 - 0.46 * cos (2.0 * NUMpi * phase);
					break;
				case 3: /* Bartlett */
					value = 1.0 - fabs ( (2.0 * phase - 1.0));
					break;
				case 4: /* Welch */
					value = 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0);
					break;
				case 5: /* Hanning */
					value = 0.5 * (1.0 - cos (2.0 * NUMpi * phase));
					break;
				case 6: { /* Gaussian */
					double edge = exp (-12.0);
					phase -= 0.5;   /* -0.5 .. +0.5 */
					value = (exp (-48.0 * phase * phase) - edge) / (1.0 - edge);
					break;
				}
				break;
			}
			my z [1] [i] = value;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from window function.");
	}
}

/* y [n] = sum(i=-n, i=n, x [n+mi])/(2*n+1) */
autoSound Sound_localAverage (Sound me, double averagingInterval, int windowType) {
	try {
		double windowDuration = windowType == 6 ? 2 * averagingInterval : averagingInterval;
		autoSound thee = Data_copy (me);
		autoSound window = Sound_createFromWindowFunction (windowDuration, 1 / my dx, windowType);

		integer nswindow2 = window -> nx / 2;
		integer nswindow2p = (window -> nx - 1) / 2; // nx is odd: one sample less in the forward direction
		if (nswindow2 < 1) {
			return thee;
		}
		double *w = & window -> z [1] [0];

		for (integer k = 1; k <= thy ny; k ++) {
			for (integer i = 1; i <= my nx; i ++) {
				longdouble sum = 0.0, wsum = 0.0;
				integer m = (nswindow2 + 1 - i + 1) < 1 ? 1 : (nswindow2 + 1 - i + 1);
				integer jfrom = (i - nswindow2) < 1 ? 1 : (i - nswindow2);
				integer jto = (i + nswindow2p) > my nx ? my nx : (i + nswindow2p);
				for (integer j = jfrom; j <= jto; j ++, m ++) {
					sum += my z [k] [j] * w [m];
					wsum += w [m];
				}
				thy z [k] [i] = double (sum / wsum);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound (local average) created.");
	}
}

static void _Sound_garnish (Sound me, Graphics g, double tmin, double tmax, double minimum, double maximum) {
	Graphics_drawInnerBox (g);
	Graphics_textBottom (g, true, U"Time (s)");
	Graphics_marksBottom (g, 2, true, true, false);
	Graphics_setWindow (g, tmin, tmax, minimum - (my ny - 1) * (maximum - minimum), maximum);
	Graphics_markLeft (g, minimum, true, true, false, nullptr);
	Graphics_markLeft (g, maximum, true, true, false, nullptr);
	if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
		Graphics_markLeft (g, 0.0, true, true, true, nullptr);
	}
	if (my ny == 2) {
		Graphics_setWindow (g, tmin, tmax, minimum, maximum + (my ny - 1) * (maximum - minimum));
		Graphics_markRight (g, minimum, true, true, false, nullptr);
		Graphics_markRight (g, maximum, true, true, false, nullptr);
		if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
			Graphics_markRight (g, 0.0, true, true, true, nullptr);
		}
	}
}

static void autowindowAndGetWindowSamplesAndAutoscale (Sound me, double *tmin, double *tmax, double *minimum, double *maximum, integer *ixmin, integer *ixmax) {
	if (*tmin == *tmax) {
		*tmin = my xmin;
		*tmax = my xmax;
	}

	Matrix_getWindowSamplesX (me, *tmin, *tmax, ixmin, ixmax);

	if (*minimum == *maximum) {
		Matrix_getWindowExtrema (me, *ixmin, *ixmax, 1, my ny, minimum, maximum);
		if (*minimum == *maximum) {
			*minimum -= 1.0;
			*maximum += 1.0;
		}
	}
}

/*
	 Given sample numbers ileft and ileft+1, where the formula evaluates to the booleans left and right, respectively.
	 We want to find the x value in this interval where the formula switches from true to false.
	 The x-value of the best point is approximated by a number of bisections.
	 It is essential that the intermediate interpolated y-values are always between the values at samples ileft and ileft+1.
	 We cannot use a sinc-interpolation because at strong amplitude changes high-frequency oscillations may occur.
*/
static void Sound_findIntermediatePoint_bs (Sound me, integer ichannel, integer ileft, bool left, bool right,
	conststring32 formula, Interpreter interpreter, integer numberOfBisections, double *x, double *y)
{
	Melder_require (left != right, U"Invalid situation.");
	
	if (left) {
		*x = Matrix_columnToX (me, ileft);
		*y = my z [ichannel] [ileft];
	} else {
		*x = Matrix_columnToX (me, ileft + 1);
		*y = my z [ichannel] [ileft + 1];
	}

	if (numberOfBisections < 1)
		return;
	
	/*
		For the bisection we create a Sound with only 3 samples in it which is sufficient for doing linear interpolation.
		The domain needs to be the same as the sound otherwise the formula might give wrong answers because 
		it might contains references to other matrix objects!
		We also need all the channels because the formula might involve relations between the 
		sample values in these channels too!
	*/

	double xleft = Matrix_columnToX (me, ileft);
	autoSound thee = Sound_create (my ny, my xmin, my xmax, 3, 0.5 * my dx, xleft);

	for (integer channel = 1; channel <= my ny; channel ++) {
		thy z [channel] [1] = my z [channel] [ileft];
		thy z [channel] [3] = my z [channel] [ileft + 1];
	}
	integer istep = 1;
	double xright = xleft + my dx, xmid; // !!
	do {
		xmid = 0.5 * (xleft + xright); // the bisection

		for (integer channel = 1; channel <= my ny; channel ++)
			thy z [channel] [2] = Vector_getValueAtX (me, xmid, channel, Vector_VALUE_INTERPOLATION_LINEAR);
		Formula_compile (interpreter, thee.get(), formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		Formula_run (ichannel, 2, & result);
		bool mid = ( result. numericResult != 0.0 );

		thy dx *= 0.5;
		if (left == mid) {
			xleft = xmid;
			for (integer channel = 1; channel <= my ny; channel ++)
				thy z [channel] [1] = thy z [channel] [2];
			thy x1 = xleft;
		} else {
			xright = xmid;
			for (integer channel = 1; channel <= my ny; channel ++)
				thy z [channel] [3] = thy z [channel] [2];
		}
		istep ++;
	} while (istep < numberOfBisections);

	*x = xmid;
	*y = thy z [ichannel] [2];
}

void Sound_drawWhere (Sound me, Graphics g, double tmin, double tmax, double minimum, double maximum,
	bool garnish, conststring32 method, integer numberOfBisections, conststring32 formula, Interpreter interpreter) {
	
	Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
	Formula_Result result;

	integer ixmin, ixmax;
	autowindowAndGetWindowSamplesAndAutoscale (me, & tmin, & tmax, & minimum, & maximum, & ixmin, & ixmax);

	// Set coordinates for drawing.

	Graphics_setInner (g);
	for (integer channel = 1; channel <= my ny; channel ++) {
		Graphics_setWindow (g, tmin, tmax, minimum - (my ny - channel) * (maximum - minimum), maximum + (channel - 1) * (maximum - minimum));
		if (str32str (method, U"bars") || str32str (method, U"Bars")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				Formula_run (channel, ix, & result);
				if (result. numericResult != 0.0) {
					double x = Sampled_indexToX (me, ix);
					double y = my z [channel] [ix];
					double left = x - 0.5 * my dx, right = x + 0.5 * my dx;
					if (y > maximum) {
						y = maximum;
					}
					if (left < tmin) {
						left = tmin;
					}
					if (right > tmax) {
						right = tmax;
					}
					Graphics_line (g, left, y, right, y);
					Graphics_line (g, left, y, left, minimum);
					Graphics_line (g, right, y, right, minimum);
				}
			}
		} else if (str32str (method, U"poles") || str32str (method, U"Poles")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				Formula_run (channel, ix, & result);
				if (result. numericResult != 0.0) {
					double x = Sampled_indexToX (me, ix);
					double y = my z [channel] [ix];
					if (y > maximum)
						y = maximum;
					if (y < minimum)
						y = minimum;
					Graphics_line (g, x, 0.0, x, y);
				}
			}
		} else if (str32str (method, U"speckles") || str32str (method, U"Speckles")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				Formula_run (channel, ix, & result);
				if (result. numericResult != 0.0) {
					double x = Sampled_indexToX (me, ix);
					Graphics_speckle (g, x, my z [channel] [ix]);
				}
			}
		} else {
			// The default: draw as a curve.

			Formula_run (channel, 1, & result);
			bool previous = (result. numericResult != 0.0); // numericResult == 0.0 means false!
			integer istart = ixmin; // first sample of segment to be drawn
			double xb, yb, xe, ye;
			for (integer ix = ixmin + 1; ix <= ixmax; ix ++) {
				Formula_run (channel, ix, & result);
				bool current = (result. numericResult != 0.0); // numericResult == 0.0 means false!
				if (previous && not current) { 
					/* 
						T to F change: we are leaving a segment to be drawn
						1. Draw the curve between the sample numbers from istart to ix-1 (previous). 
						2. Find the (x,y) in the interval between sample numbers ix-1 and ix (current) where the change from
							T to F occurs and draw the line between the previous point and (x,y).
						3. Compile the formula again because it has been changed during the interpolation
					*/
					xb = Matrix_columnToX (me, ix - 1);
					yb = my z [channel] [ix - 1];
					if (ix - istart > 1) {
						double x1 = Matrix_columnToX (me, istart);
						Graphics_function (g, & my z [channel] [0], istart, ix - 1, x1, xb);
					}
					Sound_findIntermediatePoint_bs (me, channel, ix - 1, previous, current, formula, interpreter, numberOfBisections, & xe, & ye);
					Graphics_line (g, xb, yb, xe, ye);
					Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
				} else if (not previous && current ) {
					/*
						F to T change: we are entering a segment to be drawn.
						1. Find the (x,y) where the F changes to T and then draw the line from that (x,y) to the current point.
						2. Compile the formula again
					*/
					istart = ix;
					Sound_findIntermediatePoint_bs (me, channel, ix - 1, previous, current, formula, interpreter, numberOfBisections, & xb, & yb);
					xe = Sampled_indexToX (me, ix);
					ye = my z [channel] [ix];
					Graphics_line (g, xb, yb, xe, ye);
					Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
				}
				previous = current;
			}
			if (previous && ixmax - istart > 0) {
				xb = Matrix_columnToX (me, istart);
				xe = Matrix_columnToX (me, ixmax);
				Graphics_function (g, & my z [channel] [0], istart, ixmax, xb, xe);
			}
		}
	}

	Graphics_setWindow (g, tmin, tmax, minimum, maximum);
	if (garnish && my ny == 2)
		Graphics_line (g, tmin, 0.5 * (minimum + maximum), tmax, 0.5 * (minimum + maximum));
	Graphics_unsetInner (g);

	if (garnish)
		_Sound_garnish (me, g, tmin, tmax, minimum, maximum);
}

void Sound_paintWhere (Sound me, Graphics g, Graphics_Colour colour, double tmin, double tmax,
	double minimum, double maximum, double level, bool garnish,
	integer numberOfBisections, conststring32 formula, Interpreter interpreter)
{
	try {
		Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;

		integer ixmin, ixmax;
		autowindowAndGetWindowSamplesAndAutoscale (me, & tmin, & tmax, & minimum, & maximum, & ixmin, & ixmax);

		Graphics_setColour (g, colour);
		Graphics_setInner (g);
		for (integer channel = 1; channel <= my ny; channel ++) {
			Graphics_setWindow (g, tmin, tmax, minimum - (my ny - channel) * (maximum - minimum), maximum + (channel - 1) * (maximum - minimum));
			bool current, previous = true, fill = false; // fill only when leaving area
			double tmini = tmin, tmaxi = tmax, xe, ye;
			integer ix = ixmin;
			do {
				Formula_run (channel, ix, & result);
				current = ( result. numericResult != 0.0 );
				if (ix == ixmin)
					previous = current;
				if (previous != current) {
					Sound_findIntermediatePoint_bs (me, channel, ix - 1, previous, current, formula, interpreter, numberOfBisections, & xe, & ye);
					if (current) { // entering painting area
						tmini = xe;
					} else { //leaving painting area
						tmaxi = xe;
						fill = true;
					}
					Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
				}
				if (ix == ixmax && current) {
					tmaxi = tmax;
					fill = true;
				}
				if (fill) {
					autoPolygon him = Sound_to_Polygon (me, channel, tmini, tmaxi, minimum, maximum, level);
					Graphics_fillArea (g, his numberOfPoints, &his x [1], &his y [1]);
					fill = false;
				}
				previous = current;
			} while ( ++ix <= ixmax);
		}
		Graphics_setWindow (g, tmin, tmax, minimum, maximum);
		if (garnish && my ny == 2)
			Graphics_line (g, tmin, 0.5 * (minimum + maximum), tmax, 0.5 * (minimum + maximum));
		Graphics_unsetInner (g);
		if (garnish)
			_Sound_garnish (me, g, tmin, tmax, minimum, maximum);
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void Sounds_paintEnclosed (Sound me, Sound thee, Graphics g, Graphics_Colour colour, double tmin, double tmax, double minimum, double maximum, bool garnish) {
	try {
		integer ixmin, ixmax, numberOfChannels = std::max (my ny, thy ny);
		double min1 = minimum, max1 = maximum, tmin1 = tmin, tmax1 = tmax;
		double min2 = min1, max2 = max1, tmin2 = tmin1, tmax2 = tmax1;
		double xmin = std::max (my xmin, thy xmin);
		double xmax = std::min (my xmax, thy xmax);
		if (xmax <= xmin)
			return;
		if (tmin >= tmax) {   // ppgb: why this, if autoscaling occurs anyway?
			tmin = xmin;
			tmax = xmax;
		}
		autowindowAndGetWindowSamplesAndAutoscale (thee, & tmin1, & tmax1, & min1, & max1, & ixmin, & ixmax);
		autowindowAndGetWindowSamplesAndAutoscale (me,   & tmin2, & tmax2, & min2, & max2, & ixmin, & ixmax);
		minimum = std::min (min1, min2);
		maximum = std::max (max1, max2);

		Graphics_setColour (g, colour);
		Graphics_setInner (g);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			autoPolygon him = Sounds_to_Polygon_enclosed (me, thee, channel, tmin, tmax, minimum, maximum);
			Graphics_setWindow (g, tmin, tmax, minimum - (numberOfChannels - channel) * (maximum - minimum), maximum + (channel - 1) * (maximum - minimum));
			Graphics_fillArea (g, his numberOfPoints, &his x [1], &his y [1]);
		}
		Graphics_setWindow (g, tmin, tmax, minimum, maximum);
		if (garnish && (my ny == 2 || thy ny == 2))
			Graphics_line (g, tmin, 0.5 * (minimum + maximum), tmax, 0.5 * (minimum + maximum));
		Graphics_unsetInner (g);
		if (garnish)
			_Sound_garnish (my ny == 2 ? me : thee, g, tmin, tmax, minimum, maximum);
	} catch (MelderError) {
		Melder_clearError ();
	}
}

autoSound Sound_copyChannelRanges (Sound me, conststring32 ranges) {
	try {
		autoINTVEC channels = NUMstring_getElementsOfRanges (ranges, my ny, U"channel", true);
		autoSound thee = Sound_create (channels.size, my xmin, my xmax, my nx, my dx, my x1);
		for (integer ichan = 1; ichan <= channels.size; ichan ++)
			thy z.row (ichan) <<= my z.row (channels [ichan]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not extract channels.");
	}
}

/* After a script by Ton Wempe */
static autoSound Sound_reduceNoiseBySpectralSubtraction_mono (Sound me, Sound noise, double windowLength, double noiseReduction_dB) {
	try {
		Melder_require (my dx == noise -> dx,
			U"The sound and the noise should have the same sampling frequency.");
		Melder_require (noise -> ny == 1 && noise -> ny == 1,
			U"The number of channels in the noise and the sound should equal 1.");

		const double samplingFrequency = 1.0 / my dx;
		autoSound denoised = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		autoSound const analysisWindow = Sound_createSimple (1, windowLength, samplingFrequency);
		const integer windowSamples = analysisWindow -> nx;
		autoSound const noise_copy = Data_copy (noise);
		Sound_multiplyByWindow (noise_copy.get(), kSound_windowShape::HANNING);
		const double bandwidth = samplingFrequency / windowSamples;
		autoLtas const noiseLtas = Sound_to_Ltas (noise_copy.get(), bandwidth);
		autoVEC const noiseAmplitudes = newVECraw (noiseLtas -> nx);
		for (integer iband = 1; iband <= noiseLtas -> nx; iband ++) {
			const double powerDensity = 4e-10 * pow (10.0, noiseLtas -> z [1] [iband] / 10.0);
			noiseAmplitudes [iband] = sqrt (0.5 * powerDensity);
		}
		
		autoMelderProgress progress (U"Remove noise");
		
		const double noiseAmplitudeSubtractionScaleFactor = 1.0 - pow (10.0, noiseReduction_dB / 20.0);
		const integer stepSizeSamples = windowSamples / 4;
		const integer numberOfSteps = my nx / stepSizeSamples;
		for (integer istep = 1; istep <= numberOfSteps; istep ++) {
			const integer istart = (istep - 1) * stepSizeSamples + 1;

			if (istart >= my nx)
				break;   // finished
			const integer nsamples = std::min (my nx - istart + 1, windowSamples);
			
			analysisWindow -> z.row (1).part (1, nsamples) <<= my z.row (1).part (istart, istart + nsamples - 1);
			if (nsamples < windowSamples)
				analysisWindow -> z.row (1).part (nsamples + 1, windowSamples) <<= 0.0;
			
			autoSpectrum const analysisSpectrum = Sound_to_Spectrum (analysisWindow.get(), false);

			/*
				Suppress noise in the analysisSpectrum by subtracting the noise spectrum
			*/
			
			VEC const re = analysisSpectrum -> z.row (1), im = analysisSpectrum -> z.row (2);
			for (integer ifreq = 1; ifreq <= analysisSpectrum -> nx; ifreq ++) {
				const double amp = sqrt (re [ifreq] * re [ifreq] + im [ifreq] * im [ifreq]);
				const double factor = std::max (1.0 - noiseAmplitudeSubtractionScaleFactor * noiseAmplitudes [ifreq] / amp, 1e-6);
				re [ifreq] *= factor;
				im [ifreq] *= factor;
			}
			autoSound const suppressed = Spectrum_to_Sound (analysisSpectrum.get());
			Sound_multiplyByWindow (suppressed.get(), kSound_windowShape::HANNING);
			denoised -> z.row (1).part (istart, istart + nsamples - 1)  +=  0.5 * suppressed -> z.row (1).part (1, nsamples); // 0.5 because of 2-fold
			if (istep % 10 == 1)
				Melder_progress (double (istep) / numberOfSteps,
					U"Remove noise: frame ", istep, U" out of ", numberOfSteps, U".");
		}
		return denoised;
	} catch (MelderError) {
		Melder_throw (me, U": noise not subtracted.");
	}
}

//TODO improve?
static void Sound_findNoise (Sound me, double minimumNoiseDuration, double *noiseStart, double *noiseEnd) {
	try {
		*noiseStart = undefined;
		*noiseEnd = undefined;
		autoIntensity const intensity = Sound_to_Intensity (me, 20.0, 0.005, true);
		double tmin = Vector_getXOfMinimum (intensity.get(), intensity -> xmin, intensity ->  xmax, 1) - minimumNoiseDuration / 2.0;
		double tmax = tmin + minimumNoiseDuration;
		if (tmin < my xmin) {
			tmin = my xmin;
			tmax = tmin + minimumNoiseDuration;
		}
		if (tmax > my xmax) {
			tmax = my xmax;
			tmin = tmax - minimumNoiseDuration;
		}
		Melder_require (tmin >= my xmin, U"Sound too short, or window length too long.");
		
		*noiseStart = tmin;
		*noiseEnd = tmax;
	} catch (MelderError) {
		Melder_throw (me, U": noise not found.");
	}
}

autoSound Sound_removeNoise (Sound me, double noiseStart, double noiseEnd, double windowLength, double minBandFilterFrequency, double maxBandFilterFrequency, double smoothing, kSoundNoiseReductionMethod method) {
	return Sound_reduceNoise (me, noiseStart, noiseEnd, windowLength, minBandFilterFrequency, maxBandFilterFrequency, smoothing, 0.0, method);
}

autoSound Sound_reduceNoise (Sound me, double noiseStart, double noiseEnd, double windowLength, double minBandFilterFrequency, double maxBandFilterFrequency, double smoothing, double noiseReduction_dB, kSoundNoiseReductionMethod method) {
	try {
		autoSound const filtered = Sound_filter_passHannBand (me, minBandFilterFrequency, maxBandFilterFrequency, smoothing);
		autoSound denoised = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		const bool findNoise = ( noiseEnd <= noiseStart );
		const double minimumNoiseDuration = 2.0 * windowLength;
		for (integer ichannel = 1; ichannel <= my ny; ichannel ++) {
			autoSound denoisedi, channeli = Sound_extractChannel (filtered.get(), ichannel);
			if (findNoise)
				Sound_findNoise (channeli.get(), minimumNoiseDuration, & noiseStart, & noiseEnd);
			autoSound noise = Sound_extractPart (channeli.get(), noiseStart, noiseEnd, kSound_windowShape::RECTANGULAR, 1.0, false);
			if (method == kSoundNoiseReductionMethod::SpectralSubtraction) {   // spectral subtraction
				denoisedi = Sound_reduceNoiseBySpectralSubtraction_mono (filtered.get(), noise.get(), windowLength, noiseReduction_dB);
			} else {
				Melder_fatal (U"Unknown method in Sound_reduceNoise.");
			}
			denoised -> z.row (ichannel) <<= denoisedi -> z.row (1);
		}
		return denoised;
	} catch (MelderError) {
		Melder_throw (me, U": not denoised.");
	}
}

void Sound_playAsFrequencyShifted (Sound me, double shiftBy, double newSamplingFrequency, integer precision) {
	try {
		autoSpectrum const spectrum = Sound_to_Spectrum (me, true);
		autoSpectrum const shifted = Spectrum_shiftFrequencies (spectrum.get(), shiftBy, newSamplingFrequency / 2, precision);
		autoSound const thee = Spectrum_to_Sound (shifted.get());
		Sound_playPart (thee.get(), my xmin, my xmax, nullptr, nullptr);
	} catch (MelderError) {
		Melder_throw (me, U" not played with frequencies shifted.");
	}
}

/* End of file Sound_extensions.cpp */
