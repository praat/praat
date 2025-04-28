# Buckeye.praat
# Paul Boersma, 28 April 2025

#
# This script cycles through all the sound files and annotations of the Buckeye corpus,
# listing the following:
#

form: "Report on Buckeye corpus"
	folder: "Buckeye folder", "/Volumes/Buckeye"
endform

corpusFolderPath$ = buckeye_folder$

writeInfoLine: "Reporting on the Buckeye folder “", corpusFolderPath$, "”..."
folderNames$# = folderNames$#: corpusFolderPath$ + "/s*"
numberOfFolders = size (folderNames$#)
assert numberOfFolders = 40
for ifolder to numberOfFolders
	folderName$ = folderNames$# [ifolder]
	folderPath$ = corpusFolderPath$ + "/" + folderName$
	appendInfoLine: folderPath$
	subfolderNames$# = folderNames$#: folderPath$ + "/*"
	numberOfSubfolders = size (subfolderNames$#)
	assert numberOfSubfolders >= 4
	assert numberOfSubfolders <= 11
	for isubfolder to numberOfSubfolders
		subfolderName$ = subfolderNames$# [isubfolder]
		if subfolderName$ <> "s3504a"
			subfolderPath$ = folderPath$ + "/" + subfolderName$
			appendInfoLine: subfolderPath$
			soundFilePath$ = subfolderPath$ + "/" + subfolderName$ + ".wav"
			Read Sound with adjacent annotation files (Buckeye): soundFilePath$
			assert numberOfSelected () = 2
			assert selected$ (1) = "Sound " + subfolderName$
			assert selected$ (2) = "TextGrid " + subfolderName$
			Remove
		endif
	endfor
endfor