#ifndef _melder_help_h_
#define _melder_help_h_
/* melder_help.h
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

void Melder_help (conststring32 query);
void Melder_search ();
void Melder_setHelpProc (void (*help) (conststring32 query));
void Melder_setSearchProc (void (*search) ());

/* End of file melder_help.h */
#endif
