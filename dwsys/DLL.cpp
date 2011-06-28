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

static void classNode_destroy (I)
{
	iam (Node);
	forget (my data);
	inherited (Node) destroy (me);
}

static int classNode_copy (I,thou)
{
	iam (Node); thouart (Node);
	thy data = (Data) Data_copy (my data); therror
	return 1;
}

class_methods (Node, Data)
{
	class_method_local (Node, copy)
	class_method_local (Node, destroy)
	class_methods_end
}

static void classDLL_destroy (I)
{
	iam (DLL);
	Node v = my front;
	while (v != 0) {
		Node cur = v;
		v = v -> next;
		forget (cur);
	}
	inherited (DLL) destroy (me);
}

class_methods (DLL, Thing)
{
	class_method_local (DLL, destroy)
	class_methods_end
}

Node Node_create (Data data)
{
	try {
		Node me = Thing_new (Node);
		my data = data;
		return me;
	} catch (MelderError) { rethrowzero; }
}

void DLL_init (I) { iam (DLL); }

DLL DLL_create()
{
	try {
		DLL me = Thing_new (DLL);
		return me;
	} catch (MelderError) { rethrowmzero ("DLL not created."); }
	
}

void DLL_addFront (DLL me, Node n)
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

void DLL_addBack (DLL me, Node n)
{
	if (my back == 0) DLL_addFront (me, n); // empty list
	else DLL_addAfter (me, my back, n);
}

void DLL_addBefore (DLL me, Node pos, Node n)
{
	n -> prev = pos -> prev;
	n -> next = pos;
	if (pos -> prev == 0) my front = n;
	else pos -> prev -> next = n;
	pos -> prev = n;
	my numberOfNodes++;
}

void DLL_addAfter (DLL me, Node pos, Node n)
{
	n -> prev = pos;
	n -> next = pos -> next;
	if (pos -> next == 0) my back = n;
	else pos -> next -> prev = n;
	pos -> next = n;
	my numberOfNodes++;
}

void DLL_remove (DLL me, Node n)
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

// end of file DLL.cpp