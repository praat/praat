praat/external/ogg/READ_ME.TXT
David Weenink 22 December 2020
Paul Boersma 16 August 2021
This file describes the adaptations to the libogg-1.3.4 sources and the libvorbis-1.3.7
to make them compilable with the C++ compiler and compatible with Praat.

libogg: we copied the files bitwise.c, framing.c, ogg.h, crctable.h
	and prepended ogg_ to distinguish them from the vorbis_ files
In ogg.h the include file "os_types.h" was removed, melder.h was included, and also types content of config_types.h was included.
We removed C variable names that clashed with the C++ reserved names `new`, `class` and `this` by renaming them to `neww`, `classs` and `thiss`.
We changed all .c file extensions to .cpp.
We replaced all _ogg_malloc, _ogg_calloc and _ogg_free with _Melder_malloc, _Melder_calloc and Melder_free, respectively, 
and added the necessary casting of the void pointer to the resultant type.

In vorbisfile.h, fseek was incorrectly cast to a function taking a 64-bit integer as its second argument. Replaced with fseeko.
