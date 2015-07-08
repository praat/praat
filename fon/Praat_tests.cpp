/* Praat_tests.cpp */
/* Paul Boersma, August 2, 2001 */
/* December 10, 2006: MelderInfo */
/* November 5, 2007: wchar */
/* 21 March 2009: modern enums */
/* 24 May 2011: C++ */
/* 5 June 2015: char32 */

#include "Praat_tests.h"

#include "Graphics.h"
#include "praat.h"

#include "enums_getText.h"
#include "Praat_tests_enums.h"
#include "enums_getValue.h"
#include "Praat_tests_enums.h"
#include <string>

int Praat_tests (int itest, char32 *arg1, char32 *arg2, char32 *arg3, char32 *arg4) {
	int64 n = Melder_atoi (arg1);
	double t;
	(void) arg1;
	(void) arg2;
	(void) arg3;
	(void) arg4;
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
			char32 word [] { U"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				MelderString_copy (& string, word);
				for (int j = 1; j <= 30; j ++)
					MelderString_append (& string, word);
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_S: {
			std::string s = "";
			char word [] { "abc" };
			word [2] = (char) NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_C: {
			std::basic_string<char> s = "";
			char word [] { "abc" };
			word [2] = (char) NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_WS: {
			std::wstring s = L"";
			wchar_t word [] { L"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_WC: {
			std::basic_string<wchar_t> s = L"";
			wchar_t word [] { L"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_32: {
			std::basic_string<char32_t> s = U"";
			char32 word [] { U"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRING_CPP_U32STRING: {
			#if ! defined (macintosh) || ! useCarbon
			std::u32string s = U"";
			char32 word [] { U"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
			#endif
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STRCPY: {
			char buffer [100];
			char word [] { "abc" };
			word [2] = (char) NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				strcpy (buffer, word);
				for (int j = 1; j <= 30; j ++)
					strcpy (buffer + strlen (buffer), word);
			}
			t = Melder_stopwatch ();
			MelderInfo_writeLine (Melder_peek8to32 (buffer));
		} break;
		case kPraatTests_TIME_WCSCPY: {
			wchar_t buffer [100];
			wchar_t word [] { L"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				wcscpy (buffer, word);
				for (int j = 1; j <= 30; j ++)
					wcscpy (buffer + wcslen (buffer), word);
			}
			t = Melder_stopwatch ();
		} break;
		case kPraatTests_TIME_STR32CPY: {
			char32 buffer [100];
			char32 word [] { U"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				str32cpy (buffer, word);
				for (int j = 1; j <= 30; j ++)
					str32cpy (buffer + str32len (buffer), word);
			}
			t = Melder_stopwatch ();
			MelderInfo_writeLine (buffer);
		} break;
		case kPraatTests_TIME_GRAPHICS_TEXT_TOP: {
			autoPraatPicture picture;
			for (int64 i = 1; i <= n; i ++) {
				Graphics_textTop (GRAPHICS, false, U"hello world");
			}
			t = Melder_stopwatch ();
		} break;
	}
	MelderInfo_writeLine (Melder_single (t / n * 1e9), U" nanoseconds");
	MelderInfo_close ();
	return 1;
}

/* End of file Praat_tests.cpp */
