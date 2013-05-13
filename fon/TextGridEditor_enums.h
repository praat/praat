/* TextGridEditor_enums.h
 *
 * Copyright (C) 1992-2007,2013 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

enums_begin (kTextGridEditor_showNumberOf, 1)
	enums_add (kTextGridEditor_showNumberOf, 1, NOTHING, L"nothing")
	enums_add (kTextGridEditor_showNumberOf, 2, INTERVALS_OR_POINTS, L"intervals or points")
	enums_add (kTextGridEditor_showNumberOf, 3, NONEMPTY_INTERVALS_OR_POINTS, L"non-empty intervals or points")
enums_end (kTextGridEditor_showNumberOf, 3, INTERVALS_OR_POINTS)

enums_begin (kTextGridEditor_language, 1)
	enums_add (kTextGridEditor_language, 1, AFRIKAANS, L"Afrikaans")
	enums_add (kTextGridEditor_language, 2, AKAN, L"Akan")
	enums_add (kTextGridEditor_language, 3, ALBANIAN, L"Albanian")
	enums_add (kTextGridEditor_language, 4, AMHARIC, L"Amharic")
	enums_add (kTextGridEditor_language, 5, ARMENIAN, L"Armenian")
	enums_add (kTextGridEditor_language, 6, ARMENIAN_WEST, L"Armenian (West)")
	enums_add (kTextGridEditor_language, 7, AZERBAIJANI, L"Azerbaijani")
	enums_add (kTextGridEditor_language, 8, BOSNIAN, L"Bosnian")
	enums_add (kTextGridEditor_language, 9, BULGARIAN, L"Bulgarian")
	enums_add (kTextGridEditor_language, 10, CANTONESE, L"Cantonese")
	enums_add (kTextGridEditor_language, 11, CATALAN, L"Catalan")
	enums_add (kTextGridEditor_language, 12, CROATIAN, L"Croatian")
	enums_add (kTextGridEditor_language, 13, CZECH, L"Czech")
	enums_add (kTextGridEditor_language, 14, DARI, L"Danish")
	enums_add (kTextGridEditor_language, 15, DEFAULT_LANGUAGE, L"Default language")
	enums_add (kTextGridEditor_language, 16, DIVEHI, L"Divehi")
	enums_add (kTextGridEditor_language, 17, DUTCH, L"Dutch-test")
	enums_add (kTextGridEditor_language, 18, ENGLISH_AMERICAN, L"English (American)")
	enums_add (kTextGridEditor_language, 19, ENGLISH_RP, L"English (RP)")
	enums_add (kTextGridEditor_language, 20, ENGLISH_SCOTLAND, L"English (Scotland)")
	enums_add (kTextGridEditor_language, 21, ENGLISH_SOUTHERN_ENGLAND, L"English (Southern England)")
	enums_add (kTextGridEditor_language, 22, ENGLISH_WEST_INDIES, L"English (West Indies)")
	enums_add (kTextGridEditor_language, 22, ENGLISH_WEST_MIDLANDS, L"English (West Midlands)")
	enums_add (kTextGridEditor_language, 23, PORTUGUESE_BRAZILIAN, L"Portuguese (Brazilian)")
	enums_add (kTextGridEditor_language, 24, PORTUGUESE_EUROPEAN, L"Portuguese (European)")
enums_end (kTextGridEditor_language, 2, ENGLISH_AMERICAN)

/* End of file TextGridEditor_enums.h */
