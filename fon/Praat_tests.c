/* Praat_tests.c */
/* Paul Boersma, August 2, 2001 */
/* December 10, 2006: MelderInfo */

#include "Praat_tests.h"
#include "enum_c.h"
#include "Praat_tests_enums.h"

int Praat_tests (int itest, char *arg1, char *arg2, char *arg3, char *arg4) {
	long i, n = atol (arg1);
	double x;
	(void) arg1;
	(void) arg2;
	(void) arg3;
	(void) arg4;
	Melder_clearInfo ();
	Melder_stopwatch ();
	switch (itest) {
		case enumi (PraatTests, CheckRandom1009_2009): {
			NUMrandomRestart (310952);
			for (i = 1; i <= 1009 * 2009 - 100 + 1; i ++)
				x = NUMrandomFraction ();
			MelderInfo_writeLine1 (Melder_double (x));
		} break;
		case enumi (PraatTests, TimeRandomFraction): {
			for (i = 1; i <= n; i ++)
				(void) NUMrandomFraction ();
		} break;
		case enumi (PraatTests, TimeSort): {
			long m = atol (arg2);
			long *array = NUMlvector (1, m);
			for (i = 1; i <= m; i ++)
				array [i] = NUMrandomInteger (1, 100);
			Melder_stopwatch ();
			for (i = 1; i <= n; i ++)
				NUMsort_l (m, array);
			NUMlvector_free (array, 1);
		} break;
	}
	MelderInfo_writeLine2 (Melder_single (Melder_stopwatch () / n * 1e9), " nanoseconds");
	MelderInfo_close ();
	return 1;
}

/* End of file Praat_tests.c */
