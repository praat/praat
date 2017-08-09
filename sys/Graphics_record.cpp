/* Graphics_record.cpp
 *
 * Copyright (C) 1992-2011,2013,2014,2015,2016,2017 Paul Boersma
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

#define RECORDING_HEADER_LENGTH 2

double * _Graphics_check (Graphics me, long number) {
	static bool messageHasAlreadyBeenShownOnce = false;
	double *result = nullptr;
	double *record = my record;
	long nrecord = my nrecord;
	if (nrecord == 0) {
		nrecord = 1000;
		try {
			record = Melder_malloc (double, 1 + nrecord);
		} catch (MelderError) {
			if (messageHasAlreadyBeenShownOnce) {
				Melder_clearError ();
			} else {
				messageHasAlreadyBeenShownOnce = true;
				Melder_flushError (U"_Graphics_growRecorder: out of memory.\n"
					U"This message will not show up on future occasions.");   // because of loop danger when redrawing
			}
			return nullptr;
		}
		my record = record; my nrecord = nrecord;
	}
	if (nrecord < my irecord + RECORDING_HEADER_LENGTH + number) {
		while (nrecord < my irecord + RECORDING_HEADER_LENGTH + number) nrecord *= 2;
		try {
			record = (double *) Melder_realloc (record, (1 + nrecord) * sizeof (double));
		} catch (MelderError) {
			if (messageHasAlreadyBeenShownOnce) {
				Melder_clearError ();
			} else {
				messageHasAlreadyBeenShownOnce = true;
				Melder_flushError (U"_Graphics_growRecorder: out of memory.\n"
					U"This message will not show up on future occasions.");   // because of loop danger when redrawing
			}
			return nullptr;
		}
		my record = record; my nrecord = nrecord;
	}
	result = my record + my irecord;
	my irecord += number + RECORDING_HEADER_LENGTH;
	return result;
}

/***** RECORD AND PLAY *****/

bool Graphics_startRecording (Graphics me) {
	bool wasRecording = my recording;
	my recording = true;
	return wasRecording;
}

bool Graphics_stopRecording (Graphics me) {
	bool wasRecording = my recording;
	my recording = false;
	return wasRecording;
}

void Graphics_clearRecording (Graphics me) {
	if (my record) {
		Melder_free (my record);
		my irecord = 0;
		my nrecord = 0;
	}
}

void Graphics_play (Graphics me, Graphics thee) {
	double *p = my record, *endp = p + my irecord;
	bool wasRecording = my recording;
	if (! p) return;
	my recording = false;   // temporarily, in case me == thee
	while (p < endp) {
		#define get  (* ++ p)
		#define mget(n)  (p += n, p - n)
		#define sget(n)  ((char *) (p += n, p - n + 1))
		int opcode = (int) get;
		(void) (long) get;   // ignore number of arguments
		switch (opcode) {
			case SET_VIEWPORT:
			{  double x1NDC = get, x2NDC = get, y1NDC = get, y2NDC = get;
				Graphics_setViewport (thee, x1NDC, x2NDC, y1NDC, y2NDC);
			}  break;
			case SET_INNER: Graphics_setInner (thee); break;
			case UNSET_INNER: Graphics_unsetInner (thee); break;
			case SET_WINDOW:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_setWindow (thee, x1, x2, y1, y2);
			}  break;
			case TEXT:
			{  double x = get, y = get; long length = get; char *text_utf8 = sget (length);
				Graphics_text (thee, x, y, Melder_peek8to32 (text_utf8));
			}  break;
			case POLYLINE:
			{  long n = get; double *x = mget (n), *y = mget (n);
				Graphics_polyline (thee, n, & x [1], & y [1]);
			} break;
			case LINE:
			{  double x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_line (thee, x1, y1, x2, y2);
			}  break;
			case ARROW:
			{  double x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_arrow (thee, x1, y1, x2, y2);
			}  break;
			case FILL_AREA:
			{  long n = get; double *x = mget (n), *y = mget (n);
				Graphics_fillArea (thee, n, & x [1], & y [1]);
			}  break;
			case FUNCTION:
			{  long n = get; double x1 = get, x2 = get, *y = mget (n);
				Graphics_function (thee, y, 1, n, x1, x2);
			}  break;
			case RECTANGLE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_rectangle (thee, x1, x2, y1, y2);
			}  break;
			case FILL_RECTANGLE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_fillRectangle (thee, x1, x2, y1, y2);
			}  break;
			case CIRCLE:
			{  double x = get, y = get, r = get;
				Graphics_circle (thee, x, y, r);
			}  break;
			case FILL_CIRCLE:
			{  double x = get, y = get, r = get;
				Graphics_fillCircle (thee, x, y, r);
			}  break;
			case ARC:
			{  double x = get, y = get, r = get, fromAngle = get, toAngle = get;
				Graphics_arc (thee, x, y, r, fromAngle, toAngle);
			}  break;
			case ARC_ARROW:
			{  double x = get, y = get, r = get, fromAngle = get, toAngle = get;
				int arrowAtStart = get, arrowAtEnd = get;
				Graphics_arcArrow (thee, x, y, r, fromAngle, toAngle, arrowAtStart, arrowAtEnd);
			}  break;
			case HIGHLIGHT:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_highlight (thee, x1, x2, y1, y2);
			}  break;
			case CELL_ARRAY:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get;
				/*
				 * We don't copy all the data into a new matrix.
				 * Instead, we create row pointers z [1..nrow] that point directly into the recorded data.
				 * This works because the data is a packed array of double, just as Graphics_cellArray expects.
				 */
				double **z = Melder_malloc_f (double *, nrow);
				z [0] = p + 1;
				for (long irow = 1; irow < nrow; irow ++) z [irow] = z [irow - 1] + ncol;
				p += nrow * ncol;
				Graphics_cellArray (thee, z, 0, ncol - 1, x1, x2,
								0, nrow - 1, y1, y2, minimum, maximum);
				Melder_free (z);
			}  break;
			case SET_FONT: Graphics_setFont (thee, (enum kGraphics_font) get); break;
			case SET_FONT_SIZE: Graphics_setFontSize (thee, (int) get); break;
			case SET_FONT_STYLE: Graphics_setFontStyle (thee, (int) get); break;
			case SET_TEXT_ALIGNMENT:
			{  int hor = get, vert = get;
				Graphics_setTextAlignment (thee, hor, vert);
			}  break;
			case SET_TEXT_ROTATION: Graphics_setTextRotation (thee, get); break;
			case SET_LINE_TYPE: Graphics_setLineType (thee, (int) get); break;
			case SET_LINE_WIDTH: Graphics_setLineWidth (thee, get); break;
			case SET_STANDARD_COLOUR:   // only used in old Praat picture files
			{  int standardColour = (int) get;
				Graphics_Colour colour =
					standardColour == 0 ? Graphics_BLACK :
					standardColour == 1 ? Graphics_WHITE :
					standardColour == 2 ? Graphics_RED :
					standardColour == 3 ? Graphics_GREEN :
					standardColour == 4 ? Graphics_BLUE :
					standardColour == 5 ? Graphics_CYAN :
					standardColour == 6 ? Graphics_MAGENTA :
					standardColour == 7 ? Graphics_YELLOW :
					standardColour == 8 ? Graphics_MAROON :
					standardColour == 9 ? Graphics_LIME :
					standardColour == 10 ? Graphics_NAVY :
					standardColour == 11 ? Graphics_TEAL :
					standardColour == 12 ? Graphics_PURPLE :
					standardColour == 13 ? Graphics_OLIVE :
					standardColour == 14 ? Graphics_PINK :
					standardColour == 15 ? Graphics_SILVER :
					Graphics_GREY;
				Graphics_setColour (thee, colour);
			} break;
			case SET_GREY: Graphics_setGrey (thee, get); break;
			case MARK_GROUP: Graphics_markGroup (thee); break;
			case ELLIPSE: {
				double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_ellipse (thee, x1, x2, y1, y2);
			} break;
			case FILL_ELLIPSE: {
				double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_fillEllipse (thee, x1, x2, y1, y2);
			} break;
			case CIRCLE_MM:
			{  double x = get, y = get, d = get;
				Graphics_circle_mm (thee, x, y, d);
			}  break;
			case FILL_CIRCLE_MM:
			{  double x = get, y = get, d = get;
				Graphics_fillCircle_mm (thee, x, y, d);
			}  break;
			case IMAGE8:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get;
				unsigned char **z = NUMmatrix <unsigned char> (1, nrow, 1, ncol);   // BUG memory
				for (long irow = 1; irow <= nrow; irow ++)
					for (long icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = get;
				Graphics_image8 (thee, z, 1, ncol, x1, x2, 1, nrow, y1, y2, minimum, maximum);
				NUMmatrix_free (z, 1, 1);
			}  break;
			case UNHIGHLIGHT:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_unhighlight (thee, x1, x2, y1, y2);
			}  break;
#if motif
			case XOR_ON:
			{  Graphics_Colour colour; colour. red = get, colour. green = get, colour. blue = get;
				Graphics_xorOn (thee, colour);
			}  break;
			case XOR_OFF: Graphics_xorOff (thee); break;
#endif
			case RECTANGLE_MM:
			{  double x = get, y = get, horSide = get, vertSide = get;
				Graphics_rectangle_mm (thee, x, y, horSide, vertSide);
			}  break;
			case FILL_RECTANGLE_MM:
			{  double x = get, y = get, horSide = get, vertSide = get;
				Graphics_fillRectangle_mm (thee, x, y, horSide, vertSide);
			}  break;
			case SET_WS_WINDOW:
			{  double x1NDC = get, x2NDC = get, y1NDC = get, y2NDC = get;
				Graphics_setWsWindow (thee, x1NDC, x2NDC, y1NDC, y2NDC);
			}  break;
			case SET_WRAP_WIDTH: Graphics_setWrapWidth (thee, get); break;
			case SET_SECOND_INDENT: Graphics_setSecondIndent (thee, get); break;
			case SET_PERCENT_SIGN_IS_ITALIC: Graphics_setPercentSignIsItalic (thee, (bool) get); break;
			case SET_NUMBER_SIGN_IS_BOLD: Graphics_setNumberSignIsBold (thee, (bool) get); break;
			case SET_CIRCUMFLEX_IS_SUPERSCRIPT: Graphics_setCircumflexIsSuperscript (thee, (bool) get); break;
			case SET_UNDERSCORE_IS_SUBSCRIPT: Graphics_setUnderscoreIsSubscript (thee, (bool) get); break;
			case SET_DOLLAR_SIGN_IS_CODE: Graphics_setDollarSignIsCode (thee, (bool) get); break;
			case SET_AT_SIGN_IS_LINK: Graphics_setAtSignIsLink (thee, (bool) get); break;
			case BUTTON:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_button (thee, x1, x2, y1, y2);
			}  break;
			case ROUNDED_RECTANGLE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, r = get;
				Graphics_roundedRectangle (thee, x1, x2, y1, y2, r);
			}  break;
			case FILL_ROUNDED_RECTANGLE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, r = get;
				Graphics_fillRoundedRectangle (thee, x1, x2, y1, y2, r);
			}  break;
			case FILL_ARC:
			{  double x = get, y = get, r = get, fromAngle = get, toAngle = get;
				Graphics_fillArc (thee, x, y, r, fromAngle, toAngle);
			}  break;
			case INNER_RECTANGLE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_innerRectangle (thee, x1, x2, y1, y2);
			}  break;
			case CELL_ARRAY8:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get;
				unsigned char **z = NUMmatrix <unsigned char> (1, nrow, 1, ncol);   // BUG memory
				for (long irow = 1; irow <= nrow; irow ++)
					for (long icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = get;
				Graphics_cellArray8 (thee, z, 1, ncol, x1, x2, 1, nrow, y1, y2, minimum, maximum);
				NUMmatrix_free (z, 1, 1);
			}  break;
			case IMAGE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get;
				/*
				 * We don't copy all the data into a new matrix.
				 * Instead, we create row pointers z [1..nrow] that point directly into the recorded data.
				 * This works because the data is a packed array of double, just as Graphics_image expects.
				 */
				double **z = Melder_malloc_f (double *, nrow);
				z [0] = p + 1;
				for (long irow = 1; irow < nrow; irow ++) z [irow] = z [irow - 1] + ncol;
				p += nrow * ncol;
				Graphics_image (thee, z, 0, ncol - 1, x1, x2,
								0, nrow - 1, y1, y2, minimum, maximum);
				Melder_free (z);
			}  break;
			case HIGHLIGHT2:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, innerX1 = get, innerX2 = get, innerY1 = get, innerY2 = get;
				Graphics_highlight2 (thee, x1, x2, y1, y2, innerX1, innerX2, innerY1, innerY2);
			}  break;
			case UNHIGHLIGHT2:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, innerX1 = get, innerX2 = get, innerY1 = get, innerY2 = get;
				Graphics_unhighlight2 (thee, x1, x2, y1, y2, innerX1, innerX2, innerY1, innerY2);
			}  break;
			case SET_ARROW_SIZE: Graphics_setArrowSize (thee, get); break;
			case DOUBLE_ARROW:
			{  double x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_doubleArrow (thee, x1, y1, x2, y2);
			}  break;
			case SET_RGB_COLOUR:
			{  Graphics_Colour colour; colour. red = get, colour. green = get, colour. blue = get;
				Graphics_setColour (thee, colour);
			} break;
			case IMAGE_FROM_FILE:
			{  double x1 = get, x2 = get, y1 = get, y2 = get; long length = get; char *text_utf8 = sget (length);
				Graphics_imageFromFile (thee, Melder_peek8to32 (text_utf8), x1, x2, y1, y2);
			}  break;
			case POLYLINE_CLOSED:
			{  long n = get; double *x = mget (n), *y = mget (n);
				Graphics_polyline_closed (thee, n, & x [1], & y [1]);
			} break;
			case CELL_ARRAY_COLOUR:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get;
				/*
				 * We don't copy all the data into a new matrix.
				 * Instead, we create row pointers z [1..nrow] that point directly into the recorded data.
				 * This works because the data is a packed array of double_rgbt, just as Graphics_cellArray_colour expects.
				 */
				double_rgbt **z = Melder_malloc_f (double_rgbt *, nrow);
				z [0] = (double_rgbt *) (p + 1);
				for (long irow = 1; irow < nrow; irow ++) z [irow] = z [irow - 1] + ncol;
				p += nrow * ncol * 4;
				Graphics_cellArray_colour (thee, z, 0, ncol - 1, x1, x2,
								0, nrow - 1, y1, y2, minimum, maximum);
				Melder_free (z);
			}  break;
			case IMAGE_COLOUR:
			{  double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get;
				/*
				 * We don't copy all the data into a new matrix.
				 * Instead, we create row pointers z [1..nrow] that point directly into the recorded data.
				 * This works because the data is a packed array of double_rgbt, just as Graphics_image_colour expects.
				 */
				double_rgbt **z = Melder_malloc_f (double_rgbt *, nrow);
				z [0] = (double_rgbt *) (p + 1);
				for (long irow = 1; irow < nrow; irow ++) z [irow] = z [irow - 1] + ncol;
				p += nrow * ncol * 4;
				Graphics_image_colour (thee, z, 0, ncol - 1, x1, x2,
								0, nrow - 1, y1, y2, minimum, maximum);
				Melder_free (z);
			}  break;
			case SET_COLOUR_SCALE: Graphics_setColourScale (thee, (enum kGraphics_colourScale) get); break;
			case SET_SPECKLE_SIZE: Graphics_setSpeckleSize (thee, get); break;
			case SPECKLE:
			{  double x = get, y = get;
				Graphics_speckle (thee, x, y);
			}  break;
			default:
				my recording = wasRecording;
				Melder_flushError (U"Graphics_play: unknown opcode (", opcode, U").\n", p [-1], U" ", p [1]);
				return;
		}
	}
	my recording = wasRecording;
}

void Graphics_writeRecordings (Graphics me, FILE *f) {
	double *p = my record, *endp = p + my irecord;
	if (! p) return;
	binputi32 (my irecord, f);
	while (p < endp) {
		#define get  (* ++ p)
		int opcode = (int) get;
		binputr32 ((float) opcode, f);
		long numberOfArguments = (long) get;
		const long largestIntegerRepresentableAs32BitFloat = 0x00FFFFFF;
		if (numberOfArguments > largestIntegerRepresentableAs32BitFloat) {
			binputr32 (-1.0, f);
			binputi32 (numberOfArguments, f);
			//Melder_warning ("This picture is very large!");
		} else {
			binputr32 ((float) numberOfArguments, f);
		}
		if (opcode == TEXT) {
			binputr32 (get, f);   // x
			binputr32 (get, f);   // y
			binputr32 (get, f);   // length
			Melder_assert (sizeof (double) == 8);
			if ((long) fwrite (++ p, 8, numberOfArguments - 3, f) < numberOfArguments - 3)   // text
				Melder_throw (U"Error writing graphics recordings.");
			p += numberOfArguments - 4;
		} else if (opcode == IMAGE_FROM_FILE) {
			binputr32 (get, f);   // x1
			binputr32 (get, f);   // x2
			binputr32 (get, f);   // y1
			binputr32 (get, f);   // y2
			binputr32 (get, f);   // length
			Melder_assert (sizeof (double) == 8);
			if ((long) fwrite (++ p, 8, numberOfArguments - 5, f) < numberOfArguments - 5)   // text
				Melder_throw (U"Error writing graphics recordings.");
			p += numberOfArguments - 6;
		} else {
			for (long i = numberOfArguments; i > 0; i --) binputr32 (get, f);
		}
	}
}

void Graphics_readRecordings (Graphics me, FILE *f) {
	long old_irecord = my irecord;
	long added_irecord = 0;
	double* p = nullptr;
	double* endp = nullptr;
	signed long numberOfArguments = 0;
	int opcode = 0;
	try {
		added_irecord = bingeti32 (f);
		p = _Graphics_check (me, added_irecord - RECORDING_HEADER_LENGTH);
		if (! p) return;
		Melder_assert (my irecord == old_irecord + added_irecord);
		endp = p + added_irecord;
		while (p < endp) {
			opcode = (int) bingetr32 (f);
			put (opcode);
			numberOfArguments = (signed long) bingetr32 (f);
			if (numberOfArguments == -1) {
				numberOfArguments = bingeti32 (f);
			}
			put (numberOfArguments);
			if (opcode == TEXT) {
				put (bingetr32 (f));   // x
				put (bingetr32 (f));   // y
				put (bingetr32 (f));   // length
				if (fread (++ p, 8, (size_t) numberOfArguments - 3, f) < (size_t) numberOfArguments - 3)   // text
					Melder_throw (U"Error reading graphics recordings.");
				p += numberOfArguments - 4;
			} else if (opcode == IMAGE_FROM_FILE) {
				put (bingetr32 (f));   // x1
				put (bingetr32 (f));   // x2
				put (bingetr32 (f));   // y1
				put (bingetr32 (f));   // y2
				put (bingetr32 (f));   // length
				if (fread (++ p, 8, (size_t) numberOfArguments - 5, f) < (size_t) numberOfArguments - 5)   // text
					Melder_throw (U"Error reading graphics recordings.");
				p += numberOfArguments - 6;
			} else {
				for (long i = numberOfArguments; i > 0; i --) put (bingetr32 (f));
			}
		}   
	} catch (MelderError) {
		my irecord = old_irecord;
		Melder_throw (U"Error reading graphics record ", added_irecord - (long) (endp - p),
			U" out of ", added_irecord, U".\nOpcode ", opcode, U", args ", numberOfArguments, U".");
	}
}

void Graphics_markGroup (Graphics me) {
	if (my recording) { op (MARK_GROUP, 0); }
}

void Graphics_undoGroup (Graphics me) {
	long lastMark = 0;   // not yet found
	long jrecord = 0;
	while (jrecord < my irecord) {   // keep looking for marks until the end
		int opcode = (int) my record [++ jrecord];
		long number = (long) my record [++ jrecord];
		if (opcode == MARK_GROUP) lastMark = jrecord - 1;   // found a mark
		jrecord += number;
	}
	if (jrecord != my irecord) Melder_flushError (U"jrecord != my irecord: ", jrecord, U", ", my irecord);
	if (lastMark > 0)   // found?
		my irecord = lastMark - 1;   // forget all graphics from and including the last mark
}

/* End of file Graphics_record.cpp */
