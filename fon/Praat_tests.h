#ifndef _Praat_tests_h_
#define _Praat_tests_h_
/* Paul Boersma, 7 May 2011 */

#ifndef _Thing_h_
	#include "Thing.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#include "Praat_tests_enums.h"

int Praat_tests (int itest, wchar_t *arg1, wchar_t *arg2, wchar_t *arg3, wchar_t *arg4);

#ifdef __cplusplus
	}
#endif

#endif
/* End of file Praat_tests.h */
