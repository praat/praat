/* Simple.c
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
 * pb 2002/03/07 GPL
 */

#include "Simple.h"

#include "oo_DESTROY.h"
#include "Simple_def.h"
#include "oo_COPY.h"
#include "Simple_def.h"
#include "oo_EQUAL.h"
#include "Simple_def.h"
#include "oo_WRITE_TEXT.h"
#include "Simple_def.h"
#include "oo_WRITE_BINARY.h"
#include "Simple_def.h"
//#include "oo_WRITE_CACHE.h"
//#include "Simple_def.h"
#include "oo_READ_TEXT.h"
#include "Simple_def.h"
#include "oo_READ_BINARY.h"
#include "Simple_def.h"
//#include "oo_READ_CACHE.h"
//#include "Simple_def.h"
#include "oo_DESCRIPTION.h"
#include "Simple_def.h"

class_methods (SimpleInt, Data)
	class_method_local (SimpleInt, description)
	class_method_local (SimpleInt, destroy)
	class_method_local (SimpleInt, copy)
	class_method_local (SimpleInt, equal)
	class_method_local (SimpleInt, writeText)
	class_method_local (SimpleInt, writeBinary)
	//class_method_local (SimpleInt, writeCache)
	class_method_local (SimpleInt, readText)
	class_method_local (SimpleInt, readBinary)
	//class_method_local (SimpleInt, readCache)
class_methods_end

SimpleInt SimpleInt_create (int number) {
	SimpleInt me = new (SimpleInt);
	if (! me) return NULL;
	my number = number;
	return me;
}

class_methods (SimpleShort, Data)
	class_method_local (SimpleShort, description)
	class_method_local (SimpleShort, destroy)
	class_method_local (SimpleShort, copy)
	class_method_local (SimpleShort, equal)
	class_method_local (SimpleShort, writeText)
	class_method_local (SimpleShort, writeBinary)
	//class_method_local (SimpleShort, writeCache)
	class_method_local (SimpleShort, readText)
	class_method_local (SimpleShort, readBinary)
	//class_method_local (SimpleShort, readCache)
class_methods_end

SimpleShort SimpleShort_create (short number) {
	SimpleShort me = new (SimpleShort);
	if (! me) return NULL;
	my number = number;
	return me;
}

class_methods (SimpleLong, Data)
	class_method_local (SimpleLong, description)
	class_method_local (SimpleLong, destroy)
	class_method_local (SimpleLong, copy)
	class_method_local (SimpleLong, equal)
	class_method_local (SimpleLong, writeText)
	class_method_local (SimpleLong, writeBinary)
	//class_method_local (SimpleLong, writeCache)
	class_method_local (SimpleLong, readText)
	class_method_local (SimpleLong, readBinary)
	//class_method_local (SimpleLong, readCache)
class_methods_end

SimpleLong SimpleLong_create (long number) {
	SimpleLong me = new (SimpleLong);
	if (! me) return NULL;
	my number = number;
	return me;
}

class_methods (SimpleFloat, Data)
	class_method_local (SimpleFloat, description)
	class_method_local (SimpleFloat, destroy)
	class_method_local (SimpleFloat, copy)
	class_method_local (SimpleFloat, equal)
	class_method_local (SimpleFloat, writeText)
	class_method_local (SimpleFloat, writeBinary)
	//class_method_local (SimpleFloat, writeCache)
	class_method_local (SimpleFloat, readText)
	class_method_local (SimpleFloat, readBinary)
	//class_method_local (SimpleFloat, readCache)
class_methods_end

SimpleFloat SimpleFloat_create (float number) {
	SimpleFloat me = new (SimpleFloat);
	if (! me) return NULL;
	my number = number;
	return me;
}

class_methods (SimpleDouble, Data)
	class_method_local (SimpleDouble, description)
	class_method_local (SimpleDouble, destroy)
	class_method_local (SimpleDouble, copy)
	class_method_local (SimpleDouble, equal)
	class_method_local (SimpleDouble, writeText)
	class_method_local (SimpleDouble, writeBinary)
	//class_method_local (SimpleDouble, writeCache)
	class_method_local (SimpleDouble, readText)
	class_method_local (SimpleDouble, readBinary)
	//class_method_local (SimpleDouble, readCache)
class_methods_end

SimpleDouble SimpleDouble_create (double number) {
	SimpleDouble me = new (SimpleDouble);
	if (! me) return NULL;
	my number = number;
	return me;
}

class_methods (SimpleString, Data)
	class_method_local (SimpleString, description)
	class_method_local (SimpleString, destroy)
	class_method_local (SimpleString, copy)
	class_method_local (SimpleString, equal)
	class_method_local (SimpleString, writeText)
	class_method_local (SimpleString, writeBinary)
	//class_method_local (SimpleString, writeCache)
	class_method_local (SimpleString, readText)
	class_method_local (SimpleString, readBinary)
	//class_method_local (SimpleString, readCache)
class_methods_end

SimpleString SimpleString_create (const char *string) {
	SimpleString me = new (SimpleString);
	if (! me || ! (my string = Melder_strdup (string)))
		{ forget (me); return NULL; }
	return me;
}

class_methods (SimpleStringW, Data)
	class_method_local (SimpleStringW, description)
	class_method_local (SimpleStringW, destroy)
	class_method_local (SimpleStringW, copy)
	class_method_local (SimpleStringW, equal)
	class_method_local (SimpleStringW, writeText)
	class_method_local (SimpleStringW, writeBinary)
	//class_method_local (SimpleStringW, writeCache)
	class_method_local (SimpleStringW, readText)
	class_method_local (SimpleStringW, readBinary)
	//class_method_local (SimpleStringW, readCache)
class_methods_end

SimpleStringW SimpleStringW_create (const wchar_t *string) {
	SimpleStringW me = new (SimpleStringW);
	if (! me || ! (my string = Melder_wcsdup (string)))
		{ forget (me); return NULL; }
	return me;
}

/* End of file Simple.c */
