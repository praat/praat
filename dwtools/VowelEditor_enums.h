/* VowelEditor_enums.h
 *
 * Copyright (C) 2019 David Weenink
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

enums_begin (kVowelEditor_speakersType, 1)
	enums_add (kVowelEditor_speakersType, 1, Man, U"Man")
	enums_add (kVowelEditor_speakersType, 2, Woman, U"Women")
	enums_add (kVowelEditor_speakersType, 3, Child, U"Child")
enums_end (kVowelEditor_speakersType, 3, Man)

enums_begin (kVowelEditor_marksDataSet, 1)
	enums_add (kVowelEditor_marksDataSet, 1, AmericanEnglish, U"American English")
	enums_add (kVowelEditor_marksDataSet, 2, Dutch, U"Dutch")
	enums_add (kVowelEditor_marksDataSet, 3, None, U"None")
	enums_add (kVowelEditor_marksDataSet, 4, Other, U"Other")
enums_end (kVowelEditor_marksDataSet, 4, AmericanEnglish)

/* End of file VowelEditor_enums.h */
