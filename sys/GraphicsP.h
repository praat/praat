#ifndef _GraphicsP_h_
#define _GraphicsP_h_
/* GraphicsP.h
 *
 * Copyright (C) 1992-2020,2022,2023 Paul Boersma, 2013 Tom Naughton
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

#include "Graphics.h"
#include "Gui.h"

#if defined (_WIN32)
	#include <windowsx.h>
	#include <gdiplus.h>
#endif

void Graphics_init (Graphics me, int resolution);
/*
	Postconditions:
		my font == Graphics_FONT_HELVETICA;
		my fontSize == 9;
		my fontStyle == Graphics_NORMAL;
*/ 

#define kGraphics_font_SYMBOL  ((int) kGraphics_font::MAX + 1)
#define kGraphics_font_IPATIMES  ((int) kGraphics_font::MAX + 2)
#define kGraphics_font_IPAPALATINO  ((int) kGraphics_font::MAX + 3)
#define kGraphics_font_CHEROKEE  ((int) kGraphics_font::MAX + 4)
#define kGraphics_font_DINGBATS  ((int) kGraphics_font::MAX + 5)
#define kGraphics_font_CHINESE  ((int) kGraphics_font::MAX + 6)
#define kGraphics_font_JAPANESE  ((int) kGraphics_font::MAX + 7)

/*
	Honour the NO_GRAPHICS compiler switch.
*/
#if defined (NO_GRAPHICS)
	#define cairo 0
	#define gdi 0
	#define direct2d 0
	#define quartz 0
#elif defined (UNIX)   /* include graphics even if there is no GUI, in order to be able to save PNG files */
	#define cairo 1   /* Cairo, including Pango */
	#define gdi 0
	#define direct2d 0
	#define quartz 0
#elif motif
	#define cairo 0
	#define gdi 1   /* to be discontinued when we no longer have to support Windows XP */
	#define direct2d 0   /* for the future: Direct2D, including DirectWrite */
	#define quartz 0
#elif cocoa
	#define cairo 0
	#define gdi 0
	#define direct2d 0
	#define quartz 1   /* Quartz, including CoreText */
#else
	/*
		Unknown platforms have no graphics.
	*/
	#define cairo 0
	#define gdi 0
	#define direct2d 0
	#define quartz 0
#endif

Thing_define (GraphicsScreen, Graphics) {
	bool d_isPng;
	structMelderFile d_file;
	#if defined (NO_GRAPHICS)
	#elif cairo
		#if gtk
			GdkDisplay *d_display;
			GdkWindow *d_window;
		#endif
		cairo_surface_t *d_cairoSurface;
		cairo_t *d_cairoGraphicsContext;
	#elif gdi
		HWND d_winWindow;
		HDC d_gdiGraphicsContext;
		COLORREF d_winForegroundColour;
		HPEN d_winPen;
		HBRUSH d_winBrush;
		bool d_fatNonSolid;
		bool d_useGdiplus;
		HBITMAP d_gdiBitmap;
		Gdiplus::Bitmap *d_gdiplusBitmap;
	#elif quartz
		NSView *d_macView;
		int d_macFont, d_macStyle;
		int d_depth;
		uint8 *d_bits;
		CGContextRef d_macGraphicsContext;
	#endif

	void v9_destroy () noexcept
		override;
	void v_polyline (integer numberOfPoints, double *xyDC, bool close)
		override;
	void v_fillArea (integer numberOfPoints, double *xyDC)
		override;
	void v_rectangle (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_fillRectangle (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_circle (double xDC, double yDC, double rDC)
		override;
	void v_ellipse (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_arc (double xDC, double yDC, double rDC, double fromAngle, double toAngle)
		override;
	void v_fillCircle (double xDC, double yDC, double rDC)
		override;
	void v_fillEllipse (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_button (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_roundedRectangle (double x1DC, double x2DC, double y1DC, double y2DC, double r)
		override;
	void v_arrowHead (double xDC, double yDC, double angle)
		override;
	void v_clearWs ()
		override;
	void v_updateWs ()
		override;
};

Thing_define (GraphicsPostscript, Graphics) {
	FILE *d_file;
	int (*d_printf) (void *stream, const char *format, ...);
	int languageLevel;
	int photocopyable, spotsDensity, spotsAngle;
	bool loadedXipa, useSilipaPS, landscape, includeFonts;
	double magnification;
	char *fontInfos [1 + kGraphics_font_DINGBATS] [1 + Graphics_BOLD_ITALIC];
	const char *lastFid;
	int pageNumber, lastSize;
	bool job, eps;

	void v9_destroy () noexcept
		override;
	void v_polyline (integer numberOfPoints, double *xyDC, bool close)
		override;
	void v_fillArea (integer numberOfPoints, double *xyDC)
		override;
	void v_rectangle (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_fillRectangle (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_circle (double xDC, double yDC, double rDC)
		override;
	void v_ellipse (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_arc (double xDC, double yDC, double rDC, double fromAngle, double toAngle)
		override;
	void v_fillCircle (double xDC, double yDC, double rDC)
		override;
	void v_fillEllipse (double x1DC, double x2DC, double y1DC, double y2DC)
		override;
	void v_arrowHead (double xDC, double yDC, double angle)
		override;
};

/* Opcodes for recording. */

double * _Graphics_check (Graphics me, integer number);
#define put(f)  * ++ p = (double) (f)
#define op(opcode,number)  double *p = _Graphics_check (me, number); if (! p) return; put (opcode); put (number)
#define mput(n,a)  { const double *f = a; for (integer l = 0; l < n; l ++) put (f [l]); }
#define sput(s,l)  { put (l); strcpy ((char *) (p + 1), s); p += l; }

/* When adding opcodes in the following list, add them at the end. */
/* Otherwise, old picture files will become incompatible with the current Graphics. */
enum opcode { SET_VIEWPORT = 101, SET_INNER, UNSET_INNER, SET_WINDOW,
	/* 105 */ TEXT, POLYLINE, LINE, ARROW, FILL_AREA, FUNCTION, RECTANGLE, FILL_RECTANGLE,
	/* 113 */ CIRCLE, FILL_CIRCLE, ARC, ARC_ARROW, HIGHLIGHT, CELL_ARRAY,
	/* 119 */ SET_FONT, SET_FONT_SIZE, SET_FONT_STYLE,
	/* 122 */ SET_TEXT_ALIGNMENT, SET_TEXT_ROTATION,
	/* 124 */ SET_LINE_TYPE, SET_LINE_WIDTH,
	/* 126 */ SET_STANDARD_COLOUR, SET_GREY,
	/* 128 */ MARK_GROUP, ELLIPSE, FILL_ELLIPSE, CIRCLE_MM, FILL_CIRCLE_MM, IMAGE8,
	/* 134 */ UNHIGHLIGHT, XOR_ON, XOR_OFF, RECTANGLE_MM, FILL_RECTANGLE_MM,
	/* 139 */ SET_WS_WINDOW, SET_WRAP_WIDTH, SET_SECOND_INDENT,
	/* 142 */ SET_PERCENT_SIGN_IS_ITALIC, SET_NUMBER_SIGN_IS_BOLD,
	/* 144 */ SET_CIRCUMFLEX_IS_SUPERSCRIPT, SET_UNDERSCORE_IS_SUBSCRIPT,
	/* 146 */ SET_DOLLAR_SIGN_IS_CODE, SET_AT_SIGN_IS_LINK,
	/* 148 */ BUTTON, ROUNDED_RECTANGLE, FILL_ROUNDED_RECTANGLE, FILL_ARC,
	/* 152 */ INNER_RECTANGLE, CELL_ARRAY8, IMAGE, HIGHLIGHT2, UNHIGHLIGHT2,
	/* 157 */ SET_ARROW_SIZE, DOUBLE_ARROW, SET_RGB_COLOUR, IMAGE_FROM_FILE,
	/* 161 */ POLYLINE_CLOSED, CELL_ARRAY_COLOUR, IMAGE_COLOUR, SET_COLOUR_SCALE,
	/* 165 */ SET_SPECKLE_SIZE, SPECKLE, CLEAR_WS, SET_BACKQUOTE_IS_VERBATIM,
	/* 169 */ RECTANGLE_TEXT_WRAP_AND_TRUNCATE, RECTANGLE_TEXT_MAXIMAL_FIT
};

void _GraphicsScreen_text_init (GraphicsScreen me);
void _Graphics_fillRectangle (Graphics me, integer x1DC, integer x2DC, integer y1DC, integer y2DC);
void _Graphics_setColour (Graphics me, MelderColour colour);
void _Graphics_setGrey (Graphics me, double grey);
void _Graphics_colour_init (Graphics me);
bool _GraphicsMac_tryToInitializeFonts ();
bool _GraphicsLin_tryToInitializeFonts ();

extern enum kGraphics_cjkFontStyle theGraphicsCjkFontStyle;

/* End of file GraphicsP.h */
#endif
