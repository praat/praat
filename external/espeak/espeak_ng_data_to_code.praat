# espeak_ng_data_to_code.praat
# djmw 20120117, 20120124, 20151130, 20171004, 20211207, 20231009
#
# This script is specific for my situation (although it can be adapted easily to 
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
#	1. I pull the sources from upstream:
#		cd ~/projects/espeak-ng
#		- git pull https://github.com/espeak-ng/espeak-ng.git
#		- make clean
#		- make
#		- Get the version of espeak-ng from the ~/projects/espeak-ng/config.h file
#	2. I copy the necessary *.c and *.h files from ~/projects/espeak-ng/src/... to espeak-work 
#		by running this script with only the option "Copy_c_and_h_files" ON (the right version option).
#		The *.c files are renamed as *.cpp files. Also a new file espeak-ng-version.h is created.
#	3. Now my *.cpp and *.h are in synchrony with upstream and we copy all files from A to B.
#	4. I generate the new dictionaries and language files in memory by running this script
#		with only the option "Create_FileInMemorySet" set to ON.
#	5. I copy the file create_espeak_ng_FileInMemorySet.cpp 
#  		and espeak-ng-version.h from A to B
#	6. In B:
#		Change all includes eg <espeak-ng/something.h> to "something.h"
#		Cast many return values and error values
#		Adapt the file reading parts: surround the procedures
#		#if ! DATA_FROM_SOURCECODE_FILES
# 		 GetFileLength(...) & GetVoices(...) 
#		#endif
# 	7. in error.c do some Melder_throw's instead of stderr stuf

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
	boolean Create_FileInMemorySet 0
endform

gpltext$ =  " * Copyright (C) David Weenink 2012-2023" + newline$ +
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
	... + "*/" + newline$ + newline$
	... + "#include ""espeakdata_FileInMemory.h""" + newline$
	.cppCode$ = Show as code: "espeak_ng_FileInMemorySet", 30
	.cppCode$ = .message$ + .cppCode$ 
	writeInfoLine: .cppCode$
	writeFile: todir$ + "/create_espeak_ng_FileInMemorySet.cpp", .cppCode$
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
	.ldirs$# = folderNames$# (espeakdata_lang_dir$ + "/*")
	for .idir to size (.ldirs$#)
		.dir$ = .ldirs$# [.idir]
		.langset = Create FileInMemorySet from directory contents: "l", espeakdata_lang_dir$ + "/" + .dir$ , "*"
		plusObject: language_fims
		.merged = Merge
		removeObject: .langset, language_fims
		language_fims = .merged
	endfor
endproc

# extract only the necesary files to espeak-ng-work.
# Subsequent changes to these files have to be made in espeak-ng-current

if copy_c_and_h_files || show_cp_command
	@espeak_ng_copyfiles
endif

procedure espeak_ng_copyfiles
	.espeakfiles_c$ = "common.c compiledata.c compiledict.c compilembrola.c dictionary.c"
	... + " encoding.c error.c espeak_api.c espeak_command.c"
	... + " event.c fifo.c ieee80.c intonation.c klatt.c langopts.c"
	... + " mnemonics.c numbers.c phoneme.c phonemelist.c readclause.c setlengths.c"
	... + " soundicon.c spect.c speech.c synthdata.c synthesize.c "
	... + " ssml.c synth_mbrola.c translate.c translateword.c tr_languages.c voices.c wavegen.c"

	.espeakfiles_h$ = "common.h compiledict.h dictionary.h error.h espeak_command.h event.h fifo.h intonation.h"
	... + " klatt.h langopts.h mbrola.h numbers.h phoneme.h phonemelist.h"
	... + " readclause.h setlengths.h sintab.h spect.h speech.h"
	... + " ssml.h synthdata.h synthesize.h translate.h translateword.h voice.h synthesize.h translate.h voice.h wavegen.h"
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

