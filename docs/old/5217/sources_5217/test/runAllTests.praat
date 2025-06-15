#! runAlltests.praat
# Paul Boersma, January 30, 2006
#
# This script runs all Praat scripts in its directory, except itself.

Create Strings as file list... allTests .
n = Get number of strings
for i to n
   select Strings allTests
   file$ = Get string... i
   if right$ (file$, 6) = ".praat" and file$ <> "runAllTests.praat"
      execute 'file$'
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
