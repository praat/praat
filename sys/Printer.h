#ifndef _Printer_h_
#define _Printer_h_
/* Printer.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2004/05/25 font choice strategy
 * pb 2005/05/19 preference for whether EPS files include a screen preview
 */

#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

/* When changing the following structure, update its initialization in Printer.c */
struct Printer {
	int spots, paperSize, orientation, postScript, allowDirectPostScript;
	int fontChoiceStrategy, epsFilesHavePreview;
	long resolution, paperWidth, paperHeight;
	double magnification;
	Graphics graphics;
};

extern struct Printer thePrinter;
extern int nt_is_running;

#if defined (macintosh) || defined (_WIN32)
	int Printer_postScript_printf (void *stream, const char *format, ... );
#endif
#ifdef _WIN32
	HDC Printer_getDC (void);
#endif
int Printer_pageSetup (void);
int Printer_postScriptSettings (void);
int Printer_print (void (*draw) (I, Graphics g), I);
void Printer_nextPage (void);

void Printer_prefs (void);

/* End of file Printer.h */
#endif
