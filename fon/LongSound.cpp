/* LongSound.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma, 2007 Erez Volk (for FLAC and MP3)
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
 * pb 2007/12/05 prefs
 * pb 2008/01/19 double
 * pb 2010/01/10 MP3 precision warning
 * fb 2010/02/25 corrected a bug that could cause LongSound_playPart to crash with an assertion on error
 * pb 2010/11/07 no longer do an assertion on thy resampledBuffer
 * pb 2010/12/20 support for more than 2 channels
 * pb 2011/06/02 C++
 * pb 2011/07/05 C++
 */

#include "LongSound.h"
#include "Preferences.h"
#include "flac_FLAC_stream_decoder.h"
#include "mp3.h"

Thing_implement (LongSound, Sampled, 0);

#define MARGIN  0.01
#define USE_MEMMOVE  1

static long prefs_bufferLength;

void LongSound_preferences (void) {
	Preferences_addLong (L"LongSound.bufferLength", & prefs_bufferLength, 60);   // seconds
}

long LongSound_getBufferSizePref_seconds (void) { return prefs_bufferLength; }
void LongSound_setBufferSizePref_seconds (long size) { prefs_bufferLength = size < 10 ? 10 : size > 10000 ? 10000: size; }

void structLongSound :: v_destroy () {
	/*
	 * The play callback may contain a pointer to my buffer.
	 * That pointer is about to dangle, so kill the playback.
	 */
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	if (mp3f)
		mp3f_delete (mp3f);
	if (flacDecoder) {
		FLAC__stream_decoder_finish (flacDecoder);   // closes f
		FLAC__stream_decoder_delete (flacDecoder);
	}
	else if (f) fclose (f);
	NUMvector_free <short> (buffer, 0);
	LongSound_Parent :: v_destroy ();
}

void structLongSound :: v_info () {
	static const wchar_t *encodingStrings [1+20] = { L"none",
		L"linear 8 bit signed", L"linear 8 bit unsigned",
		L"linear 16 bit big-endian", L"linear 16 bit little-endian",
		L"linear 24 bit big-endian", L"linear 24 bit little-endian",
		L"linear 32 bit big-endian", L"linear 32 bit little-endian",
		L"mu-law", L"A-law", L"shorten", L"polyphone",
		L"IEEE float 32 bit big-endian", L"IEEE float 32 bit little-endian",
		L"FLAC", L"FLAC", L"FLAC", L"MP3", L"MP3", L"MP3" };
	structData :: v_info ();
	MelderInfo_writeLine (L"Duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"File name: ", Melder_fileToPath (& file));
	MelderInfo_writeLine (L"File type: ", audioFileType > Melder_NUMBER_OF_AUDIO_FILE_TYPES ? L"unknown" : Melder_audioFileTypeString (audioFileType));
	MelderInfo_writeLine (L"Number of channels: ", Melder_integer (numberOfChannels));
	MelderInfo_writeLine (L"Encoding: ", encoding > 20 ? L"unknown" : encodingStrings [encoding]);
	MelderInfo_writeLine (L"Sampling frequency: ", Melder_double (sampleRate), L" Hz");
	MelderInfo_writeLine (L"Size: ", Melder_integer (nx), L" samples");
	MelderInfo_writeLine (L"Start of sample data: ", Melder_integer (startOfData), L" bytes from the start of the file");
}

static void _LongSound_FLAC_convertFloats (LongSound me, const FLAC__int32 * const samples[], long bitsPerSample, long numberOfSamples) {
	double multiplier;
	switch (bitsPerSample) {
		case 8: multiplier = (1.0f / 128); break;
		case 16: multiplier = (1.0f / 32768); break;
		case 24: multiplier = (1.0f / 8388608); break;
		case 32: multiplier = (1.0f / 32768 / 65536); break;
		default: multiplier = 0.0;
	}
	for (long i = 0; i < 2; ++i) {
		const FLAC__int32 *input = samples [i];
		double *output = my compressedFloats [i];
		if (! output ) continue;
		for (long j = 0; j < numberOfSamples; ++j)
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
	for (long i = 0; i < 2; ++i) {
		const MP3F_SAMPLE *input = channels [i];
		double *output = my compressedFloats [i];
		if (! output ) continue;
		for (long j = 0; j < numberOfSamples; ++j)
			output [j] = mp3f_sample_to_float (input [j]);
		my compressedFloats [i] += numberOfSamples;
	}
}

static void _LongSound_MP3_convertShorts (LongSound me, const MP3F_SAMPLE *channels[MP3F_MAX_CHANNELS], long numberOfSamples) {
	for (long i = 0; i < my numberOfChannels; ++ i) {
		const MP3F_SAMPLE *input = channels [i];
		short *output = my compressedShorts + i;
		for (long j = 0; j < numberOfSamples; ++j, output += my numberOfChannels) {
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

static void LongSound_init (LongSound me, MelderFile file) {
	MelderFile_copy (file, & my file);
	MelderFile_open (file);   // BUG: should be auto, but that requires an implemented .transfer()
	my f = file -> filePointer;
	my audioFileType = MelderFile_checkSoundFile (file, & my numberOfChannels, & my encoding, & my sampleRate, & my startOfData, & my nx);
	if (my audioFileType == 0)
		Melder_throw ("File not recognized (LongSound only supports AIFF, AIFC, WAV, NeXT/Sun, NIST and FLAC).");
	if (my encoding == Melder_SHORTEN || my encoding == Melder_POLYPHONE)
		Melder_throw ("LongSound does not support sound files compressed with \"shorten\".");
	my xmin = 0.0;
	my dx = 1 / my sampleRate;
	my xmax = my nx * my dx;
	my x1 = 0.5 * my dx;
	my numberOfBytesPerSamplePoint = Melder_bytesPerSamplePoint (my encoding);
	my bufferLength = prefs_bufferLength;
	for (;;) {
		my nmax = my bufferLength * my numberOfChannels * my sampleRate * (1 + 3 * MARGIN);
		try {
			my buffer = NUMvector <short> (0, my nmax * my numberOfChannels);
			break;
		} catch (MelderError) {
			my bufferLength *= 0.5;   // try 30, 15, or 7.5 seconds
			if (my bufferLength < 5.0)   // too short to be good
				throw;
			Melder_clearError ();   // delete out-of-memory message
		}
	}
	my imin = 1;
	my imax = 0;
	my flacDecoder = NULL;
	if (my audioFileType == Melder_FLAC) {
		my flacDecoder = FLAC__stream_decoder_new ();
		FLAC__stream_decoder_init_FILE (my flacDecoder, my f, _LongSound_FLAC_write, NULL, _LongSound_FLAC_error, me);
	}
	my mp3f = NULL;
	if (my audioFileType == Melder_MP3) {
		my mp3f = mp3f_new ();
		mp3f_set_file (my mp3f, my f);
		mp3f_set_callback (my mp3f, _LongSound_MP3_convert, me);
		if (! mp3f_analyze (my mp3f))
			Melder_throw ("Unable to analyze MP3 file.");
		Melder_warning (L"Time measurements in MP3 files can be off by several tens of milliseconds. "
			"Please convert to WAV file if you need time precision or annotation.");
	}
}

void structLongSound :: v_copy (thou) {
	thouart (LongSound);
	thy f = NULL;
	thy buffer = NULL;
	LongSound_init (thee, & file);
}

LongSound LongSound_open (MelderFile file) {
	try {
		autoLongSound me = Thing_new (LongSound);
		LongSound_init (me.peek(), file);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("LongSound not created.");
	}
}

static void _LongSound_FLAC_process (LongSound me, long firstSample, long numberOfSamples) {
	my compressedSamplesLeft = numberOfSamples - 1;
	if (! FLAC__stream_decoder_seek_absolute (my flacDecoder, firstSample))
		Melder_throw ("Cannot seek in FLAC file ", & my file, ".");
	while (my compressedSamplesLeft > 0) {
		if (FLAC__stream_decoder_get_state (my flacDecoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			Melder_throw ("FLAC file ", & my file, " too short.");
		if (! FLAC__stream_decoder_process_single (my flacDecoder))
			Melder_throw ("Error decoding FLAC file ", & my file, ".");
	}
}

static void _LongSound_FILE_seekSample (LongSound me, long firstSample) {
	if (fseek (my f, my startOfData + (firstSample - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		Melder_throw ("Cannot seek in file ", & my file, ".");
}

static void _LongSound_FLAC_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_SHORT;
	my compressedShorts = buffer + 1;
	_LongSound_FLAC_process (me, firstSample, numberOfSamples);
}

static void _LongSound_MP3_process (LongSound me, long firstSample, long numberOfSamples) {
	if (! mp3f_seek (my mp3f, firstSample))
		Melder_throw ("Cannot seek in MP3 file ", & my file, ".");
	my compressedSamplesLeft = numberOfSamples;
	if (! mp3f_read (my mp3f, numberOfSamples))
		Melder_throw ("Error decoding MP3 file ", & my file, ".");
}

static void _LongSound_MP3_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_SHORT;
	my compressedShorts = buffer + 1;
	_LongSound_MP3_process (me, firstSample, numberOfSamples - 1);
}

void LongSound_readAudioToFloat (LongSound me, double **buffer, long firstSample, long numberOfSamples) {
	if (my encoding == Melder_FLAC_COMPRESSION_16) {
		my compressedMode = COMPRESSED_MODE_READ_FLOAT;
		for (int ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			my compressedFloats [ichan - 1] = & buffer [ichan] [1];
		}
		_LongSound_FLAC_process (me, firstSample, numberOfSamples);
	} else if (my encoding == Melder_MPEG_COMPRESSION_16) {
		my compressedMode = COMPRESSED_MODE_READ_FLOAT;
		for (int ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			my compressedFloats [ichan - 1] = & buffer [ichan] [1];
		}
		_LongSound_MP3_process (me, firstSample, numberOfSamples);
	} else {
		_LongSound_FILE_seekSample (me, firstSample);
		Melder_readAudioToFloat (my f, my numberOfChannels, my encoding, buffer, numberOfSamples);
	}
}

void LongSound_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples) {
	if (my encoding == Melder_FLAC_COMPRESSION_16) {
		_LongSound_FLAC_readAudioToShort (me, buffer, firstSample, numberOfSamples);
	} else if (my encoding == Melder_MPEG_COMPRESSION_16) {
		_LongSound_MP3_readAudioToShort (me, buffer, firstSample, numberOfSamples);
	} else {
		_LongSound_FILE_seekSample (me, firstSample);
		Melder_readAudioToShort (my f, my numberOfChannels, my encoding, buffer, numberOfSamples);
	}
}

Sound LongSound_extractPart (LongSound me, double tmin, double tmax, int preserveTimes) {
	try {
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
		if (tmin < my xmin) tmin = my xmin;
		if (tmax > my xmax) tmax = my xmax;
		long imin, imax;
		long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
		if (n < 1) Melder_throw ("Less than 1 sample in window.");
		autoSound thee = Sound_create (my numberOfChannels, tmin, tmax, n, my dx, my x1 + (imin - 1) * my dx);
		if (! preserveTimes) thy xmin = 0.0, thy xmax -= tmin, thy x1 -= tmin;
		LongSound_readAudioToFloat (me, thy z, imin, n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Sound not extracted.");
	}
}

static void _LongSound_readSamples (LongSound me, short *buffer, long imin, long imax) {
	LongSound_readAudioToShort (me, buffer, imin, imax - imin + 1);
}

static void writePartToOpenFile (LongSound me, int audioFileType, long imin, long n, MelderFile file, int numberOfChannels_override, int numberOfBitsPerSamplePoint) {
	long ibuffer, offset, numberOfBuffers, numberOfSamplesInLastBuffer;
	offset = imin;
	numberOfBuffers = (n - 1) / my nmax + 1;
	numberOfSamplesInLastBuffer = (n - 1) % my nmax + 1;
	if (file -> filePointer) for (ibuffer = 1; ibuffer <= numberOfBuffers; ibuffer ++) {
		long numberOfSamplesToCopy = ibuffer < numberOfBuffers ? my nmax : numberOfSamplesInLastBuffer;
		LongSound_readAudioToShort (me, my buffer, offset, numberOfSamplesToCopy);
		offset += numberOfSamplesToCopy;
		MelderFile_writeShortToAudio (file, numberOfChannels_override ? numberOfChannels_override : my numberOfChannels, Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint), my buffer, numberOfSamplesToCopy);
	}
	/*
	 * We "have" no samples any longer.
	 */
	my imin = 1;
	my imax = 0;
}

void LongSound_writePartToAudioFile (LongSound me, int audioFileType, double tmin, double tmax, MelderFile file) {
	try {
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
		if (tmin < my xmin) tmin = my xmin;
		if (tmax > my xmax) tmax = my xmax;
		long imin, imax;
		long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
		if (n < 1) Melder_throw (L"Less than 1 sample selected.");
		autoMelderFile mfile = MelderFile_create (file);
		MelderFile_writeAudioFileHeader (file, audioFileType, my sampleRate, n, my numberOfChannels, 8 * my numberOfBytesPerSamplePoint);
		writePartToOpenFile (me, audioFileType, imin, n, file, 0, 8 * my numberOfBytesPerSamplePoint);
		MelderFile_writeAudioFileTrailer (file, audioFileType, my sampleRate, n, my numberOfChannels, 8 * my numberOfBytesPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, ": not written to sound file ", file, ".");
	}
}

void LongSound_writeChannelToAudioFile (LongSound me, int audioFileType, int channel, MelderFile file) {
	try {
		if (my numberOfChannels != 2)
			Melder_throw ("This audio file is not a stereo file. It does not have a ", channel == 0 ? L"left" : L"right", L" channel.");
		autoMelderFile mfile = MelderFile_create (file);
		if (file -> filePointer) {
			MelderFile_writeAudioFileHeader (file, audioFileType, my sampleRate, my nx, 1, 8 * my numberOfBytesPerSamplePoint);
		}
		writePartToOpenFile (me, audioFileType, 1, my nx, file, channel == 0 ? -1 : -2, 8 * my numberOfBytesPerSamplePoint);
		MelderFile_writeAudioFileTrailer (file, audioFileType, my sampleRate, my nx, 1, 8 * my numberOfBytesPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw ("Channel ", channel, " of ", me, ": not written to sound file ", file, ".");
	}
}

static void _LongSound_haveSamples (LongSound me, long imin, long imax) {
	long n = imax - imin + 1;
	Melder_assert (n <= my nmax);
	/*
	 * Included?
	 */
	if (imin >= my imin && imax <= my imax) return;
	/*
	 * Extendable?
	 */
	if (imin >= my imin && imax - my imin + 1 <= my nmax) {
		_LongSound_readSamples (me, my buffer + (my imax - my imin + 1) * my numberOfChannels, my imax + 1, imax);
		my imax = imax;
		return;
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
		_LongSound_readSamples (me, my buffer, imin, imax);
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
			_LongSound_readSamples (me, my buffer, imin, my imin - 1);
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
			_LongSound_readSamples (me, my buffer, imin, my imin - 1);
			_LongSound_readSamples (me, my buffer + (my imax - imin + 1) * my numberOfChannels, my imax + 1, imax);
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
		_LongSound_readSamples (me, my buffer + (my imax - imin + 1) * my numberOfChannels, my imax + 1, imax);
	}
	my imin = imin, my imax = imax;
}

bool LongSound_haveWindow (LongSound me, double tmin, double tmax) {
	long imin, imax;
	long n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if ((1.0 + 2 * MARGIN) * n + 1 > my nmax) return false;
	_LongSound_haveSamples (me, imin, imax);
	return true;
}

void LongSound_getWindowExtrema (LongSound me, double tmin, double tmax, int channel, double *minimum, double *maximum) {
	long imin, imax;
	long i, minimum_int = 32767, maximum_int = -32768;
	(void) Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	*minimum = 1.0;
	*maximum = -1.0;
	try {
		LongSound_haveWindow (me, tmin, tmax);
	} catch (MelderError) {
		Melder_clearError ();
		return;
	}
	for (i = imin; i <= imax; i ++) {
		long value = my buffer [(i - my imin) * my numberOfChannels + channel - 1];
		if (value < minimum_int) minimum_int = value;
		if (value > maximum_int) maximum_int = value;
	}
	*minimum = minimum_int / 32768.0;
	*maximum = maximum_int / 32768.0;
}

static struct LongSoundPlay {
	long numberOfSamples, i1, i2, silenceBefore, silenceAfter;
	double tmin, tmax, dt, t1;
	short *resampledBuffer;
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t);
	void *closure;
} thePlayingLongSound;

static bool melderPlayCallback (void *closure, long samplesPlayed) {
	struct LongSoundPlay *me = (struct LongSoundPlay *) closure;
	int phase = 2;
	double t = samplesPlayed <= my silenceBefore ? my tmin :
		samplesPlayed >= my silenceBefore + my numberOfSamples ? my tmax :
		my t1 + (my i1 - 1.5 + samplesPlayed - my silenceBefore) * my dt;
	if (! MelderAudio_isPlaying) {
		phase = 3;
		Melder_free (my resampledBuffer);
	}
	if (my callback)
		return my callback (my closure, phase, my tmin, my tmax, t);
	return true;
}

void LongSound_playPart (LongSound me, double tmin, double tmax,
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure)
{
	struct LongSoundPlay *thee = (struct LongSoundPlay *) & thePlayingLongSound;
	Melder_free (thy resampledBuffer);   // just in case
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	try {
		int fits = LongSound_haveWindow (me, tmin, tmax);
		long bestSampleRate = MelderAudio_getOutputBestSampleRate (my sampleRate), n, i1, i2;
		if (! fits)
			Melder_throw ("Sound too long (", tmax - tmin, " seconds).");
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
			thy silenceBefore = (long) (my sampleRate * MelderAudio_getOutputSilenceBefore ());
			thy silenceAfter = (long) (my sampleRate * MelderAudio_getOutputSilenceAfter ());
			if (thy callback) thy callback (thy closure, 1, tmin, tmax, tmin);
			if (thy silenceBefore > 0 || thy silenceAfter > 0) {
				thy resampledBuffer = Melder_calloc (short, (thy silenceBefore + thy numberOfSamples + thy silenceAfter) * my numberOfChannels);
				memcpy (& thy resampledBuffer [thy silenceBefore * my numberOfChannels], & my buffer [(i1 - my imin) * my numberOfChannels],
					thy numberOfSamples * sizeof (short) * my numberOfChannels);
				MelderAudio_play16 (thy resampledBuffer, my sampleRate, thy silenceBefore + thy numberOfSamples + thy silenceAfter,
					my numberOfChannels, melderPlayCallback, thee);
			} else {
				MelderAudio_play16 (my buffer + (i1 - my imin) * my numberOfChannels, my sampleRate,
				   thy numberOfSamples, my numberOfChannels, melderPlayCallback, thee);
			}
		} else {
			long newSampleRate = bestSampleRate;
			long newN = ((double) n * newSampleRate) / my sampleRate - 1, i;
			long silenceBefore = (long) (newSampleRate * MelderAudio_getOutputSilenceBefore ());
			long silenceAfter = (long) (newSampleRate * MelderAudio_getOutputSilenceAfter ());
			short *resampledBuffer = Melder_calloc (short, (silenceBefore + newN + silenceAfter) * my numberOfChannels);
			short *from = my buffer + (i1 - my imin) * my numberOfChannels;   /* Guaranteed: from [0 .. (my imax - my imin + 1) * nchan] */
			double t1 = my x1, dt = 1.0 / newSampleRate;
			thy numberOfSamples = newN;
			thy dt = dt;
			thy t1 = t1 + i1 / my sampleRate;
			thy i1 = 0;
			thy i2 = newN - 1;
			thy silenceBefore = silenceBefore;
			thy silenceAfter = silenceAfter;
			thy resampledBuffer = resampledBuffer;
			if (my numberOfChannels == 1) {
				for (i = 0; i < newN; i ++) {
					double t = t1 + i * dt;   /* From t1 to t1 + (newN-1) * dt */
					double index = (t - t1) * my sampleRate;   /* From 0. */
					long flore = index;   /* DANGEROUS: Implicitly rounding down... */
					double fraction = index - flore;
					resampledBuffer [i + silenceBefore] = (1 - fraction) * from [flore] + fraction * from [flore + 1];
				}
			} else {
				for (i = 0; i < newN; i ++) {
					double t = t1 + i * dt;
					double index = (t - t1) * newSampleRate;
					long flore = index, ii = i + silenceBefore;
					double fraction = index - flore;
					resampledBuffer [ii + ii] = (1 - fraction) * from [flore + flore] + fraction * from [flore + flore + 2];
					resampledBuffer [ii + ii + 1] = (1 - fraction) * from [flore + flore + 1] + fraction * from [flore + flore + 3];
				}
			}
			if (thy callback) thy callback (thy closure, 1, tmin, tmax, tmin);
			MelderAudio_play16 (resampledBuffer, newSampleRate, silenceBefore + newN + silenceAfter, my numberOfChannels, melderPlayCallback, thee);
		}
		//Melder_free (thy resampledBuffer);   // cannot do that, because MelderAudio_play16 isn't necessarily synchronous
	} catch (MelderError) {
		Melder_free (thy resampledBuffer);
		Melder_throw (me, ": not played.");
	}
}

void LongSound_concatenate (Collection me, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint) {
	try {
		long sampleRate, n;   /* Integer sampling frequencies only, because of possible rounding errors. */
		int numberOfChannels;
		if (my size < 1) Melder_throw ("No Sound or LongSound objects to concatenate.");
		/*
		 * The sampling frequencies and numbers of channels must be equal for all (long)sounds.
		 */
		Sampled data = (Sampled) my item [1];
		if (data -> classInfo == classSound) {
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
		for (long i = 2; i <= my size; i ++) {
			int sampleRatesMatch, numbersOfChannelsMatch;
			data = (Sampled) my item [i];
			if (data -> classInfo == classSound) {
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
				Melder_throw ("Sampling frequencies are not equal.");
			if (! numbersOfChannelsMatch)
				Melder_throw ("Cannot mix stereo and mono.");
		}
		/*
		 * Create output file and write header.
		 */
		autoMelderFile mfile = MelderFile_create (file);
		if (file -> filePointer) {
			MelderFile_writeAudioFileHeader (file, audioFileType, sampleRate, n, numberOfChannels, numberOfBitsPerSamplePoint);
		}
		for (long i = 1; i <= my size; i ++) {
			data = (Sampled) my item [i];
			if (data -> classInfo == classSound) {
				Sound sound = (Sound) data;
				if (file -> filePointer) {
					MelderFile_writeFloatToAudio (file, sound -> ny, Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint),
						sound -> z, sound -> nx, TRUE);
				}
			} else {
				LongSound longSound = (LongSound) data;
				writePartToOpenFile (longSound, audioFileType, 1, longSound -> nx, file, 0, numberOfBitsPerSamplePoint);
			}
		}
		MelderFile_writeAudioFileTrailer (file, audioFileType, sampleRate, n, numberOfChannels, numberOfBitsPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw ("Sounds not concatenated and not saved to ", file, ".");
	}
}

/* End of file LongSound.cpp */
