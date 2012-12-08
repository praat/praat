/* GraphicsPostscript.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include <math.h>	/* For 'floor' and 'ceil' in BoundingBox. */
#include <time.h>	/* For creation date. */
#include "GraphicsP.h"
#include "Printer.h"

Thing_implement (GraphicsPostscript, Graphics, 0);

static void downloadPrologAndSetUp (GraphicsPostscript me) {
	/*
	 * Procedures section of the document prolog: procedure definitions valid for all pages.
	 */
	my d_printf (my d_file, "%%%%BeginProlog\n");
	my d_printf (my d_file, "%%%%BeginResource: procset (ppgb GraphicsPostscript procs) 1.0 0\n");
	my d_printf (my d_file, "/N { newpath } bind def /M { newpath moveto } bind def /L { rlineto } bind def\n");
	my d_printf (my d_file, "/F { 10 { 1 exch rlineto } repeat } bind def\n");
	my d_printf (my d_file, "/C { 0 360 arc stroke } bind def /FC { 0 360 arc fill } bind def\n");
	my d_printf (my d_file,
		"/PraatEncoding [\n"
		"	/dotlessi/Aogonek/aogonek/Cacute/cacute/Ccaron/ccaron/Dcaron/dcaron/Dbar\n"
		"	/dbar/Ecaron/ecaron/Eogonek/eogonek/Gcaron/gcaron/Lslash/lslash/Nacute\n"
		"	/nacute/Ncaron/ncaron/Ohungarumlaut/ohungarumlaut/Rcaron/rcaron/Sacute/sacute/Scaron\n"
		"	/scaron/Tcaron/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quotesinglright\n"
		"	/parenleft/parenright/asterisk/plus/comma/hyphen/period/slash/zero/one\n"
		"	/two/three/four/five/six/seven/eight/nine/colon/semicolon\n"
		"	/less/equal/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z\n"
		"	/bracketleft/backslash/bracketright/asciicircum/underscore/quotesinglleft\n"
		"	/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde\n"
		"	/Zdot/Adieresis/Aring/Ccedilla/Eacute/Ntilde/Odieresis/Udieresis/aacute/agrave/acircumflex\n"
		"	/adieresis/atilde/aring/ccedilla/eacute/egrave/ecircumflex/edieresis/iacute/igrave/icircumflex\n");
	my d_printf (my d_file,
		"	/idieresis/ntilde/oacute/ograve/ocircumflex/odieresis/otilde/uacute/ugrave/ucircumflex/udieresis\n"
		"	/dagger/degree/cent/sterling/section/bullet/paragraph/germandbls/registered/copyright/trademark\n"
		"	/acute/dieresis/notequal/AE/Oslash/infinity/plusminus/lessequal/greaterequal/yen/mu\n"
		"	/partialdiff/summation/product/pi/integral/ordfeminine/ordmasculine/Omega/ae/oslash\n"
		"	/questiondown/exclamdown/logicalnot/radical/florin/approxequal/Delta/guillemotleft/guillemotright\n"
		"	/ellipsis/zdot/Agrave/Atilde/Otilde/OE/oe/endash/emdash/quotedblleft/quotedblright\n"
		"	/quoteleft/quoteright/divide/lozenge/ydieresis/Ydieresis/fraction/currency\n"
		"	/guilsinglleft/guilsinglright/fi/fl/daggerdbl/periodcentered/quotesinglbase/quotedblbase\n"
		"	/perthousand/Acircumflex/Ecircumflex/Aacute/Edieresis/Egrave/Iacute/Icircumflex/Idieresis\n"
		"	/Igrave/Oacute/Ocircumflex/apple/Ograve/Uacute/Ucircumflex/Ugrave/tcaron\n"
		"	/Uhungarumlaut/uhungarumlaut/Uring/uring/Yacute/yacute/Zacute/zacute/Zcaron/zcaron ] def\n");
	my d_printf (my d_file,
		"/PraatEncode { /font exch def /base exch def\n"
		"	/basedict base findfont def /new basedict maxlength dict def\n"
		"	basedict { exch dup dup /FID ne exch /Encoding ne and\n"
		"	{ exch new 3 1 roll put } { pop pop } ifelse } forall\n"
		"	new /FontName font put new /Encoding PraatEncoding put font new definefont pop } def\n");
	my d_printf (my d_file, "%%%%EndResource\n");
	my d_printf (my d_file, "%%%%EndProlog\n");

	/*
	 * Document setup: graphics state changes persistent across showpage calls.
	 */
	my d_printf (my d_file, "%%%%BeginSetup\n");
	//my d_printf (my d_file, "%d %d { dup mul exch dup mul add 1.0 exch sub } setscreen\n", my spotsDensity, my spotsAngle);
	my d_printf (my d_file, "true setstrokeadjust\n");
	my d_printf (my d_file, "%%%%EndSetup\n");
}

static void initPage (GraphicsPostscript me) {
	++ my pageNumber;
	if (my printer && my pageNumber > 1) downloadPrologAndSetUp (me);   // has to be repeated for every page
	if (my job) {
		my d_printf (my d_file, "%%%%Page: %d %d\n", my pageNumber, my pageNumber);
		/*my d_printf (my d_file, "save\n");*/
		my d_printf (my d_file, "%%%%BeginPageSetup\n");
	}
	my d_printf (my d_file, "%g setlinewidth 2 setlinejoin\n", my resolution / 192.0);   /* 0.375 point */
	if (my job || my printer) {
		if (my landscape)
			my d_printf (my d_file, "%d 0 translate 90 rotate ", (int) (my paperHeight * 72 * my magnification));
	}
	my d_printf (my d_file, "%.6g dup scale\n", 72.0 * my magnification / my resolution);
	if (my job) my d_printf (my d_file, "%%%%EndPageSetup\n");
	my lastFid = NULL;
}

static void exitPage (GraphicsPostscript me) {
	int font, style;
	/*
	 * A showpage is only needed if we are printing to a file.
	 * If we are printing directly to a printer, the page will be ejected
	 * by EndPage (Windows NT/2000/XP/Vista) or PMSessionEndPage (MacOSX).
	 */
	if (my d_file) {
		if (my job) {
			/*my d_printf (my d_file, "restore\n");*/
			my d_printf (my d_file, "showpage\n");
		} else if (my eps) {
			my d_printf (my d_file, "showpage %% redefined by encapsulating program\n");
		} 
	}
	for (font = 0; font <= kGraphics_font_DINGBATS; font ++)
		for (style = 0; style <= Graphics_BOLD_ITALIC; style ++)
			Melder_free (my fontInfos [font] [style]);
	my loadedXipa = FALSE;   /* BUG. Include this because of the unpredictable page order with DSC? */
}

void structGraphicsPostscript :: v_destroy () {
	exitPage (this);
	if (d_file) {
		if (job) {
			d_printf (d_file, "%%%%Trailer\n");
			d_printf (d_file, "%%%%Pages: %d\n", pageNumber);
		}
		d_printf (d_file, "%%%%EOF\n");   // BUG. Correct according to DSC. But not good in EPS files?
		fclose (d_file);
	}
	GraphicsPostscript_Parent :: v_destroy ();
}

Graphics Graphics_create_postscriptjob (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
	enum kGraphicsPostscript_paperSize paperSize, enum kGraphicsPostscript_orientation rotation, double magnification)
{
	autoGraphicsPostscript me = Thing_new (GraphicsPostscript);
	time_t today;
	my postScript = true, my yIsZeroAtTheTop = false, my languageLevel = 2;
	my job = TRUE, my eps = FALSE, my printer = FALSE;
	my d_printf = (int (*)(void *, const char*, ...)) fprintf;
	Graphics_init (me.peek());
	my resolution = resolution;   /* Virtual resolution; may not be equal to that of the printer; */
					/* there is no problem if this always equals 600 dpi. */
	my photocopyable = spots == kGraphicsPostscript_spots_PHOTOCOPYABLE;
	if (my photocopyable) { my spotsDensity = 85; my spotsAngle = 35; }
	else { my spotsDensity = 106; my spotsAngle = 46; }
 	if (paperSize == kGraphicsPostscript_paperSize_A3) my paperWidth = 842 / 72.0, my paperHeight = 1191 / 72.0;
	else if (paperSize == kGraphicsPostscript_paperSize_US_LETTER) my paperWidth = 612 / 72.0, my paperHeight = 792 / 72.0;
	else my paperWidth = 595 / 72.0, my paperHeight = 842 / 72.0;
	my landscape = rotation == kGraphicsPostscript_orientation_LANDSCAPE;
	my magnification = magnification;
	my includeFonts = TRUE;
	my d_file = Melder_fopen (file, "w");
	/*
	 * The Device Coordinates are the PostScript user coordinates.
	 * They are chosen in such a way that a distance of 1 in device coordinates
	 * equals one dot if the printer's resolution is 'resolution' dots per inch.
	 * Take a sensible default margin: half an inch on all sides.
	 */
	my d_x1DC = my d_x1DCmin = resolution / 2;
	my d_x2DC = my d_x2DCmax = (my paperWidth - 0.5) * resolution;
	my d_y1DC = my d_y1DCmin = resolution / 2;
	my d_y2DC = my d_y2DCmax = (my paperHeight - 0.5) * resolution;
	/*
	 * Now don't just set x1wNDC etc, but force computation of the scaling as well.
	 */
	Graphics_setWsWindow ((Graphics) me.peek(), 0, my paperWidth - 1.0, 13.0 - my paperHeight, 12.0);
	/*
	 * We will adhere to version 3.0 of the Document Structuring Conventions for print jobs.
	 */
	my d_printf (my d_file, "%%!PS-Adobe-3.0\n");
	my d_printf (my d_file, "%%%%Creator: Praat Shell 4.2\n");
	my d_printf (my d_file, "%%%%Title: %s\n", Melder_peekWcsToUtf8 (MelderFile_name (file)));
	today = time (NULL);
	my d_printf (my d_file, "%%%%CreationDate: %s", ctime (& today));   /* Contains newline symbol. */
	my d_printf (my d_file, "%%%%PageOrder: Special\n");
	my d_printf (my d_file, "%%%%Pages: (atend)\n");
	my d_printf (my d_file, "%%%%EndComments\n");
	downloadPrologAndSetUp (me.peek());
	initPage (me.peek());
	return (Graphics) me.transfer();
}

#if defined (macintosh)
static int Eps_postScript_printf (void *stream, const char *format, ... ) {
	static char theLine [3002];
	char *p;
	va_list args;
	va_start (args, format);
	vsprintf (theLine, format, args);
	va_end (args);
	for (p = theLine; *p != '\0'; p ++) if (*p == '\n') *p = '\r';
	return fwrite (theLine, sizeof (char), strlen (theLine), reinterpret_cast <FILE *> (stream));
}
#endif

Graphics Graphics_create_epsfile (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
	double x1inches, double x2inches, double y1inches, double y2inches, bool includeFonts, bool useSilipaPS)
{
	autoGraphicsPostscript me = Thing_new (GraphicsPostscript);
	time_t today;
	int left, right, top, bottom;
	my postScript = true, my languageLevel = 2;
	my job = false, my eps = true, my printer = false;
	#if defined (macintosh)
		/* Replace newlines with carriage returns to be compatible with MS Word 5.1. */
		my d_printf = Eps_postScript_printf;
	#else
		my d_printf = (int (*)(void *, const char*, ...)) fprintf;
	#endif
	Graphics_init (me.peek());
	my resolution = resolution;   /* Virtual resolution; may not be equal to that of the printer; */
					/* there is no problem if this always equals 600 dpi. */
	my photocopyable = spots == kGraphicsPostscript_spots_PHOTOCOPYABLE;
	if (my photocopyable) { my spotsDensity = 85; my spotsAngle = 35; }
	else { my spotsDensity = 106; my spotsAngle = 46; }
	my paperWidth = 7.5, my paperHeight = 11.0;
	my landscape = FALSE;
	my magnification = 1.0;
	my includeFonts = includeFonts;
	my useSilipaPS = useSilipaPS;
	my d_file = Melder_fopen (file, "w");
	my d_x1DC = my d_x1DCmin = 0;
	my d_x2DC = my d_x2DCmax = my paperWidth * resolution; /* 600 dpi -> 4500 virtual dots */
	my d_y1DC = my d_y1DCmin = 0;
	my d_y2DC = my d_y2DCmax = my paperHeight * resolution; /* 600 dpi -> 6600 virtual dots */
	Graphics_setWsWindow ((Graphics) me.peek(), 0, my paperWidth, 12.0 - my paperHeight, 12.0);   // force scaling
	/*
	 * We will honour version 3.0 of the DSC for Encapsulated PostScript files,
	 * which includes supplying the bounding box information.
	 */
	left = (int) floor (x1inches * 72);
	right = (int) ceil (x2inches * 72);
	top = (int) ceil ((y2inches - my d_y1wNDC) * 72);
	bottom = (int) floor ((y1inches - my d_y1wNDC) * 72);
	my d_printf (my d_file, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	my d_printf (my d_file, "%%%%BoundingBox: %d %d %d %d\n", left, bottom, right, top);
	my d_printf (my d_file, "%%%%Creator: Praat Shell 5.1\n");
	/*
	 * In an EPS file without screen preview, the file name will be visible anyway.
	 * This leaves us room to show a warning that should keep users from thinking anything is wrong.
	 */
	my d_printf (my d_file, "%%%%Title: NO SCREEN PREVIEW, BUT WILL PRINT CORRECTLY\n");
	today = time (NULL);
	my d_printf (my d_file, "%%%%CreationDate: %s", ctime (& today));   /* Contains newline symbol. */
	my d_printf (my d_file, "%%%%EndComments\n");
	downloadPrologAndSetUp (me.peek());
	initPage (me.peek());
	return (Graphics) me.transfer();
}

#ifndef UNIX
Graphics Graphics_create_postscriptprinter (void) {
	GraphicsPostscript me = Thing_new (GraphicsPostscript);
	my postScript = TRUE, my languageLevel = 2;
	my job = FALSE, my eps = FALSE, my printer = TRUE;
	my d_printf = Printer_postScript_printf;
	Graphics_init (me);
	my resolution = thePrinter. resolution;   /* Virtual resolution. */
	my photocopyable = thePrinter. spots == kGraphicsPostscript_spots_PHOTOCOPYABLE;
	if (my photocopyable) { my spotsDensity = 85; my spotsAngle = 35; }
	else { my spotsDensity = 106; my spotsAngle = 46; }
	my paperWidth = (double) thePrinter. paperWidth / my resolution;
	my paperHeight = (double) thePrinter. paperHeight / my resolution;
	my landscape = thePrinter. orientation == kGraphicsPostscript_orientation_LANDSCAPE;
	my magnification = thePrinter. magnification;
	my includeFonts = TRUE;
	my d_x1DC = my d_x1DCmin = my resolution / 2;
	my d_x2DC = my d_x2DCmax = (my paperWidth - 0.5) * my resolution;
	my d_y1DC = my d_y1DCmin = my resolution / 2;
	my d_y2DC = my d_y2DCmax = (my paperHeight - 0.5) * my resolution;
	Graphics_setWsWindow ((Graphics) me, 0, my paperWidth - 1.0, 13.0 - my paperHeight, 12.0);
	downloadPrologAndSetUp (me);
	initPage (me);
	return (Graphics) me;
}
#endif

void Graphics_nextSheetOfPaper (Graphics graphics) {
	if (graphics -> postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (graphics);
		exitPage (me);
		if (my printer) Printer_nextPage ();
		initPage (me);
	} else if (graphics -> printer) {
		Printer_nextPage ();
	}
}

/* End of file GraphicsPostscript.cpp */
