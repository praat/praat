praat/external/ogg/READ_ME.TXT
David Weenink 22 December 2020
This file describes the adaptations to the libogg-1.3.4 sources and the libvorbis-1.3.7
to make them compilable with the c++ compiler and compatible with Praat.

libogg: we copied the files bitwise.c, framing.c, ogg.h, crctable.h
	and prepended ogg_ to distinguish them from the vorbis_ files
In ogg.h the #include "os_types.h" was removed and melder.h included

c++ supplies hypot (a,b)
