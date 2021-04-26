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

enums_begin (kUi_realVectorFormat, 1)
	enums_add (kUi_realVectorFormat, 1, WHITESPACE_SEPARATED_, U"(whitespace-separated)")
	enums_add (kUi_realVectorFormat, 2, FORMULA_, U"(formula)")
enums_end (kUi_realVectorFormat, 2, WHITESPACE_SEPARATED_)

enums_begin (kUi_integerVectorFormat, 1)
	enums_add (kUi_integerVectorFormat, 1, WHITESPACE_SEPARATED_, U"(whitespace-separated)")
	enums_add (kUi_integerVectorFormat, 2, RANGES_, U"(ranges)")
	enums_add (kUi_integerVectorFormat, 3, FORMULA_, U"(formula)")
enums_end (kUi_integerVectorFormat, 3, RANGES_)

enums_begin (kUi_realMatrixFormat, 1)
	enums_add (kUi_realMatrixFormat, 1, ONE_ROW_PER_LINE_, U"(one row per line)")
	enums_add (kUi_realMatrixFormat, 2, FORMULA_, U"(formula)")
enums_end (kUi_realMatrixFormat, 2, ONE_ROW_PER_LINE_)

enums_begin (kUi_stringArrayFormat, 1)
	enums_add (kUi_stringArrayFormat, 1, WHITESPACE_SEPARATED_, U"(whitespace-separated)")
	enums_add (kUi_stringArrayFormat, 2, COMMA_SEPARATED_, U"(comma-separated)")
	enums_add (kUi_stringArrayFormat, 3, SEMICOLON_SEPARATED_, U"(semicolon-separated)")
	enums_add (kUi_stringArrayFormat, 4, PIPE_SEPARATED_, U"(pipe-separated)")
	enums_add (kUi_stringArrayFormat, 5, ONE_PER_LINE_, U"(one per line)")
	enums_add (kUi_stringArrayFormat, 6, FORMULA_, U"(formula)")
enums_end (kUi_stringArrayFormat, 6, WHITESPACE_SEPARATED_)

/* End of file Ui_enums.h */

