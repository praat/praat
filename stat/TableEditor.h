#ifndef _TableEditor_h_
#define _TableEditor_h_
/* TableEditor.h
 *
 * Copyright (C) 2006-2011 Paul Boersma
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

/*
 * pb 2011/07/02
 */

#include "Editor.h"
#include "Table.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (TableEditor);

TableEditor TableEditor_create (GuiObject parent, const wchar_t *title, Table table);

#ifdef __cplusplus
	}
#endif

/* End of file TableEditor.h */
#endif
