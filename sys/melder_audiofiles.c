/* melder_audiofiles.c
 *
 * Copyright (C) 1992-2007 Paul Boersma & David Weenink, 2007 Erez Volk (for FLAC)
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
 * pb 2002/12/16 corrected bug for cases in which format chunk follows data chunk in WAV file
 * pb 2003/09/12 Sound Designer II files
 * pb 2004/05/14 support for reading 24-bit and 32-bit audio files
 * pb 2004/11/12 writeShortToAudio can write single channels of stereo signal
 * pb 2004/11/15 fast reading of 16-bit audio files
 * pb 2006/12/13 32-bit IEEE float audio files
 * pb 2007/01/24 better error message for DVI ADPCM
 * Erez Volk 2007/03 FLAC reading
 * Erez Volk 2007/05/14 FLAC writing
 * pb 2007/05/17 corrected stereo FLAC writing
 * Erez Volk 2007/06/02 MP3 reading
 */

#include "melder.h"
#include "abcio.h"
#include "math.h"
#include "flac_FLAC_metadata.h"
#include "flac_FLAC_stream_decoder.h"
#include "flac_FLAC_stream_encoder.h"
#include "mp3.h"
#if defined (macintosh)
	#include <Resources.h>
#endif

/***** WRITING *****/

#define BYTES_PER_SAMPLE_PER_CHANNEL  2
#define BITS_PER_SAMPLE_PER_CHANNEL  16
#ifndef WAVE_FORMAT_PCM
	#define WAVE_FORMAT_PCM  0x0001
#endif
#define WAVE_FORMAT_IEEE_FLOAT  0x0003
#define WAVE_FORMAT_ALAW  0x0006
#define WAVE_FORMAT_MULAW  0x0007
#define WAVE_FORMAT_DVI_ADPCM 0x0011

static int MelderFile_createFlacFile (MelderFile file, long sampleRate, long numberOfSamples, int numberOfChannels) {
	FLAC__StreamEncoder *encoder;

	if ((encoder = FLAC__stream_encoder_new ()) == NULL)
		return Melder_error ("Error creating FLAC stream encoder");
	FLAC__stream_encoder_set_bits_per_sample (encoder, 16);
	FLAC__stream_encoder_set_channels (encoder, numberOfChannels);
	FLAC__stream_encoder_set_sample_rate (encoder, sampleRate);
	FLAC__stream_encoder_set_total_samples_estimate (encoder, numberOfSamples);

	if (FLAC__stream_encoder_init_FILE (encoder, file -> filePointer, NULL, NULL) != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
		FLAC__stream_encoder_delete (encoder);
		return Melder_error ("Error creating FLAC stream encoder");
	}

	file -> flacEncoder = encoder;
	file -> outputEncoding = Melder_OUTPUT_ENCODING_FLAC;
	return 1;
}

int MelderFile_writeAudioFileHeader16 (MelderFile file, int audioFileType, long sampleRate, long numberOfSamples, int numberOfChannels) {
	FILE *f = file -> filePointer;
	switch (audioFileType) {
		case Melder_AIFF: {
			long dataSize = numberOfSamples * BYTES_PER_SAMPLE_PER_CHANNEL * numberOfChannels;

			/* Form Chunk: contains all other chunks. */
			fwrite ("FORM", 1, 4, f);
			binputi4 (4 + (8 + 4) + (8 + 18) + (8 + 8 + dataSize), f);   /* Size of Form Chunk. */
			fwrite ("AIFF", 1, 4, f);   /* File type. */

			/* Format Version Chunk: 8 + 4 bytes. */
			fwrite ("FVER", 1, 4, f); binputi4 (4, f);
			binputi4 (0xA2805140, f); /* Time of version. */

			/* Common Chunk: 8 + 18 bytes. */
			fwrite ("COMM", 1, 4, f); binputi4 (18, f);
			binputi2 (numberOfChannels, f);
			binputi4 (numberOfSamples, f);
			binputi2 (BITS_PER_SAMPLE_PER_CHANNEL, f);
			binputr10 (sampleRate, f);

			/* Sound Data Chunk: 8 + 8 bytes + samples. */
			fwrite ("SSND", 1, 4, f); binputi4 (8 + dataSize, f);
			binputi4 (0, f);   /* Offset. */
			binputi4 (0, f);   /* Block size. */
		} break;
		case Melder_AIFC: {
			long dataSize = numberOfSamples * BYTES_PER_SAMPLE_PER_CHANNEL * numberOfChannels;

			/* Form Chunk: contains all other chunks. */
			fwrite ("FORM", 1, 4, f);
			binputi4 (4 + (8 + 4) + (8 + 24) + (8 + 8 + dataSize), f);   /* Size of Form Chunk. */
			fwrite ("AIFC", 1, 4, f);   /* File type. */

			/* Format Version Chunk: 8 + 4 bytes. */
			fwrite ("FVER", 1, 4, f); binputi4 (4, f);
			binputi4 (0xA2805140, f); /* Time of version. */

			/* Common Chunk: 8 + 24 bytes. */
			fwrite ("COMM", 1, 4, f); binputi4 (24, f);
			binputi2 (numberOfChannels, f);
			binputi4 (numberOfSamples, f);
			binputi2 (BITS_PER_SAMPLE_PER_CHANNEL, f);
			binputr10 (sampleRate, f);
			fwrite ("NONE", 1, 4, f);   /* Type of compression. */
			binputi2 (0, f);   /* Name of compression. */

			/* Sound Data Chunk: 8 + 8 bytes + samples. */
			fwrite ("SSND", 1, 4, f); binputi4 (8 + dataSize, f);
			binputi4 (0, f);   /* Offset. */
			binputi4 (0, f);   /* Block size. */
		} break;
		case Melder_WAV: {
			long dataSize = numberOfSamples * BYTES_PER_SAMPLE_PER_CHANNEL * numberOfChannels;

			/* RIFF Chunk: contains all other chunks. */
			fwrite ("RIFF", 1, 4, f);
			binputi4LE (4 + (12 + 16) + (4 + dataSize), f);
			fwrite ("WAVE", 1, 4, f);   /* File type. */

			/* Format Chunk: 8 + 16 bytes. */
			fwrite ("fmt ", 1, 4, f); binputi4LE (16, f);
			binputi2LE (WAVE_FORMAT_PCM, f);
			binputi2LE (numberOfChannels, f);
			binputi4LE (sampleRate, f);   /* Number of samples per second. */
			binputi4LE (sampleRate * BYTES_PER_SAMPLE_PER_CHANNEL * numberOfChannels, f);   /* Average number of bytes per second. */
			binputi2LE (BYTES_PER_SAMPLE_PER_CHANNEL * numberOfChannels, f);   /* Block alignment. */
			binputi2LE (BITS_PER_SAMPLE_PER_CHANNEL, f);   /* Bits per sample point. */

			/* Data Chunk: 8 bytes + samples. */
			fwrite ("data", 1, 4, f); binputi4LE (dataSize, f);
		} break;
		case Melder_NEXT_SUN: {
			fwrite (".snd", 1, 4, f);   /* Tag. */
			binputi4 (32, f);   /* Length of header. */
			binputi4 (numberOfSamples * 2 * numberOfChannels, f);   /* Length of data. */
			binputi4 (3, f);   /* 16-bits linear, not mu-law or A-law. */
			binputi4 (sampleRate, f);
			binputi4 (numberOfChannels, f);
			binputi4 (0, f);
			binputi4 (0, f);
		} break;
		case Melder_NIST: {
			char header [1024];
			memset (header, 0, 1024);
			sprintf (header, "NIST_1A\n   1024\n"
				"channel_count -i %d\n"
				"sample_count -i %ld\n"
				"sample_n_bytes -i 2\n"
				"sample_byte_format -s2 01\n" /* 01=LE 10=BE */
				"sample_coding -s3 pcm\n"
				"sample_rate -i %ld\n"
				"sample_min -i -32768\n"
				"sample_max -i 32767\n"
				"end_head\n", numberOfChannels, numberOfSamples, sampleRate);
			fwrite (header, 1, 1024, f);
		} break;
		case Melder_SOUND_DESIGNER_TWO: {
			return Melder_error ("Cannot yet write Sound Designer II files.");
		} break;
		case Melder_FLAC: {
			return MelderFile_createFlacFile (file, sampleRate, numberOfSamples, numberOfChannels);
		} break;
		default: return Melder_error ("Unknown audio file type %d.", audioFileType);
	}
	return 1;
}

static wchar_t *audioFileTypeString [] = { L"none", L"AIFF", L"AIFC", L"WAV", L"NeXT/Sun", L"NIST", L"Sound Designer II", L"FLAC", L"MP3" };
wchar_t * Melder_audioFileTypeString (int audioFileType) { return audioFileType > Melder_NUMBER_OF_AUDIO_FILE_TYPES ? L"unknown" : audioFileTypeString [audioFileType]; }
static wchar_t *macAudioFileType [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { L"", L"AIFF", L"AIFC", L"WAVE", L"ULAW", L"NIST", L"Sd2f", L"FLAC", L"MP3" };
wchar_t * Melder_macAudioFileType (int audioFileType) { return macAudioFileType [audioFileType]; }
static wchar_t *winAudioFileExtension [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { L"", L".aiff", L".aifc", L".wav", L".au", L".nist", L".sd2", L".flac", L".mp3" };
wchar_t * Melder_winAudioFileExtension (int audioFileType) { return winAudioFileExtension [audioFileType]; }
static int defaultAudioFileEncoding16 [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { 0, Melder_LINEAR_16_BIG_ENDIAN, Melder_LINEAR_16_BIG_ENDIAN, Melder_LINEAR_16_LITTLE_ENDIAN,
	     Melder_LINEAR_16_BIG_ENDIAN, Melder_LINEAR_16_LITTLE_ENDIAN, Melder_LINEAR_16_BIG_ENDIAN,
	     Melder_FLAC_COMPRESSION, Melder_MPEG_COMPRESSION };
int Melder_defaultAudioFileEncoding16 (int audioFileType) { return defaultAudioFileEncoding16 [audioFileType]; }

int MelderFile_writeAudioFile16 (MelderFile file, int audioFileType, const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels) {
	MelderFile_create (file, macAudioFileType [audioFileType], L"PpgB", winAudioFileExtension [audioFileType]);
	if (file -> filePointer) {
		MelderFile_writeAudioFileHeader16 (file, audioFileType, sampleRate, numberOfSamples, numberOfChannels);
		MelderFile_writeShortToAudio (file, numberOfChannels, defaultAudioFileEncoding16 [audioFileType], buffer, numberOfSamples);
	}
	MelderFile_close (file);
	iferror return Melder_error ("(MelderFile_writeAudioFile16:) File not written.");
	return 1;
}

/***** READING *****/

int Melder_bytesPerSamplePoint (int encoding) {
	return
		encoding == Melder_LINEAR_16_BIG_ENDIAN || encoding == Melder_LINEAR_16_LITTLE_ENDIAN ? 2 :
		encoding == Melder_LINEAR_24_BIG_ENDIAN || encoding == Melder_LINEAR_24_LITTLE_ENDIAN ? 3 :
		encoding == Melder_LINEAR_32_BIG_ENDIAN || encoding == Melder_LINEAR_32_LITTLE_ENDIAN ||
		encoding == Melder_IEEE_FLOAT_32_BIG_ENDIAN || encoding == Melder_IEEE_FLOAT_32_LITTLE_ENDIAN ? 4 :
		1;
}

static int ulaw2linear [] = 
      { -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
        -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
        -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
        -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
         -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
         -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
         -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
         -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
         -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
         -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
          -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
          -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
          -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
          -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
          -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
           -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
         32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
         23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
         15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
         11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
          7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
          5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
          3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
          2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
          1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
          1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
           876,    844,    812,    780,    748,    716,    684,    652,
           620,    588,    556,    524,    492,    460,    428,    396,
           372,    356,    340,    324,    308,    292,    276,    260,
           244,    228,    212,    196,    180,    164,    148,    132,
           120,    112,    104,     96,     88,     80,     72,     64,
            56,     48,     40,     32,     24,     16,      8,      0
       };

static short alaw2linear[] = 
{
   -5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736,
   -7552,  -7296,  -8064,  -7808,  -6528,  -6272,  -7040,  -6784,
   -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368,
   -3776,  -3648,  -4032,  -3904,  -3264,  -3136,  -3520,  -3392,
  -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944,
  -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136,
  -11008, -10496, -12032, -11520,  -8960,  -8448,  -9984,  -9472,
  -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568,
    -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296,
    -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,
     -88,    -72,   -120,   -104,    -24,     -8,    -56,    -40,
    -216,   -200,   -248,   -232,   -152,   -136,   -184,   -168,
   -1376,  -1312,  -1504,  -1440,  -1120,  -1056,  -1248,  -1184,
   -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696,
    -688,   -656,   -752,   -720,   -560,   -528,   -624,   -592,
    -944,   -912,  -1008,   -976,   -816,   -784,   -880,   -848,
    5504,   5248,   6016,   5760,   4480,   4224,   4992,   4736,
    7552,   7296,   8064,   7808,   6528,   6272,   7040,   6784,
    2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,
    3776,   3648,   4032,   3904,   3264,   3136,   3520,   3392,
   22016,  20992,  24064,  23040,  17920,  16896,  19968,  18944,
   30208,  29184,  32256,  31232,  26112,  25088,  28160,  27136,
   11008,  10496,  12032,  11520,   8960,   8448,   9984,   9472,
   15104,  14592,  16128,  15616,  13056,  12544,  14080,  13568,
     344,    328,    376,    360,    280,    264,    312,    296,
     472,    456,    504,    488,    408,    392,    440,    424,
      88,     72,    120,    104,     24,      8,     56,     40,
     216,    200,    248,    232,    152,    136,    184,    168,
    1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184,
    1888,   1824,   2016,   1952,   1632,   1568,   1760,   1696,
     688,    656,    752,    720,    560,    528,    624,    592,
     944,    912,   1008,    976,    816,    784,    880,    848
};

static int Melder_checkAiffFile (FILE *f, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	char data [8], chunkID [4];
	int commonChunkPresent = FALSE, dataChunkPresent = FALSE, isAifc = TRUE, numberOfBitsPerSamplePoint;
	long chunkSize, i;

	/* Read header of AIFF(-C) file: 12 bytes. */

	if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no FORM statement.");
	if (! strnequ (data, "FORM", 4)) return Melder_error ("Not an AIFF or AIFC file (FORM statement expected).");
	if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no size of FORM chunk.");
	if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no file type info (expected AIFF or AIFC).");
	if (! strnequ (data, "AIFF", 4) && ! strnequ (data, "AIFC", 4)) return Melder_error ("Not an AIFF or AIFC file (wrong file type info).");
	if (strnequ (data, "AIFF", 4)) isAifc = FALSE;

	/* Search for Common Chunk and Data Chunk. */

	while (fread (chunkID, 1, 4, f) == 4) {
		chunkSize = bingeti4 (f);
		if (chunkSize & 1) ++ chunkSize;   /* Round up to nearest even number. */
		/* IN SOUND FILES PRODUCED BY THE SGI'S soundeditor PROGRAM, */
		/* THE COMMON CHUNK HAS A chunkSize OF 18 INSTEAD OF 38, */
		/* ALTHOUGH THE COMMON CHUNK CONTAINS */
		/* THE 20-BYTE SEQUENCE "\016not compressed\0". */
		/* START FIX OF FOREIGN BUG */
		if(strnequ(chunkID,"NONE",4)&&
			(chunkSize==(14<<24)+('n'<<16)+('o'<<8)+'t'||chunkSize==('t'<<24)+('o'<<16)+('n'<<8)+14))
		{Melder_casual("Ha! a buggy SGI \"soundeditor\" file...");for(i=1;i<=20/*diff*/-8/*header*/;i++)fread(data,1,1,f);continue;}
		/* FINISH FIX OF FOREIGN BUG */
		if (strnequ (chunkID, "COMM", 4)) {
			/*
			 * Found a Common Chunk.
			 */
			commonChunkPresent = TRUE;
			*numberOfChannels = bingeti2 (f);
			if (*numberOfChannels < 1) return Melder_error ("%d sound channels is too few.", *numberOfChannels);
			if (*numberOfChannels > 2) return Melder_error ("%d sound channels is too many.", *numberOfChannels);
			*numberOfSamples = bingeti4 (f);
			if (*numberOfSamples <= 0) return Melder_error ("Too few samples (%ld).", *numberOfSamples);
			if (*numberOfSamples >= 1000000000) return Melder_error ("Too many samples (%ld).", *numberOfSamples);
			numberOfBitsPerSamplePoint = bingeti2 (f);
			if (numberOfBitsPerSamplePoint > 32) return Melder_error ("%d bits per sample is too many (maximum is 32).", numberOfBitsPerSamplePoint);
			*encoding =
				numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_BIG_ENDIAN :
				numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_BIG_ENDIAN :
				numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_BIG_ENDIAN :
				Melder_LINEAR_8_SIGNED;
			*sampleRate = bingetr10 (f);
			if (*sampleRate <= 0.0) return Melder_error ("Wrong sampling frequency (%.17g Hz).", *sampleRate);
			if (isAifc) {
				/*
				 * Read compression data; should be "NONE" or "sowt".
				 */
				if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no compression info.");
				if (! strnequ (data, "NONE", 4) && ! strnequ (data, "sowt", 4)) return Melder_error ("Cannot read compressed AIFC files (compression type %c%c%c%c).",
					data [0], data [1], data [2], data [3]);
				if (strnequ (data, "sowt", 4))
					*encoding =
						numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_LITTLE_ENDIAN :
						Melder_LINEAR_8_SIGNED;
				/*
				 * Read rest of compression info.
				 */
				for (i = 23; i <= chunkSize; i ++)
					if (fread (data, 1, 1, f) < 1) return Melder_error ("File too small: expected chunk of %ld bytes, but found %ld.", chunkSize, i + 22);
			}
		} else if (strnequ (chunkID, "SSND", 4)) {
			/*
			 * Found a Data Chunk.
			 */
			dataChunkPresent = TRUE;
			*startOfData = ftell (f) + 8;   /* Ignore "offset" (4 bytes) and "blocksize" (4 bytes). */
			if (commonChunkPresent) break;   /* Optimization: do not read whole data chunk if we have already read the common chunk. */
		} else /* Ignore Version Chunk and unknown chunks. */
			for (i = 1; i <= chunkSize; i ++)
				if (fread (data, 1, 1, f) < 1) return Melder_error ("File too small: expected %ld bytes, but found %ld.", chunkSize, i);
	}

	if (! commonChunkPresent) return Melder_error ("Found no Common Chunk.");
	if (! dataChunkPresent) return Melder_error ("Found no Data Chunk.");

	return 1;
}

static int Melder_checkWavFile (FILE *f, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	char data [8], chunkID [4];
	int formatChunkPresent = FALSE, dataChunkPresent = FALSE, numberOfBitsPerSamplePoint = -1;
	long chunkSize, dataChunkSize = -1, i;

	if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no RIFF statement.");
	if (! strnequ (data, "RIFF", 4)) return Melder_error ("Not a WAV file (RIFF statement expected).");
	if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no size of RIFF chunk.");
	if (fread (data, 1, 4, f) < 4) return Melder_error ("File too small: no file type info (expected WAVE statement).");
	if (! strnequ (data, "WAVE", 4) && ! strnequ (data, "CDDA", 4)) return Melder_error ("Not a WAVE or CD audio file (wrong file type info).");

	/* Search for Format Chunk and Data Chunk. */

	while (fread (chunkID, 1, 4, f) == 4) {
		chunkSize = bingeti4LE (f);
		if (strnequ (chunkID, "fmt ", 4)) {
			/*
			 * Found a Format Chunk.
			 */
			int winEncoding = bingeti2LE (f);
			formatChunkPresent = TRUE;			
			*numberOfChannels = bingeti2LE (f);
			if (*numberOfChannels < 1) return Melder_error ("%d sound channels is too few.", *numberOfChannels);
			if (*numberOfChannels > 2) return Melder_error ("%d sound channels is too many.", *numberOfChannels);
			*sampleRate = (double) bingeti4LE (f);
			if (*sampleRate <= 0.0) return Melder_error ("Wrong sampling frequency (%.17g Hz).", *sampleRate);
			(void) bingeti4LE (f);   /* avgBytesPerSec */
			(void) bingeti2LE (f);   /* blockAlign */
			numberOfBitsPerSamplePoint = bingeti2LE (f);
			if (numberOfBitsPerSamplePoint == 0) numberOfBitsPerSamplePoint = 16;   /* Default. */
			else if (numberOfBitsPerSamplePoint < 4) return Melder_error ("%d bits per sample is too few (minimum is 4).", numberOfBitsPerSamplePoint);
			else if (numberOfBitsPerSamplePoint > 32) return Melder_error ("%d bits per sample is too many (maximum is 32).", numberOfBitsPerSamplePoint);
			switch (winEncoding) {
				case WAVE_FORMAT_PCM:
					*encoding =
						numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_LITTLE_ENDIAN :
						Melder_LINEAR_8_UNSIGNED;
					break;
				case WAVE_FORMAT_IEEE_FLOAT:
					*encoding = Melder_IEEE_FLOAT_32_LITTLE_ENDIAN;
					break;
				case WAVE_FORMAT_ALAW:
					*encoding = Melder_ALAW;
					break;
				case WAVE_FORMAT_MULAW:
					*encoding = Melder_MULAW;
					break;
				case WAVE_FORMAT_DVI_ADPCM:
					return Melder_error ("Cannot read lossy compressed audio files (this one is DVI ADPCM).\n"
						"Please use uncompressed audio files. If you must open this file,\n"
						"please use an audio converter program to convert it first to normal (PCM) WAV format\n"
						"(Praat may have difficulty analysing the poor recording, though).");
				default:
					return Melder_error ("Unsupported Windows audio encoding %d.", winEncoding);
			}
			for (i = 17; i <= chunkSize; i ++)
				if (fread (data, 1, 1, f) < 1) return Melder_error ("File too small: expected %ld bytes in fmt chunk, but found %ld.", chunkSize, i);
		} else if (strnequ (chunkID, "data", 4)) {
			/*
			 * Found a Data Chunk.
			 */
			dataChunkPresent = TRUE;
			dataChunkSize = chunkSize;
			*startOfData = ftell (f);
			if (formatChunkPresent) break;   /* Optimization: do not read whole data chunk if we have already read the format chunk. */
		} else {   /* Ignore other chunks. */
		    /*Melder_warning ("chunk %c%c%c%c",chunkID[0],chunkID[1],chunkID[2],chunkID[3]);*/
			for (i = 1; i <= chunkSize; i ++)
				if (fread (data, 1, 1, f) < 1) return Melder_error ("File too small: expected %ld bytes, but found %ld.", chunkSize, i);
		}
	}

	if (! formatChunkPresent) return Melder_error ("Found no Format Chunk.");
	if (! dataChunkPresent) return Melder_error ("Found no Data Chunk.");
	Melder_assert (numberOfBitsPerSamplePoint != -1 && dataChunkSize != -1);
	*numberOfSamples = dataChunkSize / *numberOfChannels / ((numberOfBitsPerSamplePoint + 7) / 8);

	return 1;
}

static int Melder_checkNextSunFile (FILE *f, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	long dataSize, sunEncoding, skip;
	char tag [4];
	fread (tag, 1, 4, f);
	if (strncmp (tag, ".snd", 4)) return Melder_error ("Not a Sun audio file.");
	*startOfData = bingeti4 (f);
	if (*startOfData < 24 || *startOfData > 320)
		return Melder_error ("Sorry, cannot read header of audio file. Length %ld.", *startOfData);
	dataSize = bingeti4 (f);
	if (dataSize <= 0) {
		/*
		 * Incorrect information. Get it from file length.
		 */
		long save = ftell (f);
		fseek (f, 0, SEEK_END);
		dataSize = ftell (f) - *startOfData;
		fseek (f, save, SEEK_SET);
	}
	sunEncoding = bingeti4 (f);
	switch (sunEncoding) {
		case 1: *encoding = Melder_MULAW; break;
		case 2: *encoding = Melder_LINEAR_8_SIGNED; break;
		case 3: *encoding = Melder_LINEAR_16_BIG_ENDIAN; break;
		case 27: *encoding = Melder_ALAW; break;
		default: return Melder_error ("Sorry, cannot translate audio file encoding %ld.", sunEncoding);
	}
	*sampleRate = bingeti4 (f);
	if (*sampleRate <= 0) return Melder_error ("Impossible sampling frequency %ld Hertz.", *sampleRate);
	*numberOfChannels = bingeti4 (f);
	if (*numberOfChannels < 1 || *numberOfChannels > 2)
		return Melder_error ("Wrong number of channels in audio file (%d).", *numberOfChannels);
	*numberOfSamples = dataSize / Melder_bytesPerSamplePoint (*encoding) / *numberOfChannels;
	skip = *startOfData - 24;
	while (skip -- > 0) (void) fgetc (f);
	return 1;
}

static int nistGetValue (const char *header, const char *object, double *rval, char *sval) {
	char obj [30], type [10], *match = strstr (header, object);
	if (! match) return 0;
	if (sscanf (match, "%s%s%s", obj, type, sval) != 3) return 0;
	if (strequ (type, "-i") || strequ (type, "-r")) *rval = atof (sval);
	else if (strncmp (type, "-s", 2)) return 0;
	return 1;
}
static int Melder_checkNistFile (FILE *f, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	char header [1024], sval [100];
 	double rval;
	int numberOfBytesPerSamplePoint;
	if (fread (header, 1, 1024, f) != 1024)
		return Melder_error ("Cannot read NISTheader.");
	if (! strnequ (header, "NIST_1A", 7))
		return Melder_error ("Not a NIST sound file.");
	*startOfData = atol (header + 9);
	if (! nistGetValue (header, "sample_count", & rval, sval) || rval < 1)
		return Melder_error ("Incorrect number of samples in NIST file.");
	*numberOfSamples = rval;
	if (! nistGetValue (header, "sample_n_bytes", & rval, sval) || rval < 1 || rval > 2)
		return Melder_error ("Incorrect number of bytes per sample (should be 1 or 2).");
	numberOfBytesPerSamplePoint = rval;
	if (! nistGetValue (header, "channel_count", & rval, sval) || rval < 1)
		return Melder_error ("Incorrect number of channels.");
	*numberOfChannels = rval;
	if (*numberOfChannels > 2)
		return Melder_error ("Too many channels in NIST file.");
	if (! nistGetValue (header, "sample_rate", sampleRate, sval) || *sampleRate < 1)
		return Melder_error ("Incorrect sampling frequency %f Hz.", *sampleRate);
	*encoding = Melder_LINEAR_16_BIG_ENDIAN;
	if (nistGetValue (header, "sample_byte_format", & rval, sval) && strequ (sval, "01"))
		*encoding = Melder_LINEAR_16_LITTLE_ENDIAN;
	if (numberOfBytesPerSamplePoint == 1)
		*encoding = Melder_LINEAR_8_SIGNED;
	if (nistGetValue (header, "sample_coding", & rval, sval)) {
		if (strnequ (sval, "ulaw", 4))
			*encoding = Melder_MULAW;
		else if (strstr (sval, "embedded-shorten-v"))
			if (nistGetValue (header, "database_id", & rval, sval) && strequ (sval, "POLYPHONE-NL"))
				*encoding = Melder_POLYPHONE;
			else
				*encoding = Melder_SHORTEN;
		else if (strnequ (sval, "alaw", 4))   /* Must be after previous, because some files have "alaw,embedded..." */
			*encoding = Melder_ALAW;
	}
	return 1;
}
#ifdef macintosh
static double Melder_getNumberFromStrResource (int resourceID) {
	char *pstring;
	char string [256];
	int length;
	Handle han = Get1Resource ('STR ', resourceID);
	if (! han) { Melder_error ("Resource %d not found.", resourceID); return 0.0; }
	pstring = (char *) (*han);
	length = (unsigned char) pstring [0];
	strncpy (string, & pstring [1], length);
	string [length] = 0;
	ReleaseResource (han);
	return atof (string);
}
static int MelderFile_checkSoundDesignerTwoFile (MelderFile file, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	FSSpec fspec;
	int path = -1, sampleSize;
	Melder_fileToMac (file, & fspec);
	path = FSpOpenResFile (& fspec, fsRdPerm);   /* Open resource fork; there is the header info. */
	if (path == -1) error1 (L"Cannot open resource fork.")
	sampleSize = Melder_getNumberFromStrResource (1000);
	iferror error1 (L"No sample size information.")
	if (sampleSize < 1 || sampleSize > 4) error3 (L"Wrong sample size: ", Melder_integer (sampleSize), L" (should be between 1 and 4).")
	*encoding =
		sampleSize == 1 ? Melder_LINEAR_8_SIGNED :
		sampleSize == 2 ? Melder_LINEAR_16_BIG_ENDIAN :
		sampleSize == 3 ? Melder_LINEAR_24_BIG_ENDIAN :
		Melder_LINEAR_32_BIG_ENDIAN;
	*sampleRate = Melder_getNumberFromStrResource (1001);
	iferror error1 (L"No sampling frequency information.")
	if (*sampleRate <= 0.0) error3 (L"Wrong sampling frequency: ", Melder_double (*sampleRate), L" Hz")
	*numberOfChannels = Melder_getNumberFromStrResource (1002);
	iferror error1 (L"No channel number information.")
	if (*numberOfChannels != 1 && *numberOfChannels != 2) error2 (L"Wrong number of channels: ", Melder_integer (*numberOfChannels))
	*numberOfSamples = MelderFile_length (file) / sampleSize / *numberOfChannels;
	if (*numberOfSamples <= 0) error1 (L"No samples in file.")
	*startOfData = 0;
end:
	if (path != -1) CloseResFile (path);
	iferror return Melder_error ("Sound Designer II file not read.");
	return 1;
}
#endif

static int Melder_checkFlacFile (MelderFile file, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	FLAC__StreamMetadata metadata;
	FLAC__StreamMetadata_StreamInfo *info;
	if (! FLAC__metadata_get_streaminfo (Melder_peekWcsToUtf8 (Melder_fileToPathW (file)), & metadata))   // BUG: not Unicode-compatible on Windows.
		return Melder_error ("Invalid FLAC file");
	info = & metadata. data. stream_info;
	*numberOfChannels = info -> channels;
	*encoding = Melder_FLAC_COMPRESSION;
	*sampleRate = (double) info -> sample_rate;
	*startOfData = 0;   /* Meaningless, libFLAC does the I/O */
	*numberOfSamples = info -> total_samples;   /* Might lose bits above LONG_MAX */
	if ((FLAC__uint64) *numberOfSamples != info -> total_samples)
		return Melder_error ("FLAC file too long.");
	return 1;
}

static int Melder_checkMp3File (FILE *f, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	MP3_FILE mp3f;
	if (! (mp3f = mp3f_new ()) )
		return Melder_error ("Cannot allocate MP3 decoder object");
	mp3f_set_file (mp3f, f);
	if (! mp3f_analyze (mp3f)) {
		mp3f_delete (mp3f);
		return Melder_error ("Cannot analyze MP3 file");
	}
	*encoding = Melder_MPEG_COMPRESSION;
	*numberOfChannels = mp3f_channels (mp3f);
	*sampleRate = mp3f_frequency (mp3f);
	*numberOfSamples = mp3f_samples (mp3f);
	if ((MP3F_OFFSET)*numberOfSamples != mp3f_samples (mp3f))
		return Melder_error ("MP3 file too long.");
	*startOfData = 0; /* Meaningless */
	mp3f_delete (mp3f);
	return 1;
}

int MelderFile_checkSoundFile (MelderFile file, int *numberOfChannels, int *encoding,
	double *sampleRate, long *startOfData, long *numberOfSamples)
{
	char data [16];
	FILE *f = file -> filePointer;
	if (f == NULL || fread (data, 1, 16, f) < 16) return 0;
	rewind (f);
	if (strnequ (data, "FORM", 4) && strnequ (data + 8, "AIFF", 4)) {
		Melder_checkAiffFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_AIFF;
	}
	if (strnequ (data, "FORM", 4) && strnequ (data + 8, "AIFC", 4)) {
		Melder_checkAiffFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_AIFC;
	}
	if (strnequ (data, "RIFF", 4) && (strnequ (data + 8, "WAVE", 4) || strnequ (data + 8, "CDDA", 4))) {
		Melder_checkWavFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_WAV;
	}
	if (strnequ (data, ".snd", 4)) {
		Melder_checkNextSunFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_NEXT_SUN;
	}
	if (strnequ (data, "NIST_1A", 7)) {
		Melder_checkNistFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_NIST;
	}
	if (strnequ (data, "fLaC", 4)) {
		Melder_checkFlacFile (file, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_FLAC;
	}
	if (mp3_recognize (16, data)) {
		Melder_checkMp3File (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		iferror return 0;
		return Melder_MP3;
	}
	#ifdef macintosh
		if (MelderFile_getMacType (file) == 'Sd2f') {
			MelderFile_checkSoundDesignerTwoFile (file, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
			iferror return 0;
			return Melder_SOUND_DESIGNER_TWO;
		}
	#endif
	return 0;   /* Not a recognized sound file. */
}

/* libFLAC works through callbacks, so we need a context struct. */

typedef struct {
	FILE *file;
	int numberOfChannels;
	long numberOfSamples;
	float *channels [2];
} MelderDecodeFlacContext;

/* The same goes for MP3 */

typedef struct {
	int numberOfChannels;
	long numberOfSamples;
	float *channels [2];
} MelderDecodeMp3Context;

static FLAC__StreamDecoderReadStatus Melder_DecodeFlac_read (const FLAC__StreamDecoder *decoder,
	FLAC__byte buffer [], size_t *bytes, void *client_data)
{
	MelderDecodeFlacContext *c = (MelderDecodeFlacContext *) client_data;
	(void) decoder;
	if (*bytes <= 0) 
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	*bytes = fread (buffer, sizeof (FLAC__byte), *bytes, c -> file);
	if (ferror (c -> file))
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	if (*bytes == 0)
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

static FLAC__StreamDecoderWriteStatus Melder_DecodeFlac_convert (const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data)
{
	MelderDecodeFlacContext *c = (MelderDecodeFlacContext *) client_data;
	const FLAC__FrameHeader *header = & frame -> header;
	unsigned count = header -> blocksize;
	const FLAC__int32 *input;
	float multiplier, *output;
	unsigned i, j;

	(void) decoder;

	switch (header -> bits_per_sample) {
		case 8: multiplier = (1.0f / 128); break;
		case 16: multiplier = (1.0f / 32768); break;
		case 24: multiplier = (1.0f / 8388608); break;
		case 32: multiplier = (1.0f / 32768 / 65536); break;
		default: return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	for (i = 0; i < c -> numberOfChannels; ++ i) {
		input = buffer [i];
		output = c -> channels [i];
		for (j = 0; j < count; ++ j)
			output [j] = ((long) input [j]) * multiplier;
		c -> channels [i] += count;
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void Melder_DecodeMp3_convert (const MP3F_SAMPLE *channels [MP3F_MAX_CHANNELS], long count, void *context) {
	MelderDecodeMp3Context *c = (MelderDecodeMp3Context *) context;
	const MP3F_SAMPLE *input;
	float *output;
	unsigned i, j;
	for (i = 0; i < c -> numberOfChannels; ++ i) {
		input = channels [i];
		output = c -> channels [i];
		for (j = 0; j < count; ++ j)
			output [j] = mp3f_sample_to_float (input [j]);
		c -> channels [i] += count;
	}
}

static void Melder_DecodeFlac_error (const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
	(void) decoder;
	(void) client_data;
	Melder_warning ("FLAC decoder error: %s", FLAC__StreamDecoderErrorStatusString [status]);
}

static int Melder_readFlacFile (FILE *f, int numberOfChannels, float *leftBuffer, float *rightBuffer, long numberOfSamples) {
	FLAC__StreamDecoder *decoder;
	MelderDecodeFlacContext c;
	int result = 0;

	c.file = f;
	c.numberOfChannels = numberOfChannels;
	c.channels [0] = leftBuffer + 1;
	c.channels [1] = rightBuffer + 1;
	c.numberOfSamples = numberOfSamples;

	if ((decoder = FLAC__stream_decoder_new ()) == NULL)
		goto end;
	if (FLAC__stream_decoder_init_stream (decoder,
				Melder_DecodeFlac_read,
				NULL, NULL, NULL, NULL,
				Melder_DecodeFlac_convert, NULL,
				Melder_DecodeFlac_error, &c) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		goto end;
	result = FLAC__stream_decoder_process_until_end_of_stream (decoder);
	FLAC__stream_decoder_finish (decoder);
end:
	FLAC__stream_decoder_delete (decoder);
	return result;
}

static int Melder_readMp3File (FILE *f, int numberOfChannels, float *leftBuffer, float *rightBuffer, long numberOfSamples) {
	MP3_FILE mp3f;
	MelderDecodeMp3Context c;
	int result = 0;
	c.numberOfChannels = numberOfChannels;
	c.channels [0] = leftBuffer + 1;
	c.channels [1] = rightBuffer + 1;
	c.numberOfSamples = numberOfSamples;
	if ((mp3f = mp3f_new ()) == NULL)
		return 0;
	mp3f_set_file (mp3f, f);
	mp3f_set_callback (mp3f, Melder_DecodeMp3_convert, &c);
	result = mp3f_read (mp3f, numberOfSamples);
	mp3f_delete (mp3f);
	return result;
}

int Melder_readAudioToFloat (FILE *f, int numberOfChannels, int encoding, float *leftBuffer, float *rightBuffer, long numberOfSamples) {
	long i;
	int readEverything = 0;
	Melder_assert (sizeof (char) == 1 && sizeof (short) == 2 && sizeof (float) == 4);
	switch (encoding) {
		case Melder_LINEAR_8_SIGNED:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++) {
					signed char value;
					if (! fread (& value, 1, 1, f)) error1 (L"File too small (mono 8-bit).")
					leftBuffer [i] = value * (1.0f / 128);
				}
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					signed char left, right;
					if (! fread (& left, 1, 1, f) || ! fread (& right, 1, 1, f)) error1 (L"File too small (stereo 8-bit).")
					leftBuffer [i] = left * (1.0f / 128);
					rightBuffer [i] = right * (1.0f / 128);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					signed char left, right;
					if (! fread (& left, 1, 1, f) || ! fread (& right, 1, 1, f)) error1 (L"File too small (stereo 8-bit).")
					leftBuffer [i] = (left + right) * (1.0f / 256);
				}
			break;
		case Melder_LINEAR_8_UNSIGNED:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingetu1 (f) * (1.0f / 128) - 1.0f;
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingetu1 (f) * (1.0f / 128) - 1.0f;
					rightBuffer [i] = bingetu1 (f) * (1.0f / 128) - 1.0f;
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					int left = bingetu1 (f), right = bingetu1 (f);
					leftBuffer [i] = (left + right) * (1.0f / 256) - 0.5f;
				}
			break;
		case Melder_LINEAR_16_BIG_ENDIAN:
			if (numberOfChannels == 1) {
				unsigned char *bytes = (unsigned char *) & leftBuffer [1] + numberOfSamples * 2;   /* in top half */
				fread (bytes, 1, numberOfSamples * 2, f);
				for (i = 1; i <= numberOfSamples; i ++) {
					unsigned char byte1 = * bytes ++, byte2 = * bytes ++;
					long value = (signed short) (((unsigned short) byte1 << 8) | (unsigned short) byte2);
					leftBuffer [i] = value * (1.0f / 32768);
				}
			} else if (rightBuffer) {
				unsigned char *bytes = (unsigned char *) & leftBuffer [1];
				fread (bytes, 1, numberOfSamples * 4, f);
				for (i = 1; i <= numberOfSamples; i ++) {
					unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
					long left = (signed short) (((unsigned short) byte1 << 8) | (unsigned short) byte2);
					long right = (signed short) (((unsigned short) byte3 << 8) | (unsigned short) byte4);
					leftBuffer [i] = left * (1.0f / 32768);
					rightBuffer [i] = right * (1.0f / 32768);
				}
			} else {
				unsigned char *bytes = (unsigned char *) & leftBuffer [1];
				fread (bytes, 1, numberOfSamples * 4, f);
				for (i = 1; i <= numberOfSamples; i ++) {
					unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
					long left = (signed short) (((unsigned short) byte1 << 8) | (unsigned short) byte2);
					long right = (signed short) (((unsigned short) byte3 << 8) | (unsigned short) byte4);
					leftBuffer [i] = (left + right) * (1.0f / 65536);
				}
			}
			break;
		case Melder_LINEAR_16_LITTLE_ENDIAN:
			if (numberOfChannels == 1) {
				unsigned char *bytes = (unsigned char *) & leftBuffer [1] + numberOfSamples * 2;   /* in top half */
				fread (bytes, 1, numberOfSamples * 2, f);
				for (i = 1; i <= numberOfSamples; i ++) {
					unsigned char byte1 = * bytes ++, byte2 = * bytes ++;
					long value = (signed short) (((unsigned short) byte2 << 8) | (unsigned short) byte1);
					leftBuffer [i] = value * (1.0f / 32768);
				}
			} else if (rightBuffer) {
				unsigned char *bytes = (unsigned char *) & leftBuffer [1];
				fread (bytes, 1, numberOfSamples * 4, f);
				for (i = 1; i <= numberOfSamples; i ++) {
					unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
					long left = (signed short) (((unsigned short) byte2 << 8) | (unsigned short) byte1);
					long right = (signed short) (((unsigned short) byte4 << 8) | (unsigned short) byte3);
					leftBuffer [i] = left * (1.0f / 32768);
					rightBuffer [i] = right * (1.0f / 32768);
				}
			} else {
				unsigned char *bytes = (unsigned char *) & leftBuffer [1];
				fread (bytes, 1, numberOfSamples * 4, f);
				for (i = 1; i <= numberOfSamples; i ++) {
					unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
					long left = (signed short) (((unsigned short) byte2 << 8) | (unsigned short) byte1);
					long right = (signed short) (((unsigned short) byte4 << 8) | (unsigned short) byte3);
					leftBuffer [i] = (left + right) * (1.0f / 65536);
				}
			}
			break;
		case Melder_LINEAR_24_BIG_ENDIAN:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingeti3 (f) * (1.0f / 8388608);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingeti3 (f) * (1.0f / 8388608);
					rightBuffer [i] = bingeti3 (f) * (1.0f / 8388608);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					long left = bingeti3 (f), right = bingeti3 (f);
					leftBuffer [i] = (left + right) * (1.0f / 16777216);
				}
			break;
		case Melder_LINEAR_24_LITTLE_ENDIAN:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingeti3LE (f) * (1.0f / 8388608);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingeti3LE (f) * (1.0f / 8388608);
					rightBuffer [i] = bingeti3LE (f) * (1.0f / 8388608);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					long left = bingeti3LE (f), right = bingeti3LE (f);
					leftBuffer [i] = (left + right) * (1.0f / 16777216);
				}
			break;
		case Melder_LINEAR_32_BIG_ENDIAN:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingeti4 (f) * (1.0f / 32768 / 65536);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingeti4 (f) * (1.0f / 32768 / 65536);
					rightBuffer [i] = bingeti4 (f) * (1.0f / 32768 / 65536);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					long left = bingeti4 (f), right = bingeti4 (f);
					leftBuffer [i] = (left + right) * (1.0f / 65536 / 65536);
				}
			break;
		case Melder_LINEAR_32_LITTLE_ENDIAN:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingeti4LE (f) * (1.0f / 32768 / 65536);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingeti4LE (f) * (1.0f / 32768 / 65536);
					rightBuffer [i] = bingeti4LE (f) * (1.0f / 32768 / 65536);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					long left = bingeti4LE (f), right = bingeti4LE (f);
					leftBuffer [i] = (left + right) * (1.0f / 65536 / 65536);
				}
			break;
		case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingetr4 (f);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingetr4 (f);
					rightBuffer [i] = bingetr4 (f);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					double left = bingetr4 (f), right = bingetr4 (f);
					leftBuffer [i] = (left + right) * 0.5;
				}
			break;
		case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = bingetr4LE (f);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = bingetr4LE (f);
					rightBuffer [i] = bingetr4LE (f);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					double left = bingetr4LE (f), right = bingetr4LE (f);
					leftBuffer [i] = (left + right) * 0.5;
				}
			break;
		case Melder_MULAW:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = ulaw2linear [bingetu1 (f)] * (1.0f / 32768);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = ulaw2linear [bingetu1 (f)] * (1.0f / 32768);
					rightBuffer [i] = ulaw2linear [bingetu1 (f)] * (1.0f / 32768);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					long left = ulaw2linear [bingetu1 (f)], right = ulaw2linear [bingetu1 (f)];
					leftBuffer [i] = (left + right) * (1.0f / 65536);
				}
			break;
		case Melder_ALAW:
			if (numberOfChannels == 1)
				for (i = 1; i <= numberOfSamples; i ++)
					leftBuffer [i] = alaw2linear [bingetu1 (f)] * (1.0f / 32768);
			else if (rightBuffer)
				for (i = 1; i <= numberOfSamples; i ++) {
					leftBuffer [i] = alaw2linear [bingetu1 (f)] * (1.0f / 32768);
					rightBuffer [i] = alaw2linear [bingetu1 (f)] * (1.0f / 32768);
				}
			else
				for (i = 1; i <= numberOfSamples; i ++) {
					long left = alaw2linear [bingetu1 (f)], right = alaw2linear [bingetu1 (f)];
					leftBuffer [i] = (left + right) * (1.0f / 65536);
				}
			break;
		case Melder_FLAC_COMPRESSION:
			if (! Melder_readFlacFile (f, numberOfChannels, leftBuffer, rightBuffer, numberOfSamples))
				return Melder_error ("(Melder_readAudioToFloat:) Error decoding FLAC file.");
			break;
		case Melder_MPEG_COMPRESSION:
			if (! Melder_readMp3File (f, numberOfChannels, leftBuffer, rightBuffer, numberOfSamples))
				return Melder_error ("(Melder_readAudioToFloat:) Error decoding MP3 file.");
			break;
		default:
			return Melder_error ("(Melder_readAudioToFloat:) Unknown encoding %d.", encoding);
	}
	readEverything = 1;
end:
	if (! readEverything) Melder_warning ("Audio file too short. Missing samples were set to zero.");
	if (ferror (f)) return Melder_error ("(Melder_readAudioToFloat:) Error reading audio samples from file.");
	return 1;
}

int Melder_readAudioToShort (FILE *f, int numberOfChannels, int encoding, short *buffer, long numberOfSamples) {
	long n = numberOfSamples * numberOfChannels, i;
	static const unsigned short byteSwapTest = 3 * 256 + 1;
	switch (encoding) {
		case Melder_LINEAR_8_SIGNED:
			for (i = 0; i < n; i ++) {
				signed char value;
				if (! fread (& value, 1, 1, f)) error1 (L"File too small (mono 8-bit).")
				buffer [i] = value * 256;
			}
			break;
		case Melder_LINEAR_8_UNSIGNED:
			for (i = 0; i < n; i ++)
				buffer [i] = bingetu1 (f) * 256L - 32768;
			break;
		case Melder_LINEAR_16_BIG_ENDIAN:
			fread (buffer, sizeof (short), n, f);
			if (* (unsigned char *) & byteSwapTest == 1) {
				for (i = 0; i < n; i ++) {
					unsigned short value = buffer [i];
					buffer [i] = (value >> 8) + (value << 8);
				}
			}
			break;
		case Melder_LINEAR_16_LITTLE_ENDIAN:
			fread (buffer, sizeof (short), n, f);
			if (* (unsigned char *) & byteSwapTest == 3) {
				for (i = 0; i < n; i ++) {
					unsigned short value = buffer [i];
					buffer [i] = (value >> 8) + (value << 8);
				}
			}
			break;
		case Melder_LINEAR_24_BIG_ENDIAN:
			for (i = 0; i < n; i ++)
				buffer [i] = bingeti3 (f) / 256;   /* BUG: truncation; not ideal. */
			break;
		case Melder_LINEAR_24_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++)
				buffer [i] = bingeti3LE (f) / 256;   /* BUG: truncation; not ideal. */
			break;
		case Melder_LINEAR_32_BIG_ENDIAN:
			for (i = 0; i < n; i ++)
				buffer [i] = bingeti4 (f) / 65536;   /* BUG: truncation; not ideal. */
			break;
		case Melder_LINEAR_32_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++)
				buffer [i] = bingeti4LE (f) / 65536;   /* BUG: truncation; not ideal. */
			break;
		case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
			for (i = 0; i < n; i ++)
				buffer [i] = bingetr4 (f) * 32768;   /* BUG: truncation; not ideal. */
			break;
		case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++)
				buffer [i] = bingetr4LE (f) * 32768;   /* BUG: truncation; not ideal. */
			break;
		case Melder_MULAW:
			for (i = 0; i < n; i ++)
				buffer [i] = ulaw2linear [bingetu1 (f)];
			break;
		case Melder_ALAW:
			for (i = 0; i < n; i ++)
				buffer [i] = alaw2linear [bingetu1 (f)];
			break;
		default:
			return Melder_error ("(Melder_readAudioToShort:) Unknown encoding %d.", encoding);
	}
end:
	if (feof (f)) Melder_warning ("Audio file too short. Missing samples were set to zero.");
	if (ferror (f)) return Melder_error ("(Melder_readAudioToShort:) Error reading audio samples from file.");
	return 1;
}

int MelderFile_writeShortToAudio (MelderFile file, int numberOfChannels, int encoding, const short *buffer, long numberOfSamples) {
	FILE *f = file -> filePointer;
	long n = numberOfSamples * numberOfChannels, start = 0, step = 1, i;
	if (numberOfChannels < 0) {
		n = numberOfSamples * 2;   /* stereo */
		step = 2;   /* Only one channel will be  written. */
		if (numberOfChannels == -2) {
			start = 1;   /* Right channel. */
		}
		numberOfChannels = 1;
	}
	switch (encoding) {
		case Melder_LINEAR_8_SIGNED:
			for (i = start; i < n; i += step)
				binputi1 (buffer [i] >> 8, f);
		break; case Melder_LINEAR_8_UNSIGNED:
			for (i = start; i < n; i += step)
				binputu1 ((buffer [i] >> 8) + 128, f);
		break; case Melder_LINEAR_16_BIG_ENDIAN:
			for (i = start; i < n; i += step)
				binputi2 (buffer [i], f);
		break; case Melder_LINEAR_16_LITTLE_ENDIAN:
			for (i = start; i < n; i += step)
				binputi2LE (buffer [i], f);
		break; case Melder_LINEAR_24_BIG_ENDIAN:
			for (i = start; i < n; i += step)
				binputi3 (buffer [i] << 8, f);
		break; case Melder_LINEAR_24_LITTLE_ENDIAN:
			for (i = start; i < n; i += step)
				binputi3LE (buffer [i] << 8, f);
		break; case Melder_LINEAR_32_BIG_ENDIAN:
			for (i = start; i < n; i += step)
				binputi4 (buffer [i] << 16, f);
		break; case Melder_LINEAR_32_LITTLE_ENDIAN:
			for (i = start; i < n; i += step)
				binputi4LE (buffer [i] << 16, f);
		break; case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
			for (i = start; i < n; i += step)
				binputr4 (buffer [i] / 32768.0, f);
		break; case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
			for (i = start; i < n; i += step)
				binputr4LE (buffer [i] / 32768.0, f);
		break; case Melder_FLAC_COMPRESSION:
			if (! file -> flacEncoder)
				return Melder_error ("(MelderFile_writeShortToAudio:) FLAC encoder not initialized.");
			for (i = start; i < n; i += step * numberOfChannels) {
				FLAC__int32 samples [2];
				samples [0] = buffer [i];
				if (numberOfChannels > 1)
					samples [1] = buffer [i + 1];
				if (! FLAC__stream_encoder_process_interleaved (file -> flacEncoder, samples, 1))
					return Melder_error ("(MelderFile_writeShortToAudio:) Error encoding FLAC stream.");
			}
		break; case Melder_MULAW: case Melder_ALAW: default:
			return Melder_error ("(MelderFile_writeShortToAudio:) Unknown encoding %d.", encoding);
	}
	if (feof (f)) return Melder_error ("(MelderFile_writeShortToAudio:) Early end of audio file.");
	if (ferror (f)) return Melder_error ("(MelderFile_writeShortToAudio:) Error writing audio samples to file.");
	return 1;
}

int MelderFile_writeFloatToAudio (MelderFile file, int encoding, const float *left, long nleft, const float *right, long nright, int warnIfClipped) {
	FILE *f = file -> filePointer;
	long n = nleft > nright ? nleft : nright, i, nclipped = 0;
	switch (encoding) {
		case Melder_LINEAR_8_SIGNED:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					long value = floor (left [i] * 128 + 0.5);
					if (value < -128) { value = -128; nclipped ++; }
					if (value > 127) { value = 127; nclipped ++; }
					binputi1 (value, f);
				} else {
					binputi1 (0, f);
				}
				if (right) {
					if (i < nright) {
						long value = floor (right [i] * 128 + 0.5);
						if (value < -128) { value = -128; nclipped ++; }
						if (value > 127) { value = 127; nclipped ++; }
						binputi1 (value, f);
					} else {
						binputi1 (0, f);
					}
				}
			}
			break;
		case Melder_LINEAR_8_UNSIGNED:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					long value = floor ((left [i] + 1.0) * 128);
					if (value < 0) { value = 0; nclipped ++; }
					if (value > 255) { value = 255; nclipped ++; }
					binputu1 (value, f);
				} else {
					binputu1 (128, f);
				}
				if (right) {
					if (i < nright) {
						long value = floor ((right [i] + 1.0) * 128);
						if (value < 0) { value = 0; nclipped ++; }
						if (value > 255) { value = 255; nclipped ++; }
						binputu1 (value, f);
					} else {
						binputu1 (128, f);
					}
				}
			}
			break;
		case Melder_LINEAR_16_BIG_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					long value = floor (left [i] * 32768 + 0.5);
					if (value < -32768) { value = -32768; nclipped ++; }
					if (value > 32767) { value = 32767; nclipped ++; }
					binputi2 (value, f);
				} else {
					binputi2 (0, f);
				}
				if (right) {
					if (i < nright) {
						long value = floor (right [i] * 32768 + 0.5);
						if (value < -32768) { value = -32768; nclipped ++; }
						if (value > 32767) { value = 32767; nclipped ++; }
						binputi2 (value, f);
					} else {
						binputi2 (0, f);
					}
				}
			}
			break;
		case Melder_LINEAR_16_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					long value = floor (left [i] * 32768 + 0.5);
					if (value < -32768) { value = -32768; nclipped ++; }
					if (value > 32767) { value = 32767; nclipped ++; }
					binputi2LE (value, f);
				} else {
					binputi2LE (0, f);
				}
				if (right) {
					if (i < nright) {
						long value = floor (right [i] * 32768 + 0.5);
						if (value < -32768) { value = -32768; nclipped ++; }
						if (value > 32767) { value = 32767; nclipped ++; }
						binputi2LE (value, f);
					} else {
						binputi2LE (0, f);
					}
				}
			}
			break;
		case Melder_LINEAR_24_BIG_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					long value = floor (left [i] * 8388608 + 0.5);
					if (value < -8388608) { value = -8388608; nclipped ++; }
					if (value > 8388607) { value = 8388607; nclipped ++; }
					binputi3 (value, f);
				} else {
					binputi3 (0, f);
				}
				if (right) {
					if (i < nright) {
						long value = floor (right [i] * 8388608 + 0.5);
						if (value < -8388608) { value = -8388608; nclipped ++; }
						if (value > 8388607) { value = 8388607; nclipped ++; }
						binputi3 (value, f);
					} else {
						binputi3 (0, f);
					}
				}
			}
			break;
		case Melder_LINEAR_24_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					long value = floor (left [i] * 8388608 + 0.5);
					if (value < -8388608) { value = -8388608; nclipped ++; }
					if (value > 8388607) { value = 8388607; nclipped ++; }
					binputi3LE (value, f);
				} else {
					binputi3LE (0, f);
				}
				if (right) {
					if (i < nright) {
						long value = floor (right [i] * 8388608 + 0.5);
						if (value < -8388608) { value = -8388608; nclipped ++; }
						if (value > 8388607) { value = 8388607; nclipped ++; }
						binputi3LE (value, f);
					} else {
						binputi3LE (0, f);
					}
				}
			}
			break;
		case Melder_LINEAR_32_BIG_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					double value = floor (left [i] * 2147483648.0 + 0.5);
					if (value < -2147483648.0) { value = -2147483648.0; nclipped ++; }
					if (value > 2147483647.0) { value = 2147483647.0; nclipped ++; }
					binputi4 (value, f);
				} else {
					binputi4 (0, f);
				}
				if (right) {
					if (i < nright) {
						double value = floor (right [i] * 32768 + 0.5);
						if (value < -2147483648.0) { value = -2147483648.0; nclipped ++; }
						if (value > 2147483647.0) { value = 2147483647.0; nclipped ++; }
						binputi4 (value, f);
					} else {
						binputi4 (0, f);
					}
				}
			}
			break;
		case Melder_LINEAR_32_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					double value = floor (left [i] * 2147483648.0 + 0.5);
					if (value < -2147483648.0) { value = -2147483648.0; nclipped ++; }
					if (value > 2147483647.0) { value = 2147483647.0; nclipped ++; }
					binputi4LE (value, f);
				} else {
					binputi4LE (0, f);
				}
				if (right) {
					if (i < nright) {
						double value = floor (right [i] * 2147483648.0 + 0.5);
						if (value < -2147483648.0) { value = -2147483648.0; nclipped ++; }
						if (value > 2147483647.0) { value = 2147483647.0; nclipped ++; }
						binputi4LE (value, f);
					} else {
						binputi4LE (0, f);
					}
				}
			}
			break;
		case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					double value = left [i];
					binputr4 (value, f);
				} else {
					binputr4 (0, f);
				}
				if (right) {
					if (i < nright) {
						double value = right [i];
						binputr4 (value, f);
					} else {
						binputr4 (0, f);
					}
				}
			}
			break;
		case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
			for (i = 0; i < n; i ++) {
				if (i < nleft) {
					double value = left [i];
					binputr4LE (value, f);
				} else {
					binputr4LE (0, f);
				}
				if (right) {
					if (i < nright) {
						double value = right [i];
						binputr4LE (value, f);
					} else {
						binputr4LE (0, f);
					}
				}
			}
			break;
		case Melder_FLAC_COMPRESSION:
			if (! file -> flacEncoder)
				return Melder_error ("(MelderFile_writeFloatToAudio:) FLAC encoder not initialized.");
			for (i = 0; i < n; i ++) {
				FLAC__int32 samples [2];
				if (i < nleft) {
					long value = floor (left [i] * 32768 + 0.5);
					if (value < -32768) { value = -32768; nclipped ++; }
					if (value > 32767) { value = 32767; nclipped ++; }
					samples [0] = (FLAC__int32) value;
				}
				else
					samples [0] = 0;
				if (right) {
					if (i < nright) {
						long value = floor (right [i] * 32768 + 0.5);
						if (value < -32768) { value = -32768; nclipped ++; }
						if (value > 32767) { value = 32767; nclipped ++; }
						samples [1] = (FLAC__int32) value;
					}
					else
						samples [1] = 0;
				}
				if (! FLAC__stream_encoder_process_interleaved (file -> flacEncoder, samples, 1))
					return Melder_error ("(MelderFile_writeFloatToAudio:) Error encoding FLAC stream.");
			}
			break;
		case Melder_MULAW:
		case Melder_ALAW:
		default:
			return Melder_error ("(MelderFile_writeFloatToAudio:) Unknown format.");
	}
	if (nclipped > 0 && warnIfClipped)
		Melder_warning ("(MelderFile_writeFloatToAudio:) %ld out of %ld samples have been clipped.\n"
			"Advice: you could scale the amplitudes or write to a binary file.", nclipped, n);
	return 1;
}

/* End of file melder_audiofiles.c */
