#ifndef _ManPagesM_h_
#define _ManPagesM_h_
/* ManPagesM.h
 *
 * Copyright (C) 1996-2005 Paul Boersma
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
 * pb 2005/05/06
 */

/* ManPages macros. */

#ifndef _ManPages_h_
	#include "ManPages.h"
#endif

#define MAN_BEGIN(t,a,d)  { const char *title = t, *author = a; long date = d; \
	static struct structManPage_Paragraph page [] = {
#define INTRO(text)  { enumi (ManPage_TYPE, intro), text },
#define ENTRY(text)  { enumi (ManPage_TYPE, entry), text },
#define NORMAL(text)  { enumi (ManPage_TYPE, normal), text },
#define LIST_ITEM(text)  { enumi (ManPage_TYPE, list_item), text },
#define LIST_ITEM1(text)  { enumi (ManPage_TYPE, list_item1), text },
#define LIST_ITEM2(text)  { enumi (ManPage_TYPE, list_item2), text },
#define LIST_ITEM3(text)  { enumi (ManPage_TYPE, list_item3), text },
#define TAG(text)  { enumi (ManPage_TYPE, tag), text },
#define TAG1(text)  { enumi (ManPage_TYPE, tag1), text },
#define TAG2(text)  { enumi (ManPage_TYPE, tag2), text },
#define TAG3(text)  { enumi (ManPage_TYPE, tag3), text },
#define DEFINITION(text)  { enumi (ManPage_TYPE, definition), text },
#define DEFINITION1(text)  { enumi (ManPage_TYPE, definition1), text },
#define DEFINITION2(text)  { enumi (ManPage_TYPE, definition2), text },
#define DEFINITION3(text)  { enumi (ManPage_TYPE, definition3), text },
#define CODE(text)  { enumi (ManPage_TYPE, code), text },
#define CODE1(text)  { enumi (ManPage_TYPE, code1), text },
#define CODE2(text)  { enumi (ManPage_TYPE, code2), text },
#define CODE3(text)  { enumi (ManPage_TYPE, code3), text },
#define CODE4(text)  { enumi (ManPage_TYPE, code4), text },
#define CODE5(text)  { enumi (ManPage_TYPE, code5), text },
#define PROTOTYPE(text)  { enumi (ManPage_TYPE, prototype), text },
#define FORMULA(text)  { enumi (ManPage_TYPE, formula), text },
#define PICTURE(width,height,draw)  { enumi (ManPage_TYPE, picture), NULL, width, height, draw },
#define SCRIPT(width,height,text)  { enumi (ManPage_TYPE, script), text, width, height },
#define MAN_END  { 0 } }; ManPages_addPage (me, title, author, date, page); }

/* End of file ManPagesM.h */
#endif
