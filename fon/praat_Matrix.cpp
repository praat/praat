/* praat_Matrix.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016 Paul Boersma
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

#include "Cochleagram.h"
#include "Excitation.h"
#include "Harmonicity.h"
#include "Intensity.h"
#include "Ltas.h"
#include "Matrix_and_PointProcess.h"
#include "Matrix_and_Polygon.h"
#include "Matrix_and_Pitch.h"
#include "MovieWindow.h"
#include "ParamCurve.h"
#include "Photo.h"
#include "Spectrogram.h"
#include "Spectrum.h"
#include "Transition.h"
#include "VocalTract.h"

#include "praat_Matrix.h"

#undef iam
#define iam iam_LOOP

int praat_Matrix_formula (UiForm dia, Interpreter interpreter) {
	int IOBJECT;
	LOOP {
		iam (Matrix);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
	return 1;
}

// MARK: - MATRIX

// MARK: New

FORM (NEW1_Matrix_create, U"Create Matrix", U"Create Matrix...") {
	WORD (U"Name", U"xy")
	REAL (U"xmin", U"1.0")
	REAL (U"xmax", U"1.0")
	NATURAL (U"Number of columns", U"1")
	POSITIVE (U"dx", U"1.0")
	REAL (U"x1", U"1.0")
	REAL (U"ymin", U"1.0")
	REAL (U"ymax", U"1.0")
	NATURAL (U"Number of rows", U"1")
	POSITIVE (U"dy", U"1.0")
	REAL (U"y1", U"1.0")
	LABEL (U"", U"Formula:")
	TEXTFIELD (U"formula", U"x*y")
	OK
DO
	double xmin = GET_REAL (U"xmin"), xmax = GET_REAL (U"xmax");
	double ymin = GET_REAL (U"ymin"), ymax = GET_REAL (U"ymax");
	if (xmax < xmin) Melder_throw (U"xmax (", Melder_single (xmax), U") should not be less than xmin (", Melder_single (xmin), U").");
	if (ymax < ymin) Melder_throw (U"ymax (", Melder_single (ymax), U") should not be less than ymin (", Melder_single (ymin), U").");
	autoMatrix me = Matrix_create (
		xmin, xmax, GET_INTEGER (U"Number of columns"), GET_REAL (U"dx"), GET_REAL (U"x1"),
		ymin, ymax, GET_INTEGER (U"Number of rows"), GET_REAL (U"dy"), GET_REAL (U"y1"));
	Matrix_formula (me.get(), GET_STRING (U"formula"), interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END }

FORM (NEW1_Matrix_createSimple, U"Create simple Matrix", U"Create simple Matrix...") {
	WORD4 (name, U"Name", U"xy")
	NATURAL4 (numberOfRows, U"Number of rows", U"10")
	NATURAL4 (numberOfColumns, U"Number of columns", U"10")
	LABEL (U"", U"Formula:")
	TEXTFIELD4 (formula, U"formula", U"x*y")
	OK
DO
	CREATE_ONE
		autoMatrix result = Matrix_createSimple (numberOfRows, numberOfColumns);
		Matrix_formula (result.get(), formula, interpreter, nullptr);
	CREATE_ONE_END (name);
}

// MARK: Open

FORM_READ (READ1_Matrix_readFromRawTextFile, U"Read Matrix from raw text file", nullptr, true) {
	READ_ONE
		autoMatrix result = Matrix_readFromRawTextFile (file);
	READ_ONE_END
}

FORM_READ (READ1_Matrix_readAP, U"Read Matrix from LVS AP file", nullptr, true) {
	READ_ONE
		autoMatrix result = Matrix_readAP (file);
	READ_ONE_END
}

// MARK: Save

FORM_SAVE (SAVE_Matrix_writeToMatrixTextFile, U"Save Matrix as matrix text file", nullptr, U"mat") {
	Matrix me = FIRST (Matrix);
	Matrix_writeToMatrixTextFile (me, file);
END }

FORM_SAVE (SAVE_Matrix_writeToHeaderlessSpreadsheetFile, U"Save Matrix as spreadsheet", nullptr, U"txt") {
	Matrix me = FIRST (Matrix);
	Matrix_writeToHeaderlessSpreadsheetFile (me, file);
END }

// MARK: Help

DIRECT (HELP_Matrix_help) {
	Melder_help (U"Matrix");
END }

// MARK: Movie

static autoGraphics theMovieGraphics;

static void gui_drawingarea_cb_expose (Thing /* boss */, GuiDrawingArea_ExposeEvent /* event */) {
	if (! theMovieGraphics) return;
	Graphics_play (theMovieGraphics.get(), theMovieGraphics.get());
}

extern "C" Graphics Movie_create (const char32 *title, int width, int height) {
	static GuiDialog dialog;
	static GuiDrawingArea drawingArea;
	if (! theMovieGraphics) {
		dialog = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, width + 2, height + 2, title, nullptr, nullptr, 0);
		drawingArea = GuiDrawingArea_createShown (dialog, 0, width, 0, height, gui_drawingarea_cb_expose, nullptr, nullptr, nullptr, nullptr, 0);
		GuiThing_show (dialog);
		theMovieGraphics = Graphics_create_xmdrawingarea (drawingArea);
	}
	GuiShell_setTitle (dialog, title);
	GuiControl_setSize (dialog, width + 2, height + 2);
	GuiControl_setSize (drawingArea, width, height);
	GuiThing_show (dialog);
	return theMovieGraphics.get();
}

DIRECT (MOVIE_Matrix_movie) {
	MOVIE_ONE (Matrix, U"Matrix movie", 300, 300)
		Matrix_movie (me, graphics);
	MOVIE_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_Matrix_drawRows, U"Draw rows", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawRows (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="),
			GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END }

FORM (GRAPHICS_Matrix_drawOneContour, U"Draw one altitude contour", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Height", U"0.5")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawOneContour (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Height"));
	}
END }

FORM (GRAPHICS_Matrix_drawContours, U"Draw altitude contours", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_drawContours (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END }

FORM (GRAPHICS_Matrix_paintImage, U"Matrix: Paint grey image", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintImage (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END }

FORM (GRAPHICS_Matrix_paintContours, U"Matrix: Paint altitude contours with greys", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintContours (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END }

FORM (GRAPHICS_Matrix_paintCells, U"Matrix: Paint cells with greys", U"Matrix: Paint cells...") {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintCells (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"));
	}
END }

FORM (GRAPHICS_Matrix_paintSurface, U"Matrix: Paint 3-D surface plot", nullptr) {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	REAL (U"Minimum", U"0.0")
	REAL (U"Maximum", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		autoPraatPicture picture;
		Matrix_paintSurface (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="),
			GET_REAL (U"Minimum"), GET_REAL (U"Maximum"), 30, 45);
	}
END }

// MARK: Query

DIRECT (REAL_Matrix_getLowestX) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my xmin, nullptr);
END }

DIRECT (REAL_Matrix_getHighestX) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my xmax, nullptr);
END }

DIRECT (REAL_Matrix_getLowestY) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my ymin, nullptr);
END }

DIRECT (REAL_Matrix_getHighestY) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my ymax, nullptr);
END }

DIRECT (INTEGER_Matrix_getNumberOfRows) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_information (my ny);
END }

DIRECT (INTEGER_Matrix_getNumberOfColumns) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_information (my nx);
END }

DIRECT (REAL_Matrix_getRowDistance) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my dy, nullptr);
END }

DIRECT (REAL_Matrix_getColumnDistance) {
	Matrix me = FIRST_ANY (Matrix);
	Melder_informationReal (my dx, nullptr);
END }

FORM (REAL_Matrix_getYofRow, U"Matrix: Get y of row", nullptr) {
	NATURAL (U"Row number", U"1")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double y = Matrix_rowToY (me, GET_INTEGER (U"Row number"));
	Melder_informationReal (y, nullptr);
END }

FORM (REAL_Matrix_getXofColumn, U"Matrix: Get x of column", nullptr) {
	NATURAL (U"Column number", U"1")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double x = Matrix_columnToX (me, GET_INTEGER (U"Column number"));
	Melder_informationReal (x, nullptr);
END }

FORM (REAL_Matrix_getValueInCell, U"Matrix: Get value in cell", nullptr) {
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	long row = GET_INTEGER (U"Row number"), column = GET_INTEGER (U"Column number");
	if (row > my ny) Melder_throw (U"Row number must not exceed number of rows.");
	if (column > my nx) Melder_throw (U"Column number must not exceed number of columns.");
	Melder_informationReal (my z [row] [column], nullptr);
END }

FORM (REAL_Matrix_getValueAtXY, U"Matrix: Get value at xy", nullptr) {
	REALVAR (x, U"X", U"0.0")
	REALVAR (y, U"Y", U"0.0")
	OK
DO
	Matrix me = FIRST_ANY (Matrix);
	double value = Matrix_getValueAtXY (me, x, y);
	Melder_information (value, U" (at x = ", x, U" and y = ", y, U")");
END }

DIRECT (REAL_Matrix_getMinimum) {
	Matrix me = FIRST_ANY (Matrix);
	double minimum = NUMundefined, maximum = NUMundefined;
	Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
	Melder_informationReal (minimum, nullptr);
END }

DIRECT (REAL_Matrix_getMaximum) {
	Matrix me = FIRST_ANY (Matrix);
	double minimum = NUMundefined, maximum = NUMundefined;
	Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
	Melder_informationReal (maximum, nullptr);
END }

DIRECT (REAL_Matrix_getSum) {
	Matrix me = FIRST_ANY (Matrix);
	double sum = Matrix_getSum (me);
	Melder_informationReal (sum, nullptr);
END }

// MARK: Modify

FORM (MODIFY_Matrix_formula, U"Matrix Formula", U"Formula...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Matrix);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Matrix may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_Matrix_setValue, U"Matrix: Set value", U"Matrix: Set value...") {
	NATURAL (U"Row number", U"1")
	NATURAL (U"Column number", U"1")
	REAL (U"New value", U"0.0")
	OK
DO
	LOOP {
		iam (Matrix);
		long row = GET_INTEGER (U"Row number"), column = GET_INTEGER (U"Column number");
		if (row > my ny) Melder_throw (U"Row number must not be greater than number of rows.");
		if (column > my nx) Melder_throw (U"Column number must not be greater than number of columns.");
		my z [row] [column] = GET_REAL (U"New value");
		praat_dataChanged (me);
	}
END }

// MARK: Analyse

DIRECT (NEWTIMES2_Matrix_eigen) {
	LOOP {
		iam (Matrix);
		autoMatrix vectors, values;
		Matrix_eigen (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	}
END }

// MARK: Synthesize

FORM (NEW_Matrix_power, U"Matrix: Power...", nullptr) {
	NATURAL (U"Power", U"2")
	OK
DO
	LOOP {
		iam (Matrix);
		autoMatrix thee = Matrix_power (me, GET_INTEGER (U"Power"));
		praat_new (thee.move(), my name);
	}
END }

// MARK: Combine

DIRECT (NEW1_Matrix_appendRows) {
	Matrix m1 = nullptr, m2 = nullptr;
	LOOP (m1 ? m2 : m1) = (Matrix) OBJECT;
	autoMatrix thee = Matrix_appendRows (m1, m2, classMatrix);
	praat_new (thee.move(), m1 -> name, U"_", m2 -> name);
END }

// MARK: Cast

DIRECT (NEW_Matrix_to_Cochleagram) {
	LOOP {
		iam (Matrix);
		autoCochleagram thee = Matrix_to_Cochleagram (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Excitation) {
	LOOP {
		iam (Matrix);
		autoExcitation thee = Matrix_to_Excitation (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Harmonicity) {
	LOOP {
		iam (Matrix);
		autoHarmonicity thee = Matrix_to_Harmonicity (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Intensity) {
	LOOP {
		iam (Matrix);
		autoIntensity thee = Matrix_to_Intensity (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Ltas) {
	LOOP {
		iam (Matrix);
		autoLtas thee = Matrix_to_Ltas (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Pitch) {
	LOOP {
		iam (Matrix);
		autoPitch thee = Matrix_to_Pitch (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_PointProcess) {
	LOOP {
		iam (Matrix);
		autoPointProcess thee = Matrix_to_PointProcess (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Polygon) {
	LOOP {
		iam (Matrix);
		autoPolygon thee = Matrix_to_Polygon (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Sound) {
	LOOP {
		iam (Matrix);
		autoSound thee = Matrix_to_Sound (me);
		praat_new (thee.move(), my name);
	}
END }

FORM (NEW_Matrix_to_Sound_mono, U"Matrix: To Sound (mono)", 0) {
	INTEGER (U"Row", U"1")
	LABEL (U"", U"(negative values count from last row)")
	OK
DO
	LOOP {
		iam (Matrix);
		autoSound thee = Matrix_to_Sound_mono (me, GET_INTEGER (U"Row"));
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Spectrogram) {
	LOOP {
		iam (Matrix);
		autoSpectrogram thee = Matrix_to_Spectrogram (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Spectrum) {
	LOOP {
		iam (Matrix);
		autoSpectrum thee = Matrix_to_Spectrum (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_TableOfReal) {
	LOOP {
		iam (Matrix);
		autoTableOfReal thee = Matrix_to_TableOfReal (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_Transition) {
	LOOP {
		iam (Matrix);
		autoTransition thee = Matrix_to_Transition (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_Matrix_to_VocalTract) {
	LOOP {
		iam (Matrix);
		autoVocalTract thee = Matrix_to_VocalTract (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW1_Matrix_to_ParamCurve) {
	Matrix m1 = nullptr, m2 = nullptr;
	LOOP (m1 ? m2 : m1) = (Matrix) OBJECT;
	autoSound sound1 = Matrix_to_Sound (m1), sound2 = Matrix_to_Sound (m2);
	autoParamCurve thee = ParamCurve_create (sound1.get(), sound2.get());
	praat_new (thee.move(), m1 -> name, U"_", m2 -> name);
END }

// MARK: - PHOTO

// MARK: New

FORM (NEW1_Photo_create, U"Create Photo", U"Create Photo...") {
	WORD (U"Name", U"xy")
	REAL (U"xmin", U"1.0")
	REAL (U"xmax", U"1.0")
	NATURAL (U"Number of columns", U"1")
	POSITIVE (U"dx", U"1.0")
	REAL (U"x1", U"1.0")
	REAL (U"ymin", U"1.0")
	REAL (U"ymax", U"1.0")
	NATURAL (U"Number of rows", U"1")
	POSITIVE (U"dy", U"1.0")
	REAL (U"y1", U"1.0")
	LABEL (U"", U"Red formula:")
	TEXTFIELD (U"redFormula", U"x*y/100")
	LABEL (U"", U"Green formula:")
	TEXTFIELD (U"greenFormula", U"x*y/1000")
	LABEL (U"", U"Blue formula:")
	TEXTFIELD (U"blueFormula", U"x*y/100")
	OK
DO
	double xmin = GET_REAL (U"xmin"), xmax = GET_REAL (U"xmax");
	double ymin = GET_REAL (U"ymin"), ymax = GET_REAL (U"ymax");
	if (xmax < xmin) Melder_throw (U"xmax (", Melder_single (xmax), U") should not be less than xmin (", Melder_single (xmin), U").");
	if (ymax < ymin) Melder_throw (U"ymax (", Melder_single (ymax), U") should not be less than ymin (", Melder_single (ymin), U").");
	autoPhoto me = Photo_create (
		xmin, xmax, GET_INTEGER (U"Number of columns"), GET_REAL (U"dx"), GET_REAL (U"x1"),
		ymin, ymax, GET_INTEGER (U"Number of rows"), GET_REAL (U"dy"), GET_REAL (U"y1"));
	Matrix_formula (my d_red  .get(), GET_STRING (U"redFormula"),   interpreter, nullptr);
	Matrix_formula (my d_green.get(), GET_STRING (U"greenFormula"), interpreter, nullptr);
	Matrix_formula (my d_blue .get(), GET_STRING (U"blueFormula"),  interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END }

FORM (NEW1_Photo_createSimple, U"Create simple Photo", U"Create simple Photo...") {
	WORD (U"Name", U"xy")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"10")
	LABEL (U"", U"Red formula:")
	TEXTFIELD (U"redFormula", U"x*y/100")
	LABEL (U"", U"Green formula:")
	TEXTFIELD (U"greenFormula", U"x*y/1000")
	LABEL (U"", U"Blue formula:")
	TEXTFIELD (U"blueFormula", U"x*y/100")
	OK
DO
	autoPhoto me = Photo_createSimple (GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"));
	Matrix_formula (my d_red.get(),   GET_STRING (U"redFormula"),   interpreter, nullptr);
	Matrix_formula (my d_green.get(), GET_STRING (U"greenFormula"), interpreter, nullptr);
	Matrix_formula (my d_blue.get(),  GET_STRING (U"blueFormula"),  interpreter, nullptr);
	praat_new (me.move(), GET_STRING (U"Name"));
END }

DIRECT (NEW_Photo_extractBlue) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_blue.get());
		praat_new (thee.move(), my name, U"_blue");
	}
END }

DIRECT (NEW_Photo_extractGreen) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_green.get());
		praat_new (thee.move(), my name, U"_green");
	}
END }

DIRECT (NEW_Photo_extractRed) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_red.get());
		praat_new (thee.move(), my name, U"_red");
	}
END }

DIRECT (NEW_Photo_extractTransparency) {
	LOOP {
		iam (Photo);
		autoMatrix thee = Data_copy (my d_transparency.get());
		praat_new (thee.move(), my name, U"_transparency");
	}
END }

FORM (MODIFY_Photo_formula_red, U"Photo Formula (red)", U"Formula (red)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_red.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_Photo_formula_green, U"Photo Formula (green)", U"Formula (green)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_green.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_Photo_formula_blue, U"Photo Formula (blue)", U"Formula (blue)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_blue.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END }

FORM (MODIFY_Photo_formula_transparency, U"Photo Formula (transparency)", U"Formula (transparency)...") {
	LABEL (U"label", U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	TEXTFIELD (U"formula", U"self")
	OK
DO
	LOOP {
		iam (Photo);
		try {
			Matrix_formula (my d_transparency.get(), GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Photo may have partially changed
			throw;
		}
	}
END }

FORM (GRAPHICS_Photo_paintCells, U"Photo: Paint cells with colour", U"Photo: Paint cells...") {
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"0.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"0.0")
	OK
DO
	LOOP {
		iam (Photo);
		autoPraatPicture picture;
		Photo_paintCells (me, GRAPHICS,
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="));
	}
END }

FORM (GRAPHICS_Photo_paintImage, U"Photo: Paint colour image", nullptr) {
	REAL4 (fromX, U"From x =", U"0.0")
	REAL4 (toX, U"To x =", U"0.0")
	REAL4 (fromY, U"From y =", U"0.0")
	REAL4 (toY, U"To y =", U"0.0")
	OK
DO
	GRAPHICS_EACH (Photo)
		Photo_paintImage (me, GRAPHICS, fromX, toX, fromY, toY);
	GRAPHICS_EACH_END
}

FORM_SAVE (SAVE_Photo_saveAsAppleIconFile, U"Save as Apple icon file", nullptr, U"icns") {
	FIND_ONE (Photo)
		Photo_saveAsAppleIconFile (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsGIF, U"Save as GIF file", nullptr, U"gif") {
	FIND_ONE (Photo)
		Photo_saveAsGIF (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsJPEG, U"Save as JPEG file", nullptr, U"jpg") {
	FIND_ONE (Photo)
		Photo_saveAsJPEG (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsJPEG2000, U"Save as JPEG-2000 file", nullptr, U"jpg") {
	FIND_ONE (Photo)
		Photo_saveAsJPEG2000 (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsPNG, U"Save as PNG file", nullptr, U"png") {
	FIND_ONE (Photo)
		Photo_saveAsPNG (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsTIFF, U"Save as TIFF file", nullptr, U"tiff") {
	FIND_ONE (Photo)
		Photo_saveAsTIFF (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsWindowsBitmapFile, U"Save as Windows bitmap file", nullptr, U"bmp") {
	FIND_ONE (Photo)
		Photo_saveAsWindowsBitmapFile (me, file);
	END
}

FORM_SAVE (SAVE_Photo_saveAsWindowsIconFile, U"Save as Windows icon file", nullptr, U"ico") {
	FIND_ONE (Photo)
		Photo_saveAsWindowsIconFile (me, file);
	END
}

// MARK: - PHOTO & MATRIX

DIRECT (MODIFY_Photo_Matrix_replaceBlue) {
	MODIFY_FIRST_OF_TWO (Photo, Matrix)
		Photo_replaceBlue (me, you);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (MODIFY_Photo_Matrix_replaceGreen) {
	MODIFY_FIRST_OF_TWO (Photo, Matrix)
		Photo_replaceGreen (me, you);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (MODIFY_Photo_Matrix_replaceRed) {
	MODIFY_FIRST_OF_TWO (Photo, Matrix)
		Photo_replaceRed (me, you);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (MODIFY_Photo_Matrix_replaceTransparency) {
	MODIFY_FIRST_OF_TWO (Photo, Matrix)
		Photo_replaceTransparency (me, you);
	MODIFY_FIRST_OF_TWO_END
}

// MARK: - MOVIE

FORM_READ (READ1_Movie_openFromSoundFile, U"Open movie file", nullptr, true) {
	READ_ONE
		autoMovie result = Movie_openFromSoundFile (file);
	READ_ONE_END   // but loses data when saving, if object is associated only with the sound file
}

FORM (GRAPHICS_Movie_paintOneImage, U"Movie: Paint one image", nullptr) {
	NATURAL (U"Frame number", U"1")
	REAL (U"From x =", U"0.0")
	REAL (U"To x =", U"1.0")
	REAL (U"From y =", U"0.0")
	REAL (U"To y =", U"1.0")
	OK
DO
	LOOP {
		iam (Movie);
		autoPraatPicture picture;
		Movie_paintOneImage (me, GRAPHICS, GET_INTEGER (U"Frame number"),
			GET_REAL (U"From x ="), GET_REAL (U"To x ="), GET_REAL (U"From y ="), GET_REAL (U"To y ="));
	}
END }

DIRECT (WINDOW_Movie_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Movie from batch.");
	LOOP {
		iam (Movie);
		autoMovieWindow editor = MovieWindow_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END }

// MARK: file recognizers

static autoDaata imageFileRecognizer (int /* nread */, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	if (Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".jpg") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".JPG") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".jpeg") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".JPEG") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".png") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".PNG") ||
	    Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".tiff") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".TIFF") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".tif") ||
		Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".TIF"))
	{
		return Photo_readFromImageFile (file);
	}
	return autoDaata ();
}

// MARK: - buttons

void praat_Matrix_init () {
	Thing_recognizeClassesByName (classMatrix, classPhoto, classMovie, nullptr);

	Data_recognizeFileType (imageFileRecognizer);

	praat_addMenuCommand (U"Objects", U"New", U"Matrix", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Matrix...", nullptr, 1, NEW1_Matrix_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Matrix...", nullptr, 1, NEW1_Matrix_createSimple);
		praat_addMenuCommand (U"Objects", U"New", U"-- colour matrix --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Photo...", nullptr, 1, NEW1_Photo_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Photo...", nullptr, 1, NEW1_Photo_createSimple);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read movie --", nullptr, praat_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Open movie file...", nullptr, praat_HIDDEN, READ1_Movie_openFromSoundFile);
	praat_addMenuCommand (U"Objects", U"Open", U"-- read raw --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Matrix from raw text file...", nullptr, 0, READ1_Matrix_readFromRawTextFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Matrix from LVS AP file...", nullptr, praat_HIDDEN, READ1_Matrix_readAP);

	praat_addAction1 (classMatrix, 0, U"Matrix help", nullptr, 0, HELP_Matrix_help);
	praat_addAction1 (classMatrix, 1, U"Save as matrix text file...", nullptr, 0, SAVE_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1,   U"Write to matrix text file...", U"*Save as matrix text file...", praat_DEPRECATED_2011, SAVE_Matrix_writeToMatrixTextFile);
	praat_addAction1 (classMatrix, 1, U"Save as headerless spreadsheet file...", nullptr, 0, SAVE_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1,   U"Write to headerless spreadsheet file...", nullptr, praat_DEPRECATED_2011, SAVE_Matrix_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classMatrix, 1, U"Play movie", nullptr, 0, MOVIE_Matrix_movie);
	praat_addAction1 (classMatrix, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"Draw rows...", nullptr, 1, GRAPHICS_Matrix_drawRows);
		praat_addAction1 (classMatrix, 0, U"Draw one contour...", nullptr, 1, GRAPHICS_Matrix_drawOneContour);
		praat_addAction1 (classMatrix, 0, U"Draw contours...", nullptr, 1, GRAPHICS_Matrix_drawContours);
		praat_addAction1 (classMatrix, 0, U"Paint image...", nullptr, 1, GRAPHICS_Matrix_paintImage);
		praat_addAction1 (classMatrix, 0, U"Paint contours...", nullptr, 1, GRAPHICS_Matrix_paintContours);
		praat_addAction1 (classMatrix, 0, U"Paint cells...", nullptr, 1, GRAPHICS_Matrix_paintCells);
		praat_addAction1 (classMatrix, 0, U"Paint surface...", nullptr, 1, GRAPHICS_Matrix_paintSurface);
	praat_addAction1 (classMatrix, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 1, U"Get lowest x", nullptr, 1, REAL_Matrix_getLowestX);
		praat_addAction1 (classMatrix, 1, U"Get highest x", nullptr, 1, REAL_Matrix_getHighestX);
		praat_addAction1 (classMatrix, 1, U"Get lowest y", nullptr, 1, REAL_Matrix_getLowestY);
		praat_addAction1 (classMatrix, 1, U"Get highest y", nullptr, 1, REAL_Matrix_getHighestY);
		praat_addAction1 (classMatrix, 1, U"-- get structure --", nullptr, 1, nullptr);
		praat_addAction1 (classMatrix, 1, U"Get number of rows", nullptr, 1, INTEGER_Matrix_getNumberOfRows);
		praat_addAction1 (classMatrix, 1, U"Get number of columns", nullptr, 1, INTEGER_Matrix_getNumberOfColumns);
		praat_addAction1 (classMatrix, 1, U"Get row distance", nullptr, 1, REAL_Matrix_getRowDistance);
		praat_addAction1 (classMatrix, 1, U"Get column distance", nullptr, 1, REAL_Matrix_getColumnDistance);
		praat_addAction1 (classMatrix, 1, U"Get y of row...", nullptr, 1, REAL_Matrix_getYofRow);
		praat_addAction1 (classMatrix, 1, U"Get x of column...", nullptr, 1, REAL_Matrix_getXofColumn);
		praat_addAction1 (classMatrix, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classMatrix, 1, U"Get value in cell...", nullptr, 1, REAL_Matrix_getValueInCell);
		praat_addAction1 (classMatrix, 1, U"Get value at xy...", nullptr, 1, REAL_Matrix_getValueAtXY);
		praat_addAction1 (classMatrix, 1, U"Get minimum", nullptr, 1, REAL_Matrix_getMinimum);
		praat_addAction1 (classMatrix, 1, U"Get maximum", nullptr, 1, REAL_Matrix_getMaximum);
		praat_addAction1 (classMatrix, 1, U"Get sum", nullptr, 1, REAL_Matrix_getSum);
	praat_addAction1 (classMatrix, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"Formula...", nullptr, 1, MODIFY_Matrix_formula);
		praat_addAction1 (classMatrix, 0, U"Set value...", nullptr, 1, MODIFY_Matrix_setValue);
praat_addAction1 (classMatrix, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classMatrix, 0, U"Eigen", nullptr, 0, NEWTIMES2_Matrix_eigen);
	praat_addAction1 (classMatrix, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classMatrix, 0, U"Power...", nullptr, 0, NEW_Matrix_power);
	praat_addAction1 (classMatrix, 0, U"Combine two Matrices -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 2, U"Merge (append rows)", nullptr, 1, NEW1_Matrix_appendRows);
		praat_addAction1 (classMatrix, 2, U"To ParamCurve", nullptr, 1, NEW1_Matrix_to_ParamCurve);
	praat_addAction1 (classMatrix, 0, U"Cast -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"To Cochleagram", nullptr, 1, NEW_Matrix_to_Cochleagram);
		praat_addAction1 (classMatrix, 0, U"To Excitation", nullptr, 1, NEW_Matrix_to_Excitation);
		praat_addAction1 (classMatrix, 0, U"To Harmonicity", nullptr, 1, NEW_Matrix_to_Harmonicity);
		praat_addAction1 (classMatrix, 0, U"To Intensity", nullptr, 1, NEW_Matrix_to_Intensity);
		praat_addAction1 (classMatrix, 0, U"To Ltas", nullptr, 1, NEW_Matrix_to_Ltas);
		praat_addAction1 (classMatrix, 0, U"To Pitch", nullptr, 1, NEW_Matrix_to_Pitch);
		praat_addAction1 (classMatrix, 0, U"To PointProcess", nullptr, 1, NEW_Matrix_to_PointProcess);
		praat_addAction1 (classMatrix, 0, U"To Polygon", nullptr, 1, NEW_Matrix_to_Polygon);
		praat_addAction1 (classMatrix, 0, U"To Sound", nullptr, 1, NEW_Matrix_to_Sound);
		praat_addAction1 (classMatrix, 0, U"To Sound (slice)...", nullptr, 1, NEW_Matrix_to_Sound_mono);
		praat_addAction1 (classMatrix, 0, U"To Spectrogram", nullptr, 1, NEW_Matrix_to_Spectrogram);
		praat_addAction1 (classMatrix, 0, U"To TableOfReal", nullptr, 1, NEW_Matrix_to_TableOfReal);
		praat_addAction1 (classMatrix, 0, U"To Spectrum", nullptr, 1, NEW_Matrix_to_Spectrum);
		praat_addAction1 (classMatrix, 0, U"To Transition", nullptr, 1, NEW_Matrix_to_Transition);
		praat_addAction1 (classMatrix, 0, U"To VocalTract", nullptr, 1, NEW_Matrix_to_VocalTract);

	praat_addAction1 (classPhoto, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classPhoto, 0, U"Paint image...", nullptr, 1, GRAPHICS_Photo_paintImage);
		praat_addAction1 (classPhoto, 0, U"Paint cells...", nullptr, 1, GRAPHICS_Photo_paintCells);
	praat_addAction1 (classPhoto, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classPhoto, 0, U"Formula (red)...", nullptr, 1, MODIFY_Photo_formula_red);
		praat_addAction1 (classPhoto, 0, U"Formula (green)...", nullptr, 1, MODIFY_Photo_formula_green);
		praat_addAction1 (classPhoto, 0, U"Formula (blue)...", nullptr, 1, MODIFY_Photo_formula_blue);
		praat_addAction1 (classPhoto, 0, U"Formula (transparency)...", nullptr, 1, MODIFY_Photo_formula_transparency);
	praat_addAction1 (classPhoto, 0, U"Extract -", nullptr, 0, nullptr);
		praat_addAction1 (classPhoto, 0, U"Extract red", nullptr, 1, NEW_Photo_extractRed);
		praat_addAction1 (classPhoto, 0, U"Extract green", nullptr, 1, NEW_Photo_extractGreen);
		praat_addAction1 (classPhoto, 0, U"Extract blue", nullptr, 1, NEW_Photo_extractBlue);
		praat_addAction1 (classPhoto, 0, U"Extract transparency", nullptr, 1, NEW_Photo_extractTransparency);
	praat_addAction1 (classPhoto, 1, U"Save as PNG file...", nullptr, 0, SAVE_Photo_saveAsPNG);
	#if defined (macintosh) || defined (_WIN32)
		praat_addAction1 (classPhoto, 1, U"Save as TIFF file...", nullptr, 0, SAVE_Photo_saveAsTIFF);
		praat_addAction1 (classPhoto, 1, U"Save as GIF file...", nullptr, 0, SAVE_Photo_saveAsGIF);
		praat_addAction1 (classPhoto, 1, U"Save as Windows bitmap file...", nullptr, 0, SAVE_Photo_saveAsWindowsBitmapFile);
		praat_addAction1 (classPhoto, 1, U"Save as lossy JPEG file...", nullptr, 0, SAVE_Photo_saveAsJPEG);
	#endif
	#if defined (macintosh)
		praat_addAction1 (classPhoto, 1, U"Save as JPEG-2000 file...", nullptr, 0, SAVE_Photo_saveAsJPEG2000);
		praat_addAction1 (classPhoto, 1, U"Save as Apple icon file...", nullptr, 0, SAVE_Photo_saveAsAppleIconFile);
		praat_addAction1 (classPhoto, 1, U"Save as Windows icon file...", nullptr, 0, SAVE_Photo_saveAsWindowsIconFile);
	#endif

	praat_addAction1 (classMovie, 1, U"Paint one image...", nullptr, 1, GRAPHICS_Movie_paintOneImage);
	praat_addAction1 (classMovie, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_Movie_viewAndEdit);

	praat_addAction2 (classMatrix, 1, classSound, 1, U"To ParamCurve", nullptr, 0, NEW1_Matrix_to_ParamCurve);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace red", nullptr, 0, MODIFY_Photo_Matrix_replaceRed);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace green", nullptr, 0, MODIFY_Photo_Matrix_replaceGreen);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace blue", nullptr, 0, MODIFY_Photo_Matrix_replaceBlue);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace transparency", nullptr, 0, MODIFY_Photo_Matrix_replaceTransparency);
}

/* End of file praat_Matrix.cpp */
