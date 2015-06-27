/* Praat_tests.cpp */
/* Paul Boersma, August 2, 2001 */
/* December 10, 2006: MelderInfo */
/* November 5, 2007: wchar */
/* 21 March 2009: modern enums */
/* 24 May 2011: C++ */
/* 5 June 2015: char32 */

#include "Praat_tests.h"

#include "enums_getText.h"
#include "Praat_tests_enums.h"
#include "enums_getValue.h"
#include "Praat_tests_enums.h"
#include <string>

#define UTF32_C(string) \
	({ static const wchar_t *_static_utf32_string = Melder_utf8ToStr32 (string); _static_utf32_string; })

constexpr char32 greeting [] {U"Hello?"};


int Praat_tests (int itest, char32 *arg1, char32 *arg2, char32 *arg3, char32 *arg4) {
	int64 n = Melder_atoi (arg1);
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
			long m = Melder_atoi (arg2);
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
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT: {
			double sum = 0.0, fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += fi * (fi - 1.0) * (fi - 2.0);
			t = Melder_stopwatch ();
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_BUILTIN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32_t) fi;
			t = Melder_stopwatch ();   // 2.59   // 1.60
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_EXTERN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32_t) ((int32_t) (fi - 2147483648.0) + 2147483647L + 1);
			t = Melder_stopwatch ();   // 1.60
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_BUILTIN: {
			double sum = 0.0;
			uint32_t nu = (uint32_t) n;
			for (uint32_t iu = 1; iu <= nu; iu ++)
				sum += (double) iu;
			t = Melder_stopwatch ();   // 1.35
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_EXTERN: {
			double sum = 0.0;
			uint32_t nu = (uint32_t) n;
			for (uint32_t iu = 1; iu <= nu; iu ++)
				sum += (double) (int32_t) (iu - 2147483647L - 1) + 2147483648.0;
			t = Melder_stopwatch ();   // 0.96
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_STRING_MELDER_32: {
			autoMelderString string;
			for (int64 i = 1; i <= n; i ++) {
				MelderString_copy (& string, U"abc");
				for (int j = 1; j <= 30; j ++)
					MelderString_append (& string, U"abc");
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_S: {
			std::string s = "";
			for (int64 i = 1; i <= n; i ++) {
				s = "abs";
				for (int j = 1; j <= 30; j ++)
					s += "abc";
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_C: {
			std::basic_string<char> s = "";
			for (int64 i = 1; i <= n; i ++) {
				s = "abc";
				for (int j = 1; j <= 30; j ++)
					s += "abc";
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_WS: {
			std::wstring s = L"";
			for (int64 i = 1; i <= n; i ++) {
				s = L"abc";
				for (int j = 1; j <= 30; j ++)
					s += L"abc";
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_WC: {
			std::basic_string<wchar_t> s = L"";
			for (int64 i = 1; i <= n; i ++) {
				s = L"abc";
				for (int j = 1; j <= 30; j ++)
					s += L"abc";
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_32: {
			std::basic_string<char32_t> s = U"";
			for (int64 i = 1; i <= n; i ++) {
				s = U"abc";
				for (int j = 1; j <= 30; j ++)
					s += U"abc";
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_U32STRING: {
			#if ! defined (macintosh) || ! useCarbon
			std::u32string s = U"";
			volatile int jj = 30;
			for (int64 i = 1; i <= n; i ++) {
				s = U"abc";
				for (int j = 1; j <= jj; j ++)
					s += U"abc";
			}
			#endif
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRCPY: {
			char buffer [100];
			for (int64 i = 1; i <= n; i ++) {
				strcpy (buffer, "abc");
				for (int j = 1; j <= 30; j ++)
					strcpy (buffer + strlen (buffer), "abc");
			}
			t = Melder_stopwatch ();
			MelderInfo_writeLine (Melder_peek8to32 (buffer));
		} break;
		case kPraatTests_TIME_STR32CPY: {
			char32 buffer [100];
			for (int64 i = 1; i <= n; i ++) {
				str32cpy (buffer, U"abc");
				for (int j = 1; j <= 30; j ++)
					str32cpy (buffer + str32len (buffer), U"abc");
			}
			t = Melder_stopwatch ();
			MelderInfo_writeLine (buffer);
		} break;
		case kPraatTests_TIME__STR32CPY: {
			char32 buffer [100];
			for (int64 i = 1; i <= n; i ++) {
				_str32cpy (buffer, U"abc");
				for (int j = 1; j <= 30; j ++)
					_str32cpy (buffer + _str32len (buffer), U"abc");
			}
			t = Melder_stopwatch ();
			MelderInfo_writeLine (buffer);
		} break;
	}
	MelderInfo_writeLine (Melder_single (t / n * 1e9), U" nanoseconds");
	MelderInfo_close ();
	return 1;
}

/* End of file Praat_tests.cpp */
