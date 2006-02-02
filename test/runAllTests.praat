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
printline                ######               #####           #####
printline            ##############           #####         #####
printline          #####        #####         #####       #####
printline         #####          #####        #####     #####
printline         #####          #####        #####   #####
printline         #####          #####        ##### #####
printline         #####          #####        #########
printline         #####          #####        #######
printline         #####          #####        #####
printline         #####          #####        #######
printline         #####          #####        #########
printline         #####          #####        ##### #####
printline         #####          #####        #####   #####
printline         #####          #####        #####     #####
printline          #####        #####         #####       #####
printline            ##############           #####         #####
printline                ######               #####           #####
