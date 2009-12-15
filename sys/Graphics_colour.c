/* Graphics_colour.c
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * sdk 2008/03/24 cairo
 * pb 2009/05/09 pink
 * pb 2009/07/09 RGB colours
 * pb 2009/12/10 colours identical on all platforms
 * pb 2009/12/14 Graphics_standardColourToRGBColour
 */

#include "GraphicsP.h"

static struct { unsigned short red, green, blue; } theColours [] = {
		{ 0x0000, 0x0000, 0x0000 },   /* black */
		{ 0xFFFF, 0xFFFF, 0xFFFF },   /* white */
		{ 0xDD6B, 0x08C2, 0x06A2 },   /* red */
		{ 0x0000, 0x8000, 0x11B0 },   /* green */
		{ 0x0000, 0x0000, 0xD400 },   /* blue */
		{ 0x0241, 0xAB54, 0xEAFF },   /* cyan */
		{ 0xF2D7, 0x0856, 0x84EC },   /* magenta */
		{ 0xFC00, 0xF37D, 0x052F },   /* yellow */
		{ 0x8000, 0x0000, 0x0000 },   /* maroon */
		{ 0x0000, 0xFFFF, 0x0000 },   /* lime */
		{ 0x0000, 0x0000, 0x8000 },   /* navy */
		{ 0x0000, 0x8000, 0x8000 },   /* teal */
		{ 0x8000, 0x0000, 0x8000 },   /* purple */
		{ 0x8000, 0x8000, 0x0000 },   /* olive */
		{ 0xFFFF, 0xC000, 0xC000 },   /* pink */
		{ 0xC000, 0xC000, 0xC000 },   /* silver */
		{ 0x8000, 0x8000, 0x8000 } };   /* grey */
static wchar_t *theColourNames [] = { L"black", L"white", L"red", L"green", L"blue", L"cyan", L"magenta", L"yellow",
	L"maroon", L"lime", L"navy", L"teal", L"purple", L"olive", L"silver", L"grey" };

void Graphics_standardColourToRGBColour (int standardColour, double *red, double *green, double *blue) {
	if (standardColour <= 0 || standardColour > Graphics_MAX_COLOUR) {
		*red = *green = *blue = 0.0;
		return;
	}
	*red = theColours [standardColour]. red / 65535.0;
	*green = theColours [standardColour]. green / 65535.0;
	*blue = theColours [standardColour]. blue / 65535.0;
}

Graphics_Colour Graphics_standardColourToRGBColour_struct (int standardColour)  {
	Graphics_Colour colour;
	if (standardColour <= 0 || standardColour > Graphics_MAX_COLOUR) {
		colour. red = colour. green = colour. blue = 0.0;
		return colour;
	}
	colour. red = theColours [standardColour]. red / 65535.0;
	colour. green = theColours [standardColour]. green / 65535.0;
	colour. blue = theColours [standardColour]. blue / 65535.0;
	return colour;
}

wchar_t * Graphics_getStandardColourName (int standardColour) {
	return standardColour < 0 || standardColour > Graphics_MAX_COLOUR ? L"(unknown)" : theColourNames [standardColour];
}

#if xwin
	unsigned long xwinColours [1+Graphics_MAX_COLOUR], xwinGreys [101];
#elif mac
	#include "macport_on.h"
	#include <LowMem.h>
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void _Graphics_setRGBColour (I, double red, double green, double blue) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if gtk
			if (my cr == NULL) return;
			cairo_set_source_rgb (my cr, red, green, blue);
		#elif xwin
			/*
			 * Find the nearest of the 17 colours.
			 */
			double smallestDistance = 1e6;
			int closest = -1;
			for (int colour = 0; colour <= Graphics_MAX_COLOUR; colour ++) {
				double distance =
					fabs (red - theColours [colour]. red / 65535.0) +
					fabs (green - theColours [colour]. green / 65535.0) +
					fabs (blue - theColours [colour]. blue / 65535.0);
				if (distance < smallestDistance) {
					smallestDistance = distance;
					closest = colour;
				}
			}
			XSetForeground (my display, my gc, xwinColours [closest]);
		#elif win
			my foregroundColour = RGB (red * 255, green * 255, blue * 255);
			SelectPen (my dc, GetStockPen (BLACK_PEN));
			DeleteObject (my pen);
			my pen = CreatePen (PS_SOLID, 0, my foregroundColour);
			SelectBrush (my dc, GetStockBrush (NULL_BRUSH));
			DeleteObject (my brush);
			my brush = CreateSolidBrush (my foregroundColour);
		#elif mac
			my macColour. red = red * 65535;
			my macColour. green = green * 65535;
			my macColour. blue = blue * 65535;
			// postpone till drawing
		#endif
	} else if (my postScript) {
		iam (GraphicsPostscript);
		my printf (my file, "%.6g %.6g %.6g setrgbcolor\n", red, green, blue);
	}
}

void Graphics_setRGBColour (I, double red, double green, double blue) {
	iam (Graphics);
	_Graphics_setRGBColour (me, red, green, blue);
	if (my recording) { op (SET_RGB_COLOUR, 3); put (red); put (green); put (blue); }
}

void Graphics_setRGBColour_struct (I, Graphics_Colour colour) {
	iam (Graphics);
	_Graphics_setRGBColour (me, colour. red, colour. green, colour. blue);
	if (my recording) { op (SET_RGB_COLOUR, 3); put (colour. red); put (colour. green); put (colour. blue); }
}

void _Graphics_setGrey (I, double fgrey) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if gtk
			if (my cr == NULL) return;
			int lightness = fgrey <= 0 ? 0 : fgrey >= 1.0 ? 255 : fgrey * 255;
			cairo_set_source_rgb (my cr, lightness, lightness, lightness);
		#elif xwin
			int igrey = (int) floor (100 * fgrey + 0.5);
			if (igrey < 0) igrey = 0; else if (igrey > 100) igrey = 100;
			XSetForeground (my display, my gc, xwinGreys [igrey]);
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
			if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
			my macColour. red = my macColour. green = my macColour. blue = fgrey * 65535;
			if (my useQuartz) {
			} else {   // QuickDraw
				/* Superfluous? */
				SetPort (my macPort);
				//RGBForeColor (& my macColour);
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
	my red = my green = my blue = grey;
	_Graphics_setGrey (me, grey);
	if (my recording) { op (SET_GREY, 1); put (grey); }
}

void Graphics_setStandardColour (I, int standardColour) {
	iam (Graphics);
	if (standardColour > 0 && standardColour <= Graphics_MAX_COLOUR) {
		my red = theColours [standardColour]. red / 65535.0;
		my green = theColours [standardColour]. green / 65535.0;
		my blue = theColours [standardColour]. blue / 65535.0;
	} else {
		my red = my green = my blue = 0.0;
	}
	_Graphics_setRGBColour (me, my red, my green, my blue);
	if (my recording) { op (SET_STANDARD_COLOUR, 1); put (standardColour); }
}

static void highlight (I, short x1DC, short x2DC, short y1DC, short y2DC) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if gtk
			if (my cr == NULL) return;
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			cairo_set_source_rgba (my cr, 1.0, 0.0, 0.0, 0.5);
			cairo_rectangle (my cr, x1DC, y2DC, width, height);
			cairo_fill (my cr);
		#elif xwin
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			XSetForeground (my display, my gc, xwinColours [Graphics_PINK] ^ xwinColours [Graphics_WHITE]);
			XSetFunction (my display, my gc, GXxor);
			XFillRectangle (my display, my window, my gc, x1DC, y2DC, width, height);
			XSetForeground (my display, my gc, xwinColours [Graphics_BLACK]);
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
			if (my drawingArea) GuiMac_clipOn (my drawingArea);
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
	#if motif
		highlight (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
	#endif
	if (my recording)
		{ op (UNHIGHLIGHT, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

static void highlight2 (I, short x1DC, short x2DC, short y1DC, short y2DC,
	short x1DC_inner, short x2DC_inner, short y1DC_inner, short y2DC_inner)
{
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if gtk
			if (my cr == NULL) return;
			int line = x1DC_inner - x1DC, width = (x2DC - x1DC) - line, height = (y1DC - y2DC) - line;
			double half = line / 2;
			if (width <= 0 || height <= 0) return;
			cairo_set_source_rgba (my cr, 1.0, 0.0, 0.0, 0.5);
			//g_debug("RECT %d %d %d %d", x1DC, x2DC, y1DC, y2DC);
			cairo_rectangle (my cr,  (double) (x1DC + half), (double) (y2DC + half), width, height);
			cairo_set_line_width (my cr, (double) line);
			cairo_stroke (my cr);
		#elif xwin
			XSetForeground (my display, my gc, xwinColours [Graphics_PINK] ^ xwinColours [Graphics_WHITE]);
			XSetFunction (my display, my gc, GXxor);
			XFillRectangle (my display, my window, my gc, x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC);
			XFillRectangle (my display, my window, my gc, x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner);
			XFillRectangle (my display, my window, my gc, x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner);
			XFillRectangle (my display, my window, my gc, x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner);
			XSetForeground (my display, my gc, xwinColours [Graphics_BLACK]);
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
			if (my drawingArea) GuiMac_clipOn (my drawingArea);
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
	#if motif
		highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner));
	#endif
	if (my recording)
		{ op (UNHIGHLIGHT2, 8); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner); }
}

#if motif
void Graphics_xorOn (I, int colour) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XSetForeground (my display, my gc, xwinColours [colour] ^ xwinColours [Graphics_WHITE]);
			XSetFunction (my display, my gc, GXxor);
		#elif win
			SetROP2 (my dc, R2_XORPEN);
			if (colour >= 0 && colour <= Graphics_MAX_COLOUR) {
				_Graphics_setRGBColour (me, theColours [colour]. red / 65536.0, theColours [colour]. green / 65536.0, theColours [colour]. blue / 65536.0);
			} else {
				_Graphics_setRGBColour (me, 0.0, 0.0, 0.0);
			}
			my duringXor = TRUE;
		#elif mac
			if (my useQuartz && 0) {
				//CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeDifference);
			} else {
				SetPort (my macPort);
				PenMode (patXor);
				TextMode (srcXor);
			}
			my duringXor = TRUE;
		#endif
	}
	if (my recording) { op (XOR_ON, 1); put (colour); }
}

void Graphics_xorOff (I) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if xwin
			XSetForeground (my display, my gc, xwinColours [Graphics_BLACK]);
			XSetFunction (my display, my gc, GXcopy);
		#elif win
			SetROP2 (my dc, R2_COPYPEN);
			_Graphics_setRGBColour (me, my red, my green, my blue);
			my duringXor = FALSE;
		#elif mac
			if (my useQuartz && 0) {
				//CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeNormal);
			} else {
				SetPort (my macPort);
				PenMode (patCopy);
				TextMode (srcOr);
			}
			my duringXor = FALSE;
		#endif
	}
	if (my recording) { op (XOR_OFF, 0); }
}
#endif

void Graphics_inqRGBColour (I, double *red, double *green, double *blue) {
	iam (Graphics);
	*red = my red;
	*green = my green;
	*blue = my blue;
}

Graphics_Colour Graphics_inqRGBColour_struct (I) {
	iam (Graphics);
	Graphics_Colour colour;
	colour. red = my red;
	colour. green = my green;
	colour. blue = my blue;
	return colour;
}

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
		xwinColours [Graphics_BLACK] = BlackPixel (my display, my xscreen);
		xwinColours [Graphics_WHITE] = WhitePixel (my display, my xscreen);
		xwinColours [Graphics_RED] = getPixel (me, "red");
		xwinColours [Graphics_GREEN] = getPixel (me, "green");
		xwinColours [Graphics_BLUE] = getPixel (me, "blue");
		xwinColours [Graphics_CYAN] = getPixel (me, "cyan");
		xwinColours [Graphics_MAGENTA] = getPixel (me, "magenta");
		xwinColours [Graphics_YELLOW] = getPixel (me, "yellow");
		xwinColours [Graphics_MAROON] = getPixel (me, "maroon");
		xwinColours [Graphics_GREEN] = getPixel (me, "lime green");
		xwinColours [Graphics_NAVY] = getPixel (me, "navy");
		xwinColours [Graphics_TEAL] = getPixel (me, "steel blue");
		xwinColours [Graphics_PURPLE] = getPixel (me, "purple");
		xwinColours [Graphics_OLIVE] = getPixel (me, "olive drab");
		xwinColours [Graphics_PINK] = getPixel (me, "pink");
		for (igrey = 0; igrey <= 100; igrey ++) {
			char string [10];
			sprintf (string, "grey%d", igrey);
			xwinGreys [igrey] = getPixel (me, string);
		}
		xwinColours [Graphics_SILVER] = xwinGreys [75];
		xwinColours [Graphics_GREY] = xwinGreys [50];
		inited = 1;
	}
	XSetWindowBackground (my display, my window, xwinColours [Graphics_WHITE]);
	values. foreground = xwinColours [Graphics_BLACK];
	values. background = xwinColours [Graphics_WHITE];
	my text.gc = my gc = XCreateGC (my display, my window, GCForeground | GCBackground, & values);
}
#endif

/* End of file Graphics_colour.c */
