READ_ME.TXT

djmw 20120130

Espeak Version 1.46

The Espeak program and its library use the espeak-data directory as a supply for the data the
synthesizer needs. The synthesizer needs the location of this directory to work correctly. The synthesizer's
version and the espeak-data version have to match.
This is not exceptable in Praat since we don't want these potential mismatches between the internal version
of the synthesizer and the external espeak-data directory to occur at all.
We have therefore "removed" espeak's dependency on the external espeak-data directory and we have moved all
the data to memory.
This means that some of the espeak code had to be modified a little bit to accomplish this.


***** Step 1:

Use the (private) script "espeakdata_to_code.praat to
1. copy the necessary files from the current espeak distribution to the espeak-work/espeak directory
2. copy files espeakdata_FileInMemory.cpp, espeakdata_FileInMemory.h and Makefile from espeak-work to /espeak-work/espeak
3. Create the files
  - espeakdata_phons.cpp // phondata, phonindex, phontab, intonations
  - espeakdata_dicts.cpp // all **_dict files
  - espeakdata_voices.cpp // all voices in voices/ and subdirs except voices/mb and voices/!v
  - espeakdata_variants.cpp // all files in voices/!v
4. remove the __cdecl from compiledict.cpp and voices.cpp


***** Step 2 Modify some espeak files

>> Adapt speech.h *******

#include "espeakdata_FileInMemory.h"

#undef INCLUDE_MBROLA
#undef PLATFORM_POSIX
#undef PLATFORM_WINDOWS
#undef USE_NANOSLEEP
#define DATA_FROM_SOURCECODE_FILES

#ifdef _WIN32
	wchar_t * Melder_peekUtf8ToWcs (const char *string);
	const uint16_t * Melder_peekWcsToUtf16 (const wchar_t *string);
#endif

>> voices.cpp : voice_t *LoadVoice(const char *vname, int control)

#ifdef DATA_FROM_SOURCECODE_FILES
	long numberOfBytes;
	const char * data;
	if (tone_only) {
		data = espeakdata_get_voiceVariant (vname, &numberOfBytes);
	} else {
		language_type = "en";
		data = espeakdata_get_voice (vname, &numberOfBytes);
		language_type = vname;
	}
	if (data == 0) {
		language_type = "en";    // default
		data = espeakdata_get_voice ("en/en", &numberOfBytes);
	}
#else
...
endif

#ifdef DATA_FROM_SOURCECODE_FILES
	long index = 1;
	const char *start = data;
	while (start = espeakdata_get_voicedata (start, numberOfBytes, buf, sizeof(buf), &index)) {
#else
	while((f_voice != NULL) && (fgets_strip(buf,sizeof(buf),f_voice) != NULL)) {
#endif

>> synthdata : int LoadPhData()

#ifdef DATA_FROM_SOURCECODE_FILES
	long llength;
	phoneme_tab_data = (unsigned char *) FilesInMemory_getData (espeakdata_phons, L"phontab", &llength);
	phoneme_index = (USHORT *) FilesInMemory_getData (espeakdata_phons, L"phonindex", &llength);
	phondata_ptr = (char *) FilesInMemory_getData (espeakdata_phons, L"phondata", &llength);
	tunes = (TUNE *) FilesInMemory_getData (espeakdata_phons, L"intonations", &llength);
	length = llength;
#else
	if((phoneme_tab_data = (unsigned char *)ReadPhFile((void *)(phoneme_tab_data),"phontab",NULL)) == NULL)
		return(-1);
	if((phoneme_index = (USHORT *)ReadPhFile((void *)(phoneme_index),"phonindex",NULL)) == NULL)
		return(-1);
	if((phondata_ptr = ReadPhFile((void *)(phondata_ptr),"phondata",NULL)) == NULL)
		return(-1);
	if((tunes = (TUNE *)ReadPhFile((void *)(tunes),"intonations",&length)) == NULL)
		return(-1);
#endif

void FreePhData(void)
{//==================
#ifndef DATA_FROM_SOURCECODE_FILES
	Free(phoneme_tab_data);
	Free(phoneme_index);
	Free(phondata_ptr);
	Free(tunes);
#endif
	phoneme_tab_data=NULL;
	phoneme_index=NULL;
	phondata_ptr=NULL;
	tunes=NULL;
}

>> speak_lib.cpp


#ifdef DATA_FROM_SOURCECODE_FILES
static void init_path(const char *path) {
	(void) path;
}
#else
static void init_path(const char *path)
{//====================================
#ifdef PLATFORM_WINDOWS

near line 464: sleep is not a WIN64 function, but luckily we don't need it.
// sleep(1)

>> dictionary.cpp

#ifdef DATA_FROM_SOURCECODE_FILES
int LoadDictionary(Translator *tr, const char *name, int no_error)
{
	strcpy (dictionary_name, name);   // currently loaded dictionary name
	strcpy (tr -> dictionary_name, name);

	// Load a pronunciation data file into memory
	// bytes 0-3:  offset to rules data
	// bytes 4-7:  number of hash table entries

	if(tr -> data_dictlist != NULL) {
		Free (tr -> data_dictlist);
		tr -> data_dictlist = NULL;
	}
	unsigned int size;
	tr -> data_dictlist = (char *) espeakdata_get_dict_data (name, &size);
	if (tr -> data_dictlist == 0) {
		return 1;
	}
	int *pw = reinterpret_cast<int *> (tr -> data_dictlist);
	int length = Reverse4Bytes (pw[1]); // was int really written with 4 bytes?

	if (size <= (N_HASH_DICT + sizeof(int)*2)) {
		Melder_error_ (L"Empty _dict: ", Melder_utf8ToWcs(name), L"_dict.");
		return(2);
	}

	if((Reverse4Bytes(pw[0]) != N_HASH_DICT) || (length <= 0) || (length > 0x8000000)) {
		Melder_error_ (L"Bad data in dict: ", Melder_utf8ToWcs(name), L" ", Melder_integer (Reverse4Bytes(pw[0])), L" ", Melder_integer (length));
		return (2);
	}
	tr -> data_dictrules = &(tr->data_dictlist[length]);

	// set up indices into data_dictrules
	InitGroups(tr);
	if (tr -> groups1[0] == NULL) {
		Melder_error_ (L"Error in ", Melder_peekUtf8ToWcs (name), L"_rules, no default rule group.");
	}

	// set up hash table for data_dictlist
	char *p = &(tr -> data_dictlist[8]);

	for (int hash = 0; hash < N_HASH_DICT; hash++) {
		tr -> dict_hashtab[hash] = p;
		while ((length = *p) != 0) {
			p += length;
		}
		p++;   // skip over the zero which terminates the list for this hash value
	}

	return (0);
}  //  end of LoadDictionary

#else
int LoadDictionary_old(Translator *tr, const char *name, int no_error)


************ klatt.cpp  ***************

replace the gen_noise with the corrected version:
static double gen_noise(double noisedummy) // repaired ppgb 20111223
{
        long temp;
        static double nlast = 0.0;

        temp = (long) getrandom (-8191, 8191);
        kt_globals.nrand = (long) temp;

        double noise = kt_globals.nrand + (0.75 * nlast);
        nlast = noise;

        return(noise);
}

???
template <size_t _align_to, typename T> T *align_address (T *p)
{
	union {
		T* ptr;
		size_t integer;
	};
	const size_t bit_mask = ~(_align_to - 1);
	ptr = p;
	Melder_assert (sizeof (size_t) == sizeof (void *));
	integer &= bit_mask;
	return ptr;
}
