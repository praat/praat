Praats/external/portaudio/READ_ME.TXT
Paul Boersma, 22 October 2018
This file describes the adaptations to the PortAudio v19 sources (2014/01)
that are needed to make them compatible with Praat.

Deleted many lines in pa_***_hostapis.c.

(At the top of pa_win_*.c: #undef UNICODE)

Duplicate pa_unix_util.c to pa_mac_util.c, but only for allocation and time functions.

Remove the hard-coded definition of SIZEOF_LONG from pa_types.h and instead use <stdint.h> to define PaInt32 and the like.

Around pa_linux_alsa.c, do
#if defined (UNIX) && defined (ALSA)
...
#endif

Around pa_win_*.c, do
#ifdef _WIN32
...
#endif

Around pa_unix_util.c, do
#if defined (UNIX)
...
#endif

Add prototype for
PaError PaAlsa_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex );
in pa_linux_alsa.h

Add prototype for
PaError PaJack_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex hostApiIndex );
in pa_jack.h

Include pa_jack.h in pa_jack.c

Insert
void
in declaration and definition of PaUnix_InitializeThreading.
