/* Graphics_linesAndAreas.cpp
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

#include "GraphicsP.h"

/* Normally on, because e.g. the intensity contour in the Sound window should not run through the play buttons: */
#define FUNCTIONS_ARE_CLIPPED  1

#define POSTSCRIPT_MAXPATH  1000
#define LINE_WIDTH_IN_PIXELS(me)  ( my resolution > 192 ? my lineWidth * (my resolution / 192.0) : my lineWidth )
#define ORDER_DC  { long temp; if (x1DC > x2DC) temp = x1DC, x1DC = x2DC, x2DC = temp; \
	if (yIsZeroAtTheTop == (y2DC > y1DC)) temp = y1DC, y1DC = y2DC, y2DC = temp; }

static void psPrepareLine (GraphicsPostscript me) {
	double lineWidth_pixels = LINE_WIDTH_IN_PIXELS (me);
	if (my lineType == Graphics_DOTTED)
		my d_printf (my d_file, "[%ld %ld] 0 setdash\n", (long) (my resolution / 100), (long) (my resolution / 75 + lineWidth_pixels));
	else if (my lineType == Graphics_DASHED)
		my d_printf (my d_file, "[%ld %ld] 0 setdash\n", (long) (my resolution / 25), (long) (my resolution / 50 + lineWidth_pixels));
	else if (my lineType == Graphics_DASHED_DOTTED)
		my d_printf (my d_file, "[%ld %ld %ld %ld] 0 setdash\n",
			(long) (my resolution / 100), (long) (my resolution / 60 + lineWidth_pixels),
			(long) (my resolution / 25), (long) (my resolution / 60 + lineWidth_pixels));
	if (my lineWidth != 1.0)
		my d_printf (my d_file, "%g setlinewidth\n", lineWidth_pixels);
}
static void psRevertLine (GraphicsPostscript me) {
	if (my lineType != Graphics_DRAWN)
		my d_printf (my d_file, "[] 0 setdash\n");
	if (my lineWidth != 1.0)
		my d_printf (my d_file, "%g setlinewidth\n", my resolution > 192 ? my resolution / 192.0 : 1.0);   // 0.375 point
}

#if cairo
	static void gdkPrepareLine (GraphicsScreen me) {
		gdk_gc_set_line_attributes (my d_gdkGraphicsContext, my lineWidth,
			my lineType >= Graphics_DOTTED ? GDK_LINE_ON_OFF_DASH : GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
	}
	static void gdkRevertLine (GraphicsScreen me) {
		if (my lineType >= Graphics_DOTTED) {
			gdk_gc_set_line_attributes (my d_gdkGraphicsContext, my lineWidth, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
		}
	}
	static void cairoPrepareLine (GraphicsScreen me) {
		if (my d_cairoGraphicsContext == NULL) return;
		double dotted_line [] = { 2, 2 };
		double dashed_line [] = { 6, 2 };
		double dashed_dotted_line [] = { 6, 2, 2, 2 };
		cairo_save (my d_cairoGraphicsContext);
		switch (my lineType) {
			case Graphics_DOTTED:
				cairo_set_dash (my d_cairoGraphicsContext, dotted_line, 2, 1.0);
				break;
			case Graphics_DASHED:
				cairo_set_dash (my d_cairoGraphicsContext, dashed_line, 2, 1.0);
				break;
			case Graphics_DASHED_DOTTED:
				cairo_set_dash (my d_cairoGraphicsContext, dashed_dotted_line, 4, 1.0);
				break;
		}
		cairo_set_line_width (my d_cairoGraphicsContext, my lineWidth);
	}
	static void cairoRevertLine (GraphicsScreen me) {
		if (my d_cairoGraphicsContext == NULL) return;
		if (my lineType >= Graphics_DOTTED) {
			cairo_set_dash (my d_cairoGraphicsContext, NULL, 0, 0);
		}
		cairo_restore (my d_cairoGraphicsContext);
	}
#elif win
	#define MY_BRUSH  SelectPen (d_gdiGraphicsContext, GetStockPen (NULL_PEN)), SelectBrush (d_gdiGraphicsContext, d_winBrush);
	#define DEFAULT  SelectPen (d_gdiGraphicsContext, GetStockPen (BLACK_PEN)), SelectBrush (d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
	static void winPrepareLine (GraphicsScreen me) {
		HPEN newPen;
		int lineWidth_pixels = LINE_WIDTH_IN_PIXELS (me) + 0.5;
		if (! lineWidth_pixels) lineWidth_pixels = 1;
		my d_fatNonSolid = my lineType != Graphics_DRAWN && my lineWidth > 1;
		if (Melder_debug == 10) {
			LOGBRUSH brush;
			brush. lbStyle = BS_SOLID;
			brush. lbColor = my d_winForegroundColour;
			brush. lbHatch = my lineType == Graphics_DRAWN ? 0 : my lineType == Graphics_DOTTED ? PS_DOT : my lineType == Graphics_DASHED ? PS_DASH : PS_DASHDOT;
			newPen = ExtCreatePen (PS_GEOMETRIC, lineWidth_pixels, & brush, 0, NULL);
		} else {
			/*newPen = CreatePen (my lineType == Graphics_DRAWN ? PS_SOLID :
				my lineType == Graphics_DOTTED ? PS_DOT : my lineType == Graphics_DASHED ? PS_DASH : PS_DASHDOT,
				my fatNonSolid ? 1 : lineWidth_pixels, my foregroundColour);*/
			LOGPEN pen;
			pen. lopnStyle = my lineType == Graphics_DRAWN ? PS_SOLID : my lineType == Graphics_DOTTED ? PS_DOT : my lineType == Graphics_DASHED ? PS_DASH : PS_DASHDOT;
			pen. lopnWidth. x = my d_fatNonSolid ? 1 : lineWidth_pixels;
			pen. lopnWidth. y = 0;
			pen. lopnColor = my d_winForegroundColour | 0x02000000;
			newPen = CreatePenIndirect (& pen);
		}
		SelectPen (my d_gdiGraphicsContext, newPen);
		DeletePen (my d_winPen);
		my d_winPen = newPen;
	}
#elif mac
	static void quartzPrepareLine (GraphicsScreen me) {
		if (my duringXor) {
			CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeDifference);
			CGContextSetAllowsAntialiasing (my d_macGraphicsContext, false);
			CGContextSetRGBStrokeColor (my d_macGraphicsContext, 1.0, 1.0, 1.0, 1.0);
		} else {
			CGContextSetRGBStrokeColor (my d_macGraphicsContext, my d_macColour.red / 65536.0, my d_macColour.green / 65536.0, my d_macColour.blue / 65536.0, 1.0);
		}
		double lineWidth_pixels = LINE_WIDTH_IN_PIXELS (me);
		CGContextSetLineWidth (my d_macGraphicsContext, lineWidth_pixels);
		CGFloat lengths [4];
		if (my lineType == Graphics_DOTTED)
			lengths [0] = my resolution > 192 ? my resolution / 100.0 : 2,
			lengths [1] = my resolution > 192 ? my resolution / 75.0 + lineWidth_pixels : 2;
		if (my lineType == Graphics_DASHED)
			lengths [0] = my resolution > 192 ? my resolution / 25 : 6,
			lengths [1] = my resolution > 192 ? my resolution / 50.0 + lineWidth_pixels : 2;
		if (my lineType == Graphics_DASHED_DOTTED)
			lengths [0] = my resolution > 192 ? my resolution / 25 : 6,
			lengths [1] = my resolution > 192 ? my resolution / 50.0 + lineWidth_pixels : 2;
			lengths [2] = my resolution > 192 ? my resolution / 100.0 : 2;
			lengths [3] = my resolution > 192 ? my resolution / 50.0 + lineWidth_pixels : 2;
		CGContextSetLineDash (my d_macGraphicsContext, 0.0, my lineType == Graphics_DRAWN ? NULL : lengths, my lineType == 0 ? 0 : my lineType == Graphics_DASHED_DOTTED ? 4 : 2);
	}
	static void quartzRevertLine (GraphicsScreen me) {
		if (my duringXor) {
			CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeNormal);
			CGContextSetAllowsAntialiasing (my d_macGraphicsContext, true);
		}
	}
	static void quartzPrepareFill (GraphicsScreen me) {
		CGContextSetAlpha (my d_macGraphicsContext, 1.0);
		CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeNormal);
		CGContextSetRGBFillColor (my d_macGraphicsContext, my d_macColour.red / 65536.0, my d_macColour.green / 65536.0, my d_macColour.blue / 65536.0, 1.0);
	}
#endif

/* First level. */

void structGraphicsScreen :: v_polyline (long numberOfPoints, long *xyDC, bool close) {
	#if cairo
		if (duringXor) {
			gdkPrepareLine (this);
			for (long i = 0; i < numberOfPoints - 1; i ++) {
				gdk_draw_line (d_window, d_gdkGraphicsContext, xyDC [i + i], xyDC [i + i + 1], xyDC [i + i + 2], xyDC [i + i + 3]);
			}
			gdkRevertLine (this);
		} else {
			if (d_cairoGraphicsContext == NULL) return;
			cairoPrepareLine (this);
			// cairo_new_path (d_cairoGraphicsContext); // move_to() automatically creates a new path
			cairo_move_to (d_cairoGraphicsContext, (double) xyDC [0], (double) xyDC [1]);
			for (long i = 1; i < numberOfPoints; i ++) {
				cairo_line_to (d_cairoGraphicsContext, (double) xyDC [i + i], (double) xyDC [i + i + 1]);
			}
			if (close) cairo_close_path (d_cairoGraphicsContext);
			cairo_stroke (d_cairoGraphicsContext);
			cairoRevertLine (this);
		}
	#elif win
		winPrepareLine (this);
		POINT *points = Melder_malloc (POINT, numberOfPoints + close);
		if (points) {
			for (long i = 0; i < numberOfPoints; i ++) {
				points [i]. x = *xyDC, xyDC ++;
				points [i]. y = *xyDC, xyDC ++;
			}
			if (close)
				points [numberOfPoints] = points [0];
			Polyline (d_gdiGraphicsContext, points, numberOfPoints + close);
			if (d_fatNonSolid) {
				for (long i = 0; i < numberOfPoints; i ++)
					points [i]. x -= 1;
				if (close)
					points [numberOfPoints] = points [0];
				Polyline (d_gdiGraphicsContext, points, numberOfPoints + close);
				for (long i = 0; i < numberOfPoints; i ++) {
					points [i]. x += 1;
					points [i]. y -= 1;
				}
				if (close)
					points [numberOfPoints] = points [0];
				Polyline (d_gdiGraphicsContext, points, numberOfPoints + close);
			}
		}
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareLine (this);
		CGContextBeginPath (d_macGraphicsContext);
		CGContextMoveToPoint (d_macGraphicsContext, xyDC [0], xyDC [1]);   // starts a new subpath
		for (long i = 1; i < numberOfPoints; i ++) {
			CGContextAddLineToPoint (d_macGraphicsContext, xyDC [i + i], xyDC [i + i + 1]);
		}
		if (close)
			CGContextClosePath (d_macGraphicsContext);   // closes only the subpath
		CGContextStrokePath (d_macGraphicsContext);
		quartzRevertLine (this);
		GraphicsQuartz_exitDraw (this);
	#endif
}

void structGraphicsPostscript :: v_polyline (long numberOfPoints, long *xyDC, bool close) {
	long nn = numberOfPoints + numberOfPoints;
	psPrepareLine (this);
	d_printf (d_file, "N %ld %ld moveto\n", xyDC [0], xyDC [1]);
	for (long i = 2, ipath = 1; i < nn; i += 2) {
		long dx = xyDC [i] - xyDC [i - 2], dy = xyDC [i + 1] - xyDC [i - 1];
		if (dx == 1 && i < nn - 20 && xyDC [i + 2] - xyDC [i] == 1 &&
			 xyDC [i + 4] - xyDC [i + 2] == 1 && xyDC [i + 6] - xyDC [i + 4] == 1 &&
			 xyDC [i + 8] - xyDC [i + 6] == 1 && xyDC [i + 10] - xyDC [i + 8] == 1 &&
			 xyDC [i + 12] - xyDC [i + 10] == 1 && xyDC [i + 14] - xyDC [i + 12] == 1 &&
			 xyDC [i + 16] - xyDC [i + 14] == 1 && xyDC [i + 18] - xyDC [i + 16] == 1)
		{
			d_printf (d_file, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld F\n",
				xyDC [i + 19] - xyDC [i + 17], xyDC [i + 17] - xyDC [i + 15],
				xyDC [i + 15] - xyDC [i + 13], xyDC [i + 13] - xyDC [i + 11],
				xyDC [i + 11] - xyDC [i + 9], xyDC [i + 9] - xyDC [i + 7],
				xyDC [i + 7] - xyDC [i + 5], xyDC [i + 5] - xyDC [i + 3],
				xyDC [i + 3] - xyDC [i + 1], dy);
			ipath += 9;
			i += 18;
		} else if (dx != 0 || dy != 0 || i < 4) {
			d_printf (d_file, "%ld %ld L\n", dx, dy);
		}
		if (++ ipath >= POSTSCRIPT_MAXPATH && i != nn - 2) {
			d_printf (d_file, "currentpoint stroke moveto\n");
			ipath = 1;
		}
	}
	if (close)
		d_printf (d_file, "closepath ");
	d_printf (d_file, "stroke\n");
	psRevertLine (this);
}

void structGraphicsScreen :: v_fillArea (long numberOfPoints, long *xyDC) {
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;
		// cairo_new_path (d_cairoGraphicsContext); // move_to() automatically creates a new path
		cairo_move_to (d_cairoGraphicsContext, xyDC [0], xyDC [1]);
		for (long i = 1; i < numberOfPoints; i ++)
			cairo_line_to (d_cairoGraphicsContext, xyDC [i + i], xyDC [i + i + 1]);
		cairo_close_path (d_cairoGraphicsContext);
		cairo_fill (d_cairoGraphicsContext);
	#elif win
		MY_BRUSH
		BeginPath (d_gdiGraphicsContext);
		MoveToEx (d_gdiGraphicsContext, xyDC [0], xyDC [1], NULL);
		for (long i = 1; i < numberOfPoints; i ++)
			LineTo (d_gdiGraphicsContext, xyDC [i + i], xyDC [i + i + 1]);
		EndPath (d_gdiGraphicsContext);
		FillPath (d_gdiGraphicsContext);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareFill (this);
		CGContextBeginPath (d_macGraphicsContext);
		CGContextMoveToPoint (d_macGraphicsContext, xyDC [0], xyDC [1]);
		for (long i = 1; i < numberOfPoints; i ++) {
			CGContextAddLineToPoint (d_macGraphicsContext, xyDC [i + i], xyDC [i + i + 1]);
		}
		CGContextFillPath (d_macGraphicsContext);
		GraphicsQuartz_exitDraw (this);
	#endif
}

void structGraphicsPostscript :: v_fillArea (long numberOfPoints, long *xyDC) {
	long nn = numberOfPoints + numberOfPoints;
	d_printf (d_file, "N %ld %ld M\n", xyDC [0], xyDC [1]);
	/*
	 * Very old (?) printers have path size restrictions.
	 * That's no reason to truncate the path on newer printers.
	 */
	#if 0
		if (numberOfPoints > POSTSCRIPT_MAXPATH) {
			Melder_warning (L"GraphicsPostscript::fillArea: path truncated.");
			numberOfPoints = POSTSCRIPT_MAXPATH, nn = numberOfPoints + numberOfPoints;
		}
	#endif
	for (long i = 2; i < nn; i += 2) {
		d_printf (d_file, "%ld %ld L\n",
			xyDC [i] - xyDC [i - 2], xyDC [i + 1] - xyDC [i - 1]);
	}
	d_printf (d_file, "closepath fill\n");
}

/* Second level. */

void structGraphicsScreen :: v_rectangle (long x1DC, long x2DC, long y1DC, long y2DC) {
	ORDER_DC
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;
		int width = x2DC - x1DC, height = y1DC - y2DC;
		if (width <= 0 || height <= 0) return;
		cairoPrepareLine (this);
		cairo_rectangle (d_cairoGraphicsContext, x1DC, y2DC, width, height);
		cairo_stroke (d_cairoGraphicsContext);
		cairoRevertLine (this);
	#elif win
		winPrepareLine (this);
		Rectangle (d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y1DC + 1);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareLine (this);
		CGContextStrokeRect (d_macGraphicsContext, CGRectMake (x1DC - 0.5, y2DC - 0.5, x2DC - x1DC, y1DC - y2DC));
		quartzRevertLine (this);
		GraphicsQuartz_exitDraw (this);
	#else
		long xyDC [8];
		xyDC [0] = x1DC;	xyDC [1] = y1DC;
		xyDC [2] = x2DC;	xyDC [3] = y1DC;
		xyDC [4] = x2DC;	xyDC [5] = y2DC;
		xyDC [6] = x1DC;	xyDC [7] = y2DC;
		v_polyline (5, & xyDC [0], true);
	#endif
}

void structGraphicsPostscript :: v_rectangle (long x1DC, long x2DC, long y1DC, long y2DC) {
	psPrepareLine (this);
	d_printf (d_file, "N %ld %ld M %ld %ld lineto %ld %ld lineto %ld %ld lineto closepath stroke\n",
		x1DC, y1DC, x2DC, y1DC, x2DC, y2DC, x1DC, y2DC);
	psRevertLine (this);
}

void structGraphicsScreen :: v_fillRectangle (long x1DC, long x2DC, long y1DC, long y2DC) {
	ORDER_DC
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;	
		int width = x2DC - x1DC + 1, height = y1DC - y2DC + 1;
		if (width <= 0 || height <= 0) return;
		cairo_rectangle (d_cairoGraphicsContext, x1DC, y2DC, width, height);
		cairo_fill (d_cairoGraphicsContext);
	#elif win
		RECT rect;
		rect. left = x1DC, rect. right = x2DC, rect. top = y2DC, rect. bottom = y1DC;   /* Superfluous? */
		MY_BRUSH
		Rectangle (d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y1DC + 1);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareFill (this);
		CGContextFillRect (d_macGraphicsContext, CGRectMake (x1DC, y2DC, x2DC - x1DC, y1DC - y2DC));
		GraphicsQuartz_exitDraw (this);
	#else
		long xyDC [10];
		xyDC [0] = x1DC;	xyDC [1] = y1DC;
		xyDC [2] = x2DC;	xyDC [3] = y1DC;
		xyDC [4] = x2DC;	xyDC [5] = y2DC;
		xyDC [6] = x1DC;	xyDC [7] = y2DC;
		xyDC [8] = x1DC;	xyDC [9] = y1DC;
		v_fillArea (5, & xyDC [0]);
	#endif
}

void structGraphicsPostscript :: v_fillRectangle (long x1DC, long x2DC, long y1DC, long y2DC) {
	d_printf (d_file,
		"N %ld %ld M %ld %ld lineto %ld %ld lineto %ld %ld lineto closepath fill\n",
		x1DC, y1DC, x2DC, y1DC, x2DC, y2DC, x1DC, y2DC);
}

void structGraphicsScreen :: v_circle (double xDC, double yDC, double rDC) {
	#if cairo
		if (duringXor) {
			gdkPrepareLine (this);
			gdk_draw_arc (d_window, d_gdkGraphicsContext, FALSE, xDC - rDC, yDC - rDC, rDC + rDC, rDC + rDC, 0, 360 * 64);
			gdkRevertLine (this);
		} else {
			if (d_cairoGraphicsContext == NULL) return;
			cairoPrepareLine (this);
			cairo_new_path (d_cairoGraphicsContext);
			cairo_arc (d_cairoGraphicsContext, xDC, yDC, rDC, 0.0, 2 * M_PI);
			cairo_stroke (d_cairoGraphicsContext);
			cairoRevertLine (this);
		}
	#elif win
		winPrepareLine (this);
		Ellipse (d_gdiGraphicsContext, xDC - rDC, yDC - rDC, xDC + rDC + 1, yDC + rDC + 1);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareLine (this);
		CGContextBeginPath (d_macGraphicsContext);
		CGContextAddArc (d_macGraphicsContext, xDC, yDC, rDC, 0.0, NUM2pi, 0);
		CGContextStrokePath (d_macGraphicsContext);
		quartzRevertLine (this);
		GraphicsQuartz_exitDraw (this);
	#endif
}

void structGraphicsPostscript :: v_circle (double xDC, double yDC, double rDC) {
	psPrepareLine (this);
	d_printf (d_file, "N %ld %ld %ld C\n", (long) xDC, (long) yDC, (long) rDC);
	psRevertLine (this);
}

void structGraphicsScreen :: v_ellipse (long x1DC, long x2DC, long y1DC, long y2DC) {
	ORDER_DC
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;
		cairoPrepareLine (this);
		cairo_new_path (d_cairoGraphicsContext);
		cairo_save (d_cairoGraphicsContext);
		cairo_translate (d_cairoGraphicsContext, 0.5 * (x2DC + x1DC), 0.5 * (y2DC + y1DC));
		cairo_scale (d_cairoGraphicsContext, 0.5 * (x2DC - x1DC), 0.5 * (y2DC - y1DC));
		cairo_arc (d_cairoGraphicsContext, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
		cairo_restore (d_cairoGraphicsContext);
		cairo_stroke (d_cairoGraphicsContext);
		cairoRevertLine (this);
	#elif win
		winPrepareLine (this);
		Ellipse (d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y1DC + 1);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareLine (this);
        NSCAssert( d_macGraphicsContext, @"nil context");

		CGContextBeginPath (d_macGraphicsContext);
		CGContextSaveGState (d_macGraphicsContext);
		CGContextTranslateCTM (d_macGraphicsContext, 0.5 * (x2DC + x1DC), 0.5 * (y2DC + y1DC));
		CGContextScaleCTM (d_macGraphicsContext, 0.5 * (x2DC - x1DC), 0.5 * (y2DC - y1DC));
		CGContextAddArc (d_macGraphicsContext, 0.0, 0.0, 1.0, 0.0, NUM2pi, 0);
		CGContextScaleCTM (d_macGraphicsContext, 2.0 / (x2DC - x1DC), 2.0 / (y2DC - y1DC));
		CGContextStrokePath (d_macGraphicsContext);
		CGContextRestoreGState (d_macGraphicsContext);
		quartzRevertLine (this);
		GraphicsQuartz_exitDraw (this);
	#endif
}

void structGraphicsPostscript :: v_ellipse (long x1DC, long x2DC, long y1DC, long y2DC) {
	if (x1DC != x2DC && y1DC != y2DC) {   // prevent division by zero
		psPrepareLine (this);
		/* To draw an ellipse, we will have to 'translate' and 'scale' and draw a circle. */
		/* However, we have to scale back before the actual 'stroke', */
		/* because we want the normal line thickness; */
		/* So we cannot use 'gsave' and 'grestore', which clear the path (Cookbook 3). */
		d_printf (d_file, "gsave %f %f translate %f %f scale N 0 0 1 0 360 arc\n"
			" %f %f scale stroke grestore\n",
			0.5 * (x2DC + x1DC), 0.5 * (y2DC + y1DC), 0.5 * (x2DC - x1DC), 0.5 * (y2DC - y1DC),
			2.0 / (x2DC - x1DC), 2.0 / (y2DC - y1DC));
		psRevertLine (this);
	}
}

void structGraphicsScreen :: v_arc (long xDC, long yDC, long rDC, double fromAngle, double toAngle) {
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;
		cairoPrepareLine (this);
		cairo_new_path (d_cairoGraphicsContext);
		cairo_arc (d_cairoGraphicsContext, xDC, yDC, rDC, -toAngle * (M_PI / 180.0), -fromAngle * (M_PI / 180.0));
		cairo_stroke (d_cairoGraphicsContext);
		cairoRevertLine (this);
	#elif win
		int arcAngle = (int) toAngle - (int) fromAngle;
		POINT pt;
		if (arcAngle < 0.0) arcAngle += 360;
		winPrepareLine (this);
		MoveToEx (d_gdiGraphicsContext, xDC + rDC * cos (NUMpi / 180 * fromAngle), yDC - rDC * sin (NUMpi / 180 * fromAngle), & pt);
		AngleArc (d_gdiGraphicsContext, xDC, yDC, rDC, fromAngle, arcAngle);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareLine (this);
		CGContextBeginPath (d_macGraphicsContext);
		CGContextAddArc (d_macGraphicsContext, xDC, yDC, rDC, NUM2pi - NUMpi / 180 * toAngle, NUM2pi - NUMpi / 180 * fromAngle, 0);
		CGContextStrokePath (d_macGraphicsContext);
		quartzRevertLine (this);
		GraphicsQuartz_exitDraw (this);
	#endif
}

void structGraphicsPostscript :: v_arc (long xDC, long yDC, long rDC, double fromAngle, double toAngle) {
	psPrepareLine (this);
	d_printf (d_file, "N %ld %ld %ld %f %f arc stroke\n", xDC, yDC, rDC, fromAngle, toAngle);
	psRevertLine (this);
}

/* Third level. */

void structGraphicsScreen :: v_fillCircle (long xDC, long yDC, long rDC) {
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;
		cairo_new_path (d_cairoGraphicsContext);
		cairo_arc (d_cairoGraphicsContext, xDC, yDC, rDC, 0, 2 * M_PI);
		cairo_fill (d_cairoGraphicsContext);
	#elif win
		MY_BRUSH
		/*
		 * NT cannot fill circles that span less than five pixels...
		 */
		Ellipse (d_gdiGraphicsContext, xDC - rDC - 1, yDC - rDC - 1, xDC + rDC + 1, yDC + rDC + 1);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareFill (this);
		CGContextBeginPath (d_macGraphicsContext);
		CGContextAddArc (d_macGraphicsContext, xDC, yDC, rDC, 0.0, NUM2pi, 0);
		CGContextFillPath (d_macGraphicsContext);
		GraphicsQuartz_exitDraw (this);
	#else
		v_circle (xDC, yDC, rDC);
	#endif
}

void structGraphicsPostscript :: v_fillCircle (long xDC, long yDC, long rDC) {
	d_printf (d_file, "N %ld %ld %ld FC\n", xDC, yDC, rDC);
}

void structGraphicsScreen :: v_fillEllipse (long x1DC, long x2DC, long y1DC, long y2DC) {
	ORDER_DC
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;	
		cairo_new_path (d_cairoGraphicsContext);
		cairo_save (d_cairoGraphicsContext);
		cairo_translate (d_cairoGraphicsContext, 0.5 * (x2DC + x1DC), 0.5 * (y2DC + y1DC));
		cairo_scale (d_cairoGraphicsContext, 0.5 * (x2DC - x1DC), 0.5 * (y2DC - y1DC));
		cairo_arc (d_cairoGraphicsContext, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
		cairo_restore (d_cairoGraphicsContext);
		cairo_fill (d_cairoGraphicsContext);
	#elif win
		MY_BRUSH
		Ellipse (d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y1DC + 1);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareFill (this);
		CGContextBeginPath (d_macGraphicsContext);
		CGContextSaveGState (d_macGraphicsContext);
        NSCAssert(d_macGraphicsContext, @"nil context");

		CGContextTranslateCTM (d_macGraphicsContext, 0.5 * (x2DC + x1DC), 0.5 * (y2DC + y1DC));
		CGContextScaleCTM (d_macGraphicsContext, 0.5 * (x2DC - x1DC), 0.5 * (y2DC - y1DC));
		CGContextAddArc (d_macGraphicsContext, 0.0, 0.0, 1.0, 0.0, NUM2pi, 0);
		CGContextScaleCTM (d_macGraphicsContext, 2.0 / (x2DC - x1DC), 2.0 / (y2DC - y1DC));
		CGContextFillPath (d_macGraphicsContext);
		CGContextRestoreGState (d_macGraphicsContext);
		GraphicsQuartz_exitDraw (this);
	#else
		v_ellipse (x1DC, x2DC, y1DC, y2DC);
	#endif
}

void structGraphicsPostscript :: v_fillEllipse (long x1DC, long x2DC, long y1DC, long y2DC) {
	d_printf (d_file, "gsave %ld %ld translate %ld %ld scale N 0 0 1 FC grestore\n",
		(x2DC + x1DC) / 2, (y2DC + y1DC) / 2, (x2DC - x1DC) / 2, (y2DC - y1DC) / 2);
}

void structGraphicsScreen :: v_button (long x1DC, long x2DC, long y1DC, long y2DC) {
	ORDER_DC
	#if cairo
		if (x2DC <= x1DC || y1DC <= y2DC) return;
		
		cairo_save (d_cairoGraphicsContext);
		if (d_drawingArea && 0) {
			// clip to drawing area
			int w, h;
			gdk_drawable_get_size (d_window, & w, & h);
			cairo_rectangle (d_cairoGraphicsContext, 0, 0, w, h);
			cairo_clip (d_cairoGraphicsContext);
		}
		cairo_set_line_width (d_cairoGraphicsContext, 1.0);
		double left = x1DC - 0.5, right = x2DC - 0.5, top = y2DC + 0.5, bottom = y1DC + 0.5;
		double width = right - left, height = bottom - top;
		cairo_set_source_rgb (d_cairoGraphicsContext, 0.1, 0.1, 0.1);   // dark grey
		cairo_rectangle (d_cairoGraphicsContext, left, top, width, height);
		cairo_stroke (d_cairoGraphicsContext);
		
		left ++, right --, top ++, bottom --, width -= 2, height -= 2;
		if (width > 0 && height > 0) {
			cairo_set_source_rgb (d_cairoGraphicsContext, 0.3, 0.3, 0.3);
			cairo_move_to (d_cairoGraphicsContext, left + 1, bottom);
			cairo_line_to (d_cairoGraphicsContext, right, bottom);
			cairo_line_to (d_cairoGraphicsContext, right, top + 1);
			cairo_stroke (d_cairoGraphicsContext);
			
			cairo_set_source_rgb (d_cairoGraphicsContext, 1.0, 1.0, 1.0);
			cairo_move_to (d_cairoGraphicsContext, left, bottom);
			cairo_line_to (d_cairoGraphicsContext, left, top);
			cairo_line_to (d_cairoGraphicsContext, right, top);
			cairo_stroke (d_cairoGraphicsContext);
			left += 0.5, right -= 0.5, top += 0.5, bottom -= 0.5, width -= 1, height -= 1;
			if (width > 0 && height > 0) {
				cairo_set_source_rgb (d_cairoGraphicsContext, 0.65, 0.65, 0.65);
				cairo_rectangle (d_cairoGraphicsContext, left, top, width, height);
				cairo_fill (d_cairoGraphicsContext);
			}
		}
		cairo_restore (d_cairoGraphicsContext);
	#elif mac
        int width = x2DC - x1DC, height = y1DC - y2DC;
		if (width <= 0 || height <= 0) return;

		#define SetRect(r, left, top, right, bottom) r.origin.x = left; r.origin.y = top; r.size.width = right - left; r.size.height = bottom - top;
    
		GraphicsQuartz_initDraw (this);
		CGContextSetLineWidth (d_macGraphicsContext, 1.0);
		CGContextSetAllowsAntialiasing (d_macGraphicsContext, false);   // because we want to draw by pixel
        CGFloat gray = 0.1;
        CGContextSetRGBStrokeColor (d_macGraphicsContext, gray, gray, gray, 1.0);
        CGRect rect;
        SetRect (rect, x1DC - 1, y2DC, x2DC + 1, y1DC);
        CGContextAddRect (d_macGraphicsContext, rect);
        CGContextStrokePath (d_macGraphicsContext);
		if (width > 2 && height > 2) {
			gray = 0.3;
			CGContextSetRGBStrokeColor (d_macGraphicsContext, gray, gray, gray, 1.0);
			CGContextMoveToPoint (d_macGraphicsContext, x1DC, y1DC - 1);
			CGContextAddLineToPoint (d_macGraphicsContext, x2DC - 2, y1DC - 1);
			CGContextMoveToPoint (d_macGraphicsContext, x2DC - 2, y1DC - 1);
			CGContextAddLineToPoint (d_macGraphicsContext, x2DC - 2, y2DC);
			CGContextStrokePath (d_macGraphicsContext);
			gray = 1.0;
			CGContextSetRGBStrokeColor (d_macGraphicsContext, gray, gray, gray, 1.0);
			CGContextMoveToPoint (d_macGraphicsContext, x1DC, y1DC - 1);
			CGContextAddLineToPoint (d_macGraphicsContext, x1DC, y2DC + 1);
			CGContextMoveToPoint (d_macGraphicsContext, x1DC, y2DC + 1);
            CGContextAddLineToPoint (d_macGraphicsContext, x2DC - 2, y2DC + 1);
            CGContextStrokePath (d_macGraphicsContext);
			if (width > 4 && height > 4) {
				gray = 0.65;
				CGContextSetRGBFillColor (d_macGraphicsContext, gray, gray, gray, 1.0);
				SetRect (rect, x1DC + 1, y2DC + 1, x2DC - 4, y1DC - 4);
				CGContextFillRect (d_macGraphicsContext, rect);
            }
        }
		CGContextSetAllowsAntialiasing (d_macGraphicsContext, true);
		CGContextSetLineDash (d_macGraphicsContext, 0, NULL, 0);
		GraphicsQuartz_exitDraw (this);
    #elif win
        RECT rect;
        rect. left = x1DC, rect. right = x2DC, rect. top = y2DC, rect. bottom = y1DC;
        DrawEdge (d_gdiGraphicsContext, & rect, EDGE_RAISED, BF_RECT);
        SelectPen (d_gdiGraphicsContext, GetStockPen (NULL_PEN));
        SelectBrush (d_gdiGraphicsContext, GetStockBrush (LTGRAY_BRUSH));
        Rectangle (d_gdiGraphicsContext, x1DC + 1, y2DC + 1, x2DC - 1, y1DC - 1);
        SelectPen (d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
        SelectBrush (d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
	#endif
}

void structGraphics :: v_roundedRectangle (long x1DC, long x2DC, long y1DC, long y2DC, long r) {
	long dy = yIsZeroAtTheTop ? - r : r, xyDC [4];
	ORDER_DC
	xyDC [0] = x1DC + r;
	xyDC [1] = y1DC;
	xyDC [2] = x2DC - r;
	xyDC [3] = y1DC;
	v_polyline (2, xyDC, false);
	v_arc (x2DC - r, y1DC + dy, r, -90, 0);
	xyDC [0] = x2DC;
	xyDC [1] = y1DC + dy;
	xyDC [2] = x2DC;
	xyDC [3] = y2DC - dy;
	v_polyline (2, xyDC, false);
	v_arc (x2DC - r, y2DC - dy, r, 0, 90);
	xyDC [0] = x2DC - r;
	xyDC [1] = y2DC;
	xyDC [2] = x1DC + r;
	xyDC [3] = y2DC;
	v_polyline (2, xyDC, false);
	v_arc (x1DC + r, y2DC - dy, r, 90, 180);
	xyDC [0] = x1DC;
	xyDC [1] = y2DC - dy;
	xyDC [2] = x1DC;
	xyDC [3] = y1DC + dy;
	v_polyline (2, xyDC, false);
	v_arc (x1DC + r, y1DC + dy, r, 180, 270);
}

void structGraphicsScreen :: v_roundedRectangle (long x1DC, long x2DC, long y1DC, long y2DC, long r) {
	#if win
		long dy = yIsZeroAtTheTop ? - r : r, xyDC [4];
		ORDER_DC
		winPrepareLine (this);
		RoundRect (d_gdiGraphicsContext, x1DC, y2DC, x2DC + 1, y1DC + 1, r + r, r + r);
		DEFAULT
		return;
	#else
		GraphicsScreen_Parent :: v_roundedRectangle (x1DC, x2DC, y1DC, y2DC, r);
	#endif
}

/* Fourth level. */

void structGraphics :: v_fillRoundedRectangle (long x1DC, long x2DC, long y1DC, long y2DC, long r) {
	long dy = yIsZeroAtTheTop ? - r : r;
	ORDER_DC
	v_fillCircle (x2DC - r, y1DC + dy, r);
	v_fillCircle (x2DC - r, y2DC - dy, r);
	v_fillCircle (x1DC + r, y2DC - dy, r);
	v_fillCircle (x1DC + r, y1DC + dy, r);
	v_fillRectangle (x1DC, x2DC, y1DC + dy, y2DC - dy);
	v_fillRectangle (x1DC + r, x2DC - r, y1DC, y2DC);
}

/* Fifth level. */

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void Graphics_polyline (Graphics me, long numberOfPoints, double *xWC, double *yWC) {   // base 0
	if (numberOfPoints == 0) return;
	long *xyDC;
	try {
		xyDC = Melder_malloc (long, 2 * numberOfPoints);
	} catch (MelderError) {
		/*
		 * Out of memory: silently refuse to draw.
		 */
		Melder_clearError ();
		return;
	}
	for (long i = 0; i < numberOfPoints; i ++) {
		xyDC [i + i] = wdx (xWC [i]);
		xyDC [i + i + 1] = wdy (yWC [i]);
	}
	my v_polyline (numberOfPoints, xyDC, false);
	Melder_free (xyDC);
	if (my recording) {
		op (POLYLINE, 1 + 2 * numberOfPoints);
		put (numberOfPoints);
		mput (numberOfPoints, & xWC [0])
		mput (numberOfPoints, & yWC [0])
	}
}

void Graphics_polyline_closed (Graphics me, long numberOfPoints, double *xWC, double *yWC) {   // base 0
	if (numberOfPoints == 0) return;
	long *xyDC;
	try {
		xyDC = Melder_malloc (long, 2 * numberOfPoints);
	} catch (MelderError) {
		/*
		 * Out of memory: silently refuse to draw.
		 */
		Melder_clearError ();
		return;
	}
	for (long i = 0; i < numberOfPoints; i ++) {
		xyDC [i + i] = wdx (xWC [i]);
		xyDC [i + i + 1] = wdy (yWC [i]);
	}
	my v_polyline (numberOfPoints, xyDC, true);
	Melder_free (xyDC);
	if (my recording) {
		op (POLYLINE_CLOSED, 1 + 2 * numberOfPoints);
		put (numberOfPoints);
		mput (numberOfPoints, & xWC [0])
		mput (numberOfPoints, & yWC [0])
	}
}

void Graphics_line (Graphics me, double x1WC, double y1WC, double x2WC, double y2WC) {
	long xyDC [4];
	xyDC [0] = wdx (x1WC);
	xyDC [1] = wdy (y1WC);
	xyDC [2] = wdx (x2WC);
	xyDC [3] = wdy (y2WC);
	my v_polyline (2, xyDC, false);
	if (my recording) { op (LINE, 4); put (x1WC); put (y1WC); put (x2WC); put (y2WC); }
}

void Graphics_fillArea (Graphics me, long numberOfPoints, double *xWC, double *yWC) {
	long *xyDC;
	try {
		xyDC = Melder_malloc (long, 2 * numberOfPoints);
	} catch (MelderError) {
		/*
		 * Out of memory: silently refuse to draw.
		 */
		Melder_clearError ();
		return;
	}
	for (long i = 0; i < numberOfPoints; i ++) {
		xyDC [i + i] = wdx (xWC [i]);
		xyDC [i + i + 1] = wdy (yWC [i]);
	}
	my v_fillArea (numberOfPoints, xyDC);
	Melder_free (xyDC);
	if (my recording) {
		op (FILL_AREA, 1 + 2 * numberOfPoints);
		put (numberOfPoints);
		mput (numberOfPoints, & xWC [0])
		mput (numberOfPoints, & yWC [0])
	}
}

#define MACRO_Graphics_function(TYPE) \
	long x1DC, x2DC; \
	long clipy1 = wdy (my d_y1WC), clipy2 = wdy (my d_y2WC); \
	double dx, offsetX, translation, scale; \
	long i, n = ix2 - ix1 + 1; \
 \
	if (ix2 <= ix1 || my scaleX == 0.0) return; \
 \
	dx = (x2WC - x1WC) / (n - 1); \
	offsetX = x1WC - ix1 * dx; \
	/* xDC = wdx (offsetX + i * dx) */ \
	translation = wdx (offsetX); \
	scale = dx * my scaleX; \
	x1DC = translation + ix1 * scale; \
	x2DC = translation + ix2 * scale; \
	if (n > (x2DC - x1DC + 1) * 2) {  /* Optimize: draw one vertical line for each device x coordinate. */ \
		long numberOfPixels = x2DC - x1DC + 1, k = 0; \
		long numberOfPointsActuallyDrawn = numberOfPixels * 2; \
		long *xyDC; \
		TYPE lastMini; \
		if (numberOfPointsActuallyDrawn < 1) return; \
		xyDC = Melder_malloc_f (long, 2 * numberOfPointsActuallyDrawn); \
		for (i = 0; i < numberOfPixels; i ++) { \
			long j, jmin = ix1 + i / scale, jmax = ix1 + (i + 1) / scale; \
			TYPE mini, maxi; \
			long minDC, maxDC; \
			if (jmin > ix2) jmin = ix2; \
			if (jmax > ix2) jmax = ix2; \
			mini = yWC [STAGGER (jmin)], maxi = mini; \
			for (j = jmin + 1; j <= jmax; j ++) {   /* One point overlap. */ \
				TYPE value = yWC [STAGGER (j)]; \
				if (value > maxi) maxi = value; \
				else if (value < mini) mini = value; \
			} \
			minDC = wdy (mini); \
			maxDC = wdy (maxi); \
			if (my yIsZeroAtTheTop) { \
				if (minDC > clipy1) minDC = clipy1; \
				if (maxDC > clipy1) maxDC = clipy1; \
				if (maxDC < clipy2) maxDC = clipy2; \
				if (minDC < clipy2) minDC = clipy2; \
			} else { \
				if (minDC < clipy1) minDC = clipy1; \
				if (maxDC < clipy1) maxDC = clipy1; \
				if (maxDC > clipy2) maxDC = clipy2; \
				if (minDC > clipy2) minDC = clipy2; \
			} \
			if (i == 0) { \
				if (yWC [STAGGER (jmin)] < yWC [STAGGER (jmax)]) { \
					xyDC [k ++] = x1DC; \
					xyDC [k ++] = minDC; \
					xyDC [k ++] = x1DC; \
					xyDC [k ++] = maxDC; \
				} else { \
					xyDC [k ++] = x1DC; \
					xyDC [k ++] = maxDC; \
					xyDC [k ++] = x1DC; \
					xyDC [k ++] = minDC; \
				} \
			} else if (minDC == xyDC [k - 1]) { \
				xyDC [k ++] = x1DC + i; \
				xyDC [k ++] = maxDC; \
			} else if (maxDC == xyDC [k - 1]) { \
				xyDC [k ++] = x1DC + i; \
				xyDC [k ++] = minDC; \
			} else if (mini > lastMini) { \
				xyDC [k ++] = x1DC + i; \
				xyDC [k ++] = minDC; \
				xyDC [k ++] = x1DC + i; \
				xyDC [k ++] = maxDC; \
			} else { \
				xyDC [k ++] = x1DC + i; \
				xyDC [k ++] = maxDC; \
				xyDC [k ++] = x1DC + i; \
				xyDC [k ++] = minDC; \
			} \
			lastMini = mini; \
		} \
		if (k > 1) my v_polyline (k / 2, xyDC, false); \
		Melder_free (xyDC); \
	} else {  /* Normal. */  \
		long *xyDC = Melder_malloc_f (long, 2 * n); \
		for (i = 0; i < n; i ++) { \
			long ix = ix1 + i; \
			long value = wdy (yWC [STAGGER (ix)]); \
			xyDC [i + i] = translation + ix * scale; \
			if (my yIsZeroAtTheTop) { \
				if (FUNCTIONS_ARE_CLIPPED && value > clipy1) value = clipy1; \
				if (FUNCTIONS_ARE_CLIPPED && value < clipy2) value = clipy2; \
			} else { \
				if (FUNCTIONS_ARE_CLIPPED && value < clipy1) value = clipy1; \
				if (FUNCTIONS_ARE_CLIPPED && value > clipy2) value = clipy2; \
			} \
			xyDC [i + i + 1] = value; \
		} \
		my v_polyline (n, xyDC, false); \
		Melder_free (xyDC); \
	}

void Graphics_function (Graphics me, double yWC [], long ix1, long ix2, double x1WC, double x2WC) {
	#define STAGGER(i)  (i)
	MACRO_Graphics_function (double)
	#undef STAGGER
	if (my recording) { op (FUNCTION, 3 + n); put (n); put (x1WC); put (x2WC); mput (n, & yWC [ix1]) }
}

void Graphics_function16 (Graphics me, int16_t yWC [], int stagger, long ix1, long ix2, double x1WC, double x2WC) {
	if (stagger == 1) {
		#define STAGGER(i)  ((i) + (i))
		MACRO_Graphics_function (int16_t)
		#undef STAGGER
	} else if (stagger > 1) {
		#define STAGGER(i)  ((stagger + 1) * (i))
		MACRO_Graphics_function (int16_t)
		#undef STAGGER
	} else {
		#define STAGGER(i)  (i)
		MACRO_Graphics_function (int16_t)
		#undef STAGGER
	}
}

void Graphics_rectangle (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	my v_rectangle (wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
	if (my recording) { op (RECTANGLE, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

void Graphics_fillRectangle (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	my v_fillRectangle (wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
	if (my recording) { op (FILL_RECTANGLE, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

void Graphics_roundedRectangle (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC, double r_mm) {
	my v_roundedRectangle (wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), ceil (r_mm * my resolution / 25.4));
	if (my recording) { op (ROUNDED_RECTANGLE, 5); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (r_mm); }
}

void Graphics_fillRoundedRectangle (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC, double r_mm) {
	my v_fillRoundedRectangle (wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC), ceil (r_mm * my resolution / 25.4));
	if (my recording) { op (FILL_ROUNDED_RECTANGLE, 5); put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (r_mm); }
}

void Graphics_button (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	my v_button (wdx (x1WC), wdx (x2WC), wdy (y1WC), wdy (y2WC));
	if (my recording) { op (BUTTON, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

void Graphics_innerRectangle (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	int dy = my yIsZeroAtTheTop ? -1 : 1;
	my v_rectangle (wdx (x1WC) + 1, wdx (x2WC) - 1, wdy (y1WC) + dy, wdy (y2WC) - dy);
	if (my recording) { op (INNER_RECTANGLE, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

void Graphics_circle (Graphics me, double xWC, double yWC, double rWC) {
	my v_circle (wdx (xWC), wdy (yWC), my scaleX * rWC);
	if (my recording) { op (CIRCLE, 3); put (xWC); put (yWC); put (rWC); }
}

void Graphics_circle_mm (Graphics me, double xWC, double yWC, double diameter) {
	my v_circle (wdx (xWC), wdy (yWC), 0.5 * diameter * my resolution / 25.4);
	if (my recording) { op (CIRCLE_MM, 3); put (xWC); put (yWC); put (diameter); }
}

void Graphics_fillCircle (Graphics me, double xWC, double yWC, double rWC) {
	my v_fillCircle (wdx (xWC), wdy (yWC), ceil (my scaleX * rWC));
	if (my recording) { op (FILL_CIRCLE, 3); put (xWC); put (yWC); put (rWC); }
}

void Graphics_fillCircle_mm (Graphics me, double xWC, double yWC, double diameter) {
	my v_fillCircle (wdx (xWC), wdy (yWC), ceil (0.5 * diameter * my resolution / 25.4));
	if (my recording) { op (FILL_CIRCLE_MM, 3); put (xWC); put (yWC); put (diameter); }
}

void Graphics_rectangle_mm (Graphics me, double xWC, double yWC, double horSide, double vertSide) {
	long xDC = wdx (xWC), yDC = wdy (yWC);
	long halfHorSide = ceil (0.5 * horSide * my resolution / 25.4);
	long halfVertSide = ceil (0.5 * vertSide * my resolution / 25.4);
	if (my yIsZeroAtTheTop) {
		my v_rectangle (xDC - halfHorSide, xDC + halfHorSide, yDC + halfVertSide, yDC - halfVertSide);
	} else {
		my v_rectangle (xDC - halfHorSide, xDC + halfHorSide, yDC - halfVertSide, yDC + halfVertSide);
	}
	if (my recording) { op (RECTANGLE_MM, 4); put (xWC); put (yWC); put (horSide); put (vertSide); }
}

void Graphics_fillRectangle_mm (Graphics me, double xWC, double yWC, double horSide, double vertSide) {
	long xDC = wdx (xWC), yDC = wdy (yWC);
	long halfHorSide = ceil (0.5 * horSide * my resolution / 25.4);
	long halfVertSide = ceil (0.5 * vertSide * my resolution / 25.4);
	if (my yIsZeroAtTheTop) {
		my v_fillRectangle (xDC - halfHorSide, xDC + halfHorSide, yDC + halfVertSide, yDC - halfVertSide);
	} else {
		my v_fillRectangle (xDC - halfHorSide, xDC + halfHorSide, yDC - halfVertSide, yDC + halfVertSide);
	}
	if (my recording) { op (FILL_RECTANGLE_MM, 4); put (xWC); put (yWC); put (horSide); put (vertSide); }
}

void Graphics_ellipse (Graphics me, double x1, double x2, double y1, double y2) {
	my v_ellipse (wdx (x1), wdx (x2), wdy (y1), wdy (y2));
	if (my recording) { op (ELLIPSE, 4); put (x1); put (x2); put (y1); put (y2); }
}

void Graphics_fillEllipse (Graphics me, double x1, double x2, double y1, double y2) {
	my v_fillEllipse (wdx (x1), wdx (x2), wdy (y1), wdy (y2));
	if (my recording) { op (FILL_ELLIPSE, 4); put (x1); put (x2); put (y1); put (y2); }
}

void Graphics_arc (Graphics me, double xWC, double yWC, double rWC, double fromAngle, double toAngle) {
	my v_arc (wdx (xWC), wdy (yWC), my scaleX * rWC, fromAngle, toAngle);
	if (my recording) { op (ARC, 5); put (xWC); put (yWC); put (rWC); put (fromAngle); put (toAngle); }
}

void Graphics_fillArc (Graphics me, double xWC, double yWC, double rWC, double fromAngle, double toAngle) {
	my v_arc (wdx (xWC), wdy (yWC), my scaleX * rWC, fromAngle, toAngle);   // NYI v_fillArc
	if (my recording) { op (FILL_ARC, 5); put (xWC); put (yWC); put (rWC); put (fromAngle); put (toAngle); }
}

/* Arrows. */

void structGraphics :: v_arrowHead (long xDC, long yDC, double angle) {
	(void) xDC;
	(void) yDC;
	(void) angle;
}

void structGraphicsScreen :: v_arrowHead (long xDC, long yDC, double angle) {
	#if cairo
		if (d_cairoGraphicsContext == NULL) return;
		double size = 10.0 * resolution * arrowSize / 75.0; // TODO: die 75 zou dat niet de scherm resolutie moeten worden?
		cairo_new_path (d_cairoGraphicsContext);
		cairo_move_to (d_cairoGraphicsContext, xDC + cos ((angle + 160) * NUMpi / 180) * size, yDC - sin ((angle + 160) * NUMpi / 180) * size);
		cairo_line_to (d_cairoGraphicsContext, xDC, yDC);
		cairo_line_to (d_cairoGraphicsContext, xDC + cos ((angle - 160) * NUMpi / 180) * size, yDC - sin ((angle - 160) * NUMpi / 180) * size);
		cairo_close_path (d_cairoGraphicsContext);
		cairo_fill (d_cairoGraphicsContext);
	#elif win
		double size = 10.0 * resolution * arrowSize / 72.0;
		MY_BRUSH
		BeginPath (d_gdiGraphicsContext);
		MoveToEx (d_gdiGraphicsContext, xDC + cos ((angle + 160) * NUMpi / 180) * size, yDC - sin ((angle + 160) * NUMpi / 180) * size, NULL);
		LineTo (d_gdiGraphicsContext, xDC, yDC);
		LineTo (d_gdiGraphicsContext, xDC + cos ((angle - 160) * NUMpi / 180) * size, yDC - sin ((angle - 160) * NUMpi / 180) * size);
		EndPath (d_gdiGraphicsContext);
		FillPath (d_gdiGraphicsContext);
		DEFAULT
	#elif mac
		GraphicsQuartz_initDraw (this);
		quartzPrepareFill (this);
		CGContextSaveGState (d_macGraphicsContext);
		CGContextBeginPath (d_macGraphicsContext);
    NSCAssert( d_macGraphicsContext, @"nil context");

		CGContextTranslateCTM (d_macGraphicsContext, xDC, yDC);
		CGContextRotateCTM (d_macGraphicsContext, - angle * NUMpi / 180);
		CGContextMoveToPoint (d_macGraphicsContext, 0.0, 0.0);
		double size = 10.0 * resolution * arrowSize / 72.0;
		double radius = resolution * arrowSize / 30;
		CGContextAddArc (d_macGraphicsContext, - size, 0.0, radius, - NUMpi / 3.0, NUMpi / 3.0, 0);
		CGContextAddLineToPoint (d_macGraphicsContext, 0.0, 0.0);
		CGContextFillPath (d_macGraphicsContext);
		CGContextRestoreGState (d_macGraphicsContext);
		GraphicsQuartz_exitDraw (this);
	#endif
}

void structGraphicsPostscript :: v_arrowHead (long xDC, long yDC, double angle) {
	long length = resolution * arrowSize / 10, radius = resolution * arrowSize / 30;
	d_printf (d_file, "gsave %ld %ld translate %f rotate\n"
		"N 0 0 M -%ld 0 %ld -60 60 arc closepath fill grestore\n", xDC, yDC, angle, length, radius);
}

void Graphics_arrow (Graphics me, double x1WC, double y1WC, double x2WC, double y2WC) {
	double angle = (180.0 / NUMpi) * atan2 ((wdy (y2WC) - wdy (y1WC)) * (my yIsZeroAtTheTop ? -1 : 1), wdx (x2WC) - wdx (x1WC));
	double size = my screen ? 10.0 * my resolution * my arrowSize / 72.0 : my resolution * my arrowSize / 10;
	long xyDC [4];
	xyDC [0] = wdx (x1WC);
	xyDC [1] = wdy (y1WC);
	xyDC [2] = wdx (x2WC) + (my screen ? 0.7 : 0.6) * cos ((angle - 180) * NUMpi / 180) * size;
	xyDC [3] = wdy (y2WC) + (my yIsZeroAtTheTop ? -1.0 : 1.0) * (my screen ? 0.7 : 0.6) * sin ((angle - 180) * NUMpi / 180) * size;
	my v_polyline (2, xyDC, false);
	my v_arrowHead (wdx (x2WC), wdy (y2WC), angle);
	if (my recording) { op (ARROW, 4); put (x1WC); put (y1WC); put (x2WC); put (y2WC); }
}

void Graphics_doubleArrow (Graphics me, double x1WC, double y1WC, double x2WC, double y2WC) {
	double angle = (180.0 / NUMpi) * atan2 ((wdy (y2WC) - wdy (y1WC)) * (my yIsZeroAtTheTop ? -1 : 1), wdx (x2WC) - wdx (x1WC));
	double size = my screen ? 10.0 * my resolution * my arrowSize / 72.0 : my resolution * my arrowSize / 10;
	long xyDC [4];
	xyDC [0] = wdx (x1WC) + (my screen ? 0.7 : 0.6) * cos (angle * NUMpi / 180) * size;
	xyDC [1] = wdy (y1WC) + (my yIsZeroAtTheTop ? -1.0 : 1.0) * (my screen ? 0.7 : 0.6) * sin (angle * NUMpi / 180) * size;
	xyDC [2] = wdx (x2WC) + (my screen ? 0.7 : 0.6) * cos ((angle - 180) * NUMpi / 180) * size;
	xyDC [3] = wdy (y2WC) + (my yIsZeroAtTheTop ? -1.0 : 1.0) * (my screen ? 0.7 : 0.6) * sin ((angle - 180) * NUMpi / 180) * size;
	my v_polyline (2, xyDC, false);
	my v_arrowHead (wdx (x1WC), wdy (y1WC), angle + 180);
	//my v_polyline (2, xyDC);
	my v_arrowHead (wdx (x2WC), wdy (y2WC), angle);
	if (my recording) { op (DOUBLE_ARROW, 4); put (x1WC); put (y1WC); put (x2WC); put (y2WC); }
}

void Graphics_arcArrow (Graphics me, double xWC, double yWC, double rWC,
	double fromAngle, double toAngle, int arrowAtStart, int arrowAtEnd)
{
	my v_arc (wdx (xWC), wdy (yWC), my scaleX * rWC, fromAngle, toAngle);
	if (arrowAtStart) my v_arrowHead (
		wdx (xWC + rWC * cos (fromAngle * (NUMpi / 180.0))),
		wdy (yWC + rWC * sin (fromAngle * (NUMpi / 180.0))), fromAngle - 90);
	if (arrowAtEnd) my v_arrowHead (
		wdx (xWC + rWC * cos (toAngle * (NUMpi / 180.0))),
		wdy (yWC + rWC * sin (toAngle * (NUMpi / 180.0))), toAngle + 90);
	if (my recording)
		{ op (ARC_ARROW, 7); put (xWC); put (yWC); put (rWC);
		  put (fromAngle); put (toAngle); put (arrowAtStart); put (arrowAtEnd); }
}

/* Output attributes. */

void Graphics_setLineType (Graphics me, int lineType) {
	my lineType = lineType;
	if (my recording) { op (SET_LINE_TYPE, 1); put (lineType); }
}

void Graphics_setLineWidth (Graphics me, double lineWidth) {
	my lineWidth = lineWidth;
	if (my recording) { op (SET_LINE_WIDTH, 1); put (lineWidth); }
}

void Graphics_setArrowSize (Graphics me, double arrowSize) {
	my arrowSize = arrowSize;
	if (my recording) { op (SET_ARROW_SIZE, 1); put (arrowSize); }
}

/* Inquiries. */

int Graphics_inqLineType (Graphics me) { return my lineType; }
double Graphics_inqLineWidth (Graphics me) { return my lineWidth; }
double Graphics_inqArrowSize (Graphics me) { return my arrowSize; }

/* End of file Graphics_linesAndAreas.cpp */
