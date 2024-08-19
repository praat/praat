/* espeak_io.cpp
 *
//  * Copyright (C) 2017-2021 David Weenink, 2024 Paul Boersma
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

#include "espeakdata_FileInMemory.h"
#include "espeak_ng.h"
#include "speech.h"
#include "synthesize.h"
#include "voice.h"
#include <errno.h>
#include "espeak_io.h"   // include *after* system includes

extern autoFileInMemoryManager espeak_ng_FileInMemoryManager;

FILE *espeak_io_fopen (const char * filename, const char * mode) {
	return FileInMemoryManager_fopen (espeak_ng_FileInMemoryManager.get(), filename, mode);
}

void espeak_io_rewind (FILE *stream) {
	FileInMemoryManager_rewind (espeak_ng_FileInMemoryManager.get(), stream);
}

int espeak_io_fclose (FILE *stream) {
	return FileInMemoryManager_fclose (espeak_ng_FileInMemoryManager.get(), stream);
}

int espeak_io_feof (FILE *stream) {
	return FileInMemoryManager_feof (espeak_ng_FileInMemoryManager.get(), stream);
}

long espeak_io_ftell (FILE *stream) {
	return FileInMemoryManager_ftell (espeak_ng_FileInMemoryManager.get(), stream);
}

int espeak_io_fseek (FILE *stream, long offset, int origin) {
	return FileInMemoryManager_fseek (espeak_ng_FileInMemoryManager.get(), stream, offset, origin);
}

char *espeak_io_fgets (char *str, int num, FILE *stream) {
	return FileInMemoryManager_fgets (espeak_ng_FileInMemoryManager.get(), str, num, stream);
}

size_t espeak_io_fread (void *ptr, size_t size, size_t count, FILE *stream) {
	return FileInMemoryManager_fread (espeak_ng_FileInMemoryManager.get(), ptr, size, count, stream);
}

int espeak_io_fgetc (FILE *stream) {
	return FileInMemoryManager_fgetc (espeak_ng_FileInMemoryManager.get(), stream);
}

int espeak_io_fprintf (FILE * stream, ... ) {
	va_list arg;
	va_start (arg, stream);
	char *format = static_cast<char *> (va_arg (arg, void*));
	int result = FileInMemoryManager_fprintf (espeak_ng_FileInMemoryManager.get(), stream, format, arg);
	va_end (arg);
	return result;
}

int espeak_io_ungetc (int character, FILE * stream) {
	return FileInMemoryManager_ungetc (espeak_ng_FileInMemoryManager.get(), character,stream);
}

/* 
	espeak_io_GetFileLength: mimics GetFileLength of espeak-ng
	Returns the number of bytes in the file.
	If the filename is a directory it return -EISDIR
*/
int espeak_io_GetFileLength (const char *filename) {
	FileInMemorySet me = espeak_ng_FileInMemoryManager -> files.get();
	integer index = FileInMemorySet_lookUp (me, Melder_peek8to32 (filename));
	if (index > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my at [index]);
		return fim -> d_numberOfBytes;
	}
	// Directory ??
	if (FileInMemorySet_hasDirectory (me, Melder_peek8to32 (filename))) {
		return -EISDIR;
	}
	return -1;
}

/* 
	espeak_io_GetVoices: mimics GetVoices of espeak-ng
	If is_languange_file == 0 then /voices/ else /lang/ 
	We know our voices are in /voices/ and our languages in /lang/
*/
void espeak_io_GetVoices (const char *path, int len_path_voices, int is_language_file) {
	(void) path;
	/*
		if is_languange_file == 0 then /voices/ else /lang/ 
		We know our voices are in /voices/!v/ and our languages in /lang/
	*/
	FileInMemoryManager me = espeak_ng_FileInMemoryManager.get();
	conststring32 criterion = is_language_file ? U"/lang/" : U"/voices/";
	autoFileInMemorySet fileList = FileInMemorySet_listFiles (my files.get(), kMelder_string :: CONTAINS, criterion);
	for (long ifile = 1; ifile <= fileList -> size; ifile ++) {
		FileInMemory fim = static_cast<FileInMemory> (fileList -> at [ifile]);
		FILE *f_voice = FileInMemoryManager_fopen (me, Melder_peek32to8_fileSystem (fim -> d_path.get()), "r");
		conststring8 fname = Melder_peek32to8_fileSystem (fim -> d_path.get());
		espeak_VOICE *voice_data = ReadVoiceFile (f_voice, fname + len_path_voices, is_language_file);
		FileInMemoryManager_fclose (me, f_voice);
		if (voice_data) {
			voices_list [n_voices_list ++] = voice_data;
		} /*else {
			Melder_warning (U"Voice data for ", fname, U" could not be gathered.");
		}*/
	}
}

int32_t get_int32_le (char *ch) {
	return (((uint8)ch[0]<<0) | ((uint8)ch[1]<<8) | ((uint8)ch[2]<<16) | ((uint8)ch[3]<<24));
}

int32_t get_int32_be (char *ch) {
	return (((uint8)ch[3]<<0) | ((uint8)ch[2]<<8) | ((uint8)ch[1]<<16) | ((uint8)ch[0]<<24));
}

short get_int16_le (char *ch) {
       return (((uint8)ch[0]<<0) | ((uint8)ch[1]<<8));
}

int32_t get_set_int32_le (char *ch) {
       int32_t i32 = (((uint8)ch[0]<<0) | ((uint8)ch[1]<<8) | ((uint8)ch[2]<<16) | ((uint8)ch[3]<<24));
       int32_t *p32 = (int32_t *) ch;
       *p32 = i32;
       return i32;
}

/*
	The espeak-ng data files have been written with little-endian byte order. To be able to use these files on big endian hardware
	we have to change these files as if they were written on a big-endian machine.
	The following routines were modeled after espeak-phonemedata.c by Jonathan Duddington.
	A serious bug in his code for the phontab_to_bigendian procedure has been corrected.
	A better solution would be:
		espeak-ng should read a little endian int32 as 4 unsigned bytes:
			int32 i = (ch[0]<<0) | (ch[1]<<8) | (ch[2]<<16) | (ch[3]<<24);
		and an int16 (short) as 2 unsigned bytes:
			int16 i = (ch[0]<<0) | (ch[1]<<8);
		Then no conversion of data files would be necessary.
*/

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define SWAP_2(i) { \
		thyData [i] = myData [i + 1]; \
		thyData [i + 1] = myData [i]; }

	#define SWAP_4(i) { \
		thyData [i] = myData [i + 3]; \
		thyData [i + 1] = myData [i + 2]; \
		thyData [i + 2] = myData [i + 1]; \
		thyData [i + 3] = myData [i]; }
#else
	#define SWAP_2(i)
	#define SWAP_4(i)
#endif

static autoFileInMemory phondata_to_bigendian (FileInMemory me, FileInMemory manifest) {
	TRACE
	try {
		autoFileInMemory thee = Data_copy (me);
		const uint8 * const myData = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		uint8 * const thyData = thy d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		FILE *phondataf = fopen (Melder_peek32to8_fileSystem (my d_path.get()), "r");
		Melder_assert (phondataf);
		FILE *manifestf = fopen (Melder_peek32to8_fileSystem (manifest -> d_path.get()), "r");
		Melder_assert (manifestf);
		char line [1024];
		// copy 4 bytes: version number
		// copy 4 bytes: sample rate
		integer totalLengthOfComments = 0, totalLengthOfS = 0, totalLengthOfW = 0, totalLengthOfE = 0;
		while (fgets (line, sizeof (line), manifestf)) {
			trace (line [0]);
			if (line [0] == '#') {   // comment
				totalLengthOfComments += strlen (line);
				continue;
			}
			unsigned int index;
			sscanf (& line [2], "%x", & index);
			trace (U"index ", index, U" (", Melder_hexadecimal (index, 8), U")");
			fseek (phondataf, index, SEEK_SET);
			integer i1 = index;
			if (line [0] == 'S') { //
				/*
					typedef struct {
						short length;
						unsigned char n_frames;
						unsigned char sqflags;
						frame_t frame[N_SEQ_FRAMES];
					} SPECT_SEQ;
				*/

				SWAP_2 (i1)
				index += 2;   // skip the short length
				const integer numberOfFrames = myData [index];   // unsigned char n_frames
				trace (U"S ", numberOfFrames, U" frames");
				index += 2;   // skip the 2 unsigned char's n_frames & sqflags
				totalLengthOfS += 4;

				for (integer n = 1; n <= numberOfFrames; n ++) {
					/*
						typedef struct { //64 bytes
							short frflags;
							short ffreq[7];
							unsigned char length;
							unsigned char rms;
							unsigned char fheight[8];
							unsigned char fwidth[6];          // width/4  f0-5
							unsigned char fright[3];          // width/4  f0-2
							unsigned char bw[4];        // Klatt bandwidth BNZ /2, f1,f2,f3
							unsigned char klattp[5];    // AV, FNZ, Tilt, Aspr, Skew
							unsigned char klattp2[5];   // continuation of klattp[],  Avp, Fric, FricBP, Turb
							unsigned char klatt_ap[7];  // Klatt parallel amplitude
							unsigned char klatt_bp[7];  // Klatt parallel bandwidth  /2
							unsigned char spare;        // pad to multiple of 4 bytes
						} frame_t;   //  with extra Klatt parameters for parallel resonators

						typedef struct {  // 44 bytes
							short frflags;
							short ffreq[7];
							unsigned char length;
							unsigned char rms;
							unsigned char fheight[8];
							unsigned char fwidth[6];          // width/4  f0-5
							unsigned char fright[3];          // width/4  f0-2
							unsigned char bw[4];        // Klatt bandwidth BNZ /2, f1,f2,f3
							unsigned char klattp[5];    // AV, FNZ, Tilt, Aspr, Skew
						} frame_t2;
						Both frame_t and frame_t2 start with 8 short's.
					*/
					i1 = index;
					for (integer i = 1; i <= 8; i ++) {
						SWAP_2 (i1)
						i1 += 2;
					}
					/*
						We will be stepping over a frame_t or frame_t2.
						The code below assumes that one can step over such a structure by
						progressing `sizeof (frame_t)` or `sizeof (frame_t2)` bytes,
						but `sizeof` is about alignment, not about size per se,
						so we check here that the two are equal. If they are ever different,
						we will have to step by size instead of alignment
						(we will be notified by Praat crashing at start-up; last checked 2024-08-16).
					*/
					Melder_assert (sizeof (frame_t) == 64);
					Melder_assert (sizeof (frame_t2) == 44);
					/*
						frflags signals whether the frame is a Klatt frame or not
						20231105 changed thy d_data [i1] to thy d_data [index + 1];
						20240816 changed thy d_data to my d_data, and then to myData [index]
					*/
					#define FRFLAG_KLATT 0x01
					uint32 length = (myData [index] & FRFLAG_KLATT) ? sizeof (frame_t) : sizeof (frame_t2);
					trace (U"S length ", length);
					index += length;
					totalLengthOfS += length;
				}
			} else if (line [0] == 'W') {  // Wave data
				uint32 length = ((uint32) myData [i1 + 1] << 8) + (uint32) myData [i1];
				index += 4;
				index += length;   // char wavedata [length]
				/*
					Round up to a multiple of 4.
				*/
				const integer lengthW = 4 + length + (3 - (index - 1) % 4);
				index += 3 - (index - 1) % 4;
				trace (U"W length ", lengthW, U" (", length, U")");
				totalLengthOfW += lengthW;
				/*
					Some W chunks, namely most of those for Myanmar and Vietnam,
					are longer than this (in the manifest file), namely 3000 to 4000 bytes or so.
				*/
			} else if (line [0] == 'E') {
				index += 128;   // Envelope: skip 128 bytes
				totalLengthOfE += 128;
			} else if (line [0] == 'Q') {
				/*
					The following code is probably incorrect as well,
					but we never seem to reach it.
				*/
				uint32 length = ((uint32) myData [index + 2] << 8) + (uint32) myData [index + 1];   // NOT TESTABLE
				length *= 4;
				index += length;
			} else
				Melder_fatal (U"Unknown tag ", line [0], U" in phondata_to_bigendian().");
			Melder_require (index < my d_numberOfBytes ||1,
				U"Position ", index + 1, U" is larger than file length (", my d_numberOfBytes, U").");
		}
		trace (U"Total length of W ", totalLengthOfW, U" S ", totalLengthOfS, U" E ", totalLengthOfE, U" comments ", totalLengthOfComments);
		trace (U"Together ", totalLengthOfW + totalLengthOfS + totalLengthOfE + totalLengthOfComments,
				U" in a file of ", my d_numberOfBytes, U" bytes.");   // they don't add up; see the comment at case 'W'
		return thee;
	} catch (MelderError) {
		Melder_throw (U"phondata not converted to bigendian.");
	}
}

static autoFileInMemory phontab_to_bigendian (FileInMemory me) {
	TRACE
	try {
		autoFileInMemory thee = Data_copy (me);
		const uint8 * const myData = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		uint8 * const thyData = thy d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		const integer numberOfPhonemeTables = myData [0];
		trace (numberOfPhonemeTables, U" phoneme tables");
		integer index = 4; // skip first 4 bytes
		for (integer itab = 1; itab <= numberOfPhonemeTables; itab ++) {
			integer numberOfPhonemes = thyData [index];

			index += 4;   // this is 8 (incorrect) in the original code of eSpeak

			index += N_PHONEME_TAB_NAME; // skip the name
			const integer phonemeTableSizes = numberOfPhonemes * (integer) sizeof (PHONEME_TAB);
			Melder_require (index + phonemeTableSizes <= my d_numberOfBytes,
				U"Too many tables to process. (table ", itab, U" from ", numberOfPhonemeTables, U").");
			for (integer j = 1; j <= numberOfPhonemes; j ++) {
				/*
					typedef struct { // 16 bytes
						unsigned int  mnemonic;      // 1st char is in the l.s.byte
						unsigned int  phflags;       // bits 16-19 place of articulation
						unsigned short program;
						unsigned char  code;         // the phoneme number
						unsigned char  type;         // phVOWEL, phPAUSE, phSTOP etc
						unsigned char  start_type;
						unsigned char  end_type;
						unsigned char  std_length;   // for vowels, in mS/2;  for phSTRESS, the stress/tone type
						unsigned char  length_mod;   // a length_mod group number, used to access length_mod_tab
					} PHONEME_TAB;
				*/
				integer i1 = index;
				SWAP_4 (i1)				
				i1 += 4;
				SWAP_4 (i1);
				i1 += 4;
				SWAP_2 (i1)
				index += sizeof (PHONEME_TAB);
			}
			Melder_require (index <= my d_numberOfBytes, U"Position ", index, U" is larger than file length (", my d_numberOfBytes, U").");
		}
		trace (U"Ending up at index ", index, U" in a file with ", my d_numberOfBytes, U" bytes.");
		Melder_assert (index == my d_numberOfBytes);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"phontab not converted to bigendian.");
	}
}
	
static autoFileInMemory phonindex_to_bigendian (FileInMemory me) {
	TRACE
	try {
		autoFileInMemory thee = Data_copy (me);
		const uint8 * const myData = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		uint8 * const thyData = thy d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		const integer numberOfShorts = (my d_numberOfBytes - 4) / 2;   // there used to be an extra "- 1" here (ppgb 20240817)
		integer index = 4; // skip first 4 bytes
		for (integer i = 0; i < numberOfShorts; i ++) {
			SWAP_2 (index)
			index += 2;
			Melder_require (index <= my d_numberOfBytes, U"Position ", index, U" is larger than file length (", my d_numberOfBytes, U").");
		}
		trace (U"Ending up at index ", index, U" in a file with ", my d_numberOfBytes, U" bytes.");
		Melder_assert (index == my d_numberOfBytes);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"phonindex not converted to bigendian.");
	}
}

void espeak_ng_data_to_bigendian () {
	FileInMemoryManager me = espeak_ng_FileInMemoryManager.get();

	const integer manifestIndex = FileInMemorySet_lookUp (my files.get(), U"" PATH_ESPEAK_DATA "/phondata-manifest");
	Melder_assert (manifestIndex > 0);
	const FileInMemory manifest = (FileInMemory) my files -> at [manifestIndex];
	// no conversion to big-endian is necessary, because `phondata-manifest` is a text file
	// we do need manifest here, for converting phondata

	const integer phondataIndex = FileInMemorySet_lookUp (my files.get(), U"" PATH_ESPEAK_DATA "/phondata");
	Melder_assert (phondataIndex > 0);
	const FileInMemory phondata = (FileInMemory) my files -> at [phondataIndex];
	autoFileInMemory phondata_new = phondata_to_bigendian (phondata, manifest);
	Thing_swap (phondata, phondata_new.get());   // not `my files -> replaceItem_move (...)`, so as not to dangle `my openFiles`!

	const integer phontabIndex = FileInMemorySet_lookUp (my files.get(), U"" PATH_ESPEAK_DATA "/phontab");
	Melder_assert (phontabIndex > 0);
	const FileInMemory phontab = (FileInMemory) my files -> at [phontabIndex];
	autoFileInMemory phontab_new = phontab_to_bigendian (phontab);
	Thing_swap (phontab, phontab_new.get());

	const integer phonindexIndex = FileInMemorySet_lookUp (my files.get(), U"" PATH_ESPEAK_DATA "/phonindex");
	Melder_assert (phonindexIndex > 0);
	const FileInMemory phonindex = (FileInMemory) my files -> at [phonindexIndex];
	autoFileInMemory phonindex_new = phonindex_to_bigendian (phonindex);
	Thing_swap (phonindex, phonindex_new.get());
}

/* End of file espeak_io.cpp */
