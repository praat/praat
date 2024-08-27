"How to integrate eSpeak into Praat"
David Weenink 20120117, 20120124, 20151130, 20171004, 20211207, 20231009
Paul Boersma 20240824: deleted compiledata.c from espeakfiles_c$

This script is `generate/espeak/GENERATE.praat` in the Praat source distribution.
It looks like a Praat notebook and can indeed be dry-run as such,
but it is actually meant as a Praat script from which you copy–paste
`bash` lines into a terminal window and run Praat chunks with "Run selection".

1. Getting the eSpeak sources
=============================

The espeak-ng program and its library are the successor of espeak 
(espeak was maintained by Jonathan Duddington). Espeak-ng is a fork 
maintained by Reece H. Dunn.

If you download the eSpeak code for the first time, you can do this by downloading or cloning the sources from upstream,
namely from `https://github.com/espeak-ng/espeak-ng`, in one of two ways.

If you just need the sources and don’t plan to send bug fixes to the eSpeak team,
you can just download the ZIP file and unpack it,
in e.g. `support/external/eSpeak`, giving a folder `support/external/eSpeak/espeak-ng-master`,
which you then rename to the appropriate version number,
which you can find in the second line of `support/external/eSpeak/espeak-ng-master/configure.ac`.
If the eSpeak version number is 1.52,
the resulting folder `support/external/eSpeak/espeak-ng/espeak-ng-1.52` takes up 66.5 MB.

Alternatively, you %clone the Git repository:
{;
	cd support/external/eSpeak
	git clone https://github.com/espeak-ng/espeak-ng.git
	mv espeak-ng espeak-ng-1.52
}
This way, the folder `support/external/eSpeak/espeak-ng/espeak-ng-1.52` will be 152.1 MB large.
The difference between the two ways of creating the folder is due to the existance of the `.git` file,
which takes up 85.7 MB.

2. Building eSpeak language dictionaries
========================================

The eSpeak sources as downloaded above are not complete. We will have to build the dictionaries.
If you can build Praat itself, you have already installed a compiler, and compiler tools (`make`, `pkg-config`).
Also install `automake`, `autoconf` and `libtool`.
{;
	./autogen.sh
	./configure
	make
}
This creates in `espeak-ng-data` the 117 files `XXX_dict` (the voices were already there),
as well as `phontab`, `phondata`, `phondata-manifest`, `phonindex`, and `intonations`.
Next to these 122 files in `espeak-ng-data`, there are 104 voices in the subfolder `voices/!v`,
and 131 files in the subfolder `lang`, for a total of 360 files (version downloaded 2024-08-25).

If you have the Git version, you can establish updates via
{;
	cd support/external/eSpeak
	git pull https://github.com/espeak-ng/espeak-ng.git
	make clean
	make
}

3. Copying eSpeak data to the Praat sources
===========================================

Copy the files in `support/external/eSpeak/espeak-ng-1.52/espeak-ng-data`
to `src/generate/espeak/espeak-ng-data` and its subfolders `lang` and `voices`,
for instance by just copying the whole folder and deleting the subfolders `mbrola_ph` and `voices/mb`.

In espeak-ng, the espeak-ng-data directory is used to supply the 
data the synthesizer needs. The synthesizer needs the location of 
this directory to work correctly. The synthesizer's version and the 
espeak-ng-data version have to match.
This scheme is not acceptable in Praat since we cannot expect users to maintain
a separate folder for the eSpeak data.
Therefore we convert the 360 data files into source code files that contain FileInMemory objects,
inside `src/generate/espeak`. The eSpeak source code will have to be converted to accomplish this.

As the license of eSpeak is GPL 3 or later, our derived FileInMemory objects are distributed under GPL 3 or later as well:
{
procedure saveFileInMemorySetAsCppFile: .name$
	.fileName$ = "create_" + .name$ + ".cpp"
	.head$ =
	... "/* " + .fileName$ + newline$ +
	... " *" + newline$ +
	... " * This file was automatically created from files in the folder `generate/espeak/espeak-ng-data`" + newline$ +
	... " * by the script `generate/espeak/GENERATE.praat` in the Praat source distribution." + newline$ +
	... " *" + newline$ +
	... " * Espeak-ng version: 1.52-dev August 2023" + newline$ +
	... " * File creation date: " + date$ () + newline$ +
	... " *" + newline$ +
	... " * Copyright (C) 2005-2014 Jonathan Duddington (for eSpeak)" + newline$ +
	... " * Copyright (C) 2015-2023 Reese Dunn (for eSpeak-NG)" + newline$ +
	... " * Copyright (C) 2012-2024 David Weenink, 2024 Paul Boersma (for Praat)" + newline$ +
	... " *" + newline$ +
	... " * This program is free software; you can redistribute it and/or modify " + newline$ +
	... " * it under the terms of the GNU General Public License as published by" + newline$ +
	... " * the Free Software Foundation; either version 3 of the License, or" + newline$ +
	... " * (at your option) any later version." + newline$ +
	... " *" + newline$ +
	... " * This program is distributed in the hope that it will be useful," + newline$ +
	... " * but WITHOUT ANY WARRANTY; without even the implied warranty of" + newline$ +
	... " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" + newline$ +
	... " * GNU General Public License for more details." + newline$ +
	... " *" + newline$ +
	... " * You should have received a copy of the GNU General Public License" + newline$ +
	... " * along with this program; if not, see: <http://www.gnu.org/licenses/>." + newline$ +
	... " */" + newline$ + newline$ +
	... “#include "espeakdata_FileInMemory.h"” + newline$
	.code$ = Show as code: .name$, 30
	.tail$ = "/* End of file " + .fileName$ + " */" + newline$
	writeFile: .fileName$, .head$, .code$, .tail$
endproc
}

Create FileInMemory objects for the five `phon` files.

%Hack: we have to make five sets of one, because `Create FileInMemory...` turns the relative path
into an absolute path, while `Create FileInMemorySet from directory contents...`
retains the relative path, which is what we want.

{
procedure generationFolders
	generationFolder$ = "."   ; the folder of this script, i.e. `generate/espeak` in the Praat sources
	generationDataFolder$ = generationFolder$ + "/espeak-ng-data"
	generationLanguageFolder$ = generationDataFolder$ + "/lang"
	generationVoicesFolder$ = generationDataFolder$ + "/voices/!v"
	generationSourceFolder$ = generationFolder$ + "/src"
endproc
}

{;
	@generationFolders
	fim1 = Create FileInMemorySet from directory contents: "fim1", generationDataFolder$, "*phondata"
	fim2 = Create FileInMemorySet from directory contents: "fim2", generationDataFolder$, "*phonindex"
	fim3 = Create FileInMemorySet from directory contents: "fim3", generationDataFolder$, "*phontab"
	fim4 = Create FileInMemorySet from directory contents: "fim4", generationDataFolder$, "*intonations"
	fim5 = Create FileInMemorySet from directory contents: "fim5", generationDataFolder$, "*phondata-manifest"
	selectObject: fim1, fim2, fim3, fim4, fim5
	Merge
	Rename: "phon"
	removeObject: fim1, fim2, fim3, fim4, fim5
}
Create FileInMemory objects for the 117 dictionary files, with 23,013,656 bytes in total.
{;
	@generationFolders
	Create FileInMemorySet from directory contents: "dicts", generationDataFolder$, "*_dict"
	numberOfDicts = Get number of files
	assert numberOfDicts = 117
}
In order to ensure compilability with 32-bit compilers, split this set up into Russian and the rest.
{;
	Remove files: "contains", "/ru_dict"   ; this extracts the removed dicts!!
	Rename: "russianDict"
	numberOfRussianDicts = Get number of files
	assert numberOfRussianDicts = 1
	selectObject: "FileInMemorySet dicts"
	Rename: "nonRussianDicts"
	numberOfNonRussianDicts = Get number of files
	assert numberOfNonRussianDicts = 116
}
Create FileInMemory objects for the 134 language files. Three language files are at the top level
of the `lang` folder:
{;
	@generationFolders
	Create FileInMemorySet from directory contents: "languages", generationLanguageFolder$, "*"
	numberOfLanguages = Get number of files
	assert numberOfLanguages = 3
}
The remaining 134 language files are in subfolders:
{;
	@generationFolders
	languageFolders$# = folderNames$# (generationLanguageFolder$ + "/*")
	for languageFolder to size (languageFolders$#)
		languageFolder$ = languageFolders$# [languageFolder]
		languageFiles = Create FileInMemorySet from directory contents: "language", generationLanguageFolder$ + "/" + languageFolder$ , "*"
		plusObject: "FileInMemorySet languages"
		merged = Merge
		# should be: Merge second into first
		removeObject: languageFiles, "FileInMemorySet languages"
		selectObject: merged
		Rename: "languages"
	endfor
	numberOfLanguages = Get number of files
	assert numberOfLanguages = 137
}
Create FileInMemory objects for the 104 voice files.
{;
	@generationFolders
	Create FileInMemorySet from directory contents: "voices", generationVoicesFolder$, "*"
	numberOfVoices = Get number of files
	assert numberOfVoices = 104
}
Combine all FileInMemorySets, except Russian, into one:
{;
	selectObject: "FileInMemorySet phon", "FileInMemorySet nonRussianDicts", "FileInMemorySet languages", "FileInMemorySet voices"
	Merge
	Rename: "everythingExceptRussianDict"
}
We safely save these into the generation folder:
{;
	@generationFolders
	selectObject: "FileInMemorySet russianDict"
	@saveFileInMemorySetAsCppFile: "espeak_ng_FileInMemorySet__ru"
	selectObject: "FileInMemorySet everythingExceptRussianDict"
	@saveFileInMemorySetAsCppFile: "espeak_ng_FileInMemorySet"
}
After some inspection for correctness, %move (not %copy, in order to save space)
these files into `src/external/espeak`.

4. Copying eSpeak source code to the Praat sources
==================================================

The following files are in `support/external/eSpeak/espeak-ng-1.52/src`:
{
procedure sources
	srcRootFolder$ = "../../../support/external/eSpeak/espeak-ng-1.52/src"

	sourceFolder$ = srcRootFolder$ + "/libespeak-ng"
	sourceFiles$# = { "common.c", "compiledict.c", "compilembrola.c", "dictionary.c",
	... "encoding.c", "error.c", "espeak_api.c", "espeak_command.c",
	... "event.c", "fifo.c", "ieee80.c", "intonation.c",
	... "klatt.c", "langopts.c", "mnemonics.c", "numbers.c", "phoneme.c", "phonemelist.c",
	... "readclause.c", "setlengths.c", "soundicon.c", "spect.c", "speech.c", "ssml.c",
	... "synthdata.c", "synthesize.c", "synth_mbrola.c", "translate.c", "translateword.c", "tr_languages.c",
	... "voices.c", "wavegen.c" }
	headerFiles$# = { "common.h", "compiledict.h", "dictionary.h",
	... "error.h", "espeak_command.h", "event.h", "fifo.h", "intonation.h",
	... "klatt.h", "langopts.h", "mnemonics.h", "mbrola.h", "numbers.h", "phoneme.h", "phonemelist.h",
	... "readclause.h", "setlengths.h", "sintab.h", "soundicon.h", "spect.h", "speech.h", "ssml.h",
	... "synthdata.h", "synthesize.h", "translate.h", "translateword.h",
	... "voice.h", "wavegen.h" }

	includeFolder$ = srcRootFolder$ + "/include/espeak-ng"
	includeFiles$# = { "encoding.h", "espeak_ng.h", "speak_lib.h" }

	ucdSourceFolder$ = srcRootFolder$ + "/ucd-tools/src"
	ucdSourceFiles$# = { "case.c", "categories.c", "proplist.c" }

	ucdHeaderFolder$ = srcRootFolder$ + "/ucd-tools/src/include/ucd"
	ucdHeaderFiles$# = { "ucd.h" }
endproc
}
All of these are moved to `generate/espeak/src`, without any recursion, while renaming `c` to `cpp` files:
{;
	@generationFolders
	@sources
	for i to size (sourceFiles$#)
		text$ = readFile$: sourceFolder$ + "/" + sourceFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + sourceFiles$# [i] + "pp", text$
	endfor
	for i to size (headerFiles$#)
		text$ = readFile$: sourceFolder$ + "/" + headerFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + headerFiles$# [i], text$
	endfor
	for i to size (includeFiles$#)
		text$ = readFile$: includeFolder$ + "/" + includeFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + includeFiles$# [i], text$
	endfor
	for i to size (ucdSourceFiles$#)
		text$ = readFile$: ucdSourceFolder$ + "/" + ucdSourceFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + ucdSourceFiles$# [i] + "pp", text$
	endfor
	for i to size (ucdHeaderFiles$#)
		text$ = readFile$: ucdHeaderFolder$ + "/" + ucdHeaderFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + ucdHeaderFiles$# [i], text$
	endfor
}

Also a new file espeak-ng-version.h is created [TO BE EDITED]:
	# check version
	.config$ = readFile$ ("~/projects/espeak-ng/config.h")
	.version$ = extractWord$ (.config$, "#define VERSION """)
	.version$ = replace$ (.version$, """", "", 0)
	if .version$ <> espeak_version$
		exitScript: "The given version (", espeak_version$, ") differs from package version ", .version$
	endif

	.version_define$ = "#define ESPEAK_NG_VERSION " + "U""" + espeak_version$ + """" + newline$
		... + "#define ESPEAK_NG_VERSIONX " + espeak_version$ + newline$
	writeFile: todir$ + "/espeak_ng_version.h", .version_define$

5. Adapting eSpeak source code to Praat
=======================================

Many changes to the sources have to be made. If the original eSpeak sources don't change to much,
this can be done by a visual `diff` between for instance `src/generate/espeak/src/speech.cpp` 
and `src/external/espeak/speech.cpp`. The former is the one to be changed (in the way below).
Once all the source and header files are fine, they can be moved over the ones in `src/external/espeak`.
After this, the folder `src/generate/espeak/src` will be empty again.

[TO BE EDITED]:
#	6. In B:
#		Change all includes eg <espeak-ng/something.h> to "something.h"
#		Cast many return values and error values
#		Adapt the file reading parts: surround the definitions of the procedures GetFileLength()
#		and GetVoices() with
#		#if ! DATA_FROM_SOURCECODE_FILES
#		#endif
# 	7. in error.cpp replace writing to stderr with calls to Melder_throw()
#	8. in dictionary.cpp, soundicon.cpp, speech.cpp, synthdata.cpp, voices.cpp
#		#include "espeak_io.h", and prepend "espeak_io_" to all calls of fopen, fclose, feof,
#		fseek, ftell, fgets, fread, fgetc, fprintf, ungetc, GetFileLength and GetVoices.
#       Use listFileIO.praat to list the occurrences of these names in those five files.
#	9. delete all "#pragma GCC visibility" lines

***** (only once)

Clone the git  repository 
./autogen.sh
CC=gcc CFLAGS="-Werror=missing-prototypes -Werror=implicit -Wreturn-type -Wunused -Wunused-parameter -Wuninitialized" ./configure --prefix=/usr

Now we can be up-to-date by pulling.

**** 
We have replaced the file io based on fopen, fclose, fgets etc... with our own io (see espeak_io.cpp)

We inserted a number of explicit casts:
static_cast<espeak_ng_STATUS> (errno)

Adapted some of the header files.

The overaching header file is `espeak_ng.h`,
so any #defines that should be global to espeak should be defined in `espeak_ng.h`.

In `espeak_ng.h`, we should ignore all the `dllexport` and `dllimport` labels:
	//ppgb #if defined(_WIN32) || defined(_WIN64)
	//ppgb #ifdef LIBESPEAK_NG_EXPORT
	//ppgb #define ESPEAK_NG_API __declspec(dllexport)
	//ppgb #else
	//ppgb #define ESPEAK_NG_API __declspec(dllimport)
	//ppgb #endif
	//ppgb #else
	#define ESPEAK_NG_API
	//ppgb #endif

In `speak_ng.h`, we make sure that `DATA_FROM_SOURCECODE_FILES` is true by default:
	//ppgb:
	#ifndef DATA_FROM_SOURCECODE_FILES
		#define DATA_FROM_SOURCECODE_FILES  1
	#endif
This is because the compilation in Praat should be the default,
whereas compilation in the environment of actually existing data files
should be restricted to special debugging cases
(compile with `-DDATA_FROM_SOURCECODE_FILES=0` in that case).

In `speak_ng.h`, we replace the Windows-specific part
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

In `speech.cpp`, in the function `espeak_ng_Initialize`, we remove `setlocale`,
because the locale of the entire program shouldn't be overwritten by a library:
	/*
		(Paul Boersma 20240426:)
		When using this library in an app, e.g. Praat,
		we should not set the locale, because it will interfere with the locale
		that has been set in praat_init().
		To nevertheless experiment with setting the locale here,
		remove the space from "set locale" in the definition of SET_LOCALE
		and set USE_SET_LOCALE_IN_THIS_LIBRARY to 1 instead of 0.
		(The space is needed to be able to automatically determine that the name
		 of the function does not appear in the present source file `speech.cpp`.)
	*/
	#define SET_LOCALE  set locale
	#define USE_SET_LOCALE_IN_THIS_LIBRARY  0
	#if USE_SET_LOCALE_IN_THIS_LIBRARY
		if (SET_LOCALE(LC_CTYPE, "C.UTF-8") == NULL) {
			if (SET_LOCALE(LC_CTYPE, "UTF-8") == NULL) {
				if (SET_LOCALE(LC_CTYPE, "en_US.UTF-8") == NULL)
					SET_LOCALE(LC_CTYPE, "");
			}
		}
	#endif
However, eSpeak does need Unicode knowledge to work correctly,
so that iswalpha and all other isw* functions should be replaced
with iswalpha_portable and so on.

There is a bug in wavegen.cpp, where it reads
	MarkerEvent(marker_type, q[1], q[2], q[3], out_ptr);
The problem here is that q[2] and q[3] are of type intptr_t,
whereas MarkerEvent expects int arguments. This is no problem
on 32-bit systems, but on 64-bit systems the higher 4 bytes of q[2] are discarded.
On little-endian systems, these higher 4 bytes are the last 4 bytes,
and these rarely contain any relevant phoneme codes, so the problem rarely surfaces;
however, on big-endian systems the higher 4 bytes are the first 4 bytes,
so that all phoneme codes are in effect lost. A correct version is:
	MarkerEvent(marker_type, q[1], * (int *) & q[2], * ((int *) & q[2] + 1), out_ptr);
On 64-bit systems this splits up q[2] into two ints (in correct order),
and on 32-bit systems * ((int *) & q[2] + 1) is actually q[3], which is also correct.
MarkerEvent() itself pastes the two ints together again, in correct order.

Finally, make sure not to include `<windows.h>` or `melder.h`
after `espeak_ng.h`, because they may redefine `fopen`.

#include "speak_lib.h"
#include "encoding.h"
#include "ucd.h"

#undef INCLUDE_MBROLA
#undef PLATFORM_POSIX
#undef PLATFORM_WINDOWS
#undef USE_NANOSLEEP

