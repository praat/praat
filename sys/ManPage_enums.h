/* ManPage_enums.h
 *
 * Copyright (C) 1996-2005,2009,2013,2015,2016,2021,2023,2024 Paul Boersma
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
	KEEP IN SYNC WITH stylesInfo IN ManPages_toHtml.cpp
*/
enums_begin (kManPage_type, 1)
	enums_add (kManPage_type, 1, INTRO, U"intro")
	enums_add (kManPage_type, 2, ENTRY, U"entry")
	enums_add (kManPage_type, 3, NORMAL, U"normal")
	enums_add (kManPage_type, 4, LIST_ITEM, U"list_item")
	enums_add (kManPage_type, 5, TERM, U"term")
	enums_alt (kManPage_type, TERM, U"tag")
	enums_add (kManPage_type, 6, DEFINITION, U"definition")
	enums_add (kManPage_type, 7, CODE, U"code")
	enums_add (kManPage_type, 8, PROTOTYPE, U"prototype")   // probably unused
	enums_add (kManPage_type, 9, EQUATION, U"formula")
	enums_add (kManPage_type, 10, PICTURE, U"picture")
	enums_add (kManPage_type, 11, SCRIPT, U"script")
	enums_add (kManPage_type, 12, LIST_ITEM1, U"list_item1")
	enums_add (kManPage_type, 13, LIST_ITEM2, U"list_item2")
	enums_add (kManPage_type, 14, LIST_ITEM3, U"list_item3")
	enums_add (kManPage_type, 15, TERM1, U"term1")
	enums_alt (kManPage_type, TERM1, U"tag1")
	enums_add (kManPage_type, 16, TERM2, U"term2")
	enums_alt (kManPage_type, TERM2, U"tag2")
	enums_add (kManPage_type, 17, TERM3, U"term3")
	enums_alt (kManPage_type, TERM3, U"tag3")
	enums_add (kManPage_type, 18, DEFINITION1, U"definition1")
	enums_add (kManPage_type, 19, DEFINITION2, U"definition2")
	enums_add (kManPage_type, 20, DEFINITION3, U"definition3")
	enums_add (kManPage_type, 21, CODE1, U"code1")   // only in pre-2023 man pages
	enums_add (kManPage_type, 22, CODE2, U"code2")   // only in pre-2023 man pages
	enums_add (kManPage_type, 23, CODE3, U"code3")   // only in pre-2023 man pages
	enums_add (kManPage_type, 24, CODE4, U"code4")   // only in pre-2023 man pages
	enums_add (kManPage_type, 25, CODE5, U"code5")   // only in pre-2023 man pages
	enums_add (kManPage_type, 26, CAPTION, U"caption")
	enums_add (kManPage_type, 27, QUOTE, U"quote")
	enums_add (kManPage_type, 28, QUOTE1, U"quote1")
	enums_add (kManPage_type, 29, QUOTE2, U"quote2")
	enums_add (kManPage_type, 30, QUOTE3, U"quote3")
	enums_add (kManPage_type, 31, SUBHEADER, U"subheader")
enums_end (kManPage_type, 31, NORMAL)

/* End of file ManPage_enums.h */

