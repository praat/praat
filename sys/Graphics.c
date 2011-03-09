/* Graphics.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2007/03/14 arrowSize
 * pb 2007/08/01 reintroduced yIsZeroAtTheTop
 * pb 2007/12/02 enums
 * pb 2007/12/20 getResolution
 */

#include <stdarg.h>
#include "GraphicsP.h"

#include "enums_getText.h"
#include "Graphics_enums.h"
#include "enums_getValue.h"
#include "Graphics_enums.h"

/***** Methods *****/

static void destroy (I) {
	iam (Graphics);
	Melder_free (my record);
	inherited (Graphics) destroy (me);
}

/* On Macintosh, the Device Coordinates are Mac window coordinates.
 * However, we would like to be able to call Graphics_setWsViewport () with widget coordinates.
 * The following routine computes the conversion. It is called by Graphics_init () and by
 * Graphics_setWsViewport ().
 */
static void widgetToWindowCoordinates (I) {
	#if mac
		iam (Graphics);
		if (my screen) {
			iam (GraphicsScreen);
			if (my drawingArea) {
				GuiObject widget = my drawingArea;
				int shellX = 0, shellY = 0;
				do {
					int x = GuiObject_getX (widget), y = GuiObject_getY (widget);
					shellX += x;
					shellY += y;
					widget = GuiObject_parent (widget);
				} while (! XtIsShell (widget));
				my x1DC += shellX;
				my x2DC += shellX;
				my y1DC += shellY;
				my y2DC += shellY;
			}
		}
	#else
		(void) void_me;
	#endif
}

class_methods (Graphics, Thing)
	class_method (destroy)
class_methods_end

static void computeTrafo (I) {
	iam (Graphics);
	double worldScaleX, worldScaleY, workScaleX, workScaleY;
	worldScaleX = (my x2NDC - my x1NDC) / (my x2WC - my x1WC);
	worldScaleY = (my y2NDC - my y1NDC) / (my y2WC - my y1WC);
	my deltaX = my x1NDC - my x1WC * worldScaleX;
	my deltaY = my y1NDC - my y1WC * worldScaleY;
	workScaleX = (my x2DC - my x1DC) / (my x2wNDC - my x1wNDC);
	my deltaX = my x1DC - (my x1wNDC - my deltaX) * workScaleX;
	my scaleX = worldScaleX * workScaleX;
	if (my yIsZeroAtTheTop) {
		workScaleY = ((int) my y1DC - (int) my y2DC) / (my y2wNDC - my y1wNDC);
		my deltaY = my y2DC - (my y1wNDC - my deltaY) * workScaleY;
	} else {
		workScaleY = ((int) my y2DC - (int) my y1DC) / (my y2wNDC - my y1wNDC);
		my deltaY = my y1DC - (my y1wNDC - my deltaY) * workScaleY;
	}
	my scaleY = worldScaleY * workScaleY;
}

/***** WORKSTATION FUNCTIONS *****/

int Graphics_init (I) {
	iam (Graphics);
	my x1DC = my x1DCmin = 0;	my x2DC = my x2DCmax = 32767;
	my y1DC = my y1DCmin = 0;	my y2DC = my y2DCmax = 32767;
	my x1WC = my x1NDC = my x1wNDC = 0.0;
	my x2WC = my x2NDC = my x2wNDC = 1.0;
	my y1WC = my y1NDC = my y1wNDC = 0.0;
	my y2WC = my y2NDC = my y2wNDC = 1.0;
	widgetToWindowCoordinates (me);
	computeTrafo (me);
	my lineWidth = 1.0;
	my arrowSize = 1.0;
	my font = kGraphics_font_HELVETICA;
	my fontSize = 10;
	my fontStyle = Graphics_NORMAL;
	my record = NULL;
	my irecord = my nrecord = 0;
	my percentSignIsItalic = 1;
	my numberSignIsBold = 1;
	my circumflexIsSuperscript = 1;
	my underscoreIsSubscript = 1;
	my dollarSignIsCode = 0;
	my atSignIsLink = 0;
	return 1;
}

Graphics Graphics_create (int resolution) {
	Graphics me = (Graphics) Thing_new (Graphics);
	if (! me || ! Graphics_init (me)) forget (me);
	my resolution = resolution;
	return me;
}

int Graphics_getResolution (I) {
	iam (Graphics);
	return my resolution;
}

void Graphics_setWsViewport (I,
	long x1DC, long x2DC, long y1DC, long y2DC)
{
	iam (Graphics);
	if (x1DC < my x1DCmin || x2DC > my x2DCmax || y1DC < my y1DCmin || y2DC > my y2DCmax) {
		static MelderString warning = { 0 };
		MelderString_empty (& warning);
		MelderString_append8 (& warning, L"Graphics_setWsViewport: coordinates too large:\n",
			Melder_integer (x1DC), L"..", Melder_integer (x2DC), L" x ", Melder_integer (y1DC), L"..", Melder_integer (y2DC));
		MelderString_append9 (& warning, L" goes outside ",
			Melder_integer (my x1DCmin), L"..", Melder_integer (my x2DCmax), L" x ", Melder_integer (my y1DCmin), L"..", Melder_integer (my y2DCmax), L".");
		Melder_warning1 (warning.string);
		x1DC = my x1DCmin;
		x2DC = my x2DCmax;
		y1DC = my y1DCmin;
		y2DC = my y2DCmax;
	}
	my x1DC = x1DC;
	my x2DC = x2DC;
	my y1DC = y1DC;
	my y2DC = y2DC;
	widgetToWindowCoordinates (me);
	#if win
		if (my screen && my printer) {
			iam (GraphicsScreen);
			/*
			 * Map page coordinates to paper coordinates.
			 */
			my x1DC -=  GetDeviceCaps (my dc, PHYSICALOFFSETX);
			my x2DC -=  GetDeviceCaps (my dc, PHYSICALOFFSETX);
			my y1DC -=  GetDeviceCaps (my dc, PHYSICALOFFSETY);
			my y2DC -=  GetDeviceCaps (my dc, PHYSICALOFFSETY);
		}
	#endif
	computeTrafo (me);
}

void Graphics_resetWsViewport (I,
	long x1DC, long x2DC, long y1DC, long y2DC)
{
	iam (Graphics);
	my x1DC = x1DC;
	my x2DC = x2DC;
	my y1DC = y1DC;
	my y2DC = y2DC;
	computeTrafo (me);
}

void Graphics_inqWsViewport (I, long *x1DC, long *x2DC, long *y1DC, long *y2DC) {
	iam (Graphics);
	*x1DC = my x1DC;
	*x2DC = my x2DC;
	*y1DC = my y1DC;
	*y2DC = my y2DC;
}

void Graphics_setWsWindow (I, double x1NDC, double x2NDC, double y1NDC, double y2NDC) {
	iam (Graphics);
	my x1wNDC = x1NDC;
	my x2wNDC = x2NDC;
	my y1wNDC = y1NDC;
	my y2wNDC = y2NDC;
	computeTrafo (me);
	if (my recording)
		{ op (SET_WS_WINDOW, 4); put (x1NDC); put (x2NDC); put (y1NDC); put (y2NDC); }
}

void Graphics_inqWsWindow (I, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC) {
	iam (Graphics);
	*x1NDC = my x1wNDC;
	*x2NDC = my x2wNDC;
	*y1NDC = my y1wNDC;
	*y2NDC = my y2wNDC;
}

/***** CO-ORDINATE TRANFORMATIONS *****/

void Graphics_DCtoWC (I, long xDC, long yDC, double *xWC, double *yWC) {
	iam (Graphics);
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

void Graphics_WCtoDC (I, double xWC, double yWC, long *xDC, long *yDC) {
	iam (Graphics);
	*xDC = wdx (xWC);
	*yDC = wdy (yWC);
}

/***** OUTPUT PRIMITIVES, RECORDABLE *****/

void Graphics_setViewport (I, double x1NDC, double x2NDC, double y1NDC, double y2NDC) {
	iam (Graphics);
	my x1NDC = x1NDC;
	my x2NDC = x2NDC;
	my y1NDC = y1NDC;
	my y2NDC = y2NDC;
	computeTrafo (me);
	if (my recording)
		{ op (SET_VIEWPORT, 4); put (x1NDC); put (x2NDC); put (y1NDC); put (y2NDC); }
}

void Graphics_setInner (I) {
	iam (Graphics);
	double margin = 2.8 * my fontSize * my resolution / 72.0, wDC, hDC, dx, dy;
	wDC = (my x2DC - my x1DC) / (my x2wNDC - my x1wNDC) * (my x2NDC - my x1NDC);
	hDC = abs (my y2DC - my y1DC) / (my y2wNDC - my y1wNDC) * (my y2NDC - my y1NDC);
	dx = 1.5 * margin / wDC;
	dy = margin / hDC;
	my horTick = 0.06 * dx, my vertTick = 0.09 * dy;
	if (dx > 0.4) dx = 0.4;
	if (dy > 0.4) dy = 0.4;
	my horTick /= 1 - 2 * dx, my vertTick /= 1 - 2 * dy;
	my outerViewport.x1NDC = my x1NDC;
	my outerViewport.x2NDC = my x2NDC;
	my outerViewport.y1NDC = my y1NDC;
	my outerViewport.y2NDC = my y2NDC;
	my x1NDC = (1 - dx) * my outerViewport.x1NDC + dx * my outerViewport.x2NDC;
	my x2NDC = (1 - dx) * my outerViewport.x2NDC + dx * my outerViewport.x1NDC;
	my y1NDC = (1 - dy) * my outerViewport.y1NDC + dy * my outerViewport.y2NDC;
	my y2NDC = (1 - dy) * my outerViewport.y2NDC + dy * my outerViewport.y1NDC;
	computeTrafo (me);
	if (my recording) { op (SET_INNER, 0); }
}

void Graphics_unsetInner (I) {
	iam (Graphics);
	my x1NDC = my outerViewport.x1NDC;
	my x2NDC = my outerViewport.x2NDC;
	my y1NDC = my outerViewport.y1NDC;
	my y2NDC = my outerViewport.y2NDC;
	computeTrafo (me);
	if (my recording)
		{ op (UNSET_INNER, 0); }
}

void Graphics_setWindow (I, double x1WC, double x2WC, double y1WC, double y2WC) {
	iam (Graphics);
	my x1WC = x1WC;
	my x2WC = x2WC;
	my y1WC = y1WC;
	my y2WC = y2WC;
	computeTrafo (me);
	if (my recording)
		{ op (SET_WINDOW, 4); put (x1WC); put (x2WC); put (y1WC); put (y2WC); }
}

/***** INQUIRIES TO CURRENT GRAPHICS *****/

void Graphics_inqViewport (I, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC) {
	iam (Graphics);
	*x1NDC = my x1NDC;
	*x2NDC = my x2NDC;
	*y1NDC = my y1NDC;
	*y2NDC = my y2NDC;
}

void Graphics_inqWindow (I, double *x1WC, double *x2WC, double *y1WC, double *y2WC) {
	iam (Graphics);
	*x1WC = my x1WC;
	*x2WC = my x2WC;
	*y1WC = my y1WC;
	*y2WC = my y2WC;
}

Graphics_Viewport Graphics_insetViewport
	(I, double x1rel, double x2rel, double y1rel, double y2rel)
{
	iam (Graphics);
	Graphics_Viewport previous;
	previous.x1NDC = my x1NDC;
	previous.x2NDC = my x2NDC;
	previous.y1NDC = my y1NDC;
	previous.y2NDC = my y2NDC;
	Graphics_setViewport
		(me, (1 - x1rel) * my x1NDC + x1rel * my x2NDC,
			  x2rel * my x2NDC + (1 - x2rel) * my x1NDC,
			  (1 - y1rel) * my y1NDC + y1rel * my y2NDC,
			  y2rel * my y2NDC + (1 - y2rel) * my y1NDC);
	return previous;
}

void Graphics_resetViewport (I, Graphics_Viewport viewport) {
	iam (Graphics);
	Graphics_setViewport (me, viewport.x1NDC, viewport.x2NDC, viewport.y1NDC, viewport.y2NDC);
}

/* Millimetres. */

double Graphics_dxMMtoWC (I, double dx_mm) {
	iam (Graphics);
	return dx_mm * my resolution / (25.4 * my scaleX);
}

double Graphics_dyMMtoWC (I, double dy_mm) {
	iam (Graphics);
	return my yIsZeroAtTheTop ?
		dy_mm * my resolution / (-25.4 * my scaleY) : dy_mm * my resolution / (25.4 * my scaleY);
}

double Graphics_distanceWCtoMM (I, double x1WC, double y1WC, double x2WC, double y2WC) {
	iam (Graphics);
	double dxDC = (x1WC - x2WC) * my scaleX;
	double dyDC = (y1WC - y2WC) * my scaleY;
	return sqrt (dxDC * dxDC + dyDC * dyDC) * 25.4 / my resolution;
}

double Graphics_dxWCtoMM (I, double dxWC) {
	iam (Graphics);
	return dxWC * my scaleX * 25.4 / my resolution;
}

double Graphics_dyWCtoMM (I, double dyWC) {
	iam (Graphics);
	return my yIsZeroAtTheTop ?
		dyWC * my scaleY * -25.4 / my resolution : dyWC * my scaleY * 25.4 / my resolution;
}

/* End of file Graphics.c */
