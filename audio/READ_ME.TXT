Changes to make PortAudio v19 (November 2007) work with Praat:

Deleted lines in pa_***_hostapis.c.

In pa_win_mme.c:
GetEnvironmentVariable -> GetEnvironmentVariableA
WAVEINCAPS -> WAVEINCAPSA (2x)
waveInGetDevCaps -> waveInGetDevCapsA
WAVEOUTCAPS -> WAVEOUTCAPSA (2x)
waveOutGetDevCaps -> waveOutGetDevCapsA
Remove the #ifndef/endif from:
#ifndef __MWERKS__
#include <malloc.h>
#include <memory.h>
#endif /* __MWERKS__ */

In pa_ringbuffer.c, the Mac part should be made compatible with 10.2 and 10.3:
//#   include <libkern/OSAtomic.h> ppgb
#   define PaUtil_FullMemoryBarrier()  /*OSMemoryBarrier() ppgb */
#   define PaUtil_ReadMemoryBarrier()  /*OSMemoryBarrier() ppgb */
#   define PaUtil_WriteMemoryBarrier() /*OSMemoryBarrier() ppgb */
(ring buffers aren't used anyway in Praat)

In pa_mac_core_blocking.c idem:
//#ifdef MOSX_USE_NON_ATOMIC_FLAG_BITS  ppgb
# define OSAtomicOr32( a, b ) ( (*(b)) |= (a) )
# define OSAtomicAnd32( a, b ) ( (*(b)) &= (a) )
//#else  ppgb
//# include <libkern/OSAtomic.h>  ppgb
//#endif  ppgb
(blocking I/O isn't used anyway in Praat)

In pa_mac_core.c idem:
//#ifdef MOSX_USE_NON_ATOMIC_FLAG_BITS  ppgb
# define OSAtomicOr32( a, b ) ( (*(b)) |= (a) )
# define OSAtomicAnd32( a, b ) ( (*(b)) &= (a) )
//#else  ppgb
//# include <libkern/OSAtomic.h>  ppgb
//#endif  ppgb
   if( isInput ) {
      volatile uint32_t a = paInputUnderflow, b = * (uint32_t *)&(stream->xrunFlags);
      OSAtomicOr32( paInputUnderflow, (uint32_t *)&(stream->xrunFlags) );
   } else {
      volatile uint32_t a = paOutputOverflow, b = * (uint32_t *)&(stream->xrunFlags);
      OSAtomicOr32( paOutputOverflow, (uint32_t *)&(stream->xrunFlags) );
   }
(this may not really work; am I right in thinking that the danger is low?)

Paul Boersma, June 1, 2008.
