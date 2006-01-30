/* Formula.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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

static Interpreter theInterpreter;
static Data theSource;
static const char *theExpression;
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
		char *string;
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
			ROW_, COL_, NROW_, NCOL_, Y_, X_,
		#define HIGH_ATTRIBUTE  X_
	#define HIGH_VALUE  HIGH_ATTRIBUTE

	SELF_, SELFSTR_, MATRIKS_, MATRIKSSTR_,
	STOPWATCH_,

/* The following symbols can be followed by "-" only if they are a variable. */

	/* Functions of 1 variable; if you add, update the #defines. */
	#define LOW_FUNCTION_1  ABS_
		ABS_, ROUND_, FLOOR_, CEILING_, SQRT_, SIN_, COS_, TAN_, ARCSIN_, ARCCOS_, ARCTAN_,
		EXP_, SINH_, COSH_, TANH_, ARCSINH_, ARCCOSH_, ARCTANH_,
		SIGMOID_, ERF_, ERFC_, GAUSS_P_, GAUSS_Q_, INV_GAUSS_Q_,
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
		SOUND_PRESSURE_TO_PHON_,
	#define HIGH_FUNCTION_2  SOUND_PRESSURE_TO_PHON_

	/* Functions of 3 variables; if you add, update the #defines. */
	#define LOW_FUNCTION_3  FISHER_P_
		FISHER_P_, FISHER_Q_, INV_FISHER_Q_,
		BINOMIAL_P_, BINOMIAL_Q_, INV_BINOMIAL_P_, INV_BINOMIAL_Q_,
	#define HIGH_FUNCTION_3  INV_BINOMIAL_Q_

	/* Functions of a variable number of variables; if you add, update the #defines. */
	#define LOW_FUNCTION_N  MIN_
		MIN_, MAX_, IMIN_, IMAX_,
	#define HIGH_FUNCTION_N  IMAX_

	/* String functions. */
	#define LOW_STRING_FUNCTION  LOW_FUNCTION_STRNUM
	#define LOW_FUNCTION_STRNUM  LENGTH_
		LENGTH_, FILE_READABLE_,
	#define HIGH_FUNCTION_STRNUM  FILE_READABLE_
		DATESTR_,
		LEFTSTR_, RIGHTSTR_, MIDSTR_, ENVIRONMENTSTR_, INDEX_, RINDEX_,
		STARTS_WITH_, ENDS_WITH_, INDEX_REGEX_, RINDEX_REGEX_,
		EXTRACT_NUMBER_, EXTRACT_WORDSTR_, EXTRACT_LINESTR_,
		SELECTED_, SELECTEDSTR_, NUMBER_OF_SELECTED_,
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
	SELECTED2_, SELECTEDSTR2_,
	END_
	#define hoogsteSymbool END_
};

/* The names that start with an underscore (_) do not occur in the formula text: */
/* they are used in error messages and in debugging (see Formula_print). */

static char *Formula_instructionNames [1 + hoogsteSymbool] = { "",
	"if", "then", "else", "(", "[", ",",
	"or", "and", "not", "=", "<>", "<=", "<", ">=", ">",
	"+", "-", "*", "/", "div", "mod", "^", "_neg",
	"endif", "fi", ")", "]",
	"_number", "pi", "e", "undefined",
	"xmin", "xmax", "ymin", "ymax", "nx", "ny", "dx", "dy",
	"row", "col", "nrow", "ncol", "y", "x",
	"self", "self$", "_matriks", "_matriks$",
	"stopwatch",
	"abs", "round", "floor", "ceiling", "sqrt", "sin", "cos", "tan", "arcsin", "arccos", "arctan",
	"exp", "sinh", "cosh", "tanh", "arcsinh", "arccosh", "arctanh",
	"sigmoid", "erf", "erfc", "gaussP", "gaussQ", "invGaussQ",
	"randomPoisson", "log2", "ln", "log10", "lnGamma",
	"hertzToBark", "barkToHertz", "phonToDifferenceLimens", "differenceLimensToPhon",
	"hertzToMel", "melToHertz", "hertzToSemitones", "semitonesToHertz",
	"erb", "hertzToErb", "erbToHertz",
	"arctan2", "randomUniform", "randomInteger", "randomGauss",
	"chiSquareP", "chiSquareQ", "invChiSquareQ", "studentP", "studentQ", "invStudentQ",
	"beta", "besselI", "besselK",
	"soundPressureToPhon",
	"fisherP", "fisherQ", "invFisherQ",
	"binomialP", "binomialQ", "invBinomialP", "invBinomialQ",
	"min", "max", "imin", "imax",
	"length", "fileReadable",
	"date$",
	"left$", "right$", "mid$", "environment$", "index", "rindex",
	"startsWith", "endsWith", "index_regex", "rindex_regex",
	"extractNumber", "extractWord$", "extractLine$",
	"selected", "selected$", "numberOfSelected",
	"fixed$", "percent$",
	".",
	"_true", "_false", "_iftrue", "_iffalse", "_gaNaar", "_label",
	"_self0", "_self0$", "_selfmatriks1", "_selfmatriks1$", "_selfmatriks2", "_selfmatriks2$",
	"_selffunktie1", "_selffunktie1$", "_selffunktie2", "_selffunktie2$",
	"_matriks0", "_matriks0$", "_matriks1", "_matriks1$", "_matriks2", "_matriks2$",
	"_funktie0", "_funktie0$", "_funktie1", "_funktie1$", "_funktie2", "_funktie2$",
	"_square",
	"_string",
	"_numericVariable", "_stringVariable",
	"_selected2", "_selected2$",
	"_end"
};

#define nieuwlabel (-- ilabel)
#define nieuwlees (lexan [++ ilexan]. symbol)
#define oudlees  (-- ilexan)

static int formulefout (char *message, int position) {
	return Melder_error ("%s:\n\\>> %.*s", message, position + 1, theExpression);
}

static int Formula_lexan (void) {
/*
	Purpose:
		translate the formula text into a series of symbols.
	Return:
		0 in case of error, otherwise 1.
	Postcondition:
		if result != 0, then the last symbol is "END_".
	Example:
		the text "x*7" yields 5 symbols:
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

	static char stok [30000];   /* String to collect a symbol name in. */
	int istok;   /* Length of "stok". */
#define stokaan istok = 0
#define stokkar { stok [istok ++] = kar; nieuwkar; }
#define stokuit stok [istok] = '\0'

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
					formulefout ("Missing exponent", ikar);
					return 0;
				}
				do stokkar while (kar >= '0' && kar <= '9');
			}
			if (kar == '%') stokkar
			stokuit;
			oudkar;
			nieuwtok (NUMBER_)
			tokgetal (Melder_atof (stok));
		} else if (kar >= 'a' && kar <= 'z' || kar == '.' && theExpression [ikar + 1] >= 'a' && theExpression [ikar + 1] <= 'z'
				&& (itok == 0 || lexan [itok]. symbol != MATRIKS_ && lexan [itok]. symbol != MATRIKSSTR_)) {
			int tok, endsInDollarSign = FALSE;
			stokaan;
			do stokkar while (kar >= 'A' && kar <= 'Z' || kar >= 'a' && kar <= 'z' || kar >= '0' && kar <= '9' || kar == '_' || kar == '.');
			if (kar == '$') { stokkar endsInDollarSign = TRUE; }
			stokuit;
			oudkar;
			/*
			 * 'stok' now contains a word, possibly ending in a dollar sign;
			 * it could be a variable name or a function name, or both!
			 * Try a language or function name first.
			 */
			tok = 1;
			while (! strequ (stok, Formula_instructionNames [tok]) && tok <= hoogsteInvoersymbool) tok++;
			if (tok <= hoogsteInvoersymbool) {
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
						InterpreterVariable var = Interpreter_hasVariable (theInterpreter, stok);
						if (var == NULL) return formulefout ("Unknown variable, or function with missing arguments", ikar);
						if (endsInDollarSign) nieuwtok (STRING_VARIABLE_) else nieuwtok (NUMERIC_VARIABLE_)
						lexan [itok]. content.variable = var;
					} else {
						return formulefout ("Function with missing arguments", ikar);
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
						if (theInterpreter && Interpreter_hasVariable (theInterpreter, stok))
							return Melder_error ("\\<<%s\\>> is ambiguous: a variable or an attribute of the current object. "
								"Change variable name.", stok);
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
						InterpreterVariable var = Interpreter_hasVariable (theInterpreter, stok);
						if (var == NULL) return Melder_error ("Unknown variable \\<<%s\\>> in formula (no \"current object\" here).", stok);
						if (endsInDollarSign) nieuwtok (STRING_VARIABLE_) else nieuwtok (NUMERIC_VARIABLE_)
						lexan [itok]. content.variable = var;
					} else {
						return Melder_error ("Unknown token \\<<%s\\>> in formula (no variables or current objects here).", stok);
					}
				} else {
					nieuwtok (tok)   /* This must be a language name. */
				}
			} else if (theInterpreter) {
				InterpreterVariable var = Interpreter_hasVariable (theInterpreter, stok);
				if (var == NULL) {
					int jkar;
					jkar = ikar + 1;
					while (theExpression [jkar] == ' ' || theExpression [jkar] == '\t') jkar ++;
					if (theExpression [jkar] == '(') {
						return Melder_error ("Unknown function \\<<%s\\>> in formula", stok);
					} else {
						return Melder_error ("Unknown variable \\<<%s\\>> in formula", stok);
					}
				}
				if (endsInDollarSign) nieuwtok (STRING_VARIABLE_) else nieuwtok (NUMERIC_VARIABLE_)
				lexan [itok]. content.variable = var;
			} else {
				return Melder_error ("Unknown function or attribute \\<<%s\\>> in formula.", stok);
			}
		} else if (kar >= 'A' && kar <= 'Z') {
			int endsInDollarSign = FALSE;
			char *underscore;
			stokaan;
			do stokkar while (isalnum (kar) || kar == '_');
			if (kar == '$') { stokkar endsInDollarSign = TRUE; }
			stokuit;
			oudkar;
			/*
			 * 'stok' now contains a word that could be an object name.
			 */
			underscore = strchr (stok, '_');
			if (strequ (stok, "Self")) {
				if (theSource == NULL) {
					formulefout ("Cannot use \"Self\" if there is no current object.", ikar);
					return 0;
				}
				nieuwtok (MATRIKS_)
				tokmatriks (theSource);
			} else if (strequ (stok, "Self$")) {
				if (theSource == NULL) {
					formulefout ("Cannot use \"Self$\" if there is no current object.", ikar);
					return 0;
				}
				nieuwtok (MATRIKSSTR_)
				tokmatriks (theSource);
			} else if (underscore == NULL) {
				return Melder_error ("Unknown symbol \\<<%s\\>> in formula "
					"(variables start with lower case; object names contain an underscore).", stok);
			} else if (strnequ (stok, "Object_", 7)) {
				long uniqueID = atol (stok + 7);
				int i = praat.n;
				while (i > 0 && uniqueID != praat.list [i]. id)
					i --;
				if (i == 0) {
					formulefout ("No such object (note: variables start with lower case)", ikar);
					return 0;
				}
				nieuwtok (endsInDollarSign ? MATRIKSSTR_ : MATRIKS_)
				tokmatriks (praat.list [i]. object);
			} else {
				int i = praat.n;
				*underscore = ' ';
				if (endsInDollarSign) stok [istok - 1] = '\0';
				while (i > 0 && ! strequ (stok, praat.list [i]. name))
					i --;
				if (i == 0) {
					formulefout ("No such object (note: variables start with lower case)", ikar);
					return 0;
				}
				nieuwtok (endsInDollarSign ? MATRIKSSTR_ : MATRIKS_)
				tokmatriks (praat.list [i]. object);
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
					formulefout ("No closing quote in string constant", ikar);
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
			lexan [itok]. content.string = Melder_strdup (stok);
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
			formulefout ("Unknown symbol", ikar);
			return 0;
		}
	} while (lexan [itok]. symbol != END_);
	return 1;
}

static int pas (int symbol) {
	if (symbol == nieuwlees)
		return 1;
	else {
		char melding [100];
		sprintf (melding, "Expected \"%s\", but found \"%s\"",
			Formula_instructionNames [symbol], Formula_instructionNames [lexan [ilexan]. symbol]);
		return formulefout (melding, lexan [ilexan]. position);
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
			nieuwontleed (NUMBER_);
			switch (nieuwlees) {
				case XMIN_:
					if (your getXmin == NULL) {
						formulefout ("Attribute \"xmin\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getXmin (thee));
						return 1;
					}
				case XMAX_:
					if (your getXmax == NULL) {
						formulefout ("Attribute \"xmax\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getXmax (thee));
						return 1;
					}
				case YMIN_:
					if (your getYmin == NULL) {
						formulefout ("Attribute \"ymin\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getYmin (thee));
						return 1;
					}
				case YMAX_:
					if (your getYmax == NULL) {
						formulefout ("Attribute \"ymax\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getYmax (thee));
						return 1;
					}
				case NX_:
					if (your getNx == NULL) {
						formulefout ("Attribute \"nx\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getNx (thee));
						return 1;
					}
				case NY_:
					if (your getNy == NULL) {
						formulefout ("Attribute \"ny\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getNy (thee));
						return 1;
					}
				case DX_:
					if (your getDx == NULL) {
						formulefout ("Attribute \"dx\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getDx (thee));
						return 1;
					}
				case DY_:
					if (your getDy == NULL) {
						formulefout ("Attribute \"dy\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getDy (thee));
						return 1;
					}
				case NCOL_:
					if (your getNcol == NULL) {
						formulefout ("Attribute \"ncol\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getNcol (thee));
						return 1;
					}
				case NROW_:
					if (your getNrow == NULL) {
						formulefout ("Attribute \"nrow\" not defined for this object", lexan [ilexan]. position);
						return 0;
					} else {
						parsenumber (your getNrow (thee));
						return 1;
					}
				default: formulefout ("Unknown attribute.", lexan [ilexan]. position); return 0;
			}
		} else {
			formulefout ("After a name of a matrix there should be \"(\", \"[\", or \".\"", lexan [ilexan]. position);
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
			formulefout ("After a name of a matrix$ there should be \"[\"", lexan [ilexan]. position);
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
		int n = 1;
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseExpression ()) return 0;
		while (nieuwlees == KOMMA_) {
			if (! parseExpression ()) return 0;
			n ++;
		}
		oudlees;
		if (! pas (HAAKJESLUITEN_)) return 0;
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
		} else if (symbol == SELECTED_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				symbol = SELECTED2_;
			} else oudlees;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == NUMBER_OF_SELECTED_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == DATESTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == LEFTSTR_ || symbol == RIGHTSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == MIDSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
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
		} else if (symbol == SELECTEDSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseExpression ()) return 0;
				symbol = SELECTEDSTR2_;
			} else oudlees;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == FIXEDSTR_ || symbol == PERCENTSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else {
			formulefout ("Function expected", lexan [ilexan]. position);
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

	formulefout ("Symbol misplaced", lexan [ilexan]. position);
	oudlees;   /* Needed for retry if we are going to be in a string comparison. */
	return 0;
}

static int parseFactor (void);

static int parsePowerFactors (void) {
	if (nieuwlees == POWER_) {
		if (ilexan > 2 && lexan [ilexan - 2]. symbol == MINUS_ && lexan [ilexan - 1]. symbol == NUMBER_) {
			formulefout ("Expressions like -3^4 are ambiguous; use (-3)^4 or -(3^4) or -(3)^4", lexan [ilexan]. position);
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
	int saveLexan = ilexan, saveParse = iparse, symbol;
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

			if (parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_
				 ||
				 parse [i]. symbol == FALSE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_)
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

			if (parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFFALSE_
				 ||
				 parse [i]. symbol == FALSE_ &&
				 parse [i + 1]. symbol == GOTO_ &&
				 parse [volg = vindLabel (parse [i + 1]. content.label) + 1]
							. symbol == IFTRUE_)
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

			if (parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == IFFALSE_
				 ||
				 parse [i]. symbol == FALSE_ &&
				 parse [i + 1]. symbol == IFTRUE_)
			{
				verbeterd = 1;
				schuif (i, 2);
			}

/* Optimalisatie 7: */
/*    true   iftrue x   ->  goto x    */
/*    false  iffalse x  ->  goto x    */
			
			if (parse [i]. symbol == TRUE_ &&
				 parse [i + 1]. symbol == IFTRUE_
				 ||
				 parse [i]. symbol == FALSE_ &&
				 parse [i + 1]. symbol == IFFALSE_)
			{
				verbeterd = 1;
				parse [i]. symbol = GOTO_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
			}

/* Optimalisatie 8: */
/*    iftrue x   ->  iftrue y   /  __  ...  label x  goto y   */
/*    iffalse x  ->  iffalse y  /  __  ...  label x  goto y   */

			if ((parse [i]. symbol == IFTRUE_ ||
				  parse [i]. symbol == IFFALSE_) &&
				 parse [volg = vindLabel (parse [i]. content.label) + 1]
							. symbol == GOTO_)
			{
				verbeterd = 1;
				parse [i]. content.label = parse [volg]. content.label;
			}

/* Optimalisatie 9a: */
/*    not  iftrue x  ->  iffalse x   */

			if (parse [i]. symbol == NOT_ &&
				 parse [i + 1]. symbol == IFTRUE_)
			{
				verbeterd = 1;
				parse [i]. symbol = IFFALSE_;
				parse [i]. content.label = parse [i + 1]. content.label;
				schuif (i + 1, 1);
			}

/* Optimalisatie 9b: */
/*    not  iffalse x  ->  iftrue x   */

			if (parse [i]. symbol == NOT_ &&
				 parse [i + 1]. symbol == IFFALSE_)
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
					if ((parse [j]. symbol == GOTO_ ||
						  parse [j]. symbol == IFFALSE_ ||
						  parse [j]. symbol == IFTRUE_) &&
						  parse [i]. content.label == parse [j]. content.label)
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
		char *instructionName;
		symbol = f [++ i]. symbol;
		instructionName = Formula_instructionNames [symbol];
		if (symbol == NUMBER_)
			Melder_casual ("%d %s %.17g", i, instructionName, f [i]. content.number);
		else if (symbol == GOTO_ || symbol == IFFALSE_ || symbol == IFTRUE_ || symbol == LABEL_)
			Melder_casual ("%d %s %d", i, instructionName, f [i]. content.label);
		else if (symbol == NUMERIC_VARIABLE_)
			Melder_casual ("%d %s %s %s", i, instructionName, f [i]. content.variable -> key, Melder_double (f [i]. content.variable -> numericValue));
		else if (symbol == STRING_VARIABLE_)
			Melder_casual ("%d %s %s %s", i, instructionName, f [i]. content.variable -> key, f [i]. content.variable -> stringValue);
		else if (symbol == STRING_)
			Melder_casual ("%d %s \"%s\"", i, instructionName, f [i]. content.string);
		else if (symbol == MATRIKS_ || symbol == MATRIKSSTR_)
			Melder_casual ("%d %s %ld %s", i, instructionName,
				Thing_className (f [i]. content.object), ((Thing) f [i]. content.object) -> name);
		else
			Melder_casual ("%d %s", i, instructionName);
	} while (symbol != END_);
}

int Formula_compile (Any interpreter, Any data, const char *expression, int expressionType, int optimize) {
	theInterpreter = interpreter ? interpreter : UiInterpreter_get ();
	theSource = data;
	theExpression = expression;
	theExpressionType = expressionType;
	theOptimize = optimize;
	if (! lexan) {
		lexan = Melder_calloc (3000, sizeof (struct FormulaInstruction));
		lexan [3000 - 1]. symbol = END_;   /* Make sure that string cleaning always terminates. */
	}
	if (! parse) parse = Melder_calloc (3000, sizeof (struct FormulaInstruction));
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
		char *string;
	} content;
} Stackel;

static void Stackel_cleanUp (Stackel *me) {
	if (my which == Stackel_STRING) {
		Melder_free (my content.string);
	}
}
static Stackel *theStack;
static int w, wmax;   /* w = stack pointer; */
#define pop  theStack [w --]
static void pushNumber (double x) {
	Stackel *stackel = & theStack [++ w];
	if (stackel -> which > 0) Stackel_cleanUp (stackel); if (w > wmax) wmax ++;
	stackel -> which = Stackel_NUMBER; stackel -> content.number = x;
}
static void pushString (char *x) {
	Stackel *stackel = & theStack [++ w];
	if (stackel -> which > 0) Stackel_cleanUp (stackel); if (w > wmax) wmax ++;
	stackel -> which = Stackel_STRING; stackel -> content.string = x;
}
static char *Stackel_whichText (Stackel *me) {
	return
		my which == Stackel_NUMBER ? "a number" :
		my which == Stackel_STRING ? "a string" :
		"???";
}

static void do_not (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : x.content.number == 0.0 ? 1.0 : 0.0);
	} else {
		Melder_error ("Cannot negate (\"not\") %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_eq (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == y.content.number ? 1.0 : 0.0);   /* Even if undefined. */
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		double result = strequ (x.content.string, y.content.string) ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_error ("Cannot compare (=) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_ne (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number != y.content.number ? 1.0 : 0.0);   /* Even if undefined. */
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		double result = strequ (x.content.string, y.content.string) ? 0.0 : 1.0;
		pushNumber (result);
	} else {
		Melder_error ("Cannot compare (<>) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_le (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number <= y.content.number ? 1.0 : 0.0);
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		double result = strcmp (x.content.string, y.content.string) <= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_error ("Cannot compare (<=) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_lt (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number < y.content.number ? 1.0 : 0.0);
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		double result = strcmp (x.content.string, y.content.string) < 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_error ("Cannot compare (<) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_ge (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number >= y.content.number ? 1.0 : 0.0);
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		double result = strcmp (x.content.string, y.content.string) >= 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_error ("Cannot compare (>=) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_gt (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number > y.content.number ? 1.0 : 0.0);
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		double result = strcmp (x.content.string, y.content.string) > 0 ? 1.0 : 0.0;
		pushNumber (result);
	} else {
		Melder_error ("Cannot compare (>) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_add (void) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number + y.content.number);
	} else if (x.which == Stackel_STRING && y.which == Stackel_STRING) {
		long length1 = strlen (x.content.string), length2 = strlen (y.content.string);
		char *result = Melder_malloc (length1 + length2 + 1); cherror
		strcpy (result, x.content.string);
		strcpy (result + length1, y.content.string);
		pushString (result);
	} else {
		Melder_error ("Cannot add (+) %s to %s.", Stackel_whichText (& y), Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_sub (void) {
	Stackel y = pop, x = pop;
	if (x. which == Stackel_NUMBER && y. which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number - y.content.number);
	} else if (x. which == Stackel_STRING && y. which == Stackel_STRING) {
		long length1 = strlen (x.content.string), length2 = strlen (y.content.string), newlength = length1 - length2;
		char *result;
		if (newlength >= 0 && strnequ (x.content.string + newlength, y.content.string, length2)) {
			result = Melder_malloc (newlength + 1); cherror
			strncpy (result, x.content.string, newlength);
			result [newlength] = '\0';
		} else {
			result = Melder_strdup (x.content.string); cherror
		}
		pushString (result);
	} else {
		Melder_error ("Cannot subtract (-) %s from %s.", Stackel_whichText (& y), Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_mul (void) {
	Stackel y = pop, x = pop;
	if (x. which == Stackel_NUMBER && y. which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			x.content.number * y.content.number);
	} else {
		Melder_error ("Cannot multiply (*) %s by %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_rdiv (void) {
	Stackel y = pop, x = pop;
	if (x. which == Stackel_NUMBER && y. which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			y.content.number == 0.0 ? NUMundefined :
			x.content.number / y.content.number);
	} else {
		Melder_error ("Cannot divide (/) %s by %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_idiv (void) {
	Stackel y = pop, x = pop;
	if (x. which == Stackel_NUMBER && y. which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			y.content.number == 0.0 ? NUMundefined :
			floor (x.content.number / y.content.number));
	} else {
		Melder_error ("Cannot divide (\"div\") %s by %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_mod (void) {
	Stackel y = pop, x = pop;
	if (x. which == Stackel_NUMBER && y. which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			y.content.number == 0.0 ? NUMundefined :
			x.content.number - floor (x.content.number / y.content.number) * y.content.number);
	} else {
		Melder_error ("Cannot divide (\"mod\") %s by %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_minus (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : - x.content.number);
	} else {
		Melder_error ("Cannot take the opposite (-) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_power (void) {
	Stackel y = pop, x = pop;
	if (x. which == Stackel_NUMBER && y. which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			pow (x.content.number, y.content.number));
	} else {
		Melder_error ("Cannot exponentiate (^) %s to %s.", Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_sqr (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : x.content.number * x.content.number);
	} else {
		Melder_error ("Cannot take the square (^ 2) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_function_n_n (double (*f) (double)) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : f (x.content.number));
	} else {
		Melder_error ("The function %s requires a numeric argument, not %s.",
			Formula_instructionNames [parse [programPointer]. symbol], Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_abs (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : fabs (x.content.number));
	} else {
		Melder_error ("Cannot take the absolute value (abs) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_round (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : floor (x.content.number + 0.5));
	} else {
		Melder_error ("Cannot round %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_floor (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : floor (x.content.number));
	} else {
		Melder_error ("Cannot round down (floor) %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_ceiling (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : ceil (x.content.number));
	} else {
		Melder_error ("Cannot round up (ceiling) %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_sqrt (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined :
			x.content.number < 0.0 ? NUMundefined : sqrt (x.content.number));
	} else {
		Melder_error ("Cannot take the square root (sqrt) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_sin (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : sin (x.content.number));
	} else {
		Melder_error ("Cannot take the sine (sin) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_cos (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : cos (x.content.number));
	} else {
		Melder_error ("Cannot take the cosine (cos) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_tan (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : tan (x.content.number));
	} else {
		Melder_error ("Cannot take the tangent (tan) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_arcsin (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined :
			fabs (x.content.number) > 1.0 ? NUMundefined : asin (x.content.number));
	} else {
		Melder_error ("Cannot take the arcsine (arcsin) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_arccos (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined :
			fabs (x.content.number) > 1.0 ? NUMundefined : acos (x.content.number));
	} else {
		Melder_error ("Cannot take the arccosine (arccos) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_arctan (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : atan (x.content.number));
	} else {
		Melder_error ("Cannot take the arctangent (arctan) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_exp (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : exp (x.content.number));
	} else {
		Melder_error ("Cannot exponentiate (exp) %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_sinh (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : sinh (x.content.number));
	} else {
		Melder_error ("Cannot take the hyperbolic sine (sinh) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_cosh (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : cosh (x.content.number));
	} else {
		Melder_error ("Cannot take the hyperbolic cosine (cosh) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_tanh (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined : tanh (x.content.number));
	} else {
		Melder_error ("Cannot take the hyperbolic tangent (tanh) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_log2 (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined :
			x.content.number <= 0.0 ? NUMundefined : log (x.content.number) * NUMlog2e);
	} else {
		Melder_error ("Cannot take the base-2 logarithm (log2) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_ln (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined :
			x.content.number <= 0.0 ? NUMundefined : log (x.content.number));
	} else {
		Melder_error ("Cannot take the natural logarithm (ln) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_log10 (void) {
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined ? NUMundefined :
			x.content.number <= 0.0 ? NUMundefined : log10 (x.content.number));
	} else {
		Melder_error ("Cannot take the base-10 logarithm (log10) of %s.", Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_function_dd_d (double (*f) (double, double)) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			f (x.content.number, y.content.number));
	} else {
		Melder_error ("The function %s requires two numeric arguments, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_function_dl_d (double (*f) (double, long)) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			f (x.content.number, y.content.number));
	} else {
		Melder_error ("The function %s requires two numeric arguments, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_function_ld_d (double (*f) (long, double)) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			f (x.content.number, y.content.number));
	} else {
		Melder_error ("The function %s requires two numeric arguments, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_function_ll_l (long (*f) (long, long)) {
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined ? NUMundefined :
			f (x.content.number, y.content.number));
	} else {
		Melder_error ("The function %s requires two numeric arguments, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& x), Stackel_whichText (& y)); goto end;
	}
end: return;
}
static void do_function_ddd_d (double (*f) (double, double, double)) {
	Stackel z = pop, y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER && z.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined || z.content.number == NUMundefined ? NUMundefined :
			f (x.content.number, y.content.number, z.content.number));
	} else {
		Melder_error ("The function %s requires three numeric arguments, not %s, %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& x), Stackel_whichText (& y), Stackel_whichText (& z)); goto end;
	}
end: return;
}
static void do_function_dll_d (double (*f) (double, long, long)) {
	Stackel z = pop, y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER && z.which == Stackel_NUMBER) {
		pushNumber (x.content.number == NUMundefined || y.content.number == NUMundefined || z.content.number == NUMundefined ? NUMundefined :
			f (x.content.number, y.content.number, z.content.number));
	} else {
		Melder_error ("The function %s requires three numeric arguments, not %s, %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& x), Stackel_whichText (& y), Stackel_whichText (& z)); goto end;
	}
end: return;
}
static void do_min (void) {
	Stackel n = pop, last;
	double result;
	int j;
	Melder_assert (n.which == Stackel_NUMBER);
	if (n.content.number < 1) {
		Melder_error ("The function \"min\" requires at least one argument."); goto end;
	}
	last = pop;
	if (last.which != Stackel_NUMBER) {
		Melder_error ("The function \"min\" can only have numeric arguments, not %s.", Stackel_whichText (& last)); goto end;
	}
	result = last.content.number;
	for (j = n.content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous.which != Stackel_NUMBER) {
			Melder_error ("The function \"min\" can only have numeric arguments, not %s.", Stackel_whichText (& previous)); goto end;
		}
		result = result == NUMundefined || previous.content.number == NUMundefined ? NUMundefined :
			result < previous.content.number ? result : previous.content.number;
	}
	pushNumber (result);
end: return;
}
static void do_max (void) {
	Stackel n = pop, last;
	double result;
	int j;
	Melder_assert (n.which == Stackel_NUMBER);
	if (n.content.number < 1) {
		Melder_error ("The function \"max\" requires at least one argument."); goto end;
	}
	last = pop;
	if (last.which != Stackel_NUMBER) {
		Melder_error ("The function \"max\" can only have numeric arguments, not %s.", Stackel_whichText (& last)); goto end;
	}
	result = last.content.number;
	for (j = n.content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous.which != Stackel_NUMBER) {
			Melder_error ("The function \"max\" can only have numeric arguments, not %s.", Stackel_whichText (& previous)); goto end;
		}
		result = result == NUMundefined || previous.content.number == NUMundefined ? NUMundefined :
			result > previous.content.number ? result : previous.content.number;
	}
	pushNumber (result);
end: return;
}
static void do_imin (void) {
	Stackel n = pop, last;
	double minimum, result;
	int j;
	Melder_assert (n.which == Stackel_NUMBER);
	if (n.content.number < 1) {
		Melder_error ("The function \"imin\" requires at least one argument."); goto end;
	}
	last = pop;
	if (last.which != Stackel_NUMBER) {
		Melder_error ("The function \"imin\" can only have numeric arguments, not %s.", Stackel_whichText (& last)); goto end;
	}
	minimum = last.content.number;
	result = n.content.number;
	for (j = n.content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous.which != Stackel_NUMBER) {
			Melder_error ("The function \"imin\" can only have numeric arguments, not %s.", Stackel_whichText (& previous)); goto end;
		}
		if (minimum == NUMundefined || previous.content.number == NUMundefined) {
			minimum = NUMundefined;
			result = NUMundefined;
		} else if (previous.content.number < minimum) {
			minimum = previous.content.number;
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
	Melder_assert (n.which == Stackel_NUMBER);
	if (n.content.number < 1) {
		Melder_error ("The function \"imax\" requires at least one argument."); goto end;
	}
	last = pop;
	if (last.which != Stackel_NUMBER) {
		Melder_error ("The function \"imax\" can only have numeric arguments, not %s.", Stackel_whichText (& last)); goto end;
	}
	maximum = last.content.number;
	result = n.content.number;
	for (j = n.content.number - 1; j > 0; j --) {
		Stackel previous = pop;
		if (previous.which != Stackel_NUMBER) {
			Melder_error ("The function \"imax\" can only have numeric arguments, not %s.", Stackel_whichText (& previous)); goto end;
		}
		if (maximum == NUMundefined || previous.content.number == NUMundefined) {
			maximum = NUMundefined;
			result = NUMundefined;
		} else if (previous.content.number > maximum) {
			maximum = previous.content.number;
			result = j;
		}
	}
	pushNumber (result);
end: return;
}
static void do_length (void) {
	Stackel s = pop;
	if (s.which == Stackel_STRING) {
		double result = strlen (s.content.string);
		pushNumber (result);
	} else {
		Melder_error ("The function \"length\" requires a string, not %s.", Stackel_whichText (& s)); goto end;
	}
end: return;
}
static void do_fileReadable (void) {
	Stackel s = pop;
	if (s.which == Stackel_STRING) {
		structMelderFile file;
		Melder_relativePathToFile (s.content.string, & file); cherror
		pushNumber (MelderFile_readable (& file));
	} else {
		Melder_error ("The function \"fileReadable\" requires a string, not %s.", Stackel_whichText (& s)); goto end;
	}
end: return;
}
static void do_dateStr (void) {
	time_t today = time (NULL);
	char *date, *newline;
	date = Melder_strdup (ctime (& today)); cherror
	newline = strchr (date, '\n');
	if (newline) *newline = '\0';
	pushString (date);
end: return;
}
static void do_leftStr (void) {
	Stackel x = pop, s = pop;
	if (s.which == Stackel_STRING && x.which == Stackel_NUMBER) {
		long newlength = x.content.number;
		char *result;
		long length = strlen (s.content.string);
		if (newlength < 0) newlength = 0;
		if (newlength > length) newlength = length;
		result = Melder_malloc (newlength + 1); cherror
		strncpy (result, s.content.string, newlength);
		result [newlength] = '\0';
		pushString (result);
	} else {
		Melder_error ("The function \"left$\" requires a string and a number."); goto end;
	}
end: return;
}
static void do_rightStr (void) {
	Stackel x = pop, s = pop;
	if (s.which == Stackel_STRING && x.which == Stackel_NUMBER) {
		long newlength = x.content.number;
		char *result;
		long length = strlen (s.content.string);
		if (newlength < 0) newlength = 0;
		if (newlength > length) newlength = length;
		result = Melder_strdup (s.content.string + length - newlength); cherror
		pushString (result);
	} else {
		Melder_error ("The function \"right$\" requires a string and a number."); goto end;
	}
end: return;
}
static void do_midStr (void) {
	Stackel y = pop, x = pop, s = pop;
	if (s.which == Stackel_STRING && x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		long newlength = y.content.number;
		long start = x.content.number;
		long length = strlen (s.content.string), finish = start + newlength - 1;
		char *result;
		if (start < 1) start = 1;
		if (finish > length) finish = length;
		newlength = finish - start + 1;
		if (newlength > 0) {
			result = Melder_malloc (newlength + 1); cherror
			strncpy (result, s.content.string + start - 1, newlength);
			result [newlength] = '\0';
		} else {
			result = Melder_strdup (""); cherror
		}
		pushString (result);
	} else {
		Melder_error ("The function \"mid$\" requires a string and two numbers."); goto end;
	}
end: return;
}
static void do_environmentStr (void) {
	Stackel s = pop;
	if (s.which == Stackel_STRING) {
		char *value = Melder_getenv (s.content.string);
		char *result = Melder_strdup (value != NULL ? value : ""); cherror
		pushString (result);
	} else {
		Melder_error ("The function \"environment$\" requires a string, not %s.", Stackel_whichText (& s)); goto end;
	}
end: return;
}
static void do_index (void) {
	Stackel t = pop, s = pop;
	if (s.which == Stackel_STRING && t.which == Stackel_STRING) {
		char *substring = strstr (s.content.string, t.content.string);
		long result = substring ? substring - s.content.string + 1 : 0;
		pushNumber (result);
	} else {
		Melder_error ("The function \"index\" requires two strings, not %s and %s.",
			Stackel_whichText (& s), Stackel_whichText (& t)); goto end;
	}
end: return;
}
static void do_rindex (void) {
	Stackel part = pop, whole = pop;
	if (whole.which == Stackel_STRING && part.which == Stackel_STRING) {
		char *lastSubstring = strstr (whole.content.string, part.content.string);
		if (part.content.string [0] == '\0') {
			long result = strlen (whole.content.string);
			pushNumber (result);
		} else if (lastSubstring) {
			for (;;) {
				char *substring = strstr (lastSubstring + 1, part.content.string);
				if (substring == NULL) break;
				lastSubstring = substring;
			}
			pushNumber (lastSubstring - whole.content.string + 1);
		} else {
			pushNumber (0);
		}
	} else {
		Melder_error ("The function \"rindex\" requires two strings, not %s and %s.",
			Stackel_whichText (& whole), Stackel_whichText (& part)); goto end;
	}
end: return;
}
static void do_stringMatchesCriterion (int criterion) {
	Stackel t = pop, s = pop;
	if (s.which == Stackel_STRING && t.which == Stackel_STRING) {
		int result = Melder_stringMatchesCriterion (s.content.string, criterion, t.content.string);
		pushNumber (result);
	} else {
		Melder_error ("The function \"%s\" requires two strings, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& s), Stackel_whichText (& t)); goto end;
	}
end: return;
}
static void do_index_regex (int backward) {
	Stackel t = pop, s = pop;
	if (s.which == Stackel_STRING && t.which == Stackel_STRING) {
		char *place = NULL, *errorMessage;
		regexp *compiled_regexp = CompileRE (t.content.string, & errorMessage, 0);
		if (compiled_regexp == NULL) {
			pushNumber (NUMundefined);
		} else if (ExecRE (compiled_regexp, NULL, s.content.string, NULL, backward, '\0', '\0', NULL)) {
			char *place = compiled_regexp -> startp [0];
			free (compiled_regexp);
			pushNumber ((place - s.content.string) + 1);
		} else {
			pushNumber (FALSE);
		}
	} else {
		Melder_error ("The function \"%s\" requires two strings, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& s), Stackel_whichText (& t)); goto end;
	}
end: return;
}
static void do_extractNumber (void) {
	Stackel t = pop, s = pop;
	if (s.which == Stackel_STRING && t.which == Stackel_STRING) {
		char *substring = strstr (s.content.string, t.content.string);
		if (substring == NULL) {
			pushNumber (NUMundefined);
		} else {
			/* Skip the prompt. */
			substring += strlen (t.content.string);
			/* Skip white space. */
			while (*substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') substring ++;
			if (substring [0] == '\0' || strnequ (substring, "--undefined--", 13)) {
				pushNumber (NUMundefined);
			} else {
				char buffer [101], *slash;
				int i;
				for (i = 0; i < 100; i ++) {
					buffer [i] = *substring;
					substring ++;
					if (*substring == '\0' || *substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') break;
				}
				if (i >= 100) {
					buffer [100] = '\0';
					pushNumber (Melder_atof (buffer));
				} else {
					buffer [i + 1] = '\0';
					slash = strchr (buffer, '/');
					if (slash) {
						double numerator, denominator;
						*slash = '\0';
						numerator = Melder_atof (buffer), denominator = Melder_atof (slash + 1);
						pushNumber (numerator / denominator);
					} else {
						pushNumber (Melder_atof (buffer));
					}
				}
			}
		}
	} else {
		Melder_error ("The function \"%s\" requires two strings, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& s), Stackel_whichText (& t)); goto end;
	}
end: return;
}
static void do_extractTextStr (int singleWord) {
	Stackel t = pop, s = pop;
	if (s.which == Stackel_STRING && t.which == Stackel_STRING) {
		char *substring = strstr (s.content.string, t.content.string), *result, *p;
		if (substring == NULL) {
			result = Melder_strdup (""); cherror
		} else {
			long length;
			/* Skip the prompt. */
			substring += strlen (t.content.string);
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
			result = Melder_malloc (length + 1); cherror
			strncpy (result, substring, length);
			result [length] = '\0';
		}
		pushString (result);
	} else {
		Melder_error ("The function \"%s\" requires two strings, not %s and %s.",
			Formula_instructionNames [parse [programPointer]. symbol],
			Stackel_whichText (& s), Stackel_whichText (& t)); goto end;
	}
end: return;
}
static void do_selected (void) {
	Stackel s = pop;
	if (s.which == Stackel_STRING) {
		long result;
		void *klas = Thing_classFromClassName (s.content.string); cherror
		result = praat_getIdOfSelected (klas, 0); cherror
		pushNumber (result);
	} else {
		Melder_error ("The function \"selected\" requires a string (an object type name), not %s.",
			Stackel_whichText (& s)); goto end;
	}
end: return;
}
static void do_selected2 (void) {
	Stackel x = pop, s = pop;
	if (s.which == Stackel_STRING && x.which == Stackel_NUMBER) {
		long result;
		void *klas = Thing_classFromClassName (s.content.string); cherror
		result = praat_getIdOfSelected (klas, x.content.number); cherror
		pushNumber (result);
	} else {
		Melder_error ("The function \"selected\" requires a string (an object type name) and a number, not %s and %s.",
			Stackel_whichText (& s), Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_selectedStr (void) {
	Stackel s = pop;
	if (s.which == Stackel_STRING) {
		char *name, *result;
		void *klas = Thing_classFromClassName (s.content.string); cherror
		name = praat_getNameOfSelected (klas, 0); cherror
		result = Melder_strdup (name); cherror
		pushString (result);
	} else {
		Melder_error ("The function \"selected$\" requires a string (an object type name), not %s.",
			Stackel_whichText (& s)); goto end;
	}
end: return;
}
static void do_selectedStr2 (void) {
	Stackel x = pop, s = pop;
	if (s.which == Stackel_STRING && x.which == Stackel_NUMBER) {
		char *name, *result;
		void *klas = Thing_classFromClassName (s.content.string); cherror
		name = praat_getNameOfSelected (klas, x.content.number); cherror
		result = Melder_strdup (name); cherror
		pushString (result);
	} else {
		Melder_error ("The function \"selected$\" requires a string (an object type name) and a number, not %s and %s.",
			Stackel_whichText (& s), Stackel_whichText (& x)); goto end;
	}
end: return;
}
static void do_numberOfSelected (void) {
	Stackel s = pop;
	if (s.which == Stackel_STRING) {
		long result;
		void *klas = Thing_classFromClassName (s.content.string); cherror
		result = praat_selection (klas);
		pushNumber (result);
	} else {
		Melder_error ("The function \"numberOfSelected\" requires a string (an object type name), not %s.",
			Stackel_whichText (& s)); goto end;
	}
end: return;
}
static void do_fixedStr (void) {
	Stackel precision = pop, value = pop;
	if (value.which == Stackel_NUMBER && precision.which == Stackel_NUMBER) {
		char *result = Melder_strdup (Melder_fixed (value.content.number, precision.content.number));
		pushString (result);
	} else {
		Melder_error ("The function \"fixed$\" requires two numbers, not %s and %s.",
			Stackel_whichText (& value), Stackel_whichText (& precision)); goto end;
	}
end: return;
}
static void do_percentStr (void) {
	Stackel precision = pop, value = pop;
	if (value.which == Stackel_NUMBER && precision.which == Stackel_NUMBER) {
		char *result = Melder_strdup (Melder_percent (value.content.number, precision.content.number));
		pushString (result);
	} else {
		Melder_error ("The function \"percent$\" requires two numbers, not %s and %s.",
			Stackel_whichText (& value), Stackel_whichText (& precision)); goto end;
	}
end: return;
}
static long Stackel_getRowNumber (Stackel *row, Data thee) {
	long result;
	if (row->which == Stackel_NUMBER) {
		result = floor (row->content.number + 0.5);   /* Round. */
	} else if (row->which == Stackel_STRING) {
		if (your getRowIndex == NULL)
			return Melder_error ("Objects of type %s do not have row labels, so row indexes have to be numbers.", Thing_className (thee));
		result = your getRowIndex (thee, row->content.string);
		if (result == 0)
			return Melder_error ("Object \"%s\" has no row labelled \"%s\".", thy name, row->content.string);
	} else {
		return Melder_error ("A row index should be a number or a string, not a %s.", Stackel_whichText (row));
	}
	return result;
}
static long Stackel_getColumnNumber (Stackel *column, Data thee) {
	long result;
	if (column->which == Stackel_NUMBER) {
		result = floor (column->content.number + 0.5);   /* Round. */
	} else if (column->which == Stackel_STRING) {
		if (your getColumnIndex == NULL)
			return Melder_error ("Objects of type %s do not have column labels, so column indexes have to be numbers.", Thing_className (thee));
		result = your getColumnIndex (thee, column->content.string);
		if (result == 0)
			return Melder_error ("Object \"%s\" has no column labelled \"%s\".", thy name, column->content.string);
	} else {
		return Melder_error ("A column index should be a number or a string, not a %s.", Stackel_whichText (column));
	}
	return result;
}
static void do_self0 (long irow, long icol) {
	Data me = theSource;
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	if (our getCell) {
		pushNumber (our getCell (me));
	} else if (our getVector) {
		if (icol == 0) {
			Melder_error ("We are not in a loop, "
				"hence no implicit column index for the current %s object (self).\n"
				"Try using the [column] index explicitly.", Thing_className (me));
			goto end;
		} else {
			pushNumber (our getVector (me, icol));
		}
	} else if (our getMatrix) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_error ("We are not in a loop over rows and columns,\n"
					"hence no implicit row and column indexing for the current %s object (self).\n"
					"Try using both [row, column] indexes explicitly.", Thing_className (me));
				goto end;
			} else {
				Melder_error ("We are not in a loop over columns only,\n"
					"hence no implicit row index for the current %s object (self).\n"
					"Try using the [row] index explicitly.", Thing_className (me));
				goto end;
			}
		} else {
			pushNumber (our getMatrix (me, irow, icol));
		}
	} else {
		Melder_error ("%s objects (like self) accept no [] indexing.", Thing_className (me));
		goto end;
	}
end: return;
}
static void do_matriks0 (long irow, long icol) {
	Data thee = parse [programPointer]. content.object;
	if (your getCell) {
		pushNumber (your getCell (thee));
	} else if (your getVector) {
		if (icol == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit column index for this %s object.\n"
				"Try using the [column] index explicitly.", Thing_className (thee));
			goto end;
		} else {
			pushNumber (your getVector (thee, icol));
		}
	} else if (your getMatrix) {
		if (irow == 0) {
			if (icol == 0) {
				Melder_error ("We are not in a loop over rows and columns,\n"
					"hence no implicit row and column indexing for this %s object.\n"
					"Try using both [row, column] indexes explicitly.", Thing_className (thee));
				goto end;
			} else {
				Melder_error ("We are not in a loop over columns only,\n"
					"hence no implicit row index for this %s object.\n"
					"Try using the [row] index explicitly.", Thing_className (thee));
				goto end;
			}
		} else {
			pushNumber (your getMatrix (thee, irow, icol));
		}
	} else {
		Melder_error ("%s objects accept no [] indexing.", Thing_className (thee));
		goto end;
	}
end: return;
}
static void do_selfMatriks1 (long irow) {
	Data me = theSource;
	Stackel column = pop;
	long icol;
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	icol = Stackel_getColumnNumber (& column, me); cherror
	if (our getVector) {
		pushNumber (our getVector (me, icol));
	} else if (our getMatrix) {
		if (irow == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit row index for the current %s object (self).\n"
				"Try using both [row, column] indexes instead.", Thing_className (me));
			goto end;
		} else {
			pushNumber (our getMatrix (me, irow, icol));
		}
	} else {
		Melder_error ("%s objects (like self) accept no [column] indexes.", Thing_className (me));
		goto end;
	}
end: return;
}
static void do_selfMatriksStr1 (long irow) {
	Data me = theSource;
	Stackel column = pop;
	long icol;
	if (me == NULL) { Melder_error ("The name \"self$\" is restricted to formulas for objects."); goto end; }
	icol = Stackel_getColumnNumber (& column, me); cherror
	if (our getVectorStr) {
		char *result = Melder_strdup (our getVectorStr (me, icol)); cherror
		pushString (result);
	} else if (our getMatrixStr) {
		if (irow == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit row index for the current %s object (self).\n"
				"Try using both [row, column] indexes instead.", Thing_className (me));
			goto end;
		} else {
			char *result = Melder_strdup (our getMatrixStr (me, irow, icol)); cherror
			pushString (result);
		}
	} else {
		Melder_error ("%s objects (like self) accept no [column] indexes.", Thing_className (me));
		goto end;
	}
end: return;
}
static void do_matriks1 (long irow) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop;
	long icol = Stackel_getColumnNumber (& column, thee); cherror
	if (your getVector) {
		pushNumber (your getVector (thee, icol));
	} else if (your getMatrix) {
		if (irow == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit row index for this %s object.\n"
				"Try using both [row, column] indexes instead.", Thing_className (thee));
			goto end;
		} else {
			pushNumber (your getMatrix (thee, irow, icol));
		}
	} else {
		Melder_error ("%s objects accept no [column] indexes.", Thing_className (thee));
		goto end;
	}
end: return;
}
static void do_matrixStr1 (long irow) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop;
	long icol = Stackel_getColumnNumber (& column, thee); cherror
	if (your getVectorStr) {
		pushString (Melder_strdup (your getVectorStr (thee, icol)));
	} else if (your getMatrixStr) {
		if (irow == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit row index for this %s object.\n"
				"Try using both [row, column] indexes instead.", Thing_className (thee));
			goto end;
		} else {
			pushString (Melder_strdup (your getMatrixStr (thee, irow, icol)));
		}
	} else {
		Melder_error ("%s objects accept no [column] indexes for string cells.", Thing_className (thee));
		goto end;
	}
end: return;
}
static void do_selfMatriks2 (void) {
	Data me = theSource;
	Stackel column = pop, row = pop;
	long irow, icol;
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	irow = Stackel_getRowNumber (& row, me); cherror
	icol = Stackel_getColumnNumber (& column, me); cherror
	if (our getMatrix == NULL) {
		Melder_error ("%s objects like \"self\" accept no [row, column] indexing.", Thing_className (me));
		goto end;
	}
	pushNumber (our getMatrix (me, irow, icol));
end: return;
}
static void do_selfMatriksStr2 (void) {
	Data me = theSource;
	Stackel column = pop, row = pop;
	long irow, icol;
	if (me == NULL) { Melder_error ("The name \"self$\" is restricted to formulas for objects."); goto end; }
	irow = Stackel_getRowNumber (& row, me); cherror
	icol = Stackel_getColumnNumber (& column, me); cherror
	if (our getMatrixStr == NULL) {
		Melder_error ("%s objects like \"self$\" accept no [row, column] indexing for string cells.", Thing_className (me));
		goto end;
	}
	pushString (Melder_strdup (our getMatrixStr (me, irow, icol)));
end: return;
}
static void do_matriks2 (void) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	long irow, icol;
	irow = Stackel_getRowNumber (& row, thee); cherror
	icol = Stackel_getColumnNumber (& column, thee); cherror
	if (your getMatrix == NULL) {
		Melder_error ("%s objects accept no [row, column] indexing.", Thing_className (thee));
		goto end;
	}
	pushNumber (your getMatrix (thee, irow, icol));
end: return;
}
static void do_matriksStr2 (void) {
	Data thee = parse [programPointer]. content.object;
	Stackel column = pop, row = pop;
	long irow, icol;
	irow = Stackel_getRowNumber (& row, thee); cherror
	icol = Stackel_getColumnNumber (& column, thee); cherror
	if (your getMatrixStr == NULL) {
		Melder_error ("%s objects accept no [row, column] indexing for string cells.", Thing_className (thee));
		goto end;
	}
	pushString (Melder_strdup (your getMatrixStr (thee, irow, icol)));
end: return;
}
static void do_funktie0 (long irow, long icol) {
	Data thee = parse [programPointer]. content.object;
	if (your getFunction0) {
		pushNumber (your getFunction0 (thee));
	} else if (your getFunction1) {
		Data me = theSource;
		if (me == NULL) {
			Melder_error ("No current object (we are not in a Formula command),\n"
				"hence no implicit x value for this %s object.\n"
				"Try using the (x) argument explicitly.", Thing_className (thee));
			goto end;
		} else if (our getX == NULL) {
			Melder_error ("The current %s object gives no implicit x values,\n"
				"hence no implicit x value for this %s object.\n"
				"Try using the (x) argument explicitly.", Thing_className (me), Thing_className (thee));
			goto end;
		} else {
			double x = our getX (me, icol);
			pushNumber (your getFunction1 (thee, x));
		}
	} else if (your getFunction2) {
		Data me = theSource;
		if (me == NULL) {
			Melder_error ("No current object (we are not in a Formula command),\n"
				"hence no implicit x or y values for this %s object.\n"
				"Try using both (x, y) arguments explicitly.", Thing_className (thee));
			goto end;
		} else if (our getX == NULL) {
			Melder_error ("The current %s object gives no implicit x values,\n"
				"hence no implicit x value for this %s object.\n"
				"Try using both (x, y) arguments explicitly.", Thing_className (me), Thing_className (thee));
			goto end;
		} else {
			double x = our getX (me, icol);
			if (our getY == NULL) {
				Melder_error ("The current %s object gives no implicit y values,\n"
					"hence no implicit y value for this %s object.\n"
					"Try using the (y) argument explicitly.", Thing_className (me), Thing_className (thee));
				goto end;
			} else {
				double y = our getY (me, irow);
				pushNumber (your getFunction2 (thee, x, y));
			}
		}
	} else {
		Melder_error ("%s objects accept no () values.", Thing_className (thee));
		goto end;
	}
end: return;
}
static void do_selfFunktie1 (long irow) {
	Data me = theSource;
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
		if (our getFunction1) {
			pushNumber (our getFunction1 (me, x.content.number));
		} else if (our getFunction2) {
			if (our getY == NULL) {
				Melder_error ("The current %s object (self) accepts no implicit y values.\n"
					"Try using both (x, y) arguments instead.", Thing_className (me));
				goto end;
			} else {
				double y = our getY (me, irow);
				pushNumber (our getFunction2 (me, x.content.number, y));
			}
		} else {
			Melder_error ("%s objects like \"self\" accept no (x) values.", Thing_className (me));
			goto end;
		}
	} else {
		Melder_error ("%s objects like \"self\" accept only numeric x values.", Thing_className (me));
	}
end: return;
}
static void do_funktie1 (long irow) {
	Data thee = parse [programPointer]. content.object;
	Stackel x = pop;
	if (x.which == Stackel_NUMBER) {
		if (your getFunction1) {
			pushNumber (your getFunction1 (thee, x.content.number));
		} else if (your getFunction2) {
			Data me = theSource;
			if (me == NULL) {
				Melder_error ("No current object (we are not in a Formula command),\n"
					"hence no implicit y value for this %s object.\n"
					"Try using both (x, y) arguments instead.", Thing_className (thee));
				goto end;
			} else if (our getY == NULL) {
				Melder_error ("The current %s object gives no implicit y values,\n"
					"hence no implicit y value for this %s object.\n"
					"Try using both (x, y) arguments instead.", Thing_className (me), Thing_className (thee));
				goto end;
			} else {
				double y = our getY (me, irow);
				pushNumber (your getFunction2 (thee, x.content.number, y));
			}
		} else {
			Melder_error ("%s objects accept no (x) values.", Thing_className (thee));
			goto end;
		}
	} else {
		Melder_error ("%s objects accept only numeric x values.", Thing_className (thee));
	}
end: return;
}
static void do_selfFunktie2 (void) {
	Data me = theSource;
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
		if (our getFunction2 == NULL) {
			Melder_error ("%s objects like \"self\" accept no (x, y) values.", Thing_className (me));
			goto end;
		}
		pushNumber (our getFunction2 (me, x.content.number, y.content.number));
	} else {
		Melder_error ("%s objects accept only numeric x values.", Thing_className (me));
	}
end: return;
}
static void do_funktie2 (void) {
	Data thee = parse [programPointer]. content.object;
	Stackel y = pop, x = pop;
	if (x.which == Stackel_NUMBER && y.which == Stackel_NUMBER) {
		if (your getFunction2 == NULL) {
			Melder_error ("%s objects accept no (x, y) values.", Thing_className (thee));
			goto end;
		}
		pushNumber (your getFunction2 (thee, x.content.number, y.content.number));
	} else {
		Melder_error ("%s objects accept only numeric x values.", Thing_className (thee));
	}
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

int Formula_run (long row, long col, double *numericResult, char **stringResult) {
	FormulaInstruction f = parse;
	programPointer = 1;   /* First symbol of the program. */
	if (theStack == NULL) theStack = (Stackel *) Melder_calloc (10000, sizeof (theStack));
	if (theStack == NULL)
		return Melder_error ("Out of memory during formula computation.");
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
	if (our getX == NULL) { Melder_error ("No values for \"x\" for this object."); goto end; }
	pushNumber (our getX (me, col));
} break; case Y_: {
	Data me = theSource;
	if (our getY == NULL) { Melder_error ("No values for \"y\" for this object."); goto end; }
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
} break; case CHI_SQUARE_P_: { do_function_dl_d (NUMchiSquareP);
} break; case CHI_SQUARE_Q_: { do_function_dl_d (NUMchiSquareQ);
} break; case INV_CHI_SQUARE_Q_: { do_function_dl_d (NUMinvChiSquareQ);
} break; case STUDENT_P_: { do_function_dl_d (NUMstudentP);
} break; case STUDENT_Q_: { do_function_dl_d (NUMstudentQ);
} break; case INV_STUDENT_Q_: { do_function_dl_d (NUMinvStudentQ);
} break; case BETA_: { do_function_dd_d (NUMbeta);
} break; case BESSEL_I_: { do_function_ld_d (NUMbesselI);
} break; case BESSEL_K_: { do_function_ld_d (NUMbesselK);
} break; case SOUND_PRESSURE_TO_PHON_: { do_function_dd_d (NUMsoundPressureToPhon);
/********** Functions of 3 numerical variables: **********/
} break; case FISHER_P_: { do_function_dll_d (NUMfisherP);
} break; case FISHER_Q_: { do_function_dll_d (NUMfisherQ);
} break; case INV_FISHER_Q_: { do_function_dll_d (NUMinvFisherQ);
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
} break; case INDEX_REGEX_: { do_index_regex (FALSE);
} break; case RINDEX_REGEX_: { do_index_regex (TRUE);
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
	if (condition.which == Stackel_NUMBER) {
		if (condition.content.number != 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_error ("A condition between \"if\" and \"then\" has to be a number, not %s.", Stackel_whichText (& condition));
	}
} break; case IFFALSE_: {
	Stackel condition = pop;
	if (condition.which == Stackel_NUMBER) {
		if (condition.content.number == 0.0) {
			programPointer = f [programPointer]. content.label - theOptimize;
		}
	} else {
		Melder_error ("A condition between \"if\" and \"then\" has to be a number, not %s.", Stackel_whichText (& condition));
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
} break; case SQR_: { do_sqr ();
} break; case STRING_: {
	char *result = Melder_strdup (f [programPointer]. content.string); cherror
	pushString (result);
} break; case NUMERIC_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	pushNumber (var -> numericValue);
} break; case STRING_VARIABLE_: {
	InterpreterVariable var = f [programPointer]. content.variable;
	char *result = Melder_strdup (var -> stringValue); cherror
	pushString (result);
} break; case SELECTED2_: { do_selected2 ();
} break; case SELECTEDSTR2_: { do_selectedStr2 ();
} break; default: return Melder_error ("Symbol \"%s\" without action.", Formula_instructionNames [parse [programPointer]. symbol]);
		} /* switch */
		cherror
		programPointer ++;
	} /* while */
	Melder_assert (w == 1);
	if (theExpressionType == EXPRESSION_TYPE_NUMERIC) {
		if (theStack [1]. which == Stackel_STRING) {
			Melder_error ("Found a string expression instead of a numeric expression."); goto end;
		}
		if (numericResult) *numericResult = theStack [1]. content.number;
		else Melder_information ("%s", Melder_double (theStack [1]. content.number));
	} else if (theExpressionType == EXPRESSION_TYPE_STRING) {
		if (theStack [1]. which == Stackel_NUMBER) {
			Melder_error ("Found a numeric expression instead of a string expression."); goto end;
		}
		if (stringResult) { *stringResult = theStack [1]. content.string; theStack [1]. content.string = NULL; }   /* Undangle. */
		else Melder_information ("%s", theStack [1]. content.string);
	} else {
		Melder_assert (theExpressionType == EXPRESSION_TYPE_UNKNOWN);
		if (theStack [1]. which == Stackel_NUMBER) {
			if (numericResult) *numericResult = theStack [1]. content.number;
			else Melder_information ("%s", Melder_double (theStack [1]. content.number));
		} else {
			Melder_assert (theStack [1]. which == Stackel_STRING);
			if (stringResult) { *stringResult = theStack [1]. content.string; theStack [1]. content.string = NULL; }   /* Undangle. */
			else Melder_information ("%s", theStack [1]. content.string);
		}
	}
end:
	/*
		Clean up the stack (theStack [1] may have been disowned).
	*/
	for (w = wmax; w > 0; w --) {
		Stackel *stackel = & theStack [w];
		if (stackel -> which > 0) Stackel_cleanUp (stackel);
	}
	iferror return 0;
	return 1;
}

/* End of file Formula.c */
