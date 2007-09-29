/* Graphics_record.c
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
 * pb 2004/02/15 highlight2
 * pb 2007/03/14 arrowSize
 * pb 2007/08/08 text is saved as UTF-8
 */

#include "GraphicsP.h"

#define RECORDING_HEADER_LENGTH 2

float * _Graphics_check (Graphics me, long number) {
	static int messageShown = FALSE;
	float *result = NULL;
	float *record = my record;
	long nrecord = my nrecord;
	if (nrecord == 0) {
		nrecord = 1000;
		record = Melder_malloc (float, 1 + nrecord);
		if (record == NULL) goto error;
		my record = record; my nrecord = nrecord;
	}
	if (nrecord < my irecord + RECORDING_HEADER_LENGTH + number) {
		while (nrecord < my irecord + RECORDING_HEADER_LENGTH + number) nrecord *= 2;
		record = Melder_realloc (record, (1 + nrecord) * sizeof (float));
		if (record == NULL) goto error;
		my record = record; my nrecord = nrecord;
	}
	result = my record + my irecord;
	my irecord += number + RECORDING_HEADER_LENGTH;
	return result;
error:
	if (messageShown) {
		Melder_clearError ();
	} else {
		messageShown = TRUE;
		Melder_flushError ("_Graphics_growRecorder: out of memory.\n"
			"This message will not show up on future occasions.");   /* Because of loop danger when redrawing. */
	}
	return NULL;
}

/***** RECORD AND PLAY *****/

int Graphics_startRecording (I) {
	iam (Graphics);
	int wasRecording = my recording;
	my recording = 1;
	return wasRecording;
}

int Graphics_stopRecording (I) {
	iam (Graphics);
	int wasRecording = my recording;
	my recording = 0;
	return wasRecording;
}

void Graphics_play (Graphics me, Graphics thee) {
	float *p = my record, *endp = p + my irecord;
	int wasRecording = my recording;
	if (! p) return;
	my recording = 0;   /* Temporarily, in case me == thee. */
	while (p < endp) {
		#define get  (* ++ p)
		#define mget(n)  (p += n, p - n)
		#define sget(n)  ((char *) (p += n, p - n + 1))
		int opcode = (int) get;
		(void) (long) get;   /* Ignore number of arguments. */
		switch (opcode) {
			case SET_VIEWPORT:
			{  float x1NDC = get, x2NDC = get, y1NDC = get, y2NDC = get;
				Graphics_setViewport (thee, x1NDC, x2NDC, y1NDC, y2NDC);
			}  break;
			case SET_INNER: Graphics_setInner (thee); break;
			case UNSET_INNER: Graphics_unsetInner (thee); break;
			case SET_WINDOW:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_setWindow (thee, x1, x2, y1, y2);
			}  break;
			case TEXT:
			{  float x = get, y = get; long length = get; char *text_utf8 = sget (length);
				Graphics_text (thee, x, y, Melder_peekUtf8ToWcs (text_utf8));
			}  break;
			case POLYLINE:
			{  long n = get; float *x = mget (n), *y = mget (n);
				Graphics_polyline (thee, n, & x [1], & y [1]);
			}  break;
			case LINE:
			{  float x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_line (thee, x1, y1, x2, y2);
			}  break;
			case ARROW:
			{  float x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_arrow (thee, x1, y1, x2, y2);
			}  break;
			case FILL_AREA:
			{  long n = get; float *x = mget (n), *y = mget (n);
				Graphics_fillArea (thee, n, & x [1], & y [1]);
			}  break;
			case FUNCTION:
			{  long n = get; float x1 = get, x2 = get, *y = mget (n);
				Graphics_function (thee, y, 1, n, x1, x2);
			}  break;
			case RECTANGLE:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_rectangle (thee, x1, x2, y1, y2);
			}  break;
			case FILL_RECTANGLE:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_fillRectangle (thee, x1, x2, y1, y2);
			}  break;
			case CIRCLE:
			{  float x = get, y = get, r = get;
				Graphics_circle (thee, x, y, r);
			}  break;
			case FILL_CIRCLE:
			{  float x = get, y = get, r = get;
				Graphics_fillCircle (thee, x, y, r);
			}  break;
			case ARC:
			{  float x = get, y = get, r = get, fromAngle = get, toAngle = get;
				Graphics_arc (thee, x, y, r, fromAngle, toAngle);
			}  break;
			case ARC_ARROW:
			{  float x = get, y = get, r = get, fromAngle = get, toAngle = get;
				int arrowAtStart = get, arrowAtEnd = get;
				Graphics_arcArrow (thee, x, y, r, fromAngle, toAngle, arrowAtStart, arrowAtEnd);
			}  break;
			case HIGHLIGHT:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_highlight (thee, x1, x2, y1, y2);
			}  break;
			case CELL_ARRAY:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get, irow;
				float **z = Melder_malloc (float *, nrow);
				z [0] = p + 1;
				for (irow = 1; irow < nrow; irow ++) z [irow] = z [irow - 1] + ncol;
				p += nrow * ncol;
				Graphics_cellArray (thee, z, 0, ncol - 1, x1, x2,
								0, nrow - 1, y1, y2, minimum, maximum);
				Melder_free (z);
			}  break;
			case SET_FONT: Graphics_setFont (thee, (int) get); break;
			case SET_FONT_SIZE: Graphics_setFontSize (thee, (int) get); break;
			case SET_FONT_STYLE: Graphics_setFontStyle (thee, (int) get); break;
			case SET_TEXT_ALIGNMENT:
			{  int hor = get, vert = get;
				Graphics_setTextAlignment (thee, hor, vert);
			}  break;
			case SET_TEXT_ROTATION: Graphics_setTextRotation (thee, get); break;
			case SET_LINE_TYPE: Graphics_setLineType (thee, (int) get); break;
			case SET_LINE_WIDTH: Graphics_setLineWidth (thee, (double) get); break;
			case SET_COLOUR: Graphics_setColour (thee, (int) get); break;
			case SET_GREY: Graphics_setGrey (thee, get); break;
			case MARK_GROUP: Graphics_markGroup (thee); break;
			case ELLIPSE: {
				float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_ellipse (thee, x1, x2, y1, y2);
			} break;
			case FILL_ELLIPSE: {
				float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_fillEllipse (thee, x1, x2, y1, y2);
			} break;
			case CIRCLE_MM:
			{  float x = get, y = get, d = get;
				Graphics_circle_mm (thee, x, y, d);
			}  break;
			case FILL_CIRCLE_MM:
			{  float x = get, y = get, d = get;
				Graphics_fillCircle_mm (thee, x, y, d);
			}  break;
			case IMAGE8:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get, irow, icol;
				unsigned char **z = NUMubmatrix (1, nrow, 1, ncol);
				for (irow = 1; irow <= nrow; irow ++)
					for (icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = get;
				Graphics_image8 (thee, z, 1, ncol, x1, x2, 1, nrow, y1, y2, minimum, maximum);
				NUMubmatrix_free (z, 1, 1);
			}  break;
			case UNHIGHLIGHT:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_unhighlight (thee, x1, x2, y1, y2);
			}  break;
			case XOR_ON: Graphics_xorOn (thee, get); break;
			case XOR_OFF: Graphics_xorOff (thee); break;
			case RECTANGLE_MM:
			{  float x = get, y = get, horSide = get, vertSide = get;
				Graphics_rectangle_mm (thee, x, y, horSide, vertSide);
			}  break;
			case FILL_RECTANGLE_MM:
			{  float x = get, y = get, horSide = get, vertSide = get;
				Graphics_fillRectangle_mm (thee, x, y, horSide, vertSide);
			}  break;
			case SET_WS_WINDOW:
			{  float x1NDC = get, x2NDC = get, y1NDC = get, y2NDC = get;
				Graphics_setWsWindow (thee, x1NDC, x2NDC, y1NDC, y2NDC);
			}  break;
			case SET_WRAP_WIDTH: Graphics_setWrapWidth (thee, get); break;
			case SET_SECOND_INDENT: Graphics_setSecondIndent (thee, get); break;
			case SET_PERCENT_SIGN_IS_ITALIC: Graphics_setPercentSignIsItalic (thee, (int) get); break;
			case SET_NUMBER_SIGN_IS_BOLD: Graphics_setNumberSignIsBold (thee, (int) get); break;
			case SET_CIRCUMFLEX_IS_SUPERSCRIPT: Graphics_setCircumflexIsSuperscript (thee, (int) get); break;
			case SET_UNDERSCORE_IS_SUBSCRIPT: Graphics_setUnderscoreIsSubscript (thee, (int) get); break;
			case SET_DOLLAR_SIGN_IS_CODE: Graphics_setDollarSignIsCode (thee, (int) get); break;
			case SET_AT_SIGN_IS_LINK: Graphics_setAtSignIsLink (thee, (int) get); break;
			case BUTTON:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_button (thee, x1, x2, y1, y2);
			}  break;
			case ROUNDED_RECTANGLE:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, r = get;
				Graphics_roundedRectangle (thee, x1, x2, y1, y2, r);
			}  break;
			case FILL_ROUNDED_RECTANGLE:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, r = get;
				Graphics_fillRoundedRectangle (thee, x1, x2, y1, y2, r);
			}  break;
			case FILL_ARC:
			{  float x = get, y = get, r = get, fromAngle = get, toAngle = get;
				Graphics_fillArc (thee, x, y, r, fromAngle, toAngle);
			}  break;
			case INNER_RECTANGLE:
			{  float x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_innerRectangle (thee, x1, x2, y1, y2);
			}  break;
			case CELL_ARRAY8:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get, irow, icol;
				unsigned char **z = NUMubmatrix (1, nrow, 1, ncol);
				for (irow = 1; irow <= nrow; irow ++)
					for (icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = get;
				Graphics_cellArray8 (thee, z, 1, ncol, x1, x2, 1, nrow, y1, y2, minimum, maximum);
				NUMubmatrix_free (z, 1, 1);
			}  break;
			case IMAGE:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				long nrow = get, ncol = get, irow;
				float **z = Melder_malloc (float *, nrow);
				z [0] = p + 1;
				for (irow = 1; irow < nrow; irow ++) z [irow] = z [irow - 1] + ncol;
				p += nrow * ncol;
				Graphics_image (thee, z, 0, ncol - 1, x1, x2,
								0, nrow - 1, y1, y2, minimum, maximum);
				Melder_free (z);
			}  break;
			case HIGHLIGHT2:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, innerX1 = get, innerX2 = get, innerY1 = get, innerY2 = get;
				Graphics_highlight2 (thee, x1, x2, y1, y2, innerX1, innerX2, innerY1, innerY2);
			}  break;
			case UNHIGHLIGHT2:
			{  float x1 = get, x2 = get, y1 = get, y2 = get, innerX1 = get, innerX2 = get, innerY1 = get, innerY2 = get;
				Graphics_unhighlight2 (thee, x1, x2, y1, y2, innerX1, innerX2, innerY1, innerY2);
			}  break;
			case SET_ARROW_SIZE: Graphics_setArrowSize (thee, (double) get); break;
			case DOUBLE_ARROW:
			{  float x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_doubleArrow (thee, x1, y1, x2, y2);
			}  break;
			default:
				my recording = wasRecording;
				Melder_flushError ("Graphics_play: unknown opcode (%d).\n%f %f", opcode, p [-1], p [1]);
				return;
		}
	}
	my recording = wasRecording;
}

/* For debugging:
#define binputi4(o,f) ascputi4(o,f,"")
#define binputr4(o,f) ascputr4(o,f,"")
*/
int Graphics_writeRecordings (I, FILE *f) {
	iam (Graphics);
	float *p = my record, *endp = p + my irecord;
	if (! p) return 0;
/*ascio_verbose(0);*/
	binputi4 (my irecord, f);
	while (p < endp) {
		#define get  (* ++ p)
		int opcode = (int) get;
		long numberOfArguments = (long) get;
		binputr4 ((float) opcode, f);
		binputr4 ((float) numberOfArguments, f);
		if (opcode == TEXT) {
			binputr4 (get, f);   /* x */
			binputr4 (get, f);   /* y */
			binputr4 (get, f);   /* length */
			fwrite (++ p, 4, numberOfArguments - 3, f);   /* text */
			p += numberOfArguments - 4;
		} else {
			long i;
			for (i = numberOfArguments; i > 0; i --) binputr4 (get, f);
		}
	}
	return ! ferror (f) && ! feof (f);
/*	long i;
	binputi4 (my irecord, f);
	for (i = 1; i <= my irecord; i ++) binputr4 (my record [i], f);
	return ! ferror (f) && ! feof (f);*/
}

#ifdef _WIN32
int Graphics_readRecordings_oldWindows (I, FILE *f) {
	iam (Graphics);
	long i, added_irecord, old_irecord = my irecord;
	float *p;
	added_irecord = bingeti4 (f);
	p = _Graphics_check (me, added_irecord - RECORDING_HEADER_LENGTH);
	if (! p) return 0;
	Melder_assert (my irecord == old_irecord + added_irecord);
	for (i = 1; i <= added_irecord; i ++) {
		float value = bingetr4 (f);
		if (ferror (f) || feof (f)) {
			my irecord = old_irecord;
			return Melder_error ("Graphics_readRecordings: "
				"error reading record %ld out of %ld.", i, added_irecord);
		}
		* ++ p = value;
	}   
	return 1;
}
#endif

int Graphics_readRecordings (I, FILE *f) {
	iam (Graphics);
	long added_irecord, old_irecord = my irecord;
	float *p, *endp;
	added_irecord = bingeti4 (f);
	p = _Graphics_check (me, added_irecord - RECORDING_HEADER_LENGTH);
	if (! p) return 0;
	Melder_assert (my irecord == old_irecord + added_irecord);
	endp = p + added_irecord;
	while (p < endp) {
		int opcode = (int) bingetr4 (f);
		long numberOfArguments = (long) bingetr4 (f);
		put (opcode);
		put (numberOfArguments);
		if (opcode == TEXT) {
			put (bingetr4 (f));   /* x */
			put (bingetr4 (f));   /* y */
			put (bingetr4 (f));   /* length */
			fread (++ p, 4, numberOfArguments - 3, f);   /* text */
			p += numberOfArguments - 4;
		} else {
			long i;
			for (i = numberOfArguments; i > 0; i --) put (bingetr4 (f));
		}
		if (ferror (f) || feof (f)) {
			my irecord = old_irecord;
			return Melder_error ("Graphics_readRecordings: "
				"error reading record %ld out of %ld.\nOpcode %d, args %ld.",
				added_irecord - (endp - p), added_irecord, opcode, numberOfArguments);
		}
	}   
	return 1;
}

void Graphics_markGroup (I) {
	iam (Graphics);
	if (my recording) { op (MARK_GROUP, 0); }
}

void Graphics_undoGroup (I) {
	iam (Graphics);
	long lastMark = 0;   /* Not yet found. */
	long jrecord = 0;
	while (jrecord < my irecord) {   /* Keep looking for marks until the end. */
		int opcode = (int) my record [++ jrecord];
		long number = (long) my record [++ jrecord];
		if (opcode == MARK_GROUP) lastMark = jrecord - 1;   /* Found a mark. */
		jrecord += number;
	}
	if (jrecord != my irecord) Melder_flushError ("jrecord != my irecord: %ld, %ld", jrecord, my irecord);
	if (lastMark > 0)   /* Found? */
		my irecord = lastMark - 1;   /* Forget all graphics from and including the last mark. */
}

/* End of file Graphics_record.c */
