# "Search dates"
# Paul Boersma, 14 September 2023

form: "Search dates"
	text: "ManPage title", ""
endform
writeInfoLine: "Searching: ", manPage_title$

previousHit$ = ""
volumes$# = folderNames$#: "/Volumes/sources_*"
for ivol to size: volumes$#
	appendInfoLine: "Searching ", volumes$# [ivol]
	volumePath$ = "/Volumes/" + volumes$# [ivol]
	rootFolders$# = folderNames$#: volumePath$
	for iroot to size: rootFolders$#
		rootFolderPath$ = volumePath$ + "/" + rootFolders$# [iroot]
		praatVersion$ = mid$ (rootFolders$# [iroot], 9, 1e9)
		numberOfManualEntries = 0
		searchFolders$# = folderNames$#: rootFolderPath$
		for ifolder to size: searchFolders$#
			searchFolderPath$ = rootFolderPath$ + "/" + searchFolders$# [ifolder]
			fileNamePatterns$# = { "manual_*.c", "manual_*.cpp" }
			for ipat to size: fileNamePatterns$#
				fileNames$# = fileNames$#: searchFolderPath$ +
				... "/" + fileNamePatterns$# [ipat]
				for ifile to size: fileNames$#
					filePath$ = searchFolderPath$ + "/" + fileNames$# [ifile]
					lines$# = readLinesFromFile$#: filePath$
					for iline to size: lines$#
						line$ = lines$# [iline]
						if index (line$, "MAN_BEGIN") and
						... index (line$, manPage_title$)
							if line$ <> previousHit$
								appendInfoLine: line$
								previousHit$ = line$
							endif
						endif
					endfor
				endfor
			endfor
		endfor
	endfor
endfor

