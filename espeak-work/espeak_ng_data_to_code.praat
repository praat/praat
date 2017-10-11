# espeak_ng_data_to_cppfunctions.praat
# djmw 20120117, 20120124, 20151130, 20171004
# assums directorystructure:
# espeak-work --> espeak-ng ; the original git clone
#                     |--> espeak-work ; copy of files needed to make libespeak-ng
#                     |--> espeak-current; equals external/espeak 
# espeak-current is necessary to be able to compare current with updated 
# This script resides in espeak-work

# Generates:
# 1. espeakdata_phons.cpp
# 2. espeakdata_dicts.cpp
# 3. espeakdata_voices.cpp
# 4. espeakdata_variants.cpp
#
# Or
#
# Copies the necessary cpp and headerfiles from from_dir to to_dir
# After copying you have to modify some files as described in the README-espeak.txt
# document.
#


myscriptname$ = "espeakdata_to_code.praat"
date$ = date$()
notify$ = "This file was created automatically on " + date$ + "."
clearinfo

form espeakdata to code
	word Espeak_version 1.49.3-dev
	boolean Copy_c_and_h_files 0
	boolean Copy_espeak_praat_couplings
	boolean Only_show_cp_command
	comment espeak-data:
	boolean Process_phon_files 1
	boolean Write_phon_files 0
	boolean Process_dict_files 1
	boolean Write_dict_files 0
	boolean Process_voice_files 1
	boolean Write_voice_files 0
	boolean Only_show_voicefiles 1
	boolean Process_language_files 0
	boolean Write_language_files 0
	comment Overrules everything: Use only if you are completely sure
	boolean Make_all_espeak_data_files 0
endform

fromdir$ = "/home/david/projects/espeak-ng"
todir$ = "espeak-ng-work"

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

if make_all_espeak_data_files
	process_phon_files = 1
	write_phon_files = 1
	process_dict_files = 1
	write_dict_files = 1
	process_voice_files = 1
	write_voice_files = 1
	only_show_Makefile = 0
	only_show_voicefiles = 0
endif


espeakdata_dir$ = fromdir$ + "/espeak-ng-data"
espeakdata_voices_dir$ = espeakdata_dir$ + "/voices/!v"
espeakdata_lang_dir$ = espeakdata_dir$ + "/lang"

include_header$ = "#include "+ """" + "espeakdata_FileInMemory.h" + """" + newline$

if process_language_files
	@create_languagefiles
endif

if copy_c_and_h_files
	@espeak_ng_copyfiles
endif

if process_phon_files
	fim1 = Create FileInMemory: espeakdata_dir$ + "/phondata"
	fim2 = Create FileInMemory: espeakdata_dir$ + "/phonindex"
	fim3 = Create FileInMemory: espeakdata_dir$ + "/phontab"
	fim4 = Create FileInMemory: espeakdata_dir$ + "/intonations"
	selectObject: fim1, fim2, fim3, fim4
	ffim = To FileInMemorySet

	cpp$ = Show as code: "espeakdata_phons", 30
	message$ = "/* File espeakdata_phons.cpp (version " + espeak_version$ + ") was automatically generated on "
	... + date$ + " from the following files in espeak-ng-data/: " + newline$ +
	... " * phondata, phonindex, phontab, intonations " +
	... " */" + newline$
	cpp$ = message$ + newline$ + include_header$ + cpp$
	file_out$ = todir$ + "/espeakdata_phons.cpp"
	if write_phon_files
		writeFile: file_out$, cpp$
	endif
	removeObject: fim1, fim2, fim3, fim4
endif

if process_dict_files

	ffim2 = Create FileInMemorySet from directory contents: "list", espeakdata_dir$, "*_dict"
	cpp$ = Show as code: "espeakdata_dicts", 30
	# skip the "_dict" part only from the id:
	cpp$ = replace$ (cpp$, "_dict"")", """)", 0)
	message$ = "/* File espeakdata_dicts.cpp (version " + espeak_version$ + ") was automatically generated on "
	... + date$ + " from all espeak-ng-data/*_dict files:  */" + newline$
	cpp$ = message$ + newline$ + include_header$ + cpp$
	file_out$ = todir$ + "/espeakdata_dicts.cpp"
	if write_dict_files
		writeFile: file_out$, cpp$
	endif

endif

if process_voice_files

	@create_voicefiles_espeak_ng

	selectObject: ffim_voices
	cpp$ = Show as code: "espeakdata_voices", 30
	message$ = "/* File espeakdata_voices.cpp (version " + espeak_version$ + ") was automatically generated on "
	... + date$ + " from files in espeak-ng-data/voices  */" + newline$
	cpp$ =  message$ + newline$ + include_header$ + cpp$
	if only_show_voicefiles
		appendInfoLine: cpp$
	else
		file_out$ = todir$ + "/espeakdata_voices.cpp"
		writeFile: file_out$, cpp$
	endif

	;selectObject: ffim_variants
	;cpp$ = Show as code: "espeakdata_variants", 30
	;message$ = "/* File espeakdata_variants.cpp (version " + espeak_version$ + ") was automaticallygenerated on "
	;... + date$ + " from files in espeak-ng-data/voices/!v  */" + newline$
	;cpp$ = message$ + newline$ + include_header$ + cpp$
	;if only_show_voicefiles
	;	appendInfoLine: cpp$
	;else
	;	file_out$ = todir$ + "/espeakdata_variants.cpp"
	;	writeFile: file_out$, cpp$
	;endif

endif

procedure create_languagefiles
	.languages = Create FileInMemorySet from directory contents: "l", espeakdata_lang_dir$, "*"
	.ldirs =  Create Strings as directory list: "dirs",  espeakdata_lang_dir$+ "/*"
	.ndirs = Get number of strings
	for .idir to .ndirs
		selectObject: .ldirs
		.dir$ = Get string: .idir
		.langset = Create FileInMemorySet from directory contents: "l", espeakdata_lang_dir$ + "/" + .dir$ , "*"
		plusObject: .languages
		.merged = Merge
		removeObject: .langset, .languages
		.languages = .merged
	endfor
	removeObject: .ldirs

	if write_language_files
		selectObject: .languages
		.message$ = "/* File espeakdata_languages.cpp (version " + espeak_version$ + ") was automatically generated on "
		... + date$ + " from files in espeak-ng-data/lang  */" + newline$
		.cpp$ = Show as code: "espeakdata_languages", 30	
		.cpp$ =  .message$ + newline$ + include_header$ + .cpp$
		.file_out$ = todir$ + "/espeakdata_languages.cpp"
		writeFile: .file_out$, .cpp$
	endif
endproc

procedure create_voicefiles_espeak_ng
	ffim_voices = Create FileInMemorySet from directory contents: "voices", espeakdata_dir$ + "/voices/!v", "*"
endproc

procedure create_voicefiles_espeak
	ffim_voices = Create FileInMemorySet from directory contents: "voices", espeakdata_dir$ + "/voices", "*"
	.dirList = Create Strings as directory list: "dirlist", espeakdata_dir$ + "/voices"
	.ndirs = Get number of strings
	for .idir to .ndirs
		selectObject: .dirList
		.dir$ = Get string: .idir
		if .dir$ <> "mb"
			if .dir$ <> "!v"
				ffim_tmp = Create FileInMemorySet from directory contents: .dir$, espeakdata_dir$ + "/voices/" + .dir$, "*"
				plusObject: ffim_voices
				ffim = Merge
				removeObject: ffim_tmp, ffim_voices
				ffim_voices = ffim
			else
				ffim_variants = Create FileInMemorySet from directory contents: "variants", espeakdata_dir$+ "/voices/" + .dir$, "*"
			endif
		endif
	endfor
endproc

if copy_espeak_praat_couplings
	externaldir$ = "../external/espeak"
	.cp_command$ = "cp " + externaldir$ + "/espeakdata_FileInMemory.cpp "
	... +  externaldir$ + "/espeakdata_FileInMemory.h "
	... +  externaldir$ + "/speech.h "
	... +  externaldir$ + "/Makefile " + todir$ + "/"
	if only_show_cp_command
		appendInfoLine: .cp_command$
	else
		runSystem: .cp_command$
	endif
endif

# extract only the necesary files to espeak-ng-work.
# changes to these files have to be made in espeak-ng-current

procedure espeak_ng_copyfiles
	.espeakfiles_c$ = "compiledata.c compiledict.c compilembrola.c dictionary.c" +
	... " encoding.c error.c espeak_api.c espeak_command.c" +
	... " event.c fifo.c ieee80.c intonation.c klatt.c" +
	... " mnemonics.c numbers.c phoneme.c phonemelist.c readclause.c setlengths.c" +
	... " spect.c speech.c synthdata.c synthesize.c " +
	... " synth_mbrola.c translate.c tr_languages.c voices.c wavegen.c"

	.espeakfiles_h$ = "error.h  espeak_command.h event.h  fifo.h klatt.h phoneme.h  sintab.h" +
	... " spect.h speech.h synthesize.h  translate.h voice.h synthesize.h translate.h voice.h"
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
	writeFile (todir$ + "/espeak_ng_version.h", "#define ESPEAK_NG_VERSION " + "U""" + espeak_version$ + """")
endproc

procedure copy_rename: .fromdir$, .files$, .ext$, .newext$
	.list = Create Strings as tokens: .files$
	.numberOfFiles = Get number of strings
	for .ifile to .numberOfFiles
		.name$ = Get string: .ifile
		.newname$ = .name$ - .ext$ + .newext$
		.command$ =  "cp " + .fromdir$ + "/" + .name$ + " " + todir$ + "/" + .newname$
		appendInfoLine: .command$
		if not only_show_cp_command
			runSystem: .command$
		endif
	endfor
endproc

