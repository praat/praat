/* Graphics_colour.c
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
 * pb 2002/05/28 GPL
 * pb 2004/09/09 Xwin: highlight2
 */

#include "GraphicsP.h"

#if mac
	typedef struct RGBColor RGBColour;
#else
	typedef struct { unsigned short red, green, blue; } RGBColour;
#endif

static RGBColour theColours [] = {
		{ 0x0000, 0x0000, 0x0000 },   /* black */
		{ 0xFFFF, 0xFFFF, 0xFFFF },   /* white */
		{ 0xDD6B, 0x08C2, 0x06A2 },   /* red */
		{ 0x0000, 0x8000, 0x11B0 },   /* green */
		{ 0x0000, 0x0000, 0xD400 },   /* blue */
		{ 0x0241, 0xAB54, 0xEAFF },   /* cyan */
		{ 0xF2D7, 0x0856, 0x84EC },   /* magenta */
		{ 0xFC00, 0xF37D, 0x052F },   /* yellow */
		{ 0x7000, 0x0000, 0x0000 },   /* maroon */
		{ 0x0000, 0xFFFF, 0x0000 },   /* lime */
		{ 0x0000, 0x0000, 0x7000 },   /* navy */
		{ 0x0000, 0x6000, 0x7000 },   /* teal */
		{ 0x8000, 0x0000, 0x4000 },   /* purple */
		{ 0x8000, 0x8000, 0x0000 },   /* olive */
		{ 0xC000, 0xC000, 0xC000 },   /* silver */
		{ 0x8000, 0x8000, 0x8000 } };   /* grey */

#if mac
static RGBColour theMacColours [] = {
		{ 0x0000, 0x0000, 0x0000 },   /* black */
		{ 0xFFFF, 0xFFFF, 0xFFFF },   /* white */
		{ 0xFF00, 0x0000, 0x0000 },   /* red */
		{ 0x0000, 0x8000, 0x0000 },   /* green */
		{ 0x0000, 0x0000, 0xFFFF },   /* blue */
		{ 0x0000, 0xFFFF, 0xFFFF },   /* cyan */
		{ 0xFFFF, 0x0000, 0xFFFF },   /* magenta */
		{ 0xFFFF, 0xFFFF, 0x0000 },   /* yellow */
		{ 0x8000, 0x0000, 0x0000 },   /* maroon */
		{ 0x0000, 0xFFFF, 0x0000 },   /* lime */
		{ 0x0000, 0x0000, 0x8000 },   /* navy */
		{ 0x0000, 0x8000, 0x8000 },   /* teal */
		{ 0x8000, 0x0000, 0x8000 },   /* purple */
		{ 0x8000, 0x8000, 0x0000 },   /* olive */
		{ 0xC000, 0xC000, 0xC000 },   /* silver */
		{ 0x8000, 0x8000, 0x8000 } };   /* grey */
#endif

#if xwin
	unsigned long black, white, red, green, blue, cyan, magenta, yellow,
		maroon, lime, navy, teal, purple, olive, pink, grey [101];
#elif mac
	#include "macport_on.h"
	#include <LowMem.h>
	static RGBColor theBlackColour = { 0, 0, 0 };
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void _Graphics_setColour (I, int colour) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XSetForeground (my display, my gc,
				colour == Graphics_WHITE ? white :
				colour == Graphics_RED ? red :
				colour == Graphics_GREEN ? green :
				colour == Graphics_BLUE ? blue :
				colour == Graphics_CYAN ? cyan :
				colour == Graphics_MAGENTA ? magenta :
				colour == Graphics_YELLOW ? yellow :
				colour == Graphics_MAROON ? maroon :
				colour == Graphics_LIME ? lime :
				colour == Graphics_NAVY ? navy :
				colour == Graphics_TEAL ? teal :
				colour == Graphics_PURPLE ? purple :
				colour == Graphics_OLIVE ? olive :
				colour == Graphics_SILVER ? grey [75] :
				colour == Graphics_GREY ? grey [50] :
				black);
		#elif win
			switch (colour) {
				case Graphics_WHITE: my foregroundColour = RGB (255, 255, 255); break;
				case Graphics_RED: my foregroundColour = RGB (255, 0, 0); break;
				case Graphics_GREEN: my foregroundColour = RGB (0, 128, 0); break;
				case Graphics_BLUE: my foregroundColour = RGB (0, 0, 255); break;
				case Graphics_CYAN: my foregroundColour = RGB (0, 255, 255); break;
				case Graphics_MAGENTA: my foregroundColour = RGB (255, 0, 255); break;
				case Graphics_YELLOW: my foregroundColour = RGB (255, 255, 0); break;
				case Graphics_MAROON: my foregroundColour = RGB (128, 0, 0); break;
				case Graphics_LIME: my foregroundColour = RGB (0, 255, 0); break;
				case Graphics_NAVY: my foregroundColour = RGB (0, 0, 128); break;
				case Graphics_TEAL: my foregroundColour = RGB (0, 128, 128); break;
				case Graphics_PURPLE: my foregroundColour = RGB (128, 0, 128); break;
				case Graphics_OLIVE: my foregroundColour = RGB (128, 128, 0); break;
				case Graphics_SILVER: my foregroundColour = RGB (192, 192, 192); break;
				case Graphics_GREY: my foregroundColour = RGB (128, 128, 128); break;
				default: my foregroundColour = RGB (0, 0, 0); break;
			}
			SelectPen (my dc, GetStockPen (BLACK_PEN));
			DeleteObject (my pen);
			my pen = CreatePen (PS_SOLID, 0, my foregroundColour);
			SelectBrush (my dc, GetStockBrush (NULL_BRUSH));
			DeleteObject (my brush);
			my brush = CreateSolidBrush (my foregroundColour);
		#elif mac
			my macColour = colour >= 0 && colour <= 15 ? theMacColours [colour] : theBlackColour;
		#endif
	} else if (my postScript) {
		iam (GraphicsPostscript);
		if (my languageLevel == 1)
			my printf (my file, "%f setgray\n",
				colour == Graphics_BLACK ? 0.0 :
				colour == Graphics_WHITE ? 1.0 :
				colour == Graphics_RED || colour == Graphics_GREEN || colour == Graphics_BLUE ? 0.5 : 0.8);
		else
			my printf (my file, "%.6g %.6g %.6g setrgbcolor\n",
				theColours [colour]. red / 65536.0, theColours [colour]. green / 65536.0, theColours [colour]. blue / 65536.0);
	}
}

void Graphics_setColour (I, int colour) {
	iam (Graphics);
	_Graphics_setColour (me, my colour = colour);
	if (my recording) { op (SET_COLOUR, 1); put (colour); }
}

void _Graphics_setGrey (I, double fgrey) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			int igrey = (int) floor (100 * fgrey + 0.5);
			if (igrey < 0) igrey = 0; else if (igrey > 100) igrey = 100;
			XSetForeground (my display, my gc, grey [igrey]);
		#elif win
			int lightness = fgrey <= 0 ? 0 : fgrey >= 1.0 ? 255 : fgrey * 255;
			my foregroundColour = RGB (lightness, lightness, lightness);
			SelectPen (my dc, GetStockPen (BLACK_PEN));
			DeleteObject (my pen);
			my pen = CreatePen (PS_SOLID, 0, my foregroundColour);
			SelectBrush (my dc, GetStockBrush (NULL_BRUSH));
			DeleteObject (my brush);
			my brush = CreateSolidBrush (my foregroundColour);
		#elif mac
			SetPort (my macPort);
			my macColour = theBlackColour;
			if (my depth == 1&&0) {   /* Black and white screen or printer. */
				static MacintoshPattern pat10 = { 0x22, 0, 0x88, 0, 0x22, 0, 0x88, 0 };
				MacintoshPattern pattern, *macPattern =
					fgrey <= 0.0 ? GetQDGlobalsBlack (& pattern) :
					fgrey >= 1.0 ? GetQDGlobalsWhite (& pattern) :
					fgrey < 0.25 ? GetQDGlobalsDarkGray (& pattern) :
					fgrey < 0.50 ? GetQDGlobalsGray (& pattern) :
					fgrey < 0.75 ? GetQDGlobalsLightGray (& pattern) :
						& pat10;
					PenPat (macPattern);   /* For drawing lines and text. */
					my macPattern = *macPattern;   /* For filling. */
			} else {   /* Colour or grey screen. */
				RGBColor rgb;
				if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
				rgb. red = rgb. green = rgb. blue = (int) (unsigned int) (fgrey * 65535.0);
				RGBForeColor (& rgb);
			}
		#endif
	} else if (my postScript) {
		iam (GraphicsPostscript);
		if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
		my printf (my file, "%.6g setgray\n", fgrey);
	}
}

void Graphics_setGrey (I, double grey) {
	iam (Graphics);
	_Graphics_setGrey (me, grey);
	if (my recording) { op (SET_GREY, 1); put (grey); }
}

static void highlight (I, short x1DC, short x2DC, short y1DC, short y2DC) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			XSetForeground (my display, my gc, pink ^ white);
			XSetFunction (my display, my gc, GXxor);
			XFillRectangle (my display, my window, my gc, x1DC, y2DC, width, height);
			XSetForeground (my display, my gc, black);
			XSetFunction (my display, my gc, GXcopy);
		#elif win
			static HBRUSH highlightBrush;
			RECT rect;
			rect. left = x1DC, rect. right = x2DC, rect. top = y2DC, rect. bottom = y1DC;
			if (! highlightBrush)
				highlightBrush = CreateSolidBrush (RGB (255, 210, 210));
			SelectPen (my dc, GetStockPen (NULL_PEN));
			SelectBrush (my dc, highlightBrush);
			SetROP2 (my dc, R2_NOTXORPEN);
			Rectangle (my dc, x1DC, y2DC, x2DC + 1, y1DC + 1);
			SetROP2 (my dc, R2_COPYPEN);
			SelectPen (my dc, GetStockPen (BLACK_PEN));
			SelectBrush (my dc, GetStockBrush (NULL_BRUSH));   /* Superfluous? */
		#elif mac
			Rect rect;
			if (my drawingArea) GuiMacDrawingArea_clipOn (my drawingArea);
			SetRect (& rect, x1DC, y2DC, x2DC, y1DC);
			SetPort (my macPort);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);   /* see IM V-61 */
			InvertRect (& rect);
			if (my drawingArea) GuiMac_clipOff ();
		#endif
	}
}

void Graphics_highlight (I, double x1WC, double x2WC, double y1WC, double y2WC) {
	iam (Graphics);
	highlight (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
	if (my recording)
		{ op (HIGHLIGHT, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

void Graphics_unhighlight (I, double x1WC, double x2WC, double y1WC, double y2WC) {
	iam (Graphics);
	highlight (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
	if (my recording)
		{ op (UNHIGHLIGHT, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

static void highlight2 (I, short x1DC, short x2DC, short y1DC, short y2DC,
	short x1DC_inner, short x2DC_inner, short y1DC_inner, short y2DC_inner)
{
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XSetForeground (my display, my gc, pink ^ white);
			XSetFunction (my display, my gc, GXxor);
			XFillRectangle (my display, my window, my gc, x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC);
			XFillRectangle (my display, my window, my gc, x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner);
			XFillRectangle (my display, my window, my gc, x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner);
			XFillRectangle (my display, my window, my gc, x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner);
			XSetForeground (my display, my gc, black);
			XSetFunction (my display, my gc, GXcopy);
		#elif win
			static HBRUSH highlightBrush;
			if (! highlightBrush)
				highlightBrush = CreateSolidBrush (RGB (255, 210, 210));
			SelectPen (my dc, GetStockPen (NULL_PEN));
			SelectBrush (my dc, highlightBrush);
			SetROP2 (my dc, R2_NOTXORPEN);
			Rectangle (my dc, x1DC, y2DC, x2DC + 1, y2DC_inner + 1);
			Rectangle (my dc, x1DC, y2DC_inner, x1DC_inner + 1, y1DC_inner + 1);
			Rectangle (my dc, x2DC_inner, y2DC_inner, x2DC + 1, y1DC_inner + 1);
			Rectangle (my dc, x1DC, y1DC_inner, x2DC + 1, y1DC + 1);
			SetROP2 (my dc, R2_COPYPEN);
			SelectPen (my dc, GetStockPen (BLACK_PEN));
			SelectBrush (my dc, GetStockBrush (NULL_BRUSH));   /* Superfluous? */
		#elif mac
			Rect rect;
			if (my drawingArea) GuiMacDrawingArea_clipOn (my drawingArea);
			SetPort (my macPort);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
			SetRect (& rect, x1DC, y2DC, x2DC, y2DC_inner);
			InvertRect (& rect);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
			SetRect (& rect, x1DC, y2DC_inner, x1DC_inner, y1DC_inner);
			InvertRect (& rect);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
			SetRect (& rect, x2DC_inner, y2DC_inner, x2DC, y1DC_inner);
			InvertRect (& rect);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
			SetRect (& rect, x1DC, y1DC_inner, x2DC, y1DC);
			InvertRect (& rect);
			if (my drawingArea) GuiMac_clipOff ();
		#endif
	}
}

void Graphics_highlight2 (I, double x1WC, double x2WC, double y1WC, double y2WC,
	double x1WC_inner, double x2WC_inner, double y1WC_inner, double y2WC_inner)
{
	iam (Graphics);
	highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner));
	if (my recording)
		{ op (HIGHLIGHT2, 8); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner); }
}

void Graphics_unhighlight2 (I, double x1WC, double x2WC, double y1WC, double y2WC,
	double x1WC_inner, double x2WC_inner, double y1WC_inner, double y2WC_inner)
{
	iam (Graphics);
	highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner));
	if (my recording)
		{ op (UNHIGHLIGHT2, 8); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner); }
}

void Graphics_xorOn (I, int colour) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XSetForeground (my display, my gc, (
				colour == Graphics_WHITE ? white :
				colour == Graphics_RED ? red :
				colour == Graphics_GREEN ? green :
				colour == Graphics_BLUE ? blue :
				colour == Graphics_CYAN ? cyan :
				colour == Graphics_MAGENTA ? magenta :
				colour == Graphics_YELLOW ? yellow : black) ^ white);
			XSetFunction (my display, my gc, GXxor);
		#elif win
			SetROP2 (my dc, R2_XORPEN);
			_Graphics_setColour (me,
				colour == Graphics_GREEN ? Graphics_MAGENTA : colour == Graphics_MAGENTA ? Graphics_GREEN :
				colour == Graphics_WHITE ? Graphics_BLACK : colour == Graphics_BLACK ? Graphics_WHITE :
				colour == Graphics_RED ? Graphics_CYAN : colour == Graphics_CYAN ? Graphics_RED :
				colour == Graphics_BLUE ? Graphics_YELLOW : Graphics_BLUE);
			my duringXor = TRUE;
		#elif mac
			SetPort (my macPort);
			PenMode (patXor);
			TextMode (srcXor);
		#endif
	}
	if (my recording) { op (XOR_ON, 1); put (colour); }
}

void Graphics_xorOff (I) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XSetForeground (my display, my gc, black);
			XSetFunction (my display, my gc, GXcopy);
		#elif win
			SetROP2 (my dc, R2_COPYPEN);
			_Graphics_setColour (me, my colour);
			my duringXor = FALSE;
		#elif mac
			SetPort (my macPort);
			PenMode (patCopy);
			TextMode (srcOr);
		#endif
	}
	if (my recording) { op (XOR_OFF, 0); }
}

int Graphics_inqColour (I) { iam (Graphics); return my colour; }

#if xwin
static unsigned long getPixel (GraphicsScreen me, const char *colour) {
	XColor colorcell_def, rgb_db_def;
	XAllocNamedColor (my display, my colourMap, (char *) colour, & colorcell_def, & rgb_db_def);
	return colorcell_def. pixel;
}
void _Graphics_colour_init (I) {
	iam (GraphicsScreen);
	XGCValues values;
	static int inited;
	if (! inited) {
		int igrey;
		black = BlackPixel (my display, my xscreen);
		white = WhitePixel (my display, my xscreen);
		red = getPixel (me, "red");
		green = getPixel (me, "green");
		blue = getPixel (me, "blue");
		cyan = getPixel (me, "cyan");
		magenta = getPixel (me, "magenta");
		yellow = getPixel (me, "yellow");
		maroon = getPixel (me, "maroon");
		lime = getPixel (me, "lime green");
		navy = getPixel (me, "navy");
		teal = getPixel (me, "steel blue");
		purple = getPixel (me, "purple");
		olive = getPixel (me, "olive drab");
		pink = getPixel (me, "pink");
		for (igrey = 0; igrey <= 100; igrey ++) {
			char string [10];
			sprintf (string, "grey%d", igrey);
			grey [igrey] = getPixel (me, string);
		}
		inited = 1;
	}
	XSetWindowBackground (my display, my window, white);
	values. foreground = black;
	values. background = white;
	my text.gc = my gc = XCreateGC (my display, my window, GCForeground | GCBackground, & values);
}
#endif

/* End of file Graphics_colour.c */
