#ifndef _LongSound_h_
#define _LongSound_h_
/* LongSound.h
 *
 * Copyright (C) 1992-2012 Paul Boersma, 2007 Erez Volk (for FLAC, MP3)
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

#include "Sound.h"
#include "Collection.h"

#define COMPRESSED_MODE_READ_FLOAT 0
#define COMPRESSED_MODE_READ_SHORT 1

struct FLAC__StreamDecoder;
struct FLAC__StreamEncoder;
struct _MP3_FILE;

Thing_define (LongSound, Sampled) {
	// new data:
	public:
		structMelderFile file;
		FILE *f;
		int audioFileType, numberOfChannels, encoding, numberOfBytesPerSamplePoint;
		double sampleRate;
		long startOfData;
		double bufferLength;
		short *buffer;
		long imin, imax, nmax;
		struct FLAC__StreamDecoder *flacDecoder;
		struct _MP3_FILE *mp3f;
		int compressedMode;
		long compressedSamplesLeft;
		double *compressedFloats [2];
		short *compressedShorts;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_info ();
		virtual void v_copy (Any data_to);
		virtual bool v_writable () { return false; }
		virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
};

LongSound LongSound_open (MelderFile fs);

Sound LongSound_extractPart (LongSound me, double tmin, double tmax, int preserveTimes);

bool LongSound_haveWindow (LongSound me, double tmin, double tmax);
/*
 * Returns 0 if error or if window exceeds buffer, otherwise 1;
 */

void LongSound_getWindowExtrema (LongSound me, double tmin, double tmax, int channel, double *minimum, double *maximum);

void LongSound_playPart (LongSound me, double tmin, double tmax,
	int (*callback) (void *closure, int phase, double tmin, double tmax, double t), void *closure);

void LongSound_writePartToAudioFile (LongSound me, int audioFileType, double tmin, double tmax, MelderFile file);
void LongSound_writeChannelToAudioFile (LongSound me, int audioFileType, int channel, MelderFile file);

void LongSound_readAudioToFloat (LongSound me, double **buffer, long firstSample, long numberOfSamples);
void LongSound_readAudioToShort (LongSound me, short *buffer, long firstSample, long numberOfSamples);

void LongSound_concatenate (Collection collection, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint);
/* Concatenate a collection of Sound and LongSound objects. */

void LongSound_preferences (void);
long LongSound_getBufferSizePref_seconds (void);
void LongSound_setBufferSizePref_seconds (long size);

/* End of file LongSound.h */
#endif
