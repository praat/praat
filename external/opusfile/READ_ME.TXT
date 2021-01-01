praat/external/opusfile/READ_ME.TXT
David Weenink 1 January 2021
This file describes the adaptations of the libopusfile-0.12 sources, the part of opus that directly interacts with Praat,
to make them compilable with the c++ compiler and compatible with Praat.

We copied only the necessary files to be able to decode an opus.ogg file, this also means we imported the opus-1.3.1 in a subdirectory which has two additional subdirectories "celt" and "silk".
We only made the sources from opusfile compatible with the c++ compiler and Praat by replacing all memory allocations with the corresponding Melder routines.
