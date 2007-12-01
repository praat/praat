/* Graphics_utils.c
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
 * pb 2004/06/18 allow reversed linear axes
 * pb 2005/12/20 allow reversed logarithmic axes
 * pb 2007/06/10 wchar_t
 * pb 2007/08/01 reintroduced yIsZeroAtTheTop
 * pb 2007/08/08 wchar_t
 */

#include <math.h>
#include "GraphicsP.h"

/***** UTILITIES: *****/
/***** THESE ROUINTES OUTPUT TO CURRENT GRAPHICS BY CALLING PRIMITIVES. *****/
/***** THE TWO UTILITIES "Graphics_grey" AND "Graphics_altitude" *****/
/***** ARE IN DIFFERENT FILES BECAUSE THEY NEED LOCAL SUBROUTINES. *****/

/********** Utility functions. **********/

static MelderString theGraphicsTextBuffer = { 0 };
void Graphics_text1 (I, double x, double y, const wchar_t *s1) {
	iam (Graphics);
	MelderString_copy (& theGraphicsTextBuffer, s1);   // make a copy because s1 may be a temporary string (Melder_integer or so)
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text2 (I, double x, double y, const wchar_t *s1, const wchar_t *s2) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append2 (& theGraphicsTextBuffer, s1, s2);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text3 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append3 (& theGraphicsTextBuffer, s1, s2, s3);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text4 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append4 (& theGraphicsTextBuffer, s1, s2, s3, s4);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text5 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append5 (& theGraphicsTextBuffer, s1, s2, s3, s4, s5);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text6 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append6 (& theGraphicsTextBuffer, s1, s2, s3, s4, s5, s6);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text7 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append7 (& theGraphicsTextBuffer, s1, s2, s3, s4, s5, s6, s7);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text8 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append8 (& theGraphicsTextBuffer, s1, s2, s3, s4, s5, s6, s7, s8);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}
void Graphics_text9 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	iam (Graphics);
	MelderString_empty (& theGraphicsTextBuffer);
	MelderString_append9 (& theGraphicsTextBuffer, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	Graphics_text (me, x, y, theGraphicsTextBuffer.string);
}

/********** Drawing into margins. **********/

void Graphics_drawInnerBox (I) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	Graphics_setInner (me);
	Graphics_setWindow (me, 0, 1, 0, 1);
	Graphics_setLineType (me, Graphics_DRAWN);
	Graphics_setLineWidth (me, 2.0 * lineWidth);
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_rectangle (me, 0, 1, 0, 1);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setLineWidth (me, lineWidth);
	Graphics_setColour (me, colour);
}

void Graphics_textLeft (I, int farr, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int vert = farr ? Graphics_TOP : Graphics_BOTTOM, colour = my colour;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, 0, 1);
	Graphics_setTextRotation (me, 90);
	Graphics_setTextAlignment (me, Graphics_CENTRE, vert);
	if (! farr) Graphics_setInner (me);
	Graphics_text (me, 0, 0.5, text);
	if (! farr) Graphics_unsetInner (me);
	Graphics_setTextRotation (me, 0);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setColour (me, colour);
}

void Graphics_textRight (I, int farr, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int vert = farr ? Graphics_TOP : Graphics_BOTTOM, colour = my colour;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, vert);
	Graphics_setWindow (me, 0, 1, 0, 1);
	Graphics_setTextRotation (me, 270);
	if (! farr) Graphics_setInner (me);
	Graphics_text (me, 1, 0.5, text);
	if (! farr) Graphics_unsetInner (me);
	Graphics_setTextRotation (me, 0);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setColour (me, colour);
}

void Graphics_textBottom (I, int farr, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int colour = my colour;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, 0, 1);
	if (farr) {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (me, 0.5, 0, text);
	} else {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
		Graphics_setInner (me);
		Graphics_text (me, 0.5, - my vertTick, text);
		Graphics_unsetInner (me);
	}
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setColour (me, colour);
}

void Graphics_textTop (I, int farr, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int colour = my colour;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, 0, 1);
	if (farr) {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
		Graphics_text (me, 0.5, 1, text);
	} else {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_setInner (me);
		Graphics_text (me, 0.5, 1 + my vertTick, text);
		Graphics_unsetInner (me);
	}
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setColour (me, colour);
}

void Graphics_marksLeft (I, int numberOfMarks, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarks < 2) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = 1; i <= numberOfMarks; i ++) {
		double f = (i - 1.0) / (numberOfMarks - 1), yWC = y1WC + (y2WC - y1WC) * f;
		if (haveNumbers) Graphics_text1 (me, - my horTick, yWC, Melder_float (Melder_half (yWC)));
		if (haveTicks) Graphics_line (me, - my horTick, yWC, 0, yWC);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = 2; i < numberOfMarks; i ++) {
			double f = (i - 1.0) / (numberOfMarks - 1), yWC = y1WC + (y2WC - y1WC) * f;
			Graphics_line (me, 0, yWC, 1, yWC);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksRight (I, int numberOfMarks, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarks < 2) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = 1; i <= numberOfMarks; i ++) {
		double f = (i - 1.0) / (numberOfMarks - 1), yWC = y1WC + (y2WC - y1WC) * f;
		if (haveNumbers) Graphics_text1 (me, 1 + my horTick, yWC, Melder_float (Melder_half (yWC)));
		if (haveTicks) Graphics_line (me, 1, yWC, 1 + my horTick, yWC);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = 2; i < numberOfMarks; i ++) {
			double f = (i - 1.0) / (numberOfMarks - 1), yWC = y1WC + (y2WC - y1WC) * f;
			Graphics_line (me, 0, yWC, 1, yWC);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksBottom (I, int numberOfMarks, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarks < 2) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = 1; i <= numberOfMarks; i ++) {
		double f = (i - 1.0) / (numberOfMarks - 1), xWC = x1WC + (x2WC - x1WC) * f;
		if (haveNumbers) Graphics_text1 (me, xWC, - my vertTick, Melder_float (Melder_half (xWC)));
		if (haveTicks) Graphics_line (me, xWC, - my vertTick, xWC, 0);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = 2; i < numberOfMarks; i ++)
		{
			double f = (i - 1.0) / (numberOfMarks - 1), xWC = x1WC + (x2WC - x1WC) * f;
			Graphics_line (me, xWC, 0, xWC, 1);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksTop (I, int numberOfMarks, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarks < 2) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = 1; i <= numberOfMarks; i ++) {
		double f = (i - 1.0) / (numberOfMarks - 1), xWC = x1WC + (x2WC - x1WC) * f;
		if (haveNumbers) Graphics_text1 (me, xWC, 1 + my vertTick, Melder_float (Melder_half (xWC)));
		if (haveTicks) Graphics_line (me, xWC, 1, xWC, 1 + my vertTick);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = 2; i < numberOfMarks; i ++) {
			double f = (i - 1.0) / (numberOfMarks - 1), xWC = x1WC + (x2WC - x1WC) * f;
			Graphics_line (me, xWC, 0, xWC, 1);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

#define MAXNUM_MARKS_PER_DECADE  7
static double decade_y [1 + MAXNUM_MARKS_PER_DECADE] [1 + MAXNUM_MARKS_PER_DECADE] = {
	{ 0 },
	{ 0, 10 },
	{ 0, 10, 30 },
	{ 0, 10, 20, 50 },
	{ 0, 10, 20, 30, 50 },
	{ 0, 10, 20, 30, 50, 70 },
	{ 0, 10, 15, 20, 30, 50, 70 },
	{ 0, 10, 15, 20, 30, 40, 50, 70 }
};

void Graphics_marksLeftLogarithmic (I, int numberOfMarksPerDecade, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1 = my x1WC, x2 = my x2WC, y1 = my y1WC, y2 = my y2WC, py1, py2;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarksPerDecade < 1) numberOfMarksPerDecade = 1;
	if (numberOfMarksPerDecade > MAXNUM_MARKS_PER_DECADE) numberOfMarksPerDecade = MAXNUM_MARKS_PER_DECADE;
	if (y1 > 300 || y2 > 300) return;
	py1 = pow (10, y1 + ( y1 < y2 ? -1e-6 : 1e-6 ));
	py2 = pow (10, y2 + ( y1 < y2 ? 1e-6 : -1e-6 ));
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1, y2);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	for (i = 1; i <= numberOfMarksPerDecade; i ++) {
		double y = decade_y [numberOfMarksPerDecade] [i];
		while (y < (y1<y2?py1:py2)) y *= 10;
		while (y >= (y1<y2?py1:py2)) y /= 10;
		for (y *= 10; y <= (y1<y2?py2:py1); y *= 10) {
			if (haveNumbers) Graphics_text1 (me, - my horTick, log10 (y), Melder_float (Melder_half (y)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, - my horTick, log10 (y), 0, log10 (y));
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, 0, log10 (y), 1, log10 (y));
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_marksRightLogarithmic (I, int numberOfMarksPerDecade, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1 = my x1WC, x2 = my x2WC, y1 = my y1WC, y2 = my y2WC, py1, py2;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarksPerDecade < 1) numberOfMarksPerDecade = 1;
	if (numberOfMarksPerDecade > MAXNUM_MARKS_PER_DECADE) numberOfMarksPerDecade = MAXNUM_MARKS_PER_DECADE;
	if (y1 > 300 || y2 > 300) return;
	py1 = pow (10, y1 + ( y1 < y2 ? -1e-6 : 1e-6 ));
	py2 = pow (10, y2 + ( y1 < y2 ? 1e-6 : -1e-6 ));
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1, y2);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	for (i = 1; i <= numberOfMarksPerDecade; i ++) {
		double y = decade_y [numberOfMarksPerDecade] [i];
		while (y < (y1<y2?py1:py2)) y *= 10;
		while (y >= (y1<y2?py1:py2)) y /= 10;
		for (y *= 10; y <= (y1<y2?py2:py1); y *= 10) {
			if (haveNumbers) Graphics_text1 (me, 1 + my horTick, log10 (y), Melder_float (Melder_half (y)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, 1, log10 (y), 1 + my horTick, log10 (y));
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, 0, log10 (y), 1, log10 (y));
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_marksTopLogarithmic (I, int numberOfMarksPerDecade, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1 = my x1WC, x2 = my x2WC, y1 = my y1WC, y2 = my y2WC, px1, px2;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarksPerDecade < 1) numberOfMarksPerDecade = 1;
	if (numberOfMarksPerDecade > MAXNUM_MARKS_PER_DECADE) numberOfMarksPerDecade = MAXNUM_MARKS_PER_DECADE;
	if (x1 > 300 || x2 > 300) return;
	px1 = pow (10, x1 + ( x1 < x2 ? -1e-6 : 1e-6 ));
	px2 = pow (10, x2 + ( x1 < x2 ? 1e-6 : -1e-6 ));
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1, x2, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	for (i = 1; i <= numberOfMarksPerDecade; i ++) {
		double x = decade_y [numberOfMarksPerDecade] [i];
		while (x < (x1<x2?px1:px2)) x *= 10;
		while (x >= (x1<x2?px1:px2)) x /= 10;
		for (x *= 10; x <= (x1<x2?px2:px1); x *= 10) {
			if (haveNumbers) Graphics_text1 (me, log10 (x), 1 + my vertTick, Melder_float (Melder_half (x)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, log10 (x), 1, log10 (x), 1 + my vertTick);
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, log10 (x), 0, log10 (x), 1);
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_marksBottomLogarithmic (I, int numberOfMarksPerDecade, int haveNumbers, int haveTicks, int haveDottedLines) {
	iam (Graphics);
	double x1 = my x1WC, x2 = my x2WC, y1 = my y1WC, y2 = my y2WC, px1, px2;
	int lineType = my lineType, colour = my colour, i;
	double lineWidth = my lineWidth;
	if (numberOfMarksPerDecade < 1) numberOfMarksPerDecade = 1;
	if (numberOfMarksPerDecade > MAXNUM_MARKS_PER_DECADE) numberOfMarksPerDecade = MAXNUM_MARKS_PER_DECADE;
	if (x1 > 300 || x2 > 300) return;
	px1 = pow (10, x1 + ( x1 < x2 ? -1e-6 : 1e-6 ));
	px2 = pow (10, x2 + ( x1 < x2 ? 1e-6 : -1e-6 ));
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1, x2, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	for (i = 1; i <= numberOfMarksPerDecade; i ++) {
		double x = decade_y [numberOfMarksPerDecade] [i];
		while (x < (x1<x2?px1:px2)) x *= 10;
		while (x >= (x1<x2?px1:px2)) x /= 10;
		for (x *= 10; x <= (x1<x2?px2:px1); x *= 10) {
			if (haveNumbers) Graphics_text1 (me, log10 (x), - my vertTick, Melder_float (Melder_half (x)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, log10 (x), - my vertTick, log10 (x), 0);
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, log10 (x), 0, log10 (x), 1);
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_markLeft (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, - my horTick, position, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, - my horTick, position, 0, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0, position, 1, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, - my horTick, position, text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markRight (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, 1 + my horTick, position, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, 1, position, 1 + my horTick, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0, position, 1, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, 1 + my horTick, position, text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markTop (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, position, 1 + my vertTick, Melder_float (Melder_single (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, position, 1, position, 1 + my vertTick);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, position, 0, position, 1);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, position, 1 + my vertTick, text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markBottom (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, position, - my vertTick, Melder_float (Melder_single (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, position, - my vertTick, position, 0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, position, 0, position, 1);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, position, - my vertTick, text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markLeftLogarithmic (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	if (position <= 0) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, - my horTick, log10 (position), Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, - my horTick, log10 (position), 0, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0, log10 (position), 1, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, - my horTick, log10 (position), text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markRightLogarithmic (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	if (position <= 0) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, 1 + my horTick, log10 (position), Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, 1, log10 (position), 1 + my horTick, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0, log10 (position), 1, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, 1 + my horTick, log10 (position), text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markTopLogarithmic (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	if (position <= 0) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, log10 (position), 1 + my vertTick, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, log10 (position), 1, log10 (position), 1 + my vertTick);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, log10 (position), 0, log10 (position), 1);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, log10 (position), 1 + my vertTick, text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markBottomLogarithmic (I, double position, int hasNumber, int hasTick, int hasDottedLine, const wchar_t *text) {
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour;
	double lineWidth = my lineWidth;
	if (position <= 0) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (hasNumber) Graphics_text1 (me, log10 (position), - my vertTick, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, log10 (position), - my vertTick, log10 (position), 0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, log10 (position), 0, log10 (position), 1);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && wcslen (text)) Graphics_text (me, log10 (position), - my vertTick, text);
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksLeftEvery (I, double units, double distance,
	int haveNumbers, int haveTicks, int haveDottedLines)
{
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, first, last, i;
	double lineWidth = my lineWidth;
	distance *= units;
	first = ceil (( y1WC < y2WC ? y1WC : y2WC ) / distance - 1e-5);
	last = floor (( y1WC < y2WC ? y2WC : y1WC) / distance + 1e-5);
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = first; i <= last; i ++) {
		double yWC = i * distance;
		if (haveNumbers) Graphics_text1 (me, - my horTick, yWC, Melder_float (Melder_half (yWC / units)));
		if (haveTicks) Graphics_line (me, - my horTick, yWC, 0, yWC);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = first; i <= last; i ++) {
			double yWC = i * distance;
			Graphics_line (me, 0, yWC, 1, yWC);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksRightEvery (I, double units, double distance,
	int haveNumbers, int haveTicks, int haveDottedLines)
{
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, first, last, i;
	double lineWidth = my lineWidth;
	distance *= units;
	first = ceil (( y1WC < y2WC ? y1WC : y2WC ) / distance - 1e-5);
	last = floor (( y1WC < y2WC ? y2WC : y1WC) / distance + 1e-5);
	if (first > last) return;
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, 0, 1, y1WC, y2WC);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = first; i <= last; i ++) {
		double yWC = i * distance;
		if (haveNumbers) Graphics_text1 (me, 1 + my horTick, yWC, Melder_float (Melder_half (yWC / units)));
		if (haveTicks) Graphics_line (me, 1, yWC, 1 + my horTick, yWC);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = first; i <= last; i ++) {
			double yWC = i * distance;
			Graphics_line (me, 0, yWC, 1, yWC);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksBottomEvery (I, double units, double distance,
	int haveNumbers, int haveTicks, int haveDottedLines)
{
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, first, last, i;
	double lineWidth = my lineWidth;
	distance *= units;
	first = ceil (( x1WC < x2WC ? x1WC : x2WC ) / distance - 1e-5);
	last = floor (( x1WC < x2WC ? x2WC : x1WC) / distance + 1e-5);
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = first; i <= last; i ++) {
		double xWC = i * distance;
		if (haveNumbers) Graphics_text1 (me, xWC, - my vertTick, Melder_float (Melder_half (xWC / units)));
		if (haveTicks) Graphics_line (me, xWC, - my vertTick, xWC, 0);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = first; i <= last; i ++) {
			double xWC = i * distance;
			Graphics_line (me, xWC, 0, xWC, 1);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksTopEvery (I, double units, double distance,
	int haveNumbers, int haveTicks, int haveDottedLines)
{
	iam (Graphics);
	double x1WC = my x1WC, x2WC = my x2WC, y1WC = my y1WC, y2WC = my y2WC;
	int lineType = my lineType, colour = my colour, first, last, i;
	double lineWidth = my lineWidth;
	distance *= units;
	first = ceil (( x1WC < x2WC ? x1WC : x2WC ) / distance - 1e-5);
	last = floor (( x1WC < x2WC ? x2WC : x1WC) / distance + 1e-5);
	Graphics_setColour (me, Graphics_BLACK);
	Graphics_setWindow (me, x1WC, x2WC, 0, 1);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (i = first; i <= last; i ++) {
		double xWC = i * distance;
		if (haveNumbers) Graphics_text1 (me, xWC, 1 + my vertTick, Melder_float (Melder_half (xWC / units)));
		if (haveTicks) Graphics_line (me, xWC, 1, xWC, 1 + my vertTick);
	}
	if (haveTicks) Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (i = first; i <= last; i ++) {
			double xWC = i * distance;
			Graphics_line (me, xWC, 0, xWC, 1);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);
	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_mark (I, double x, double y, double size_mm, const wchar_t *markString) {
	iam (Graphics);
	int mark;
	if (! markString || ! markString [0]) mark = 0;
	else if (! markString [1]) {
		if (markString [0] == '+') mark = 1;
		else if (markString [0] == 'x') mark = 2;
		else if (markString [0] == 'o') mark = 3;
		else if (markString [0] == '.') mark = 0;
		else mark = -1;
	} else mark = -1;
	if (mark == -1) {
		int oldSize = my fontSize;
		int oldHorizontalAlignment = my horizontalTextAlignment;
		int oldVerticalAlignment = my verticalTextAlignment;
		Graphics_setFontSize (me, size_mm * 72 / 25.4);
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (me, x, y, markString);
		Graphics_setFontSize (me, oldSize);
		Graphics_setTextAlignment (me, oldHorizontalAlignment, oldVerticalAlignment);
	} else if (mark == 0) {
		Graphics_fillCircle_mm (me, x, y, size_mm);
	} else if (mark == 1) {
		double dx = 0.5 * Graphics_dxMMtoWC (me, size_mm);
		double dy = 0.5 * Graphics_dyMMtoWC (me, size_mm);
		Graphics_line (me, x - dx, y, x + dx, y);
		Graphics_line (me, x, y - dy, x, y + dy);
	} else if (mark == 2) {
		double dx = 0.4 * Graphics_dxMMtoWC (me, size_mm);
		double dy = 0.4 * Graphics_dyMMtoWC (me, size_mm);
		Graphics_line (me, x - dx, y - dy, x + dx, y + dy);
		Graphics_line (me, x + dx, y - dy, x - dx, y + dy);
	} else {
		Graphics_circle_mm (me, x, y, size_mm);
	}
}

void Graphics_setTextRotation_vector (I, double dx, double dy) {
	iam (Graphics);
	double angle;
	if (dy == 0.0) {
		angle = dx >= 0.0 ? 0.0 : 180.0;
	} else if (dx == 0.0) {
		angle = dy > 0.0 ? 90.0 : 270.0;
	} else {
		double dxDC = dx * my scaleX, dyDC = my yIsZeroAtTheTop ? -dy * my scaleY : dy * my scaleY;
		angle = atan2 (dyDC, dxDC) * (180 / NUMpi);
	}
	Graphics_setTextRotation (me, angle);
}

/* End of file Graphics_utils.c */

