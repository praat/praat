"How to integrate eSpeak into Praat"
David Weenink 20120117, 20120124, 20151130, 20171004, 20211207, 20231009
Paul Boersma 20240824: deleted compiledata.c from espeakfiles_c$

This script is `generate/espeak/GENERATE.praat` in the Praat source distribution.
It looks like a Praat notebook and can indeed be dry-run as such,
but it is actually meant as a Praat script from which you copy–paste
`bash` lines into a terminal window and run Praat chunks with "Run selection".

1. Getting the eSpeak sources
=============================

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

Copy the files in `support/external/eSpeak/espeak-ng-data` to `src/generate/espeak/espeak-ng-data` and its subfolders `lang` and `voices`,
for instance by just copying the whole folder and deleting the subfolders `mbrola_ph` and `voices/mb`.

Praat cannot handle external data files, so we convert the 360 data files into source code files that contain FileInMemory objects,
inside `src/generate/espeak`.

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
these files to 


Go to the folder `src/generate/espeak` and run the following code:

#
# This script is specific for my (David's) situation (although it can be adapted easily to 
# any directory structure and non-Linux system).
# My espeak-work/ has subdirectories espeak-ng-work, espeak-ng-current and espeak-ng-previous.
# Into espeak-ng-work are copied from the original espeak-ng project only those *.c and *.h files 
# that are needed for the praat version of the synthesizer.
# The needed files are specified below in the variables:
# .espeakfiles_h$, .espeakfiles_c$, .espeakfiles_include_h$, 
# .espeakfiles_ucd_h$ and .espeakfiles_ucd_c$
#
# If the upstream espeak-ng has been modified then I follow the following procedure:
#
# First something like (base dir is espeak-work/):
#		cp ../external/espeak/* espeak-ng-previous/ 
#		rm espeak-ng-work/* rm espeak-ng-current/*
#		use this script to cp the necessary *.c and *.h into espeak-ng-work,
#			while renaming the *.c to *.cpp files.

# Given the following directories:
#	A: ~/projects/praat/espeak-work/espeak-ng-work 
#	B: ~/projects/praat/espeak-work/espeak-ng-current
#	C: ~/projects/praat/external/espeak 
#


#	2. I copy the necessary *.c and *.h files from ~/projects/espeak-ng/src/... to espeak-work 
#		by running this script with only the option "Copy_c_and_h_files" ON (the right version option).
#		The *.c files are renamed as *.cpp files. Also a new file espeak-ng-version.h is created.
#	3. Now my *.cpp and *.h are in synchrony with upstream and we copy all files from A to B.
#	4. Generate the new dictionaries and language files in memory by running
#		generate/espeak.createFileInMemorySets.praat.
#	5. Move the files generate/create_espeak_ng_FileInMemorySet.cpp and generate/create_espeak_ng_FileInMemorySet__ru.cpp
#		to external/espeak.
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

# Function of this script:
# Generates in espeak-ng-work
# 	1. create_espeak_ng_FileInMemorySet.cpp
# 	2. espeak-ng-version.h
#
# 	and
#
# 	3. Copies the necessary *.c(pp) and *.h from from_dir to to_dir
#

myscriptname$ = "espeak_ng_data_to_code.praat"
date$ = date$()
notify$ = "This file was created automatically on " + date$ + "."
clearinfo

form Espeakdata to code
	word Espeak_version 1.52-dev
	boolean Copy_c_and_h_files 0
	boolean Show_cp_command 1
	boolean Create_espeak_ng_version.h 1
endform

fromdir$ = "/home/david/projects/espeak-ng"
todir$ = "/home/david/projects/praat/espeak-work/espeak-ng-work"

# extract only the necesary files to espeak-ng-work.
# Subsequent changes to these files have to be made in espeak-ng-current

if copy_c_and_h_files || show_cp_command
	@espeak_ng_copyfiles
endif

procedure espeak_ng_copyfiles
	.espeakfiles_c$ = "common.c compiledict.c compilembrola.c dictionary.c"
	... + " encoding.c error.c espeak_api.c espeak_command.c"
	... + " event.c fifo.c ieee80.c intonation.c klatt.c langopts.c"
	... + " mnemonics.c numbers.c phoneme.c phonemelist.c readclause.c setlengths.c"
	... + " soundicon.c spect.c speech.c synthdata.c synthesize.c "
	... + " ssml.c synth_mbrola.c translate.c translateword.c tr_languages.c voices.c wavegen.c"

	.espeakfiles_h$ = "common.h compiledict.h dictionary.h error.h espeak_command.h event.h fifo.h intonation.h"
	... + " klatt.h langopts.h mbrola.h numbers.h phoneme.h phonemelist.h"
	... + " readclause.h setlengths.h sintab.h spect.h speech.h"
	... + " ssml.h synthdata.h synthesize.h translate.h translateword.h voice.h wavegen.h"
	.espeakfiles_include_h$ = "speak_lib.h espeak_ng.h encoding.h"
	.espeakfiles_ucd_h$ = "ucd.h"
	.espeakfiles_ucd_c$ = "case.c categories.c proplist.c"

	# check version
	.config$ = readFile$ ("~/projects/espeak-ng/config.h")
	.version$ = extractWord$ (.config$, "#define VERSION """)
	.version$ = replace$ (.version$, """", "", 0)
	if .version$ <> espeak_version$
		exitScript: "The given version (", espeak_version$, ") differs from package version ", .version$
	endif

	# rename and cp files from espeak-ng to espeak-work
	@copy_rename: fromdir$+"/src/libespeak-ng", .espeakfiles_c$, ".c", ".cpp"
	@copy_rename: fromdir$+"/src/libespeak-ng", .espeakfiles_h$, ".h", ".h"
	@copy_rename: fromdir$+"/src/include/espeak-ng", .espeakfiles_include_h$, ".h", ".h"
	@copy_rename: fromdir$+"/src/ucd-tools/src", .espeakfiles_ucd_c$, ".c", ".cpp"
	@copy_rename: fromdir$+"/src/ucd-tools/src/include/ucd", .espeakfiles_ucd_h$, ".h", ".h"
	.version_define$ = "#define ESPEAK_NG_VERSION " + "U""" + espeak_version$ + """" + newline$
		... + "#define ESPEAK_NG_VERSIONX " + espeak_version$ + newline$
	if create_espeak_ng_version.h
		writeFile: todir$ + "/espeak_ng_version.h", .version_define$
	endif
endproc

procedure copy_rename: .fromdir$, .files$, .ext$, .newext$
	.filelist$# = splitByWhitespace$# (.files$)
	for .ifile to size (.filelist$#)
		.name$ = .filelist$# [.ifile]
		.newname$ = .name$ - .ext$ + .newext$
		.command$ =  "cp " + .fromdir$ + "/" + .name$ + " " + todir$ + "/" + .newname$
		appendInfoLine: .command$
		if show_cp_command
			appendInfoLine: .command$
		endif
		if copy_c_and_h_files
			runSystem: .command$
		endif
	endfor
endproc

