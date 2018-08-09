#ifndef _melder_search_h_
#define _melder_search_h_
/* melder_search.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

/********** NUMBER AND STRING COMPARISON **********/

bool Melder_numberMatchesCriterion (double value, kMelder_number which, double criterion);
bool Melder_stringMatchesCriterion (conststring32 value, kMelder_string which, conststring32 criterion, bool caseSensitive);

/* End of file melder_search.h */
#endif
