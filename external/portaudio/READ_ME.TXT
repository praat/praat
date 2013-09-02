Praats/external/portaudio/READ_ME.TXT
Paul Boersma, 1 September 2013
This file describes the adaptations to the PortAudio v19 sources (2007/11)
that are needed to make them compatible with Praat.

Deleted lines in pa_***_hostapis.c.

At the top of pa_win_*.c:
#undef UNICODE

In pa_win_mme.c, remove the #ifndef/endif from:
#ifndef __MWERKS__
#include <malloc.h>
#include <memory.h>
#endif /* __MWERKS__ */

In pa_mac_core.c:
#include <Components.h>

Duplicate pa_unix_util.c to pa_mac_util.c, but only for allocation and time routines.

Remove the hard-coded definition of SIZEOF_LONG from pa_types.h and instead use <stdint.h> to define PaInt32 and the like. Correct the associated error in pa_dither.c: SIZEOF_LONG -> sizeof(long).