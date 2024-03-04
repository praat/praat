# createFileInMemorySets.praat
# djmw 20240210
# up-to-date with the espeak-ng git repository until:
#   commit cb62d93fd7b61d8593b9ae432e6e2a78e3711a77 
#   Date:   Thu Feb 8 16:21:15 2024 +0300
# 
myscriptname$ = "createFileInMemorySets.praat"
date$ = date$()
notify$ = "This file was created automatically on " + date$ + "."
clearinfo

form Espeakdata to code
	word Espeak_version 1.52-dev
	boolean Create_FileInMemorySet 0
endform

gpltext$ =  " * Copyright (C) David Weenink 2012-2024" + newline$ +
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

fromdir$ = "/home/david/projects/praat/generate/espeak"
todir$ = "./"

espeakdata_dir$ = fromdir$ + "/espeak-ng-data"
espeakdata_voices_dir$ = espeakdata_dir$ + "/voices/!v"
espeakdata_lang_dir$ = espeakdata_dir$ + "/lang"

include_header$ = "#include "+ """" + "espeakdata_FileInMemory.h" + """" + newline$

if create_FileInMemorySet

	@create_phonFileInMemorySet

	@create_languageFileInMemorySet
	voice_fims = Create FileInMemorySet from directory contents: "voices", espeakdata_dir$ + "/voices/!v", "*"

	dict_fims = Create FileInMemorySet from directory contents: "dicts", espeakdata_dir$, "*_dict"
	ru_dict_fims = Extract files: "contains", "ru_dict"

	selectObject: language_fims, phon_fims, dict_fims, voice_fims
	espeak_ng_fims = Merge

	@saveFileInMemorySet_asCPPFile: espeak_ng_fims, ""
	@saveFileInMemorySet_asCPPFile: ru_dict_fims, "__ru"
endif

procedure saveFileInMemorySet_asCPPFile: .fims, .specification$
	selectObject: .fims 
	.cppFile$ = "create_espeak_ng_FileInMemorySet" + .specification$ + ".cpp"
	.message$ = "/* " + .cppFile$ + newline$
	... + " * This file was automatically created from files in espeak-ng-data by" + newline$
	... + " * the script " + myscriptname$ + newline$
	... + " * Espeak-ng version: " + espeak_version$ + "." + newline$
	... + " * Date: " + date$() + "." + newline$
	... + "*/" + newline$ + newline$
	... + "#include ""espeakdata_FileInMemory.h""" + newline$
	.cppCode$ = Show as code: "espeak_ng_FileInMemorySet" + .specification$, 30
	.cppCode$ = .message$ + .cppCode$ 
	writeInfoLine: .cppCode$
	writeFile: todir$ + .cppFile$, .cppCode$

endproc

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

