/* Praat_tests.cpp
 *
 * Copyright (C) 2001-2012,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/* December 10, 2006: MelderInfo */
/* November 5, 2007: wchar */
/* 21 March 2009: modern enums */
/* 24 May 2011: C++ */
/* 5 June 2015: char32 */

#include "Praat_tests.h"

#include "Graphics.h"
#include "praat.h"
#include "NUM2.h"
#include "Sound.h"

#include "enums_getText.h"
#include "Praat_tests_enums.h"
#include "enums_getValue.h"
#include "Praat_tests_enums.h"
#include <string>

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
static int length (const char32 *s) {
	int result = str32len (s);
	Melder_free (s);
	return result;
}

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
			for (int64 i = 1; i <= n; i ++)
				sum += i * (i - 1) * (i - 2);
			t = Melder_stopwatch ();
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT: {
			double sum = 0.0, fn = n;
			for (double fi = 1.0; fi <= fn; fi ++)
				sum += fi * (fi - 1.0) * (fi - 2.0);
			t = Melder_stopwatch ();   // 2.02 ns
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_BUILTIN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32) fi;
			t = Melder_stopwatch ();   // 1.45 ns
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_FLOAT_TO_UNSIGNED_EXTERN: {
			uint64_t sum = 0;
			double fn = n;
			for (double fi = 1.0; fi <= fn; fi = fi + 1.0)
				sum += (uint32) ((int32) (fi - 2147483648.0) + 2147483647L + 1);
			t = Melder_stopwatch ();   // 1.47 ns
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_BUILTIN: {
			double sum = 0.0;
			uint32 nu = (uint32) n;
			for (uint32 iu = 1; iu <= nu; iu ++)
				sum += (double) iu;
			t = Melder_stopwatch ();   // 0.88 ns
			MelderInfo_writeLine (sum);
		} break;
		case kPraatTests_TIME_UNSIGNED_TO_FLOAT_EXTERN: {
			double sum = 0.0;
			uint32 nu = (uint32) n;
			for (uint32 iu = 1; iu <= nu; iu ++)
				sum += (double) (int32) (iu - 2147483647L - 1) + 2147483648.0;
			t = Melder_stopwatch ();   // 0.87 ns
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
			std::u32string s = U"";
			char32 word [] { U"abc" };
			word [2] = NUMrandomInteger ('a', 'z');
			for (int64 i = 1; i <= n; i ++) {
				s = word;
				for (int j = 1; j <= 30; j ++)
					s += word;
			}
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
		case kPraatTests_TIME_UNDEFINED_NUMUNDEFINED: {
			bool isAllDefined = true;
			double x = 0.0;
			for (int64 i = 1; i <= n; i ++) {
				x += (double) i;
				isAllDefined &= ( x != undefined );
			}
			t = Melder_stopwatch ();   // 0.86 ns
			MelderInfo_writeLine (isAllDefined, U" ", x);
		} break;
		case kPraatTests_TIME_UNDEFINED_ISINF_OR_ISNAN: {
			bool isAllDefined = true;
			double x = 0.0;
			for (int64 i = 1; i <= n; i ++) {
				x += (double) i;
				isAllDefined &= ! isinf (x) && ! isnan (x);
			}
			t = Melder_stopwatch ();   // 1.29 ns
			MelderInfo_writeLine (isAllDefined, U" ", x);
		} break;
		case kPraatTests_TIME_UNDEFINED_0x7FF: {
			bool isAllDefined = true;
			double x = 0.0;
			for (int64 i = 1; i <= n; i ++) {
				x += (double) i;
				isAllDefined &= ((* (uint64_t *) & x) & 0x7FF0000000000000) != 0x7FF0000000000000;
			}
			t = Melder_stopwatch ();   // 0.90 ns
			MelderInfo_writeLine (isAllDefined, U" ", x);
		} break;
		case kPraatTests_TIME_INNER: {
			int size = 100;
			autonumvec x { size, false }, y { size, false };
			for (int64 i = 1; i <= size; i ++) {
				x [i] = NUMrandomGauss (0.0, 1.0);
				y [i] = NUMrandomGauss (0.0, 1.0);
			}
			double z = 0.0;
			for (int64 i = 1; i <= n; i ++) {
				z += inner_scalar (x.get(), y.get());
			}
			t = Melder_stopwatch () / size;   // 0.91 ns per multiplication
		} break;
		case kPraatTests_TIME_OUTER_NUMMAT: {
			int nrow = 100, ncol = 100;
			numvec x { NUMvector<double> (1, nrow), nrow }, y { NUMvector<double> (1, ncol), ncol };
			for (int64 i = 1; i <= nrow; i ++)
				x.at [i] = NUMrandomGauss (0.0, 1.0);
			for (int64 i = 1; i <= ncol; i ++)
				y.at [i] = NUMrandomGauss (0.0, 1.0);
			for (int64 i = 1; i <= n; i ++) {
				const autonummat mat = outer_nummat (x, y);
			}
			t = Melder_stopwatch () / nrow / ncol;   // 0.29 ns, i.e. less than one clock cycle per cell
			NUMvector_free (x.at, 1);
			NUMvector_free (y.at, 1);
		} break;
		case kPraatTests_CHECK_INVFISHERQ: {
			MelderInfo_writeLine (NUMinvFisherQ (0.003, 1, 100000));
		} break;
		case kPraatTests_TIME_AUTOSTRING: {
			const char32 *strings [6] = { U"ghdg", U"jhd", U"hkfjjd", U"fhfj", U"jhksfd", U"hfjs" };
			int64 sumOfLengths = 0;
			for (int64 i = 1; i <= n; i ++) {
				int istring = i % 6;
				autostring32 s = Melder_dup (strings [istring]);
				sumOfLengths += length (s.transfer());
			}
			t = Melder_stopwatch ();   // 72 ns (but 152 bytes more)
			MelderInfo_writeLine (sumOfLengths);
		} break;
		case kPraatTests_TIME_CHAR32: {
			const char32 *strings [6] = { U"ghdg", U"jhd", U"hkfjjd", U"fhfj", U"jhksfd", U"hfjs" };
			int64 sumOfLengths = 0;
			for (int64 i = 1; i <= n; i ++) {
				int istring = i % 6;
				char32 *s = Melder_dup (strings [istring]);
				sumOfLengths += length (s);
			}
			t = Melder_stopwatch ();   // 72 ns
			MelderInfo_writeLine (sumOfLengths);
		} break;
		case kPraatTests_TIME_STDEV: {
			integer size = 10000;
			autonumvec x { size, false };
			for (integer i = 1; i <= size; i ++)
				x.at [i] = NUMrandomGauss (0.0, 1.0);
			double z = 0.0;
			for (int64 i = 1; i <= n; i ++) {
				real stdev = stdev_scalar (x.get());
				z += stdev;
			}
			t = Melder_stopwatch () / size;
			MelderInfo_writeLine (z);
		} break;
		case kPraatTests_THING_AUTO: {
			int numberOfThingsBefore = theTotalNumberOfThings;
			{
				Melder_casual (U"1\n");
				autoDaata data = Thing_new (Daata);
				Thing_setName (data.get(), U"hello");
				Melder_casual (U"2\n");
				testData (data.get());
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
				//data11 = nullptr;   // disabled implicit assignment of pointer to autopointer
				fprintf (stderr, "21\n");
			}
			int numberOfThingsAfter = theTotalNumberOfThings;
			fprintf (stderr, "Number of things: before %d, after %d\n", numberOfThingsBefore, numberOfThingsAfter);
			#if 0
				MelderCallback<void,structDaata>::FunctionType f;
				typedef void (*DataFunc) (Daata);
				typedef void (*OrderedFunc) (Ordered);
				DataFunc dataFun;
				OrderedFunc orderedFun;
				MelderCallback<void,structDaata> dataFun2 (dataFun);
				MelderCallback<void,structOrdered> orderedFun2 (orderedFun);
				MelderCallback<void,structDaata> dataFun3 (orderedFun);
				//MelderCallback<void,structOrdered> orderedFun3 (dataFun);   // rightfully refused by compiler
				autoDaata data = Thing_new (Daata);
				dataFun3 (data.get());
			#endif
			{
				#if 0
				autoMelderAsynchronous x;
				//autoMelderAsynchronous y = x;   // deleted copy constructor
				autoMelderAsynchronous y = x.move();   // defined move constructor
				//x = y;   // deleted copy assignment
				x = y.move();   // defined move assignment
				autonumvec a;
				autonumvec b = a.move();
				const autonumvec c;
				const autonumvec d { };
				double *e;
				const autonumvec f { e, 10 };
				const autonumvec g { 100, true };
				//return f;   // call to deleted constructor
				#endif
				autoSound sound = Sound_create (1, 0.0, 1.0, 10000, 0.0001, 0.0);
				sound = Sound_create (1, 0.0, 1.0, 10000, 0.0001, 0.00005);
				Melder_casual (U"hello ", sound -> dx);
			}
		} break;
	}
	MelderInfo_writeLine (Melder_single (t / n * 1e9), U" nanoseconds");
	MelderInfo_close ();
	return 1;
}

/* More compiler stuff */
#if 1
/*
	Trying out inheritance without encapsulation...
	Advantage: everything is a method; therefore, the Law of Demeter is satisfied idiomatically
	Disadvantage: problematic encapsulation
*/
Thing_declare (Matrix_);
Thing_declare (Sound_);
Thing_declare (Pitch_);

/*
	The following two sets of files have to be included
	in Pitch_to_Sound.cpp as well as in Sound_to_Pitch.cpp,
	but can come in either order:
*/

/*
	Set 1: Pitch.h
*/
struct structPitch_ : structThing {
	double f0;
	autoSound_ toSound ();   // anti-encapsulation
};

/*
	Set 2: Matrix.h followed by Sound.h
*/
struct structMatrix_ : structThing {
	private: double x, y;
	public: double getX () { return x; }
	void setX (double newX) { x = newX; }
};
struct structSound_ : public structMatrix_ {   // the definition of structSound_ requires the prior definition of structMatrix_
	autoPitch_ toPitch ();   // anti-encapsulation
};

/*
	The following two files are independent of each other:
*/

/*
	Pitch_to_Sound.cpp:
	#include "Pitch.h"
	#include "Sound.h"
*/
autoSound_ structPitch_::toSound () {   // this requires the prior definition of structPitch_ and the prior declaration of structSound_
	autoSound_ result = autoSound_ ();
	result -> setX (f0);   // this requires the prior definition of structSound_ and structMatrix_
	return result;
}

/*
	Sound_to_Pitch.cpp:
	#include "Sound.h"
	#include "Pitch.h"
*/
autoPitch_ structSound_::toPitch () {   // this requires the prior definition of structSound_ and the prior declaration of structPitch_
	double x = getX ();   // this requires the prior definition of structSound_ and structMatrix_
	autoPitch_ result = autoPitch_ ();
	result -> f0 = x;   // this requires the prior definition of structPitch_
	return result;
}

#endif

/* End of file Praat_tests.cpp */
