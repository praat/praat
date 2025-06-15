#ifndef _Cepstrum_and_Spectrum_h_
#define _Cepstrum_and_Spectrum_h_
/* David Weenink, 20010101 */

#ifndef _Spectrum_h_
	#include "Spectrum.h"
#endif
#ifndef _Cepstrum_h_
	#include "Cepstrum.h"
#endif

Cepstrum Spectrum_to_Cepstrum (Spectrum me);

Spectrum Cepstrum_to_Spectrum (Cepstrum me);

#endif /* _Cepstrum_and_Spectrum_h_ */
