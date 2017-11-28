# espeak_ng_data_to_code.praat
# djmw 20120117, 20120124, 20151130, 20171004
#
# This script is specific for my situation (althought it can be adapted easily to any directory structure
#	and non-Linux system).
#	My espeak-work/ has subdirectories espeak-ng-work and espeak-ng-current.
# 	Into espeak-ng-work are copied from the original espeak-ng project only those *.c and *.h files that are 
# 	needed for the praat version of the synthesizer .
#
# If the upstream espeak-ng has been modified then I follow the following procedure:
#
# Given the following directories:
#	A: ~/projects/praat/espeak-work/espeak-ng-work 
#	B: ~/projects/praat/espeak-work/espeak-ng-current
#	C: ~/projects/praat/external/espeak 
#
#	0. I pull the sources from upstream:
#		cd ~/projects/espeak-ng
#		- git pull https://github.com/espeak-ng/espeak-ng.git
#		- make clean
#		- make
#		- Get the version of espeak-ng from the ~/projects/espeak-ng/config.h file
#	1. I copy the necessary *.c and *.h files from ~/projects/espeak-ng/src/... to espeak-work 
#		by running this script with only the option "Copy_c_and_h_files" ON (the right version option).
#		The *.c files are renamed as *.cpp files. Also a new file espeak-ng-version.h is created.
#	2. I use a three-way diff program (kdiff3) to see which files in A are different from B and 
#		merge the differences into C.
#	3. Now my *.cpp and *.h are in synchrony with upstream and we copy all files from A to B.
#	4. I generate the new dictionaries and language files in memory by running this script
#		with only the option "Create_FileInMemorySet" set to ON.
#	5. I copy the file create_espeak_ng_FileInMemorySet.cpp 
#  		and espeak-ng-version.h from espeak-ng-work/ to external/espeak
#	6. Now the praat synthesizer is up to date.
# 
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
	word Espeak_version 1.49.3-dev
	boolean Copy_c_and_h_files 0
	boolean Show_cp_command 1
	boolean Create_FileInMemorySet 0
endform

gpltext$ =  " * Copyright (C) David Weenink 2012-2017" + newline$ +
	... " *" + newline$ +
	... " * This program is free software; you can redistribute it and/or modify " + newline$ +
 	... " * it under the terms of the GNU General Public License as published by" + newline$ +
 	... " * the Free Software Foundation; either version 2 of the License, or (at" + newline$ +
 	... " * your option) any later version." + newline$ +
  	... " *" + newline$ +
  	... " * This program is distributed in the hope that it will be useful, but" + newline$ +
 	... " * WITHOUT ANY WARRANTY; without even the implied warranty of" + newline$ +
  	... " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU" + newline$ +
 	... " * General Public License for more details." + newline$ +
 	... " *" + newline$ +
  	... " * You should have received a copy of the GNU General Public License" + newline$ +
  	... " * along with this program; if not, write to the Free Software" + newline$ +
 	... " * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA." + newline$

fromdir$ = "/home/david/projects/espeak-ng"
todir$ = "/home/david/projects/praat/espeak-work/espeak-ng-work"

espeakdata_dir$ = fromdir$ + "/espeak-ng-data"
espeakdata_voices_dir$ = espeakdata_dir$ + "/voices/!v"
espeakdata_lang_dir$ = espeakdata_dir$ + "/lang"

include_header$ = "#include "+ """" + "espeakdata_FileInMemory.h" + """" + newline$

if create_FileInMemorySet

	@create_phonFileInMemorySet

	@create_languageFileInMemorySet

	dict_fims = Create FileInMemorySet from directory contents: "list", espeakdata_dir$, "*_dict"
	
	voice_fims = Create FileInMemorySet from directory contents: "voices", espeakdata_dir$ + "/voices/!v", "*"

	selectObject: language_fims, phon_fims, dict_fims, voice_fims
	espeak_ng_fims = Merge
	
	.message$ = "/* espeak_ng_createFileInMemorySet.cpp" + newline$
	... + " * This file was automatically created from files in directories in espeak-ng-data." + newline$
	... + " * Espeak-ng version: " + espeak_version$ + "." + newline$
	... + " * Date: " + date$() + "." + newline$
	... + "*/" + newline$ +newline$
	... + "#include ""espeakdata_FileInMemory.h""" + newline$
	.cpp$ = Show as code: "espeak_ng_FileInMemorySet", 30
	.cpp$ = .message$ + .cpp$ 
	writeInfoLine: .cpp$
	writeFile: todir$ + "/create_espeak_ng_FileInMemorySet.cpp", .cpp$
endif

procedure create_phonFileInMemorySet
	fim1 = Create FileInMemory: espeakdata_dir$ + "/phondata"
	fim2 = Create FileInMemory: espeakdata_dir$ + "/phonindex"
	fim3 = Create FileInMemory: espeakdata_dir$ + "/phontab"
	fim4 = Create FileInMemory: espeakdata_dir$ + "/intonations"
	fim5 = Create FileInMemory: espeakdata_dir$ + "/phondata-manifest"
	selectObject: fim1, fim2, fim3, fim4, fim5
	phon_fims = To FileInMemorySet
	removeObject: fim1, fim2, fim3, fim4, fim5
endproc

procedure create_languageFileInMemorySet
	language_fims = Create FileInMemorySet from directory contents: "l", espeakdata_lang_dir$, "*"
	.ldirs =  Create Strings as directory list: "dirs",  espeakdata_lang_dir$+ "/*"
	.ndirs = Get number of strings
	for .idir to .ndirs
		selectObject: .ldirs
		.dir$ = Get string: .idir
		.langset = Create FileInMemorySet from directory contents: "l", espeakdata_lang_dir$ + "/" + .dir$ , "*"
		plusObject: language_fims
		.merged = Merge
		removeObject: .langset, language_fims
		language_fims = .merged
	endfor
	removeObject: .ldirs
endproc

# extract only the necesary files to espeak-ng-work.
# changes to these files have to be made in espeak-ng-current

if copy_c_and_h_files
	@espeak_ng_copyfiles
endif

procedure espeak_ng_copyfiles
	.espeakfiles_c$ = "compiledata.c compiledict.c compilembrola.c dictionary.c" +
	... " encoding.c error.c espeak_api.c espeak_command.c" +
	... " event.c fifo.c ieee80.c intonation.c klatt.c" +
	... " mnemonics.c numbers.c phoneme.c phonemelist.c readclause.c setlengths.c" +
	... " spect.c speech.c synthdata.c synthesize.c " +
	... " synth_mbrola.c translate.c tr_languages.c voices.c wavegen.c"

	.espeakfiles_h$ = "error.h espeak_command.h event.h fifo.h klatt.h phoneme.h sintab.h" +
	... " spect.h speech.h synthesize.h translate.h voice.h synthesize.h translate.h voice.h"
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
		... + "#define ESPEAK_NG_VERSIONX " + espeak_version$
	writeFile: todir$ + "/espeak_ng_version.h", .version_define$
endproc

procedure copy_rename: .fromdir$, .files$, .ext$, .newext$
	.list = Create Strings as tokens: .files$
	.numberOfFiles = Get number of strings
	for .ifile to .numberOfFiles
		.name$ = Get string: .ifile
		.newname$ = .name$ - .ext$ + .newext$
		.command$ =  "cp " + .fromdir$ + "/" + .name$ + " " + todir$ + "/" + .newname$
		appendInfoLine: .command$
		if show_cp_command
			appendInfoLine: .command$
		endif
		runSystem: .command$
	endfor
endproc

