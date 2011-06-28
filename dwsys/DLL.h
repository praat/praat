#ifndef _DLL_h_
#define _DLL_h_
/* DLL.h
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
#ifndef _Data_h_
	#include "Data.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

/*typedef struct structNode {
	struct structNode *next, *prev;
	Data data;
} *Node;*/

#define Node_members Data_members\
	Node next, prev; \
	Data data;
#define Node_methods Data_methods
class_create (Node, Data);

#define DLL_members Thing_members \
	long numberOfNodes; \
	Node front, back;
#define DLL_methods Thing_methods
class_create (DLL, Thing);

Node Node_create (Data data); // Node owns the data

void DLL_init (I);
DLL DLL_create();

void DLL_addFront (DLL me, Node n);
void DLL_addBack (DLL me, Node n);
void DLL_addBefore (DLL me, Node pos, Node n);
void DLL_addAfter (DLL me, Node pos, Node n);
void DLL_remove (DLL me, Node n);

#ifdef __cplusplus
	}
#endif

#endif // _DLL_h_

