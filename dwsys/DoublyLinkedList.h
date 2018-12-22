#ifndef _DoublyLinkedList_h_
#define _DoublyLinkedList_h_
/* DoublyLinkedList.h
 *
 * Copyright (C) 2011 - 2018 David Weenink, 2015 Paul Boersma
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


#include "Data.h"

Thing_define (DoublyLinkedNode, Daata) {
	DoublyLinkedNode next, prev;
	autoDaata data;

	void v_destroy () noexcept
		override;
	void v_copy (Daata data_to)
		override;
};

Thing_define (DoublyLinkedList, Thing) {
	integer numberOfNodes;
	DoublyLinkedNode front, back;

	void v_destroy () noexcept
		override;

	static int s_compareHook (Daata /* data1 */, Daata /* data2 */) noexcept;
	virtual Data_CompareHook v_getCompareHook () { return s_compareHook; }
};

autoDoublyLinkedNode DoublyLinkedNode_create (autoDaata data);

void DoublyLinkedList_init (DoublyLinkedList me);

autoDoublyLinkedList DoublyLinkedList_create ();

void DoublyLinkedList_addFront (DoublyLinkedList me, DoublyLinkedNode n);

void DoublyLinkedList_addBack (DoublyLinkedList me, DoublyLinkedNode n);

void DoublyLinkedList_addBefore (DoublyLinkedList me, DoublyLinkedNode pos, DoublyLinkedNode n);

void DoublyLinkedList_addAfter (DoublyLinkedList me, DoublyLinkedNode pos, DoublyLinkedNode n);

void DoublyLinkedList_remove (DoublyLinkedList me, DoublyLinkedNode n);

void DoublyLinkedList_sort (DoublyLinkedList me);

void DoublyLinkedList_sortPart (DoublyLinkedList me, DoublyLinkedNode from, DoublyLinkedNode to);

#endif // _DoublyLinkedList_h_

