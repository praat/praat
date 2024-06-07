/* wctype_portable.h
 *
 * Copyright (C) 2024 Paul Boersma
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

/*
	The behaviour of several functions in `wctype.h` is locale-dependent.
	Praat is international software in the sense that it should be able
	to be used for exchange between researchers internationally.
	This means that its behaviour should not depend on localized settings.
	We therefore replace the `wctype.h` functions by fixed international ones,
	making use of the Unicode Character Database,
	with a fixed interpretation of the semantics of Unicode and scripts.
	Any divergences will have to be expressed explicitly.
*/

#include <wchar.h>

#ifdef __cplusplus
extern "C"
{
#endif

int iswalpha_portable (wint_t wc);   // implemented by Melder_isLetter()
int iswalnum_portable (wint_t wc);   // implemented by Melder_isAlphanumeric()
int iswpunct_portable (wint_t wc);   // implemented by Melder_isPunctuationOrSymbol()
int iswdigit_portable (wint_t wc);   // implemented by Melder_isDecimalNumber()
int iswspace_portable (wint_t wc);   // implemented by Melder_isHorizontalOrVerticalSpace()
int iswlower_portable (wint_t wc);   // implemented by Melder_isLowerCaseLetter()
int iswupper_portable (wint_t wc);   // implemented by Melder_isUpperCaseLetter()

#ifndef linux
	// on Linux this is unfortunately not possible, because of "::iswalpha"
	#define iswalpha  DONT_USE_ISWALPHA
	#define iswalnum  DONT_USE_ISWALNUM
	#define iswpunct  DONT_USE_ISWPUNCT
	#define iswdigit  DONT_USE_ISWDIGIT
	#define iswspace  DONT_USE_ISWSPACE
	#define iswlower  DONT_USE_ISWLOWER
	#define iswupper  DONT_USE_ISWUPPER
#endif

#ifdef __cplusplus
}
#endif

/* End of file wctype_portable.h */
