/* praat_Matrix.cpp
 *
 * Copyright (C) 1992-2005,2007,2011-2024 Paul Boersma
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
#include "RealTier.h"
#include "Spectrogram.h"
#include "Spectrum.h"
#include "Transition.h"
#include "VocalTract.h"

#include "praat_Matrix.h"

// MARK: - MATRIX

// MARK: New

FORM (NEW1_Matrix_create, U"Create Matrix", U"Create Matrix...") {
	WORD (name, U"Name", U"xy")
	REAL (xmin, U"xmin", U"1.0")
	REAL (xmax, U"xmax", U"1.0")
	NATURAL (numberOfColumns, U"Number of columns", U"1")
	POSITIVE (dx, U"dx", U"1.0")
	REAL (x1, U"x1", U"1.0")
	REAL (ymin, U"ymin", U"1.0")
	REAL (ymax, U"ymax", U"1.0")
	NATURAL (numberOfRows, U"Number of rows", U"1")
	POSITIVE (dy, U"dy", U"1.0")
	REAL (y1, U"y1", U"1.0")
	FORMULA (formula, U"Formula", U"x*y")
	OK
DO
	if (xmax < xmin) Melder_throw (U"Your xmax (", Melder_single (xmax), U") should not be less than your xmin (", Melder_single (xmin), U").");
	if (ymax < ymin) Melder_throw (U"Your ymax (", Melder_single (ymax), U") should not be less than your ymin (", Melder_single (ymin), U").");
	CREATE_ONE
		autoMatrix result = Matrix_create (
			xmin, xmax, numberOfColumns, dx, x1,
			ymin, ymax, numberOfRows, dy, y1);
		Matrix_formula (result.get(), formula, interpreter, nullptr);
	CREATE_ONE_END (name)
}

FORM (NEW1_Matrix_createSimple, U"Create simple Matrix", U"Create simple Matrix...") {
	WORD (name, U"Name", U"xy")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"10")
	FORMULA (formula, U"Formula", U"x*y")
	OK
DO
	CREATE_ONE
		autoMatrix result = Matrix_createSimple (numberOfRows, numberOfColumns);
		Matrix_formula (result.get(), formula, interpreter, nullptr);
	CREATE_ONE_END (name);
}

FORM (NEW1_Matrix_createSimpleFromValues, U"Create simple Matrix from values", U"Create simple Matrix from values...") {
	WORD (name, U"Name", U"xy")
	REALMATRIX (values, U"Values", autoMAT ({ { 10, 20, 30, 40 }, { 60, 70, 80, 90 }, { 170, 180, 190, -300 } }))
	OK
DO
	CREATE_ONE
		autoMatrix result = Matrix_createSimple (values.nrow, values.ncol);
		result -> z.all()  <<=  values;
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
	SAVE_ONE (Matrix)
		Matrix_writeToMatrixTextFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Matrix_writeToHeaderlessSpreadsheetFile, U"Save Matrix as spreadsheet", nullptr, U"txt") {
	SAVE_ONE (Matrix)
		Matrix_writeToHeaderlessSpreadsheetFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP_Matrix_help) {
	HELP (U"Matrix")
}

// MARK: Movie

static autoGraphics theMovieGraphics;

static void gui_drawingarea_cb_expose (Thing /* boss */, GuiDrawingArea_ExposeEvent /* event */) {
	if (! theMovieGraphics)
		return;
	Graphics_play (theMovieGraphics.get(), theMovieGraphics.get());
}

extern "C" Graphics Movie_create (conststring32 title, int width, int height) {
	static GuiDialog dialog;
	static GuiDrawingArea drawingArea;
	if (! theMovieGraphics) {
		dialog = GuiDialog_create (theCurrentPraatApplication -> topShell, 100, 100, width + 2, height + 2,
				title, nullptr, nullptr, GuiDialog_Modality::MODELESS);
		drawingArea = GuiDrawingArea_createShown (dialog, 0, width, 0, height,
			 	gui_drawingarea_cb_expose, nullptr, nullptr, nullptr, nullptr, nullptr, 0);
		GuiThing_show (dialog);
		theMovieGraphics = Graphics_create_xmdrawingarea (drawingArea);
	}
	GuiShell_setTitle (dialog, title);
	GuiControl_setSize (dialog, width + 2, height + 2);
	GuiControl_setSize (drawingArea, width, height);
	GuiThing_show (dialog);
	return theMovieGraphics.get();
}

DIRECT (MOVIE_Matrix_playMovie) {
	MOVIE_ONE (Matrix, U"Matrix movie", 300, 300)
		Matrix_playMovie (me, graphics);
	MOVIE_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_Matrix_drawRows, U"Draw rows", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawRows (me, GRAPHICS, fromX, toX, fromY, toY, minimum, maximum);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_drawOneContour, U"Draw one altitude contour", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (height, U"Height", U"0.5")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawOneContour (me, GRAPHICS,fromX, toX, fromY, toY, height);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_drawContours, U"Draw altitude contours", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_drawContours (me, GRAPHICS, fromX, toX, fromY, toY, minimum, maximum);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_paintImage, U"Matrix: Paint grey image", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintImage (me, GRAPHICS, fromX, toX, fromY, toY, minimum, maximum);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_paintContours, U"Matrix: Paint altitude contours with greys", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintContours (me, GRAPHICS, fromX, toX, fromY, toY, minimum, maximum);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_paintCells, U"Matrix: Paint cells with greys", U"Matrix: Paint cells...") {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintCells (me, GRAPHICS, fromX, toX, fromY, toY, minimum, maximum);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Matrix_paintSurface, U"Matrix: Paint 3-D surface plot", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0")
	OK
DO
	GRAPHICS_EACH (Matrix)
		Matrix_paintSurface (me, GRAPHICS, fromX, toX, fromY, toY, minimum, maximum, 30, 45);
	GRAPHICS_EACH_END
}

// MARK: Query

DIRECT (REAL_Matrix_getLowestX) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = my xmin;
	QUERY_ONE_FOR_REAL_END (U" (xmin)")
}

DIRECT (REAL_Matrix_getHighestX) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = my xmax;
	QUERY_ONE_FOR_REAL_END (U" (xmax)")
}

DIRECT (REAL_Matrix_getLowestY) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = my ymin;
	QUERY_ONE_FOR_REAL_END (U" (ymin)")
}

DIRECT (REAL_Matrix_getHighestY) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = my ymax;
	QUERY_ONE_FOR_REAL_END (U" (xmax)")
}

DIRECT (INTEGER_Matrix_getNumberOfRows) {
	QUERY_ONE_FOR_REAL (Matrix)
		integer result = my ny;
	QUERY_ONE_FOR_REAL_END (U" rows")
}

DIRECT (INTEGER_Matrix_getNumberOfColumns) {
	QUERY_ONE_FOR_REAL (Matrix)
		integer result = my nx;
	QUERY_ONE_FOR_REAL_END (U" columns")
}

DIRECT (REAL_Matrix_getRowDistance) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = my dy;
	QUERY_ONE_FOR_REAL_END (U" (row distance)")
}

DIRECT (REAL_Matrix_getColumnDistance) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" (column distance)")
}

FORM (REAL_Matrix_getYofRow, U"Matrix: Get y of row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		double result = Matrix_rowToY (me, rowNumber);
	QUERY_ONE_FOR_REAL_END (U" (y of row ", rowNumber, U")")
}

FORM (REAL_Matrix_getXofColumn, U"Matrix: Get x of column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		double result = Matrix_columnToX (me, columnNumber);
	QUERY_ONE_FOR_REAL_END (U" (x of column ", columnNumber, U")")
}

FORM (REAL_Matrix_getValueInCell, U"Matrix: Get value in cell", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		if (rowNumber > my ny) Melder_throw (U"Row number should not exceed number of rows.");
		if (columnNumber > my nx) Melder_throw (U"Column number should not exceed number of columns.");
		double result = my z [rowNumber] [columnNumber];
	QUERY_ONE_FOR_REAL_END (U" (value in column ", columnNumber, U" of row ", rowNumber, U")")
}

FORM (REAL_Matrix_getValueAtXY, U"Matrix: Get value at xy", nullptr) {
	REAL (x, U"X", U"0.0")
	REAL (y, U"Y", U"0.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Matrix)
		double result = Matrix_getValueAtXY (me, x, y);
	QUERY_ONE_FOR_REAL_END (U" (at x = ", x, U" and y = ", y, U")");
}

DIRECT (REAL_Matrix_getMinimum) {
	QUERY_ONE_FOR_REAL (Matrix)
		double minimum = undefined, maximum = undefined;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		double result = minimum;
	QUERY_ONE_FOR_REAL_END (U" (minimum)");
}

DIRECT (REAL_Matrix_getMaximum) {
	QUERY_ONE_FOR_REAL (Matrix)
		double minimum = undefined, maximum = undefined;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		double result = maximum;
	QUERY_ONE_FOR_REAL_END (U" (maximum)");
}

DIRECT (REAL_Matrix_getSum) {
	QUERY_ONE_FOR_REAL (Matrix)
		double result = Matrix_getSum (me);
	QUERY_ONE_FOR_REAL_END (U" (sum)");
}

DIRECT (NUMMAT_Matrix_getAllValues) {
	QUERY_ONE_FOR_MATRIX (Matrix)
		autoMAT result = copy_MAT (my z.all());
	QUERY_ONE_FOR_MATRIX_END
}

FORM (NUMVEC_Matrix_getAllValuesInColumn, U"Get all values in column", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Matrix)
		Melder_require (columnNumber <= my nx,
			U"The column number (", columnNumber, U") should not be greater than the number of columns (", my nx, U").");
		autoVEC result = copy_VEC (my z.column (columnNumber));
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (NUMVEC_Matrix_getAllValuesInRow, U"Get all values in row", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Matrix)
		Melder_require (rowNumber <= my ny,
			U"The row number (", rowNumber, U") should not be greater than the number of rows (", my ny, U").");
		autoVEC result = copy_VEC (my z.row (rowNumber));
	QUERY_ONE_FOR_REAL_VECTOR_END
}

// MARK: Modify

FORM (MODIFY_Matrix_formula, U"Matrix Formula", U"Formula...") {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula` ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Matrix)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Matrix_setValue, U"Matrix: Set value", U"Matrix: Set value...") {
	NATURAL (rowNumber, U"Row number", U"1")
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (newValue, U"New value", U"0.0")
	OK
DO
	MODIFY_EACH (Matrix)
		if (rowNumber > my ny)
			Melder_throw (U"Your row number should not be greater than your number of rows.");
		if (columnNumber > my nx)
			Melder_throw (U"Your column number should not be greater than your number of columns.");
		my z [rowNumber] [columnNumber] = newValue;
	MODIFY_EACH_END
}

// MARK: Analyse

DIRECT (CONVERT_EACH_TO_MULTIPLE_Matrix_eigen) {
	CONVERT_EACH_TO_MULTIPLE (Matrix)
		autoMatrix vectors, values;
		Matrix_eigen (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	CONVERT_EACH_TO_MULTIPLE_END
}

// MARK: Synthesize

FORM (NEW_Matrix_power, U"Matrix: Power...", nullptr) {
	NATURAL (power, U"Power", U"2")
	OK
DO
	CONVERT_EACH_TO_ONE (Matrix)
		autoMatrix result = Matrix_power (me, power);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Combine

DIRECT (NEW1_Matrix_appendRows) {
	CONVERT_TWO_TO_ONE (Matrix)
		autoMatrix result = Matrix_appendRows (me, you, classMatrix);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

// MARK: Cast

DIRECT (NEW_Matrix_to_Cochleagram) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoCochleagram result = Matrix_to_Cochleagram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Excitation) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoExcitation result = Matrix_to_Excitation (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Harmonicity) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoHarmonicity result = Matrix_to_Harmonicity (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Intensity) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoIntensity result = Matrix_to_Intensity (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Ltas) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoLtas result = Matrix_to_Ltas (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Pitch) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoPitch result = Matrix_to_Pitch (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_PointProcess) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoPointProcess result = Matrix_to_PointProcess (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Polygon) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoPolygon result = Matrix_to_Polygon (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Sound) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoSound result = Matrix_to_Sound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Matrix_to_Sound_mono, U"Matrix: To Sound (mono)", 0) {
	INTEGER (rowNumber, U"Row number", U"1")
	COMMENT (U"(negative values count from last row)")
	OK
DO
	CONVERT_EACH_TO_ONE (Matrix)
		autoSound result = Matrix_to_Sound_mono (me, rowNumber);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Spectrogram) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoSpectrogram result = Matrix_to_Spectrogram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Spectrum) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoSpectrum result = Matrix_to_Spectrum (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_TableOfReal) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoTableOfReal result = Matrix_to_TableOfReal (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_Transition) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoTransition result = Matrix_to_Transition (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Matrix_to_VocalTract) {
	CONVERT_EACH_TO_ONE (Matrix)
		autoVocalTract result = Matrix_to_VocalTract (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW1_Matrix_to_ParamCurve) {
	CONVERT_TWO_TO_ONE (Matrix)
		autoSound sound1 = Matrix_to_Sound (me), sound2 = Matrix_to_Sound (you);
		autoParamCurve result = ParamCurve_create (sound1.get(), sound2.get());
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get());
}

FORM (CONVERT_EACH_TO_ONE__Matrix_to_RealTier, U"Matrix: To RealTier", nullptr) {
	NATURAL (columnWithTimes, U"Column with times", U"1")
	NATURAL (columnWithValues, U"Column with values", U"2")
	REAL_OR_UNDEFINED (startTime, U"Start time (s)", U"0.0")
	REAL_OR_UNDEFINED (endTime, U"End time (s)", U"undefined")
	OK
DO
	CONVERT_EACH_TO_ONE (Matrix)
		autoRealTier result = Matrix_to_RealTier (me, columnWithTimes, columnWithValues, startTime, endTime);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - PHOTO

// MARK: New

FORM (NEW1_Photo_create, U"Create Photo", U"Create Photo...") {
	WORD (name, U"Name", U"xy")
	REAL (xmin, U"left xmin, xmax", U"1.0")
	REAL (xmax, U"right xmin, xmax", U"1.0")
	NATURAL (numberOfColumns, U"Number of columns", U"1")
	POSITIVE (dx, U"dx", U"1.0")
	REAL (x1, U"x1", U"1.0")
	REAL (ymin, U"left ymin, ymax", U"1.0")
	REAL (ymax, U"right ymin, ymax", U"1.0")
	NATURAL (numberOfRows, U"Number of rows", U"1")
	POSITIVE (dy, U"dy", U"1.0")
	REAL (y1, U"y1", U"1.0")
	FORMULA (redFormula, U"Red formula", U"x*y/100")
	FORMULA (greenFormula, U"Green formula", U"x*y/1000")
	FORMULA (blueFormula, U"Blue formula", U"x*y/100")
	OK
DO
	if (xmax < xmin)
		Melder_throw (U"Your xmax (", Melder_single (xmax), U") should not be less than your xmin (", Melder_single (xmin), U").");
	if (ymax < ymin)
		Melder_throw (U"Your ymax (", Melder_single (ymax), U") should not be less than your ymin (", Melder_single (ymin), U").");
	CREATE_ONE
		autoPhoto result = Photo_create (
			xmin, xmax, numberOfColumns, dx, x1,
			ymin, ymax, numberOfRows, dy, y1);
		Matrix_formula (result -> d_red  .get(), redFormula,   interpreter, nullptr);
		Matrix_formula (result -> d_green.get(), greenFormula, interpreter, nullptr);
		Matrix_formula (result -> d_blue .get(), blueFormula,  interpreter, nullptr);
	CREATE_ONE_END (name)
}

FORM (NEW1_Photo_createSimple, U"Create simple Photo", U"Create simple Photo...") {
	WORD (name, U"Name", U"xy")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"10")
	FORMULA (redFormula, U"Red formula", U"x*y/100")
	FORMULA (greenFormula, U"Green formula", U"x*y/1000")
	FORMULA (blueFormula, U"Blue formula", U"x*y/100")
	OK
DO
	CREATE_ONE
		autoPhoto result = Photo_createSimple (numberOfRows, numberOfColumns);
		Matrix_formula (result -> d_red.get(),   redFormula,   interpreter, nullptr);
		Matrix_formula (result -> d_green.get(), greenFormula, interpreter, nullptr);
		Matrix_formula (result -> d_blue.get(),  blueFormula,  interpreter, nullptr);
	CREATE_ONE_END (name)
}

DIRECT (NEW_Photo_extractBlue) {
	CONVERT_EACH_TO_ONE (Photo)
		autoMatrix result = Data_copy (my d_blue.get());
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_blue");
}

DIRECT (NEW_Photo_extractGreen) {
	CONVERT_EACH_TO_ONE (Photo)
		autoMatrix result = Data_copy (my d_green.get());
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_green");
}

DIRECT (NEW_Photo_extractRed) {
	CONVERT_EACH_TO_ONE (Photo)
		autoMatrix result = Data_copy (my d_red.get());
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_red");
}

DIRECT (NEW_Photo_extractTransparency) {
	CONVERT_EACH_TO_ONE (Photo)
		autoMatrix result = Data_copy (my d_transparency.get());
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_transparency");
}

FORM (MODIFY_Photo_formula_red, U"Photo Formula (red)", U"Formula (red)...") {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula` ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Photo)
		Matrix_formula (my d_red.get(), formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Photo_formula_green, U"Photo Formula (green)", U"Formula (green)...") {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula` ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Photo)
		Matrix_formula (my d_green.get(), formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Photo_formula_blue, U"Photo Formula (blue)", U"Formula (blue)...") {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula` ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Photo)
		Matrix_formula (my d_blue.get(), formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Photo_formula_transparency, U"Photo Formula (transparency)", U"Formula (transparency)...") {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula` ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Photo)
		Matrix_formula (my d_transparency.get(), formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (GRAPHICS_Photo_paintCells, U"Photo: Paint cells with colour", U"Photo: Paint cells...") {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	OK
DO
	GRAPHICS_EACH (Photo)
		Photo_paintCells (me, GRAPHICS, fromX, toX, fromY, toY);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Photo_paintImage, U"Photo: Paint colour image", nullptr) {
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"0.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"0.0")
	OK
DO
	GRAPHICS_EACH (Photo)
		Photo_paintImage (me, GRAPHICS, fromX, toX, fromY, toY);
	GRAPHICS_EACH_END
}

FORM_SAVE (SAVE_Photo_saveAsAppleIconFile, U"Save as Apple icon file", nullptr, U"icns") {
	SAVE_ONE (Photo)
		Photo_saveAsAppleIconFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsGIF, U"Save as GIF file", nullptr, U"gif") {
	SAVE_ONE (Photo)
		Photo_saveAsGIF (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsJPEG, U"Save as JPEG file", nullptr, U"jpg") {
	SAVE_ONE (Photo)
		Photo_saveAsJPEG (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsJPEG2000, U"Save as JPEG-2000 file", nullptr, U"jpg") {
	SAVE_ONE (Photo)
		Photo_saveAsJPEG2000 (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsPNG, U"Save as PNG file", nullptr, U"png") {
	SAVE_ONE (Photo)
		Photo_saveAsPNG (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsTIFF, U"Save as TIFF file", nullptr, U"tiff") {
	SAVE_ONE (Photo)
		Photo_saveAsTIFF (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsWindowsBitmapFile, U"Save as Windows bitmap file", nullptr, U"bmp") {
	SAVE_ONE (Photo)
		Photo_saveAsWindowsBitmapFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Photo_saveAsWindowsIconFile, U"Save as Windows icon file", nullptr, U"ico") {
	SAVE_ONE (Photo)
		Photo_saveAsWindowsIconFile (me, file);
	SAVE_ONE_END
}

// MARK: - PHOTO & MATRIX

DIRECT (MODIFY_Photo_Matrix_replaceBlue) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Photo, Matrix)
		Photo_replaceBlue (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (MODIFY_Photo_Matrix_replaceGreen) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Photo, Matrix)
		Photo_replaceGreen (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (MODIFY_Photo_Matrix_replaceRed) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Photo, Matrix)
		Photo_replaceRed (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (MODIFY_Photo_Matrix_replaceTransparency) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Photo, Matrix)
		Photo_replaceTransparency (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

// MARK: - MOVIE

FORM_READ (READ1_Movie_openFromSoundFile, U"Open movie file", nullptr, true) {
	READ_ONE
		autoMovie result = Movie_openFromSoundFile (file);
	READ_ONE_END   // but loses data when saving, if object is associated only with the sound file
}

FORM (GRAPHICS_Movie_paintOneImage, U"Movie: Paint one image", nullptr) {
	NATURAL (frameNumber, U"Frame number", U"1")
	REAL (fromX, U"From x =", U"0.0")
	REAL (toX, U"To x =", U"1.0")
	REAL (fromY, U"From y =", U"0.0")
	REAL (toY, U"To y =", U"1.0")
	OK
DO
	GRAPHICS_EACH (Movie)
		Movie_paintOneImage (me, GRAPHICS, frameNumber, fromX, toX, fromY, toY);
	GRAPHICS_EACH_END
}

DIRECT (EDITOR_ONE_Movie_viewAndEdit) {
	EDITOR_ONE (a,Movie)
		autoMovieWindow editor = MovieWindow_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: file recognizers

static autoDaata imageFileRecognizer (integer /* nread */, const char * /* header */, MelderFile file) {
	conststring32 fileName = MelderFile_name (file);
	if (Melder_endsWith_caseAware (fileName, U".jpg") ||
	    Melder_endsWith_caseAware (fileName, U".jpeg") ||
	    Melder_endsWith_caseAware (fileName, U".png") ||
	    Melder_endsWith_caseAware (fileName, U".tiff") ||
		Melder_endsWith_caseAware (fileName, U".tif"))
	{
		return Photo_readFromImageFile (file);
	}
	return autoDaata ();
}

// MARK: - buttons

void praat_Matrix_init () {
	Thing_recognizeClassesByName (classMatrix, classPhoto, classMovie, nullptr);

	Data_recognizeFileType (imageFileRecognizer);

	praat_addMenuCommand (U"Objects", U"New", U"Matrix", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Matrix...", nullptr, 2, NEW1_Matrix_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Matrix...", nullptr, 2, NEW1_Matrix_createSimple);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Matrix from values...", nullptr, 2, NEW1_Matrix_createSimpleFromValues);
	praat_addMenuCommand (U"Objects", U"New", U"Photo", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Photo...", nullptr, 2, NEW1_Photo_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create simple Photo...", nullptr, 2, NEW1_Photo_createSimple);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read movie --", nullptr, GuiMenu_HIDDEN, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Open movie file...", nullptr, GuiMenu_HIDDEN, READ1_Movie_openFromSoundFile);
	praat_addMenuCommand (U"Objects", U"Open", U"-- read raw --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Matrix from raw text file...", nullptr, 0, READ1_Matrix_readFromRawTextFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Matrix from LVS AP file...", nullptr, GuiMenu_HIDDEN, READ1_Matrix_readAP);

	praat_addAction1 (classMatrix, 0, U"Matrix help", nullptr, 0, HELP_Matrix_help);
	praat_addAction1 (classMatrix, 1, U"Save as matrix text file... || Write to matrix text file...", nullptr, 0,
			SAVE_Matrix_writeToMatrixTextFile);   // alternative COMPATIBILITY <= 2011
	praat_addAction1 (classMatrix, 1, U"Save as headerless spreadsheet file... || Write to headerless spreadsheet file...", nullptr, 0,
			SAVE_Matrix_writeToHeaderlessSpreadsheetFile);   // alternative COMPATIBILITY <= 2011
	praat_addAction1 (classMatrix, 1, U"Play movie", nullptr, 0, MOVIE_Matrix_playMovie);
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
		praat_addAction1 (classMatrix, 1, U"Get all values", nullptr, 1, NUMMAT_Matrix_getAllValues);
		praat_addAction1 (classMatrix, 1, U"Get all values in row...", nullptr, 1, NUMVEC_Matrix_getAllValuesInRow);
		praat_addAction1 (classMatrix, 1, U"Get all values in column...", nullptr, 1, NUMVEC_Matrix_getAllValuesInColumn);
		praat_addAction1 (classMatrix, 1, U"Get minimum", nullptr, 1, REAL_Matrix_getMinimum);
		praat_addAction1 (classMatrix, 1, U"Get maximum", nullptr, 1, REAL_Matrix_getMaximum);
		praat_addAction1 (classMatrix, 1, U"Get sum", nullptr, 1, REAL_Matrix_getSum);
	praat_addAction1 (classMatrix, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classMatrix, 0, U"Formula...", nullptr, 1, MODIFY_Matrix_formula);
		praat_addAction1 (classMatrix, 0, U"Set value...", nullptr, 1, MODIFY_Matrix_setValue);
praat_addAction1 (classMatrix, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classMatrix, 0, U"Eigen", nullptr, 0, CONVERT_EACH_TO_MULTIPLE_Matrix_eigen);
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
		praat_addAction1 (classMatrix, 0, U"To RealTier...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Matrix_to_RealTier);
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
	praat_addAction1 (classMovie, 1, U"View & Edit", nullptr, GuiMenu_ATTRACTIVE, EDITOR_ONE_Movie_viewAndEdit);

	praat_addAction2 (classMatrix, 1, classSound, 1, U"To ParamCurve", nullptr, 0, NEW1_Matrix_to_ParamCurve);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace red", nullptr, 0, MODIFY_Photo_Matrix_replaceRed);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace green", nullptr, 0, MODIFY_Photo_Matrix_replaceGreen);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace blue", nullptr, 0, MODIFY_Photo_Matrix_replaceBlue);
	praat_addAction2 (classPhoto, 1, classMatrix, 1, U"Replace transparency", nullptr, 0, MODIFY_Photo_Matrix_replaceTransparency);
}

/* End of file praat_Matrix.cpp */
