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
	forget (data);
	DLLNode_Parent :: v_destroy ();
}

void structDLLNode :: v_copy (thou) {
	thouart (DLLNode);
	thy data = Data_copy (data);
}

Thing_implement (DLL, Thing, 0);

void structDLL :: v_destroy () {
	DLLNode v = front;
	while (v != nullptr) {
		DLLNode cur = v;
		v = v -> next;
		forget (cur);
	}
	DLL_Parent :: v_destroy ();
}

int structDLL :: s_compare (Any node1, Any node2) {
	(void) node1;
	(void) node2;
	return 0;
}

DLLNode DLLNode_create (Daata data) {
	DLLNode me = Thing_new (DLLNode);
	my data = data;
	return me;
}

void DLL_init (DLL) {
}

DLL DLL_create() {
	try {
		autoDLL me = Thing_new (DLL);
		DLL_init (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"DLL not created.");
	}

}

void DLL_addFront (DLL me, DLLNode n) {
	if (my front == nullptr) { // empty list
		my front = n;
		my back = n;
		n -> next = nullptr;
		n -> prev = nullptr;
		my numberOfNodes++;
	} else {
		DLL_addBefore (me, my front, n);
	}
}

void DLL_addBack (DLL me, DLLNode n) {
	if (my back == nullptr) {
		DLL_addFront (me, n);    // empty list
	} else {
		DLL_addAfter (me, my back, n);
	}
}

void DLL_addBefore (DLL me, DLLNode pos, DLLNode n) {
	n -> prev = pos -> prev;
	n -> next = pos;
	if (pos -> prev == nullptr) {
		my front = n;
	} else {
		pos -> prev -> next = n;
	}
	pos -> prev = n;
	my numberOfNodes++;
}

void DLL_addAfter (DLL me, DLLNode pos, DLLNode n) {
	n -> prev = pos;
	n -> next = pos -> next;
	if (pos -> next == nullptr) {
		my back = n;
	} else {
		pos -> next -> prev = n;
	}
	pos -> next = n;
	my numberOfNodes++;
}

void DLL_remove (DLL me, DLLNode n) {
	if (my numberOfNodes == 0) {
		return;
	}
	if (n == my front) {
		my front = my front -> next;
		my front -> prev = nullptr;
	} else if (n == my back) {
		my back = my back -> prev;
		my back -> next = nullptr;
	} else {
		n -> prev -> next = n -> next;
		n -> next -> prev = n -> prev;
	}
	forget (n);
	my numberOfNodes++;
}

// Preconditions:
//	from and to must be part of the list
//	from must occur before to
void DLL_sortPart (DLL me, DLLNode from, DLLNode to) {
	// Save data
	if (from == to) {
		return;    // nothing to do
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
	Data_CompareFunction compare = my v_getCompareFunction ();
	long increment = 1;
	DLLNode front = my front, back;
	for (;;) {
		DLLNode n1 = front;
		front = nullptr;
		back = nullptr;

		long numberOfMerges = 0;

		while (n1 != nullptr) {
			DLLNode n2 = n1, n;
			long n1size = 0;
			numberOfMerges++;

			for (long i = 1; i <= increment; i++) {
				n1size++;
				n2 = n2 -> next;
				if (!n2) {
					break;
				}
			}

			long n2size = increment;

			while (n1size > 0 || (n2size > 0 && n2)) { // merge n1 and n2
				if (n1size == 0) {
					n2size--; n = n2; n2 = n2 -> next;
				} else if (n2size == 0 || !n2) {
					n1size--; n = n1; n1 = n1 -> next;
				} else if (compare (n1, n2) <= 0) {
					n1size--; n = n1; n1 = n1 -> next;
				} else {
					n2size--; n = n2; n2 = n2 -> next;
				}

				if (back) {
					back -> next = n;
				} else {
					front = n;
				}
				n -> prev = back;
				back = n;
			}
			n1 = n2;
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
