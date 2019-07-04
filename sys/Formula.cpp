/* Formula.cpp
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

#include <ctype.h>
#if defined (UNIX)
	#include <sys/stat.h>
#endif
#include "../dwsys/NUM2.h"
#include "Formula.h"
#include "Interpreter.h"
#include "Ui.h"
#include "praatP.h"
#include "praat_script.h"
#include "../kar/UnicodeData.h"
#include "../kar/longchar.h"
#include "UiPause.h"
#include "DemoEditor.h"

static Interpreter theInterpreter;
static autoInterpreter theLocalInterpreter;
static Daata theSource;
static conststring32 theExpression;
static int theLevel = 1;
#define MAXIMUM_NUMBER_OF_LEVELS  20
static int theExpressionType [1 + MAXIMUM_NUMBER_OF_LEVELS];
static bool theOptimize;

typedef struct structFormulaInstruction {
	int symbol;
	int position;
	union {
		double number;
		int label;
		char32 *string;
		Daata object;
		InterpreterVariable variable;
	} content;
} *FormulaInstruction;

static FormulaInstruction lexan, parse;
static int ilabel, ilexan, iparse, numberOfInstructions, numberOfStringConstants;

enum { NO_SYMBOL_,

/* First, all symbols after which "-" is unary. */
/* The list ends with "MINUS_" itself. */

	/* Opening symbols. */
	IF_, THEN_, ELSE_, OPENING_PARENTHESIS_, OPENING_BRACKET_, OPENING_BRACE_, COMMA_, COLON_, FROM_, TO_,
	/* Operators with boolean results. */
	OR_, AND_, NOT_, EQ_, NE_, LE_, LT_, GE_, GT_,
	/* Operators with results. */
	ADD_, SUB_, MUL_, RDIV_, IDIV_, MOD_, POWER_, CALL_, MINUS_,

/* Then, the symbols after which "-" is binary. */

	/* Closing symbols. */
	ENDIF_, FI_, CLOSING_PARENTHESIS_, CLOSING_BRACKET_, CLOSING_BRACE_,
	/* Things with a value. */
	#define LOW_VALUE  NUMBER_
		NUMBER_, NUMBER_PI_, NUMBER_E_, NUMBER_UNDEFINED_,
		/* Attributes of objects. */
		#define LOW_ATTRIBUTE  XMIN_
			XMIN_, XMAX_, YMIN_, YMAX_, NX_, NY_, DX_, DY_,
			ROW_, COL_, NROW_, NCOL_, ROWSTR_, COLSTR_, Y_, X_,
		#define HIGH_ATTRIBUTE  X_
	#define HIGH_VALUE  HIGH_ATTRIBUTE

	SELF_, SELFSTR_, OBJECT_, OBJECTSTR_, MATRIX_, MATRIXSTR_,
	STOPWATCH_,

/* The following symbols can be followed by "-" only if they are a variable. */

	/* Functions of 1 variable; if you add, update the #defines. */
	#define LOW_FUNCTION_1  ABS_
		ABS_, ROUND_, FLOOR_, CEILING_,
		RECTIFY_, RECTIFY_H_, RECTIFY_HH_,
		SQRT_, SIN_, COS_, TAN_, ARCSIN_, ARCCOS_, ARCTAN_, SINC_, SINCPI_,
		EXP_, VEC_EXP_, MAT_EXP_,
		SINH_, COSH_, TANH_, ARCSINH_, ARCCOSH_, ARCTANH_,
		SIGMOID_, VEC_SIGMOID_, SOFTMAX_H_, SOFTMAX_PER_ROW_HH_,
		INV_SIGMOID_, ERF_, ERFC_, GAUSS_P_, GAUSS_Q_, INV_GAUSS_Q_,
		RANDOM_BERNOULLI_, VEC_RANDOM_BERNOULLI_,
		RANDOM_POISSON_, MAT_TRANSPOSE_,
		SUM_PER_ROW_H_, SUM_PER_COLUMN_H_,
		LOG2_, LN_, LOG10_, LN_GAMMA_,
		HERTZ_TO_BARK_, BARK_TO_HERTZ_, PHON_TO_DIFFERENCE_LIMENS_, DIFFERENCE_LIMENS_TO_PHON_,
		HERTZ_TO_MEL_, MEL_TO_HERTZ_, HERTZ_TO_SEMITONES_, SEMITONES_TO_HERTZ_,
		ERB_, HERTZ_TO_ERB_, ERB_TO_HERTZ_,
		SUM_, MEAN_, STDEV_, CENTER_,
		EVALUATE_, EVALUATE_NOCHECK_, EVALUATE_STR_, EVALUATE_NOCHECK_STR_,
		STRINGSTR_, SLEEP_, UNICODE_, UNICODESTR_,
	#define HIGH_FUNCTION_1  UNICODESTR_

	/* Functions of 2 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_2  ARCTAN2_
		ARCTAN2_, RANDOM_UNIFORM_, RANDOM_INTEGER_, RANDOM_GAUSS_, RANDOM_BINOMIAL_,
		CHI_SQUARE_P_, CHI_SQUARE_Q_, INCOMPLETE_GAMMAP_,
		INV_CHI_SQUARE_Q_, STUDENT_P_, STUDENT_Q_, INV_STUDENT_Q_,
		BETA_, BETA2_, BESSEL_I_, BESSEL_K_, LN_BETA_,
		SOUND_PRESSURE_TO_PHON_, OBJECTS_ARE_IDENTICAL_,
		INNER_, MAT_OUTER_, VEC_MUL_, MAT_MUL_, MAT_MUL_FAST_, MAT_MUL_METAL_,
		MAT_MUL_TN_, MAT_MUL_NT_, MAT_MUL_TT_, VEC_REPEAT_,
		VEC_ROW_INNERS_,
	#define HIGH_FUNCTION_2  VEC_ROW_INNERS_

	/* Functions of 3 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_3  FISHER_P_
		FISHER_P_, FISHER_Q_, INV_FISHER_Q_,
		BINOMIAL_P_, BINOMIAL_Q_, INCOMPLETE_BETA_, INV_BINOMIAL_P_, INV_BINOMIAL_Q_,
	#define HIGH_FUNCTION_3  INV_BINOMIAL_Q_

	/* Functions of a variable number of variables; if you add, update the #defines. */
	#define LOW_FUNCTION_N  DO_
		DO_, DOSTR_,
		WRITE_INFO_, WRITE_INFO_LINE_, APPEND_INFO_, APPEND_INFO_LINE_,
		WRITE_FILE_, WRITE_FILE_LINE_, APPEND_FILE_, APPEND_FILE_LINE_,
		PAUSE_SCRIPT_, EXIT_SCRIPT_, RUN_SCRIPT_, RUN_SYSTEM_, RUN_SYSTEM_NOCHECK_, RUN_SUBPROCESS_,
		MIN_, MAX_, IMIN_, IMAX_, NORM_,
		LEFTSTR_, RIGHTSTR_, MIDSTR_,
		SELECTED_, SELECTEDSTR_, NUMBER_OF_SELECTED_, VEC_SELECTED_,
		SELECT_OBJECT_, PLUS_OBJECT_, MINUS_OBJECT_, REMOVE_OBJECT_,
		BEGIN_PAUSE_FORM_, PAUSE_FORM_ADD_REAL_, PAUSE_FORM_ADD_POSITIVE_, PAUSE_FORM_ADD_INTEGER_, PAUSE_FORM_ADD_NATURAL_,
		PAUSE_FORM_ADD_WORD_, PAUSE_FORM_ADD_SENTENCE_, PAUSE_FORM_ADD_TEXT_, PAUSE_FORM_ADD_BOOLEAN_,
		PAUSE_FORM_ADD_CHOICE_, PAUSE_FORM_ADD_OPTION_MENU_, PAUSE_FORM_ADD_OPTION_,
		PAUSE_FORM_ADD_COMMENT_, END_PAUSE_FORM_,
		CHOOSE_READ_FILESTR_, CHOOSE_WRITE_FILESTR_, CHOOSE_DIRECTORYSTR_,
		DEMO_WINDOW_TITLE_, DEMO_SHOW_, DEMO_WAIT_FOR_INPUT_, DEMO_PEEK_INPUT_, DEMO_INPUT_, DEMO_CLICKED_IN_,
		DEMO_CLICKED_, DEMO_X_, DEMO_Y_, DEMO_KEY_PRESSED_, DEMO_KEY_,
		DEMO_SHIFT_KEY_PRESSED_, DEMO_COMMAND_KEY_PRESSED_, DEMO_OPTION_KEY_PRESSED_, DEMO_EXTRA_CONTROL_KEY_PRESSED_,
		VEC_ZERO_, MAT_ZERO_,
		VEC_LINEAR_, MAT_LINEAR_, VEC_TO_, VEC_FROM_TO_, VEC_FROM_TO_BY_,
		VEC_RANDOM_UNIFORM_, MAT_RANDOM_UNIFORM_,
		VEC_RANDOM_INTEGER_, MAT_RANDOM_INTEGER_,
		VEC_RANDOM_GAUSS_, MAT_RANDOM_GAUSS_,
		MAT_PEAKS_,
		SIZE_, NUMBER_OF_ROWS_, NUMBER_OF_COLUMNS_, EDITOR_, HASH_,
	#define HIGH_FUNCTION_N  HASH_

	/* String functions. */
	#define LOW_STRING_FUNCTION  LOW_FUNCTION_STR1
	#define LOW_FUNCTION_STR1  LENGTH_
		LENGTH_, STRING_TO_NUMBER_, FILE_READABLE_, DELETE_FILE_, CREATE_DIRECTORY_, VARIABLE_EXISTS_,
		READ_FILE_, READ_FILESTR_, UNICODE_TO_BACKSLASH_TRIGRAPHS_, BACKSLASH_TRIGRAPHS_TO_UNICODE_, ENVIRONMENTSTR_,
	#define HIGH_FUNCTION_STR1  ENVIRONMENTSTR_
		DATESTR_, INFOSTR_,
		INDEX_, RINDEX_,
		STARTS_WITH_, ENDS_WITH_, REPLACESTR_, INDEX_REGEX_, RINDEX_REGEX_, REPLACE_REGEXSTR_,
		EXTRACT_NUMBER_, EXTRACT_WORDSTR_, EXTRACT_LINESTR_,
		FIXEDSTR_, PERCENTSTR_, HEXADECIMALSTR_,
	#define HIGH_STRING_FUNCTION  HEXADECIMALSTR_

	/* Range functions. */
	#define LOW_RANGE_FUNCTION  SUM_OVER_
		SUM_OVER_,
	#define HIGH_RANGE_FUNCTION  SUM_OVER_

	#define LOW_FUNCTION  LOW_FUNCTION_1
	#define HIGH_FUNCTION  HIGH_RANGE_FUNCTION

	/* Membership operator. */
	PERIOD_,
	#define highestInputSymbol  PERIOD_

/* Symbols introduced by the parser. */

	TRUE_, FALSE_,
	GOTO_, IFTRUE_, IFFALSE_, INCREMENT_GREATER_GOTO_,
	LABEL_,
	DECREMENT_AND_ASSIGN_, ADD_3DOWN_, POP_2_,
	VEC_CELL_, MAT_CELL_, VARIABLE_REFERENCE_,
	TENSOR_LITERAL_,
	SELF0_, SELFSTR0_, TO_OBJECT_,
	OBJECT_XMIN_, OBJECT_XMAX_, OBJECT_YMIN_, OBJECT_YMAX_, OBJECT_NX_, OBJECT_NY_,
	OBJECT_DX_, OBJECT_DY_, OBJECT_NROW_, OBJECT_NCOL_, OBJECT_ROWSTR_, OBJECT_COLSTR_,
	OBJECTCELL0_, OBJECTCELLSTR0_, OBJECTCELL1_, OBJECTCELLSTR1_, OBJECTCELL2_, OBJECTCELLSTR2_,
	OBJECTLOCATION0_, OBJECTLOCATIONSTR0_, OBJECTLOCATION1_, OBJECTLOCATIONSTR1_, OBJECTLOCATION2_, OBJECTLOCATIONSTR2_,
	SELFMATRIX1_, SELFMATRIXSTR1_, SELFMATRIX2_, SELFMATRIXSTR2_,
	SELFFUNCTION1_, SELFFUNCTIONSTR1_, SELFFUNCTION2_, SELFFUNCTIONSTR2_,
	MATRIX0_, MATRIXSTR0_, MATRIX1_, MATRIXSTR1_, MATRIX2_, MATRIXSTR2_,
	FUNCTION0_, FUNCTIONSTR0_, FUNCTION1_, FUNCTIONSTR1_, FUNCTION2_, FUNCTIONSTR2_,
	SQR_,

/* Symbols introduced by lexical analysis. */

	STRING_,
	NUMERIC_VARIABLE_, NUMERIC_VECTOR_VARIABLE_, NUMERIC_MATRIX_VARIABLE_,
	STRING_VARIABLE_, STRING_ARRAY_VARIABLE_,
	VARIABLE_NAME_, INDEXED_NUMERIC_VARIABLE_, INDEXED_STRING_VARIABLE_,
	END_
	#define highestSymbol END_
};

/* The names that start with an underscore (_) do not occur in the formula text: */
/* they are used in error messages and in debugging (see Formula_print). */

static const conststring32 Formula_instructionNames [1 + highestSymbol] = { U"",
	U"if", U"then", U"else", U"(", U"[", U"{", U",", U":", U"from", U"to",
	U"or", U"and", U"not", U"=", U"<>", U"<=", U"<", U">=", U">",
	U"+", U"-", U"*", U"/", U"div", U"mod", U"^", U"_call", U"_neg",
	U"endif", U"fi", U")", U"]", U"}",
	U"a number", U"pi", U"e", U"undefined",
	U"xmin", U"xmax", U"ymin", U"ymax", U"nx", U"ny", U"dx", U"dy",
	U"row", U"col", U"nrow", U"ncol", U"row$", U"col$", U"y", U"x",
	U"self", U"self$", U"object", U"object$", U"_matrix", U"_matrix$",
	U"stopwatch",
	U"abs", U"round", U"floor", U"ceiling",
	U"rectify", U"rectify#", U"rectify##",
	U"sqrt", U"sin", U"cos", U"tan", U"arcsin", U"arccos", U"arctan", U"sinc", U"sincpi",
	U"exp", U"exp#", U"exp##",
	U"sinh", U"cosh", U"tanh", U"arcsinh", U"arccosh", U"arctanh",
	U"sigmoid", U"sigmoid#", U"softmax#", U"softmaxPerRow##",
	U"invSigmoid", U"erf", U"erfc", U"gaussP", U"gaussQ", U"invGaussQ",
	U"randomBernoulli", U"randomBernoulli#",
	U"randomPoisson", U"transpose##",
	U"rowSums#", U"columnSums#",
	U"log2", U"ln", U"log10", U"lnGamma",
	U"hertzToBark", U"barkToHertz", U"phonToDifferenceLimens", U"differenceLimensToPhon",
	U"hertzToMel", U"melToHertz", U"hertzToSemitones", U"semitonesToHertz",
	U"erb", U"hertzToErb", U"erbToHertz",
	U"sum", U"mean", U"stdev", U"center",
	U"evaluate", U"evaluate_nocheck", U"evaluate$", U"evaluate_nocheck$",
	U"string$", U"sleep", U"unicode", U"unicode$",
	U"arctan2", U"randomUniform", U"randomInteger", U"randomGauss", U"randomBinomial",
	U"chiSquareP", U"chiSquareQ", U"incompleteGammaP", U"invChiSquareQ", U"studentP", U"studentQ", U"invStudentQ",
	U"beta", U"beta2", U"besselI", U"besselK", U"lnBeta",
	U"soundPressureToPhon", U"objectsAreIdentical",
	U"inner", U"outer##", U"mul#", U"mul##", U"mul_fast##", U"mul_metal##",
	U"mul_tn##", U"mul_nt##", U"mul_tt##", U"repeat#",
	U"rowInners#",
	U"fisherP", U"fisherQ", U"invFisherQ",
	U"binomialP", U"binomialQ", U"incompleteBeta", U"invBinomialP", U"invBinomialQ",

	U"do", U"do$",
	U"writeInfo", U"writeInfoLine", U"appendInfo", U"appendInfoLine",
	U"writeFile", U"writeFileLine", U"appendFile", U"appendFileLine",
	U"pauseScript", U"exitScript", U"runScript", U"runSystem", U"runSystem_nocheck", U"runSubprocess",
	U"min", U"max", U"imin", U"imax", U"norm",
	U"left$", U"right$", U"mid$",
	U"selected", U"selected$", U"numberOfSelected", U"selected#",
	U"selectObject", U"plusObject", U"minusObject", U"removeObject",
	U"beginPause", U"real", U"positive", U"integer", U"natural",
	U"word", U"sentence", U"text", U"boolean",
	U"choice", U"optionMenu", U"option",
	U"comment", U"endPause",
	U"chooseReadFile$", U"chooseWriteFile$", U"chooseDirectory$",
	U"demoWindowTitle", U"demoShow", U"demoWaitForInput", U"demoPeekInput", U"demoInput", U"demoClickedIn",
	U"demoClicked", U"demoX", U"demoY", U"demoKeyPressed", U"demoKey$",
	U"demoShiftKeyPressed", U"demoCommandKeyPressed", U"demoOptionKeyPressed", U"demoExtraControlKeyPressed",
	U"zero#", U"zero##",
	U"linear#", U"linear##", U"to#", U"from_to#", U"from_to_by#",
	U"randomUniform#", U"randomUniform##",
	U"randomInteger#", U"randomInteger##",
	U"randomGauss#", U"randomGauss##",
	U"peaks##",
	U"size", U"numberOfRows", U"numberOfColumns", U"editor", U"hash",

	U"length", U"number", U"fileReadable",	U"deleteFile", U"createDirectory", U"variableExists",
	U"readFile", U"readFile$", U"unicodeToBackslashTrigraphs$", U"backslashTrigraphsToUnicode$", U"environment$",
	U"date$", U"info$",
	U"index", U"rindex",
	U"startsWith", U"endsWith", U"replace$", U"index_regex", U"rindex_regex", U"replace_regex$",
	U"extractNumber", U"extractWord$", U"extractLine$",
	U"fixed$", U"percent$", U"hexadecimal$",
	U"sumOver",
	U".",
	U"_true", U"_false",
	U"_goto", U"_iftrue", U"_iffalse", U"_incrementGreaterGoto",
	U"_label",
	U"_decrementAndAssign", U"_add3Down", U"_pop2",
	U"_numericVectorElement", U"_numericMatrixElement", U"_variableReference",
	U"_numericVectorLiteral",
	U"_self0", U"_self0$", U"_toObject",
	U"_object_xmin", U"_object_xmax", U"_object_ymin", U"_object_ymax", U"_object_dnx", U"_object_ny",
	U"_object_dx", U"_object_dy", U"_object_nrow", U"_object_ncol", U"_object_row$", U"_object_col$",
	U"_objectcell0", U"_objectcell0$", U"_objectcell1", U"_objectcell1$", U"_objectcell2", U"_objectcell2$",
	U"_objectlocation0", U"_objectlocation0$", U"_objectlocation1", U"_objectlocation1$", U"_objectlocation2", U"_objectlocation2$",
	U"_selfmatrix1", U"_selfmatrix1$", U"_selfmatrix2", U"_selfmatrix2$",
	U"_selffunction1", U"_selffunction1$", U"_selffunction2", U"_selffunction2$",
	U"_matrix0", U"_matrix0$", U"_matrix1", U"_matrix1$", U"_matrix2", U"_matrix2$",
	U"_function0", U"_function0$", U"_function1", U"_function1$", U"_function2", U"_function2$",
	U"_square",
	U"_string",
	U"a numeric variable", U"a vector variable", U"a matrix variable",
	U"a string variable", U"a string array variable",
	U"a variable name", U"an indexed numeric variable", U"an indexed string variable",
	U"the end of the formula"
};

#define newlabel (-- ilabel)
#define newread (lexan [++ ilexan]. symbol)
#define oldread  (-- ilexan)

static void formulaError (conststring32 message, int position) {
	static MelderString truncatedExpression { };
	MelderString_ncopy (& truncatedExpression, theExpression, position + 1);
	Melder_throw (message, U":\n« ", truncatedExpression.string);
}

static conststring32 languageNameCompare_searchString;

static int languageNameCompare (const void *first, const void *second) {
	int i = * (int *) first, j = * (int *) second;
	return str32cmp (i == 0 ? languageNameCompare_searchString : Formula_instructionNames [i],
		j == 0 ? languageNameCompare_searchString : Formula_instructionNames [j]);
}

static int Formula_hasLanguageName (conststring32 f) {
	static int *index;
	if (! index) {
		index = NUMvector <int> (1, highestInputSymbol);
		for (int tok = 1; tok <= highestInputSymbol; tok ++) {
			index [tok] = tok;
		}
		qsort (& index [1], highestInputSymbol, sizeof (int), languageNameCompare);
	}
	if (! index) {   // linear search
		for (int tok = 1; tok <= highestInputSymbol; tok ++) {
			if (str32equ (f, Formula_instructionNames [tok])) return tok;
		}
	} else {   // binary search
		int dummy = 0, *found;
		languageNameCompare_searchString = f;
		found = (int *) bsearch (& dummy, & index [1], highestInputSymbol, sizeof (int), languageNameCompare);
		if (found) return *found;
	}
	return 0;
}

static void Formula_lexan () {
/*
	Purpose:
		translate the formula text into a series of symbols.
	Return:
		0 in case of error, otherwise 1.
	Postcondition:
		if result != 0, then the last symbol is L"END_".
	Example:
		the text L"x*7" yields 5 symbols:
			lexan [0] is empty;
			lexan [1]. symbol = X_;
			lexan [2]. symbol = MUL_;
			lexan [3]. symbol = NUMBER_;
			lexan [3]. number = 7.00000000e+00;
			lexan [4]. symbol = END_;
*/
	char32 kar;   /* The character most recently read from theExpression. */
	int ikar = -1;   /* The position of that character in theExpression. */
#define newchar kar = theExpression [++ ikar]
#define oldchar -- ikar

	int itok = 0;   /* Position of most recent symbol in "lexan". */
#define newtok(s)  { lexan [++ itok]. symbol = s; lexan [itok]. position = ikar; }
#define toknumber(g)  lexan [itok]. content.number = (g)
#define tokmatrix(m)  lexan [itok]. content.object = (m)

	static MelderString token { };   /* String to collect a symbol name in. */
#define stringtokon MelderString_empty (& token);
#define stringtokchar { MelderString_appendCharacter (& token, kar); newchar; }
#define stringtokoff (void) 0

	ilexan = iparse = ilabel = numberOfStringConstants = 0;
	do {
		newchar;
		if (Melder_isHorizontalOrVerticalSpace (kar)) {
			;   // ignore spaces and tabs
		} else if (kar == U'\0') {
			newtok (END_)
		} else if (Melder_isAsciiDecimalNumber (kar)) {
			char32 saveKar = kar;
			bool isHexadecimal = false;
			if (kar == U'0') {
				newchar;
				if (kar == U'x') {
					isHexadecimal = true;
					newchar;
				} else {
					oldchar;
				}
			}
			if (isHexadecimal) {
				stringtokon;
				do stringtokchar while (Melder_isHexadecimalDigit (kar));
				stringtokoff;
				oldchar;
				newtok (NUMBER_)
				toknumber (strtoull (Melder_peek32to8 (token.string), nullptr, 16));
			} else {
				kar = saveKar;
				stringtokon;
				do stringtokchar while (Melder_isAsciiDecimalNumber (kar));
				if (kar == U'.') do stringtokchar while (Melder_isAsciiDecimalNumber (kar));
				if (kar == U'e' || kar == U'E') {
					kar = U'e'; stringtokchar
					if (kar == U'-') stringtokchar
					else if (kar == U'+') newchar;
					if (! Melder_isAsciiDecimalNumber (kar))
						formulaError (U"Missing exponent", ikar);
					do stringtokchar while (Melder_isAsciiDecimalNumber (kar));
				}
				if (kar == U'%') stringtokchar
				stringtokoff;
				oldchar;
				newtok (NUMBER_)
				toknumber (Melder_atof (token.string));
			}
		} else if (Melder_isLetter (kar) && ! Melder_isUpperCaseLetter (kar) ||
				(kar == U'.' && Melder_isLetter (theExpression [ikar + 1]) && ! Melder_isUpperCaseLetter (theExpression [ikar + 1])
				&& (itok == 0 || (lexan [itok]. symbol != MATRIX_ && lexan [itok]. symbol != MATRIXSTR_
					&& lexan [itok]. symbol != CLOSING_BRACKET_)))) {
			int tok;
			bool isString = false;
			int rank = 0;
			stringtokon;
			do stringtokchar while (Melder_isWordCharacter (kar) || kar == U'.');
			if (kar == '$') {
				stringtokchar
				isString = true;
			}
			if (kar == '#') {
				rank += 1;
				stringtokchar
				if (kar == '#') {
					rank += 1;
					stringtokchar
					if (kar == '#') {
						rank += 1;
						stringtokchar
						if (kar == '#') {
							rank += 1;
							stringtokchar
						}
					}
				}
			}
			stringtokoff;
			oldchar;
			/*
			 * 'token' now contains a word, possibly ending in a dollar or number sign;
			 * it could be a variable name, a function name, both, or a procedure name.
			 * Try a language or function name first.
			 */
			tok = Formula_hasLanguageName (token.string);
			if (tok) {
				/*
				 * We have a language name or function name. It MIGHT be meant to be a variable name, though,
				 * regarding the large and expanding number of language names.
				 */
				/*
				 * First the constants. They are reserved words and can never be variable names.
				 */
				if (tok == NUMBER_PI_) {
					newtok (NUMBER_)
					toknumber (NUMpi);
				} else if (tok == NUMBER_E_) {
					newtok (NUMBER_)
					toknumber (NUMe);
				} else if (tok == NUMBER_UNDEFINED_) {
					newtok (NUMBER_)
					toknumber (undefined);
				/*
				 * One very common language name must be converted to a synonym.
				 */
				} else if (tok == FI_) {
					newtok (ENDIF_)
				/*
				 * Is it a function name? These may be ambiguous.
				 */
				} else if (tok >= LOW_FUNCTION && tok <= HIGH_FUNCTION) {
					/*
					 * Look ahead to find out whether the next token is a left parenthesis (or a colon).
					 */
					int jkar;
					jkar = ikar + 1;
					while (Melder_isHorizontalSpace (theExpression [jkar])) jkar ++;
					if (theExpression [jkar] == U'(' || theExpression [jkar] == U':') {
						newtok (tok)   // this must be a function name
					} else if (theExpression [jkar] == U'[' && rank == 0) {
						if (isString) {
							newtok (INDEXED_STRING_VARIABLE_)
						} else {
							newtok (INDEXED_NUMERIC_VARIABLE_)
						}
						lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
						numberOfStringConstants ++;
					} else {
						/*
						 * This could be a variable with the same name as a function.
						 */
						InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
						if (! var) {
							newtok (VARIABLE_NAME_)
							lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
							numberOfStringConstants ++;
						} else {
							if (rank == 0) {
								if (isString) {
									newtok (STRING_VARIABLE_)
								} else {
									newtok (NUMERIC_VARIABLE_)
								}
							} else if (rank == 1) {
								if (isString) {
									newtok (STRING_ARRAY_VARIABLE_)
								} else {
									newtok (NUMERIC_VECTOR_VARIABLE_)
								}
							} else if (rank == 2) {
								if (isString) {
									formulaError (U"String matrices not implemented.", ikar);
								} else {
									newtok (NUMERIC_MATRIX_VARIABLE_)
								}
							} else if (rank == 3) {
								formulaError (U"Rank-3 tensors not implemented.", ikar);
							} else {
								formulaError (U"Rank-4 tensors not implemented.", ikar);
							}
							lexan [itok]. content.variable = var;
						}
					}
				/*
				 * Not a function name.
				 * Must be a language name (if, then, else, endif, or, and, not, div, mod, x, ncol, stopwatch).
				 * Some can be used as variable names (x, ncol...).
				 */
				} else if (tok >= LOW_ATTRIBUTE && tok <= HIGH_ATTRIBUTE) {
					/*
					 * Look back to find out whether this is an attribute.
					 */
					if (itok > 0 && lexan [itok]. symbol == PERIOD_) {
						/*
						 * This must be an attribute that follows a period.
						 */
						newtok (tok)
					} else if (theSource) {
						/*
						 * Look for ambiguity.
						 */
						if (Interpreter_hasVariable (theInterpreter, token.string))
							Melder_throw (
								U"«", token.string,
								U"» is ambiguous: a variable or an attribute of the current object. "
								U"Please change variable name.");
						if (tok == ROW_ || tok == COL_ || tok == X_ || tok == Y_) {
							newtok (tok)
						} else {
							newtok (MATRIX_)
							tokmatrix (theSource);
							newtok (PERIOD_)
							newtok (tok)
						}
					} else {
						/*
						 * This must be a variable, since there is no "current object" here.
						 */
						int jkar = ikar + 1;
						while (Melder_isHorizontalSpace (theExpression [jkar])) jkar ++;
						if (theExpression [jkar] == U'[' && rank == 0) {
							if (isString) {
								newtok (INDEXED_STRING_VARIABLE_)
							} else {
								newtok (INDEXED_NUMERIC_VARIABLE_)
							}
							lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
							numberOfStringConstants ++;
						} else {
							InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
							if (! var) {
								newtok (VARIABLE_NAME_)
								lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
								numberOfStringConstants ++;
							} else {
								if (rank == 0) {
									if (isString) {
										newtok (STRING_VARIABLE_)
									} else {
										newtok (NUMERIC_VARIABLE_)
									}
								} else if (rank == 1) {
									if (isString) {
										newtok (STRING_ARRAY_VARIABLE_)
									} else {
										newtok (NUMERIC_VECTOR_VARIABLE_)
									}
								} else if (rank == 2) {
									if (isString) {
										formulaError (U"String matrices not implemented.", ikar);
									} else {
										newtok (NUMERIC_MATRIX_VARIABLE_)
									}
								} else if (rank == 3) {
									formulaError (U"Rank-3 tensors not implemented.", ikar);
								} else {
									formulaError (U"Rank-4 tensors not implemented.", ikar);
								}
								lexan [itok]. content.variable = var;
							}
						}
					}
				} else {
					newtok (tok)   /* This must be a language name. */
				}
			} else {
				/*
				 * token.string is not a language name
				 */
				int jkar = ikar + 1;
				while (Melder_isHorizontalSpace (theExpression [jkar])) jkar ++;
				if (theExpression [jkar] == U'(' || theExpression [jkar] == U':') {
					Melder_throw (
						U"Unknown function «", token.string, U"» in formula.");
				} else if (theExpression [jkar] == '[' && rank == 0) {
					if (isString) {
						newtok (INDEXED_STRING_VARIABLE_)
					} else {
						newtok (INDEXED_NUMERIC_VARIABLE_)
					}
					lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
					numberOfStringConstants ++;
				} else {
					InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
					if (! var) {
						newtok (VARIABLE_NAME_)
						lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
						numberOfStringConstants ++;
					} else {
						if (rank == 0) {
							if (isString) {
								newtok (STRING_VARIABLE_)
							} else {
								newtok (NUMERIC_VARIABLE_)
							}
						} else if (rank == 1) {
							if (isString) {
								newtok (STRING_ARRAY_VARIABLE_)
							} else {
								newtok (NUMERIC_VECTOR_VARIABLE_)
							}
						} else if (rank == 2) {
							if (isString) {
								formulaError (U"String matrices not implemented.", ikar);
							} else {
								newtok (NUMERIC_MATRIX_VARIABLE_)
							}
						} else if (rank == 3) {
							formulaError (U"Rank-3 tensors not implemented.", ikar);
						} else {
							formulaError (U"Rank-4 tensors not implemented.", ikar);
						}
						lexan [itok]. content.variable = var;
					}
				}
			}
		} else if (kar >= U'A' && kar <= U'Z') {
			bool endsInDollarSign = false;
			stringtokon;
			do stringtokchar while (Melder_isAlphanumeric (kar) || kar == U'_');
			if (kar == U'$') { stringtokchar endsInDollarSign = true; }
			stringtokoff;
			oldchar;
			/*
			 * 'token' now contains a word that could be an object name.
			 */
			char32 *underscore = str32chr (token.string, '_');
			if (str32equ (token.string, U"Self")) {
				if (! theSource)
					formulaError (U"Cannot use \"Self\" if there is no current object.", ikar);
				newtok (MATRIX_)
				tokmatrix (theSource);
			} else if (str32equ (token.string, U"Self$")) {
				if (! theSource)
					formulaError (U"Cannot use \"Self$\" if there is no current object.", ikar);
				newtok (MATRIXSTR_)
				tokmatrix (theSource);
			} else if (! underscore) {
				Melder_throw (
					U"Unknown symbol «", token.string, U"» in formula "
					U"(variables start with nonupper case; object names contain an underscore).");
			} else if (str32nequ (token.string, U"Object_", 7)) {
				integer uniqueID = Melder_atoi (token.string + 7);
				int i = theCurrentPraatObjects -> n;
				while (i > 0 && uniqueID != theCurrentPraatObjects -> list [i]. id)
					i --;
				if (i == 0)
					formulaError (U"No such object (note: variables start with nonupper case)", ikar);
				newtok (endsInDollarSign ? MATRIXSTR_ : MATRIX_)
				tokmatrix ((Daata) theCurrentPraatObjects -> list [i]. object);
			} else {
				int i = theCurrentPraatObjects -> n;
				*underscore = ' ';
				if (endsInDollarSign) token.string [-- token.length] = '\0';
				while (i > 0 && ! str32equ (token.string, theCurrentPraatObjects -> list [i]. name.get()))
					i --;
				if (i == 0)
					formulaError (U"No such object (note: variables start with nonupper case)", ikar);
				newtok (endsInDollarSign ? MATRIXSTR_ : MATRIX_)
				tokmatrix ((Daata) theCurrentPraatObjects -> list [i]. object);
			}
		} else if (kar == U'(') {
			newtok (OPENING_PARENTHESIS_)
		} else if (kar == U')') {
			newtok (CLOSING_PARENTHESIS_)
		} else if (kar == U'+') {
			newtok (ADD_)
		} else if (kar == U'-') {
			if (itok == 0 || lexan [itok]. symbol <= MINUS_) {
				newtok (MINUS_)
			} else {
				newtok (SUB_)
			}
		} else if (kar == U'*') {
			newchar;
			if (kar == U'*') {
				newtok (POWER_)   /* "**" = "^" */
			} else {
				oldchar;
				newtok (MUL_)
			}
		} else if (kar == U'/') {
			newchar;
			if (kar == U'=') {
				newtok (NE_)   /* "/=" = "<>" */
			} else {
				oldchar;
				newtok (RDIV_)
			}
		} else if (kar == U'=') {
			newtok (EQ_)   /* "=" */
			newchar;
			if (kar != U'=') {
				oldchar;   /* "==" = "=" */
			}
		} else if (kar == U'>') {
			newchar;
			if (kar == U'=') {
				newtok (GE_)
			} else {
				oldchar;
				newtok (GT_)
			}
		} else if (kar == U'<') {
			newchar;
			if (kar == U'=') {
				newtok (LE_)
			} else if (kar == U'>') {
				newtok (NE_)
			} else {
				oldchar;
				newtok (LT_)
			}
		} else if (kar == U'!') {
			newchar;
			if (kar == U'=') {
				newtok (NE_)   /* "!=" = "<>" */
			} else {
				oldchar;
				newtok (NOT_)
			}
		} else if (kar == U',') {
			newtok (COMMA_)
		} else if (kar == U':') {
			newtok (COLON_)
		} else if (kar == U';') {
			newtok (END_)
		} else if (kar == U'^') {
			newtok (POWER_)
		} else if (kar == U'@') {
			do {
				newchar;
			} while (Melder_isHorizontalSpace (kar));
			stringtokon;
			do stringtokchar while (Melder_isWordCharacter (kar) || kar == U'.');
			stringtokoff;
			oldchar;
			newtok (CALL_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'\"') {
			/*
			 * String constant.
			 */
			newchar;
			stringtokon;
			for (;;) {
				if (kar == U'\0')
					formulaError (U"No closing quote in string constant", ikar);
				if (kar == U'\"') {
					newchar;
					if (kar == U'\"') stringtokchar
					else break;
				} else {
					stringtokchar
				}
			}
			stringtokoff;
			oldchar;
			newtok (STRING_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'~') {
			/*
				The content of the remainder of the line,
				including any leading and trailing space,
				will become a string constant (this is good for formulas).
			*/
			newchar;
			stringtokon;
			for (;;) {
				if (kar == U'\0') break;
				stringtokchar
			}
			stringtokoff;
			oldchar;
			newtok (STRING_)
			lexan [itok]. content.string = Melder_dup_f (token.string).transfer();
			numberOfStringConstants ++;
		} else if (kar == U'|') {
			newtok (OR_)   /* "|" = "or" */
			newchar;
			if (kar != U'|') {
				oldchar;   /* "||" = "or" */
			}
		} else if (kar == U'&') {
			newtok (AND_)   /* "&" = "and" */
			newchar;
			if (kar != U'&') {
				oldchar;   /* "&&" = "and" */
			}
		} else if (kar == U'[') {
			newtok (OPENING_BRACKET_)
		} else if (kar == U']') {
			newtok (CLOSING_BRACKET_)
		} else if (kar == U'{') {
			newtok (OPENING_BRACE_)
		} else if (kar == U'}') {
			newtok (CLOSING_BRACE_)
		} else if (kar == U'.') {
			newtok (PERIOD_)
		} else {
			formulaError (U"Unknown symbol", ikar);
		}
	} while (lexan [itok]. symbol != END_);
}

static void fit (int symbol) {
	if (symbol == newread) {
		return;   // success
	} else {
		const conststring32 symbolName1 = Formula_instructionNames [symbol];
		const conststring32 symbolName2 = Formula_instructionNames [lexan [ilexan]. symbol];
		const bool needQuotes1 = ! str32chr (symbolName1, U' ');
		const bool needQuotes2 = ! str32chr (symbolName2, U' ');
		static MelderString melding { };
		MelderString_copy (& melding,
			U"Expected ", ( needQuotes1 ? U"\"" : nullptr ), symbolName1, ( needQuotes1 ? U"\"" : nullptr ),
			U", but found ", ( needQuotes2 ? U"\"" : nullptr ), symbolName2, ( needQuotes2 ? U"\"" : nullptr ));
		formulaError (melding.string, lexan [ilexan]. position);
	}
}

static bool fitArguments () {
    int symbol = newread;
    if (symbol == OPENING_PARENTHESIS_) return true;   // success: a function call like: myFunction (...)
    if (symbol == COLON_) return false;   // success: a function call like: myFunction: ...
    const conststring32 symbolName2 = Formula_instructionNames [lexan [ilexan]. symbol];
    bool needQuotes2 = ! str32chr (symbolName2, U' ');
    static MelderString melding { };
    MelderString_copy (& melding,
		U"Expected \"(\" or \":\", but found ", ( needQuotes2 ? U"\"" : nullptr ), symbolName2, ( needQuotes2 ? U"\"" : nullptr ));
    formulaError (melding.string, lexan [ilexan]. position);
    return false;   // will never occur
}

#define newparse(s)  parse [++ iparse]. symbol = (s)
#define parsenumber(g)  parse [iparse]. content.number = (g)
#define parselabel(l)  parse [iparse]. content.label = (l)

static void parseExpression ();

static void parsePowerFactor () {
	int symbol = newread;

	if (symbol >= LOW_VALUE && symbol <= HIGH_VALUE) {
		newparse (symbol);
		if (symbol == NUMBER_) parsenumber (lexan [ilexan]. content.number);
		return;
	}

	if (symbol == STRING_) {
		newparse (symbol);
		parse [iparse]. content.string = lexan [ilexan]. content.string;   // reference copy!
		return;
	}

	if (symbol == NUMERIC_VARIABLE_ || symbol == STRING_VARIABLE_) {
		newparse (symbol);
		parse [iparse]. content.variable = lexan [ilexan]. content.variable;
		return;
	}

	if (symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_) {
		char32 *var = lexan [ilexan]. content.string;   // Save before incrementing ilexan.
		if (newread == OPENING_BRACKET_) {
			int n = 0;
			if (newread != CLOSING_BRACKET_) {
				oldread;
				parseExpression ();
				n ++;
				while (newread == COMMA_) {
					parseExpression ();
					n ++;
				}
				oldread;
				fit (CLOSING_BRACKET_);
			}
			newparse (NUMBER_); parsenumber (n);
			newparse (symbol);
		} else {
			Melder_fatal (U"Formula:parsePowerFactor (indexed variable): No '['; cannot happen.");
		}
		parse [iparse]. content.string = var;
		return;
	}

	if (symbol == NUMERIC_VECTOR_VARIABLE_) {
		InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (newread == OPENING_BRACKET_) {
			parseExpression ();
			fit (CLOSING_BRACKET_);
			newparse (VEC_CELL_);
		} else {
			oldread;
			newparse (NUMERIC_VECTOR_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == NUMERIC_MATRIX_VARIABLE_) {
		InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (newread == OPENING_BRACKET_) {
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			fit (CLOSING_BRACKET_);
			newparse (MAT_CELL_);
		} else {
			oldread;
			newparse (NUMERIC_MATRIX_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == VARIABLE_NAME_) {
		InterpreterVariable var = Interpreter_hasVariable (theInterpreter, lexan [ilexan]. content.string);
		if (! var)
			formulaError (U"Unknown variable", lexan [ilexan]. position);
		newparse (NUMERIC_VARIABLE_);
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == SELF_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFMATRIX2_);
				fit (CLOSING_BRACKET_);
				return;
			}
			oldread;
			newparse (SELFMATRIX1_);
			fit (CLOSING_BRACKET_);
			return;
		}
		if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFFUNCTION2_);
				fit (CLOSING_PARENTHESIS_);
				return;
			}
			oldread;
			newparse (SELFFUNCTION1_);
			fit (CLOSING_PARENTHESIS_);
			return;
		}
		oldread;
		newparse (SELF0_);
		return;
	}

	if (symbol == SELFSTR_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFMATRIXSTR2_);
				fit (CLOSING_BRACKET_);
				return;
			}
			oldread;
			newparse (SELFMATRIXSTR1_);
			fit (CLOSING_BRACKET_);
			return;
		}
		if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();
			if (newread == COMMA_) {
				parseExpression ();
				newparse (SELFFUNCTIONSTR2_);
				fit (CLOSING_PARENTHESIS_);
				return;
			}
			oldread;
			newparse (SELFFUNCTIONSTR1_);
			fit (CLOSING_PARENTHESIS_);
			return;
		}
		oldread;
		newparse (SELFSTR0_);
		return;
	}

	if (symbol == OBJECT_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_BRACKET_) {
				symbol = newread;
				if (symbol == PERIOD_) {
					switch (newread) {
						case XMIN_:
							newparse (OBJECT_XMIN_);
							return;
						case XMAX_:
							newparse (OBJECT_XMAX_);
							return;
						case YMIN_:
							newparse (OBJECT_YMIN_);
							return;
						case YMAX_:
							newparse (OBJECT_YMAX_);
							return;
						case NX_:
							newparse (OBJECT_NX_);
							return;
						case NY_:
							newparse (OBJECT_NY_);
							return;
						case DX_:
							newparse (OBJECT_DX_);
							return;
						case DY_:
							newparse (OBJECT_DY_);
							return;
						case NROW_:
							newparse (OBJECT_NROW_);
							return;
						case NCOL_:
							newparse (OBJECT_NCOL_);
							return;
						case ROWSTR_:
							fit (OPENING_BRACKET_);
							parseExpression ();
							newparse (OBJECT_ROWSTR_);
							fit (CLOSING_BRACKET_);
							return;
						case COLSTR_:
							fit (OPENING_BRACKET_);
							parseExpression ();
							newparse (OBJECT_COLSTR_);
							fit (CLOSING_BRACKET_);
							return;
						default:
							formulaError (U"After \"object [number].\" there should be \"xmin\", \"xmax\", \"ymin\", "
								"\"ymax\", \"nx\", \"ny\", \"dx\", \"dy\", \"nrow\" or \"ncol\"", lexan [ilexan]. position);
					}
				} else if (symbol == OPENING_BRACKET_) {
					parseExpression ();
					if (newread == COMMA_) {
						parseExpression ();
						newparse (OBJECTCELL2_);
						fit (CLOSING_BRACKET_);
					} else {
						oldread;
						newparse (OBJECTCELL1_);
						fit (CLOSING_BRACKET_);
					}
				} else {
					oldread;
					newparse (OBJECTCELL0_);
				}
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTCELL2_);
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (OBJECTCELL1_);
					fit (CLOSING_BRACKET_);
				}
			}
		} else if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_PARENTHESIS_) {
				newparse (OBJECTLOCATION0_);
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTLOCATION2_);
					fit (CLOSING_PARENTHESIS_);
				} else {
					oldread;
					newparse (OBJECTLOCATION1_);
					fit (CLOSING_PARENTHESIS_);
				}
			}
		} else {
			formulaError (U"After \"object\" there should be \"(\" or \"[\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == OBJECTSTR_) {
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_BRACKET_) {
				newparse (OBJECTCELLSTR0_);
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTCELLSTR2_);
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (OBJECTCELLSTR1_);
					fit (CLOSING_BRACKET_);
				}
			}
		} else if (symbol == OPENING_PARENTHESIS_) {
			parseExpression ();   // the object's name or ID
			newparse (TO_OBJECT_);
			if (newread == CLOSING_PARENTHESIS_) {
				newparse (OBJECTLOCATIONSTR0_);
			} else {
				oldread;
				fit (COMMA_);
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (OBJECTLOCATIONSTR2_);
					fit (CLOSING_PARENTHESIS_);
				} else {
					oldread;
					newparse (OBJECTLOCATIONSTR1_);
					fit (CLOSING_PARENTHESIS_);
				}
			}
		} else {
			formulaError (U"After \"object$\" there should be \"(\" or \"[\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == OPENING_PARENTHESIS_) {
		parseExpression ();
		fit (CLOSING_PARENTHESIS_);
		return;
	}

	if (symbol == IF_) {
		int elseLabel = newlabel;   // has to be local,
		int endifLabel = newlabel;   // because of recursion
		parseExpression ();
		newparse (IFFALSE_);  parselabel (elseLabel);
		fit (THEN_);
		parseExpression ();
		newparse (GOTO_);     parselabel (endifLabel);
		fit (ELSE_);
		newparse (LABEL_);    parselabel (elseLabel);
		parseExpression ();
		fit (ENDIF_);
		newparse (LABEL_);    parselabel (endifLabel);
		return;
	}

	if (symbol == MATRIX_) {
		Daata thee = lexan [ilexan]. content.object;
		Melder_assert (thee != nullptr);
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			if (newread == CLOSING_BRACKET_) {
				newparse (MATRIX0_);
				parse [iparse]. content.object = thee;
			} else {
				oldread;
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (MATRIX2_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (MATRIX1_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				}
			}
		} else if (symbol == OPENING_PARENTHESIS_) {
			if (newread == CLOSING_PARENTHESIS_) {
				newparse (FUNCTION0_);
				parse [iparse]. content.object = thee;
			} else {
				oldread;
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (FUNCTION2_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_PARENTHESIS_);
				} else {
					oldread;
					newparse (FUNCTION1_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_PARENTHESIS_);
				}
			}
		} else if (symbol == PERIOD_) {
			switch (newread) {
				case XMIN_:
					if (! thy v_hasGetXmin ()) {
						formulaError (U"Attribute \"xmin\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getXmin ());
						return;
					}
				case XMAX_:
					if (! thy v_hasGetXmax ()) {
						formulaError (U"Attribute \"xmax\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getXmax ());
						return;
					}
				case YMIN_:
					if (! thy v_hasGetYmin ()) {
						formulaError (U"Attribute \"ymin\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getYmin ());
						return;
					}
				case YMAX_:
					if (! thy v_hasGetYmax ()) {
						formulaError (U"Attribute \"ymax\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getYmax ());
						return;
					}
				case NX_:
					if (! thy v_hasGetNx ()) {
						formulaError (U"Attribute \"nx\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNx ());
						return;
					}
				case NY_:
					if (! thy v_hasGetNy ()) {
						formulaError (U"Attribute \"ny\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNy ());
						return;
					}
				case DX_:
					if (! thy v_hasGetDx ()) {
						formulaError (U"Attribute \"dx\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getDx ());
						return;
					}
				case DY_:
					if (! thy v_hasGetDy ()) {
						formulaError (U"Attribute \"dy\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getDy ());
						return;
					}
				case NCOL_:
					if (! thy v_hasGetNcol ()) {
						formulaError (U"Attribute \"ncol\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNcol ());
						return;
					}
				case NROW_:
					if (! thy v_hasGetNrow ()) {
						formulaError (U"Attribute \"nrow\" not defined for this object", lexan [ilexan]. position);
					} else {
						newparse (NUMBER_);
						parsenumber (thy v_getNrow ());
						return;
					}
				case ROWSTR_:
					if (! thy v_hasGetRowStr ()) {
						formulaError (U"Attribute \"row$\" not defined for this object", lexan [ilexan]. position);
					} else {
						fit (OPENING_BRACKET_);
						parseExpression ();
						newparse (ROWSTR_);
						parse [iparse]. content.object = thee;
						fit (CLOSING_BRACKET_);
						return;
					}
				case COLSTR_:
					if (! thy v_hasGetColStr ()) {
						formulaError (U"Attribute \"col$\" not defined for this object", lexan [ilexan]. position);
					} else {
						fit (OPENING_BRACKET_);
						parseExpression ();
						newparse (COLSTR_);
						parse [iparse]. content.object = thee;
						fit (CLOSING_BRACKET_);
						return;
					}
				default: formulaError (U"Unknown attribute.", lexan [ilexan]. position);
			}
		} else {
			formulaError (U"After a name of a matrix there should be \"(\", \"[\", or \".\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == MATRIXSTR_) {
		Daata thee = lexan [ilexan]. content.object;
		Melder_assert (thee != nullptr);
		symbol = newread;
		if (symbol == OPENING_BRACKET_) {
			if (newread == CLOSING_BRACKET_) {
				newparse (MATRIXSTR0_);
				parse [iparse]. content.object = thee;
			} else {
				oldread;
				parseExpression ();
				if (newread == COMMA_) {
					parseExpression ();
					newparse (MATRIXSTR2_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				} else {
					oldread;
					newparse (MATRIXSTR1_);
					parse [iparse]. content.object = thee;
					fit (CLOSING_BRACKET_);
				}
			}
		} else {
			formulaError (U"After a name of a matrix$ there should be \"[\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol >= LOW_FUNCTION_1 && symbol <= HIGH_FUNCTION_1) {
		bool isParenthesis = fitArguments ();
		parseExpression ();
		if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_2 && symbol <= HIGH_FUNCTION_2) {
		bool isParenthesis = fitArguments ();
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_3 && symbol <= HIGH_FUNCTION_3) {
		bool isParenthesis = fitArguments ();
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		fit (COMMA_);
		parseExpression ();
		if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_N && symbol <= HIGH_FUNCTION_N) {
		int n = 0;
		bool isParenthesis = fitArguments ();
		if (newread != CLOSING_PARENTHESIS_) {
			oldread;
			parseExpression ();
			n ++;
			while (newread == COMMA_) {
				parseExpression ();
				n ++;
			}
			oldread;
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		}
		newparse (NUMBER_); parsenumber (n);
		newparse (symbol);
		return;
	}

	if (symbol == OPENING_BRACE_) {
		parseExpression ();
		int n = 1;
		while (newread == COMMA_) {
			parseExpression ();
			n ++;
		}
		oldread;
		fit (CLOSING_BRACE_);
		newparse (NUMBER_); parsenumber (n);
		newparse (TENSOR_LITERAL_);
		return;
	}

	if (symbol == CALL_) {
		char32 *procedureName = lexan [ilexan]. content.string;   // reference copy!
		int n = 0;
		bool isParenthesis = fitArguments ();
		if (newread != CLOSING_PARENTHESIS_) {
			oldread;
			parseExpression ();
			n ++;
			while (newread == COMMA_) {
				parseExpression ();
				n ++;
			}
			oldread;
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		}
		newparse (NUMBER_); parsenumber (n);
		newparse (CALL_);
		parse [iparse]. content.string = procedureName;
		return;
	}

	if (symbol >= LOW_STRING_FUNCTION && symbol <= HIGH_STRING_FUNCTION) {
		if (symbol >= LOW_FUNCTION_STR1 && symbol <= HIGH_FUNCTION_STR1) {
            bool isParenthesis = fitArguments ();
			parseExpression ();
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		} else if (symbol == INDEX_ || symbol == RINDEX_ ||
			symbol == STARTS_WITH_ || symbol == ENDS_WITH_ ||
			symbol == INDEX_REGEX_ || symbol == RINDEX_REGEX_ || symbol == EXTRACT_NUMBER_)
		{
            bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		} else if (symbol == DATESTR_ || symbol == INFOSTR_) {
			fit (OPENING_PARENTHESIS_);
			fit (CLOSING_PARENTHESIS_);
		} else if (symbol == EXTRACT_WORDSTR_ || symbol == EXTRACT_LINESTR_) {
            bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		} else if (symbol == FIXEDSTR_ || symbol == PERCENTSTR_ || symbol == HEXADECIMALSTR_) {
            bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		} else if (symbol == REPLACESTR_ || symbol == REPLACE_REGEXSTR_) {
            bool isParenthesis = fitArguments ();
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
		} else {
			oldread;   // needed for retry if we are going to be in a string comparison?
			formulaError (U"Function expected", lexan [ilexan + 1]. position);
		}
		newparse (symbol);
		return;
	}

	if (symbol >= LOW_RANGE_FUNCTION && symbol <= HIGH_RANGE_FUNCTION) {
		if (symbol == SUM_OVER_) {
			//theOptimize = 1;
			newparse (NUMBER_); parsenumber (0.0);   // initialize the sum
            bool isParenthesis = fitArguments ();
			int symbol2 = newread;
			if (symbol2 == NUMERIC_VARIABLE_) {   // an existing variable
				newparse (VARIABLE_REFERENCE_);
				InterpreterVariable loopVariable = lexan [ilexan]. content.variable;
				parse [iparse]. content.variable = loopVariable;
			} else if (symbol2 == VARIABLE_NAME_) {   // a new variable
				InterpreterVariable loopVariable = Interpreter_lookUpVariable (theInterpreter, lexan [ilexan]. content.string);
				newparse (VARIABLE_REFERENCE_);
				parse [iparse]. content.variable = loopVariable;
			} else {
				formulaError (U"Numeric variable expected", lexan [ilexan]. position);
			}
			// now on stack: sum, loop variable
			if (newread == FROM_) {
				parseExpression ();
			} else {
				oldread;
				newparse (NUMBER_); parsenumber (1.0);
			}
			newparse (DECREMENT_AND_ASSIGN_);   // this pushes the variable back on the stack
			// now on stack: sum, loop variable
			fit (TO_);
			parseExpression ();
			// now on stack: sum, loop variable, end value
			int startLabel = newlabel;
			int endLabel = newlabel;
			newparse (LABEL_); parselabel (startLabel);
			newparse (INCREMENT_GREATER_GOTO_); parselabel (endLabel);
			fit (COMMA_);
			parseExpression ();
			if (isParenthesis) fit (CLOSING_PARENTHESIS_);
			// now on stack: sum, loop variable, end value, value to add
			newparse (ADD_3DOWN_);
			// now on stack: sum, loop variable, end value
			newparse (GOTO_); parselabel (startLabel);
			newparse (LABEL_); parselabel (endLabel);
			newparse (POP_2_);
			// now on stack: sum
			return;
		}
	}

	if (symbol == STOPWATCH_) {
		newparse (symbol);
		return;
	}

	oldread;   // needed for retry if we are going to be in a string comparison
	formulaError (U"Symbol misplaced", lexan [ilexan + 1]. position);
}

static void parseFactor ();

static void parsePowerFactors () {
	if (newread == POWER_) {
		if (ilexan > 2 && lexan [ilexan - 2]. symbol == MINUS_ && lexan [ilexan - 1]. symbol == NUMBER_) {
			oldread;
			formulaError (U"Expressions like -3^4 are ambiguous; use (-3)^4 or -(3^4) or -(3)^4", lexan [ilexan + 1]. position);
		}
		parseFactor ();   // like a^-b
		newparse (POWER_);
	}
	else
		oldread;
}

static void parseMinus () {
	parsePowerFactor ();
	parsePowerFactors ();
}

static void parseFactor () {
	if (newread == MINUS_) {
		parseFactor ();   // there can be multiple consecutive minuses
		newparse (MINUS_);
		return;
	}
	oldread;
	parseMinus ();   // like -a^b
}

static void parseFactors () {
	int symbol = newread;   // has to be local, because of recursion
	if (symbol == MUL_ || symbol == RDIV_ || symbol == IDIV_ || symbol == MOD_) {
		parseFactor ();
		newparse (symbol);
		parseFactors ();
	}
	else oldread;
}

static void parseTerm () {
	parseFactor ();
	parseFactors ();
}

static void parseTerms () {
	int symbol = newread;
	if (symbol == ADD_ || symbol == SUB_) {
		parseTerm ();
		newparse (symbol);
		parseTerms ();
	}
	else oldread;
}

static void parseNot () {
	parseTerm ();
	parseTerms ();
	int symbol = newread;
	if (symbol >= EQ_ && symbol <= GT_) {
		parseTerm ();
		parseTerms ();
		newparse (symbol);
	}
	else oldread;
}

static void parseAnd () {
	if (newread == NOT_) {
		parseAnd ();   // like not not not
		newparse (NOT_);
		return;
	}
	oldread;
	parseNot ();
}

static void parseOr () {
	parseAnd ();
	if (newread == AND_) {
		int falseLabel = newlabel;
		int andLabel = newlabel;
		do {
			newparse (IFFALSE_); parselabel (falseLabel);
			parseAnd ();
		} while (newread == AND_);
		newparse (IFFALSE_); parselabel (falseLabel);
		newparse (TRUE_);
		newparse (GOTO_); parselabel (andLabel);
		newparse (LABEL_); parselabel (falseLabel);
		newparse (FALSE_);
		newparse (LABEL_); parselabel (andLabel);
	}
	oldread;
}

static void parseExpression () {
	parseOr ();
	if (newread == OR_) {
		int trueLabel = newlabel;
		int orLabel = newlabel;
		do {
			newparse (IFTRUE_); parselabel (trueLabel);
			parseOr ();
		} while (newread == OR_);
		newparse (IFTRUE_); parselabel (trueLabel);
		newparse (FALSE_);
		newparse (GOTO_); parselabel (orLabel);
		newparse (LABEL_); parselabel (trueLabel);
		newparse (TRUE_);
		newparse (LABEL_); parselabel (orLabel);
	}
	oldread;
}

/*
	Translate the infix expression "my lexan" into the postfix expression "my parse":
	remove parentheses and brackets, commas, colons, FROM_, TO_,
	IF_ THEN_ ELSE_ ENDIF_ OR_ AND_;
	introduce LABEL_ GOTO_ IFTRUE_ IFFALSE_ TRUE_ FALSE_
	SELF0_ SELF1_ SELF2_ MATRIX0_ MATRIX1_ MATRIX2_
	Return:
		0 if error, otherwise 1.
	Precondition:
		"my lexan" contains an END_ symbol.
	Postconditions:
		*my lexan not changed.
		result == 0 || my parse [my numberOfInstructions]. symbol == END_
*/

static void Formula_parseExpression () {
	ilabel = ilexan = iparse = 0;
	if (lexan [1]. symbol == END_) Melder_throw (U"Empty formula.");
	parseExpression ();
	fit (END_);
	newparse (END_);
	numberOfInstructions = iparse;
}

static void shift (int begin, int distance) {
	numberOfInstructions -= distance;
	for (int j = begin; j <= numberOfInstructions; j ++)
		parse [j] = parse [j + distance];
}

static int findLabel (int label) {
	int result = numberOfInstructions;
	while (parse [result]. symbol != LABEL_ ||
			 parse [result]. content.label != label)
		result --;
	return result;
}

static void Formula_optimizeFlow ()
/* Vereenvoudig boolse uitdrukkingen.					*/
/* Nadien:								*/
/*    de stroom volgt het kortste pad;					*/
/*    als de rekenkundige waarden van boolse uitdrukkingen		*/
/*    in de formule niet voorkomen, zijn alle TRUE_s en FALSE_s weg;	*/
/*    als in de formule geen NOT_s voorkwamen op rekenkundige		*/
/*    uitdrukkingen, zijn alle NOT_s weg;				*/
/*    onbereikbare kode is weg;						*/
{
	int i, j, volg;
	for (;;) {
		bool improved = false;
		for (i = 1; i <= numberOfInstructions; i ++)
		{

/* Optimalisatie 1: */
/*    true   goto x  ->  goto y  /  __  ...  label x  iftrue y    */
/*    false  goto x  ->  goto y  /  __  ...  label x  iffalse y   */

			if ((parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_)
				 ||
				 (parse [i]. symbol == FALSE_ &&
				  parse [i + 1]. symbol == GOTO_ &&
				  parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_))
			{
				 improved = true;
				 parse [i]. symbol = GOTO_;
				 parse [i]. content.label = parse [volg]. content.label;
				 shift (i + 1, 1);
			}

/* Optimalisatie 2: */
/*    true  goto x  ...  label x  iffalse y  ->        */
/*          goto z  ...  label x  iffalse y  label z   */
/*    en analoog met false en iftrue. */

			if ((parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_)
				 ||
				 (parse [i]. symbol == FALSE_ &&
				  parse [i + 1]. symbol == GOTO_ &&
				  parse [volg = findLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_))
			{
				improved = true;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = newlabel;
				for (j = i + 1; j < volg; j ++)
					parse [j] = parse [j + 1];
				parse [volg]. symbol = LABEL_;
				parse [volg]. content.label = ilabel;
			}

/* Optimalisatie 3a: */
/*    iftrue x  goto y  label x  ->  iffalse y  label x   */

			if (parse [i]. symbol == IFTRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [i + 2]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 2]. content.label)
			{
				improved = true;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 3b: */
/*    iffalse x  goto y  label x  ->  iftrue y  label x   */

			if (parse [i]. symbol == IFFALSE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [i + 2]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 2]. content.label)
			{
				improved = true;
				parse [i]. symbol = IFTRUE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 4: */
/*    verwijder onbereikbare kode: na een GOTO_ hoort een LABEL_. */

			if (parse [i]. symbol == GOTO_ &&
				 parse [i + 1]. symbol != LABEL_)
			{
				improved = true;
				j = i + 2;
				while (parse [j]. symbol != LABEL_) j ++;
				shift (i + 1, j - i - 1);
			}

/* Optimalisatie 5: */
/*    goto x  ->  0  /  __  label x   */

			if (parse [i]. symbol == GOTO_ &&
				 parse [i]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 1]. content.label)
			{
				improved = true;
				shift (i, 1);
			}

/* Optimalisatie 6: */
/*    true   iffalse x  ->  0  */
/*    false  iftrue x   ->  0  */

			if ((parse [i]. symbol == TRUE_ && parse [i + 1]. symbol == IFFALSE_)
				|| (parse [i]. symbol == FALSE_ && parse [i + 1]. symbol == IFTRUE_))
			{
				improved = true;
				shift (i, 2);
			}

/* Optimalisatie 7: */
/*    true   iftrue x   ->  goto x    */
/*    false  iffalse x  ->  goto x    */
			
			if ((parse [i]. symbol == TRUE_ && parse [i + 1]. symbol == IFTRUE_)
				|| (parse [i]. symbol == FALSE_ && parse [i + 1]. symbol == IFFALSE_))
			{
				improved = true;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 8: */
/*    iftrue x   ->  iftrue y   /  __  ...  label x  goto y   */
/*    iffalse x  ->  iffalse y  /  __  ...  label x  goto y   */

			if ((parse [i]. symbol == IFTRUE_ || parse [i]. symbol == IFFALSE_)
				&& parse [volg = findLabel (parse [i]. content.label) + 1]. symbol == GOTO_)
			{
				improved = true;
				parse [i]. content.label = parse [volg]. content.label;
			}

/* Optimalisatie 9a: */
/*    not  iftrue x  ->  iffalse x   */

			if (parse [i]. symbol == NOT_ && parse [i + 1]. symbol == IFTRUE_)
			{
				improved = true;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* Optimalisatie 9b: */
/*    not  iffalse x  ->  iftrue x   */

			if (parse [i]. symbol == NOT_ && parse [i + 1]. symbol == IFFALSE_)
			{
				improved = true;
				parse [i]. symbol = IFTRUE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				shift (i + 1, 1);
			}

/* De volgende optimalisaties ontbreken want zijn hier overbodig: */
/*    goto x  ->  goto y  /  __  ... label x  goto y   */
/*    trek twee opeenvolgende labels samen             */

		} /* for i */

		/* Verwijder labels waar niet naar verwezen wordt. */

		for (i = 1; i <= numberOfInstructions; i ++)
			if (parse [i]. symbol == LABEL_)
			{
				int gevonden = 0;
				for (j = 1; j <= numberOfInstructions; j ++)
					if ((parse [j]. symbol == GOTO_ || parse [j]. symbol == IFFALSE_ || parse [j]. symbol == IFTRUE_
						|| parse [j]. symbol == INCREMENT_GREATER_GOTO_)
						&& parse [i]. content.label == parse [j]. content.label)
						gevonden = 1;
				if (! gevonden)
				{
					improved = true;
					shift (i, 1);
				}
			}
		if (! improved) break;
	}
}

static int praat_findObjectById (integer id) {
	int IOBJECT;
	WHERE_DOWN (ID == id)
		return IOBJECT;
	Melder_throw (U"No object with number ", id, U".");
}

static int praat_findObjectByName (conststring32 name) {
	int IOBJECT;
	if (*name >= U'A' && *name <= U'Z') {
		static MelderString buffer { };
		MelderString_copy (& buffer, name);
		char32 *spaceLocation = str32chr (buffer.string, U' ');
		if (! spaceLocation)
			Melder_throw (U"Missing space in object name \"", name, U"\".");
		*spaceLocation = U'\0';
		conststring32 className = & buffer.string [0], givenName = spaceLocation + 1;
		WHERE_DOWN (1) {
			Daata object = OBJECT;
			if (str32equ (className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
				return IOBJECT;
		}
		ClassInfo klas = Thing_classFromClassName (className, nullptr);
		WHERE_DOWN (1) {
			Daata object = OBJECT;
			if (str32equ (klas -> className, Thing_className (OBJECT)) && str32equ (givenName, object -> name.get()))
				return IOBJECT;
		}
	}
	Melder_throw (U"No object with name \"", name, U"\".");
}

static void Formula_evaluateConstants () {
	for (;;) {
		bool improved = false;
		for (int i = 1; i <= numberOfInstructions; i ++) {
			int gain = 0;
			if (parse [i]. symbol == NUMBER_) {
				if (parse [i]. content.number == 2.0 && parse [i + 1]. symbol == POWER_)
					{ gain = 1; parse [i]. symbol = SQR_; }
				else if (parse [i + 1]. symbol == MINUS_)
					{ gain = 1; parse [i]. content.number = - parse [i]. content.number; } 
				else if (parse [i + 1]. symbol == SQR_)
					{ gain = 1; parse [i]. content.number *= parse [i]. content.number; }
				else if (parse [i + 1]. symbol == NUMBER_) {
					if (parse [i + 2]. symbol == ADD_)
						{ gain = 2; parse [i]. content.number += parse [i + 1]. content.number; }
					else if (parse [i + 2]. symbol == SUB_)
						{ gain = 2; parse [i]. content.number -= parse [i + 1]. content.number; }
					else if (parse [i + 2]. symbol == MUL_)
						{ gain = 2; parse [i]. content.number *= parse [i + 1]. content.number; }
					else if (parse [i + 2]. symbol == RDIV_)
						{ gain = 2; parse [i]. content.number /= parse [i + 1]. content.number; }
				} else if (parse [i + 1]. symbol == TO_OBJECT_) {
					parse [i]. symbol = OBJECT_;
					int IOBJECT = praat_findObjectById (Melder_iround (parse [i]. content.number));
					parse [i]. content.object = OBJECT;
					gain = 1;
				}
			} else if (parse [i]. symbol == STRING_) {
				if (parse [i + 1]. symbol == TO_OBJECT_) {
					parse [i]. symbol = OBJECT_;
					int IOBJECT = praat_findObjectByName (parse [i]. content.string);
					parse [i]. content.object = OBJECT;
					gain = 1;
				}
			} else if (parse [i]. symbol == NUMERIC_VARIABLE_) {
				parse [i]. symbol = NUMBER_;
				parse [i]. content.number = parse [i]. content.variable -> numericValue;
				gain = 0;
				improved = true;
			} else if (parse [i]. symbol == STRING_VARIABLE_) {
				parse [i]. symbol = STRING_;
				parse [i]. content.string = parse [i]. content.variable -> stringValue.get();   // again a reference copy (lexan is still the owner)
				gain = 0;
				improved = true;
			#if 0
			} else if (parse [i]. symbol == ROW_) {
				if (parse [i + 1]. symbol == COL_ && parse [i + 2]. symbol == SELFMATRIX2_)
					{ gain = 2; parse [i]. symbol = SELF0_; }   // TODO: SELF0_ may not have the same restrictions as SELFMATRIX2_
			} else if (parse [i]. symbol == COL_) {
				if (parse [i + 1]. symbol == SELFMATRIX1_)
					{ gain = 1; parse [i]. symbol = SELF0_; }
			#endif
			}
			if (gain > 0) {
				improved = true;
				shift (i + 1, gain);
			}
		}
		if (! improved) break;
	}
}

static void Formula_removeLabels () {
	/*
	 * First translate symbolic labels (< 0) into instructions locations (> 0).
	 */
	for (int i = 1; i <= numberOfInstructions; i ++) {
		int symboli = parse [i]. symbol;
		if (symboli == GOTO_ || symboli == IFTRUE_ || symboli == IFFALSE_ || symboli == INCREMENT_GREATER_GOTO_) {
			int label = parse [i]. content.label;
			for (int j = 1; j <= numberOfInstructions; j ++) {
				if (parse [j]. symbol == LABEL_ && parse [j]. content.label == label) {
					parse [i]. content.label = j;
				}
			}
		}
	}
	/*
		Then remove the labels,
		which have become superfluous.
	*/
	if (theOptimize) {
		int i = 1;
		while (i <= numberOfInstructions) {
			int symboli = parse [i]. symbol;
			if (symboli == LABEL_) {
				shift (i, 1);   // remove one label
				for (int j = 1; j <= numberOfInstructions; j ++) {
					int symbolj = parse [j]. symbol;
					if ((symbolj == GOTO_ || symbolj == IFTRUE_ || symbolj == IFFALSE_ || symbolj == INCREMENT_GREATER_GOTO_) && parse [j]. content.label > i)
						parse [j]. content.label --;  /* Pas een label aan. */
				}
				i --;   // voorkom ophogen i (overbodig?)
			}
			i ++;
		}
	}
	numberOfInstructions --;   /* Het END_-symbol hoeft niet geinterpreteerd. */
}

#include <inttypes.h>

/*
 * For debugging.
 */
static void Formula_print (FormulaInstruction f) {
	int i = 0, symbol;
	do {
		conststring32 instructionName;
		symbol = f [++ i]. symbol;
		instructionName = Formula_instructionNames [symbol];
		if (symbol == NUMBER_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.number);
		else if (symbol == GOTO_ || symbol == IFFALSE_ || symbol == IFTRUE_ || symbol == LABEL_ || symbol == INCREMENT_GREATER_GOTO_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.label);
		else if (symbol == NUMERIC_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.variable -> string.get(), U" ", f [i]. content.variable -> numericValue);
		else if (symbol == STRING_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.variable -> string.get(), U" ", f [i]. content.variable -> stringValue.get());
		else if (symbol == STRING_ || symbol == VARIABLE_NAME_ || symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.string);
		else if (symbol == MATRIX_ || symbol == MATRIXSTR_ || symbol == MATRIX1_ || symbol == MATRIXSTR1_ ||
		         symbol == MATRIX2_ || symbol == MATRIXSTR2_ || symbol == ROWSTR_ || symbol == COLSTR_)
		{
			Thing object = f [i]. content.object;
			if (object) {
				Melder_casual (i, U" ", instructionName, U" ", Thing_className (object), U" ", object -> name.get());
			} else {
				Melder_casual (i, U" ", instructionName);
			}
		}
		else if (symbol == CALL_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.string);
		else
			Melder_casual (i, U" ", instructionName);
	} while (symbol != END_);
}

void Formula_compile (Interpreter interpreter, Daata data, conststring32 expression, int expressionType, bool optimize) {
	theInterpreter = interpreter;
	if (! theInterpreter) {
		if (! theLocalInterpreter)
			theLocalInterpreter = Interpreter_create (nullptr, nullptr);
		theInterpreter = theLocalInterpreter.get();
		theInterpreter -> variablesMap. clear ();
	}
	theSource = data;
	theExpression = expression;
	theExpressionType [theLevel] = expressionType;
	theOptimize = optimize;
	if (! lexan) {
		lexan = Melder_calloc_f (struct structFormulaInstruction, 3000);
		lexan [3000 - 1]. symbol = END_;   // make sure that cleaning up always terminates
	}
	if (! parse)
		parse = Melder_calloc_f (struct structFormulaInstruction, 3000);

	/*
		Clean up strings from the previous call.
		These strings are in a union, that's why this cannot be done later, when a new string is created.
	*/
	if (numberOfStringConstants) {
		ilexan = 1;
		for (;;) {
			int symbol = lexan [ilexan]. symbol;
			if (symbol == STRING_ ||
				symbol == VARIABLE_NAME_ ||
				symbol == INDEXED_NUMERIC_VARIABLE_ ||
				symbol == INDEXED_STRING_VARIABLE_ ||
				symbol == CALL_
			) {
				Melder_free (lexan [ilexan]. content.string);
			}
			else if (symbol == END_) break;   // either the end of a formula, or the end of lexan
			ilexan ++;
		}
		numberOfStringConstants = 0;
	}

	Formula_lexan ();
	if (Melder_debug == 17) Formula_print (lexan);
	Formula_parseExpression ();
	if (Melder_debug == 17) Formula_print (parse);
	if (theOptimize) {
		Formula_optimizeFlow ();
		if (Melder_debug == 17) Formula_print (parse);
		Formula_evaluateConstants ();
		if (Melder_debug == 17) Formula_print (parse);
	}
	Formula_removeLabels ();
	if (Melder_debug == 17) Formula_print (parse);
}

/*
	Running.
*/

conststring32 structStackel :: whichText () {
	return
		our which == Stackel_NUMBER ? U"a number" :
		our which == Stackel_NUMERIC_VECTOR ? U"a numeric vector" :
		our which == Stackel_NUMERIC_MATRIX ? U"a numeric matrix" :
		our which == Stackel_STRING ? U"a string" :
		our which == Stackel_STRING_ARRAY ? U"a string array" :
		our which == Stackel_OBJECT ? U"an object" :
		U"???";
}

static int programPointer;

#define Formula_MAXIMUM_STACK_SIZE  1000

static Stackel theStack;
static integer w, wmax;   /* w = stack pointer; */
#define pop  & theStack [w --]
#define topOfStack  & theStack [w]
inline static void pushNumber (double x) {
	/* inline runs 10 to 20 percent faster; here's the test script:
		stopwatch
		Create Sound from formula: "test", 1, 0.0, 1000.0, 44100, ~ x + 1 + 2 + 3 + 4 + 5 + 6
		writeInfoLine: stopwatch
		Remove
	 * Mac: 3.76 -> 3.20 seconds
	 */
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_NUMBER;
	stackel -> number = isdefined (x) ? x : undefined;
	//stackel -> number = x;   // this one would be 2 percent faster
	//stackel -> owned = true;
}
static void pushNumericVector (autoVEC x) {
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_NUMERIC_VECTOR;
	stackel -> numericVector = x.releaseToAmbiguousOwner();
	stackel -> owned = true;
}
static void pushNumericVectorReference (VEC x) {
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_NUMERIC_VECTOR;
	stackel -> numericVector = x;
	stackel -> owned = false;
}
static void pushNumericMatrix (autoMAT x) {
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_NUMERIC_MATRIX;
	stackel -> numericMatrix = x.releaseToAmbiguousOwner();
	stackel -> owned = true;
}
static void pushNumericMatrixReference (MAT x) {
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_NUMERIC_MATRIX;
	stackel -> numericMatrix = x;
	stackel -> owned = false;
}
static void pushString (autostring32 x) {
	Stackel stackel = & theStack [++ w];
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> setString (x.move());
	//stackel -> owned = true;
}
static void pushObject (Daata object) {
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_OBJECT;
	stackel -> object = object;
	//stackel -> owned = false;
}
static void pushVariable (InterpreterVariable var) {
	Stackel stackel = & theStack [++ w];
	stackel -> reset();
	if (w > wmax) {
		wmax ++;
		if (wmax > Formula_MAXIMUM_STACK_SIZE)
			Melder_throw (U"Formula: stack overflow. Please simplify your formulas.");
	}
	stackel -> which = Stackel_VARIABLE;
	stackel -> variable = var;
	//stackel -> owned = false;
}

static void do_not () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number == 0.0 ? 1.0 : 0.0);
	} else {
		Melder_throw (U"Cannot negate (\"not\") ", x->whichText(), U".");
	}
}
static void do_eq () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		/*
			It is possible that we are comparing a value against --undefined--.
			Any defined value is unequal to --undefined--,
			but any undefined value (inf or NaN) *is* equal to --undefined--.
			Note that this is different from how "==" works in C.
		*/
		pushNumber (NUMequal (x->number, y->number) ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		pushNumber (str32equ (x->getString(), y->getString()) ? 1.0 : 0.0);
	} else if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMequal (x->numericVector, y->numericVector) ? 1.0 : 0.0);
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMequal (x->numericMatrix, y->numericMatrix) ? 1.0 : 0.0);
	} else {
		Melder_throw (U"Cannot compare (=) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_ne () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		/*
			Unequal is defined as the opposite of equal.
		*/
		pushNumber (NUMequal (x->number, y->number) ? 0.0 : 1.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		pushNumber (str32equ (x->getString(), y->getString()) ? 0.0 : 1.0);
	} else if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMequal (x->numericVector, y->numericVector) ? 0.0 : 1.0);
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMequal (x->numericMatrix, y->numericMatrix) ? 0.0 : 1.0);
	} else {
		Melder_throw (U"Cannot compare (<>) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_le () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number <= y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not equal to, nor less than, undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not equal to, nor less than, defined
			} else {
				pushNumber (1.0);   // undefined is equal to undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = str32cmp (x->getString(), y->getString()) <= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<=) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_lt () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number < y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not less than undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not less than defined
			} else {
				pushNumber (0.0);   // undefined is not less than undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = str32cmp (x->getString(), y->getString()) < 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_ge () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number >= y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not equal to, nor greater than, undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not equal to, nor greater than, defined
			} else {
				pushNumber (1.0);   // undefined is equal to undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = str32cmp (x->getString(), y->getString()) >= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (>=) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_gt () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number > y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not greater than undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not greater than defined
			} else {
				pushNumber (0.0);   // undefined is not greater than undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = str32cmp (x->getString(), y->getString()) > 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (>) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
inline static void moveNumericVector (Stackel from, Stackel to) {
	//Melder_assert (from -> owned);
	//Melder_assert (to -> which == Stackel_NUMERIC_VECTOR);
	from -> owned = false;
	to -> numericVector = from -> numericVector;
	to -> owned = true;
}
inline static void moveNumericMatrix (Stackel from, Stackel to) {
	//Melder_assert (from -> owned);
	//Melder_assert (to -> which == Stackel_NUMERIC_MATRIX);
	from -> owned = false;
	to -> numericMatrix = from -> numericMatrix;
	to -> owned = true;
}

/**
	result.. = x.. + y..
*/
static void do_add () {
	Stackel y = pop, x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		if (y->which == Stackel_NUMBER) {
			/*@praat
				#
				# result = x + y
				#
				x = 5
				y = 6
				result = x + y
				assert result = 11
			@*/
			x->number += y->number;
			//x->which = Stackel_NUMBER;   // superfluous, as is cleaning up
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x + y#
			*/
			if (y->owned) {
				/*@praat
					#
					# result# = x + owned y#
					#
					result# = 5 + { 11, 13, 31 }   ; numeric vector literals are owned
					assert result# = { 16, 18, 36 }
				@*/
				y->numericVector  +=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = x + unowned y#
					#
					y# = { 17, -11, 29 }
					result# = 30 + y#   ; numeric vector variables are not owned
					assert result# = { 47, 19, 59 }
				@*/
				// x does not have to be cleaned up, because it was a number
				x->numericVector = newVECadd (y->numericVector, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_VECTOR;
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x + y##
			*/
			if (y->owned) {
				y->numericMatrix  +=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was a number
				x->numericMatrix = newMATadd (y->numericMatrix, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x# + y#
				i.e.
				result# [i] = x# [i] + y# [i]
			*/
			integer nx = x->numericVector.size, ny = y->numericVector.size;
			if (nx != ny) {
				/*@praat
					#
					# Error: unequal sizes.
					#
					x# = { 11, 13, 17 }
					y# = { 8, 90 }
					asserterror When adding vectors, their numbers of elements should be equal, instead of 3 and 2.
					result# = x# + y#
				@*/
				Melder_throw (U"When adding vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
			}
			if (x -> owned) {
				/*@praat
					#
					# result# = owned x# + y#
					#
					result# = { 11, 13, 17 } + { 44, 56, 67 }   ; owned + owned
					assert result# = { 55, 69, 84 }
					y# = { 3, 2, 89.5 }
					result# = { 11, 13, 17 } + y#   ; owned + unowned
					assert result# = { 14, 15, 106.5 }
				@*/
				x->numericVector  +=  y->numericVector;
			} else if (y -> owned) {
				/*@praat
					#
					# result# = unowned x# + owned y#
					#
					x# = { 14, -3, 6.25 }
					result# = x# + { 55, 1, -89 }
					assert result# = { 69, -2, -82.75 }
				@*/
				y->numericVector  +=  x->numericVector;
				// x does not have to be cleaned up, because it was not owned
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = unowned x# + unowned y#
					#
					x# = { 14, -33, 6.25 }
					y# = { -33, 17, 9 }
					result# = x# + y#
					assert result# = { -19, -16, 15.25 }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = newVECadd (x->numericVector, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x# + y##
				i.e.
				result## [i, j] = x# [i] + y## [i, j]
			*/
			integer xsize = x->numericVector.size;
			integer ynrow = y->numericMatrix.nrow;
			Melder_require (ynrow == xsize,
				U"When adding a matrix to a vector, the matrix’s number of rows should be equal to the vector’s size, "
				"instead of ", ynrow, U" and ", xsize, U"."
			);
			if (x->owned) {
				/*@praat
					assert { 1, 2, 3 } + { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 2, 3 }, { 5, 6 }, { 8, 9 } }
				@*/
				autoMAT newMatrix = newMATadd (x->numericVector, y->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix. releaseToAmbiguousOwner();
			} else {
				/*@praat
					a# = { 1, 2, 3 }
					assert a# + { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 2, 3 }, { 5, 6 }, { 8, 9 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATadd (x->numericVector, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# + y
				i.e.
				result# [i] = x# [i] + y
			*/
			if (x->owned) {
				x->numericVector  +=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = newVECadd (x->numericVector, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x## + y##
				i.e.
				result## [i, j] = x## [i, j] + y## [i, j]
			*/
			integer xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
			integer ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
			if (xnrow != ynrow)
				Melder_throw (U"When adding matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
			if (xncol != yncol)
				Melder_throw (U"When adding matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
			if (x->owned) {
				x->numericMatrix  +=  y->numericMatrix;
			} else if (y->owned) {
				y->numericMatrix  +=  x->numericMatrix;
				// x does not have to be cleaned up, because it was not owned
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATadd (x->numericMatrix, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result## = x## + y#
				i.e.
				result## [i, j] = x## [i, j] + y# [j]
			*/
			Melder_require (y->numericVector.size == x->numericMatrix.ncol,
				U"Cannot add a vector with ", y->numericVector.size, U" elements "
				"to a matrix with ", x->numericMatrix.ncol, U" columns. "
				"These numbers should be equal."
			);
			if (x->owned) {
				/*@praat
					#
					# result## = owned x## + y#
					#
					y# = { -5, 6, -19 }
					result## = { { 14, -33, 6.25 }, { -33, 17, 9 } } + y#
					assert result## = { { 9, -27, -12.75 }, { -38, 23, -10 } }
				@*/
				x->numericMatrix  +=  y->numericVector;
			} else {
				/*@praat
					#
					# result## = unowned x## + y#
					#
					x## = { { 14, -33, 6.25 }, { -33, 17, 9 } }
					y# = { -5, 6, -19 }
					result## = x## + y#
					assert result## = { { 9, -27, -12.75 }, { -38, 23, -10 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATadd (x->numericMatrix, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result## = x## + y
				i.e.
				result## [i, j] = x## [i, j] + y
			*/
			if (x->owned) {
				x->numericMatrix  +=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATadd (x->numericMatrix, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		/*
			result$ = x$ + y$
		*/
		integer length1 = str32len (x->getString()), length2 = str32len (y->getString());
		autostring32 result (length1 + length2);
		str32cpy (result.get(), x->getString());
		str32cpy (result.get() + length1, y->getString());
		x->setString (result.move());
		return;
	}
	Melder_throw (U"Cannot add ", y->whichText(), U" to ", x->whichText(), U".");
}
static void do_sub () {
	/*
		result.. = x.. - y..
	*/
	Stackel y = pop, x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		if (y->which == Stackel_NUMBER) {
			/*
				result = x - y
			*/
			x->number -= y->number;
			//x->which = Stackel_NUMBER;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x - y#
			*/
			if (y->owned) {
				y->numericVector <<= x->number  -  y->numericVector;
				moveNumericVector (y, x);
			} else {
				x->numericVector = newVECsubtract (x->number, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_VECTOR;
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x - y##
			*/
			if (y->owned) {
				MATsubtractReversed_inplace (y->numericMatrix, x->number);
				moveNumericMatrix (y, x);
			} else {
				x->numericMatrix = newMATsubtract (x->number, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x# - y#
				i.e.
				result# [i] = x# [i] - y# [i]
			*/
			integer nx = x->numericVector.size, ny = y->numericVector.size;
			if (nx != ny)
				Melder_throw (U"When subtracting vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
			if (x -> owned) {
				x->numericVector  -=  y->numericVector;
			} else if (y -> owned) {
				y->numericVector <<= x->numericVector  -  y->numericVector;
				moveNumericVector (y, x);
			} else {
				// no clean-up of x required, because x is not owned and has the right type
				x->numericVector = newVECsubtract (x->numericVector, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# - y
				i.e.
				result# [i] = x# [i] - y
			*/
			if (x->owned) {
				x->numericVector  -=  y->number;
			} else {
				x->numericVector = newVECsubtract (x->numericVector, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (y->which == Stackel_NUMERIC_MATRIX) {
			integer xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
			integer ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
			if (xnrow != ynrow)
				Melder_throw (U"When subtracting matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
			if (xncol != yncol)
				Melder_throw (U"When subtracting matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
			if (x->owned) {
				x->numericMatrix  -=  y->numericMatrix;
			} else if (y->owned) {
				MATsubtractReversed_inplace (y->numericMatrix, x->numericMatrix);
				moveNumericMatrix (y, x);
			} else {
				// no clean-up of x required, because x is not owned and has the right type
				x->numericMatrix = newMATsubtract (x->numericMatrix, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMBER) {
			if (x->owned) {
				x->numericMatrix  -=  y->number;
			} else {
				x->numericMatrix = newMATsubtract (x->numericMatrix, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		integer length1 = str32len (x->getString()), length2 = str32len (y->getString()), newlength = length1 - length2;
		autostring32 result;
		if (newlength >= 0 && str32nequ (x->getString() + newlength, y->getString(), length2)) {
			result = autostring32 (newlength);
			str32ncpy (result.get(), x->getString(), newlength);
		} else {
			result = Melder_dup (x->getString());
		}
		x->setString (result.move());
		return;
	}
	Melder_throw (U"Cannot subtract (-) ", y->whichText(), U" from ", x->whichText(), U".");
}
static void do_mul () {
	/*
		result.. = x.. * y..
	*/
	Stackel y = pop, x = topOfStack;
	if (x->which == Stackel_NUMBER) {
		if (y->which == Stackel_NUMBER) {
			/*@praat
				#
				# result = x * y
				#
				x = 5
				y = 6
				result = x * y
				assert result = 30
			@*/
			x->number *= y->number;
			//x->which = Stackel_NUMBER;   // superfluous, as is cleaning up
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x * y#
			*/
			if (y->owned) {
				/*@praat
					#
					# result# = x * owned y#
					#
					result# = 5 * { 11, 13, 31 }   ; numeric vector literals are owned
					assert result# = { 55, 65, 155 }
				@*/
				y->numericVector  *=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = x * unowned y#
					#
					y# = { 17, -11, 29 }
					result# = 30 * y#   ; numeric vector variables are not owned
					assert result# = { 510, -330, 870 }
				@*/
				// x does not have to be cleaned up, because it was a number
				x->numericVector = newVECmultiply (y->numericVector, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_VECTOR;
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x * y##
			*/
			if (y->owned) {
				y->numericMatrix  *=  x->number;
				// x does not have to be cleaned up, because it was a number
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was a number
				x->numericMatrix = newMATmultiply (y->numericMatrix, x->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x# * y#
				i.e.
				result# [i] = x# [i] * y# [i]
			*/
			integer nx = x->numericVector.size, ny = y->numericVector.size;
			if (nx != ny) {
				/*@praat
					#
					# Error: unequal sizes.
					#
					x# = { 11, 13, 17 }
					y# = { 8, 90 }
					asserterror When multiplying vectors, their numbers of elements should be equal, instead of 3 and 2.
					result# = x# * y#
				@*/
				Melder_throw (U"When multiplying vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
			}
			if (x -> owned) {
				/*@praat
					#
					# result# = owned x# * y#
					#
					result# = { 11, 13, 17 } * { 44, 56, 67 }   ; owned * owned
					assert result# = { 484, 728, 1139 }
					y# = { 3, 2, 89.5 }
					result# = { 11, 13, 17 } * y#   ; owned * unowned
					assert result# = { 33, 26, 1521.5 }
				@*/
				x->numericVector  *=  y->numericVector;
			} else if (y -> owned) {
				/*@praat
					#
					# result# = unowned x# * owned y#
					#
					x# = { 14, -3, 6.25 }
					result# = x# * { 55, 1, -89 }
					assert result# = { 770, -3, -556.25 }
				@*/
				y->numericVector  *=  x->numericVector;
				// x does not have to be cleaned up, because it was not owned
				moveNumericVector (y, x);
			} else {
				/*@praat
					#
					# result# = unowned x# * unowned y#
					#
					x# = { 14, -33, 6.25 }
					y# = { -33, 17, 9 }
					result# = x# * y#
					assert result# = { -462, -561, 56.25 }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = newVECmultiply (x->numericVector, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x# * y##
				i.e.
				result## [i, j] = x# [i] * y## [i, j]
			*/
			integer xsize = x->numericVector.size;
			integer ynrow = y->numericMatrix.nrow;
			Melder_require (ynrow == xsize,
				U"When multiplying a vector with a matrix, the matrix’s number of rows should be equal to the vector’s size, "
				"instead of ", ynrow, U" and ", xsize, U"."
			);
			if (x->owned) {
				/*@praat
					assert { 1, 2, 3 } * { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 1, 2 }, { 6, 8 }, { 15, 18 } }
				@*/
				autoMAT newMatrix = newMATmultiply (x->numericVector, y->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix. releaseToAmbiguousOwner();
			} else {
				/*@praat
					a# = { 1, 2, 3 }
					assert a# * { { 1, 2 }, { 3, 4 }, { 5, 6 } } = { { 1, 2 }, { 6, 8 }, { 15, 18 } }
				@*/
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATmultiply (x->numericVector, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# * y
				i.e.
				result# [i] = x# [i] * y
			*/
			if (x->owned) {
				x->numericVector  *=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericVector = newVECmultiply (x->numericVector, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_VECTOR;   // superfluous
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x## * y##
				i.e.
				result## [i, j] = x## [i, j] * y## [i, j]
			*/
			integer xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
			integer ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
			if (xnrow != ynrow)
				Melder_throw (U"When multiplying matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
			if (xncol != yncol)
				Melder_throw (U"When multiplying matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
			if (x->owned) {
				x->numericMatrix  *=  y->numericMatrix;
			} else if (y->owned) {
				y->numericMatrix  *=  x->numericMatrix;
				// x does not have to be cleaned up, because it was not owned
				moveNumericMatrix (y, x);
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATmultiply (x->numericMatrix, y->numericMatrix). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result## = x## * y#
				i.e.
				result## [i, j] = x## [i, j] * y# [j]
			*/
			integer xncol = x->numericMatrix.ncol;
			integer ysize = y->numericVector.size;
			Melder_require (xncol == ysize,
				U"When multiplying a matrix with a vector, the vector’s size should be equal to the matrix’s number of columns, "
				"instead of ", ysize, U" and ", xncol, U"."
			);
			if (x->owned) {
				x->numericMatrix  *=  y->numericVector;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATmultiply (x->numericMatrix, y->numericVector). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result## = x## * y
				i.e.
				result## [i, j] = x## [i, j] * y
			*/
			if (x->owned) {
				x->numericMatrix  *=  y->number;
			} else {
				// x does not have to be cleaned up, because it was not owned
				x->numericMatrix = newMATmultiply (x->numericMatrix, y->number). releaseToAmbiguousOwner();
				x->owned = true;
			}
			//x->which = Stackel_NUMERIC_MATRIX;   // superfluous
			return;
		}
	}
	Melder_throw (U"Cannot multiply (*) ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_rdiv () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->number / y->number);   // result could be inf (1/0) or NaN (0/0), which is OK
		return;
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			integer nelem1 = x->numericVector.size, nelem2 = y->numericVector.size;
			if (nelem1 != nelem2)
				Melder_throw (U"When dividing vectors, their numbers of elements should be equal, instead of ", nelem1, U" and ", nelem2, U".");
			autoVEC result { nelem1, kTensorInitializationType::RAW };
			for (integer ielem = 1; ielem <= nelem1; ielem ++)
				result [ielem] = x->numericVector [ielem] / y->numericVector [ielem];
			pushNumericVector (result.move());
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# / y
			*/
			integer xn = x->numericVector.size;
			autoVEC result { xn, kTensorInitializationType::RAW };
			double yvalue = y->number;
			if (yvalue == 0.0) {
				Melder_throw (U"Cannot divide (/) ", x->whichText(), U" by zero.");
			} else {
				for (integer i = 1; i <= xn; i ++) {
					double xvalue = x->numericVector [i];
					result [i] = xvalue / yvalue;
				}
			}
			pushNumericVector (result.move());
			return;
		}
	}
	Melder_throw (U"Cannot divide (/) ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_idiv () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (floor (x->number / y->number));
		return;
	}
	Melder_throw (U"Cannot divide (\"div\") ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_mod () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->number - floor (x->number / y->number) * y->number);
		return;
	}
	Melder_throw (U"Cannot divide (\"mod\") ", x->whichText(), U" by ", y->whichText(), U".");
}
static void do_minus () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (- x->number);
	} else {
		Melder_throw (U"Cannot take the opposite (-) of ", x->whichText(), U".");
	}
}
static void do_power () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : pow (x->number, y->number));
	} else if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMBER) {
		/*@praat
			assert { 3, 4 } ^ 3 = { 27, 64 }
			assert { 3, -4 } ^ 3 = { 27, -64 }
			assert { -4 } ^ 2.3 = { undefined }
		@*/
		pushNumericVector (newVECpower (x->numericVector, y->number));
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMBER) {
		pushNumericMatrix (newMATpower (x->numericMatrix, y->number));
	} else {
		Melder_throw (U"Cannot exponentiate (^) ", x->whichText(), U" to ", y->whichText(), U".");
	}
}
static void do_sqr () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number * x->number);
	} else if (x->which == Stackel_NUMERIC_VECTOR) {
		/*@praat
			a# = zero# (10)
			a# ~ sum ({ 3, 4 } ^ 2)
			assert sum (a#) = 250
			a# ~ sum ({ col } ^ 2)
			assert sum (a#) = 385
		@*/
		pushNumericVector (newVECpower (x->numericVector, 2.0));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumericMatrix (newMATpower (x->numericMatrix, 2.0));
	} else {
		Melder_throw (U"Cannot take the square (^ 2) of ", x->whichText(), U".");
	}
}
static void do_function_n_n (double (*f) (double)) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : f (x->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric argument, not ", x->whichText(), U".");
	}
}
static void do_functionvec_n_n (double (*f) (double)) {
	Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		integer n = x->numericVector.size;
		double *at = x->numericVector.at;
		if (x->owned) {
			for (integer i = 1; i <= n; i ++)
				at [i] = f (at [i]);
		} else {
			autoVEC result { n, kTensorInitializationType::RAW };
			for (integer i = 1; i <= n; i ++)
				result [i] = f (at [i]);
			x->numericVector = result. releaseToAmbiguousOwner();
			x->owned = true;
		}
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", x->whichText(), U".");
	}
}
static void do_softmaxH () {
	Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (! x->owned) {
			x->numericVector = newVECcopy (x->numericVector). releaseToAmbiguousOwner();   // TODO: no need to copy
			x->owned = true;
		}
		integer nelm = x->numericVector.size;
		double maximum = -1e308;
		for (integer i = 1; i <= nelm; i ++) {
			if (x->numericVector [i] > maximum)
				maximum = x->numericVector [i];
		}
		for (integer i = 1; i <= nelm; i ++)
			x->numericVector [i] -= maximum;
		longdouble sum = 0.0;
		for (integer i = 1; i <= nelm; i ++) {
			x->numericVector [i] = exp (x->numericVector [i]);
			sum += x->numericVector [i];
		}
		for (integer i = 1; i <= nelm; i ++)
			x->numericVector [i] /= (double) sum;
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", x->whichText(), U".");
	}
}
static void do_softmaxPerRowHH () {
	Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (! x->owned) {
			x->numericMatrix = newMATcopy (x->numericMatrix). releaseToAmbiguousOwner();   // TODO: no need to copy
			x->owned = true;
		}
		integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol;
		for (integer irow = 1; irow <= nrow; irow ++) {
			double maximum = -1e308;
			for (integer icol = 1; icol <= ncol; icol ++) {
				if (x->numericMatrix [irow] [icol] > maximum)
					maximum = x->numericMatrix [irow] [icol];
			}
			for (integer icol = 1; icol <= ncol; icol ++)
				x->numericMatrix [irow] [icol] -= maximum;
			longdouble sum = 0.0;
			for (integer icol = 1; icol <= ncol; icol ++) {
				x->numericMatrix [irow] [icol] = exp (x->numericMatrix [irow] [icol]);
				sum += x->numericMatrix [irow] [icol];
			}
			for (integer icol = 1; icol <= ncol; icol ++)
				x->numericMatrix [irow] [icol] /= (double) sum;
		}
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric matrix argument, not ", x->whichText(), U".");
	}
}
static void do_abs () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : fabs (x->number));
	} else {
		Melder_throw (U"Cannot take the absolute value (abs) of ", x->whichText(), U".");
	}
}
static void do_round () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : floor (x->number + 0.5));
	} else {
		Melder_throw (U"Cannot round ", x->whichText(), U".");
	}
}
static void do_floor () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : Melder_roundDown (x->number));
	} else {
		Melder_throw (U"Cannot round down (floor) ", x->whichText(), U".");
	}
}
static void do_ceiling () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : Melder_roundUp (x->number));
	} else {
		Melder_throw (U"Cannot round up (ceiling) ", x->whichText(), U".");
	}
}
static void do_rectify () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number > 0.0 ? x->number : 0.0);
	} else {
		Melder_throw (U"Cannot rectify ", x->whichText(), U".");
	}
}
static void do_rectifyH () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		integer nelm = x->numericVector.size;
		autoVEC result { nelm, kTensorInitializationType::RAW };
		for (integer i = 1; i <= nelm; i ++) {
			double xvalue = x->numericVector [i];
			result [i] = isundef (xvalue) ? undefined : xvalue > 0.0 ? xvalue : 0.0;
		}
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"Cannot rectify ", x->whichText(), U".");
	}
}
static void do_rectifyHH () {
	Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (x->owned) {
			integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol;
			for (integer irow = 1; irow <= nrow; irow ++) {
				for (integer icol = 1; icol <= ncol; icol ++) {
					double xvalue = x->numericMatrix [irow] [icol];
					x->numericMatrix [irow] [icol] = isundef (xvalue) ? undefined : xvalue > 0.0 ? xvalue : 0.0;
				}
			}
		} else {
			pop;
			integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol;
			autoMAT result = newMATraw (nrow, ncol);
			for (integer irow = 1; irow <= nrow; irow ++) {
				for (integer icol = 1; icol <= ncol; icol ++) {
					double xvalue = x->numericMatrix [irow] [icol];
					result [irow] [icol] = isundef (xvalue) ? undefined : xvalue > 0.0 ? xvalue : 0.0;
				}
			}
			pushNumericMatrix (result.move());
		}
	} else {
		Melder_throw (U"Cannot rectify ", x->whichText(), U".");
	}
}
static void do_sqrt () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number < 0.0 ? undefined : sqrt (x->number));
	} else {
		Melder_throw (U"Cannot take the square root (sqrt) of ", x->whichText(), U".");
	}
}
static void do_sin () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : sin (x->number));
	} else {
		Melder_throw (U"Cannot take the sine (sin) of ", x->whichText(), U".");
	}
}
static void do_cos () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : cos (x->number));
	} else {
		Melder_throw (U"Cannot take the cosine (cos) of ", x->whichText(), U".");
	}
}
static void do_tan () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : tan (x->number));
	} else {
		Melder_throw (U"Cannot take the tangent (tan) of ", x->whichText(), U".");
	}
}
static void do_arcsin () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			fabs (x->number) > 1.0 ? undefined : asin (x->number));
	} else {
		Melder_throw (U"Cannot take the arcsine (arcsin) of ", x->whichText(), U".");
	}
}
static void do_arccos () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			fabs (x->number) > 1.0 ? undefined : acos (x->number));
	} else {
		Melder_throw (U"Cannot take the arccosine (arccos) of ", x->whichText(), U".");
	}
}
static void do_arctan () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : atan (x->number));
	} else {
		Melder_throw (U"Cannot take the arctangent (arctan) of ", x->whichText(), U".");
	}
}
static void do_exp () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : exp (x->number));
	} else {
		Melder_throw (U"Cannot exponentiate (exp) ", x->whichText(), U".");
	}
}
static void do_VECexp () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		integer nelm = x->numericVector.size;
		autoVEC result (nelm, kTensorInitializationType::RAW);
		for (integer i = 1; i <= nelm; i ++) {
			result [i] = exp (x->numericVector [i]);
		}
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"Cannot exponentiate (exp) ", x->whichText(), U".");
	}
}
static void do_MATexp () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		integer nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol;
		autoMAT result (nrow, ncol, kTensorInitializationType::RAW);
		for (integer irow = 1; irow <= nrow; irow ++) {
			for (integer icol = 1; icol <= ncol; icol ++) {
				result [irow] [icol] = exp (x->numericMatrix [irow] [icol]);
			}
		}
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"Cannot exponentiate (exp) ", x->whichText(), U".");
	}
}
static void do_sinh () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : sinh (x->number));
	} else {
		Melder_throw (U"Cannot take the hyperbolic sine (sinh) of ", x->whichText(), U".");
	}
}
static void do_cosh () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : cosh (x->number));
	} else {
		Melder_throw (U"Cannot take the hyperbolic cosine (cosh) of ", x->whichText(), U".");
	}
}
static void do_tanh () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : tanh (x->number));
	} else {
		Melder_throw (U"Cannot take the hyperbolic tangent (tanh) of ", x->whichText(), U".");
	}
}
static void do_log2 () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number <= 0.0 ? undefined : log (x->number) * NUMlog2e);
	} else {
		Melder_throw (U"Cannot take the base-2 logarithm (log2) of ", x->whichText(), U".");
	}
}
static void do_ln () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number <= 0.0 ? undefined : log (x->number));
	} else {
		Melder_throw (U"Cannot take the natural logarithm (ln) of ", x->whichText(), U".");
	}
}
static void do_log10 () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number <= 0.0 ? undefined : log10 (x->number));
	} else {
		Melder_throw (U"Cannot take the base-10 logarithm (log10) of ", x->whichText(), U".");
	}
}
static void do_sum () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMsum (x->numericVector));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMsum (x->numericMatrix));
	} else {
		Melder_throw (U"Cannot compute the sum of ", x->whichText(), U".");
	}
}
static void do_mean () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMmean (x->numericVector));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMmean (x->numericMatrix));
	} else {
		Melder_throw (U"Cannot compute the mean of ", x->whichText(), U".");
	}
}
static void do_stdev () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMstdev (x->numericVector));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMstdev (x->numericMatrix));
	} else {
		Melder_throw (U"Cannot compute the standard deviation of ", x->whichText(), U".");
	}
}
static void do_center () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMcenterOfGravity (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute the center of ", x->whichText(), U".");
	}
}
static void do_function_dd_d (double (*f) (double, double)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : f (x->number, y->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_VECdd_d (double (*f) (double, double)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if ((a->which == Stackel_NUMERIC_VECTOR || a->which == Stackel_NUMBER) && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		integer numberOfElements = ( a->which == Stackel_NUMBER ? Melder_iround (a->number) : a->numericVector.size );
		autoVEC newData (numberOfElements, kTensorInitializationType::RAW);
		for (integer ielem = 1; ielem <= numberOfElements; ielem ++) {
			newData [ielem] = f (x->number, y->number);
		}
		pushNumericVector (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires either three numeric arguments, or one vector argument and two numeric arguments, not ",
			a->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_MATdd_d (double (*f) (double, double)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number == 3) {
		Stackel y = pop, x = pop, model = pop;
		if (model->which == Stackel_NUMERIC_MATRIX && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
			integer numberOfRows = model->numericMatrix.nrow;
			integer numberOfColumns = model->numericMatrix.ncol;
			autoMAT newData (numberOfRows, numberOfColumns, kTensorInitializationType::RAW);
			for (integer irow = 1; irow <= numberOfRows; irow ++)
				for (integer icol = 1; icol <= numberOfColumns; icol ++)
					newData [irow] [icol] = f (x->number, y->number);
			pushNumericMatrix (newData.move());
		} else {
			Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
				U" requires one matrix argument and two numeric arguments, not ",
				model->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
		}
	} else if (n -> number == 4) {
		Stackel y = pop, x = pop, ncol = pop, nrow = pop;
		if (nrow->which == Stackel_NUMBER && ncol->which == Stackel_NUMBER && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
			integer numberOfRows = Melder_iround (nrow->number);
			integer numberOfColumns = Melder_iround (ncol->number);
			autoMAT newData (numberOfRows, numberOfColumns, kTensorInitializationType::RAW);
			for (integer irow = 1; irow <= numberOfRows; irow ++)
				for (integer icol = 1; icol <= numberOfColumns; icol ++)
					newData [irow] [icol] = f (x->number, y->number);
			pushNumericMatrix (newData.move());
		} else {
			Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
				U" requires four numeric arguments, not ",
				nrow->whichText(), U", ", ncol->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
		}
	} else
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three or four arguments.");
}
static void do_function_VECll_l (integer (*f) (integer, integer)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if ((a->which == Stackel_NUMERIC_VECTOR || a->which == Stackel_NUMBER) && x->which == Stackel_NUMBER) {
		integer numberOfElements = ( a->which == Stackel_NUMBER ? Melder_iround (a->number) : a->numericVector.size );
		autoVEC newData (numberOfElements, kTensorInitializationType::RAW);
		for (integer ielem = 1; ielem <= numberOfElements; ielem ++) {
			newData [ielem] = f (Melder_iround (x->number), Melder_iround (y->number));
		}
		pushNumericVector (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires either three numeric arguments, or one vector argument and two numeric arguments, not ",
			a->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_MATll_l (integer (*f) (integer, integer)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if (a->which == Stackel_NUMERIC_MATRIX && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		integer numberOfRows = a->numericMatrix.nrow;
		integer numberOfColumns = a->numericMatrix.ncol;
		autoMAT newData (numberOfRows, numberOfColumns, kTensorInitializationType::RAW);
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			for (integer icol = 1; icol <= numberOfColumns; icol ++) {
				newData [irow] [icol] = f (Melder_iround (x->number), Melder_iround (y->number));
			}
		}
		pushNumericMatrix (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires one matrix argument and two numeric arguments, not ",
			a->whichText(), U", ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_dl_d (double (*f) (double, integer)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (x->number, Melder_iround (y->number)));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_ld_d (double (*f) (integer, double)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (Melder_iround (x->number), y->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_ll_l (integer (*f) (integer, integer)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (Melder_iround (x->number), Melder_iround (y->number)));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_objects_are_identical () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		integer id1 = Melder_iround (x->number), id2 = Melder_iround (y->number);
		integer i = theCurrentPraatObjects -> n;
		while (i > 0 && id1 != theCurrentPraatObjects -> list [i]. id) i --;
		if (i == 0) Melder_throw (U"Object #", id1, U" does not exist in function objectsAreIdentical.");
		Daata object1 = (Daata) theCurrentPraatObjects -> list [i]. object;
		i = theCurrentPraatObjects -> n;
		while (i > 0 && id2 != theCurrentPraatObjects -> list [i]. id) i --;
		if (i == 0) Melder_throw (U"Object #", id2, U" does not exist in function objectsAreIdentical.");
		Daata object2 = (Daata) theCurrentPraatObjects -> list [i]. object;
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : Data_equal (object1, object2));
	} else {
		Melder_throw (U"The function objectsAreIdentical requires two numeric arguments (object IDs), not ",
			x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_function_ddd_d (double (*f) (double, double, double)) {
	Stackel z = pop, y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER && z->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) || isundef (z->number) ? undefined :
			f (x->number, y->number, z->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires three numeric arguments, not ", x->whichText(), U", ",
			y->whichText(), U", and ", z->whichText(), U".");
	}
}
static void do_do () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number < 1)
		Melder_throw (U"The function \"do\" requires at least one argument, namely a menu command.");
	integer numberOfArguments = Melder_iround (narg->number) - 1;
	#define MAXNUM_FIELDS  40
	structStackel stack [1+MAXNUM_FIELDS];
	for (integer iarg = numberOfArguments; iarg >= 0; iarg --) {
		Stackel arg = pop;
		stack [iarg] = std::move (*arg);
	}
	if (stack [0]. which != Stackel_STRING)
		Melder_throw (U"The first argument of the function \"do\" should be a string, namely a menu command, and not ", stack [0]. whichText(), U".");
	conststring32 command = stack [0]. getString();
	if (theCurrentPraatObjects == & theForegroundPraatObjects && praatP. editor != nullptr) {
		autoMelderString valueString;
		MelderString_appendCharacter (& valueString, 1);   // TODO: check whether this is needed at all, or is just MelderString_empty enough?
		autoMelderDivertInfo divert (& valueString);
		autostring32 command2 = Melder_dup (command);   // allow the menu command to reuse the stack (?)
		Editor_doMenuCommand (praatP. editor, command2.get(), numberOfArguments, & stack [0], nullptr, theInterpreter);
		pushNumber (Melder_atof (valueString.string));
		return;
	} else if (theCurrentPraatObjects != & theForegroundPraatObjects &&
		(str32nequ (command, U"Save ", 5) || str32nequ (command, U"Write ", 6) || str32nequ (command, U"Append ", 7) || str32equ (command, U"Quit")))
	{
		Melder_throw (U"Commands that write files (including Quit) are not available inside manuals.");
	} else {
		autoMelderString valueString;
		MelderString_appendCharacter (& valueString, 1);   // a semaphor to check whether praat_doAction or praat_doMenuCommand wrote anything with MelderInfo
		autoMelderDivertInfo divert (& valueString);
		autostring32 command2 = Melder_dup (command);   // allow the menu command to reuse the stack (?)
		if (! praat_doAction (command2.get(), numberOfArguments, & stack [0], theInterpreter) &&
		    ! praat_doMenuCommand (command2.get(), numberOfArguments, & stack [0], theInterpreter))
		{
			Melder_throw (U"Command \"", command, U"\" not available for current selection.");
		}
		//praat_updateSelection ();
		double value = undefined;
		if (valueString.string [0] == 1) {   // nothing written with MelderInfo by praat_doAction or praat_doMenuCommand? then the return value is the ID of the selected object
			int IOBJECT, result = 0, found = 0;
			WHERE (SELECTED) {
				result = IOBJECT;
				found += 1;
			}
			if (found == 1)
				value = theCurrentPraatObjects -> list [result]. id;
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
		pushNumber (value);
		return;
	}
	praat_updateSelection ();   // BUG: superfluous? flickering?
	pushNumber (1);
}
static void do_evaluate () {
	Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		double result;
		Interpreter_numericExpression (theInterpreter, expression->getString(), & result);
		pushNumber (result);
	} else Melder_throw (U"The argument of the function \"evaluate\" should be a string with a numeric expression, not ", expression->whichText());
}
static void do_evaluate_nocheck () {
	Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		try {
			double result;
			Interpreter_numericExpression (theInterpreter, expression->getString(), & result);
			pushNumber (result);
		} catch (MelderError) {
			Melder_clearError ();
			pushNumber (undefined);
		}
	} else Melder_throw (U"The argument of the function \"evaluate_nocheck\" should be a string with a numeric expression, not ", expression->whichText());
}
static void do_evaluateStr () {
	Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		autostring32 result = Interpreter_stringExpression (theInterpreter, expression->getString());
		pushString (result.move());
	} else Melder_throw (U"The argument of the function \"evaluate$\" should be a string with a string expression, not ", expression->whichText());
}
static void do_evaluate_nocheckStr () {
	Stackel expression = pop;
	if (expression->which == Stackel_STRING) {
		try {
			autostring32 result = Interpreter_stringExpression (theInterpreter, expression->getString());
			pushString (result.move());
		} catch (MelderError) {
			Melder_clearError ();
			pushString (Melder_dup (U""));
		}
	} else Melder_throw (U"The argument of the function \"evaluate_nocheck$\" should be a string with a string expression, not ", expression->whichText());
}
static void do_doStr () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number < 1)
		Melder_throw (U"The function \"do$\" requires at least one argument, namely a menu command.");
	integer numberOfArguments = Melder_iround (narg->number) - 1;
	#define MAXNUM_FIELDS  40
	structStackel stack [1+MAXNUM_FIELDS];
	for (integer iarg = numberOfArguments; iarg >= 0; iarg --) {
		Stackel arg = pop;
		stack [iarg] = std::move (*arg);
	}
	if (stack [0]. which != Stackel_STRING)
		Melder_throw (U"The first argument of the function \"do$\" should be a string, namely a menu command, and not ", stack [0]. whichText(), U".");
	conststring32 command = stack [0]. getString();
	if (theCurrentPraatObjects == & theForegroundPraatObjects && praatP. editor != nullptr) {
		static MelderString info;
		MelderString_empty (& info);
		autoMelderDivertInfo divert (& info);
		autostring32 command2 = Melder_dup (command);
		Editor_doMenuCommand (praatP. editor, command2.get(), numberOfArguments, & stack [0], nullptr, theInterpreter);
		pushString (Melder_dup (info.string));
		return;
	} else if (theCurrentPraatObjects != & theForegroundPraatObjects &&
		(str32nequ (command, U"Save ", 5) || str32nequ (command, U"Write ", 6) || str32nequ (command, U"Append ", 7) || str32equ (command, U"Quit")))
	{
		Melder_throw (U"Commands that write files (including Quit) are not available inside manuals.");
	} else {
		static MelderString info;
		MelderString_empty (& info);
		autoMelderDivertInfo divert (& info);
		autostring32 command2 = Melder_dup (command);
		if (! praat_doAction (command2.get(), numberOfArguments, & stack [0], theInterpreter) &&
		    ! praat_doMenuCommand (command2.get(), numberOfArguments, & stack [0], theInterpreter))
		{
			Melder_throw (U"Command \"", command, U"\" not available for current selection.");
		}
		praat_updateSelection ();
		pushString (Melder_dup (info.string));
		return;
	}
	praat_updateSelection ();   // BUG: superfluous? flickering?
	pushString (Melder_dup (U""));
}
static void shared_do_writeInfo (integer numberOfArguments) {
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER) {
			MelderInfo_write (arg->number);
		} else if (arg->which == Stackel_STRING) {
			MelderInfo_write (arg->getString());
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			for (integer i = 1; i <= arg->numericVector.size; i ++)
				MelderInfo_write (arg->numericVector [i],
						i == arg->numericVector.size ? U"" : U" ");
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++) {
				for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++) {
					MelderInfo_write (arg->numericMatrix [irow] [icol],
							icol == arg->numericMatrix.ncol ? U"" : U" ");
				}
				MelderInfo_write (irow == arg->numericMatrix.nrow ? U"" : U"\n");
			}
		}
	}
}
static void do_writeInfo () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	MelderInfo_open ();
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_writeInfoLine () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	MelderInfo_open ();
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_write (U"\n");
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_appendInfo () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_appendInfoLine () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_write (U"\n");
	MelderInfo_drain ();
	pushNumber (1);
}
static void shared_do_writeFile (autoMelderString *text, integer numberOfArguments) {
	for (int iarg = 2; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER) {
			MelderString_append (text, arg->number);
		} else if (arg->which == Stackel_STRING) {
			MelderString_append (text, arg->getString());
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			for (integer i = 1; i <= arg->numericVector.size; i ++)
				MelderString_append (text, arg->numericVector [i],
						i == arg->numericVector.size ? U"" : U" ");
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			for (integer irow = 1; irow <= arg->numericMatrix.nrow; irow ++) {
				for (integer icol = 1; icol <= arg->numericMatrix.ncol; icol ++) {
					MelderString_append (text, arg->numericMatrix [irow] [icol],
							icol == arg->numericMatrix.ncol ? U"" : U" ");
				}
				MelderString_append (text, irow == arg->numericMatrix.nrow ? U"" : U"\n");
			}
		}
	}
}
static void do_writeFile () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"writeFile\" should be a string (a file name), not ", fileName->whichText(), U".");
	}
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> getString(), & file);
	MelderFile_writeText (& file, text.string, Melder_getOutputEncoding ());
	pushNumber (1);
}
static void do_writeFileLine () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"writeFileLine\" should be a string (a file name), not ", fileName->whichText(), U".");
	}
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	MelderString_appendCharacter (& text, U'\n');
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> getString(), & file);
	MelderFile_writeText (& file, text.string, Melder_getOutputEncoding ());
	pushNumber (1);
}
static void do_appendFile () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"appendFile\" should be a string (a file name), not ", fileName->whichText(), U".");
	}
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> getString(), & file);
	MelderFile_appendText (& file, text.string);
	pushNumber (1);
}
static void do_appendFileLine () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"appendFileLine\" should be a string (a file name), not ", fileName->whichText(), U".");
	}
	autoMelderString text;
	shared_do_writeFile (& text, numberOfArguments);
	MelderString_appendCharacter (& text, '\n');
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> getString(), & file);
	MelderFile_appendText (& file, text.string);
	pushNumber (1);
}
static void do_pauseScript () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"pause\" is not available inside manuals.");
	if (theCurrentPraatApplication -> batch) return;   // in batch we ignore pause statements
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	autoMelderString buffer;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& buffer, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& buffer, arg->getString());
	}
	UiPause_begin (theCurrentPraatApplication -> topShell, U"stop or continue", theInterpreter);
	UiPause_comment (numberOfArguments == 0 ? U"..." : buffer.string);
	UiPause_end (1, 1, 0, U"Continue", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, theInterpreter);
	pushNumber (1);
}
static void do_exitScript () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			Melder_appendError_noLine (arg->number);
		else if (arg->which == Stackel_STRING)
			Melder_appendError_noLine (arg->getString());
	}
	Melder_throw (U"\nScript exited.");
	pushNumber (1);
}
static void do_runScript () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	if (numberOfArguments < 1)
		Melder_throw (U"The function \"runScript\" requires at least one argument, namely the file name.");
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName->which != Stackel_STRING)
		Melder_throw (U"The first argument to \"runScript\" should be a string (the file name), not ", fileName->whichText());
	theLevel += 1;
	try {
		praat_executeScriptFromFileName (fileName->getString(), numberOfArguments - 1, & theStack [w + 1]);
		theLevel -= 1;
	} catch (MelderError) {
		theLevel -= 1;
		throw;
	}
	pushNumber (1);
}
static void do_runSystem () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"runSystem\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	autoMelderString text;
	for (integer iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->getString());
	}
	try {
		Melder_system (text.string);
	} catch (MelderError) {
		Melder_throw (U"System command \"", text.string, U"\" returned error status;\n"
			U"if you want to ignore this, use `runSystem_nocheck' instead of `runSystem'.");
	}
	pushNumber (1);
}
static void do_runSystem_nocheck () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"runSystem\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	autoMelderString text;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->getString());
	}
	try {
		Melder_system (text.string);
	} catch (MelderError) {
		Melder_clearError ();
	}
	pushNumber (1);
}
static void do_runSubprocess () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"runSubprocess\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	integer numberOfArguments = Melder_iround (narg->number);
	w -= numberOfArguments;
	Stackel commandFile = & theStack [w + 1];
	if (commandFile->which != Stackel_STRING)
		Melder_throw (U"The first argument to \"runSubprocess\" should be a command name.");
	autostring32vector arguments (numberOfArguments - 1);
	for (int iarg = 1; iarg < numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + 1 + iarg];
		if (arg->which == Stackel_NUMBER)
			arguments [iarg] = Melder_dup (Melder_double (arg->number));
		else if (arg->which == Stackel_STRING)
			arguments [iarg] = Melder_dup (arg->getString());
	}
	try {
		Melder_execv (commandFile->getString(), numberOfArguments - 1, arguments.peek2());
	} catch (MelderError) {
		Melder_throw (U"Command \"", commandFile->getString(), U"\" returned error status.");
	}
	pushNumber (1);
}
static void do_min () {
	Stackel n = pop, last;
	double result;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number < 1)
		Melder_throw (U"The function \"min\" requires at least one argument.");
	last = pop;
	if (last->which != Stackel_NUMBER)
		Melder_throw (U"The function \"min\" can only have numeric arguments, not ", last->whichText(), U".");
	result = last->number;
	for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			Melder_throw (U"The function \"min\" can only have numeric arguments, not ", previous->whichText(), U".");
		result = isundef (result) || isundef (previous->number) ? undefined :
			result < previous->number ? result : previous->number;
	}
	pushNumber (result);
}
static void do_max () {
	Stackel n = pop, last;
	double result;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number < 1)
		Melder_throw (U"The function \"max\" requires at least one argument.");
	last = pop;
	if (last->which != Stackel_NUMBER)
		Melder_throw (U"The function \"max\" can only have numeric arguments, not ", last->whichText(), U".");
	result = last->number;
	for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			Melder_throw (U"The function \"max\" can only have numeric arguments, not ", previous->whichText(), U".");
		result = isundef (result) || isundef (previous->number) ? undefined :
			result > previous->number ? result : previous->number;
	}
	pushNumber (result);
}
static void do_imin () {
	Stackel n = pop, last;
	double minimum, result;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number < 1)
		Melder_throw (U"The function \"imin\" requires at least one argument.");
	last = pop;
	if (last->which != Stackel_NUMBER)
		Melder_throw (U"The function \"imin\" can only have numeric arguments, not ", last->whichText(), U".");
	minimum = last->number;
	result = n->number;
	for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			Melder_throw (U"The function \"imin\" can only have numeric arguments, not ", previous->whichText(), U".");
		if (isundef (minimum) || isundef (previous->number)) {
			minimum = undefined;
			result = undefined;
		} else if (previous->number < minimum) {
			minimum = previous->number;
			result = j;
		}
	}
	pushNumber (result);
}
static void do_imax () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number < 1)
		Melder_throw (U"The function \"imax\" requires at least one argument.");
	Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		double maximum = last->number;
		double result = n->number;
		for (integer j = Melder_iround (n->number) - 1; j > 0; j --) {
			Stackel previous = pop;
			if (previous->which != Stackel_NUMBER)
				Melder_throw (U"The function \"imax\" cannot mix a numeric argument with ", previous->whichText(), U".");
			if (isundef (maximum) || isundef (previous->number)) {
				maximum = undefined;
				result = undefined;
			} else if (previous->number > maximum) {
				maximum = previous->number;
				result = j;
			}
		}
		pushNumber (result);
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		if (n->number != 1)
			Melder_throw (U"The function \"imax\" requires exactly one vector argument.");
		integer numberOfElements = last->numericVector.size;
		integer result = 1;
		double maximum = last->numericVector [1];
		for (integer i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] > maximum) {
				result = i;
				maximum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Stackel nn = pop;
		Melder_throw (U"Cannot compute the imax of ", nn->whichText(), U".");
	}
}
static void do_norm () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number < 1 || n->number > 2)
		Melder_throw (U"The function \"norm\" requires one or two arguments.");
	double powerNumber = 2.0;
	if (n->number == 2) {
		Stackel power = pop;
		if (power->which != Stackel_NUMBER)
			Melder_throw (U"The second argument to \"norm\" should be a number, not ", power->whichText(), U".");
		powerNumber = power->number;
	}
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMnorm (x->numericVector, powerNumber));
	} else if (x->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (NUMnorm (x->numericMatrix, powerNumber));
	} else {
		Melder_throw (U"Cannot compute the norm of ", x->whichText(), U".");
	}
}
static void do_VECzero () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	integer rank = Melder_iround (n -> number);
	if (rank < 1)
		Melder_throw (U"The function \"zero#\" requires an argument.");
	if (rank > 1) {
		Melder_throw (U"The function \"zero#\" cannot have more than one argument (consider using zero##).");
	}
	Stackel nelem = pop;
	if (nelem -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"zero#\", the number of elements should be a number, not ", nelem->whichText(), U".");
	double numberOfElements = nelem -> number;
	if (isundef (numberOfElements))
		Melder_throw (U"In the function \"zero#\", the number of elements is undefined.");
	if (numberOfElements < 0.0)
		Melder_throw (U"In the function \"zero#\", the number of elements should not be negative.");
	pushNumericVector (newVECzero (Melder_iround (numberOfElements)));
}
static void do_MATzero () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	integer rank = Melder_iround (n -> number);
	if (rank != 2)
		Melder_throw (U"The function \"zero##\" requires two arguments.");
	Stackel ncol = pop;
	if (ncol -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"zero##\", the number of columns should be a number, not ", ncol->whichText(), U".");
	double numberOfColumns = ncol -> number;
	Stackel nrow = pop;
	if (nrow -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"zero##\", the number of rows should be a number, not ", nrow->whichText(), U".");
	double numberOfRows = nrow -> number;
	if (isundef (numberOfRows))
		Melder_throw (U"In the function \"zero##\", the number of rows is undefined.");
	if (isundef (numberOfColumns))
		Melder_throw (U"In the function \"zero##\", the number of columns is undefined.");
	if (numberOfRows < 0.0)
		Melder_throw (U"In the function \"zero##\", the number of rows should not be negative.");
	if (numberOfColumns < 0.0)
		Melder_throw (U"In the function \"zero##\", the number of columns should not be negative.");
	autoMAT result = newMATzero (Melder_iround (numberOfRows), Melder_iround (numberOfColumns));
	pushNumericMatrix (result.move());
}
static void do_VEClinear () {
	Stackel stackel_narg = pop;
	Melder_assert (stackel_narg -> which == Stackel_NUMBER);
	integer narg = Melder_iround (stackel_narg -> number);
	if (narg < 3 || narg > 4)
		Melder_throw (U"The function \"linear#\" requires three or four arguments.");
	bool excludeEdges = false;   // default
	if (narg == 4) {
		Stackel stack_excludeEdges = pop;
		if (stack_excludeEdges -> which != Stackel_NUMBER)
			Melder_throw (U"In the function \"linear#\", the edge exclusion flag (fourth argument) should be a number, not ", stack_excludeEdges->whichText(), U".");
		excludeEdges = Melder_iround (stack_excludeEdges -> number);
	}
	Stackel stack_numberOfSteps = pop, stack_maximum = pop, stack_minimum = pop;
	if (stack_minimum -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"linear#\", the minimum (first argument) should be a number, not ", stack_minimum->whichText(), U".");
	double minimum = stack_minimum -> number;
	if (isundef (minimum))
		Melder_throw (U"Undefined minimum in the function \"linear#\" (first argument).");
	if (stack_maximum -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"linear#\", the maximum (second argument) should be a number, not ", stack_maximum->whichText(), U".");
	double maximum = stack_maximum -> number;
	if (isundef (maximum))
		Melder_throw (U"Undefined maximum in the function \"linear#\" (second argument).");
	if (maximum < minimum)
		Melder_throw (U"Maximum (", maximum, U") smaller than minimum (", minimum, U") in function \"linear#\".");
	if (stack_numberOfSteps -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"linear#\", the number of steps (third argument) should be a number, not ", stack_numberOfSteps->whichText(), U".");
	if (isundef (stack_numberOfSteps -> number))
		Melder_throw (U"Undefined number of steps in the function \"linear#\" (third argument).");
	integer numberOfSteps = Melder_iround (stack_numberOfSteps -> number);
	if (numberOfSteps <= 0)
		Melder_throw (U"In the function \"linear#\", the number of steps (third argument) should be positive, not ", numberOfSteps, U".");
	autoVEC result = newVECraw (numberOfSteps);
	for (integer ielem = 1; ielem <= numberOfSteps; ielem ++) {
		result [ielem] = excludeEdges ?
			minimum + (ielem - 0.5) * (maximum - minimum) / numberOfSteps :
			minimum + (ielem - 1) * (maximum - minimum) / (numberOfSteps - 1);
	}
	if (! excludeEdges) result [numberOfSteps] = maximum;   // remove rounding problems
	pushNumericVector (result.move());
}
static void do_VECto () {
	Stackel stackel_narg = pop;
	Melder_assert (stackel_narg -> which == Stackel_NUMBER);
	integer narg = (integer) stackel_narg -> number;
	if (narg != 1)
		Melder_throw (U"The function to#() requires one argument.");
	Stackel stack_to = pop;
	if (stack_to -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"to#\", the argument should be a number, not ", stack_to->whichText(), U".");
	autoVEC result = newVECto (stack_to -> number);
	pushNumericVector (result.move());
}
static void do_VECfrom_to () {
	Stackel stackel_narg = pop;
	Melder_assert (stackel_narg -> which == Stackel_NUMBER);
	integer narg = (integer) stackel_narg -> number;
	if (narg != 2)
		Melder_throw (U"The function from_to#() requires two arguments.");
	Stackel stack_to = pop, stack_from = pop;
	if (stack_from -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"from_to#\", the first argument should be a number, not ", stack_from->whichText(), U".");
	if (stack_to -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"from_to#\", the second argument should be a number, not ", stack_to->whichText(), U".");
	autoVEC result = newVECfrom_to (stack_from -> number, stack_to -> number);
	pushNumericVector (result.move());
}
static void do_VECfrom_to_by () {
	Stackel stackel_narg = pop;
	Melder_assert (stackel_narg -> which == Stackel_NUMBER);
	integer narg = (integer) stackel_narg -> number;
	if (narg != 3)
		Melder_throw (U"The function from_to_by#() requires three arguments.");
	Stackel stack_by = pop, stack_to = pop, stack_from = pop;
	if (stack_from -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"from_to_by#\", the first argument should be a number, not ", stack_from->whichText(), U".");
	if (stack_to -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"from_to_by#\", the second argument should be a number, not ", stack_to->whichText(), U".");
	if (stack_by -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"from_to_by#\", the third argument should be a number, not ", stack_by->whichText(), U".");
	autoVEC result = newVECfrom_to_by_left (stack_from -> number, stack_to -> number, stack_by -> number);
	pushNumericVector (result.move());
}
static void do_MATpeaks () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number != 4)
		Melder_throw (U"The function peaks## requires four arguments (vector, edges, interpolation, sortByHeight).");
	Stackel s = pop;
	if (s->which != Stackel_NUMBER)
		Melder_throw (U"The fourth argument to peaks## should be a number, not ", s->whichText(), U".");
	bool sortByHeight = s->number != 0.0;
	Stackel i = pop;
	if (i->which != Stackel_NUMBER)
		Melder_throw (U"The third argument to peaks## should be a number, not ", i->whichText(), U".");
	integer interpolation = Melder_iround (i->number);
	Stackel e = pop;
	if (e->which != Stackel_NUMBER)
		Melder_throw (U"The second argument to peaks## should be a number, not ", e->whichText(), U".");
	bool includeEdges = e->number != 0.0;
	Stackel vec = pop;
	if (vec->which != Stackel_NUMERIC_VECTOR)
		Melder_throw (U"The first argument to peaks## should be a numeric vector, not ", vec->whichText(), U".");
	autoMAT result = newMATpeaks (vec->numericVector, includeEdges, interpolation, sortByHeight);
	pushNumericMatrix (result.move());
}
static void do_size () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number != 1)
		Melder_throw (U"The function \"size\" requires one (vector) argument.");
	Stackel array = pop;
	if (array->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (array->numericVector.size);
	} else {
		Melder_throw (U"The function size requires a vector argument, not ", array->whichText(), U".");
	}
}
static void do_numberOfRows () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number != 1)
		Melder_throw (U"The function \"numberOfRows\" requires one argument.");
	Stackel array = pop;
	if (array->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (array->numericMatrix.nrow);
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a matrix argument, not ", array->whichText(), U".");
	}
}
static void do_numberOfColumns () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number != 1)
		Melder_throw (U"The function \"numberOfColumns\" requires one argument.");
	Stackel array = pop;
	if (array->which == Stackel_NUMERIC_MATRIX) {
		pushNumber (array->numericMatrix.ncol);
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a matrix argument, not ", array->whichText(), U".");
	}
}
static void do_editor () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number == 0) {
		if (theInterpreter && theInterpreter -> editorClass) {
			praatP. editor = praat_findEditorFromString (theInterpreter -> environmentName.get());
		} else {
			Melder_throw (U"The function \"editor\" requires an argument when called from outside an editor.");
		}
	} else if (n->number == 1) {
		Stackel editor = pop;
		if (editor->which == Stackel_STRING) {
			praatP. editor = praat_findEditorFromString (editor->getString());
		} else if (editor->which == Stackel_NUMBER) {
			praatP. editor = praat_findEditorById (Melder_iround (editor->number));
		} else {
			Melder_throw (U"The function \"editor\" requires a numeric or string argument, not ", editor->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"editor\" requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushNumber (1);
}

static void do_hash () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			double result = NUMhashString (s->getString());
			pushNumber (result);
		} else {
			Melder_throw (U"The function \"hash\" requires a string, not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"hash\" requires 1 argument, not ", n->number, U".");
	}
}

static void do_numericVectorElement () {
	InterpreterVariable vector = parse [programPointer]. content.variable;
	integer element = 1;   // default
	Stackel r = pop;
	if (r -> which != Stackel_NUMBER)
		Melder_throw (U"In vector indexing, the index should be a number, not ", r->whichText(), U".");
	if (isundef (r -> number))
		Melder_throw (U"The element index is undefined.");
	element = Melder_iround (r -> number);
	if (element <= 0)
		Melder_throw (U"In vector indexing, the element index should be positive.");
	if (element > vector -> numericVectorValue.size)
		Melder_throw (U"Element index out of bounds.");
	pushNumber (vector -> numericVectorValue [element]);
}
static void do_numericMatrixElement () {
	InterpreterVariable matrix = parse [programPointer]. content.variable;
	integer row = 1, column = 1;   // default
	Stackel c = pop;
	if (c -> which != Stackel_NUMBER)
		Melder_throw (U"In matrix indexing, the column index should be a number, not ", c->whichText(), U".");
	if (isundef (c -> number))
		Melder_throw (U"The column index is undefined.");
	column = Melder_iround (c -> number);
	if (column <= 0)
		Melder_throw (U"In matrix indexing, the column index should be positive.");
	if (column > matrix -> numericMatrixValue. ncol)
		Melder_throw (U"Column index out of bounds.");
	Stackel r = pop;
	if (r -> which != Stackel_NUMBER)
		Melder_throw (U"In matrix indexing, the row index should be a number, not ", r->whichText(), U".");
	if (isundef (r -> number))
		Melder_throw (U"The row index is undefined.");
	row = Melder_iround (r -> number);
	if (row <= 0)
		Melder_throw (U"In matrix indexing, the row index should be positive.");
	if (row > matrix -> numericMatrixValue. nrow)
		Melder_throw (U"Row index out of bounds.");
	pushNumber (matrix -> numericMatrixValue [row] [column]);
}
static void do_indexedNumericVariable () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	integer nindex = Melder_iround (n -> number);
	if (nindex < 1)
		Melder_throw (U"Indexed variables require at least one index.");
	char32 *indexedVariableName = parse [programPointer]. content.string;
	static MelderString totalVariableName { };
	MelderString_copy (& totalVariableName, indexedVariableName, U"[");
	w -= nindex;
	for (int iindex = 1; iindex <= nindex; iindex ++) {
		Stackel index = & theStack [w + iindex];
		if (index -> which == Stackel_NUMBER) {
			MelderString_append (& totalVariableName, index -> number, iindex == nindex ? U"]" : U",");
		} else if (index -> which == Stackel_STRING) {
			MelderString_append (& totalVariableName, U"\"", index -> getString(), U"\"", iindex == nindex ? U"]" : U",");
		} else {
			Melder_throw (U"In indexed variables, the index should be a number or a string, not ", index->whichText(), U".");
		}
	}
	InterpreterVariable var = Interpreter_hasVariable (theInterpreter, totalVariableName.string);
	if (! var)
		Melder_throw (U"Undefined indexed variable «", totalVariableName.string, U"».");
	pushNumber (var -> numericValue);
}
static void do_indexedStringVariable () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	integer nindex = Melder_iround (n -> number);
	if (nindex < 1)
		Melder_throw (U"Indexed variables require at least one index.");
	char32 *indexedVariableName = parse [programPointer]. content.string;
	static MelderString totalVariableName { };
	MelderString_copy (& totalVariableName, indexedVariableName, U"[");
	w -= nindex;
	for (int iindex = 1; iindex <= nindex; iindex ++) {
		Stackel index = & theStack [w + iindex];
		if (index -> which == Stackel_NUMBER) {
			MelderString_append (& totalVariableName, index -> number, iindex == nindex ? U"]" : U",");
		} else if (index -> which == Stackel_STRING) {
			MelderString_append (& totalVariableName, U"\"", index -> getString(), U"\"", iindex == nindex ? U"]" : U",");
		} else {
			Melder_throw (U"In indexed variables, the index should be a number or a string, not ", index->whichText(), U".");
		}
	}
	InterpreterVariable var = Interpreter_hasVariable (theInterpreter, totalVariableName.string);
	if (! var)
		Melder_throw (U"Undefined indexed variable «", totalVariableName.string, U"».");
	autostring32 result = Melder_dup (var -> stringValue.get());
	pushString (result.move());
}
static void do_length () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		double result = str32len (s->getString());
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"length\" requires a string, not ", s->whichText(), U".");
	}
}
static void do_number () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		double result = Melder_atof (s->getString());
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"number\" requires a string, not ", s->whichText(), U".");
	}
}
static void do_fileReadable () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (s->getString(), & file);
		pushNumber (MelderFile_readable (& file));
	} else {
		Melder_throw (U"The function \"fileReadable\" requires a string, not ", s->whichText(), U".");
	}
}
static void do_STRdate () {
	pushString (STRdate ());
}
static void do_infoStr () {
	autostring32 info = Melder_dup (Melder_getInfo ());
	pushString (info.move());
}
static void do_STRleft () {
	trace (U"enter");
	Stackel narg = pop;
	if (narg->number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			pushString (newSTRleft (s->getString()));
		} else {
			Melder_throw (U"The function \"left$\" requires a string (or a string and a number).");
		}
	} else if (narg->number == 2) {
		Stackel n = pop, s = pop;
		if (s->which == Stackel_STRING && n->which == Stackel_NUMBER) {
			pushString (newSTRleft (s->getString(), Melder_iround (n->number)));
		} else {
			Melder_throw (U"The function \"left$\" requires a string and a number (or a string only).");
		}
	} else {
		Melder_throw (U"The function \"left$\" requires one or two arguments: a string and optionally a number.");
	}
	trace (U"exit");
}
static void do_STRright () {
	Stackel narg = pop;
	if (narg->number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			pushString (newSTRright (s->getString()));
		} else {
			Melder_throw (U"The function \"right$\" requires a string (or a string and a number).");
		}
	} else if (narg->number == 2) {
		Stackel n = pop, s = pop;
		if (s->which == Stackel_STRING && n->which == Stackel_NUMBER) {
			pushString (newSTRright (s->getString(), Melder_iround (n->number)));
		} else {
			Melder_throw (U"The function \"right$\" requires a string and a number (or a string only).");
		}
	} else {
		Melder_throw (U"The function \"right$\" requires one or two arguments: a string and optionally a number.");
	}
}
static void do_STRmid () {
	Stackel narg = pop;
	if (narg->number == 2) {
		Stackel position = pop, str = pop;
		if (str->which == Stackel_STRING && position->which == Stackel_NUMBER) {
			pushString (newSTRmid (str->getString(), Melder_iround (position->number)));
		} else {
			Melder_throw (U"The function \"mid$\" requires a string and a number (or two).");
		}
	} else if (narg->number == 3) {
		Stackel numberOfCharacters = pop, startingPosition = pop, str = pop;
		if (str->which == Stackel_STRING && startingPosition->which == Stackel_NUMBER && numberOfCharacters->which == Stackel_NUMBER) {
			pushString (newSTRmid (str->getString(), Melder_iround (startingPosition->number), Melder_iround (numberOfCharacters->number)));
		} else {
			Melder_throw (U"The function \"mid$\" requires a string and two numbers (or one).");
		}
	} else {
		Melder_throw (U"The function \"mid$\" requires two or three arguments.");
	}
}
static void do_unicodeToBackslashTrigraphsStr () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		integer length = str32len (s->getString());
		autostring32 trigraphs (3 * length);
		Longchar_genericize (s->getString(), trigraphs.get());
		pushString (trigraphs.move());
	} else {
		Melder_throw (U"The function \"unicodeToBackslashTrigraphs$\" requires a string, not ", s->whichText(), U".");
	}
}
static void do_backslashTrigraphsToUnicodeStr () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		integer length = str32len (s->getString());
		autostring32 unicode (length);
		Longchar_nativize (s->getString(), unicode.get(), false);   // noexcept
		pushString (unicode.move());
	} else {
		Melder_throw (U"The function \"unicodeToBackslashTrigraphs$\" requires a string, not ", s->whichText(), U".");
	}
}
static void do_environmentStr () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		conststring32 value = Melder_getenv (s->getString());
		autostring32 result = Melder_dup (value ? value : U"");
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"environment$\" requires a string, not ", s->whichText(), U".");
	}
}
static void do_index () {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->getString(), t->getString());
		integer result = substring ? substring - s->getString() + 1 : 0;
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"index\" requires two strings, not ",
			s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_rindex () {
	Stackel part = pop, whole = pop;
	if (whole->which == Stackel_STRING && part->which == Stackel_STRING) {
		char32 *lastSubstring = str32str (whole->getString(), part->getString());
		if (part->getString() [0] == U'\0') {
			integer result = str32len (whole->getString());
			pushNumber (result);
		} else if (lastSubstring) {
			for (;;) {
				char32 *substring = str32str (lastSubstring + 1, part->getString());
				if (! substring) break;
				lastSubstring = substring;
			}
			pushNumber (lastSubstring - whole->getString() + 1);
		} else {
			pushNumber (0);
		}
	} else {
		Melder_throw (U"The function \"rindex\" requires two strings, not ",
			whole->whichText(), U" and ", part->whichText(), U".");
	}
}
static void do_stringMatchesCriterion (kMelder_string criterion) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		int result = Melder_stringMatchesCriterion (s->getString(), criterion, t->getString(), true);
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_index_regex (int backward) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		conststring32 errorMessage;
		regexp *compiled_regexp = CompileRE (t->getString(), & errorMessage, 0);
		if (! compiled_regexp) {
			Melder_throw (U"index_regex(): ", errorMessage, U".");
		} else {
			if (ExecRE (compiled_regexp, nullptr, s->getString(), nullptr, backward, U'\0', U'\0', nullptr, nullptr)) {
				char32 *location = (char32 *) compiled_regexp -> startp [0];
				pushNumber (location - s->getString() + 1);
				free (compiled_regexp);
			} else {
				pushNumber (false);
			}
		}
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_STRreplace () {
	Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		autostring32 result = newSTRreplace (s->getString(), t->getString(), u->getString(), Melder_iround (x->number));
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"replace$\" requires three strings and a number.");
	}
}
static void do_STRreplace_regex () {
	Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		conststring32 errorMessage;
		regexp *compiled_regexp = CompileRE (t->getString(), & errorMessage, 0);
		if (! compiled_regexp) {
			Melder_throw (U"replace_regex$(): ", errorMessage, U".");
		} else {
			autostring32 result = newSTRreplace_regex (s->getString(), compiled_regexp, u->getString(), Melder_iround (x->number));
			pushString (result.move());
		}
	} else {
		Melder_throw (U"The function \"replace_regex$\" requires three strings and a number.");
	}
}
static void do_extractNumber () {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->getString(), t->getString());
		if (! substring) {
			pushNumber (undefined);
		} else {
			/* Skip the prompt. */
			substring += str32len (t->getString());
			/* Skip white space. */
			while (Melder_isHorizontalOrVerticalSpace (*substring)) substring ++;
			if (substring [0] == U'\0' || str32nequ (substring, U"--undefined--", 13)) {
				pushNumber (undefined);
			} else {
				char32 buffer [101];
				int i = 0;
				for (; i < 100; i ++) {
					buffer [i] = *substring;
					substring ++;
					if (*substring == U'\0' || Melder_isHorizontalOrVerticalSpace (*substring))
						break;
				}
				if (i >= 100) {
					buffer [100] = U'\0';
					pushNumber (Melder_atof (buffer));
				} else {
					buffer [i + 1] = U'\0';
					char32 *slash = str32chr (buffer, U'/');
					if (slash) {
						*slash = U'\0';
						double numerator = Melder_atof (buffer), denominator = Melder_atof (slash + 1);
						pushNumber (numerator / denominator);
					} else {
						pushNumber (Melder_atof (buffer));
					}
				}
			}
		}
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_extractTextStr (bool singleWord) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->getString(), t->getString());
		autostring32 result;
		if (! substring) {
			result = Melder_dup (U"");
		} else {
			integer length;
			/* Skip the prompt. */
			substring += str32len (t->getString());
			if (singleWord) {
				/* Skip white space. */
				while (Melder_isHorizontalOrVerticalSpace (*substring)) substring ++;
			}
			char32 *p = substring;
			if (singleWord) {
				/* Proceed until next white space. */
				while (Melder_staysWithinInk (*p)) p ++;
			} else {
				/* Proceed until end of line. */
				while (Melder_staysWithinLine (*p)) p ++;
			}
			length = p - substring;
			result = autostring32 (length);
			str32ncpy (result.get(), substring, length);
		}
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", s->whichText(), U" and ", t->whichText(), U".");
	}
}
static void do_selected () {
	Stackel n = pop;
	integer result = 0;
	if (n->number == 0) {
		result = praat_idOfSelected (nullptr, 0);
	} else if (n->number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (a->getString(), nullptr);
			result = praat_idOfSelected (klas, 0);
		} else if (a->which == Stackel_NUMBER) {
			result = praat_idOfSelected (nullptr, Melder_iround (a->number));
		} else {
			Melder_throw (U"The function \"selected\" requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_idOfSelected (klas, Melder_iround (x->number));
		} else {
			Melder_throw (U"The function \"selected\" requires a string (an object type name) and/or a number.");
		}
	} else {
		Melder_throw (U"The function \"selected\" requires 0, 1, or 2 arguments, not ", n->number, U".");
	}
	pushNumber (result);
}
#if 0
static void do_selectedStr () {
	Stackel n = pop;
	autostring32 result;
	if (n->number == 0) {
		result = Melder_dup (praat_nameOfSelected (nullptr, 0));
	} else if (n->number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (a->string, nullptr);
			result = Melder_dup (praat_nameOfSelected (klas, 0));
		} else if (a->which == Stackel_NUMBER) {
			result = Melder_dup (praat_nameOfSelected (nullptr, Melder_iround (a->number)));
		} else {
			Melder_throw (U"The function \"selected$\" requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->string, nullptr);
			result = Melder_dup (praat_nameOfSelected (klas, Melder_iround (x->number)));
		} else {
			Melder_throw (U"The function \"selected$\" requires a string (an object type name) and a number.");
		}
	} else {
		Melder_throw (U"The function \"selected$\" requires 0, 1, or 2 arguments, not ", n->number, U".");
	}
	pushString (result.transfer());
}
#else
static void do_selectedStr () {
	Stackel n = pop;
	char32 *resultSource;   // purposefully don't initialize, so that the compiler can check that has been assigned to when used
	if (n->number == 0) {
		resultSource = praat_nameOfSelected (nullptr, 0);
	} else if (n->number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (a->getString(), nullptr);
			resultSource = praat_nameOfSelected (klas, 0);
		} else if (a->which == Stackel_NUMBER) {
			resultSource = praat_nameOfSelected (nullptr, Melder_iround (a->number));
		} else {
			Melder_throw (U"The function \"selected$\" requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			resultSource = praat_nameOfSelected (klas, Melder_iround (x->number));
		} else {
			Melder_throw (U"The function \"selected$\" requires a string (an object type name) and a number.");
		}
	} else {
		Melder_throw (U"The function \"selected$\" requires 0, 1, or 2 arguments, not ", n->number, U".");
	}
	pushString (Melder_dup (resultSource));
}
#endif
static void do_numberOfSelected () {
	Stackel n = pop;
	integer result = 0;
	if (n->number == 0) {
		result = praat_numberOfSelected (nullptr);
	} else if (n->number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_numberOfSelected (klas);
		} else {
			Melder_throw (U"The function \"numberOfSelected\" requires a string (an object type name), not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"numberOfSelected\" requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushNumber (result);
}
static void do_VECselected () {
	Stackel n = pop;
	autoVEC result;
	if (n->number == 0) {
		result = praat_idsOfAllSelected (nullptr);
	} else if (n->number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (s->getString(), nullptr);
			result = praat_idsOfAllSelected (klas);
		} else {
			Melder_throw (U"The function \"numberOfSelected\" requires a string (an object type name), not ", s->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"numberOfSelected\" requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushNumericVector (result.move());
}
static void do_selectObject () {
	Stackel n = pop;
	praat_deselectAll ();
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			int IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectByName (object -> getString());
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			VEC vec = object -> numericVector;
			for (int ielm = 1; ielm <= vec.size; ielm ++) {
				int IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_select (IOBJECT);
			}
		} else {
			Melder_throw (U"The function \"selectObject\" takes numbers, strings, or numeric vectors, not ", object->whichText());
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_plusObject () {
	Stackel n = pop;
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			int IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectByName (object -> getString());
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			VEC vec = object -> numericVector;
			for (int ielm = 1; ielm <= vec.size; ielm ++) {
				int IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_select (IOBJECT);
			}
		} else {
			Melder_throw (U"The function \"plusObject\" takes numbers, strings, or numeric vectors, not ", object->whichText(), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_minusObject () {
	Stackel n = pop;
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			int IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_deselect (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectByName (object -> getString());
			praat_deselect (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			VEC vec = object -> numericVector;
			for (int ielm = 1; ielm <= vec.size; ielm ++) {
				int IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_deselect (IOBJECT);
			}
		} else {
			Melder_throw (U"The function \"minusObject\" takes numbers, strings, or numeric vectors, not ", object->whichText(), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_removeObject () {
	Stackel n = pop;
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			int IOBJECT = praat_findObjectById (Melder_iround (object -> number));
			praat_removeObject (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectByName (object -> getString());
			praat_removeObject (IOBJECT);
		} else if (object -> which == Stackel_NUMERIC_VECTOR) {
			VEC vec = object -> numericVector;
			for (int ielm = 1; ielm <= vec.size; ielm ++) {
				int IOBJECT = praat_findObjectById (Melder_iround (vec [ielm]));
				praat_removeObject (IOBJECT);
			}
		} else {
			Melder_throw (U"The function \"removeObject\" takes numbers, strings, or numeric vectors, not ", object->whichText(), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static Daata _do_object (Stackel object, conststring32 expressionMessage) {
	Daata data;
	if (object -> which == Stackel_NUMBER) {
		int IOBJECT = praat_findObjectById (Melder_iround (object -> number));
		data = OBJECT;
	} else if (object -> which == Stackel_STRING) {
		int IOBJECT = praat_findObjectByName (object -> getString());
		data = OBJECT;
	} else if (object -> which == Stackel_OBJECT) {
		data = object -> object;
	} else {
		Melder_throw (U"The expression \"", expressionMessage, U"\" requires xx to be a number or a string, not ", object->whichText(), U".");
	}
	return data;
}
static void do_object_xmin () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].xmin");
	Melder_require (data -> v_hasGetXmin (),
		U"An object of type ", Thing_className (data), U" has no \"xmin\" attribute.");
	pushNumber (data -> v_getXmin ());
}
static void do_object_xmax () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].xmax");
	Melder_require (data -> v_hasGetXmax (),
		U"An object of type ", Thing_className (data), U" has no \"xmax\" attribute.");
	pushNumber (data -> v_getXmax ());
}
static void do_object_ymin () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].ymin");
	Melder_require (data -> v_hasGetYmin (),
		U"An object of type ", Thing_className (data), U" has no \"ymin\" attribute.");
	pushNumber (data -> v_getYmin ());
}
static void do_object_ymax () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].ymax");
	Melder_require (data -> v_hasGetYmax (),
		U"An object of type ", Thing_className (data), U" has no \"ymax\" attribute.");
	pushNumber (data -> v_getYmax ());
}
static void do_object_nx () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].nx");
	Melder_require (data -> v_hasGetNx (),
		U"An object of type ", Thing_className (data), U" has no \"nx\" attribute.");
	pushNumber (data -> v_getNx ());
}
static void do_object_ny () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].ny");
	Melder_require (data -> v_hasGetNy (),
		U"An object of type ", Thing_className (data), U" has no \"ny\" attribute.");
	pushNumber (data -> v_getNy ());
}
static void do_object_dx () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].dx");
	Melder_require (data -> v_hasGetDx (),
		U"An object of type ", Thing_className (data), U" has no \"dx\" attribute.");
	pushNumber (data -> v_getDx ());
}
static void do_object_dy () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].dy");
	Melder_require (data -> v_hasGetDy (),
		U"An object of type ", Thing_className (data), U" has no \"dy\" attribute.");
	pushNumber (data -> v_getDy ());
}
static void do_object_nrow () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].nrow");
	Melder_require (data -> v_hasGetNrow (),
		U"An object of type ", Thing_className (data), U" has no \"nrow\" attribute.");
	pushNumber (data -> v_getNrow ());
}
static void do_object_ncol () {
	Stackel object = pop;
	Daata data = _do_object (object, U"object[xx].ncol");
	Melder_require (data -> v_hasGetNcol (),
		U"An object of type ", Thing_className (data), U" has no \"ncol\" attribute.");
	pushNumber (data -> v_getNcol ());
}
static void do_object_rowstr () {
	Stackel index = pop, object = pop;
	Daata data = _do_object (object, U"object[xx].row$[]");
	Melder_require (data -> v_hasGetRowStr (),
		U"An object of type ", Thing_className (data), U" has no \"row$[]\" attribute.");
	Melder_require (index -> which == Stackel_NUMBER,
		U"The expression \"object[].row$[xx]\" requires xx to be a number, not ", index->whichText(), U".");
	integer number = Melder_iround (index->number);
	autostring32 result = Melder_dup (data -> v_getRowStr (number));
	if (! result)
		Melder_throw (U"Row index out of bounds.");
	pushString (result.move());
}
static void do_object_colstr () {
	Stackel index = pop, object = pop;
	Daata data = _do_object (object, U"object[xx].col$[]");
	Melder_require (data -> v_hasGetColStr (),
		U"An object of type ", Thing_className (data), U" has no \"col$[]\" attribute.");
	Melder_require (index -> which == Stackel_NUMBER,
		U"The expression \"object[].col$[xx]\" requires xx to be a number, not ", index->whichText(), U".");
	integer number = Melder_iround (index->number);
	autostring32 result = Melder_dup (data -> v_getColStr (number));
	if (! result)
		Melder_throw (U"Column index out of bounds.");
	pushString (result.move());
}
static void do_stringStr () {
	Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_double (value->number));
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"string$\" requires a number, not ", value->whichText(), U".");
	}
}
static void do_sleep () {
	Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		Melder_sleep (value->number);
		pushNumber (1);
	} else {
		Melder_throw (U"The function \"sleep\" requires a number, not ", value->whichText(), U".");
	}
}
static void do_unicode () {
	Stackel value = pop;
	if (value->which == Stackel_STRING) {
		pushNumber (value->getString() [0]);
	} else {
		Melder_throw (U"The function \"unicode\" requires a character, not ", value->whichText(), U".");
	}
}
static void do_unicodeStr () {
	Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		Melder_require (value->number >= 0.0 && value->number < (double) (1 << 21),
			U"A unicode number cannot be greater than ", (1 << 21) - 1, U".");
		Melder_require (value->number < 0xD800 || value->number > 0xDFFF,
			U"A unicode number cannot lie between 0xD800 and 0xDFFF. Those are \"surrogates\".");
		char32 string [2] = { U'\0', U'\0' };
		string [0] = (char32) value->number;
		pushString (Melder_dup (string).move());
	} else {
		Melder_throw (U"The function \"unicode$\" requires a number, not ", value->whichText(), U".");
	}
}
static void do_fixedStr () {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_fixed (value->number, Melder_iround (precision->number)));
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"fixed$\" requires two numbers (value and precision), not ", value->whichText(), U" and ", precision->whichText(), U".");
	}
}
static void do_percentStr () {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_percent (value->number, Melder_iround (precision->number)));
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"percent$\" requires two numbers (value and precision), not ", value->whichText(), U" and ", precision->whichText(), U".");
	}
}
static void do_hexadecimalStr () {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_hexadecimal (Melder_iround (value->number), Melder_iround (precision->number)));
		pushString (result.move());
	} else {
		Melder_throw (U"The function \"hexadecimal$\" requires two numbers (value and precision), not ", value->whichText(), U" and ", precision->whichText(), U".");
	}
}
static void do_deleteFile () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"deleteFile\" is not available inside manuals.");
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		MelderFile_delete (& file);
		pushNumber (1);
	} else {
		Melder_throw (U"The function \"deleteFile\" requires a string, not ", f->whichText(), U".");
	}
}
static void do_createDirectory () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"createDirectory\" is not available inside manuals.");
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderDir currentDirectory { };
		Melder_getDefaultDir (& currentDirectory);
		#if defined (UNIX) || defined (macintosh)
			Melder_createDirectory (& currentDirectory, f->getString(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#else
			Melder_createDirectory (& currentDirectory, f->getString(), 0);
		#endif
		pushNumber (1);
	} else {
		Melder_throw (U"The function \"createDirectory\" requires a string, not ", f->whichText(), U".");
	}
}
static void do_variableExists () {
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		bool result = !! Interpreter_hasVariable (theInterpreter, f->getString());
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"variableExists\" requires a string, not ", f->whichText(), U".");
	}
}
static void do_readFile () {
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		autostring32 text = MelderFile_readText (& file);
		pushNumber (Melder_atof (text.get()));
	} else {
		Melder_throw (U"The function \"readFile\" requires a string (a file name), not ", f->whichText(), U".");
	}
}
static void do_readFileStr () {
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->getString(), & file);
		autostring32 text = MelderFile_readText (& file);
		pushString (text.move());
	} else {
		Melder_throw (U"The function \"readFile$\" requires a string (a file name), not ", f->whichText(), U".");
	}
}
static void do_tensorLiteral () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	integer numberOfElements = Melder_iround (n->number);
	Melder_assert (numberOfElements > 0);
	/*
		The type of the tensor can be a vector, or a matrix, or a tensor3...
		This depends on whether the last element is a number, a vector, or a matrix...
	*/
	Stackel last = pop;
	if (last->which == Stackel_NUMBER) {
		autoVEC result = newVECraw (numberOfElements);
		result [numberOfElements] = last->number;
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			Stackel element = pop;
			if (element->which != Stackel_NUMBER)
				Melder_throw (U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a number.");
			result [ielement] = element->number;
		}
		pushNumericVector (result.move());
	} else if (last->which == Stackel_NUMERIC_VECTOR) {
		integer sharedNumberOfColumns = last->numericVector.size;
		autoMAT result = newMATraw (numberOfElements, sharedNumberOfColumns);
		result.row (numberOfElements) <<= last->numericVector;
		for (integer ielement = numberOfElements - 1; ielement > 0; ielement --) {
			Stackel element = pop;
			Melder_require (element->which == Stackel_NUMERIC_VECTOR,
				U"The tensor elements have to be of the same type, not ", element->whichText(), U" and a vector.");
			Melder_require (element->numericVector.size == sharedNumberOfColumns,
				U"The vectors have to be of the same size, not ", element->numericVector.size, U" and ", sharedNumberOfColumns);
			result.row (ielement) <<= element->numericVector;
		}
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"Cannot (yet?) create a tensor containing ", last->whichText(), U".");
	}
}
static void do_inner () {
	/*
		result = inner (x#, y#)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (NUMinner (x->numericVector, y->numericVector));
	} else {
		Melder_throw (U"The function \"inner\" requires two vectors, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATouter () {
	/*
		result## = outer## (x#, y#)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		autoMAT result = newMATouter (x->numericVector, y->numericVector);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"outer##\" requires two vectors, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_VECmul () {
	/*
		result# = mul# (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul# (x#, y##)
		*/
		integer xSize = x->numericVector.size, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xSize,
			U"In the function \"mul#\", the dimension of the vector and the number of rows of the matrix should be equal, "
			U"not ", xSize, U" and ", yNrow
		);
		autoVEC result = newVECmul (x->numericVector, y->numericMatrix);
		pushNumericVector (result.move());
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR) {
		/*
			result# = mul# (x##, y#)
		*/
		integer xNcol = x->numericMatrix.ncol, ySize = y->numericVector.size;
		Melder_require (ySize == xNcol,
			U"In the function \"mul#\", the number of columns of the matrix and the dimension of the vector should be equal, "
			U"not ", xNcol, U" and ", ySize, U"."
		);
		autoVEC result = newVECmul (x->numericMatrix, y->numericVector);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function \"mul#\" requires a vector and a matrix, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATmul () {
	/*
		result## = mul## (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul## (x##, y##)
		*/
		integer xNcol = x->numericMatrix.ncol, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNcol,
			U"In the function \"mul##\", the number of columns of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNrow, U"."
		);
		autoMAT result = newMATzero (x->numericMatrix.nrow, y->numericMatrix.ncol);
		MATmul_allowAllocation_ (result.get(), x->numericMatrix, y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"mul##\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATmul_metal () {
	/*
		result## = mul## (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul## (x##, y##)
		*/
		integer xNcol = x->numericMatrix.ncol, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNcol,
			U"In the function \"mul##\", the number of columns of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNrow, U"."
		);
		autoMAT result = newMATzero (x->numericMatrix.nrow, y->numericMatrix.ncol);
		MATmul_forceMetal_ (result.get(), x->numericMatrix, y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"mul_metal##\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATmul_fast () {
	/*
		result## = mul_fast## (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_fast## (x##, y##)
		*/
		integer xNcol = x->numericMatrix.ncol, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNcol,
			U"In the function \"mul_fast##\", the number of columns of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNrow, U"."
		);
		autoMAT result = newMATmul_fast (x->numericMatrix, y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"mul_fast##\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATmul_tn () {
	/*
		result## = mul_tn## (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_tn## (x##, y##)
		*/
		integer xNrow = x->numericMatrix.nrow, yNrow = y->numericMatrix.nrow;
		Melder_require (yNrow == xNrow,
			U"In the function \"mul_tn##\", the number of rows of the first matrix and the number of rows of the second matrix should be equal, "
			U"not ", xNrow, U" and ", yNrow, U"."
		);
		autoMAT result = newMATmul_allowAllocation (x->numericMatrix.transpose(), y->numericMatrix);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"mul_tn##\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATmul_nt () {
	/*
		result## = mul_nt## (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_nt## (x##, y##)
		*/
		integer xNcol = x->numericMatrix.ncol, yNcol = y->numericMatrix.ncol;
		Melder_require (yNcol == xNcol,
			U"In the function \"mul_tn##\", the number of columns of the first matrix and the number of columns of the second matrix should be equal, "
			U"not ", xNcol, U" and ", yNcol, U"."
		);
		autoMAT result = newMATmul_allowAllocation (x->numericMatrix, y->numericMatrix.transpose());
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"mul_nt##\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATmul_tt () {
	/*
		result## = mul_tt## (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul_tt## (x##, y##)
		*/
		integer xNrow = x->numericMatrix.nrow, yNcol = y->numericMatrix.ncol;
		Melder_require (yNcol == xNrow,
			U"In the function \"mul_tt##\", the number of rows of the first matrix and the number of columns of the second matrix should be equal, "
			U"not ", xNrow, U" and ", yNcol, U"."
		);
		autoMAT result = newMATmul_allowAllocation (x->numericMatrix.transpose(), y->numericMatrix.transpose());
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"mul_tt##\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_MATtranspose () {
	Stackel x = topOfStack;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		if (x->owned) {
			if (NUMisSymmetric (x->numericMatrix)) {
				/*@praat
					assert transpose## ({ { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } }) = { { 1, 4, 7 }, { 2, 5, 8 }, { 3, 6, 9 } }
				@*/
				MATtranspose_inplace_mustBeSquare (x->numericMatrix);
			} else {
				/*@praat
					assert transpose## ({ { 1, 2, 3 }, { 4, 5, 6 } }) = { { 1, 4 }, { 2, 5 }, { 3, 6 } }
				@*/
				autoMAT newMatrix = newMATtranspose (x->numericMatrix);
				x->reset();
				x->numericMatrix = newMatrix.releaseToAmbiguousOwner();
			}
		} else {
			/*@praat
				a## = { { 1, 2, 3 }, { 4, 5, 6 } }
				assert transpose## (a##) = { { 1, 4 }, { 2, 5 }, { 3, 6 } }
				assert transpose## (transpose## (a##)) = a##
			@*/
			x->numericMatrix = newMATtranspose (x->numericMatrix). releaseToAmbiguousOwner();
			x->owned = true;
		}
	} else {
		Melder_throw (U"The function \"transpose##\" requires a matrix, not ", x->whichText(), U".");
	}
}
static void do_rowSumsH () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		autoVEC result = newVECrowSums (x->numericMatrix);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function \"rowSums#\" requires a matrix, not ", x->whichText(), U".");
	}
}
static void do_columnSumsH () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		/*@praat
			a## = { { 4, 7, -10 }, { 16, 0, 88 } }
			result# = columnSums# (a##)
			assert result# = { 20, 7, 78 }
		@*/
		autoVEC result = newVECcolumnSums (x->numericMatrix);
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function \"columnSums#\" requires a matrix, not ", x->whichText(), U".");
	}
}
static void do_VECrepeat () {
	Stackel n = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && n->which == Stackel_NUMBER) {
		integer n_old = x->numericVector.size;
		integer times = Melder_iround (n->number);
		autoVEC result { n_old * times, kTensorInitializationType::RAW };
		for (integer i = 1; i <= times; i ++)
			for (integer j = 1; j <= n_old; j ++)
				result [(i - 1) * n_old + j] = x->numericVector [j];
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function \"repeat#\" requires a vector and a number, not ", x->whichText(), U" and ", n->whichText(), U".");
	}
}
static void do_VECrowInners () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		Melder_require (x->numericMatrix.nrow == y->numericMatrix.nrow && x->numericMatrix.ncol == y->numericMatrix.ncol,
			U"In the function rowInners#, the two matrices should have the same shape, not ",
			x->numericMatrix.nrow, U"x", x->numericMatrix.ncol, U" and ", y->numericMatrix.nrow, U"x", y->numericMatrix.ncol
		);
		pushNumericVector (newVECrowInners (x->numericMatrix, y->numericMatrix));
	} else {
		Melder_throw (U"The function \"rowInners#\" requires two matrices, not ", x->whichText(), U" and ", y->whichText(), U".");
	}
}
static void do_beginPauseForm () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"beginPauseForm\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 1) {
		Stackel title = pop;
		if (title->which == Stackel_STRING) {
			UiPause_begin (theCurrentPraatApplication -> topShell, title->getString(), theInterpreter);
		} else {
			Melder_throw (U"The function \"beginPauseForm\" requires a string (the title), not ", title->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"beginPauseForm\" requires 1 argument (a title), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddReal () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"real\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		conststring32 defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->getString();
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"real\" (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_real (label->getString(), defaultString);
		} else {
			Melder_throw (U"The first argument of \"real\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"real\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddPositive () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"positive\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		conststring32 defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->getString();
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"positive\" (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_positive (label->getString(), defaultString);
		} else {
			Melder_throw (U"The first argument of \"positive\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"positive\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddInteger () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"integer\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		conststring32 defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->getString();
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"integer\" (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_integer (label->getString(), defaultString);
		} else {
			Melder_throw (U"The first argument of \"integer\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"integer\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddNatural () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"natural\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		conststring32 defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->getString();
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"natural\" (the default value) should be a string or a number, not ", defaultValue->whichText(), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_natural (label->getString(), defaultString);
		} else {
			Melder_throw (U"The first argument of \"natural\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"natural\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddWord () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"word\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		Melder_require (defaultValue->which == Stackel_STRING,
			U"The second argument of \"word\" (the default value) should be a string, not ", defaultValue->whichText(), U".");
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_word (label->getString(), defaultValue->getString());
		} else {
			Melder_throw (U"The first argument of \"word\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"word\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddSentence () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"sentence\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		Melder_require (defaultValue->which == Stackel_STRING,
			U"The second argument of \"sentence\" (the default value) should be a string, not ", defaultValue->whichText(), U".");
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_sentence (label->getString(), defaultValue->getString());
		} else {
			Melder_throw (U"The first argument of \"sentence\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"sentence\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddText () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"text\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		Melder_require (defaultValue->which == Stackel_STRING,
			U"The second argument of \"text\" (the default value) should be a string, not ", defaultValue->whichText(), U".");
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_text (label->getString(), defaultValue->getString());
		} else {
			Melder_throw (U"The first argument of \"text\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"text\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddBoolean () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"boolean\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		Melder_require (defaultValue->which == Stackel_NUMBER,
			U"The second argument of \"boolean\" (the default value) should be a number (0 or 1), not ", defaultValue->whichText(), U".");
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_boolean (label->getString(), defaultValue->number != 0.0);
		} else {
			Melder_throw (U"The first argument of \"boolean\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"boolean\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddChoice () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"choice\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		if (defaultValue->which != Stackel_NUMBER) {
			Melder_throw (U"The second argument of \"choice\" (the default value) should be a whole number, not ", defaultValue->whichText(), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_choice (label->getString(), Melder_iround (defaultValue->number));
		} else {
			Melder_throw (U"The first argument of \"choice\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"choice\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddOptionMenu () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"optionMenu\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultValue = pop;
		if (defaultValue->which != Stackel_NUMBER) {
			Melder_throw (U"The second argument of \"optionMenu\" (the default value) should be a whole number, not ", defaultValue->whichText(), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_optionMenu (label->getString(), Melder_iround (defaultValue->number));
		} else {
			Melder_throw (U"The first argument of \"optionMenu\" (the label) should be a string, not ", label->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"optionMenu\" requires 2 arguments (a label and a default value), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddOption () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"option\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 1) {
		Stackel text = pop;
		if (text->which == Stackel_STRING) {
			UiPause_option (text->getString());
		} else {
			Melder_throw (U"The argument of \"option\" should be a string (the text), not ", text->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"option\" requires 1 argument (a text), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_pauseFormAddComment () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"comment\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 1) {
		Stackel text = pop;
		if (text->which == Stackel_STRING) {
			UiPause_comment (text->getString());
		} else {
			Melder_throw (U"The argument of \"comment\" should be a string (the text), not ", text->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"comment\" requires 1 argument (a text), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_endPauseForm () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"endPause\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number < 2 || n->number > 12)
		Melder_throw (U"The function \"endPause\" requires 2 to 12 arguments, not ", n->number, U".");
	Stackel d = pop;
	if (d->which != Stackel_NUMBER)
		Melder_throw (U"The last argument of \"endPause\" should be a number (the default or cancel continue button), not ", d->whichText(), U".");
	integer numberOfContinueButtons = Melder_iround (n->number) - 1;
	integer cancelContinueButton = 0, defaultContinueButton = Melder_iround (d->number);
	Stackel ca = pop;
	if (ca->which == Stackel_NUMBER) {
		cancelContinueButton = defaultContinueButton;
		defaultContinueButton = Melder_iround (ca->number);
		numberOfContinueButtons --;
		if (cancelContinueButton < 1 || cancelContinueButton > numberOfContinueButtons)
			Melder_throw (U"Your last argument of \"endPause\" is the number of the cancel button; it cannot be ", cancelContinueButton,
				U" but should lie between 1 and ", numberOfContinueButtons, U".");
	}
	Stackel co [1+10] = { 0 };
	for (integer i = numberOfContinueButtons; i >= 1; i --) {
		co [i] = cancelContinueButton != 0 || i != numberOfContinueButtons ? pop : ca;
		if (co[i]->which != Stackel_STRING)
			Melder_throw (U"Each of the first ", numberOfContinueButtons,
				U" argument(s) of \"endPause\" should be a string (a button text), not ", co[i]->whichText(), U".");
	}
	int buttonClicked = UiPause_end (numberOfContinueButtons, defaultContinueButton, cancelContinueButton,
		! co [1] ? nullptr : co[1]->getString(), ! co [2] ? nullptr : co[2]->getString(),
		! co [3] ? nullptr : co[3]->getString(), ! co [4] ? nullptr : co[4]->getString(),
		! co [5] ? nullptr : co[5]->getString(), ! co [6] ? nullptr : co[6]->getString(),
		! co [7] ? nullptr : co[7]->getString(), ! co [8] ? nullptr : co[8]->getString(),
		! co [9] ? nullptr : co[9]->getString(), ! co [10] ? nullptr : co[10]->getString(),
		theInterpreter);
	//Melder_casual (U"Button ", buttonClicked);
	pushNumber (buttonClicked);
}
static void do_chooseReadFileStr () {
	Stackel n = pop;
	if (n->number == 1) {
		Stackel title = pop;
		if (title->which == Stackel_STRING) {
			autoStringSet fileNames = GuiFileSelect_getInfileNames (nullptr, title->getString(), false);
			if (fileNames->size == 0) {
				pushString (Melder_dup (U""));
			} else {
				SimpleString fileName = fileNames->at [1];
				pushString (Melder_dup (fileName -> string.get()));
			}
		} else {
			Melder_throw (U"The argument of \"chooseReadFile$\" should be a string (the title), not ", title->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"chooseReadFile$\" requires 1 argument (a title), not ", n->number, U".");
	}
}
static void do_chooseWriteFileStr () {
	Stackel n = pop;
	if (n->number == 2) {
		Stackel defaultName = pop, title = pop;
		if (title->which == Stackel_STRING && defaultName->which == Stackel_STRING) {
			autostring32 result = GuiFileSelect_getOutfileName (nullptr, title->getString(), defaultName->getString());
			if (! result)
				result = Melder_dup (U"");
			pushString (result.move());
		} else {
			Melder_throw (U"The arguments of \"chooseWriteFile$\" should be two strings (the title and the default name).");
		}
	} else {
		Melder_throw (U"The function \"chooseWriteFile$\" requires 2 arguments (a title and a default name), not ", n->number, U".");
	}
}
static void do_chooseDirectoryStr () {
	Stackel n = pop;
	if (n->number == 1) {
		Stackel title = pop;
		if (title->which == Stackel_STRING) {
			autostring32 result = GuiFileSelect_getDirectoryName (nullptr, title->getString());
			if (! result)
				result = Melder_dup (U"");
			pushString (result.move());
		} else {
			Melder_throw (U"The argument of \"chooseDirectory$\" should be a string (the title).");
		}
	} else {
		Melder_throw (U"The function \"chooseDirectory$\" requires 1 argument (a title), not ", n->number, U".");
	}
}
static void do_demoWindowTitle () {
	Stackel n = pop;
	if (n->number == 1) {
		Stackel title = pop;
		if (title->which == Stackel_STRING) {
			Demo_windowTitle (title->getString());
		} else {
			Melder_throw (U"The argument of \"demoWindowTitle\" should be a string (the title), not ", title->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"demoWindowTitle\" requires 1 argument (a title), not ", n->number, U".");
	}
	pushNumber (1);
}
static void do_demoShow () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoShow\" requires 0 arguments, not ", n->number, U".");
	Demo_show ();
	pushNumber (1);
}
static void do_demoWaitForInput () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoWaitForInput\" requires 0 arguments, not ", n->number, U".");
	Demo_waitForInput (theInterpreter);
	pushNumber (1);
}
static void do_demoPeekInput () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoPeekInput\" requires 0 arguments, not ", n->number, U".");
	Demo_peekInput (theInterpreter);
	pushNumber (1);
}
static void do_demoInput () {
	Stackel n = pop;
	if (n->number == 1) {
		Stackel keys = pop;
		if (keys->which == Stackel_STRING) {
			bool result = Demo_input (keys->getString());
			pushNumber (result);
		} else {
			Melder_throw (U"The argument of \"demoInput\" should be a string (the keys), not ", keys->whichText(), U".");
		}
	} else {
		Melder_throw (U"The function \"demoInput\" requires 1 argument (keys), not ", n->number, U".");
	}
}
static void do_demoClickedIn () {
	Stackel n = pop;
	if (n->number == 4) {
		Stackel top = pop, bottom = pop, right = pop, left = pop;
		if (left->which == Stackel_NUMBER && right->which == Stackel_NUMBER && bottom->which == Stackel_NUMBER && top->which == Stackel_NUMBER) {
			bool result = Demo_clickedIn (left->number, right->number, bottom->number, top->number);
			pushNumber (result);
		} else {
			Melder_throw (U"All arguments of \"demoClickedIn\" should be numbers (the x and y ranges).");
		}
	} else {
		Melder_throw (U"The function \"demoClickedIn\" requires 4 arguments (x and y ranges), not ", n->number, U".");
	}
}
static void do_demoClicked () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoClicked\" requires 0 arguments, not ", n->number, U".");
	bool result = Demo_clicked ();
	pushNumber (result);
}
static void do_demoX () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoX\" requires 0 arguments, not ", n->number, U".");
	double result = Demo_x ();
	pushNumber (result);
}
static void do_demoY () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoY\" requires 0 arguments, not ", n->number, U".");
	double result = Demo_y ();
	pushNumber (result);
}
static void do_demoKeyPressed () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoKeyPressed\" requires 0 arguments, not ", n->number, U".");
	bool result = Demo_keyPressed ();
	pushNumber (result);
}
static void do_demoKey () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoKey\" requires 0 arguments, not ", n->number, U".");
	autostring32 key (1);
	key [0] = Demo_key ();
	pushString (key.move());
}
static void do_demoShiftKeyPressed () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoShiftKeyPressed\" requires 0 arguments, not ", n->number, U".");
	bool result = Demo_shiftKeyPressed ();
	pushNumber (result);
}
static void do_demoCommandKeyPressed () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoCommandKeyPressed\" requires 0 arguments, not ", n->number, U".");
	bool result = Demo_commandKeyPressed ();
	pushNumber (result);
}
static void do_demoOptionKeyPressed () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoOptionKeyPressed\" requires 0 arguments, not ", n->number, U".");
	bool result = Demo_optionKeyPressed ();
	pushNumber (result);
}
static void do_demoExtraControlKeyPressed () {
	Stackel n = pop;
	if (n->number != 0)
		Melder_throw (U"The function \"demoControlKeyPressed\" requires 0 arguments, not ", n->number, U".");
	bool result = Demo_extraControlKeyPressed ();
	pushNumber (result);
}
static integer Stackel_getRowNumber (Stackel row, Daata thee) {
	integer result = 0;
	if (row->which == Stackel_NUMBER) {
		result = Melder_iround (row->number);
	} else if (row->which == Stackel_STRING) {
		if (! thy v_hasGetRowIndex ())
			Melder_throw (U"Objects of type ", Thing_className (thee), U" do not have row labels, so row indexes have to be numbers.");
		result = Melder_iround (thy v_getRowIndex (row->getString()));
		if (result == 0)
			Melder_throw (U"Object \"", thy name.get(), U"\" has no row labelled \"", row->getString(), U"\".");
	} else {
		Melder_throw (U"A row index should be a number or a string, not ", row->whichText(), U".");
	}
	return result;
}
static integer Stackel_getColumnNumber (Stackel column, Daata thee) {
	integer result = 0;
	if (column->which == Stackel_NUMBER) {
		result = Melder_iround (column->number);
	} else if (column->which == Stackel_STRING) {
		if (! thy v_hasGetColIndex ())
			Melder_throw (U"Objects of type ", Thing_className (thee), U" do not have column labels, so column indexes have to be numbers.");
		result = Melder_iround (thy v_getColIndex (column->getString()));
		if (result == 0)
			Melder_throw (U"Object ", thee, U" has no column labelled \"", column->getString(), U"\".");
	} else {
		Melder_throw (U"A column index should be a number or a string, not ", column->whichText(), U".");
	}
	return result;
}
static void do_self0 (integer irow, integer icol) {
	Daata me = theSource;
	if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
	if (my v_hasGetCell ()) {
		pushNumber (my v_getCell ());
	} else if (my v_hasGetVector ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop, hence no implicit column index for the current ",
				Thing_className (me), U" object (self).\nTry using the [column] index explicitly.");
		} else {
			pushNumber (my v_getVector (irow, icol));
		}
	} else if (my v_hasGetMatrix ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using both [row, column] indexes explicitly.");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using the [row] index explicitly.");
			}
		} else {
			pushNumber (my v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [] indexing.");
	}
}
static void do_selfStr0 (integer irow, integer icol) {
	Daata me = theSource;
	if (! me) Melder_throw (U"The name \"self$\" is restricted to formulas for objects.");
	if (my v_hasGetCellStr ()) {
		pushString (Melder_dup (my v_getCellStr ()));
	} else if (my v_hasGetVectorStr ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop, hence no implicit column index for the current ",
				Thing_className (me), U" object (self).\nTry using the [column] index explicitly.");
		} else {
			pushString (Melder_dup (my v_getVectorStr (icol)));
		}
	} else if (my v_hasGetMatrixStr ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using both [row, column] indexes explicitly.");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for the current ",
					Thing_className (me), U" object (self).\n"
					U"Try using the [row] index explicitly.");
			}
		} else {
			pushString (Melder_dup (my v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [] indexing.");
	}
}
static void do_toObject () {
	Stackel object = pop;
	Daata thee = nullptr;
	if (object->which == Stackel_NUMBER) {
		int i = theCurrentPraatObjects -> n;
		while (i > 0 && object->number != theCurrentPraatObjects -> list [i]. id)
			i --;
		if (i == 0)
			Melder_throw (U"No such object: ", object->number);
		thee = (Daata) theCurrentPraatObjects -> list [i]. object;
	} else if (object->which == Stackel_STRING) {
		int i = theCurrentPraatObjects -> n;
		while (i > 0 && ! Melder_equ (object->getString(), theCurrentPraatObjects -> list [i]. name.get()))
			i --;
		if (i == 0)
			Melder_throw (U"No such object: ", object->getString());
		thee = (Daata) theCurrentPraatObjects -> list [i]. object;
	} else {
		Melder_throw (U"The first argument to \"object\" should be a number (unique ID) or a string (name), not ", object->whichText(), U".");
	}
	pushObject (thee);
}
static void do_objectCell0 (integer irow, integer icol) {
	Stackel object = pop;
	Daata thee = object->object;
	if (thy v_hasGetCell ()) {
		pushNumber (thy v_getCell ());
	} else if (thy v_hasGetVector ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit column index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, column].");
		} else {
			pushNumber (thy v_getVector (irow, icol));
		}
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for this ", Thing_className (thee), U" object.\n"
					U"Try using: object [id, row, column].");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
					U"Try using: object [id, row].");
			}
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [] indexing.");
	}
}
static void do_matrix0 (integer irow, integer icol) {
	Daata thee = parse [programPointer]. content.object;
	if (thy v_hasGetCell ()) {
		pushNumber (thy v_getCell ());
	} else if (thy v_hasGetVector ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit column index for this ", Thing_className (thee), U" object.\n"
				U"Try using the [column] index explicitly.");
		} else {
			pushNumber (thy v_getVector (irow, icol));
		}
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_throw (U"We are not in a loop over rows and columns,\n"
					U"hence no implicit row and column indexing for this ", Thing_className (thee), U" object.\n"
					U"Try using both [row, column] indexes explicitly.");
			} else {
				Melder_throw (U"We are not in a loop over columns only,\n"
					U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
					U"Try using the [row] index explicitly.");
			}
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [] indexing.");
	}
}
static void do_selfMatrix1 (integer irow) {
	Daata me = theSource;
	Stackel column = pop;
	if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
	integer icol = Stackel_getColumnNumber (column, me);
	if (my v_hasGetVector ()) {
		pushNumber (my v_getVector (irow, icol));
	} else if (my v_hasGetMatrix ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for the current ", Thing_className (me), U" object (self).\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushNumber (my v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [column] indexes.");
	}
}
static void do_selfMatrixStr1 (integer irow) {
	Daata me = theSource;
	Stackel column = pop;
	if (! me) Melder_throw (U"The name \"self$\" is restricted to formulas for objects.");
	integer icol = Stackel_getColumnNumber (column, me);
	if (my v_hasGetVectorStr ()) {
		pushString (Melder_dup (my v_getVectorStr (icol)));
	} else if (my v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for the current ", Thing_className (me), U" object (self).\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushString (Melder_dup (my v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [column] indexes.");
	}
}
static void do_objectCell1 (integer irow) {
	Stackel column = pop, object = pop;
	Daata thee = object->object;
	integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVector ()) {
		pushNumber (thy v_getVector (irow, icol));
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, row, column].");
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes.");
	}
}
static void do_matrix1 (integer irow) {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop;
	integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVector ()) {
		pushNumber (thy v_getVector (irow, icol));
	} else if (thy v_hasGetMatrix ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushNumber (thy v_getMatrix (irow, icol));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes.");
	}
}
static void do_objectCellStr1 (integer irow) {
	Stackel column = pop, object = pop;
	Daata thee = object->object;
	integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVectorStr ()) {
		pushString (Melder_dup (thy v_getVectorStr (icol)));
	} else if (thy v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, row, column].");
		} else {
			pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes for string cells.");
	}
}
static void do_matrixStr1 (integer irow) {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop;
	integer icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVectorStr ()) {
		pushString (Melder_dup (thy v_getVectorStr (icol)));
	} else if (thy v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes for string cells.");
	}
}
static void do_selfMatrix2 () {
	Daata me = theSource;
	Stackel column = pop, row = pop;
	if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
	integer irow = Stackel_getRowNumber (row, me);
	integer icol = Stackel_getColumnNumber (column, me);
	if (! my v_hasGetMatrix ())
		Melder_throw (Thing_className (me), U" objects like \"self\" accept no [row, column] indexing.");
	pushNumber (my v_getMatrix (irow, icol));
}
static void do_selfMatrixStr2 () {
	Daata me = theSource;
	Stackel column = pop, row = pop;
	if (! me) Melder_throw (U"The name \"self$\" is restricted to formulas for objects.");
	integer irow = Stackel_getRowNumber (row, me);
	integer icol = Stackel_getColumnNumber (column, me);
	if (! my v_hasGetMatrixStr ())
		Melder_throw (Thing_className (me), U" objects like \"self$\" accept no [row, column] indexing for string cells.");
	pushString (Melder_dup (my v_getMatrixStr (irow, icol)));
}
static void do_objectCell2 () {
	Stackel column = pop, row = pop, object = pop;
	Daata thee = object->object;
	integer irow = Stackel_getRowNumber (row, thee);
	integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrix ())
		Melder_throw (Thing_className (thee), U" objects accept no [id, row, column] indexing.");
	pushNumber (thy v_getMatrix (irow, icol));
}
static void do_matrix2 () {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	integer irow = Stackel_getRowNumber (row, thee);
	integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrix ())
		Melder_throw (Thing_className (thee), U" objects accept no [row, column] indexing.");
	pushNumber (thy v_getMatrix (irow, icol));
}
static void do_objectCellStr2 () {
	Stackel column = pop, row = pop, object = pop;
	Daata thee = object->object;
	integer irow = Stackel_getRowNumber (row, thee);
	integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrixStr ())
		Melder_throw (Thing_className (thee), U" objects accept no [id, row, column] indexing for string cells.");
	pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
}
static void do_matrixStr2 () {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	integer irow = Stackel_getRowNumber (row, thee);
	integer icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrixStr ())
		Melder_throw (Thing_className (thee), U" objects accept no [row, column] indexing for string cells.");
	pushString (Melder_dup (thy v_getMatrixStr (irow, icol)));
}
static void do_objectLocation0 (integer irow, integer icol) {
	Stackel object = pop;
	Daata thee = object->object;
	if (thy v_hasGetFunction0 ()) {
		pushNumber (thy v_getFunction0 ());
	} else if (thy v_hasGetFunction1 ()) {
		Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using: object (id, x).");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me),
				U" object gives no implicit x values,\nhence no implicit x value for this ",
				Thing_className (thee), " object.\n"
				U"Try using: object (id, x).");
		double x = my v_getX (icol);
		pushNumber (thy v_getFunction1 (irow, x));
	} else if (thy v_hasGetFunction2 ()) {
		Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x or y values for this ", Thing_className (thee), U" object.\n"
				U"Try using: object (id, x, y).");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit x values,\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using: object (id, x, y).");
		double x = my v_getX (icol);
		if (! my v_hasGetY ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using: object (id, y).");
		double y = my v_getY (irow);
		pushNumber (thy v_getFunction2 (x, y));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no () values.");
	}
}
static void do_function0 (integer irow, integer icol) {
	Daata thee = parse [programPointer]. content.object;
	if (thy v_hasGetFunction0 ()) {
		pushNumber (thy v_getFunction0 ());
	} else if (thy v_hasGetFunction1 ()) {
		Daata me = theSource;
		if (!me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using the (x) argument explicitly.");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me),
				U" object gives no implicit x values,\nhence no implicit x value for this ",
				Thing_className (thee), U" object.\n"
				U"Try using the (x) argument explicitly.");
		double x = my v_getX (icol);
		pushNumber (thy v_getFunction1 (irow, x));
	} else if (thy v_hasGetFunction2 ()) {
		Daata me = theSource;
		if (! me)
			Melder_throw (U"No current object (we are not in a Formula command),\n"
				U"hence no implicit x or y values for this ", Thing_className (thee), U" object.\n"
				U"Try using both (x, y) arguments explicitly.");
		if (! my v_hasGetX ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit x values,\n"
				U"hence no implicit x value for this ", Thing_className (thee), U" object.\n"
				U"Try using both (x, y) arguments explicitly.");
		double x = my v_getX (icol);
		if (! my v_hasGetY ())
			Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using the (y) argument explicitly.");
		double y = my v_getY (irow);
		pushNumber (thy v_getFunction2 (x, y));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no () values.");
	}
}
static void do_selfFunction1 (integer irow) {
	Daata me = theSource;
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
		if (my v_hasGetFunction1 ()) {
			pushNumber (my v_getFunction1 (irow, x->number));
		} else if (my v_hasGetFunction2 ()) {
			if (! my v_hasGetY ())
				Melder_throw (U"The current ", Thing_className (me), U" object (self) accepts no implicit y values.\n"
					U"Try using both (x, y) arguments instead.");
			double y = my v_getY (irow);
			pushNumber (my v_getFunction2 (x->number, y));
		} else {
			Melder_throw (Thing_className (me), U" objects like \"self\" accept no (x) values.");
		}
	} else {
		Melder_throw (Thing_className (me), U" objects like \"self\" accept only numeric x values.");
	}
}
static void do_objectLocation1 (integer irow) {
	Stackel x = pop, object = pop;
	Daata thee = object->object;
	if (x->which == Stackel_NUMBER) {
		if (thy v_hasGetFunction1 ()) {
			pushNumber (thy v_getFunction1 (irow, x->number));
		} else if (thy v_hasGetFunction2 ()) {
			Daata me = theSource;
			if (! me)
				Melder_throw (U"No current object (we are not in a Formula command),\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using: object (id, x, y).");
			if (! my v_hasGetY ())
				Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using: object (id, x, y).");
			double y = my v_getY (irow);
			pushNumber (thy v_getFunction2 (x->number, y));
		} else {
			Melder_throw (Thing_className (thee), U" objects accept no (x) values.");
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_function1 (integer irow) {
	Daata thee = parse [programPointer]. content.object;
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		if (thy v_hasGetFunction1 ()) {
			pushNumber (thy v_getFunction1 (irow, x->number));
		} else if (thy v_hasGetFunction2 ()) {
			Daata me = theSource;
			if (! me)
				Melder_throw (U"No current object (we are not in a Formula command),\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using both (x, y) arguments instead.");
			if (! my v_hasGetY ())
				Melder_throw (U"The current ", Thing_className (me), U" object gives no implicit y values,\n"
					U"hence no implicit y value for this ", Thing_className (thee), U" object.\n"
					U"Try using both (x, y) arguments instead.");
			double y = my v_getY (irow);
			pushNumber (thy v_getFunction2 (x->number, y));
		} else {
			Melder_throw (Thing_className (thee), U" objects accept no (x) values.");
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_selfFunction2 () {
	Daata me = theSource;
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
		if (! my v_hasGetFunction2 ())
			Melder_throw (Thing_className (me), U" objects like \"self\" accept no (x, y) values.");
		pushNumber (my v_getFunction2 (x->number, y->number));
	} else {
		Melder_throw (Thing_className (me), U" objects accept only numeric x values.");
	}
}
static void do_objectLocation2 () {
	Stackel y = pop, x = pop, object = pop;
	Daata thee = object->object;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (! thy v_hasGetFunction2 ())
			Melder_throw (Thing_className (thee), U" objects accept no (x, y) values.");
		pushNumber (thy v_getFunction2 (x->number, y->number));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_function2 () {
	Daata thee = parse [programPointer]. content.object;
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (! thy v_hasGetFunction2 ())
			Melder_throw (Thing_className (thee), U" objects accept no (x, y) values.");
		pushNumber (thy v_getFunction2 (x->number, y->number));
	} else {
		Melder_throw (Thing_className (thee), U" objects accept only numeric x values.");
	}
}
static void do_rowStr () {
	Daata thee = parse [programPointer]. content.object;
	Stackel row = pop;
	integer irow = Stackel_getRowNumber (row, thee);
	autostring32 result = Melder_dup (thy v_getRowStr (irow));
	if (! result)
		Melder_throw (U"Row index out of bounds.");
	pushString (result.move());
}
static void do_colStr () {
	Daata thee = parse [programPointer]. content.object;
	Stackel col = pop;
	integer icol = Stackel_getColumnNumber (col, thee);
	autostring32 result = Melder_dup (thy v_getColStr (icol));
	if (! result)
		Melder_throw (U"Column index out of bounds.");
	pushString (result.move());
}

static double NUMarcsinh (double x) {
	//Melder_casual (U"NUMarcsinh ", fileno(stdout));
	return log (x + sqrt (1.0 + x * x));
}
static double NUMarccosh (double x) {
	return x < 1.0 ? undefined : log (x + sqrt (x * x - 1.0));
}
static double NUMarctanh (double x) {
	return x <= -1.0 || x >= 1.0 ? undefined : 0.5 * log ((1.0 + x) / (1.0 - x));
}
static double NUMerf (double x) {
	return 1.0 - NUMerfcc (x);
}

void Formula_run (integer row, integer col, Formula_Result *result) {
	FormulaInstruction f = parse;
	programPointer = 1;   // first symbol of the program
	if (! theStack) {
		theStack = Melder_calloc_f (struct structStackel, 1+Formula_MAXIMUM_STACK_SIZE);
		if (! theStack)
			Melder_throw (U"Out of memory during formula computation.");
	}
	w = 0;   // start new stack
	wmax = 0;   // start new stack
	try {
		while (programPointer <= numberOfInstructions) {
			int symbol;
				switch (symbol = f [programPointer]. symbol) {

case NUMBER_: { pushNumber (f [programPointer]. content.number);
} break; case STOPWATCH_: { pushNumber (Melder_stopwatch ());
} break; case ROW_: { pushNumber (row);
} break; case COL_: { pushNumber (col);
} break; case X_: {
	Daata me = theSource;
	if (! my v_hasGetX ()) Melder_throw (U"No values for \"x\" for this object.");
	pushNumber (my v_getX (col));
} break; case Y_: {
	Daata me = theSource;
	if (! my v_hasGetY ()) Melder_throw (U"No values for \"y\" for this object.");
	pushNumber (my v_getY (row));
} break; case NOT_: { do_not ();
} break; case EQ_: { do_eq ();
} break; case NE_: { do_ne ();
} break; case LE_: { do_le ();
} break; case LT_: { do_lt ();
} break; case GE_: { do_ge ();
} break; case GT_: { do_gt ();
} break; case ADD_: { do_add ();
} break; case SUB_: { do_sub ();
} break; case MUL_: { do_mul ();
} break; case RDIV_: { do_rdiv ();
} break; case IDIV_: { do_idiv ();
} break; case MOD_: { do_mod ();
} break; case MINUS_: { do_minus ();
} break; case POWER_: { do_power ();
/********** Functions of 1 variable: **********/
} break; case ABS_: { do_abs ();
} break; case ROUND_: { do_round ();
} break; case FLOOR_: { do_floor ();
} break; case CEILING_: { do_ceiling ();
} break; case RECTIFY_: { do_rectify ();
} break; case RECTIFY_H_: { do_rectifyH ();
} break; case RECTIFY_HH_: { do_rectifyHH ();
} break; case SQRT_: { do_sqrt ();
} break; case SIN_: { do_sin ();
} break; case COS_: { do_cos ();
} break; case TAN_: { do_tan ();
} break; case ARCSIN_: { do_arcsin ();
} break; case ARCCOS_: { do_arccos ();
} break; case ARCTAN_: { do_arctan ();
} break; case SINC_: { do_function_n_n (NUMsinc);
} break; case SINCPI_: { do_function_n_n (NUMsincpi);
} break; case EXP_: { do_exp ();
} break; case VEC_EXP_: { do_VECexp ();
} break; case MAT_EXP_: { do_MATexp ();
} break; case SINH_: { do_sinh ();
} break; case COSH_: { do_cosh ();
} break; case TANH_: { do_tanh ();
} break; case ARCSINH_: { do_function_n_n (NUMarcsinh);
} break; case ARCCOSH_: { do_function_n_n (NUMarccosh);
} break; case ARCTANH_: { do_function_n_n (NUMarctanh);
} break; case SIGMOID_: { do_function_n_n (NUMsigmoid);
} break; case VEC_SIGMOID_: { do_functionvec_n_n (NUMsigmoid);
} break; case SOFTMAX_H_: { do_softmaxH ();
} break; case SOFTMAX_PER_ROW_HH_: { do_softmaxPerRowHH ();
} break; case INV_SIGMOID_: { do_function_n_n (NUMinvSigmoid);
} break; case ERF_: { do_function_n_n (NUMerf);
} break; case ERFC_: { do_function_n_n (NUMerfcc);
} break; case GAUSS_P_: { do_function_n_n (NUMgaussP);
} break; case GAUSS_Q_: { do_function_n_n (NUMgaussQ);
} break; case INV_GAUSS_Q_: { do_function_n_n (NUMinvGaussQ);
} break; case RANDOM_BERNOULLI_: { do_function_n_n (NUMrandomBernoulli_real);
} break; case VEC_RANDOM_BERNOULLI_: { do_functionvec_n_n (NUMrandomBernoulli_real);
} break; case RANDOM_POISSON_: { do_function_n_n (NUMrandomPoisson);
} break; case MAT_TRANSPOSE_: { do_MATtranspose ();
} break; case SUM_PER_ROW_H_: { do_rowSumsH ();
} break; case SUM_PER_COLUMN_H_: { do_columnSumsH ();
} break; case LOG2_: { do_log2 ();
} break; case LN_: { do_ln ();
} break; case LOG10_: { do_log10 ();
} break; case LN_GAMMA_: { do_function_n_n (NUMlnGamma);
} break; case HERTZ_TO_BARK_: { do_function_n_n (NUMhertzToBark);
} break; case BARK_TO_HERTZ_: { do_function_n_n (NUMbarkToHertz);
} break; case PHON_TO_DIFFERENCE_LIMENS_: { do_function_n_n (NUMphonToDifferenceLimens);
} break; case DIFFERENCE_LIMENS_TO_PHON_: { do_function_n_n (NUMdifferenceLimensToPhon);
} break; case HERTZ_TO_MEL_: { do_function_n_n (NUMhertzToMel);
} break; case MEL_TO_HERTZ_: { do_function_n_n (NUMmelToHertz);
} break; case HERTZ_TO_SEMITONES_: { do_function_n_n (NUMhertzToSemitones);
} break; case SEMITONES_TO_HERTZ_: { do_function_n_n (NUMsemitonesToHertz);
} break; case ERB_: { do_function_n_n (NUMerb);
} break; case HERTZ_TO_ERB_: { do_function_n_n (NUMhertzToErb);
} break; case ERB_TO_HERTZ_: { do_function_n_n (NUMerbToHertz);
} break; case SUM_: { do_sum ();
} break; case MEAN_: { do_mean ();
} break; case STDEV_: { do_stdev ();
} break; case CENTER_: { do_center ();
} break; case EVALUATE_: { do_evaluate ();
} break; case EVALUATE_NOCHECK_: { do_evaluate_nocheck ();
} break; case EVALUATE_STR_: { do_evaluateStr ();
} break; case EVALUATE_NOCHECK_STR_: { do_evaluate_nocheckStr ();
/********** Functions of 2 numerical variables: **********/
} break; case ARCTAN2_: { do_function_dd_d (atan2);
} break; case RANDOM_UNIFORM_: { do_function_dd_d (NUMrandomUniform);
} break; case RANDOM_INTEGER_: { do_function_ll_l (NUMrandomInteger);
} break; case RANDOM_GAUSS_: { do_function_dd_d (NUMrandomGauss);
} break; case RANDOM_BINOMIAL_: { do_function_dl_d (NUMrandomBinomial_real);
} break; case CHI_SQUARE_P_: { do_function_dd_d (NUMchiSquareP);
} break; case CHI_SQUARE_Q_: { do_function_dd_d (NUMchiSquareQ);
} break; case INCOMPLETE_GAMMAP_: { do_function_dd_d (NUMincompleteGammaP);
} break; case INV_CHI_SQUARE_Q_: { do_function_dd_d (NUMinvChiSquareQ);
} break; case STUDENT_P_: { do_function_dd_d (NUMstudentP);
} break; case STUDENT_Q_: { do_function_dd_d (NUMstudentQ);
} break; case INV_STUDENT_Q_: { do_function_dd_d (NUMinvStudentQ);
} break; case BETA_: { do_function_dd_d (NUMbeta);
} break; case BETA2_: { do_function_dd_d (NUMbeta2);
} break; case BESSEL_I_: { do_function_ld_d (NUMbesselI);
} break; case BESSEL_K_: { do_function_ld_d (NUMbesselK);
} break; case LN_BETA_: { do_function_dd_d (NUMlnBeta);
} break; case SOUND_PRESSURE_TO_PHON_: { do_function_dd_d (NUMsoundPressureToPhon);
} break; case OBJECTS_ARE_IDENTICAL_: { do_objects_are_identical ();
/********** Functions of 3 numerical variables: **********/
} break; case FISHER_P_: { do_function_ddd_d (NUMfisherP);
} break; case FISHER_Q_: { do_function_ddd_d (NUMfisherQ);
} break; case INV_FISHER_Q_: { do_function_ddd_d (NUMinvFisherQ);
} break; case BINOMIAL_P_: { do_function_ddd_d (NUMbinomialP);
} break; case BINOMIAL_Q_: { do_function_ddd_d (NUMbinomialQ);
} break; case INCOMPLETE_BETA_: { do_function_ddd_d (NUMincompleteBeta);
} break; case INV_BINOMIAL_P_: { do_function_ddd_d (NUMinvBinomialP);
} break; case INV_BINOMIAL_Q_: { do_function_ddd_d (NUMinvBinomialQ);
/********** Functions of a variable number of variables: **********/
} break; case DO_   : { do_do    ();
} break; case DOSTR_: { do_doStr ();
} break; case WRITE_INFO_      : { do_writeInfo      ();
} break; case WRITE_INFO_LINE_ : { do_writeInfoLine  ();
} break; case APPEND_INFO_     : { do_appendInfo     ();
} break; case APPEND_INFO_LINE_: { do_appendInfoLine ();
} break; case WRITE_FILE_      : { do_writeFile      ();
} break; case WRITE_FILE_LINE_ : { do_writeFileLine  ();
} break; case APPEND_FILE_     : { do_appendFile     ();
} break; case APPEND_FILE_LINE_: { do_appendFileLine ();
} break; case PAUSE_SCRIPT_: { do_pauseScript ();
} break; case EXIT_SCRIPT_: { do_exitScript ();
} break; case RUN_SCRIPT_: { do_runScript ();
} break; case RUN_SYSTEM_: { do_runSystem ();
} break; case RUN_SYSTEM_NOCHECK_: { do_runSystem_nocheck ();
} break; case RUN_SUBPROCESS_: { do_runSubprocess ();
} break; case MIN_: { do_min ();
} break; case MAX_: { do_max ();
} break; case IMIN_: { do_imin ();
} break; case IMAX_: { do_imax ();
} break; case NORM_: { do_norm ();
} break; case VEC_ZERO_: { do_VECzero ();
} break; case MAT_ZERO_: { do_MATzero ();
} break; case VEC_LINEAR_: { do_VEClinear ();
} break; case VEC_TO_: { do_VECto ();
} break; case VEC_FROM_TO_: { do_VECfrom_to ();
} break; case VEC_FROM_TO_BY_: { do_VECfrom_to_by ();
} break; case VEC_RANDOM_UNIFORM_: { do_function_VECdd_d (NUMrandomUniform);
} break; case MAT_RANDOM_UNIFORM_: { do_function_MATdd_d (NUMrandomUniform);
} break; case VEC_RANDOM_INTEGER_: { do_function_VECll_l (NUMrandomInteger);
} break; case MAT_RANDOM_INTEGER_: { do_function_MATll_l (NUMrandomInteger);
} break; case VEC_RANDOM_GAUSS_: { do_function_VECdd_d (NUMrandomGauss);
} break; case MAT_RANDOM_GAUSS_: { do_function_MATdd_d (NUMrandomGauss);
} break; case MAT_PEAKS_: { do_MATpeaks ();
} break; case SIZE_: { do_size ();
} break; case NUMBER_OF_ROWS_: { do_numberOfRows ();
} break; case NUMBER_OF_COLUMNS_: { do_numberOfColumns ();
} break; case EDITOR_: { do_editor ();
} break; case HASH_: { do_hash ();
/********** String functions: **********/
} break; case LENGTH_: { do_length ();
} break; case STRING_TO_NUMBER_: { do_number ();
} break; case FILE_READABLE_: { do_fileReadable ();
} break; case DATESTR_: { do_STRdate ();
} break; case INFOSTR_: { do_infoStr ();
} break; case LEFTSTR_: { do_STRleft ();
} break; case RIGHTSTR_: { do_STRright ();
} break; case MIDSTR_: { do_STRmid ();
} break; case UNICODE_TO_BACKSLASH_TRIGRAPHS_: { do_unicodeToBackslashTrigraphsStr ();
} break; case BACKSLASH_TRIGRAPHS_TO_UNICODE_: { do_backslashTrigraphsToUnicodeStr ();
} break; case ENVIRONMENTSTR_: { do_environmentStr ();
} break; case INDEX_: { do_index ();
} break; case RINDEX_: { do_rindex ();
} break; case STARTS_WITH_: { do_stringMatchesCriterion (kMelder_string::STARTS_WITH);
} break; case ENDS_WITH_: { do_stringMatchesCriterion (kMelder_string::ENDS_WITH);
} break; case REPLACESTR_: { do_STRreplace ();
} break; case INDEX_REGEX_: { do_index_regex (false);
} break; case RINDEX_REGEX_: { do_index_regex (true);
} break; case REPLACE_REGEXSTR_: { do_STRreplace_regex ();
} break; case EXTRACT_NUMBER_: { do_extractNumber ();
} break; case EXTRACT_WORDSTR_: { do_extractTextStr (true);
} break; case EXTRACT_LINESTR_: { do_extractTextStr (false);
} break; case SELECTED_: { do_selected ();
} break; case SELECTEDSTR_: { do_selectedStr ();
} break; case NUMBER_OF_SELECTED_: { do_numberOfSelected ();
} break; case VEC_SELECTED_: { do_VECselected ();
} break; case SELECT_OBJECT_: { do_selectObject ();
} break; case PLUS_OBJECT_  : { do_plusObject   ();
} break; case MINUS_OBJECT_ : { do_minusObject  ();
} break; case REMOVE_OBJECT_: { do_removeObject ();
} break; case OBJECT_XMIN_: { do_object_xmin ();
} break; case OBJECT_XMAX_: { do_object_xmax ();
} break; case OBJECT_YMIN_: { do_object_ymin ();
} break; case OBJECT_YMAX_: { do_object_ymax ();
} break; case OBJECT_NX_: { do_object_nx ();
} break; case OBJECT_NY_: { do_object_ny ();
} break; case OBJECT_DX_: { do_object_dx ();
} break; case OBJECT_DY_: { do_object_dy ();
} break; case OBJECT_NROW_: { do_object_nrow ();
} break; case OBJECT_NCOL_: { do_object_ncol ();
} break; case OBJECT_ROWSTR_: { do_object_rowstr ();
} break; case OBJECT_COLSTR_: { do_object_colstr ();
} break; case STRINGSTR_: { do_stringStr ();
} break; case SLEEP_: { do_sleep ();
} break; case UNICODE_: { do_unicode ();
} break; case UNICODESTR_: { do_unicodeStr ();
} break; case FIXEDSTR_: { do_fixedStr ();
} break; case PERCENTSTR_: { do_percentStr ();
} break; case HEXADECIMALSTR_: { do_hexadecimalStr ();
} break; case DELETE_FILE_: { do_deleteFile ();
} break; case CREATE_DIRECTORY_: { do_createDirectory ();
} break; case VARIABLE_EXISTS_: { do_variableExists ();
} break; case READ_FILE_: { do_readFile ();
} break; case READ_FILESTR_: { do_readFileStr ();
/********** Matrix functions: **********/
} break; case INNER_: { do_inner ();
} break; case MAT_OUTER_: { do_MATouter ();
} break; case VEC_MUL_: { do_VECmul ();
} break; case MAT_MUL_: { do_MATmul ();
} break; case MAT_MUL_FAST_: { do_MATmul_fast ();
} break; case MAT_MUL_METAL_: { do_MATmul_metal ();
} break; case MAT_MUL_TN_: { do_MATmul_tn ();
} break; case MAT_MUL_NT_: { do_MATmul_nt ();
} break; case MAT_MUL_TT_: { do_MATmul_tt ();
} break; case VEC_REPEAT_: { do_VECrepeat ();
} break; case VEC_ROW_INNERS_: { do_VECrowInners ();
/********** Pause window functions: **********/
} break; case BEGIN_PAUSE_FORM_: { do_beginPauseForm ();
} break; case PAUSE_FORM_ADD_REAL_: { do_pauseFormAddReal ();
} break; case PAUSE_FORM_ADD_POSITIVE_: { do_pauseFormAddPositive ();
} break; case PAUSE_FORM_ADD_INTEGER_: { do_pauseFormAddInteger ();
} break; case PAUSE_FORM_ADD_NATURAL_: { do_pauseFormAddNatural ();
} break; case PAUSE_FORM_ADD_WORD_: { do_pauseFormAddWord ();
} break; case PAUSE_FORM_ADD_SENTENCE_: { do_pauseFormAddSentence ();
} break; case PAUSE_FORM_ADD_TEXT_: { do_pauseFormAddText ();
} break; case PAUSE_FORM_ADD_BOOLEAN_: { do_pauseFormAddBoolean ();
} break; case PAUSE_FORM_ADD_CHOICE_: { do_pauseFormAddChoice ();
} break; case PAUSE_FORM_ADD_OPTION_MENU_: { do_pauseFormAddOptionMenu ();
} break; case PAUSE_FORM_ADD_OPTION_: { do_pauseFormAddOption ();
} break; case PAUSE_FORM_ADD_COMMENT_: { do_pauseFormAddComment ();
} break; case END_PAUSE_FORM_: { do_endPauseForm ();
} break; case CHOOSE_READ_FILESTR_: { do_chooseReadFileStr ();
} break; case CHOOSE_WRITE_FILESTR_: { do_chooseWriteFileStr ();
} break; case CHOOSE_DIRECTORYSTR_: { do_chooseDirectoryStr ();
/********** Demo window functions: **********/
} break; case DEMO_WINDOW_TITLE_: { do_demoWindowTitle ();
} break; case DEMO_SHOW_: { do_demoShow ();
} break; case DEMO_WAIT_FOR_INPUT_: { do_demoWaitForInput ();
} break; case DEMO_PEEK_INPUT_: { do_demoPeekInput ();
} break; case DEMO_INPUT_: { do_demoInput ();
} break; case DEMO_CLICKED_IN_: { do_demoClickedIn ();
} break; case DEMO_CLICKED_: { do_demoClicked ();
} break; case DEMO_X_: { do_demoX ();
} break; case DEMO_Y_: { do_demoY ();
} break; case DEMO_KEY_PRESSED_: { do_demoKeyPressed ();
} break; case DEMO_KEY_: { do_demoKey ();
} break; case DEMO_SHIFT_KEY_PRESSED_: { do_demoShiftKeyPressed ();
} break; case DEMO_COMMAND_KEY_PRESSED_: { do_demoCommandKeyPressed ();
} break; case DEMO_OPTION_KEY_PRESSED_: { do_demoOptionKeyPressed ();
} break; case DEMO_EXTRA_CONTROL_KEY_PRESSED_: { do_demoExtraControlKeyPressed ();
/********** **********/
} break; case TRUE_: {
	pushNumber (1.0);
} break; case FALSE_: {
	pushNumber (0.0);
} break; case IFTRUE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->number != 0.0) {
/* Possible compiler BUG: some compilers cannot handle the following assignment. */
/* Those compilers will have trouble with praat's AND and OR. */
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_throw (U"A condition between \"if\" and \"then\" should be a number, not ", condition->whichText(), U".");
	}
} break; case IFFALSE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->number == 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_throw (U"A condition between \"if\" and \"then\" should be a number, not ", condition->whichText(), U".");
	}
} break; case GOTO_: {
	programPointer = f [programPointer]. content.label - theOptimize;
} break; case LABEL_: {
	;
} break; case DECREMENT_AND_ASSIGN_: {
	Stackel x = pop, v = pop;
	InterpreterVariable var = v->variable;
	var -> numericValue = x->number - 1.0;
	//Melder_casual (U"starting value ", var -> numericValue);
	pushVariable (var);
} break; case INCREMENT_GREATER_GOTO_: {
	//Melder_casual (U"top of loop, stack depth ", w);
	Stackel e = & theStack [w], v = & theStack [w - 1];
	Melder_assert (e->which == Stackel_NUMBER);
	Melder_assert (v->which == Stackel_VARIABLE);
	InterpreterVariable var = v->variable;
	//Melder_casual (U"loop variable ", var -> numericValue);
	var -> numericValue += 1.0;
	//Melder_casual (U"loop variable ", var -> numericValue);
	//Melder_casual (U"end value ", e->number);
	if (var -> numericValue > e->number) {
		programPointer = f [programPointer]. content.label - theOptimize;
	}
} break; case ADD_3DOWN_: {
	Stackel x = pop, s = & theStack [w - 2];
	Melder_assert (x->which == Stackel_NUMBER);
	Melder_assert (s->which == Stackel_NUMBER);
	//Melder_casual (U"to add ", x->number);
	s->number += x->number;
	//Melder_casual (U"sum ", s->number);
} break; case POP_2_: {
	w -= 2;
	//Melder_casual (U"total ", theStack[w].number);
} break; case VEC_CELL_: { do_numericVectorElement ();
} break; case MAT_CELL_: { do_numericMatrixElement ();
} break; case INDEXED_NUMERIC_VARIABLE_: { do_indexedNumericVariable ();
} break; case INDEXED_STRING_VARIABLE_: { do_indexedStringVariable ();
} break; case VARIABLE_REFERENCE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushVariable (var);
} break; case SELF0_: { do_self0 (row, col);
} break; case SELFSTR0_: { do_selfStr0 (row, col);
} break; case OBJECT_: { pushObject (f [programPointer]. content.object);
} break; case TO_OBJECT_: { do_toObject ();
} break; case SELFMATRIX1_: { do_selfMatrix1 (row);
} break; case SELFMATRIXSTR1_: { do_selfMatrixStr1 (row);
} break; case SELFMATRIX2_: { do_selfMatrix2 ();
} break; case SELFMATRIXSTR2_: { do_selfMatrixStr2 ();
} break; case SELFFUNCTION1_: { do_selfFunction1 (row);
} break; case SELFFUNCTION2_: { do_selfFunction2 ();
} break; case OBJECTCELL0_: { do_objectCell0 (row, col);
} break; case OBJECTCELL1_: { do_objectCell1 (row);
} break; case OBJECTCELLSTR1_: { do_objectCellStr1 (row);
} break; case OBJECTCELL2_: { do_objectCell2 ();
} break; case OBJECTCELLSTR2_: { do_objectCellStr2 ();
} break; case OBJECTLOCATION0_: { do_objectLocation0 (row, col);
} break; case OBJECTLOCATION1_: { do_objectLocation1 (row);
} break; case OBJECTLOCATION2_: { do_objectLocation2 ();
} break; case MATRIX0_: { do_matrix0 (row, col);
} break; case MATRIX1_: { do_matrix1 (row);
} break; case MATRIXSTR1_: { do_matrixStr1 (row);
} break; case MATRIX2_: { do_matrix2 ();
} break; case MATRIXSTR2_: { do_matrixStr2 ();
} break; case FUNCTION0_: { do_function0 (row, col);
} break; case FUNCTION1_: { do_function1 (row);
} break; case FUNCTION2_: { do_function2 ();
} break; case ROWSTR_: { do_rowStr ();
} break; case COLSTR_: { do_colStr ();
} break; case SQR_: { do_sqr ();
} break; case STRING_: {
	autostring32 string = Melder_dup (f [programPointer]. content.string);
	pushString (string.move());
} break; case TENSOR_LITERAL_: { do_tensorLiteral ();
} break; case NUMERIC_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumber (var -> numericValue);
} break; case NUMERIC_VECTOR_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumericVectorReference (var -> numericVectorValue.get());
} break; case NUMERIC_MATRIX_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumericMatrixReference (var -> numericMatrixValue.get());
} break; case STRING_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	autostring32 string = Melder_dup (var -> stringValue.get());
	pushString (string.move());
} break; default: Melder_throw (U"Symbol \"", Formula_instructionNames [parse [programPointer]. symbol], U"\" without action.");
			} // endswitch
			programPointer ++;
		} // endwhile
		if (w != 1)
			Melder_fatal (U"Formula: stackpointer ends at ", w, U" instead of 1.");
		/*
			Move the result from the stack to `result`.
		*/
		result -> reset();
		if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC) {
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a numeric expression.");
			Melder_assert (theStack [1]. which == Stackel_NUMBER);
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
			result -> numericResult = theStack [1]. number;
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_STRING) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression (value ", theStack [1]. number, U") instead of a string expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a string expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a string expression.");
			Melder_assert (theStack [1]. which == Stackel_STRING);
			result -> expressionType = kFormula_EXPRESSION_TYPE_STRING;
			Melder_assert (! result -> stringResult);
			result -> stringResult = theStack [1]. moveString();
			Melder_assert (theStack [1]. which == Stackel_STRING);
			Melder_assert (! theStack [1]. getString());
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Found a matrix expression instead of a vector expression.");
			Melder_assert (theStack [1]. which == Stackel_NUMERIC_VECTOR);
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR;
			result -> numericVectorResult = theStack [1]. numericVector;
			result -> owned = theStack [1]. owned;
			theStack [1]. owned = false;
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_STRING)
				Melder_throw (U"Found a string expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Found a vector expression instead of a matrix expression.");
			Melder_assert (theStack [1]. which == Stackel_NUMERIC_MATRIX);
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX;
			result -> numericMatrixResult = theStack [1]. numericMatrix;
			result -> owned = theStack [1]. owned;
			theStack [1]. owned = false;
		} else {
			Melder_assert (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_UNKNOWN);
			if (theStack [1]. which == Stackel_NUMBER) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
				result -> numericResult = theStack [1]. number;
			} else if (theStack [1]. which == Stackel_STRING) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_STRING;
				Melder_assert (! result -> stringResult);
				result -> stringResult = theStack [1]. moveString();
				Melder_assert (theStack [1]. which == Stackel_STRING);
				Melder_assert (! theStack [1]. getString());
			} else if (theStack [1]. which == Stackel_NUMERIC_VECTOR) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR;
				result -> numericVectorResult = theStack [1]. numericVector;
				result -> owned = theStack [1]. owned;
				theStack [1]. owned = false;
			} else if (theStack [1]. which == Stackel_NUMERIC_MATRIX) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX;
				result -> numericMatrixResult = theStack [1]. numericMatrix;
				result -> owned = theStack [1]. owned;
				theStack [1]. owned = false;
			} else {
				Melder_throw (U"Don't know yet how to write ", theStack [1]. whichText(), U".");
			}
		}
		/*
			Clean up the stack (theStack [1] has probably been disowned).
		*/
		for (w = wmax; w > 0; w --)
			theStack [w]. reset();
	} catch (MelderError) {
		/*
			Clean up the stack (theStack [1] has probably not been disowned).
		*/
		for (w = wmax; w > 0; w --)
			theStack [w]. reset();
		if (Melder_hasError (U"Script exited.")) {
			throw;
		} else {
			Melder_throw (U"Formula not run.");
		}
	}
}

/* End of file Formula.cpp */
