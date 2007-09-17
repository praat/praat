/* GraphicsPostscript.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/03/14 Mach
 * pb 2002/11/17 removed showpage from Windows printing
 * pb 2004/03/15 updated version number to 4.2 (after introduction of flexible fonts)
 * pb 2005/09/18 useSilipaPS
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/08/01 reintroduced yIsZeroAtTheTop
 */

#include <math.h>	/* For 'floor' and 'ceil' in BoundingBox. */
#include <time.h>	/* For creation date. */
#include "GraphicsP.h"
#include "Printer.h"

static void downloadPrologAndSetUp (GraphicsPostscript me) {
	/*
	 * Procedures section of the document prolog: procedure definitions valid for all pages.
	 */
	my printf (my file, "%%%%BeginProlog\n");
	my printf (my file, "%%%%BeginResource: procset (ppgb GraphicsPostscript procs) 1.0 0\n");
	my printf (my file, "/N { newpath } bind def /M { newpath moveto } bind def /L { rlineto } bind def\n");
	my printf (my file, "/F { 10 { 1 exch rlineto } repeat } bind def\n");
	if (my languageLevel == 1)
		my printf (my file, "/FONT { exch findfont exch scalefont setfont } bind def\n");
	my printf (my file, "/C { 0 360 arc stroke } bind def /FC { 0 360 arc fill } bind def\n");
	my printf (my file,
		"/PraatEncoding [\n"
		"	/.notdef/ccaron/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
		"	/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
		"	/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
		"	/.notdef/.notdef/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quotesinglright\n"
		"	/parenleft/parenright/asterisk/plus/comma/hyphen/period/slash/zero/one\n"
		"	/two/three/four/five/six/seven/eight/nine/colon/semicolon\n"
		"	/less/equal/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N/O/P/Q/R/S/T/U/V/W/X/Y/Z\n"
		"	/bracketleft/backslash/bracketright/asciicircum/underscore/quotesinglleft\n"
		"	/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde\n"
		"	/.notdef/Adieresis/Aring/Ccedilla/Eacute/Ntilde/Odieresis/Udieresis/aacute/agrave/acircumflex\n"
		"	/adieresis/atilde/aring/ccedilla/eacute/egrave/ecircumflex/edieresis/iacute/igrave/icircumflex\n");
	my printf (my file,
		"	/idieresis/ntilde/oacute/ograve/ocircumflex/odieresis/otilde/uacute/ugrave/ucircumflex/udieresis\n"
		"	/dagger/degree/cent/sterling/section/bullet/paragraph/germandbls/registered/copyright/trademark\n"
		"	/acute/dieresis/notequal/AE/Oslash/infinity/plusminus/lessequal/greaterequal/yen/mu\n"
		"	/partialdiff/summation/product/pi/integral/ordfeminine/ordmasculine/Omega/ae/oslash\n"
		"	/questiondown/exclamdown/logicalnot/radical/florin/approxequal/Delta/guillemotleft/guillemotright\n"
		"	/ellipsis/.notdef/Agrave/Atilde/Otilde/OE/oe/endash/emdash/quotedblleft/quotedblright\n"
		"	/quoteleft/quoteright/divide/lozenge/ydieresis/Ydieresis/fraction/currency\n"
		"	/guilsinglleft/guilsinglright/fi/fl/daggerdbl/periodcentered/quotesinglbase/quotedblbase\n"
		"	/perthousand/Acircumflex/Ecircumflex/Aacute/Edieresis/Egrave/Iacute/Icircumflex/Idieresis\n"
		"	/Igrave/Oacute/Ocircumflex/apple/Ograve/Uacute/Ucircumflex/Ugrave/dotlessi\n"
		"	/circumflex/tilde/macron/breve/dotaccent/ring/cedilla/hungarumlaut/ogonek/caron ] def\n");
	my printf (my file,
		"/PraatEncode { /font exch def /base exch def\n"
		"	/basedict base findfont def /new basedict maxlength dict def\n"
		"	basedict { exch dup dup /FID ne exch /Encoding ne and\n"
		"	{ exch new 3 1 roll put } { pop pop } ifelse } forall\n"
		"	new /FontName font put new /Encoding PraatEncoding put font new definefont pop } def\n");
	my printf (my file, "%%%%EndResource\n");
	my printf (my file, "%%%%EndProlog\n");

	/*
	 * Document setup: graphics state changes persistent across showpage calls.
	 */
	my printf (my file, "%%%%BeginSetup\n");
	my printf (my file, "%d %d { dup mul exch dup mul add 1.0 exch sub } setscreen\n", my spotsDensity, my spotsAngle);
	if (my languageLevel == 1)
		my printf (my file, "/languagelevel where {pop languagelevel 1 ne {true setstrokeadjust} if} if\n");
	else
		my printf (my file, "true setstrokeadjust\n");
	my printf (my file, "%%%%EndSetup\n");
}

static void initPage (GraphicsPostscript me) {
	++ my pageNumber;
	if (my printer && my pageNumber > 1) downloadPrologAndSetUp (me);   /* Has to be repeated at every page. */
	if (my job) {
		my printf (my file, "%%%%Page: %d %d\n", my pageNumber, my pageNumber);
		/*my printf (my file, "save\n");*/
		my printf (my file, "%%%%BeginPageSetup\n");
	}
	my printf (my file, "%g setlinewidth 2 setlinejoin\n", my resolution / 192.0);   /* 0.375 point */
	if (my job || my printer) {
		if (my landscape)
			my printf (my file, "%d 0 translate 90 rotate ", (int) (my paperHeight * 72 * my magnification));
	}
	my printf (my file, "%.6g dup scale\n", 72.0 * my magnification / my resolution);
	if (my job) my printf (my file, "%%%%EndPageSetup\n");
	my lastFid = NULL;
}

static void exitPage (GraphicsPostscript me) {
	int font, style;
	/*
	 * A showpage is only needed if we are printing to a file.
	 * If we are printing directly to a printer, the page will be ejected
	 * by EndPage (Windows NT/2000/XP) or PMSessionEndPage (MacOSX) or PrClosePage (Old Mac).
	 * Only on Windows 95 and 98 do we need the showpage, since EndPage is not called there.
	 */
	if (my file) {
		if (my job) {
			/*my printf (my file, "restore\n");*/
			my printf (my file, "showpage\n");
		} else if (my eps) {
			my printf (my file, "showpage %% redefined by encapsulating program\n");
		} 
	}
	#ifdef _WIN32
		if (my printer && ! nt_is_running)
			my printf (my file, "showpage\n");
	#endif
	for (font = 0; font <= Graphics_DINGBATS; font ++)
		for (style = 0; style <= Graphics_BOLD_ITALIC; style ++)
			Melder_free (my fontInfos [font] [style]);
	my loadedXipa = FALSE;   /* BUG. Include this because of the unpredictable page order with DSC? */
}

static void destroy (I) {
	iam (GraphicsPostscript);
	exitPage (me);
	if (my file) {
		if (my job) {
			my printf (my file, "%%%%Trailer\n");
			my printf (my file, "%%%%Pages: %d\n", my pageNumber);
		}
		my printf (my file, "%%%%EOF\n");   /* BUG. Correct according to DSC. But not good in EPS files? */
		fclose (my file);
	}
	inherited (GraphicsPostscript) destroy (me);
}

class_methods (GraphicsPostscript, Graphics)
	class_method (destroy)
class_methods_end

Graphics Graphics_create_postscriptjob (MelderFile fs, int resolution, int spots,
	int paperSize, int rotation, double magnification)
{
	GraphicsPostscript me = new (GraphicsPostscript);
	time_t today;
	my postScript = true, my yIsZeroAtTheTop = true, my languageLevel = 2;
	my job = TRUE, my eps = FALSE, my printer = FALSE;
	my printf = (int (*)(void *, const char*, ...)) fprintf;
	if (! Graphics_init (me)) return NULL;
	my resolution = resolution;   /* Virtual resolution; may not be equal to that of the printer; */
					/* there is no problem if this always equals 600 dpi. */
	my photocopyable = spots == GraphicsPostscript_PHOTOCOPYABLE;
	if (my photocopyable) { my spotsDensity = 85; my spotsAngle = 35; }
	else { my spotsDensity = 106; my spotsAngle = 46; }
 	if (paperSize == Graphics_A3) my paperWidth = 842 / 72.0, my paperHeight = 1191 / 72.0;
	else if (paperSize == Graphics_US_LETTER) my paperWidth = 612 / 72.0, my paperHeight = 792 / 72.0;
	else my paperWidth = 595 / 72.0, my paperHeight = 842 / 72.0;
	my landscape = rotation == Graphics_LANDSCAPE;
	my magnification = magnification;
	my includeFonts = TRUE;
	if ((my file = Melder_fopen (fs, "w")) == NULL) { forget (me); return 0; }
	/*
	 * The Device Coordinates are the PostScript user coordinates.
	 * They are chosen in such a way that a distance of 1 in device coordinates
	 * equals one dot if the printer's resolution is 'resolution' dots per inch.
	 * Take a sensible default margin: half an inch on all sides.
	 */
	my x1DC = my x1DCmin = resolution / 2;
	my x2DC = my x2DCmax = (my paperWidth - 0.5) * resolution;
	my y1DC = my y1DCmin = resolution / 2;
	my y2DC = my y2DCmax = (my paperHeight - 0.5) * resolution;
	/*
	 * Now don't just set x1wNDC etc, but force computation of the scaling as well.
	 */
	Graphics_setWsWindow ((Graphics) me, 0, my paperWidth - 1.0, 13.0 - my paperHeight, 12.0);
	/*
	 * We will adhere to version 3.0 of the Document Structuring Conventions for print jobs.
	 */
	my printf (my file, "%%!PS-Adobe-3.0\n");
	my printf (my file, "%%%%Creator: Praat Shell 4.2\n");
	my printf (my file, "%%%%Title: %s\n", MelderFile_name (fs));
	today = time (NULL);
	my printf (my file, "%%%%CreationDate: %s", ctime (& today));   /* Contains newline symbol. */
	my printf (my file, "%%%%PageOrder: Special\n");
	my printf (my file, "%%%%Pages: (atend)\n");
	my printf (my file, "%%%%EndComments\n");
	downloadPrologAndSetUp (me);
	initPage (me);
	return (Graphics) me;
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
	return fwrite (theLine, sizeof (char), strlen (theLine), stream);
}
#endif

Graphics Graphics_create_epsfile (MelderFile fs, int resolution, int spots,
	double x1inches, double x2inches, double y1inches, double y2inches, int includeFonts, int useSilipaPS)
{
	GraphicsPostscript me = new (GraphicsPostscript);
	time_t today;
	int left, right, top, bottom;
	my postScript = TRUE, my languageLevel = 2;
	my job = FALSE, my eps = TRUE, my printer = FALSE;
	#if defined (macintosh)
		/* Replace newlines with carriage returns to be compatible with MS Word 5.1. */
		my printf = Eps_postScript_printf;
	#else
		my printf = (int (*)(void *, const char*, ...)) fprintf;
	#endif
	if (! Graphics_init (me)) return NULL;
	my resolution = resolution;   /* Virtual resolution; may not be equal to that of the printer; */
					/* there is no problem if this always equals 600 dpi. */
	my photocopyable = spots == GraphicsPostscript_PHOTOCOPYABLE;
	if (my photocopyable) { my spotsDensity = 85; my spotsAngle = 35; }
	else { my spotsDensity = 106; my spotsAngle = 46; }
	my paperWidth = 7.5, my paperHeight = 11.0;
	my landscape = FALSE;
	my magnification = 1.0;
	my includeFonts = includeFonts;
	my useSilipaPS = useSilipaPS;
	if ((my file = Melder_fopen (fs, "w")) == NULL) { forget (me); return 0; }
	my x1DC = my x1DCmin = 0;
	my x2DC = my x2DCmax = my paperWidth * resolution; /* 600 dpi -> 4500 virtual dots */
	my y1DC = my y1DCmin = 0;
	my y2DC = my y2DCmax = my paperHeight * resolution; /* 600 dpi -> 6600 virtual dots */
	Graphics_setWsWindow ((Graphics) me, 0, my paperWidth, 12.0 - my paperHeight, 12.0);   /* Force scaling. */
	/*
	 * We will honour version 3.0 of the DSC for Encapsulated PostScript files,
	 * which includes supplying the bounding box information.
	 */
	left = (int) floor (x1inches * 72);
	right = (int) ceil (x2inches * 72);
	top = (int) ceil ((y2inches - my y1wNDC) * 72);
	bottom = (int) floor ((y1inches - my y1wNDC) * 72);
	my printf (my file, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	my printf (my file, "%%%%BoundingBox: %d %d %d %d\n", left, bottom, right, top);
	my printf (my file, "%%%%Creator: Praat Shell 3.8\n");
	/*
	 * In an EPS file without screen preview, the file name will be visible anyway.
	 * This leaves us room to show a warning that should keep users from thinking anything is wrong.
	 */
	my printf (my file, "%%%%Title: NO SCREEN PREVIEW, BUT WILL PRINT CORRECTLY\n");
	today = time (NULL);
	my printf (my file, "%%%%CreationDate: %s", ctime (& today));   /* Contains newline symbol. */
	my printf (my file, "%%%%EndComments\n");
	downloadPrologAndSetUp (me);
	initPage (me);
	return (Graphics) me;
}

#ifndef UNIX
Graphics Graphics_create_postscriptprinter (void) {
	GraphicsPostscript me = new (GraphicsPostscript);
	my postScript = TRUE, my languageLevel = 2;
	my job = FALSE, my eps = FALSE, my printer = TRUE;
	my printf = Printer_postScript_printf;
	if (! Graphics_init (me)) return NULL;
	my resolution = thePrinter. resolution;   /* Virtual resolution. */
	my photocopyable = thePrinter. spots == GraphicsPostscript_PHOTOCOPYABLE;
	if (my photocopyable) { my spotsDensity = 85; my spotsAngle = 35; }
	else { my spotsDensity = 106; my spotsAngle = 46; }
	my paperWidth = (double) thePrinter. paperWidth / my resolution;
	my paperHeight = (double) thePrinter. paperHeight / my resolution;
	my landscape = thePrinter. orientation == Graphics_LANDSCAPE;
	my magnification = thePrinter. magnification;
	my includeFonts = TRUE;
	my x1DC = my x1DCmin = my resolution / 2;
	my x2DC = my x2DCmax = (my paperWidth - 0.5) * my resolution;
	my y1DC = my y1DCmin = my resolution / 2;
	my y2DC = my y2DCmax = (my paperHeight - 0.5) * my resolution;
	Graphics_setWsWindow ((Graphics) me, 0, my paperWidth - 1.0, 13.0 - my paperHeight, 12.0);
	downloadPrologAndSetUp (me);
	initPage (me);
	return (Graphics) me;
}
#endif

void Graphics_nextSheetOfPaper (I) {
	iam (Graphics);
	if (my postScript) {
		iam (GraphicsPostscript);
		exitPage (me);
		if (my printer) Printer_nextPage ();
		initPage (me);
	} else if (my printer) {
		Printer_nextPage ();
	}
}

/* End of file GraphicsPostscript.c */
