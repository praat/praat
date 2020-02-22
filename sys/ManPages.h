#ifndef _ManPages_h_
#define _ManPages_h_
/* ManPages.h
 *
 * Copyright (C) 1996-2005,2007,2011,2012,2015-2020 Paul Boersma
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

#include "ManPage.h"
#include "Collection.h"

Thing_define (ManPages, Daata) {
	OrderedOf <structManPage> pages;
	autoSTRVEC titles;
	bool ground, dynamic, executable;
	structMelderDir rootDirectory;

	void v_destroy () noexcept
		override;
	void v_readText (MelderReadText text, int formatVersion)
		override;
};

autoManPages ManPages_create ();

void ManPages_addPage (ManPages me, conststring32 title, conststring32 author, integer date,
	structManPage_Paragraph paragraphs []);
/*
	All string and struct arguments must be statically allocated
	and not change after adding them to the ManPages.
*/

integer ManPages_lookUp (ManPages me, conststring32 title);

void ManPages_writeOneToHtmlFile (ManPages me, integer ipage, MelderFile file);
void ManPages_writeAllToHtmlDir (ManPages me, conststring32 dirPath);

integer ManPages_uniqueLinksHither (ManPages me, integer ipage);
constSTRVEC ManPages_getTitles (ManPages me);

/* End of file ManPages.h */
#endif
