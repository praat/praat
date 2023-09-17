ESPEAK_NG_READ_ME.TXT

djmw 20230915

Espeak-ng Version 1.52-dev

The espeak-ng program and its library are the successor of espeak 
(espeak was maintained by Jonathan Duddington). Espeak-ng is a fork 
maintained by Reece H. Dunn.
We have cloned espeak-ng's git repository with the command 
'git clone https://github.com/espeak-ng/espeak-ng.git'.

In espeak-ng, the espeak-ng-data directory is used to supply the 
data the synthesizer needs. The synthesizer needs the location of 
this directory to work correctly. The synthesizer's version and the 
espeak-ng-data version have to match.
This scheme is not acceptable in Praat since we don't want potential 
mismatches between our version of the synthesizer and the external 
espeak-ng-data directory to occur at all.
We have therefore "removed" espeak-ng's dependency on the external 
espeak-ng-data directory by moving all the data to memory.
This means that some of the espeak code had to be modified a 
little bit to accomplish this.


***** (only once)

Clone the git  repository 
./autogen.sh
CC=gcc CFLAGS="-Werror=missing-prototypes -Werror=implicit -Wreturn-type -Wunused -Wunused-parameter -Wuninitialized" ./configure --prefix=/usr

Now we can be up-to-date by pulling.

**** 

make

Compile all code and dictonaries.

**** 
We have replaced the file io based on fopen, fclose, fgets etc... with our own io (see espeak_io.cpp)

We inserted a number of explicit casts:
static_cast<espeak_ng_STATUS> (errno)

Adapted some of the header files.

In "espeak_ng.h", we should ignore all the dllexport and dllimport labels:
	//ppgb #if defined(_WIN32) || defined(_WIN64)
	//ppgb #ifdef LIBESPEAK_NG_EXPORT
	//ppgb #define ESPEAK_NG_API __declspec(dllexport)
	//ppgb #else
	//ppgb #define ESPEAK_NG_API __declspec(dllimport)
	//ppgb #endif
	//ppgb #else
	#define ESPEAK_NG_API
	//ppgb #endif

In "speak_ng.h", we make sure that DATA_FROM_SOURCECODE_FILES is true by default:
	//ppgb:
	#ifndef DATA_FROM_SOURCECODE_FILES
		#define DATA_FROM_SOURCECODE_FILES  1
	#endif

In "speak_ng.h", we replace the Windows-specific part
	#define PLATFORM_WINDOWS  1
	#define PATHSEP '\\'
with
	#if DATA_FROM_SOURCECODE_FILES
		#define PLATFORM_WINDOWS  0
		#define PATHSEP '/'
	#else
		#define PLATFORM_WINDOWS  1
		#define PATHSEP '\\'
	#endif
#endif
This is because David hard-coded the paths to the data files with forward slashes.

In "speech.cpp", in the function espeak_ng_Initialize, we remove setlocale,
because the locale of the entire program shouldn't be overwritten by a library:
	//ppgb if (set lo ca le(LC_CTYPE, "C.UTF-8") == NULL) {
	//ppgb 	if (set lo ca le(LC_CTYPE, "UTF-8") == NULL) {
	//ppgb 		if (set lo ca le(LC_CTYPE, "en_US.UTF-8") == NULL)
	//ppgb 			set lo ca le(LC_CTYPE, "");
	//ppgb 	}
	//ppgb }
(the inserted spaces are there because the absence of setlocale in this file
 is automatically checked in the Praat test suite)

#include "speak_lib.h"
#include "encoding.h"
#include "ucd.h"

#undef INCLUDE_MBROLA
#undef PLATFORM_POSIX
#undef PLATFORM_WINDOWS
#undef USE_NANOSLEEP

****

More details can be found in the "espeak_ng_data_to_code.praat" file.



