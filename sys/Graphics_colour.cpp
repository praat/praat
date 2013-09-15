/* Graphics_colour.cpp
 *
 * Copyright (C) 1992-2011,2012,2013 Paul Boersma, 2013 Tom Naughton
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
 * pb 2011/01/15 Windows: inverted the colour in XOR mode
 * pb 2011/03/17 C++
 * pb 2013/08/27 Cocoa: trick: triple kCGBlendModeDifference approximates coloured XOR
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
		//colour. red == colour. green && colour. red == colour. blue ? Melder_fixed (colour. red, 6) :
		Melder_wcscat (L"{", Melder_fixed (colour. red, 6), L",", Melder_fixed (colour. green, 6), L",",
			Melder_fixed (colour. blue, 6), L"}");
}

#if mac
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #endif
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void _Graphics_setColour (Graphics graphics, Graphics_Colour colour) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (my d_cairoGraphicsContext == NULL) return;
			cairo_set_source_rgb (my d_cairoGraphicsContext, colour. red, colour. green, colour. blue);
		#elif win
			my d_winForegroundColour = RGB (colour. red * 255, colour. green * 255, colour. blue * 255);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			DeleteObject (my d_winPen);
			my d_winPen = CreatePen (PS_SOLID, 0, my d_winForegroundColour);
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
			DeleteObject (my d_winBrush);
			my d_winBrush = CreateSolidBrush (my d_winForegroundColour);
		#elif mac
			my d_macColour. red = colour. red * 65535;
			my d_macColour. green = colour. green * 65535;
			my d_macColour. blue = colour. blue * 65535;
			// postpone till drawing
		#endif
	} else if (graphics -> postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (graphics);
		my d_printf (my d_file, "%.6g %.6g %.6g setrgbcolor\n", colour. red, colour. green, colour. blue);
	}
}

void Graphics_setColour (Graphics me, Graphics_Colour colour) {
	my colour = colour;
	_Graphics_setColour (me, colour);
	if (my recording) { op (SET_RGB_COLOUR, 3); put (colour. red); put (colour. green); put (colour. blue); }
}

void _Graphics_setGrey (Graphics graphics, double fgrey) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (my d_cairoGraphicsContext == NULL) return;
			if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
			cairo_set_source_rgb (my d_cairoGraphicsContext, fgrey, fgrey, fgrey);
		#elif win
			int lightness = fgrey <= 0 ? 0 : fgrey >= 1.0 ? 255 : fgrey * 255;
			my d_winForegroundColour = RGB (lightness, lightness, lightness);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			DeleteObject (my d_winPen);
			my d_winPen = CreatePen (PS_SOLID, 0, my d_winForegroundColour);
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
			DeleteObject (my d_winBrush);
			my d_winBrush = CreateSolidBrush (my d_winForegroundColour);
		#elif mac
			if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
			my d_macColour. red = my d_macColour. green = my d_macColour. blue = fgrey * 65535;
		#endif
	} else if (graphics ->  postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (graphics);
		if (fgrey < 0.0) fgrey = 0.0; else if (fgrey > 1.0) fgrey = 1.0;
		my d_printf (my d_file, "%.6g setgray\n", fgrey);
	}
}

void Graphics_setGrey (Graphics me, double grey) {
	my colour. red = my colour. green = my colour. blue = grey;
	_Graphics_setGrey (me, grey);
	if (my recording) { op (SET_GREY, 1); put (grey); }
}

static void highlight (Graphics graphics, long x1DC, long x2DC, long y1DC, long y2DC, int direction) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (my d_cairoGraphicsContext == NULL) return;
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			gdk_gc_set_function (my d_gdkGraphicsContext, GDK_XOR);
			GdkColor pinkXorWhite = { 0, 0x0000, 0x4000, 0x4000 }, black = { 0, 0x0000, 0x0000, 0x0000 };
			gdk_gc_set_rgb_fg_color (my d_gdkGraphicsContext, & pinkXorWhite);
			gdk_draw_rectangle (my d_window, my d_gdkGraphicsContext, TRUE, x1DC, y2DC, width, height);
			gdk_gc_set_rgb_fg_color (my d_gdkGraphicsContext, & black);
			gdk_gc_set_function (my d_gdkGraphicsContext, GDK_COPY);
			gdk_flush ();
		#elif cocoa
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			GuiCocoaDrawingArea *drawingArea = (GuiCocoaDrawingArea *) my d_drawingArea -> d_widget;
			if (drawingArea) {
				[drawingArea lockFocus];
				CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
				CGContextSaveGState (context);
				NSCAssert (context, @"nil context");
				CGContextTranslateCTM (context, 0, drawingArea. bounds. size. height);
				CGContextScaleCTM (context, 1.0, -1.0);
				NSRect rect = NSMakeRect (x1DC,  y2DC, width, height);
				CGContextSetBlendMode (context, kCGBlendModeDifference);
				CGContextSetShouldAntialias (context, false);
				NSColor *colour = [[NSColor selectedTextBackgroundColor] colorUsingColorSpaceName: NSCalibratedRGBColorSpace];
				double red = 0.5 + 0.5 * colour.redComponent, green = 0.5 + 0.5 * colour.greenComponent, blue = 0.5 + 0.5 * colour.blueComponent;
				if (direction == 1) {   // forward
					CGContextSetRGBFillColor (context, 1.0 - red, 1.0 - green, 1.0 - blue, 1.0);
					CGContextFillRect (context, rect);
				} else {   // backward
					CGContextSetRGBFillColor (context, red, green, blue, 1.0);
					CGContextFillRect (context, rect);
					CGContextSetRGBFillColor (context, 1.0, 1.0, 1.0, 1.0);
					CGContextFillRect (context, rect);
				}
				CGContextRestoreGState (context);
				//CGContextSynchronize (context);
				[drawingArea unlockFocus];
			}
        #elif mac
			Rect rect;
			if (my d_drawingArea) GuiMac_clipOn (my d_drawingArea -> d_widget);
			SetRect (& rect, x1DC, y2DC, x2DC, y1DC);
			SetPort (my d_macPort);
			LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);   // see IM V-61
			InvertRect (& rect);
			if (my d_drawingArea) GuiMac_clipOff ();
		#elif win
			static HBRUSH highlightBrush;
			RECT rect;
			rect. left = x1DC, rect. right = x2DC, rect. top = y2DC, rect. bottom = y1DC;
			if (! highlightBrush)
				highlightBrush = CreateSolidBrush (RGB (255, 210, 210));
			SelectPen (my d_gdiGraphicsContext, GetStockPen (NULL_PEN));
			SelectBrush (my d_gdiGraphicsContext, highlightBrush);
			SetROP2 (my d_gdiGraphicsContext, R2_NOTXORPEN);
			Rectangle (my d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y1DC + 1);
			SetROP2 (my d_gdiGraphicsContext, R2_COPYPEN);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));   /* Superfluous? */
		#endif
	}
}

void Graphics_highlight (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	highlight (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), 1);
	if (my recording)
		{ op (HIGHLIGHT, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

void Graphics_unhighlight (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	highlight (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), 2);
	if (my recording)
		{ op (UNHIGHLIGHT, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

static void highlight2 (Graphics graphics, long x1DC, long x2DC, long y1DC, long y2DC,
	long x1DC_inner, long x2DC_inner, long y1DC_inner, long y2DC_inner, int direction)
{
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (my d_cairoGraphicsContext == NULL) return;
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0) return;
			gdk_gc_set_function (my d_gdkGraphicsContext, GDK_XOR);
			GdkColor pinkXorWhite = { 0, 0x0000, 0x4000, 0x4000 }, black = { 0, 0x0000, 0x0000, 0x0000 };
			gdk_gc_set_rgb_fg_color (my d_gdkGraphicsContext, & pinkXorWhite);
			gdk_draw_rectangle (my d_window, my d_gdkGraphicsContext, TRUE, x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC); // upper
			gdk_draw_rectangle (my d_window, my d_gdkGraphicsContext, TRUE, x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner); // left part
			gdk_draw_rectangle (my d_window, my d_gdkGraphicsContext, TRUE, x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner); // right part
			gdk_draw_rectangle (my d_window, my d_gdkGraphicsContext, TRUE, x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner); // lower
			gdk_gc_set_rgb_fg_color (my d_gdkGraphicsContext, & black);
			gdk_gc_set_function (my d_gdkGraphicsContext, GDK_COPY);
			gdk_flush ();
		#elif cocoa
			GuiCocoaDrawingArea *drawingArea = (GuiCocoaDrawingArea*) my d_drawingArea -> d_widget;
			if (drawingArea) {
				[drawingArea lockFocus];

				my d_macGraphicsContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
				CGContextSaveGState (my d_macGraphicsContext);

				NSCAssert (my d_macGraphicsContext, @"nil context");
				CGContextTranslateCTM (my d_macGraphicsContext, 0, drawingArea. bounds. size. height);
				CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);

				NSRect upperRect = NSMakeRect (x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC);
				NSRect leftRect  = NSMakeRect (x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner);
				NSRect rightRect = NSMakeRect (x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner);
				NSRect lowerRect = NSMakeRect (x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner);
				CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeDifference);
				NSColor *colour = [[NSColor selectedTextBackgroundColor] colorUsingColorSpaceName: NSCalibratedRGBColorSpace];
				double red = 0.5 + 0.5 * colour.redComponent, green = 0.5 + 0.5 * colour.greenComponent, blue = 0.5 + 0.5 * colour.blueComponent;
				if (direction == 1) {
					CGContextSetRGBFillColor (my d_macGraphicsContext, 1.0 - red, 1.0 - green, 1.0 - blue, 1.0);
					CGContextFillRect (my d_macGraphicsContext, upperRect);
					CGContextFillRect (my d_macGraphicsContext, leftRect);
					CGContextFillRect (my d_macGraphicsContext, rightRect);
					CGContextFillRect (my d_macGraphicsContext, lowerRect);
				} else {
					CGContextSetRGBFillColor (my d_macGraphicsContext, red, green, blue, 1.0);
					CGContextFillRect (my d_macGraphicsContext, upperRect);
					CGContextFillRect (my d_macGraphicsContext, leftRect);
					CGContextFillRect (my d_macGraphicsContext, rightRect);
					CGContextFillRect (my d_macGraphicsContext, lowerRect);
					CGContextSetRGBFillColor (my d_macGraphicsContext, 1.0, 1.0, 1.0, 1.0);
					CGContextFillRect (my d_macGraphicsContext, upperRect);
					CGContextFillRect (my d_macGraphicsContext, leftRect);
					CGContextFillRect (my d_macGraphicsContext, rightRect);
					CGContextFillRect (my d_macGraphicsContext, lowerRect);
				}

				CGContextRestoreGState (my d_macGraphicsContext);
				//CGContextSynchronize ( my d_macGraphicsContext);   // needed?
				[drawingArea unlockFocus];
			}
        #elif mac
			Rect rect;
			if (my d_drawingArea) GuiMac_clipOn (my d_drawingArea -> d_widget);
			SetPort (my d_macPort);
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
			if (my d_drawingArea) GuiMac_clipOff ();
		#elif win
			static HBRUSH highlightBrush;
			if (! highlightBrush)
				highlightBrush = CreateSolidBrush (RGB (255, 210, 210));
			SelectPen (my d_gdiGraphicsContext, GetStockPen (NULL_PEN));
			SelectBrush (my d_gdiGraphicsContext, highlightBrush);
			SetROP2 (my d_gdiGraphicsContext, R2_NOTXORPEN);
			Rectangle (my d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y2DC_inner + 1);
			Rectangle (my d_gdiGraphicsContext, x1DC, y2DC_inner, x1DC_inner + 1, y1DC_inner + 1);
			Rectangle (my d_gdiGraphicsContext, x2DC_inner, y2DC_inner, x2DC + 1, y1DC_inner + 1);
			Rectangle (my d_gdiGraphicsContext, x1DC, y1DC_inner, x2DC + 1, y1DC + 1);
			SetROP2 (my d_gdiGraphicsContext, R2_COPYPEN);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));   /* Superfluous? */
		#endif
	}
}

void Graphics_highlight2 (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC,
	double x1WC_inner, double x2WC_inner, double y1WC_inner, double y2WC_inner)
{
	highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner), 1);
	if (my recording)
		{ op (HIGHLIGHT2, 8); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner); }
}

void Graphics_unhighlight2 (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC,
	double x1WC_inner, double x2WC_inner, double y1WC_inner, double y2WC_inner)
{
	highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner), 2);
	if (my recording)
		{ op (UNHIGHLIGHT2, 8); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner); }
}

void Graphics_xorOn (Graphics graphics, Graphics_Colour colour) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			GdkColor colourXorWhite = { 0,
				(uint16_t) (colour. red * 65535.0) ^ 0xFFFF,
				(uint16_t) (colour. green * 65535.0) ^ 0xFFFF,
				(uint16_t) (colour. blue * 65535.0) ^ 0xFFFF };
			gdk_gc_set_rgb_fg_color (my d_gdkGraphicsContext, & colourXorWhite);
			gdk_gc_set_function (my d_gdkGraphicsContext, GDK_XOR);
			//cairo_set_source_rgba (my d_cairoGraphicsContext, 1.0, 0.8, 0.8, 0.5);
			//cairo_set_operator (my d_cairoGraphicsContext, CAIRO_OPERATOR_XOR);
			gdk_flush ();
		#elif win
			SetROP2 (my d_gdiGraphicsContext, R2_XORPEN);
			colour. red = ((uint16_t) (colour. red * 65535.0) ^ 0xFFFF) / 65535.0;
			colour. green = ((uint16_t) (colour. green * 65535.0) ^ 0xFFFF) / 65535.0;
			colour. blue = ((uint16_t) (colour. blue * 65535.0) ^ 0xFFFF) / 65535.0;
			_Graphics_setColour (me, colour);
		#elif cocoa
		#elif mac
			//CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeDifference);
		#endif
		my duringXor = true;
		if (graphics -> recording) { op (XOR_ON, 3); put (colour. red); put (colour. green); put (colour. blue); }
	}
}

void Graphics_xorOff (Graphics graphics) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			GdkColor black = { 0, 0x0000, 0x0000, 0x0000 };
			gdk_gc_set_rgb_fg_color (my d_gdkGraphicsContext, & black);
			gdk_gc_set_function (my d_gdkGraphicsContext, GDK_COPY);
			//cairo_set_source_rgba (my d_cairoGraphicsContext, 0.0, 0.0, 0.0, 1.0);
			//cairo_set_operator (my d_cairoGraphicsContext, CAIRO_OPERATOR_OVER);
			gdk_flush ();   // to undraw the last drawing
		#elif win
			SetROP2 (my d_gdiGraphicsContext, R2_COPYPEN);
			_Graphics_setColour (me, my colour);
		#elif cocoa
			Graphics_flushWs (graphics);   // to undraw the last drawing
		#elif mac
			//CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeNormal);
		#endif
		my duringXor = false;
		if (graphics -> recording) { op (XOR_OFF, 0); }
	}
}

Graphics_Colour Graphics_inqColour (Graphics me) {
	return my colour;
}

/* End of file Graphics_colour.cpp */
