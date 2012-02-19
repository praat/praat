Praats/external/flac/READ_ME.TXT
Paul Boersma, May 13, 2008
This file describes the adaptations to the FLAC 1.2.1 sources
that are needed to make them compatible with Praat.

The .c and .h files are put into the single FLAC directory.

The #include statements are flattened, e.g.
#include private/float.h becomes #include flac_private_float.h.

The FLaC__INLINE statement is turned into an inline statement
in flac_share_alloc.h, and removed elsewhere (i.e. wherever there is a
compiler message).

For MinGW we need to change in flac_share_alloc.h:

#if !defined _MSC_VER && !defined __EMX__#include <stdint.h> /* for SIZE_MAX in case limits.h didn't get it */#endif

For win32 on Metrowerks CodeWarrior we do a
#if defined (_WIN32) && ! defined (off_t)
#define off_t long
#endif
just above the declaration of
FLAC__metadata_simple_iterator_get_block_offset;
we also do
#define fseeko fseek
#define ftello ftell
on win32 on Metrowerks CodeWarrior.
This measure may be expendable once we compile on mingw or so.

The sources contain a confusion of FLAC__int32 and int,
especially in calls to local_bitreader_read_rice_signed_block
or FLAC__bitreader_read_rice_signed_block;
Some changes from int to FLAC__int32 may be necessary.
We also have to insert
Melder_assert (sizeof (int) == 4)
in read_residual_partitioned_rice_.

To ensure compatibility with international file names on Windows,
the following is added to flac_FLAC_formant.h:
#ifdef _WIN32
	#include "melder.h"
	#define fopen(filename,mode)  _wfopen (Melder_peekWcsToUtf16 (Melder_peekUtf8ToWcs (filename)), L"" mode)
#endif
