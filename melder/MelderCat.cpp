/* MelderCat.cpp
 *
 * Copyright (C) 2006-2012,2014-2024 Paul Boersma
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

#include "melder.h"

MelderString MelderCat::_buffers [MelderCat::_k_NUMBER_OF_BUFFERS] { };
int MelderCat::_bufferNumber = 0;

/* End of file MelderCat.cpp */
