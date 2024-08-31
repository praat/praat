/*
	The config file typically contains lots of stuff that is specific to the machine on which eSpeak is compiled.
	However, in Praat we relegate all of this to `melder.h`.

	This file should be included before any other include file, in order to prevent definition clashes.

	In the Praat sources, this file should be included only in `.cpp` files inside `src/external/espeak`.
	Inclusion elsewhere in Praat can lead to clashes (e.g., `USE_ASYNC` undergoes an `#ifdef` in `mad`).
*/

#define PACKAGE_VERSION  "1.52-dev"

#define USE_ASYNC  0
#define USE_LIBPCAUDIO  0
#define USE_SPEECHPLAYER  0

#define USE_KLATT  1
#define USE_MBROLA  0

/*
	Integration into Praat
*/
#define DATA_FROM_SOURCECODE_FILES  1
#define PATH_ESPEAK_DATA  "./data"   /* a relative path */
