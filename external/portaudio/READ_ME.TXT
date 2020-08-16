Praats/external/portaudio/READ_ME.TXT
Paul Boersma, 13 August 2020
This file describes the adaptations to the PortAudio v19.06 sources (2016/10)
that are needed to make them compatible with Praat.

Deleted many lines in pa_***_hostapis.c.

(At the top of pa_win_*.c: #undef UNICODE)

Duplicate pa_unix_util.c to pa_mac_util.c, but only for allocation and time functions.

Around pa_linux_alsa.c, do
#if defined (UNIX) && defined (ALSA)
...
#endif

Around pa_jack.c, do
#if defined (UNIX) && defined (JACK)
...
#endif

Around pa_win_*.c, do
#ifdef _WIN32
...
#endif

Around pa_unix_util.c and pa_jack.c, do
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
