/* espeak_praat.cpp
 *
//  * Copyright (C) 2017-2021 David Weenink, 2024,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

#include "espeak__config.h"

#include "espeak_praat.h"

#include "NUM2.h"   // for strstr_regexp()

#include "espeak_ng.h"
#include "speech.h"
#include "synthesize.h"
#include "voice.h"
#include <errno.h>

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

static /*constexpr*/ bool nativeIsBigEndian () {   // FIXME: C++-20 will have a constexpr function for this
	const int test = 1;
	if /*constexpr*/ (* ((char *) & test) == 1)
		return false;
	else
		return true;
}

static void littleEndian2ToNativeEndian (uint8 *twoBytes) {
	if (nativeIsBigEndian ())
		std::swap (twoBytes [0], twoBytes [1]);
}

static void littleEndian4ToNativeEndian (uint8 *fourBytes) {
	if (nativeIsBigEndian ()) {
		std::swap (fourBytes [0], fourBytes [3]);
		std::swap (fourBytes [1], fourBytes [2]);
	}
}

static void phondata_makeNativeEndian (FileInMemory me, FileInMemory manifest) {
	//TRACE
	try {
		uint8 *const data = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		FileInMemory phondataf = FileInMemorySet_fopen (theEspeakPraatFileInMemorySet(), Melder_peek32to8_fileSystem (my string.get()), "r");
		Melder_assert (phondataf);
		FileInMemory manifestf = FileInMemorySet_fopen (theEspeakPraatFileInMemorySet(), Melder_peek32to8_fileSystem (manifest -> string.get()), "r");
		Melder_assert (manifestf);
		char line [1024];
		// copy 4 bytes: version number
		// copy 4 bytes: sample rate
		integer totalLengthOfComments = 0, totalLengthOfS = 0, totalLengthOfW = 0, totalLengthOfE = 0;
		while (FileInMemory_fgets (line, sizeof (line), manifestf)) {
			trace (line [0]);
			if (line [0] == '#') {   // comment
				totalLengthOfComments += strlen (line);
				continue;
			}
			unsigned int index;
			sscanf (& line [2], "%x", & index);
			trace (U"index ", index, U" (", Melder_hexadecimal (index, 8), U")");
			FileInMemory_fseek (phondataf, index, SEEK_SET);
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

				littleEndian2ToNativeEndian (& data [i1]);   // nativize `short length`
				index += 2;   // skip `short length`
				const integer numberOfFrames = data [index];   // interpret `unsigned char n_frames`
				trace (U"S ", numberOfFrames, U" frames");
				index += 2;   // skip `unsigned char n_frames` and `unsigned char sqflags`
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
					i1 = index;   // both `i1` and `index` now point to the start of a `frame_t`
					const short frflags = (int16) (uint16) (((uint32) data [i1 + 1] << 8) + (uint32) data [i1]);   // save before swapping!
					for (integer i = 1; i <= 8; i ++) {
						littleEndian2ToNativeEndian (& data [i1]);   // nativize `short frflags` and the seven `short ffreq`s
						i1 += 2;   // skip `short frflags` and the seven `short ffreq`s
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
						20240904 changed myData [index] to data [index] (in order to do the byteswapping in place;
						         however, as `index` still pointed to the start of the frame, where `frflags` was,
						         this was a bug, because `frflags` had already been byteswapped;
						         so we changed data [index] to a saved version of frflags.
					*/
					#define FRFLAG_KLATT 0x01
					uint32 length = (frflags & FRFLAG_KLATT) ? sizeof (frame_t) : sizeof (frame_t2);
					trace (U"S length ", length);
					index += length;
					totalLengthOfS += length;
				}
			} else if (line [0] == 'W') {  // Wave data
				uint32 length = ((uint32) data [i1 + 1] << 8) + (uint32) data [i1];
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
				uint32 length = ((uint32) data [index + 2] << 8) + (uint32) data [index + 1];   // NOT TESTABLE
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
	} catch (MelderError) {
		Melder_throw (U"phondata not converted to bigendian.");
	}
}

static void phontab_makeNativeEndian (FileInMemory me) {
	//TRACE
	try {
		uint8 *const data = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		const integer numberOfPhonemeTables = data [0];
		trace (numberOfPhonemeTables, U" phoneme tables");
		integer index = 4; // skip first 4 bytes
		for (integer itab = 1; itab <= numberOfPhonemeTables; itab ++) {
			integer numberOfPhonemes = data [index];

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
				littleEndian4ToNativeEndian (& data [i1]);   // `unsigned int mnemonic`
				i1 += 4;
				littleEndian4ToNativeEndian (& data [i1]);   // `unsigned int phflags`
				i1 += 4;
				littleEndian2ToNativeEndian (& data [i1]);   // `unsigned short program`
				index += sizeof (PHONEME_TAB);
			}
			Melder_require (index <= my d_numberOfBytes, U"Position ", index, U" is larger than file length (", my d_numberOfBytes, U").");
		}
		trace (U"Ending up at index ", index, U" in a file with ", my d_numberOfBytes, U" bytes.");
		Melder_assert (index == my d_numberOfBytes);
	} catch (MelderError) {
		Melder_throw (U"phontab not converted to bigendian.");
	}
}
	
static void phonindex_makeNativeEndian (FileInMemory me) {
	//TRACE
	try {
		uint8 *const data = my d_data.asArgumentToFunctionThatExpectsZeroBasedArray();
		const integer numberOfShorts = (my d_numberOfBytes - 4) / 2;   // there used to be an extra "- 1" here (ppgb 20240817)
		integer index = 4; // skip first 4 bytes
		for (integer i = 0; i < numberOfShorts; i ++) {
			littleEndian2ToNativeEndian (& data [index]);
			index += 2;
			Melder_require (index <= my d_numberOfBytes, U"Position ", index, U" is larger than file length (", my d_numberOfBytes, U").");
		}
		trace (U"Ending up at index ", index, U" in a file with ", my d_numberOfBytes, U" bytes.");
		Melder_assert (index == my d_numberOfBytes);
	} catch (MelderError) {
		Melder_throw (U"phonindex not converted to bigendian.");
	}
}

static void espeak_praat_FileInMemorySet_makeNativeEndian () {
	FileInMemorySet me = theEspeakPraatFileInMemorySet();

	const integer manifestIndex = my lookUp (U"" PATH_ESPEAK_DATA "/phondata-manifest");
	Melder_assert (manifestIndex > 0);
	const FileInMemory manifest = my at [manifestIndex];
	// no conversion to big-endian is necessary, because `phondata-manifest` is a text file
	// we do need manifest here, for converting phondata

	const integer phondataIndex = my lookUp (U"" PATH_ESPEAK_DATA "/phondata");
	Melder_assert (phondataIndex > 0);
	const FileInMemory phondata = my at [phondataIndex];
	phondata_makeNativeEndian (phondata, manifest);

	const integer phontabIndex = my lookUp (U"" PATH_ESPEAK_DATA "/phontab");
	Melder_assert (phontabIndex > 0);
	const FileInMemory phontab = my at [phontabIndex];
	phontab_makeNativeEndian (phontab);

	const integer phonindexIndex = my lookUp (U"" PATH_ESPEAK_DATA "/phonindex");
	Melder_assert (phonindexIndex > 0);
	const FileInMemory phonindex = my at [phonindexIndex];
	phonindex_makeNativeEndian (phonindex);
}

FileInMemorySet theEspeakPraatFileInMemorySet() {
	static autoFileInMemorySet me;   // singleton
	if (! me) {
		me = FileInMemorySet_create ();
		espeak_praat_FileInMemorySet_addPhon (me.get());
		espeak_praat_FileInMemorySet_addRussianDict (me.get());
		espeak_praat_FileInMemorySet_addFaroeseDict (me.get());
		espeak_praat_FileInMemorySet_addOtherDicts (me.get());
		espeak_praat_FileInMemorySet_addLanguages (me.get());
		espeak_praat_FileInMemorySet_addVoices (me.get());
		if (nativeIsBigEndian ())   // OPTIMIZATION: 1 ms less start-up time
			espeak_praat_FileInMemorySet_makeNativeEndian ();
	}
	return me.get();
}

/* End of file espeak_praat.cpp */
