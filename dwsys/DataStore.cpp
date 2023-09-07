/* DataStore.cpp
 *
 * Copyright (C) 2023 David Weenink
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
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataStore.h"

Thing_implement (StoredData, Daata, 0);
Thing_implement (StoredDataList, Ordered, 0);

autoStoredData StoredData_create (Daata data, conststring32 description) {
	try {
		autoStoredData me = Thing_new (StoredData);
		autoDaata datacopy = Data_copy (data);
		my data = datacopy.move();
		my description = SimpleString_create (description);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create StoredData with description '", description, U"'.");
	}
}

/* End of file DataStore.cpp */
