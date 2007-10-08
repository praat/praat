/* Formula.c
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
 * pb 2002/10/02 getenv -> Melder_getenv
 * pb 2002/11/24 Melder_double
 * pb 2002/11/30 extractWord, extractLine
 * pb 2002/12/01 expressionType unknown
 * pb 2002/12/14 nicer error messages
 * pb 2003/03/09 allowed theInterpreter and theSource to both exist or not exist
 * pb 2003/05/19 Melder_atof, percent
 * pb 2003/06/23 removed Bessel functions J and Y
 * pb 2003/07/26 min and max
 * pb 2004/10/16 C++ compatible struct tags
 * pb 2005/05/15 messages
 * pb 2005/06/14 startsWith, endsWith, index_regex
 * pb 2006/01/11 local variables
 * pb 2006/01/21 allow things like Object_137 [row, col]
 * pb 2006/01/29 run-time type checking
 * pb 2006/01/30 lexical analysis: binary rather than linear search for language names
 * pb 2006/04/17 .row$, .col$
 * pb 2006/05/30 replace_regex$
 * pb 2006/06/10 prevented replace_regex$ from returning null string (now empty string)
 * pb 2006/12/18 better info
 * pb 2006/12/26 theCurrentPraat
 * pb 2007/01/28 made compatible with multi-channelled vectors
 * pb 2007/05/26 wchar_t
 * pb 2007/06/09 wchar_t for Interpreter
 */

#include <ctype.h>
#include <time.h>
#include "NUM.h"
#include "NUM2.h"
#include "regularExp.h"
#include "Formula.h"
#include "Interpreter.h"
#include "Ui.h"
#include "praatP.h"
#include "UnicodeData.h"

static Interpreter theInterpreter;
static Data theSource;
static const wchar_t *theExpression;
static int theExpressionType, theOptimize;
#define EXPRESSION_TYPE_NUMERIC  0
#define EXPRESSION_TYPE_STRING  1
#define EXPRESSION_TYPE_UNKNOWN  2

typedef struct FormulaInstruction {
	int symbol;
	int position;
	union {
		double number;
		int label;
		wchar_t *string;
		Any object;
		InterpreterVariable variable;
	} content;
} *FormulaInstruction;

static FormulaInstruction lexan, parse;
static int ilabel, ilexan, iparse, numberOfInstructions, numberOfStringConstants;

enum { GEENSYMBOOL_,

/* First, all symbols after which "-" is unary. */
/* The list ends with "MINUS_" itself. */

	/* Haakjes-openen. */
	IF_, THEN_, ELSE_, HAAKJEOPENEN_, RECHTEHAAKOPENEN_, KOMMA_,
	/* Operatoren met boolean resultaat. */
	OR_, AND_, NOT_, EQ_, NE_, LE_, LT_, GE_, GT_,
	/* Operatoren met reeel resultaat. */
	ADD_, SUB_, MUL_, RDIV_, IDIV_, MOD_, POWER_, MINUS_,

/* Then, the symbols after which "-" is binary. */

	/* Haakjes-sluiten. */
	ENDIF_, FI_, HAAKJESLUITEN_, RECHTEHAAKSLUITEN_,
	/* Dingen met een waarde. */
	#define LOW_VALUE  NUMBER_
		NUMBER_, NUMBER_PI_, NUMBER_E_, NUMBER_UNDEFINED_,
		/* Attributes of objects. */
		#define LOW_ATTRIBUTE  XMIN_
			XMIN_, XMAX_, YMIN_, YMAX_, NX_, NY_, DX_, DY_,
			ROW_, COL_, NROW_, NCOL_, ROWSTR_, COLSTR_, Y_, X_,
		#define HIGH_ATTRIBUTE  X_
	#define HIGH_VALUE  HIGH_ATTRIBUTE

	SELF_, SELFSTR_, MATRIKS_, MATRIKSSTR_,
	STOPWATCH_,

/* The following symbols can be followed by "-" only if they are a variable. */

	/* Functions of 1 variable; if you add, update the #defines. */
	#define LOW_FUNCTION_1  ABS_
		ABS_, ROUND_, FLOOR_, CEILING_, SQRT_, SIN_, COS_, TAN_, ARCSIN_, ARCCOS_, ARCTAN_,
		EXP_, SINH_, COSH_, TANH_, ARCSINH_, ARCCOSH_, ARCTANH_,
		SIGMOID_, INV_SIGMOID_, ERF_, ERFC_, GAUSS_P_, GAUSS_Q_, INV_GAUSS_Q_,
		RANDOM_POISSON_, LOG2_, LN_, LOG10_, LN_GAMMA_,
		HERTZ_TO_BARK_, BARK_TO_HERTZ_, PHON_TO_DIFFERENCE_LIMENS_, DIFFERENCE_LIMENS_TO_PHON_,
		HERTZ_TO_MEL_, MEL_TO_HERTZ_, HERTZ_TO_SEMITONES_, SEMITONES_TO_HERTZ_,
		ERB_, HERTZ_TO_ERB_, ERB_TO_HERTZ_,
	#define HIGH_FUNCTION_1  ERB_TO_HERTZ_

	/* Functions of 2 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_2  ARCTAN2_
		ARCTAN2_, RANDOM_UNIFORM_, RANDOM_INTEGER_, RANDOM_GAUSS_,
		CHI_SQUARE_P_, CHI_SQUARE_Q_, INV_CHI_SQUARE_Q_, STUDENT_P_, STUDENT_Q_, INV_STUDENT_Q_,
		BETA_, BESSEL_I_, BESSEL_K_,
		SOUND_PRESSURE_TO_PHON_, OBJECTS_ARE_IDENTICAL_,
	#define HIGH_FUNCTION_2  OBJECTS_ARE_IDENTICAL_

	/* Functions of 3 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_3  FISHER_P_
		FISHER_P_, FISHER_Q_, INV_FISHER_Q_,
		BINOMIAL_P_, BINOMIAL_Q_, INV_BINOMIAL_P_, INV_BINOMIAL_Q_,
	#define HIGH_FUNCTION_3  INV_BINOMIAL_Q_

	/* Functions of a variable number of variables; if you add, update the #defines. */
	#define LOW_FUNCTION_N  MIN_
		MIN_, MAX_, IMIN_, IMAX_,
		LEFTSTR_, RIGHTSTR_, MIDSTR_,
		SELECTED_, SELECTEDSTR_, NUMBER_OF_SELECTED_,
	#define HIGH_FUNCTION_N  NUMBER_OF_SELECTED_

	/* String functions. */
	#define LOW_STRING_FUNCTION  LOW_FUNCTION_STRNUM
	#define LOW_FUNCTION_STRNUM  LENGTH_
		LENGTH_, FILE_READABLE_,
	#define HIGH_FUNCTION_STRNUM  FILE_READABLE_
		DATESTR_,
		ENVIRONMENTSTR_, INDEX_, RINDEX_,
		STARTS_WITH_, ENDS_WITH_, REPLACESTR_, INDEX_REGEX_, RINDEX_REGEX_, REPLACE_REGEXSTR_,
		EXTRACT_NUMBER_, EXTRACT_WORDSTR_, EXTRACT_LINESTR_,
		FIXEDSTR_, PERCENTSTR_,
	#define HIGH_STRING_FUNCTION  PERCENTSTR_

	#define LOW_FUNCTION  LOW_FUNCTION_1
	#define HIGH_FUNCTION  HIGH_STRING_FUNCTION

	/* Membership operator. */
	PERIOD_,
	#define hoogsteInvoersymbool  PERIOD_

/* Symbols introduced by the parser. */

	TRUE_, FALSE_, IFTRUE_, IFFALSE_, GOTO_, LABEL_,
	SELF0_, SELFSTR0_, SELFMATRIKS1_, SELFMATRIKSSTR1_, SELFMATRIKS2_, SELFMATRIKSSTR2_,
	SELFFUNKTIE1_, SELFFUNKTIESTR1_, SELFFUNKTIE2_, SELFFUNKTIESTR2_,
	MATRIKS0_, MATRIKSSTR0_, MATRIKS1_, MATRIKSSTR1_, MATRIKS2_, MATRIKSSTR2_,
	FUNKTIE0_, FUNKTIESTR0_, FUNKTIE1_, FUNKTIESTR1_, FUNKTIE2_, FUNKTIESTR2_,
	SQR_,

/* Symbols introduced by lexical analysis. */

	STRING_,
	NUMERIC_VARIABLE_, STRING_VARIABLE_,
	END_
	#define hoogsteSymbool END_
};

/* The names that start with an underscore (_) do not occur in the formula text: */
/* they are used in error messages and in debugging (see Formula_print). */

static wchar_t *Formula_instructionNames [1 + hoogsteSymbool] = { L"",
	L"if", L"then", L"else", L"(", L"[", L",",
	L"or", L"and", L"not", L"=", L"<>", L"<=", L"<", L">=", L">",
	L"+", L"-", L"*", L"/", L"div", L"mod", L"^", L"_neg",
	L"endif", L"fi", L")", L"]",
	L"_number", L"pi", L"e", L"undefined",
	L"xmin", L"xmax", L"ymin", L"ymax", L"nx", L"ny", L"dx", L"dy",
	L"row", L"col", L"nrow", L"ncol", L"row$", L"col$", L"y", L"x",
	L"self", L"self$", L"_matriks", L"_matriks$",
	L"stopwatch",
	L"abs", L"round", L"floor", L"ceiling", L"sqrt", L"sin", L"cos", L"tan", L"arcsin", L"arccos", L"arctan",
	L"exp", L"sinh", L"cosh", L"tanh", L"arcsinh", L"arccosh", L"arctanh",
	L"sigmoid", L"invSigmoid", L"erf", L"erfc", L"gaussP", L"gaussQ", L"invGaussQ",
	L"randomPoisson", L"log2", L"ln", L"log10", L"lnGamma",
	L"hertzToBark", L"barkToHertz", L"phonToDifferenceLimens", L"differenceLimensToPhon",
	L"hertzToMel", L"melToHertz", L"hertzToSemitones", L"semitonesToHertz",
	L"erb", L"hertzToErb", L"erbToHertz",
	L"arctan2", L"randomUniform", L"randomInteger", L"randomGauss",
	L"chiSquareP", L"chiSquareQ", L"invChiSquareQ", L"studentP", L"studentQ", L"invStudentQ",
	L"beta", L"besselI", L"besselK",
	L"soundPressureToPhon", L"objectsAreIdentical",
	L"fisherP", L"fisherQ", L"invFisherQ",
	L"binomialP", L"binomialQ", L"invBinomialP", L"invBinomialQ",

	L"min", L"max", L"imin", L"imax",
	L"left$", L"right$", L"mid$",
	L"selected", L"selected$", L"numberOfSelected",

	L"length", L"fileReadable",
	L"date$",
	L"environment$", L"index", L"rindex",
	L"startsWith", L"endsWith", L"replace$", L"index_regex", L"rindex_regex", L"replace_regex$",
	L"extractNumber", L"extractWord$", L"extractLine$",
	L"fixed$", L"percent$",
	L".",
	L"_true", L"_false", L"_iftrue", L"_iffalse", L"_gaNaar", L"_label",
	L"_self0", L"_self0$", L"_selfmatriks1", L"_selfmatriks1$", L"_selfmatriks2", L"_selfmatriks2$",
	L"_selffunktie1", L"_selffunktie1$", L"_selffunktie2", L"_selffunktie2$",
	L"_matriks0", L"_matriks0$", L"_matriks1", L"_matriks1$", L"_matriks2", L"_matriks2$",
	L"_funktie0", L"_funktie0$", L"_funktie1", L"_funktie1$", L"_funktie2", L"_funktie2$",
	L"_square",
	L"_string",
	L"_numericVariable", L"_stringVariable",
	L"_end"
};

#define nieuwlabel (-- ilabel)
#define nieuwlees (lexan [++ ilexan]. symbol)
#define oudlees  (-- ilexan)

static int formulefout (wchar_t *message, int position) {
	static MelderString truncatedExpression = { 0 };
	MelderString_ncopy (& truncatedExpression, theExpression, position + 1);
	return Melder_error3 (message, L":\n" L_LEFT_GUILLEMET L" ", truncatedExpression.string);
}

static const wchar_t *languageNameCompare_searchString;

static int languageNameCompare (const void *first, const void *second) {
	int i = * (int *) first, j = * (int *) second;
	return wcscmp (i == 0 ? languageNameCompare_searchString : Formula_instructionNames [i],
		j == 0 ? languageNameCompare_searchString : Formula_instructionNames [j]);
}

static int Formula_hasLanguageName (const wchar_t *f) {
	static int *index;
	int tok;
	if (index == NULL) {
		index = NUMivector (1, hoogsteInvoersymbool);
		for (tok = 1; tok <= hoogsteInvoersymbool; tok ++) {
			index [tok] = tok;
		}
		qsort (& index [1], hoogsteInvoersymbool, sizeof (int), languageNameCompare);
	}
	if (index == NULL) {   /* Linear search. */
		for (tok = 1; tok <= hoogsteInvoersymbool; tok ++) {
			if (wcsequ (f, Formula_instructionNames [tok])) return tok;
		}
	} else {   /* Binary search. */
		int dummy = 0, *found;
		languageNameCompare_searchString = f;
		found = bsearch (& dummy, & index [1], hoogsteInvoersymbool, sizeof (int), languageNameCompare);
		if (found != NULL) return *found;
	}
	return 0;
}

static int Formula_lexan (void) {
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
	int kar;   /* The character most recently read from theExpression. */
	int ikar = -1;   /* The position of that character in theExpression. */
#define nieuwkar kar = theExpression [++ ikar]
#define oudkar -- ikar

	int itok = 0;   /* Position of most recent symbol in "lexan". */
#define nieuwtok(s)  { lexan [++ itok]. symbol = s; lexan [itok]. position = ikar; }
#define tokgetal(g)  lexan [itok]. content.number = (g)
#define tokmatriks(m)  lexan [itok]. content.object = (m)

	static MelderString token = { 0 };   /* String to collect a symbol name in. */
#define stokaan MelderString_empty (& token);
#define stokkar { MelderString_appendCharacter (& token, kar); nieuwkar; }
#define stokuit (void) 0

	ilexan = iparse = ilabel = numberOfStringConstants = 0;
	do {
		nieuwkar;
		if (kar == ' ' || kar == '\t') {
			;   /* Ignore spaces and tabs. */
		} else if (kar == '\0') {
			nieuwtok (END_)
		} else if (kar >= '0' && kar <= '9') {
			stokaan;
			do stokkar while (kar >= '0' && kar <= '9');
			if (kar == '.') do stokkar while (kar >= '0' && kar <= '9');
			if (kar == 'e' || kar == 'E') {
				kar = 'e'; stokkar
				if (kar == '-') stokkar
				else if (kar == '+') nieuwkar;
				if (! (kar >= '0' && kar <= '9')) {
					formulefout (L"Missing exponent", ikar);
					return 0;
				}
				do stokkar while (kar >= '0' && kar <= '9');
			}
			if (kar == '%') stokkar
			stokuit;
			oudkar;
			nieuwtok (NUMBER_)
			tokgetal (Melder_atofW (token.string));
		} else if ((kar >= 'a' && kar <= 'z') || (kar == '.' && theExpression [ikar + 1] >= 'a' && theExpression [ikar + 1] <= 'z'
				&& (itok == 0 || (lexan [itok]. symbol != MATRIKS_ && lexan [itok]. symbol != MATRIKSSTR_)))) {
			int tok, endsInDollarSign = FALSE;
			stokaan;
			do stokkar while ((kar >= 'A' && kar <= 'Z') || (kar >= 'a' && kar <= 'z') || (kar >= '0' && kar <= '9') || kar == '_' || kar == '.');
			if (kar == '$') { stokkar endsInDollarSign = TRUE; }
			stokuit;
			oudkar;
			/*
			 * 'token' now contains a word, possibly ending in a dollar sign;
			 * it could be a variable name or a function name, or both!
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
					tokgetal (NUMundefined);
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
					 * Look ahead to find out whether the next token is a left parenthesis.
					 */
					int jkar;
					jkar = ikar + 1;
					while (theExpression [jkar] == ' ' || theExpression [jkar] == '\t') jkar ++;
					if (theExpression [jkar] == '(') {
						nieuwtok (tok)   /* This must be a function name. */
					} else if (theInterpreter) {
						/*
						 * This could be a variable with the same name as a function.
						 */
						InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
						if (var == NULL) return formulefout (L"Unknown variable, or function with missing arguments", ikar);
						if (endsInDollarSign) nieuwtok (STRING_VARIABLE_) else nieuwtok (NUMERIC_VARIABLE_)
						lexan [itok]. content.variable = var;
					} else {
						return formulefout (L"Function with missing arguments", ikar);
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
						if (theInterpreter && Interpreter_hasVariable (theInterpreter, token.string))
							return Melder_error3 (
								L_LEFT_GUILLEMET, token.string,
								L_RIGHT_GUILLEMET L" is ambiguous: a variable or an attribute of the current object. "
								L"Please change variable name.");
						if (tok == ROW_ || tok == COL_ || tok == X_ || tok == Y_) {
							nieuwtok (tok)
						} else {
							nieuwtok (MATRIKS_)
							tokmatriks (theSource);
							nieuwtok (PERIOD_)
							nieuwtok (tok)
						}
					} else if (theInterpreter) {
						/*
						 * This must be a variable, since there is no "current object" here.
						 */
						InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
						if (var == NULL) return Melder_error3 (
							L"Unknown variable " L_LEFT_GUILLEMET, token.string,
							L_RIGHT_GUILLEMET L" in formula (no \"current object\" here).");
						if (endsInDollarSign) nieuwtok (STRING_VARIABLE_) else nieuwtok (NUMERIC_VARIABLE_)
						lexan [itok]. content.variable = var;
					} else {
						return Melder_error3 (
							L"Unknown token " L_LEFT_GUILLEMET, token.string,
							L_RIGHT_GUILLEMET L" in formula (no variables or current objects here).");
					}
				} else {
					nieuwtok (tok)   /* This must be a language name. */
				}
			} else if (theInterpreter) {
				InterpreterVariable var = Interpreter_hasVariable (theInterpreter, token.string);
				if (var == NULL) {
					int jkar;
					jkar = ikar + 1;
					while (theExpression [jkar] == ' ' || theExpression [jkar] == '\t') jkar ++;
					if (theExpression [jkar] == '(') {
						return Melder_error3 (
							L"Unknown function " L_LEFT_GUILLEMET, token.string,
							L_RIGHT_GUILLEMET L" in formula.");
					} else {
						return Melder_error3 (
							L"Unknown variable " L_LEFT_GUILLEMET, token.string,
							L_RIGHT_GUILLEMET L" in formula.");
					}
				}
				if (endsInDollarSign) nieuwtok (STRING_VARIABLE_) else nieuwtok (NUMERIC_VARIABLE_)
				lexan [itok]. content.variable = var;
			} else {
				return Melder_error3 (
					L"Unknown function or attribute " L_LEFT_GUILLEMET, token.string,
						L_RIGHT_GUILLEMET L" in formula.");
			}
		} else if (kar >= 'A' && kar <= 'Z') {
			int endsInDollarSign = FALSE;
			wchar_t *underscore;
			stokaan;
			do stokkar while (isalnum (kar) || kar == '_');
			if (kar == '$') { stokkar endsInDollarSign = TRUE; }
			stokuit;
			oudkar;
			/*
			 * 'token' now contains a word that could be an object name.
			 */
			underscore = wcschr (token.string, '_');
			if (wcsequ (token.string, L"Self")) {
				if (theSource == NULL) {
					formulefout (L"Cannot use \"Self\" if there is no current object.", ikar);
					return 0;
				}
				nieuwtok (MATRIKS_)
				tokmatriks (theSource);
			} else if (wcsequ (token.string, L"Self$")) {
				if (theSource == NULL) {
					formulefout (L"Cannot use \"Self$\" if there is no current object.", ikar);
					return 0;
				}
				nieuwtok (MATRIKSSTR_)
				tokmatriks (theSource);
			} else if (underscore == NULL) {
				return Melder_error3 (
					L"Unknown symbol " L_LEFT_GUILLEMET , token.string,
					L_RIGHT_GUILLEMET L" in formula "
					L"(variables start with lower case; object names contain an underscore).");
			} else if (wcsnequ (token.string, L"Object_", 7)) {
				long uniqueID = wcstol (token.string + 7, NULL, 10);
				int i = theCurrentPraat -> n;
				while (i > 0 && uniqueID != theCurrentPraat -> list [i]. id)
					i --;
				if (i == 0) {
					formulefout (L"No such object (note: variables start with lower case)", ikar);
					return 0;
				}
				nieuwtok (endsInDollarSign ? MATRIKSSTR_ : MATRIKS_)
				tokmatriks (theCurrentPraat -> list [i]. object);
			} else {
				int i = theCurrentPraat -> n;
				*underscore = ' ';
				if (endsInDollarSign) token.string [-- token.length] = '\0';
				while (i > 0 && ! wcsequ (token.string, theCurrentPraat -> list [i]. name))
					i --;
				if (i == 0) {
					formulefout (L"No such object (note: variables start with lower case)", ikar);
					return 0;
				}
				nieuwtok (endsInDollarSign ? MATRIKSSTR_ : MATRIKS_)
				tokmatriks (theCurrentPraat -> list [i]. object);
			}
		} else if (kar == '(') {
			nieuwtok (HAAKJEOPENEN_)
		} else if (kar == ')') {
			nieuwtok (HAAKJESLUITEN_)
		} else if (kar == '+') {
			nieuwtok (ADD_)
		} else if (kar == '-') {
			if (itok == 0 || lexan [itok]. symbol <= MINUS_) {
				nieuwtok (MINUS_)
			} else {
				nieuwtok (SUB_)
			}
		} else if (kar == '*') {
			nieuwkar;
			if (kar == '*') {
				nieuwtok (POWER_)   /* "**" = "^" */
			} else {
				oudkar;
				nieuwtok (MUL_)
			}
		} else if (kar == '/') {
			nieuwkar;
			if (kar == '=') {
				nieuwtok (NE_)   /* "/=" = "<>" */
			} else {
				oudkar;
				nieuwtok (RDIV_)
			}
		} else if (kar == '=') {
			nieuwtok (EQ_)   /* "=" */
			nieuwkar;
			if (kar != '=') {
				oudkar;   /* "==" = "=" */
			}
		} else if (kar == '>') {
			nieuwkar;
			if (kar == '=') {
				nieuwtok (GE_)
			} else {
				oudkar;
				nieuwtok (GT_)
			}
		} else if (kar == '<') {
			nieuwkar;
			if (kar == '=') {
				nieuwtok (LE_)
			} else if (kar == '>') {
				nieuwtok (NE_)
			} else {
				oudkar;
				nieuwtok (LT_)
			}
		} else if (kar == '!') {
			nieuwkar;
			if (kar == '=') {
				nieuwtok (NE_)   /* "!=" = "<>" */
			} else {
				oudkar;
				nieuwtok (NOT_)
			}
		} else if (kar == ',') {
			nieuwtok (KOMMA_)
		} else if (kar == ';') {
			nieuwtok (END_)
		} else if (kar == '^') {
			nieuwtok (POWER_)
		} else if (kar == '\"') {
			/*
			 * String constant.
			 */
			nieuwkar;
			stokaan;
			for (;;) {
				if (kar == '\0') {
					formulefout (L"No closing quote in string constant", ikar);
					return 0;
				}
				if (kar == '\"') {
					nieuwkar;
					if (kar == '\"') stokkar
					else break;
				} else {
					stokkar
				}
			}
			stokuit;
			oudkar;
			nieuwtok (STRING_)
			lexan [itok]. content.string = Melder_wcsdup (token.string);
			numberOfStringConstants ++;
		} else if (kar == '|') {
			nieuwtok (OR_)   /* "|" = "or" */
			nieuwkar;
			if (kar != '|') {
				oudkar;   /* "||" = "or" */
			}
		} else if (kar == '&') {
			nieuwtok (AND_)   /* "&" = "and" */
			nieuwkar;
			if (kar != '&') {
				oudkar;   /* "&&" = "and" */
			}
		} else if (kar == '[') {
			nieuwtok (RECHTEHAAKOPENEN_)
		} else if (kar == ']') {
			nieuwtok (RECHTEHAAKSLUITEN_)
		} else if (kar == '.') {
			nieuwtok (PERIOD_)
		} else {
			formulefout (L"Unknown symbol", ikar);
			return 0;
		}
	} while (lexan [itok]. symbol != END_);
	return 1;
}

static int pas (int symbol) {
	if (symbol == nieuwlees) {
		return 1;
	} else {
		static MelderString melding = { 0 };
		MelderString_empty (& melding);
		MelderString_append5 (& melding, L"Expected \"", Formula_instructionNames [symbol], L"\", but found \"",
			Formula_instructionNames [lexan [ilexan]. symbol], L"\"");
		return formulefout (melding.string, lexan [ilexan]. position);
	}
}

#define nieuwontleed(s)  parse [++ iparse]. symbol = (s)
#define parsenumber(g)  parse [iparse]. content.number = (g)
#define ontleedlabel(l)  parse [iparse]. content.label = (l)

static int parseExpression (void);

static int parsePowerFactor (void) {
	int symbol = nieuwlees;

	if (symbol >= LOW_VALUE && symbol <= HIGH_VALUE) {
		nieuwontleed (symbol);
		if (symbol == NUMBER_) parsenumber (lexan [ilexan]. content.number);
		return 1;
	}

	if (symbol == STRING_) {
		nieuwontleed (symbol);
		parse [iparse]. content.string = lexan [ilexan]. content.string;   /* Reference copy! */
		return 1;
	}

	if (symbol == NUMERIC_VARIABLE_ || symbol == STRING_VARIABLE_) {
		nieuwontleed (symbol);
		parse [iparse]. content.variable = lexan [ilexan]. content.variable;
		return 1;
	}

	if (symbol == SELF_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (! parseExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				nieuwontleed (SELFMATRIKS2_);
				return pas (RECHTEHAAKSLUITEN_);
			}
			oudlees;
			nieuwontleed (SELFMATRIKS1_);
			return pas (RECHTEHAAKSLUITEN_);
		}
		if (symbol == HAAKJEOPENEN_) {
			if (! parseExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				nieuwontleed (SELFFUNKTIE2_);
				return pas (HAAKJESLUITEN_);
			}
			oudlees;
			nieuwontleed (SELFFUNKTIE1_);
			return pas (HAAKJESLUITEN_);
		}
		oudlees;
		nieuwontleed (SELF0_);
		return 1;
	}

	if (symbol == SELFSTR_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (! parseExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				nieuwontleed (SELFMATRIKSSTR2_);
				return pas (RECHTEHAAKSLUITEN_);
			}
			oudlees;
			nieuwontleed (SELFMATRIKSSTR1_);
			return pas (RECHTEHAAKSLUITEN_);
		}
		if (symbol == HAAKJEOPENEN_) {
			if (! parseExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				nieuwontleed (SELFFUNKTIESTR2_);
				return pas (HAAKJESLUITEN_);
			}
			oudlees;
			nieuwontleed (SELFFUNKTIESTR1_);
			return pas (HAAKJESLUITEN_);
		}
		oudlees;
		nieuwontleed (SELFSTR0_);
		return 1;
	}

	if (symbol == HAAKJEOPENEN_) {
		if (! parseExpression ()) return 0;
		return pas (HAAKJESLUITEN_);
	}

	if (symbol == IF_) {
		int elseLabel = nieuwlabel;   /* Moet lokaal, */
		int endifLabel = nieuwlabel;   /* vanwege rekursie. */
		if (! parseExpression ()) return 0;
		nieuwontleed (IFFALSE_);  ontleedlabel (elseLabel);
		if (! pas (THEN_)) return 0;
		if (! parseExpression ()) return 0;
		nieuwontleed (GOTO_);     ontleedlabel (endifLabel);
		if (! pas (ELSE_)) return 0;
		nieuwontleed (LABEL_);    ontleedlabel (elseLabel);
		if (! parseExpression ()) return 0;
		if (! pas (ENDIF_)) return 0;
		nieuwontleed (LABEL_);    ontleedlabel (endifLabel);
		return 1;
	}

	if (symbol == MATRIKS_) {
		Data thee = lexan [ilexan]. content.object;
		Melder_assert (thee != NULL);
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (nieuwlees == RECHTEHAAKSLUITEN_) {
				nieuwontleed (MATRIKS0_);
				parse [iparse]. content.object = thee;
			} else {
				oudlees;
				if (! parseExpression ()) return 0;
				if (nieuwlees == KOMMA_) {
					if (! parseExpression ()) return 0;
					nieuwontleed (MATRIKS2_);
					parse [iparse]. content.object = thee;
					if (! pas (RECHTEHAAKSLUITEN_)) return 0;
				} else {
					oudlees;
					nieuwontleed (MATRIKS1_);
					parse [iparse]. content.object = thee;
					if (! pas (RECHTEHAAKSLUITEN_)) return 0;
				}
			}
		} else if (symbol == HAAKJEOPENEN_) {
			if (nieuwlees == HAAKJESLUITEN_) {
				nieuwontleed (FUNKTIE0_);
				parse [iparse]. content.object = thee;
			} else {
				oudlees;
				if (! parseExpression ()) return 0;
				if (nieuwlees == KOMMA_) {
					if (! parseExpression ()) return 0;
					nieuwontleed (FUNKTIE2_);
					parse [iparse]. content.object = thee;
					if (! pas (HAAKJESLUITEN_)) return 0;
				} else {
					oudlees;
					nieuwontleed (FUNKTIE1_);
					parse [iparse]. content.object = thee;
					if (! pas (HAAKJESLUITEN_)) return 0;
				}
			}
		} else if (symbol == PERIOD_) {
			switch (nieuwlees) {
				case XMIN_:
					if (your getXmin == NULL) {
						formulefout (L"Attribute \"xmin\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getXmin (thee));
						return 1;
					}
				case XMAX_:
					if (your getXmax == NULL) {
						formulefout (L"Attribute \"xmax\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getXmax (thee));
						return 1;
					}
				case YMIN_:
					if (your getYmin == NULL) {
						formulefout (L"Attribute \"ymin\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getYmin (thee));
						return 1;
					}
				case YMAX_:
					if (your getYmax == NULL) {
						formulefout (L"Attribute \"ymax\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getYmax (thee));
						return 1;
					}
				case NX_:
					if (your getNx == NULL) {
						formulefout (L"Attribute \"nx\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getNx (thee));
						return 1;
					}
				case NY_:
					if (your getNy == NULL) {
						formulefout (L"Attribute \"ny\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getNy (thee));
						return 1;
					}
				case DX_:
					if (your getDx == NULL) {
						formulefout (L"Attribute \"dx\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getDx (thee));
						return 1;
					}
				case DY_:
					if (your getDy == NULL) {
						formulefout (L"Attribute \"dy\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getDy (thee));
						return 1;
					}
				case NCOL_:
					if (your getNcol == NULL) {
						formulefout (L"Attribute \"ncol\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getNcol (thee));
						return 1;
					}
				case NROW_:
					if (your getNrow == NULL) {
						formulefout (L"Attribute \"nrow\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						nieuwontleed (NUMBER_);
						parsenumber (your getNrow (thee));
						return 1;
					}
				case ROWSTR_:
					if (your getRowStr == NULL) {
						formulefout (L"Attribute \"row$\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						if (! pas (RECHTEHAAKOPENEN_)) return 0;
						if (! parseExpression ()) return 0;
						nieuwontleed (ROWSTR_);
						parse [iparse]. content.object = thee;
						if (! pas (RECHTEHAAKSLUITEN_)) return 0;
						return 1;
					}
				case COLSTR_:
					if (your getColStr == NULL) {
						formulefout (L"Attribute \"col$\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						if (! pas (RECHTEHAAKOPENEN_)) return 0;
						if (! parseExpression ()) return 0;
						nieuwontleed (COLSTR_);
						parse [iparse]. content.object = thee;
						if (! pas (RECHTEHAAKSLUITEN_)) return 0;
						return 1;
					}
				default: formulefout (L"Unknown attribute.", lexan [ilexan]. position); return 0;
			}
		} else {
			formulefout (L"After a name of a matrix there should be \"(\", \"[\", or \".\"", lexan [ilexan]. position);
			return 0;
		}
		return 1;
	}

	if (symbol == MATRIKSSTR_) {
		Data thee = lexan [ilexan]. content.object;
		Melder_assert (thee != NULL);
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (nieuwlees == RECHTEHAAKSLUITEN_) {
				nieuwontleed (MATRIKSSTR0_);
				parse [iparse]. content.object = thee;
			} else {
				oudlees;
				if (! parseExpression ()) return 0;
				if (nieuwlees == KOMMA_) {
					if (! parseExpression ()) return 0;
					nieuwontleed (MATRIKSSTR2_);
					parse [iparse]. content.object = thee;
					if (! pas (RECHTEHAAKSLUITEN_)) return 0;
				} else {
					oudlees;
					nieuwontleed (MATRIKSSTR1_);
					parse [iparse]. content.object = thee;
					if (! pas (RECHTEHAAKSLUITEN_)) return 0;
				}
			}
		} else {
			formulefout (L"After a name of a matrix$ there should be \"[\"", lexan [ilexan]. position);
			return 0;
		}
		return 1;
	}

	if (symbol >= LOW_FUNCTION_1 && symbol <= HIGH_FUNCTION_1) {
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseExpression ()) return 0;
		if (! pas (HAAKJESLUITEN_)) return 0;
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_FUNCTION_2 && symbol <= HIGH_FUNCTION_2) {
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseExpression ()) return 0;
		if (! pas (KOMMA_)) return 0;
		if (! parseExpression ()) return 0;
		if (! pas (HAAKJESLUITEN_)) return 0;
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_FUNCTION_3 && symbol <= HIGH_FUNCTION_3) {
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseExpression ()) return 0;
		if (! pas (KOMMA_)) return 0;
		if (! parseExpression ()) return 0;
		if (! pas (KOMMA_)) return 0;
		if (! parseExpression ()) return 0;
		if (! pas (HAAKJESLUITEN_)) return 0;
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_FUNCTION_N && symbol <= HIGH_FUNCTION_N) {
		int n = 0;
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (nieuwlees != HAAKJESLUITEN_) {
			oudlees;
			if (! parseExpression ()) return 0;
			n ++;
			while (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				n ++;
			}
			oudlees;
			if (! pas (HAAKJESLUITEN_)) return 0;
		}
		nieuwontleed (NUMBER_); parsenumber (n);
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_STRING_FUNCTION && symbol <= HIGH_STRING_FUNCTION) {
		if (symbol >= LOW_FUNCTION_STRNUM && symbol <= HIGH_FUNCTION_STRNUM) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == INDEX_ || symbol == RINDEX_ ||
			symbol == STARTS_WITH_ || symbol == ENDS_WITH_ ||
			symbol == INDEX_REGEX_ || symbol == RINDEX_REGEX_ || symbol == EXTRACT_NUMBER_)
		{
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == DATESTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == EXTRACT_WORDSTR_ || symbol == EXTRACT_LINESTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == ENVIRONMENTSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == FIXEDSTR_ || symbol == PERCENTSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == REPLACESTR_ || symbol == REPLACE_REGEXSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else {
			formulefout (L"Function expected", lexan [ilexan]. position);
			oudlees;
			return 0;
		}
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol == STOPWATCH_) {
		nieuwontleed (symbol);
		return 1;
	}

	formulefout (L"Symbol misplaced", lexan [ilexan]. position);
	oudlees;   /* Needed for retry if we are going to be in a string comparison. */
	return 0;
}

static int parseFactor (void);

static int parsePowerFactors (void) {
	if (nieuwlees == POWER_) {
		if (ilexan > 2 && lexan [ilexan - 2]. symbol == MINUS_ && lexan [ilexan - 1]. symbol == NUMBER_) {
			formulefout (L"Expressions like -3^4 are ambiguous; use (-3)^4 or -(3^4) or -(3)^4", lexan [ilexan]. position);
			oudlees;
			return 0;
		}
		if (! parseFactor ()) return 0;   /* Like a^-b */
		nieuwontleed (POWER_);
	}
	else
		oudlees;
	return 1;
}

static int parseMinus (void) {
	if (! parsePowerFactor ()) return 0;
	return parsePowerFactors ();
}

static int parseFactor (void) {
	if (nieuwlees == MINUS_) {
		if (! parseFactor ()) return 0;   /* There can be multiple consecutive minuses. */
		nieuwontleed (MINUS_);
		return 1;
	}
	oudlees;
	return parseMinus ();   /* Like -a^b */
}

static int parseFactors (void) {
	int sym = nieuwlees;  /* Moet lokaal, vanwege rekursie. */
	if (sym == MUL_ || sym == RDIV_ || sym == IDIV_ || sym == MOD_) {
		if (! parseFactor ()) return 0;
		nieuwontleed (sym);
		if (! parseFactors ()) return 0;
	}
	else oudlees;
	return 1;
}

static int parseTerm (void) {
	if (! parseFactor ()) return 0;
	return parseFactors ();
}

static int parseTerms (void) {
	int symbol = nieuwlees;
	if (symbol == ADD_ || symbol == SUB_) {
		if (! parseTerm ()) return 0;
		nieuwontleed (symbol);
		if (! parseTerms ()) return 0;
	}
	else oudlees;
	return 1;
}

static int parseNot (void) {
	int symbol;
	if (! parseTerm ()) return 0;
	if (! parseTerms ()) return 0;
	symbol = nieuwlees;
	if (symbol >= EQ_ && symbol <= GT_) {
		if (! parseTerm ()) return 0;
		if (! parseTerms ()) return 0;
		nieuwontleed (symbol);
	}
	else oudlees;
	return 1;
}

static int parseAnd (void) {
	if (nieuwlees == NOT_) {
		if (! parseAnd ()) return 0;   /* Like not not not */
		nieuwontleed (NOT_);
		return 1;
	}
	oudlees;
	return parseNot ();
}

static int parseOr (void) {
	if (! parseAnd ()) return 0;
	if (nieuwlees == AND_) {
		int falseLabel = nieuwlabel;
		int andLabel = nieuwlabel;
		do {
			nieuwontleed (IFFALSE_); ontleedlabel (falseLabel);
			if (! parseAnd ()) return 0;
		} while (nieuwlees == AND_);
		nieuwontleed (IFFALSE_); ontleedlabel (falseLabel);
		nieuwontleed (TRUE_);
		nieuwontleed (GOTO_); ontleedlabel (andLabel);
		nieuwontleed (LABEL_); ontleedlabel (falseLabel);
		nieuwontleed (FALSE_);
		nieuwontleed (LABEL_); ontleedlabel (andLabel);
	}
	oudlees;
	return 1;
}

static int parseExpression (void) {
	if (! parseOr ()) return 0;
	if (nieuwlees == OR_) {
		int trueLabel = nieuwlabel;
		int orLabel = nieuwlabel;
		do {
			nieuwontleed (IFTRUE_); ontleedlabel (trueLabel);
			if (! parseOr ()) return 0;
		} while (nieuwlees == OR_);
		nieuwontleed (IFTRUE_); ontleedlabel (trueLabel);
		nieuwontleed (FALSE_);
		nieuwontleed (GOTO_); ontleedlabel (orLabel);
		nieuwontleed (LABEL_); ontleedlabel (trueLabel);
		nieuwontleed (TRUE_);
		nieuwontleed (LABEL_); ontleedlabel (orLabel);
	}
	oudlees;
	return 1;
}

/*
	Translate the infix expression "my lexan" into the postfix expression "my parse":
	remove parentheses and brackets, commas,
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

static int Formula_parseExpression (void) {
	ilabel = ilexan = iparse = 0;
	if (lexan [1]. symbol == END_) return Melder_error ("Empty formula.");
	if (! parseExpression () || ! pas (END_)) return 0;
	nieuwontleed (END_);
	numberOfInstructions = iparse;
	return 1;
}

static void schuif (int begin, int afstand) {
	int j;
	numberOfInstructions -= afstand;
	for (j = begin; j <= numberOfInstructions; j ++)
		parse [j] = parse [j + afstand];
}

static int vindLabel (int label) {
	int result = numberOfInstructions;
	while (parse [result]. symbol != LABEL_ ||
			 parse [result]. content.label != label)
		result --;
	return result;
}

static void Formula_optimizeFlow (void)
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
				for (j = 1; j < i; j ++)
					if ((parse [j]. symbol == GOTO_ || parse [j]. symbol == IFFALSE_ || parse [j]. symbol == IFTRUE_)
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

static void Formula_evaluateConstants (void) {
	for (;;) {
		int improved = 0, i;
		for (i = 1; i <= numberOfInstructions; i ++) {
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
				}
			}
			if (gain) { improved = 1; schuif (i + 1, gain); }
		}
		if (! improved) break;
	}
}

static void Formula_removeLabels (void) {
/* Vertaal symbolische labels (<0) in adressen (> 0). */
	int i = 1, j;
	while (i <= numberOfInstructions) {
		int symboli = parse [i]. symbol;
		if (symboli == GOTO_ || symboli == IFTRUE_ || symboli == IFFALSE_)
			for (j = theOptimize ? i + 1 : 1; j <= numberOfInstructions; j ++) /* Alleen voorwaarts. */
				if (parse [j]. symbol == LABEL_ && parse [i]. content.label == parse [j]. content.label)
					parse [i]. content.label = j;
				else
					(void) 0;
		else if (theOptimize && symboli == LABEL_) {
			schuif (i, 1);      /* Verwijder een label. */
			for (j = 1; j < i; j ++) {
				int symbolj = parse [j]. symbol;
				if ((symbolj == GOTO_ || symbolj == IFTRUE_ || symbolj == IFFALSE_) && parse [j]. content.label > i)
					parse [j]. content.label --;  /* Pas een label aan. */
			}
			i --;   /* Voorkom ophogen i (overbodig?). */
		}
		i ++;
	}
	numberOfInstructions --;   /* Het END_-symbol hoeft niet geinterpreteerd. */
}

/*
 * For debugging.
 */
static void Formula_print (FormulaInstruction f) {
	int i = 0, symbol;
	do {
		wchar_t *instructionName;
		symbol = f [++ i]. symbol;
		instructionName = Formula_instructionNames [symbol];
		if (symbol == NUMBER_)
			Melder_casual ("%d %ls %.17g", i, instructionName, f [i]. content.number);
		else if (symbol == GOTO_ || symbol == IFFALSE_ || symbol == IFTRUE_ || symbol == LABEL_)
			Melder_casual ("%d %ls %d", i, instructionName, f [i]. content.label);
		else if (symbol == NUMERIC_VARIABLE_)
			Melder_casual ("%d %ls %ls %ls", i, instructionName, f [i]. content.variable -> key, Melder_double (f [i]. content.variable -> numericValue));
		else if (symbol == STRING_VARIABLE_)
			Melder_casual ("%d %ls %ls %ls", i, instructionName, f [i]. content.variable -> key, f [i]. content.variable -> stringValue);
		else if (symbol == STRING_)
			Melder_casual ("%d %ls \"%ls\"", i, instructionName, f [i]. content.string);
		else if (symbol == MATRIKS_ || symbol == MATRIKSSTR_ || symbol == MATRIKS1_ || symbol == MATRIKSSTR1_ ||
		         symbol == MATRIKS2_ || symbol == MATRIKSSTR2_ || symbol == ROWSTR_ || symbol == COLSTR_)
		{
			Thing object = (Thing) f [i]. content.object;
			if (object) {
				Melder_casual ("%d %ls %s %s", i, instructionName, Thing_className (object), object -> name);
			} else {
				Melder_casual ("%d %ls", i, instructionName);
			}
		}
		else
			Melder_casual ("%d %ls", i, instructionName);
	} while (symbol != END_);
}

int Formula_compile (Any interpreter, Any data, const wchar_t *expression, int expressionType, int optimize) {
	theInterpreter = interpreter ? interpreter : UiInterpreter_get ();
	theSource = data;
	theExpression = expression;
	theExpressionType = expressionType;
	theOptimize = optimize;
	if (! lexan) {
		lexan = Melder_calloc (struct FormulaInstruction, 3000);
		lexan [3000 - 1]. symbol = END_;   /* Make sure that string cleaning always terminates. */
	}
	if (! parse) parse = Melder_calloc (struct FormulaInstruction, 3000);
	if (lexan == NULL || parse == NULL)
		return Melder_error ("Out of memory during formula computation.");

	/*
		Clean up strings from the previous call.
		These strings are in a union, that's why this cannot be done later, when a new string is created.
	*/
	if (numberOfStringConstants) {
		ilexan = 1;
		for (;;) {
			int symbol = lexan [ilexan]. symbol;
			if (symbol == STRING_) Melder_free (lexan [ilexan]. content.string);
			else if (symbol == END_) break;   /* Either the end of a formula, or the end of lexan. */
			ilexan ++;
		}
		numberOfStringConstants = 0;
	}

	if (! Formula_lexan ()) return 0;
	if (Melder_debug == 17) Formula_print (lexan);
	if (! Formula_parseExpression ()) return 0;
	if (Melder_debug == 17) Formula_print (parse);
	if (theOptimize) {
		Formula_optimizeFlow ();
		if (Melder_debug == 17) Formula_print (parse);
		Formula_evaluateConstants ();
		if (Melder_debug == 17) Formula_print (parse);
	}
	Formula_removeLabels ();
	if (Melder_debug == 17) Formula_print (parse);
	return 1;
}

/*
 * Running.
 */

static int programPointer;

typedef struct Stackel {
	#define Stackel_NUMBER  0
	#define Stackel_STRING  1
	int which;   /* 0 or negative = no clean-up required, positive = requires clean-up */
	union {
		double number;
		wchar_t *string;
	} content;
} *Stackel;

static void Stackel_cleanUp (Stackel me) {
	if (my which == Stackel_STRING) {
		Melder_free (my content.string);
	}
}
static Stackel theStack;
static int w, wmax;   /* w = stack pointer; */
#define pop  & theStack [w --]
static void pushNumber (double x) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > 0) Stackel_cleanUp (stackel); if (w > wmax) wmax ++;
	stackel -> which = Stackel_NUMBER; stackel -> content.number = x;
}
static void pushString (wchar_t *x) {
	Stackel stackel = & theStack [++ w];
	if (stackel -> which > 0) Stackel_cleanUp (stackel); if (w > wmax) wmax ++;
	stackel -> which = Stackel_STRING; stackel -> content.string = x;
}
static wchar_t *Stackel_whichText (Stackel me) {
	return
		my which == Stackel_NUMBER ? L"a number" :
		my which == Stackel_STRING ? L"a string" :
		L"???";
}

static void do_not (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : x->content.number == 0.0 ? 1.0 : 0.0);
	} else {
		error3 (L"Cannot negate (\"not\") ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_eq (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == y->content.number ? 1.0 : 0.0);   /* Even if undefined. */
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = wcsequ (x->content.string, y->content.string) ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		error5 (L"Cannot compare (=) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_ne (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number != y->content.number ? 1.0 : 0.0);   /* Even if undefined. */
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = wcsequ (x->content.string, y->content.string) ? 0.0 : 1.0;
		pushNumber (result);
	} else {
		error5 (L"Cannot compare (<>) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_le (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number <= y->content.number ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = wcscmp (x->content.string, y->content.string) <= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		error5 (L"Cannot compare (<=) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_lt (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number < y->content.number ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = wcscmp (x->content.string, y->content.string) < 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		error5 (L"Cannot compare (<) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_ge (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number >= y->content.number ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = wcscmp (x->content.string, y->content.string) >= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		error5 (L"Cannot compare (>=) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_gt (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number > y->content.number ? 1.0 : 0.0);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		double result = wcscmp (x->content.string, y->content.string) > 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		error5 (L"Cannot compare (>) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_add (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number + y->content.number);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		long length1 = wcslen (x->content.string), length2 = wcslen (y->content.string);
		wchar_t *result = Melder_malloc (wchar_t, length1 + length2 + 1); cherror
		wcscpy (result, x->content.string);
		wcscpy (result + length1, y->content.string);
		pushString (result);
	} else {
		error5 (L"Cannot add ", Stackel_whichText (y), L" to ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_sub (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number - y->content.number);
	} else if (x->which == Stackel_STRING && y->which == Stackel_STRING) {
		long length1 = wcslen (x->content.string), length2 = wcslen (y->content.string), newlength = length1 - length2;
		wchar_t *result;
		if (newlength >= 0 && wcsnequ (x->content.string + newlength, y->content.string, length2)) {
			result = Melder_malloc (wchar_t, newlength + 1); cherror
			wcsncpy (result, x->content.string, newlength);
			result [newlength] = '\0';
		} else {
			result = Melder_wcsdup (x->content.string); cherror
		}
		pushString (result);
	} else {
		error5 (L"Cannot subtract (-) ", Stackel_whichText (y), L" from ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_mul (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			x->content.number * y->content.number);
	} else {
		error5 (L"Cannot multiply (*) ", Stackel_whichText (x), L" by ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_rdiv (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			y->content.number == 0.0 ? NUMundefined :
			x->content.number / y->content.number);
	} else {
		error5 (L"Cannot divide (/) ", Stackel_whichText (x), L" by ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_idiv (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			y->content.number == 0.0 ? NUMundefined :
			floor (x->content.number / y->content.number));
	} else {
		error5 (L"Cannot divide (\"div\") ", Stackel_whichText (x), L" by ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_mod (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			y->content.number == 0.0 ? NUMundefined :
			x->content.number - floor (x->content.number / y->content.number) * y->content.number);
	} else {
		error5 (L"Cannot divide (\"mod\") ", Stackel_whichText (x), L" by ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_minus (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : - x->content.number);
	} else {
		error3 (L"Cannot take the opposite (-) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_power (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			pow (x->content.number, y->content.number));
	} else {
		error5 (L"Cannot exponentiate (^) ", Stackel_whichText (x), L" to ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_sqr (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : x->content.number * x->content.number);
	} else {
		error3 (L"Cannot take the square (^ 2) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_function_n_n (double (*f) (double)) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : f (x->content.number));
	} else {
		error5 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires a numeric argument, not ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_abs (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : fabs (x->content.number));
	} else {
		error3 (L"Cannot take the absolute value (abs) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_round (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : floor (x->content.number + 0.5));
	} else {
		error3 (L"Cannot round ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_floor (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : floor (x->content.number));
	} else {
		error3 (L"Cannot round down (floor) ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_ceiling (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : ceil (x->content.number));
	} else {
		error3 (L"Cannot round up (ceiling) ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_sqrt (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined :
			x->content.number < 0.0 ? NUMundefined : sqrt (x->content.number));
	} else {
		error3 (L"Cannot take the square root (sqrt) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_sin (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : sin (x->content.number));
	} else {
		error3 (L"Cannot take the sine (sin) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_cos (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : cos (x->content.number));
	} else {
		error3 (L"Cannot take the cosine (cos) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_tan (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : tan (x->content.number));
	} else {
		error3 (L"Cannot take the tangent (tan) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_arcsin (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined :
			fabs (x->content.number) > 1.0 ? NUMundefined : asin (x->content.number));
	} else {
		error3 (L"Cannot take the arcsine (arcsin) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_arccos (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined :
			fabs (x->content.number) > 1.0 ? NUMundefined : acos (x->content.number));
	} else {
		error3 (L"Cannot take the arccosine (arccos) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_arctan (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : atan (x->content.number));
	} else {
		error3 (L"Cannot take the arctangent (arctan) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_exp (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : exp (x->content.number));
	} else {
		error3 (L"Cannot exponentiate (exp) ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_sinh (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : sinh (x->content.number));
	} else {
		error3 (L"Cannot take the hyperbolic sine (sinh) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_cosh (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : cosh (x->content.number));
	} else {
		error3 (L"Cannot take the hyperbolic cosine (cosh) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_tanh (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined : tanh (x->content.number));
	} else {
		error3 (L"Cannot take the hyperbolic tangent (tanh) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_log2 (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined :
			x->content.number <= 0.0 ? NUMundefined : log (x->content.number) * NUMlog2e);
	} else {
		error3 (L"Cannot take the base-2 logarithm (log2) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_ln (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined :
			x->content.number <= 0.0 ? NUMundefined : log (x->content.number));
	} else {
		error3 (L"Cannot take the natural logarithm (ln) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_log10 (void) {
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined ? NUMundefined :
			x->content.number <= 0.0 ? NUMundefined : log10 (x->content.number));
	} else {
		error3 (L"Cannot take the base-10 logarithm (log10) of ", Stackel_whichText (x), L".")
	}
end: return;
}
static void do_function_dd_d (double (*f) (double, double)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			f (x->content.number, y->content.number));
	} else {
		error7 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires two numeric arguments, not ",
			Stackel_whichText (x), L" and ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_function_dl_d (double (*f) (double, long)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			f (x->content.number, y->content.number));
	} else {
		error7 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires two numeric arguments, not ",
			Stackel_whichText (x), L" and ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_function_ld_d (double (*f) (long, double)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			f (x->content.number, y->content.number));
	} else {
		error7 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires two numeric arguments, not ",
			Stackel_whichText (x), L" and ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_function_ll_l (long (*f) (long, long)) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			f (x->content.number, y->content.number));
	} else {
		error7 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires two numeric arguments, not ",
			Stackel_whichText (x), L" and ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_objects_are_identical (void) {
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		int id1 = x->content.number, id2 = y->content.number;
		int i = theCurrentPraat -> n;
		while (i > 0 && id1 != theCurrentPraat -> list [i]. id) i --;
		if (i == 0) error3 (L"Object #", Melder_integer (id1), L" does not exist in function objectsAreIdentical.")
		Data object1 = theCurrentPraat -> list [i]. object;
		i = theCurrentPraat -> n;
		while (i > 0 && id2 != theCurrentPraat -> list [i]. id) i --;
		if (i == 0) error3 (L"Object #", Melder_integer (id2), L" does not exist in function objectsAreIdentical.")
		Data object2 = theCurrentPraat -> list [i]. object;
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined ? NUMundefined :
			Data_equal (object1, object2));
	} else {
		error5 (L"The function objectsAreIdentical requires two numeric arguments (object IDs), not ",
			Stackel_whichText (x), L" and ", Stackel_whichText (y), L".")
	}
end: return;
}
static void do_function_ddd_d (double (*f) (double, double, double)) {
	Stackel z = pop, y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER && z->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined || z->content.number == NUMundefined ? NUMundefined :
			f (x->content.number, y->content.number, z->content.number));
	} else {
		error9 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires three numeric arguments, not ", Stackel_whichText (x), L", ",
			Stackel_whichText (y), L", and ", Stackel_whichText (z), L".")
	}
end: return;
}
static void do_function_dll_d (double (*f) (double, long, long)) {
	Stackel z = pop, y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER && z->which == Stackel_NUMBER) {
		pushNumber (x->content.number == NUMundefined || y->content.number == NUMundefined || z->content.number == NUMundefined ? NUMundefined :
			f (x->content.number, y->content.number, z->content.number));
	} else {
		error9 (L"The function ", Formula_instructionNames [parse [programPointer]. symbol],
			L" requires three numeric arguments, not ", Stackel_whichText (x), L", ",
			Stackel_whichText (y), L", and ", Stackel_whichText (z), L".")
	}
end: return;
}
static void do_min (void) {
	Stackel n = pop, last;
	double result;
	int j;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->content.number < 1) error1 (L"The function \"min\" requires at least one argument.")
	last = pop;
	if (last->which != Stackel_NUMBER) error3 (L"The function \"min\" can only have numeric arguments, not ", Stackel_whichText (last), L".")
	result = last->content.number;
	for (j = n->content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER)
			error3 (L"The function \"min\" can only have numeric arguments, not ", Stackel_whichText (previous), L".")
		result = result == NUMundefined || previous->content.number == NUMundefined ? NUMundefined :
			result < previous->content.number ? result : previous->content.number;
	}
	pushNumber (result);
end: return;
}
static void do_max (void) {
	Stackel n = pop, last;
	double result;
	int j;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->content.number < 1) error1 (L"The function \"max\" requires at least one argument.")
	last = pop;
	if (last->which != Stackel_NUMBER) error3 (L"The function \"max\" can only have numeric arguments, not ", Stackel_whichText (last), L".")
	result = last->content.number;
	for (j = n->content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER) error3 (L"The function \"max\" can only have numeric arguments, not ", Stackel_whichText (previous), L".")
		result = result == NUMundefined || previous->content.number == NUMundefined ? NUMundefined :
			result > previous->content.number ? result : previous->content.number;
	}
	pushNumber (result);
end: return;
}
static void do_imin (void) {
	Stackel n = pop, last;
	double minimum, result;
	int j;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->content.number < 1) error1 (L"The function \"imin\" requires at least one argument.")
	last = pop;
	if (last->which != Stackel_NUMBER) error3 (L"The function \"imin\" can only have numeric arguments, not ", Stackel_whichText (last), L".")
	minimum = last->content.number;
	result = n->content.number;
	for (j = n->content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER) error3 (L"The function \"imin\" can only have numeric arguments, not ", Stackel_whichText (previous), L".")
		if (minimum == NUMundefined || previous->content.number == NUMundefined) {
			minimum = NUMundefined;
			result = NUMundefined;
		} else if (previous->content.number < minimum) {
			minimum = previous->content.number;
			result = j;
		}
	}
	pushNumber (result);
end: return;
}
static void do_imax (void) {
	Stackel n = pop, last;
	double maximum, result;
	int j;
	Melder_assert (n->which == Stackel_NUMBER);
	if (n->content.number < 1) error1 (L"The function \"imax\" requires at least one argument.")
	last = pop;
	if (last->which != Stackel_NUMBER) error3 (L"The function \"imax\" can only have numeric arguments, not ", Stackel_whichText (last), L".")
	maximum = last->content.number;
	result = n->content.number;
	for (j = n->content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous->which != Stackel_NUMBER) error3 (L"The function \"imax\" can only have numeric arguments, not ", Stackel_whichText (previous), L".")
		if (maximum == NUMundefined || previous->content.number == NUMundefined) {
			maximum = NUMundefined;
			result = NUMundefined;
		} else if (previous->content.number > maximum) {
			maximum = previous->content.number;
			result = j;
		}
	}
	pushNumber (result);
end: return;
}
static void do_length (void) {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		double result = wcslen (s->content.string);
		pushNumber (result);
	} else {
		error3 (L"The function \"length\" requires a string, not ", Stackel_whichText (s), L".")
	}
end: return;
}
static void do_fileReadable (void) {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		structMelderFile file = { 0 };
		Melder_relativePathToFile (s->content.string, & file); cherror
		pushNumber (MelderFile_readable (& file));
	} else {
		error3 (L"The function \"fileReadable\" requires a string, not ", Stackel_whichText (s), L".")
	}
end: return;
}
static void do_dateStr (void) {
	time_t today = time (NULL);
	wchar_t *date, *newline;
	date = Melder_utf8ToWcs (ctime (& today)); cherror
	newline = wcschr (date, '\n');
	if (newline) *newline = '\0';
	pushString (date);
end: return;
}
static void do_leftStr (void) {
	Stackel narg = pop;
	if (narg->content.number == 1 || narg->content.number == 2) {
		Stackel x = ( narg->content.number == 2 ? pop : NULL ), s = pop;
		if (s->which == Stackel_STRING && (x == NULL || x->which == Stackel_NUMBER)) {
			long newlength = x ? x->content.number : 1;
			wchar_t *result;
			long length = wcslen (s->content.string);
			if (newlength < 0) newlength = 0;
			if (newlength > length) newlength = length;
			result = Melder_malloc (wchar_t, newlength + 1); cherror
			wcsncpy (result, s->content.string, newlength);
			result [newlength] = '\0';
			pushString (result);
		} else {
			error1 (L"The function \"left$\" requires a string, or a string and a number.")
		}
	} else {
		error1 (L"The function \"left$\" requires one or two arguments.")
	}
end: return;
}
static void do_rightStr (void) {
	Stackel narg = pop;
	if (narg->content.number == 1 || narg->content.number == 2) {
		Stackel x = ( narg->content.number == 2 ? pop : NULL ), s = pop;
		if (s->which == Stackel_STRING && (x == NULL || x->which == Stackel_NUMBER)) {
			long newlength = x ? x->content.number : 1;
			wchar_t *result;
			long length = wcslen (s->content.string);
			if (newlength < 0) newlength = 0;
			if (newlength > length) newlength = length;
			result = Melder_wcsdup (s->content.string + length - newlength); cherror
			pushString (result);
		} else {
			error1 (L"The function \"right$\" requires a string, or a string and a number.")
		}
	} else {
		error1 (L"The function \"right$\" requires one or two arguments.")
	}
end: return;
}
static void do_midStr (void) {
	Stackel narg = pop;
	if (narg->content.number == 2 || narg->content.number == 3) {
		Stackel y = ( narg->content.number == 3 ? pop : NULL ), x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER && (y == NULL || y->which == Stackel_NUMBER)) {
			long newlength = y ? y->content.number : 1;
			long start = x->content.number;
			long length = wcslen (s->content.string), finish = start + newlength - 1;
			wchar_t *result;
			if (start < 1) start = 1;
			if (finish > length) finish = length;
			newlength = finish - start + 1;
			if (newlength > 0) {
				result = Melder_malloc (wchar_t, newlength + 1); cherror
				wcsncpy (result, s->content.string + start - 1, newlength);
				result [newlength] = '\0';
			} else {
				result = Melder_wcsdup (L""); cherror
			}
			pushString (result);
		} else {
			error1 (L"The function \"mid$\" requires a string and one or two numbers.")
		}
	} else {
		error1 (L"The function \"mid$\" requires two or three arguments.")
	}
end: return;
}
static void do_environmentStr (void) {
	Stackel s = pop;
	if (s->which == Stackel_STRING) {
		wchar_t *value = Melder_getenv (s->content.string);
		wchar_t *result = Melder_wcsdup (value != NULL ? value : L""); cherror
		pushString (result);
	} else {
		error3 (L"The function \"environment$\" requires a string, not ", Stackel_whichText (s), L".")
	}
end: return;
}
static void do_index (void) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		wchar_t *substring = wcsstr (s->content.string, t->content.string);
		long result = substring ? substring - s->content.string + 1 : 0;
		pushNumber (result);
	} else {
		error5 (L"The function \"index\" requires two strings, not ",
			Stackel_whichText (s), L" and ", Stackel_whichText (t), L".")
	}
end: return;
}
static void do_rindex (void) {
	Stackel part = pop, whole = pop;
	if (whole->which == Stackel_STRING && part->which == Stackel_STRING) {
		wchar_t *lastSubstring = wcsstr (whole->content.string, part->content.string);
		if (part->content.string [0] == '\0') {
			long result = wcslen (whole->content.string);
			pushNumber (result);
		} else if (lastSubstring) {
			for (;;) {
				wchar_t *substring = wcsstr (lastSubstring + 1, part->content.string);
				if (substring == NULL) break;
				lastSubstring = substring;
			}
			pushNumber (lastSubstring - whole->content.string + 1);
		} else {
			pushNumber (0);
		}
	} else {
		error5 (L"The function \"rindex\" requires two strings, not ",
			Stackel_whichText (whole), L" and ", Stackel_whichText (part), L".")
	}
end: return;
}
static void do_stringMatchesCriterion (int criterion) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		int result = Melder_stringMatchesCriterion (s->content.string, criterion, t->content.string);
		pushNumber (result);
	} else {
		error7 (L"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			L"\" requires two strings, not ", Stackel_whichText (s), L" and ", Stackel_whichText (t), L".")
	}
end: return;
}
static void do_index_regex (int backward) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		char *sA = Melder_wcsToUtf8 (s->content.string), *tA = Melder_wcsToUtf8 (t->content.string);
		char *errorMessage;
		regexp *compiled_regexp = CompileRE (tA, & errorMessage, 0);
		if (compiled_regexp == NULL) {
			pushNumber (NUMundefined);
		} else if (ExecRE (compiled_regexp, NULL, sA, NULL, backward, '\0', '\0', NULL, NULL, NULL)) {
			char *place = compiled_regexp -> startp [0];
			free (compiled_regexp);
			long numberOfCharacters = 0;
			for (char *p = sA; place - p > 0; p ++) if ((unsigned char) *p <= 127 || (unsigned char) *p >= 0xC2) numberOfCharacters ++;
			pushNumber (numberOfCharacters + 1);
		} else {
			pushNumber (FALSE);
		}
		Melder_free (sA);
		Melder_free (tA);
	} else {
		error7 (L"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			L"\" requires two strings, not ", Stackel_whichText (s), L" and ", Stackel_whichText (t), L".")
	}
end: return;
}
static void do_replaceStr (void) {
	Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		long numberOfMatches;
		wchar_t *result = str_replace_literal (s->content.string, t->content.string, u->content.string, x->content.number, & numberOfMatches); cherror
		pushString (result);
	} else {
		error1 (L"The function \"replace$\" requires three strings and a number.")
	}
end: return;
}
static void do_replace_regexStr (void) {
	Stackel x = pop, u = pop, t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING && u->which == Stackel_STRING && x->which == Stackel_NUMBER) {
		char *sA = Melder_wcsToUtf8 (s->content.string), *tA = Melder_wcsToUtf8 (t->content.string), *uA = Melder_wcsToUtf8 (u->content.string);
		char *errorMessage;
		regexp *compiled_regexp = CompileRE (tA, & errorMessage, 0);
		if (compiled_regexp == NULL) {
			wchar_t *result = Melder_wcsdup (L""); cherror
			pushString (result);
		} else {
			long numberOfMatches;
			char *resultA = str_replace_regexp (sA, compiled_regexp, uA, x->content.number, & numberOfMatches); cherror
			wchar_t *result = Melder_utf8ToWcs (resultA ? resultA : ""); cherror
			free (resultA);
			pushString (result);
		}
		Melder_free (sA);
		Melder_free (tA);
		Melder_free (uA);
	} else {
		error1 (L"The function \"replace_regex$\" requires three strings and a number.")
	}
end: return;
}
static void do_extractNumber (void) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		wchar_t *substring = wcsstr (s->content.string, t->content.string);
		if (substring == NULL) {
			pushNumber (NUMundefined);
		} else {
			/* Skip the prompt. */
			substring += wcslen (t->content.string);
			/* Skip white space. */
			while (*substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') substring ++;
			if (substring [0] == '\0' || wcsnequ (substring, L"--undefined--", 13)) {
				pushNumber (NUMundefined);
			} else {
				wchar_t buffer [101], *slash;
				int i;
				for (i = 0; i < 100; i ++) {
					buffer [i] = *substring;
					substring ++;
					if (*substring == '\0' || *substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') break;
				}
				if (i >= 100) {
					buffer [100] = '\0';
					pushNumber (Melder_atofW (buffer));
				} else {
					buffer [i + 1] = '\0';
					slash = wcschr (buffer, '/');
					if (slash) {
						double numerator, denominator;
						*slash = '\0';
						numerator = Melder_atofW (buffer), denominator = Melder_atofW (slash + 1);
						pushNumber (numerator / denominator);
					} else {
						pushNumber (Melder_atofW (buffer));
					}
				}
			}
		}
	} else {
		error7 (L"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			L"\" requires two strings, not ", Stackel_whichText (s), L" and ", Stackel_whichText (t), L".")
	}
end: return;
}
static void do_extractTextStr (int singleWord) {
	Stackel t = pop, s = pop;
	if (s->which == Stackel_STRING && t->which == Stackel_STRING) {
		wchar_t *substring = wcsstr (s->content.string, t->content.string), *result, *p;
		if (substring == NULL) {
			result = Melder_wcsdup (L""); cherror
		} else {
			long length;
			/* Skip the prompt. */
			substring += wcslen (t->content.string);
			if (singleWord) {
				/* Skip white space. */
				while (*substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') substring ++;
			}
			p = substring;
			if (singleWord) {
				/* Proceed until next white space. */
				while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') p ++;
			} else {
				/* Proceed until end of line. */
				while (*p != '\0' && *p != '\n' && *p != '\r') p ++;
			}
			length = p - substring;
			result = Melder_malloc (wchar_t, length + 1); cherror
			wcsncpy (result, substring, length);
			result [length] = '\0';
		}
		pushString (result);
	} else {
		error7 (L"The function \"", Formula_instructionNames [parse [programPointer]. symbol],
			L"\" requires two strings, not ", Stackel_whichText (s), L" and ", Stackel_whichText (t), L".")
	}
end: return;
}
static void do_selected (void) {
	Stackel n = pop;
	long result;
	if (n->content.number == 0) {
		result = praat_getIdOfSelected (NULL, 0); cherror
	} else if (n->content.number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			void *klas = Thing_classFromClassNameW (a->content.string); cherror
			result = praat_getIdOfSelected (klas, 0); cherror
		} else if (a->which == Stackel_NUMBER) {
			result = praat_getIdOfSelected (NULL, a->content.number); cherror
		} else {
			error1 (L"The function \"selected\" requires a string (an object type name) and/or a number.")
		}
	} else if (n->content.number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			void *klas = Thing_classFromClassNameW (s->content.string); cherror
			result = praat_getIdOfSelected (klas, x->content.number); cherror
		} else {
			error1 (L"The function \"selected\" requires a string (an object type name) and/or a number.")
		}
	} else {
		error3 (L"The function \"selected\" requires 0, 1, or 2 arguments, not ", Melder_integer (n->content.number), L".")
	}
	pushNumber (result);
end: return;
}
static void do_selectedStr (void) {
	Stackel n = pop;
	wchar_t *name, *result = NULL;
	if (n->content.number == 0) {
		name = praat_getNameOfSelected (NULL, 0); cherror
		result = Melder_wcsdup (name); cherror
	} else if (n->content.number == 1) {
		Stackel a = pop;
		if (a->which == Stackel_STRING) {
			void *klas = Thing_classFromClassNameW (a->content.string); cherror
			name = praat_getNameOfSelected (klas, 0); cherror
			result = Melder_wcsdup (name); cherror
		} else if (a->which == Stackel_NUMBER) {
			name = praat_getNameOfSelected (NULL, a->content.number); cherror
			result = Melder_wcsdup (name); cherror
		} else {
			error1 (L"The function \"selected$\" requires a string (an object type name) and/or a number.")
		}
	} else if (n->content.number == 2) {
		Stackel x = pop, s = pop;
		if (s->which == Stackel_STRING && x->which == Stackel_NUMBER) {
			void *klas = Thing_classFromClassNameW (s->content.string); cherror
			name = praat_getNameOfSelected (klas, x->content.number); cherror
			result = Melder_wcsdup (name); cherror
		} else {
			error3 (L"The function \"selected$\" requires 0, 1, or 2 arguments, not ", Melder_integer (n->content.number), L".")
		}
	}
	pushString (result);
end: return;
}
static void do_numberOfSelected (void) {
	Stackel n = pop;
	long result;
	if (n->content.number == 0) {
		result = praat_selection (NULL);
	} else if (n->content.number == 1) {
		Stackel s = pop;
		if (s->which == Stackel_STRING) {
			void *klas = Thing_classFromClassNameW (s->content.string); cherror
			result = praat_selection (klas);
		} else {
			error3 (L"The function \"numberOfSelected\" requires a string (an object type name), not ", Stackel_whichText (s), L".")
		}
	} else {
		error3 (L"The function \"numberOfSelected\" requires 0 or 1 arguments, not ", Melder_integer (n->content.number), L".")
	}
	pushNumber (result);
end: return;
}
static void do_fixedStr (void) {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		wchar_t *result = Melder_wcsdup (Melder_fixed (value->content.number, precision->content.number));
		pushString (result);
	} else {
		error5 (L"The function \"fixed$\" requires two numbers, not ", Stackel_whichText (value), L" and ", Stackel_whichText (precision), L".")
	}
end: return;
}
static void do_percentStr (void) {
	Stackel precision = pop, value = pop;
	if (value->which == Stackel_NUMBER && precision->which == Stackel_NUMBER) {
		wchar_t *result = Melder_wcsdup (Melder_percent (value->content.number, precision->content.number));
		pushString (result);
	} else {
		error5 (L"The function \"percent$\" requires two numbers, not ", Stackel_whichText (value), L" and ", Stackel_whichText (precision), L".")
	}
end: return;
}
static long Stackel_getRowNumber (Stackel row, Data thee) {
	long result;
	if (row->which == Stackel_NUMBER) {
		result = floor (row->content.number + 0.5);   /* Round. */
	} else if (row->which == Stackel_STRING) {
		if (your getRowIndex == NULL)
			return Melder_error3 (L"Objects of type ", Thing_classNameW (thee),
				L" do not have row labels, so row indexes have to be numbers.");
		result = your getRowIndex (thee, row->content.string);
		if (result == 0)
			return Melder_error5 (L"Object \"", thy nameW,
				L"\" has no row labelled \"", row->content.string, L"\".");
	} else {
		return Melder_error3 (L"A row index should be a number or a string, not ", Stackel_whichText (row), L".");
	}
	return result;
}
static long Stackel_getColumnNumber (Stackel column, Data thee) {
	long result;
	if (column->which == Stackel_NUMBER) {
		result = floor (column->content.number + 0.5);   /* Round. */
	} else if (column->which == Stackel_STRING) {
		if (your getColumnIndex == NULL)
			return Melder_error3 (L"Objects of type ", Thing_classNameW (thee),
				L" do not have column labels, so column indexes have to be numbers.");
		result = your getColumnIndex (thee, column->content.string);
		if (result == 0)
			return Melder_error5 (L"Object \"", thy nameW,
				L"\" has no column labelled \"", column->content.string, L"\".");
	} else {
		return Melder_error3 (L"A column index should be a number or a string, not ", Stackel_whichText (column), L".");
	}
	return result;
}
static void do_self0 (long irow, long icol) {
	Data me = theSource;
	if (me == NULL) error1 (L"The name \"self\" is restricted to formulas for objects.")
	if (our getCell) {
		pushNumber (our getCell (me));
	} else if (our getVector) {
		if (icol == 0) {
			error3 (L"We are not in a loop, hence no implicit column index for the current ",
				Thing_classNameW (me), L" object (self).\nTry using the [column] index explicitly.")
		} else {
			pushNumber (our getVector (me, irow, icol));
		}
	} else if (our getMatrix) {
		if (irow == 0) {
			if (icol == 0) {
				error3 (L"We are not in a loop over rows and columns,\n"
					"hence no implicit row and column indexing for the current ",
					Thing_classNameW (me), L" object (self).\n"
					"Try using both [row, column] indexes explicitly.")
			} else {
				error3 (L"We are not in a loop over columns only,\n"
					"hence no implicit row index for the current ",
					Thing_classNameW (me), L" object (self).\n"
					"Try using the [row] index explicitly.")
			}
		} else {
			pushNumber (our getMatrix (me, irow, icol));
		}
	} else {
		error2 (Thing_classNameW (me), L" objects (like self) accept no [] indexing.")
	}
end: return;
}
static void do_matriks0 (long irow, long icol) {
	Data thee = parse [programPointer]. content.object;
	if (your getCell) {
		pushNumber (your getCell (thee));
	} else if (your getVector) {
		if (icol == 0) {
			error3 (L"We are not in a loop,\n"
				"hence no implicit column index for this ", Thing_classNameW (thee), L" object.\n"
				"Try using the [column] index explicitly.")
		} else {
			pushNumber (your getVector (thee, irow, icol));
		}
	} else if (your getMatrix) {
		if (irow == 0) {
			if (icol == 0) {
				error3 (L"We are not in a loop over rows and columns,\n"
					"hence no implicit row and column indexing for this ", Thing_classNameW (thee), L" object.\n"
					"Try using both [row, column] indexes explicitly.")
			} else {
				error3 (L"We are not in a loop over columns only,\n"
					"hence no implicit row index for this ", Thing_classNameW (thee), L" object.\n"
					"Try using the [row] index explicitly.")
			}
		} else {
			pushNumber (your getMatrix (thee, irow, icol));
		}
	} else {
		error2 (Thing_classNameW (thee), L" objects accept no [] indexing.")
	}
end: return;
}
static void do_selfMatriks1 (long irow) {
	Data me = theSource;
	Stackel column = pop;
	long icol;
	if (me == NULL) error1 (L"The name \"self\" is restricted to formulas for objects.")
	icol = Stackel_getColumnNumber (column, me); cherror
	if (our getVector) {
		pushNumber (our getVector (me, irow, icol));
	} else if (our getMatrix) {
		if (irow == 0) {
			error3 (L"We are not in a loop,\n"
				"hence no implicit row index for the current ", Thing_classNameW (me), L" object (self).\n"
				"Try using both [row, column] indexes instead.")
		} else {
			pushNumber (our getMatrix (me, irow, icol));
		}
	} else {
		error2 (Thing_classNameW (me), L" objects (like self) accept no [column] indexes.")
	}
end: return;
}
static void do_selfMatriksStr1 (long irow) {
	Data me = theSource;
	Stackel column = pop;
	long icol;
	if (me == NULL) error1 (L"The name \"self$\" is restricted to formulas for objects.")
	icol = Stackel_getColumnNumber (column, me); cherror
	if (our getVectorStr) {
		wchar_t *result = Melder_wcsdup (our getVectorStr (me, icol)); cherror
		pushString (result);
	} else if (our getMatrixStr) {
		if (irow == 0) {
			error3 (L"We are not in a loop,\n"
				"hence no implicit row index for the current ", Thing_classNameW (me), L" object (self).\n"
				"Try using both [row, column] indexes instead.")
		} else {
			wchar_t *result = Melder_wcsdup (our getMatrixStr (me, irow, icol)); cherror
			pushString (result);
		}
	} else {
		error2 (Thing_classNameW (me), L" objects (like self) accept no [column] indexes.")
	}
end: return;
}
static void do_matriks1 (long irow) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop;
	long icol = Stackel_getColumnNumber (column, thee); cherror
	if (your getVector) {
		pushNumber (your getVector (thee, irow, icol));
	} else if (your getMatrix) {
		if (irow == 0) {
			error3 (L"We are not in a loop,\n"
				"hence no implicit row index for this ", Thing_classNameW (thee), L" object.\n"
				"Try using both [row, column] indexes instead.")
		} else {
			pushNumber (your getMatrix (thee, irow, icol));
		}
	} else {
		error2 (Thing_classNameW (thee), L" objects accept no [column] indexes.")
	}
end: return;
}
static void do_matrixStr1 (long irow) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop;
	long icol = Stackel_getColumnNumber (column, thee); cherror
	if (your getVectorStr) {
		pushString (Melder_wcsdup (your getVectorStr (thee, icol)));
	} else if (your getMatrixStr) {
		if (irow == 0) {
			error3 (L"We are not in a loop,\n"
				"hence no implicit row index for this ", Thing_classNameW (thee), L" object.\n"
				"Try using both [row, column] indexes instead.")
		} else {
			pushString (Melder_wcsdup (your getMatrixStr (thee, irow, icol)));
		}
	} else {
		error2 (Thing_classNameW (thee), L" objects accept no [column] indexes for string cells.")
	}
end: return;
}
static void do_selfMatriks2 (void) {
	Data me = theSource;
	Stackel column = pop, row = pop;
	long irow, icol;
	if (me == NULL) error1 (L"The name \"self\" is restricted to formulas for objects.")
	irow = Stackel_getRowNumber (row, me); cherror
	icol = Stackel_getColumnNumber (column, me); cherror
	if (our getMatrix == NULL) error2 (Thing_classNameW (me), L" objects like \"self\" accept no [row, column] indexing.")
	pushNumber (our getMatrix (me, irow, icol));
end: return;
}
static void do_selfMatriksStr2 (void) {
	Data me = theSource;
	Stackel column = pop, row = pop;
	wchar_t *result;
	long irow, icol;
	if (me == NULL) error1 (L"The name \"self$\" is restricted to formulas for objects.")
	irow = Stackel_getRowNumber (row, me); cherror
	icol = Stackel_getColumnNumber (column, me); cherror
	if (our getMatrixStr == NULL) error2 (Thing_classNameW (me), L" objects like \"self$\" accept no [row, column] indexing for string cells.")
	result = Melder_wcsdup (our getMatrixStr (me, irow, icol)); cherror
	pushString (result);
end: return;
}
static void do_matriks2 (void) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	long irow = Stackel_getRowNumber (row, thee); cherror
	long icol = Stackel_getColumnNumber (column, thee); cherror
	if (your getMatrix == NULL) error2 (Thing_classNameW (thee), L" objects accept no [row, column] indexing.")
	pushNumber (your getMatrix (thee, irow, icol));
end: return;
}
static void do_matriksStr2 (void) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	long irow = Stackel_getRowNumber (row, thee); cherror
	long icol = Stackel_getColumnNumber (column, thee); cherror
	if (your getMatrixStr == NULL) error2 (Thing_classNameW (thee), L" objects accept no [row, column] indexing for string cells.")
	wchar_t *result = Melder_wcsdup (your getMatrixStr (thee, irow, icol)); cherror
	pushString (result);
end: return;
}
static void do_funktie0 (long irow, long icol) {
	Data thee = parse [programPointer]. content.object;
	if (your getFunction0) {
		pushNumber (your getFunction0 (thee));
	} else if (your getFunction1) {
		Data me = theSource;
		if (me == NULL)
			error3 (L"No current object (we are not in a Formula command),\n"
				"hence no implicit x value for this ", Thing_classNameW (thee), L" object.\n"
				"Try using the (x) argument explicitly.")
		if (our getX == NULL)
			error5 (L"The current ", Thing_classNameW (me),
				L" object gives no implicit x values,\nhence no implicit x value for this ",
				Thing_classNameW (thee), L" object.\n"
				"Try using the (x) argument explicitly.")
		double x = our getX (me, icol);
		pushNumber (your getFunction1 (thee, irow, x));
	} else if (your getFunction2) {
		Data me = theSource;
		if (me == NULL)
			error3 (L"No current object (we are not in a Formula command),\n"
				"hence no implicit x or y values for this ", Thing_classNameW (thee), L" object.\n"
				"Try using both (x, y) arguments explicitly.")
		if (our getX == NULL)
			error5 (L"The current ", Thing_classNameW (me), L" object gives no implicit x values,\n"
				"hence no implicit x value for this ", Thing_classNameW (thee), L" object.\n"
				"Try using both (x, y) arguments explicitly.")
		double x = our getX (me, icol);
		if (our getY == NULL)
			error5 (L"The current ", Thing_classNameW (me), L" object gives no implicit y values,\n"
					"hence no implicit y value for this ", Thing_classNameW (thee), L" object.\n"
					"Try using the (y) argument explicitly.")
		double y = our getY (me, irow);
		pushNumber (your getFunction2 (thee, x, y));
	} else {
		error2 (Thing_classNameW (thee), L" objects accept no () values.")
	}
end: return;
}
static void do_selfFunktie1 (long irow) {
	Data me = theSource;
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		if (me == NULL) error1 (L"The name \"self\" is restricted to formulas for objects.")
		if (our getFunction1) {
			pushNumber (our getFunction1 (me, irow, x->content.number));
		} else if (our getFunction2) {
			if (our getY == NULL)
				error3 (L"The current ", Thing_classNameW (me), L" object (self) accepts no implicit y values.\n"
					"Try using both (x, y) arguments instead.")
			double y = our getY (me, irow);
			pushNumber (our getFunction2 (me, x->content.number, y));
		} else {
			error2 (Thing_classNameW (me), L" objects like \"self\" accept no (x) values.")
		}
	} else {
		error2 (Thing_classNameW (me), L" objects like \"self\" accept only numeric x values.")
	}
end: return;
}
static void do_funktie1 (long irow) {
	Data thee = parse [programPointer]. content.object;
	Stackel x = pop;
	if (x->which == Stackel_NUMBER) {
		if (your getFunction1) {
			pushNumber (your getFunction1 (thee, irow, x->content.number));
		} else if (your getFunction2) {
			Data me = theSource;
			if (me == NULL)
				error3 (L"No current object (we are not in a Formula command),\n"
					"hence no implicit y value for this ", Thing_classNameW (thee), L" object.\n"
					"Try using both (x, y) arguments instead.")
			if (our getY == NULL)
				error5 (L"The current ", Thing_classNameW (me), L" object gives no implicit y values,\n"
					"hence no implicit y value for this ", Thing_classNameW (thee), L" object.\n"
					"Try using both (x, y) arguments instead.")
			double y = our getY (me, irow);
			pushNumber (your getFunction2 (thee, x->content.number, y));
		} else {
			error2 (Thing_classNameW (thee), L" objects accept no (x) values.")
		}
	} else {
		error2 (Thing_classNameW (thee), L" objects accept only numeric x values.")
	}
end: return;
}
static void do_selfFunktie2 (void) {
	Data me = theSource;
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (me == NULL) error1 (L"The name \"self\" is restricted to formulas for objects.")
		if (our getFunction2 == NULL) error2 (Thing_classNameW (me), L" objects like \"self\" accept no (x, y) values.")
		pushNumber (our getFunction2 (me, x->content.number, y->content.number));
	} else {
		error2 (Thing_classNameW (me), L" objects accept only numeric x values.")
	}
end: return;
}
static void do_funktie2 (void) {
	Data thee = parse [programPointer]. content.object;
	Stackel y = pop, x = pop;
	if (x->which == Stackel_NUMBER && y->which == Stackel_NUMBER) {
		if (your getFunction2 == NULL) error2 (Thing_classNameW (thee), L" objects accept no (x, y) values.")
		pushNumber (your getFunction2 (thee, x->content.number, y->content.number));
	} else {
		error2 (Thing_classNameW (thee), L" objects accept only numeric x values.")
	}
end: return;
}
static void do_rowStr (void) {
	Data thee = parse [programPointer]. content.object;
	wchar_t *string;
	Stackel row = pop;
	long irow = Stackel_getRowNumber (row, thee); cherror
	string = Melder_wcsdup (your getRowStr (thee, irow));
	if (string == NULL) Melder_error1 (L"Row index out of bounds.");
	pushString (string);
end: return;
}
static void do_colStr (void) {
	Data thee = parse [programPointer]. content.object;
	wchar_t *string;
	Stackel col = pop;
	long icol = Stackel_getColumnNumber (col, thee); cherror
	string = Melder_wcsdup (your getColStr (thee, icol));
	if (string == NULL) Melder_error1 (L"Column index out of bounds.");
	pushString (string);
end: return;
}

static double NUMarcsinh (double x) {
	return log (x + sqrt (1.0 + x * x));
}
static double NUMarccosh (double x) {
	return x < 1.0 ? NUMundefined : log (x + sqrt (x * x - 1.0));
}
static double NUMarctanh (double x) {
	return x <= -1.0 || x >= 1.0 ? NUMundefined : 0.5 * log ((1.0 + x) / (1.0 - x));
}
static double NUMerf (double x) {
	return 1.0 - NUMerfcc (x);
}

int Formula_run (long row, long col, double *numericResult, wchar_t **stringResult) {
	FormulaInstruction f = parse;
	programPointer = 1;   /* First symbol of the program. */
	if (theStack == NULL) theStack = Melder_calloc (struct Stackel, 10000);
	if (theStack == NULL)
		return Melder_error1 (L"Out of memory during formula computation.");
	w = 0, wmax = 0;   /* start new stack. */
	while (programPointer <= numberOfInstructions) {
		int symbol;
		switch (symbol = f [programPointer]. symbol) {

case NUMBER_: { pushNumber (f [programPointer]. content.number);
} break; case STOPWATCH_: { pushNumber (Melder_stopwatch ());
} break; case ROW_: { pushNumber (row);
} break; case COL_: { pushNumber (col);
} break; case X_: {
	Data me = theSource;
	if (our getX == NULL) error1 (L"No values for \"x\" for this object.")
	pushNumber (our getX (me, col));
} break; case Y_: {
	Data me = theSource;
	if (our getY == NULL) error1 (L"No values for \"y\" for this object.")
	pushNumber (our getY (me, row));
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
/********** Functions of 1 numerical variable: **********/
} break; case ABS_: { do_abs ();
} break; case ROUND_: { do_round ();
} break; case FLOOR_: { do_floor ();
} break; case CEILING_: { do_ceiling ();
} break; case SQRT_: { do_sqrt ();
} break; case SIN_: { do_sin ();
} break; case COS_: { do_cos ();
} break; case TAN_: { do_tan ();
} break; case ARCSIN_: { do_arcsin ();
} break; case ARCCOS_: { do_arccos ();
} break; case ARCTAN_: { do_arctan ();
} break; case EXP_: { do_exp ();
} break; case SINH_: { do_sinh ();
} break; case COSH_: { do_cosh ();
} break; case TANH_: { do_tanh ();
} break; case ARCSINH_: { do_function_n_n (NUMarcsinh);
} break; case ARCCOSH_: { do_function_n_n (NUMarccosh);
} break; case ARCTANH_: { do_function_n_n (NUMarctanh);
} break; case SIGMOID_: { do_function_n_n (NUMsigmoid);
} break; case INV_SIGMOID_: { do_function_n_n (NUMinvSigmoid);
} break; case ERF_: { do_function_n_n (NUMerf);
} break; case ERFC_: { do_function_n_n (NUMerfcc);
} break; case GAUSS_P_: { do_function_n_n (NUMgaussP);
} break; case GAUSS_Q_: { do_function_n_n (NUMgaussQ);
} break; case INV_GAUSS_Q_: { do_function_n_n (NUMinvGaussQ);
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
/********** Functions of 2 numerical variables: **********/
} break; case ARCTAN2_: { do_function_dd_d (atan2);
} break; case RANDOM_UNIFORM_: { do_function_dd_d (NUMrandomUniform);
} break; case RANDOM_INTEGER_: { do_function_ll_l (NUMrandomInteger);
} break; case RANDOM_GAUSS_: { do_function_dd_d (NUMrandomGauss);
} break; case CHI_SQUARE_P_: { do_function_dd_d (NUMchiSquareP);
} break; case CHI_SQUARE_Q_: { do_function_dd_d (NUMchiSquareQ);
} break; case INV_CHI_SQUARE_Q_: { do_function_dd_d (NUMinvChiSquareQ);
} break; case STUDENT_P_: { do_function_dd_d (NUMstudentP);
} break; case STUDENT_Q_: { do_function_dd_d (NUMstudentQ);
} break; case INV_STUDENT_Q_: { do_function_dd_d (NUMinvStudentQ);
} break; case BETA_: { do_function_dd_d (NUMbeta);
} break; case BESSEL_I_: { do_function_ld_d (NUMbesselI);
} break; case BESSEL_K_: { do_function_ld_d (NUMbesselK);
} break; case SOUND_PRESSURE_TO_PHON_: { do_function_dd_d (NUMsoundPressureToPhon);
} break; case OBJECTS_ARE_IDENTICAL_: { do_objects_are_identical ();
/********** Functions of 3 numerical variables: **********/
} break; case FISHER_P_: { do_function_ddd_d (NUMfisherP);
} break; case FISHER_Q_: { do_function_ddd_d (NUMfisherQ);
} break; case INV_FISHER_Q_: { do_function_ddd_d (NUMinvFisherQ);
} break; case BINOMIAL_P_: { do_function_ddd_d (NUMbinomialP);
} break; case BINOMIAL_Q_: { do_function_ddd_d (NUMbinomialQ);
} break; case INV_BINOMIAL_P_: { do_function_ddd_d (NUMinvBinomialP);
} break; case INV_BINOMIAL_Q_: { do_function_ddd_d (NUMinvBinomialQ);
/********** Functions of a variable number of variables: **********/
} break; case MIN_: { do_min ();
} break; case MAX_: { do_max ();
} break; case IMIN_: { do_imin ();
} break; case IMAX_: { do_imax ();
/********** String functions: **********/
} break; case LENGTH_: { do_length ();
} break; case FILE_READABLE_: { do_fileReadable ();
} break; case DATESTR_: { do_dateStr ();
} break; case LEFTSTR_: { do_leftStr ();
} break; case RIGHTSTR_: { do_rightStr ();
} break; case MIDSTR_: { do_midStr ();
} break; case ENVIRONMENTSTR_: { do_environmentStr ();
} break; case INDEX_: { do_index ();
} break; case RINDEX_: { do_rindex ();
} break; case STARTS_WITH_: { do_stringMatchesCriterion (Melder_STRING_STARTS_WITH);
} break; case ENDS_WITH_: { do_stringMatchesCriterion (Melder_STRING_ENDS_WITH);
} break; case REPLACESTR_: { do_replaceStr ();
} break; case INDEX_REGEX_: { do_index_regex (FALSE);
} break; case RINDEX_REGEX_: { do_index_regex (TRUE);
} break; case REPLACE_REGEXSTR_: { do_replace_regexStr ();
} break; case EXTRACT_NUMBER_: { do_extractNumber ();
} break; case EXTRACT_WORDSTR_: { do_extractTextStr (TRUE);
} break; case EXTRACT_LINESTR_: { do_extractTextStr (FALSE);
} break; case SELECTED_: { do_selected ();
} break; case SELECTEDSTR_: { do_selectedStr ();
} break; case NUMBER_OF_SELECTED_: { do_numberOfSelected ();
} break; case FIXEDSTR_: { do_fixedStr ();
} break; case PERCENTSTR_: { do_percentStr ();
/********** **********/
} break; case TRUE_: {
	pushNumber (1.0);
} break; case FALSE_: {
	pushNumber (0.0);
/* Possible compiler BUG: many compilers cannot handle the following assignment. */
/* Those compilers have trouble with praat's AND and OR. */
} break; case IFTRUE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->content.number != 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_error3 (L"A condition between \"if\" and \"then\" has to be a number, not ",
			Stackel_whichText (condition), L".");
	}
} break; case IFFALSE_: {
	Stackel condition = pop;
	if (condition->which == Stackel_NUMBER) {
		if (condition->content.number == 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_error3 (L"A condition between \"if\" and \"then\" has to be a number, not ",
			Stackel_whichText (condition), L".");
	}
} break; case GOTO_: {
	programPointer = f [programPointer]. content.label - theOptimize;
} break; case LABEL_: {
	;
} break; case SELF0_: { do_self0 (row, col);
} break; case SELFMATRIKS1_: { do_selfMatriks1 (row);
} break; case SELFMATRIKSSTR1_: { do_selfMatriksStr1 (row);
} break; case SELFMATRIKS2_: { do_selfMatriks2 ();
} break; case SELFMATRIKSSTR2_: { do_selfMatriksStr2 ();
} break; case SELFFUNKTIE1_: { do_selfFunktie1 (row);
} break; case SELFFUNKTIE2_: { do_selfFunktie2 ();
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
	wchar_t *result = Melder_wcsdup (f [programPointer]. content.string); cherror
	pushString (result);
} break; case NUMERIC_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumber (var -> numericValue);
} break; case STRING_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	wchar_t *result = Melder_wcsdup (var -> stringValue); cherror
	pushString (result);
} break; default: return Melder_error3
			(L"Symbol \"", Formula_instructionNames [parse [programPointer]. symbol], L"\" without action.");
		} /* switch */
		cherror
		programPointer ++;
	} /* while */
	if (w != 1) Melder_fatal ("Formula: stackpointer ends at %ld instead of 1.", w);
	if (theExpressionType == EXPRESSION_TYPE_NUMERIC) {
		if (theStack [1]. which == Stackel_STRING) error1 (L"Found a string expression instead of a numeric expression.")
		if (numericResult) *numericResult = theStack [1]. content.number;
		else Melder_information1 (Melder_double (theStack [1]. content.number));
	} else if (theExpressionType == EXPRESSION_TYPE_STRING) {
		if (theStack [1]. which == Stackel_NUMBER) error1 (L"Found a numeric expression instead of a string expression.")
		if (stringResult) { *stringResult = theStack [1]. content.string; theStack [1]. content.string = NULL; }   /* Undangle. */
		else Melder_information1 (theStack [1]. content.string);
	} else {
		Melder_assert (theExpressionType == EXPRESSION_TYPE_UNKNOWN);
		if (theStack [1]. which == Stackel_NUMBER) {
			if (numericResult) *numericResult = theStack [1]. content.number;
			else Melder_information1 (Melder_double (theStack [1]. content.number));
		} else {
			Melder_assert (theStack [1]. which == Stackel_STRING);
			if (stringResult) { *stringResult = theStack [1]. content.string; theStack [1]. content.string = NULL; }   /* Undangle. */
			else Melder_information1 (theStack [1]. content.string);
		}
	}
end:
	/*
		Clean up the stack (theStack [1] may have been disowned).
	*/
	for (w = wmax; w > 0; w --) {
		Stackel stackel = & theStack [w];
		if (stackel -> which > 0) Stackel_cleanUp (stackel);
	}
	iferror return 0;
	return 1;
}

/* End of file Formula.c */
