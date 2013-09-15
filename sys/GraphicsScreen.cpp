/* GraphicsScreen.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma, 2013 Tom Naughton
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
 * pb 2004/10/18 ReleaseDC
 * pb 2007/08/01 reintroduced yIsZeroAtTheTop
 * sdk 2008/03/24 cairo
 * sdk 2008/05/09 cairo
 * pb 2009/07/24 quartz
 * fb 2010/02/23 cairo & clipping on updateWs()
 * pb 2010/05/13 support XOR mode
 * pb 2010/07/13 split erasure of recording off from Graphics_clearWs
 * pb 2011/03/17 C++
 */

#include "GraphicsP.h"
#include "Printer.h"
#include "GuiP.h"

#if win
	//#include "winport_on.h"
	#include <gdiplus.h>
	//#include "winport_off.h"
	//using namespace Gdiplus;
	static bool _GraphicsWindows_tryToInitializeGdiPlus (void) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup (& gdiplusToken, & gdiplusStartupInput, NULL);
		return true;
	}
#elif mac
	#include "macport_on.h"
	static RGBColor theBlackColour = { 0, 0, 0 };
	static bool _GraphicsMacintosh_tryToInitializeQuartz (void) {
		#if cocoa
			return true;
		#else
			return _GraphicsMac_tryToInitializeAtsuiFonts ();
		#endif
	}
#endif

Thing_implement (GraphicsScreen, Graphics, 0);

void structGraphicsScreen :: v_destroy () {
	#if cairo
		if (d_gdkGraphicsContext != NULL) {
			g_object_unref (d_gdkGraphicsContext);			
			d_gdkGraphicsContext = NULL;
		}
		if (d_cairoGraphicsContext != NULL) {
			cairo_destroy (d_cairoGraphicsContext);
			d_cairoGraphicsContext = NULL;
		}
	#elif win
		if (d_gdiGraphicsContext != NULL) {
			SelectPen (d_gdiGraphicsContext, GetStockPen (BLACK_PEN));
			SelectBrush (d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
			d_gdiGraphicsContext = NULL;
		}
		if (d_winPen != NULL) {
			DeleteObject (d_winPen);
			d_winPen = NULL;
		}
		if (d_winBrush != NULL) {
			DeleteObject (d_winBrush);
			d_winBrush = NULL;
		}
		/*
		 * No ReleaseDC here, because we have not created it ourselves,
		 * not even with GetDC.
		 */
	#elif mac
        #if useCarbon
            if (d_macPort == NULL) {
                CGContextEndPage (d_macGraphicsContext);
                CGContextRelease (d_macGraphicsContext);
            }

        #else
            if (d_macView == NULL) {
                CGContextEndPage (d_macGraphicsContext);
                CGContextRelease (d_macGraphicsContext);
            }
        #endif
	#endif
	GraphicsScreen_Parent :: v_destroy ();
}

void structGraphicsScreen :: v_flushWs () {
	#if cairo
		// Ik weet niet of dit is wat het zou moeten zijn ;)
		//gdk_window_process_updates (d_window, TRUE);   // this "works" but is incorrect because it's not the expose events that have to be carried out
		//gdk_window_flush (d_window);
		//gdk_flush ();
		// TODO: een aanroep die de eventuele grafische buffer ledigt,
		// zodat de gebruiker de grafica ziet ook al blijft Praat in hetzelfde event zitten
	#elif cocoa
		if (d_drawingArea) {
			GuiShell shell = d_drawingArea -> d_shell;
			Melder_assert (shell != NULL);
        	[shell -> d_cocoaWindow   flushWindow];
		}
	#elif win
		/*GdiFlush ();*/
	#elif mac
		if (d_drawingArea) {
			GuiShell shell = d_drawingArea -> d_shell;
			Melder_assert (shell != NULL);
			shell -> f_drain ();
		}
	#endif
}

void Graphics_flushWs (Graphics me) {
	my v_flushWs ();
}

void structGraphicsScreen :: v_clearWs () {
	#if cairo
		GdkRectangle rect;
		if (this -> d_x1DC < this -> d_x2DC) {
			rect.x = this -> d_x1DC;
			rect.width = this -> d_x2DC - this -> d_x1DC;
		} else {
			rect.x = this -> d_x2DC;
			rect.width = this -> d_x1DC - this -> d_x2DC;
		}
		if (this -> d_y1DC < this -> d_y2DC) {
			rect.y = this -> d_y1DC;
			rect.height = this -> d_y2DC - this -> d_y1DC;
		} else {
			rect.y = this -> d_y2DC;
			rect.height = this -> d_y1DC - this -> d_y2DC;
		}
		if (d_cairoGraphicsContext == NULL) {
			trace ("clear and null");
			//gdk_window_clear (this -> window);
			//gdk_window_invalidate_rect (this -> window, & rect, true);   // BUG: it seems weird that this is necessary.
		} else {
			trace ("clear and not null");
			cairo_set_source_rgb (d_cairoGraphicsContext, 1.0, 1.0, 1.0);
			cairo_rectangle (d_cairoGraphicsContext, rect.x, rect.y, rect.width, rect.height);
			cairo_fill (d_cairoGraphicsContext);
			cairo_set_source_rgb (d_cairoGraphicsContext, 0.0, 0.0, 0.0);
		}
	#elif cocoa
        GuiCocoaDrawingArea *cocoaDrawingArea = (GuiCocoaDrawingArea *) d_drawingArea -> d_widget;
        if (cocoaDrawingArea) {
            NSRect rect;
            if (this -> d_x1DC < this -> d_x2DC) {
                rect.origin.x = this -> d_x1DC;
                rect.size.width = this -> d_x2DC - this -> d_x1DC;
            } else {
                rect.origin.x = this -> d_x2DC;
                rect.size.width = this -> d_x1DC - this -> d_x2DC;
            }
            if (this -> d_y1DC < this -> d_y2DC) {
                rect.origin.y = this -> d_y1DC;
                rect.size.height = this -> d_y2DC - this -> d_y1DC;
            } else {
                rect.origin.y = this -> d_y2DC;
                rect.size.height = this -> d_y1DC - this -> d_y2DC;
            }
			[cocoaDrawingArea lockFocus];
            CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
            CGContextSaveGState (context);
            CGContextSetAlpha (context, 1.0);
            CGContextSetBlendMode (context, kCGBlendModeNormal);
            CGContextSetRGBFillColor (context, 1.0, 1.0, 1.0, 1.0);
			//rect.origin.x -= 1000;
			//rect.origin.y -= 1000;
			//rect.size.width += 2000;
			//rect.size.height += 2000;
			trace ("clearing %f %f %f %f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
                //CGContextTranslateCTM (context, 0, cocoaDrawingArea.bounds.size.height);
                //CGContextScaleCTM (context, 1.0, -1.0);
            CGContextFillRect (context, rect);
            //CGContextSynchronize (context);
            CGContextRestoreGState (context);
			[cocoaDrawingArea unlockFocus];
        }
	#elif win
		RECT rect;
		rect. left = rect. top = 0;
		rect. right = d_x2DC - d_x1DC;
		rect. bottom = d_y2DC - d_y1DC;
		FillRect (d_gdiGraphicsContext, & rect, GetStockBrush (WHITE_BRUSH));
		/*if (d_winWindow) SendMessage (d_winWindow, WM_ERASEBKGND, (WPARAM) d_gdiGraphicsContext, 0);*/
	#elif mac
		QDBeginCGContext (d_macPort, & d_macGraphicsContext);
		CGContextSetAlpha (d_macGraphicsContext, 1.0);
		CGContextSetBlendMode (d_macGraphicsContext, kCGBlendModeNormal);
		//CGContextSetAllowsAntialiasing (my macGraphicsContext, false);
		int shellHeight = GuiMac_clipOn_graphicsContext (d_drawingArea -> d_widget, d_macGraphicsContext);
		CGContextSetRGBFillColor (d_macGraphicsContext, 1.0, 1.0, 1.0, 1.0);
		CGContextFillRect (d_macGraphicsContext, CGRectMake (this -> d_x1DC, shellHeight - this -> d_y1DC, this -> d_x2DC - this -> d_x1DC, this -> d_y1DC - this -> d_y2DC));
		QDEndCGContext (d_macPort, & d_macGraphicsContext);
	#endif
}

void Graphics_clearWs (Graphics me) {
	my v_clearWs ();
}

void structGraphicsScreen :: v_updateWs () {
	/*
	 * A function that invalidates the graphics.
	 * This function is typically called by the owner of the drawing area
	 * whenever the data to be displayed in the drawing area has changed;
	 * the idea is to generate an expose event to which the drawing area will
	 * respond by redrawing its contents from the (changed) data.
	 */
	#if gtk
		//GdkWindow *window = gtk_widget_get_parent_window (GTK_WIDGET (my drawingArea -> d_widget));
		GdkRectangle rect;

		if (this -> d_x1DC < this -> d_x2DC) {
			rect.x = this -> d_x1DC;
			rect.width = this -> d_x2DC - this -> d_x1DC;
		} else {
			rect.x = this -> d_x2DC;
			rect.width = this -> d_x1DC - this -> d_x2DC;
		}

		if (this -> d_y1DC < this -> d_y2DC) {
			rect.y = this -> d_y1DC;
			rect.height = this -> d_y2DC - this -> d_y1DC;
		} else {
			rect.y = this -> d_y2DC;
			rect.height = this -> d_y1DC - this -> d_y2DC;
		}

		if (d_cairoGraphicsContext && d_drawingArea) {  // update clipping rectangle to new graphics size
			cairo_reset_clip (d_cairoGraphicsContext);
			cairo_rectangle (d_cairoGraphicsContext, rect.x, rect.y, rect.width, rect.height);
			cairo_clip (d_cairoGraphicsContext);
		}
		gdk_window_clear (d_window);
		gdk_window_invalidate_rect (d_window, & rect, true);
		//gdk_window_process_updates (d_window, true);
	#elif cocoa
        NSView *view =  d_macView;
        NSRect rect;
    
        if (this -> d_x1DC < this -> d_x2DC) {
            rect.origin.x = this -> d_x1DC;
            rect.size.width = this -> d_x2DC - this -> d_x1DC;
        } else {
            rect.origin.x = this -> d_x2DC;
            rect.size.width = this -> d_x1DC - this -> d_x2DC;
        }
        
        if (this -> d_y1DC < this -> d_y2DC) {
            rect.origin.y = this -> d_y1DC;
            rect.size.height = this -> d_y2DC - this -> d_y1DC;
        } else {
            rect.origin.y = this -> d_y2DC;
            rect.size.height = this -> d_y1DC - this -> d_y2DC;
        }
    
        //[view setNeedsDisplayInRect: rect];
        [view setNeedsDisplay: YES];
    
	#elif win
		//clear (this); // lll
		if (d_winWindow) InvalidateRect (d_winWindow, NULL, TRUE);
	#elif mac
		Rect r;
		if (d_drawingArea) GuiMac_clipOn (d_drawingArea -> d_widget);   // to prevent invalidating invisible parts of the canvas
		SetRect (& r, this -> d_x1DC, this -> d_y1DC, this -> d_x2DC, this -> d_y2DC);
		SetPort (d_macPort);
		/*EraseRect (& r);*/
		InvalWindowRect (GetWindowFromPort ((CGrafPtr) d_macPort), & r);
		if (d_drawingArea) GuiMac_clipOff ();
	#endif
}

void Graphics_updateWs (Graphics me) {
	if (me)
		my v_updateWs ();
}

static int GraphicsScreen_init (GraphicsScreen me, void *voidDisplay, void *voidWindow, int resolution) {

	/* Fill in new members. */

	#if cairo
		my d_display = (GdkDisplay *) gdk_display_get_default ();
		_GraphicsScreen_text_init (me);
		my resolution = 100;
		trace ("retrieving window");
		my d_window = GDK_DRAWABLE (GTK_WIDGET (voidDisplay) -> window);
		trace ("retrieved window");
		my d_gdkGraphicsContext = gdk_gc_new (my d_window);
		my d_cairoGraphicsContext = gdk_cairo_create (my d_window);
	#elif win
		if (my printer) {
			my d_gdiGraphicsContext = (HDC) voidWindow;
		} else if (voidDisplay) {
			my d_gdiGraphicsContext = (HDC) voidDisplay;
			my metafile = TRUE;
		} else {
			my d_winWindow = (HWND) voidWindow;
			my d_gdiGraphicsContext = GetDC (my d_winWindow);   // window must have a constant display context; see XtInitialize ()
		}
		Melder_assert (my d_gdiGraphicsContext != NULL);
		my resolution = resolution;
		SetBkMode (my d_gdiGraphicsContext, TRANSPARENT);   // not the default!
		/*
		 * Create pens and brushes.
		 */
		my d_winPen = CreatePen (PS_SOLID, 0, RGB (0, 0, 0));
		my d_winBrush = CreateSolidBrush (RGB (0, 0, 0));
		SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
		SetTextAlign (my d_gdiGraphicsContext, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
		_GraphicsScreen_text_init (me);
	#elif mac
		(void) voidDisplay;
        #if useCarbon
            my d_macPort = (GrafPtr) voidWindow;
        #else
            my d_macView = (NSView*) voidWindow;
        #endif
		my d_macColour = theBlackColour;
		my resolution = resolution;
		my d_depth = my resolution > 150 ? 1 : 8;   /* BUG: replace by true depth (1=black/white) */
		(void) my d_macGraphicsContext;   // will be retreived from QuickDraw with every drawing command!
		_GraphicsScreen_text_init (me);
	#endif
	return 1;
}

Graphics Graphics_create_screen (void *display, void *window, int resolution) {
	GraphicsScreen me = Thing_new (GraphicsScreen);
	my screen = true;
	my yIsZeroAtTheTop = true;
	Graphics_init (me);
	Graphics_setWsViewport ((Graphics) me, 0, 100, 0, 100);
	#if mac && useCarbon
		GraphicsScreen_init (me, display, GetWindowPort ((WindowRef) window), resolution);
	#else
		GraphicsScreen_init (me, display, window, resolution);
	#endif
	return (Graphics) me;
}

Graphics Graphics_create_screenPrinter (void *display, void *window) {
	GraphicsScreen me = Thing_new (GraphicsScreen);
	my screen = true;
	my yIsZeroAtTheTop = true;
	my printer = true;
	#ifdef macintosh
		_GraphicsMacintosh_tryToInitializeQuartz ();
	#endif
	Graphics_init (me);
	my paperWidth = (double) thePrinter. paperWidth / thePrinter. resolution;
	my paperHeight = (double) thePrinter. paperHeight / thePrinter. resolution;
	my d_x1DC = my d_x1DCmin = thePrinter. resolution / 2;
	my d_x2DC = my d_x2DCmax = (my paperWidth - 0.5) * thePrinter. resolution;
	my d_y1DC = my d_y1DCmin = thePrinter. resolution / 2;
	my d_y2DC = my d_y2DCmax = (my paperHeight - 0.5) * thePrinter. resolution;
	#if win
		/*
		 * Map page coordinates to paper coordinates.
		 */
		my d_x1DC -= GetDeviceCaps ((HDC) window, PHYSICALOFFSETX);
		my d_x2DC -= GetDeviceCaps ((HDC) window, PHYSICALOFFSETX);
		my d_y1DC -= GetDeviceCaps ((HDC) window, PHYSICALOFFSETY);
		my d_y2DC -= GetDeviceCaps ((HDC) window, PHYSICALOFFSETY);
	#endif
	Graphics_setWsWindow ((Graphics) me, 0, my paperWidth - 1.0, 13.0 - my paperHeight, 12.0);
	GraphicsScreen_init (me, display, window, thePrinter. resolution);
	return (Graphics) me;
}

#if mac && useCarbon
/* Drawing areas support resize callbacks.
 * However, Mac drawing areas also support move callbacks.
 * This is the only way for a graphics driver to get notified of a move,
 * which would bring about a change in device coordinates.
 * On Xwin and Win, we are not interested in moves, because we draw in widget coordinates.
 */
static void cb_move (GuiObject w, XtPointer void_me, XtPointer call) {
	iam (GraphicsScreen);
	Dimension width, height;
	XtVaGetValues (w, XmNwidth, & width, XmNheight, & height, NULL);

	/* The four values returned are probably equal to the previous ones.
	 * However, the following call forces a new computation of the device coordinates
	 * by widgetToWindowCoordinates ().
	 */

	Graphics_setWsViewport ((Graphics) me, 0 /* Left x value in widget coordinates */,
		width, 0, height);
	Graphics_updateWs ((Graphics) me);
}
#endif

Graphics Graphics_create_xmdrawingarea (GuiDrawingArea w) {
	trace ("begin");
	GraphicsScreen me = Thing_new (GraphicsScreen);
	#if gtk
		GtkRequisition realsize;
		GtkAllocation allocation;
	#elif motif
		Dimension width, height;
	#endif

	my d_drawingArea = static_cast <GuiDrawingArea> (w);   /* Now !!!!!!!!!! */
	Melder_assert (my d_drawingArea -> d_widget);
	my screen = true;
	my yIsZeroAtTheTop = true;
	#if win
		my d_useGdiplus = _GraphicsWindows_tryToInitializeGdiPlus ();
	#elif mac
		_GraphicsMacintosh_tryToInitializeQuartz ();
	#endif
	Graphics_init (me);
	#if mac 
    #if useCarbon
            GraphicsScreen_init (me,
                XtDisplay (my d_drawingArea -> d_widget),
                GetWindowPort ((WindowRef) XtWindow (my d_drawingArea -> d_widget)),
                Gui_getResolution (NULL));
    #else
            GraphicsScreen_init (me,
                                 my d_drawingArea -> d_widget,
                                 my d_drawingArea -> d_widget,
                                 Gui_getResolution (NULL));
    #endif
    
	#else
		#if gtk
			GraphicsScreen_init (me, GTK_WIDGET (my d_drawingArea -> d_widget), GTK_WIDGET (my d_drawingArea -> d_widget), Gui_getResolution (my d_drawingArea -> d_widget));
		#elif motif
			GraphicsScreen_init (me, XtDisplay (my d_drawingArea -> d_widget), XtWindow (my d_drawingArea -> d_widget), Gui_getResolution (my d_drawingArea -> d_widget));
		#endif
	#endif

	#if gtk
		// fb: is really the request meant or rather the actual size, aka allocation?
		gtk_widget_size_request (GTK_WIDGET (my d_drawingArea -> d_widget), & realsize);
		gtk_widget_get_allocation (GTK_WIDGET (my d_drawingArea -> d_widget), & allocation);
		// HIER WAS IK
		trace ("requested %d x %d, allocated %d x %d", realsize.width, realsize.height, allocation.width, allocation.height);
		Graphics_setWsViewport ((Graphics) me, 0, realsize.width, 0, realsize.height);
	#elif motif
		XtVaGetValues (my d_drawingArea -> d_widget, XmNwidth, & width, XmNheight, & height, NULL);
		Graphics_setWsViewport ((Graphics) me, 0, width, 0, height);
    #elif cocoa
        NSView *view = (NSView *)my d_drawingArea -> d_widget;
        NSRect bounds = [view bounds];
        Graphics_setWsViewport ((Graphics) me, 0, bounds.size.width, 0, bounds.size.height);
	#endif
	#if mac && useCarbon
		XtAddCallback (my d_drawingArea -> d_widget, XmNmoveCallback, cb_move, (XtPointer) me);
	#endif
	return (Graphics) me;
}

Graphics Graphics_create_pdffile (MelderFile file, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches)
{
	GraphicsScreen me = Thing_new (GraphicsScreen);
	my screen = true;
	my yIsZeroAtTheTop = true;
	#ifdef macintosh
		_GraphicsMacintosh_tryToInitializeQuartz ();
	#endif
	Graphics_init (me);
	my resolution = resolution;
	#ifdef macintosh
		CFURLRef url = CFURLCreateWithFileSystemPath (NULL, (CFStringRef) Melder_peekWcsToCfstring (file -> path), kCFURLPOSIXPathStyle, false);
		CGRect rect = CGRectMake (0, 0, (x2inches - x1inches) * 72, (y2inches - y1inches) * 72);   // don't tire PDF viewers with funny origins
		CFStringRef key = (CFStringRef) Melder_peekWcsToCfstring (L"Creator");
		CFStringRef value = (CFStringRef) Melder_peekWcsToCfstring (L"Praat");
		CFIndex numberOfValues = 1;
		CFDictionaryRef dictionary = CFDictionaryCreate (NULL, (const void **) & key, (const void **) & value, numberOfValues,
			& kCFTypeDictionaryKeyCallBacks, & kCFTypeDictionaryValueCallBacks);
		my d_macGraphicsContext = CGPDFContextCreateWithURL (url, & rect, dictionary);
		CFRelease (url);
		CFRelease (dictionary);
		my d_x1DC = my d_x1DCmin = 0;
		my d_x2DC = my d_x2DCmax = 7.5 * resolution;
		my d_y1DC = my d_y1DCmin = 0;
		my d_y2DC = my d_y2DCmax = 11.0 * resolution;
		Graphics_setWsWindow ((Graphics) me, 0, 7.5, 1.0, 12.0);
    NSCAssert(my d_macGraphicsContext, @"nil context");

		CGContextBeginPage (my d_macGraphicsContext, & rect);
		CGContextScaleCTM (my d_macGraphicsContext, 72.0/resolution, 72.0/resolution);
		CGContextTranslateCTM (my d_macGraphicsContext, - x1inches * resolution, (12.0 - y1inches) * resolution);
		CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
	#endif
	return (Graphics) me;
}
Graphics Graphics_create_pdf (void *context, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches)
{
	GraphicsScreen me = Thing_new (GraphicsScreen);
	my screen = true;
	my yIsZeroAtTheTop = true;
	#ifdef macintosh
		_GraphicsMacintosh_tryToInitializeQuartz ();
	#endif
	Graphics_init (me);
	my resolution = resolution;
	#ifdef macintosh
		my d_macGraphicsContext = static_cast <CGContext *> (context);
		CGRect rect = CGRectMake (0, 0, (x2inches - x1inches) * 72, (y2inches - y1inches) * 72);   // don't tire PDF viewers with funny origins
		my d_x1DC = my d_x1DCmin = 0;
		my d_x2DC = my d_x2DCmax = 7.5 * resolution;
		my d_y1DC = my d_y1DCmin = 0;
		my d_y2DC = my d_y2DCmax = 11.0 * resolution;
		Graphics_setWsWindow ((Graphics) me, 0, 7.5, 1.0, 12.0);
    NSCAssert(my d_macGraphicsContext, @"nil context");

		CGContextBeginPage (my d_macGraphicsContext, & rect);
		CGContextScaleCTM (my d_macGraphicsContext, 72.0/resolution, 72.0/resolution);
		CGContextTranslateCTM (my d_macGraphicsContext, - x1inches * resolution, (12.0 - y1inches) * resolution);
		CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
	#endif
	return (Graphics) me;
}

#if cairo
	void *Graphics_x_getCR (Graphics me) {
		return ((GraphicsScreen) me) -> d_cairoGraphicsContext;
	}
	void Graphics_x_setCR (Graphics me, void *cairoGraphicsContext) {
		((GraphicsScreen) me) -> d_cairoGraphicsContext = (cairo_t *) cairoGraphicsContext;
	}
#endif

#if mac
	void GraphicsQuartz_initDraw (GraphicsScreen me) {
		#if useCarbon
			if (my d_macPort) {
					QDBeginCGContext (my d_macPort, & my d_macGraphicsContext);
				//CGContextSetAlpha (my macGraphicsContext, 1.0);
				//CGContextSetAllowsAntialiasing (my macGraphicsContext, false);
				if (my d_drawingArea != NULL) {
					int shellHeight = GuiMac_clipOn_graphicsContext (my d_drawingArea -> d_widget, my d_macGraphicsContext);
					CGContextTranslateCTM (my d_macGraphicsContext, 0, shellHeight);
				} else if (my printer) {
					CGContextTranslateCTM (my d_macGraphicsContext, 0, my d_y2DC - my d_y1DC);
				}
				CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
			}
        #else
            if (my d_macView) {            
                [my d_macView lockFocus];
                my d_macGraphicsContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
                Melder_assert (my d_macGraphicsContext != NULL);
                GuiCocoaDrawingArea *cocoaDrawingArea = (GuiCocoaDrawingArea *) my d_drawingArea -> d_widget;
                CGContextTranslateCTM (my d_macGraphicsContext, 0, cocoaDrawingArea.bounds.size.height);
                CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
			}
		#endif
	}
	void GraphicsQuartz_exitDraw (GraphicsScreen me) {
		#if useCarbon
			if (my d_macPort) {
				CGContextSynchronize (my d_macGraphicsContext);
				QDEndCGContext (my d_macPort, & my d_macGraphicsContext);
			}
        #else
            if (my d_macView) {
                CGContextSynchronize (my d_macGraphicsContext);   // BUG: should not be needed
                [my d_macView unlockFocus];
            }
		#endif
	}
#endif

/* End of file GraphicsScreen.cpp */
