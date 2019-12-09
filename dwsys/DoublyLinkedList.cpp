/* DoublyLinkedList.cpp
 *
 * Copyright (C) 2011-2019 David Weenink
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

#include "DoublyLinkedList.h"

Thing_implement (DoublyLinkedNode, Daata, 0);

void structDoublyLinkedNode :: v_destroy () noexcept {
	DoublyLinkedNode_Parent :: v_destroy ();
}

void structDoublyLinkedNode :: v_copy (Daata thee_Daata) {
	DoublyLinkedNode thee = static_cast <DoublyLinkedNode> (thee_Daata);
	thy data = Data_copy (our data.get());
}

Thing_implement (DoublyLinkedList, Thing, 0);

void structDoublyLinkedList :: v_destroy () noexcept {
	DoublyLinkedNode v = front;
	while (v) {
		DoublyLinkedNode cur = v;
		v = v -> next;
		forget (cur);
	}
	DoublyLinkedList_Parent :: v_destroy ();
}

int structDoublyLinkedList :: s_compareHook (Daata /* node1 */, Daata /* node2 */) noexcept {
	return 0;
}

autoDoublyLinkedNode DoublyLinkedNode_create (autoDaata data) {
	autoDoublyLinkedNode me = Thing_new (DoublyLinkedNode);
	my data = data.move();
	return me;
}

void DoublyLinkedList_init (DoublyLinkedList) {
}

autoDoublyLinkedList DoublyLinkedList_create() {
	try {
		autoDoublyLinkedList me = Thing_new (DoublyLinkedList);
		DoublyLinkedList_init (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"DoublyLinkedList not created.");
	}

}

void DoublyLinkedList_addFront (DoublyLinkedList me, DoublyLinkedNode node) {
	if (my front) {
		DoublyLinkedList_addBefore (me, my front, node);
	} else {   // empty list
		my front = node;
		my back = node;
		node -> next = nullptr;
		node -> prev = nullptr;
		my numberOfNodes ++;
	}
}

void DoublyLinkedList_addBack (DoublyLinkedList me, DoublyLinkedNode node) {
	if (my back)
		DoublyLinkedList_addAfter (me, my back, node);
	else
		DoublyLinkedList_addFront (me, node);    // empty list
}

void DoublyLinkedList_addBefore (DoublyLinkedList me, DoublyLinkedNode pos, DoublyLinkedNode node) {
	node -> prev = pos -> prev;
	node -> next = pos;
	if (pos -> prev == nullptr)
		my front = node;
	else
		pos -> prev -> next = node;
	pos -> prev = node;
	my numberOfNodes ++;
}

void DoublyLinkedList_addAfter (DoublyLinkedList me, DoublyLinkedNode pos, DoublyLinkedNode node) {
	node -> prev = pos;
	node -> next = pos -> next;
	if (pos -> next == nullptr)
		my back = node;
	else
		pos -> next -> prev = node;
	pos -> next = node;
	my numberOfNodes ++;
}

void DoublyLinkedList_remove (DoublyLinkedList me, DoublyLinkedNode node) {
	if (my numberOfNodes == 0)
		return;
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
	my numberOfNodes --;
}

// Preconditions:
//	from and to should be part of the list
//	from must occur before to
void DoublyLinkedList_sortPart (DoublyLinkedList me, DoublyLinkedNode from, DoublyLinkedNode to) {
	// Save data
	if (from == to)
		return;   // nothing to do

	const DoublyLinkedNode from_prev = from -> prev;
	const DoublyLinkedNode to_next = to -> next;
	const DoublyLinkedNode my_front = my front;
	const DoublyLinkedNode my_back = my back;

	from -> prev = to -> next = nullptr;
	my front = from;
	my back = to;
	DoublyLinkedList_sort (me);
	// restore complete list
	my front -> prev = from_prev;
	if (from_prev)
		from_prev -> next = my front;

	my back -> next = to_next;
	if (to_next)
		to_next -> prev = my back;

	if (my_front != from)
		my front = my_front;

	if (my_back != to)
		my back = my_back;

}

void DoublyLinkedList_sort (DoublyLinkedList me) {
	Data_CompareHook::FunctionType compare = my v_getCompareHook ().get();
	integer increment = 1;
	DoublyLinkedNode front = my front, back;
	for (;;) {
		DoublyLinkedNode node1 = front;
		front = nullptr;
		back = nullptr;

		integer numberOfMerges = 0;

		while (node1) {
			DoublyLinkedNode node2 = node1, node;
			integer node1size = 0;
			numberOfMerges ++;

			for (integer i = 1; i <= increment; i++) {
				node1size ++;
				node2 = node2 -> next;
				if (! node2)
					break;
			}

			integer node2size = increment;

			while (node1size > 0 || (node2size > 0 && node2)) { // merge node1 and node2
				if (node1size == 0) {
					node2size --;
					node = node2;
					node2 = node2 -> next;
				} else if (node2size == 0 || ! node2) {
					node1size --;
					node = node1;
					node1 = node1 -> next;
				} else if (compare (node1, node2) <= 0) {
					node1size --;
					node = node1;
					node1 = node1 -> next;
				} else {
					node2size --;
					node = node2;
					node2 = node2 -> next;
				}

				if (back)
					back -> next = node;
				else
					front = node;

				node -> prev = back;
				back = node;
			}
			node1 = node2;
		}
		back -> next = nullptr;
		if (numberOfMerges <= 1)
			break;

		increment *= 2;
	}
	//
	my front = front;
	my back = back;
}

/* end of file DoublyLinkedList.cpp */
