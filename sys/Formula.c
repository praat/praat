/* Formula.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2002/06/11
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
 */

#include <ctype.h>
#include <time.h>
#include "NUM.h"
#include "NUM2.h"
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
static double *s;   /* Numeric stack. */
static char **ss;   /* String stack. */

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

	SELF_, MATRIKS_,
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
		LEFTSTR_, RIGHTSTR_, MIDSTR_, ENVIRONMENT_, INDEX_, RINDEX_, EXTRACT_NUMBER_, EXTRACT_WORD_, EXTRACT_LINE_,
		SELECTED_, SELECTEDSTR_, NUMBER_OF_SELECTED_,
		FIXED_, PERCENT_,
	#define HIGH_STRING_FUNCTION  PERCENT_

	#define LOW_FUNCTION  LOW_FUNCTION_1
	#define HIGH_FUNCTION  HIGH_STRING_FUNCTION

	/* Membership operator. */
	PERIOD_,
	#define hoogsteInvoersymbool  PERIOD_

/* Symbols introduced by the parser. */

	TRUE_, FALSE_, IFTRUE_, IFFALSE_, GOTO_, LABEL_,
	SELF0_, SELFMATRIKS1_, SELFMATRIKS2_, SELFFUNKTIE1_, SELFFUNKTIE2_,
	MATRIKS0_, MATRIKS1_, MATRIKS2_, FUNKTIE0_, FUNKTIE1_, FUNKTIE2_,
	ROW_INDEX_, COLUMN_INDEX_,
	SQR_,

/* Symbols introduced by lexical analysis. */

	STRING_, STRING_EQ_, STRING_NE_, STRING_LE_, STRING_LT_, STRING_GE_, STRING_GT_, STRING_ADD_, STRING_SUB_,
	NUMERIC_VARIABLE_, STRING_VARIABLE_, NUMERIC_COLUMN_, STRING_COLUMN_,
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
	"self", "_matriks",
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
	"left$", "right$", "mid$", "environment$", "index", "rindex", "extractNumber", "extractWord$", "extractLine$",
	"selected", "selected$", "numberOfSelected",
	"fixed$", "percent$",
	".",
	"_true", "_false", "_iftrue", "_iffalse", "_gaNaar", "_label",
	"_self0", "_selfmatriks1", "_selfmatriks2", "_selffunktie1", "_selffunktie2",
	"_matriks0", "_matriks1", "_matriks2", "_funktie0", "_funktie1", "_funktie2",
	"_rowIndex", "_columnIndex",
	"_square",
	"_string", "_str=", "_str<>", "_str<=", "_str<", "_str>=", "_str>", "_str+", "_str-",
	"_numericVariable", "_stringVariable", "_numericColumn", "_stringColumn",
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
		} else if (kar >= 'a' && kar <= 'z') {
			int tok, endsInDollarSign = FALSE;
			stokaan;
			do stokkar while (kar >= 'A' && kar <= 'Z' || kar >= 'a' && kar <= 'z' || kar >= '0' && kar <= '9' || kar == '_');
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
			char *underscore;
			stokaan;
			do stokkar while (isalnum (kar) || kar == '_');
			stokuit;
			oudkar;
			/*
			 * 'stok' now contains a word that could be an object name.
			 */
			underscore = strchr (stok, '_');
			if (underscore == NULL) {
				formulefout ("Object name should contain an underscore (note: variables start with lower case)", ikar);
				return 0;
			} else {
				int i = praat.n;
				*underscore = ' ';
				while (i > 0 && ! strequ (stok, praat.list [i]. name))
					i --;
				if (i == 0) {
					formulefout ("No such object (note: variables start with lower case)", ikar);
					return 0;
				}
				nieuwtok (MATRIKS_)
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

static int parseNumericExpression (void);
static int parseStringExpression (void);

static int parseNumericOrStringCellIndex (Data data) {
	int saveLexan = ilexan, saveParse = iparse, iparseNumeric;
	if (parseNumericExpression ()) return 1;
	iparseNumeric = iparse;
	Melder_clearError ();
	ilexan = saveLexan, iparse = saveParse;
	if (parseStringExpression ()) {
		nieuwontleed (COLUMN_INDEX_);
		parse [iparse]. content.object = data;
		return 1;
	}
	/*
	 * If we arrive here, both the numeric parse and the string parse went wrong.
	 * Give the most sensible message.
	 */
	if (iparse > iparseNumeric) return 0;   /* Message for string error. */
	Melder_clearError ();
	ilexan = saveLexan, iparse = saveParse;
	parseNumericExpression ();   /* Re-generate the numeric error message. */
	return 0;
}

static int parsePowerFactor (void) {
	int symbol = nieuwlees;

	if (symbol >= LOW_VALUE && symbol <= HIGH_VALUE) {
		nieuwontleed (symbol);
		if (symbol == NUMBER_) parsenumber (lexan [ilexan]. content.number);
		return 1;
	}

	if (symbol == NUMERIC_VARIABLE_) {
		nieuwontleed (symbol);
		parse [iparse]. content.variable = lexan [ilexan]. content.variable;
		return 1;
	}

	if (symbol == SELF_) {
		symbol = nieuwlees;
		if (symbol == RECHTEHAAKOPENEN_) {
			if (! parseNumericOrStringCellIndex (theSource)) return 0;
			if (nieuwlees == KOMMA_) {
				if (parse [iparse]. symbol == COLUMN_INDEX_)
					parse [iparse]. symbol = ROW_INDEX_;
				if (! parseNumericOrStringCellIndex (theSource)) return 0;
				nieuwontleed (SELFMATRIKS2_);
				return pas (RECHTEHAAKSLUITEN_);
			}
			oudlees;
			nieuwontleed (SELFMATRIKS1_);
			return pas (RECHTEHAAKSLUITEN_);
		}
		if (symbol == HAAKJEOPENEN_) {
			if (! parseNumericExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseNumericExpression ()) return 0;
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

	if (symbol == HAAKJEOPENEN_) {
		if (! parseNumericExpression ()) return 0;
		return pas (HAAKJESLUITEN_);
	}

	if (symbol == IF_) {
		int elseLabel = nieuwlabel;   /* Moet lokaal, */
		int endifLabel = nieuwlabel;   /* vanwege rekursie. */
		if (! parseNumericExpression ()) return 0;
		nieuwontleed (IFFALSE_);  ontleedlabel (elseLabel);
		if (! pas (THEN_)) return 0;
		if (! parseNumericExpression ()) return 0;
		nieuwontleed (GOTO_);     ontleedlabel (endifLabel);
		if (! pas (ELSE_)) return 0;
		nieuwontleed (LABEL_);    ontleedlabel (elseLabel);
		if (! parseNumericExpression ()) return 0;
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
				if (! parseNumericOrStringCellIndex (thee)) return 0;
				if (nieuwlees == KOMMA_) {
					if (parse [iparse]. symbol == COLUMN_INDEX_)
						parse [iparse]. symbol = ROW_INDEX_;
					if (! parseNumericOrStringCellIndex (thee)) return 0;
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
				if (! parseNumericExpression ()) return 0;
				if (nieuwlees == KOMMA_) {
					if (! parseNumericExpression ()) return 0;
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

	if (symbol >= LOW_FUNCTION_1 && symbol <= HIGH_FUNCTION_1) {
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseNumericExpression ()) return 0;
		if (! pas (HAAKJESLUITEN_)) return 0;
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_FUNCTION_2 && symbol <= HIGH_FUNCTION_2) {
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseNumericExpression ()) return 0;
		if (! pas (KOMMA_)) return 0;
		if (! parseNumericExpression ()) return 0;
		if (! pas (HAAKJESLUITEN_)) return 0;
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_FUNCTION_3 && symbol <= HIGH_FUNCTION_3) {
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseNumericExpression ()) return 0;
		if (! pas (KOMMA_)) return 0;
		if (! parseNumericExpression ()) return 0;
		if (! pas (KOMMA_)) return 0;
		if (! parseNumericExpression ()) return 0;
		if (! pas (HAAKJESLUITEN_)) return 0;
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol >= LOW_FUNCTION_N && symbol <= HIGH_FUNCTION_N) {
		int n = 1;
		if (! pas (HAAKJEOPENEN_)) return 0;
		if (! parseNumericExpression ()) return 0;
		while (nieuwlees == KOMMA_) {
			if (! parseNumericExpression ()) return 0;
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
			if (! parseStringExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == INDEX_ || symbol == RINDEX_ || symbol == EXTRACT_NUMBER_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == SELECTED_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseNumericExpression ()) return 0;
				symbol = SELECTED2_;
			} else oudlees;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == NUMBER_OF_SELECTED_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else {
			formulefout ("Function returning a number expected", lexan [ilexan]. position);
			oudlees;
			return 0;
		}
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol == NUMERIC_COLUMN_) {
		nieuwontleed (symbol);
		parsenumber (lexan [ilexan]. content.number);
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

static int parseStringTerm (void) {
	int symbol = nieuwlees;

	if (symbol == STRING_VARIABLE_) {
		nieuwontleed (symbol);
		parse [iparse]. content.variable = lexan [ilexan]. content.variable;
		return 1;
	}
		
	if (symbol == HAAKJEOPENEN_) {
		if (! parseStringExpression ()) return 0;
		return pas (HAAKJESLUITEN_);
	}

	if (symbol == STRING_) {
		nieuwontleed (symbol);
		parse [iparse]. content.string = lexan [ilexan]. content.string;   /* Reference copy! */
		return 1;
	}

	if (symbol == IF_) {
		int elseLabel = nieuwlabel;   /* Moet lokaal, */
		int endifLabel = nieuwlabel;   /* vanwege rekursie. */
		if (! parseNumericExpression ()) return 0;
		nieuwontleed (IFFALSE_);  ontleedlabel (elseLabel);
		if (! pas (THEN_)) return 0;
		if (! parseStringExpression ()) return 0;
		nieuwontleed (GOTO_);     ontleedlabel (endifLabel);
		if (! pas (ELSE_)) return 0;
		nieuwontleed (LABEL_);    ontleedlabel (elseLabel);
		if (! parseStringExpression ()) return 0;
		if (! pas (ENDIF_)) return 0;
		nieuwontleed (LABEL_);    ontleedlabel (endifLabel);
		return 1;
	}

	if (symbol >= LOW_STRING_FUNCTION && symbol <= HIGH_STRING_FUNCTION) {
		if (symbol == DATESTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == LEFTSTR_ || symbol == RIGHTSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseNumericExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == MIDSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseNumericExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseNumericExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == EXTRACT_WORD_ || symbol == EXTRACT_LINE_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == ENVIRONMENT_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == SELECTEDSTR_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseStringExpression ()) return 0;
			if (nieuwlees == KOMMA_) {
				if (! parseNumericExpression ()) return 0;
				symbol = SELECTEDSTR2_;
			} else oudlees;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else if (symbol == FIXED_ || symbol == PERCENT_) {
			if (! pas (HAAKJEOPENEN_)) return 0;
			if (! parseNumericExpression ()) return 0;
			if (! pas (KOMMA_)) return 0;
			if (! parseNumericExpression ()) return 0;
			if (! pas (HAAKJESLUITEN_)) return 0;
		} else {
			formulefout ("Function returning a string expected", lexan [ilexan]. position);
			oudlees;
			return 0;
		}
		nieuwontleed (symbol);
		return 1;
	}

	if (symbol == STRING_COLUMN_) {
		nieuwontleed (symbol);
		parsenumber (lexan [ilexan]. content.number);
		return 1;
	}

	formulefout ("Symbol misplaced...", lexan [ilexan]. position);
	oudlees;
	return 0;
}

static int parseStringTerms (void) {
	int symbol = nieuwlees;
	if (symbol == ADD_ || symbol == SUB_) {
		if (! parseStringTerm ()) return 0;
		nieuwontleed (symbol == ADD_ ? STRING_ADD_ : STRING_SUB_);
		if (! parseStringTerms ()) return 0;
	}
	else oudlees;
	return 1;
}

static int parseStringExpression (void) {
	if (! parseStringTerm ()) return 0;
	if (! parseStringTerms ()) return 0;
	return 1;
}

static int parseStringComparison (void) {
	int symbol;
	if (! parseStringExpression ()) return 0;
	symbol = nieuwlees;
	if (symbol >= EQ_ && symbol <= GT_) {
		if (! parseStringExpression ()) return 0;
		nieuwontleed (symbol + STRING_EQ_ - EQ_);
	} else return formulefout ("Expected numeric expression but found string expression", lexan [ilexan]. position);
	return 1;
}

static int parseNot (void) {
	int saveLexan = ilexan, saveParse = iparse, symbol;
	if (! parseTerm ()) {
		int iparseNumeric = iparse;
		Melder_clearError ();
		ilexan = saveLexan, iparse = saveParse;
		if (parseStringComparison ()) return 1;
		/*
		 * If we arrive here, both the numeric parse and the string parse went wrong.
		 * Give the most sensible message.
		 */
		if (iparse > iparseNumeric) return 0;   /* Message for string error. */
		Melder_clearError ();
		ilexan = saveLexan, iparse = saveParse;
		parseTerm ();   /* Re-generate the numeric error message. */
		return 0;
	}
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

static int parseNumericExpression (void) {
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

static int Formula_parseNumericExpression (void) {
	ilabel = ilexan = iparse = 0;
	if (! parseNumericExpression () || ! pas (END_)) return 0;
	nieuwontleed (END_);
	numberOfInstructions = iparse;
	return 1;
}
static int Formula_parseStringExpression (void) {
	ilabel = ilexan = iparse = 0;
	if (! parseStringExpression () || ! pas (END_)) return 0;
	nieuwontleed (END_);
	numberOfInstructions = iparse;
	return 1;
}
static int Formula_parseNumericOrStringExpression (void) {
	int iparseNumeric;
	ilabel = ilexan = iparse = 0;
	if (parseNumericExpression ()) {
		if (! pas (END_)) return 0;
		nieuwontleed (END_);
		numberOfInstructions = iparse;
		return 1;
	}
	iparseNumeric = iparse;
	Melder_clearError ();
	ilabel = ilexan = iparse = 0;
	if (parseStringExpression ()) {
		if (! pas (END_)) return 0;
		nieuwontleed (END_);
		numberOfInstructions = iparse;
		return 1;
	}
	/*
	 * If we arrive here, both the numeric parse and the string parse went wrong.
	 * Give the most sensible message.
	 */
	if (iparse > iparseNumeric) return 0;   /* String parsing went a bit better than numeric parsing: message for string error. */
	Melder_clearError ();
	ilabel = ilexan = iparse = 0;
	parseNumericExpression ();   /* Re-generate the numeric error message. */
	return 0;
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
			Melder_info ("%d %s %.17g", i, instructionName, f [i]. content.number);
		else if (symbol == GOTO_ || symbol == IFFALSE_ || symbol == IFTRUE_ || symbol == LABEL_)
			Melder_info ("%d %s %d", i, instructionName, f [i]. content.label);
		else if (symbol == NUMERIC_VARIABLE_)
			Melder_info ("%d %s %s %s", i, instructionName, f [i]. content.variable -> key, Melder_double (f [i]. content.variable -> numericValue));
		else if (symbol == STRING_VARIABLE_)
			Melder_info ("%d %s %s %s", i, instructionName, f [i]. content.variable -> key, f [i]. content.variable -> stringValue);
		else if (symbol == STRING_)
			Melder_info ("%d %s \"%s\"", i, instructionName, f [i]. content.string);
		else if (symbol == MATRIKS_)
			Melder_info ("%d %s %ld %s", i, instructionName,
				Thing_className (f [i]. content.object), ((Thing) f [i]. content.object) -> name);
		else
			Melder_info ("%d %s", i, instructionName);
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
	if (! s) s = (double *) Melder_calloc (10000, sizeof (double));
	if (! ss) ss = (char **) Melder_calloc (1000, sizeof (char *));
	if (lexan == NULL || parse == NULL || s == NULL || ss == NULL)
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
	/*Formula_print (lexan);*/
	if (theExpressionType == EXPRESSION_TYPE_NUMERIC) {
		if (! Formula_parseNumericExpression ()) return 0;
	} else if (theExpressionType == EXPRESSION_TYPE_STRING) {
		if (! Formula_parseStringExpression ()) return 0;
	} else {
		if (! Formula_parseNumericOrStringExpression ()) return 0;
	}
	/*Formula_print (parse);*/
	if (theOptimize) {
		Formula_optimizeFlow ();
		/*Formula_print (parse);*/
		Formula_evaluateConstants ();
		/*Formula_print (parse);*/
	}
	Formula_removeLabels ();
	/*Formula_print (parse);*/
	return 1;
}

int Formula_run (long row, long col, double *numericResult, char **stringResult) {
	FormulaInstruction f = parse;
	Data me = theSource;
	int w = 0, sw = 0;   /* w = numeric stack pointer, sw = string stack pointer; start new stacks. */
	int i = 1;   /* First symbol of the program. */
	while (i <= numberOfInstructions) {
		char *string;
		int symbol;
		switch (symbol = f [i]. symbol) {

case NUMBER_: {
	s [++ w] = f [i]. content.number;
} break; case STOPWATCH_: {
	s [++ w] = Melder_stopwatch ();
} break; case ROW_: {
	s [++ w] = row;
} break; case COL_: {
	s [++ w] = col;
} break; case X_: {
	if (our getX == NULL) { Melder_error ("No values for \"x\" for this object."); goto end; }
	s [++ w] = our getX (me, col);
} break; case Y_: {
	if (our getY == NULL) { Melder_error ("No values for \"y\" for this object."); goto end; }
	s [++ w] = our getY (me, row);
} break; case NOT_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : s [w] == 0.0 ? 1.0 : 0.0;
} break; case EQ_: {
	-- w; s [w] = s [w] == s [w + 1] ? 1.0 : 0.0;
} break; case NE_: {
	-- w; s [w] = s [w] != s [w + 1] ? 1.0 : 0.0;
} break; case LE_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] <= s [w + 1] ? 1.0 : 0.0;
} break; case LT_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] <  s [w + 1] ? 1.0 : 0.0;
} break; case GE_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] >= s [w + 1] ? 1.0 : 0.0;
} break; case GT_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] >  s [w + 1] ? 1.0 : 0.0;
} break; case ADD_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] + s [w + 1];
} break; case SUB_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] - s [w + 1];
} break; case MUL_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : s [w] * s [w + 1];
} break; case RDIV_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 1] == 0.0 ? NUMundefined : s [w] / s [w + 1];
} break; case IDIV_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 1] == 0.0 ? NUMundefined : floor (s [w] / s [w + 1]);
} break; case MOD_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 1] == 0.0 ? NUMundefined : s [w] - floor (s [w] / s [w + 1]) * s [w + 1];
} break; case MINUS_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : - s [w];
} break; case POWER_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : pow (s [w], s [w + 1]);
/********** Functions of 1 variable: **********/
} break; case ABS_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : fabs (s [w]);
} break; case ROUND_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : floor (s [w] + 0.5);
} break; case FLOOR_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : floor (s [w]);
} break; case CEILING_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : ceil (s [w]);
} break; case SQRT_: {
	s [w] = s [w] == NUMundefined || s [w] < 0.0 ? NUMundefined : sqrt (s [w]);
} break; case SIN_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : sin (s [w]);
} break; case COS_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : cos (s [w]);
} break; case TAN_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : tan (s [w]);
} break; case ARCSIN_: {
	s [w] = s [w] == NUMundefined || fabs (s [w]) > 1.0 ? NUMundefined : asin (s [w]);
} break; case ARCCOS_: {
	s [w] = s [w] == NUMundefined || fabs (s [w]) > 1.0 ? NUMundefined : acos (s [w]);
} break; case ARCTAN_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : atan (s [w]);
} break; case EXP_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : exp (s [w]);
} break; case SINH_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : sinh (s [w]);
} break; case COSH_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : cosh (s [w]);
} break; case TANH_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : tanh (s [w]);
} break; case ARCSINH_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : log (s [w] + sqrt (1 + s [w] * s [w]));
} break; case ARCCOSH_: {
	s [w] = s [w] == NUMundefined || s [w] < 1.0 ? NUMundefined : log (s [w] + sqrt (s [w] * s [w] - 1));
} break; case ARCTANH_: {
	s [w] = s [w] == NUMundefined || s [w] <= -1.0 || s [w] >= 1.0 ? NUMundefined : 0.5 * log ((1.0 + s [w]) / (1.0 - s [w]));
} break; case SIGMOID_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMsigmoid (s [w]);
} break; case ERF_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : 1 - NUMerfcc (s [w]);
} break; case ERFC_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMerfcc (s [w]);
} break; case GAUSS_P_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMgaussP (s [w]);
} break; case GAUSS_Q_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMgaussQ (s [w]);
} break; case INV_GAUSS_Q_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMinvGaussQ (s [w]);
} break; case RANDOM_POISSON_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMrandomPoisson (s [w]);
} break; case LOG2_: {
	s [w] = s [w] == NUMundefined || s [w] <= 0.0 ? NUMundefined : log (s [w]) * NUMlog2e;
} break; case LN_: {
	s [w] = s [w] == NUMundefined || s [w] <= 0.0 ? NUMundefined : log (s [w]);
} break; case LOG10_: {
	s [w] = s [w] == NUMundefined || s [w] <= 0.0 ? NUMundefined : log10 (s [w]);
} break; case LN_GAMMA_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMlnGamma (s [w]);
} break; case HERTZ_TO_BARK_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMhertzToBark (s [w]);
} break; case BARK_TO_HERTZ_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMbarkToHertz (s [w]);
} break; case PHON_TO_DIFFERENCE_LIMENS_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMphonToDifferenceLimens (s [w]);
} break; case DIFFERENCE_LIMENS_TO_PHON_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMdifferenceLimensToPhon (s [w]);
} break; case HERTZ_TO_MEL_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMhertzToMel (s [w]);
} break; case MEL_TO_HERTZ_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMmelToHertz (s [w]);
} break; case HERTZ_TO_SEMITONES_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMhertzToSemitones (s [w]);
} break; case SEMITONES_TO_HERTZ_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMsemitonesToHertz (s [w]);
} break; case ERB_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMerb (s [w]);
} break; case HERTZ_TO_ERB_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMhertzToErb (s [w]);
} break; case ERB_TO_HERTZ_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : NUMerbToHertz (s [w]);
/********** Functions of 2 variables: **********/
} break; case ARCTAN2_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : atan2 (s [w], s [w + 1]);
} break; case RANDOM_UNIFORM_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMrandomUniform (s [w], s [w + 1]);
} break; case RANDOM_INTEGER_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMrandomInteger (s [w], s [w + 1]);
} break; case RANDOM_GAUSS_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMrandomGauss (s [w], s [w + 1]);
} break; case CHI_SQUARE_P_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMchiSquareP (s [w], s [w + 1]);
} break; case CHI_SQUARE_Q_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMchiSquareQ (s [w], s [w + 1]);
} break; case INV_CHI_SQUARE_Q_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMinvChiSquareQ (s [w], s [w + 1]);
} break; case STUDENT_P_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMstudentP (s [w], s [w + 1]);
} break; case STUDENT_Q_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMstudentQ (s [w], s [w + 1]);
} break; case INV_STUDENT_Q_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMinvStudentQ (s [w], s [w + 1]);
} break; case BETA_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMbeta (s [w], s [w + 1]);
} break; case BESSEL_I_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMbesselI (s [w], s [w + 1]);
} break; case BESSEL_K_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMbesselK (s [w], s [w + 1]);
} break; case SOUND_PRESSURE_TO_PHON_: {
	-- w; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined ? NUMundefined : NUMsoundPressureToPhon (s [w], s [w + 1]);
/********** Functions of 3 variables: **********/
} break; case FISHER_P_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMfisherP (s [w], s [w + 1], s [w + 2]);
} break; case FISHER_Q_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMfisherQ (s [w], s [w + 1], s [w + 2]);
} break; case INV_FISHER_Q_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMinvFisherQ (s [w], s [w + 1], s [w + 2]);
} break; case BINOMIAL_P_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMbinomialP (s [w], s [w + 1], s [w + 2]);
} break; case BINOMIAL_Q_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMbinomialQ (s [w], s [w + 1], s [w + 2]);
} break; case INV_BINOMIAL_P_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMinvBinomialP (s [w], s [w + 1], s [w + 2]);
} break; case INV_BINOMIAL_Q_: {
	w -= 2; s [w] = s [w] == NUMundefined || s [w + 1] == NUMundefined || s [w + 2] == NUMundefined ? NUMundefined : NUMinvBinomialQ (s [w], s [w + 1], s [w + 2]);
/********** Functions of a variable number of variables: **********/
} break; case MIN_: {
	int n = s [w --], j;
	double minimum;
	Melder_assert (n >= 1);
	minimum = s [w --];
	for (j = n - 1; j > 0; j --) {
		double value = s [w --];
		minimum = minimum == NUMundefined || value == NUMundefined ? NUMundefined : minimum < value ? minimum : value;
	}
	s [++ w] = minimum;
} break; case MAX_: {
	int n = s [w --], j;
	double maximum;
	Melder_assert (n >= 1);
	maximum = s [w --];
	for (j = n - 1; j > 0; j --) {
		double value = s [w --];
		maximum = maximum == NUMundefined || value == NUMundefined ? NUMundefined : maximum > value ? maximum : value;
	}
	s [++ w] = maximum;
} break; case IMIN_: {
	int n = s [w --], j;
	double imin = n, minimum;
	Melder_assert (n >= 1);
	minimum = s [w --];
	for (j = n - 1; j > 0; j --) {
		double value = s [w --];
		if (minimum == NUMundefined || value == NUMundefined) {
			minimum = NUMundefined;
			imin = NUMundefined;
		} else if (value < minimum) {
			minimum = value;
			imin = j;
		}
	}
	s [++ w] = imin;
} break; case IMAX_: {
	int n = s [w --], j;
	double imax = n, maximum;
	Melder_assert (n >= 1);
	maximum = s [w --];
	for (j = n - 1; j > 0; j --) {
		double value = s [w --];
		if (maximum == NUMundefined || value == NUMundefined) {
			maximum = NUMundefined;
			imax = NUMundefined;
		} else if (value > maximum) {
			maximum = value;
			imax = j;
		}
	}
	s [++ w] = imax;
/********** String functions: **********/
} break; case LENGTH_: {
	s [++ w] = strlen (ss [sw]);
	Melder_free (ss [sw]);
	-- sw;
} break; case FILE_READABLE_: {
	structMelderFile file;
	Melder_relativePathToFile (ss [sw], & file); cherror
	s [++ w] = MelderFile_readable (& file); Melder_free (ss [sw]);
	-- sw;
} break; case DATESTR_: {
	time_t today = time (NULL);
	char *newline;
	++ sw;
	Melder_free (ss [sw]);
	ss [sw] = Melder_strdup (ctime (& today));
	newline = strchr (ss [sw], '\n');
	if (newline) *newline = '\0';
} break; case LEFTSTR_: {
	long length = strlen (ss [sw]), newlength = s [w --];
	if (newlength < length) {
		if (newlength < 0) newlength = 0;
		ss [sw] [newlength] = '\0';   /* Truncate in place. */
	}
} break; case RIGHTSTR_: {
	long length = strlen (ss [sw]), newlength = s [w --];
	if (newlength < length) {
		char *string;
		if (newlength < 0) newlength = 0;
		string = Melder_strdup (ss [sw] + length - newlength); cherror
		Melder_free (ss [sw]);
		ss [sw] = string;
	}
} break; case MIDSTR_: {
	long length = strlen (ss [sw]), newlength = s [w --], start = s [w --], finish = start + newlength - 1;
	char *string;
	if (start < 1) start = 1;
	if (finish > length) finish = length;
	newlength = finish - start + 1;
	if (newlength > 0) {
		string = Melder_malloc (newlength + 1); cherror
		strncpy (string, ss [sw] + start - 1, newlength);
		string [newlength] = '\0';
	} else {
		string = Melder_strdup (""); cherror
	}
	Melder_free (ss [sw]);
	ss [sw] = string;
} break; case ENVIRONMENT_: {
	char *env = Melder_getenv (ss [sw]);
	char *string = Melder_strdup (env != NULL ? env : "");
	Melder_free (ss [sw]);
	ss [sw] = string;
} break; case INDEX_: {
	char *substring = strstr (ss [sw - 1], ss [sw]);
	s [++ w] = substring ? substring - ss [sw - 1] + 1 : 0;
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	sw -= 2;
} break; case RINDEX_: {
	char *string = ss [sw - 1], *lastSubstring = strstr (string, ss [sw]);
	if (ss [sw] [0] == '\0') {
		s [++ w] = strlen (string);
	} else if (lastSubstring) {
		for (;;) {
			char *substring = strstr (lastSubstring + 1, ss [sw]);
			if (substring == NULL) break;
			lastSubstring = substring;
		}
		s [++ w] = lastSubstring - string + 1;
	} else {
		s [++ w] = 0;
	}
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	sw -= 2;
} break; case EXTRACT_NUMBER_: {
	char *string = ss [sw - 1], *substring = strstr (string, ss [sw]);
	if (substring == NULL) {
		s [++ w] = NUMundefined;
	} else {
		substring += strlen (ss [sw]);
		/* Skip white space. */
		while (*substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') substring ++;
		if (substring [0] == '\0' || strnequ (substring, "--undefined--", 13)) {
			s [++ w] = NUMundefined;
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
				s [++ w] = Melder_atof (buffer);
			} else {
				buffer [i + 1] = '\0';
				slash = strchr (buffer, '/');
				if (slash) {
					double numerator, denominator;
					*slash = '\0';
					numerator = Melder_atof (buffer), denominator = Melder_atof (slash + 1);
					s [++ w] = numerator / denominator;
				} else {
					s [++ w] = Melder_atof (buffer);
				}
			}
		}
	}
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	sw -= 2;
} break; case EXTRACT_WORD_: {
	char *string = ss [sw - 1], *substring = strstr (string, ss [sw]), *target, *p;
	if (substring == NULL) {
		target = Melder_strdup (""); cherror
	} else {
		long length;
		substring += strlen (ss [sw]);
		/* Skip white space. */
		while (*substring == ' ' || *substring == '\t' || *substring == '\n' || *substring == '\r') substring ++;
		p = substring;
		/* Proceed until next white space. */
		while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') p ++;
		length = p - substring;
		target = Melder_malloc (length + 1); cherror
		strncpy (target, substring, length);
		target [length] = '\0';
	}
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	sw -= 1;
	ss [sw] = target;
} break; case EXTRACT_LINE_: {
	char *string = ss [sw - 1], *substring = strstr (string, ss [sw]), *target, *p;
	if (substring == NULL) {
		target = Melder_strdup (""); cherror
	} else {
		long length;
		substring += strlen (ss [sw]);
		/* Don't skip white space. */
		p = substring;
		/* Proceed until end of line. */
		while (*p != '\0' && *p != '\n' && *p != '\r') p ++;
		length = p - substring;
		target = Melder_malloc (length + 1); cherror
		strncpy (target, substring, length);
		target [length] = '\0';
	}
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	sw -= 1;
	ss [sw] = target;
} break; case SELECTED_: {
	void *klas = Thing_classFromClassName (ss [sw]); cherror
	s [++ w] = praat_getIdOfSelected (klas, 0); cherror
	Melder_free (ss [sw]); -- sw;
} break; case SELECTEDSTR_: {
	char *name;
	void *klas = Thing_classFromClassName (ss [sw]); cherror
	name = praat_getNameOfSelected (klas, 0); cherror
	Melder_free (ss [sw]);
	ss [sw] = Melder_strdup (name); cherror
} break; case NUMBER_OF_SELECTED_: {
	void *klas = Thing_classFromClassName (ss [sw]); cherror
	s [++ w] = praat_selection (klas); Melder_free (ss [sw]); -- sw;
} break; case FIXED_: {
	double precision = s [w --];
	double value = s [w --];
	ss [++ sw] = Melder_strdup (Melder_fixed (value, precision));
} break; case PERCENT_: {
	double precision = s [w --];
	double value = s [w --];
	ss [++ sw] = Melder_strdup (Melder_percent (value, precision));
/********** **********/
} break; case TRUE_: {
	s [++ w] = 1.0;
} break; case FALSE_: {
	s [++ w] = 0.0;
/* Possible compiler BUG: many compilers cannot handle the following assignment. */
/* Those compilers have trouble with praat's AND and OR. */
} break; case IFTRUE_: {
	if (s [w --] != 0.0) i = f [i]. content.label - theOptimize;
} break; case IFFALSE_: {
	if (s [w --] == 0.0) i = f [i]. content.label - theOptimize;
} break; case GOTO_: {
	i = f [i]. content.label - theOptimize;
/* Possible compiler BUG: CodeWarrior 5.3 for Windows cannot handle a simple "break" here. */
/* It has trouble with praat's AND and OR. */
} break; case LABEL_: {
	s[w+1]=2.0;   /* Dummy assignment. */
} break; case SELF0_: {
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	if (our getCell) {
		s [++ w] = our getCell (me);
	} else if (our getVector) {
		if (col == 0) {
			Melder_error ("We are not in a loop, "
				"hence no implicit column index for the current %s object (self).\n"
				"Try using the [column] index explicitly.", Thing_className (me));
			goto end;
		} else {
			s [++ w] = our getVector (me, col);
		}
	} else if (our getMatrix) {
		if (row == 0) {
			if (col == 0) {
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
			s [++ w] = our getMatrix (me, row, col);
		}
	} else {
		Melder_error ("%s objects (like self) accept no [] indexing.", Thing_className (me));
		goto end;
	}
} break; case SELFMATRIKS1_: {
	long icol = floor (s [w] + 0.5);   /* Round. */
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	if (our getVector) {
		s [w] = our getVector (me, icol);
	} else if (our getMatrix) {
		if (row == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit row index for the current %s object (self).\n"
				"Try using both [row, column] indexes instead.", Thing_className (me));
			goto end;
		} else {
			s [w] = our getMatrix (me, row, icol);
		}
	} else {
		Melder_error ("%s objects (like self) accept no [column] indexes.", Thing_className (me));
		goto end;
	}
} break; case SELFMATRIKS2_: {
	long icol = floor (s [w --] + 0.5), irow = floor (s [w] + 0.5);   /* Round. */
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	if (our getMatrix == NULL) {
		Melder_error ("%s objects like \"self\" accept no [row, column] indexing.", Thing_className (me));
		goto end;
	}
	s [w] = our getMatrix (me, irow, icol);
} break; case SELFFUNKTIE1_: {
	double x = s [w];
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	if (our getFunction1) {
		s [w] = our getFunction1 (me, x);
	} else if (our getFunction2) {
		if (our getY == NULL) {
			Melder_error ("The current %s object (self) accepts no implicit y values.\n"
				"Try using both (x, y) arguments instead.", Thing_className (me));
			goto end;
		} else {
			double y = our getY (me, row);
			s [w] = our getFunction2 (me, x, y);
		}
	} else {
		Melder_error ("%s objects like \"self\" accept no (x) values.", Thing_className (me));
		goto end;
	}
} break; case SELFFUNKTIE2_: {
	double y = s [w --], x = s [w];
	if (me == NULL) { Melder_error ("The name \"self\" is restricted to formulas for objects."); goto end; }
	if (our getFunction2 == NULL) {
		Melder_error ("%s objects like \"self\" accept no (x, y) values.", Thing_className (me));
		goto end;
	}
	s [w] = our getFunction2 (me, x, y);
} break; case MATRIKS0_: {
	Data thee = f [i]. content.object;
	if (your getCell) {
		s [++ w] = your getCell (thee);
	} else if (your getVector) {
		if (col == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit column index for this %s object.\n"
				"Try using the [column] index explicitly.", Thing_className (thee));
			goto end;
		} else {
			s [++ w] = your getVector (thee, col);
		}
	} else if (your getMatrix) {
		if (row == 0) {
			if (col == 0) {
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
			s [++ w] = your getMatrix (thee, row, col);
		}
	} else {
		Melder_error ("%s objects accept no [] indexing.", Thing_className (thee));
		goto end;
	}
} break; case MATRIKS1_: {
	Data thee = f [i]. content.object;
	long icol = floor (s [w] + 0.5);   /* Round. */
	if (your getVector) {
		s [w] = your getVector (thee, icol);
	} else if (your getMatrix) {
		if (row == 0) {
			Melder_error ("We are not in a loop,\n"
				"hence no implicit row index for this %s object.\n"
				"Try using both [row, column] indexes instead.", Thing_className (thee));
			goto end;
		} else {
			s [w] = your getMatrix (thee, row, icol);
		}
	} else {
		Melder_error ("%s objects accept no [column] indexes.", Thing_className (thee));
		goto end;
	}
} break; case MATRIKS2_: {
	Data thee = f [i]. content.object;
	long icol = floor (s [w --] + 0.5), irow = floor (s [w] + 0.5);   /* Round. */
	if (your getMatrix == NULL) {
		Melder_error ("%s objects accept no [row, column] indexing.", Thing_className (thee));
		goto end;
	}
	s [w] = your getMatrix (thee, irow, icol);
} break; case FUNKTIE0_: {
	Data thee = f [i]. content.object;
	if (your getFunction0) {
		s [++ w] = your getFunction0 (thee);
	} else if (your getFunction1) {
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
			double x = our getX (me, col);
			s [++ w] = your getFunction1 (thee, x);
		}
	} else if (your getFunction2) {
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
			double x = our getX (me, col);
			if (our getY == NULL) {
				Melder_error ("The current %s object gives no implicit y values,\n"
					"hence no implicit y value for this %s object.\n"
					"Try using the (y) argument explicitly.", Thing_className (me), Thing_className (thee));
				goto end;
			} else {
				double y = our getY (me, row);
				s [++ w] = your getFunction2 (thee, x, y);
			}
		}
	} else {
		Melder_error ("%s objects accept no () values.", Thing_className (thee));
		goto end;
	}
} break; case FUNKTIE1_: {
	Data thee = f [i]. content.object;
	double x = s [w];
	if (your getFunction1) {
		s [w] = your getFunction1 (thee, x);
	} else if (your getFunction2) {
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
			double y = our getY (me, row);
			s [w] = your getFunction2 (thee, x, y);
		}
	} else {
		Melder_error ("%s objects accept no (x) values.", Thing_className (thee));
		goto end;
	}
} break; case FUNKTIE2_: {
	Data thee = f [i]. content.object;
	double y = s [w --], x = s [w];
	if (your getFunction2 == NULL) {
		Melder_error ("%s objects accept no (x, y) values.", Thing_className (thee));
		goto end;
	}
	s [w] = your getFunction2 (thee, x, y);
} break; case ROW_INDEX_: {
	Data thee = f [i]. content.object;
	double rowIndex;
	if (your getRowIndex == NULL) { Melder_error ("No row labels for this object."); goto end; }
	rowIndex = your getRowIndex (thee, ss [sw]);
	if (rowIndex == 0) { Melder_error ("No row \"%s\" for this object.", ss [sw]); goto end; }
	s [++ w] = rowIndex;
	Melder_free (ss [sw]); -- sw;
} break; case COLUMN_INDEX_: {
	Data thee = f [i]. content.object;
	double columnIndex;
	if (your getColumnIndex == NULL) { Melder_error ("No column labels for this object."); goto end; }
	columnIndex = your getColumnIndex (thee, ss [sw]);
	if (columnIndex == 0) { Melder_error ("No column \"%s\" for this object.", ss [sw]); goto end; }
	s [++ w] = columnIndex;
	Melder_free (ss [sw]); -- sw;
} break; case SQR_: {
	s [w] = s [w] == NUMundefined ? NUMundefined : s [w] * s [w];
} break; case STRING_: {
	++ sw; string = f [i]. content.string;
	if (ss [sw] != NULL && strlen (string) <= strlen (ss [sw])) {
		strcpy (ss [sw], string);
	} else {
		Melder_free (ss [sw]);
		ss [sw] = Melder_strdup (string);
	}
} break; case STRING_EQ_: {
	s [++ w] = strequ (ss [sw - 1], ss [sw]) ? 1.0 : 0.0;
	Melder_free (ss [sw]); -- sw; Melder_free (ss [sw]); -- sw;
} break; case STRING_NE_: {
	s [++ w] = strequ (ss [sw - 1], ss [sw]) ? 0.0 : 1.0;
	Melder_free (ss [sw]); -- sw; Melder_free (ss [sw]); -- sw;
} break; case STRING_LE_: {
	s [++ w] = strcmp (ss [sw - 1], ss [sw]) <= 0 ? 1.0 : 0.0;
	Melder_free (ss [sw]); -- sw; Melder_free (ss [sw]); -- sw;
} break; case STRING_LT_: {
	s [++ w] = strcmp (ss [sw - 1], ss [sw]) < 0 ? 1.0 : 0.0;
	Melder_free (ss [sw]); -- sw; Melder_free (ss [sw]); -- sw;
} break; case STRING_GE_: {
	s [++ w] = strcmp (ss [sw - 1], ss [sw]) >= 0 ? 1.0 : 0.0;
	Melder_free (ss [sw]); -- sw; Melder_free (ss [sw]); -- sw;
} break; case STRING_GT_: {
	s [++ w] = strcmp (ss [sw - 1], ss [sw]) > 0 ? 1.0 : 0.0;
	Melder_free (ss [sw]); -- sw; Melder_free (ss [sw]); -- sw;
} break; case STRING_ADD_: {
	long length1 = strlen (ss [sw - 1]), length2 = strlen (ss [sw]);
	char *string = Melder_malloc (length1 + length2 + 1); cherror
	strcpy (string, ss [sw - 1]);
	strcpy (string + length1, ss [sw]);
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	ss [-- sw] = string;
} break; case STRING_SUB_: {
	long length1 = strlen (ss [sw - 1]), length2 = strlen (ss [sw]), newlength = length1 - length2;
	char *string;
	if (newlength >= 0 && strnequ (ss [sw - 1] + newlength, ss [sw], length2)) {
		string = Melder_malloc (newlength + 1); if (! string) return 0;
		strncpy (string, ss [sw - 1], newlength);
		string [newlength] = '\0';
	} else {
		string = Melder_strdup (ss [sw - 1]); if (! string) return 0;
	}
	Melder_free (ss [sw - 1]);
	Melder_free (ss [sw]);
	ss [-- sw] = string;
} break; case NUMERIC_VARIABLE_: {
	InterpreterVariable var = f [i]. content.variable;
	s [++ w] = var -> numericValue;
} break; case STRING_VARIABLE_: {
	InterpreterVariable var = f [i]. content.variable;
	string = var -> stringValue;
	++ sw;
	if (ss [sw] != NULL && strlen (string) <= strlen (ss [sw])) {   /* Try an optimization. */
		strcpy (ss [sw], string);
	} else {
		Melder_free (ss [sw]);
		ss [sw] = Melder_strdup (string); cherror
	}
} break; case NUMERIC_COLUMN_: {
	s [++ w] = our getMatrix (me, row, f [i]. content.number);
} break; case STRING_COLUMN_: {
	++ sw;
	Melder_free (ss [sw]);
	/*ss [sw] = Melder_strdup (our getMatrixStr (me, row, f [i]. content.number));*/
} break; case SELECTED2_: {
	void *klas = Thing_classFromClassName (ss [sw]); cherror
	s [w] = praat_getIdOfSelected (klas, s [w]); cherror
	Melder_free (ss [sw]); -- sw;
} break; case SELECTEDSTR2_: {
	char *name;
	void *klas = Thing_classFromClassName (ss [sw]); cherror
	name = praat_getNameOfSelected (klas, s [w --]); cherror
	Melder_free (ss [sw]);
	ss [sw] = Melder_strdup (name); cherror
} break; default: return Melder_error ("Symbol \"%s\" without action.", Formula_instructionNames [f [i]. symbol]);
		} /* switch */
		i ++;
	} /* while */
	if (theExpressionType == EXPRESSION_TYPE_NUMERIC) {
		Melder_assert (w == 1 && sw == 0);
		if (numericResult) *numericResult = s [1];
		else Melder_information ("%s", Melder_double (s [1]));
	} else if (theExpressionType == EXPRESSION_TYPE_STRING) {
		Melder_assert (w == 0 && sw == 1);
		if (stringResult) { *stringResult = ss [1]; ss [1] = NULL; }   /* Undangle. */
		else { Melder_information ("%s", ss [1]); Melder_free (ss [1]); }
	} else {
		Melder_assert (theExpressionType == EXPRESSION_TYPE_UNKNOWN);
		if (w == 1 && sw == 0) {
			if (numericResult) *numericResult = s [1];
			else Melder_information ("%s", Melder_double (s [1]));
		} else {
			Melder_assert (w == 0 && sw == 1);
			if (stringResult) { *stringResult = ss [1]; ss [1] = NULL; }   /* Undangle. */
			else { Melder_information ("%s", ss [1]); Melder_free (ss [1]); }
		}
	}
end:
	iferror {
		/*
			Clean up the remaining string stack.
		*/
		for (sw += 3; sw > 0; sw --)
			Melder_free (ss [sw]);
		return 0;
	}
	return 1;
}

/* End of file Formula.c */
