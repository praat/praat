praat/external/opusfile/READ_ME.TXT
David Weenink 2 January 2021
This file describes the adaptations made to libopusfile-0.12 sources, the part of opus that directly interacts with Praat,
to make them compilable with the c++ compiler and compatible with Praat. We adapted the header files and sources to be able to compile with the g++ compiler. We replaced all memory allocations with the corresponding Melder routines.
Only files needed for decoding were copied.

In a subdirectory "opus" we copied the opus-1.3.1 sources which also include a "celt" and a "silk" directory. A couple of files that were clearly not meant for decoding were left out. The silk code als needs a subdirectort "float" for proper decoding with floating point arithmatic. The fies in the opus directories and subdirectories were not modified and compile with the C-compiler.
