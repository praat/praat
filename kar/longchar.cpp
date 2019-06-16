/* longchar.cpp
 *
 * Copyright (C) 1992-2009,2011-2019 Paul Boersma
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

/*
 * pb 2002/11/17 removed bug that genericized to non-Roman glyphs (thanks Mietta Lennes)
 * pb 2003/08/20 added five phonetic Greek characters
 * pb 2004/12/01 added many bitmaps
 * pb 2004/12/02 support for TIPA font (xwin column); unfortunately, several characters are missing (ringunder, diaeresisunder, tildeunder)
 * pb 2005/02/04 the IPA widths become those of the PostScript font TeX-xipa10-Praat-Regular
 * pb 2005/02/04 erev and kidneybean
 * pb 2005/03/08 ps encoding; SILIPA93 encoding for Windows and Mac
 * pb 2005/09/18 SILIPA93 widths for fontless EPS files, including bold
 * pb 2006/11/17 Unicode
 * pb 2006/12/05 first wchar support
 * pb 2006/12/15 stress marks
 * pb 2007/08/08 Longchar_genericizeW
 * pb 2008/02/27 \d- and \D-
 * pb 2009/03/24 removed bug that caused Longchar_getInfoFromNative to be able to work with uninitialized table
 * pb 2011/04/06 C++
 * pb 2015/05/30 char32_t
 */

#include "longchar.h"
#include "UnicodeData.h"

static struct structLongchar_Info Longchar_database [] = {

/* Space. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ ' ', ' ', 0, 0, { "/space",          250, 250, 250, 250,  278, 278,  250, 250, 250, 250 },  32,  32,  32,  32, UNICODE_SPACE },

/* Letters. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'a', ' ', 0, 0, { "/a",              444, 500, 500, 500,  556, 556,  500, 500, 444, 556 },  97,  97,  97,  97, UNICODE_LATIN_SMALL_LETTER_A },
{ 'b', ' ', 0, 0, { "/b",              500, 556, 500, 500,  556, 611,  553, 611, 463, 537 },  98,  98,  98,  98, UNICODE_LATIN_SMALL_LETTER_B },
{ 'c', ' ', 0, 0, { "/c",              444, 444, 444, 444,  500, 556,  444, 444, 407, 444 },  99,  99,  99,  99, UNICODE_LATIN_SMALL_LETTER_C },
{ 'd', ' ', 0, 0, { "/d",              500, 556, 500, 500,  556, 611,  611, 611, 500, 556 }, 100, 100, 100, 100, UNICODE_LATIN_SMALL_LETTER_D },
{ 'e', ' ', 0, 0, { "/e",              444, 444, 444, 444,  556, 556,  479, 500, 389, 444 }, 101, 101, 101, 101, UNICODE_LATIN_SMALL_LETTER_E },
{ 'f', ' ', 0, 0, { "/f",              333, 333, 278, 333,  278, 333,  333, 389, 278, 333 }, 102, 102, 102, 102, UNICODE_LATIN_SMALL_LETTER_F },
{ 'g', ' ', 0, 0, { "/g",              500, 500, 500, 500,  556, 611,  556, 556, 500, 500 }, 103, 103, 103, 103, UNICODE_LATIN_SMALL_LETTER_G },
{ 'h', ' ', 0, 0, { "/h",              500, 556, 500, 556,  556, 611,  582, 611, 500, 556 }, 104, 104, 104, 104, UNICODE_LATIN_SMALL_LETTER_H },
{ 'i', ' ', 0, 0, { "/i",              278, 278, 278, 278,  222, 278,  291, 333, 278, 333 }, 105, 105, 105, 105, UNICODE_LATIN_SMALL_LETTER_I },
{ 'j', ' ', 0, 0, { "/j",              278, 333, 278, 278,  222, 278,  234, 333, 278, 333 }, 106, 106, 106, 106, UNICODE_LATIN_SMALL_LETTER_J },
{ 'k', ' ', 0, 0, { "/k",              500, 556, 444, 500,  500, 556,  556, 611, 444, 556 }, 107, 107, 107, 107, UNICODE_LATIN_SMALL_LETTER_K },
{ 'l', ' ', 0, 0, { "/l",              278, 278, 278, 278,  222, 278,  291, 333, 278, 333 }, 108, 108, 108, 108, UNICODE_LATIN_SMALL_LETTER_L },
{ 'm', ' ', 0, 0, { "/m",              778, 833, 722, 778,  833, 889,  883, 889, 778, 833 }, 109, 109, 109, 109, UNICODE_LATIN_SMALL_LETTER_M },
{ 'n', ' ', 0, 0, { "/n",              500, 556, 500, 556,  556, 611,  582, 611, 556, 556 }, 110, 110, 110, 110, UNICODE_LATIN_SMALL_LETTER_N },
{ 'o', ' ', 0, 0, { "/o",              500, 500, 500, 500,  556, 611,  546, 556, 444, 556 }, 111, 111, 111, 111, UNICODE_LATIN_SMALL_LETTER_O },
{ 'p', ' ', 0, 0, { "/p",              500, 556, 500, 500,  556, 611,  601, 611, 500, 556 }, 112, 112, 112, 112, UNICODE_LATIN_SMALL_LETTER_P },
{ 'q', ' ', 0, 0, { "/q",              500, 556, 500, 500,  556, 611,  560, 611, 463, 537 }, 113, 113, 113, 113, UNICODE_LATIN_SMALL_LETTER_Q },
{ 'r', ' ', 0, 0, { "/r",              333, 444, 389, 389,  333, 389,  395, 389, 389, 389 }, 114, 114, 114, 114, UNICODE_LATIN_SMALL_LETTER_R },
{ 's', ' ', 0, 0, { "/s",              389, 389, 389, 389,  500, 556,  424, 444, 389, 444 }, 115, 115, 115, 115, UNICODE_LATIN_SMALL_LETTER_S },
{ 't', ' ', 0, 0, { "/t",              278, 333, 278, 278,  278, 333,  326, 333, 333, 389 }, 116, 116, 116, 116, UNICODE_LATIN_SMALL_LETTER_T },
{ 'u', ' ', 0, 0, { "/u",              500, 556, 500, 556,  556, 611,  603, 611, 556, 556 }, 117, 117, 117, 117, UNICODE_LATIN_SMALL_LETTER_U },
{ 'v', ' ', 0, 0, { "/v",              500, 500, 444, 444,  500, 556,  565, 556, 500, 556 }, 118, 118, 118, 118, UNICODE_LATIN_SMALL_LETTER_V },
{ 'w', ' ', 0, 0, { "/w",              722, 722, 667, 667,  722, 778,  834, 833, 722, 833 }, 119, 119, 119, 119, UNICODE_LATIN_SMALL_LETTER_W },
{ 'x', ' ', 0, 0, { "/x",              500, 500, 444, 500,  500, 556,  516, 500, 500, 500 }, 120, 120, 120, 120, UNICODE_LATIN_SMALL_LETTER_X },
{ 'y', ' ', 0, 0, { "/y",              500, 500, 444, 444,  500, 556,  556, 556, 500, 556 }, 121, 121, 121, 121, UNICODE_LATIN_SMALL_LETTER_Y },
{ 'z', ' ', 0, 0, { "/z",              444, 444, 389, 389,  500, 500,  500, 500, 444, 500 }, 122, 122, 122, 122, UNICODE_LATIN_SMALL_LETTER_Z },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'A', ' ', 0, 0, { "/A",              722, 722, 611, 667,  667, 722,  778, 778, 722, 722 },  65,  65,  65,  65, UNICODE_LATIN_CAPITAL_LETTER_A },
{ 'B', ' ', 0, 0, { "/B",              667, 667, 611, 667,  667, 722,  611, 667, 611, 667 },  66,  66,  66,  66, UNICODE_LATIN_CAPITAL_LETTER_B },
{ 'C', ' ', 0, 0, { "/C",              667, 722, 667, 667,  722, 722,  709, 722, 667, 685 },  67,  67,  67,  67, UNICODE_LATIN_CAPITAL_LETTER_C },
{ 'D', ' ', 0, 0, { "/D",              722, 722, 722, 722,  722, 722,  774, 833, 778, 778 },  68,  68,  68,  68, UNICODE_LATIN_CAPITAL_LETTER_D },
{ 'E', ' ', 0, 0, { "/E",              611, 667, 611, 667,  667, 667,  611, 611, 611, 611 },  69,  69,  69,  69, UNICODE_LATIN_CAPITAL_LETTER_E },
{ 'F', ' ', 0, 0, { "/F",              556, 611, 611, 667,  611, 611,  556, 556, 556, 556 },  70,  70,  70,  70, UNICODE_LATIN_CAPITAL_LETTER_F },
{ 'G', ' ', 0, 0, { "/G",              722, 778, 722, 722,  778, 778,  763, 833, 722, 778 },  71,  71,  71,  71, UNICODE_LATIN_CAPITAL_LETTER_G },
{ 'H', ' ', 0, 0, { "/H",              722, 778, 722, 778,  722, 722,  832, 833, 778, 778 },  72,  72,  72,  72, UNICODE_LATIN_CAPITAL_LETTER_H },
{ 'I', ' ', 0, 0, { "/I",              333, 389, 333, 389,  278, 278,  337, 389, 333, 389 },  73,  73,  73,  73, UNICODE_LATIN_CAPITAL_LETTER_I },
{ 'J', ' ', 0, 0, { "/J",              389, 500, 444, 500,  500, 556,  333, 389, 333, 389 },  74,  74,  74,  74, UNICODE_LATIN_CAPITAL_LETTER_J },
{ 'K', ' ', 0, 0, { "/K",              722, 778, 667, 667,  667, 722,  726, 778, 667, 722 },  75,  75,  75,  75, UNICODE_LATIN_CAPITAL_LETTER_K },
{ 'L', ' ', 0, 0, { "/L",              611, 667, 556, 611,  556, 611,  611, 611, 556, 611 },  76,  76,  76,  76, UNICODE_LATIN_CAPITAL_LETTER_L },
{ 'M', ' ', 0, 0, { "/M",              889, 944, 833, 889,  833, 833,  946,1000, 944, 944 },  77,  77,  77,  77, UNICODE_LATIN_CAPITAL_LETTER_M },
{ 'N', ' ', 0, 0, { "/N",              722, 722, 667, 722,  722, 722,  831, 833, 778, 778 },  78,  78,  78,  78, UNICODE_LATIN_CAPITAL_LETTER_N },
{ 'O', ' ', 0, 0, { "/O",              722, 778, 722, 722,  778, 778,  786, 833, 778, 833 },  79,  79,  79,  79, UNICODE_LATIN_CAPITAL_LETTER_O },
{ 'P', ' ', 0, 0, { "/P",              556, 611, 611, 611,  667, 667,  604, 611, 611, 667 },  80,  80,  80,  80, UNICODE_LATIN_CAPITAL_LETTER_P },
{ 'Q', ' ', 0, 0, { "/Q",              722, 778, 722, 722,  778, 778,  786, 833, 778, 833 },  81,  81,  81,  81, UNICODE_LATIN_CAPITAL_LETTER_Q },
{ 'R', ' ', 0, 0, { "/R",              667, 722, 611, 667,  722, 722,  668, 722, 667, 722 },  82,  82,  82,  82, UNICODE_LATIN_CAPITAL_LETTER_R },
{ 'S', ' ', 0, 0, { "/S",              556, 556, 500, 556,  667, 667,  525, 611, 556, 556 },  83,  83,  83,  83, UNICODE_LATIN_CAPITAL_LETTER_S },
{ 'T', ' ', 0, 0, { "/T",              611, 667, 556, 611,  611, 611,  613, 667, 611, 611 },  84,  84,  84,  84, UNICODE_LATIN_CAPITAL_LETTER_T },
{ 'U', ' ', 0, 0, { "/U",              722, 722, 722, 722,  722, 722,  778, 778, 778, 778 },  85,  85,  85,  85, UNICODE_LATIN_CAPITAL_LETTER_U },
{ 'V', ' ', 0, 0, { "/V",              722, 722, 611, 667,  667, 667,  722, 778, 722, 778 },  86,  86,  86,  86, UNICODE_LATIN_CAPITAL_LETTER_V },
{ 'W', ' ', 0, 0, { "/W",              944,1000, 833, 889,  944, 944, 1000,1000, 944,1000 },  87,  87,  87,  87, UNICODE_LATIN_CAPITAL_LETTER_W },
{ 'X', ' ', 0, 0, { "/X",              722, 722, 611, 667,  667, 667,  667, 667, 722, 722 },  88,  88,  88,  88, UNICODE_LATIN_CAPITAL_LETTER_X },
{ 'Y', ' ', 0, 0, { "/Y",              722, 722, 556, 611,  667, 667,  667, 667, 667, 611 },  89,  89,  89,  89, UNICODE_LATIN_CAPITAL_LETTER_Y },
{ 'Z', ' ', 0, 0, { "/Z",              611, 667, 556, 611,  611, 611,  667, 667, 667, 667 },  90,  90,  90,  90, UNICODE_LATIN_CAPITAL_LETTER_Z },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '.', ' ', 0, 0, { "/period",         250, 250, 250, 250,  278, 278,  250, 250, 250, 250 },  46,  46,  46,  46, UNICODE_FULL_STOP },
{ ',', ' ', 0, 0, { "/comma",          250, 250, 250, 250,  278, 278,  250, 250, 250, 250 },  44,  44,  44,  44, UNICODE_COMMA },
{ ':', ' ', 0, 0, { "/colon",          278, 333, 333, 333,  278, 333,  250, 250, 250, 250 },  58,  58,  58,  58, UNICODE_COLON },
{ ';', ' ', 0, 0, { "/semicolon",      278, 333, 333, 333,  278, 333,  250, 250, 250, 250 },  59,  59,  59,  59, UNICODE_SEMICOLON },
{ '!', ' ', 0, 0, { "/exclam",         333, 333, 333, 389,  278, 333,  278, 278, 333, 333 },  33,  33,  33,  33, UNICODE_EXCLAMATION_MARK },
{ '!', 'd', 0, 0, { "/exclamdown",     333, 333, 389, 389,  333, 333,  278, 278, 333, 333 }, 161, 161, 193, 193, UNICODE_INVERTED_EXCLAMATION_MARK },
{ '?', ' ', 0, 0, { "/question",       444, 500, 500, 500,  556, 611,  444, 444, 500, 444 },  63,  63,  63,  63, UNICODE_QUESTION_MARK },
{ '?', 'd', 0, 0, { "/questiondown",   444, 500, 500, 500,  611, 611,  444, 444, 500, 444 }, 191, 191, 192, 192, UNICODE_INVERTED_QUESTION_MARK },

/* Numbers. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '0', ' ', 0, 0, { "/zero",           500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  48,  48,  48,  48, UNICODE_DIGIT_ZERO },
{ '1', ' ', 0, 0, { "/one",            500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  49,  49,  49,  49, UNICODE_DIGIT_ONE },
{ '2', ' ', 0, 0, { "/two",            500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  50,  50,  50,  50, UNICODE_DIGIT_TWO },
{ '3', ' ', 0, 0, { "/three",          500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  51,  51,  51,  51, UNICODE_DIGIT_THREE },
{ '4', ' ', 0, 0, { "/four",           500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  52,  52,  52,  52, UNICODE_DIGIT_FOUR },
{ '5', ' ', 0, 0, { "/five",           500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  53,  53,  53,  53, UNICODE_DIGIT_FIVE },
{ '6', ' ', 0, 0, { "/six",            500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  54,  54,  54,  54, UNICODE_DIGIT_SIX },
{ '7', ' ', 0, 0, { "/seven",          500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  55,  55,  55,  55, UNICODE_DIGIT_SEVEN },
{ '8', ' ', 0, 0, { "/eight",          500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  56,  56,  56,  56, UNICODE_DIGIT_EIGHT },
{ '9', ' ', 0, 0, { "/nine",           500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  57,  57,  57,  57, UNICODE_DIGIT_NINE },

/* Parentheses, brackets, braces. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '(', ' ', 0, 0, { "/parenleft",      333, 333, 333, 333,  333, 333,  333, 333, 333, 333 },  40,  40,  40,  40, UNICODE_LEFT_PARENTHESIS },
{ ')', ' ', 0, 0, { "/parenright",     333, 333, 333, 333,  333, 333,  333, 333, 333, 333 },  41,  41,  41,  41, UNICODE_RIGHT_PARENTHESIS },
{ '[', ' ', 0, 0, { "/bracketleft",    333, 333, 389, 333,  278, 333,  333, 333, 333, 333 },  91,  91,  91,  91, UNICODE_LEFT_SQUARE_BRACKET },
{ ']', ' ', 0, 0, { "/bracketright",   333, 333, 389, 333,  278, 333,  333, 333, 333, 333 },  93,  93,  93,  93, UNICODE_RIGHT_SQUARE_BRACKET },
{ '{', ' ', 0, 0, { "/braceleft",      480, 394, 400, 348,  334, 389,  333, 310, 333, 333 }, 123, 123, 123, 123, UNICODE_LEFT_CURLY_BRACKET },
{ '}', ' ', 0, 0, { "/braceright",     480, 394, 400, 348,  334, 389,  333, 310, 333, 333 }, 125, 125, 125, 125, UNICODE_RIGHT_CURLY_BRACKET },

{ '+', ' ', 0, 0, { "/plus",           564, 570, 675, 570,  584, 584,  606, 606, 606, 606 },  43,  43,  43,  43, UNICODE_PLUS_SIGN },
//{ '-', ' ', 0, 0, { "/minus",          564, 570, 675, 606,  584, 584,  606, 606, 606, 606 },  45,  45, 208,  45, UNICODE_HYPHEN_MINUS }, /* Add minus to ps */
{ '-', ' ', 0, 0, { "/hyphen",         333, 333, 333, 333,  333, 333,  333, 333, 333, 389 }, 173, 173,  45,  45, UNICODE_HYPHEN_MINUS }, /* Add minus to ps */
{ '-', 'h', 0, 0, { "/hyphen",         333, 333, 333, 333,  333, 333,  333, 333, 333, 389 }, 173, 173,  45,  45, UNICODE_SOFT_HYPHEN },
{ '-', '-', 0, 0, { "/endash",         500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  45,  45, 208, 208, UNICODE_EN_DASH },
{ '+', '-', 0, 0, { "/plusminus",      564, 570, 675, 570,  584, 584,  606, 606, 606, 606 }, 177, 177, 177, 177, UNICODE_PLUS_MINUS_SIGN },
{ '*', ' ', 0, 0, { "/asterisk",       500, 500, 500, 500,  389, 389,  389, 444, 389, 444 },  42,  42,  42,  42, UNICODE_ASTERISK },
{ '/', ' ', 0, 0, { "/slash",          278, 278, 278, 278,  278, 278,  606, 296, 296, 315 },  47,  47,  47,  47, UNICODE_SOLIDUS },

/* Comparison. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '<', ' ', 0, 0, { "/less",           564, 570, 675, 570,  584, 584,  606, 606, 606, 606 },  60,  60,  60,  60, UNICODE_LESS_THAN_SIGN },
{ '=', ' ', 0, 0, { "/equal",          564, 570, 675, 570,  584, 584,  606, 606, 606, 606 },  61,  61,  61,  61, UNICODE_EQUALS_SIGN },
{ '>', ' ', 0, 0, { "/greater",        564, 570, 675, 570,  584, 584,  606, 606, 606, 606 },  62,  62,  62,  62, UNICODE_GREATER_THAN_SIGN },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '#', ' ', 0, 0, { "/numbersign",     500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  35,  35,  35,  35, UNICODE_NUMBER_SIGN },
{ '$', ' ', 0, 0, { "/dollar",         500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  36,  36,  36,  36, UNICODE_DOLLAR_SIGN },
{ '%', ' ', 0, 0, { "/percent",        833,1000, 833, 833,  889, 889,  840, 889, 889, 889 },  37,  37,  37,  37, UNICODE_PERCENT_SIGN },
{ '&', ' ', 0, 0, { "/ampersand",      778, 833, 778, 778,  667, 722,  778, 833, 778, 833 },  38,  38,  38,  38, UNICODE_AMPERSAND },
{ '@', ' ', 0, 0, { "/at",             921, 930, 920, 832, 1015, 975,  747, 747, 747, 833 },  64,  64,  64,  64, UNICODE_COMMERCIAL_AT },
{ 'b', 's', 0, 0, { "/backslash",      278, 278, 278, 278,  278, 278,  606, 606, 606, 606 },  92,  92,  92,  92, UNICODE_REVERSE_SOLIDUS },
{ '_', ' ', 0, 0, { "/underscore",     500, 500, 500, 500,  556, 556,  500, 500, 500, 500 },  95,  95,  95,  95, UNICODE_LOW_LINE },
{ '^', ' ', 0, 0, { "/asciicircum",    469, 581, 422, 570,  469, 584,  606, 606, 606, 606 },  94,  94,  94,  94, UNICODE_CIRCUMFLEX_ACCENT },
{ '|', ' ', 0, 0, { "/bar",            200, 220, 275, 220,  260, 280,  606, 606, 606, 606 }, 124, 124, 124, 124, UNICODE_VERTICAL_LINE },
{ '~', ' ', 0, 0, { "/asciitilde",     541, 520, 541, 570,  584, 584,  606, 606, 606, 606 }, 126, 126, 126, 126, UNICODE_TILDE },

/* Quotes: conversion. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '`', ' ', 0, 0, { "/quotesinglleft", 333, 333, 333, 333,  222, 278,  278, 278, 278, 278 },  96,  96, 212, 212, UNICODE_LEFT_SINGLE_QUOTATION_MARK },
{'\'', ' ', 0, 0, { "/quotesinglright",333, 333, 333, 333,  222, 278,  278, 278, 278, 278 },  39,  39, 213, 213, UNICODE_RIGHT_SINGLE_QUOTATION_MARK },
{'\'', 'a', 0, 0, { "/apostrophe",     333, 333, 333, 333,  222, 278,  278, 278, 278, 278 },  39,  39,  39,  39, UNICODE_APOSTROPHE },
{'\"', 'l', 0, 0, { "/quotedblleft",   444, 500, 556, 500,  333, 500,  500, 500, 500, 500 },  34,  34, 210, 210, UNICODE_LEFT_DOUBLE_QUOTATION_MARK },
{'\"', 'r', 0, 0, { "/quotedblright",  444, 500, 556, 500,  333, 500,  500, 500, 500, 500 },  34,  34, 211, 211, UNICODE_RIGHT_DOUBLE_QUOTATION_MARK },
{'\"', ' ', 0, 0, { "/quotedbl",       408, 555, 420, 555,  355, 474,  371, 402, 500, 500 },  34,  34,  34,  34, UNICODE_QUOTATION_MARK },
{ '<', '<', 0, 0, { "/guillemotleft",  500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 171, 171, 199, 199, UNICODE_LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK },
{ '>', '>', 0, 0, { "/guillemotright", 500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 187, 187, 200, 200, UNICODE_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK },

/* Accented letters. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'a', '`', 0, 0, { "/agrave",         444, 500, 500, 500,  556, 556,  500, 500, 444, 556 }, 224, 224, 136, 136, UNICODE_LATIN_SMALL_LETTER_A_WITH_GRAVE },
{ 'a','\'', 0, 0, { "/aacute",         444, 500, 500, 500,  556, 556,  500, 500, 444, 556 }, 225, 225, 135, 135, UNICODE_LATIN_SMALL_LETTER_A_WITH_ACUTE },
{ 'a', '^', 0, 0, { "/acircumflex",    444, 500, 500, 500,  556, 556,  500, 500, 444, 556 }, 226, 226, 137, 137, UNICODE_LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX },
{ 'a', '~', 0, 0, { "/atilde",         444, 500, 500, 500,  556, 556,  500, 500, 444, 556 }, 227, 227, 139, 139, UNICODE_LATIN_SMALL_LETTER_A_WITH_TILDE },
{ 'a','\"', 0, 0, { "/adieresis",      444, 500, 500, 500,  556, 556,  500, 500, 444, 556 }, 228, 228, 138, 138, UNICODE_LATIN_SMALL_LETTER_A_WITH_DIAERESIS },
{ 'a', 'o', 0, 0, { "/aring",          444, 500, 500, 500,  556, 556,  500, 500, 444, 556 }, 229, 229, 140, 140, UNICODE_LATIN_SMALL_LETTER_A_WITH_RING_ABOVE },
{ 'a', ';', 0, 0, { "/aogonek",        444, 500, 500, 500,  556, 556,  500, 500, 444, 556 },   0,   0,   0,   2, UNICODE_LATIN_SMALL_LETTER_A_WITH_OGONEK },
{ 'c','\'', 0, 0, { "/cacute",         444, 444, 444, 444,  500, 556,  444, 444, 407, 444 },   0,   0,   0,   4, UNICODE_LATIN_SMALL_LETTER_C_WITH_ACUTE },
{ 'c', '<', 0, 0, { "/ccaron",         444, 444, 444, 444,  500, 556,  444, 444, 407, 444 },   0,   0,   0,   6, UNICODE_LATIN_SMALL_LETTER_C_WITH_CARON },
{ 'd', '<', 0, 0, { "/dcaron",         500, 556, 500, 500,  556, 611,  611, 611, 500, 556 },   0,   0,   0,   8, UNICODE_LATIN_SMALL_LETTER_D_WITH_CARON },
{ 'd', '-', 0, 0, { "/dbar",           500, 556, 500, 500,  556, 611,  611, 611, 500, 556 },   0,   0,   0,  10, UNICODE_LATIN_SMALL_LETTER_D_WITH_STROKE },
{ 'e', '`', 0, 0, { "/egrave",         444, 444, 444, 444,  556, 556,  479, 500, 389, 444 }, 232, 232, 143, 143, UNICODE_LATIN_SMALL_LETTER_E_WITH_GRAVE },
{ 'e','\'', 0, 0, { "/eacute",         444, 444, 444, 444,  556, 556,  479, 500, 389, 444 }, 233, 233, 142, 142, UNICODE_LATIN_SMALL_LETTER_E_WITH_ACUTE },
{ 'e', '^', 0, 0, { "/ecircumflex",    444, 444, 444, 444,  556, 556,  479, 500, 389, 444 }, 234, 234, 144, 144, UNICODE_LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX },
{ 'e','\"', 0, 0, { "/edieresis",      444, 444, 444, 444,  556, 556,  479, 500, 389, 444 }, 235, 235, 145, 145, UNICODE_LATIN_SMALL_LETTER_E_WITH_DIAERESIS },
{ 'e', '<', 0, 0, { "/ecaron",         444, 444, 444, 444,  556, 556,  479, 500, 389, 444 },   0,   0,   0,  12, UNICODE_LATIN_SMALL_LETTER_E_WITH_CARON },
{ 'e', ';', 0, 0, { "/eogonek",        444, 444, 444, 444,  556, 556,  479, 500, 389, 444 },   0,   0,   0,  14, UNICODE_LATIN_SMALL_LETTER_E_WITH_OGONEK },
{ 'g', '<', 0, 0, { "/gcaron",         500, 500, 500, 500,  556, 611,  556, 556, 500, 500 },   0,   0,   0,  16, UNICODE_LATIN_SMALL_LETTER_G_WITH_CARON },
{ 'i', '`', 0, 0, { "/igrave",         278, 278, 278, 278,  222, 278,  287, 333, 278, 333 }, 236, 236, 147, 147, UNICODE_LATIN_SMALL_LETTER_I_WITH_GRAVE },
{ 'i','\'', 0, 0, { "/iacute",         278, 278, 278, 278,  222, 278,  287, 333, 278, 333 }, 237, 237, 146, 146, UNICODE_LATIN_SMALL_LETTER_I_WITH_ACUTE },
{ 'i', '^', 0, 0, { "/icircumflex",    278, 278, 278, 278,  222, 278,  287, 333, 278, 333 }, 238, 238, 148, 148, UNICODE_LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX },
{ 'i','\"', 0, 0, { "/idieresis",      278, 278, 278, 278,  222, 278,  287, 333, 278, 333 }, 239, 239, 149, 149, UNICODE_LATIN_SMALL_LETTER_I_WITH_DIAERESIS },
{ 'l', '/', 0, 0, { "/lslash",         278, 278, 278, 278,  222, 278,  291, 333, 278, 333 },   0,   0,   0,  18, UNICODE_LATIN_SMALL_LETTER_L_WITH_STROKE },
{ 'n','\'', 0, 0, { "/nacute",         500, 556, 500, 556,  556, 611,  582, 611, 556, 556 },   0,   0,   0,  20, UNICODE_LATIN_SMALL_LETTER_N_WITH_ACUTE },
{ 'n', '~', 0, 0, { "/ntilde",         500, 556, 500, 556,  556, 611,  582, 611, 556, 556 }, 241, 241, 150, 150, UNICODE_LATIN_SMALL_LETTER_N_WITH_TILDE },
{ 'n', '<', 0, 0, { "/ncaron",         500, 556, 500, 556,  556, 611,  582, 611, 556, 556 },   0,   0,   0,  22, UNICODE_LATIN_SMALL_LETTER_N_WITH_CARON },
{ 'o', '`', 0, 0, { "/ograve",         500, 500, 500, 500,  556, 611,  546, 556, 444, 556 }, 242, 242, 152, 152, UNICODE_LATIN_SMALL_LETTER_O_WITH_GRAVE },
{ 'o','\'', 0, 0, { "/oacute",         500, 500, 500, 500,  556, 611,  546, 556, 444, 556 }, 243, 243, 151, 151, UNICODE_LATIN_SMALL_LETTER_O_WITH_ACUTE },
{ 'o', '^', 0, 0, { "/ocircumflex",    500, 500, 500, 500,  556, 611,  546, 556, 444, 556 }, 244, 244, 153, 153, UNICODE_LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX },
{ 'o', '~', 0, 0, { "/otilde",         444, 500, 500, 500,  556, 611,  546, 556, 444, 556 }, 245, 245, 155, 155, UNICODE_LATIN_SMALL_LETTER_O_WITH_TILDE },
{ 'o','\"', 0, 0, { "/odieresis",      500, 500, 500, 500,  556, 611,  546, 556, 444, 556 }, 246, 246, 154, 154, UNICODE_LATIN_SMALL_LETTER_O_WITH_DIAERESIS },
{ 'o', ':', 0, 0, { "/ohungarumlaut",  500, 500, 500, 500,  556, 611,  546, 556, 444, 556 },   0,   0,   0,  24, UNICODE_LATIN_SMALL_LETTER_O_WITH_DOUBLE_ACUTE },
{ 'r', '<', 0, 0, { "/rcaron",         333, 444, 389, 389,  333, 389,  395, 389, 389, 389 },   0,   0,   0,  26, UNICODE_LATIN_SMALL_LETTER_R_WITH_CARON },
{ 's','\'', 0, 0, { "/sacute",         389, 389, 389, 389,  500, 556,  424, 444, 389, 444 },   0,   0,   0,  28, UNICODE_LATIN_SMALL_LETTER_S_WITH_ACUTE },
{ 's', '<', 0, 0, { "/scaron",         389, 389, 389, 389,  500, 556,  424, 444, 389, 444 },   0,   0,   0,  30, UNICODE_LATIN_SMALL_LETTER_S_WITH_CARON },
{ 't', '<', 0, 0, { "/tcaron",         278, 333, 278, 278,  278, 333,  326, 333, 333, 389 },   0,   0,   0, 245, UNICODE_LATIN_SMALL_LETTER_T_WITH_CARON },
{ 'u', '`', 0, 0, { "/ugrave",         500, 556, 500, 556,  556, 611,  603, 611, 556, 556 }, 249, 249, 157, 157, UNICODE_LATIN_SMALL_LETTER_U_WITH_GRAVE },
{ 'u','\'', 0, 0, { "/uacute",         500, 556, 500, 556,  556, 611,  603, 611, 556, 556 }, 250, 250, 156, 156, UNICODE_LATIN_SMALL_LETTER_U_WITH_ACUTE },
{ 'u', '^', 0, 0, { "/ucircumflex",    500, 556, 500, 556,  556, 611,  603, 611, 556, 556 }, 251, 251, 158, 158, UNICODE_LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX },
{ 'u','\"', 0, 0, { "/udieresis",      500, 556, 500, 556,  556, 611,  603, 611, 556, 556 }, 252, 252, 159, 159, UNICODE_LATIN_SMALL_LETTER_U_WITH_DIAERESIS },
{ 'u', ':', 0, 0, { "/uhungarumlaut",  500, 556, 500, 556,  556, 611,  603, 611, 556, 556 },   0,   0,   0, 247, UNICODE_LATIN_SMALL_LETTER_U_WITH_DOUBLE_ACUTE },
{ 'u', 'o', 0, 0, { "/uring",          500, 556, 500, 556,  556, 611,  603, 611, 556, 556 },   0,   0,   0, 249, UNICODE_LATIN_SMALL_LETTER_U_WITH_RING_ABOVE },
{ 'y','\'', 0, 0, { "/yacute",         500, 500, 444, 444,  500, 556,  556, 556, 500, 556 }, 253, 253,   0, 251, UNICODE_LATIN_SMALL_LETTER_Y_WITH_ACUTE },
{ 'y','\"', 0, 0, { "/ydieresis",      500, 500, 444, 444,  500, 556,  556, 556, 500, 556 }, 255, 255, 216, 216, UNICODE_LATIN_SMALL_LETTER_Y_WITH_DIAERESIS },
{ 'z','\'', 0, 0, { "/zacute",         444, 444, 389, 389,  500, 500,  500, 500, 444, 500 },   0,   0,   0, 253, UNICODE_LATIN_SMALL_LETTER_Z_WITH_ACUTE },
{ 'z', '<', 0, 0, { "/zcaron",         444, 444, 389, 389,  500, 500,  500, 500, 444, 500 },   0,   0,   0, 255, UNICODE_LATIN_SMALL_LETTER_Z_WITH_CARON },
{ 'z', '!', 0, 0, { "/zdot",           444, 444, 389, 389,  500, 500,  500, 500, 444, 500 },   0,   0,   0, 202, UNICODE_LATIN_SMALL_LETTER_Z_WITH_DOT_ABOVE },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'A', '`', 0, 0, { "/Agrave",         722, 722, 611, 667,  667, 722,  778, 778, 722, 722 }, 192, 192, 203, 203, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_GRAVE },
{ 'A','\'', 0, 0, { "/Aacute",         722, 722, 611, 667,  667, 722,  778, 778, 722, 722 }, 193, 193, 231, 231, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_ACUTE },
{ 'A', '^', 0, 0, { "/Acircumflex",    722, 722, 611, 667,  667, 722,  778, 778, 722, 722 }, 194, 194, 229, 229, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX },
{ 'A', '~', 0, 0, { "/Atilde",         722, 722, 611, 667,  667, 722,  778, 778, 722, 722 }, 195, 195, 204, 204, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_TILDE },
{ 'A','\"', 0, 0, { "/Adieresis",      722, 722, 611, 667,  667, 722,  778, 778, 722, 722 }, 196, 196, 128, 128, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS },
{ 'A', 'o', 0, 0, { "/Aring",          722, 722, 611, 667,  667, 722,  778, 778, 722, 722 }, 197, 197, 129, 129, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE },
{ 'A', ';', 0, 0, { "/Aogonek",        722, 722, 611, 667,  667, 722,  778, 778, 722, 722 },   0,   0,   0,   1, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_OGONEK },
{ 'C','\'', 0, 0, { "/Cacute",         667, 722, 667, 667,  722, 722,  709, 722, 667, 685 },   0,   0,   0,   3, UNICODE_LATIN_CAPITAL_LETTER_C_WITH_ACUTE },
{ 'C', '<', 0, 0, { "/Ccaron",         667, 722, 667, 667,  722, 722,  709, 722, 667, 685 },   0,   0,   0,   5, UNICODE_LATIN_CAPITAL_LETTER_C_WITH_CARON },
{ 'D', '<', 0, 0, { "/Dcaron",         722, 722, 722, 722,  722, 722,  774, 833, 778, 778 },   0,   0,   0,   7, UNICODE_LATIN_CAPITAL_LETTER_D_WITH_CARON },
{ 'D', '-', 0, 0, { "/Dbar",           722, 722, 722, 722,  722, 722,  774, 833, 778, 778 },   0,   0,   0,   9, UNICODE_LATIN_CAPITAL_LETTER_D_WITH_STROKE },
{ 'E', '`', 0, 0, { "/Egrave",         611, 667, 611, 667,  667, 667,  611, 611, 611, 611 }, 200, 200, 233, 233, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_GRAVE },
{ 'E','\'', 0, 0, { "/Eacute",         611, 667, 611, 667,  667, 667,  611, 611, 611, 611 }, 201, 201, 131, 131, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_ACUTE },
{ 'E', '^', 0, 0, { "/Ecircumflex",    611, 667, 611, 667,  667, 667,  611, 611, 611, 611 }, 202, 202, 230, 230, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX },
{ 'E','\"', 0, 0, { "/Edieresis",      611, 667, 611, 667,  667, 667,  611, 611, 611, 611 }, 203, 203, 232, 232, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS },
{ 'E', '<', 0, 0, { "/Ecaron",         611, 667, 611, 667,  667, 667,  611, 611, 611, 611 },   0,   0,   0,  11, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_CARON },
{ 'E', ';', 0, 0, { "/Eogonek",        611, 667, 611, 667,  667, 667,  611, 611, 611, 611 },   0,   0,   0,  13, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_OGONEK },
{ 'G', '<', 0, 0, { "/Gcaron",         722, 778, 722, 722,  778, 778,  763, 833, 722, 778 },   0,   0,   0,  15, UNICODE_LATIN_CAPITAL_LETTER_G_WITH_CARON },
{ 'I', '`', 0, 0, { "/Igrave",         333, 389, 333, 389,  278, 278,  337, 389, 333, 389 }, 204, 204, 237, 237, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_GRAVE },
{ 'I','\'', 0, 0, { "/Iacute",         333, 389, 333, 389,  278, 278,  337, 389, 333, 389 }, 205, 205, 234, 234, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_ACUTE },
{ 'I', '^', 0, 0, { "/Icircumflex",    333, 389, 333, 389,  278, 278,  337, 389, 333, 389 }, 206, 206, 235, 235, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX },
{ 'I','\"', 0, 0, { "/Idieresis",      333, 389, 333, 389,  278, 278,  337, 389, 333, 389 }, 207, 207, 236, 236, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS },
{ 'L', '/', 0, 0, { "/Lslash",         611, 667, 556, 611,  556, 611,  611, 611, 556, 611 },   0,   0,   0,  17, UNICODE_LATIN_CAPITAL_LETTER_L_WITH_STROKE },
{ 'N','\'', 0, 0, { "/Nacute",         722, 722, 667, 722,  722, 722,  831, 833, 778, 778 },   0,   0,   0,  19, UNICODE_LATIN_CAPITAL_LETTER_N_WITH_ACUTE },
{ 'N', '~', 0, 0, { "/Ntilde",         722, 722, 667, 722,  722, 722,  831, 833, 778, 778 }, 209, 209, 132, 132, UNICODE_LATIN_CAPITAL_LETTER_N_WITH_TILDE },
{ 'N', '<', 0, 0, { "/Ncaron",         722, 722, 667, 722,  722, 722,  831, 833, 778, 778 },   0,   0,   0,  21, UNICODE_LATIN_CAPITAL_LETTER_N_WITH_CARON },
{ 'O', '`', 0, 0, { "/Ograve",         722, 778, 722, 722,  778, 778,  786, 833, 778, 833 }, 210, 210, 241, 241, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_GRAVE },
{ 'O','\'', 0, 0, { "/Oacute",         722, 778, 722, 722,  778, 778,  786, 833, 778, 833 }, 211, 211, 238, 238, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_ACUTE },
{ 'O', '^', 0, 0, { "/Ocircumflex",    722, 778, 722, 722,  778, 778,  786, 833, 778, 833 }, 212, 212, 239, 239, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX },
{ 'O', '~', 0, 0, { "/Otilde",         722, 778, 722, 722,  778, 778,  786, 833, 778, 833 }, 213, 213, 205, 205, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_TILDE },
{ 'O','\"', 0, 0, { "/Odieresis",      722, 778, 722, 722,  778, 778,  786, 833, 778, 833 }, 214, 214, 133, 133, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS },
{ 'O', ':', 0, 0, { "/Ohungarumlaut",  722, 778, 722, 722,  778, 778,  786, 833, 778, 833 },   0,   0,   0,  23, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_DOUBLE_ACUTE },
{ 'R', '<', 0, 0, { "/Rcaron",         667, 722, 611, 667,  722, 722,  668, 722, 667, 722 },   0,   0,   0,  25, UNICODE_LATIN_CAPITAL_LETTER_R_WITH_CARON },
{ 'S','\'', 0, 0, { "/Sacute",         556, 556, 500, 556,  667, 667,  525, 611, 556, 556 },   0,   0,   0,  27, UNICODE_LATIN_CAPITAL_LETTER_S_WITH_ACUTE },
{ 'S', '<', 0, 0, { "/Scaron",         556, 556, 500, 556,  667, 667,  525, 611, 556, 556 },   0,   0,   0,  29, UNICODE_LATIN_CAPITAL_LETTER_S_WITH_CARON },
{ 'T', '<', 0, 0, { "/Tcaron",         611, 667, 556, 611,  611, 611,  613, 667, 611, 611 },   0,   0,   0,  31, UNICODE_LATIN_CAPITAL_LETTER_T_WITH_CARON },
{ 'U', '`', 0, 0, { "/Ugrave",         722, 722, 722, 722,  722, 722,  778, 778, 778, 778 }, 217, 217, 244, 244, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_GRAVE },
{ 'U','\'', 0, 0, { "/Uacute",         722, 722, 722, 722,  722, 722,  778, 778, 778, 778 }, 218, 218, 242, 242, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_ACUTE },
{ 'U', '^', 0, 0, { "/Ucircumflex",    722, 722, 722, 722,  722, 722,  778, 778, 778, 778 }, 219, 219, 243, 243, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX },
{ 'U','\"', 0, 0, { "/Udieresis",      722, 722, 722, 722,  722, 722,  778, 778, 778, 778 }, 220, 220, 134, 134, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS },
{ 'U', ':', 0, 0, { "/Uhungarumlaut",  722, 722, 722, 722,  722, 722,  778, 778, 778, 778 },   0,   0,   0, 246, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_DOUBLE_ACUTE },
{ 'U', 'o', 0, 0, { "/Uring",          722, 722, 722, 722,  722, 722,  778, 778, 778, 778 },   0,   0,   0, 248, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_RING_ABOVE },
{ 'Y','\'', 0, 0, { "/Yacute",         722, 722, 556, 611,  667, 667,  667, 667, 667, 611 }, 221, 221,   0, 250, UNICODE_LATIN_CAPITAL_LETTER_Y_WITH_ACUTE },
{ 'Y','\"', 0, 0, { "/Ydieresis",      722, 722, 556, 611,  667, 667,  667, 667, 667, 611 },   0,   0, 217, 217, UNICODE_LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS },
{ 'Z','\'', 0, 0, { "/Zacute",         611, 667, 556, 611,  611, 611,  667, 667, 667, 667 },   0,   0,   0, 252, UNICODE_LATIN_CAPITAL_LETTER_Z_WITH_ACUTE },
{ 'Z', '<', 0, 0, { "/Zcaron",         611, 667, 556, 611,  611, 611,  667, 667, 667, 667 },   0,   0,   0, 254, UNICODE_LATIN_CAPITAL_LETTER_Z_WITH_CARON },
{ 'Z', '!', 0, 0, { "/Zdot",           611, 667, 556, 611,  611, 611,  667, 667, 667, 667 },   0,   0,   0, 129, UNICODE_LATIN_CAPITAL_LETTER_Z_WITH_DOT_ABOVE },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 's', 's', 0, 0, { "/germandbls",     500, 556, 500, 500,  611, 611,  556, 611, 500, 556 }, 223, 223, 167, 167, UNICODE_LATIN_SMALL_LETTER_SHARP_S }, /* Ringel-s. */
{ 'a', 'e', 0, 0, { "/ae",             667, 722, 667, 722,  889, 889,  758, 778, 638, 738 }, 230, 230, 190, 190, UNICODE_LATIN_SMALL_LETTER_AE }, /* ash */
{ 'c', ',', 0, 0, { "/ccedilla",       444, 444, 444, 444,  500, 556,  444, 444, 407, 444 }, 231, 231, 141, 141, UNICODE_LATIN_SMALL_LETTER_C_WITH_CEDILLA },
{ 'o', '/', 0, 0, { "/oslash",         500, 500, 500, 500,  611, 611,  556, 556, 444, 556 }, 248, 248, 191, 191, UNICODE_LATIN_SMALL_LETTER_O_WITH_STROKE },
{ 't', 'h', 0, 0, { "/thorn",          500, 556, 500, 500,  556, 611,  601, 611, 500, 556 }, 254, 254,   0,   0, UNICODE_LATIN_SMALL_LETTER_THORN },
{ 'A', 'e', 0, 0, { "/AE",             722,1000, 889, 944, 1000,1000,  944,1000, 941, 944 }, 198, 198, 174, 174, UNICODE_LATIN_CAPITAL_LETTER_AE }, /* Ash */
{ 'C', ',', 0, 0, { "/Ccedilla",       667, 722, 667, 667,  722, 722,  709, 722, 667, 685 }, 199, 199, 130, 130, UNICODE_LATIN_CAPITAL_LETTER_C_WITH_CEDILLA },
{ 'O', '/', 0, 0, { "/Oslash",         722, 778, 722, 722,  778, 778,  833, 833, 778, 833 }, 216, 216, 175, 175, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_STROKE },
{ 'T', 'h', 0, 0, { "/Thorn",          556, 611, 611, 611,  667, 667,  604, 611, 611, 667 }, 222, 222,   0,   0, UNICODE_LATIN_CAPITAL_LETTER_THORN },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '.', 'c', 0, 0, { "/periodcentered", 250, 250, 250, 250,  278, 278,  250, 250, 250, 250 }, 183, 183, 225, 225, UNICODE_MIDDLE_DOT },
{ 'd', 'g', 0, 0, { "/degree",         400, 400, 400, 400,  400, 400,  400, 400, 400, 400 }, 176, 176, 161, 161, UNICODE_DEGREE_SIGN },
{ 'c', '/', 0, 0, { "/cent",           500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 162, 162, 162, 162, UNICODE_CENT_SIGN },
{ 'L', 'p', 0, 0, { "/sterling",       500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 163, 163, 163, 163, UNICODE_POUND_SIGN },
{ 'c', 'u', 0, 0, { "/currency",       500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 164, 164, 219, 219, UNICODE_CURRENCY_SIGN },
{ 'e', 'u', 0, 0, { "/euro",           500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 164, 164, 219, 219, UNICODE_EURO_SIGN },   // = currency?
{ 'Y', '=', 0, 0, { "/yen",            500, 500, 500, 500,  556, 556,  500, 500, 500, 500 }, 165, 165, 180, 180, UNICODE_YEN_SIGN },
{ 'S', 'S', 0, 0, { "/section",        500, 500, 500, 500,  556, 556,  500, 500, 500, 556 }, 167, 167, 164, 164, UNICODE_SECTION_SIGN },
{ '|', '|', 0, 0, { "/paragraph",      453, 540, 523, 500,  537, 556,  628, 641, 500, 556 }, 182, 182, 166, 166, UNICODE_PILCROW_SIGN },
{ 'c', 'o', 0, 0, { "/copyright",      760, 747, 760, 747,  737, 737,  747, 747, 747, 747 }, 169, 169, 169, 169, UNICODE_COPYRIGHT_SIGN },
{ 'r', 'e', 0, 0, { "/registered",     760, 747, 760, 747,  737, 737,  747, 747, 747, 747 }, 174, 174, 168, 168, UNICODE_REGISTERED_SIGN },
{ 'a', '_', 0, 0, { "/ordfeminine",    276, 300, 276, 266,  370, 370,  333, 438, 333, 333 }, 170, 170, 187, 187, UNICODE_FEMININE_ORDINAL_INDICATOR },
{ 'o', '_', 0, 0, { "/ordmasculine",   310, 330, 310, 300,  365, 365,  333, 488, 333, 333 }, 186, 186, 188, 188, UNICODE_MASCULINE_ORDINAL_INDICATOR },

{ 'F', 'I', 0, 0, { "/fi",             556, 556, 500, 556,  500, 611,  605, 611, 528, 611 },   0,   0, 222, 222, UNICODE_LATIN_SMALL_LIGATURE_FI },
{ 'F', 'L', 0, 0, { "/fl",             556, 556, 500, 556,  500, 611,  608, 611, 545, 611 },   0,   0, 223, 223, UNICODE_LATIN_SMALL_LIGATURE_FL },

/* Greek. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'a', 'l', 1, 0, { "/alpha",          631, 0,   0,   0,    631, 0,    631, 0,   0,   0   },  97,  97,  97,  97, UNICODE_GREEK_SMALL_LETTER_ALPHA },
{ 'b', 'e', 1, 0, { "/beta",           549, 0,   0,   0,    549, 0,    549, 0,   0,   0   },  98,  98,  98,  98, UNICODE_GREEK_SMALL_LETTER_BETA },
{ 'g', 'a', 1, 0, { "/gamma",          411, 0,   0,   0,    411, 0,    411, 0,   0,   0   }, 103, 103, 103, 103, UNICODE_GREEK_SMALL_LETTER_GAMMA },
{ 'd', 'e', 1, 0, { "/delta",          494, 0,   0,   0,    494, 0,    494, 0,   0,   0   }, 100, 100, 100, 100, UNICODE_GREEK_SMALL_LETTER_DELTA },
{ 'e', 'p', 1, 0, { "/epsilon",        439, 0,   0,   0,    439, 0,    439, 0,   0,   0   }, 101, 101, 101, 101, UNICODE_GREEK_SMALL_LETTER_EPSILON },
{ 'z', 'e', 1, 0, { "/zeta",           494, 0,   0,   0,    494, 0,    494, 0,   0,   0   }, 122, 122, 122, 122, UNICODE_GREEK_SMALL_LETTER_ZETA },
{ 'e', 't', 1, 0, { "/eta",            603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 104, 104, 104, 104, UNICODE_GREEK_SMALL_LETTER_ETA },
{ 't', 'e', 1, 0, { "/theta",          521, 0,   0,   0,    521, 0,    521, 0,   0,   0   }, 113, 113, 113, 113, UNICODE_GREEK_SMALL_LETTER_THETA }, /* like obar */
{ 't', '2', 1, 0, { "/theta1",         631, 0,   0,   0,    631, 0,    631, 0,   0,   0   },  74,  74,  74,  74, UNICODE_GREEK_THETA_SYMBOL },   // curly
{ 'i', 'o', 1, 0, { "/iota",           329, 0,   0,   0,    329, 0,    329, 0,   0,   0   }, 105, 105, 105, 105, UNICODE_GREEK_SMALL_LETTER_IOTA },
{ 'k', 'a', 1, 0, { "/kappa",          549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 107, 107, 107, 107, UNICODE_GREEK_SMALL_LETTER_KAPPA },
{ 'l', 'a', 1, 0, { "/lambda",         549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 108, 108, 108, 108, UNICODE_GREEK_SMALL_LETTER_LAMDA },
{ 'm', 'u', 1, 0, { "/mu",             576, 0,   0,   0,    576, 0,    576, 0,   0,   0   }, 109, 109, 109, 109, UNICODE_GREEK_SMALL_LETTER_MU },
{ 'n', 'u', 1, 0, { "/nu",             521, 0,   0,   0,    521, 0,    521, 0,   0,   0   }, 110, 110, 110, 110, UNICODE_GREEK_SMALL_LETTER_NU },
{ 'x', 'i', 1, 0, { "/xi",             493, 0,   0,   0,    493, 0,    493, 0,   0,   0   }, 120, 120, 120, 120, UNICODE_GREEK_SMALL_LETTER_XI },
{ 'o', 'n', 1, 0, { "/omicron",        549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 111, 111, 111, 111, UNICODE_GREEK_SMALL_LETTER_OMICRON },
{ 'p', 'i', 1, 0, { "/pi",             549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 112, 112, 112, 112, UNICODE_GREEK_SMALL_LETTER_PI },
{ 'r', 'o', 1, 0, { "/rho",            549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 114, 114, 114, 114, UNICODE_GREEK_SMALL_LETTER_RHO },
{ 's', 'i', 1, 0, { "/sigma",          603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 115, 115, 115, 115, UNICODE_GREEK_SMALL_LETTER_SIGMA },
{ 's', '2', 1, 0, { "/sigma1",         439, 0,   0,   0,    439, 0,    439, 0,   0,   0   },  86,  86,  86,  86, UNICODE_GREEK_SMALL_LETTER_FINAL_SIGMA },
{ 't', 'a', 1, 0, { "/tau",            439, 0,   0,   0,    439, 0,    439, 0,   0,   0   }, 116, 116, 116, 116, UNICODE_GREEK_SMALL_LETTER_TAU },
{ 'u', 'p', 1, 0, { "/upsilon",        576, 0,   0,   0,    576, 0,    576, 0,   0,   0   }, 117, 117, 117, 117, UNICODE_GREEK_SMALL_LETTER_UPSILON },
{ 'f', 'i', 1, 0, { "/phi",            603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 106, 106, 106, 106, UNICODE_GREEK_SMALL_LETTER_PHI },   // curly
{ 'f', '2', 1, 0, { "/phi1",           521, 0,   0,   0,    521, 0,    521, 0,   0,   0   }, 102, 102, 102, 102, UNICODE_GREEK_PHI_SYMBOL }, /* like oslash */
{ 'c', 'i', 1, 0, { "/chi",            549, 0,   0,   0,    549, 0,    549, 0,   0,   0   },  99,  99,  99,  99, UNICODE_GREEK_SMALL_LETTER_CHI },
{ 'p', 's', 1, 0, { "/psi",            686, 0,   0,   0,    686, 0,    686, 0,   0,   0   }, 121, 121, 121, 121, UNICODE_GREEK_SMALL_LETTER_PSI },
{ 'o', 'm', 1, 0, { "/omega",          686, 0,   0,   0,    686, 0,    686, 0,   0,   0   }, 119, 119, 119, 119, UNICODE_GREEK_SMALL_LETTER_OMEGA },
{ 'o', '2', 1, 0, { "/omega1",         713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 118, 118, 118, 118, UNICODE_GREEK_PI_SYMBOL },

/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'A', 'l', 1, 0, { "/Alpha",          722, 0,   0,   0,    722, 0,    722, 0,   0,   0   },  65,  65,  65,  65, UNICODE_GREEK_CAPITAL_LETTER_ALPHA },
{ 'B', 'e', 1, 0, { "/Beta",           667, 0,   0,   0,    667, 0,    667, 0,   0,   0   },  66,  66,  66,  66, UNICODE_GREEK_CAPITAL_LETTER_BETA },
{ 'G', 'a', 1, 0, { "/Gamma",          603, 0,   0,   0,    603, 0,    603, 0,   0,   0   },  71,  71,  71,  71, UNICODE_GREEK_CAPITAL_LETTER_GAMMA },
{ 'D', 'e', 1, 0, { "/Delta",          612, 0,   0,   0,    612, 0,    612, 0,   0,   0   },  68,  68,  68,  68, UNICODE_GREEK_CAPITAL_LETTER_DELTA },
{ 'E', 'p', 1, 0, { "/Epsilon",        611, 0,   0,   0,    611, 0,    611, 0,   0,   0   },  69,  69,  69,  69, UNICODE_GREEK_CAPITAL_LETTER_EPSILON },
{ 'Z', 'e', 1, 0, { "/Zeta",           611, 0,   0,   0,    611, 0,    611, 0,   0,   0   },  90,  90,  90,  90, UNICODE_GREEK_CAPITAL_LETTER_ZETA },
{ 'E', 't', 1, 0, { "/Eta",            722, 0,   0,   0,    722, 0,    722, 0,   0,   0   },  72,  72,  72,  72, UNICODE_GREEK_CAPITAL_LETTER_ETA },
{ 'T', 'e', 1, 0, { "/Theta",          741, 0,   0,   0,    741, 0,    741, 0,   0,   0   },  81,  81,  81,  81, UNICODE_GREEK_CAPITAL_LETTER_THETA },
{ 'I', 'o', 1, 0, { "/Iota",           333, 0,   0,   0,    333, 0,    333, 0,   0,   0   },  73,  73,  73,  73, UNICODE_GREEK_CAPITAL_LETTER_IOTA },
{ 'K', 'a', 1, 0, { "/Kappa",          722, 0,   0,   0,    722, 0,    722, 0,   0,   0   },  75,  75,  75,  75, UNICODE_GREEK_CAPITAL_LETTER_KAPPA },
{ 'L', 'a', 1, 0, { "/Lambda",         686, 0,   0,   0,    686, 0,    686, 0,   0,   0   },  76,  76,  76,  76, UNICODE_GREEK_CAPITAL_LETTER_LAMDA },
{ 'M', 'u', 1, 0, { "/Mu",             889, 0,   0,   0,    889, 0,    889, 0,   0,   0   },  77,  77,  77,  77, UNICODE_GREEK_CAPITAL_LETTER_MU },
{ 'N', 'u', 1, 0, { "/Nu",             722, 0,   0,   0,    722, 0,    722, 0,   0,   0   },  78,  78,  78,  78, UNICODE_GREEK_CAPITAL_LETTER_NU },
{ 'X', 'i', 1, 0, { "/Xi",             645, 0,   0,   0,    645, 0,    645, 0,   0,   0   },  88,  88,  88,  88, UNICODE_GREEK_CAPITAL_LETTER_XI },
{ 'O', 'n', 1, 0, { "/Omicron",        722, 0,   0,   0,    722, 0,    722, 0,   0,   0   },  79,  79,  79,  79, UNICODE_GREEK_CAPITAL_LETTER_OMICRON },
{ 'P', 'i', 1, 0, { "/Pi",             768, 0,   0,   0,    768, 0,    768, 0,   0,   0   },  80,  80,  80,  80, UNICODE_GREEK_CAPITAL_LETTER_PI },
{ 'R', 'o', 1, 0, { "/Rho",            556, 0,   0,   0,    556, 0,    556, 0,   0,   0   },  82,  82,  82,  82, UNICODE_GREEK_CAPITAL_LETTER_RHO },
{ 'S', 'i', 1, 0, { "/Sigma",          592, 0,   0,   0,    592, 0,    592, 0,   0,   0   },  83,  83,  83,  83, UNICODE_GREEK_CAPITAL_LETTER_SIGMA },
{ 'T', 'a', 1, 0, { "/Tau",            611, 0,   0,   0,    611, 0,    611, 0,   0,   0   },  84,  84,  84,  84, UNICODE_GREEK_CAPITAL_LETTER_TAU },
{ 'U', 'p', 1, 0, { "/Upsilon",        690, 0,   0,   0,    690, 0,    690, 0,   0,   0   },  85,  85,  85,  85, UNICODE_GREEK_CAPITAL_LETTER_UPSILON },
{ 'F', 'i', 1, 0, { "/Phi",            763, 0,   0,   0,    763, 0,    763, 0,   0,   0   },  70,  70,  70,  70, UNICODE_GREEK_CAPITAL_LETTER_PHI },
{ 'C', 'i', 1, 0, { "/Chi",            722, 0,   0,   0,    722, 0,    722, 0,   0,   0   },  67,  67,  67,  67, UNICODE_GREEK_CAPITAL_LETTER_CHI },
{ 'P', 's', 1, 0, { "/Psi",            795, 0,   0,   0,    795, 0,    795, 0,   0,   0   },  89,  89,  89,  89, UNICODE_GREEK_CAPITAL_LETTER_PSI },
{ 'O', 'm', 1, 0, { "/Omega",          768, 0,   0,   0,    768, 0,    768, 0,   0,   0   },  87,  87,  87,  87, UNICODE_GREEK_CAPITAL_LETTER_OMEGA },

/* Hebrew. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ '?', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_ALEF },
{ 'B', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_BET },
{ 'G', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_GIMEL },
{ 'D', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_DALET },
{ 'H', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_HE },
{ 'V', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_VAV },
{ 'Z', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_ZAYIN },
{ 'X', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_HET },
{ 'Y', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_TET },
{ 'J', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_YOD },
{ 'K', '%', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_FINAL_KAF },
{ 'K', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_KAF },
{ 'L', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_LAMED },
{ 'M', '%', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_FINAL_MEM },
{ 'M', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_MEM },
{ 'N', '%', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_FINAL_NUN },
{ 'N', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_NUN },
{ 'S', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_SAMEKH },
{ '9', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_AYIN },
{ 'P', '%', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_FINAL_PE },
{ 'P', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_PE },
{ 'C', '%', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_FINAL_TSADI },
{ 'C', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_TSADI },
{ 'Q', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_QOF },
{ 'R', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_RESH },
{ 'W', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_SHIN },
{ 'T', '+', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_TAV },
{ 'h', 'I', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_HIRIQ },
{ 's', 'E', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_SEGOL },
{ 'c', 'E', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_TSERE },
{ 'q', 'A', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_QAMATS },
{ 'p', 'A', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_PATAH },
{ 'h', 'O', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_HOLAM },
{ 'v', 'O', 4, 0, { "",                500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_VAV_WITH_HOLAM },
{ 'q', 'U', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_QUBUTS },
{ 'd', 'q', 4, 1, { "",                  0,   0,   0,   0,    0,   0,    0,   0,   0,   0 },   0,   0,   0,   0, UNICODE_HEBREW_POINT_DAGESH_OR_MAPIQ },
{ 's', 'U', 4, 0, { "/shuruq",         500, 500, 500, 500,  500, 500,  500, 500, 500, 500 },   0,   0,   0,   0, UNICODE_HEBREW_LETTER_VAV_WITH_DAGESH },

/* Symbol. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 't', 'm', 1, 0, { "/trademarkserif", 890, 0,   0,   0,    890, 0,    890, 0,   0,   0   }, 212, 212, 212, 212, UNICODE_TRADE_MARK_SIGN },
{ 'T', 'M', 1, 0, { "/trademarksans",  786, 0,   0,   0,    786, 0,    786, 0,   0,   0   }, 228, 228, 228, 228, UNICODE_TRADE_MARK_SIGN },

{ 'n', 'o', 1, 0, { "/logicalnot",     713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 216, 216, 216, 215, UNICODE_NOT_SIGN },
{ 'x', 'x', 1, 0, { "/multiply",       549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 180, 180, 180, 180, UNICODE_MULTIPLICATION_SIGN },
{ ':', '-', 1, 0, { "/divide",         549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 184, 184, 184, 184, UNICODE_DIVISION_SIGN },
{ 'f', 'd', 1, 0, { "/florin",         500, 0,   0,   0,    500, 0,    500, 0,   0,   0   }, 166, 166, 166, 166, UNICODE_LATIN_SMALL_LETTER_F_WITH_HOOK },

{ 'b', 'u', 1, 0, { "/bullet",         460, 0,   0,   0,    460, 0,    460, 0,   0,   0   }, 183, 183, 183, 183, UNICODE_BULLET },
{'\'', 'p', 1, 0, { "/minute",         247, 0,   0,   0,    247, 0,    247, 0,   0,   0   }, 162, 162, 162, 162, UNICODE_PRIME },
{'\"', 'p', 1, 0, { "/second",         411, 0,   0,   0,    411, 0,    411, 0,   0,   0   }, 178, 178, 178, 178, UNICODE_DOUBLE_PRIME },

{ 'A', 't', 1, 0, { "/universal",      713, 0,   0,   0,    713, 0,    713, 0,   0,   0   },  34,  34,  34,  34, UNICODE_FOR_ALL },
{ 'd', 'd', 1, 0, { "/partialdiff",    494, 0,   0,   0,    494, 0,    494, 0,   0,   0   }, 182, 182, 182, 182, UNICODE_PARTIAL_DIFFERENTIAL },
{ 'E', 'r', 1, 0, { "/existential",    549, 0,   0,   0,    549, 0,    549, 0,   0,   0   },  36,  36,  36,  36, UNICODE_THERE_EXISTS },
{ 'O', '|', 1, 0, { "/emptyset",       823, 0,   0,   0,    823, 0,    823, 0,   0,   0   }, 198, 198, 198, 198, UNICODE_EMPTY_SET },
{ 'e', '=', 1, 0, { "/element",        713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 206, 206, 206, 206, UNICODE_ELEMENT_OF },
{ 's', 'u', 1, 0, { "/summation",      713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 229, 229, 229, 229, UNICODE_N_ARY_SUMMATION },
{ '-', 'm', 1, 0, { "/minus",          549, 0,   0,   0,    549, 0,    549, 0,   0,   0   },  45,  45,  45,  45, UNICODE_MINUS_SIGN },
{ '/', 'd', 1, 0, { "/fraction",       167, 0,   0,   0,    167, 0,    167, 0,   0,   0   }, 164, 164, 164, 164, UNICODE_DIVISION_SLASH },
{ 'V', 'r', 1, 0, { "/radical",        549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 214, 214, 214, 214, UNICODE_SQUARE_ROOT },
{ 'o', 'c', 1, 0, { "/proportional",   713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 181, 181, 181, 181, UNICODE_PROPORTIONAL_TO },
{ 'o', 'o', 1, 0, { "/infinity",       713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 165, 165, 165, 165, UNICODE_INFINITY },
{ 'a', 'n', 1, 0, { "/logicaland",     603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 217, 217, 217, 217, UNICODE_LOGICAL_AND },
{ 'o', 'r', 1, 0, { "/logicalor",      603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 218, 218, 218, 218, UNICODE_LOGICAL_OR },
{ 'n', 'i', 1, 0, { "/intersection",   768, 0,   0,   0,    768, 0,    768, 0,   0,   0   }, 199, 199, 199, 199, UNICODE_INTERSECTION },
{ 'u', 'u', 1, 0, { "/union",          768, 0,   0,   0,    768, 0,    768, 0,   0,   0   }, 200, 200, 200, 200, UNICODE_UNION },
{ 'i', 'n', 1, 0, { "/integral",       274, 0,   0,   0,    274, 0,    274, 0,   0,   0   }, 242, 242, 242, 242, UNICODE_INTEGRAL },
{ '.', '3', 1, 0, { "/therefore",      863, 0,   0,   0,    863, 0,    863, 0,   0,   0   },  92,  92,  92,  92, UNICODE_THEREFORE },
{ '=', '~', 1, 0, { "/congruent",      549, 0,   0,   0,    549, 0,    549, 0,   0,   0   },  64,  64,  64,  64, UNICODE_APPROXIMATELY_EQUAL_TO },
{ '~', '~', 1, 0, { "/approxequal",    549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 187, 187, 187, 187, UNICODE_ALMOST_EQUAL_TO },
{ 'u', 'n', 1, 0, { "/underscore",     500, 0,   0,   0,    500, 0,    500, 0,   0,   0   },  95,  95,  95,  95, UNICODE_LOW_LINE },
//{ 'o', 'v', 1, 0, { "/radicalex",      500, 0,   0,   0,    500, 0,    500, 0,   0,   0   },  96,  96,  96,  96, UNICODE_GRAVE_ACCENT /* BUG */ },
{ '=', '/', 1, 0, { "/notequal",       549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 185, 185, 185, 185, UNICODE_NOT_EQUAL_TO },
{ '=', '3', 1, 0, { "/equivalence",    549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 186, 186, 186, 186, UNICODE_IDENTICAL_TO }, /* defined as */
{ '<', '_', 1, 0, { "/lessequal",      549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 163, 163, 163, 163, UNICODE_LESS_THAN_OR_EQUAL_TO },
{ '>', '_', 1, 0, { "/greaterequal",   549, 0,   0,   0,    549, 0,    549, 0,   0,   0   }, 179, 179, 179, 179, UNICODE_GREATER_THAN_OR_EQUAL_TO },
{ 'c', '=', 1, 0, { "/propersubset",   713, 0,   0,   0,    713, 0,    713, 0,   0,   0   }, 204, 204, 204, 204, UNICODE_SUBSET_OF },
{ 'o', '+', 1, 0, { "/circleplus",     768, 0,   0,   0,    768, 0,    768, 0,   0,   0   }, 197, 197, 197, 197, UNICODE_CIRCLED_PLUS },
{ 'o', 'x', 1, 0, { "/circlemultiply", 768, 0,   0,   0,    768, 0,    768, 0,   0,   0   }, 196, 196, 196, 196, UNICODE_CIRCLED_TIMES },
{ 'T', 't', 1, 0, { "/perpendicular",  658, 0,   0,   0,    658, 0,    658, 0,   0,   0   },  94,  94,  94,  94, UNICODE_UP_TACK },
{ '.', '.', 1, 0, { "/ellipsis",      1000, 0,   0,   0,   1000, 0,   1000, 0,   0,   0   }, 188, 188, 188, 188, UNICODE_MIDLINE_HORIZONTAL_ELLIPSIS },
{ 'c', 'l', 1, 0, { "/club",           753, 0,   0,   0,    753, 0,    753, 0,   0,   0   }, 167, 167, 167, 167, UNICODE_BLACK_CLUB_SUIT },
{ 'd', 'i', 1, 0, { "/diamond",        753, 0,   0,   0,    753, 0,    753, 0,   0,   0   }, 168, 168, 168, 168, UNICODE_BLACK_DIAMOND_SUIT },
{ 'h', 'e', 1, 0, { "/heart",          753, 0,   0,   0,    753, 0,    753, 0,   0,   0   }, 169, 169, 169, 169, UNICODE_BLACK_HEART_SUIT },
{ 's', 'p', 1, 0, { "/spade",          753, 0,   0,   0,    753, 0,    753, 0,   0,   0   }, 170, 170, 170, 170, UNICODE_BLACK_SPADE_SUIT },
{ '<', '-', 1, 0, { "/arrowleft",      987, 0,   0,   0,    987, 0,    987, 0,   0,   0   }, 172, 172, 172, 172, UNICODE_LEFTWARDS_ARROW },
{ '^', '|', 1, 0, { "/arrowup",        603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 173, 173, 173, 173, UNICODE_UPWARDS_ARROW },
{ '-', '>', 1, 0, { "/arrowright",     987, 0,   0,   0,    987, 0,    987, 0,   0,   0   }, 174, 174, 174, 174, UNICODE_RIGHTWARDS_ARROW },
{ '_', '|', 1, 0, { "/arrowdown",      603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 175, 175, 175, 175, UNICODE_DOWNWARDS_ARROW },
{ '<', '>', 1, 0, { "/arrowboth",     1042, 0,   0,   0,   1042, 0,   1042, 0,   0,   0   }, 171, 171, 171, 171, UNICODE_LEFT_RIGHT_ARROW },
{ '<', '=', 1, 0, { "/arrowdblleft",   987, 0,   0,   0,    987, 0,    987, 0,   0,   0   }, 220, 220, 220, 220, UNICODE_LEFTWARDS_DOUBLE_ARROW }, /* follows from */
{ '^', '#', 1, 0, { "/arrowdblup",     603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 221, 221, 221, 221, UNICODE_UPWARDS_DOUBLE_ARROW },
{ '=', '>', 1, 0, { "/arrowdblright",  987, 0,   0,   0,    987, 0,    987, 0,   0,   0   }, 222, 222, 222, 222, UNICODE_RIGHTWARDS_DOUBLE_ARROW }, /* implies */
{ '_', '#', 1, 0, { "/arrowdbldown",   603, 0,   0,   0,    603, 0,    603, 0,   0,   0   }, 223, 223, 223, 223, UNICODE_DOWNWARDS_DOUBLE_ARROW },
{ 'e', 'q', 1, 0, { "/arrowdblboth",  1042, 0,   0,   0,   1042, 0,   1042, 0,   0,   0   }, 219, 219, 219, 219, UNICODE_LEFT_RIGHT_DOUBLE_ARROW }, /* equivalence */

/* Phonetic.                                                                              i89+ i93  i93  i89+ */
/*fir  sec  al di    ps                xipa b   ipa93   b   xipa b     xipa b    i    bi    xwin  win  mac   ps  unicode decomp  */
{ 'd', 'h', 2, 0, { "/eth",            500, 0,   510, 532,  500, 0,    500, 0,   0,   0   },  68,  68,  68,  68, UNICODE_LATIN_SMALL_LETTER_ETH },
{ 'h', '-', 2, 0, { "/hbar",           525, 0,   520, 578,  525, 0,    525, 0,   0,   0   }, 240, 240, 240, 240, UNICODE_LATIN_SMALL_LETTER_H_WITH_STROKE },
{ 'o', 'e', 2, 0, { "/oe",             700, 0,   751, 769,  700, 0,    700, 0,   0,   0   }, 191, 191, 191, 191, UNICODE_LATIN_SMALL_LIGATURE_OE },

{ 'a', 't', 2, 0, { "/aturn",          444, 0,   462, 520,  444, 0,    444, 0,   0,   0   }, 140, 140, 140, 140, UNICODE_LATIN_SMALL_LETTER_TURNED_A },
{ 'a', 's', 2, 0, { "/ascript",        500, 0,   520, 578,  500, 0,    500, 0,   0,   0   },  65,  65,  65,  65, UNICODE_LATIN_SMALL_LETTER_ALPHA },
{ 'a', 'y', 2, 0, { "/ascriptturn",    500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 129, 129, 129, 129, UNICODE_LATIN_SMALL_LETTER_TURNED_ALPHA }, // Am. pot
{ 'a', 'b', 2, 0, { "/ascriptturn",    500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 129, 129, 129, 129, UNICODE_LATIN_SMALL_LETTER_TURNED_ALPHA }, // Am. pot
{ 'b', '^', 2, 0, { "/bhooktop",       475, 0,   510, 580,  475, 0,    475, 0,   0,   0   }, 186, 186, 186, 186, UNICODE_LATIN_SMALL_LETTER_B_WITH_HOOK },
{ '[', 'f', 2, 0, { "/bracketleft",    333, 0,   346, 356,  333, 0,    333, 0,   0,   0   },  91,  91,  91,  91, UNICODE_LEFT_SQUARE_BRACKET }, // second version
{ ']', 'f', 2, 0, { "/bracketright",   333, 0,   346, 356,  333, 0,    333, 0,   0,   0   },  93,  93,  93,  93, UNICODE_RIGHT_SQUARE_BRACKET }, // second version
{ 'b', 'c', 2, 0, { "/bcap",           513, 0,   539, 572,  513, 0,    513, 0,   0,   0   }, 245, 245, 245, 245, UNICODE_LATIN_LETTER_SMALL_CAPITAL_B }, // bilabial trill
{ 'c', 't', 2, 0, { "/cturn",          444, 0,   452, 462,  444, 0,    444, 0,   0,   0   }, 141, 141, 141, 141, UNICODE_LATIN_SMALL_LETTER_OPEN_O },
{ 'c', 'c', 2, 0, { "/ccurl",          444, 0,   462, 462,  444, 0,    444, 0,   0,   0   }, 254, 254, 254, 254, UNICODE_LATIN_SMALL_LETTER_C_WITH_CURL },
{ 'd', '.', 2, 0, { "/drighttail",     500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 234, 234, 234, 234, UNICODE_LATIN_SMALL_LETTER_D_WITH_TAIL },
{ 'd', '^', 2, 0, { "/dhooktop",       500, 0,   523, 578,  500, 0,    500, 0,   0,   0   }, 235, 235, 235, 235, UNICODE_LATIN_SMALL_LETTER_D_WITH_HOOK },
{ 'e', '-', 2, 0, { "/erev",           444, 0,   462, 462,  444, 0,    444, 0,   0,   0   }, 251, 130, 130, 251, UNICODE_LATIN_SMALL_LETTER_REVERSED_E }, // 1993 addition
{ 's', 'w', 2, 0, { "/schwa",          444, 0,   462, 462,  444, 0,    444, 0,   0,   0   }, 171, 171, 171, 171, UNICODE_LATIN_SMALL_LETTER_SCHWA },
{ 's', 'r', 2, 0, { "/schwarighthook", 600, 0,   600, 600,  600, 0,    600, 0,   0,   0   }, 212,   0,   0, 212, UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK }, // Am. bird
{ 'e', 'f', 2, 0, { "/epsilonphonetic",444, 0,   441, 471,  444, 0,    444, 0,   0,   0   },  69,  69,  69,  69, UNICODE_LATIN_SMALL_LETTER_OPEN_E },
{ 'e', 'r', 2, 0, { "/epsilonrev",     444, 0,   441, 471,  444, 0,    444, 0,   0,   0   }, 206, 206, 206, 206, UNICODE_LATIN_SMALL_LETTER_REVERSED_OPEN_E },
{ 'k', 'b', 2, 0, { "/kidneybean",     500, 0,   471, 514,  500, 0,    500, 0,   0,   0   }, 185, 207, 207, 185, UNICODE_LATIN_SMALL_LETTER_CLOSED_REVERSED_OPEN_E }, // 1993 addition, 1996 correction
{ 'j', '-', 2, 0, { "/jdotlessbar",    333, 0,   289, 340,  333, 0,    333, 0,   0,   0   }, 239, 239, 239, 239, UNICODE_LATIN_SMALL_LETTER_DOTLESS_J_WITH_STROKE },
{ 'g', '^', 2, 0, { "/ghooktop",       500, 0,   520, 572,  500, 0,    500, 0,   0,   0   }, 169, 169, 169, 169, UNICODE_LATIN_SMALL_LETTER_G_WITH_HOOK },
{ 'g', 's', 2, 0, { "/gscript",        475, 0,   500, 555,  475, 0,    475, 0,   0,   0   }, 103, 103, 103, 103, UNICODE_LATIN_SMALL_LETTER_SCRIPT_G },
{ 'g', 'c', 2, 0, { "/gcap",           565, 0,   605, 659,  565, 0,    565, 0,   0,   0   },  71,  71,  71,  71, UNICODE_LATIN_LETTER_SMALL_CAPITAL_G },
{ 'g', 'f', 2, 0, { "/gammaphonetic",  500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 196, 196, 196, 196, UNICODE_LATIN_SMALL_LETTER_GAMMA },
{ 'r', 'h', 2, 0, { "/ramshorn",       500, 0,   573, 603,  500, 0,    500, 0,   0,   0   },  70,  70,  70,  70, UNICODE_LATIN_SMALL_LETTER_RAMS_HORN }, // formerly a baby gamma
{ 'G', '^', 2, 0, { "/gcaphooktop",    584, 0,   638, 662,  584, 0,    584, 0,   0,   0   }, 253, 253, 253, 253, UNICODE_LATIN_LETTER_SMALL_CAPITAL_G_WITH_HOOK },
{ 'h', 't', 2, 0, { "/hturn",          500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 231, 231, 231, 231, UNICODE_LATIN_SMALL_LETTER_TURNED_H },
{ 'h', '^', 2, 0, { "/hhooktop",       500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 250, 250, 250, 250, UNICODE_LATIN_SMALL_LETTER_H_WITH_HOOK },
{ 'h', 'j', 2, 0, { "/henghooktop",    475, 0,   520, 578,  475, 0,    475, 0,   0,   0   }, 238, 238, 238, 238, UNICODE_LATIN_SMALL_LETTER_HENG_WITH_HOOK }, // Swedish fricative sj
{ 'h', 'c', 2, 0, { "/hcap",           547, 0,   605, 659,  547, 0,    547, 0,   0,   0   },  75,  75,  75,  75, UNICODE_LATIN_LETTER_SMALL_CAPITAL_H },
{ 'i', '-', 2, 0, { "/ibar",           308, 0,   289, 289,  308, 0,    308, 0,   0,   0   }, 246, 246, 246, 246, UNICODE_LATIN_SMALL_LETTER_I_WITH_STROKE },
{ 'i', 'c', 2, 0, { "/icap",           280, 0,   289, 300,  280, 0,    280, 0,   0,   0   },  73,  73,  73,  73, UNICODE_LATIN_LETTER_SMALL_CAPITAL_I },
{ 'j', 'c', 2, 0, { "/jcurl",          318, 0,   289, 371,  318, 0,    318, 0,   0,   0   }, 198, 198, 198, 198, UNICODE_LATIN_SMALL_LETTER_J_WITH_CROSSED_TAIL },
{ 'l', '~', 2, 0, { "/ltilde",         380, 0,   0,   0,    380, 0,    380, 0,   0,   0   }, 201,   0,   0, 201, UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE },
{ 'l', '-', 2, 0, { "/lbelt",          350, 0,   337, 371,  350, 0,    350, 0,   0,   0   }, 194, 194, 194, 194, UNICODE_LATIN_SMALL_LETTER_L_WITH_BELT }, // Welsh ll
{ 'l', '.', 2, 0, { "/lrighttail",     278, 0,   289, 298,  278, 0,    278, 0,   0,   0   }, 241, 241, 241, 241, UNICODE_LATIN_SMALL_LETTER_L_WITH_RETROFLEX_HOOK },
{ 'l', 'z', 2, 0, { "/lyogh",          506, 0,   604, 641,  506, 0,    506, 0,   0,   0   },  76,  76,  76,  76, UNICODE_LATIN_SMALL_LETTER_LEZH },
{ 'l', 'c', 2, 0, { "/lcap",           455, 0,   502, 559,  455, 0,    455, 0,   0,   0   },  59,  59,  59,  59, UNICODE_LATIN_LETTER_SMALL_CAPITAL_L },
{ 'm', 't', 2, 0, { "/mturn",          778, 0,   809, 866,  778, 0,    778, 0,   0,   0   }, 181, 181, 181, 181, UNICODE_LATIN_SMALL_LETTER_TURNED_M },
{ 'm', 'l', 2, 0, { "/mturnleg",       778, 0,   809, 866,  778, 0,    778, 0,   0,   0   }, 229, 229, 229, 229, UNICODE_LATIN_SMALL_LETTER_TURNED_M_WITH_LONG_LEG },
{ 'm', 'j', 2, 0, { "/mlefttail",      753, 0,   795, 866,  753, 0,    753, 0,   0,   0   },  77,  77,  77,  77, UNICODE_LATIN_SMALL_LETTER_M_WITH_HOOK }, // labiodental nasal
{ 'n', 'g', 2, 0, { "/eng",            475, 0,   506, 578,  475, 0,    475, 0,   0,   0   },  78,  78,  78,  78, UNICODE_LATIN_SMALL_LETTER_ENG }, /* velar nasal */
{ 'n', 'j', 2, 0, { "/nlefttail",      500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 248, 248, 248, 248, UNICODE_LATIN_SMALL_LETTER_N_WITH_LEFT_HOOK }, // palatal nasal
{ 'n', '.', 2, 0, { "/nrighttail",     500, 0,   506, 578,  500, 0,    500, 0,   0,   0   }, 247, 247, 247, 247, UNICODE_LATIN_SMALL_LETTER_N_WITH_RETROFLEX_HOOK },
{ 'n', 'c', 2, 0, { "/ncap",           547, 0,   595, 614,  547, 0,    547, 0,   0,   0   }, 178, 178, 178, 178, UNICODE_LATIN_LETTER_SMALL_CAPITAL_N }, // uvular nasal
{ 'o', '-', 2, 0, { "/obar",           500, 0,   520, 520,  500, 0,    500, 0,   0,   0   },  80,  80,  80,  80, UNICODE_LATIN_SMALL_LETTER_BARRED_O }, // Swedish short u
{ 'O', 'e', 2, 0, { "/oecap",          727, 0,   745, 845,  727, 0,    727, 0,   0,   0   }, 175, 175, 175, 175, UNICODE_LATIN_LETTER_SMALL_CAPITAL_OE },
{ '|', '1', 2, 0, { "/pipe",           278, 0,   221, 221,  278, 0,    278, 0,   0,   0   }, 142, 142, 142, 142, UNICODE_LATIN_LETTER_DENTAL_CLICK },
{ '|', '2', 2, 0, { "/pipedouble",     444, 0,   221, 221,  444, 0,    444, 0,   0,   0   }, 146, 146, 146, 146, UNICODE_LATIN_LETTER_LATERAL_CLICK },
{ '|', '-', 2, 0, { "/pipedoublebar",  500, 0,   435, 435,  500, 0,    500, 0,   0,   0   }, 156, 156, 156, 156, UNICODE_LATIN_LETTER_ALVEOLAR_CLICK },
{ '|', 'f', 2, 0, { "/stroke",         278, 0,   208, 229,  278, 0,    278, 0,   0,   0   }, 150, 150, 150, 150, UNICODE_VERTICAL_LINE }, // second version
{ 'f', 'f', 2, 0, { "/phiphonetic",    550, 0,   549, 616,  550, 0,    550, 0,   0,   0   }, 184, 184, 184, 184, UNICODE_LATIN_SMALL_LETTER_PHI },
{ 'r', 't', 2, 0, { "/rturn",          333, 0,   356, 462,  333, 0,    333, 0,   0,   0   }, 168, 168, 168, 168, UNICODE_LATIN_SMALL_LETTER_TURNED_R },
{ 'r', 'l', 2, 0, { "/rturnleg",       333, 0,   356, 462,  333, 0,    333, 0,   0,   0   }, 228, 228, 228, 228, UNICODE_LATIN_SMALL_LETTER_TURNED_R_WITH_LONG_LEG },
{ 'r', '.', 2, 0, { "/rturnrighttail", 333, 0,   362, 457,  333, 0,    333, 0,   0,   0   }, 211, 211, 211, 211, UNICODE_LATIN_SMALL_LETTER_TURNED_R_WITH_HOOK },
{ 'f', '.', 2, 0, { "/rrighttail",     333, 0,   356, 462,  333, 0,    333, 0,   0,   0   }, 125, 125, 125, 125, UNICODE_LATIN_SMALL_LETTER_R_WITH_TAIL },
{ 'f', 'h', 2, 0, { "/fishhook",       333, 0,   380, 433,  333, 0,    333, 0,   0,   0   },  82,  82,  82,  82, UNICODE_LATIN_SMALL_LETTER_R_WITH_FISHHOOK }, // tap
{ 'r', 'c', 2, 0, { "/rcap",           541, 0,   559, 614,  541, 0,    541, 0,   0,   0   }, 123, 123, 123, 123, UNICODE_LATIN_LETTER_SMALL_CAPITAL_R },
{ 'r', 'i', 2, 0, { "/rcapinv",        541, 0,   559, 613,  541, 0,    541, 0,   0,   0   }, 210, 210, 210, 210, UNICODE_LATIN_LETTER_SMALL_CAPITAL_INVERTED_R },
{ 's', '.', 2, 0, { "/srighttail",     389, 0,   405, 405,  389, 0,    389, 0,   0,   0   }, 167, 167, 167, 167, UNICODE_LATIN_SMALL_LETTER_S_WITH_HOOK },
{ 's', 'h', 2, 0, { "/esh",            328, 0,   351, 361,  328, 0,    328, 0,   0,   0   },  83,  83,  83,  83, UNICODE_LATIN_SMALL_LETTER_ESH },
{ 'j', '^', 2, 0, { "/jbarhooktop",    300, 0,   289, 360,  300, 0,    300, 0,   0,   0   }, 215, 215, 215, 215, UNICODE_LATIN_SMALL_LETTER_DOTLESS_J_WITH_STROKE_AND_HOOK },
{ 't', '.', 2, 0, { "/trighttail",     325, 0,   289, 345,  325, 0,    325, 0,   0,   0   }, 255, 255, 255, 255, UNICODE_LATIN_SMALL_LETTER_T_WITH_RETROFLEX_HOOK },
{ 'u', '-', 2, 0, { "/ubar",           500, 0,   520, 578,  500, 0,    500, 0,   0,   0   }, 172, 172, 172, 172, UNICODE_LATIN_SMALL_LETTER_U_BAR }, // Swedish long u
{ 'h', 's', 2, 0, { "/horseshoe",      550, 0,   520, 549,  550, 0,    550, 0,   0,   0   },  85,  85,  85,  85, UNICODE_LATIN_SMALL_LETTER_UPSILON }, // omegaturn, "upsilon"
{ 'v', 's', 2, 0, { "/vscript",        461, 0,   488, 533,  461, 0,    461, 0,   0,   0   },  86,  86,  86,  86, UNICODE_LATIN_SMALL_LETTER_V_WITH_HOOK }, // Dutch w
{ 'v', 't', 2, 0, { "/vturn",          500, 0,   520, 520,  500, 0,    500, 0,   0,   0   }, 195, 195, 195, 195, UNICODE_LATIN_SMALL_LETTER_TURNED_V }, // wedge
{ 'w', 't', 2, 0, { "/wturn",          722, 0,   751, 751,  722, 0,    722, 0,   0,   0   }, 227, 227, 227, 227, UNICODE_LATIN_SMALL_LETTER_TURNED_W },
{ 'y', 't', 2, 0, { "/yturn",          500, 0,   520, 520,  500, 0,    500, 0,   0,   0   }, 180, 180, 180, 180, UNICODE_LATIN_SMALL_LETTER_TURNED_Y },
{ 'y', 'c', 2, 0, { "/ycap",           547, 0,   605, 612,  547, 0,    547, 0,   0,   0   },  89,  89,  89,  89, UNICODE_LATIN_LETTER_SMALL_CAPITAL_Y },
{ 'z', '.', 2, 0, { "/zrighttail",     444, 0,   462, 462,  444, 0,    444, 0,   0,   0   }, 189, 189, 189, 189, UNICODE_LATIN_SMALL_LETTER_Z_WITH_RETROFLEX_HOOK },
{ 'z', 'c', 2, 0, { "/zcurl",          494, 0,   482, 521,  494, 0,    494, 0,   0,   0   }, 252, 252, 252, 252, UNICODE_LATIN_SMALL_LETTER_Z_WITH_CURL },
{ 'z', 'h', 2, 0, { "/yogh",           444, 0,   459, 521,  444, 0,    444, 0,   0,   0   },  90,  90,  90,  90, UNICODE_LATIN_SMALL_LETTER_EZH },
{ '?', 'g', 2, 0, { "/glottalstop",    500, 0,   440, 477,  500, 0,    500, 0,   0,   0   },  47,  63,  63,  47, UNICODE_LATIN_LETTER_GLOTTAL_STOP },
{ '9', 'e', 2, 0, { "/glotrev",        500, 0,   440, 477,  500, 0,    500, 0,   0,   0   }, 214, 192, 192, 214, UNICODE_LATIN_LETTER_PHARYNGEAL_VOICED_FRICATIVE },
{ 'O', '.', 2, 0, { "/bullseye",       722, 0,   799, 818,  722, 0,    722, 0,   0,   0   }, 135, 135, 135, 135, UNICODE_LATIN_LETTER_BILABIAL_CLICK },
{ '?', '-', 2, 0, { "/glotbar",        500, 0,   440, 477,  500, 0,    500, 0,   0,   0   },  63, 251, 251,  63, UNICODE_LATIN_LETTER_GLOTTAL_STOP_WITH_STROKE }, // epiglottal plosive
{ '9', '-', 2, 0, { "/glotrevbar",     500, 0,   440, 477,  500, 0,    500, 0,   0,   0   }, 192, 185, 185, 192, UNICODE_LATIN_LETTER_REVERSED_GLOTTAL_STOP_WITH_STROKE }, // epiglottal fricative
{ ':', 'f', 2, 0, { "/lengthsign",     250, 0,   217, 235,  250, 0,    250, 0,   0,   0   }, 249, 249, 249, 249, UNICODE_MODIFIER_LETTER_TRIANGULAR_COLON },
{ '.', 'f', 2, 0, { "/halflength",     250, 0,   217, 235,  250, 0,    250, 0,   0,   0   },  62,  62,  62,  62, UNICODE_MODIFIER_LETTER_HALF_TRIANGULAR_COLON },
{ 'h', 'r', 2, 0, { "/righthook",      300, 0,   250, 265,  300, 0,    300, 0,   0,   0   }, 213, 213, 213, 213, UNICODE_MODIFIER_LETTER_RHOTIC_HOOK }, // or rturnsuper
{ '`', '^', 2, 1, { "/graveover",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  36,  36,  36,  36, UNICODE_COMBINING_GRAVE_ACCENT },
{ '\'','^', 2, 1, { "/acuteover",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  64,  64,  64,  64, UNICODE_COMBINING_ACUTE_ACCENT },
{ '^', '^', 2, 1, { "/circumover",       0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  94,  94,  94,  94, UNICODE_COMBINING_CIRCUMFLEX_ACCENT },
{ '~', '^', 2, 1, { "/tildeover",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  41,  41,  41,  41, UNICODE_COMBINING_TILDE }, // nasalized
{ '-', '^', 2, 1, { "/minusover",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  35,  35,  35,  35, UNICODE_COMBINING_MACRON }, // mid tone or so
{ ':', '^', 2, 1, { "/diaeresisover",    0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  95,  95,  95,  95, UNICODE_COMBINING_DIAERESIS }, // centralized
{ '0', '^', 2, 1, { "/ringover",         0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  42,  42,  42,  42, UNICODE_COMBINING_RING_ABOVE }, // voiceless
{ 'v', '^', 2, 1, { "/caronover",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  38,  38,  38,  38, UNICODE_COMBINING_CARON }, // hacek
{ 'N', '^', 2, 1, { "/breveover",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  40,  40,  40,  40, UNICODE_COMBINING_BREVE }, // nonsyllabic
{ 'c', 'n', 2, 2, { "/corner",         260, 0,   299, 299,  260, 0,    260, 0,   0,   0   }, 124, 124, 124, 124, UNICODE_COMBINING_LEFT_ANGLE_ABOVE }, // ? unreleased
{ 'c', 'v', 2, 1, { "/halfringleft",     0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  55,  55,  55,  55, UNICODE_COMBINING_LEFT_HALF_RING_BELOW }, // unrounded
{ 'T', '^', 2, 1, { "/raising",          0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  51,  51,  51,  51, UNICODE_COMBINING_UP_TACK_BELOW },
{ 'T', 'v', 2, 1, { "/lowering",         0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  52,  52,  52,  52, UNICODE_COMBINING_DOWN_TACK_BELOW },
{ 'T', '(', 2, 1, { "/atr",              0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  49,  49,  49,  49, UNICODE_COMBINING_LEFT_TACK_BELOW },
{ 'T', ')', 2, 1, { "/rtr",              0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  50,  50,  50,  50, UNICODE_COMBINING_RIGHT_TACK_BELOW },
{ '+', 'v', 2, 1, { "/plusunder",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  43,  43,  43,  43, UNICODE_COMBINING_PLUS_SIGN_BELOW }, // fronted
{ ':', 'v', 2, 1, { "/diaeresisunder",   0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  45,  45,  45,  45, UNICODE_COMBINING_DIAERESIS_BELOW }, // breathy voiced
{ '0', 'v', 2, 1, { "/ringunder",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  56,  56,  56,  56, UNICODE_COMBINING_RING_BELOW }, // voiceless
{ '|', 'v', 2, 1, { "/strokeunder",      0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  96,  96,  96,  96, UNICODE_COMBINING_VERTICAL_LINE_BELOW }, // syllabicity mark
{ 'N', 'v', 2, 1, { "/bridgeunder",      0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  53,  53,  53,  53, UNICODE_COMBINING_BRIDGE_BELOW }, // dental
{ 'U', 'v', 2, 1, { "/shelfunder",       0, 0,   0,   0,      0, 0,      0, 0,   0,   0   }, 176, 176, 176, 176, UNICODE_COMBINING_INVERTED_BRIDGE_BELOW }, // apical
{ 'D', 'v', 2, 1, { "/squareunder",      0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  54,  54,  54,  54, UNICODE_COMBINING_SQUARE_BELOW }, // laminal
{ 'n', 'v', 2, 1, { "/archunder",        0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  57,  57,  57,  57, UNICODE_COMBINING_INVERTED_BREVE_BELOW }, // nonsyllabic
{ '~', 'v', 2, 1, { "/tildeunder",       0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  48,  48,  48,  48, UNICODE_COMBINING_TILDE_BELOW }, // creaky voiced
{ '-', 'v', 2, 1, { "/minusunder",       0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },  61,  61,  61,  61, UNICODE_COMBINING_MINUS_SIGN_BELOW }, // backed
{ '~', '<', 2, 1, { "/tildethrough",     0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },   0, 242, 242,   0, UNICODE_COMBINING_TILDE_OVERLAY }, // velarized l
{ '3', 'v', 2, 1, { "/halfringright",    0, 0,   0,   0,      0, 0,      0, 0,   0,   0   }, 166, 166, 166, 166, UNICODE_COMBINING_RIGHT_HALF_RING_BELOW }, // rounded
{ 'l', 'i', 2, 1, { "/ligature",         0, 0,   0,   0,      0, 0,      0, 0,   0,   0   }, 131, 131, 131, 131, UNICODE_COMBINING_DOUBLE_INVERTED_BREVE },
{ 'b', 'f', 2, 0, { "/betaphonetic",   500, 0,   520, 597,  500, 0,    500, 0,   0,   0   },  66,  66,  66,  66, UNICODE_GREEK_SMALL_LETTER_BETA }, // second version
{ 't', 'f', 2, 0, { "/thetaphonetic",  444, 0,   520, 585,  444, 0,    444, 0,   0,   0   },  84,  84,  84,  84, UNICODE_GREEK_SMALL_LETTER_THETA }, // second version
{ 'c', 'f', 2, 0, { "/chiphonetic",    500, 0,   572, 610,  500, 0,    500, 0,   0,   0   },  88,  88,  88,  88, UNICODE_GREEK_SMALL_LETTER_CHI }, // second version

{ '\'','1', 2, 0, { "/stress1",        200, 0,   222, 222,  200, 0,    200, 0,   0,   0   }, 200, 200, 200, 200, UNICODE_MODIFIER_LETTER_VERTICAL_LINE },
{ '\'','2', 2, 0, { "/stress2",        200, 0,   222, 222,  200, 0,    200, 0,   0,   0   }, 199, 199, 199, 199, UNICODE_MODIFIER_LETTER_LOW_VERTICAL_LINE },

{ '^', 'h', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_H },
{ '^', 'H', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_H_WITH_HOOK },
{ '^', 'j', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_J },
{ '^', 'w', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_W },
{ '^', 'Y', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_TURNED_H },
{ '^', 'y', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_Y },
{ '^', '?', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_GLOTTAL_STOP },
{ '^', '9', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_REVERSED_GLOTTAL_STOP },
{ '^', 'l', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_L },
{ '^', 's', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_S },
{ '^', 'g', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_GAMMA },
{ '^', 'M', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_TURNED_M },
{ '^', 'G', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_TURNED_M_WITH_LONG_LEG },
{ '^', 'x', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_X },
{ '^', 'f', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_F },
{ '^', 'n', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_SUPERSCRIPT_LATIN_SMALL_LETTER_N },
{ '^', 'm', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_M },
{ '^', 'N', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_SMALL_ENG },
{ 'i', 'd', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_LATIN_SMALL_LETTER_REVERSED_R_WITH_FISHHOOK },
{ 'i', 'r', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_LATIN_SMALL_LETTER_SQUAT_REVERSED_ESH },
{ '_', 'u', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_UNDERTIE },

{ 't', 's', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_LATIN_SMALL_LETTER_TS_DIGRAPH },
{ 't', 'S', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_LATIN_SMALL_LETTER_TESH_DIGRAPH },
{ 'a', 'p', 2, 0, { "",                444, 500, 500, 500,  556, 611,  444, 444, 500, 444 }, '?', '?', '?', '?', UNICODE_MODIFIER_LETTER_APOSTROPHE },

/* Dingbats. */
/*fir  sec  al di    ps                tim  b    i    bi    hel  b     pal  b    i    bi     xwin win  mac   ps  unicode decomp  */
{ 'p', 'f', 3, 0, { "/fingerright",   1000, 0,   0,   0,   1000, 0,   1000, 0,   0,   0   },  43,  70,  43,  43, UNICODE_WHITE_RIGHT_POINTING_INDEX }, /* pointing finger */
/* Not yet bitmapped or measured. */
{ 'f', '5', 3, 0, { "/flower5",        800, 0,   0,   0,    800, 0,    800, 0,   0,   0   },  96,  96,  96,  96, UNICODE_WHITE_FLORETTE }, /* sympathy flower */

{'\0','\0', 0, 0, { 0,                   0, 0,   0,   0,      0, 0,      0, 0,   0,   0   },   0,   0,   0,   0, 0 }  /* Closing. */
};

static short where [95] [95];
static short inited = 0;

UCD_CodePointInfo theUnicodeDatabase [1+kUCD_TOP_OF_LIST] =
{
	#include "UCD_features_generated.h"
};

void Longchar_init () {
	Longchar_Info data = & Longchar_database [0];
	short i = 0;
	for (; data -> first != '\0'; i ++, data ++) {
		short *location = & where [data -> first - 32] [data -> second - 32];
		if (*location) {
			/*
				Doubly defined symbol; an error!
				We may not be able to use Melder_error yet,
				so just write a warning to stderr.
			*/
			fprintf (stderr, "Longchar init: symbol \"%c%c\" doubly defined.\n",
					data -> first, data -> second);
		}
		*location = i;
		if (data -> unicode <= kUCD_TOP_OF_LIST) {
			theUnicodeDatabase [data -> unicode]. first = data -> first;
			theUnicodeDatabase [data -> unicode]. second = data -> second;
		}
	}
	inited = 1;
}

char32 * Longchar_nativize (conststring32 generic, char32 *native, bool educateQuotes) {
	integer nquote = 0;
	char32 kar, kar1, kar2;
	if (! inited) Longchar_init ();
	while ((kar = *generic++) != U'\0') {
		if (educateQuotes) {
			if (kar == U'\"') {
				*native++ = ++nquote & 1 ? UNICODE_LEFT_DOUBLE_QUOTATION_MARK : UNICODE_RIGHT_DOUBLE_QUOTATION_MARK;
				continue;
			} else if (kar == U'`') {   /* Grave. */
				*native++ = UNICODE_LEFT_SINGLE_QUOTATION_MARK;
				continue;
			} else if (kar == U'\'') {   /* Straight apostrophe. */
				*native++ = UNICODE_RIGHT_SINGLE_QUOTATION_MARK;   /* Right single quote. */
				continue;
			}
		}
		if (kar == U'\\' && (kar1 = generic [0]) >= 32 && kar1 <= 126 && (kar2 = generic [1]) >= 32 && kar2 <= 126) {
			integer location = where [kar1 - 32] [kar2 - 32];
			if (location == 0) {
				*native++ = kar;
				*native++ = kar1;   /* Even if this is a backslash itself... */
				*native++ = kar2;   /* Even if this is a backslash itself... */
				/* These "evens" are here to ensure that Longchar_nativize does nothing on an already nativized string. */
			} else {
				*native++ = Longchar_database [location]. unicode ? Longchar_database [location]. unicode : UNICODE_INVERTED_QUESTION_MARK;
			}	
			generic += 2;
		} else {
			*native++ = kar;
		}
	}
	*native++ = U'\0';
	return native;
}

char32 *Longchar_genericize (conststring32 native, char32 *g) {
	char32 kar;
	if (! inited) Longchar_init ();
	while ((kar = *native++) != U'\0') {
		if (kar > 128 && kar <= kUCD_TOP_OF_LIST && theUnicodeDatabase [kar]. first != U'\0') {
			*g++ = '\\';
			*g++ = theUnicodeDatabase [kar]. first;
			*g++ = theUnicodeDatabase [kar]. second;
		} else {
			*g++ = kar;
		}
	}
	*g++ = U'\0';
	return g;
}

Longchar_Info Longchar_getInfo (char32 kar1, char32 kar2) {
	if (! inited) Longchar_init ();
	short position = kar1 < 32 || kar1 > 126 || kar2 < 32 || kar2 > 126 ?
		0 :   /* Return the 'space' character. */
		where [kar1 - 32] [kar2 - 32];
	return & Longchar_database [position];
}

Longchar_Info Longchar_getInfoFromNative (char32 kar) {
	if (! inited) Longchar_init ();
	return kar > kUCD_TOP_OF_LIST ? Longchar_getInfo (U' ', U' ') : Longchar_getInfo (theUnicodeDatabase [kar]. first, theUnicodeDatabase [kar]. second);
}

/* End of file longchar.cpp */
