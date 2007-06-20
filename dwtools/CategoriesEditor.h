#ifndef _CategoriesEditor_h_
#define _CategoriesEditor_h_
/* CategoriesEditor.h
 *
 * Copyright (C) 1993-2002 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 19950713
 djmw 20020813 GPL header
*/

#include "Command.h"
#include "Editor.h"
#include "Categories.h"

/* 'CommandHistory history' should (in the future) belong to Editor.h */
#define CategoriesEditor_members Editor_members				\
	CommandHistory history;									\
	int position;											\
	Widget list, text, outOfView, undo, redo;				\
	Widget remove, insert, insertAtEnd, replace, moveUp, moveDown;
	
#define CategoriesEditor_methods Editor_methods
class_create (CategoriesEditor, Editor);

Any CategoriesEditor_create (Widget parent, wchar_t *title, Any data);

#endif /* _CategoriesEditor_h_ */
