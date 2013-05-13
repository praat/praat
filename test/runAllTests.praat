# Praat script runAlltests.praat
# Paul Boersma, 6 May 2013
#
# This script runs all Praat scripts in its subdirectories.

directories = Create Strings as directory list... directories .
numberOfDirectories = Get number of strings
for directory to numberOfDirectories
printline 'directory'
	select Strings directories
	directory$ = Get string... directory
	printline 'directory$'
	if directory$ <> "manually"
		files = Create Strings as file list... files 'directory$'/*.praat
		numberOfFiles = Get number of strings
		for file to numberOfFiles
			select files
			file$ = Get string... file
			printline ### executing 'directory$'/'file$':
			execute 'directory$'/'file$'
		endfor
	endif
endfor

directories1 = Create Strings as directory list... directories1 .
numberOfDirectories1 = Get number of strings
for directory1 to numberOfDirectories1
	select Strings directories1
	directory1$ = Get string... directory1
	if directory1$ <> "manually"
		directories2 = Create Strings as directory list... directories2 'directory1$'/*
		numberOfDirectories2 = Get number of strings
		for directory2 to numberOfDirectories2
			select Strings directories2
			directory2$ = Get string... directory2
			files = Create Strings as file list... files 'directory1$'/'directory2$'/*.praat
			numberOfFiles = Get number of strings
			for file to numberOfFiles
				select files
				file$ = Get string... file
				printline ### executing 'directory1$'/'directory2$'/'file$':
				execute 'directory1$'/'directory2$'/'file$'
			endfor
		endfor
	endif
endfor

echo                  ALL PRAAT TESTS WENT OK
printline
line5$ = "        #####          #####        #####   #####"
line8$ = "        #####          #####        #######"
line1$ = "               ######               #####           #####"
line2$ = "           ##############           #####         #####"
line4$ = "        #####          #####        #####     #####"
line7$ = "        #####          #####        #########"
line9$ = "        #####          #####        #####"
line3$ = "         #####        #####         #####       #####"
line6$ = "        #####          #####        ##### #####"
for line from 1 to 9
	line$ = line'line'$
	printline 'line$'
endfor
for line2 from 1 to 8
	line = 9 - line2
	line$ = line'line'$
	printline 'line$'
endfor
