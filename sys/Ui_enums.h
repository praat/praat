/* Ui_enums.h
 *
 * Copyright (C) 2021 Paul Boersma
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

enums_begin (kUi_numericVectorFormat, 1)
	enums_add (kUi_numericVectorFormat, 1, ENUMERATE_, U"(enumerate)")
	enums_add (kUi_numericVectorFormat, 2, FORMULA_, U"(formula)")
enums_end (kUi_numericVectorFormat, 2, ENUMERATE_)

enums_begin (kUi_numericMatrixFormat, 1)
	enums_add (kUi_numericMatrixFormat, 1, ONE_ROW_PER_LINE_, U"(one row per line)")
	enums_add (kUi_numericMatrixFormat, 2, FORMULA_, U"(formula)")
enums_end (kUi_numericMatrixFormat, 2, ONE_ROW_PER_LINE_)

enums_begin (kUi_stringArrayFormat, 1)
	enums_add (kUi_stringArrayFormat, 1, SPLIT_BY_WHITESPACE_, U"(split by whitespace)")
	enums_add (kUi_stringArrayFormat, 2, SPLIT_BY_COMMAS_, U"(split by commas)")
	enums_add (kUi_stringArrayFormat, 3, SPLIT_BY_SEMICOLONS_, U"(split by semicolons)")
	enums_add (kUi_stringArrayFormat, 4, SPLIT_BY_PIPES_, U"(split by pipes)")
	enums_add (kUi_stringArrayFormat, 5, ONE_PER_LINE_, U"(one per line)")
	enums_add (kUi_stringArrayFormat, 6, FORMULA_, U"(formula)")
enums_end (kUi_stringArrayFormat, 6, SPLIT_BY_WHITESPACE_)

/* End of file Ui_enums.h */

