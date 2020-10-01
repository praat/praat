/* Graphics_utils.cpp
 *
 * Copyright (C) 1992-2007,2009-2012,2015-2020 Paul Boersma
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

/***** UTILITIES: *****/
/***** THESE ROUINTES OUTPUT TO CURRENT GRAPHICS BY CALLING PRIMITIVES. *****/
/***** THE TWO UTILITIES "Graphics_grey" AND "Graphics_altitude" *****/
/***** ARE IN DIFFERENT FILES BECAUSE THEY NEED LOCAL SUBROUTINES. *****/

/********** Utility functions. **********/

/********** Drawing into margins. **********/

void Graphics_drawInnerBox (Graphics me) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const int original_lineType = my lineType;
	const double original_lineWidth = my lineWidth;
	const MelderColour original_colour = my colour;

	Graphics_setInner (me);
	Graphics_setWindow (me, 0.0, 1.0, 0.0, 1.0);
	Graphics_setLineType (me, Graphics_DRAWN);
	Graphics_setLineWidth (me, 2.0 * original_lineWidth);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_rectangle (me, 0.0, 1.0, 0.0, 1.0);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setLineType (me, original_lineType);
	Graphics_setLineWidth (me, original_lineWidth);
	Graphics_setColour (me, original_colour);
}

void Graphics_textLeft (Graphics me, bool farr, conststring32 text) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const int vert = ( farr ? Graphics_TOP : Graphics_BOTTOM );
	const MelderColour original_colour = my colour;

	Graphics_setColour (me, Melder_BLACK);
	Graphics_setWindow (me, 0.0, 1.0, 0.0, 1.0);
	Graphics_setTextRotation (me, 90.0);
	Graphics_setTextAlignment (me, Graphics_CENTRE, vert);
	if (! farr)
		Graphics_setInner (me);
	Graphics_text (me, 0.0, 0.5, text);
	if (! farr)
		Graphics_unsetInner (me);

	Graphics_setTextRotation (me, 0.0);
	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setColour (me, original_colour);
}

void Graphics_textRight (Graphics me, bool farr, conststring32 text) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const int vert = ( farr ? Graphics_TOP : Graphics_BOTTOM );
	const MelderColour original_colour = my colour;

	Graphics_setWindow (me, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, vert);
	Graphics_setTextRotation (me, 270.0);
	if (! farr)
		Graphics_setInner (me);
	Graphics_text (me, 1.0, 0.5, text);
	if (! farr)
		Graphics_unsetInner (me);

	Graphics_setTextRotation (me, 0.0);
	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setColour (me, original_colour);
}

void Graphics_textBottom (Graphics me, bool farr, conststring32 text) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const MelderColour original_colour = my colour;

	Graphics_setWindow (me, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	if (farr) {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (me, 0.5, 0.0, text);
	} else {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
		Graphics_setInner (me);
		Graphics_text (me, 0.5, - my vertTick, text);
		Graphics_unsetInner (me);
	}

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setColour (me, original_colour);
}

void Graphics_textTop (Graphics me, bool farr, conststring32 text) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const MelderColour original_colour = my colour;

	Graphics_setWindow (me, 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	if (farr) {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
		Graphics_text (me, 0.5, 1.0, text);
	} else {
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_setInner (me);
		Graphics_text (me, 0.5, 1.0 + my vertTick, text);
		Graphics_unsetInner (me);
	}

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setColour (me, original_colour);
}

void Graphics_marksLeft (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const int original_lineType = my lineType;
	const double original_lineWidth = my lineWidth;
	const MelderColour original_colour = my colour;
	if (numberOfMarks < 2)
		return;

	Graphics_setWindow (me, 0.0, 1.0, original_y1WC, original_y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * original_lineWidth);
	}
	for (int i = 1; i <= numberOfMarks; i ++) {
		const double f = (i - 1.0) / (numberOfMarks - 1);
		const double yWC = original_y1WC + (original_y2WC - original_y1WC) * f;
		if (haveNumbers)
			Graphics_text (me, - my horTick, yWC, Melder_float (Melder_half (yWC)));
		if (haveTicks)
			Graphics_line (me, - my horTick, yWC, 0, yWC);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, original_lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * original_lineWidth);
		for (int i = 2; i < numberOfMarks; i ++) {
			const double f = (i - 1.0) / (numberOfMarks - 1);
			const double yWC = original_y1WC + (original_y2WC - original_y1WC) * f;
			Graphics_line (me, 0.0, yWC, 1.0, yWC);
		}
		Graphics_setLineWidth (me, original_lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setLineType (me, original_lineType);
	Graphics_setColour (me, original_colour);
}

void Graphics_marksRight (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const int original_lineType = my lineType;
	const double original_lineWidth = my lineWidth;
	const MelderColour original_colour = my colour;
	if (numberOfMarks < 2)
		return;

	Graphics_setWindow (me, 0.0, 1.0, original_y1WC, original_y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * original_lineWidth);
	}
	for (int i = 1; i <= numberOfMarks; i ++) {
		const double f = (i - 1.0) / (numberOfMarks - 1);
		const double yWC = original_y1WC + (original_y2WC - original_y1WC) * f;
		if (haveNumbers)
			Graphics_text (me, 1.0 + my horTick, yWC, Melder_float (Melder_half (yWC)));
		if (haveTicks)
			Graphics_line (me, 1.0, yWC, 1.0 + my horTick, yWC);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, original_lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * original_lineWidth);
		for (int i = 2; i < numberOfMarks; i ++) {
			const double f = (i - 1.0) / (numberOfMarks - 1);
			const double yWC = original_y1WC + (original_y2WC - original_y1WC) * f;
			Graphics_line (me, 0.0, yWC, 1.0, yWC);
		}
		Graphics_setLineWidth (me, original_lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setLineType (me, original_lineType);
	Graphics_setColour (me, original_colour);
}

void Graphics_marksBottom (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	int original_lineType = my lineType;
	const double original_lineWidth = my lineWidth;
	const MelderColour original_colour = my colour;
	if (numberOfMarks < 2)
		return;

	Graphics_setWindow (me, original_x1WC, original_x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * original_lineWidth);
	}
	for (int i = 1; i <= numberOfMarks; i ++) {
		const double f = (i - 1.0) / (numberOfMarks - 1);
		const double xWC = original_x1WC + (original_x2WC - original_x1WC) * f;
		if (haveNumbers)
			Graphics_text (me, xWC, - my vertTick, Melder_float (Melder_half (xWC)));
		if (haveTicks)
			Graphics_line (me, xWC, - my vertTick, xWC, 0);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, original_lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * original_lineWidth);
		for (int i = 2; i < numberOfMarks; i ++) {
			const double f = (i - 1.0) / (numberOfMarks - 1);
			const double xWC = original_x1WC + (original_x2WC - original_x1WC) * f;
			Graphics_line (me, xWC, 0.0, xWC, 1.0);
		}
		Graphics_setLineWidth (me, original_lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setLineType (me, original_lineType);
	Graphics_setColour (me, original_colour);
}

void Graphics_marksTop (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double original_x1WC = my d_x1WC, original_x2WC = my d_x2WC, original_y1WC = my d_y1WC, original_y2WC = my d_y2WC;
	const int original_lineType = my lineType;
	const double original_lineWidth = my lineWidth;
	const MelderColour original_colour = my colour;
	if (numberOfMarks < 2)
		return;

	Graphics_setWindow (me, original_x1WC, original_x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * original_lineWidth);
	}
	for (int i = 1; i <= numberOfMarks; i ++) {
		const double f = (i - 1.0) / (numberOfMarks - 1);
		const double xWC = original_x1WC + (original_x2WC - original_x1WC) * f;
		if (haveNumbers)
			Graphics_text (me, xWC, 1.0 + my vertTick, Melder_float (Melder_half (xWC)));
		if (haveTicks)
			Graphics_line (me, xWC, 1.0, xWC, 1.0 + my vertTick);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, original_lineWidth);
	if (haveDottedLines && numberOfMarks > 2) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * original_lineWidth);
		for (int i = 2; i < numberOfMarks; i ++) {
			const double f = (i - 1.0) / (numberOfMarks - 1);
			const double xWC = original_x1WC + (original_x2WC - original_x1WC) * f;
			Graphics_line (me, xWC, 0.0, xWC, 1.0);
		}
		Graphics_setLineWidth (me, original_lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, original_x1WC, original_x2WC, original_y1WC, original_y2WC);
	Graphics_setLineType (me, original_lineType);
	Graphics_setColour (me, original_colour);
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

void Graphics_marksLeftLogarithmic (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1 = my d_x1WC, x2 = my d_x2WC, y1 = my d_y1WC, y2 = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	Melder_clip (1, & numberOfMarksPerDecade, MAXNUM_MARKS_PER_DECADE);
	if (y1 > 300.0 || y2 > 300.0)
		return;

	const double py1 = pow (10.0, y1 + ( y1 < y2 ? -1e-6 : 1e-6 ));
	const double py2 = pow (10.0, y2 + ( y1 < y2 ? 1e-6 : -1e-6 ));
	Graphics_setWindow (me, 0, 1, y1, y2);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	for (int i = 1; i <= numberOfMarksPerDecade; i ++) {
		double y = decade_y [numberOfMarksPerDecade] [i];
		while (y < (y1<y2?py1:py2))
			y *= 10.0;
		while (y >= (y1<y2?py1:py2))
			y /= 10.0;
		for (y *= 10.0; y <= (y1<y2?py2:py1); y *= 10.0) {
			if (haveNumbers)
				Graphics_text (me, - my horTick, log10 (y), Melder_float (Melder_half (y)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, - my horTick, log10 (y), 0, log10 (y));
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, 0.0, log10 (y), 1.0, log10 (y));
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_marksRightLogarithmic (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1 = my d_x1WC, x2 = my d_x2WC, y1 = my d_y1WC, y2 = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	Melder_clip (1, & numberOfMarksPerDecade, MAXNUM_MARKS_PER_DECADE);
	if (y1 > 300.0 || y2 > 300.0)
		return;

	const double py1 = pow (10.0, y1 + ( y1 < y2 ? -1e-6 : 1e-6 ));
	const double py2 = pow (10.0, y2 + ( y1 < y2 ? 1e-6 : -1e-6 ));
	Graphics_setWindow (me, 0.0, 1.0, y1, y2);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	for (int i = 1; i <= numberOfMarksPerDecade; i ++) {
		double y = decade_y [numberOfMarksPerDecade] [i];
		while (y < (y1<y2?py1:py2))
			y *= 10.0;
		while (y >= (y1<y2?py1:py2))
			y /= 10.0;
		for (y *= 10.0; y <= (y1<y2?py2:py1); y *= 10.0) {
			if (haveNumbers)
				Graphics_text (me, 1.0 + my horTick, log10 (y), Melder_float (Melder_half (y)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, 1.0, log10 (y), 1.0 + my horTick, log10 (y));
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, 0.0, log10 (y), 1.0, log10 (y));
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_marksTopLogarithmic (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1 = my d_x1WC, x2 = my d_x2WC, y1 = my d_y1WC, y2 = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	Melder_clip (1, & numberOfMarksPerDecade, MAXNUM_MARKS_PER_DECADE);
	if (x1 > 300.0 || x2 > 300.0)
		return;

	const double px1 = pow (10.0, x1 + ( x1 < x2 ? -1e-6 : 1e-6 ));
	const double px2 = pow (10.0, x2 + ( x1 < x2 ? 1e-6 : -1e-6 ));
	Graphics_setWindow (me, x1, x2, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	for (int i = 1; i <= numberOfMarksPerDecade; i ++) {
		double x = decade_y [numberOfMarksPerDecade] [i];
		while (x < (x1<x2?px1:px2))
			x *= 10.0;
		while (x >= (x1<x2?px1:px2))
			x /= 10.0;
		for (x *= 10.0; x <= (x1<x2?px2:px1); x *= 10.0) {
			if (haveNumbers)
				Graphics_text (me, log10 (x), 1.0 + my vertTick, Melder_float (Melder_half (x)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, log10 (x), 1.0, log10 (x), 1.0 + my vertTick);
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, log10 (x), 0.0, log10 (x), 1.0);
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_marksBottomLogarithmic (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1 = my d_x1WC, x2 = my d_x2WC, y1 = my d_y1WC, y2 = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	Melder_clip (1, & numberOfMarksPerDecade, MAXNUM_MARKS_PER_DECADE);
	if (x1 > 300.0 || x2 > 300.0)
		return;

	const double px1 = pow (10.0, x1 + ( x1 < x2 ? -1e-6 : 1e-6 ));
	const double px2 = pow (10.0, x2 + ( x1 < x2 ? 1e-6 : -1e-6 ));
	Graphics_setWindow (me, x1, x2, 0, 1);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	for (int i = 1; i <= numberOfMarksPerDecade; i ++) {
		double x = decade_y [numberOfMarksPerDecade] [i];
		while (x < (x1<x2?px1:px2))
			x *= 10.0;
		while (x >= (x1<x2?px1:px2))
			x /= 10.0;
		for (x *= 10.0; x <= (x1<x2?px2:px1); x *= 10.0) {
			if (haveNumbers)
				Graphics_text (me, log10 (x), - my vertTick, Melder_float (Melder_half (x)));
			if (haveTicks) {
				Graphics_setLineWidth (me, 2.0 * lineWidth);
				Graphics_setLineType (me, Graphics_DRAWN);
				Graphics_line (me, log10 (x), - my vertTick, log10 (x), 0.0);
				Graphics_setLineWidth (me, lineWidth);
			}
			if (haveDottedLines) {
				Graphics_setLineType (me, Graphics_DOTTED);
				Graphics_setLineWidth (me, 0.67 * lineWidth);
				Graphics_line (me, log10 (x), 0.0, log10 (x), 1.0);
				Graphics_setLineType (me, lineType);
				Graphics_setLineWidth (me, lineWidth);
			}
		}
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1, x2, y1, y2);
	Graphics_setColour (me, colour);
}

void Graphics_markLeft (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;

	Graphics_setWindow (me, 0.0, 1.0, y1WC, y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, - my horTick, position, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, - my horTick, position, 0.0, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0.0, position, 1.0, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, - my horTick, position, text);   // 'text' has to stay valid until here; no Graphics is allowed to use the cat buffer!
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markRight (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;

	Graphics_setWindow (me, 0.0, 1.0, y1WC, y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, 1.0 + my horTick, position, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, 1.0, position, 1.0 + my horTick, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0.0, position, 1.0, position);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, 1.0 + my horTick, position, text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markTop (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;

	Graphics_setWindow (me, x1WC, x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, position, 1.0 + my vertTick, Melder_float (Melder_single (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, position, 1.0, position, 1.0 + my vertTick);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, position, 0.0, position, 1.0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, position, 1.0 + my vertTick, text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markBottom (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;

	Graphics_setWindow (me, x1WC, x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, position, - my vertTick, Melder_float (Melder_single (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, position, - my vertTick, position, 0.0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, position, 0.0, position, 1.0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, position, - my vertTick, text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markLeftLogarithmic (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	if (position <= 0.0)
		return;

	Graphics_setWindow (me, 0.0, 1.0, y1WC, y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, - my horTick, log10 (position), Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, - my horTick, log10 (position), 0.0, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0.0, log10 (position), 1.0, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, - my horTick, log10 (position), text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markRightLogarithmic (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	if (position <= 0.0)
		return;

	Graphics_setWindow (me, 0.0, 1.0, y1WC, y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, 1.0 + my horTick, log10 (position), Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, 1.0, log10 (position), 1.0 + my horTick, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, 0.0, log10 (position), 1.0, log10 (position));
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, 1.0 + my horTick, log10 (position), text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markTopLogarithmic (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	if (position <= 0.0)
		return;

	Graphics_setWindow (me, x1WC, x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, log10 (position), 1.0 + my vertTick, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, log10 (position), 1.0, log10 (position), 1.0 + my vertTick);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, log10 (position), 0.0, log10 (position), 1.0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, log10 (position), 1.0 + my vertTick, text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_markBottomLogarithmic (Graphics me, double position, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	if (position <= 0.0)
		return;

	Graphics_setWindow (me, x1WC, x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (hasNumber)
		Graphics_text (me, log10 (position), - my vertTick, Melder_float (Melder_half (position)));
	if (hasTick) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
		Graphics_line (me, log10 (position), - my vertTick, log10 (position), 0.0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (hasDottedLine) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		Graphics_line (me, log10 (position), 0.0, log10 (position), 1.0);
		Graphics_setLineWidth (me, lineWidth);
	}
	if (text && text [0])
		Graphics_text (me, log10 (position), - my vertTick, text);
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksLeftEvery (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	distance *= units;

	const integer first = Melder_iceiling (( y1WC < y2WC ? y1WC : y2WC ) / distance - 1e-5);
	const integer last  = Melder_ifloor   (( y1WC < y2WC ? y2WC : y1WC ) / distance + 1e-5);
	Graphics_setWindow (me, 0.0, 1.0, y1WC, y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_RIGHT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (integer i = first; i <= last; i ++) {
		const double yWC = i * distance;
		if (haveNumbers)
			Graphics_text (me, - my horTick, yWC, Melder_float (Melder_half (yWC / units)));
		if (haveTicks)
			Graphics_line (me, - my horTick, yWC, 0.0, yWC);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (integer i = first; i <= last; i ++) {
			const double yWC = i * distance;
			Graphics_line (me, 0.0, yWC, 1.0, yWC);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksRightEvery (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	distance *= units;

	const integer first = Melder_iceiling (( y1WC < y2WC ? y1WC : y2WC ) / distance - 1e-5);
	const integer last  = Melder_ifloor   (( y1WC < y2WC ? y2WC : y1WC ) / distance + 1e-5);
	if (first > last)
		return;   // TODO: describe why this is. ppgb 2020-10-01
	Graphics_setWindow (me, 0.0, 1.0, y1WC, y2WC);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_LEFT, Graphics_HALF);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (integer i = first; i <= last; i ++) {
		const double yWC = i * distance;
		if (haveNumbers)
			Graphics_text (me, 1.0 + my horTick, yWC, Melder_float (Melder_half (yWC / units)));
		if (haveTicks)
			Graphics_line (me, 1.0, yWC, 1.0 + my horTick, yWC);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (integer i = first; i <= last; i ++) {
			const double yWC = i * distance;
			Graphics_line (me, 0.0, yWC, 1.0, yWC);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksBottomEvery (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	distance *= units;

	const integer first = Melder_iceiling (( x1WC < x2WC ? x1WC : x2WC ) / distance - 1e-5);
	const integer last  = Melder_ifloor   (( x1WC < x2WC ? x2WC : x1WC ) / distance + 1e-5);
	Graphics_setWindow (me, x1WC, x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_TOP);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (integer i = first; i <= last; i ++) {
		const double xWC = i * distance;
		if (haveNumbers)
			Graphics_text (me, xWC, - my vertTick, Melder_float (Melder_half (xWC / units)));
		if (haveTicks)
			Graphics_line (me, xWC, - my vertTick, xWC, 0.0);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (integer i = first; i <= last; i ++) {
			const double xWC = i * distance;
			Graphics_line (me, xWC, 0.0, xWC, 1.0);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_marksTopEvery (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines) {
	const double x1WC = my d_x1WC, x2WC = my d_x2WC, y1WC = my d_y1WC, y2WC = my d_y2WC;
	const int lineType = my lineType;
	const double lineWidth = my lineWidth;
	const MelderColour colour = my colour;
	distance *= units;

	const integer first = Melder_iceiling (( x1WC < x2WC ? x1WC : x2WC ) / distance - 1e-5);
	const integer last  = Melder_ifloor   (( x1WC < x2WC ? x2WC : x1WC ) / distance + 1e-5);
	Graphics_setWindow (me, x1WC, x2WC, 0.0, 1.0);
	Graphics_setColour (me, Melder_BLACK);
	Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (me);
	if (haveTicks) {
		Graphics_setLineType (me, Graphics_DRAWN);
		Graphics_setLineWidth (me, 2.0 * lineWidth);
	}
	for (integer i = first; i <= last; i ++) {
		const double xWC = i * distance;
		if (haveNumbers)
			Graphics_text (me, xWC, 1.0 + my vertTick, Melder_float (Melder_half (xWC / units)));
		if (haveTicks)
			Graphics_line (me, xWC, 1.0, xWC, 1.0 + my vertTick);
	}
	if (haveTicks)
		Graphics_setLineWidth (me, lineWidth);
	if (haveDottedLines) {
		Graphics_setLineType (me, Graphics_DOTTED);
		Graphics_setLineWidth (me, 0.67 * lineWidth);
		for (integer i = first; i <= last; i ++) {
			const double xWC = i * distance;
			Graphics_line (me, xWC, 0.0, xWC, 1.0);
		}
		Graphics_setLineWidth (me, lineWidth);
	}
	Graphics_unsetInner (me);

	Graphics_setWindow (me, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineType (me, lineType);
	Graphics_setColour (me, colour);
}

void Graphics_mark (Graphics me, double x, double y, double size_mm, conststring32 markString /* cattable */) {
	int mark;
	if (! markString || ! markString [0])
		mark = 0;
	else if (! markString [1]) {
		if (markString [0] == '+')
			mark = 1;
		else if (markString [0] == 'x')
			mark = 2;
		else if (markString [0] == 'o')
			mark = 3;
		else if (markString [0] == '.')
			mark = 0;
		else
			mark = -1;
	} else
		mark = -1;
	if (mark == -1) {
		const double oldSize = my fontSize;
		const int oldHorizontalAlignment = my horizontalTextAlignment;
		const int oldVerticalAlignment = my verticalTextAlignment;
		Graphics_setFontSize (me, size_mm * 72.0 / 25.4);
		Graphics_setTextAlignment (me, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (me, x, y, markString);
		Graphics_setFontSize (me, oldSize);
		Graphics_setTextAlignment (me, (kGraphics_horizontalAlignment) oldHorizontalAlignment, oldVerticalAlignment);
	} else if (mark == 0) {
		Graphics_fillCircle_mm (me, x, y, size_mm);
	} else if (mark == 1) {
		const double dx = 0.5 * Graphics_dxMMtoWC (me, size_mm);
		const double dy = 0.5 * Graphics_dyMMtoWC (me, size_mm);
		Graphics_line (me, x - dx, y, x + dx, y);
		Graphics_line (me, x, y - dy, x, y + dy);
	} else if (mark == 2) {
		const double dx = 0.4 * Graphics_dxMMtoWC (me, size_mm);
		const double dy = 0.4 * Graphics_dyMMtoWC (me, size_mm);
		Graphics_line (me, x - dx, y - dy, x + dx, y + dy);
		Graphics_line (me, x + dx, y - dy, x - dx, y + dy);
	} else {
		Graphics_circle_mm (me, x, y, size_mm);
	}
}

void Graphics_setTextRotation_vector (Graphics me, double dx, double dy) {
	double angle;
	if (dy == 0.0) {
		angle = ( dx >= 0.0 ? 0.0 : 180.0 );
	} else if (dx == 0.0) {
		angle = ( dy > 0.0 ? 90.0 : 270.0 );
	} else {
		const double dxDC = dx * my scaleX;
		const double dyDC = ( my yIsZeroAtTheTop ? -dy * my scaleY : dy * my scaleY );
		angle = atan2 (dyDC, dxDC) * (180.0 / NUMpi);
	}
	Graphics_setTextRotation (me, angle);
}

/* End of file Graphics_utils.cpp */
