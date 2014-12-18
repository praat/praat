Praats/external/flac/READ_ME.TXT
Paul Boersma, 18 December 2014
This file describes the adaptations to the FLAC 1.2.1 sources
that are needed to make them compatible with Praat.

The .c and .h files are put into the single FLAC directory.

The #include statements are flattened, e.g.
#include private/float.h becomes #include flac_private_float.h.

The FLaC__INLINE statement is turned into an inline statement
in flac_share_alloc.h, and removed elsewhere (i.e. wherever there is a
compiler message).

For MinGW we need to change in flac_share_alloc.h:

#if !defined _MSC_VER && !defined __EMX__
#include <stdint.h> /* for SIZE_MAX in case limits.h didn't get it */
#endif

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

There were two mistakes in FLAC__MD%Final () in flac_md5.c:
	memset(ctx, 0, sizeof(ctx));	/* In case it's sensitive */
	if(0 != ctx->internal_buf) {
		free(ctx->internal_buf);
		ctx->internal_buf = 0;
		ctx->capacity = 0;
	}
should be
	if(0 != ctx->internal_buf) {   // test before clearing!
		free(ctx->internal_buf);
		ctx->internal_buf = 0;
		ctx->capacity = 0;
	}
	memset(ctx, 0, sizeof(*ctx));	// clear the whole structure, not just the first four or eight bytes!

