/* Photo.cpp
 *
 * Copyright (C) 2013 Paul Boersma
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

#include "Photo.h"
#include "NUM2.h"
#include "Formula.h"
#if defined (macintosh)
	#include "macport_on.h"
	#include <Cocoa/Cocoa.h>
	#include "macport_off.h"
#endif

#include "oo_DESTROY.h"
#include "Photo_def.h"
#include "oo_COPY.h"
#include "Photo_def.h"
#include "oo_EQUAL.h"
#include "Photo_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Photo_def.h"
#include "oo_WRITE_TEXT.h"
#include "Photo_def.h"
#include "oo_READ_TEXT.h"
#include "Photo_def.h"
#include "oo_WRITE_BINARY.h"
#include "Photo_def.h"
#include "oo_READ_BINARY.h"
#include "Photo_def.h"
#include "oo_DESCRIPTION.h"
#include "Photo_def.h"

Thing_implement (Photo, SampledXY, 0);

void structPhoto :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"xmin: ", Melder_double (xmin));
	MelderInfo_writeLine (L"xmax: ", Melder_double (xmax));
	MelderInfo_writeLine (L"Number of columns: ", Melder_integer (nx));
	MelderInfo_writeLine (L"dx: ", Melder_double (dx), L" (-> sampling rate ", Melder_double (1.0 / dx), L" )");
	MelderInfo_writeLine (L"x1: ", Melder_double (x1));
	MelderInfo_writeLine (L"ymin: ", Melder_double (ymin));
	MelderInfo_writeLine (L"ymax: ", Melder_double (ymax));
	MelderInfo_writeLine (L"Number of rows: ", Melder_integer (ny));
	MelderInfo_writeLine (L"dy: ", Melder_double (dy), L" (-> sampling rate ", Melder_double (1.0 / dy), L" )");
	MelderInfo_writeLine (L"y1: ", Melder_double (y1));
}

void structPhoto :: f_init
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1)
{
	structSampledXY :: f_init (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
	this -> d_red =          Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
	this -> d_green =        Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
	this -> d_blue =         Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
	this -> d_transparency = Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
}

Photo Photo_create
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1)
{
	try {
		autoPhoto me = Thing_new (Photo);
		my f_init (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix object not created.");
	}
}

Photo Photo_createSimple (long numberOfRows, long numberOfColumns) {
	try {
		autoPhoto me = Thing_new (Photo);
		my f_init (0.5, numberOfColumns + 0.5, numberOfColumns, 1, 1,
		           0.5, numberOfRows    + 0.5, numberOfRows,    1, 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Matrix object not created.");
	}
}

Photo Photo_readFromImageFile (MelderFile file) {
	autoPhoto me = NULL;
	#if defined (linux)
		(void) file;
		Melder_throw ("Cannot read image files on Linux yet. Try the Mac.");
		// NYI
	#elif defined (_WIN32)
		(void) file;
		Melder_throw ("Cannot read image files on Windows yet. Try the Mac.");
		// NYI
	#elif defined (macintosh)
		char utf8 [500];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, utf8);
		CFStringRef path = CFStringCreateWithCString (NULL, utf8, kCFStringEncodingUTF8);
		CFURLRef url = CFURLCreateWithFileSystemPath (NULL, path, kCFURLPOSIXPathStyle, false);
		CFRelease (path);
		CGImageSourceRef imageSource = CGImageSourceCreateWithURL (url, NULL);
		CFRelease (url);
		if (imageSource != NULL) {
			CGImageRef image = CGImageSourceCreateImageAtIndex (imageSource, 0, NULL);
			CFRelease (imageSource);
			if (image != NULL) {
				long width = CGImageGetWidth (image);
				long height = CGImageGetHeight (image);
				me.reset (Photo_createSimple (height, width));
				long bitsPerPixel = CGImageGetBitsPerPixel (image);
				long bitsPerComponent = CGImageGetBitsPerComponent (image);
				long bytesPerRow = CGImageGetBytesPerRow (image);
				trace ("%ld bits per pixel, %ld bits per component, %ld bytes per row", bitsPerPixel, bitsPerComponent, bytesPerRow);
				/*
				 * Now we probably need to use:
				 * CGColorSpaceRef CGImageGetColorSpace (CGImageRef image);
				 * CGImageAlphaInfo CGImageGetAlphaInfo (CGImageRef image);
				 */
				CGDataProviderRef dataProvider = CGImageGetDataProvider (image);   // not retained, so don't release
				CFDataRef data = CGDataProviderCopyData (dataProvider);
				uint8_t *pixelData = (uint8_t *) CFDataGetBytePtr (data);
				for (long irow = 1; irow <= height; irow ++) {
					uint8_t *rowAddress = pixelData + bytesPerRow * (height - irow);
					for (long icol = 1; icol <= width; icol ++) {
						my d_red -> z [irow] [icol] = (*rowAddress ++) / 255.0;
						my d_green -> z [irow] [icol] = (*rowAddress ++) / 255.0;
						my d_blue -> z [irow] [icol] = (*rowAddress ++) / 255.0;
						my d_transparency -> z [irow] [icol] = 1.0 - (*rowAddress ++) / 255.0;
					}
				}
				CFRelease (data);
				CGImageRelease (image);
			}
		}
	#endif
	return me.transfer();
}

#if defined (macintosh)
	#include <time.h>
	#include "macport_on.h"
	static void _mac_releaseDataCallback (void *info, const void *data, size_t size) {
		(void) info;
		(void) size;
		Melder_free (data);
	}
#endif

#ifdef macintosh
void structPhoto :: _mac_saveAsImageFile (MelderFile file, const void *which) {
		long bytesPerRow = this -> nx * 4;
		long numberOfRows = this -> ny;
		unsigned char *imageData = Melder_malloc_f (unsigned char, bytesPerRow * numberOfRows);
		for (long irow = 1; irow <= ny; irow ++) {
			uint8_t *rowAddress = imageData + bytesPerRow * (ny - irow);
			for (long icol = 1; icol <= nx; icol ++) {
				* rowAddress ++ = round (d_red          -> z [irow] [icol] * 255.0);
				* rowAddress ++ = round (d_green        -> z [irow] [icol] * 255.0);
				* rowAddress ++ = round (d_blue         -> z [irow] [icol] * 255.0);
				* rowAddress ++ = 255 - round (d_transparency -> z [irow] [icol] * 255.0);
			}
		}
		CGColorSpaceRef colourSpace = CGColorSpaceCreateWithName (kCGColorSpaceGenericRGB);   // used to be kCGColorSpaceUserRGB
		Melder_assert (colourSpace != NULL);
		CGDataProviderRef dataProvider = CGDataProviderCreateWithData (NULL,
			imageData,
			bytesPerRow * numberOfRows,
			_mac_releaseDataCallback   // needed?
		);
		Melder_assert (dataProvider != NULL);
		CGImageRef image = CGImageCreate (this -> nx, numberOfRows,
			8, 32, bytesPerRow, colourSpace, kCGImageAlphaNone, dataProvider, NULL, false, kCGRenderingIntentDefault);
		CGDataProviderRelease (dataProvider);
		Melder_assert (image != NULL);
		NSString *path = (NSString *) Melder_peekWcsToCfstring (Melder_fileToPath (file));
		CFURLRef url = (CFURLRef) [NSURL   fileURLWithPath: path   isDirectory: NO];
		CGImageDestinationRef destination = CGImageDestinationCreateWithURL (url, (CFStringRef) which, 1, NULL);
		CGImageDestinationAddImage (destination, image, nil);

		if (! CGImageDestinationFinalize (destination)) {
			//Melder_throw;
		}

		CFRelease (destination);
		CGColorSpaceRelease (colourSpace);
		CGImageRelease (image);
}
#endif

void structPhoto :: f_saveAsPNG (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypePNG);
	#endif
}

void structPhoto :: f_saveAsTIFF (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeTIFF);
	#endif
}

void structPhoto :: f_saveAsGIF (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeGIF);
	#endif
}

void structPhoto :: f_saveAsWindowsBitmapFile (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeBMP);
	#endif
}

void structPhoto :: f_saveAsJPEG (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeJPEG);
	#endif
}

void structPhoto :: f_saveAsJPEG2000 (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeJPEG2000);
	#endif
}

void structPhoto :: f_saveAsAppleIconFile (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeAppleICNS);
	#endif
}

void structPhoto :: f_saveAsWindowsIconFile (MelderFile file) {
	#ifdef macintosh
		_mac_saveAsImageFile (file, kUTTypeICO);
	#endif
}

void structPhoto :: f_replaceRed (Matrix red) {
	autoMatrix copy = Data_copy (red);
	forget (d_red);
	d_red = copy.transfer();
}

void structPhoto :: f_replaceGreen (Matrix green) {
	autoMatrix copy = Data_copy (green);
	forget (d_green);
	d_green = copy.transfer();
}

void structPhoto :: f_replaceBlue (Matrix blue) {
	autoMatrix copy = Data_copy (blue);
	forget (d_blue);
	d_blue = copy.transfer();
}

void structPhoto :: f_replaceTransparency (Matrix transparency) {
	autoMatrix copy = Data_copy (transparency);
	forget (d_transparency);
	d_transparency = copy.transfer();
}

static void cellArrayOrImage (Photo me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool interpolate) {
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (ymax <= ymin) { ymin = my ymin; ymax = my ymax; }
	long ixmin, ixmax, iymin, iymax;
	Sampled_getWindowSamples (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx, & ixmin, & ixmax);
	my f_getWindowSamplesY       (ymin - 0.49999 * my dy, ymax + 0.49999 * my dy, & iymin, & iymax);
	if (xmin >= xmax || ymin >= ymax) return;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	autoNUMmatrix <double_rgbt> z (iymin, iymax, ixmin, ixmax);
	for (long iy = iymin; iy <= iymax; iy ++) {
		for (long ix = ixmin; ix <= ixmax; ix ++) {
			z [iy] [ix]. red          = my d_red          -> z [iy] [ix];
			z [iy] [ix]. green        = my d_green        -> z [iy] [ix];
			z [iy] [ix]. blue         = my d_blue         -> z [iy] [ix];
			z [iy] [ix]. transparency = my d_transparency -> z [iy] [ix];
		}
	}
	if (interpolate)
		Graphics_image_colour (g, z.peek(),
			ixmin, ixmax, Matrix_columnToX (me, ixmin - 0.5), Matrix_columnToX (me, ixmax + 0.5),
			iymin, iymax, Matrix_rowToY (me, iymin - 0.5), Matrix_rowToY (me, iymax + 0.5), 0.0, 1.0);
	else
		Graphics_cellArray_colour (g, z.peek(),
			ixmin, ixmax, Matrix_columnToX (me, ixmin - 0.5), Matrix_columnToX (me, ixmax + 0.5),
			iymin, iymax, Matrix_rowToY (me, iymin - 0.5), Matrix_rowToY (me, iymax + 0.5), 0.0, 1.0);
	//Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	Graphics_unsetInner (g);
}

void structPhoto :: f_paintImage (Graphics g, double xmin, double xmax, double ymin, double ymax) {
	cellArrayOrImage (this, g, xmin, xmax, ymin, ymax, true);
}

void structPhoto :: f_paintCells (Graphics g, double xmin, double xmax, double ymin, double ymax) {
	cellArrayOrImage (this, g, xmin, xmax, ymin, ymax, false);
}

/* End of file Photo.cpp */
