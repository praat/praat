#ifndef _ManPages_h_
#define _ManPages_h_
/* ManPages.h
 *
 * Copyright (C) 1996-2007 Paul Boersma
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
 * pb 2007/06/11
 */

#ifndef _ManPage_h_
	#include "ManPage.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif

#define ManPages_members Data_members \
	Ordered pages; \
	const wchar_t **titles; \
	int ground, dynamic, executable; \
	structMelderDir rootDirectory;
#define ManPages_methods Data_methods
class_create (ManPages, Data);

ManPages ManPages_create (void);

int ManPages_addPage (ManPages me, const char *title, const char *author, long date,
	struct structManPage_Paragraph paragraphs []);
/*
	All string and struct arguments must be statically allocated
	and not change after adding them to the ManPages.
*/

long ManPages_lookUp (ManPages me, const char *title);

int ManPages_writeOneToHtmlFile (ManPages me, long ipage, MelderFile file);
int ManPages_writeAllToHtmlDir (ManPages me, const wchar_t *dirPath);

long ManPages_uniqueLinksHither (ManPages me, long ipage);
const wchar_t **ManPages_getTitles (ManPages me, long *numberOfTitles);

/* End of file ManPages.h */
#endif
