/* Articulation.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Articulation.h"

#include "oo_DESTROY.h"
#include "Articulation_def.h"
#include "oo_COPY.h"
#include "Articulation_def.h"
#include "oo_EQUAL.h"
#include "Articulation_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Articulation_def.h"
#include "oo_WRITE_TEXT.h"
#include "Articulation_def.h"
#include "oo_WRITE_BINARY.h"
#include "Articulation_def.h"
#include "oo_READ_TEXT.h"
#include "Articulation_def.h"
#include "oo_READ_BINARY.h"
#include "Articulation_def.h"
#include "oo_DESCRIPTION.h"
#include "Articulation_def.h"

#include "enums_getText.h"
#include "Articulation_enums.h"
#include "enums_getValue.h"
#include "Articulation_enums.h"

Thing_implement (Art, Data, 0);

Art Art_create (void) {
	return Thing_new (Art);
}

/* End of file Articulation.cpp */
