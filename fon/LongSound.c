/* LongSound.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/05/28
 * pb 2002/07/16 GPL
 * pb 2003/09/12 changed 5 to Melder_NUMBER_OF_AUDIO_FILE_TYPES
 * pb 2004/05/14 support for reading 24-bit and 32-bit audio files
 * pb 2004/11/24 made buffer length settable
 */

#include "LongSound.h"
#include "Preferences.h"
#define MARGIN  0.01
#define USE_MEMMOVE  1

static long prefs_bufferLength = 60;   /* seconds */

void LongSound_prefs (void) {
	Resources_addLong ("LongSound.bufferLength", & prefs_bufferLength);
}

long LongSound_getBufferSizePref_seconds (void) { return prefs_bufferLength; }
void LongSound_setBufferSizePref_seconds (long size) { prefs_bufferLength = size < 10 ? 10 : size > 10000 ? 10000: size; }

static void destroy (I) {
	iam (LongSound);
	/*
	 * The play callback may contain a pointer to my byffer.
	 * That pointer is about to dangle, so kill the playback.
	 */
	Melder_stopPlaying (Melder_IMPLICIT);
	if (my f) fclose (my f);
	NUMsvector_free (my buffer, 0);
	inherited (LongSound) destroy (me);
}

static void info (I) {
	iam (LongSound);
	static const char *channelStrings [] = { "none", "mono", "stereo", "three", "quadro" };
	static const char *encodingStrings [] = { "none",
		"linear 8 bit signed", "linear 8 bit unsigned",
		"linear 16 bit big-endian", "linear 16 bit little-endian",
		"linear 24 bit big-endian", "linear 24 bit little-endian",
		"linear 32 bit big-endian", "linear 32 bit little-endian",
		"mu-law", "A-law", "shorten", "polyphone" };
	Melder_info ("Duration: %.17g seconds\nFile name: %s\nFile type: %s\nChannels: %s\nEncoding: %s"
		"\nSample rate: %.17g Hz\nSize: %ld samples\nStart of sample data: %ld bytes from the start of the file.",
		my xmax - my xmin, Melder_fileToPath (& my file),
		my audioFileType > Melder_NUMBER_OF_AUDIO_FILE_TYPES ? "unknown" : Melder_audioFileTypeString (my audioFileType),
		my numberOfChannels > 4 ? "unknown" : channelStrings [my numberOfChannels],
		my encoding > 8 ? "unknown" : encodingStrings [my encoding] ,
		my sampleRate, my nx, my startOfData);
}

static int LongSound_init (LongSound me, MelderFile file) {
	MelderFile_copy (file, & my file);
	MelderFile_open (file); cherror
	my f = file -> filePointer;
	my audioFileType = MelderFile_checkSoundFile (file, & my numberOfChannels, & my encoding, & my sampleRate, & my startOfData, & my nx); cherror
	if (my audioFileType == 0)
		return Melder_error ("File not recognized (LongSound only supports AIFF, AIFC, WAV, NeXT/Sun, and NIST files).");
	if (my encoding == Melder_SHORTEN || my encoding == Melder_POLYPHONE)
		return Melder_error ("LongSound does not support sound files compressed with \"shorten\".");
	my xmin = 0.0;
	my dx = 1 / my sampleRate;
	my xmax = my nx * my dx;
	my x1 = 0.5 * my dx;
	my numberOfBytesPerSamplePoint = Melder_bytesPerSamplePoint (my encoding);
	my bufferLength = prefs_bufferLength;
	for (;;) {
		my nmax = my bufferLength * my numberOfChannels * my sampleRate * (1 + 3 * MARGIN);
		my buffer = NUMsvector (0, my nmax * my numberOfChannels);
		if (my buffer)
			break;
		my bufferLength *= 0.5;   /* Try 30, 15, or 7.5 seconds. */
		if (my bufferLength < 5.0)   /* Too short to be good. */
			return 0;
		Melder_clearError ();   /* Delete out-of-memory message. */
	}
	my imin = 1;
	my imax = 0;
end:
	iferror return 0;
	return 1;
}

static int copy (I, thou) {
	iam (LongSound);
	thouart (LongSound);
	thy f = NULL;
	thy buffer = NULL;
	if (! LongSound_init (thee, & my file)) return 0;
	return 1;
}

class_methods (LongSound, Sampled)
	class_method (destroy)
	class_method (info)
	class_method (copy);
	us -> writeAscii = classData -> writeAscii;
	us -> writeBinary = classData -> writeBinary;
class_methods_end

LongSound LongSound_open (MelderFile fs) {
	LongSound me = new (LongSound);
	if (! me || ! LongSound_init (me, fs)) { forget (me); return Melder_errorp ("LongSound not created."); }
	return me;
}

Sound LongSound_extractPart (LongSound me, double tmin, double tmax, int preserveTimes) {
	Sound thee = NULL;
	long imin, imax, n;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (tmin < my xmin) tmin = my xmin;
	if (tmax > my xmax) tmax = my xmax;
	n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) { Melder_error ("Less than 1 sample in window."); goto end; }
	thee = Sound_create (tmin, tmax, n, my dx, my x1 + (imin - 1) * my dx); cherror
	if (! preserveTimes) thy xmin = 0.0, thy xmax -= tmin, thy x1 -= tmin;
	if (fseek (my f, my startOfData + (imin - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		{ Melder_error ("Cannot seek in file %s.", MelderFile_messageName (& my file)); goto end; }
	Melder_readAudioToFloat (my f, my numberOfChannels, my encoding, thy z [1], NULL, n); cherror
end:
	iferror { forget (thee); Melder_error ("Sound not extracted from LongSound."); }
	return thee;
}

static int _LongSound_readSamples (LongSound me, short *buffer, long imin, long imax) {
	if (fseek (my f, my startOfData + (imin - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		{ Melder_error ("Cannot seek in file %s.", MelderFile_messageName (& my file)); goto end; }
	Melder_readAudioToShort (my f, my numberOfChannels, my encoding, buffer, imax - imin + 1);
end:
	iferror return 0;
	return 1;
}

static void writePartToOpenFile16 (LongSound me, int audioFileType, long imin, long n, MelderFile file, int numberOfChannels_override) {
	long ibuffer, numberOfBuffers, numberOfSamplesInLastBuffer;
	if (fseek (my f, my startOfData + (imin - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		{ Melder_error ("Cannot seek in file %s.", MelderFile_messageName (& my file)); goto end; }
	numberOfBuffers = (n - 1) / my nmax + 1;
	numberOfSamplesInLastBuffer = (n - 1) % my nmax + 1;
	if (file -> filePointer) for (ibuffer = 1; ibuffer <= numberOfBuffers; ibuffer ++) {
		long numberOfSamplesToCopy = ibuffer < numberOfBuffers ? my nmax : numberOfSamplesInLastBuffer;
		Melder_readAudioToShort (my f, my numberOfChannels, my encoding, my buffer, numberOfSamplesToCopy);
		Melder_writeShortToAudio (file -> filePointer, numberOfChannels_override ? numberOfChannels_override : my numberOfChannels, Melder_defaultAudioFileEncoding16 (audioFileType), my buffer, numberOfSamplesToCopy);
	}
end:
	/*
	 * We "have" no samples any longer.
	 */
	my imin = 1;
	my imax = 0;
}

int LongSound_writePartToAudioFile16 (LongSound me, int audioFileType, double tmin, double tmax, MelderFile file) {
	long imin, imax, n;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (tmin < my xmin) tmin = my xmin;
	if (tmax > my xmax) tmax = my xmax;
	n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) return Melder_error ("Less than 1 sample selected.");
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer)
		Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, my sampleRate, n, my numberOfChannels);
	writePartToOpenFile16 (me, audioFileType, imin, n, file, 0);
	MelderFile_close (file);
	iferror return Melder_error ("Sound file not written.");
	return 1;
}

int LongSound_writeChannelToAudioFile16 (LongSound me, int audioFileType, int channel, MelderFile file) {
	if (my numberOfChannels != 2)
		return Melder_error ("This audio file is not a stereo file. It does not have a %s channel.", channel == 0 ? "left" : "right");
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer)
		Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, my sampleRate, my nx, 1);
	writePartToOpenFile16 (me, audioFileType, 1, my nx, file, channel == 0 ? -1 : -2);
	MelderFile_close (file);
	iferror return Melder_error ("Sound file not written.");
	return 1;
}

static int _LongSound_haveSamples (LongSound me, long imin, long imax) {
	long n = imax - imin + 1;
	Melder_assert (n <= my nmax);
	/*
	 * Included?
	 */
	if (imin >= my imin && imax <= my imax) return 1;
	/*
	 * Extendable?
	 */
	if (imin >= my imin && imax - my imin + 1 <= my nmax) {
		if (! _LongSound_readSamples (me, my buffer + (my imax - my imin + 1) * my numberOfChannels, my imax + 1, imax)) return 0;
		my imax = imax;
		return 1;
	}
	/*
	 * Determine the loadable imin..imax.
	 * Add margins on both sides.
	 */
	imin -= MARGIN * n;
	if (imin < 1) imin = 1;
	imax = imin + (1.0 + 2 * MARGIN) * n;
	if (imax > my nx) imax = my nx;
	imin = imax - (1.0 + 2 * MARGIN) * n;
	if (imin < 1) imin = 1;
	Melder_assert (imax - imin + 1 <= my nmax);
	/*
	 * Overlap?
	 */
	if (imax < my imin || imin > my imax) {
		/*
		 * No overlap.
		 */
		if (! _LongSound_readSamples (me, my buffer, imin, imax)) return 0;
	} else if (imin < my imin) {
		/*
		 * Left overlap.
		 */
		if (imax <= my imax) {
			/*
			 * Only left overlap (e.g. scrolling up).
			 */
			long nshift = (imax - my imin + 1) * my numberOfChannels, shift = (my imin - imin) * my numberOfChannels;
			#if USE_MEMMOVE
			memmove (my buffer + shift, my buffer, nshift * sizeof (short));
			#else
			for (i = nshift - 1; i >= 0; i --)
				my buffer [i + shift] = my buffer [i];
			#endif
			if (! _LongSound_readSamples (me, my buffer, imin, my imin - 1)) return 0;
		} else {
			/*
			 * Left and right overlap (e.g. zooming out).
			 */
			long nshift = (my imax - my imin + 1) * my numberOfChannels, shift = (my imin - imin) * my numberOfChannels;
			#if USE_MEMMOVE
			memmove (my buffer + shift, my buffer, nshift * sizeof (short));
			#else
			for (i = nshift - 1; i >= 0; i --)
				my buffer [i + shift] = my buffer [i];
			#endif
			if (! _LongSound_readSamples (me, my buffer, imin, my imin - 1)) return 0;
			if (! _LongSound_readSamples (me, my buffer + (my imax - imin + 1) * my numberOfChannels, my imax + 1, imax)) return 0;
		}
	} else {
		/*
		 * Only right overlap (e.g. scrolling down).
		 */
		long nshift = (my imax - imin + 1) * my numberOfChannels, shift = (imin - my imin) * my numberOfChannels;
		#if USE_MEMMOVE
		memmove (my buffer, my buffer + shift, nshift * sizeof (short));
		#else
		for (i = 0; i < nshift; i ++)
			my buffer [i] = my buffer [i + shift];
		#endif
		if (! _LongSound_readSamples (me, my buffer + (my imax - imin + 1) * my numberOfChannels, my imax + 1, imax)) return 0;
	}
	my imin = imin, my imax = imax;
	return 1;
}

int LongSound_haveWindow (LongSound me, double tmin, double tmax) {
	long imin, imax, n;
	n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if ((1.0 + 2 * MARGIN) * n + 1 > my nmax) return 0;
	_LongSound_haveSamples (me, imin, imax); iferror return 0;
	return 1;
}

void LongSound_getWindowExtrema (LongSound me, double tmin, double tmax, int channel, double *minimum, double *maximum) {
	long imin, imax;
	long i, minimum_int = 32767, maximum_int = -32768;
	(void) Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	*minimum = 1.0;
	*maximum = -1.0;
	LongSound_haveWindow (me, tmin, tmax); iferror { Melder_clearError (); return; }
	for (i = imin; i <= imax; i ++) {
		long value = my buffer [(i - my imin) * my numberOfChannels + channel - 1];
		if (value < minimum_int) minimum_int = value;
		if (value > maximum_int) maximum_int = value;
	}
	*minimum = minimum_int / 32768.0;
	*maximum = maximum_int / 32768.0;
}

static struct LongSoundPlay {
	long numberOfSamples, i1, i2, zeroPadding;
	double tmin, tmax, dt, t1;
	short *resampledBuffer;
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t);
	void *closure;
} thePlayingLongSound;

static int melderPlayCallback (void *closure, long samplesPlayed) {
	struct LongSoundPlay *me = (struct LongSoundPlay *) closure;
	int phase = 2;
	double t = samplesPlayed <= my zeroPadding ? my tmin :
		samplesPlayed >= my zeroPadding + my numberOfSamples ? my tmax :
		my t1 + (my i1 - 1.5 + samplesPlayed - my zeroPadding) * my dt;
	if (! Melder_isPlaying) {
		phase = 3;
		Melder_free (my resampledBuffer);
	}
	if (my callback)
		return my callback (my closure, phase, my tmin, my tmax, t);
	return 1;
}

void LongSound_playPart (LongSound me, double tmin, double tmax,
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure)
{
	struct LongSoundPlay *thee = (struct LongSoundPlay *) & thePlayingLongSound;
	int fits = LongSound_haveWindow (me, tmin, tmax);
	long bestSampleRate = Melder_getBestSampleRate (my sampleRate), n, i1, i2;
	iferror { Melder_flushError (NULL); return; }
	if (! fits) { Melder_flushError ("Sound too long (%ld seconds). Cannot play.", (long) (tmax - tmin)); return; }
	Melder_stopPlaying (Melder_IMPLICIT);
	/*
	 * Assign to *thee only after stopping the playing sound.
	 */
	thy tmin = tmin;
	thy tmax = tmax;
	thy callback = callback;
	thy closure = closure;
	if ((n = Sampled_getWindowSamples (me, tmin, tmax, & i1, & i2)) < 2) return;
	if (bestSampleRate == my sampleRate) {
		thy numberOfSamples = n;
		thy dt = 1 / my sampleRate;
		thy t1 = my x1;
		thy i1 = i1;
		thy i2 = i2;
		thy zeroPadding = (long) (my sampleRate * Melder_getZeroPadding ());
		if (thy callback) thy callback (thy closure, 1, tmin, tmax, tmin);
		if (thy zeroPadding) {
			thy resampledBuffer = Melder_calloc ((thy zeroPadding + thy numberOfSamples + thy zeroPadding) * my numberOfChannels, sizeof (short));
			memcpy (& thy resampledBuffer [thy zeroPadding * my numberOfChannels], & my buffer [(i1 - my imin) * my numberOfChannels],
				thy numberOfSamples * sizeof (short) * my numberOfChannels);
			if (! Melder_play16 (thy resampledBuffer, my sampleRate, thy zeroPadding + thy numberOfSamples + thy zeroPadding,
			    my numberOfChannels, melderPlayCallback, thee))
				Melder_flushError (NULL);
		} else {
			if (! Melder_play16 (my buffer + (i1 - my imin) * my numberOfChannels, my sampleRate,
			   thy numberOfSamples, my numberOfChannels, melderPlayCallback, thee))
				Melder_flushError (NULL);
		}
	} else {
		long newSampleRate = my sampleRate < 11025 ? 11025 : my sampleRate < 22050 ? 22050 : 44100;
		long newN = ((double) n * newSampleRate) / my sampleRate - 1, i;
		long zeroPadding = (long) (newSampleRate * Melder_getZeroPadding ());
		short *resampledBuffer = Melder_calloc ((zeroPadding + newN + zeroPadding) * my numberOfChannels, sizeof (short));
		short *from = my buffer + (i1 - my imin) * my numberOfChannels;   /* Guaranteed: from [0 .. (my imax - my imin + 1) * nchan] */
		double t1 = my x1, dt = 1.0 / newSampleRate;
		if (! resampledBuffer) { Melder_flushError ("Cannot resample and play this sound."); return; }
		thy numberOfSamples = newN;
		thy dt = dt;
		thy t1 = t1 + i1 / my sampleRate;
		thy i1 = 0;
		thy i2 = newN - 1;
		thy zeroPadding = zeroPadding;
		thy resampledBuffer = resampledBuffer;
		if (my numberOfChannels == 1) {
			for (i = 0; i < newN; i ++) {
				double t = t1 + i * dt;   /* From t1 to t1 + (newN-1) * dt */
				double index = (t - t1) * my sampleRate;   /* From 0. */
				long flore = index;   /* DANGEROUS: Implicitly rounding down... */
				double fraction = index - flore;
				resampledBuffer [i + zeroPadding] = (1 - fraction) * from [flore] + fraction * from [flore + 1];
			}
		} else {
			for (i = 0; i < newN; i ++) {
				double t = t1 + i * dt;
				double index = (t - t1) * newSampleRate;
				long flore = index, ii = i + zeroPadding;
				double fraction = index - flore;
				resampledBuffer [ii + ii] = (1 - fraction) * from [flore + flore] + fraction * from [flore + flore + 2];
				resampledBuffer [ii + ii + 1] = (1 - fraction) * from [flore + flore + 1] + fraction * from [flore + flore + 3];
			}
		}
		if (thy callback) thy callback (thy closure, 1, tmin, tmax, tmin);
		if (! Melder_play16 (resampledBuffer, newSampleRate, zeroPadding + newN + zeroPadding, my numberOfChannels, melderPlayCallback, thee))
			Melder_flushError (NULL);
	}
}

int LongSound_concatenate (Ordered me, MelderFile file, int audioFileType) {
	long i, sampleRate, n;   /* Integer sampling frequencies only, because of possible rounding errors. */
	int numberOfChannels;
	Data data;
	if (my size < 1) return Melder_error ("(LongSound_concatenate:) No Sound or LongSound objects to concatenate.");
	/*
	 * The sampling frequencies and numbers of channels must be equal for all (long)sounds.
	 */
	data = my item [1];
	if (data -> methods == (Data_Table) classSound) {
		Sound sound = (Sound) data;
		sampleRate = floor (1.0 / sound -> dx + 0.5);
		numberOfChannels = 1;
		n = sound -> nx;
	} else {
		LongSound longSound = (LongSound) data;
		sampleRate = longSound -> sampleRate;
		numberOfChannels = longSound -> numberOfChannels;
		n = longSound -> nx;
	}
	/*
	 * Check whether all the sampling frequencies and channels match.
	 */
	for (i = 2; i <= my size; i ++) {
		int sampleRatesMatch, numbersOfChannelsMatch;
		data = my item [i];
		if (data -> methods == (Data_Table) classSound) {
			Sound sound = (Sound) data;
			sampleRatesMatch = floor (1.0 / sound -> dx + 0.5) == sampleRate;
			numbersOfChannelsMatch = 1 == numberOfChannels;
			n += sound -> nx;
		} else {
			LongSound longSound = (LongSound) data;
			sampleRatesMatch = longSound -> sampleRate == sampleRate;
			numbersOfChannelsMatch = longSound -> numberOfChannels == numberOfChannels;
			n += longSound -> nx;
		}
		if (! sampleRatesMatch)
			return Melder_error ("(LongSound_concatenate:) Sampling frequencies do not match.");
		if (! numbersOfChannelsMatch)
			return Melder_error ("(LongSound_concatenate:) Cannot mix stereo and mono.");
	}
	/*
	 * Create output file and write header.
	 */
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), "PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer)
		Melder_writeAudioFileHeader16 (file -> filePointer, audioFileType, sampleRate, n, numberOfChannels);
	for (i = 1; i <= my size; i ++) {
		data = my item [i];
		if (data -> methods == (Data_Table) classSound) {
			Sound sound = (Sound) data;
			if (file -> filePointer)
				Melder_writeFloatToAudio (file -> filePointer, Melder_defaultAudioFileEncoding16 (audioFileType),
					& sound -> z [1] [1], sound -> nx, NULL, 0, TRUE);
		} else {
			LongSound longSound = (LongSound) data;
			writePartToOpenFile16 (longSound, audioFileType, 1, longSound -> nx, file, 0);
		}
	}
	MelderFile_close (file);
	iferror return Melder_error ("Sound file not written.");
	return 1;
}

/* End of file LongSound.c */
