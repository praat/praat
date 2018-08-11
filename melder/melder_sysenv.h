#ifndef _melder_sysenv_h_
#define _melder_sysenv_h_
/* melder_sysenv.h
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

conststring32 Melder_getenv (conststring32 variableName);
void Melder_system (conststring32 command);   // spawn a system command
void Melder_execv (conststring32 executableFileName, integer narg, char32 **args);   // spawn a subprocess

/* End of file melder_sysenv.h */
#endif
