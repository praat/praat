# createFileInMemorySets.praat
# djmw 20240210
# Paul Boersma 20 May 2024: relative paths,
#   to make it work on other computers than just David's
# up-to-date with the espeak-ng git repository until:
#   commit cb62d93fd7b61d8593b9ae432e6e2a78e3711a77 
#   Date:   Thu Feb 8 16:21:15 2024 +0300
# 

myscriptname$ = "createFileInMemorySets.praat"
date$ = date$()
notify$ = "This file was created automatically on " + date$ + "."
writeInfo ()

espeakVersion$ = "1.52-dev"
espeakdata_dir$ = "./espeak-ng-data"
espeakdata_voices_dir$ = espeakdata_dir$ + "/voices/!v"
espeakdata_lang_dir$ = espeakdata_dir$ + "/lang"

gpltext$ =  " * Copyright (C) David Weenink 2012-2024, Paul Boersma 2024" + newline$ +
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

include_header$ = "#include "+ """" + "espeakdata_FileInMemory.h" + """" + newline$

@create_phonFileInMemorySet
@create_languageFileInMemorySet
voice_fims = Create FileInMemorySet from directory contents: "voices", espeakdata_dir$ + "/voices/!v", "*"
dict_fims = Create FileInMemorySet from directory contents: "dicts", espeakdata_dir$, "*_dict"
ru_dict_fims = Extract files: "contains", "/ru_dict"
selectObject: dict_fims
Remove files: "contains", "/ru_dict"
selectObject: language_fims, phon_fims, dict_fims, voice_fims
espeak_ng_fims = Merge
@saveFileInMemorySet_asCppFile: espeak_ng_fims, ""
@saveFileInMemorySet_asCppFile: ru_dict_fims, "__ru"

procedure saveFileInMemorySet_asCppFile: .fims, .specification$
	selectObject: .fims 
	.cppFile$ = "create_espeak_ng_FileInMemorySet" + .specification$ + ".cpp"
	.message$ = "/* " + .cppFile$ + newline$
	... + " * This file was automatically created from files in the folder `espeak-ng-data`" + newline$
	... + " * by the script `" + myscriptname$ + "`." + newline$
	... + " * Espeak-ng version: " + espeakVersion$ + "." + newline$
	... + " * Date: " + date$() + "." + newline$
	... + "*/" + newline$ + newline$
	... + "#include ""espeakdata_FileInMemory.h""" + newline$
	.cppCode$ = Show as code: "espeak_ng_FileInMemorySet" + .specification$, 30
	.cppCode$ = .message$ + .cppCode$ 
	writeInfoLine: .cppCode$
	writeFile: .cppFile$, .cppCode$
endproc

procedure create_phonFileInMemorySet
	#
	# HACK:
	# We have to make five sets of one, because `Create FileInMemory...` turns the relative path
	# into an absolute path, while `Create FileInMemorySet from directory contents`
	# retains the relative path, which is what we want.
	#
	fim1 = Create FileInMemorySet from directory contents: "fim1", espeakdata_dir$, "*phondata"
	fim2 = Create FileInMemorySet from directory contents: "fim2", espeakdata_dir$, "*phonindex"
	fim3 = Create FileInMemorySet from directory contents: "fim3", espeakdata_dir$, "*phontab"
	fim4 = Create FileInMemorySet from directory contents: "fim4", espeakdata_dir$, "*intonations"
	fim5 = Create FileInMemorySet from directory contents: "fim5", espeakdata_dir$, "*phondata-manifest"
	selectObject: fim1, fim2, fim3, fim4, fim5
	phon_fims = Merge
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
