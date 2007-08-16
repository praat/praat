/* LongSound.c
 *
 * Copyright (C) 1992-2007 Paul Boersma, 2007 Erez Volk (for FLAC and MP3)
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
 * pb 2006/12/10 MelderInfo
 * pb 2006/12/13 support for IEEE float 32-bit audio files
 * pb 2007/01/01 compatible with stereo sounds
 * pb 2007/01/27 more compatible with stereo sounds
 * pb 2007/03/17 domain quantity
 * Erez Volk 2007/03 FLAC reading
 * Erez Volk 2007/05/14 FLAC writing
 * Erez Volk 2007/06/04 MP3 reading
 */

#include "LongSound.h"
#include "Preferences.h"
#include "flac_FLAC_stream_decoder.h"
#include "mp3.h"
#define MARGIN  0.01
#define USE_MEMMOVE  1

static long prefs_bufferLength = 60;   /* seconds */

void LongSound_prefs (void) {
	Resources_addLong (L"LongSound.bufferLength", & prefs_bufferLength);
}

long LongSound_getBufferSizePref_seconds (void) { return prefs_bufferLength; }
void LongSound_setBufferSizePref_seconds (long size) { prefs_bufferLength = size < 10 ? 10 : size > 10000 ? 10000: size; }

static void destroy (I) {
	iam (LongSound);
	/*
	 * The play callback may contain a pointer to my buffer.
	 * That pointer is about to dangle, so kill the playback.
	 */
	Melder_stopPlaying (Melder_IMPLICIT);
	if (my mp3f)
		mp3f_delete (my mp3f);
	if (my flacDecoder) {
		FLAC__stream_decoder_finish (my flacDecoder); /* Closes my f */
		FLAC__stream_decoder_delete (my flacDecoder);
	}
	else if (my f) fclose (my f);
	NUMsvector_free (my buffer, 0);
	inherited (LongSound) destroy (me);
}

static void info (I) {
	iam (LongSound);
	static const wchar_t *channelStrings [] = { L"none", L"mono", L"stereo", L"three", L"quadro" };
	static const wchar_t *encodingStrings [1+16] = { L"none",
		L"linear 8 bit signed", L"linear 8 bit unsigned",
		L"linear 16 bit big-endian", L"linear 16 bit little-endian",
		L"linear 24 bit big-endian", L"linear 24 bit little-endian",
		L"linear 32 bit big-endian", L"linear 32 bit little-endian",
		L"mu-law", L"A-law", L"shorten", L"polyphone",
		L"IEEE float 32 bit big-endian", L"IEEE float 32 bit little-endian",
		L"FLAC", L"MP3" };
	classData -> info (me);
	MelderInfo_writeLine3 (L"Duration: ", Melder_double (my xmax - my xmin), L" seconds");
	MelderInfo_writeLine2 (L"File name: ", Melder_fileToPathW (& my file));
	MelderInfo_writeLine2 (L"File type: ", my audioFileType > Melder_NUMBER_OF_AUDIO_FILE_TYPES ? L"unknown" : Melder_audioFileTypeString (my audioFileType));
	MelderInfo_writeLine2 (L"Channels: ", my numberOfChannels > 4 ? L"unknown" : channelStrings [my numberOfChannels]);
	MelderInfo_writeLine2 (L"Encoding: ", my encoding > 16 ? L"unknown" : encodingStrings [my encoding]);
	MelderInfo_writeLine3 (L"Sampling frequency: ", Melder_double (my sampleRate), L" Hertz");
	MelderInfo_writeLine3 (L"Size: ", Melder_integer (my nx), L" samples");
	MelderInfo_writeLine3 (L"Start of sample data: ", Melder_integer (my startOfData), L" bytes from the start of the file");
}

static void _LongSound_FLAC_convertFloats (LongSound me, const FLAC__int32 * const samples[], long bitsPerSample, long numberOfSamples) {
	long i, j;
	const FLAC__int32 *input;
	float multiplier, *output;
	switch (bitsPerSample) {
		case 8: multiplier = (1.0f / 128); break;
		case 16: multiplier = (1.0f / 32768); break;
		case 24: multiplier = (1.0f / 8388608); break;
		case 32: multiplier = (1.0f / 32768 / 65536); break;
		default: multiplier = 0.0;
	}

	for (i = 0; i < 2; ++i) {
		input = samples [i];
		output = my compressedFloats [i];
		if (! output ) continue;
		for (j = 0; j < numberOfSamples; ++j)
			output [j] = (long)input [j] * multiplier;
		my compressedFloats [i] += numberOfSamples;
	}
}

static void _LongSound_FLAC_convertShorts (LongSound me, const FLAC__int32 * const samples[], long bitsPerSample, long numberOfSamples) {
	for (long channel = 0; channel < my numberOfChannels; ++ channel) {
		short *output = my compressedShorts + channel;
		const FLAC__int32 *input = samples [channel];
		for (long j = 0; j < numberOfSamples; ++ j, output += my numberOfChannels) {
			FLAC__int32 sample = * (input ++);
			switch (bitsPerSample) {
				case 8: sample *= 256; break;
				case 16: break;
				case 24: sample /= 256; break;
				case 32: sample /= 65536; break;
				default: sample = 0; break;
			}
			*output = (short) sample;
		}
	}
	my compressedShorts += numberOfSamples * my numberOfChannels;
}

static FLAC__StreamDecoderWriteStatus _LongSound_FLAC_write (const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], I) {
	iam (LongSound);
	const FLAC__FrameHeader *header = & frame -> header;
	long numberOfSamples = header -> blocksize;
	long bitsPerSample = header -> bits_per_sample;
	(void) decoder;
	if (numberOfSamples > my compressedSamplesLeft)
		numberOfSamples = my compressedSamplesLeft;
	if (numberOfSamples == 0)
		return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
	if (my compressedMode == COMPRESSED_MODE_READ_FLOAT)
		_LongSound_FLAC_convertFloats (me, buffer, bitsPerSample, numberOfSamples);
	else
		_LongSound_FLAC_convertShorts (me, buffer, bitsPerSample, numberOfSamples);
	my compressedSamplesLeft -= numberOfSamples;
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void _LongSound_FLAC_error (const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, I) {
	iam (LongSound);
	(void) me;
	(void) decoder;
	(void) status;
}

static void _LongSound_MP3_convertFloats (LongSound me, const MP3F_SAMPLE *channels[MP3F_MAX_CHANNELS], long numberOfSamples) {
	long i, j;
	const MP3F_SAMPLE *input;
	float *output;
	for (i = 0; i < 2; ++i) {
		input = channels [i];
		output = my compressedFloats [i];
		if (! output ) continue;
		for (j = 0; j < numberOfSamples; ++j)
			output [j] = mp3f_sample_to_float (input [j]);
		my compressedFloats [i] += numberOfSamples;
	}
}

static void _LongSound_MP3_convertShorts (LongSound me, const MP3F_SAMPLE *channels[MP3F_MAX_CHANNELS], long numberOfSamples) {
	long i, j;
	const MP3F_SAMPLE *input;
	short *output;
	for (i = 0; i < my numberOfChannels; ++ i) {
		output = my compressedShorts + i;
		input = channels [i];
		for (j = 0; j < numberOfSamples; ++j, output += my numberOfChannels) {
			int sample = *input++;
			*output = mp3f_sample_to_short (sample);
		}
	}
	my compressedShorts += numberOfSamples * my numberOfChannels;
}

static void _LongSound_MP3_convert (const MP3F_SAMPLE *channels[MP3F_MAX_CHANNELS], long numberOfSamples, I) {
	iam (LongSound);
	if (numberOfSamples > my compressedSamplesLeft)
		numberOfSamples = my compressedSamplesLeft;
	if (numberOfSamples == 0)
		return;
	if (my compressedMode == COMPRESSED_MODE_READ_FLOAT)
		_LongSound_MP3_convertFloats (me, channels, numberOfSamples);
	else
		_LongSound_MP3_convertShorts (me, channels, numberOfSamples);
	my compressedSamplesLeft -= numberOfSamples;
}

static int LongSound_init (LongSound me, MelderFile file) {
	MelderFile_copy (file, & my file);
	MelderFile_open (file); cherror
	my f = file -> filePointer;
	my audioFileType = MelderFile_checkSoundFile (file, & my numberOfChannels, & my encoding, & my sampleRate, & my startOfData, & my nx); cherror
	if (my audioFileType == 0)
		return Melder_error ("File not recognized (LongSound only supports AIFF, AIFC, WAV, NeXT/Sun, NIST and FLAC).");
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
	my flacDecoder = NULL;
	if (my audioFileType == Melder_FLAC) {
		my flacDecoder = FLAC__stream_decoder_new ();
		FLAC__stream_decoder_init_FILE (my flacDecoder,
				my f,
				_LongSound_FLAC_write,
				NULL,
				_LongSound_FLAC_error,
				me);
	}
	my mp3f = NULL;
	if (my audioFileType == Melder_MP3) {
		my mp3f = mp3f_new ();
		mp3f_set_file (my mp3f, my f);
		mp3f_set_callback (my mp3f, _LongSound_MP3_convert, me);
		if (! mp3f_analyze (my mp3f))
			return Melder_error ("Unable to analyze MP3 file.");
	}
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
	us -> writeText = classData -> writeText;
	us -> writeBinary = classData -> writeBinary;
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
class_methods_end

LongSound LongSound_open (MelderFile fs) {
	LongSound me = new (LongSound);
	if (! me || ! LongSound_init (me, fs)) { forget (me); return Melder_errorp ("LongSound not created."); }
	return me;
}

static int _LongSound_FLAC_process (LongSound me, long firstSample, long numberOfSamples) {
	my compressedSamplesLeft = numberOfSamples - 1;
	if (! FLAC__stream_decoder_seek_absolute (my flacDecoder, firstSample))
		return Melder_error ("Cannot seek in FLAC file %s.", MelderFile_messageName (& my file));
	while (my compressedSamplesLeft > 0) {
		if (FLAC__stream_decoder_get_state (my flacDecoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			return Melder_error ("FLAC file %s too short.", MelderFile_messageName (& my file ));
		if (! FLAC__stream_decoder_process_single (my flacDecoder))
			return Melder_error ("Error decoding FLAC file %s.", MelderFile_messageName (& my file ));
	}
	return 1;
}

static int _LongSound_FILE_seekSample (LongSound me, long firstSample) {
	if (fseek (my f, my startOfData + (firstSample - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		return Melder_error3 (L"Cannot seek in file \"", MelderFile_messageNameW (& my file), L"\".");
	return 1;
}

static int _LongSound_FLAC_readAudioToFloat (LongSound me, float *leftBuffer, float *rightBuffer, long firstSample, long numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_FLOAT;
	my compressedFloats [0] = leftBuffer ? leftBuffer + 1 : NULL;
	my compressedFloats [1] = rightBuffer ? rightBuffer + 1 : NULL;
	return _LongSound_FLAC_process (me, firstSample, numberOfSamples);
}

static int _LongSound_FLAC_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_SHORT;
	my compressedShorts = buffer + 1;
	return _LongSound_FLAC_process (me, firstSample, numberOfSamples);
}

static int _LongSound_MP3_process (LongSound me, long firstSample, long numberOfSamples) {
	if (! mp3f_seek (my mp3f, firstSample))
		return Melder_error3 (L"Cannot seek in MP3 file \"", MelderFile_messageNameW (& my file), L"\".");
	my compressedSamplesLeft = numberOfSamples;
	if (! mp3f_read (my mp3f, numberOfSamples))
		return Melder_error3 (L"Error decoding MP3 file \"", MelderFile_messageNameW (& my file), L"\".");
	return 1;
}

static int _LongSound_MP3_readAudioToFloat (LongSound me, float *leftBuffer, float *rightBuffer, long firstSample, long numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_FLOAT;
	my compressedFloats [0] = leftBuffer ? leftBuffer + 1 : NULL;
	my compressedFloats [1] = rightBuffer ? rightBuffer + 1 : NULL;
	return _LongSound_MP3_process (me, firstSample, numberOfSamples - 1);
}

static int _LongSound_MP3_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_SHORT;
	my compressedShorts = buffer + 1;
	return _LongSound_MP3_process (me, firstSample, numberOfSamples - 1);
}

int LongSound_readAudioToFloat (LongSound me, float *leftBuffer, float *rightBuffer, long firstSample, long numberOfSamples) {
	if (my encoding == Melder_FLAC_COMPRESSION)
		return _LongSound_FLAC_readAudioToFloat (me, leftBuffer, rightBuffer, firstSample, numberOfSamples);
	if (my encoding == Melder_MPEG_COMPRESSION)
		return _LongSound_MP3_readAudioToFloat (me, leftBuffer, rightBuffer, firstSample, numberOfSamples);
	return _LongSound_FILE_seekSample (me, firstSample) &&
		Melder_readAudioToFloat (my f, rightBuffer == NULL ? 1 : 2, my encoding, leftBuffer, rightBuffer, numberOfSamples);
}

int LongSound_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples) {
	if (my encoding == Melder_FLAC_COMPRESSION)
		return _LongSound_FLAC_readAudioToShort (me, buffer, firstSample, numberOfSamples);
	if (my encoding == Melder_MPEG_COMPRESSION)
		return _LongSound_MP3_readAudioToShort (me, buffer, firstSample, numberOfSamples);
	return _LongSound_FILE_seekSample (me, firstSample) &&
		Melder_readAudioToShort (my f, my numberOfChannels, my encoding, buffer, numberOfSamples);
}

Sound LongSound_extractPart (LongSound me, double tmin, double tmax, int preserveTimes) {
	Sound thee = NULL;
	long imin, imax, n;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (tmin < my xmin) tmin = my xmin;
	if (tmax > my xmax) tmax = my xmax;
	n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 1) { Melder_error ("Less than 1 sample in window."); goto end; }
	thee = Sound_create (my numberOfChannels, tmin, tmax, n, my dx, my x1 + (imin - 1) * my dx); cherror
	if (! preserveTimes) thy xmin = 0.0, thy xmax -= tmin, thy x1 -= tmin;
	LongSound_readAudioToFloat (me, thy z [1], thy ny == 1 ? NULL : thy z [2], imin, n); cherror
end:
	iferror { forget (thee); Melder_error ("Sound not extracted from LongSound."); }
	return thee;
}

static int _LongSound_readSamples (LongSound me, short *buffer, long imin, long imax) {
	return LongSound_readAudioToShort (me, buffer, imin, imax - imin + 1);
}

static void writePartToOpenFile16 (LongSound me, int audioFileType, long imin, long n, MelderFile file, int numberOfChannels_override) {
	long ibuffer, offset, numberOfBuffers, numberOfSamplesInLastBuffer;
	offset = imin;
	numberOfBuffers = (n - 1) / my nmax + 1;
	numberOfSamplesInLastBuffer = (n - 1) % my nmax + 1;
	if (file -> filePointer) for (ibuffer = 1; ibuffer <= numberOfBuffers; ibuffer ++) {
		long numberOfSamplesToCopy = ibuffer < numberOfBuffers ? my nmax : numberOfSamplesInLastBuffer;
		LongSound_readAudioToShort (me, my buffer, offset, numberOfSamplesToCopy);
		offset += numberOfSamplesToCopy;
		MelderFile_writeShortToAudio (file, numberOfChannels_override ? numberOfChannels_override : my numberOfChannels, Melder_defaultAudioFileEncoding16 (audioFileType), my buffer, numberOfSamplesToCopy);
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
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer)
		MelderFile_writeAudioFileHeader16 (file, audioFileType, my sampleRate, n, my numberOfChannels);
	writePartToOpenFile16 (me, audioFileType, imin, n, file, 0);
	MelderFile_close (file);
	iferror return Melder_error ("Sound file not written.");
	return 1;
}

int LongSound_writeChannelToAudioFile16 (LongSound me, int audioFileType, int channel, MelderFile file) {
	if (my numberOfChannels != 2)
		return Melder_error ("This audio file is not a stereo file. It does not have a %s channel.", channel == 0 ? "left" : "right");
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer)
		MelderFile_writeAudioFileHeader16 (file, audioFileType, my sampleRate, my nx, 1);
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
			thy resampledBuffer = Melder_calloc (short, (thy zeroPadding + thy numberOfSamples + thy zeroPadding) * my numberOfChannels);
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
		short *resampledBuffer = Melder_calloc (short, (zeroPadding + newN + zeroPadding) * my numberOfChannels);
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
		numberOfChannels = sound -> ny;
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
			numbersOfChannelsMatch = sound -> ny == numberOfChannels;
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
	MelderFile_create (file, Melder_macAudioFileType (audioFileType), L"PpgB", Melder_winAudioFileExtension (audioFileType));
	if (file -> filePointer)
		MelderFile_writeAudioFileHeader16 (file, audioFileType, sampleRate, n, numberOfChannels);
	for (i = 1; i <= my size; i ++) {
		data = my item [i];
		if (data -> methods == (Data_Table) classSound) {
			Sound sound = (Sound) data;
			if (file -> filePointer)
				MelderFile_writeFloatToAudio (file, Melder_defaultAudioFileEncoding16 (audioFileType),
					& sound -> z [1] [1], sound -> nx,
					sound -> ny > 1 ? & sound -> z [2] [1] : NULL, sound -> ny > 1 ? sound -> nx : 0, TRUE);
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
