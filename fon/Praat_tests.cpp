/* Praat_tests.cpp */
/* Paul Boersma, August 2, 2001 */
/* December 10, 2006: MelderInfo */
/* November 5, 2007: wchar_t */
/* 21 March 2009: modern enums */
/* 24 May 2011: C++ */

#include "Praat_tests.h"

#include "enums_getText.h"
#include "Praat_tests_enums.h"
#include "enums_getValue.h"
#include "Praat_tests_enums.h"
#include <string>

#define UTF32_C(string) \
	({ static const wchar_t *_static_utf32_string = Melder_utf8ToStr32 (string); _static_utf32_string; })

constexpr char32_t greeting [] {U"Hello?"};


int Praat_tests (int itest, wchar_t *arg1, wchar_t *arg2, wchar_t *arg3, wchar_t *arg4) {
	int64 n = wcstoll (arg1, NULL, 10);
	double t;
	(void) arg1;
	(void) arg2;
	(void) arg3;
	(void) arg4;
	constexpr int a {2};   // modern syntax
	constexpr double d {2};
	int b = ({ int c {4}; c+10; });   // unusual syntax: a "statement expression", which is a GNU C extension
	Melder_clearInfo ();
	Melder_stopwatch ();
	switch (itest) {
		case kPraatTests_TIME_RANDOM_FRACTION: {
			for (int64 i = 1; i <= n; i ++)
				(void) NUMrandomFraction ();
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_RANDOM_GAUSS: {
			for (int64 i = 1; i <= n; i ++)
				(void) NUMrandomGauss (0.0, 1.0);
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_SORT: {
			long m = wcstol (arg2, NULL, 10);
			long *array = NUMvector <long> (1, m);
			for (int64 i = 1; i <= m; i ++)
				array [i] = NUMrandomInteger (1, 100);
			Melder_stopwatch ();
			for (int64 i = 1; i <= n; i ++)
				NUMsort_l (m, array);
			t = Melder_stopwatch ();
			NUMvector_free (array, 1);
		} break;
		case kPraatTests_TIME_INTEGER: {
			double sum = 0;
			for (int64 i = 1; i <= n; i ++)
				sum += i * (i - 1) * (i - 2);
			t = Melder_stopwatch ();
			MelderInfo_writeLine (Melder_double (sum));
		} break;
		case kPraatTests_TIME_FLOAT: {
			double sum = 0.0, fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += fi * (fi - 1.0) * (fi - 2.0);
			t = Melder_stopwatch ();
			MelderInfo_writeLine (Melder_double (sum));
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_BUILTIN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32_t) fi;
			t = Melder_stopwatch ();   // 2.59   // 1.60
			MelderInfo_writeLine (Melder_double (sum));
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_EXTERN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32_t) ((int32_t) (fi - 2147483648.0) + 2147483647L + 1);
			t = Melder_stopwatch ();   // 1.60
			MelderInfo_writeLine (Melder_double (sum));
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_BUILTIN: {
			double sum = 0.0;
			uint32_t nu = (uint32_t) n;
			for (uint32_t iu = 1; iu <= nu; iu ++)
				sum += (double) iu;
			t = Melder_stopwatch ();   // 1.35
			MelderInfo_writeLine (Melder_double (sum));
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_EXTERN: {
			double sum = 0.0;
			uint32_t nu = (uint32_t) n;
			for (uint32_t iu = 1; iu <= nu; iu ++)
				sum += (double) (int32_t) (iu - 2147483647L - 1) + 2147483648.0;
			t = Melder_stopwatch ();   // 0.96
			MelderInfo_writeLine (Melder_double (sum));
		} break;
		case kPraatTests_TIME_STRING_MELDER: {
			MelderString string = { 0 };
			for (int64 i = 1; i <= n; i ++)
				MelderString_append (& string, L"abc");
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP: {
			std::wstring s = L"";
			for (int64 i = 1; i <= n; i ++)
				s += L"abc";
			t = Melder_stopwatch ();
		} break;
	}
	MelderInfo_writeLine (Melder_single (t / n * 1e9), L" nanoseconds");
	MelderInfo_writeLine (Melder_integer (5 + 6 << 1));
	MelderInfo_close ();
	return 1;
}

/* End of file Praat_tests.cpp */
