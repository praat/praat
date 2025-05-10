# Polyphone.praat
# Paul Boersma, 4 August 2024

rootFolder$ = "/Volumes/Polyphone"

stopwatch
disks$# = folderNames$#: rootFolder$
for disk to size (disks$#)
	dataFolder$ = if disk = 9 then "DEV_TEST" else if disk = 10 then "EVALTEST" else "TRAIN" fi fi
	dataPath$ = rootFolder$ + "/" + disks$# [disk] + "/" + dataFolder$
	numberFolders$# = folderNames$#: dataPath$
	for numberFolder to size (numberFolders$#)
		numberPath$ = dataPath$ + "/" + numberFolders$# [numberFolder]
		writeInfoLine: numberPath$
		participantFolders$# = folderNames$#: numberPath$
		for participantFolder to size (participantFolders$#)
			participantPath$ = numberPath$ + "/" + participantFolders$# [participantFolder]
			files$# = fileNames$#: participantPath$
			for file to size (files$#)
				filePath$ = participantPath$ + "/" + files$# [file]
				Read from file: filePath$
				Remove
			endfor
		endfor
	endfor
endfor
appendInfoLine: "Elapsed time: ", stopwatch, " seconds"
