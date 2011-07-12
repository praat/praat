/* DLL.cpp
 *
 * Copyright (C) 2011 David Weenink
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

static void classDLLNode_destroy (I)
{
	iam (DLLNode);
	forget (my data);
	inherited (DLLNode) destroy (me);
}

static void classDLLNode_copy (I,thou)
{
	iam (DLLNode); thouart (DLLNode);
	thy data = (Data) Data_copy (my data); therror
}

class_methods (DLLNode, Data)
{
	class_method_local (DLLNode, copy)
	class_method_local (DLLNode, destroy)
	class_methods_end
}

static int classDLL_compare (Any node1, Any node2)
{
	(void) node1;
	(void) node2;
	return 0;
}

static void classDLL_destroy (I)
{
	iam (DLL);
	DLLNode v = my front;
	while (v != 0) {
		DLLNode cur = v;
		v = v -> next;
		forget (cur);
	}
	inherited (DLL) destroy (me);
}
 
class_methods (DLL, Thing)
{
	class_method_local (DLL, compare)
	class_method_local (DLL, destroy)
	class_methods_end
}

DLLNode DLLNode_create (Data data)
{
		DLLNode me = Thing_new (DLLNode);
		my data = data;
		return me;
}

void DLL_init (I) { iam (DLL); }

DLL DLL_create()
{
	try {
		DLL me = Thing_new (DLL);
		return me;
	} catch (MelderError) { Melder_thrown ("DLL not created."); }
	
}

void DLL_addFront (DLL me, DLLNode n)
{
	if (my front == 0) // empty list
	{
		my front = n;
		my back = n;
		n -> next = 0;
		n -> prev = 0;
		my numberOfNodes++;
	}
	else DLL_addBefore (me, my front, n);
}

void DLL_addBack (DLL me, DLLNode n)
{
	if (my back == 0) DLL_addFront (me, n); // empty list
	else DLL_addAfter (me, my back, n);
}

void DLL_addBefore (DLL me, DLLNode pos, DLLNode n)
{
	n -> prev = pos -> prev;
	n -> next = pos;
	if (pos -> prev == 0) my front = n;
	else pos -> prev -> next = n;
	pos -> prev = n;
	my numberOfNodes++;
}

void DLL_addAfter (DLL me, DLLNode pos, DLLNode n)
{
	n -> prev = pos;
	n -> next = pos -> next;
	if (pos -> next == 0) my back = n;
	else pos -> next -> prev = n;
	pos -> next = n;
	my numberOfNodes++;
}

void DLL_remove (DLL me, DLLNode n)
{
	if (my numberOfNodes == 0) return;
	if (n == my front)
	{
		my front = my front -> next;
		my front -> prev = 0;
	}
	else if (n == my back)
	{
		my back = my back -> prev;
		my back -> next = 0;
	}
	else
	{
		n -> prev -> next = n -> next;
		n -> next -> prev = n -> prev;
	}
	forget (n);
	my numberOfNodes++;
}

#undef our
#define our ((DLL_Table) my methods) ->   // tijdelijk

void DLL_sort (DLL me, DLLNode from, DLLNode to)
{
	try {
		if (from == to) return; // ok nothing to do ?
		// first pass : count the number of nodes
		long numberOfNodes = 2;
		DLLNode current = from;
		while ((current = current -> next) != to && numberOfNodes < my numberOfNodes) { numberOfNodes++; }
		// reserve storage
		autoNUMvector<Data> data (1, numberOfNodes);
		current = from;
		for (long inode = 1; inode <= numberOfNodes; inode++)
		{
			data[inode] = current -> data;
			current = current -> next;
		}
		NUMsort_p (numberOfNodes, (void**) data.peek(), (int (*) (const void *, const void *)) our compare);
		// 
		current = from;
		for (long inode = 1; inode <= numberOfNodes; inode++)
		{
			current -> data = data[inode]; current = current -> next;
		}
	} catch (MelderError) { Melder_throw (me, ": not sorted."); }
}

// end of file DLL.cpp