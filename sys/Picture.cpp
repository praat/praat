/* Picture.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brauße
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

#include "melder.h"
#include "Gui.h"
#include "Printer.h"
#include "Picture.h"
#include "site.h"
#ifdef _WIN32
	#include "GraphicsP.h"
#endif

Thing_implement (Picture, Thing, 0);

static void drawMarkers (Picture me)
/*
	The drawing area is a square measuring 12x12 inches.
*/
#define SIDE 12
/*
	The selection grid has a resolution of 1/2 inch.
*/
#define SQUARES 24_integer
/*
	Vertical and horizontal lines every 3 inches.
*/
#define YELLOW_GRID 3
{
	/*
		Fill the entire canvas with GC's background.
	*/
	Graphics_setColour (my selectionGraphics.get(), Melder_WHITE);
	Graphics_fillRectangle (my selectionGraphics.get(), 0, SIDE, 0, SIDE);

	/*
		Draw yellow grid lines for coarse navigation.
	*/
	Graphics_setColour (my selectionGraphics.get(), Melder_YELLOW);
	for (int i = YELLOW_GRID; i < SIDE; i += YELLOW_GRID) {
		Graphics_line (my selectionGraphics.get(), 0, i, SIDE, i);
		Graphics_line (my selectionGraphics.get(), i, 0, i, SIDE);
	}

	/*
		Draw red ticks and numbers for feedback on viewport measurement.
	*/
	Graphics_setColour (my selectionGraphics.get(), Melder_RED);
	for (int i = 1; i < SIDE; i ++) {
		const double x = i;
		Graphics_setTextAlignment (my selectionGraphics.get(), Graphics_CENTRE, Graphics_TOP);
		Graphics_text (my selectionGraphics.get(), x, SIDE, i);
		Graphics_setTextAlignment (my selectionGraphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my selectionGraphics.get(), x, 0, i);
	}
	for (int i = 1; i < SQUARES ; i ++) {   // vertical ticks
		const double x = 0.5 * i;
		Graphics_line (my selectionGraphics.get(), x, SIDE - 0.04, x, SIDE);
		Graphics_line (my selectionGraphics.get(), x, 0, x, 0.04);
	}
	for (int i = 1; i < SIDE; i ++) {
		const double y = SIDE - i;
		Graphics_setTextAlignment (my selectionGraphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (my selectionGraphics.get(), 0.04, y, i);
		Graphics_setTextAlignment (my selectionGraphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (my selectionGraphics.get(), SIDE - 0.03, y, i);
	}
	for (int i = 1; i < SQUARES; i ++) {   // horizontal ticks
		const double y = SIDE - 0.5 * i;
		Graphics_line (my selectionGraphics.get(), SIDE - 0.04, y, SIDE, y);
		Graphics_line (my selectionGraphics.get(), 0, y, 0.04, y);
	}
	Graphics_setColour (my selectionGraphics.get(), Melder_BLACK);
}

static void drawSelection (Picture me) {
	const double dy = Melder_clippedRight (2.8 * Graphics_inqFontSize (my graphics.get()) / 72.0,
			0.4 * (my sely2 - my sely1));
	const double dx = Melder_clippedRight (1.5 * dy,
			0.4 * (my selx2 - my selx1));
	Graphics_highlight2 (my selectionGraphics.get(),
		my selx1, my selx2, my sely1, my sely2,
		my selx1 + dx, my selx2 - dx, my sely1 + dy, my sely2 - dy
	);
}

static void gui_drawingarea_cb_expose (Picture me, GuiDrawingArea_ExposeEvent event) {
	Melder_assert (event -> widget);
	drawMarkers (me);
	Graphics_play (my graphics.get(), my graphics.get());
	drawSelection (me);
}

static void gui_drawingarea_cb_mouse (Picture me, GuiDrawingArea_MouseEvent event) {
	/*
		Dragging the mouse selects an integer number of "blocks".
	*/
	struct Block {
		integer ix, iy;
	};
	static Block anchorBlock, previousBlock;
	double xWC, yWC;
	Graphics_DCtoWC (my selectionGraphics.get(), event -> x, event -> y, & xWC, & yWC);
	const Block currentBlock {
		Melder_clipped (1_integer, 1 + (integer) floor (xWC * SQUARES / SIDE), SQUARES),
		Melder_clipped (1_integer, SQUARES - (integer) floor (yWC * SQUARES / SIDE), SQUARES)
	};
	bool didBlockChange = false;   // optimization: don't redraw if we stay in the same block
	if (event -> isClick()) {
		constexpr int INVALID_BLOCK_NUMBER = 0;
		previousBlock = { INVALID_BLOCK_NUMBER, INVALID_BLOCK_NUMBER };
		if (event -> shiftKeyPressed) {
			const integer ix1 = Melder_clipped (1_integer, 1 + (integer) floor (my selx1 * SQUARES / SIDE), SQUARES);   // BUG not compatible with mouseSelectsInnerViewport
			const integer ix2 = Melder_clipped (1_integer, (integer) floor (my selx2 * SQUARES / SIDE), SQUARES);
			const integer iy1 = Melder_clipped (1_integer, SQUARES + 1 - (integer) floor (my sely2 * SQUARES / SIDE), SQUARES);
			const integer iy2 = Melder_clipped (1_integer, SQUARES - (integer) floor (my sely1 * SQUARES / SIDE), SQUARES);
			anchorBlock = { currentBlock. ix < (ix1 + ix2) / 2 ? ix2 : ix1, currentBlock. iy < (iy1 + iy2) / 2 ? iy2 : iy1 };
		} else {
			anchorBlock = currentBlock;
		}
		didBlockChange = true;
	} else if (event -> isDrag() || event -> isDrop()) {
		didBlockChange = ( currentBlock. ix != previousBlock. ix || currentBlock. iy != previousBlock. iy );
	}
	if (didBlockChange) {
		previousBlock = currentBlock;
		const auto ix12 = std::minmax (currentBlock. ix, anchorBlock. ix);
		const integer ix1 = ix12. first, ix2 = ix12. second;
		const auto iy12 = std::minmax (currentBlock. iy, anchorBlock. iy);
		const integer iy1 = iy12. first, iy2 = iy12. second;
		double xmargin = 0.0, ymargin = 0.0;
		if (my mouseSelectsInnerViewport) {
			const double fontSize = Graphics_inqFontSize (my graphics.get());
			xmargin = std::min (fontSize * 4.2 / 72.0, double (ix2 - ix1 + 1));
			ymargin = std::min (fontSize * 2.8 / 72.0, double (iy2 - iy1 + 1));
		}
		Picture_setSelection (me,
			0.5 * (ix1 - 1) - xmargin, 0.5 * ix2 + xmargin,
			0.5 * (SQUARES - iy2) - ymargin, 0.5 * (SQUARES + 1 - iy1) + ymargin
		);
		Graphics_updateWs (my selectionGraphics.get());
	}
	if (event -> isDrop() && my selectionChangedCallback)
		my selectionChangedCallback (me, my selectionChangedClosure,
			my selx1, my selx2, my sely1, my sely2);
}

autoPicture Picture_create (GuiDrawingArea drawingArea, bool sensitive) {
	try {
		autoPicture me = Thing_new (Picture);
		my drawingArea = drawingArea;
		/*
			The initial viewport is a rectangle 6 inches wide and 4 inches high.
		*/
		my selx1 = 0.0;
		my selx2 = 6.0;
		my sely1 = 8.0;
		my sely2 = 12.0;
		my sensitive = sensitive && drawingArea;
		/*
			Create a Graphics to directly draw in.
		*/
		if (drawingArea) {
			// the drawing area must have been realized; see manual at XtWindow
			my graphics = Graphics_create_xmdrawingarea (my drawingArea);
			GuiDrawingArea_setExposeCallback (my drawingArea, gui_drawingarea_cb_expose, me.get());
		} else {
			/*
				Create a dummy Graphics.
				It does have to be a GraphicsScreen, because it has to support Graphics_textWidth().
			*/
			const integer dummyResolution = 600;
			my graphics = Graphics_create_screen (nullptr, nullptr, dummyResolution);
			Graphics_setWsViewport (my graphics.get(), 0.0, 12 * dummyResolution, 0.0, 12 * dummyResolution);
		}
		Graphics_setWsWindow (my graphics.get(), 0.0, 12.0, 0.0, 12.0);
		Graphics_setViewport (my graphics.get(), my selx1, my selx2, my sely1, my sely2);
		if (my sensitive) {
			my selectionGraphics = Graphics_create_xmdrawingarea (my drawingArea);
			Graphics_setWindow (my selectionGraphics.get(), 0.0, 12.0, 0.0, 12.0);
			GuiDrawingArea_setMouseCallback (my drawingArea, gui_drawingarea_cb_mouse, me.get());
		}
		Graphics_startRecording (my graphics.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Picture not created.");
	}
}

void Picture_setSelectionChangedCallback (Picture me,
	void (*selectionChangedCallback) (Picture, void *, double, double, double, double),
	void *selectionChangedClosure)
{
	my selectionChangedCallback = selectionChangedCallback;
	my selectionChangedClosure = selectionChangedClosure;
}

void Picture_setMouseSelectsInnerViewport (Picture me, int mouseSelectsInnerViewport) {
	my mouseSelectsInnerViewport = mouseSelectsInnerViewport;
}

Graphics Picture_peekGraphics (Picture me) { return my graphics.get(); }

void Picture_erase (Picture me) {
	Graphics_clearRecording (my graphics.get());
	Graphics_updateWs (my graphics.get());
}

void Picture_writeToPraatPictureFile (Picture me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "wb");
		if (fprintf (f, "PraatPictureFile") < 0)
			Melder_throw (U"Write error.");
		Graphics_writeRecordings (my graphics.get(), f);
		f.close (file);
	} catch (MelderError) {
		Melder_throw (U"Cannot write Praat picture file ", file, U".");
	}
}

void Picture_readFromPraatPictureFile (Picture me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		char line [200];
		integer n = uinteger_to_integer (fread (line, 1, 199, f));
		line [n] = '\0';
		const char *tag = "PraatPictureFile";
		char *end = strstr (line, tag);
		if (! end)
			Melder_throw (U"This is not a Praat picture file.");
		*end = '\0';
		rewind (f);
		fread (line, 1, integer_to_uinteger (end - line + Melder8_length (tag)), f);
		Graphics_readRecordings (my graphics.get(), f);
		Graphics_updateWs (my graphics.get());
		f.close (file);
	} catch (MelderError) {
		Melder_throw (U"Praat picture not read from file ", file);
	}
}

#ifdef macintosh
static size_t appendBytes (void *info, const void *buffer, size_t count) {
    CFDataAppendBytes ((CFMutableDataRef) info, (const UInt8 *) buffer, uinteger_to_integer (count));
    return count;
}
void Picture_copyToClipboard (Picture me) {
	/*
		Find the clipboard and clear it.
	*/
	PasteboardRef clipboard = nullptr;
	PasteboardCreate (kPasteboardClipboard, & clipboard);
	PasteboardClear (clipboard);
	/*
		Add a PDF flavour to the clipboard.
	*/
	static CGDataConsumerCallbacks callbacks = { appendBytes, nullptr };
	CFDataRef data = CFDataCreateMutable (kCFAllocatorDefault, 0);
	CGDataConsumerRef consumer = CGDataConsumerCreate ((void *) data, & callbacks);
	int resolution = 600;
	CGRect rect = CGRectMake (0, 0, (my selx2 - my selx1) * resolution, (my sely1 - my sely2) * resolution);
	CGContextRef context = CGPDFContextCreate (consumer, & rect, nullptr);
	//my selx1 * RES, (12 - my sely2) * RES, my selx2 * RES, (12 - my sely1) * RES)
	{// scope
		autoGraphics pdfGraphics = Graphics_create_pdf (context, resolution, my selx1, my selx2, my sely1, my sely2);
		Graphics_play (my graphics.get(), pdfGraphics.get());
	}
	PasteboardPutItemFlavor (clipboard, (PasteboardItemID) 1, kUTTypePDF, data, kPasteboardFlavorNoFlags);
	CFRelease (data);
	/*
		Forget the clipboard.
	*/
	CFRelease (clipboard);
}
#endif

#ifdef _WIN32
/* Windows pictures.
 */
#define WIN_WIDTH  7.5
#define WIN_HEIGHT  11
static HENHMETAFILE copyToMetafile (Picture me) {
	PRINTDLG defaultPrinter;
	memset (& defaultPrinter, 0, sizeof (PRINTDLG));
	defaultPrinter. lStructSize = sizeof (PRINTDLG);
	defaultPrinter. Flags = PD_RETURNDEFAULT | PD_RETURNDC;
	PrintDlg (& defaultPrinter);
	RECT rect;
	SetRect (& rect, my selx1 * 2540, (12 - my sely2) * 2540, my selx2 * 2540, (12 - my sely1) * 2540);
	const HDC dc = CreateEnhMetaFile (defaultPrinter. hDC, nullptr, & rect, L"Praat\0");
	if (! dc)
		Melder_throw (U"Cannot create Windows metafile.");
	const int resolution = GetDeviceCaps (dc, LOGPIXELSX);   // Virtual PC: 360; Parallels Desktop: 600
	//Melder_fatal (U"resolution ", resolution);
	if (Melder_debug == 6) {
		DEVMODE *devMode = * (DEVMODE **) defaultPrinter. hDevMode;
		MelderInfo_open ();
		MelderInfo_writeLine (U"DEVICE CAPS:");
		MelderInfo_writeLine (U"aspect x ", GetDeviceCaps (dc, ASPECTX),
			U" y ", GetDeviceCaps (dc, ASPECTY));
		MelderInfo_writeLine (U"res(pixels) hor ", GetDeviceCaps (dc, HORZRES),
			U" vert ", GetDeviceCaps (dc, VERTRES));
		MelderInfo_writeLine (U"size(mm) hor ", GetDeviceCaps (dc, HORZSIZE),
			U" vert ", GetDeviceCaps (dc, VERTSIZE));
		MelderInfo_writeLine (U"pixels/inch hor ", GetDeviceCaps (dc, LOGPIXELSX),
			U" vert ", GetDeviceCaps (dc, LOGPIXELSY));
		MelderInfo_writeLine (U"physicalOffset(pixels) hor ", GetDeviceCaps (dc, PHYSICALOFFSETX),
			U" vert ", GetDeviceCaps (dc, PHYSICALOFFSETY));
		MelderInfo_writeLine (U"PRINTER:");
		MelderInfo_writeLine (U"dmFields ", devMode -> dmFields);
		if (devMode -> dmFields & DM_YRESOLUTION)
			MelderInfo_writeLine (U"y resolution ", devMode -> dmYResolution);
		if (devMode -> dmFields & DM_PRINTQUALITY)
			MelderInfo_writeLine (U"print quality ", devMode -> dmPrintQuality);
		if (devMode -> dmFields & DM_PAPERWIDTH)
			MelderInfo_writeLine (U"paper width ", devMode -> dmPaperWidth);
		if (devMode -> dmFields & DM_PAPERLENGTH)
			MelderInfo_writeLine (U"paper length ", devMode -> dmPaperLength);
		if (devMode -> dmFields & DM_PAPERSIZE)
			MelderInfo_writeLine (U"paper size ", devMode -> dmPaperSize);
		if (devMode -> dmFields & DM_ORIENTATION)
			MelderInfo_writeLine (U"orientation ", devMode -> dmOrientation);
		MelderInfo_close ();
	}
	autoGraphics pictGraphics = Graphics_create_screen ((void *) dc, nullptr, resolution);
	Graphics_setWsViewport (pictGraphics.get(), 0, WIN_WIDTH * resolution, 0, WIN_HEIGHT * resolution);
	Graphics_setWsWindow (pictGraphics.get(), 0.0, WIN_WIDTH, 12.0 - WIN_HEIGHT, 12.0);
	Graphics_play (my graphics.get(), pictGraphics.get());
	HENHMETAFILE metafile = CloseEnhMetaFile (dc);
	return metafile;
}
void Picture_copyToClipboard (Picture me) {
	try {
		HENHMETAFILE metafile = copyToMetafile (me);
		OpenClipboard (((GraphicsScreen) my graphics.get()) -> d_winWindow);
		EmptyClipboard ();
		SetClipboardData (CF_ENHMETAFILE, metafile);
		CloseClipboard ();
		/*
			We should NOT call DeleteEnhMetaFile,
			because the clipboard becomes the owner of this global memory object.
		*/
	} catch (MelderError) {
		Melder_throw (U"Picture not copied to clipboard.");
	}
}
void Picture_writeToWindowsMetafile (Picture me, MelderFile file) {
	try {
		HENHMETAFILE metafile = copyToMetafile (me);
		MelderFile_delete (file);   // overwrite any existing file with the same name
		DeleteEnhMetaFile (CopyEnhMetaFile (metafile, Melder_peek32toW_fileSystem (file -> path)));
		DeleteEnhMetaFile (metafile);
	} catch (MelderError) {
		Melder_throw (U"Picture not written to Windows metafile ", file);
	}
}
#endif

void Picture_writeToEpsFile (Picture me, MelderFile file, bool includeFonts, bool useSilipaPS) {
	try {
		MelderFile_delete (file);   // to kill resources as well (fopen only kills data fork)
		// BUG: no message if file cannot be deleted (e.g. because still open by Microsoft Word 2001 after reading)

		{// scope
			autoGraphics postscriptGraphics = Graphics_create_epsfile (file, 600, thePrinter. spots,
					my selx1, my selx2, my sely1, my sely2, includeFonts, useSilipaPS);
			Graphics_play (my graphics.get(), postscriptGraphics.get());
		}
	} catch (MelderError) {
		Melder_throw (U"Picture not written to EPS file ", file);
	}
}

void Picture_writeToPdfFile (Picture me, MelderFile file) {
	try {
		autoGraphics pdfGraphics = Graphics_create_pdffile (file, 300, my selx1, my selx2, my sely1, my sely2);
		Graphics_play (my graphics.get(), pdfGraphics.get());
	} catch (MelderError) {
		Melder_throw (U"Picture not written to PDF file ", file, U".");
	}
}

void Picture_writeToPngFile_300 (Picture me, MelderFile file) {
	try {
		autoGraphics pngGraphics = Graphics_create_pngfile (file, 300, my selx1, my selx2, my sely1, my sely2);
		Graphics_play (my graphics.get(), pngGraphics.get());
	} catch (MelderError) {
		Melder_throw (U"Picture not written to PNG file ", file, U".");
	}
}

void Picture_writeToPngFile_600 (Picture me, MelderFile file) {
	try {
		autoGraphics pngGraphics = Graphics_create_pngfile (file, 600, my selx1, my selx2, my sely1, my sely2);
		Graphics_play (my graphics.get(), pngGraphics.get());
	} catch (MelderError) {
		Melder_throw (U"Picture not written to PNG file ", file, U".");
	}
}

static void print (void *void_me, Graphics printer) {
	iam (Picture);
	Graphics_play (my graphics.get(), printer);
}

void Picture_print (Picture me) {
	try {
		Printer_print (print, me);
	} catch (MelderError) {
		Melder_flushError (U"Picture not printed.");
	}
}

void Picture_setSelection
	(Picture me, double x1NDC, double x2NDC, double y1NDC, double y2NDC)
{
	if (my drawingArea)
		Melder_assert (my drawingArea -> d_widget);
	my selx1 = x1NDC;
	my selx2 = x2NDC;
	my sely1 = y1NDC;
	my sely2 = y2NDC;
}

/* End of file Picture.cpp */
