/* Picture.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2002/01/29
 * pb 2002/03/07 GPL
 * pb 2003/07/19 extra null byte in Windows metafile name
 * pb 2004/02/19 outer selection
 * pb 2004/09/05 inner selection
 */

#include "melder.h"

#if defined (macintosh)
	#include "macport_on.h"
	#include <Resources.h>
	#include <Files.h>
	#include <Memory.h>
	#include <Scrap.h>
	#include <Quickdraw.h>
	#include <MacWindows.h>
	#include <PictUtils.h>
	#include <Script.h>
	#if TARGET_API_MAC_CARBON
		#define carbon 1
	#else
		#define carbon 0
		#define GetPortVisibleRegion(port,region)  (region = (port) -> visRgn)
	#endif
	#include "macport_off.h"
#endif

#include "Gui.h"
#include "Printer.h"
#include "Picture.h"
#include "site.h"

struct structPicture {
	Widget drawingArea;
	Any graphics, selectionGraphics;
	Boolean sensitive;
	double selx1, selx2, sely1, sely2;   /* Selection in NDC co-ordinates. */
	void (*selectionChangedCallback) (struct structPicture *, XtPointer, double, double, double, double);
	XtPointer selectionChangedClosure;
	int backgrounding, mouseSelectsInnerViewport;
#if defined (UNIX)
	Region updateRegion;
#endif
};

static void drawMarkers (Picture me)
/*
 * The drawing area is a square measuring 12x12 inches.
 */
#define SIDE 12
/*
 * The selection grid has a resolution of 1/2 inch.
 */
#define SQUARES 24
/*
 * Vertical and horizontal lines every 3 inches.
 */
#define YELLOW_GRID 3
{
	int i;

	/* Fill the entire canvas with GC's background. */

	Graphics_setColour (my selectionGraphics, Graphics_WHITE);
	Graphics_fillRectangle (my selectionGraphics, 0, SIDE, 0, SIDE);

	/* Draw yellow grid lines for coarse navigation. */

	Graphics_setColour (my selectionGraphics, Graphics_YELLOW);
	for (i = YELLOW_GRID; i < SIDE; i += YELLOW_GRID) {
		Graphics_line (my selectionGraphics, 0, i, SIDE, i);
		Graphics_line (my selectionGraphics, i, 0, i, SIDE);
	}

	/* Draw red ticks and numbers for feedback on viewport measurement. */

	Graphics_setColour (my selectionGraphics, Graphics_RED);
	for (i = 1; i < SIDE; i ++) {
		double x = i;
		Graphics_setTextAlignment (my selectionGraphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_printf (my selectionGraphics, x, SIDE, "%d", i);
		Graphics_setTextAlignment (my selectionGraphics, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_printf (my selectionGraphics, x, 0, "%d", i);
	}
	for (i = 1; i < SQUARES ; i ++) {   /* Vertical ticks. */
		double x = 0.5 * i;
		Graphics_line (my selectionGraphics, x, SIDE - 0.04, x, SIDE);
		Graphics_line (my selectionGraphics, x, 0, x, 0.04);
	}
	for (i = 1; i < SIDE; i ++) {
		double y = SIDE - i;
		Graphics_setTextAlignment (my selectionGraphics, Graphics_LEFT, Graphics_HALF);
		Graphics_printf (my selectionGraphics, 0.04, y, "%d", i);
		Graphics_setTextAlignment (my selectionGraphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_printf (my selectionGraphics, SIDE - 0.03, y, "%d", i);
	}
	for (i = 1; i < SQUARES; i ++) {   /* Horizontal ticks. */
		double y = SIDE - 0.5 * i;
		Graphics_line (my selectionGraphics, SIDE - 0.04, y, SIDE, y);
		Graphics_line (my selectionGraphics, 0, y, 0.04, y);
	}

	Graphics_setColour (my selectionGraphics, Graphics_BLACK);
}

static void drawSelection (Picture me, int high) {
	double dx, dy;
	if (my backgrounding) return;
	dy = 2.8 * Graphics_inqFontSize (my graphics) / 72.0;
	dx = 1.5 * dy;
	if (dy > 0.4 * (my sely2 - my sely1)) dy = 0.4 * (my sely2 - my sely1);
	if (dx > 0.4 * (my selx2 - my selx1)) dx = 0.4 * (my selx2 - my selx1);
	if (high)
		Graphics_highlight2 (my selectionGraphics, my selx1, my selx2, my sely1, my sely2,
			my selx1 + dx, my selx2 - dx, my sely1 + dy, my sely2 - dy);
	else
		Graphics_unhighlight2 (my selectionGraphics, my selx1, my selx2, my sely1, my sely2,
			my selx1 + dx, my selx2 - dx, my sely1 + dy, my sely2 - dy);
}

static int SMERIG_valid;
MOTIF_CALLBACK (cb_draw)
	iam (Picture);
#if defined (macintosh)
	WindowPtr window = (WindowPtr) ((EventRecord *) call) -> message;
	static RgnHandle visRgn;
	Rect rect;
	short x1DC, x2DC, y1DC, y2DC;
	Graphics_inqWsViewport (my selectionGraphics, & x1DC, & x2DC, & y1DC, & y2DC);
	SetRect (& rect, x1DC, y1DC, x2DC, y2DC);
	/* No clearing needed; macintosh clips to update region. */
	#if carbon
		if (visRgn == NULL) visRgn = NewRgn ();
		GetPortVisibleRegion (GetWindowPort (window), visRgn);
	#else
		visRgn = window -> visRgn;
	#endif
	if (RectInRgn (& rect, visRgn)) {
		drawMarkers (me);
		Graphics_play (my graphics, my graphics);
		drawSelection (me, 1);
	}
#elif defined (_WIN32)
	drawMarkers (me);
	Graphics_play (my graphics, my graphics);
	drawSelection (me, 1);
#else
	XExposeEvent *event = (XExposeEvent *) & ((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose;
	XRectangle rectangle;
	if (! my updateRegion) my updateRegion = XCreateRegion ();
	rectangle. x = (short) event -> x;
	rectangle. y = (short) event -> y;
	rectangle. width = (short) event -> width;
	rectangle. height = (short) event -> height;
	XUnionRectWithRegion (& rectangle, my updateRegion, my updateRegion);
	if (event -> count == 0) {
		XSetRegion (XtDisplay (my drawingArea), Graphics_x_getGC (my graphics), my updateRegion);
		if (my sensitive) XSetRegion (XtDisplay (my drawingArea), Graphics_x_getGC (my selectionGraphics), my updateRegion);
		drawMarkers (me);
		Graphics_play (my graphics, my graphics);
		if (my sensitive && ! SMERIG_valid) drawSelection (me, 1);
		XSetClipMask (XtDisplay (my drawingArea), Graphics_x_getGC (my graphics), None);
		if (my sensitive) XSetClipMask (XtDisplay (my drawingArea), Graphics_x_getGC (my selectionGraphics), None);
		XDestroyRegion (my updateRegion);
		my updateRegion = NULL;
	}
	SMERIG_valid = 0;
#endif
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_click)
	iam (Picture);
	MotifEvent event = MotifEvent_fromCallData (call);
	int xstart = MotifEvent_x (event);
	int ystart = MotifEvent_y (event);
	double xWC, yWC;
	int ixstart, iystart, ix, iy, oldix = 0, oldiy = 0;
	if (! MotifEvent_isButtonPressedEvent (event)) return;

	Graphics_DCtoWC (my selectionGraphics, xstart, ystart, & xWC, & yWC);
	ix = ixstart = 1 + floor (xWC * SQUARES / SIDE);
	iy = iystart = SQUARES - floor (yWC * SQUARES / SIDE);
	if (ixstart < 1 || ixstart > SQUARES || iystart < 1 || iystart > SQUARES) return;
	if (MotifEvent_shiftKeyPressed (event)) {
		int ix1 = 1 + floor (my selx1 * SQUARES / SIDE);
		int ix2 = floor (my selx2 * SQUARES / SIDE);
		int iy1 = SQUARES + 1 - floor (my sely2 * SQUARES / SIDE);
		int iy2 = SQUARES - floor (my sely1 * SQUARES / SIDE);
		ixstart = ix < (ix1 + ix2) / 2 ? ix2 : ix1;
		iystart = iy < (iy1 + iy2) / 2 ? iy2 : iy1;
	}
	while (Graphics_mouseStillDown (my selectionGraphics)) {
		if (ix >= 1 && ix <= SQUARES && iy >= 1 && iy <= SQUARES && (ix != oldix || iy != oldiy)) {
			int ix1, ix2, iy1, iy2;
			if (ix < ixstart) { ix1 = ix; ix2 = ixstart; }
			else              { ix1 = ixstart; ix2 = ix; }
			if (iy < iystart) { iy1 = iy; iy2 = iystart; }
			else              { iy1 = iystart; iy2 = iy; }
			if (my mouseSelectsInnerViewport) {
				int fontSize = Graphics_inqFontSize (my graphics);
				double xmargin = fontSize * 4.2 / 72.0, ymargin = fontSize * 2.8 / 72.0;
				if (xmargin > ix2 - ix1 + 1) xmargin = ix2 - ix1 + 1;
				if (ymargin > iy2 - iy1 + 1) ymargin = iy2 - iy1 + 1;
				Picture_setSelection (me, 0.5 * (ix1 - 1) - xmargin, 0.5 * ix2 + xmargin,
					0.5 * (SQUARES - iy2) - ymargin, 0.5 * (SQUARES + 1 - iy1) + ymargin, False);
			} else {
				Picture_setSelection (me, 0.5 * (ix1 - 1), 0.5 * ix2,
					0.5 * (SQUARES - iy2), 0.5 * (SQUARES + 1 - iy1), False);
			}
			oldix = ix; oldiy = iy;
		}
		Graphics_getMouseLocation (my selectionGraphics, & xWC, & yWC);
		ix = 1 + floor (xWC * SQUARES / SIDE);
		iy = SQUARES - floor (yWC * SQUARES / SIDE);
	}
	if (my selectionChangedCallback)
		my selectionChangedCallback (me, my selectionChangedClosure,
			my selx1, my selx2, my sely1, my sely2);
MOTIF_CALLBACK_END

Picture Picture_create (Widget drawingArea, Boolean sensitive) {
	Picture me = (Picture) Melder_calloc (1, sizeof (struct structPicture));
	if (! me) return NULL;
	my drawingArea = drawingArea;
	/*
	 * The initial viewport is a rectangle 6 inches wide and 4 inches high.
	 */
	my selx1 = 0.0;
	my selx2 = 6.0;
	my sely1 = 8.0;
	my sely2 = 12.0;
	my sensitive = sensitive && drawingArea;
	if (drawingArea) {
		/* The drawing area must have been realized; see manual at XtWindow. */
		my graphics = Graphics_create_xmdrawingarea (my drawingArea);
		XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	} else {
		/*
		 * Create a dummy Graphics.
		 * This has device coordinates from 0 to 32767.
		 * This will be mapped on an area of 12x12 inches,
		 * so the resolution is 32767 / 12 = 2731.
		 */
		my graphics = Graphics_create (2731);
	}
	Graphics_setWsWindow (my graphics, 0.0, 12.0, 0.0, 12.0);
	Graphics_setViewport (my graphics, my selx1, my selx2, my sely1, my sely2);
	if (my sensitive) {
		my selectionGraphics = Graphics_create_xmdrawingarea (my drawingArea);
		XtAddCallback (my drawingArea, XmNinputCallback, cb_click, (XtPointer) me);
		Graphics_setWindow (my selectionGraphics, 0, 12, 0, 12);
	}
	Graphics_startRecording (my graphics);
	return me;
}

void Picture_setSelectionChangedCallback (Picture me,
	void (*selectionChangedCallback) (Picture, XtPointer, double, double, double, double),
	XtPointer selectionChangedClosure)
{
	my selectionChangedCallback = selectionChangedCallback;
	my selectionChangedClosure = selectionChangedClosure;
}

void Picture_setMouseSelectsInnerViewport (Picture me, int mouseSelectsInnerViewport) {
	my mouseSelectsInnerViewport = mouseSelectsInnerViewport;
}

void Picture_remove (Picture *pme) {
	Picture me = *pme;
	if (! me) return;
	Picture_erase (me);
	forget (my graphics);
	if (my sensitive) forget (my selectionGraphics);
	Melder_free (me);
	*pme = NULL;
}

Any Picture_getGraphics (Picture me) { return my graphics; }

void Picture_unhighlight (Picture me) {
	if (my drawingArea) drawSelection (me, 0);   /* Unselect. */
}

void Picture_highlight (Picture me) {
	if (my drawingArea) drawSelection (me, 1);   /* Select. */
}

void Picture_erase (Picture me) {
	Graphics_clearWs (my graphics);
	if (my drawingArea) {
		drawMarkers (me);
		drawSelection (me, 1);
	}
SMERIG_valid = 1;
}

int Picture_writeToPraatPictureFile (Picture me, MelderFile file) {
	FILE *f = Melder_fopen (file, "wb");
	if (! f) return 0;
	if (fprintf (f, "PraatPictureFile") < 0 || ! Graphics_writeRecordings (my graphics, f)) {
		fclose (f);
		return Melder_error ("Picture_writeToPraatPictureFile: "
			"error while writing file %.200s", MelderFile_messageName (file));
	}
	fclose (f);
	return 1;
}

static int Picture_readFromPraatPictureFile_any (Picture me, MelderFile file, int old) {
	FILE *f;
	int n;
	char line [200], *end, *tag = "PraatPictureFile";
	if ((f = Melder_fopen (file, "rb")) == NULL) return 0;
	n = fread (line, 1, 199, f); line [n] = '\0';
	end = strstr (line, tag);
	if (! end) {
		fclose (f);
		return Melder_error ("Picture_readFromPraatPictureFile: "
			"file %.200s is not a Praat picture file.", MelderFile_messageName (file));
	}
	*end = '\0';
	rewind (f);
	fread (line, 1, end - line + strlen (tag), f);
	if (old) {
		#ifdef _WIN32
		if (! Graphics_readRecordings_oldWindows (my graphics, f)) { fclose (f); return 0; }
		#endif
	} else {
		if (! Graphics_readRecordings (my graphics, f)) { fclose (f); return 0; }
	}
	fclose (f);
	Graphics_updateWs (my graphics);
	return 1;
}

int Picture_readFromPraatPictureFile (Picture me, MelderFile file) {
	return Picture_readFromPraatPictureFile_any (me, file, FALSE);
}

#ifdef _WIN32
int Picture_readFromOldWindowsPraatPictureFile (Picture me, MelderFile file) {
	return Picture_readFromPraatPictureFile_any (me, file, TRUE);
}
#endif

#ifdef macintosh
/* Macintosh pictures.
 * The maximum size for clipboard pictures, PICT pictures, and EPS previews is 7.5x11 inches.
 * Larger pictures may show recursion at the right, and noise at the bottom.
 * A4 printing in QuickDraw is also 7.5x11 inches.
 */
#define MAC_WIDTH  7.5
#define MAC_HEIGHT  11
static PicHandle copyToPict (Picture me) {
	PicHandle pict;
	Rect rect;
	#define RES 600
	OpenCPicParams openCPicParams;
	Graphics pictGraphics;
	SetRect (& rect, my selx1 * RES, (12 - my sely2) * RES, my selx2 * RES, (12 - my sely1) * RES);
	pictGraphics = Graphics_create_screen (NULL, (unsigned long) XtWindow (my drawingArea), RES);
	Graphics_setWsViewport (pictGraphics, 0, MAC_WIDTH * RES, 0, MAC_HEIGHT * RES);
	Graphics_setWsWindow (pictGraphics, 0.0, MAC_WIDTH, 12.0 - MAC_HEIGHT, 12.0);
	SetPortWindowPort ((WindowPtr) XtWindow (my drawingArea));
	openCPicParams. srcRect = rect;
	openCPicParams. hRes = RES << 16;
	openCPicParams. vRes = RES << 16;
	openCPicParams. version = -2;
	openCPicParams. reserved1 = 0;
	openCPicParams. reserved2 = 0;
	pict = OpenCPicture (& openCPicParams);
	if (! pict) return Melder_errorp ("Cannot create PICT.");
	PenSize (0, 0); MoveTo (0, 0); LineTo (0, 0); PenSize (1, 1);   /* Flush GrafPort state. */
	Graphics_play (my graphics, pictGraphics);
	ClosePicture ();
	forget (pictGraphics);
	/*{PictInfo info;
	GetPictInfo(pict,&info,recordComments,0, systemMethod,0);
	Melder_warning("res %d %d",info.hRes>>16,info.vRes>>16);
	}*/
	return pict;
}
static PicHandle copyToPict_screenImage (Picture me) {
	CGrafPtr savePort;
	GrafPtr offscreenPort;
	GDHandle saveDevice, offscreenDevice;
	GWorldPtr offscreenWorld;
	PicHandle pict;
	Graphics offScreen;
	Rect rect;
	GetGWorld (& savePort, & saveDevice);
	SetRect (& rect, 0, 0, MAC_WIDTH * 72, MAC_HEIGHT * 72);
	NewGWorld (& offscreenWorld, 8, & rect, NULL, NULL, keepLocal);
	SetGWorld (offscreenWorld, NULL);
	EraseRect (& rect);
	GetGWorld ((CGrafPtr *) & offscreenPort, & offscreenDevice);
	offScreen = Graphics_create_port (NULL, (unsigned long) offscreenPort, 72);
	Graphics_setWsViewport (offScreen, 0, MAC_WIDTH * 72, 0, MAC_HEIGHT * 72);
	Graphics_setWsWindow (offScreen, 0.0, MAC_WIDTH, 12.0 - MAC_HEIGHT, 12.0);
	Graphics_play (my graphics, offScreen);
	/*
	 * Copy the contents of the bitmap to a PICT.
	 */
	SetRect (& rect, my selx1 * 72, (12 - my sely2) * 72, my selx2 * 72, (12 - my sely1) * 72);
	SetPort (offscreenPort);
	pict = OpenPicture (& rect);
	/* The following statement may make the PICT larger than 32 kilobytes.
	 * BUG: from what system version is this possible?
	 * For 1-bit previews, it will usually be smaller than 32k,
	 * since Macintosh seems to use some sort of data compression.
	 */
	#if carbon
		CopyBits (GetPortBitMapForCopyBits (offscreenPort), GetPortBitMapForCopyBits (offscreenPort),
	#else
		CopyBits (& offscreenPort -> portBits, & offscreenPort -> portBits,
	#endif
			& rect, & rect, srcCopy, NULL);
	ClosePicture ();
	SetGWorld (savePort, saveDevice);
	DisposeGWorld (offscreenWorld);
	forget (offScreen);
	return pict;
}
void Picture_copyToClipboard (Picture me) {
	PicHandle pict = copyToPict (me);
	if (! pict) Melder_flushError (NULL);
	HLock ((Handle) pict);
	#if carbon
	{
		ScrapRef scrap;
		ClearCurrentScrap ();
		GetCurrentScrap (& scrap);
		PutScrapFlavor (scrap, 'PICT', 0, GetHandleSize ((Handle) pict), (Ptr) *pict);
	}
	#else
		ZeroScrap ();
		PutScrap (GetHandleSize ((Handle) pict), 'PICT', (Ptr) *pict);
	#endif
	HUnlock ((Handle) pict);
	KillPicture (pict);
}
void Picture_copyToClipboard_screenImage (Picture me) {
	PicHandle pict = copyToPict_screenImage (me);
	if (! pict) Melder_flushError (NULL);
	HLock ((Handle) pict);
	#if carbon
	{
		ScrapRef scrap;
		ClearCurrentScrap ();
		GetCurrentScrap (& scrap);
		PutScrapFlavor (scrap, 'PICT', 0, GetHandleSize ((Handle) pict), (Ptr) *pict);
	}
	#else
		ZeroScrap ();
		PutScrap (GetHandleSize ((Handle) pict), 'PICT', (Ptr) *pict);
	#endif
	HUnlock ((Handle) pict);
	KillPicture (pict);
}
int Picture_writeToMacPictFile (Picture me, MelderFile file) {
	long i, zero = 0, count;
	FSSpec fspec;
	short int path;
	PicHandle pict = copyToPict (me);	
	MelderFile_delete (file);   /* Overwrite existing file with same name. */
	Melder_fileToMac (file, & fspec);
	if (FSpCreate (& fspec, 'PpgB', 'PICT', smSystemScript) != noErr)
		return Melder_error ("Picture_writeToMacPictFile: "
			"cannot open file %.200s", MelderFile_messageName (file));
	FSpOpenDF (& fspec, fsWrPerm, & path);
	count = 4;
	for (i = 1; i <= 128; i ++)
		FSWrite (path, & count, & zero);
	HLock ((Handle) pict);
	count = GetHandleSize ((Handle) pict);
	FSWrite (path, & count, *pict);
	HUnlock ((Handle) pict);
	FSClose (path);
	KillPicture (pict);
	return 1;
}
#endif

#ifdef _WIN32
/* Windows pictures.
 */
#define WIN_WIDTH  7.5
#define WIN_HEIGHT  11
static HENHMETAFILE copyToMetafile (Picture me) {
	HENHMETAFILE metafile;
	RECT rect;
	HDC dc;
	PRINTDLG defaultPrinter;
	int resolution;
	Graphics pictGraphics;
	memset (& defaultPrinter, 0, sizeof (PRINTDLG));
	defaultPrinter. lStructSize = sizeof (PRINTDLG);
	defaultPrinter. Flags = PD_RETURNDEFAULT | PD_RETURNDC;
	PrintDlg (& defaultPrinter);
	SetRect (& rect, my selx1 * 2540, (12 - my sely2) * 2540, my selx2 * 2540, (12 - my sely1) * 2540);
	dc = CreateEnhMetaFile (defaultPrinter. hDC, NULL, & rect, "Praat\0");
	if (! dc) return Melder_errorp ("Cannot create metafile.");
	resolution = GetDeviceCaps (dc, LOGPIXELSX);   /* Virtual PC: 360 */
	if (Melder_debug == 6) {
		DEVMODE *devMode = * (DEVMODE **) defaultPrinter. hDevMode;
		MelderInfo_open ();
		MelderInfo_writeLine1 ("DEVICE CAPS:");
		MelderInfo_writeLine4 ("aspect x ", Melder_integer (GetDeviceCaps (dc, ASPECTX)),
			" y ", Melder_integer (GetDeviceCaps (dc, ASPECTY)));
		MelderInfo_writeLine4 ("res(pixels) hor ", Melder_integer (GetDeviceCaps (dc, HORZRES)),
			" vert ", Melder_integer (GetDeviceCaps (dc, VERTRES)));
		MelderInfo_writeLine4 ("size(mm) hor ", Melder_integer (GetDeviceCaps (dc, HORZSIZE)),
			" vert ", Melder_integer (GetDeviceCaps (dc, VERTSIZE)));
		MelderInfo_writeLine4 ("pixels/inch hor ", Melder_integer (GetDeviceCaps (dc, LOGPIXELSX)),
			" vert ", Melder_integer (GetDeviceCaps (dc, LOGPIXELSY)));
		MelderInfo_writeLine4 ("physicalOffset(pixels) hor ", Melder_integer (GetDeviceCaps (dc, PHYSICALOFFSETX)),
			" vert ", Melder_integer (GetDeviceCaps (dc, PHYSICALOFFSETY)));
		MelderInfo_writeLine1 ("PRINTER:");
		MelderInfo_writeLine2 ("dmFields ", Melder_integer (devMode -> dmFields));
		if (devMode -> dmFields & DM_YRESOLUTION)
			MelderInfo_writeLine2 ("y resolution ", Melder_integer (devMode -> dmYResolution));
		if (devMode -> dmFields & DM_PRINTQUALITY)
			MelderInfo_writeLine2 ("print quality ", Melder_integer (devMode -> dmPrintQuality));
		if (devMode -> dmFields & DM_PAPERWIDTH)
			MelderInfo_writeLine2 ("paper width ", Melder_integer (devMode -> dmPaperWidth));
		if (devMode -> dmFields & DM_PAPERLENGTH)
			MelderInfo_writeLine2 ("paper length ", Melder_integer (devMode -> dmPaperLength));
		if (devMode -> dmFields & DM_PAPERSIZE)
			MelderInfo_writeLine2 ("paper size ", Melder_integer (devMode -> dmPaperSize));
		if (devMode -> dmFields & DM_ORIENTATION)
			MelderInfo_writeLine2 ("orientation ", Melder_integer (devMode -> dmOrientation));
		MelderInfo_close ();
	}
	pictGraphics = Graphics_create_screen ((void *) dc, 0, resolution);
	Graphics_setWsViewport (pictGraphics, 0, WIN_WIDTH * resolution, 0, WIN_HEIGHT * resolution);
	Graphics_setWsWindow (pictGraphics, 0.0, WIN_WIDTH, 12.0 - WIN_HEIGHT, 12.0);
	Graphics_play (my graphics, pictGraphics);
	metafile = CloseEnhMetaFile (dc);
	forget (pictGraphics);
	return metafile;
}
void Picture_copyToClipboard (Picture me) {
	HENHMETAFILE metafile = copyToMetafile (me);
	if (! metafile) Melder_flushError (NULL);
	OpenClipboard (NULL);
	EmptyClipboard ();
	SetClipboardData (CF_ENHMETAFILE, metafile);
	CloseClipboard ();
	/*
	 * We should NOT call DeleteEnhMetaFile,
	 * because the clipboard becomes the owner of this global memory object.
	 */
}
int Picture_writeToWindowsMetafile (Picture me, MelderFile file) {
	HENHMETAFILE metafile = copyToMetafile (me);
	if (! metafile) Melder_flushError (NULL);
	MelderFile_delete (file);   /* Overwrite existing file with same name. */
	DeleteEnhMetaFile (CopyEnhMetaFile (metafile, file -> path));
	DeleteEnhMetaFile (metafile);
	return 1;
}
#endif

int Picture_writeToEpsFile (Picture me, MelderFile file, int includeFonts) {
	Graphics ps;
	MelderFile_delete (file);   /* To kill resources as well (fopen only kills data fork). */
	/* BUG: no message if file cannot be deleted (e.g. because still open by Microsoft Word 2001 after reading). */

	ps = Graphics_create_epsfile (file, 600, thePrinter. spots,
		my selx1, my selx2, my sely1, my sely2, includeFonts);
	if (! ps)
		return Melder_error ("Picture_writeToPostScriptFile: "
			"error opening file %.200s", MelderFile_messageName (file));
	Graphics_play (my graphics, ps);
	forget (ps);
	MelderFile_setMacTypeAndCreator (file, 'EPSF', 'vgrd');

	#ifdef macintosh
	/*
		Create an 8-bit screen preview.
	*/
	{
		int path;
		FSSpec fspec;
		PicHandle pict = copyToPict_screenImage (me);
		/*
		 * Copy the PICT to the file.
		 */
		Melder_fileToMac (file, & fspec);
		FSpCreateResFile (& fspec, 'vgrd', 'EPSF', smSystemScript);   /* Make a resource fork... */
		path = FSpOpenResFile (& fspec, fsWrPerm);   /* ...and open it. */

		/* Write the data to the file as a 'PICT' resource. */
		/* The Id of this resource is 256 (PS. Lang. Ref. Man., 2nd ed., p. 728. */
		/* The name of the resource will be equal to the name of the file. */

		AddResource ((Handle) pict, 'PICT', 256, fspec. name);   /* Resource manager's. */
		if (ResError () != noErr) {
			/*
			 * Disk full, or PICT resource larger than 32 kilobytes?
			 */
			CloseResFile (path);
			return Melder_error ("Picture_writeToEpsFile: not enough disk space.");
		}
		SetResAttrs ((Handle) pict, resPurgeable + resChanged);
		CloseResFile (path);
	}
	#endif

	return 1;
}

static void print (I, Graphics printer) {
	iam (Picture);
	Graphics_play (my graphics, printer);
}

void Picture_print (Picture me) {
	Printer_print (print, me); iferror Melder_flushError ("Picture not printed.");
}

void Picture_setSelection
	(Picture me, double x1NDC, double x2NDC, double y1NDC, double y2NDC, Boolean notify)
{
	if (my drawingArea) drawSelection (me, 0);   /* Unselect. */
	my selx1 = x1NDC;
	my selx2 = x2NDC;
	my sely1 = y1NDC;
	my sely2 = y2NDC;
	if (my drawingArea) drawSelection (me, 1);   /* Select. */

	if (notify && my selectionChangedCallback)
		my selectionChangedCallback (me, my selectionChangedClosure,
			my selx1, my selx2, my sely1, my sely2);
}

void Picture_background (Picture me) { my backgrounding = TRUE; }
void Picture_foreground (Picture me) { my backgrounding = FALSE; }

/* End of file Picture.c */
