/* Formula.cpp
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

#include <ctype.h>
#include <time.h>
#if defined (UNIX)
	#include <sys/stat.h>
#endif
#include "NUM2.h"
#include "regularExp.h"
#include "Formula.h"
#include "Interpreter.h"
#include "Ui.h"
#include "praatP.h"
#include "praat_script.h"
#include "UnicodeData.h"
#include "longchar.h"
#include "UiPause.h"
#include "DemoEditor.h"

static Interpreter theInterpreter;
static autoInterpreter theLocalInterpreter;
static Daata theSource;
static const char32 *theExpression;
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
		//struct Formula_NumericArray numericArray;
		Daata object;
		InterpreterVariable variable;
	} content;
} *FormulaInstruction;

static FormulaInstruction lexan, parse;
static int ilabel, ilexan, iparse, numberOfInstructions, numberOfStringConstants;

enum { GEENSYMBOOL_,

/* First, all symbols after which "-" is unary. */
/* The list ends with "MINUS_" itself. */

	/* Haakjes-openen. */
	IF_, THEN_, ELSE_, HAAKJEOPENEN_, RECHTEHAAKOPENEN_, OPENING_BRACE_, KOMMA_, COLON_, FROM_, TO_,
	/* Operatoren met boolean resultaat. */
	OR_, AND_, NOT_, EQ_, NE_, LE_, LT_, GE_, GT_,
	/* Operatoren met reeel resultaat. */
	ADD_, SUB_, MUL_, RDIV_, IDIV_, MOD_, POWER_, CALL_, MINUS_,

/* Then, the symbols after which "-" is binary. */

	/* Haakjes-sluiten. */
	ENDIF_, FI_, HAAKJESLUITEN_, RECHTEHAAKSLUITEN_, CLOSING_BRACE_,
	/* Dingen met een waarde. */
	#define LOW_VALUE  NUMBER_
		NUMBER_, NUMBER_PI_, NUMBER_E_, NUMBER_UNDEFINED_,
		/* Attributes of objects. */
		#define LOW_ATTRIBUTE  XMIN_
			XMIN_, XMAX_, YMIN_, YMAX_, NX_, NY_, DX_, DY_,
			ROW_, COL_, NROW_, NCOL_, ROWSTR_, COLSTR_, Y_, X_,
		#define HIGH_ATTRIBUTE  X_
	#define HIGH_VALUE  HIGH_ATTRIBUTE

	SELF_, SELFSTR_, OBJECT_, OBJECTSTR_, MATRIKS_, MATRIKSSTR_,
	STOPWATCH_,

/* The following symbols can be followed by "-" only if they are a variable. */

	/* Functions of 1 variable; if you add, update the #defines. */
	#define LOW_FUNCTION_1  ABS_
		ABS_, ROUND_, FLOOR_, CEILING_,
		RECTIFY_, RECTIFY_NUMVEC_,
		SQRT_, SIN_, COS_, TAN_, ARCSIN_, ARCCOS_, ARCTAN_, SINC_, SINCPI_,
		EXP_, EXP_NUMVEC_, EXP_NUMMAT_,
		SINH_, COSH_, TANH_, ARCSINH_, ARCCOSH_, ARCTANH_,
		SIGMOID_, SIGMOID_NUMVEC_, SOFTMAX_NUMVEC_,
		INV_SIGMOID_, ERF_, ERFC_, GAUSS_P_, GAUSS_Q_, INV_GAUSS_Q_,
		RANDOM_BERNOULLI_, RANDOM_BERNOULLI_NUMVEC_,
		RANDOM_POISSON_,
		LOG2_, LN_, LOG10_, LN_GAMMA_,
		HERTZ_TO_BARK_, BARK_TO_HERTZ_, PHON_TO_DIFFERENCE_LIMENS_, DIFFERENCE_LIMENS_TO_PHON_,
		HERTZ_TO_MEL_, MEL_TO_HERTZ_, HERTZ_TO_SEMITONES_, SEMITONES_TO_HERTZ_,
		ERB_, HERTZ_TO_ERB_, ERB_TO_HERTZ_,
		SUM_, MEAN_, STDEV_, CENTER_,
		STRINGSTR_, SLEEP_,
	#define HIGH_FUNCTION_1  SLEEP_

	/* Functions of 2 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_2  ARCTAN2_
		ARCTAN2_, RANDOM_UNIFORM_, RANDOM_INTEGER_, RANDOM_GAUSS_, RANDOM_BINOMIAL_,
		CHI_SQUARE_P_, CHI_SQUARE_Q_, INCOMPLETE_GAMMAP_,
		INV_CHI_SQUARE_Q_, STUDENT_P_, STUDENT_Q_, INV_STUDENT_Q_,
		BETA_, BETA2_, BESSEL_I_, BESSEL_K_, LN_BETA_,
		SOUND_PRESSURE_TO_PHON_, OBJECTS_ARE_IDENTICAL_,
		OUTER_NUMMAT_, MUL_NUMVEC_, REPEAT_NUMVEC_,
	#define HIGH_FUNCTION_2  REPEAT_NUMVEC_

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
		MIN_, MAX_, IMIN_, IMAX_,
		LEFTSTR_, RIGHTSTR_, MIDSTR_,
		SELECTED_, SELECTEDSTR_, NUMBER_OF_SELECTED_, SELECT_OBJECT_, PLUS_OBJECT_, MINUS_OBJECT_, REMOVE_OBJECT_,
		BEGIN_PAUSE_FORM_, PAUSE_FORM_ADD_REAL_, PAUSE_FORM_ADD_POSITIVE_, PAUSE_FORM_ADD_INTEGER_, PAUSE_FORM_ADD_NATURAL_,
		PAUSE_FORM_ADD_WORD_, PAUSE_FORM_ADD_SENTENCE_, PAUSE_FORM_ADD_TEXT_, PAUSE_FORM_ADD_BOOLEAN_,
		PAUSE_FORM_ADD_CHOICE_, PAUSE_FORM_ADD_OPTION_MENU_, PAUSE_FORM_ADD_OPTION_,
		PAUSE_FORM_ADD_COMMENT_, END_PAUSE_FORM_,
		CHOOSE_READ_FILESTR_, CHOOSE_WRITE_FILESTR_, CHOOSE_DIRECTORYSTR_,
		DEMO_WINDOW_TITLE_, DEMO_SHOW_, DEMO_WAIT_FOR_INPUT_, DEMO_PEEK_INPUT_, DEMO_INPUT_, DEMO_CLICKED_IN_,
		DEMO_CLICKED_, DEMO_X_, DEMO_Y_, DEMO_KEY_PRESSED_, DEMO_KEY_,
		DEMO_SHIFT_KEY_PRESSED_, DEMO_COMMAND_KEY_PRESSED_, DEMO_OPTION_KEY_PRESSED_, DEMO_EXTRA_CONTROL_KEY_PRESSED_,
		ZERO_NUMVEC_, ZERO_NUMMAT_,
		LINEAR_NUMVEC_, LINEAR_NUMMAT_,
		RANDOM_UNIFORM_NUMVEC_, RANDOM_UNIFORM_NUMMAT_,
		RANDOM_INTEGER_NUMVEC_, RANDOM_INTEGER_NUMMAT_,
		RANDOM_GAUSS_NUMVEC_, RANDOM_GAUSS_NUMMAT_,
		PEAKS_NUMMAT_,
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
		FIXEDSTR_, PERCENTSTR_,
	#define HIGH_STRING_FUNCTION  PERCENTSTR_

	/* Range functions. */
	#define LOW_RANGE_FUNCTION  SUM_OVER_
		SUM_OVER_,
	#define HIGH_RANGE_FUNCTION  SUM_OVER_

	#define LOW_FUNCTION  LOW_FUNCTION_1
	#define HIGH_FUNCTION  HIGH_RANGE_FUNCTION

	/* Membership operator. */
	PERIOD_,
	#define hoogsteInvoersymbool  PERIOD_

/* Symbols introduced by the parser. */

	TRUE_, FALSE_,
	GOTO_, IFTRUE_, IFFALSE_, INCREMENT_GREATER_GOTO_,
	LABEL_,
	DECREMENT_AND_ASSIGN_, ADD_3DOWN_, POP_2_,
	NUMERIC_VECTOR_ELEMENT_, NUMERIC_MATRIX_ELEMENT_, VARIABLE_REFERENCE_,
	NUMERIC_VECTOR_LITERAL_,
	SELF0_, SELFSTR0_, TO_OBJECT_,
	OBJECT_XMIN_, OBJECT_XMAX_, OBJECT_YMIN_, OBJECT_YMAX_, OBJECT_NX_, OBJECT_NY_,
	OBJECT_DX_, OBJECT_DY_, OBJECT_NROW_, OBJECT_NCOL_, OBJECT_ROWSTR_, OBJECT_COLSTR_,
	OBJECTCELL0_, OBJECTCELLSTR0_, OBJECTCELL1_, OBJECTCELLSTR1_, OBJECTCELL2_, OBJECTCELLSTR2_,
	OBJECTLOCATION0_, OBJECTLOCATIONSTR0_, OBJECTLOCATION1_, OBJECTLOCATIONSTR1_, OBJECTLOCATION2_, OBJECTLOCATIONSTR2_,
	SELFMATRIKS1_, SELFMATRIKSSTR1_, SELFMATRIKS2_, SELFMATRIKSSTR2_,
	SELFFUNKTIE1_, SELFFUNKTIESTR1_, SELFFUNKTIE2_, SELFFUNKTIESTR2_,
	MATRIKS0_, MATRIKSSTR0_, MATRIKS1_, MATRIKSSTR1_, MATRIKS2_, MATRIKSSTR2_,
	FUNKTIE0_, FUNKTIESTR0_, FUNKTIE1_, FUNKTIESTR1_, FUNKTIE2_, FUNKTIESTR2_,
	SQR_,

/* Symbols introduced by lexical analysis. */

	STRING_,
	NUMERIC_VARIABLE_, NUMERIC_VECTOR_VARIABLE_, NUMERIC_MATRIX_VARIABLE_,
	STRING_VARIABLE_, STRING_ARRAY_VARIABLE_,
	VARIABLE_NAME_, INDEXED_NUMERIC_VARIABLE_, INDEXED_STRING_VARIABLE_,
	END_
	#define hoogsteSymbool END_
};

/* The names that start with an underscore (_) do not occur in the formula text: */
/* they are used in error messages and in debugging (see Formula_print). */

static const char32 *Formula_instructionNames [1 + hoogsteSymbool] = { U"",
	U"if", U"then", U"else", U"(", U"[", U"{", U",", U":", U"from", U"to",
	U"or", U"and", U"not", U"=", U"<>", U"<=", U"<", U">=", U">",
	U"+", U"-", U"*", U"/", U"div", U"mod", U"^", U"_call", U"_neg",
	U"endif", U"fi", U")", U"]", U"}",
	U"a number", U"pi", U"e", U"undefined",
	U"xmin", U"xmax", U"ymin", U"ymax", U"nx", U"ny", U"dx", U"dy",
	U"row", U"col", U"nrow", U"ncol", U"row$", U"col$", U"y", U"x",
	U"self", U"self$", U"object", U"object$", U"_matriks", U"_matriks$",
	U"stopwatch",
	U"abs", U"round", U"floor", U"ceiling",
	U"rectify", U"rectify#",
	U"sqrt", U"sin", U"cos", U"tan", U"arcsin", U"arccos", U"arctan", U"sinc", U"sincpi",
	U"exp", U"exp#", U"exp##",
	U"sinh", U"cosh", U"tanh", U"arcsinh", U"arccosh", U"arctanh",
	U"sigmoid", U"sigmoid#", U"softmax#",
	U"invSigmoid", U"erf", U"erfc", U"gaussP", U"gaussQ", U"invGaussQ",
	U"randomBernoulli", U"randomBernoulli#",
	U"randomPoisson",
	U"log2", U"ln", U"log10", U"lnGamma",
	U"hertzToBark", U"barkToHertz", U"phonToDifferenceLimens", U"differenceLimensToPhon",
	U"hertzToMel", U"melToHertz", U"hertzToSemitones", U"semitonesToHertz",
	U"erb", U"hertzToErb", U"erbToHertz",
	U"sum", U"mean", U"stdev", U"center",
	U"string$", U"sleep",
	U"arctan2", U"randomUniform", U"randomInteger", U"randomGauss", U"randomBinomial",
	U"chiSquareP", U"chiSquareQ", U"incompleteGammaP", U"invChiSquareQ", U"studentP", U"studentQ", U"invStudentQ",
	U"beta", U"beta2", U"besselI", U"besselK", U"lnBeta",
	U"soundPressureToPhon", U"objectsAreIdentical",
	U"outer##", U"mul#", U"repeat#",
	U"fisherP", U"fisherQ", U"invFisherQ",
	U"binomialP", U"binomialQ", U"incompleteBeta", U"invBinomialP", U"invBinomialQ",

	U"do", U"do$",
	U"writeInfo", U"writeInfoLine", U"appendInfo", U"appendInfoLine",
	U"writeFile", U"writeFileLine", U"appendFile", U"appendFileLine",
	U"pauseScript", U"exitScript", U"runScript", U"runSystem", U"runSystem_nocheck", U"runSubprocess",
	U"min", U"max", U"imin", U"imax",
	U"left$", U"right$", U"mid$",
	U"selected", U"selected$", U"numberOfSelected", U"selectObject", U"plusObject", U"minusObject", U"removeObject",
	U"beginPause", U"real", U"positive", U"integer", U"natural",
	U"word", U"sentence", U"text", U"boolean",
	U"choice", U"optionMenu", U"option",
	U"comment", U"endPause",
	U"chooseReadFile$", U"chooseWriteFile$", U"chooseDirectory$",
	U"demoWindowTitle", U"demoShow", U"demoWaitForInput", U"demoPeekInput", U"demoInput", U"demoClickedIn",
	U"demoClicked", U"demoX", U"demoY", U"demoKeyPressed", U"demoKey$",
	U"demoShiftKeyPressed", U"demoCommandKeyPressed", U"demoOptionKeyPressed", U"demoExtraControlKeyPressed",
	U"zero#", U"zero##",
	U"linear#", U"linear##",
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
	U"fixed$", U"percent$",
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
	U"_selfmatriks1", U"_selfmatriks1$", U"_selfmatriks2", U"_selfmatriks2$",
	U"_selffunktie1", U"_selffunktie1$", U"_selffunktie2", U"_selffunktie2$",
	U"_matriks0", U"_matriks0$", U"_matriks1", U"_matriks1$", U"_matriks2", U"_matriks2$",
	U"_funktie0", U"_funktie0$", U"_funktie1", U"_funktie1$", U"_funktie2", U"_funktie2$",
	U"_square",
	U"_string",
	U"a numeric variable", U"a vector variable", U"a matrix variable",
	U"a string variable", U"a string array variable",
	U"a variable name", U"an indexed numeric variable", U"an indexed string variable",
	U"the end of the formula"
};

#define nieuwlabel (-- ilabel)
#define nieuwlees (lexan [++ ilexan]. symbol)
#define oudlees  (-- ilexan)

static void formulefout (const char32 *message, int position) {
	static MelderString truncatedExpression { };
	MelderString_ncopy (& truncatedExpression, theExpression, position + 1);
	Melder_throw (message, U":\n" U_LEFT_GUILLEMET U" ", truncatedExpression.string);
}

static const char32 *languageNameCompare_searchString;

static int languageNameCompare (const void *first, const void *second) {
	int i = * (int *) first, j = * (int *) second;
	return str32cmp (i == 0 ? languageNameCompare_searchString : Formula_instructionNames [i],
		j == 0 ? languageNameCompare_searchString : Formula_instructionNames [j]);
}

static int Formula_hasLanguageName (const char32 *f) {
	static int *index;
	if (! index) {
		index = NUMvector <int> (1, hoogsteInvoersymbool);
		for (int tok = 1; tok <= hoogsteInvoersymbool; tok ++) {
			index [tok] = tok;
		}
		qsort (& index [1], hoogsteInvoersymbool, sizeof (int), languageNameCompare);
	}
	if (! index) {   // linear search
		for (int tok = 1; tok <= hoogsteInvoersymbool; tok ++) {
			if (str32equ (f, Formula_instructionNames [tok])) return tok;
		}
	} else {   // binary search
		int dummy = 0, *found;
		languageNameCompare_searchString = f;
		found = (int *) bsearch (& dummy, & index [1], hoogsteInvoersymbool, sizeof (int), languageNameCompare);
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
#define nieuwkar kar = theExpression [++ ikar]
#define oudkar -- ikar

	int itok = 0;   /* Position of most recent symbol in "lexan". */
#define nieuwtok(s)  { lexan [++ itok]. symbol = s; lexan [itok]. position = ikar; }
#define tokgetal(g)  lexan [itok]. content.number = (g)
#define tokmatriks(m)  lexan [itok]. content.object = (m)

	static MelderString token { };   /* String to collect a symbol name in. */
#define stokaan MelderString_empty (& token);
#define stokkar { MelderString_appendCharacter (& token, kar); nieuwkar; }
#define stokuit (void) 0

	ilexan = iparse = ilabel = numberOfStringConstants = 0;
	do {
		nieuwkar;
		if (kar == U' ' || kar == U'\t') {
			;   // ignore spaces and tabs
		} else if (kar == U'\0') {
			nieuwtok (END_)
		} else if (kar >= U'0' && kar <= U'9') {
			stokaan;
			do stokkar while (kar >= U'0' && kar <= U'9');
			if (kar == U'.') do stokkar while (kar >= U'0' && kar <= U'9');
			if (kar == U'e' || kar == U'E') {
				kar = U'e'; stokkar
				if (kar == U'-') stokkar
				else if (kar == U'+') nieuwkar;
				if (! (kar >= U'0' && kar <= U'9'))
					formulefout (U"Missing exponent", ikar);
				do stokkar while (kar >= U'0' && kar <= U'9');
			}
			if (kar == U'%') stokkar
			stokuit;
			oudkar;
			nieuwtok (NUMBER_)
			tokgetal (Melder_atof (token.string));
		} else if ((kar >= U'a' && kar <= U'z') || kar >= 192 || (kar == U'.' &&
				((theExpression [ikar + 1] >= U'a' && theExpression [ikar + 1] <= U'z') || theExpression [ikar + 1] >= 192)
				&& (itok == 0 || (lexan [itok]. symbol != MATRIKS_ && lexan [itok]. symbol != MATRIKSSTR_
					&& lexan [itok]. symbol != RECHTEHAAKSLUITEN_)))) {
			int tok;
			bool isString = false;
			int rank = 0;
			stokaan;
			do stokkar while ((kar >= U'A' && kar <= U'Z') || (kar >= U'a' && kar <= U'z') || kar >= 192 || (kar >= U'0' && kar <= U'9') || kar == U'_' || kar == U'.');
			if (kar == '$') {
				stokkar
				isString = true;
			}
			if (kar == '#') {
				rank += 1;
				stokkar
				if (kar == '#') {
					rank += 1;
					stokkar
					if (kar == '#') {
						rank += 1;
						stokkar
						if (kar == '#') {
							rank += 1;
							stokkar
						}
					}
				}
			}
			stokuit;
			oudkar;
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
					nieuwtok (NUMBER_)
					tokgetal (NUMpi);
				} else if (tok == NUMBER_E_) {
					nieuwtok (NUMBER_)
					tokgetal (NUMe);
				} else if (tok == NUMBER_UNDEFINED_) {
					nieuwtok (NUMBER_)
					tokgetal (undefined);
				/*
				 * One very common language name must be converted to a synonym.
				 */
				} else if (tok == FI_) {
					nieuwtok (ENDIF_)
				/*
				 * Is it a function name? These may be ambiguous.
				 */
				} else if (tok >= LOW_FUNCTION && tok <= HIGH_FUNCTION) {
					/*
					 * Look ahead to find out whether the next token is a left parenthesis (or a colon).
					 */
					int jkar;
					jkar = ikar + 1;
					while (theExpression [jkar] == U' ' || theExpression [jkar] == U'\t') jkar ++;
					if (theExpression [jkar] == U'(' || theExpression [jkar] == U':') {
						nieuwtok (tok)   // this must be a function name
					} else if (theExpression [jkar] == U'[' && rank == 0) {
						if (isString) {
							nieuwtok (INDEXED_STRING_VARIABLE_)
						} else {
							nieuwtok (INDEXED_NUMERIC_VARIABLE_)
						}
						lexan [itok]. content.string = Melder_dup_f (token.string);
						numberOfStringConstants ++;
					} else {
						/*
						 * This could be a variable with the same name as a function.
						 */
						InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
						if (! var) {
							nieuwtok (VARIABLE_NAME_)
							lexan [itok]. content.string = Melder_dup_f (token.string);
							numberOfStringConstants ++;
						} else {
							if (rank == 0) {
								if (isString) {
									nieuwtok (STRING_VARIABLE_)
								} else {
									nieuwtok (NUMERIC_VARIABLE_)
								}
							} else if (rank == 1) {
								if (isString) {
									nieuwtok (STRING_ARRAY_VARIABLE_)
								} else {
									nieuwtok (NUMERIC_VECTOR_VARIABLE_)
								}
							} else if (rank == 2) {
								if (isString) {
									formulefout (U"String matrices not implemented.", ikar);
								} else {
									nieuwtok (NUMERIC_MATRIX_VARIABLE_)
								}
							} else if (rank == 3) {
								formulefout (U"Rank-3 tensors not implemented.", ikar);
							} else {
								formulefout (U"Rank-4 tensors not implemented.", ikar);
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
						nieuwtok (tok)
					} else if (theSource) {
						/*
						 * Look for ambiguity.
						 */
						if (Interpreter_hasVariable (theInterpreter, token.string))
							Melder_throw (
								U_LEFT_GUILLEMET, token.string,
								U_RIGHT_GUILLEMET U" is ambiguous: a variable or an attribute of the current object. "
								U"Please change variable name.");
						if (tok == ROW_ || tok == COL_ || tok == X_ || tok == Y_) {
							nieuwtok (tok)
						} else {
							nieuwtok (MATRIKS_)
							tokmatriks (theSource);
							nieuwtok (PERIOD_)
							nieuwtok (tok)
						}
					} else {
						/*
						 * This must be a variable, since there is no "current object" here.
						 */
						int jkar = ikar + 1;
						while (theExpression [jkar] == U' ' || theExpression [jkar] == U'\t') jkar ++;
						if (theExpression [jkar] == U'[' && rank == 0) {
							if (isString) {
								nieuwtok (INDEXED_STRING_VARIABLE_)
							} else {
								nieuwtok (INDEXED_NUMERIC_VARIABLE_)
							}
							lexan [itok]. content.string = Melder_dup_f (token.string);
							numberOfStringConstants ++;
						} else {
							InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
							if (! var) {
								nieuwtok (VARIABLE_NAME_)
								lexan [itok]. content.string = Melder_dup_f (token.string);
								numberOfStringConstants ++;
							} else {
								if (rank == 0) {
									if (isString) {
										nieuwtok (STRING_VARIABLE_)
									} else {
										nieuwtok (NUMERIC_VARIABLE_)
									}
								} else if (rank == 1) {
									if (isString) {
										nieuwtok (STRING_ARRAY_VARIABLE_)
									} else {
										nieuwtok (NUMERIC_VECTOR_VARIABLE_)
									}
								} else if (rank == 2) {
									if (isString) {
										formulefout (U"String matrices not implemented.", ikar);
									} else {
										nieuwtok (NUMERIC_MATRIX_VARIABLE_)
									}
								} else if (rank == 3) {
									formulefout (U"Rank-3 tensors not implemented.", ikar);
								} else {
									formulefout (U"Rank-4 tensors not implemented.", ikar);
								}
								lexan [itok]. content.variable = var;
							}
						}
					}
				} else {
					nieuwtok (tok)   /* This must be a language name. */
				}
			} else {
				/*
				 * token.string is not a language name
				 */
				int jkar = ikar + 1;
				while (theExpression [jkar] == U' ' || theExpression [jkar] == U'\t') jkar ++;
				if (theExpression [jkar] == U'(' || theExpression [jkar] == U':') {
					Melder_throw (
						U"Unknown function " U_LEFT_GUILLEMET, token.string, U_RIGHT_GUILLEMET U" in formula.");
				} else if (theExpression [jkar] == '[' && rank == 0) {
					if (isString) {
						nieuwtok (INDEXED_STRING_VARIABLE_)
					} else {
						nieuwtok (INDEXED_NUMERIC_VARIABLE_)
					}
					lexan [itok]. content.string = Melder_dup_f (token.string);
					numberOfStringConstants ++;
				} else {
					InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
					if (! var) {
						nieuwtok (VARIABLE_NAME_)
						lexan [itok]. content.string = Melder_dup_f (token.string);
						numberOfStringConstants ++;
					} else {
						if (rank == 0) {
							if (isString) {
								nieuwtok (STRING_VARIABLE_)
							} else {
								nieuwtok (NUMERIC_VARIABLE_)
							}
						} else if (rank == 1) {
							if (isString) {
								nieuwtok (STRING_ARRAY_VARIABLE_)
							} else {
								nieuwtok (NUMERIC_VECTOR_VARIABLE_)
							}
						} else if (rank == 2) {
							if (isString) {
								formulefout (U"String matrices not implemented.", ikar);
							} else {
								nieuwtok (NUMERIC_MATRIX_VARIABLE_)
							}
						} else if (rank == 3) {
							formulefout (U"Rank-3 tensors not implemented.", ikar);
						} else {
							formulefout (U"Rank-4 tensors not implemented.", ikar);
						}
						lexan [itok]. content.variable = var;
					}
				}
			}
		} else if (kar >= U'A' && kar <= U'Z') {
			bool endsInDollarSign = false;
			stokaan;
			do stokkar while (isalnum ((int) kar) || kar == U'_');   // TODO: allow more than just ASCII
			if (kar == U'$') { stokkar endsInDollarSign = true; }
			stokuit;
			oudkar;
			/*
			 * 'token' now contains a word that could be an object name.
			 */
			char32 *underscore = str32chr (token.string, '_');
			if (str32equ (token.string, U"Self")) {
				if (! theSource)
					formulefout (U"Cannot use \"Self\" if there is no current object.", ikar);
				nieuwtok (MATRIKS_)
				tokmatriks (theSource);
			} else if (str32equ (token.string, U"Self$")) {
				if (! theSource)
					formulefout (U"Cannot use \"Self$\" if there is no current object.", ikar);
				nieuwtok (MATRIKSSTR_)
				tokmatriks (theSource);
			} else if (! underscore) {
				Melder_throw (
					U"Unknown symbol " U_LEFT_GUILLEMET , token.string,
					U_RIGHT_GUILLEMET U" in formula "
					U"(variables start with lower case; object names contain an underscore).");
			} else if (str32nequ (token.string, U"Object_", 7)) {
				long uniqueID = a32tol (token.string + 7);
				int i = theCurrentPraatObjects -> n;
				while (i > 0 && uniqueID != theCurrentPraatObjects -> list [i]. id)
					i --;
				if (i == 0)
					formulefout (U"No such object (note: variables start with lower case)", ikar);
				nieuwtok (endsInDollarSign ? MATRIKSSTR_ : MATRIKS_)
				tokmatriks ((Daata) theCurrentPraatObjects -> list [i]. object);
			} else {
				int i = theCurrentPraatObjects -> n;
				*underscore = ' ';
				if (endsInDollarSign) token.string [-- token.length] = '\0';
				while (i > 0 && ! str32equ (token.string, theCurrentPraatObjects -> list [i]. name))
					i --;
				if (i == 0)
					formulefout (U"No such object (note: variables start with lower case)", ikar);
				nieuwtok (endsInDollarSign ? MATRIKSSTR_ : MATRIKS_)
				tokmatriks ((Daata) theCurrentPraatObjects -> list [i]. object);
			}
		} else if (kar == U'(') {
			nieuwtok (HAAKJEOPENEN_)
		} else if (kar == U')') {
			nieuwtok (HAAKJESLUITEN_)
		} else if (kar == U'+') {
			nieuwtok (ADD_)
		} else if (kar == U'-') {
			if (itok == 0 || lexan [itok]. symbol <= MINUS_) {
				nieuwtok (MINUS_)
			} else {
				nieuwtok (SUB_)
			}
		} else if (kar == U'*') {
			nieuwkar;
			if (kar == U'*') {
				nieuwtok (POWER_)   /* "**" = "^" */
			} else {
				oudkar;
				nieuwtok (MUL_)
			}
		} else if (kar == U'/') {
			nieuwkar;
			if (kar == U'=') {
				nieuwtok (NE_)   /* "/=" = "<>" */
			} else {
				oudkar;
				nieuwtok (RDIV_)
			}
		} else if (kar == U'=') {
			nieuwtok (EQ_)   /* "=" */
			nieuwkar;
			if (kar != U'=') {
				oudkar;   /* "==" = "=" */
			}
		} else if (kar == U'>') {
			nieuwkar;
			if (kar == U'=') {
				nieuwtok (GE_)
			} else {
				oudkar;
				nieuwtok (GT_)
			}
		} else if (kar == U'<') {
			nieuwkar;
			if (kar == U'=') {
				nieuwtok (LE_)
			} else if (kar == U'>') {
				nieuwtok (NE_)
			} else {
				oudkar;
				nieuwtok (LT_)
			}
		} else if (kar == U'!') {
			nieuwkar;
			if (kar == U'=') {
				nieuwtok (NE_)   /* "!=" = "<>" */
			} else {
				oudkar;
				nieuwtok (NOT_)
			}
		} else if (kar == U',') {
			nieuwtok (KOMMA_)
		} else if (kar == U':') {
			nieuwtok (COLON_)
		} else if (kar == U';') {
			nieuwtok (END_)
		} else if (kar == U'^') {
			nieuwtok (POWER_)
		} else if (kar == U'@') {
			do {
				nieuwkar;
			} while (kar == U' ' || kar == U'\t');
			stokaan;
			do stokkar while ((kar >= U'A' && kar <= U'Z') || (kar >= U'a' && kar <= U'z') || kar >= 192 || (kar >= U'0' && kar <= U'9') || kar == U'_' || kar == U'.');
			stokuit;
			oudkar;
			nieuwtok (CALL_)
			lexan [itok]. content.string = Melder_dup_f (token.string);
			numberOfStringConstants ++;
		} else if (kar == U'\"') {
			/*
			 * String constant.
			 */
			nieuwkar;
			stokaan;
			for (;;) {
				if (kar == U'\0')
					formulefout (U"No closing quote in string constant", ikar);
				if (kar == U'\"') {
					nieuwkar;
					if (kar == U'\"') stokkar
					else break;
				} else {
					stokkar
				}
			}
			stokuit;
			oudkar;
			nieuwtok (STRING_)
			lexan [itok]. content.string = Melder_dup_f (token.string);
			numberOfStringConstants ++;
		} else if (kar == U'|') {
			nieuwtok (OR_)   /* "|" = "or" */
			nieuwkar;
			if (kar != U'|') {
				oudkar;   /* "||" = "or" */
			}
		} else if (kar == U'&') {
			nieuwtok (AND_)   /* "&" = "and" */
			nieuwkar;
			if (kar != U'&') {
				oudkar;   /* "&&" = "and" */
			}
		} else if (kar == U'[') {
			nieuwtok (RECHTEHAAKOPENEN_)
		} else if (kar == U']') {
			nieuwtok (RECHTEHAAKSLUITEN_)
		} else if (kar == U'{') {
			nieuwtok (OPENING_BRACE_)
		} else if (kar == U'}') {
			nieuwtok (CLOSING_BRACE_)
		} else if (kar == U'.') {
			nieuwtok (PERIOD_)
		} else {
			formulefout (U"Unknown symbol", ikar);
		}
	} while (lexan [itok]. symbol != END_);
}

static void pas (int symbol) {
	if (symbol == nieuwlees) {
		return;   // success
	} else {
		const char32 *symbolName1 = Formula_instructionNames [symbol];
		const char32 *symbolName2 = Formula_instructionNames [lexan [ilexan]. symbol];
		bool needQuotes1 = ( str32chr (symbolName1, U' ') == nullptr );
		bool needQuotes2 = ( str32chr (symbolName2, U' ') == nullptr );
		static MelderString melding { };
		MelderString_copy (& melding,
			U"Expected ", needQuotes1 ? U"\"" : nullptr, symbolName1, needQuotes1 ? U"\"" : nullptr,
			U", but found ", needQuotes2 ? U"\"" : nullptr, symbolName2, needQuotes2 ? U"\"" : nullptr);
		formulefout (melding.string, lexan [ilexan]. position);
	}
}

static bool pasArguments () {
    int symbol = nieuwlees;
    if (symbol == HAAKJEOPENEN_) return true;   // success: a function call like: myFunction (...)
    if (symbol == COLON_) return false;   // success: a function call like: myFunction: ...
    const char32 *symbolName2 = Formula_instructionNames [lexan [ilexan]. symbol];
    bool needQuotes2 = ( str32chr (symbolName2, U' ') == nullptr );
    static MelderString melding { };
    MelderString_copy (& melding,
		U"Expected \"(\" or \":\", but found ", needQuotes2 ? U"\"" : nullptr, symbolName2, needQuotes2 ? U"\"" : nullptr);
    formulefout (melding.string, lexan [ilexan]. position);
    return false;   // will never occur
}

#define nieuwontleed(s)  parse [++ iparse]. symbol = (s)
#define parsenumber(g)  parse [iparse]. content.number = (g)
#define ontleedlabel(l)  parse [iparse]. content.label = (l)

static void parseExpression ();

static void parsePowerFactor () {
	int symbol = nieuwlees;

	if (symbol >= LOW_VALUE && symbol <= HIGH_VALUE) {
		nieuwontleed (symbol);
		if (symbol == NUMBER_) parsenumber (lexan [ilexan]. content.number);
		return;
	}

	if (symbol == STRING_) {
		nieuwontleed (symbol);
		parse [iparse]. content.string = lexan [ilexan]. content.string;   // reference copy!
		return;
	}

	if (symbol == NUMERIC_VARIABLE_ || symbol == STRING_VARIABLE_) {
		nieuwontleed (symbol);
		parse [iparse]. content.variable = lexan [ilexan]. content.variable;
		return;
	}

	if (symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_) {
		char32 *var = lexan [ilexan]. content.string;   // Save before incrementing ilexan.
		if (nieuwlees == RECHTEHAAKOPENEN_) {
			int n = 0;
			if (nieuwlees != RECHTEHAAKSLUITEN_) {
				oudlees;
				parseExpression ();
				n ++;
				while (nieuwlees == KOMMA_) {
					parseExpression ();
					n ++;
				}
				oudlees;
				pas (RECHTEHAAKSLUITEN_);
			}
			nieuwontleed (NUMBER_); parsenumber (n);
			nieuwontleed (symbol);
		} else {
			Melder_fatal (U"Formula:parsePowerFactor (indexed variable): No '['; cannot happen.");
		}
		parse [iparse]. content.string = var;
		return;
	}

	if (symbol == NUMERIC_VECTOR_VARIABLE_) {
		InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (nieuwlees == RECHTEHAAKOPENEN_) {
			parseExpression ();
			pas (RECHTEHAAKSLUITEN_);
			nieuwontleed (NUMERIC_VECTOR_ELEMENT_);
		} else {
			oudlees;
			nieuwontleed (NUMERIC_VECTOR_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == NUMERIC_MATRIX_VARIABLE_) {
		InterpreterVariable var = lexan [ilexan]. content.variable;   // save before incrementing ilexan
		if (nieuwlees == RECHTEHAAKOPENEN_) {
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			pas (RECHTEHAAKSLUITEN_);
			nieuwontleed (NUMERIC_MATRIX_ELEMENT_);
		} else {
			oudlees;
			nieuwontleed (NUMERIC_MATRIX_VARIABLE_);
		}
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == VARIABLE_NAME_) {
		InterpreterVariable var = Interpreter_hasVariable (theInterpreter, lexan [ilexan]. content.string);
		if (! var)
			formulefout (U"Unknown variable", lexan [ilexan]. position);
		nieuwontleed (NUMERIC_VARIABLE_);
		parse [iparse]. content.variable = var;
		return;
	}

	if (symbol == SELF_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			parseExpression ();
			if (nieuwlees == KOMMA_) {
				parseExpression ();
				nieuwontleed (SELFMATRIKS2_);
				pas (RECHTEHAAKSLUITEN_);
				return;
			}
			oudlees;
			nieuwontleed (SELFMATRIKS1_);
			pas (RECHTEHAAKSLUITEN_);
			return;
		}
		if (symbol == HAAKJEOPENEN_) {
			parseExpression ();
			if (nieuwlees == KOMMA_) {
				parseExpression ();
				nieuwontleed (SELFFUNKTIE2_);
				pas (HAAKJESLUITEN_);
				return;
			}
			oudlees;
			nieuwontleed (SELFFUNKTIE1_);
			pas (HAAKJESLUITEN_);
			return;
		}
		oudlees;
		nieuwontleed (SELF0_);
		return;
	}

	if (symbol == SELFSTR_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			parseExpression ();
			if (nieuwlees == KOMMA_) {
				parseExpression ();
				nieuwontleed (SELFMATRIKSSTR2_);
				pas (RECHTEHAAKSLUITEN_);
				return;
			}
			oudlees;
			nieuwontleed (SELFMATRIKSSTR1_);
			pas (RECHTEHAAKSLUITEN_);
			return;
		}
		if (symbol == HAAKJEOPENEN_) {
			parseExpression ();
			if (nieuwlees == KOMMA_) {
				parseExpression ();
				nieuwontleed (SELFFUNKTIESTR2_);
				pas (HAAKJESLUITEN_);
				return;
			}
			oudlees;
			nieuwontleed (SELFFUNKTIESTR1_);
			pas (HAAKJESLUITEN_);
			return;
		}
		oudlees;
		nieuwontleed (SELFSTR0_);
		return;
	}

	if (symbol == OBJECT_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			parseExpression ();   // the object's name or ID
			nieuwontleed (TO_OBJECT_);
			if (nieuwlees == RECHTEHAAKSLUITEN_) {
				symbol = nieuwlees;
				if (symbol == PERIOD_) {
					switch (nieuwlees) {
						case XMIN_:
							nieuwontleed (OBJECT_XMIN_);
							return;
						case XMAX_:
							nieuwontleed (OBJECT_XMAX_);
							return;
						case YMIN_:
							nieuwontleed (OBJECT_YMIN_);
							return;
						case YMAX_:
							nieuwontleed (OBJECT_YMAX_);
							return;
						case NX_:
							nieuwontleed (OBJECT_NX_);
							return;
						case NY_:
							nieuwontleed (OBJECT_NY_);
							return;
						case DX_:
							nieuwontleed (OBJECT_DX_);
							return;
						case DY_:
							nieuwontleed (OBJECT_DY_);
							return;
						case NROW_:
							nieuwontleed (OBJECT_NROW_);
							return;
						case NCOL_:
							nieuwontleed (OBJECT_NCOL_);
							return;
						case ROWSTR_:
							pas (RECHTEHAAKOPENEN_);
							parseExpression ();
							nieuwontleed (OBJECT_ROWSTR_);
							pas (RECHTEHAAKSLUITEN_);
							return;
						case COLSTR_:
							pas (RECHTEHAAKOPENEN_);
							parseExpression ();
							nieuwontleed (OBJECT_COLSTR_);
							pas (RECHTEHAAKSLUITEN_);
							return;
						default:
							formulefout (U"After \"object [number].\" there should be \"xmin\", \"xmax\", \"ymin\", "
								"\"ymax\", \"nx\", \"ny\", \"dx\", \"dy\", \"nrow\" or \"ncol\"", lexan [ilexan]. position);
					}
				} else if (symbol == RECHTEHAAKOPENEN_) {
					parseExpression ();
					if (nieuwlees == KOMMA_) {
						parseExpression ();
						nieuwontleed (OBJECTCELL2_);
						pas (RECHTEHAAKSLUITEN_);
					} else {
						oudlees;
						nieuwontleed (OBJECTCELL1_);
						pas (RECHTEHAAKSLUITEN_);
					}
				} else {
					oudlees;
					nieuwontleed (OBJECTCELL0_);
				}
			} else {
				oudlees;
				pas (KOMMA_);
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (OBJECTCELL2_);
					pas (RECHTEHAAKSLUITEN_);
				} else {
					oudlees;
					nieuwontleed (OBJECTCELL1_);
					pas (RECHTEHAAKSLUITEN_);
				}
			}
		} else if (symbol == HAAKJEOPENEN_) {
			parseExpression ();   // the object's name or ID
			nieuwontleed (TO_OBJECT_);
			if (nieuwlees == HAAKJESLUITEN_) {
				nieuwontleed (OBJECTLOCATION0_);
			} else {
				oudlees;
				pas (KOMMA_);
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (OBJECTLOCATION2_);
					pas (HAAKJESLUITEN_);
				} else {
					oudlees;
					nieuwontleed (OBJECTLOCATION1_);
					pas (HAAKJESLUITEN_);
				}
			}
		} else {
			formulefout (U"After \"object\" there should be \"(\" or \"[\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == OBJECTSTR_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			parseExpression ();   // the object's name or ID
			nieuwontleed (TO_OBJECT_);
			if (nieuwlees == RECHTEHAAKSLUITEN_) {
				nieuwontleed (OBJECTCELLSTR0_);
			} else {
				oudlees;
				pas (KOMMA_);
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (OBJECTCELLSTR2_);
					pas (RECHTEHAAKSLUITEN_);
				} else {
					oudlees;
					nieuwontleed (OBJECTCELLSTR1_);
					pas (RECHTEHAAKSLUITEN_);
				}
			}
		} else if (symbol == HAAKJEOPENEN_) {
			parseExpression ();   // the object's name or ID
			nieuwontleed (TO_OBJECT_);
			if (nieuwlees == HAAKJESLUITEN_) {
				nieuwontleed (OBJECTLOCATIONSTR0_);
			} else {
				oudlees;
				pas (KOMMA_);
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (OBJECTLOCATIONSTR2_);
					pas (HAAKJESLUITEN_);
				} else {
					oudlees;
					nieuwontleed (OBJECTLOCATIONSTR1_);
					pas (HAAKJESLUITEN_);
				}
			}
		} else {
			formulefout (U"After \"object$\" there should be \"(\" or \"[\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == HAAKJEOPENEN_) {
		parseExpression ();
		pas (HAAKJESLUITEN_);
		return;
	}

	if (symbol == IF_) {
		int elseLabel = nieuwlabel;   // has to be local,
		int endifLabel = nieuwlabel;   // because of recursion
		parseExpression ();
		nieuwontleed (IFFALSE_);  ontleedlabel (elseLabel);
		pas (THEN_);
		parseExpression ();
		nieuwontleed (GOTO_);     ontleedlabel (endifLabel);
		pas (ELSE_);
		nieuwontleed (LABEL_);    ontleedlabel (elseLabel);
		parseExpression ();
		pas (ENDIF_);
		nieuwontleed (LABEL_);    ontleedlabel (endifLabel);
		return;
	}

	if (symbol == MATRIKS_) {
		Daata thee = lexan [ilexan]. content.object;
		Melder_assert (thee != nullptr);
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (nieuwlees == RECHTEHAAKSLUITEN_) {
				nieuwontleed (MATRIKS0_);
				parse [iparse]. content.object = thee;
			} else {
				oudlees;
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (MATRIKS2_);
					parse [iparse]. content.object = thee;
					pas (RECHTEHAAKSLUITEN_);
				} else {
					oudlees;
					nieuwontleed (MATRIKS1_);
					parse [iparse]. content.object = thee;
					pas (RECHTEHAAKSLUITEN_);
				}
			}
		} else if (symbol == HAAKJEOPENEN_) {
			if (nieuwlees == HAAKJESLUITEN_) {
				nieuwontleed (FUNKTIE0_);
				parse [iparse]. content.object = thee;
			} else {
				oudlees;
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (FUNKTIE2_);
					parse [iparse]. content.object = thee;
					pas (HAAKJESLUITEN_);
				} else {
					oudlees;
					nieuwontleed (FUNKTIE1_);
					parse [iparse]. content.object = thee;
					pas (HAAKJESLUITEN_);
				}
			}
		} else if (symbol == PERIOD_) {
			switch (nieuwlees) {
				case XMIN_:
					if (! thy v_hasGetXmin ()) {
						formulefout (U"Attribute \"xmin\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getXmin ());
						return;
					}
				case XMAX_:
					if (! thy v_hasGetXmax ()) {
						formulefout (U"Attribute \"xmax\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getXmax ());
						return;
					}
				case YMIN_:
					if (! thy v_hasGetYmin ()) {
						formulefout (U"Attribute \"ymin\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getYmin ());
						return;
					}
				case YMAX_:
					if (! thy v_hasGetYmax ()) {
						formulefout (U"Attribute \"ymax\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getYmax ());
						return;
					}
				case NX_:
					if (! thy v_hasGetNx ()) {
						formulefout (U"Attribute \"nx\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getNx ());
						return;
					}
				case NY_:
					if (! thy v_hasGetNy ()) {
						formulefout (U"Attribute \"ny\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getNy ());
						return;
					}
				case DX_:
					if (! thy v_hasGetDx ()) {
						formulefout (U"Attribute \"dx\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getDx ());
						return;
					}
				case DY_:
					if (! thy v_hasGetDy ()) {
						formulefout (U"Attribute \"dy\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getDy ());
						return;
					}
				case NCOL_:
					if (! thy v_hasGetNcol ()) {
						formulefout (U"Attribute \"ncol\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getNcol ());
						return;
					}
				case NROW_:
					if (! thy v_hasGetNrow ()) {
						formulefout (U"Attribute \"nrow\" not defined for this object", lexan [ilexan]. position);
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (thy v_getNrow ());
						return;
					}
				case ROWSTR_:
					if (! thy v_hasGetRowStr ()) {
						formulefout (U"Attribute \"row$\" not defined for this object", lexan [ilexan]. position);
					} else {
						pas (RECHTEHAAKOPENEN_);
						parseExpression ();
						nieuwontleed (ROWSTR_);
						parse [iparse]. content.object = thee;
						pas (RECHTEHAAKSLUITEN_);
						return;
					}
				case COLSTR_:
					if (! thy v_hasGetColStr ()) {
						formulefout (U"Attribute \"col$\" not defined for this object", lexan [ilexan]. position);
					} else {
						pas (RECHTEHAAKOPENEN_);
						parseExpression ();
						nieuwontleed (COLSTR_);
						parse [iparse]. content.object = thee;
						pas (RECHTEHAAKSLUITEN_);
						return;
					}
				default: formulefout (U"Unknown attribute.", lexan [ilexan]. position);
			}
		} else {
			formulefout (U"After a name of a matrix there should be \"(\", \"[\", or \".\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol == MATRIKSSTR_) {
		Daata thee = lexan [ilexan]. content.object;
		Melder_assert (thee != nullptr);
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (nieuwlees == RECHTEHAAKSLUITEN_) {
				nieuwontleed (MATRIKSSTR0_);
				parse [iparse]. content.object = thee;
			} else {
				oudlees;
				parseExpression ();
				if (nieuwlees == KOMMA_) {
					parseExpression ();
					nieuwontleed (MATRIKSSTR2_);
					parse [iparse]. content.object = thee;
					pas (RECHTEHAAKSLUITEN_);
				} else {
					oudlees;
					nieuwontleed (MATRIKSSTR1_);
					parse [iparse]. content.object = thee;
					pas (RECHTEHAAKSLUITEN_);
				}
			}
		} else {
			formulefout (U"After a name of a matrix$ there should be \"[\"", lexan [ilexan]. position);
		}
		return;
	}

	if (symbol >= LOW_FUNCTION_1 && symbol <= HIGH_FUNCTION_1) {
		bool isParenthesis = pasArguments ();
		parseExpression ();
		if (isParenthesis) pas (HAAKJESLUITEN_);
		nieuwontleed (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_2 && symbol <= HIGH_FUNCTION_2) {
		bool isParenthesis = pasArguments ();
		parseExpression ();
		pas (KOMMA_);
		parseExpression ();
		if (isParenthesis) pas (HAAKJESLUITEN_);
		nieuwontleed (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_3 && symbol <= HIGH_FUNCTION_3) {
		bool isParenthesis = pasArguments ();
		parseExpression ();
		pas (KOMMA_);
		parseExpression ();
		pas (KOMMA_);
		parseExpression ();
		if (isParenthesis) pas (HAAKJESLUITEN_);
		nieuwontleed (symbol);
		return;
	}

	if (symbol >= LOW_FUNCTION_N && symbol <= HIGH_FUNCTION_N) {
		int n = 0;
		bool isParenthesis = pasArguments ();
		if (nieuwlees != HAAKJESLUITEN_) {
			oudlees;
			parseExpression ();
			n ++;
			while (nieuwlees == KOMMA_) {
				parseExpression ();
				n ++;
			}
			oudlees;
			if (isParenthesis) pas (HAAKJESLUITEN_);
		}
		nieuwontleed (NUMBER_); parsenumber (n);
		nieuwontleed (symbol);
		return;
	}

	if (symbol == OPENING_BRACE_) {
		parseExpression ();
		int n = 1;
		while (nieuwlees == KOMMA_) {
			parseExpression ();
			n ++;
		}
		oudlees;
		pas (CLOSING_BRACE_);
		nieuwontleed (NUMBER_); parsenumber (n);
		nieuwontleed (NUMERIC_VECTOR_LITERAL_);
		return;
	}

	if (symbol == CALL_) {
		char32 *procedureName = lexan [ilexan]. content.string;   // reference copy!
		int n = 0;
		bool isParenthesis = pasArguments ();
		if (nieuwlees != HAAKJESLUITEN_) {
			oudlees;
			parseExpression ();
			n ++;
			while (nieuwlees == KOMMA_) {
				parseExpression ();
				n ++;
			}
			oudlees;
			if (isParenthesis) pas (HAAKJESLUITEN_);
		}
		nieuwontleed (NUMBER_); parsenumber (n);
		nieuwontleed (CALL_);
		parse [iparse]. content.string = procedureName;
		return;
	}

	if (symbol >= LOW_STRING_FUNCTION && symbol <= HIGH_STRING_FUNCTION) {
		if (symbol >= LOW_FUNCTION_STR1 && symbol <= HIGH_FUNCTION_STR1) {
            bool isParenthesis = pasArguments ();
			parseExpression ();
			if (isParenthesis) pas (HAAKJESLUITEN_);
		} else if (symbol == INDEX_ || symbol == RINDEX_ ||
			symbol == STARTS_WITH_ || symbol == ENDS_WITH_ ||
			symbol == INDEX_REGEX_ || symbol == RINDEX_REGEX_ || symbol == EXTRACT_NUMBER_)
		{
            bool isParenthesis = pasArguments ();
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			if (isParenthesis) pas (HAAKJESLUITEN_);
		} else if (symbol == DATESTR_ || symbol == INFOSTR_) {
			pas (HAAKJEOPENEN_);
			pas (HAAKJESLUITEN_);
		} else if (symbol == EXTRACT_WORDSTR_ || symbol == EXTRACT_LINESTR_) {
            bool isParenthesis = pasArguments ();
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			if (isParenthesis) pas (HAAKJESLUITEN_);
		} else if (symbol == FIXEDSTR_ || symbol == PERCENTSTR_) {
            bool isParenthesis = pasArguments ();
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			if (isParenthesis) pas (HAAKJESLUITEN_);
		} else if (symbol == REPLACESTR_ || symbol == REPLACE_REGEXSTR_) {
            bool isParenthesis = pasArguments ();
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			pas (KOMMA_);
			parseExpression ();
			if (isParenthesis) pas (HAAKJESLUITEN_);
		} else {
			oudlees;   // needed for retry if we are going to be in a string comparison?
			formulefout (U"Function expected", lexan [ilexan + 1]. position);
		}
		nieuwontleed (symbol);
		return;
	}

	if (symbol >= LOW_RANGE_FUNCTION && symbol <= HIGH_RANGE_FUNCTION) {
		if (symbol == SUM_OVER_) {
			//theOptimize = 1;
			nieuwontleed (NUMBER_); parsenumber (0.0);   // initialize the sum
            bool isParenthesis = pasArguments ();
			int symbol2 = nieuwlees;
			if (symbol2 == NUMERIC_VARIABLE_) {   // an existing variable
				nieuwontleed (VARIABLE_REFERENCE_);
				InterpreterVariable loopVariable = lexan [ilexan]. content.variable;
				parse [iparse]. content.variable = loopVariable;
			} else if (symbol2 == VARIABLE_NAME_) {   // a new variable
				InterpreterVariable loopVariable = Interpreter_lookUpVariable (theInterpreter, lexan [ilexan]. content.string);
				nieuwontleed (VARIABLE_REFERENCE_);
				parse [iparse]. content.variable = loopVariable;
			} else {
				formulefout (U"Numeric variable expected", lexan [ilexan]. position);
			}
			// now on stack: sum, loop variable
			if (nieuwlees == FROM_) {
				parseExpression ();
			} else {
				oudlees;
				nieuwontleed (NUMBER_); parsenumber (1.0);
			}
			nieuwontleed (DECREMENT_AND_ASSIGN_);   // this pushes the variable back on the stack
			// now on stack: sum, loop variable
			pas (TO_);
			parseExpression ();
			// now on stack: sum, loop variable, end value
			int startLabel = nieuwlabel;
			int endLabel = nieuwlabel;
			nieuwontleed (LABEL_); ontleedlabel (startLabel);
			nieuwontleed (INCREMENT_GREATER_GOTO_); ontleedlabel (endLabel);
			pas (KOMMA_);
			parseExpression ();
			if (isParenthesis) pas (HAAKJESLUITEN_);
			// now on stack: sum, loop variable, end value, value to add
			nieuwontleed (ADD_3DOWN_);
			// now on stack: sum, loop variable, end value
			nieuwontleed (GOTO_); ontleedlabel (startLabel);
			nieuwontleed (LABEL_); ontleedlabel (endLabel);
			nieuwontleed (POP_2_);
			// now on stack: sum
			return;
		}
	}

	if (symbol == STOPWATCH_) {
		nieuwontleed (symbol);
		return;
	}

	oudlees;   // needed for retry if we are going to be in a string comparison
	formulefout (U"Symbol misplaced", lexan [ilexan + 1]. position);
}

static void parseFactor ();

static void parsePowerFactors () {
	if (nieuwlees == POWER_) {
		if (ilexan > 2 && lexan [ilexan - 2]. symbol == MINUS_ && lexan [ilexan - 1]. symbol == NUMBER_) {
			oudlees;
			formulefout (U"Expressions like -3^4 are ambiguous; use (-3)^4 or -(3^4) or -(3)^4", lexan [ilexan + 1]. position);
		}
		parseFactor ();   // like a^-b
		nieuwontleed (POWER_);
	}
	else
		oudlees;
}

static void parseMinus () {
	parsePowerFactor ();
	parsePowerFactors ();
}

static void parseFactor () {
	if (nieuwlees == MINUS_) {
		parseFactor ();   // there can be multiple consecutive minuses
		nieuwontleed (MINUS_);
		return;
	}
	oudlees;
	parseMinus ();   // like -a^b
}

static void parseFactors () {
	int sym = nieuwlees;   // has to be local, because of recursion
	if (sym == MUL_ || sym == RDIV_ || sym == IDIV_ || sym == MOD_) {
		parseFactor ();
		nieuwontleed (sym);
		parseFactors ();
	}
	else oudlees;
}

static void parseTerm () {
	parseFactor ();
	parseFactors ();
}

static void parseTerms () {
	int symbol = nieuwlees;
	if (symbol == ADD_ || symbol == SUB_) {
		parseTerm ();
		nieuwontleed (symbol);
		parseTerms ();
	}
	else oudlees;
}

static void parseNot () {
	int symbol;
	parseTerm ();
	parseTerms ();
	symbol = nieuwlees;
	if (symbol >= EQ_ && symbol <= GT_) {
		parseTerm ();
		parseTerms ();
		nieuwontleed (symbol);
	}
	else oudlees;
}

static void parseAnd () {
	if (nieuwlees == NOT_) {
		parseAnd ();   // like not not not
		nieuwontleed (NOT_);
		return;
	}
	oudlees;
	parseNot ();
}

static void parseOr () {
	parseAnd ();
	if (nieuwlees == AND_) {
		int falseLabel = nieuwlabel;
		int andLabel = nieuwlabel;
		do {
			nieuwontleed (IFFALSE_); ontleedlabel (falseLabel);
			parseAnd ();
		} while (nieuwlees == AND_);
		nieuwontleed (IFFALSE_); ontleedlabel (falseLabel);
		nieuwontleed (TRUE_);
		nieuwontleed (GOTO_); ontleedlabel (andLabel);
		nieuwontleed (LABEL_); ontleedlabel (falseLabel);
		nieuwontleed (FALSE_);
		nieuwontleed (LABEL_); ontleedlabel (andLabel);
	}
	oudlees;
}

static void parseExpression () {
	parseOr ();
	if (nieuwlees == OR_) {
		int trueLabel = nieuwlabel;
		int orLabel = nieuwlabel;
		do {
			nieuwontleed (IFTRUE_); ontleedlabel (trueLabel);
			parseOr ();
		} while (nieuwlees == OR_);
		nieuwontleed (IFTRUE_); ontleedlabel (trueLabel);
		nieuwontleed (FALSE_);
		nieuwontleed (GOTO_); ontleedlabel (orLabel);
		nieuwontleed (LABEL_); ontleedlabel (trueLabel);
		nieuwontleed (TRUE_);
		nieuwontleed (LABEL_); ontleedlabel (orLabel);
	}
	oudlees;
}

/*
	Translate the infix expression "my lexan" into the postfix expression "my parse":
	remove parentheses and brackets, commas, colons, FROM_, TO_,
	IF_ THEN_ ELSE_ ENDIF_ OR_ AND_;
	introduce LABEL_ GOTO_ IFTRUE_ IFFALSE_ TRUE_ FALSE_
	SELF0_ SELF1_ SELF2_ MATRIKS0_ MATRIKS1_ MATRIKS2_
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
	pas (END_);
	nieuwontleed (END_);
	numberOfInstructions = iparse;
}

static void schuif (int begin, int afstand) {
	numberOfInstructions -= afstand;
	for (int j = begin; j <= numberOfInstructions; j ++)
		parse [j] = parse [j + afstand];
}

static int vindLabel (int label) {
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
		int verbeterd = 0;
		for (i = 1; i <= numberOfInstructions; i ++)
		{

/* Optimalisatie 1: */
/*    true   goto x  ->  goto y  /  __  ...  label x  iftrue y    */
/*    false  goto x  ->  goto y  /  __  ...  label x  iffalse y   */

			if ((parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_)
				 ||
				 (parse [i]. symbol == FALSE_ &&
				  parse [i + 1]. symbol == GOTO_ &&
				  parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_))
			{
				 verbeterd = 1;
				 parse [i]. symbol = GOTO_;
				 parse [i]. content.label = parse [volg]. content.label;
				 schuif (i + 1, 1);
			}

/* Optimalisatie 2: */
/*    true  goto x  ...  label x  iffalse y  ->        */
/*          goto z  ...  label x  iffalse y  label z   */
/*    en analoog met false en iftrue. */

			if ((parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_)
				 ||
				 (parse [i]. symbol == FALSE_ &&
				  parse [i + 1]. symbol == GOTO_ &&
				  parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_))
			{
				verbeterd = 1;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = nieuwlabel;
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
				verbeterd = 1;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
			}

/* Optimalisatie 3b: */
/*    iffalse x  goto y  label x  ->  iftrue y  label x   */

			if (parse [i]. symbol == IFFALSE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [i + 2]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 2]. content.label)
			{
				verbeterd = 1;
				parse [i]. symbol = IFTRUE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
			}

/* Optimalisatie 4: */
/*    verwijder onbereikbare kode: na een GOTO_ hoort een LABEL_. */

			if (parse [i]. symbol == GOTO_ &&
				 parse [i + 1]. symbol != LABEL_)
			{
				verbeterd = 1;
				j = i + 2;
				while (parse [j]. symbol != LABEL_) j ++;
				schuif (i + 1, j - i - 1);
			}

/* Optimalisatie 5: */
/*    goto x  ->  0  /  __  label x   */

			if (parse [i]. symbol == GOTO_ &&
				 parse [i]. symbol == LABEL_ &&
				 parse [i]. content.label == parse [i + 1]. content.label)
			{
				verbeterd = 1;
				schuif (i, 1);
			}

/* Optimalisatie 6: */
/*    true   iffalse x  ->  0  */
/*    false  iftrue x   ->  0  */

			if ((parse [i]. symbol == TRUE_ && parse [i + 1]. symbol == IFFALSE_)
				|| (parse [i]. symbol == FALSE_ && parse [i + 1]. symbol == IFTRUE_))
			{
				verbeterd = 1;
				schuif (i, 2);
			}

/* Optimalisatie 7: */
/*    true   iftrue x   ->  goto x    */
/*    false  iffalse x  ->  goto x    */
			
			if ((parse [i]. symbol == TRUE_ && parse [i + 1]. symbol == IFTRUE_)
				|| (parse [i]. symbol == FALSE_ && parse [i + 1]. symbol == IFFALSE_))
			{
				verbeterd = 1;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
			}

/* Optimalisatie 8: */
/*    iftrue x   ->  iftrue y   /  __  ...  label x  goto y   */
/*    iffalse x  ->  iffalse y  /  __  ...  label x  goto y   */

			if ((parse [i]. symbol == IFTRUE_ || parse [i]. symbol == IFFALSE_)
				&& parse [volg = vindLabel (parse [i]. content.label) + 1]. symbol == GOTO_)
			{
				verbeterd = 1;
				parse [i]. content.label = parse [volg]. content.label;
			}

/* Optimalisatie 9a: */
/*    not  iftrue x  ->  iffalse x   */

			if (parse [i]. symbol == NOT_ && parse [i + 1]. symbol == IFTRUE_)
			{
				verbeterd = 1;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
			}

/* Optimalisatie 9b: */
/*    not  iffalse x  ->  iftrue x   */

			if (parse [i]. symbol == NOT_ && parse [i + 1]. symbol == IFFALSE_)
			{
				verbeterd = 1;
				parse [i]. symbol = IFTRUE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
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
					verbeterd = 1;
					schuif (i, 1);
				}
			}
		if (! verbeterd) break;
	}
}

static int praat_findObjectById (int id) {
	int IOBJECT;
	WHERE_DOWN (ID == id)
		return IOBJECT;
	Melder_throw (U"No object with number ", id, U".");
}

static int praat_findObjectFromString (const char32 *name) {
	int IOBJECT;
	if (*name >= U'A' && *name <= U'Z') {
		/*
		 * Find the object by its name.
		 */
		static MelderString buffer { };
		MelderString_copy (& buffer, name);
		char32 *space = str32chr (buffer.string, U' ');
		if (space == nullptr)
			Melder_throw (U"Missing space in object name \"", name, U"\".");
		*space = U'\0';
		char32 *className = & buffer.string [0], *givenName = space + 1;
		WHERE_DOWN (1) {
			Daata object = OBJECT;
			if (str32equ (className, Thing_className (OBJECT)) && str32equ (givenName, object -> name))
				return IOBJECT;
		}
		ClassInfo klas = Thing_classFromClassName (className, nullptr);
		WHERE_DOWN (1) {
			Daata object = OBJECT;
			if (str32equ (klas -> className, Thing_className (OBJECT)) && str32equ (givenName, object -> name))
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
					int IOBJECT = praat_findObjectById (lround (parse [i]. content.number));
					parse [i]. content.object = OBJECT;
					gain = 1;
				}
			} else if (parse [i]. symbol == STRING_) {
				if (parse [i + 1]. symbol == TO_OBJECT_) {
					parse [i]. symbol = OBJECT_;
					int IOBJECT = praat_findObjectFromString (parse [i]. content.string);
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
				parse [i]. content.string = parse [i]. content.variable -> stringValue;   // again a reference copy (lexan is still the owner)
				gain = 0;
				improved = true;
			#if 0
			} else if (parse [i]. symbol == ROW_) {
				if (parse [i + 1]. symbol == COL_ && parse [i + 2]. symbol == SELFMATRIKS2_)
					{ gain = 2; parse [i]. symbol = SELF0_; }   // TODO: SELF0_ may not have the same restrictions as SELFMATRIKS2_
			} else if (parse [i]. symbol == COL_) {
				if (parse [i + 1]. symbol == SELFMATRIKS1_)
					{ gain = 1; parse [i]. symbol = SELF0_; }
			#endif
			}
			if (gain) { improved = true; schuif (i + 1, gain); }
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
	 * Then remove the labels, which have become superfluous.
	 */
	if (theOptimize) {
		int i = 1;
		while (i <= numberOfInstructions) {
			int symboli = parse [i]. symbol;
			if (symboli == LABEL_) {
				schuif (i, 1);   // remove one label
				for (int j = 1; j <= numberOfInstructions; j ++) {
					int symbolj = parse [j]. symbol;
					if ((symbolj == GOTO_ || symbolj == IFTRUE_ || symbolj == IFFALSE_ || symbolj == INCREMENT_GREATER_GOTO_) && parse [j]. content.label > i)
						parse [j]. content.label --;  /* Pas een label aan. */
				}
				i --;   /* Voorkom ophogen i (overbodig?). */
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
		const char32 *instructionName;
		symbol = f [++ i]. symbol;
		instructionName = Formula_instructionNames [symbol];
		if (symbol == NUMBER_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.number);
		else if (symbol == GOTO_ || symbol == IFFALSE_ || symbol == IFTRUE_ || symbol == LABEL_ || symbol == INCREMENT_GREATER_GOTO_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.label);
		else if (symbol == NUMERIC_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.variable -> string, U" ", f [i]. content.variable -> numericValue);
		else if (symbol == STRING_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.variable -> string, U" ", f [i]. content.variable -> stringValue);
		else if (symbol == STRING_ || symbol == VARIABLE_NAME_ || symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_)
			Melder_casual (i, U" ", instructionName, U" ", f [i]. content.string);
		else if (symbol == MATRIKS_ || symbol == MATRIKSSTR_ || symbol == MATRIKS1_ || symbol == MATRIKSSTR1_ ||
		         symbol == MATRIKS2_ || symbol == MATRIKSSTR2_ || symbol == ROWSTR_ || symbol == COLSTR_)
		{
			Thing object = f [i]. content.object;
			if (object) {
				Melder_casual (i, U" ", instructionName, U" ", Thing_className (object), U" ", object -> name);
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

void Formula_compile (Interpreter interpreter, Daata data, const char32 *expression, int expressionType, bool optimize) {
	theInterpreter = interpreter;
	if (! theInterpreter) {
		if (! theLocalInterpreter) {
			theLocalInterpreter = Interpreter_create (nullptr, nullptr);
		}
		theInterpreter = theLocalInterpreter.get();
		for (std::unordered_map<std::u32string, InterpreterVariable>::iterator it = theInterpreter -> variablesMap. begin(); it != theInterpreter -> variablesMap. end(); it ++) {
			InterpreterVariable var = it -> second;
			forget (var);
		}
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
	if (! parse) parse = Melder_calloc_f (struct structFormulaInstruction, 3000);

	/*
		Clean up strings from the previous call.
		These strings are in a union, that's why this cannot be done later, when a new string is created.
	*/
	if (numberOfStringConstants) {
		ilexan = 1;
		for (;;) {
			int symbol = lexan [ilexan]. symbol;
			if (symbol == STRING_ || symbol == VARIABLE_NAME_ || symbol == INDEXED_NUMERIC_VARIABLE_ || symbol == INDEXED_STRING_VARIABLE_ || symbol == CALL_) Melder_free (lexan [ilexan]. content.string);
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
 * Running.
 */

static int programPointer;

static void Stackel_cleanUp (Stackel me) {
	if (my which == Stackel_STRING) {
		Melder_free (my string);
	} else if (my which == Stackel_NUMERIC_VECTOR) {
		NUMvector_free (my numericVector.at, 1);
		my numericVector = empty_numvec;
	} else if (my which == Stackel_NUMERIC_MATRIX) {
		NUMmatrix_free (my numericMatrix.at, 1, 1);
		my numericMatrix = empty_nummat;
	}
}
static Stackel theStack;
static int w, wmax;   /* w = stack pointer; */
#define pop  & theStack [w --]
inline static void pushNumber (double x) {
	/* inline runs 10 to 20 percent faster; here's the test script:
		stopwatch
		Create Sound from formula: "test", 1, 0.0, 1000.0, 44100, "x + 1 + 2 + 3 + 4 + 5 + 6"
		writeInfoLine: stopwatch
		Remove
	 * Mac: 3.76 -> 3.20 seconds
	 */
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
	if (w > wmax) wmax ++;
	stackel -> which = Stackel_NUMBER;
	stackel -> number = isdefined (x) ? x : undefined;
	//stackel -> number = x;   // this one would be 2 percent faster
}
static void pushNumericVector (autonumvec x) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
	if (w > wmax) wmax ++;
	stackel -> which = Stackel_NUMERIC_VECTOR;
	stackel -> numericVector = x.releaseToAmbiguousOwner();
}
static void pushNumericMatrix (autonummat x) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
	if (w > wmax) wmax ++;
	stackel -> which = Stackel_NUMERIC_MATRIX;
	stackel -> numericMatrix = x.releaseToAmbiguousOwner();
}
static void pushString (char32 *x) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
	if (w > wmax) wmax ++;
	stackel -> which = Stackel_STRING;
	stackel -> string = x;
}
static void pushObject (Daata object) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
	if (w > wmax) wmax ++;
	stackel -> which = Stackel_OBJECT;
	stackel -> object = object;
}
static void pushVariable (InterpreterVariable var) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
	if (w > wmax) wmax ++;
	stackel -> which = Stackel_VARIABLE;
	stackel -> variable = var;
}
const char32 *Stackel_whichText (Stackel me) {
	return
		my which == Stackel_NUMBER ? U"a number" :
		my which == Stackel_NUMERIC_VECTOR ? U"a numeric vector" :
		my which == Stackel_NUMERIC_MATRIX ? U"a numeric matrix" :
		my which == Stackel_STRING ? U"a string" :
		my which == Stackel_STRING_ARRAY ? U"a string array" :
		my which == Stackel_OBJECT ? U"an object" :
		U"???";
}

static void do_not () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number == 0.0 ? 1.0 : 0.0);
	} else {
		Melder_throw (U"Cannot negate (\"not\") ", Stackel_whichText (x), U".");
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
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number == y->number ? 1.0 : 0.0);
			} else {
				pushNumber (0.0);   // defined is not equal to undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (0.0);   // undefined is not equal to defined
			} else {
				pushNumber (1.0);   // undefined is equal to undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = str32equ (x->string, y->string) ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (=) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
	}
}
static void do_ne () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		/*
			Unequal is defined as the opposite of equal.
		*/
		double xvalue = x->number, yvalue = y->number;
		if (isdefined (xvalue)) {
			if (isdefined (yvalue)) {
				pushNumber (x->number != y->number ? 1.0 : 0.0);
			} else {
				pushNumber (1.0);   // defined is unequal to undefined
			}
		} else {
			if (isdefined (yvalue)) {
				pushNumber (1.0);   // undefined is unequal to defined
			} else {
				pushNumber (0.0);   // undefined is not unequal to undefined
			}
		}
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = str32equ (x->string, y->string) ? 0.0 : 1.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<>) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
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
		double result = str32cmp (x->string, y->string) <= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<=) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
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
		double result = str32cmp (x->string, y->string) < 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (<) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
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
		double result = str32cmp (x->string, y->string) >= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (>=) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
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
		double result = str32cmp (x->string, y->string) > 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_throw (U"Cannot compare (>) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
	}
}
static void do_add () {
	/*
		result.. = x.. + y..
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER) {
		double xvalue = x->number;
		if (y->which == Stackel_NUMBER) {
			/*
				result = x + y
			*/
			double yvalue = y->number;
			pushNumber (xvalue + yvalue);
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x + y#
			*/
			long ny = y->numericVector.size;
			autonumvec result (ny, false);
			for (long i = 1; i <= ny; i ++) {
				double yvalue = y->numericVector [i];
				result [i] = xvalue + yvalue;
			}
			pushNumericVector (result.move());
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x + y##
			*/
			long nrow = y->numericMatrix.nrow, ncol = y->numericMatrix.ncol;
			autonummat result (nrow, ncol, false);
			for (long irow = 1; irow <= nrow; irow ++) {
				for (long icol = 1; icol <= ncol; icol ++) {
					double yvalue = y->numericMatrix [irow] [icol];
					result [irow] [icol] = xvalue + yvalue;
				}
			}
			pushNumericMatrix (result.move());
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		long nx = x->numericVector.size, ny = y->numericVector.size;
		if (nx != ny)
			Melder_throw (U"When adding vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
		pushNumericVector (add_numvec (x->numericVector, y->numericVector));
		return;
	}
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		long xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
		long ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
		if (xnrow != ynrow)
			Melder_throw (U"When adding matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
		if (xncol != yncol)
			Melder_throw (U"When adding matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
		autonummat result (xnrow, xncol, false);
		for (long irow = 1; irow <= xnrow; irow ++) {
			for (long icol = 1; icol <= xncol; icol ++) {
				double xvalue = x->numericMatrix [irow] [icol];
				double yvalue = y->numericMatrix [irow] [icol];
				result [irow] [icol] = xvalue + yvalue;
			}
		}
		pushNumericMatrix (result.move());
		return;
	}
	if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		long length1 = str32len (x->string), length2 = str32len (y->string);
		char32 *result = Melder_malloc (char32, length1 + length2 + 1);
		str32cpy (result, x->string);
		str32cpy (result + length1, y->string);
		pushString (result);
		return;
	}
	Melder_throw (U"Cannot add ", Stackel_whichText (y), U" to ", Stackel_whichText (x), U".");
}
static void do_sub () {
	/*
		result.. = x.. - y..
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER) {
		double xvalue = x->number;
		if (y->which == Stackel_NUMBER) {
			/*
				result = x - y
			*/
			double yvalue = y->number;
			pushNumber (xvalue - yvalue);
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x - y#
			*/
			long ny = y->numericVector.size;
			autonumvec result (ny, false);
			for (long i = 1; i <= ny; i ++) {
				double yvalue = y->numericVector [i];
				result [i] = xvalue - yvalue;
			}
			pushNumericVector (result.move());
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x - y##
			*/
			long nrow = y->numericMatrix.nrow, ncol = y->numericMatrix.ncol;
			autonummat result (nrow, ncol, false);
			for (long irow = 1; irow <= nrow; irow ++) {
				for (long icol = 1; icol <= ncol; icol ++) {
					double yvalue = y->numericMatrix [irow] [icol];
					result [irow] [icol] = xvalue - yvalue;
				}
			}
			pushNumericMatrix (result.move());
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		long nx = x->numericVector.size, ny = y->numericVector.size;
		if (nx != ny)
			Melder_throw (U"When subtracting vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
		autonumvec result { nx, false };
		for (long i = 1; i <= nx; i ++) {
			double xvalue = x->numericVector [i];
			double yvalue = y->numericVector [i];
			result [i] = xvalue - yvalue;
		}
		pushNumericVector (result.move());
		return;
	}
	if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_MATRIX) {
		long xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol;
		long ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
		if (xnrow != ynrow)
			Melder_throw (U"When subtracting matrices, their numbers of rows should be equal, instead of ", xnrow, U" and ", ynrow, U".");
		if (xncol != yncol)
			Melder_throw (U"When subtracting matrices, their numbers of columns should be equal, instead of ", xncol, U" and ", yncol, U".");
		autonummat result (xnrow, xncol, false);
		for (long irow = 1; irow <= xnrow; irow ++) {
			for (long icol = 1; icol <= xncol; icol ++) {
				double xvalue = x->numericMatrix [irow] [icol];
				double yvalue = y->numericMatrix [irow] [icol];
				result [irow] [icol] = xvalue - yvalue;
			}
		}
		pushNumericMatrix (result.move());
		return;
	}
	if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		int64 length1 = str32len (x->string), length2 = str32len (y->string), newlength = length1 - length2;
		char32 *result;
		if (newlength >= 0 && str32nequ (x->string + newlength, y->string, length2)) {
			result = Melder_malloc (char32, newlength + 1);
			str32ncpy (result, x->string, newlength);
			result [newlength] = '\0';
		} else {
			result = Melder_dup (x->string);
		}
		pushString (result);
		return;
	}
	Melder_throw (U"Cannot subtract (-) ", Stackel_whichText (y), U" from ", Stackel_whichText (x), U".");
}
static void do_mul () {
	/*
		result.. = x.. * y..
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER) {
		double xvalue = x->number;
		if (y->which == Stackel_NUMBER) {
			/*
				result = x * y
			*/
			double yvalue = y->number;
			pushNumber (xvalue * yvalue);
			return;
		}
		if (y->which == Stackel_NUMERIC_VECTOR) {
			/*
				result# = x * y#
			*/
			long ny = y->numericVector.size;
			autonumvec result { ny, false };
			for (long i = 1; i <= ny; i ++) {
				double yvalue = y->numericVector [i];
				result [i] = xvalue * yvalue;
			}
			pushNumericVector (result.move());
			return;
		}
		if (y->which == Stackel_NUMERIC_MATRIX) {
			/*
				result## = x * y##
			*/
			long nrow = y->numericMatrix.nrow, ncol = y->numericMatrix.ncol;
			autonummat result (nrow, ncol, false);
			for (long irow = 1; irow <= nrow; irow ++) {
				for (long icol = 1; icol <= ncol; icol ++) {
					double yvalue = y->numericMatrix [irow] [icol];
					result [irow] [icol] = xvalue * yvalue;
				}
			}
			pushNumericMatrix (result.move());
			return;
		}
	}
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		/*
			result# = x# * y#
		*/
		long nx = x->numericVector.size, ny = y->numericVector.size;
		if (nx != ny)
			Melder_throw (U"When multiplying vectors, their numbers of elements should be equal, instead of ", nx, U" and ", ny, U".");
		autonumvec result { nx, false };
		for (long i = 1; i <= nx; i ++) {
			double xvalue = x->numericVector [i];
			double yvalue = y->numericVector [i];
			result [i] = xvalue * yvalue;
		}
		pushNumericVector (result.move());
		return;
	}
	Melder_throw (U"Cannot multiply (*) ", Stackel_whichText (x), U" by ", Stackel_whichText (y), U".");
}
static void do_rdiv () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->number / y->number);   // result could be inf (1/0) or NaN (0/0), which is OK
		return;
	}
	if (x->which == Stackel_NUMERIC_VECTOR) {
		if (y->which == Stackel_NUMERIC_VECTOR) {
			long nelem1 = x->numericVector.size, nelem2 = y->numericVector.size;
			if (nelem1 != nelem2)
				Melder_throw (U"When dividing vectors, their numbers of elements should be equal, instead of ", nelem1, U" and ", nelem2, U".");
			autonumvec result { nelem1, false };
			for (long ielem = 1; ielem <= nelem1; ielem ++)
				result [ielem] = x->numericVector [ielem] / y->numericVector [ielem];
			pushNumericVector (result.move());
			return;
		}
		if (y->which == Stackel_NUMBER) {
			/*
				result# = x# / y
			*/
			long xn = x->numericVector.size;
			autonumvec result { xn, false };
			double yvalue = y->number;
			if (yvalue == 0.0) {
				Melder_throw (U"Cannot divide (/) ", Stackel_whichText (x), U" by zero.");
			} else {
				for (long i = 1; i <= xn; i ++) {
					double xvalue = x->numericVector [i];
					result [i] = xvalue / yvalue;
				}
			}
			pushNumericVector (result.move());
			return;
		}
	}
	Melder_throw (U"Cannot divide (/) ", Stackel_whichText (x), U" by ", Stackel_whichText (y), U".");
}
static void do_idiv () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (floor (x->number / y->number));
		return;
	}
	Melder_throw (U"Cannot divide (\"div\") ", Stackel_whichText (x), U" by ", Stackel_whichText (y), U".");
}
static void do_mod () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->number - floor (x->number / y->number) * y->number);
		return;
	}
	Melder_throw (U"Cannot divide (\"mod\") ", Stackel_whichText (x), U" by ", Stackel_whichText (y), U".");
}
static void do_minus () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (- x->number);
	} else {
		Melder_throw (U"Cannot take the opposite (-) of ", Stackel_whichText (x), U".");
	}
}
static void do_power () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : pow (x->number, y->number));
	} else {
		Melder_throw (U"Cannot exponentiate (^) ", Stackel_whichText (x), U" to ", Stackel_whichText (y), U".");
	}
}
static void do_sqr () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number * x->number);
	} else {
		Melder_throw (U"Cannot take the square (^ 2) of ", Stackel_whichText (x), U".");
	}
}
static void do_function_n_n (double (*f) (double)) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : f (x->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric argument, not ", Stackel_whichText (x), U".");
	}
}
static void do_functionvec_n_n (double (*f) (double)) {
	#if 0
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		long nelm = x->numericVector.numberOfElements;
		double *result = NUMvector<double> (1, nelm);
		for (long i = 1; i <= nelm; i ++) {
			result [i] = f (x->numericVector.data [i]);
		}
		pushNumericVector (nelm, result);
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", Stackel_whichText (x), U".");
	}
	#else
	Stackel x = & theStack [w];
	if (x->which == Stackel_NUMERIC_VECTOR) {
		long nelm = x->numericVector.size;
		for (long i = 1; i <= nelm; i ++) {
			x->numericVector [i] = f (x->numericVector [i]);
		}
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", Stackel_whichText (x), U".");
	}
	#endif
}
static void do_softmax () {
	Stackel x = & theStack [w];
	if (x->which == Stackel_NUMERIC_VECTOR) {
		long nelm = x->numericVector.size;
		double maximum = -1e308;
		for (long i = 1; i <= nelm; i ++) {
			if (x->numericVector [i] > maximum) {
				maximum = x->numericVector [i];
			}
		}
		for (long i = 1; i <= nelm; i ++) {
			x->numericVector [i] -= maximum;
		}
		double sum = 0.0;
		for (long i = 1; i <= nelm; i ++) {
			x->numericVector [i] = exp (x->numericVector [i]);
			sum += x->numericVector [i];
		}
		for (long i = 1; i <= nelm; i ++) {
			x->numericVector [i] /= sum;
		}
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires a numeric vector argument, not ", Stackel_whichText (x), U".");
	}
}
static void do_abs () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : fabs (x->number));
	} else {
		Melder_throw (U"Cannot take the absolute value (abs) of ", Stackel_whichText (x), U".");
	}
}
static void do_round () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : floor (x->number + 0.5));
	} else {
		Melder_throw (U"Cannot round ", Stackel_whichText (x), U".");
	}
}
static void do_floor () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : floor (x->number));
	} else {
		Melder_throw (U"Cannot round down (floor) ", Stackel_whichText (x), U".");
	}
}
static void do_ceiling () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : ceil (x->number));
	} else {
		Melder_throw (U"Cannot round up (ceiling) ", Stackel_whichText (x), U".");
	}
}
static void do_rectify () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : x->number > 0.0 ? x->number : 0.0);
	} else {
		Melder_throw (U"Cannot rectify ", Stackel_whichText (x), U".");
	}
}
static void do_rectify_numvec () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		long nelm = x->numericVector.size;
		autonumvec result { nelm, false };
		for (long i = 1; i <= nelm; i ++) {
			double xvalue = x->numericVector [i];
			result [i] = isundef (xvalue) ? undefined : xvalue > 0.0 ? xvalue : 0.0;
		}
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"Cannot rectify ", Stackel_whichText (x), U".");
	}
}
static void do_sqrt () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number < 0.0 ? undefined : sqrt (x->number));
	} else {
		Melder_throw (U"Cannot take the square root (sqrt) of ", Stackel_whichText (x), U".");
	}
}
static void do_sin () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : sin (x->number));
	} else {
		Melder_throw (U"Cannot take the sine (sin) of ", Stackel_whichText (x), U".");
	}
}
static void do_cos () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : cos (x->number));
	} else {
		Melder_throw (U"Cannot take the cosine (cos) of ", Stackel_whichText (x), U".");
	}
}
static void do_tan () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : tan (x->number));
	} else {
		Melder_throw (U"Cannot take the tangent (tan) of ", Stackel_whichText (x), U".");
	}
}
static void do_arcsin () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			fabs (x->number) > 1.0 ? undefined : asin (x->number));
	} else {
		Melder_throw (U"Cannot take the arcsine (arcsin) of ", Stackel_whichText (x), U".");
	}
}
static void do_arccos () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			fabs (x->number) > 1.0 ? undefined : acos (x->number));
	} else {
		Melder_throw (U"Cannot take the arccosine (arccos) of ", Stackel_whichText (x), U".");
	}
}
static void do_arctan () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : atan (x->number));
	} else {
		Melder_throw (U"Cannot take the arctangent (arctan) of ", Stackel_whichText (x), U".");
	}
}
static void do_exp () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : exp (x->number));
	} else {
		Melder_throw (U"Cannot exponentiate (exp) ", Stackel_whichText (x), U".");
	}
}
static void do_exp_numvec () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		long nelm = x->numericVector.size;
		autonumvec result (nelm, false);
		for (long i = 1; i <= nelm; i ++) {
			result [i] = exp (x->numericVector [i]);
		}
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"Cannot exponentiate (exp) ", Stackel_whichText (x), U".");
	}
}
static void do_exp_nummat () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_MATRIX) {
		long nrow = x->numericMatrix.nrow, ncol = x->numericMatrix.ncol;
		autonummat result (nrow, ncol, false);
		for (long irow = 1; irow <= nrow; irow ++) {
			for (long icol = 1; icol <= ncol; icol ++) {
				result [irow] [icol] = exp (x->numericMatrix [irow] [icol]);
			}
		}
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"Cannot exponentiate (exp) ", Stackel_whichText (x), U".");
	}
}
static void do_sinh () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : sinh (x->number));
	} else {
		Melder_throw (U"Cannot take the hyperbolic sine (sinh) of ", Stackel_whichText (x), U".");
	}
}
static void do_cosh () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : cosh (x->number));
	} else {
		Melder_throw (U"Cannot take the hyperbolic cosine (cosh) of ", Stackel_whichText (x), U".");
	}
}
static void do_tanh () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined : tanh (x->number));
	} else {
		Melder_throw (U"Cannot take the hyperbolic tangent (tanh) of ", Stackel_whichText (x), U".");
	}
}
static void do_log2 () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number <= 0.0 ? undefined : log (x->number) * NUMlog2e);
	} else {
		Melder_throw (U"Cannot take the base-2 logarithm (log2) of ", Stackel_whichText (x), U".");
	}
}
static void do_ln () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number <= 0.0 ? undefined : log (x->number));
	} else {
		Melder_throw (U"Cannot take the natural logarithm (ln) of ", Stackel_whichText (x), U".");
	}
}
static void do_log10 () {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) ? undefined :
			x->number <= 0.0 ? undefined : log10 (x->number));
	} else {
		Melder_throw (U"Cannot take the base-10 logarithm (log10) of ", Stackel_whichText (x), U".");
	}
}
static void do_sum () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (sum_scalar (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute the sum of ", Stackel_whichText (x), U".");
	}
}
static void do_mean () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (mean_scalar (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute the mean of ", Stackel_whichText (x), U".");
	}
}
static void do_stdev () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (stdev_scalar (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute the mean of ", Stackel_whichText (x), U".");
	}
}
static void do_center () {
	Stackel x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR) {
		pushNumber (center_scalar (x->numericVector));
	} else {
		Melder_throw (U"Cannot compute the center of ", Stackel_whichText (x), U".");
	}
}
static void do_function_dd_d (double (*f) (double, double)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined : f (x->number, y->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_dd_d_numvec (double (*f) (double, double)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if (a->which == Stackel_NUMERIC_VECTOR && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		long numberOfElements = a->numericVector.size;
		autonumvec newData (numberOfElements, false);
		for (long ielem = 1; ielem <= numberOfElements; ielem ++) {
			newData [ielem] = f (x->number, y->number);
		}
		pushNumericVector (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires one vector argument and two numeric arguments, not ",
			Stackel_whichText (a), U", ", Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_dd_d_nummat (double (*f) (double, double)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if (a->which == Stackel_NUMERIC_MATRIX && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		long numberOfRows = a->numericMatrix.nrow;
		long numberOfColumns = a->numericMatrix.ncol;
		autonummat newData (numberOfRows, numberOfColumns, false);
		for (long irow = 1; irow <= numberOfRows; irow ++) {
			for (long icol = 1; icol <= numberOfColumns; icol ++) {
				newData [irow] [icol] = f (x->number, y->number);
			}
		}
		pushNumericMatrix (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires one matrix argument and two numeric arguments, not ",
			Stackel_whichText (a), U", ", Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_ll_l_numvec (long (*f) (long, long)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if (a->which == Stackel_NUMERIC_VECTOR && x->which == Stackel_NUMBER) {
		long numberOfElements = a->numericVector.size;
		autonumvec newData (numberOfElements, false);
		for (long ielem = 1; ielem <= numberOfElements; ielem ++) {
			newData [ielem] = f (lround (x->number), lround (y->number));
		}
		pushNumericVector (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires one vector argument and two numeric arguments, not ",
			Stackel_whichText (a), U", ", Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_ll_l_nummat (long (*f) (long, long)) {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	if (n -> number != 3)
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol], U" requires three arguments.");
	Stackel y = pop, x = pop, a = pop;
	if (a->which == Stackel_NUMERIC_MATRIX && x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		long numberOfRows = a->numericMatrix.nrow;
		long numberOfColumns = a->numericMatrix.ncol;
		autonummat newData (numberOfRows, numberOfColumns, false);
		for (long irow = 1; irow <= numberOfRows; irow ++) {
			for (long icol = 1; icol <= numberOfColumns; icol ++) {
				newData [irow] [icol] = f (lround (x->number), lround (y->number));
			}
		}
		pushNumericMatrix (newData.move());
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires one matrix argument and two numeric arguments, not ",
			Stackel_whichText (a), U", ", Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_dl_d (double (*f) (double, long)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (x->number, lround (y->number)));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_ld_d (double (*f) (long, double)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (lround (x->number), y->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_ll_l (long (*f) (long, long)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) ? undefined :
			f (lround (x->number), lround (y->number)));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires two numeric arguments, not ",
			Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_objects_are_identical () {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		int id1 = lround (x->number), id2 = lround (y->number);
		int i = theCurrentPraatObjects -> n;
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
			Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_function_ddd_d (double (*f) (double, double, double)) {
	Stackel z = pop, y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER && z->which == Stackel_NUMBER) {
		pushNumber (isundef (x->number) || isundef (y->number) || isundef (z->number) ? undefined :
			f (x->number, y->number, z->number));
	} else {
		Melder_throw (U"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			U" requires three numeric arguments, not ", Stackel_whichText (x), U", ",
			Stackel_whichText (y), U", and ", Stackel_whichText (z), U".");
	}
}
static void do_do () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number < 1)
		Melder_throw (U"The function \"do\" requires at least one argument, namely a menu command.");
	int numberOfArguments = lround (narg->number) - 1;
	#define MAXNUM_FIELDS  40
	structStackel stack [1+MAXNUM_FIELDS];
	for (int iarg = numberOfArguments; iarg >= 0; iarg --) {
		Stackel arg = pop;
		stack [iarg] = * arg;
	}
	if (stack [0]. which != Stackel_STRING)
		Melder_throw (U"The first argument of the function \"do\" has to be a string, namely a menu command, and not ", Stackel_whichText (& stack [0]), U".");
	const char32 *command = stack [0]. string;
	if (theCurrentPraatObjects == & theForegroundPraatObjects && praatP. editor != nullptr) {
		autoMelderString valueString;
		MelderString_appendCharacter (& valueString, 1);   // TODO: check whether this is needed at all, or is just MelderString_empty enough?
		autoMelderDivertInfo divert (& valueString);
		autostring32 command2 = Melder_dup (command);   // allow the menu command to reuse the stack (?)
		Editor_doMenuCommand (praatP. editor, command2.peek(), numberOfArguments, & stack [0], nullptr, theInterpreter);
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
		if (! praat_doAction (command2.peek(), numberOfArguments, & stack [0], theInterpreter) &&
		    ! praat_doMenuCommand (command2.peek(), numberOfArguments, & stack [0], theInterpreter))
		{
			Melder_throw (U"Command \"", command, U"\" not available for current selection.");
		}
		//praat_updateSelection ();
		double value = undefined;
		if (valueString.string [0] == 1) {   // nothing written with MelderInfo by praat_doAction or praat_doMenuCommand? then the return value is the ID of the selected object
			int IOBJECT, result = 0, found = 0;
			WHERE (SELECTED) { result = IOBJECT; found += 1; }
			if (found == 1) {
				value = theCurrentPraatObjects -> list [result]. id;
			}
		} else {
			value = Melder_atof (valueString.string);   // including --undefined--
		}
		pushNumber (value);
		return;
	}
	praat_updateSelection ();   // BUG: superfluous? flickering?
	pushNumber (1);
}
static void do_doStr () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	if (narg->number < 1)
		Melder_throw (U"The function \"do$\" requires at least one argument, namely a menu command.");
	int numberOfArguments = lround (narg->number) - 1;
	#define MAXNUM_FIELDS  40
	structStackel stack [1+MAXNUM_FIELDS];
	for (int iarg = numberOfArguments; iarg >= 0; iarg --) {
		Stackel arg = pop;
		stack [iarg] = * arg;
	}
	if (stack [0]. which != Stackel_STRING)
		Melder_throw (U"The first argument of the function \"do$\" has to be a string, namely a menu command, and not ", Stackel_whichText (& stack [0]), U".");
	const char32 *command = stack [0]. string;
	if (theCurrentPraatObjects == & theForegroundPraatObjects && praatP. editor != nullptr) {
		static MelderString info;
		MelderString_empty (& info);
		autoMelderDivertInfo divert (& info);
		autostring32 command2 = Melder_dup (command);
		Editor_doMenuCommand (praatP. editor, command2.peek(), numberOfArguments, & stack [0], nullptr, theInterpreter);
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
		if (! praat_doAction (command2.peek(), numberOfArguments, & stack [0], theInterpreter) &&
		    ! praat_doMenuCommand (command2.peek(), numberOfArguments, & stack [0], theInterpreter))
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
static void shared_do_writeInfo (int numberOfArguments) {
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER) {
			MelderInfo_write (arg->number);
		} else if (arg->which == Stackel_STRING) {
			MelderInfo_write (arg->string);
		} else if (arg->which == Stackel_NUMERIC_VECTOR) {
			long numberOfElements = arg->numericVector.size;
			double *data = arg->numericVector.at;
			for (long i = 1; i <= numberOfElements; i ++) {
				MelderInfo_write (data [i], i == numberOfElements ? U"" : U" ");
			}
		} else if (arg->which == Stackel_NUMERIC_MATRIX) {
			long numberOfRows = arg->numericMatrix.nrow;
			long numberOfColumns = arg->numericMatrix.ncol;
			double **data = arg->numericMatrix.at;
			for (long irow = 1; irow <= numberOfRows; irow ++) {
				for (long icol = 1; icol <= numberOfRows; icol ++) {
					MelderInfo_write (data [irow] [icol], icol == numberOfColumns ? U"" : U" ");
				}
				MelderInfo_write (irow == numberOfRows ? U"" : U"\n");
			}
		}
	}
}
static void do_writeInfo () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	MelderInfo_open ();
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_writeInfoLine () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
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
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_appendInfoLine () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	shared_do_writeInfo (numberOfArguments);
	MelderInfo_write (U"\n");
	MelderInfo_drain ();
	pushNumber (1);
}
static void do_writeFile () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"writeFile\" has to be a string (a file name), not ", Stackel_whichText (fileName), U".");
	}
	autoMelderString text;
	for (int iarg = 2; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->string);
	}
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> string, & file);
	MelderFile_writeText (& file, text.string, Melder_getOutputEncoding ());
	pushNumber (1);
}
static void do_writeFileLine () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"writeFile\" has to be a string (a file name), not ", Stackel_whichText (fileName), U".");
	}
	autoMelderString text;
	for (int iarg = 2; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->string);
	}
	MelderString_appendCharacter (& text, U'\n');
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> string, & file);
	MelderFile_writeText (& file, text.string, Melder_getOutputEncoding ());
	pushNumber (1);
}
static void do_appendFile () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"writeFile\" has to be a string (a file name), not ", Stackel_whichText (fileName), U".");
	}
	autoMelderString text;
	for (int iarg = 2; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->string);
	}
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> string, & file);
	MelderFile_appendText (& file, text.string);
	pushNumber (1);
}
static void do_appendFileLine () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"writeFile\" is not available inside manuals.");
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName -> which != Stackel_STRING) {
		Melder_throw (U"The first argument of \"writeFile\" has to be a string (a file name), not ", Stackel_whichText (fileName), U".");
	}
	autoMelderString text;
	for (int iarg = 2; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->string);
	}
	MelderString_appendCharacter (& text, '\n');
	structMelderFile file { };
	Melder_relativePathToFile (fileName -> string, & file);
	MelderFile_appendText (& file, text.string);
	pushNumber (1);
}
static void do_pauseScript () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"pause\" is not available inside manuals.");
	if (theCurrentPraatApplication -> batch) return;   // in batch we ignore pause statements
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	autoMelderString buffer;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& buffer, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& buffer, arg->string);
	}
	UiPause_begin (theCurrentPraatApplication -> topShell, U"stop or continue", theInterpreter);
	UiPause_comment (numberOfArguments == 0 ? U"..." : buffer.string);
	UiPause_end (1, 1, 0, U"Continue", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, theInterpreter);
	pushNumber (1);
}
static void do_exitScript () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			Melder_appendError_noLine (arg->number);
		else if (arg->which == Stackel_STRING)
			Melder_appendError_noLine (arg->string);
	}
	Melder_throw (U"\nScript exited.");
	pushNumber (1);
}
static void do_runScript () {
	Stackel narg = pop;
	Melder_assert (narg->which == Stackel_NUMBER);
	int numberOfArguments = lround (narg->number);
	if (numberOfArguments < 1)
		Melder_throw (U"The function \"runScript\" requires at least one argument, namely the file name.");
	w -= numberOfArguments;
	Stackel fileName = & theStack [w + 1];
	if (fileName->which != Stackel_STRING)
		Melder_throw (U"The first argument to \"runScript\" has to be a string (the file name), not ", Stackel_whichText (fileName));
	theLevel += 1;
	try {
		praat_executeScriptFromFileName (fileName->string, numberOfArguments - 1, & theStack [w + 1]);
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
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	autoMelderString text;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->string);
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
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	autoMelderString text;
	for (int iarg = 1; iarg <= numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + iarg];
		if (arg->which == Stackel_NUMBER)
			MelderString_append (& text, arg->number);
		else if (arg->which == Stackel_STRING)
			MelderString_append (& text, arg->string);
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
	int numberOfArguments = lround (narg->number);
	w -= numberOfArguments;
	Stackel commandFile = & theStack [w + 1];
	if (commandFile->which != Stackel_STRING)
		Melder_throw (U"The first argument to \"runSubprocess\" has to be a command name.");
	autostring32vector arguments (1, numberOfArguments - 1);
	for (int iarg = 1; iarg < numberOfArguments; iarg ++) {
		Stackel arg = & theStack [w + 1 + iarg];
		if (arg->which == Stackel_NUMBER)
			arguments [iarg] = Melder_dup (Melder_double (arg->number));
		else if (arg->which == Stackel_STRING)
			arguments [iarg] = Melder_dup (arg->string);
	}
	try {
		Melder_execv (commandFile->string, numberOfArguments - 1, arguments.peek());
	} catch (MelderError) {
		Melder_throw (U"Command \"", commandFile->string, U"\" returned error status.");
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
		Melder_throw (U"The function \"min\" can only have numeric arguments, not ", Stackel_whichText (last), U".");
	result = last->number;
	for (int j = lround (n->number) - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			Melder_throw (U"The function \"min\" can only have numeric arguments, not ", Stackel_whichText (previous), U".");
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
		Melder_throw (U"The function \"max\" can only have numeric arguments, not ", Stackel_whichText (last), U".");
	result = last->number;
	for (int j = lround (n->number) - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			Melder_throw (U"The function \"max\" can only have numeric arguments, not ", Stackel_whichText (previous), U".");
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
		Melder_throw (U"The function \"imin\" can only have numeric arguments, not ", Stackel_whichText (last), U".");
	minimum = last->number;
	result = n->number;
	for (int j = lround (n->number) - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			Melder_throw (U"The function \"imin\" can only have numeric arguments, not ", Stackel_whichText (previous), U".");
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
		for (int j = lround (n->number) - 1; j > 0; j --) {
			Stackel previous = pop;
			if (previous->which != Stackel_NUMBER)
				Melder_throw (U"The function \"imax\" cannot mix a numeric argument with ", Stackel_whichText (previous), U".");
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
		long numberOfElements = last->numericVector.size;
		long result = 1;
		double maximum = last->numericVector [1];
		for (long i = 2; i <= numberOfElements; i ++) {
			if (last->numericVector [i] > maximum) {
				result = i;
				maximum = last->numericVector [i];
			}
		}
		pushNumber (result);
	} else {
		Stackel nn = pop;
		Melder_throw (U"Cannot compute the imax of ", Stackel_whichText (nn), U".");
	}
}
static void do_zeroNumvec () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	int rank = lround (n -> number);
	if (rank < 1)
		Melder_throw (U"The function \"zero#\" requires an argument.");
	if (rank > 1) {
		Melder_throw (U"The function \"zero#\" cannot have more than one argument (consider using zero##).");
	}
	Stackel nelem = pop;
	if (nelem -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"zero#\", the number of elements has to be a number, not ", Stackel_whichText (nelem), U".");
	double numberOfElements = nelem -> number;
	if (isundef (numberOfElements))
		Melder_throw (U"In the function \"zero#\", the number of elements is undefined.");
	if (numberOfElements < 0.0)
		Melder_throw (U"In the function \"zero#\", the number of elements should not be negative.");
	autonumvec result (lround (numberOfElements), true);
	pushNumericVector (result.move());
}
static void do_zeroNummat () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	int rank = lround (n -> number);
	if (rank != 2)
		Melder_throw (U"The function \"zero##\" requires two arguments.");
	Stackel ncol = pop;
	if (ncol -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"zero##\", the number of columns has to be a number, not ", Stackel_whichText (ncol), U".");
	double numberOfColumns = ncol -> number;
	Stackel nrow = pop;
	if (nrow -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"zero##\", the number of rows has to be a number, not ", Stackel_whichText (nrow), U".");
	double numberOfRows = nrow -> number;
	if (isundef (numberOfRows))
		Melder_throw (U"In the function \"zero##\", the number of rows is undefined.");
	if (isundef (numberOfColumns))
		Melder_throw (U"In the function \"zero##\", the number of columns is undefined.");
	if (numberOfRows < 0.0)
		Melder_throw (U"In the function \"zero##\", the number of rows should not be negative.");
	if (numberOfColumns < 0.0)
		Melder_throw (U"In the function \"zero##\", the number of columns should not be negative.");
	autonummat result (lround (numberOfRows), lround (numberOfColumns), true);
	pushNumericMatrix (result.move());
}
static void do_linearNumvec () {
	Stackel stackel_narg = pop;
	Melder_assert (stackel_narg -> which == Stackel_NUMBER);
	int narg = lround (stackel_narg -> number);
	if (narg < 3 || narg > 4)
		Melder_throw (U"The function \"linear#\" requires three or four arguments.");
	bool excludeEdges = false;   // default
	if (narg == 4) {
		Stackel stack_excludeEdges = pop;
		if (stack_excludeEdges -> which != Stackel_NUMBER)
			Melder_throw (U"In the function \"linear#\", the edge exclusion flag (fourth argument) has to be a number, not ", Stackel_whichText (stack_excludeEdges), U".");
		excludeEdges = lround (stack_excludeEdges -> number);
	}
	Stackel stack_numberOfSteps = pop, stack_maximum = pop, stack_minimum = pop;
	if (stack_minimum -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"linear#\", the minimum (first argument) has to be a number, not ", Stackel_whichText (stack_minimum), U".");
	double minimum = stack_minimum -> number;
	if (isundef (minimum))
		Melder_throw (U"Undefined minimum in the function \"linear#\" (first argument).");
	if (stack_maximum -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"linear#\", the maximum (second argument) has to be a number, not ", Stackel_whichText (stack_maximum), U".");
	double maximum = stack_maximum -> number;
	if (isundef (maximum))
		Melder_throw (U"Undefined maximum in the function \"linear#\" (second argument).");
	if (maximum < minimum)
		Melder_throw (U"Maximum (", maximum, U") smaller than minimum (", minimum, U") in function \"linear#\".");
	if (stack_numberOfSteps -> which != Stackel_NUMBER)
		Melder_throw (U"In the function \"linear#\", the number of steps (third argument) has to be a number, not ", Stackel_whichText (stack_numberOfSteps), U".");
	if (isundef (stack_numberOfSteps -> number))
		Melder_throw (U"Undefined number of steps in the function \"linear#\" (third argument).");
	long numberOfSteps = lround (stack_numberOfSteps -> number);
	if (numberOfSteps <= 0)
		Melder_throw (U"In the function \"linear#\", the number of steps (third argument) has to be positive, not ", numberOfSteps, U".");
	autonumvec result { numberOfSteps, false };
	for (long ielem = 1; ielem <= numberOfSteps; ielem ++) {
		result [ielem] = excludeEdges ?
			minimum + (ielem - 0.5) * (maximum - minimum) / numberOfSteps :
			minimum + (ielem - 1) * (maximum - minimum) / (numberOfSteps - 1);
	}
	if (! excludeEdges) result [numberOfSteps] = maximum;   // remove rounding problems
	pushNumericVector (result.move());
}
static void do_peaksNummat () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number != 4)
		Melder_throw (U"The function peaks## requires four arguments (vector, edges, interpolation, sortByHeight).");
	Stackel s = pop;
	if (s->which != Stackel_NUMBER)
		Melder_throw (U"The fourth argument to peaks## has to be a number, not ", Stackel_whichText (s), U".");
	bool sortByHeight = s->number != 0.0;
	Stackel i = pop;
	if (i->which != Stackel_NUMBER)
		Melder_throw (U"The third argument to peaks## has to be a number, not ", Stackel_whichText (i), U".");
	int interpolation = lround (i->number);
	Stackel e = pop;
	if (e->which != Stackel_NUMBER)
		Melder_throw (U"The second argument to peaks## has to be a number, not ", Stackel_whichText (e), U".");
	bool includeEdges = e->number != 0.0;
	Stackel vec = pop;
	if (vec->which != Stackel_NUMERIC_VECTOR)
		Melder_throw (U"The first argument to peaks## has to be a numeric vector, not ", Stackel_whichText (vec), U".");
	autonummat result = peaks_nummat (vec->numericVector, includeEdges, interpolation, sortByHeight);
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
		Melder_throw (U"The function size requires a vector argument, not ", Stackel_whichText (array), U".");
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
			U" requires a matrix argument, not ", Stackel_whichText (array), U".");
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
			U" requires a matrix argument, not ", Stackel_whichText (array), U".");
	}
}
static void do_editor () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->number == 0) {
		if (theInterpreter && theInterpreter -> editorClass) {
			praatP. editor = praat_findEditorFromString (theInterpreter -> environmentName);
		} else {
			Melder_throw (U"The function \"editor\" requires an argument when called from outside an editor.");
		}
	} else if (n->number == 1) {
		Stackel editor = pop;
		if (editor->which == Stackel_STRING) {
			praatP. editor = praat_findEditorFromString (editor->string);
		} else if (editor->which == Stackel_NUMBER) {
			praatP. editor = praat_findEditorById (lround (editor->number));
		} else {
			Melder_throw (U"The function \"editor\" requires a numeric or string argument, not ", Stackel_whichText (editor), U".");
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
			double result = NUMhashString (s->string);
			pushNumber (result);
		} else {
			Melder_throw (U"The function \"hash\" requires a string, not ", Stackel_whichText (s), U".");
		}
	} else {
		Melder_throw (U"The function \"hash\" requires 1 argument, not ", n->number, U".");
	}
}

static void do_numericVectorElement () {
	InterpreterVariable vector = parse [programPointer]. content.variable;
	long element = 1;   // default
	Stackel r = pop;
	if (r -> which != Stackel_NUMBER)
		Melder_throw (U"In vector indexing, the index has to be a number, not ", Stackel_whichText (r), U".");
	if (isundef (r -> number))
		Melder_throw (U"The element index is undefined.");
	element = lround (r -> number);
	if (element <= 0)
		Melder_throw (U"In vector indexing, the element index has to be positive.");
	if (element > vector -> numericVectorValue.size)
		Melder_throw (U"Element index out of bounds.");
	pushNumber (vector -> numericVectorValue [element]);
}
static void do_numericMatrixElement () {
	InterpreterVariable matrix = parse [programPointer]. content.variable;
	long row = 1, column = 1;   // default
	Stackel c = pop;
	if (c -> which != Stackel_NUMBER)
		Melder_throw (U"In matrix indexing, the column index has to be a number, not ", Stackel_whichText (c), U".");
	if (isundef (c -> number))
		Melder_throw (U"The column index is undefined.");
	column = lround (c -> number);
	if (column <= 0)
		Melder_throw (U"In matrix indexing, the column index has to be positive.");
	if (column > matrix -> numericMatrixValue. ncol)
		Melder_throw (U"Column index out of bounds.");
	Stackel r = pop;
	if (r -> which != Stackel_NUMBER)
		Melder_throw (U"In matrix indexing, the row index has to be a number, not ", Stackel_whichText (r), U".");
	if (isundef (r -> number))
		Melder_throw (U"The row index is undefined.");
	row = lround (r -> number);
	if (row <= 0)
		Melder_throw (U"In matrix indexing, the row index has to be positive.");
	if (row > matrix -> numericMatrixValue. nrow)
		Melder_throw (U"Row index out of bounds.");
	pushNumber (matrix -> numericMatrixValue [row] [column]);
}
static void do_indexedNumericVariable () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	int nindex = lround (n -> number);
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
			MelderString_append (& totalVariableName, U"\"", index -> string, U"\"", iindex == nindex ? U"]" : U",");
		} else {
			Melder_throw (U"In indexed variables, the index has to be a number or a string, not ", Stackel_whichText (index), U".");
		}
	}
	InterpreterVariable var = Interpreter_hasVariable (theInterpreter, totalVariableName.string);
	if (! var)
		Melder_throw (U"Undefined indexed variable " U_LEFT_GUILLEMET, totalVariableName.string, U_RIGHT_GUILLEMET U".");
	pushNumber (var -> numericValue);
}
static void do_indexedStringVariable () {
	Stackel n = pop;
	Melder_assert (n -> which == Stackel_NUMBER);
	int nindex = lround (n -> number);
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
			MelderString_append (& totalVariableName, U"\"", index -> string, U"\"", iindex == nindex ? U"]" : U",");
		} else {
			Melder_throw (U"In indexed variables, the index has to be a number or a string, not ", Stackel_whichText (index), U".");
		}
	}
	InterpreterVariable var = Interpreter_hasVariable (theInterpreter, totalVariableName.string);
	if (! var)
		Melder_throw (U"Undefined indexed variable " U_LEFT_GUILLEMET, totalVariableName.string, U_RIGHT_GUILLEMET U".");
	autostring32 result = Melder_dup (var -> stringValue);
	pushString (result.transfer());
}
static void do_length () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		double result = str32len (s->string);
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"length\" requires a string, not ", Stackel_whichText (s), U".");
	}
}
static void do_number () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		double result = Melder_atof (s->string);
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"number\" requires a string, not ", Stackel_whichText (s), U".");
	}
}
static void do_fileReadable () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (s->string, & file);
		pushNumber (MelderFile_readable (& file));
	} else {
		Melder_throw (U"The function \"fileReadable\" requires a string, not ", Stackel_whichText (s), U".");
	}
}
static void do_dateStr () {
	time_t today = time (nullptr);
	char32 *date, *newline;
	date = Melder_8to32 (ctime (& today));
	newline = str32chr (date, U'\n');
	if (newline) *newline = U'\0';
	pushString (date);
}
static void do_infoStr () {
	char32 *info = Melder_dup (Melder_getInfo ());
	pushString (info);
}
static void do_leftStr () {
	trace (U"enter");
	Stackel narg = pop;
	if (narg->number == 1 || narg->number == 2) {
		Stackel x = ( narg->number == 2 ? pop : nullptr ), s = pop;
		if (s->which == Stackel_STRING && (x == nullptr || x->which == Stackel_NUMBER)) {
			long newlength = x ? lround (x->number) : 1;
			long length = str32len (s->string);
			if (newlength < 0) newlength = 0;
			if (newlength > length) newlength = length;
			#if 0
				autostring s1;
				autostring s2 = s1;   // copy constructor disabled
				s1 = s2;
			#endif
			autostring32 result = Melder_malloc (char32, newlength + 1);
			str32ncpy (result.peek(), s->string, newlength);
			result [newlength] = '\0';
			pushString (result.transfer());
		} else {
			Melder_throw (U"The function \"left$\" requires a string, or a string and a number.");
		}
	} else {
		Melder_throw (U"The function \"left$\" requires one or two arguments.");
	}
	trace (U"exit");
}
static void do_rightStr () {
	Stackel narg = pop;
	if (narg->number == 1 || narg->number == 2) {
		Stackel x = ( narg->number == 2 ? pop : nullptr ), s = pop;
		if (s->which == Stackel_STRING && (x == nullptr || x->which == Stackel_NUMBER)) {
			long newlength = x ? lround (x->number) : 1;
			long length = str32len (s->string);
			if (newlength < 0) newlength = 0;
			if (newlength > length) newlength = length;
			pushString (Melder_dup (s->string + length - newlength));
		} else {
			Melder_throw (U"The function \"right$\" requires a string, or a string and a number.");
		}
	} else {
		Melder_throw (U"The function \"right$\" requires one or two arguments.");
	}
}
static void do_midStr () {
	Stackel narg = pop;
	if (narg->number == 2 || narg->number == 3) {
		Stackel y = ( narg->number == 3 ? pop : nullptr ), x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER && (y == nullptr || y->which == Stackel_NUMBER)) {
			long newlength = y ? lround (y->number) : 1;
			long start = lround (x->number);
			long length = str32len (s->string), finish = start + newlength - 1;
			autostring32 result;
			if (start < 1) start = 1;
			if (finish > length) finish = length;
			newlength = finish - start + 1;
			if (newlength > 0) {
				result.reset (Melder_malloc (char32, newlength + 1));
				str32ncpy (result.peek(), s->string + start - 1, newlength);
				result [newlength] = '\0';
			} else {
				result.reset (Melder_dup (U""));
			}
			pushString (result.transfer());
		} else {
			Melder_throw (U"The function \"mid$\" requires a string and one or two numbers.");
		}
	} else {
		Melder_throw (U"The function \"mid$\" requires two or three arguments.");
	}
}
static void do_unicodeToBackslashTrigraphsStr () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		long length = str32len (s->string);
		autostring32 trigraphs = Melder_calloc (char32, 3 * length + 1);
		Longchar_genericize32 (s->string, trigraphs.peek());
		pushString (trigraphs.transfer());
	} else {
		Melder_throw (U"The function \"unicodeToBackslashTrigraphs$\" requires a string, not ", Stackel_whichText (s), U".");
	}
}
static void do_backslashTrigraphsToUnicodeStr () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		long length = str32len (s->string);
		//autostring32 unicode = Melder_calloc (char32, length + 1);
		//Longchar_nativize32 (s->string, unicode.peek(), false);
		//pushString (unicode.transfer());
		char32 *unicode = Melder_calloc (char32, length + 1);   // OPTIMIZE
		Longchar_nativize32 (s->string, unicode, false);   // noexcept
		pushString (unicode);
	} else {
		Melder_throw (U"The function \"unicodeToBackslashTrigraphs$\" requires a string, not ", Stackel_whichText (s), U".");
	}
}
static void do_environmentStr () {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		char32 *value = Melder_getenv (s->string);
		//autostring32 result = Melder_dup (value ? value : U"");
		//pushString (result.transfer());
		pushString (Melder_dup (value ? value : U""));
	} else {
		Melder_throw (U"The function \"environment$\" requires a string, not ", Stackel_whichText (s), U".");
	}
}
static void do_index () {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->string, t->string);
		long result = substring ? substring - s->string + 1 : 0;
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"index\" requires two strings, not ",
			Stackel_whichText (s), U" and ", Stackel_whichText (t), U".");
	}
}
static void do_rindex () {
	Stackel part = pop, whole = pop;
	if (whole->which == Stackel_STRING && part->which == Stackel_STRING) {
		char32 *lastSubstring = str32str (whole->string, part->string);
		if (part->string [0] == '\0') {
			long result = str32len (whole->string);
			pushNumber (result);
		} else if (lastSubstring) {
			for (;;) {
				char32 *substring = str32str (lastSubstring + 1, part->string);
				if (! substring) break;
				lastSubstring = substring;
			}
			pushNumber (lastSubstring - whole->string + 1);
		} else {
			pushNumber (0);
		}
	} else {
		Melder_throw (U"The function \"rindex\" requires two strings, not ",
			Stackel_whichText (whole), U" and ", Stackel_whichText (part), U".");
	}
}
static void do_stringMatchesCriterion (int criterion) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		int result = Melder_stringMatchesCriterion (s->string, criterion, t->string);
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", Stackel_whichText (s), U" and ", Stackel_whichText (t), U".");
	}
}
static void do_index_regex (int backward) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		const char32 *errorMessage;
		regexp *compiled_regexp = CompileRE (t->string, & errorMessage, 0);
		if (! compiled_regexp) {
			Melder_throw (U"index_regex(): ", errorMessage, U".");
		} else {
			if (ExecRE (compiled_regexp, nullptr, s->string, nullptr, backward, '\0', '\0', nullptr, nullptr, nullptr)) {
				char32 *place = (char32 *) compiled_regexp -> startp [0];
				pushNumber (place - s->string + 1);
				free (compiled_regexp);
			} else {
				pushNumber (false);
			}
		}
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", Stackel_whichText (s), U" and ", Stackel_whichText (t), U".");
	}
}
static void do_replaceStr () {
	Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		long numberOfMatches;
		//autostring32 result = str_replace_literal (s->string, t->string, u->string, lround (x->number), & numberOfMatches);
		//pushString (result.transfer());
		char32 *result = str_replace_literal (s->string, t->string, u->string, lround (x->number), & numberOfMatches);
		pushString (result);
	} else {
		Melder_throw (U"The function \"replace$\" requires three strings and a number.");
	}
}
static void do_replace_regexStr () {
	Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		const char32 *errorMessage;
		regexp *compiled_regexp = CompileRE (t->string, & errorMessage, 0);
		if (! compiled_regexp) {
			Melder_throw (U"replace_regex$(): ", errorMessage, U".");
		} else {
			long numberOfMatches;
			//autostring32 result = str_replace_regexp (s->string, compiled_regexp, u->string, lround (x->number), & numberOfMatches);
			//pushString (result.transfer());
			char32 *result = str_replace_regexp (s->string, compiled_regexp, u->string, lround (x->number), & numberOfMatches);
			pushString (result);
		}
	} else {
		Melder_throw (U"The function \"replace_regex$\" requires three strings and a number.");
	}
}
static void do_extractNumber () {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->string, t->string);
		if (! substring) {
			pushNumber (undefined);
		} else {
			/* Skip the prompt. */
			substring += str32len (t->string);
			/* Skip white space. */
			while (*substring == U' ' || *substring == U'\t' || *substring == U'\n' || *substring == U'\r') substring ++;
			if (substring [0] == U'\0' || str32nequ (substring, U"--undefined--", 13)) {
				pushNumber (undefined);
			} else {
				char32 buffer [101], *slash;
				int i;
				for (i = 0; i < 100; i ++) {
					buffer [i] = *substring;
					substring ++;
					if (*substring == U'\0' || *substring == U' ' || *substring == U'\t' || *substring == U'\n' || *substring == U'\r') break;
				}
				if (i >= 100) {
					buffer [100] = U'\0';
					pushNumber (Melder_atof (buffer));
				} else {
					buffer [i + 1] = U'\0';
					slash = str32chr (buffer, U'/');
					if (slash) {
						double numerator, denominator;
						*slash = U'\0';
						numerator = Melder_atof (buffer), denominator = Melder_atof (slash + 1);
						pushNumber (numerator / denominator);
					} else {
						pushNumber (Melder_atof (buffer));
					}
				}
			}
		}
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", Stackel_whichText (s), U" and ", Stackel_whichText (t), U".");
	}
}
static void do_extractTextStr (bool singleWord) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char32 *substring = str32str (s->string, t->string);
		autostring32 result;
		if (! substring) {
			result.reset (Melder_dup (U""));
		} else {
			long length;
			/* Skip the prompt. */
			substring += str32len (t->string);
			if (singleWord) {
				/* Skip white space. */
				while (*substring == U' ' || *substring == U'\t' || *substring == U'\n' || *substring == U'\r') substring ++;
			}
			char32 *p = substring;
			if (singleWord) {
				/* Proceed until next white space. */
				while (*p != U'\0' && *p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\r') p ++;
			} else {
				/* Proceed until end of line. */
				while (*p != U'\0' && *p != U'\n' && *p != U'\r') p ++;
			}
			length = p - substring;
			result.reset (Melder_malloc (char32, length + 1));
			str32ncpy (result.peek(), substring, length);
			result [length] = U'\0';
		}
		pushString (result.transfer());
	} else {
		Melder_throw (U"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			U"\" requires two strings, not ", Stackel_whichText (s), U" and ", Stackel_whichText (t), U".");
	}
}
static void do_selected () {
	Stackel n = pop;
	long result = 0;
	if (n->number == 0) {
		result = praat_idOfSelected (nullptr, 0);
	} else if (n->number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (a->string, nullptr);
			result = praat_idOfSelected (klas, 0);
		} else if (a->which == Stackel_NUMBER) {
			result = praat_idOfSelected (nullptr, lround (a->number));
		} else {
			Melder_throw (U"The function \"selected\" requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->string, nullptr);
			result = praat_idOfSelected (klas, lround (x->number));
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
		result.reset (Melder_dup (praat_nameOfSelected (nullptr, 0)));
	} else if (n->number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (a->string, nullptr);
			result.reset (Melder_dup (praat_nameOfSelected (klas, 0)));
		} else if (a->which == Stackel_NUMBER) {
			result.reset (Melder_dup (praat_nameOfSelected (nullptr, lround (a->number))));
		} else {
			Melder_throw (U"The function \"selected$\" requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->string, nullptr);
			result.reset (Melder_dup (praat_nameOfSelected (klas, lround (x->number))));
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
			ClassInfo klas = Thing_classFromClassName (a->string, nullptr);
			resultSource = praat_nameOfSelected (klas, 0);
		} else if (a->which == Stackel_NUMBER) {
			resultSource = praat_nameOfSelected (nullptr, lround (a->number));
		} else {
			Melder_throw (U"The function \"selected$\" requires a string (an object type name) and/or a number.");
		}
	} else if (n->number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			ClassInfo klas = Thing_classFromClassName (s->string, nullptr);
			resultSource = praat_nameOfSelected (klas, lround (x->number));
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
	long result = 0;
	if (n->number == 0) {
		result = praat_numberOfSelected (nullptr);
	} else if (n->number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			ClassInfo klas = Thing_classFromClassName (s->string, nullptr);
			result = praat_numberOfSelected (klas);
		} else {
			Melder_throw (U"The function \"numberOfSelected\" requires a string (an object type name), not ", Stackel_whichText (s), U".");
		}
	} else {
		Melder_throw (U"The function \"numberOfSelected\" requires 0 or 1 arguments, not ", n->number, U".");
	}
	pushNumber (result);
}
static void do_selectObject () {
	Stackel n = pop;
	praat_deselectAll ();
	for (int iobject = 1; iobject <= n -> number; iobject ++) {
		Stackel object = pop;
		if (object -> which == Stackel_NUMBER) {
			int IOBJECT = praat_findObjectById (lround (object -> number));
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectFromString (object -> string);
			praat_select (IOBJECT);
		} else {
			Melder_throw (U"The function \"selectObject\" takes numbers and strings, not ", Stackel_whichText (object));
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
			int IOBJECT = praat_findObjectById (lround (object -> number));
			praat_select (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectFromString (object -> string);
			praat_select (IOBJECT);
		} else {
			Melder_throw (U"The function \"plusObject\" takes numbers and strings, not ", Stackel_whichText (object), U".");
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
			int IOBJECT = praat_findObjectById (lround (object -> number));
			praat_deselect (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectFromString (object -> string);
			praat_deselect (IOBJECT);
		} else {
			Melder_throw (U"The function \"minusObject\" takes numbers and strings, not ", Stackel_whichText (object), U".");
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
			int IOBJECT = praat_findObjectById (lround (object -> number));
			praat_removeObject (IOBJECT);
		} else if (object -> which == Stackel_STRING) {
			int IOBJECT = praat_findObjectFromString (object -> string);
			praat_removeObject (IOBJECT);
		} else {
			Melder_throw (U"The function \"removeObject\" takes numbers and strings, not ", Stackel_whichText (object), U".");
		}
	}
	praat_show ();
	pushNumber (1);
}
static void do_object_xmin () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetXmin ()) {
			pushNumber (data -> v_getXmin ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"xmin\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].xmin\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_xmax () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetXmax ()) {
			pushNumber (data -> v_getXmax ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"xmax\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].xmax\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_ymin () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetYmin ()) {
			pushNumber (data -> v_getYmin ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"ymin\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].ymin\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_ymax () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetYmax ()) {
			pushNumber (data -> v_getYmax ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"ymax\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].ymax\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_nx () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetNx ()) {
			pushNumber (data -> v_getNx ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"nx\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].nx\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_ny () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetNy ()) {
			pushNumber (data -> v_getNy ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"ny\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].ny\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_dx () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetDx ()) {
			pushNumber (data -> v_getDx ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"dx\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].dx\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_dy () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetDy ()) {
			pushNumber (data -> v_getDy ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"dy\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].dy\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_nrow () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetNrow ()) {
			pushNumber (data -> v_getNrow ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"nrow\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].nrow\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_ncol () {
	Stackel object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetNcol ()) {
			pushNumber (data -> v_getNcol ());
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"ncol\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].ncol\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_rowstr () {
	Stackel index = pop, object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetRowStr ()) {
			if (index -> which == Stackel_NUMBER) {
				long number = lround (index->number);
				autostring32 result = Melder_dup (data -> v_getRowStr (number));
				if (! result.peek())
					Melder_throw (U"Row index out of bounds.");
				pushString (result.transfer());
			} else {
				Melder_throw (U"The expression \"object[].row$[xx]\" requires xx to be a number, not ", Stackel_whichText (index), U".");
			}
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"row$[]\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].row$[]\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_object_colstr () {
	Stackel index = pop, object = pop;
	if (object -> which == Stackel_NUMBER || object -> which == Stackel_STRING) {
		int IOBJECT = object -> which == Stackel_NUMBER ?
			praat_findObjectById (lround (object -> number)) :
			praat_findObjectFromString (object -> string);
		Daata data = OBJECT;
		if (data -> v_hasGetColStr ()) {
			if (index -> which == Stackel_NUMBER) {
				long number = lround (index->number);
				autostring32 result = Melder_dup (data -> v_getColStr (number));
				if (! result.peek())
					Melder_throw (U"Column index out of bounds.");
				pushString (result.transfer());
			} else {
				Melder_throw (U"The expression \"object[].col$[xx]\" requires xx to be a number, not ", Stackel_whichText (index), U".");
			}
		} else {
			Melder_throw (U"An object of type ", Thing_className (data), U" has no \"col$[]\" attribute.");
		}
	} else {
		Melder_throw (U"The expression \"object[xx].col$[]\" requires xx to be a number or a string, not ", Stackel_whichText (object), U".");
	}
}
static void do_stringStr () {
	Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		//autostring32 result = Melder_dup (Melder_double (value->number));
		pushString (Melder_dup (Melder_double (value->number)));
	} else {
		Melder_throw (U"The function \"string$\" requires a number, not ", Stackel_whichText (value), U".");
	}
}
static void do_sleep () {
	Stackel value = pop;
	if (value->which == Stackel_NUMBER) {
		Melder_sleep (value->number);
		pushNumber (1);
	} else {
		Melder_throw (U"The function \"sleep\" requires a number, not ", Stackel_whichText (value), U".");
	}
}
static void do_fixedStr () {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		//autostring32 result = Melder_dup (Melder_fixed (value->number, lround (precision->number)));
		pushString (Melder_dup (Melder_fixed (value->number, lround (precision->number))));
	} else {
		Melder_throw (U"The function \"fixed$\" requires two numbers (value and precision), not ", Stackel_whichText (value), U" and ", Stackel_whichText (precision), U".");
	}
}
static void do_percentStr () {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		autostring32 result = Melder_dup (Melder_percent (value->number, lround (precision->number)));
		pushString (result.transfer());
	} else {
		Melder_throw (U"The function \"percent$\" requires two numbers (value and precision), not ", Stackel_whichText (value), U" and ", Stackel_whichText (precision), U".");
	}
}
static void do_deleteFile () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"deleteFile\" is not available inside manuals.");
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->string, & file);
		MelderFile_delete (& file);
		pushNumber (1);
	} else {
		Melder_throw (U"The function \"deleteFile\" requires a string, not ", Stackel_whichText (f), U".");
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
			Melder_createDirectory (& currentDirectory, f->string, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#else
			Melder_createDirectory (& currentDirectory, f->string, 0);
		#endif
		pushNumber (1);
	} else {
		Melder_throw (U"The function \"createDirectory\" requires a string, not ", Stackel_whichText (f), U".");
	}
}
static void do_variableExists () {
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		bool result = Interpreter_hasVariable (theInterpreter, f->string) != nullptr;
		pushNumber (result);
	} else {
		Melder_throw (U"The function \"variableExists\" requires a string, not ", Stackel_whichText (f), U".");
	}
}
static void do_readFile () {
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->string, & file);
		autostring32 text = MelderFile_readText (& file);
		pushNumber (Melder_atof (text.peek()));
	} else {
		Melder_throw (U"The function \"readFile\" requires a string (a file name), not ", Stackel_whichText (f), U".");
	}
}
static void do_readFileStr () {
	Stackel f = pop;
	if (f->which == Stackel_STRING) {
		structMelderFile file { };
		Melder_relativePathToFile (f->string, & file);
		autostring32 text = MelderFile_readText (& file);
		pushString (text.transfer());
	} else {
		Melder_throw (U"The function \"readFile$\" requires a string (a file name), not ", Stackel_whichText (f), U".");
	}
}
static void do_numericVectorLiteral () {
	Stackel n = pop;
	Melder_assert (n->which == Stackel_NUMBER);
	long numberOfElements = lround (n->number);
	Melder_assert (numberOfElements > 0);
	autonumvec result { numberOfElements, false };
	for (long ielement = numberOfElements; ielement > 0; ielement --) {
		Stackel e = pop;
		if (e->which != Stackel_NUMBER)
			Melder_throw (U"Vector element has to be a number, not ", Stackel_whichText (e));
		result [ielement] = e->number;
	}
	pushNumericVector (result.move());
}
static void do_outerNummat () {
	/*
		result## = outer## (x#, y#)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_VECTOR) {
		autonummat result = outer_nummat (x->numericVector, y->numericVector);
		pushNumericMatrix (result.move());
	} else {
		Melder_throw (U"The function \"outer##\" requires two vectors, not ", Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_mulNumvec () {
	/*
		result# = mul# (x.., y..)
	*/
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && y->which == Stackel_NUMERIC_MATRIX) {
		/*
			result# = mul# (x#, y##)
		*/
		long xn = x->numericVector.size, ynrow = y->numericMatrix.nrow, yncol = y->numericMatrix.ncol;
		if (ynrow != xn)
			Melder_throw (U"In the function \"mul#\", the dimension of the vector and the number of rows of the matrix should be equal, "
				"not ", xn, U" and ", ynrow);
		autonumvec result { yncol, false };
		for (long j = 1; j <= yncol; j ++) {
			result [j] = 0.0;
			for (long i = 1; i <= ynrow; i ++) {
				double xvalue = x->numericVector [i];
				double yvalue = y->numericMatrix [i] [j];
				result [j] += xvalue * yvalue;
			}
		}
		pushNumericVector (result.move());
	} else if (x->which == Stackel_NUMERIC_MATRIX && y->which == Stackel_NUMERIC_VECTOR) {
		/*
			result# = mul# (x##, y#)
		*/
		long xnrow = x->numericMatrix.nrow, xncol = x->numericMatrix.ncol, yn = y->numericVector.size;
		if (yn != xncol)
			Melder_throw (U"In the function \"mul#\", the number of columns of the matrix and the dimension of the vector should be equal, "
				"not ", xncol, U" and ", yn);
		autonumvec result { xnrow, false };
		for (long i = 1; i <= xnrow; i ++) {
			result [i] = 0.0;
			for (long j = 1; j <= xncol; j ++) {
				double xvalue = x->numericMatrix [i] [j];
				double yvalue = y->numericVector [j];
				result [i] += xvalue * yvalue;
			}
		}
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function \"mul#\" requires a vector and a matrix, not ", Stackel_whichText (x), U" and ", Stackel_whichText (y), U".");
	}
}
static void do_repeatNumvec () {
	Stackel n = pop, x = pop;
	if (x->which == Stackel_NUMERIC_VECTOR && n->which == Stackel_NUMBER) {
		long n_old = x->numericVector.size;
		long times = lround (n->number);
		autonumvec result { n_old * times, false };
		for (long i = 1; i <= times; i ++) {
			for (long j = 1; j <= n_old; j ++) {
				result [(i - 1) * n_old + j] = x->numericVector [j];
			}
		}
		pushNumericVector (result.move());
	} else {
		Melder_throw (U"The function \"repeat#\" requires a vector and a number, not ", Stackel_whichText (x), U" and ", Stackel_whichText (n), U".");
	}
}
static void do_beginPauseForm () {
	if (theCurrentPraatObjects != & theForegroundPraatObjects)
		Melder_throw (U"The function \"beginPauseForm\" is not available inside manuals.");
	Stackel n = pop;
	if (n->number == 1) {
		Stackel title = pop;
		if (title->which == Stackel_STRING) {
			UiPause_begin (theCurrentPraatApplication -> topShell, title->string, theInterpreter);
		} else {
			Melder_throw (U"The function \"beginPauseForm\" requires a string (the title), not ", Stackel_whichText (title), U".");
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
		const char32 *defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->string;
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"real\" (the default value) must be a string or a number, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_real (label->string, defaultString);
		} else {
			Melder_throw (U"The first argument of \"real\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		const char32 *defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->string;
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"positive\" (the default value) must be a string or a number, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_positive (label->string, defaultString);
		} else {
			Melder_throw (U"The first argument of \"positive\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		const char32 *defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->string;
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"integer\" (the default value) must be a string or a number, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_integer (label->string, defaultString);
		} else {
			Melder_throw (U"The first argument of \"integer\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		const char32 *defaultString = nullptr;
		if (defaultValue->which == Stackel_STRING) {
			defaultString = defaultValue->string;
		} else if (defaultValue->which == Stackel_NUMBER) {
			defaultString = Melder_double (defaultValue->number);
		} else {
			Melder_throw (U"The second argument of \"natural\" (the default value) must be a string or a number, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_natural (label->string, defaultString);
		} else {
			Melder_throw (U"The first argument of \"natural\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		if (defaultValue->which != Stackel_STRING) {
			Melder_throw (U"The second argument of \"word\" (the default value) must be a string, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_word (label->string, defaultValue->string);
		} else {
			Melder_throw (U"The first argument of \"word\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		if (defaultValue->which != Stackel_STRING) {
			Melder_throw (U"The second argument of \"sentence\" (the default value) must be a string, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_sentence (label->string, defaultValue->string);
		} else {
			Melder_throw (U"The first argument of \"sentence\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		if (defaultValue->which != Stackel_STRING) {
			Melder_throw (U"The second argument of \"text\" (the default value) must be a string, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_text (label->string, defaultValue->string);
		} else {
			Melder_throw (U"The first argument of \"text\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
		if (defaultValue->which != Stackel_NUMBER) {
			Melder_throw (U"The second argument of \"boolean\" (the default value) must be a number (0 or 1), not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_boolean (label->string, lround (defaultValue->number));
		} else {
			Melder_throw (U"The first argument of \"boolean\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
			Melder_throw (U"The second argument of \"choice\" (the default value) must be a whole number, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_choice (label->string, lround (defaultValue->number));
		} else {
			Melder_throw (U"The first argument of \"choice\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
			Melder_throw (U"The second argument of \"optionMenu\" (the default value) must be a whole number, not ", Stackel_whichText (defaultValue), U".");
		}
		Stackel label = pop;
		if (label->which == Stackel_STRING) {
			UiPause_optionMenu (label->string, lround (defaultValue->number));
		} else {
			Melder_throw (U"The first argument of \"optionMenu\" (the label) must be a string, not ", Stackel_whichText (label), U".");
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
			UiPause_option (text->string);
		} else {
			Melder_throw (U"The argument of \"option\" must be a string (the text), not ", Stackel_whichText (text), U".");
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
			UiPause_comment (text->string);
		} else {
			Melder_throw (U"The argument of \"comment\" must be a string (the text), not ", Stackel_whichText (text), U".");
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
		Melder_throw (U"The last argument of \"endPause\" has to be a number (the default or cancel continue button), not ", Stackel_whichText (d), U".");
	int numberOfContinueButtons = lround (n->number) - 1;
	int cancelContinueButton = 0, defaultContinueButton = lround (d->number);
	Stackel ca = pop;
	if (ca->which == Stackel_NUMBER) {
		cancelContinueButton = defaultContinueButton;
		defaultContinueButton = lround (ca->number);
		numberOfContinueButtons --;
		if (cancelContinueButton < 1 || cancelContinueButton > numberOfContinueButtons)
			Melder_throw (U"Your last argument of \"endPause\" is the number of the cancel button; it cannot be ", cancelContinueButton,
				U" but has to lie between 1 and ", numberOfContinueButtons, U".");
	}
	Stackel co [1+10] = { 0 };
	for (int i = numberOfContinueButtons; i >= 1; i --) {
		co [i] = cancelContinueButton != 0 || i != numberOfContinueButtons ? pop : ca;
		if (co[i]->which != Stackel_STRING)
			Melder_throw (U"Each of the first ", numberOfContinueButtons,
				U" argument(s) of \"endPause\" has to be a string (a button text), not ", Stackel_whichText (co[i]), U".");
	}
	int buttonClicked = UiPause_end (numberOfContinueButtons, defaultContinueButton, cancelContinueButton,
		! co [1] ? nullptr : co[1]->string, ! co [2] ? nullptr : co[2]->string,
		! co [3] ? nullptr : co[3]->string, ! co [4] ? nullptr : co[4]->string,
		! co [5] ? nullptr : co[5]->string, ! co [6] ? nullptr : co[6]->string,
		! co [7] ? nullptr : co[7]->string, ! co [8] ? nullptr : co[8]->string,
		! co [9] ? nullptr : co[9]->string, ! co [10] ? nullptr : co[10]->string,
		theInterpreter);
	//Melder_casual (U"Button ", buttonClicked);
	pushNumber (buttonClicked);
}
static void do_chooseReadFileStr () {
	Stackel n = pop;
	if (n->number == 1) {
		Stackel title = pop;
		if (title->which == Stackel_STRING) {
			autoStringSet fileNames = GuiFileSelect_getInfileNames (nullptr, title->string, false);
			if (fileNames->size == 0) {
				autostring32 result = Melder_dup (U"");
				pushString (result.transfer());
			} else {
				SimpleString fileName = fileNames->at [1];
				autostring32 result = Melder_dup (fileName -> string);
				pushString (result.transfer());
			}
		} else {
			Melder_throw (U"The argument of \"chooseReadFile$\" must be a string (the title), not ", Stackel_whichText (title), U".");
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
			autostring32 result = GuiFileSelect_getOutfileName (nullptr, title->string, defaultName->string);
			if (! result.peek()) {
				result.reset (Melder_dup (U""));
			}
			pushString (result.transfer());
		} else {
			Melder_throw (U"The arguments of \"chooseWriteFile$\" must be two strings (the title and the default name).");
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
			autostring32 result = GuiFileSelect_getDirectoryName (nullptr, title->string);
			if (! result.peek()) {
				result.reset (Melder_dup (U""));
			}
			pushString (result.transfer());
		} else {
			Melder_throw (U"The argument of \"chooseDirectory$\" must be a string (the title).");
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
			Demo_windowTitle (title->string);
		} else {
			Melder_throw (U"The argument of \"demoWindowTitle\" must be a string (the title), not ", Stackel_whichText (title), U".");
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
			bool result = Demo_input (keys->string);
			pushNumber (result);
		} else {
			Melder_throw (U"The argument of \"demoInput\" must be a string (the keys), not ", Stackel_whichText (keys), U".");
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
			Melder_throw (U"All arguments of \"demoClickedIn\" must be numbers (the x and y ranges).");
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
	autostring32 key = Melder_malloc (char32, 2);
	key [0] = Demo_key ();
	key [1] = U'\0';
	pushString (key.transfer());
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
static long Stackel_getRowNumber (Stackel row, Daata thee) {
	long result = 0;
	if (row->which == Stackel_NUMBER) {
		result = lround (row->number);
	} else if (row->which == Stackel_STRING) {
		if (! thy v_hasGetRowIndex ())
			Melder_throw (U"Objects of type ", Thing_className (thee), U" do not have row labels, so row indexes have to be numbers.");
		result = lround (thy v_getRowIndex (row->string));
		if (result == 0)
			Melder_throw (U"Object \"", thy name, U"\" has no row labelled \"", row->string, U"\".");
	} else {
		Melder_throw (U"A row index should be a number or a string, not ", Stackel_whichText (row), U".");
	}
	return result;
}
static long Stackel_getColumnNumber (Stackel column, Daata thee) {
	long result = 0;
	if (column->which == Stackel_NUMBER) {
		result = lround (column->number);
	} else if (column->which == Stackel_STRING) {
		if (! thy v_hasGetColIndex ())
			Melder_throw (U"Objects of type ", Thing_className (thee), U" do not have column labels, so column indexes have to be numbers.");
		result = lround (thy v_getColIndex (column->string));
		if (result == 0)
			Melder_throw (U"Object ", thee, U" has no column labelled \"", column->string, U"\".");
	} else {
		Melder_throw (U"A column index should be a number or a string, not ", Stackel_whichText (column), U".");
	}
	return result;
}
static void do_self0 (long irow, long icol) {
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
static void do_selfStr0 (long irow, long icol) {
	Daata me = theSource;
	if (! me) Melder_throw (U"The name \"self$\" is restricted to formulas for objects.");
	if (my v_hasGetCellStr ()) {
		autostring32 result = Melder_dup (my v_getCellStr ());
		pushString (result.transfer());
	} else if (my v_hasGetVectorStr ()) {
		if (icol == 0) {
			Melder_throw (U"We are not in a loop, hence no implicit column index for the current ",
				Thing_className (me), U" object (self).\nTry using the [column] index explicitly.");
		} else {
			autostring32 result = Melder_dup (my v_getVectorStr (icol));
			pushString (result.transfer());
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
			autostring32 result = Melder_dup (my v_getMatrixStr (irow, icol));
			pushString (result.transfer());
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
		if (i == 0) {
			Melder_throw (U"No such object: ", object->number);
		}
		thee = (Daata) theCurrentPraatObjects -> list [i]. object;
	} else if (object->which == Stackel_STRING) {
		int i = theCurrentPraatObjects -> n;
		while (i > 0 && ! Melder_equ (object->string, theCurrentPraatObjects -> list [i]. name))
			i --;
		if (i == 0) {
			Melder_throw (U"No such object: ", object->string);
		}
		thee = (Daata) theCurrentPraatObjects -> list [i]. object;
	} else {
		Melder_throw (U"The first argument to \"object\" must be a number (unique ID) or a string (name), not ", Stackel_whichText (object), U".");
	}
	pushObject (thee);
}
static void do_objectCell0 (long irow, long icol) {
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
static void do_matriks0 (long irow, long icol) {
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
static void do_selfMatriks1 (long irow) {
	Daata me = theSource;
	Stackel column = pop;
	if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
	long icol = Stackel_getColumnNumber (column, me);
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
static void do_selfMatriksStr1 (long irow) {
	Daata me = theSource;
	Stackel column = pop;
	if (! me) Melder_throw (U"The name \"self$\" is restricted to formulas for objects.");
	long icol = Stackel_getColumnNumber (column, me);
	if (my v_hasGetVectorStr ()) {
		autostring32 result = Melder_dup (my v_getVectorStr (icol));
		pushString (result.transfer());
	} else if (my v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for the current ", Thing_className (me), U" object (self).\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			autostring32 result = Melder_dup (my v_getMatrixStr (irow, icol));
			pushString (result.transfer());
		}
	} else {
		Melder_throw (Thing_className (me), U" objects (like self) accept no [column] indexes.");
	}
}
static void do_objectCell1 (long irow) {
	Stackel column = pop, object = pop;
	Daata thee = object->object;
	long icol = Stackel_getColumnNumber (column, thee);
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
static void do_matriks1 (long irow) {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop;
	long icol = Stackel_getColumnNumber (column, thee);
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
static void do_objectCellStr1 (long irow) {
	Stackel column = pop, object = pop;
	Daata thee = object->object;
	long icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVectorStr ()) {
		autostring32 result = Melder_dup (thy v_getVectorStr (icol));
		pushString (result.transfer());
	} else if (thy v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using: object [id, row, column].");
		} else {
			autostring32 result = Melder_dup (thy v_getMatrixStr (irow, icol));
			pushString (result.transfer());
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes for string cells.");
	}
}
static void do_matrixStr1 (long irow) {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop;
	long icol = Stackel_getColumnNumber (column, thee);
	if (thy v_hasGetVectorStr ()) {
		autostring32 result = Melder_dup (thy v_getVectorStr (icol));
		pushString (result.transfer());
	} else if (thy v_hasGetMatrixStr ()) {
		if (irow == 0) {
			Melder_throw (U"We are not in a loop,\n"
				U"hence no implicit row index for this ", Thing_className (thee), U" object.\n"
				U"Try using both [row, column] indexes instead.");
		} else {
			autostring32 result = Melder_dup (thy v_getMatrixStr (irow, icol));
			pushString (result.transfer());
		}
	} else {
		Melder_throw (Thing_className (thee), U" objects accept no [column] indexes for string cells.");
	}
}
static void do_selfMatriks2 () {
	Daata me = theSource;
	Stackel column = pop, row = pop;
	if (! me) Melder_throw (U"The name \"self\" is restricted to formulas for objects.");
	long irow = Stackel_getRowNumber (row, me);
	long icol = Stackel_getColumnNumber (column, me);
	if (! my v_hasGetMatrix ())
		Melder_throw (Thing_className (me), U" objects like \"self\" accept no [row, column] indexing.");
	pushNumber (my v_getMatrix (irow, icol));
}
static void do_selfMatriksStr2 () {
	Daata me = theSource;
	Stackel column = pop, row = pop;
	if (! me) Melder_throw (U"The name \"self$\" is restricted to formulas for objects.");
	long irow = Stackel_getRowNumber (row, me);
	long icol = Stackel_getColumnNumber (column, me);
	if (! my v_hasGetMatrixStr ())
		Melder_throw (Thing_className (me), U" objects like \"self$\" accept no [row, column] indexing for string cells.");
	autostring32 result = Melder_dup (my v_getMatrixStr (irow, icol));
	pushString (result.transfer());
}
static void do_objectCell2 () {
	Stackel column = pop, row = pop, object = pop;
	Daata thee = object->object;
	long irow = Stackel_getRowNumber (row, thee);
	long icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrix ())
		Melder_throw (Thing_className (thee), U" objects accept no [id, row, column] indexing.");
	pushNumber (thy v_getMatrix (irow, icol));
}
static void do_matriks2 () {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	long irow = Stackel_getRowNumber (row, thee);
	long icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrix ())
		Melder_throw (Thing_className (thee), U" objects accept no [row, column] indexing.");
	pushNumber (thy v_getMatrix (irow, icol));
}
static void do_objectCellStr2 () {
	Stackel column = pop, row = pop, object = pop;
	Daata thee = object->object;
	long irow = Stackel_getRowNumber (row, thee);
	long icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrixStr ())
		Melder_throw (Thing_className (thee), U" objects accept no [id, row, column] indexing for string cells.");
	autostring32 result = Melder_dup (thy v_getMatrixStr (irow, icol));
	pushString (result.transfer());
}
static void do_matriksStr2 () {
	Daata thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	long irow = Stackel_getRowNumber (row, thee);
	long icol = Stackel_getColumnNumber (column, thee);
	if (! thy v_hasGetMatrixStr ())
		Melder_throw (Thing_className (thee), U" objects accept no [row, column] indexing for string cells.");
	autostring32 result = Melder_dup (thy v_getMatrixStr (irow, icol));
	pushString (result.transfer());
}
static void do_objectLocation0 (long irow, long icol) {
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
static void do_funktie0 (long irow, long icol) {
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
static void do_selfFunktie1 (long irow) {
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
static void do_objectLocation1 (long irow) {
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
static void do_funktie1 (long irow) {
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
static void do_selfFunktie2 () {
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
static void do_funktie2 () {
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
	long irow = Stackel_getRowNumber (row, thee);
	autostring32 result = Melder_dup (thy v_getRowStr (irow));
	if (! result.peek())
		Melder_throw (U"Row index out of bounds.");
	pushString (result.transfer());
}
static void do_colStr () {
	Daata thee = parse [programPointer]. content.object;
	Stackel col = pop;
	long icol = Stackel_getColumnNumber (col, thee);
	autostring32 result = Melder_dup (thy v_getColStr (icol));
	if (! result.peek())
		Melder_throw (U"Column index out of bounds.");
	pushString (result.transfer());
}

static double NUMarcsinh (double x) {
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

void Formula_run (long row, long col, Formula_Result *result) {
	FormulaInstruction f = parse;
	programPointer = 1;   // first symbol of the program
	if (! theStack) theStack = Melder_calloc_f (struct structStackel, 10000);
	if (! theStack)
		Melder_throw (U"Out of memory during formula computation.");
	w = 0, wmax = 0;   // start new stack
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
} break; case RECTIFY_NUMVEC_: { do_rectify_numvec ();
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
} break; case EXP_NUMVEC_: { do_exp_numvec ();
} break; case EXP_NUMMAT_: { do_exp_nummat ();
} break; case SINH_: { do_sinh ();
} break; case COSH_: { do_cosh ();
} break; case TANH_: { do_tanh ();
} break; case ARCSINH_: { do_function_n_n (NUMarcsinh);
} break; case ARCCOSH_: { do_function_n_n (NUMarccosh);
} break; case ARCTANH_: { do_function_n_n (NUMarctanh);
} break; case SIGMOID_: { do_function_n_n (NUMsigmoid);
} break; case SIGMOID_NUMVEC_: { do_functionvec_n_n (NUMsigmoid);
} break; case SOFTMAX_NUMVEC_: { do_softmax ();
} break; case INV_SIGMOID_: { do_function_n_n (NUMinvSigmoid);
} break; case ERF_: { do_function_n_n (NUMerf);
} break; case ERFC_: { do_function_n_n (NUMerfcc);
} break; case GAUSS_P_: { do_function_n_n (NUMgaussP);
} break; case GAUSS_Q_: { do_function_n_n (NUMgaussQ);
} break; case INV_GAUSS_Q_: { do_function_n_n (NUMinvGaussQ);
} break; case RANDOM_BERNOULLI_: { do_function_n_n (NUMrandomBernoulli_real);
} break; case RANDOM_BERNOULLI_NUMVEC_: { do_functionvec_n_n (NUMrandomBernoulli_real);
} break; case RANDOM_POISSON_: { do_function_n_n (NUMrandomPoisson);
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
} break; case ZERO_NUMVEC_: { do_zeroNumvec ();
} break; case ZERO_NUMMAT_: { do_zeroNummat ();
} break; case LINEAR_NUMVEC_: { do_linearNumvec ();
} break; case RANDOM_UNIFORM_NUMVEC_: { do_function_dd_d_numvec (NUMrandomUniform);
} break; case RANDOM_UNIFORM_NUMMAT_: { do_function_dd_d_nummat (NUMrandomUniform);
} break; case RANDOM_INTEGER_NUMVEC_: { do_function_ll_l_numvec (NUMrandomInteger);
} break; case RANDOM_INTEGER_NUMMAT_: { do_function_ll_l_nummat (NUMrandomInteger);
} break; case RANDOM_GAUSS_NUMVEC_: { do_function_dd_d_numvec (NUMrandomGauss);
} break; case RANDOM_GAUSS_NUMMAT_: { do_function_dd_d_nummat (NUMrandomGauss);
} break; case PEAKS_NUMMAT_: { do_peaksNummat ();
} break; case SIZE_: { do_size ();
} break; case NUMBER_OF_ROWS_: { do_numberOfRows ();
} break; case NUMBER_OF_COLUMNS_: { do_numberOfColumns ();
} break; case EDITOR_: { do_editor ();
} break; case HASH_: { do_hash ();
/********** String functions: **********/
} break; case LENGTH_: { do_length ();
} break; case STRING_TO_NUMBER_: { do_number ();
} break; case FILE_READABLE_: { do_fileReadable ();
} break; case DATESTR_: { do_dateStr ();
} break; case INFOSTR_: { do_infoStr ();
} break; case LEFTSTR_: { do_leftStr ();
} break; case RIGHTSTR_: { do_rightStr ();
} break; case MIDSTR_: { do_midStr ();
} break; case UNICODE_TO_BACKSLASH_TRIGRAPHS_: { do_unicodeToBackslashTrigraphsStr ();
} break; case BACKSLASH_TRIGRAPHS_TO_UNICODE_: { do_backslashTrigraphsToUnicodeStr ();
} break; case ENVIRONMENTSTR_: { do_environmentStr ();
} break; case INDEX_: { do_index ();
} break; case RINDEX_: { do_rindex ();
} break; case STARTS_WITH_: { do_stringMatchesCriterion (kMelder_string_STARTS_WITH);
} break; case ENDS_WITH_: { do_stringMatchesCriterion (kMelder_string_ENDS_WITH);
} break; case REPLACESTR_: { do_replaceStr ();
} break; case INDEX_REGEX_: { do_index_regex (false);
} break; case RINDEX_REGEX_: { do_index_regex (true);
} break; case REPLACE_REGEXSTR_: { do_replace_regexStr ();
} break; case EXTRACT_NUMBER_: { do_extractNumber ();
} break; case EXTRACT_WORDSTR_: { do_extractTextStr (true);
} break; case EXTRACT_LINESTR_: { do_extractTextStr (false);
} break; case SELECTED_: { do_selected ();
} break; case SELECTEDSTR_: { do_selectedStr ();
} break; case NUMBER_OF_SELECTED_: { do_numberOfSelected ();
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
} break; case FIXEDSTR_: { do_fixedStr ();
} break; case PERCENTSTR_: { do_percentStr ();
} break; case DELETE_FILE_: { do_deleteFile ();
} break; case CREATE_DIRECTORY_: { do_createDirectory ();
} break; case VARIABLE_EXISTS_: { do_variableExists ();
} break; case READ_FILE_: { do_readFile ();
} break; case READ_FILESTR_: { do_readFileStr ();
/********** Matrix functions: **********/
} break; case OUTER_NUMMAT_: { do_outerNummat ();
} break; case MUL_NUMVEC_: { do_mulNumvec ();
} break; case REPEAT_NUMVEC_: { do_repeatNumvec ();
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
		Melder_throw (U"A condition between \"if\" and \"then\" has to be a number, not ", Stackel_whichText (condition), U".");
	}
} break; case IFFALSE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->number == 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_throw (U"A condition between \"if\" and \"then\" has to be a number, not ", Stackel_whichText (condition), U".");
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
} break; case NUMERIC_VECTOR_ELEMENT_: { do_numericVectorElement ();
} break; case NUMERIC_MATRIX_ELEMENT_: { do_numericMatrixElement ();
} break; case INDEXED_NUMERIC_VARIABLE_: { do_indexedNumericVariable ();
} break; case INDEXED_STRING_VARIABLE_: { do_indexedStringVariable ();
} break; case VARIABLE_REFERENCE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushVariable (var);
} break; case SELF0_: { do_self0 (row, col);
} break; case SELFSTR0_: { do_selfStr0 (row, col);
} break; case OBJECT_: { pushObject (f [programPointer]. content.object);
} break; case TO_OBJECT_: { do_toObject ();
} break; case SELFMATRIKS1_: { do_selfMatriks1 (row);
} break; case SELFMATRIKSSTR1_: { do_selfMatriksStr1 (row);
} break; case SELFMATRIKS2_: { do_selfMatriks2 ();
} break; case SELFMATRIKSSTR2_: { do_selfMatriksStr2 ();
} break; case SELFFUNKTIE1_: { do_selfFunktie1 (row);
} break; case SELFFUNKTIE2_: { do_selfFunktie2 ();
} break; case OBJECTCELL0_: { do_objectCell0 (row, col);
} break; case OBJECTCELL1_: { do_objectCell1 (row);
} break; case OBJECTCELLSTR1_: { do_objectCellStr1 (row);
} break; case OBJECTCELL2_: { do_objectCell2 ();
} break; case OBJECTCELLSTR2_: { do_objectCellStr2 ();
} break; case OBJECTLOCATION0_: { do_objectLocation0 (row, col);
} break; case OBJECTLOCATION1_: { do_objectLocation1 (row);
} break; case OBJECTLOCATION2_: { do_objectLocation2 ();
} break; case MATRIKS0_: { do_matriks0 (row, col);
} break; case MATRIKS1_: { do_matriks1 (row);
} break; case MATRIKSSTR1_: { do_matrixStr1 (row);
} break; case MATRIKS2_: { do_matriks2 ();
} break; case MATRIKSSTR2_: { do_matriksStr2 ();
} break; case FUNKTIE0_: { do_funktie0 (row, col);
} break; case FUNKTIE1_: { do_funktie1 (row);
} break; case FUNKTIE2_: { do_funktie2 ();
} break; case ROWSTR_: { do_rowStr ();
} break; case COLSTR_: { do_colStr ();
} break; case SQR_: { do_sqr ();
} break; case STRING_: {
	autostring32 string = Melder_dup (f [programPointer]. content.string);
	pushString (string.transfer());
} break; case NUMERIC_VECTOR_LITERAL_: { do_numericVectorLiteral ();
} break; case NUMERIC_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumber (var -> numericValue);
} break; case NUMERIC_VECTOR_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	autonumvec vec = copy_numvec (var -> numericVectorValue);
	pushNumericVector (vec.move());
} break; case NUMERIC_MATRIX_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	autonummat mat = copy_nummat (var -> numericMatrixValue);
	pushNumericMatrix (mat.move());
} break; case STRING_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	autostring32 string = Melder_dup (var -> stringValue);
	pushString (string.transfer());
} break; default: Melder_throw (U"Symbol \"", Formula_instructionNames [parse [programPointer]. symbol], U"\" without action.");
			} // endswitch
			programPointer ++;
		} // endwhile
		if (w != 1) Melder_fatal (U"Formula: stackpointer ends at ", w, U" instead of 1.");
		if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC) {
			if (theStack [1]. which == Stackel_STRING) Melder_throw (U"Found a string expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR) Melder_throw (U"Found a vector expression instead of a numeric expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX) Melder_throw (U"Found a matrix expression instead of a numeric expression.");
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
			result -> result.numericResult = theStack [1]. number;
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_STRING) {
			if (theStack [1]. which == Stackel_NUMBER)
				Melder_throw (U"Found a numeric expression (value ", theStack [1]. number, U") instead of a string expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR) Melder_throw (U"Found a vector expression instead of a string expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX) Melder_throw (U"Found a matrix expression instead of a string expression.");
			result -> expressionType = kFormula_EXPRESSION_TYPE_STRING;
			result -> result.stringResult = theStack [1]. string;   // dangle...
			theStack [1]. string = nullptr;   // ...undangle (and disown)
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR) {
			if (theStack [1]. which == Stackel_NUMBER) Melder_throw (U"Found a numeric expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_STRING) Melder_throw (U"Found a string expression instead of a vector expression.");
			if (theStack [1]. which == Stackel_NUMERIC_MATRIX) Melder_throw (U"Found a matrix expression instead of a vector expression.");
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR;
			result -> result.numericVectorResult = theStack [1]. numericVector;   // dangle
			theStack [1]. numericVector = empty_numvec;   // ...undangle (and disown)
		} else if (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX) {
			if (theStack [1]. which == Stackel_NUMBER) Melder_throw (U"Found a numeric expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_STRING) Melder_throw (U"Found a string expression instead of a matrix expression.");
			if (theStack [1]. which == Stackel_NUMERIC_VECTOR) Melder_throw (U"Found a vector expression instead of a matrix expression.");
			result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX;
			result -> result.numericMatrixResult = theStack [1]. numericMatrix;   // dangle
			theStack [1]. numericMatrix = empty_nummat;   // ...undangle (and disown)
		} else {
			Melder_assert (theExpressionType [theLevel] == kFormula_EXPRESSION_TYPE_UNKNOWN);
			if (theStack [1]. which == Stackel_NUMBER) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC;
				result -> result.numericResult = theStack [1]. number;
			} else if (theStack [1]. which == Stackel_STRING) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_STRING;
				result -> result.stringResult = theStack [1]. string;   // dangle...
				theStack [1]. string = nullptr;   // ...undangle (and disown)
			} else if (theStack [1]. which == Stackel_NUMERIC_VECTOR) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_VECTOR;
				result -> result.numericVectorResult = theStack [1]. numericVector;   // dangle...
				theStack [1]. numericVector = empty_numvec;   // ...undangle (and disown)
			} else if (theStack [1]. which == Stackel_NUMERIC_MATRIX) {
				result -> expressionType = kFormula_EXPRESSION_TYPE_NUMERIC_MATRIX;
				result -> result.numericMatrixResult = theStack [1]. numericMatrix;   // dangle...
				theStack [1]. numericMatrix = empty_nummat;   // ...undangle (and disown)
			} else {
				Melder_throw (U"Don't know yet how to write ", Stackel_whichText (& theStack [1]), U".");
			}
		}
		/*
			Clean up the stack (theStack [1] has probably been disowned).
		*/
		for (w = wmax; w > 0; w --) {
			Stackel stackel = & theStack [w];
			if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
		}
	} catch (MelderError) {
		/*
			Clean up the stack (theStack [1] has probably not been disowned).
		*/
		for (w = wmax; w > 0; w --) {
			Stackel stackel = & theStack [w];
			if (stackel -> which > Stackel_NUMBER) Stackel_cleanUp (stackel);
		}
		if (Melder_hasError (U"Script exited.")) {
			throw;
		} else {
			Melder_throw (U"Formula not run.");
		}
	}
}

/* End of file Formula.cpp */
