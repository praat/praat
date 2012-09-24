#ifndef _ManPages_h_
#define _ManPages_h_
/* ManPages.h
 *
 * Copyright (C) 1996-2011 Paul Boersma
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

#include "ManPage.h"
#include "Collection.h"

Thing_define (ManPages, Data) {
	// new data:
	public:
		Ordered pages;
		const wchar_t **titles;
		int ground, dynamic, executable;
		structMelderDir rootDirectory;
	// overridden methods:
	protected:
		virtual void v_destroy ();
		virtual void v_readText (MelderReadText text);
};

ManPages ManPages_create (void);

void ManPages_addPage (ManPages me, const wchar_t *title, const wchar_t *author, long date,
	struct structManPage_Paragraph paragraphs []);
/*
	All string and struct arguments must be statically allocated
	and not change after adding them to the ManPages.
*/

long ManPages_lookUp (ManPages me, const wchar_t *title);

void ManPages_writeOneToHtmlFile (ManPages me, long ipage, MelderFile file);
void ManPages_writeAllToHtmlDir (ManPages me, const wchar_t *dirPath);

long ManPages_uniqueLinksHither (ManPages me, long ipage);
const wchar_t **ManPages_getTitles (ManPages me, long *numberOfTitles);

/* End of file ManPages.h */
#endif
