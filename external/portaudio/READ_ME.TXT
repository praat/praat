Praats/external/portaudio/READ_ME.TXT
Paul Boersma, 4 January 2010
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
