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
#include <functional>

static void testAutoData (autoDaata data) {
	fprintf (stderr, "testAutoData: %p %p\n", data.get(), data -> name);
}
static void testAutoDataRef (autoDaata& data) {
	fprintf (stderr, "testAutoDataRef: %p %p\n", data.get(), data -> name);
}
static void testData (Daata data) {
	fprintf (stderr, "testData: %p %s\n", data, Melder_peek32to8 (data -> name));
}
static autoDaata newAutoData () {
	autoDaata data (Thing_new (Daata));
	return data;
}

#if defined (macintosh) && useCarbon
	#define Thing_IF_CLASS_IS_DERIVED_FROM(A,B)   /* not typesafe */
#else
	#define Thing_IF_CLASS_IS_DERIVED_FROM(A,B)  , class = typename std::enable_if<std::is_base_of<B,A>::value>::type
#endif

#if 1
template <class T> class Func {
	typedef void (*Function) (Daata);
	Function func;
public:
	Func (void (*funcT) (T*)) : func ((Function) funcT) { }
	template <class Y  Thing_IF_CLASS_IS_DERIVED_FROM(Y,T)>
		Func (void (*funcY) (Y*)) : func ((Function) funcY) { };
	void operator () (Daata data) { return func (data); }
};
#endif

int Praat_tests (int itest, char32 *arg1, char32 *arg2, char32 *arg3, char32 *arg4) {
	int64 n = Melder_atoi (arg1);
	double t = 0.0;
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
			int64 sum = 0;
			#if ! useCarbon
			for (int64 i = 1; i <= n; i ++)
				sum += i * (i - 1) * (i - 2);
			#endif
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
				sum += (uint32) fi;
			t = Melder_stopwatch ();   // 2.59   // 1.60
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_EXTERN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32) ((int32) (fi - 2147483648.0) + 2147483647L + 1);
			t = Melder_stopwatch ();   // 1.60
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_BUILTIN: {
			double sum = 0.0;
			uint32 nu = (uint32) n;
			for (uint32 iu = 1; iu <= nu; iu ++)
				sum += (double) iu;
			t = Melder_stopwatch ();   // 1.35
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_EXTERN: {
			double sum = 0.0;
			uint32 nu = (uint32) n;
			for (uint32 iu = 1; iu <= nu; iu ++)
				sum += (double) (int32) (iu - 2147483647L - 1) + 2147483648.0;
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
		case kPraatTests_TIME_STRING_MELDER_32_ALLOC: {
			char32 word [] { U"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				autoMelderString string;
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
		case kPraatTests_THING_AUTO: {
			int numberOfThingsBefore = Thing_getTotalNumberOfThings ();
			{
				Melder_casual (U"1\n");
				autoDaata data = Thing_new (Daata);
				Thing_setName (data.get(), U"hello");
				Melder_casual (U"2\n");
				testData (data.peek());
				testAutoData (data.move());
				autoDaata data18 = Thing_new (Daata);
				testAutoData (data18.move());
				fprintf (stderr, "3\n");
				autoDaata data2 = newAutoData ();
				fprintf (stderr, "4\n");
				autoDaata data3 = newAutoData ();
				fprintf (stderr, "5\n");
				//data2 = data;   // disabled l-value copy assignment from same class
				fprintf (stderr, "6\n");
				autoOrdered ordered = Thing_new (Ordered);
				fprintf (stderr, "7\n");
				//data = ordered;   // disabled l-value copy assignment from subclass
				data = ordered.move();
				//ordered = data;   // disabled l-value copy assignment from superclass
				//ordered = data.move();   // assignment from superclass to subclass is rightfully refused by compiler
				fprintf (stderr, "8\n");
				data2 = newAutoData ();
				fprintf (stderr, "8a\n");
				autoDaata data5 = newAutoData ();
				fprintf (stderr, "8b\n");
				data2 = data5.move();
				fprintf (stderr, "9\n");
				//ordered = data;   // rightfully refused by compiler
				fprintf (stderr, "10\n");
				//autoOrdered ordered2 = Thing_new (Daata);   // rightfully refused by compiler
				fprintf (stderr, "11\n");
				autoDaata data4 = Thing_new (Ordered);   // constructor
				fprintf (stderr, "12\n");
				//autoDaata data6 = data4;   // disabled l-value copy constructor from same class
				fprintf (stderr, "13\n");
				autoDaata data7 = data4.move();
				fprintf (stderr, "14\n");
				autoOrdered ordered3 = Thing_new (Ordered);
				autoDaata data8 = ordered3.move();
				fprintf (stderr, "15\n");
				//autoDaata data9 = ordered;   // disabled l-value copy constructor from subclass
				fprintf (stderr, "16\n");
				autoDaata data10 = data7.move();
				fprintf (stderr, "17\n");
				autoDaata data11 = Thing_new (Daata);   // constructor, move assignment, null destructor
				fprintf (stderr, "18\n");
				data11 = Thing_new (Ordered);
				fprintf (stderr, "19\n");
				testAutoDataRef (data11);
				fprintf (stderr, "20\n");
				data11 = nullptr;
				fprintf (stderr, "21\n");
			}
			int numberOfThingsAfter = Thing_getTotalNumberOfThings ();
			fprintf (stderr, "Number of things: before %d, after %d\n", numberOfThingsBefore, numberOfThingsAfter);
			#if 1
				typedef void (*DataFunc) (Daata);
				typedef void (*OrderedFunc) (Ordered);
				DataFunc dataFun;
				OrderedFunc orderedFun;
				Func<structDaata> dataFun2 (dataFun);
				Func<structOrdered> orderedFun2 (orderedFun);
				Func<structDaata> dataFun3 (orderedFun);
				//Func<structOrdered> orderedFun3 (dataFun);   // rightfully refused by compiler
				autoDaata data;
				dataFun3 (data.get());
			#endif
		} break;
	}
	MelderInfo_writeLine (Melder_single (t / n * 1e9), U" nanoseconds");
	MelderInfo_close ();
	return 1;
}

/* End of file Praat_tests.cpp */
