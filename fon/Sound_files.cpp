/* Sound_files.cpp
 *
 * Copyright (C) 1992-2011,2012,2014,2015,2016,2017 Paul Boersma & David Weenink
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
 * pb 2002/07/16 GPL
 * pb 2003/09/12 MelderFile_checkSoundFile
 * pb 2004/10/17 test for NULL file pointer when closing in Sound_read(2)FromSoundFile
 * pb 2005/06/17 Mac headers
 * pb 2006/01/05 movies for Mac
 * pb 2006/05/29 QuickTime inclusion made optional
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/30 stereo
 * pb 2006/01/01 more stereo
 * pb 2007/01/28 removed a warning
 * pb 2007/01/28 made readFromMovieFile compatible with stereo
 * pb 2007/03/17 resistant against conflicting declarations of Collection
 * pb 2007/05/08 removed warning about stereo sounds
 * pb 2007/10/05 removed FSSpec
 * pb 2007/10/05 made Sound_readFromMacSoundFile compatible with sampling frequencies between 32768 and 65535 Hz
 * pb 2008/01/19 double
 * pb 2009/09/21 made stereo movies readable
 * pb 2010/12/27 support for multiple channels (i.e. more than two)
 * pb 2011/06/07 C++
 * pb 2012/04/29 removed Sound Manager stuff
 * pb 2012/04/29 removed QuickTime
 */

/*
static void Sound_ulawDecode (Sound me) {
	double mu = 100, lnu1 = log (1 + mu);
	for (long i = 1; i <= my nx; i ++) {
		double zabs = (exp (fabs (my z [1] [i]) * lnu1) - 1.0) / mu;
		my z [1] [i] = my z [1] [i] < 0 ? -zabs : zabs;
	} 
}
static void Sound_alawDecode (Sound me) {
	double a = 87.6, lna1 = 1.0 + log (a);
	for (long i = 1; i <= my nx; i ++) {
		double zabs = fabs (my z [1] [i]);
		if (zabs <= 1.0 / lna1) {
			my z [1] [i] *= lna1 / a;
		} else {
			double t = exp (lna1 * zabs - 1.0) / a;
			my z [1] [i] = my z [1] [i] > 0 ? t : - t;
		}
	}
}
*/

#include <time.h>
#include "Sound.h"

autoSound Sound_readFromSoundFile (MelderFile file) {
	try {
		autoMelderFile mfile = MelderFile_open (file);
		int numberOfChannels, encoding;
		double sampleRate;
		long startOfData;
		int32 numberOfSamples;
		int fileType = MelderFile_checkSoundFile (file, & numberOfChannels, & encoding, & sampleRate, & startOfData, & numberOfSamples);
		if (fileType == 0)
			Melder_throw (U"Not an audio file.");
		if (fseek (file -> filePointer, startOfData, SEEK_SET) == EOF)   // start from beginning of Data Chunk
			Melder_throw (U"No data in audio file.");
		if (numberOfSamples < 1)
			Melder_throw (U"Audio file contains 0 samples.");
		autoSound me = Sound_createSimple (numberOfChannels, numberOfSamples / sampleRate, sampleRate);
		if (encoding == Melder_SHORTEN || encoding == Melder_POLYPHONE)
			Melder_throw (U"Cannot unshorten. Write to paul.boersma@uva.nl for more information.");
		Melder_readAudioToFloat (file -> filePointer, numberOfChannels, encoding, my z, numberOfSamples);
		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from sound file ", file, U".");
	}
}

autoSound Sound_readFromSesamFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		int32_t header [1 + 128];
		for (long i = 1; i <= 128; i ++)
			header [i] = bingeti32LE (f);
		/*
		 * Try SESAM header.
		 */
		double samplingFrequency = header [126];   // converting up (from 32 to 54 bits)
		int32_t numberOfSamples = header [127];
		if (samplingFrequency == 0.0 || numberOfSamples == 0) {
			/*
			 * Try LVS header.
			 */
			samplingFrequency = header [62];
			numberOfSamples = (header [6] << 8) - header [68];
		}
		if (numberOfSamples < 1 || numberOfSamples > 1000000000 || samplingFrequency < 10.0 || samplingFrequency > 100000000.0)
			Melder_throw (U"Not a correct SESAM or LVS file.");
		autoSound me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);
		for (int32_t i = 1; i <= numberOfSamples; i ++) {
			my z [1] [i] = (double) bingeti16LE (f) * (1.0 / 2048);   // 12 bits
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from Sesam file ", file, U".");
	}
}

autoSound Sound_readFromBellLabsFile (MelderFile file) {
	try {
		/*
		 * Check existence and permissions of file.
		 */
		autofile f = Melder_fopen (file, "rb");

		/*
		 * Check identity of file: first line is "SIG", second line contains a number.
		 */
		char tag [200];
		if (fread (tag, 1, 16, f) < 16 || ! strnequ (tag, "SIG\n", 4))
			Melder_throw (U"Not a Bell-Labs sound file.");
		char *endOfTag = strchr (tag + 4, '\n');
		if (! endOfTag)
			Melder_throw (U"Second line missing or too long.");
		unsigned long tagLength = (endOfTag - tag) + 1;   // probably 12
		unsigned long headerLength = atol (tag + 4);
		if (headerLength <= 0)
			Melder_throw (U"Wrong header-length info.");

		/*
		 * Read data from header.
		 * Use defaults if necessary.
		 */
		autostring8 lines = Melder_calloc (char, headerLength + 1);
		if (fread (lines.peek(), 1, headerLength, f) < headerLength)
			Melder_throw (U"Header too short.");
		unsigned long numberOfSamples = 0;
		char *psamples = & lines [-1];
		while (!! (psamples = strstr (psamples + 1, "samples ")))   // take last occurrence
			numberOfSamples = atol (psamples + 8);
		if (numberOfSamples < 1) {
			/* Use file length. */
			fseek (f, 0, SEEK_END);   /* Position file pointer at end of file. */
			numberOfSamples = (ftell (f) - tagLength - headerLength) / 2;
		}
		if (numberOfSamples < 1)
			Melder_throw (U"No samples found.");
		double samplingFrequency = 0.0;
		char *pfrequency = & lines [-1];
		while (!! (pfrequency = strstr (pfrequency + 1, "frequency ")))   // take last occurrence
			samplingFrequency = atof (pfrequency + 10);
		if (samplingFrequency <= 0.0)
			samplingFrequency = 16000.0;

		/*
		 * Create sound.
		 */
		autoSound me = Sound_createSimple (1, numberOfSamples / samplingFrequency, samplingFrequency);

		/*
		 * Read samples.
		 */
		fseek (f, tagLength + headerLength, SEEK_SET);
		for (unsigned long i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = (double) bingeti16 (f) * (1.0 / 32768);   // 16-bits big-endian

		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from Bell Labs sound file ", file, U".");
	}
}

static void readError () {
	Melder_throw (U"Error reading bytes from file.");
}

autoSound Sound_readFromKayFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");

		/* Read header of KAY file: 12 bytes. */

		char data [100];
		if (fread (data, 1, 12, f) < 12) readError ();
		if (! strnequ (data, "FORMDS16", 8))
			Melder_throw (U"Not a KAY DS-16 file.");

		/* HEDR or HDR8 chunk */

		if (fread (data, 1, 4, f) < 4) readError ();
		if (! strnequ (data, "HEDR", 4) && ! strnequ (data, "HDR8", 4))
			Melder_throw (U"Missing HEDR or HDR8 chunk. Please report to paul.boersma@uva.nl.");
		uint32_t chunkSize = bingetu32LE (f);
		if (chunkSize & 1) ++ chunkSize;
		if (chunkSize != 32 && chunkSize != 44)
			Melder_throw (U"Unknown chunk size %ld. Please report to paul.boersma@uva.nl.", chunkSize);
		if (fread (data, 1, 20, f) < 20) readError ();
		double samplingFrequency = bingetu32LE (f);   // converting up (from 32 to 53 bits)
		uint32_t numberOfSamples = bingetu32LE (f);
		if (samplingFrequency <= 0 || samplingFrequency > 1e7 || numberOfSamples >= 1000000000)
			Melder_throw (U"Not a correct Kay file.");
		int16_t tmp1 = bingeti16LE (f);
		int16_t tmp2 = bingeti16LE (f);
		long numberOfChannels = tmp1 == -1 || tmp2 == -1 ? 1 : 2;
		if (chunkSize == 44)
			if (fread (data, 1, 12, f) < 12) readError ();

		/* SD chunk */

		if (fread (data, 1, 4, f) < 4) readError ();
		while (! strnequ (data, "SDA_", 4) && ! strnequ (data, "SD_B", 4)) {
			if (feof ((FILE *) f))
				Melder_throw (U"Missing or unreadable SD chunk. Please report to paul.boersma@uva.nl.");
			chunkSize = bingetu32LE (f);
			if (chunkSize & 1) ++ chunkSize;
			if (fread (data, 1, chunkSize, f) < chunkSize) readError ();
			if (fread (data, 1, 4, f) < 4) readError ();
		}
		chunkSize = bingetu32LE (f);
		if (chunkSize != numberOfSamples * 2)
			Melder_throw (U"Incomplete SD chunk. Please report to paul.boersma@uva.nl.");

		autoSound me = Sound_createSimple (numberOfChannels, numberOfSamples / samplingFrequency, samplingFrequency);
		for (long ichan = 1; ichan <= numberOfChannels; ichan ++) {
			for (unsigned long i = 1; i <= numberOfSamples; i ++) {
				my z [ichan] [i] = (double) bingeti16LE (f) / 32768.0;
			}
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from Kay file ", file, U".");
	}
}

autoSound Sound_readFromRawAlawFile (MelderFile file) {
	try {
		double sampleRate = 8000.0;
		autofile f = Melder_fopen (file, "rb");
		fseek (f, 0, SEEK_END);
		long numberOfSamples = ftell (f);
		rewind (f);
		autoSound me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate); 
		Melder_readAudioToFloat (f, 1, Melder_ALAW, my z, numberOfSamples);
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not read from raw A-law file ", file, U".");
	}
}

void Sound_saveAsAudioFile (Sound me, MelderFile file, int audioFileType, int numberOfBitsPerSamplePoint) {
	try {
		autoMelderFile mfile = MelderFile_create (file);
		MelderFile_writeAudioFileHeader (file, audioFileType, lround (1.0 / my dx), my nx, my ny, numberOfBitsPerSamplePoint);
		MelderFile_writeFloatToAudio (file, my ny, Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint), my z, my nx, true);
		MelderFile_writeAudioFileTrailer (file, audioFileType, lround (1.0 / my dx), my nx, my ny, numberOfBitsPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to 16-bit sound file ", file, U".");
	}
}

void Sound_saveAsSesamFile (Sound me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "wb");
		long header [1 + 128], tail;
		for (long i = 1; i <= 128; i ++) header [i] = 0;
		/* ILS header. */
			header [6] = ((my nx - 1) >> 8) + 1;   // number of disk blocks
			header [64] = 32149;   // ILS magic
		/* LVS header. */
			header [62] = lround (1 / my dx);   // sampling frequency, rounded to n Hz
			header [63] = -32000;   // magic: "sampled signal"
			header [66] = INT12_MAX;   // maximum absolute value: 12 bits
			header [67] = 2047;   // LVS magic
			header [68] = my nx % 256;   // number of samples in last block
			header [69] = 1;   // ?
		/* Sesam header. */
			header [126] = lround (1 / my dx);   // sampling frequency, rounded to n Hz
			header [127] = my nx;   // number of samples
		for (long i = 1; i <= 128; i ++) binputi32LE (header [i], f);
		for (long i = 1; i <= my nx; i ++) binputi16LE (lround (my z [1] [i] * 2048), f);
		tail = 256 - my nx % 256;
		if (tail == 256) tail = 0;
		for (long i = 1; i <= tail; i ++) binputi16LE (0, f);   // pad last block with zeroes
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": not written to Sesam file ", file, U".");
	}
}

void Sound_saveAsKayFile (Sound me, MelderFile file) {
	try {
		autoMelderFile mfile = MelderFile_create (file);

		/* Form Chunk: contains all other chunks. */
		fwrite ("FORMDS16", 1, 8, file -> filePointer);
		binputi32LE (48 + my nx * 2, file -> filePointer);   // size of Form Chunk
		fwrite ("HEDR", 1, 4, file -> filePointer);
		binputi32LE (32, file -> filePointer);

		char date [100];
		time_t today = time (nullptr);
		strcpy (date, ctime (& today));	
		fwrite (date+4, 1, 20, file -> filePointer);   // skip weekday

		binputi32LE (lround (1 / my dx), file -> filePointer);   // sampling frequency
		binputi32LE (my nx, file -> filePointer);   // number of samples
		int maximumA = 0;
		for (long i = 1; i <= my nx; i ++) {
			long value = lround (my z [1] [i] * 32768);
			if (value < - maximumA) maximumA = - value;
			if (value > maximumA) maximumA = value;
		}
		binputi16LE (maximumA, file -> filePointer);   // absolute maximum window A
		if (my ny == 1) {
			binputi16LE (-1, file -> filePointer);
		} else {
			int maximumB = 0;
			for (long i = 1; i <= my nx; i ++) {
				long value = lround (my z [2] [i] * 32768);
				if (value < - maximumB) maximumB = - value;
				if (value > maximumB) maximumB = value;
			}
			binputi16LE (maximumB, file -> filePointer);   // absolute maximum window B
		}
		fwrite ("SDA_", 1, 4, file -> filePointer);
		binputi32LE (my nx * 2, file -> filePointer);   // chunk size

		MelderFile_writeFloatToAudio (file, 1, Melder_LINEAR_16_LITTLE_ENDIAN, my z, my nx, true);
		if (my ny > 1)
			MelderFile_writeFloatToAudio (file, 1, Melder_LINEAR_16_LITTLE_ENDIAN, my z + 1, my nx, true);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to Kay sound file ", file, U".");
	}
}

void Sound_saveAsRawSoundFile (Sound me, MelderFile file, int encoding) {
	try {
		autoMelderFile mfile = MelderFile_create (file);
		MelderFile_writeFloatToAudio (file, my ny, encoding, my z, my nx, true);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to raw sound file ", file, U".");
	}
}

/* End of file Sound_files.cpp */
