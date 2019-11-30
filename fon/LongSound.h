#ifndef _LongSound_h_
#define _LongSound_h_
/* LongSound.h
 *
 * Copyright (C) 1992-2005,2007,2008,2010-2012,2015-2019 Paul Boersma, 2007 Erez Volk (for FLAC, MP3)
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

#include "Sound.h"
#include "Collection.h"

#define COMPRESSED_MODE_READ_FLOAT 0
#define COMPRESSED_MODE_READ_SHORT 1

struct FLAC__StreamDecoder;
struct FLAC__StreamEncoder;
struct _MP3_FILE;

Thing_define (LongSound, Sampled) {
	structMelderFile file;
	FILE *f;
	int audioFileType, encoding, numberOfBytesPerSamplePoint;
	integer numberOfChannels;
	double sampleRate;
	integer startOfData;
	double bufferLength;

	integer nmax;
	autovector <int16> buffer;   // this is always 16-bit, because we will always play sounds in 16-bit, even those from 24-bit files
	integer imin, imax;
	void invalidateBuffer () noexcept { our imin = 1; our imax = 0; }

	struct FLAC__StreamDecoder *flacDecoder;
	struct _MP3_FILE *mp3f;
	int compressedMode;
	integer compressedSamplesLeft;
	double *compressedFloats [2];
	int16 *compressedShorts;

	void v_destroy () noexcept
		override;
	void v_info ()
		override;
	void v_copy (Daata data_to)
		override;
	bool v_writable ()
		override { return false; }
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoLongSound LongSound_open (MelderFile file);

autoSound LongSound_extractPart (LongSound me, double tmin, double tmax, bool preserveTimes);

bool LongSound_haveWindow (LongSound me, double tmin, double tmax);
/*
 * Returns 0 if error or if window exceeds buffer, otherwise 1;
 */

void LongSound_getWindowExtrema (LongSound me, double tmin, double tmax, integer channel, double *minimum, double *maximum);

void LongSound_playPart (LongSound me, double tmin, double tmax,
	Sound_PlayCallback callback, Thing boss);

void LongSound_savePartAsAudioFile (LongSound me, int audioFileType, double tmin, double tmax, MelderFile file, int numberOfBitsPerSamplePoint);
void LongSound_saveChannelAsAudioFile (LongSound me, int audioFileType, integer channel, MelderFile file);

void LongSound_readAudioToFloat (LongSound me, MAT buffer, integer firstSample);
void LongSound_readAudioToShort (LongSound me, int16 *buffer, integer firstSample, integer numberOfSamples);

Collection_define (SoundAndLongSoundList, OrderedOf, Sampled) {
};

void LongSound_concatenate (SoundAndLongSoundList collection, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint);

void LongSound_preferences ();
integer LongSound_getBufferSizePref_seconds ();
void LongSound_setBufferSizePref_seconds (integer size);

/* End of file LongSound.h */
#endif
