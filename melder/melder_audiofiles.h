#ifndef _melder_audiofiles_h_
#define _melder_audiofiles_h_
/* melder_audiofiles.h
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

#pragma mark - AUDIO FILES

/* Audio file types. */
#define Melder_AIFF  1
#define Melder_AIFC  2
#define Melder_WAV  3
#define Melder_NEXT_SUN  4
#define Melder_NIST  5
#define Melder_FLAC 6
#define Melder_MP3 7
#define Melder_NUMBER_OF_AUDIO_FILE_TYPES  7
conststring32 Melder_audioFileTypeString (int audioFileType);   // "AIFF", "AIFC", "WAV", "NeXT/Sun", "NIST", "FLAC", "MP3"
/* Audio encodings. */
#define Melder_LINEAR_8_SIGNED  1
#define Melder_LINEAR_8_UNSIGNED  2
#define Melder_LINEAR_16_BIG_ENDIAN  3
#define Melder_LINEAR_16_LITTLE_ENDIAN  4
#define Melder_LINEAR_24_BIG_ENDIAN  5
#define Melder_LINEAR_24_LITTLE_ENDIAN  6
#define Melder_LINEAR_32_BIG_ENDIAN  7
#define Melder_LINEAR_32_LITTLE_ENDIAN  8
#define Melder_MULAW  9
#define Melder_ALAW  10
#define Melder_SHORTEN  11
#define Melder_POLYPHONE  12
#define Melder_IEEE_FLOAT_32_BIG_ENDIAN  13
#define Melder_IEEE_FLOAT_32_LITTLE_ENDIAN  14
#define Melder_IEEE_FLOAT_64_BIG_ENDIAN  15
#define Melder_IEEE_FLOAT_64_LITTLE_ENDIAN  16
#define Melder_FLAC_COMPRESSION_16 17
#define Melder_FLAC_COMPRESSION_24 18
#define Melder_FLAC_COMPRESSION_32 19
#define Melder_MPEG_COMPRESSION_16 20
#define Melder_MPEG_COMPRESSION_24 21
#define Melder_MPEG_COMPRESSION_32 22
int Melder_defaultAudioFileEncoding (int audioFileType, int numberOfBitsPerSamplePoint);   /* BIG_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN, BIG_ENDIAN, LITTLE_ENDIAN */
void MelderFile_writeAudioFileHeader (MelderFile file, int audioFileType, integer sampleRate, integer numberOfSamples, integer numberOfChannels, int numberOfBitsPerSamplePoint);
void MelderFile_writeAudioFileTrailer (MelderFile file, int audioFileType, integer sampleRate, integer numberOfSamples, integer numberOfChannels, int numberOfBitsPerSamplePoint);
void MelderFile_writeAudioFile (MelderFile file, int audioFileType, const short *buffer, integer sampleRate, integer numberOfSamples, integer numberOfChannels, int numberOfBitsPerSamplePoint);

int MelderFile_checkSoundFile (MelderFile file, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples);
/* Returns information about a just opened audio file.
 * The return value is the audio file type, or 0 if it is not a sound file or in case of error.
 * The data start at 'startOfData' bytes from the start of the file.
 */
int Melder_bytesPerSamplePoint (int encoding);
void Melder_readAudioToFloat (FILE *f, int encoding, MAT buffer);
/* Reads channels into buffer [ichannel], which are base-1.
 */
void Melder_readAudioToShort (FILE *f, integer numberOfChannels, int encoding, short *buffer, integer numberOfSamples);
/* If stereo, buffer will contain alternating left and right values.
 * Buffer is base-0.
 */
void MelderFile_writeFloatToAudio (MelderFile file, constMATVU const& buffer, int encoding, bool warnIfClipped);
void MelderFile_writeShortToAudio (MelderFile file, integer numberOfChannels, int encoding, const short *buffer, integer numberOfSamples);

/* End of file melder_audiofiles.h */
#endif
