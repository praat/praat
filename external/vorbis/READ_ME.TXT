praat/external/ogg/READ_ME.TXT
David Weenink 22 December 2020
This file describes the adaptations to the libogg-1.3.4 sources and the libvorbis-1.3.7
to make them compilable with the c++ compiler and compatible with Praat.

libogg: we copied the files bitwise.c, framing.c, ogg.h, crctable.h
	and prepended ogg_ to distinguish them from the vorbis_ files
In ogg.h the #include "os_types.h" was removed, melder.h included and also types content of config_types.h was included.
We removed C variable name clashes with C++ defined reserved names new, class and this by renaming these variables to neww, classs and thiss.
We changed all .c extensions to .cpp extensions.	
We replaced all _ogg_malloc, _ogg_calloc and _ogg_free with _Melder_malloc, _Melder_calloc and Melder_free, respectively, 
and added the necessary casting of the void pointer to the resultant type.
