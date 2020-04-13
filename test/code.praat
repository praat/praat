echo Checking code updates...

code$ < ../dwtools/Strings_extensions.cpp
assert not index (code$, "Get string...")   ; 2012-08-11

code$ < ../external/GSL/Makefile
assert not index (code$, "CFLAGS")   ; 2012-08-08

code$ < ../melder/regularExp.cpp
assert not index (code$, "'0' }")   ; 2012-08-07

code$ < ../external/espeak/speech.cpp
assert not index (code$, "setlocale")   ; 2012-10-01

printline OK...