/* Graphics_colour.cpp
 *
 * Copyright (C) 1992-2005,2007-2020 Paul Boersma, 2013 Tom Naughton
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

#include "GraphicsP.h"

#if quartz
	#include "macport_on.h"
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void _Graphics_setColour (Graphics graphics, MelderColour colour) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (! my d_cairoGraphicsContext)
				return;
			cairo_set_source_rgb (my d_cairoGraphicsContext, colour. red, colour. green, colour. blue);
		#elif gdi
			my d_winForegroundColour = RGB (colour. red * 255, colour. green * 255, colour. blue * 255);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			DeleteObject (my d_winPen);
			my d_winPen = CreatePen (PS_SOLID, 0, my d_winForegroundColour);
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
			DeleteObject (my d_winBrush);
			my d_winBrush = CreateSolidBrush (my d_winForegroundColour);
		#elif quartz
			// postpone till drawing
		#endif
	} else if (graphics -> postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (graphics);
		my d_printf (my d_file, "%.6g %.6g %.6g setrgbcolor\n", colour. red, colour. green, colour. blue);
	}
}

void Graphics_setColour (Graphics me, MelderColour colour) {
	my colour = colour;
	_Graphics_setColour (me, colour);
	if (my recording) { op (SET_RGB_COLOUR, 3); put (colour. red); put (colour. green); put (colour. blue); }
}

void Graphics_setColourScale (Graphics me, enum kGraphics_colourScale colourScale) {
	my colourScale = colourScale;
	if (my recording) { op (SET_COLOUR_SCALE, 1); put (colourScale); }
}

void _Graphics_setGrey (Graphics graphics, double fgrey) {
	Melder_clip (0.0, & fgrey, 1.0);
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (! my d_cairoGraphicsContext)
				return;
			cairo_set_source_rgb (my d_cairoGraphicsContext, fgrey, fgrey, fgrey);
		#elif gdi
			int lightness = fgrey * 255;
			my d_winForegroundColour = RGB (lightness, lightness, lightness);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			DeleteObject (my d_winPen);
			my d_winPen = CreatePen (PS_SOLID, 0, my d_winForegroundColour);
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
			DeleteObject (my d_winBrush);
			my d_winBrush = CreateSolidBrush (my d_winForegroundColour);
		#elif quartz
		#endif
	} else if (graphics -> postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (graphics);
		my d_printf (my d_file, "%.6g setgray\n", fgrey);
	}
}

void Graphics_setGrey (Graphics me, double grey) {
	my colour. red = my colour. green = my colour. blue = grey;
	_Graphics_setGrey (me, grey);
	if (my recording) { op (SET_GREY, 1); put (grey); }
}

static void highlight (Graphics graphics, integer x1DC, integer x2DC, integer y1DC, integer y2DC) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (! my d_cairoGraphicsContext)
				return;
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0)
				return;
			if (! my d_cairoGraphicsContext)
				return;
			cairo_set_source_rgba (my d_cairoGraphicsContext, 1.0, 0.7, 0.7, 0.5);
			cairo_rectangle (my d_cairoGraphicsContext, x1DC, y2DC, width, height);
			cairo_fill (my d_cairoGraphicsContext);
		#elif gdi
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
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));   // superfluous?
		#elif quartz
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0)
				return;
			GuiCocoaDrawingArea *drawingArea = (GuiCocoaDrawingArea *) my d_drawingArea -> d_widget;
			if (! drawingArea)
				return;
			NSRect rect = NSMakeRect (x1DC, y2DC, width, height);
			CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
			if (context) {
				CGContextSaveGState (context);
				CGContextSetBlendMode (context, kCGBlendModeDarken);
				CGContextSetShouldAntialias (context, false);
				CGContextSetRGBFillColor (context, 1.0, 0.83, 0.83, 1.0);
				CGContextFillRect (context, rect);
				CGContextRestoreGState (context);
			}
		#endif
	}
}

void Graphics_highlight (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	if (my recording) {
		op (HIGHLIGHT, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC);
	} else
		highlight (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
}

static void highlight2 (Graphics graphics, integer x1DC, integer x2DC, integer y1DC, integer y2DC,
	integer x1DC_inner, integer x2DC_inner, integer y1DC_inner, integer y2DC_inner)
{
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		#if cairo
			if (! my d_cairoGraphicsContext)
				return;
			int width = x2DC - x1DC, height = y1DC - y2DC;
			if (width <= 0 || height <= 0)
				return;
			cairo_set_source_rgba (my d_cairoGraphicsContext, 1.0, 0.7, 0.7, 0.5);
			cairo_rectangle (my d_cairoGraphicsContext, x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC); // upper
			cairo_rectangle (my d_cairoGraphicsContext, x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner); // left part
			cairo_rectangle (my d_cairoGraphicsContext, x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner); // right part
			cairo_rectangle (my d_cairoGraphicsContext, x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner); // lower
			cairo_fill (my d_cairoGraphicsContext);
		#elif gdi
			static HBRUSH highlightBrush;
			if (! highlightBrush)
				highlightBrush = CreateSolidBrush (RGB (255, 210, 210));
			SelectPen (my d_gdiGraphicsContext, GetStockPen (NULL_PEN));
			SelectBrush (my d_gdiGraphicsContext, highlightBrush);
			SetROP2 (my d_gdiGraphicsContext, R2_MASKPEN);
			Rectangle (my d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y2DC_inner + 1);
			Rectangle (my d_gdiGraphicsContext, x1DC, y2DC_inner, x1DC_inner + 1, y1DC_inner + 1);
			Rectangle (my d_gdiGraphicsContext, x2DC_inner, y2DC_inner, x2DC + 1, y1DC_inner + 1);
			Rectangle (my d_gdiGraphicsContext, x1DC, y1DC_inner, x2DC + 1, y1DC + 1);
			SetROP2 (my d_gdiGraphicsContext, R2_COPYPEN);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));   // superfluous?
		#elif quartz
			GuiCocoaDrawingArea *drawingArea = (GuiCocoaDrawingArea *) my d_drawingArea -> d_widget;
			if (! drawingArea)
				return;
			my d_macGraphicsContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
			CGContextSaveGState (my d_macGraphicsContext);
			NSRect upperRect = NSMakeRect (x1DC, y2DC, x2DC - x1DC, y2DC_inner - y2DC);
			NSRect leftRect  = NSMakeRect (x1DC, y2DC_inner, x1DC_inner - x1DC, y1DC_inner - y2DC_inner);
			NSRect rightRect = NSMakeRect (x2DC_inner, y2DC_inner, x2DC - x2DC_inner, y1DC_inner - y2DC_inner);
			NSRect lowerRect = NSMakeRect (x1DC, y1DC_inner, x2DC - x1DC, y1DC - y1DC_inner);
			NSColor *colour = [[NSColor selectedTextBackgroundColor] colorUsingColorSpaceName: NSCalibratedRGBColorSpace];
			double red = 0.5 + 0.5 * colour.redComponent, green = 0.5 + 0.5 * colour.greenComponent, blue = 0.5 + 0.5 * colour.blueComponent;
			CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeDarken);
			CGContextSetRGBFillColor (my d_macGraphicsContext, 1.0, 0.83, 0.83, 1.0);
			CGContextFillRect (my d_macGraphicsContext, upperRect);
			CGContextFillRect (my d_macGraphicsContext, leftRect);
			CGContextFillRect (my d_macGraphicsContext, rightRect);
			CGContextFillRect (my d_macGraphicsContext, lowerRect);
			CGContextRestoreGState (my d_macGraphicsContext);
		#endif
	}
}

void Graphics_highlight2 (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC,
	double x1WC_inner, double x2WC_inner, double y1WC_inner, double y2WC_inner)
{
	if (my recording) {
		op (HIGHLIGHT2, 8);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (x1WC_inner); put (x2WC_inner); put (y1WC_inner); put (y2WC_inner);
	} else
		highlight2 (me, wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), wdx (x1WC_inner), wdx (x2WC_inner), wdy (y1WC_inner), wdy (y2WC_inner));
}

void Graphics_xorOn (Graphics graphics, MelderColour colourOnWhiteBackground) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		if (my recording) {
			op (XOR_ON, 3);
			put (colourOnWhiteBackground. red);
			put (colourOnWhiteBackground. green);
			put (colourOnWhiteBackground. blue);
		} else {
			my colour. red   = 1.0 - colourOnWhiteBackground. red;
			my colour. green = 1.0 - colourOnWhiteBackground. green;
			my colour. blue  = 1.0 - colourOnWhiteBackground. blue;
			#if cairo
				cairo_set_operator (my d_cairoGraphicsContext, CAIRO_OPERATOR_DIFFERENCE);
			#elif gdi
				SetROP2 (my d_gdiGraphicsContext, R2_XORPEN);
			#elif quartz
				CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeDifference);
			#endif
			_Graphics_setColour (me, my colour);
			my duringXor = true;
		}
	}
}

void Graphics_xorOff (Graphics graphics) {
	if (graphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (graphics);
		if (my recording) {
			op (XOR_OFF, 0);
		} else {
			#if cairo
				cairo_set_operator (my d_cairoGraphicsContext, CAIRO_OPERATOR_OVER);
			#elif gdi
				SetROP2 (my d_gdiGraphicsContext, R2_COPYPEN);
			#elif quartz
				CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeNormal);
			#endif
			_Graphics_setColour (me, my colour);
			my duringXor = false;
		}
	}
}

MelderColour Graphics_inqColour (Graphics me) {
	return my colour;
}

enum kGraphics_colourScale Graphics_inqColourScale (Graphics me) {
	return my colourScale;
}

/* End of file Graphics_colour.cpp */
