#ifndef _ManPage_h_
#define _ManPage_h_
/* ManPage.h
 *
 * Copyright (C) 1996-2004 Paul Boersma
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
 * pb 2004/10/16
 */

#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#include "ManPage_enums.h"

typedef struct structManPage_Paragraph {
	short type;
	const char *text;
	float width, height;
	void (*draw) (Graphics g);
} *ManPage_Paragraph;

#define ManPage_members Thing_members \
	const char *title, *author; \
	long date; \
	struct structManPage_Paragraph *paragraphs; \
	long nlinksHither, nlinksThither; \
	long *linksHither, *linksThither; \
	double recordingTime;
#define ManPage_methods Thing_methods
class_create (ManPage, Thing)

/* End of file ManPage.h */
#endif
