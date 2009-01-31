#ifndef _OTMultiEditor_h_
#define _OTMultiEditor_h_
/* OTMultiEditor.h
 *
 * Copyright (C) 2005-2007 Paul Boersma
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
 * pb 2007/06/10
 */

#ifndef _HyperPage_h_
	#include "HyperPage.h"
#endif
#ifndef _OTMulti_h_
	#include "OTMulti.h"
#endif

#define OTMultiEditor__parents(Klas) HyperPage__parents(Klas) Thing_inherit (Klas, HyperPage)
Thing_declare1 (OTMultiEditor);

OTMultiEditor OTMultiEditor_create (Widget parent, const wchar_t *title, OTMulti grammar);

/* End of file OTMultiEditor.h */
#endif
