/* Articulation.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1996/08/16
 * pb 2002/07/16 GPL
 */

#include "Articulation.h"
#include "enum_c.h"
#include "Articulation_enums.h"
#include "oo_DESTROY.h"
#include "Articulation_def.h"
#include "oo_COPY.h"
#include "Articulation_def.h"
#include "oo_EQUAL.h"
#include "Articulation_def.h"
#include "oo_WRITE_ASCII.h"
#include "Articulation_def.h"
#include "oo_WRITE_BINARY.h"
#include "Articulation_def.h"
#include "oo_WRITE_CACHE.h"
#include "Articulation_def.h"
#include "oo_READ_ASCII.h"
#include "Articulation_def.h"
#include "oo_READ_BINARY.h"
#include "Articulation_def.h"
#include "oo_READ_CACHE.h"
#include "Articulation_def.h"
#include "oo_DESCRIPTION.h"
#include "Articulation_def.h"

class_methods (Art, Data)
	class_method_local (Art, destroy)
	class_method_local (Art, copy)
	class_method_local (Art, equal)
	class_method_local (Art, writeAscii)
	class_method_local (Art, writeBinary)
	class_method_local (Art, writeCache)
	class_method_local (Art, readAscii)
	class_method_local (Art, readBinary)
	class_method_local (Art, readCache)
	class_method_local (Art, description)
class_methods_end

Art Art_create (void) {
	return new (Art);
}

/* End of file Articulation.c */
