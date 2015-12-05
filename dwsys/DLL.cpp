/* DLL.cpp
 *
 * Copyright (C) 2011-2013, 2015 David Weenink
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

#include "DLL.h"

Thing_implement (DLLNode, Daata, 0);

void structDLLNode :: v_destroy () {
	DLLNode_Parent :: v_destroy ();
}

void structDLLNode :: v_copy (Daata thee_Daata) {
	DLLNode thee = static_cast <DLLNode> (thee_Daata);
	thy data = Data_copy (our data.get());
}

Thing_implement (DLL, Thing, 0);

void structDLL :: v_destroy () {
	DLLNode v = front;
	while (v) {
		DLLNode cur = v;
		v = v -> next;
		forget (cur);
	}
	DLL_Parent :: v_destroy ();
}

int structDLL :: s_compareHook (Daata /* node1 */, Daata /* node2 */) noexcept {
	return 0;
}

autoDLLNode DLLNode_create (autoDaata data) {
	autoDLLNode me = Thing_new (DLLNode);
	my data = data.move();
	return me;
}

void DLL_init (DLL) {
}

autoDLL DLL_create() {
	try {
		autoDLL me = Thing_new (DLL);
		DLL_init (me.peek());
		return me;
	} catch (MelderError) {
		Melder_throw (U"DLL not created.");
	}

}

void DLL_addFront (DLL me, DLLNode node) {
	if (my front) {
		DLL_addBefore (me, my front, node);
	} else {   // empty list
		my front = node;
		my back = node;
		node -> next = nullptr;
		node -> prev = nullptr;
		my numberOfNodes++;
	}
}

void DLL_addBack (DLL me, DLLNode node) {
	if (my back) {
		DLL_addAfter (me, my back, node);
	} else {
		DLL_addFront (me, node);    // empty list
	}
}

void DLL_addBefore (DLL me, DLLNode pos, DLLNode node) {
	node -> prev = pos -> prev;
	node -> next = pos;
	if (pos -> prev == nullptr) {
		my front = node;
	} else {
		pos -> prev -> next = node;
	}
	pos -> prev = node;
	my numberOfNodes++;
}

void DLL_addAfter (DLL me, DLLNode pos, DLLNode node) {
	node -> prev = pos;
	node -> next = pos -> next;
	if (pos -> next == nullptr) {
		my back = node;
	} else {
		pos -> next -> prev = node;
	}
	pos -> next = node;
	my numberOfNodes++;
}

void DLL_remove (DLL me, DLLNode node) {
	if (my numberOfNodes == 0) {
		return;
	}
	if (node == my front) {
		my front = my front -> next;
		my front -> prev = nullptr;
	} else if (node == my back) {
		my back = my back -> prev;
		my back -> next = nullptr;
	} else {
		node -> prev -> next = node -> next;
		node -> next -> prev = node -> prev;
	}
	forget (node);
	my numberOfNodes++;
}

// Preconditions:
//	from and to must be part of the list
//	from must occur before to
void DLL_sortPart (DLL me, DLLNode from, DLLNode to) {
	// Save data
	if (from == to) {
		return;   // nothing to do
	}
	DLLNode from_prev = from -> prev;
	DLLNode to_next = to -> next;
	DLLNode my_front = my front;
	DLLNode my_back = my back;

	from -> prev = to -> next = nullptr;
	my front = from;
	my back = to;
	DLL_sort (me);
	// restore complete list
	my front -> prev = from_prev;
	if (from_prev) {
		from_prev -> next = my front;
	}
	my back -> next = to_next;
	if (to_next) {
		to_next -> prev = my back;
	}
	if (my_front != from) {
		my front = my_front;
	}
	if (my_back != to) {
		my back = my_back;
	}
}

void DLL_sort (DLL me) {
	Data_CompareHook::FunctionType compare = my v_getCompareHook ().get();
	long increment = 1;
	DLLNode front = my front, back;
	for (;;) {
		DLLNode node1 = front;
		front = nullptr;
		back = nullptr;

		long numberOfMerges = 0;

		while (node1) {
			DLLNode node2 = node1, node;
			long node1size = 0;
			numberOfMerges++;

			for (long i = 1; i <= increment; i++) {
				node1size++;
				node2 = node2 -> next;
				if (! node2) {
					break;
				}
			}

			long node2size = increment;

			while (node1size > 0 || (node2size > 0 && node2)) { // merge node1 and node2
				if (node1size == 0) {
					node2size--; node = node2; node2 = node2 -> next;
				} else if (node2size == 0 || ! node2) {
					node1size--; node = node1; node1 = node1 -> next;
				} else if (compare (node1, node2) <= 0) {
					node1size--; node = node1; node1 = node1 -> next;
				} else {
					node2size--; node = node2; node2 = node2 -> next;
				}

				if (back) {
					back -> next = node;
				} else {
					front = node;
				}
				node -> prev = back;
				back = node;
			}
			node1 = node2;
		}
		back -> next = nullptr;
		if (numberOfMerges <= 1) {
			break;
		}
		increment *= 2;
	}
	//
	my front = front;
	my back = back;
}

// end of file DLL.cpp
