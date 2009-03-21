#ifndef _ManPagesM_h_
#define _ManPagesM_h_
/* ManPagesM.h
 *
 * Copyright (C) 1996-2009 Paul Boersma
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
 * pb 2009/03/18
 */

/* ManPages macros. */

#ifndef _ManPages_h_
	#include "ManPages.h"
#endif

#define MAN_BEGIN(t,a,d)  { const wchar_t *title = t, *author = a; long date = d; \
	static struct structManPage_Paragraph page [] = {
#define INTRO(text)  { kManPage_type_INTRO, text },
#define ENTRY(text)  { kManPage_type_ENTRY, text },
#define NORMAL(text)  { kManPage_type_NORMAL, text },
#define LIST_ITEM(text)  { kManPage_type_LIST_ITEM, text },
#define LIST_ITEM1(text)  { kManPage_type_LIST_ITEM1, text },
#define LIST_ITEM2(text)  { kManPage_type_LIST_ITEM2, text },
#define LIST_ITEM3(text)  { kManPage_type_LIST_ITEM3, text },
#define TAG(text)  { kManPage_type_TAG, text },
#define TAG1(text)  { kManPage_type_TAG1, text },
#define TAG2(text)  { kManPage_type_TAG2, text },
#define TAG3(text)  { kManPage_type_TAG3, text },
#define DEFINITION(text)  { kManPage_type_DEFINITION, text },
#define DEFINITION1(text)  { kManPage_type_DEFINITION1, text },
#define DEFINITION2(text)  { kManPage_type_DEFINITION2, text },
#define DEFINITION3(text)  { kManPage_type_DEFINITION3, text },
#define CODE(text)  { kManPage_type_CODE, text },
#define CODE1(text)  { kManPage_type_CODE1, text },
#define CODE2(text)  { kManPage_type_CODE2, text },
#define CODE3(text)  { kManPage_type_CODE3, text },
#define CODE4(text)  { kManPage_type_CODE4, text },
#define CODE5(text)  { kManPage_type_CODE5, text },
#define PROTOTYPE(text)  { kManPage_type_PROTOTYPE, text },
#define FORMULA(text)  { kManPage_type_FORMULA, text },
#define PICTURE(width,height,draw)  { kManPage_type_PICTURE, NULL, width, height, draw },
#define SCRIPT(width,height,text)  { kManPage_type_SCRIPT, text, width, height },
#define MAN_END  { 0 } }; ManPages_addPage (me, title, author, date, page); }

/* End of file ManPagesM.h */
#endif
