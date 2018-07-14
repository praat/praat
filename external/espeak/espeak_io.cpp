/* espeak_io.cpp
 *
//  * Copyright (C) 2017 David Weenink
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

/*
	djmw 20171024
*/

#include "espeakdata_FileInMemory.h"
#include "espeak_ng.h"
#include "speech.h"
#include "synthesize.h"
#include <errno.h>

extern autoFileInMemoryManager espeak_ng_FileInMemoryManager;
#define ESPEAK_FILEINMEMORYMANAGER espeak_ng_FileInMemoryManager.get()

FILE *espeak_io_fopen (const char * filename, const char * mode) {
	return FileInMemoryManager_fopen (ESPEAK_FILEINMEMORYMANAGER, filename, mode);
}

void espeak_io_rewind (FILE *stream) {
	FileInMemoryManager_rewind (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_fclose (FILE *stream) {
	return FileInMemoryManager_fclose (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_feof (FILE *stream) {
	return FileInMemoryManager_feof (ESPEAK_FILEINMEMORYMANAGER, stream);
}

long espeak_io_ftell (FILE *stream) {
	return FileInMemoryManager_ftell (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_fseek (FILE *stream, long offset, int origin) {
	return FileInMemoryManager_fseek (ESPEAK_FILEINMEMORYMANAGER, stream, offset, origin);
}

char *espeak_io_fgets (char *str, int num, FILE *stream) {
	return FileInMemoryManager_fgets (ESPEAK_FILEINMEMORYMANAGER, str, num, stream);
}

size_t espeak_io_fread (void *ptr, size_t size, size_t count, FILE *stream) {
	return FileInMemoryManager_fread (ESPEAK_FILEINMEMORYMANAGER, ptr, size, count, stream);
}

int espeak_io_fgetc (FILE *stream) {
	return FileInMemoryManager_fgetc (ESPEAK_FILEINMEMORYMANAGER, stream);
}

int espeak_io_fprintf (FILE * stream, ... ) {
	va_list arg;
	va_start (arg, stream);
	char *format = static_cast<char *> (va_arg (arg, void*));
	int result = FileInMemoryManager_fprintf (ESPEAK_FILEINMEMORYMANAGER, stream, format, arg);
	va_end (arg);
	return result;
}

int espeak_io_ungetc (int character, FILE * stream) {
	return FileInMemoryManager_ungetc (ESPEAK_FILEINMEMORYMANAGER, character,stream);
}
/* This mimics GetFileLength of espeak-ng */
int FileInMemoryManager_GetFileLength (FileInMemoryManager me, const char *filename) {
		integer index = FileInMemorySet_lookUp (my files.get(), Melder_peek8to32(filename));
		if (index > 0) {
			FileInMemory fim = static_cast<FileInMemory> (my files -> at [index]);
			return fim -> d_numberOfBytes;
		}
		// Directory ??
		if (FileInMemorySet_hasDirectory (my files.get(), Melder_peek8to32(filename))) {
			return -EISDIR;
		}
		return -1;
}

/* 
	espeak_io_GetFileLength: mimics GetFileLength of espeak-ng
	Returns the number of bytes in the file.
	If the filename is a directory it return -EISDIR
*/
int espeak_io_GetFileLength (const char *filename) {
	FileInMemorySet me = ESPEAK_FILEINMEMORYMANAGER -> files.get();
	integer index = FileInMemorySet_lookUp (me, Melder_peek8to32(filename));
	if (index > 0) {
		FileInMemory fim = static_cast<FileInMemory> (my at [index]);
		return fim -> d_numberOfBytes;
	}
	// Directory ??
	if (FileInMemorySet_hasDirectory (me, Melder_peek8to32(filename))) {
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
	FileInMemoryManager me = ESPEAK_FILEINMEMORYMANAGER;
	conststring32 criterion = is_language_file ? U"/lang/" : U"/voices/";
	autoFileInMemorySet fileList = FileInMemorySet_listFiles (my files.get(), kMelder_string :: CONTAINS, criterion);
	for (long ifile = 1; ifile <= fileList -> size; ifile ++) {
		FileInMemory fim = static_cast<FileInMemory> (fileList -> at [ifile]);
		FILE *f_voice = FileInMemoryManager_fopen (me, Melder_peek32to8 (fim -> d_path.get()), "r");
		conststring8 fname = Melder_peek32to8 (fim -> d_path.get());
		espeak_VOICE *voice_data = ReadVoiceFile (f_voice, fname + len_path_voices, is_language_file);
		FileInMemoryManager_fclose (me, f_voice);
		if (voice_data) {
			voices_list [n_voices_list ++] = voice_data;
		} /*else {
			Melder_warning (U"Voice data for ", fname, U" could not be gathered.");
		}*/
	}
}

int get_int32_le (char *ch) {
	return (((uint8)ch[0]<<0) | ((uint8)ch[1]<<8) | ((uint8)ch[2]<<16) | ((uint8)ch[3]<<24));
}

short get_int16_le (char *ch) {
       return (((uint8)ch[0]<<0) | ((uint8)ch[1]<<8));
}

int get_set_int32_le (char *ch) {
       int i32 = (((uint8)ch[0]<<0) | ((uint8)ch[1]<<8) | ((uint8)ch[2]<<16) | ((uint8)ch[3]<<24));
       int *p32 = (int *) ch;
       *p32 = i32;
       return i32;
}

/*
	The espeak-ng data files have been written with little endian byte order. To be able to use these files on big endian hardware we have to change these files as if they were written on a big endian machine.
	The following routines were modeled after espeak-phonemedata.c by Jonathan Duddington.
	A serious bug in his code for the phontab_to_bigendian procedure has been corrected.
	A better solution would be:
		espeak-ng should read a little endian int32 as 4 unsigned bytes:
			int32 i = (ch[0]<<0) | (ch[1]<<8) | (ch[2]<<16) | (ch[3]<<24);
		a int16 (short) as 2 unsigned bytes:
			int16 i = (ch[0]<<0) | (ch[1]<<8);
		Then no conversion of data files would be necessary.

*/

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define SWAP_2(i1) { integer i2 = i1 + 1; \
		thy d_data [i1] = my d_data [i2]; \
		thy d_data [i2] = my d_data [i1]; }

	#define SWAP_4(i1) { integer i2 = i1 + 1, i3 = i1 + 2, i4 = i1 + 3; \
		thy d_data [i1] = my d_data [i4]; \
		thy d_data [i2] = my d_data [i3]; \
		thy d_data [i3] = my d_data [i2]; \
		thy d_data [i4] = my d_data [i1]; }
#else
	#define SWAP_2(i1)
	#define SWAP_4(i1)
#endif

static autoFileInMemory phondata_to_bigendian (FileInMemory me, FileInMemory manifest) {
	try {
		autoFileInMemory thee = Data_copy (me);
		FILE *phondataf = fopen (Melder_peek32to8 (my d_path.get()), "r");
		FILE *manifestf = fopen (Melder_peek32to8 (manifest -> d_path.get()), "r");
		char line [1024];
		// copy 4 bytes: version number
		// copy 4 bytes: sample rate
		while (fgets (line, sizeof (line), manifestf)) {
			if (! isupper (line [0])) continue;
			unsigned int index;
			sscanf(& line [2], "%x", & index);
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
				index += 2; // skip the short length
				integer numberOfFrames = my d_data [index]; // unsigned char n_frames
				index += 2; // skip the 2 unsigned char's n_frames & sqflags
				
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
					// 
					#define FRFLAG_KLATT 0x01
					index += (thy d_data [i1] & FRFLAG_KLATT) ? sizeof (frame_t) : sizeof (frame_t2); // thy is essential!
				}				
			} else if (line [0] == 'W') { // Wave data
				int length = my d_data [i1 + 1] * 256 + my d_data [i1];
				index += 4;
				
				index += length; // char wavedata[length]
				
				index += index % 3;
				
			} else if (line [0] == 'E') {
				
				index += 128; // Envelope: skip 128 bytes
				
				
			} else if (line [0] == 'Q') {
				unsigned int length = my d_data [index + 2] << 8 + my d_data [index + 3];
				length *= 4;
				
				index += length;
			}
			Melder_require (index <= my d_numberOfBytes, U"Position ", index, U"is larger than file length (", my d_numberOfBytes, U")."); 
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"phondata not converted to bigendian.");
	}
}

static autoFileInMemory phontab_to_bigendian (FileInMemory me) {
	try {
		autoFileInMemory thee = Data_copy (me);
		integer numberOfPhonemeTables = my d_data [0];
		integer index = 4; // skip first 4 bytes
		for (integer itab = 1; itab <= numberOfPhonemeTables; itab ++) {
			integer numberOfPhonemes = thy d_data [index];
			
			index += 4; // This is 8 (incorrect) in the original code of espeak.
			
			index += N_PHONEME_TAB_NAME; // skip the name
			integer phonemeTableSizes = numberOfPhonemes * sizeof (PHONEME_TAB);
			Melder_require (index + phonemeTableSizes <= my d_numberOfBytes, U"Too many tables to process. (table ", itab, U" from ", numberOfPhonemeTables, U").");
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
		return thee;
	} catch (MelderError) {
		Melder_throw (U"phontab not converted to bigendian.");
	}
}
	
static autoFileInMemory phonindex_to_bigendian (FileInMemory me) {
	try {
		autoFileInMemory thee = Data_copy (me);
		integer numberOfShorts = (my d_numberOfBytes - 4 - 1) / 2;
		integer index = 4; // skip first 4 bytes
		for (integer i = 0; i < numberOfShorts; i ++) {
			SWAP_2 (index)
			index += 2;
			Melder_require (index <= my d_numberOfBytes, U"Position ", index, U" is larger than file length (", my d_numberOfBytes, U").");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"phonindex not converted to bigendian.");
	}
}

void espeak_ng_data_to_bigendian () {
	FileInMemoryManager me = ESPEAK_FILEINMEMORYMANAGER;
	autoMelderString file;

	MelderString_append (& file, Melder_peek8to32 (PATH_ESPEAK_DATA), U"/phondata-manifest");
	integer index = FileInMemorySet_lookUp (my files.get(), file.string);
	Melder_require (index > 0, U"phondata-manifest not present.");
	FileInMemory manifest = (FileInMemory) my files -> at [index];

	MelderString_empty (& file);
	MelderString_append (& file, Melder_peek8to32 (PATH_ESPEAK_DATA), U"/phondata");
	index = FileInMemorySet_lookUp (my files.get(), file.string);
	Melder_require (index > 0, U"phondata not present.");
	FileInMemory phondata = (FileInMemory) my files -> at [index];

	autoFileInMemory phondata_new = phondata_to_bigendian (phondata, manifest);
	my files -> replaceItem_move (phondata_new.move(), index);

	MelderString_empty (& file);
	MelderString_append (& file, Melder_peek8to32 (PATH_ESPEAK_DATA), U"/phontab");
	index = FileInMemorySet_lookUp (my files.get(), file.string);
	Melder_require (index > 0, U"phonindex not present.");
	FileInMemory phontab = (FileInMemory) my files -> at [index];

	autoFileInMemory phontab_new = phontab_to_bigendian (phontab);	
	my files -> replaceItem_move (phontab_new.move(), index);

	MelderString_empty (& file);
	MelderString_append (& file, Melder_peek8to32 (PATH_ESPEAK_DATA), U"/phonindex");
	index = FileInMemorySet_lookUp (my files.get(), file.string);
	Melder_require (index > 0, U"phonindex not present.");
	FileInMemory phonindex = (FileInMemory) my files -> at [index];

	autoFileInMemory phonindex_new = phonindex_to_bigendian (phonindex);	
	my files -> replaceItem_move (phonindex_new.move(), index);
}

/* End of file espeak_io.cpp */
