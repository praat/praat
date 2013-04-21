/* Graphics_image.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
 * pb 2007/04/25 better image drawing on the Mac
 * pb 2007/08/03 Quartz
 * pb 2008/01/19 double
 * pb 2009/08/10 image from file
 * fb 2010/02/24 GTK
 * pb 2011/03/17 C++
 * pb 2012/04/21 on PostScript, minimal image resolution raised from 106 to 300 dpi
 * pb 2012/05/08 erased all QuickDraw
 */

#include "GraphicsP.h"

#if win
	#include <GdiPlus.h>
#elif mac
	#include <time.h>
	#include "macport_on.h"
	static void _mac_releaseDataCallback (void *info, const void *data, size_t size) {
		(void) info;
		(void) size;
		Melder_free (data);
	}
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

static void _GraphicsScreen_cellArrayOrImage (GraphicsScreen me, double **z_float, unsigned char **z_byte,
	long ix1, long ix2, long x1DC, long x2DC,
	long iy1, long iy2, long y1DC, long y2DC,
	double minimum, double maximum,
	long clipx1, long clipx2, long clipy1, long clipy2, int interpolate)
{
	/*long t=clock();*/
	long nx = ix2 - ix1 + 1;   /* The number of cells along the horizontal axis. */
	long ny = iy2 - iy1 + 1;   /* The number of cells along the vertical axis. */
	double dx = (double) (x2DC - x1DC) / (double) nx;   /* Horizontal pixels per cell. Positive. */
	double dy = (double) (y2DC - y1DC) / (double) ny;   /* Vertical pixels per cell. Negative. */
	double scale = 255.0 / (maximum - minimum), offset = 255.0 + minimum * scale;
	if (x2DC <= x1DC || y1DC <= y2DC) return;
	trace ("scale %f", scale);
	/* Clip by the intersection of the world window and the outline of the cells. */
	//Melder_casual ("clipy1 %ld clipy2 %ld", clipy1, clipy2);
	if (clipx1 < x1DC) clipx1 = x1DC;
	if (clipx2 > x2DC) clipx2 = x2DC;
	if (clipy1 > y1DC) clipy1 = y1DC;
	if (clipy2 < y2DC) clipy2 = y2DC;
	/*
	 * The first decision is whether we are going to use the standard rectangle drawing
	 * (cellArray only), or whether we are going to write into a bitmap.
	 * The standard drawing is best for small numbers of cells,
	 * provided that some cells are larger than a pixel.
	 */
	if (! interpolate && nx * ny < 3000 && (dx > 1.0 || dy < -1.0)) {
		try {
			/*unsigned int cellWidth = (unsigned int) dx + 1;*/
			unsigned int cellHeight = (unsigned int) (- (int) dy) + 1;
			long ix, iy;
			#if cairo
				cairo_pattern_t *grey [256];
				for (int igrey = 0; igrey < sizeof (grey) / sizeof (*grey); igrey ++) {
					double v = igrey / ((double) (sizeof (grey) / sizeof (*grey)) - 1.0);
					grey [igrey] = cairo_pattern_create_rgb (v, v, v);
				}
			#elif win
				static HBRUSH greyBrush [256];
				RECT rect;
				if (! greyBrush [0])
					for (int igrey = 0; igrey <= 255; igrey ++)
						greyBrush [igrey] = CreateSolidBrush (RGB (igrey, igrey, igrey));   // once
			#elif mac
				GraphicsQuartz_initDraw (me);
				CGContextSetAlpha (my d_macGraphicsContext, 1.0);
				CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeNormal);
			#endif
			autoNUMvector <long> lefts (ix1, ix2 + 1);
			for (ix = ix1; ix <= ix2 + 1; ix ++)
				lefts [ix] = x1DC + (long) ((ix - ix1) * dx);
			for (iy = iy1; iy <= iy2; iy ++) {
				long bottom = y1DC + (long) ((iy - iy1) * dy), top = bottom - cellHeight;
				if (top > clipy1 || bottom < clipy2) continue;
				if (top < clipy2) top = clipy2;
				if (bottom > clipy1) bottom = clipy1;
				#if win
					rect. bottom = bottom; rect. top = top;
				#endif
				for (ix = ix1; ix <= ix2; ix ++) {
					long left = lefts [ix], right = lefts [ix + 1];
					long value = offset - scale * ( z_float ? z_float [iy] [ix] : z_byte [iy] [ix] );
					if (right < clipx1 || left > clipx2) continue;
					if (left < clipx1) left = clipx1;
					if (right > clipx2) right = clipx2;
					#if cairo
						cairo_set_source (my d_cairoGraphicsContext, grey [value <= 0 ? 0 : value >= sizeof (grey) / sizeof (*grey) ? sizeof (grey) / sizeof (*grey) : value]);
						cairo_rectangle (my d_cairoGraphicsContext, left, top, right - left, bottom - top);
						cairo_fill (my d_cairoGraphicsContext);
					#elif win
						rect. left = left; rect. right = right;
						FillRect (my d_gdiGraphicsContext, & rect, greyBrush [value <= 0 ? 0 : value >= 255 ? 255 : value]);
					#elif mac
						double igrey = ( value <= 0 ? 0 : value >= 255 ? 255 : value ) / 255.0;
						CGContextSetRGBFillColor (my d_macGraphicsContext, igrey, igrey, igrey, 1.0);
						CGContextFillRect (my d_macGraphicsContext, CGRectMake (left, top, right - left, bottom - top));
					#endif
				}
			}
			
			#if cairo
				for (int igrey = 0; igrey < sizeof (grey) / sizeof (*grey); igrey ++)
					cairo_pattern_destroy (grey [igrey]);
				cairo_paint (my d_cairoGraphicsContext);
			#elif mac
				CGContextSetRGBFillColor (my d_macGraphicsContext, 0.0, 0.0, 0.0, 1.0);
				GraphicsQuartz_exitDraw (me);
			#endif
		} catch (MelderError) { }
	} else {
		long xDC, yDC;
		long undersampling = 1;
		/*
		 * Prepare for off-screen bitmap drawing.
		 */
		#if cairo
			long arrayWidth = clipx2 - clipx1;
			long arrayHeight = clipy1 - clipy2;
			trace ("arrayWidth %f, arrayHeight %f", (double) arrayWidth, (double) arrayHeight);
			// We're creating an alpha-only surface here (size is 1/4 compared to RGB24 and ARGB)
			// The grey values are reversed as we let the foreground colour shine through instead of blackening
			cairo_surface_t *sfc = cairo_image_surface_create (/*CAIRO_FORMAT_A8*/ CAIRO_FORMAT_RGB24, arrayWidth, arrayHeight);
			unsigned char *bits = cairo_image_surface_get_data (sfc);
			int scanLineLength = cairo_image_surface_get_stride (sfc);
			unsigned char grey [256];
			trace ("image surface address %p, bits address %p, scanLineLength %d, numberOfGreys %d", sfc, bits, scanLineLength, sizeof(grey)/sizeof(*grey));
			for (int igrey = 0; igrey < sizeof (grey) / sizeof (*grey); igrey++)
				grey [igrey] = 255 - (unsigned char) (igrey * 255.0 / (sizeof (grey) / sizeof (*grey) - 1));
		#elif win
			long bitmapWidth = clipx2 - clipx1, bitmapHeight = clipy1 - clipy2;
			int igrey;
			/*
			 * Create a device-independent bitmap, 8 pixels deep, for 256 greys.
			 */
			struct { BITMAPINFOHEADER header; RGBQUAD colours [256]; } bitmapInfo;
			long scanLineLength = (bitmapWidth + 3) & ~3L;
			HBITMAP bitmap;
			unsigned char *bits;
			bitmapInfo. header.biSize = sizeof (BITMAPINFOHEADER);
			bitmapInfo. header.biWidth = scanLineLength;
			bitmapInfo. header.biHeight = bitmapHeight;
			bitmapInfo. header.biPlanes = 1;
			bitmapInfo. header.biBitCount = 8;
			bitmapInfo. header.biCompression = 0;
			bitmapInfo. header.biSizeImage = 0;
			bitmapInfo. header.biXPelsPerMeter = 0;
			bitmapInfo. header.biYPelsPerMeter = 0;
			bitmapInfo. header.biClrUsed = 0;
			bitmapInfo. header.biClrImportant = 0;
			for (igrey = 0; igrey <= 255; igrey ++) {
				bitmapInfo. colours [igrey]. rgbRed = igrey;
				bitmapInfo. colours [igrey]. rgbGreen = igrey;
				bitmapInfo. colours [igrey]. rgbBlue = igrey;
			}
			bitmap = CreateDIBSection (my d_gdiGraphicsContext /* ignored */, (CONST BITMAPINFO *) & bitmapInfo,
				DIB_RGB_COLORS, (VOID **) & bits, NULL, 0);
		#elif mac
			long bytesPerRow = (clipx2 - clipx1) * 4;
			Melder_assert (bytesPerRow > 0);
			long numberOfRows = clipy1 - clipy2;
			Melder_assert (numberOfRows > 0);
			unsigned char *imageData = Melder_malloc_f (unsigned char, bytesPerRow * numberOfRows);
		#endif
		/*
		 * Draw into the bitmap.
		 */
		#if cairo
			#define ROW_START_ADDRESS  (bits + (clipy1 - 1 - yDC) * scanLineLength)
			#define PUT_PIXEL \
				if (1) { \
					unsigned char kar = value <= 0 ? 0 : value >= 255 ? 255 : (int) value; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = 0; \
				}
		#elif win
			#define ROW_START_ADDRESS  (bits + (clipy1 - 1 - yDC) * scanLineLength)
			#define PUT_PIXEL  *pixelAddress ++ = value <= 0 ? 0 : value >= 255 ? 255 : (int) value;
		#elif mac
			#define ROW_START_ADDRESS  (imageData + (clipy1 - 1 - yDC) * bytesPerRow)
			#define PUT_PIXEL \
				if (1) { \
					unsigned char kar = value <= 0 ? 0 : value >= 255 ? 255 : (int) value; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = 0; \
				}
		#endif
		if (interpolate) {
			try {
				autoNUMvector <long> ileft (clipx1, clipx2);
				autoNUMvector <long> iright (clipx1, clipx2);
				autoNUMvector <double> rightWeight (clipx1, clipx2);
				autoNUMvector <double> leftWeight (clipx1, clipx2);
				for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
					double ix_real = ix1 - 0.5 + ((double) nx * (xDC - x1DC)) / (x2DC - x1DC);
					ileft [xDC] = floor (ix_real), iright [xDC] = ileft [xDC] + 1;
					rightWeight [xDC] = ix_real - ileft [xDC], leftWeight [xDC] = 1.0 - rightWeight [xDC];
					if (ileft [xDC] < ix1) ileft [xDC] = ix1;
					if (iright [xDC] > ix2) iright [xDC] = ix2;
				}
				for (yDC = clipy2; yDC < clipy1; yDC += undersampling) {
					double iy_real = iy2 + 0.5 - ((double) ny * (yDC - y2DC)) / (y1DC - y2DC);
					long itop = ceil (iy_real), ibottom = itop - 1;
					double bottomWeight = itop - iy_real, topWeight = 1.0 - bottomWeight;
					unsigned char *pixelAddress = ROW_START_ADDRESS;
					if (itop > iy2) itop = iy2;
					if (ibottom < iy1) ibottom = iy1;
					if (z_float) {
						double *ztop = z_float [itop], *zbottom = z_float [ibottom];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double interpol =
								rightWeight [xDC] *
									(topWeight * ztop [iright [xDC]] + bottomWeight * zbottom [iright [xDC]]) +
								leftWeight [xDC] *
									(topWeight * ztop [ileft [xDC]] + bottomWeight * zbottom [ileft [xDC]]);
							double value = offset - scale * interpol;
							PUT_PIXEL
						}
					} else {
						unsigned char *ztop = z_byte [itop], *zbottom = z_byte [ibottom];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double interpol =
								rightWeight [xDC] *
									(topWeight * ztop [iright [xDC]] + bottomWeight * zbottom [iright [xDC]]) +
								leftWeight [xDC] *
									(topWeight * ztop [ileft [xDC]] + bottomWeight * zbottom [ileft [xDC]]);
							double value = offset - scale * interpol;
							PUT_PIXEL
						}
					}
				}
			} catch (MelderError) { Melder_clearError (); }
		} else {
			try {
				autoNUMvector <long> ix (clipx1, clipx2);
				for (xDC = clipx1; xDC < clipx2; xDC += undersampling)
					ix [xDC] = floor (ix1 + (nx * (xDC - x1DC)) / (x2DC - x1DC));
				for (yDC = clipy2; yDC < clipy1; yDC += undersampling) {
					long iy = ceil (iy2 - (ny * (yDC - y2DC)) / (y1DC - y2DC));
					unsigned char *pixelAddress = ROW_START_ADDRESS;
					Melder_assert (iy >= iy1 && iy <= iy2);
					if (z_float) {
						double *ziy = z_float [iy];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double value = offset - scale * ziy [ix [xDC]];
							PUT_PIXEL
						}
					} else {
						unsigned char *ziy = z_byte [iy];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double value = offset - scale * ziy [ix [xDC]];
							PUT_PIXEL
						}
					}
				}
			} catch (MelderError) { Melder_clearError (); }
		}
		/*
		 * Copy the bitmap to the screen.
		 */
		#if cairo
			cairo_matrix_t clip_trans;
			cairo_matrix_init_identity (& clip_trans);
			cairo_matrix_scale (& clip_trans, 1, -1);		// we painted in the reverse y-direction
			cairo_matrix_translate (& clip_trans, - clipx1, - clipy1);
			cairo_pattern_t *bitmap_pattern = cairo_pattern_create_for_surface (sfc);
			trace ("bitmap pattern %p", bitmap_pattern);
			if (cairo_status_t status = cairo_pattern_status (bitmap_pattern)) {
				Melder_casual ("bitmap pattern status: %s", cairo_status_to_string (status));
			} else {
				cairo_pattern_set_matrix (bitmap_pattern, & clip_trans);
				cairo_save (my d_cairoGraphicsContext);
				cairo_set_source (my d_cairoGraphicsContext, bitmap_pattern);
				cairo_paint (my d_cairoGraphicsContext);
				cairo_restore (my d_cairoGraphicsContext);
			}
			cairo_pattern_destroy (bitmap_pattern);
		#elif win
			SetDIBitsToDevice (my d_gdiGraphicsContext, clipx1, clipy2, bitmapWidth, bitmapHeight, 0, 0, 0, bitmapHeight,
				bits, (CONST BITMAPINFO *) & bitmapInfo, DIB_RGB_COLORS);
		#elif mac
			CGImageRef image;
			CGColorSpaceRef colourSpace = CGColorSpaceCreateWithName (kCGColorSpaceGenericRGB);   // used to be kCGColorSpaceUserRGB
			Melder_assert (colourSpace != NULL);
			if (1) {
				CGDataProviderRef dataProvider = CGDataProviderCreateWithData (NULL,
					imageData,
					bytesPerRow * numberOfRows,
					_mac_releaseDataCallback   // we need this because we cannot release the image data immediately after drawing,
						// because in PDF files the imageData has to stay available through EndPage
				);
				Melder_assert (dataProvider != NULL);
				image = CGImageCreate (clipx2 - clipx1, numberOfRows,
					8, 32, bytesPerRow, colourSpace, kCGImageAlphaNone, dataProvider, NULL, false, kCGRenderingIntentDefault);
				CGDataProviderRelease (dataProvider);
			} else if (0) {
				Melder_assert (CGBitmapContextCreate != NULL);
				CGContextRef bitmaptest = CGBitmapContextCreate (imageData, 100, 100,
					8, 800, colourSpace, 0);
				Melder_assert (bitmaptest != NULL);
				CGContextRef bitmap = CGBitmapContextCreate (NULL/*imageData*/, clipx2 - clipx1, numberOfRows,
					8, bytesPerRow, colourSpace, kCGImageAlphaLast);
				Melder_assert (bitmap != NULL);
				image = CGBitmapContextCreateImage (bitmap);
				// release bitmap?
			}
			Melder_assert (image != NULL);
			GraphicsQuartz_initDraw (me);
			CGContextDrawImage (my d_macGraphicsContext, CGRectMake (clipx1, clipy2, clipx2 - clipx1, clipy1 - clipy2), image);
			GraphicsQuartz_exitDraw (me);
			CGColorSpaceRelease (colourSpace);
			CGImageRelease (image);
		#endif
		/*
		 * Clean up.
		 */
		#if cairo
			cairo_surface_destroy (sfc);
		#elif win
			DeleteBitmap (bitmap);
		#endif
	}
	#if win
		end:
		return;
	#endif
}

static void _GraphicsPostscript_cellArrayOrImage (GraphicsPostscript me, double **z_float, unsigned char **z_byte,
	long ix1, long ix2, long x1DC, long x2DC,
	long iy1, long iy2, long y1DC, long y2DC,
	double minimum, double maximum,
	long clipx1, long clipx2, long clipy1, long clipy2, int interpolate)
{
	long interpolateX = 1, interpolateY = 1;
	long nx = ix2 - ix1 + 1, ny = iy2 - iy1 + 1, filling = 0;
	double scale = ( my photocopyable ? 200.1f : 255.1f ) / (maximum - minimum);
	double offset = 255.1f + minimum * scale;
	int minimalGrey = my photocopyable ? 55 : 0;
	my d_printf (my d_file, "gsave N %ld %ld M %ld %ld L %ld %ld L %ld %ld L closepath clip\n",
		clipx1, clipy1, clipx2 - clipx1, 0L, 0L, clipy2 - clipy1, clipx1 - clipx2, 0L);
	my d_printf (my d_file, "%ld %ld translate %ld %ld scale\n",
		x1DC, y1DC, x2DC - x1DC, y2DC - y1DC);
	if (interpolate) {
		/* The smallest image resolution is 300 dpi. If a sample takes up more than 25.4/300 mm, the 300 dpi resolution is achieved by interpolation. */
		const double smallestImageResolution = 300.0;
		double colSize_pixels = (double) (x2DC - x1DC) / nx;
		double rowSize_pixels = (double) (y2DC - y1DC) / ny;
		double colSize_inches = colSize_pixels / my resolution;
		double rowSize_inches = rowSize_pixels / my resolution;
		interpolateX = ceil (colSize_inches * smallestImageResolution);   // number of interpolation points per horizontal sample
		interpolateY = ceil (rowSize_inches * smallestImageResolution);   // number of interpolation points per vertical sample
	}

	if (interpolateX <= 1 && interpolateY <= 1) {
		/* Do not interpolate. */
		my d_printf (my d_file, "/picstr %ld string def %ld %ld 8 [%ld 0 0 %ld 0 0]\n"
			"{ currentfile picstr readhexstring pop } image\n",
			nx, nx, ny, nx, ny);
	} else if (interpolateX > 1 && interpolateY > 1) {
		/* Interpolate both horizontally and vertically. */
		long nx_new = nx * interpolateX;
		long ny_new = ny * interpolateY;
		/* Interpolation between rows requires us to remember two original rows: */
		my d_printf (my d_file, "/lorow %ld string def /hirow %ld string def\n", nx, nx);
		/* New rows (scanlines) are longer: */
		my d_printf (my d_file, "/scanline %ld string def\n", nx_new);
		/* The first four arguments to the 'image' command,
		/* namely the new number of columns, the new number of rows, the bit depth, and the matrix: */
		my d_printf (my d_file, "%ld %ld 8 [%ld 0 0 %ld 0 0]\n", nx_new, ny_new, nx_new, ny_new);
		/* Since our imageproc is going to output only one scanline at a time, */
		/* the outer loop variable (scanline number) has to be initialized outside the imageproc: */
		my d_printf (my d_file, "/irow 0 def\n");
		/* The imageproc starts here. First, we fill one or two original rows if necessary; */
		/* they are read as hexadecimal strings from the current file, i.e. just after the image command. */
		my d_printf (my d_file, "{\n"
			/* First test: are we in the first scanline? If so, read two original rows: */
			"irow 0 eq { currentfile lorow readhexstring pop pop lorow hirow copy pop } if\n"
			/* Second test: did we just pass an original data row? */
			/* If so, */
			/*    (1) move that row backwards; */
			/*    (2) read a new one unless we just passed the last original row: */
			"irow %ld mod %ld eq { hirow lorow copy pop\n"
			"irow %ld ne { currentfile hirow readhexstring pop pop } if } if\n",
			interpolateY, interpolateY / 2, ny_new - interpolateY + interpolateY / 2);
		/* Where are we between those two rows? */
		my d_printf (my d_file, "/rowphase irow %ld add %ld mod %ld div def\n",
			interpolateY - interpolateY / 2, interpolateY, interpolateY);
		/* Inner loop starts here. It cycles through all new columns: */
		my d_printf (my d_file, "0 1 %ld {\n", nx_new - 1);
		/* Get the inner loop variable: */
		my d_printf (my d_file, "   /icol exch def\n");
		/* Where are the two original columns? */
		my d_printf (my d_file, "   /locol icol %ld sub %ld idiv def\n", interpolateX / 2, interpolateX);
		my d_printf (my d_file, "   /hicol icol %ld ge { %ld } { icol %ld add %ld idiv } ifelse def\n",
			nx_new - interpolateX / 2, nx - 1, interpolateX / 2, interpolateX);
		/* Where are we between those two columns? */
		my d_printf (my d_file, "   /colphase icol %ld add %ld mod %ld div def\n",
			interpolateX - interpolateX / 2, interpolateX, interpolateX);
		/* Four-point interpolation: */
		my d_printf (my d_file,
			"   /plow lorow locol get def\n"
			"   /phigh lorow hicol get def\n"
			"   /qlow hirow locol get def\n"
			"   /qhigh hirow hicol get def\n"
			"   /value\n"
			"      plow phigh plow sub colphase mul add 1 rowphase sub mul\n"
			"      qlow qhigh qlow sub colphase mul add rowphase mul\n"
			"      add def\n"
			"   scanline icol value 0 le { 0 } { value 255 ge { 255 } { value } ifelse } ifelse cvi put\n"
			"} for\n"
			"/irow irow 1 add def scanline } image\n");
	} else if (interpolateX > 1) {
		/* Interpolate horizontally only. */
		long nx_new = nx * interpolateX;
		/* Remember one original row: */
		my d_printf (my d_file, "/row %ld string def\n", nx, nx);
		/* New rows (scanlines) are longer: */
		my d_printf (my d_file, "/scanline %ld string def\n", nx_new);
		/* The first four arguments to the 'image' command,
		/* namely the new number of columns, the number of rows, the bit depth, and the matrix: */
		my d_printf (my d_file, "%ld %ld 8 [%ld 0 0 %ld 0 0]\n", nx_new, ny, nx_new, ny);
		/* The imageproc starts here. We fill one original row. */
		my d_printf (my d_file, "{\n"
			"currentfile row readhexstring pop pop\n");
		/* Loop starts here. It cycles through all new columns: */
		my d_printf (my d_file, "0 1 %ld {\n", nx_new - 1);
		/* Get the loop variable: */
		my d_printf (my d_file, "   /icol exch def\n");
		/* Where are the two original columns? */
		my d_printf (my d_file, "   /locol icol %ld sub %ld idiv def\n", interpolateX / 2, interpolateX);
		my d_printf (my d_file, "   /hicol icol %ld ge { %ld } { icol %ld add %ld idiv } ifelse def\n",
			nx_new - interpolateX / 2, nx - 1, interpolateX / 2, interpolateX);
		/* Where are we between those two columns? */
		my d_printf (my d_file, "   /colphase icol %ld add %ld mod %ld div def\n",
			interpolateX - interpolateX / 2, interpolateX, interpolateX);
		/* Two-point interpolation: */
		my d_printf (my d_file,
			"   /plow row locol get def\n"
			"   /phigh row hicol get def\n"
			"   /value plow phigh plow sub colphase mul add def\n"
			"   scanline icol value 0 le { 0 } { value 255 ge { 255 } { value } ifelse } ifelse cvi put\n"
			"} for\n"
			"scanline } image\n");
	} else {
		/* Interpolate vertically only. */
		long ny_new = ny * interpolateY;
		/* Interpolation between rows requires us to remember two original rows: */
		my d_printf (my d_file, "/lorow %ld string def /hirow %ld string def\n", nx, nx);
		/* New rows (scanlines) are equally long: */
		my d_printf (my d_file, "/scanline %ld string def\n", nx);
		/* The first four arguments to the 'image' command,
		/* namely the number of columns, the new number of rows, the bit depth, and the matrix: */
		my d_printf (my d_file, "%ld %ld 8 [%ld 0 0 %ld 0 0]\n", nx, ny_new, nx, ny_new);
		/* Since our imageproc is going to output only one scanline at a time, */
		/* the outer loop variable (scanline number) has to be initialized outside the imageproc: */
		my d_printf (my d_file, "/irow 0 def\n");
		/* The imageproc starts here. First, we fill one or two original rows if necessary; */
		/* they are read as hexadecimal strings from the current file, i.e. just after the image command. */
		my d_printf (my d_file, "{\n"
			/* First test: are we in the first scanline? If so, read two original rows: */
			"irow 0 eq { currentfile lorow readhexstring pop pop lorow hirow copy pop } if\n"
			/* Second test: did we just pass an original data row? */
			/* If so, */
			/*    (1) move that row backwards; */
			/*    (2) read a new one unless we just passed the last original row: */
			"irow %ld mod %ld eq { hirow lorow copy pop\n"
			"irow %ld ne { currentfile hirow readhexstring pop pop } if } if\n",
			interpolateY, interpolateY / 2, ny_new - interpolateY + interpolateY / 2);
		/* Where are we between those two rows? */
		my d_printf (my d_file, "/rowphase irow %ld add %ld mod %ld div def\n",
			interpolateY - interpolateY / 2, interpolateY, interpolateY);
		/* Inner loop starts here. It cycles through all columns: */
		my d_printf (my d_file, "0 1 %ld {\n", nx - 1);
		/* Get the inner loop variable: */
		my d_printf (my d_file, "   /icol exch def\n");
		/* Two-point interpolation: */
		my d_printf (my d_file,
			"   /p lorow icol get def\n"
			"   /q hirow icol get def\n"
			"   /value\n"
			"      p 1 rowphase sub mul\n"
			"      q rowphase mul\n"
			"      add def\n"
			"   scanline icol value 0 le { 0 } { value 255 ge { 255 } { value } ifelse } ifelse cvi put\n"
			"} for\n"
			"/irow irow 1 add def scanline } image\n");
	}
	for (long iy = iy1; iy <= iy2; iy ++) for (long ix = ix1; ix <= ix2; ix ++) {
		int value = (int) (offset - scale * ( z_float ? z_float [iy] [ix] : z_byte [iy] [ix] ));
		my d_printf (my d_file, "%.2x", value <= minimalGrey ? minimalGrey : value >= 255 ? 255 : value);
		if (++ filling == 39) { my d_printf (my d_file, "\n"); filling = 0; }
	}
	if (filling) my d_printf (my d_file, "\n");
	my d_printf (my d_file, "grestore\n");
}

static void _cellArrayOrImage (Graphics me, double **z_float, unsigned char **z_byte,
	long ix1, long ix2, long x1DC, long x2DC,
	long iy1, long iy2, long y1DC, long y2DC, double minimum, double maximum,
	long clipx1, long clipx2, long clipy1, long clipy2, int interpolate)
{
	if (my screen) {
		_GraphicsScreen_cellArrayOrImage (static_cast <GraphicsScreen> (me), z_float, z_byte, ix1, ix2, x1DC, x2DC, iy1, iy2, y1DC, y2DC,
			minimum, maximum, clipx1, clipx2, clipy1, clipy2, interpolate);
	} else if (my postScript) {
		_GraphicsPostscript_cellArrayOrImage (static_cast <GraphicsPostscript> (me), z_float, z_byte, ix1, ix2, x1DC, x2DC, iy1, iy2, y1DC, y2DC,
			minimum, maximum, clipx1, clipx2, clipy1, clipy2, interpolate);
	}
	_Graphics_setColour (me, my colour);
}

static void cellArrayOrImage (I, double **z_float, unsigned char **z_byte,
	long ix1, long ix2, double x1WC, double x2WC,
	long iy1, long iy2, double y1WC, double y2WC,
	double minimum, double maximum, int interpolate)
{
	iam (Graphics);
	if (ix2 < ix1 || iy2 < iy1 || minimum == maximum) return;
	_cellArrayOrImage (me, z_float, z_byte,
		ix1, ix2, wdx (x1WC), wdx (x2WC),
		iy1, iy2, wdy (y1WC), wdy (y2WC), minimum, maximum,
		wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), interpolate);
	if (my recording) {
		long nrow = iy2 - iy1 + 1, ncol = ix2 - ix1 + 1, ix, iy;
		op (interpolate ? ( z_float ? IMAGE : IMAGE8 ) :
			 (z_float ? CELL_ARRAY : CELL_ARRAY8 ), 8 + nrow * ncol);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC); put (minimum); put (maximum);
		put (nrow); put (ncol);
		if (z_float) for (iy = iy1; iy <= iy2; iy ++)
			{ double *row = z_float [iy]; for (ix = ix1; ix <= ix2; ix ++) put (row [ix]); }
		else for (iy = iy1; iy <= iy2; iy ++)
			{ unsigned char *row = z_byte [iy]; for (ix = ix1; ix <= ix2; ix ++) put (row [ix]); }
	}
}

void Graphics_cellArray (Graphics me, double **z, long ix1, long ix2, double x1WC, double x2WC,
	long iy1, long iy2, double y1WC, double y2WC, double minimum, double maximum)
{ cellArrayOrImage (me, z, NULL, ix1, ix2, x1WC, x2WC, iy1, iy2, y1WC, y2WC, minimum, maximum, FALSE); }

void Graphics_cellArray8 (Graphics me, unsigned char **z, long ix1, long ix2, double x1WC, double x2WC,
	long iy1, long iy2, double y1WC, double y2WC, unsigned char minimum, unsigned char maximum)
{ cellArrayOrImage (me, NULL, z, ix1, ix2, x1WC, x2WC, iy1, iy2, y1WC, y2WC, minimum, maximum, FALSE); }

void Graphics_image (Graphics me, double **z, long ix1, long ix2, double x1WC, double x2WC,
	long iy1, long iy2, double y1WC, double y2WC, double minimum, double maximum)
{ cellArrayOrImage (me, z, NULL, ix1, ix2, x1WC, x2WC, iy1, iy2, y1WC, y2WC, minimum, maximum, TRUE); }

void Graphics_image8 (Graphics me, unsigned char **z, long ix1, long ix2, double x1WC, double x2WC,
	long iy1, long iy2, double y1WC, double y2WC, unsigned char minimum, unsigned char maximum)
{ cellArrayOrImage (me, NULL, z, ix1, ix2, x1WC, x2WC, iy1, iy2, y1WC, y2WC, minimum, maximum, TRUE); }

static void _GraphicsScreen_imageFromFile (GraphicsScreen me, const wchar_t *relativeFileName, double x1, double x2, double y1, double y2) {
	long x1DC = wdx (x1), x2DC = wdx (x2), y1DC = wdy (y1), y2DC = wdy (y2);
	long width = x2DC - x1DC, height = my yIsZeroAtTheTop ? y1DC - y2DC : y2DC - y1DC;
	#if win
		if (my d_useGdiplus) {
			structMelderFile file;
			Melder_relativePathToFile (relativeFileName, & file);
			Gdiplus::Image image (file. path);
			Gdiplus::Graphics dcplus (my d_gdiGraphicsContext);
			if (x1 == x2 && y1 == y2) {
				width = image. GetWidth (), x1DC -= width / 2, x2DC = x1DC + width;
				height = image. GetHeight (), y2DC -= height / 2, y1DC = y2DC + height;
			} else if (x1 == x2) {
				width = height * (double) image. GetWidth () / (double) image. GetHeight ();
				x1DC -= width / 2, x2DC = x1DC + width;
			} else if (y1 == y2) {
				height = width * (double) image. GetHeight () / (double) image. GetWidth ();
				y2DC -= height / 2, y1DC = y2DC + height;
			}
			Gdiplus::Rect rect (x1DC, y2DC, width, height);
			dcplus. DrawImage (& image, rect);
		}
	#elif mac
		structMelderFile file;
		Melder_relativePathToFile (relativeFileName, & file);
		char utf8 [500];
		Melder_wcsTo8bitFileRepresentation_inline (file. path, utf8);
		CFStringRef path = CFStringCreateWithCString (NULL, utf8, kCFStringEncodingUTF8);
		CFURLRef url = CFURLCreateWithFileSystemPath (NULL, path, kCFURLPOSIXPathStyle, false);
		CFRelease (path);
		CGImageSourceRef imageSource = CGImageSourceCreateWithURL (url, NULL);
		CFRelease (url);
		if (imageSource != NULL) {
			CGImageRef image = CGImageSourceCreateImageAtIndex (imageSource, 0, NULL);
			CFRelease (imageSource);
			if (image != NULL) {
				if (x1 == x2 && y1 == y2) {
					width = CGImageGetWidth (image), x1DC -= width / 2, x2DC = x1DC + width;
					height = CGImageGetHeight (image), y2DC -= height / 2, y1DC = y2DC + height;
				} else if (x1 == x2) {
					width = height * (double) CGImageGetWidth (image) / (double) CGImageGetHeight (image);
					x1DC -= width / 2, x2DC = x1DC + width;
				} else if (y1 == y2) {
					height = width * (double) CGImageGetHeight (image) / (double) CGImageGetWidth (image);
					y2DC -= height / 2, y1DC = y2DC + height;
				}
				GraphicsQuartz_initDraw (me);
				CGContextSaveGState (my d_macGraphicsContext);
                
                NSCAssert(my d_macGraphicsContext, @"nil context");

				CGContextTranslateCTM (my d_macGraphicsContext, 0, y1DC);
				CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
				CGContextDrawImage (my d_macGraphicsContext, CGRectMake (x1DC, 0, width, height), image);
				CGContextRestoreGState (my d_macGraphicsContext);
				GraphicsQuartz_exitDraw (me);
				CGImageRelease (image);
			}
		}
	#endif
}

void Graphics_imageFromFile (Graphics me, const wchar_t *relativeFileName, double x1, double x2, double y1, double y2) {
	if (my screen) {
		_GraphicsScreen_imageFromFile (static_cast <GraphicsScreen> (me), relativeFileName, x1, x2, y1, y2);
	}
	if (my recording) {
		char *txt_utf8 = Melder_peekWcsToUtf8 (relativeFileName);
		int length = strlen (txt_utf8) / sizeof (double) + 1;
		op (IMAGE_FROM_FILE, 5 + length); put (x1); put (x2); put (y1); put (y2); sput (txt_utf8, length)
	}
}

/* End of file Graphics_image.cpp */
