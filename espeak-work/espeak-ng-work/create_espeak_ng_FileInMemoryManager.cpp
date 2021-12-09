/* create_espeak_ng_FileInMemoryManager.cpp
 *
 * Copyright (C) 2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * ainteger with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "espeakdata_FileInMemory.h"

autoFileInMemoryManager create_espeak_ng_FileInMemoryManager () {
	try{
		autoFileInMemorySet espeak_ng = create_espeak_ng_FileInMemorySet ();
		autoFileInMemoryManager me = FileInMemoryManager_create (espeak_ng.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"FileInMemoryManager for espeak-ng not created.");
	}
}

/* End of file create_espeak_ng_FileInMemoryManager.cpp */
