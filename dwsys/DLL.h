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

/*typedef struct structDLLNode {
	struct structDLLNode *next, *prev;
	Data data;
} *DLLNode;*/

Thing_declare1cpp (DLLNode);
Thing_declare1cpp (DLL);

DLLNode DLLNode_create (Data data); // DLLNode owns the data

void DLL_init (I);
DLL DLL_create();

void DLL_addFront (DLL me, DLLNode n);
void DLL_addBack (DLL me, DLLNode n);
void DLL_addBefore (DLL me, DLLNode pos, DLLNode n);
void DLL_addAfter (DLL me, DLLNode pos, DLLNode n);
void DLL_remove (DLL me, DLLNode n);
void DLL_sort (DLL me, DLLNode from, DLLNode to);

#ifdef __cplusplus
	}

	struct structDLLNode : public structData {
		DLLNode next, prev;
		Data data;
	};
	#define DLLNode__methods(klas) Data__methods(klas)
	Thing_declare2cpp (DLLNode, Data);

	struct structDLL : public structThing {
		long numberOfNodes;
		DLLNode front, back;
	};
	#define DLL__methods(klas) Thing__methods(klas) \
		int (*compare) (I, thou);
	Thing_declare2cpp (DLL, Thing);

#endif

#endif // _DLL_h_

