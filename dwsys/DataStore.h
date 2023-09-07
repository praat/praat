#ifndef _DataStore_h_
#define _DataStore_h_
/* DataStore.h
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

#include "Data.h"
#include "Collection.h"
#include "Simple.h"

Thing_define (StoredData, Daata) {
	autoDaata data;
	autoSimpleString description;
};


autoStoredData StoredData_create (Daata data, conststring32 description);

Collection_define (StoredDataList, OrderedOf, StoredData) {
};

/*
	Active data structure. 'current' is position of the cursor in the history list
	Queries and insertions are at the current position.
	Inserting new data at current position invalidates data at current+1, current+2, ...
	Invariants:
	0 <= current <= size + 1;
*/

class autoDataStore {

	autoStoredDataList storedData;
	integer current;
	
public:
	
	autoDataStore () {
		storedData = StoredDataList_create ();
		current = 0;
	}
		
	void forth () {
		if (current <= storedData -> size)
			current ++;
	}
	/*
		Precondition: ! offright
		Postcondition: my current++;
	*/

	void back () {
		if (current > 0)
			current --;
	}
	/*
		Precondition: ! offleft
		Postcondition: my current--;
	*/

	StoredData getData () {
		Melder_assert (current > 0 && current <= storedData -> size);
		return storedData -> at [current];
	}
	/*
		return StoredData at my item [my current];
	*/

	void insertData_move (Daata data, conststring32 description) {
		for (integer i = storedData -> size; i >= current + 1; i --)
			storedData -> removeItem (i);
		autoStoredData thee = StoredData_create (data, description);
		storedData -> addItem_move (thee.move());
		current = storedData -> size;
	}
	/*
		1. forget about item [ current+1..size ]
		2. insert item after current.
		3. new_size = current+1
	*/

	bool isEmpty () {
		return storedData -> size == 0;
	}

	bool isOffleft () {
		return current == 0;
	}

	bool isOffright (){
		return storedData -> size == 0 || current == storedData -> size + 1;
	}
	
	autostring32 getDataDescription (integer offsetFromCurrent) {
		const integer pos = current + offsetFromCurrent;
		autostring32 result;
		if (pos >= 1 && pos <= storedData -> size) {
			StoredData data = storedData -> at [pos];
			SimpleString ss = data -> description.get();
			result = Melder_dup (ss ->string.get());
		}
		return result;
	}
	/*
		offsetFromCurrent may be zero, positive or negative.
		References outside the list will return nullptr.
	*/
};

#endif /* _DataStore_h_ */
