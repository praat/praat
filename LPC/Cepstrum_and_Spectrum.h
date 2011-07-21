#ifndef _Cepstrum_and_Spectrum_h_
#define _Cepstrum_and_Spectrum_h_
/* David Weenink, 20010101 */

#include "Spectrum.h"
#include "Cepstrum.h"

#ifdef __cplusplus
	extern "C" {
#endif

Cepstrum Spectrum_to_Cepstrum (Spectrum me);

Spectrum Cepstrum_to_Spectrum (Cepstrum me);

#ifdef __cplusplus
	}
#endif

#endif /* _Cepstrum_and_Spectrum_h_ */
