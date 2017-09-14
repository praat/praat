/* Graphics.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2016,2017 Paul Boersma
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

#include <stdarg.h>
#include "GraphicsP.h"

#include "enums_getText.h"
#include "Graphics_enums.h"
#include "enums_getValue.h"
#include "Graphics_enums.h"

#include "GuiP.h"
#include "Preferences.h"

/***** Methods *****/

Thing_implement (Graphics, Thing, 0);

enum kGraphics_cjkFontStyle theGraphicsCjkFontStyle;

void Graphics_prefs () {
	Preferences_addEnum (U"Graphics.cjkFontStyle", & theGraphicsCjkFontStyle, kGraphics_cjkFontStyle, (int) kGraphics_cjkFontStyle::DEFAULT);
}

void structGraphics :: v_destroy () noexcept {
	Melder_free (record);
	Graphics_Parent :: v_destroy ();
}

static void computeTrafo (Graphics me) {
	double worldScaleX, worldScaleY, workScaleX, workScaleY;
	Melder_assert (my d_x2WC != my d_x1WC);
	worldScaleX = (my d_x2NDC - my d_x1NDC) / (my d_x2WC - my d_x1WC);
	Melder_assert (my d_y2WC != my d_y1WC);
	worldScaleY = (my d_y2NDC - my d_y1NDC) / (my d_y2WC - my d_y1WC);
	my deltaX = my d_x1NDC - my d_x1WC * worldScaleX;
	my deltaY = my d_y1NDC - my d_y1WC * worldScaleY;
	Melder_assert (my d_x2wNDC != my d_x1wNDC);
	workScaleX = (my d_x2DC - my d_x1DC) / (my d_x2wNDC - my d_x1wNDC);
	my deltaX = my d_x1DC - (my d_x1wNDC - my deltaX) * workScaleX;
	my scaleX = worldScaleX * workScaleX;
	Melder_assert (my d_y2wNDC != my d_y1wNDC);
	if (my yIsZeroAtTheTop) {
		workScaleY = ((int) my d_y1DC - (int) my d_y2DC) / (my d_y2wNDC - my d_y1wNDC);
		my deltaY = my d_y2DC - (my d_y1wNDC - my deltaY) * workScaleY;
	} else {
		workScaleY = ((int) my d_y2DC - (int) my d_y1DC) / (my d_y2wNDC - my d_y1wNDC);
		my deltaY = my d_y1DC - (my d_y1wNDC - my deltaY) * workScaleY;
	}
	my scaleY = worldScaleY * workScaleY;
}

/***** WORKSTATION FUNCTIONS *****/

void Graphics_init (Graphics me, int resolution) {
	my resolution = resolution;
	if (resolution == 96) {
		my resolutionNumber = kGraphics_resolution::DPI_96;
	} else if (resolution == 100) {
		my resolutionNumber = kGraphics_resolution::DPI_100;
	} else if (resolution == 180) {
		my resolutionNumber = kGraphics_resolution::DPI_180;
	} else if (resolution == 200) {
		my resolutionNumber = kGraphics_resolution::DPI_200;
	} else if (resolution == 300) {
		my resolutionNumber = kGraphics_resolution::DPI_300;
	} else if (resolution == 360) {
		my resolutionNumber = kGraphics_resolution::DPI_360;
	} else if (resolution == 600) {
		my resolutionNumber = kGraphics_resolution::DPI_600;
	} else if (resolution == 720) {
		my resolutionNumber = kGraphics_resolution::DPI_720;
	} else if (resolution == 900) {
		my resolutionNumber = kGraphics_resolution::DPI_900;
	} else if (resolution == 1200) {
		my resolutionNumber = kGraphics_resolution::DPI_1200;
	} else {
		Melder_fatal (U"Unsupported resolution ", resolution, U" dpi.");
	}
	my d_x1DC = my d_x1DCmin = 0;	my d_x2DC = my d_x2DCmax = 32767;
	my d_y1DC = my d_y1DCmin = 0;	my d_y2DC = my d_y2DCmax = 32767;
	my d_x1WC = my d_x1NDC = my d_x1wNDC = 0.0;
	my d_x2WC = my d_x2NDC = my d_x2wNDC = 1.0;
	my d_y1WC = my d_y1NDC = my d_y1wNDC = 0.0;
	my d_y2WC = my d_y2NDC = my d_y2wNDC = 1.0;
	computeTrafo (me);
	my lineWidth = 1.0;
	my arrowSize = 1.0;
	my speckleSize = 1.0;
	my font = kGraphics_font::HELVETICA;
	my fontSize = 10;
	my fontStyle = Graphics_NORMAL;
	my record = nullptr;
	my irecord = my nrecord = 0;
	my percentSignIsItalic = 1;
	my numberSignIsBold = 1;
	my circumflexIsSuperscript = 1;
	my underscoreIsSubscript = 1;
	my dollarSignIsCode = 0;
	my atSignIsLink = 0;
}

autoGraphics Graphics_create (int resolution) {
	autoGraphics me = Thing_new (Graphics);
	Graphics_init (me.get(), resolution);
	return me;
}

int Graphics_getResolution (Graphics me) {
	return my resolution;
}

void Graphics_setWsViewport (Graphics me,
	long x1DC, long x2DC, long y1DC, long y2DC)
{
	if (x1DC < my d_x1DCmin || x2DC > my d_x2DCmax || y1DC < my d_y1DCmin || y2DC > my d_y2DCmax) {
		Melder_warning (U"Graphics_setWsViewport: coordinates too large:\n",
			x1DC, U"..", x2DC, U" x ", y1DC, U"..", y2DC,
			U" goes outside ",
			my d_x1DCmin, U"..", my d_x2DCmax, U" x ", my d_y1DCmin, U"..", my d_y2DCmax,
			U"."
		);
		x1DC = my d_x1DCmin;
		x2DC = my d_x2DCmax;
		y1DC = my d_y1DCmin;
		y2DC = my d_y2DCmax;
	}
	my d_x1DC = x1DC;
	my d_x2DC = x2DC;
	my d_y1DC = y1DC;
	my d_y2DC = y2DC;
	#if gdi
		if (my screen && my printer) {
			GraphicsScreen mescreen = (GraphicsScreen) me;
			/*
			 * Map page coordinates to paper coordinates.
			 */
			mescreen -> d_x1DC -=  GetDeviceCaps (mescreen -> d_gdiGraphicsContext, PHYSICALOFFSETX);
			mescreen -> d_x2DC -=  GetDeviceCaps (mescreen -> d_gdiGraphicsContext, PHYSICALOFFSETX);
			mescreen -> d_y1DC -=  GetDeviceCaps (mescreen -> d_gdiGraphicsContext, PHYSICALOFFSETY);
			mescreen -> d_y2DC -=  GetDeviceCaps (mescreen -> d_gdiGraphicsContext, PHYSICALOFFSETY);
		}
	#endif
	computeTrafo (me);
}

void Graphics_resetWsViewport (Graphics me,
	long x1DC, long x2DC, long y1DC, long y2DC)
{
	my d_x1DC = x1DC;
	my d_x2DC = x2DC;
	my d_y1DC = y1DC;
	my d_y2DC = y2DC;
	computeTrafo (me);
}

void Graphics_inqWsViewport (Graphics me, long *x1DC, long *x2DC, long *y1DC, long *y2DC) {
	*x1DC = my d_x1DC;
	*x2DC = my d_x2DC;
	*y1DC = my d_y1DC;
	*y2DC = my d_y2DC;
}

void Graphics_setWsWindow (Graphics me, double x1NDC, double x2NDC, double y1NDC, double y2NDC) {
	my d_x1wNDC = x1NDC;
	my d_x2wNDC = x2NDC;
	my d_y1wNDC = y1NDC;
	my d_y2wNDC = y2NDC;
	computeTrafo (me);
	if (my recording)
		{ op (SET_WS_WINDOW, 4); put (x1NDC); put (x2NDC); put (y1NDC); put (y2NDC); }
}

void Graphics_inqWsWindow (Graphics me, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC) {
	*x1NDC = my d_x1wNDC;
	*x2NDC = my d_x2wNDC;
	*y1NDC = my d_y1wNDC;
	*y2NDC = my d_y2wNDC;
}

/***** CO-ORDINATE TRANFORMATIONS *****/

void Graphics_DCtoWC (Graphics me, long xDC, long yDC, double *xWC, double *yWC) {
	if (my yIsZeroAtTheTop) {
		*xWC = (xDC + 0.5 - my deltaX) / my scaleX;
		*yWC = (yDC - 0.5 - my deltaY) / my scaleY;
	} else {
		*xWC = (xDC + 0.5 - my deltaX) / my scaleX;
		*yWC = (yDC + 0.5 - my deltaY) / my scaleY;
	}
}

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

void Graphics_WCtoDC (Graphics me, double xWC, double yWC, long *xDC, long *yDC) {
	*xDC = wdx (xWC);
	*yDC = wdy (yWC);
}

/***** OUTPUT PRIMITIVES, RECORDABLE *****/

void Graphics_setViewport (Graphics me, double x1NDC, double x2NDC, double y1NDC, double y2NDC) {
	trace (U"enter ", x1NDC, U_SPACE, x2NDC, U_SPACE, y1NDC, U_SPACE, y2NDC);
	my d_x1NDC = x1NDC;
	my d_x2NDC = x2NDC;
	my d_y1NDC = y1NDC;
	my d_y2NDC = y2NDC;
	computeTrafo (me);
	if (my recording)
		{ op (SET_VIEWPORT, 4); put (x1NDC); put (x2NDC); put (y1NDC); put (y2NDC); }
}

void Graphics_setInner (Graphics me) {
	double margin = 2.8 * my fontSize * my resolution / 72.0;
	double wDC = (my d_x2DC - my d_x1DC) / (my d_x2wNDC - my d_x1wNDC) * (my d_x2NDC - my d_x1NDC);
	double hDC = labs (my d_y2DC - my d_y1DC) / (my d_y2wNDC - my d_y1wNDC) * (my d_y2NDC - my d_y1NDC);
	double dx = 1.5 * margin / wDC;
	double dy = margin / hDC;
	my horTick = 0.06 * dx, my vertTick = 0.09 * dy;
	if (dx > 0.4) dx = 0.4;
	if (dy > 0.4) dy = 0.4;
	my horTick /= 1 - 2 * dx, my vertTick /= 1 - 2 * dy;
	my outerViewport.x1NDC = my d_x1NDC;
	my outerViewport.x2NDC = my d_x2NDC;
	my outerViewport.y1NDC = my d_y1NDC;
	my outerViewport.y2NDC = my d_y2NDC;
	my d_x1NDC = (1 - dx) * my outerViewport.x1NDC + dx * my outerViewport.x2NDC;
	my d_x2NDC = (1 - dx) * my outerViewport.x2NDC + dx * my outerViewport.x1NDC;
	my d_y1NDC = (1 - dy) * my outerViewport.y1NDC + dy * my outerViewport.y2NDC;
	my d_y2NDC = (1 - dy) * my outerViewport.y2NDC + dy * my outerViewport.y1NDC;
	trace (U"done ", my d_x1NDC, U" ", my d_x2NDC, U" ", my d_y1NDC, U" ", my d_y2NDC);
	computeTrafo (me);
	if (my recording) { op (SET_INNER, 0); }
}

void Graphics_unsetInner (Graphics me) {
	my d_x1NDC = my outerViewport.x1NDC;
	my d_x2NDC = my outerViewport.x2NDC;
	my d_y1NDC = my outerViewport.y1NDC;
	my d_y2NDC = my outerViewport.y2NDC;
	trace (U"done ", my d_x1NDC, U" ", my d_x2NDC, U" ", my d_y1NDC, U" ", my d_y2NDC);
	computeTrafo (me);
	if (my recording)
		{ op (UNSET_INNER, 0); }
}

void Graphics_setWindow (Graphics me, double x1WC, double x2WC, double y1WC, double y2WC) {
	Melder_assert (x1WC != x2WC);
	Melder_assert (y1WC != y2WC);
	my d_x1WC = x1WC;
	my d_x2WC = x2WC;
	my d_y1WC = y1WC;
	my d_y2WC = y2WC;
	computeTrafo (me);
	if (my recording)
		{ op (SET_WINDOW, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

/***** INQUIRIES TO CURRENT GRAPHICS *****/

void Graphics_inqViewport (Graphics me, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC) {
	*x1NDC = my d_x1NDC;
	*x2NDC = my d_x2NDC;
	*y1NDC = my d_y1NDC;
	*y2NDC = my d_y2NDC;
}

void Graphics_inqWindow (Graphics me, double *x1WC, double *x2WC, double *y1WC, double *y2WC) {
	*x1WC = my d_x1WC;
	*x2WC = my d_x2WC;
	*y1WC = my d_y1WC;
	*y2WC = my d_y2WC;
}

Graphics_Viewport Graphics_insetViewport
	(Graphics me, double x1rel, double x2rel, double y1rel, double y2rel)
{
	trace (U"enter");
	Graphics_Viewport previous;
	previous.x1NDC = my d_x1NDC;
	previous.x2NDC = my d_x2NDC;
	previous.y1NDC = my d_y1NDC;
	previous.y2NDC = my d_y2NDC;
	Graphics_setViewport
		(me, (1 - x1rel) * my d_x1NDC + x1rel * my d_x2NDC,
			  x2rel * my d_x2NDC + (1 - x2rel) * my d_x1NDC,
			  (1 - y1rel) * my d_y1NDC + y1rel * my d_y2NDC,
			  y2rel * my d_y2NDC + (1 - y2rel) * my d_y1NDC);
	return previous;
}

void Graphics_resetViewport (Graphics me, Graphics_Viewport viewport) {
	trace (U"enter");
	Graphics_setViewport (me, viewport.x1NDC, viewport.x2NDC, viewport.y1NDC, viewport.y2NDC);
}

/* Millimetres. */

double Graphics_dxMMtoWC (Graphics me, double dx_mm) {
	return dx_mm * my resolution / (25.4 * my scaleX);
}

double Graphics_dyMMtoWC (Graphics me, double dy_mm) {
	return my yIsZeroAtTheTop ?
		dy_mm * my resolution / (-25.4 * my scaleY) : dy_mm * my resolution / (25.4 * my scaleY);
}

double Graphics_distanceWCtoMM (Graphics me, double x1WC, double y1WC, double x2WC, double y2WC) {
	double dxDC = (x1WC - x2WC) * my scaleX;
	double dyDC = (y1WC - y2WC) * my scaleY;
	return sqrt (dxDC * dxDC + dyDC * dyDC) * 25.4 / my resolution;
}

double Graphics_dxWCtoMM (Graphics me, double dxWC) {
	return dxWC * my scaleX * 25.4 / my resolution;
}

double Graphics_dyWCtoMM (Graphics me, double dyWC) {
	return my yIsZeroAtTheTop ?
		dyWC * my scaleY * -25.4 / my resolution : dyWC * my scaleY * 25.4 / my resolution;
}

/* End of file Graphics.cpp */
