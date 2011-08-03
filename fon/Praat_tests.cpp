/* Praat_tests.cpp */
/* Paul Boersma, August 2, 2001 */
/* December 10, 2006: MelderInfo */
/* November 5, 2007: wchar */
/* 21 March 2009: modern enums */
/* 24 May 2011: C++ */

#include "Praat_tests.h"

#include "enums_getText.h"
#include "Praat_tests_enums.h"

int Praat_tests (int itest, wchar *arg1, wchar *arg2, wchar *arg3, wchar *arg4) {
	long i, n = wcstol (arg1, NULL, 10);
	double x;
	(void) arg1;
	(void) arg2;
	(void) arg3;
	(void) arg4;
	Melder_clearInfo ();
	Melder_stopwatch ();
	switch (itest) {
		case kPraatTests_CHECK_RANDOM_1009_2009: {
			NUMrandomRestart (310952);
			for (i = 1; i <= 1009 * 2009 - 100 + 1; i ++)
				x = NUMrandomFraction ();
			MelderInfo_writeLine1 (Melder_double (x));
		} break;
		case kPraatTests_TIME_RANDOM_FRACTION: {
			for (i = 1; i <= n; i ++)
				(void) NUMrandomFraction ();
		} break;
		case kPraatTests_TIME_SORT: {
			long m = wcstol (arg2, NULL, 10);
			long *array = NUMlvector (1, m);
			for (i = 1; i <= m; i ++)
				array [i] = NUMrandomInteger (1, 100);
			Melder_stopwatch ();
			for (i = 1; i <= n; i ++)
				NUMsort_l (m, array);
			NUMlvector_free (array, 1);
		} break;
	}
	MelderInfo_writeLine2 (Melder_single (Melder_stopwatch () / n * 1e9), L" nanoseconds");
	MelderInfo_close ();
	return 1;
}

/* End of file Praat_tests.cpp */
