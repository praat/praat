/* melder_enums.h
 *
 * Copyright (C) 2007 Paul Boersma
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

/*
 * pb 2007/12/03
 */

enums_begin (kMelder_number, 1)
	enums_add (kMelder_number, EQUAL_TO, L"equal to", 1)
	enums_add (kMelder_number, NOT_EQUAL_TO, L"not equal to", 2)
	enums_add (kMelder_number, LESS_THAN, L"less than", 3)
	enums_add (kMelder_number, LESS_THAN_OR_EQUAL_TO, L"less than or equal to", 4)
	enums_add (kMelder_number, GREATER_THAN, L"greater than", 5)
	enums_add (kMelder_number, GREATER_THAN_OR_EQUAL_TO, L"greater than or equal to", 6)
	/* For reading old preferences files: */
	enums_alt (kMelder_number, EQUAL_TO, L"1")
	enums_alt (kMelder_number, NOT_EQUAL_TO, L"2")
	enums_alt (kMelder_number, LESS_THAN, L"3")
	enums_alt (kMelder_number, LESS_THAN_OR_EQUAL_TO, L"4")
	enums_alt (kMelder_number, GREATER_THAN, L"5")
	enums_alt (kMelder_number, GREATER_THAN_OR_EQUAL_TO, L"6")
enums_end (kMelder_number, EQUAL_TO, 6)

enums_begin (kMelder_string, 1)
	enums_add (kMelder_string, EQUAL_TO, L"is equal to", 1)
	enums_add (kMelder_string, NOT_EQUAL_TO, L"is not equal to", 2)
	enums_add (kMelder_string, CONTAINS, L"contains", 3)
	enums_add (kMelder_string, DOES_NOT_CONTAIN, L"does not contain", 4)
	enums_add (kMelder_string, STARTS_WITH, L"starts with", 5)
	enums_add (kMelder_string, DOES_NOT_START_WITH, L"does not start with", 6)
	enums_add (kMelder_string, ENDS_WITH, L"ends with", 7)
	enums_add (kMelder_string, DOES_NOT_END_WITH, L"does not end with", 8)
	enums_add (kMelder_string, MATCH_REGEXP, L"matches (regex)", 9)
	/* For reading old preferences files: */
	enums_alt (kMelder_string, EQUAL_TO, L"1")
	enums_alt (kMelder_string, NOT_EQUAL_TO, L"2")
	enums_alt (kMelder_string, CONTAINS, L"3")
	enums_alt (kMelder_string, DOES_NOT_CONTAIN, L"4")
	enums_alt (kMelder_string, STARTS_WITH, L"5")
	enums_alt (kMelder_string, DOES_NOT_START_WITH, L"6")
	enums_alt (kMelder_string, ENDS_WITH, L"7")
	enums_alt (kMelder_string, DOES_NOT_END_WITH, L"8")
	enums_alt (kMelder_string, MATCH_REGEXP, L"9")
enums_end (kMelder_string, EQUAL_TO, 9)

/* End of file melder_enums.h */
