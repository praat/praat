/* LongSound.cpp
 *
 * Copyright (C) 1992-2008,2010-2019 Paul Boersma, 2007 Erez Volk (for FLAC and MP3)
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
 * pb 2014/06/16 more support for more than 2 channels
 */

#include "LongSound.h"
#include "Preferences.h"
#include "flac_FLAC_stream_decoder.h"
#include "mp3.h"

Thing_implement (LongSound, Sampled, 0);
Thing_implement (SoundAndLongSoundList, Ordered, 0);

#define MARGIN  0.01
#define USE_MEMMOVE  1

constexpr integer minimumBufferDuration = 10;   // seconds
constexpr integer defaultBufferDuration = 60;   // seconds
constexpr integer maximumBufferDuration = 10000;   // seconds

static integer prefs_bufferLength;

void LongSound_preferences () {
	Preferences_addInteger (U"LongSound.bufferLength", & prefs_bufferLength, defaultBufferDuration);
}

integer LongSound_getBufferSizePref_seconds () {
	return prefs_bufferLength;
}

void LongSound_setBufferSizePref_seconds (integer size) {
	prefs_bufferLength = Melder_clipped (minimumBufferDuration, size, maximumBufferDuration);
}

void structLongSound :: v_destroy () noexcept {
	/*
		The play callback may contain a pointer to my buffer.
		That pointer is about to dangle, so kill the playback.
	*/
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	if (mp3f)
		mp3f_delete (mp3f);
	if (flacDecoder) {
		FLAC__stream_decoder_finish (flacDecoder);   // closes f
		FLAC__stream_decoder_delete (flacDecoder);
	} else if (f) {
		fclose (f);
	}
	LongSound_Parent :: v_destroy ();
}

void structLongSound :: v_info () {
	static const conststring32 encodingStrings [1+22] = { U"none",
		U"linear 8 bit signed", U"linear 8 bit unsigned",
		U"linear 16 bit big-endian", U"linear 16 bit little-endian",
		U"linear 24 bit big-endian", U"linear 24 bit little-endian",
		U"linear 32 bit big-endian", U"linear 32 bit little-endian",
		U"mu-law", U"A-law", U"shorten", U"polyphone",
		U"IEEE float 32 bit big-endian", U"IEEE float 32 bit little-endian",
		U"IEEE float 64 bit big-endian", U"IEEE float 64 bit little-endian",
		U"FLAC", U"FLAC", U"FLAC", U"MP3", U"MP3", U"MP3" };
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"File name: ", Melder_fileToPath (& file));
	MelderInfo_writeLine (U"File type: ", audioFileType > Melder_NUMBER_OF_AUDIO_FILE_TYPES ? U"unknown" : Melder_audioFileTypeString (audioFileType));
	MelderInfo_writeLine (U"Number of channels: ", numberOfChannels);
	MelderInfo_writeLine (U"Encoding: ", encoding > 20 ? U"unknown" : encodingStrings [encoding]);
	MelderInfo_writeLine (U"Sampling frequency: ", sampleRate, U" Hz");
	MelderInfo_writeLine (U"Size: ", nx, U" samples");
	MelderInfo_writeLine (U"Start of sample data: ", startOfData, U" bytes from the start of the file");
}

static void _LongSound_FLAC_convertFloats (LongSound me, const int32 * const samples[], integer bitsPerSample, integer numberOfSamples) {
	double multiplier;
	switch (bitsPerSample) {
		case 8: multiplier = (1.0 / 128.0); break;
		case 16: multiplier = (1.0 / 32768.0); break;
		case 24: multiplier = (1.0 / 8388608.0); break;
		case 32: multiplier = (1.0 / 32768.0 / 65536.0); break;
		default: multiplier = 0.0;
	}
	for (integer i = 0; i < 2; ++i) {
		const int32 *input = samples [i];
		double *output = my compressedFloats [i];
		if (! output ) continue;
		for (integer j = 0; j < numberOfSamples; ++j)
			output [j] = (double) input [j] * multiplier;
		my compressedFloats [i] += numberOfSamples;
	}
}

static void _LongSound_FLAC_convertShorts (LongSound me, const int32 * const samples[], integer bitsPerSample, integer numberOfSamples) {
	for (integer channel = 0; channel < my numberOfChannels; ++ channel) {
		int16 *output = my compressedShorts + channel;
		const int32 *input = samples [channel];
		for (integer j = 0; j < numberOfSamples; ++ j, output += my numberOfChannels) {
			int32 sample = * (input ++);
			switch (bitsPerSample) {
				case 8: sample *= 256; break;
				case 16: break;
				case 24: sample /= 256; break;
				case 32: sample /= 65536; break;
				default: sample = 0; break;
			}
			*output = (int16) sample;
		}
	}
	my compressedShorts += numberOfSamples * my numberOfChannels;
}

static FLAC__StreamDecoderWriteStatus _LongSound_FLAC_write (const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *void_me) {
	iam (LongSound);
	const FLAC__FrameHeader *header = & frame -> header;
	integer numberOfSamples = header -> blocksize;
	integer bitsPerSample = header -> bits_per_sample;
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

static void _LongSound_FLAC_error (const FLAC__StreamDecoder * /* decoder */, FLAC__StreamDecoderErrorStatus /* status */, void * /* longSound */) {
}

static void _LongSound_MP3_convertFloats (LongSound me, const MP3F_SAMPLE *channels [MP3F_MAX_CHANNELS], integer numberOfSamples) {
	for (integer i = 0; i < 2; ++i) {
		const MP3F_SAMPLE *input = channels [i];
		double *output = my compressedFloats [i];
		if (! output ) continue;
		for (integer j = 0; j < numberOfSamples; ++j)
			output [j] = mp3f_sample_to_float (input [j]);
		my compressedFloats [i] += numberOfSamples;
	}
}

static void _LongSound_MP3_convertShorts (LongSound me, const MP3F_SAMPLE *channels [MP3F_MAX_CHANNELS], integer numberOfSamples) {
	for (integer i = 0; i < my numberOfChannels; ++ i) {
		const MP3F_SAMPLE *input = channels [i];
		int16 *output = my compressedShorts + i;
		for (integer j = 0; j < numberOfSamples; ++j, output += my numberOfChannels) {
			int sample = *input++;
			*output = mp3f_sample_to_short (sample);
		}
	}
	my compressedShorts += numberOfSamples * my numberOfChannels;
}

static void _LongSound_MP3_convert (const MP3F_SAMPLE *channels [MP3F_MAX_CHANNELS], integer numberOfSamples, void *void_me) {
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
		Melder_throw (U"File not recognized (LongSound only supports AIFF, AIFC, WAV, NeXT/Sun, NIST and FLAC).");
	if (my encoding == Melder_SHORTEN || my encoding == Melder_POLYPHONE)
		Melder_throw (U"LongSound does not support sound files compressed with \"shorten\".");
	if (my nx < 1)
		Melder_throw (U"Audio file contains 0 samples.");
	my xmin = 0.0;
	my dx = 1 / my sampleRate;
	my xmax = my nx * my dx;
	my x1 = 0.5 * my dx;
	my numberOfBytesPerSamplePoint = Melder_bytesPerSamplePoint (my encoding);
	my bufferLength = prefs_bufferLength;
	for (;;) {
		my nmax = my bufferLength * my sampleRate * (1 + 3 * MARGIN);
		try {
			my buffer = newvectorzero <int16> (my nmax * my numberOfChannels + 1);
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
	my flacDecoder = nullptr;
	if (my audioFileType == Melder_FLAC) {
		my flacDecoder = FLAC__stream_decoder_new ();
		FLAC__stream_decoder_init_FILE (my flacDecoder, my f, _LongSound_FLAC_write, nullptr, _LongSound_FLAC_error, me);
	}
	my mp3f = nullptr;
	if (my audioFileType == Melder_MP3) {
		my mp3f = mp3f_new ();
		mp3f_set_file (my mp3f, my f);
		mp3f_set_callback (my mp3f, _LongSound_MP3_convert, me);
		if (! mp3f_analyze (my mp3f))
			Melder_throw (U"Unable to analyze MP3 file.");
		Melder_warning (U"Time measurements in MP3 files can be off by several tens of milliseconds. "
			U"Please convert to WAV file if you need time precision or annotation.");
	}
}

void structLongSound :: v_copy (Daata thee_Daata) {
	LongSound thee = static_cast <LongSound> (thee_Daata);
	thy f = nullptr;
	thy buffer.releaseToAmbiguousOwner();   // this may have been shallow-copied, so undangle and nullify
	LongSound_init (thee, & our file);   // this recreates a new buffer
}

autoLongSound LongSound_open (MelderFile file) {
	try {
		autoLongSound me = Thing_new (LongSound);
		LongSound_init (me.get(), file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LongSound not created.");
	}
}

static void _LongSound_FLAC_process (LongSound me, integer firstSample, integer numberOfSamples) {
	my compressedSamplesLeft = numberOfSamples - 1;
	if (! FLAC__stream_decoder_seek_absolute (my flacDecoder, firstSample))
		Melder_throw (U"Cannot seek in FLAC file ", & my file, U".");
	while (my compressedSamplesLeft > 0) {
		if (FLAC__stream_decoder_get_state (my flacDecoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			Melder_throw (U"FLAC file ", & my file, U" too short.");
		if (! FLAC__stream_decoder_process_single (my flacDecoder))
			Melder_throw (U"Error decoding FLAC file ", & my file, U".");
	}
}

static void _LongSound_FILE_seekSample (LongSound me, integer firstSample) {
	if (fseek (my f, my startOfData + (firstSample - 1) * my numberOfChannels * my numberOfBytesPerSamplePoint, SEEK_SET))
		Melder_throw (U"Cannot seek in file ", & my file, U".");
}

static void _LongSound_FLAC_readAudioToShort (LongSound me, int16 *buffer, integer firstSample, integer numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_SHORT;
	my compressedShorts = buffer + 1;
	_LongSound_FLAC_process (me, firstSample, numberOfSamples);
}

static void _LongSound_MP3_process (LongSound me, integer firstSample, integer numberOfSamples) {
	if (! mp3f_seek (my mp3f, firstSample))
		Melder_throw (U"Cannot seek in MP3 file ", & my file, U".");
	my compressedSamplesLeft = numberOfSamples;
	if (! mp3f_read (my mp3f, numberOfSamples))
		Melder_throw (U"Error decoding MP3 file ", & my file, U".");
}

static void _LongSound_MP3_readAudioToShort (LongSound me, int16 *buffer, integer firstSample, integer numberOfSamples) {
	my compressedMode = COMPRESSED_MODE_READ_SHORT;
	my compressedShorts = buffer + 1;
	_LongSound_MP3_process (me, firstSample, numberOfSamples - 1);
}

void LongSound_readAudioToFloat (LongSound me, MAT buffer, integer firstSample) {
	Melder_assert (buffer.nrow == my numberOfChannels);
	if (my encoding == Melder_FLAC_COMPRESSION_16) {
		my compressedMode = COMPRESSED_MODE_READ_FLOAT;
		for (int ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			my compressedFloats [ichan - 1] = & buffer [ichan] [1];
		}
		_LongSound_FLAC_process (me, firstSample, buffer.ncol);
	} else if (my encoding == Melder_MPEG_COMPRESSION_16) {
		my compressedMode = COMPRESSED_MODE_READ_FLOAT;
		for (int ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			my compressedFloats [ichan - 1] = & buffer [ichan] [1];
		}
		_LongSound_MP3_process (me, firstSample, buffer.ncol);
	} else {
		_LongSound_FILE_seekSample (me, firstSample);
		Melder_readAudioToFloat (my f, my encoding, buffer);
	}
}

void LongSound_readAudioToShort (LongSound me, int16 *buffer, integer firstSample, integer numberOfSamples) {
	if (my encoding == Melder_FLAC_COMPRESSION_16) {
		_LongSound_FLAC_readAudioToShort (me, buffer, firstSample, numberOfSamples);
	} else if (my encoding == Melder_MPEG_COMPRESSION_16) {
		_LongSound_MP3_readAudioToShort (me, buffer, firstSample, numberOfSamples);
	} else {
		_LongSound_FILE_seekSample (me, firstSample);
		Melder_readAudioToShort (my f, my numberOfChannels, my encoding, buffer, numberOfSamples);
	}
}

autoSound LongSound_extractPart (LongSound me, double tmin, double tmax, bool preserveTimes) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		if (tmin < my xmin)
			tmin = my xmin;
		if (tmax > my xmax)
			tmax = my xmax;
		integer firstSample, lastSample;
		integer numberOfSamples = Sampled_getWindowSamples (me, tmin, tmax, & firstSample, & lastSample);
		if (numberOfSamples < 1)
			Melder_throw (U"Less than 1 sample in window.");
		autoSound thee = Sound_create (my numberOfChannels,
				tmin, tmax, numberOfSamples, my dx, Sampled_indexToX (me, firstSample));
		if (! preserveTimes) {
			thy xmin = 0.0;
			thy xmax -= tmin;
			thy x1 -= tmin;
		}
		LongSound_readAudioToFloat (me, thy z.get(), firstSample);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Sound not extracted.");
	}
}

static void _LongSound_readSamples (LongSound me, int16 *buffer, integer imin, integer imax) {
	LongSound_readAudioToShort (me, buffer, imin, imax - imin + 1);
}

static void writePartToOpenFile (LongSound me, int audioFileType, integer imin, integer n, MelderFile file, int numberOfChannels_override, int numberOfBitsPerSamplePoint) {
	integer ibuffer, offset, numberOfBuffers, numberOfSamplesInLastBuffer;
	offset = imin;
	numberOfBuffers = (n - 1) / my nmax + 1;
	numberOfSamplesInLastBuffer = (n - 1) % my nmax + 1;
	if (file -> filePointer) for (ibuffer = 1; ibuffer <= numberOfBuffers; ibuffer ++) {
		integer numberOfSamplesToCopy = ( ibuffer < numberOfBuffers ? my nmax : numberOfSamplesInLastBuffer );
		my invalidateBuffer();
		LongSound_readAudioToShort (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), offset, numberOfSamplesToCopy);
		offset += numberOfSamplesToCopy;
		MelderFile_writeShortToAudio (file, numberOfChannels_override ? numberOfChannels_override : my numberOfChannels, Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint), my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), numberOfSamplesToCopy);
	}
}

void LongSound_savePartAsAudioFile (LongSound me, int audioFileType, double tmin, double tmax, MelderFile file, int numberOfBitsPerSamplePoint) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		if (tmin < my xmin)
			tmin = my xmin;
		if (tmax > my xmax)
			tmax = my xmax;
		integer imin, imax;
		integer n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
		if (n < 1)
			Melder_throw (U"Less than 1 sample selected.");
		autoMelderFile mfile = MelderFile_create (file);
		MelderFile_writeAudioFileHeader (file, audioFileType, my sampleRate, n, my numberOfChannels, numberOfBitsPerSamplePoint);
		writePartToOpenFile (me, audioFileType, imin, n, file, 0, numberOfBitsPerSamplePoint);
		MelderFile_writeAudioFileTrailer (file, audioFileType, my sampleRate, n, my numberOfChannels, numberOfBitsPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to sound file ", file, U".");
	}
}

void LongSound_saveChannelAsAudioFile (LongSound me, int audioFileType, integer channel, MelderFile file) {
	try {
		if (my numberOfChannels != 2)
			Melder_throw (U"This audio file is not a stereo file. It does not have a ", channel == 0 ? U"left" : U"right", U" channel.");
		autoMelderFile mfile = MelderFile_create (file);
		if (file -> filePointer)
			MelderFile_writeAudioFileHeader (file, audioFileType, my sampleRate, my nx, 1, 8 * my numberOfBytesPerSamplePoint);
		writePartToOpenFile (me, audioFileType, 1, my nx, file, channel == 0 ? -1 : -2, 8 * my numberOfBytesPerSamplePoint);
		MelderFile_writeAudioFileTrailer (file, audioFileType, my sampleRate, my nx, 1, 8 * my numberOfBytesPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (U"Channel ", channel, U" of ", me, U": not written to sound file ", file, U".");
	}
}

static void _LongSound_haveSamples (LongSound me, integer imin, integer imax) {
	integer n = imax - imin + 1;
	Melder_assert (n <= my nmax);
	/*
		Included?
	*/
	if (imin >= my imin && imax <= my imax)
		return;
	/*
		Extendable?
	*/
	if (imin >= my imin && imax - my imin + 1 <= my nmax) {
		_LongSound_readSamples (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + (my imax - my imin + 1) * my numberOfChannels, my imax + 1, imax);
		my imax = imax;
		return;
	}
	/*
		Determine the loadable imin..imax.
		Add margins on both sides.
	*/
	imin -= MARGIN * n;
	if (imin < 1)
		imin = 1;
	imax = imin + Melder_ifloor ((1.0 + 2.0 * MARGIN) * n);
	if (imax > my nx)
		imax = my nx;
	imin = imax - Melder_ifloor ((1.0 + 2.0 * MARGIN) * n);
	if (imin < 1)
		imin = 1;
	Melder_assert (imax - imin + 1 <= my nmax);
	/*
		Overlap?
	*/
	if (imax < my imin || imin > my imax) {
		/*
			No overlap.
		*/
		_LongSound_readSamples (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), imin, imax);
	} else if (imin < my imin) {
		/*
			Left overlap.
		*/
		if (imax <= my imax) {
			/*
				Only left overlap (e.g. scrolling up).
			*/
			const integer nshift = (imax - my imin + 1) * my numberOfChannels, shift = (my imin - imin) * my numberOfChannels;
			#if USE_MEMMOVE
				memmove (my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + shift, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), nshift * sizeof (int16));
			#else
				for (i = nshift; i >= 1; i --)
					my buffer [i + shift] = my buffer [i];
			#endif
			_LongSound_readSamples (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), imin, my imin - 1);
		} else {
			/*
				Left and right overlap (e.g. zooming out).
			*/
			const integer nshift = (my imax - my imin + 1) * my numberOfChannels, shift = (my imin - imin) * my numberOfChannels;
			#if USE_MEMMOVE
				memmove (my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + shift, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), nshift * sizeof (int16));
			#else
				for (i = nshift; i >= 1; i --)
					my buffer [i + shift] = my buffer [i];
			#endif
			_LongSound_readSamples (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), imin, my imin - 1);
			_LongSound_readSamples (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + (my imax - imin + 1) * my numberOfChannels, my imax + 1, imax);
		}
	} else {
		/*
			Only right overlap (e.g. scrolling down).
		*/
		const integer nshift = (my imax - imin + 1) * my numberOfChannels, shift = (imin - my imin) * my numberOfChannels;
		#if USE_MEMMOVE
			memmove (my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + shift, nshift * sizeof (int16));
		#else
			for (i = 1; i <= nshift; i ++)
				my buffer [i] = my buffer [i + shift];
		#endif
		_LongSound_readSamples (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + (my imax - imin + 1) * my numberOfChannels, my imax + 1, imax);
	}
	my imin = imin;
	my imax = imax;
}

bool LongSound_haveWindow (LongSound me, double tmin, double tmax) {
	integer imin, imax;
	const integer n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if ((1.0 + 2 * MARGIN) * n + 1 > my nmax)
		return false;
	_LongSound_haveSamples (me, imin, imax);
	return true;
}

void LongSound_getWindowExtrema (LongSound me, double tmin, double tmax, integer channel, double *minimum, double *maximum) {
	integer imin, imax;
	(void) Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	*minimum = 1.0;
	*maximum = -1.0;
	try {
		LongSound_haveWindow (me, tmin, tmax);
	} catch (MelderError) {
		Melder_clearError ();
		return;
	}
	integer minimum_int = 32767, maximum_int = -32768;
	for (integer i = imin; i <= imax; i ++) {
		const integer value = my buffer [(i - my imin) * my numberOfChannels + channel];
		if (value < minimum_int)
			minimum_int = value;
		if (value > maximum_int)
			maximum_int = value;
	}
	*minimum = minimum_int / 32768.0;
	*maximum = maximum_int / 32768.0;
}

static struct LongSoundPlay {
	integer numberOfSamples, i1, i2, silenceBefore, silenceAfter;
	double tmin, tmax, dt, t1;
	int16 *resampledBuffer;
	Sound_PlayCallback callback;
	Thing boss;
} thePlayingLongSound;

static bool melderPlayCallback (void *closure, integer samplesPlayed) {
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
		return my callback (my boss, phase, my tmin, my tmax, t);
	return true;
}

void LongSound_playPart (LongSound me, double tmin, double tmax,
	Sound_PlayCallback callback, Thing boss)
{
	struct LongSoundPlay *thee = (struct LongSoundPlay *) & thePlayingLongSound;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	Melder_free (thy resampledBuffer);   // just in case, and after playing has stopped
	try {
		bool fits = LongSound_haveWindow (me, tmin, tmax);
		integer bestSampleRate = MelderAudio_getOutputBestSampleRate (my sampleRate), n, i1, i2;
		if (! fits)
			Melder_throw (U"Sound too long (", tmax - tmin, U" seconds).");
		/*
			Assign to *thee only after stopping the playing sound.
		*/
		thy tmin = tmin;
		thy tmax = tmax;
		thy callback = callback;
		thy boss = boss;
		if ((n = Sampled_getWindowSamples (me, tmin, tmax, & i1, & i2)) < 2) return;
		if (bestSampleRate == my sampleRate) {
			thy numberOfSamples = n;
			thy dt = 1 / my sampleRate;
			thy t1 = my x1;
			thy i1 = i1;
			thy i2 = i2;
			thy silenceBefore = Melder_iroundTowardsZero (my sampleRate * MelderAudio_getOutputSilenceBefore ());
			thy silenceAfter = Melder_iroundTowardsZero (my sampleRate * MelderAudio_getOutputSilenceAfter ());
			if (thy callback)
				thy callback (thy boss, 1, tmin, tmax, tmin);
			if (thy silenceBefore > 0 || thy silenceAfter > 0 || 1) {
				thy resampledBuffer = Melder_calloc (int16, (thy silenceBefore + thy numberOfSamples + thy silenceAfter) * my numberOfChannels);
				memcpy (& thy resampledBuffer [thy silenceBefore * my numberOfChannels],
						my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + (i1 - my imin) * my numberOfChannels,
						thy numberOfSamples * sizeof (int16) * my numberOfChannels);
				MelderAudio_play16 (thy resampledBuffer, my sampleRate, thy silenceBefore + thy numberOfSamples + thy silenceAfter,
						my numberOfChannels, melderPlayCallback, thee);
			} else {
				MelderAudio_play16 (my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + (i1 - my imin) * my numberOfChannels, my sampleRate,
				   thy numberOfSamples, my numberOfChannels, melderPlayCallback, thee);
			}
		} else {
			const integer newSampleRate = bestSampleRate;
			const integer newN = ((double) n * newSampleRate) / my sampleRate - 1;
			const integer silenceBefore = Melder_iroundTowardsZero (newSampleRate * MelderAudio_getOutputSilenceBefore ());
			const integer silenceAfter = Melder_iroundTowardsZero (newSampleRate * MelderAudio_getOutputSilenceAfter ());
			int16 *resampledBuffer = Melder_calloc (int16, (silenceBefore + newN + silenceAfter) * my numberOfChannels);
			int16 *from = my buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + (i1 - my imin) * my numberOfChannels;   // guaranteed: from [0 .. (my imax - my imin + 1) * nchan]
			const double t1 = my x1, dt = 1.0 / newSampleRate;
			thy numberOfSamples = newN;
			thy dt = dt;
			thy t1 = t1 + i1 / my sampleRate;
			thy i1 = 0;
			thy i2 = newN - 1;
			thy silenceBefore = silenceBefore;
			thy silenceAfter = silenceAfter;
			thy resampledBuffer = resampledBuffer;
			if (my numberOfChannels == 1) {
				for (integer i = 0; i < newN; i ++) {
					const double t = t1 + i * dt;   // from t1 to t1 + (newN-1) * dt
					const double index = (t - t1) * my sampleRate;   // from 0
					const integer flore = Melder_iroundTowardsZero (index);
					const double fraction = index - flore;
					resampledBuffer [i + silenceBefore] = (1.0 - fraction) * from [flore] + fraction * from [flore + 1];
				}
			} else if (my numberOfChannels == 2) {
				for (integer i = 0; i < newN; i ++) {
					const double t = t1 + i * dt;
					const double index = (t - t1) * my sampleRate;
					const integer flore = Melder_iroundTowardsZero (index);
					const double fraction = index - flore;
					const integer ii = i + silenceBefore;
					resampledBuffer [ii + ii] = (1.0 - fraction) * from [flore + flore] + fraction * from [flore + flore + 2];
					resampledBuffer [ii + ii + 1] = (1.0 - fraction) * from [flore + flore + 1] + fraction * from [flore + flore + 3];
				}
			} else {
				for (integer i = 0; i < newN; i ++) {
					const double t = t1 + i * dt;
					const double index = (t - t1) * my sampleRate;
					const integer flore = Melder_iroundTowardsZero (index);
					const double fraction = index - flore;
					const integer ii = (i + silenceBefore) * my numberOfChannels;
					for (integer chan = 0; chan < my numberOfChannels; chan ++) {
						resampledBuffer [ii + chan] =
							(1.0 - fraction) * from [flore * my numberOfChannels + chan] +
							fraction * from [(flore + 1) * my numberOfChannels + chan];
					}
				}
			}
			if (thy callback)
				thy callback (thy boss, 1, tmin, tmax, tmin);
			MelderAudio_play16 (resampledBuffer, newSampleRate, silenceBefore + newN + silenceAfter, my numberOfChannels, melderPlayCallback, thee);
		}
		//Melder_free (thy resampledBuffer);   // cannot do that, because MelderAudio_play16 isn't necessarily synchronous
	} catch (MelderError) {
		Melder_free (thy resampledBuffer);
		Melder_throw (me, U": not played.");
	}
}

void LongSound_concatenate (SoundAndLongSoundList me, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint) {
	try {
		integer sampleRate, n;   // integer sampling frequencies only, because of possible rounding errors
		integer numberOfChannels;
		if (my size < 1)
			Melder_throw (U"No Sound or LongSound objects to concatenate.");
		/*
			The sampling frequencies and numbers of channels must be equal for all (long)sounds.
		*/
		Sampled data = my at [1];
		if (data -> classInfo == classSound) {
			Sound sound = (Sound) data;
			sampleRate = Melder_iround (1.0 / sound -> dx);
			numberOfChannels = sound -> ny;
			n = sound -> nx;
		} else {
			LongSound longSound = (LongSound) data;
			sampleRate = Melder_iroundTowardsZero (longSound -> sampleRate);
			numberOfChannels = longSound -> numberOfChannels;
			n = longSound -> nx;
		}
		/*
			Check whether all the sampling frequencies and channels match.
		*/
		for (integer i = 2; i <= my size; i ++) {
			bool sampleRatesMatch, numbersOfChannelsMatch;
			data = my at [i];
			if (data -> classInfo == classSound) {
				Sound sound = (Sound) data;
				sampleRatesMatch = ( Melder_iround (1.0 / sound -> dx) == sampleRate );
				numbersOfChannelsMatch = sound -> ny == numberOfChannels;
				n += sound -> nx;
			} else {
				LongSound longSound = (LongSound) data;
				sampleRatesMatch = ( longSound -> sampleRate == sampleRate );
				numbersOfChannelsMatch = longSound -> numberOfChannels == numberOfChannels;
				n += longSound -> nx;
			}
			if (! sampleRatesMatch)
				Melder_throw (U"Sampling frequencies are not equal.");
			if (! numbersOfChannelsMatch)
				Melder_throw (U"Cannot mix stereo and mono.");
		}
		/*
			Create output file and write header.
		*/
		autoMelderFile mfile = MelderFile_create (file);
		if (file -> filePointer)
			MelderFile_writeAudioFileHeader (file, audioFileType, sampleRate, n, numberOfChannels, numberOfBitsPerSamplePoint);
		for (integer i = 1; i <= my size; i ++) {
			data = my at [i];
			if (data -> classInfo == classSound) {
				Sound sound = (Sound) data;
				if (file -> filePointer) {
					MelderFile_writeFloatToAudio (file, sound -> z.get(),
							Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint), true);
				}
			} else {
				LongSound longSound = (LongSound) data;
				writePartToOpenFile (longSound, audioFileType, 1, longSound -> nx, file, 0, numberOfBitsPerSamplePoint);
			}
		}
		MelderFile_writeAudioFileTrailer (file, audioFileType, sampleRate, n, numberOfChannels, numberOfBitsPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (U"Sounds not concatenated and not saved to ", file, U".");
	}
}

/* End of file LongSound.cpp */
