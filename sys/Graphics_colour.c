/* Graphics_colour.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2009/12/20 gotten rid of numbered standard colours
 * fb 2010/03/01 fix cairo in highlight2 ()
 * pb 2010/05/12 highlighting in GDK instead of Cairo because of the availability of a XOR mode
 * pb 2010/05/12 xorOn in GDK instead of Cairo because of the availability of a XOR mode
 * pb 2010/06/05 set my colour in setColour
 */

#include "GraphicsP.h"
#include <stdint.h>

Graphics_Colour
	Graphics_BLACK = { 0.0, 0.0, 0.0 },
	Graphics_WHITE = { 1.0, 1.0, 1.0 },
	Graphics_RED = { 0.865, 0.034, 0.026 },
	Graphics_GREEN = { 0.000, 0.500, 0.069 },
	Graphics_BLUE = { 0.000, 0.000, 0.828 },
	Graphics_CYAN = { 0.009, 0.669, 0.918 },
	Graphics_MAGENTA = { 0.949, 0.033, 0.519 },
	Graphics_YELLOW = { 0.984, 0.951, 0.020 },
	Graphics_MAROON = { 0.5, 0.0, 0.0 },
	Graphics_LIME = { 0.0, 1.0, 0.0 },
	Graphics_NAVY = { 0.0, 0.0, 0.5 },
	Graphics_TEAL = { 0.0, 0.5, 0.5 },
	Graphics_PURPLE = { 0.5, 0.0, 0.5 },
	Graphics_OLIVE = { 0.5, 0.5, 0.0 },
	Graphics_PINK = { 1.0, 0.75, 0.75 },
	Graphics_SILVER = { 0.75, 0.75, 0.75 },
	Graphics_GREY = { 0.5, 0.5, 0.5 };

const wchar_t * Graphics_Colour_name (Graphics_Colour colour) {
	return
		Graphics_Colour_equal (colour, Graphics_BLACK) ? L"black" :
		Graphics_Colour_equal (colour, Graphics_WHITE) ? L"white" :
		Graphics_Colour_equal (colour, Graphics_RED) ? L"red" :
		Graphics_Colour_equal (colour, Graphics_GREEN) ? L"green" :
		Graphics_Colour_equal (colour, Graphics_BLUE) ? L"blue" :
		Graphics_Colour_equal (colour, Graphics_CYAN) ? L"cyan" :
		Graphics_Colour_equal (colour, Graphics_MAGENTA) ? L"magenta" :
		Graphics_Colour_equal (colour, Graphics_YELLOW) ? L"yellow" :
		Graphics_Colour_equal (colour, Graphics_MAROON) ? L"maroon" :
		Graphics_Colour_equal (colour, Graphics_LIME) ? L"lime" :
		Graphics_Colour_equal (colour, Graphics_NAVY) ? L"navy" :
		Graphics_Colour_equal (colour, Graphics_TEAL) ? L"teal" :
		Graphics_Colour_equal (colour, Graphics_PURPLE) ? L"purple" :
		Graphics_Colour_equal (colour, Graphics_OLIVE) ? L"olive" :
		Graphics_Colour_equal (colour, Graphics_SILVER) ? L"silver" :
		Graphics_Colour_equal (colour, Graphics_GREY) ? L"grey" :
		colour. red == colour. green && colour. red == colour. blue ? Melder_fixed (colour. red, 6) :
		Melder_wcscat7 (L"{", Melder_fixed (colour. red, 6), L",", Melder_fixed (colour. green, 6), L",",
			Melder_fixed (colour. blue, 6), L"}");
}

#if mac
	#include "macport_on.h"
	#include <LowMem.h>
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void _Graphics_setColour (I, Graphics_Colour colour) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			if (my cr == NULL) return;
			cairo_set_source_rgb (my cr, colour. red, colour. green, colour. blue);
		#elif win
			my foregroundColour = RGB (colour. red * 255, colour. green * 255, colour. blue * 255);
			SelectPen (my dc, GetStockPen (BLACK_PEN));
			DeleteObject (my pen);
			my pen = CreatePen (PS_SOLID, 0, my foregroundColour);
			SelectBrush (my dc, GetStockBrush (NULL_BRUSH));
			DeleteObject (my brush);
			my brush = CreateSolidBrush (my foregroundColour);
		#elif mac
			my macColour. red = colour. red * 65535;
			my macColour. green = colour. green * 65535;
			my macColour. blue = colour. blue * 65535;
			// postpone till drawing
		#endif
	} else if (my postScript) {
		iam (GraphicsPostscript);
		my printf (my file, "%.6g %.6g %.6g setrgbcolor\n", colour. red, colour. green, colour. blue);
	}
}

void Graphics_setColour (I, Graphics_Colour colour) {
	iam (Graphics);
	my colour = colour;
	_Graphics_setColour (me, colour);
	if (my recording) { op (SET_RGB_COLOUR, 3); put (colour. red); put (colour. green); put (colour. blue); }
}

void _Graphics_setGrey (I, double fgrey) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			if (my cr == NULL) return;
			if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
			cairo_set_source_rgb (my cr, fgrey, fgrey, fgrey);
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
	my colour. red = my colour. green = my colour. blue = grey;
	_Graphics_setGrey (me, grey);
	if (my recording) { op (SET_GREY, 1); put (grey); }
}

static void highlight (I, short x1DC, short x2DC, short y1DC, short y2DC) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			if (my cr == NULL) return;
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			gdk_gc_set_function (my gc, GDK_XOR);
			GdkColor pinkXorWhite = { 0, 0x0000, 0x4000, 0x4000 }, black = { 0, 0x0000, 0x0000, 0x0000 };
			gdk_gc_set_rgb_fg_color (my gc, & pinkXorWhite);
			gdk_draw_rectangle (my window, my gc, TRUE, x1DC, y2DC, width, height);
			gdk_gc_set_rgb_fg_color (my gc, & black);
			gdk_gc_set_function (my gc, GDK_COPY);
			//cairo_set_source_rgba (my cr, 1.0, 0.8, 0.8, 0.5);
			//cairo_set_operator (my cr, CAIRO_OPERATOR_BITXOR);   // this blend mode doesn't exist
			//cairo_rectangle (my cr, x1DC, y2DC, width, height);
			//cairo_fill (my cr);
			//cairo_set_source_rgb (my cr, 0.0, 0.0, 0.0);
			//cairo_set_operator (my cr, CAIRO_OPERATOR_OVER);
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
		#if cairo
			if (my cr == NULL) return;
			cairo_save (my cr);
			cairo_set_source_rgba (my cr, 1.0, 0.8, 0.8, 0.5);
			//cairo_set_operator (my cr, CAIRO_OPERATOR_XOR);
			cairo_rectangle (my cr, x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC); // upper
			cairo_rectangle (my cr, x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner); // left part
			cairo_rectangle (my cr, x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner); // right part
			cairo_rectangle (my cr, x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner); // lower
			cairo_fill (my cr);
			cairo_restore (my cr);
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
	#if ! cairo
		highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner));
	#endif
	if (my recording)
		{ op (UNHIGHLIGHT2, 8); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner); }
}

void Graphics_xorOn (I, Graphics_Colour colour) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			GdkColor colourXorWhite = { 0,
				(uint16_t) (colour. red * 65535.0) ^ 0xFFFF,
				(uint16_t) (colour. green * 65535.0) ^ 0xFFFF,
				(uint16_t) (colour. blue * 65535.0) ^ 0xFFFF };
			gdk_gc_set_rgb_fg_color (my gc, & colourXorWhite);
			gdk_gc_set_function (my gc, GDK_XOR);
			//cairo_set_source_rgba (my cr, 1.0, 0.8, 0.8, 0.5);
			//cairo_set_operator (my cr, CAIRO_OPERATOR_XOR);
		#elif win
			SetROP2 (my dc, R2_XORPEN);
			_Graphics_setColour (me, colour);
		#elif mac
			if (my useQuartz) {
				//CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeDifference);
			} else {
				SetPort (my macPort);
				PenMode (patXor);
				TextMode (srcXor);
			}
		#endif
		my duringXor = true;
	}
	if (my recording) { op (XOR_ON, 3); put (colour. red); put (colour. green); put (colour. blue); }
}

void Graphics_xorOff (I) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			GdkColor black = { 0, 0x0000, 0x0000, 0x0000 };
			gdk_gc_set_rgb_fg_color (my gc, & black);
			gdk_gc_set_function (my gc, GDK_COPY);
			//cairo_set_source_rgba (my cr, 0.0, 0.0, 0.0, 1.0);
			//cairo_set_operator (my cr, CAIRO_OPERATOR_OVER);
		#elif win
			SetROP2 (my dc, R2_COPYPEN);
			_Graphics_setColour (me, my colour);
		#elif mac
			if (my useQuartz) {
				//CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeNormal);
			} else {
				SetPort (my macPort);
				PenMode (patCopy);
				TextMode (srcOr);
			}
		#endif
		my duringXor = false;
	}
	if (my recording) { op (XOR_OFF, 0); }
}

Graphics_Colour Graphics_inqColour (I) {
	iam (Graphics);
	return my colour;
}

/* End of file Graphics_colour.c */
