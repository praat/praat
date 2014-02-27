Praats/external/portaudio/READ_ME.TXT
Paul Boersma, 14 February 2014
This file describes the adaptations to the PortAudio v19 sources (2014/01)
that are needed to make them compatible with Praat.

Deleted many lines in pa_***_hostapis.c.

(At the top of pa_win_*.c: #undef UNICODE)

Duplicate pa_unix_util.c to pa_mac_util.c, but only for allocation and time routines.

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
in pa_linux_als.h

Insert
void
in declaration and definition of PaUnix_InitializeThreading.
