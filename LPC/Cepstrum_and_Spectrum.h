#ifndef _Cepstrum_and_Spectrum_h_
#define _Cepstrum_and_Spectrum_h_
/* David Weenink, 20010101 */

#include "Spectrum.h"
#include "Cepstrum.h"

Cepstrum Spectrum_to_Cepstrum (Spectrum me);
Spectrum Cepstrum_to_Spectrum (Cepstrum me);
PowerCepstrum Spectrum_to_PowerCepstrum (Spectrum me);
#endif /* _Cepstrum_and_Spectrum_h_ */
