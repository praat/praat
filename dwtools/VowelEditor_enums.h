/* VowelEditor_enums.h
 *
 * Copyright (C) 2019-2020 David Weenink
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

enums_begin (kVowelEditor_speakerType, 0)
	enums_add (kVowelEditor_speakerType, 0, MAN, U"Man")
	enums_add (kVowelEditor_speakerType, 1, WOMAN, U"Woman")
	enums_add (kVowelEditor_speakerType, 2, CHILD, U"Child")
	enums_add (kVowelEditor_speakerType, 3, UNKNOWN, U"Unknown")
enums_end (kVowelEditor_speakerType, 3, UNKNOWN)

enums_begin (kVowelEditor_marksDataSet, 1)
	enums_add (kVowelEditor_marksDataSet, 1, AMERICAN_ENGLISH, U"American English")
	enums_add (kVowelEditor_marksDataSet, 2, DUTCH, U"Dutch")
	enums_add (kVowelEditor_marksDataSet, 3, NONE, U"None")
	enums_add (kVowelEditor_marksDataSet, 4, OTHER, U"Other")
enums_end (kVowelEditor_marksDataSet, 4, OTHER)

enums_begin (kVowelEditor_frequencyScale, 1)
	enums_add (kVowelEditor_frequencyScale, 1, LOGARITHMIC, U"Logaritmic")
enums_end (kVowelEditor_frequencyScale, 1, LOGARITHMIC)

/* End of file VowelEditor_enums.h */
