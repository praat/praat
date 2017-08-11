/* ManPages.cpp
 *
 * Copyright (C) 1996-2012,2014,2015,2016,2017 Paul Boersma
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
#include "ManPages.h"
#include "longchar.h"
#include "Interpreter.h"
#include "praat.h"

Thing_implement (ManPages, Daata, 0);

#define LONGEST_FILE_NAME  55

static bool isAllowedFileNameCharacter (char32 c) {
	return isalnum ((int) c) || c == U'_' || c == U'-' || c == U'+';
}
static bool isSingleWordCharacter (char32 c) {
	return isalnum ((int) c) || c == U'_';
}

static long lookUp_unsorted (ManPages me, const char32 *title);

void structManPages :: v_destroy () noexcept {
	if (our dynamic) {
		for (long ipage = 1; ipage <= our pages.size; ipage ++) {
			ManPage page = our pages.at [ipage];
			Melder_free (page -> title);
			Melder_free (page -> author);
			if (page -> paragraphs) {
				ManPage_Paragraph par;
				for (par = page -> paragraphs; par -> type; par ++)
					Melder_free (par -> text);
				NUMvector_free <struct structManPage_Paragraph> (page -> paragraphs, 0);
			}
			if (ipage == 1) {
				NUMvector_free <long> (page -> linksHither, 1);
				NUMvector_free <long> (page -> linksThither, 1);
			}
		}
	}
	if (our titles)
		for (long ipage = 1; ipage <= our pages.size; ipage ++) {
			Melder_free (titles [ipage]);
		}
	NUMvector_free <const char32 *> (titles, 1);
	ManPages_Parent :: v_destroy ();
}

static const char32 *extractLink (const char32 *text, const char32 *p, char32 *link) {
	char32 *to = link, *max = link + 300;
	if (! p) p = text;
	/*
	 * Search for next '@' that is not in a backslash sequence.
	 */
	for (;;) {
		p = str32chr (p, U'@');
		if (! p) return nullptr;   // no more '@'
		if (p - text <= 0 || (p [-1] != U'\\' && (p - text <= 1 || p [-2] != U'\\'))) break;
		p ++;
	}
	Melder_assert (*p == U'@');
	if (p [1] == U'@') {
		const char32 *from = p + 2;
		while (*from != U'@' && *from != U'|' && *from != U'\0') {
			if (to >= max) {
				Melder_throw (U"(ManPages::grind:) Link starting with \"@@\" is too long:\n", text);
			}
			*to ++ = *from ++;
		}
		if (*from == U'|') { from ++; while (*from != U'@' && *from != U'\0') from ++; }
		if (*from) p = from + 1; else p = from;   /* Skip '@' but not '\0'. */
	} else {
		const char32 *from = p + 1;
		while (isSingleWordCharacter (*from)) {
			if (to >= max) {
				Melder_throw (U"(ManPages::grind:) Link starting with \"@@\" is too long:\n", text);
			}
			*to ++ = *from ++;
		}
		p = from;
	}
	*to = U'\0';
	return p;
}

static void readOnePage (ManPages me, MelderReadText text) {
	char32 *title;
	try {
		title = texgetw16 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find page title.");
	}

	/*
	 * Check whether a page with this title is already present.
	 */
	if (lookUp_unsorted (me, title)) {
		Melder_free (title);   // memory leak repaired, ppgb 20061228
		return;
	}

	autoManPage autopage = Thing_new (ManPage);
	autopage -> title = title;
	ManPage page = autopage.get();

	/*
	 * Add the page early, so that lookUp can find it.
	 */
	my pages. addItem_move (autopage.move());

	try {
		page -> author = texgetw16 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find author.");
	}
	try {
		page -> date = texgetu32 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find date.");
	}
	try {
		page -> recordingTime = texgetr64 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find recording time.");
	}
	page -> paragraphs = NUMvector <struct structManPage_Paragraph> (0, 500);

	ManPage_Paragraph par = & page -> paragraphs [0];
	for (;; par ++) {
		char32 link [501], fileName [256];
		try {
			par -> type = texgete8 (text, kManPage_type_getValue);
		} catch (MelderError) {
			if (Melder_hasError (U"end of text")) {
				Melder_clearError ();
				break;
			} else {
				throw;
			}
		}
		if (par -> type == kManPage_type_SCRIPT) {
			par -> width = texgetr64 (text);
			par -> height = texgetr64 (text);
		}
		try {
			par -> text = texgetw16 (text);
		} catch (MelderError) {
			Melder_throw (U"Cannot find text.");
		}
		for (const char32 *plink = extractLink (par -> text, nullptr, link); plink != nullptr; plink = extractLink (par -> text, plink, link)) {
			/*
			 * Now, `link' contains the link text, with spaces and all.
			 * Transform it into a file name.
			 */
			structMelderFile file2 { };
			if (link [0] == U'\\' && link [1] == U'F' && link [2] == U'I') {
				/*
				 * A link to a sound file: see if it exists.
				 */
				MelderDir_relativePathToFile (& my rootDirectory, link + 3, & file2);
				if (! MelderFile_exists (& file2)) {
					Melder_warning (U"Cannot find sound file ", MelderFile_messageName (& file2), U".");
				}
			} else if (link [0] == U'\\' && link [1] == U'S' && link [2] == U'C') {
				/*
				 * A link to a script: see if it exists.
				 */
				char32 *p = link + 3;
				if (*p == '\"') {
					char32 *q = fileName;
					p ++;
					while (*p != U'\"' && *p != U'\0') * q ++ = * p ++;
					*q = '\0';
				} else {
					char32 *q = fileName;
					while (*p != U' ' && *p != U'\0') * q ++ = * p ++;   // one word, up to the next space
					*q = '\0';
				}
				MelderDir_relativePathToFile (& my rootDirectory, fileName, & file2);
				if (! MelderFile_exists (& file2)) {
					Melder_warning (U"Cannot find script ", MelderFile_messageName (& file2), U".");
				}
				my executable = true;
			} else {
				char32 *q;
				/*
				 * A link to another page: follow it.
				 */
				for (q = link; *q; q ++) if (! isAllowedFileNameCharacter (*q)) *q = U'_';
				Melder_sprint (fileName,256, link, U".man");
				MelderDir_getFile (& my rootDirectory, fileName, & file2);
				try {
					autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
					try {
						readOnePage (me, text2.peek());
					} catch (MelderError) {
						Melder_throw (U"File ", & file2, U".");
					}
				} catch (MelderError) {
					/*
					 * Second try: with upper case.
					 */
					Melder_clearError ();
					link [0] = toupper32 (link [0]);
					Melder_sprint (fileName,256, link, U".man");
					MelderDir_getFile (& my rootDirectory, fileName, & file2);
					autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
					try {
						readOnePage (me, text2.peek());
					} catch (MelderError) {
						Melder_throw (U"File ", & file2, U".");
					}
				}
			}
		}
	}
	++ par;   // room for the last paragraph (because counting starts at 0)
	++ par;   // room for the final zero-type paragraph
	page -> paragraphs = (ManPage_Paragraph) Melder_realloc (page -> paragraphs, (int64) sizeof (struct structManPage_Paragraph) * (par - page -> paragraphs));
}
void structManPages :: v_readText (MelderReadText text, int /*formatVersion*/) {
	our dynamic = true;
	MelderDir_copy (& Data_directoryBeingRead, & rootDirectory);
	readOnePage (this, text);
}

autoManPages ManPages_create () {
	autoManPages me = Thing_new (ManPages);
	return me;
}

void ManPages_addPage (ManPages me, const char32 *title, const char32 *author, long date,
	struct structManPage_Paragraph paragraphs [])
{
	autoManPage page = Thing_new (ManPage);
	page -> title = title;
	page -> paragraphs = & paragraphs [0];
	page -> author = author;
	page -> date = date;
	my pages. addItem_move (page.move());
}

static int pageCompare (const void *first, const void *second) {
	ManPage me = * (ManPage *) first, thee = * (ManPage *) second;
	const char32 *p = my title, *q = thy title;
	for (;;) {
		char32 plower = tolower32 (*p), qlower = tolower32 (*q);
		if (plower < qlower) return -1;
		if (plower > qlower) return 1;
		if (plower == '\0') return str32cmp (my title, thy title);
		p ++, q ++;
	}
	return 0;   // should not occur
}

static long lookUp_unsorted (ManPages me, const char32 *title) {
	long i;

	/*
	 * First try to match an unaltered 'title' with the titles of the man pages.
	 */
	for (i = 1; i <= my pages.size; i ++) {
		ManPage page = my pages.at [i];
		if (str32equ (page -> title, title)) return i;
	}

	/*
	 * If that fails, try to find the upper-case variant.
	 */
	if (islower32 (title [0])) {
		char32 upperTitle [300];
		Melder_sprint (upperTitle,300, title);
		upperTitle [0] = toupper32 (upperTitle [0]);
		for (i = 1; i <= my pages.size; i ++) {
			ManPage page = my pages.at [i];
			if (str32equ (page -> title, upperTitle)) return i;
		}
	}
	return 0;
}

static long lookUp_sorted (ManPages me, const char32 *title) {
	static autoManPage dummy;
	ManPage *page;
	if (! dummy) dummy = Thing_new (ManPage);
	dummy -> title = title;
	page = (ManPage *) bsearch (& dummy, & my pages.at [1], my pages.size, sizeof (ManPage), pageCompare);   // noexcept
	dummy -> title = nullptr;   // undangle
	if (page) return (page - & my pages.at [1]) + 1;
	if (islower32 (title [0]) || isupper32 (title [0])) {
		char32 caseSwitchedTitle [300];
		Melder_sprint (caseSwitchedTitle,300, title);
		caseSwitchedTitle [0] = islower32 (title [0]) ? toupper32 (caseSwitchedTitle [0]) : tolower32 (caseSwitchedTitle [0]);
		dummy -> title = caseSwitchedTitle;
		page = (ManPage *) bsearch (& dummy, & my pages.at [1], my pages.size, sizeof (ManPage), pageCompare);   // noexcept
		dummy -> title = nullptr;   // undangle
		if (page) return (page - & my pages.at [1]) + 1;
	}
	return 0;
}

static void grind (ManPages me) {
	long ipage, ndangle = 0, jpage, grandNlinks, ilinkHither, ilinkThither;
	long *grandLinksHither, *grandLinksThither;

	qsort (& my pages.at [1], my pages.size, sizeof (ManPage), pageCompare);

	/*
	 * First pass: count and check links: fill in nlinksHither and nlinksThither.
	 */
	grandNlinks = 0;
	for (ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		int ipar;
		for (ipar = 0; page -> paragraphs [ipar]. type; ipar ++) {
			const char32 *text = page -> paragraphs [ipar]. text, *p;
			char32 link [301];
			if (text) for (p = extractLink (text, nullptr, link); p != nullptr; p = extractLink (text, p, link)) {
				if (link [0] == U'\\' && ((link [1] == U'F' && link [2] == U'I') || (link [1] == U'S' && link [2] == U'C')))
					continue;   // ignore "FILE" links
				if ((jpage = lookUp_sorted (me, link)) != 0) {
					page -> nlinksThither ++;
					my pages.at [jpage] -> nlinksHither ++;
					grandNlinks ++;
				} else {
					MelderInfo_writeLine (U"Page \"", page -> title, U"\" contains a dangling link to \"", link, U"\".");
					ndangle ++;
				}
			}
		}
	}
	if (ndangle) {
		MelderInfo_close ();
		Melder_warning (U"(ManPages::grind:) ", ndangle, U" dangling links encountered. See console window.");
	}

	/*
	 * Second pass: allocate memory: fill in linksHither and linksThither.
	 * Some optimization required: use only two mallocs.
	 * Forget nlinksHither and nlinksThither.
	 */
	if (grandNlinks == 0) { my ground = true; return; }
	if (! (grandLinksHither = NUMvector <long> (1, grandNlinks)) || ! (grandLinksThither = NUMvector <long> (1, grandNlinks))) {
		Melder_flushError ();
		return;
	}
	ilinkHither = ilinkThither = 0;
	for (ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		page -> linksHither = grandLinksHither + ilinkHither;
		page -> linksThither = grandLinksThither + ilinkThither;
		ilinkHither += page -> nlinksHither;
		ilinkThither += page -> nlinksThither;
		page -> nlinksHither = 0;
		page -> nlinksThither = 0;
	}
	Melder_assert (ilinkHither == grandNlinks && ilinkThither == grandNlinks);

	/*
	 * Third pass: remember the links: fill in linksThither [1..nlinksThither] and linksHither [1..nlinksHither].
	 * Rebuild nlinksHither and nlinksThither.
	 */
	for (ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		for (int ipar = 0; page -> paragraphs [ipar]. type; ipar ++) {
			const char32 *text = page -> paragraphs [ipar]. text, *p;
			char32 link [301];
			if (text) for (p = extractLink (text, nullptr, link); p != nullptr; p = extractLink (text, p, link)) {
				if (link [0] == U'\\' && ((link [1] == U'F' && link [2] == U'I') || (link [1] == U'S' && link [2] == U'C')))
					continue;   // ignore "FILE" links
				if ((jpage = lookUp_sorted (me, link)) != 0) {
					bool alreadyPresent = false;
					for (int ilink = 1; ilink <= page -> nlinksThither; ilink ++) {
						if (page -> linksThither [ilink] == jpage) {
							alreadyPresent = true;
							break;
						}
					}
					if (! alreadyPresent) {
						ManPage otherPage = my pages.at [jpage];
						page -> linksThither [++ page -> nlinksThither] = jpage;
						otherPage -> linksHither [++ otherPage -> nlinksHither] = ipage;
					}
				}
			}
		}
	}

	my ground = true;
}

long ManPages_uniqueLinksHither (ManPages me, long ipage) {
	ManPage page = my pages.at [ipage];
	long result = page -> nlinksHither, ilinkHither, ilinkThither;
	for (ilinkHither = 1; ilinkHither <= page -> nlinksHither; ilinkHither ++) {
		long link = page -> linksHither [ilinkHither];
		for (ilinkThither = 1; ilinkThither <= page -> nlinksThither; ilinkThither ++)
			if (page -> linksThither [ilinkThither] == link) { result --; break; }
	}
	return result;
}

long ManPages_lookUp (ManPages me, const char32 *title) {
	if (! my ground) grind (me);
	return lookUp_sorted (me, title);
}

static long ManPages_lookUp_caseSensitive (ManPages me, const char32 *title) {
	if (! my ground) grind (me);
	for (long i = 1; i <= my pages.size; i ++) {
		ManPage page = my pages.at [i];
		if (str32equ (page -> title, title)) return i;
	}
	return 0;
}

const char32 **ManPages_getTitles (ManPages me, long *numberOfTitles) {
	if (! my ground) grind (me);
	if (! my titles) {
		my titles = NUMvector <const char32 *> (1, my pages.size);   // TODO
		for (long i = 1; i <= my pages.size; i ++) {
			ManPage page = my pages.at [i];
			my titles [i] = Melder_dup_f (page -> title);
		}
	}
	*numberOfTitles = my pages.size;
	return my titles;
}

static struct stylesInfo {
	const char32 *htmlIn, *htmlOut;
} stylesInfo [] = {
{ 0 },
/* INTRO: */ { U"<p>", U"</p>" },
/* ENTRY: */ { U"<h3>", U"</h3>" },
/* NORMAL: */ { U"<p>", U"</p>" },
/* LIST_ITEM: */ { U"<dd>", U"" },
/* TAG: */ { U"<dt>", U"" },
/* DEFINITION: */ { U"<dd>", U"" },
/* CODE: */ { U"<code>", U"<br></code>" },
/* PROTOTYPE: */ { U"<p>", U"</p>" },
/* FORMULA: */ { U"<table width=\"100%\"><tr><td align=middle>", U"</table>" },
/* PICTURE: */ { U"<p>", U"</p>" },
/* SCRIPT: */ { U"<p>", U"</p>" },
/* LIST_ITEM1: */ { U"<dd>&nbsp;&nbsp;&nbsp;", U"" },
/* LIST_ITEM2: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* LIST_ITEM3: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* TAG1: */ { U"<dt>&nbsp;&nbsp;&nbsp;", U"" },
/* TAG2: */ { U"<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* TAG3: */ { U"<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* DEFINITION1: */ { U"<dd>&nbsp;&nbsp;&nbsp;", U"" },
/* DEFINITION2: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* DEFINITION3: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* CODE1: */ { U"<code>&nbsp;&nbsp;&nbsp;", U"<br></code>" },
/* CODE2: */ { U"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"<br></code>" },
/* CODE3: */ { U"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"<br></code>" },
/* CODE4: */ { U"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"<br></code>" },
/* CODE5: */ { U"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"<br></code>" }
};

static void writeParagraphsAsHtml (ManPages me, MelderFile file, ManPage_Paragraph paragraphs, MelderString *buffer) {
	long numberOfPictures = 0;
	bool inList = false, inItalic = false, inBold = false;
	bool inSub = false, inCode = false, inSuper = false, ul = false, inSmall = false;
	bool wordItalic = false, wordBold = false, wordCode = false, letterSuper = false;
	for (ManPage_Paragraph paragraph = paragraphs; paragraph -> type != 0; paragraph ++) {
		const char32 *p = paragraph -> text;
		int type = paragraph -> type, inTable;
		bool isListItem = type == kManPage_type_LIST_ITEM ||
			(type >= kManPage_type_LIST_ITEM1 && type <= kManPage_type_LIST_ITEM3);
		bool isTag = type == kManPage_type_TAG ||
			(type >= kManPage_type_TAG1 && type <= kManPage_type_TAG3);
		bool isDefinition = type == kManPage_type_DEFINITION ||
			(type >= kManPage_type_DEFINITION1 && type <= kManPage_type_DEFINITION3);
		/*bool isCode = type == kManPage_type_CODE ||
			(type >= kManPage_type_CODE1 && type <= kManPage_type_CODE5);*/

		if (type == kManPage_type_PICTURE) {
			numberOfPictures ++;
			structMelderFile pdfFile;
			MelderFile_copy (file, & pdfFile);
			pdfFile. path [str32len (pdfFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cpy (pdfFile. path + str32len (pdfFile. path),
				Melder_cat (U"_", numberOfPictures, U".pdf"));
			{// scope
				autoGraphics graphics = Graphics_create_pdffile (& pdfFile, 100, 0.0, paragraph -> width, 0.0, paragraph -> height);
				Graphics_setFont (graphics.get(), kGraphics_font_TIMES);
				Graphics_setFontStyle (graphics.get(), 0);
				Graphics_setFontSize (graphics.get(), 12);
				Graphics_setWrapWidth (graphics.get(), 0);
				Graphics_setViewport (graphics.get(), 0.0, paragraph -> width, 0.0, paragraph -> height);
				paragraph -> draw (graphics.get());
				Graphics_setViewport (graphics.get(), 0, 1, 0, 1);
				Graphics_setWindow (graphics.get(), 0, 1, 0, 1);
				Graphics_setTextAlignment (graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
			}
			structMelderFile tiffFile;
			MelderFile_copy (file, & tiffFile);
			tiffFile. path [str32len (tiffFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cpy (tiffFile. path + str32len (tiffFile. path),
				Melder_cat (U"_", numberOfPictures, U".png"));
			system (Melder_peek32to8 (Melder_cat (U"/usr/local/bin/gs -q -dNOPAUSE "
				U"-r200x200 -sDEVICE=png16m -sOutputFile=", tiffFile.path,
				U" ", pdfFile. path, U" quit.ps")));
			MelderFile_delete (& pdfFile);
			MelderString_append (buffer, Melder_cat (U"<p align=middle><img height=", paragraph -> height * 100,
				U" width=", paragraph -> width * 100, U" src=", MelderFile_name (& tiffFile), U"></p>"));
			continue;
		}
		if (type == kManPage_type_SCRIPT) {
			autoInterpreter interpreter = Interpreter_createFromEnvironment (nullptr);
			numberOfPictures ++;
			structMelderFile pdfFile;
			MelderFile_copy (file, & pdfFile);
			pdfFile. path [str32len (pdfFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cpy (pdfFile. path + str32len (pdfFile.path),
				Melder_cat (U"_", numberOfPictures, U".pdf"));
			{// scope
				autoGraphics graphics = Graphics_create_pdffile (& pdfFile, 100, 0.0, paragraph -> width, 0.0, paragraph -> height);
				Graphics_setFont (graphics.get(), kGraphics_font_TIMES);
				Graphics_setFontStyle (graphics.get(), 0);
				Graphics_setFontSize (graphics.get(), 12);
				Graphics_setWrapWidth (graphics.get(), 0);
				static structPraatApplication praatApplication;
				static structPraatObjects praatObjects;
				static structPraatPicture praatPicture;
				theCurrentPraatApplication = & praatApplication;
				theCurrentPraatApplication -> batch = true;
				theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
				theCurrentPraatObjects = (PraatObjects) & praatObjects;
				theCurrentPraatPicture = (PraatPicture) & praatPicture;
				theCurrentPraatPicture -> graphics = graphics.get();   // FIXME: should be move()?
				theCurrentPraatPicture -> font = kGraphics_font_TIMES;
				theCurrentPraatPicture -> fontSize = 12;
				theCurrentPraatPicture -> lineType = Graphics_DRAWN;
				theCurrentPraatPicture -> colour = Graphics_BLACK;
				theCurrentPraatPicture -> lineWidth = 1.0;
				theCurrentPraatPicture -> arrowSize = 1.0;
				theCurrentPraatPicture -> speckleSize = 1.0;
				theCurrentPraatPicture -> x1NDC = 0.0;
				theCurrentPraatPicture -> x2NDC = paragraph -> width;
				theCurrentPraatPicture -> y1NDC = 0.0;
				theCurrentPraatPicture -> y2NDC = paragraph -> height;
				Graphics_setViewport (graphics.get(), theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
				Graphics_setWindow (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				long x1DC, y1DC, x2DC, y2DC;
				Graphics_WCtoDC (graphics.get(), 0.0, 0.0, & x1DC, & y2DC);
				Graphics_WCtoDC (graphics.get(), 1.0, 1.0, & x2DC, & y1DC);
				Graphics_resetWsViewport (graphics.get(), x1DC, x2DC, y1DC, y2DC);
				Graphics_setWsWindow (graphics.get(), 0.0, paragraph -> width, 0.0, paragraph -> height);
				theCurrentPraatPicture -> x1NDC = 0.0;
				theCurrentPraatPicture -> x2NDC = paragraph -> width;
				theCurrentPraatPicture -> y1NDC = 0.0;
				theCurrentPraatPicture -> y2NDC = paragraph -> height;
				Graphics_setViewport (graphics.get(), theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
				{// scope
					autoMelderProgressOff progress;
					autoMelderWarningOff warning;
					autoMelderSaveDefaultDir saveDir;
					if (! MelderDir_isNull (& my rootDirectory)) {
						Melder_setDefaultDir (& my rootDirectory);
					}
					try {
						autostring32 text = Melder_dup (p);
						Interpreter_run (interpreter.get(), text.peek());
					} catch (MelderError) {
						trace (U"interpreter fails on ", pdfFile. path);
						Melder_flushError ();
					}
				}
				Graphics_setViewport (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				Graphics_setWindow (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				Graphics_setTextAlignment (graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
			}
			structMelderFile tiffFile;
			MelderFile_copy (file, & tiffFile);
			tiffFile. path [str32len (tiffFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cpy (tiffFile. path + str32len (tiffFile. path),
				Melder_cat (U"_", numberOfPictures, U".png"));
			system (Melder_peek32to8 (Melder_cat (U"/usr/local/bin/gs -q -dNOPAUSE "
				"-r200x200 -sDEVICE=png16m -sOutputFile=", tiffFile.path,
				U" ", pdfFile. path, U" quit.ps")));
			MelderFile_delete (& pdfFile);
			MelderString_append (buffer, U"<p align=middle><img height=", paragraph -> height * 100,
				U" width=", paragraph -> width * 100, U" src=", MelderFile_name (& tiffFile), U"></p>");
			theCurrentPraatApplication = & theForegroundPraatApplication;
			theCurrentPraatObjects = & theForegroundPraatObjects;
			theCurrentPraatPicture = & theForegroundPraatPicture;
			continue;
		}

		if (isListItem || isTag || isDefinition) {
			if (! inList) {
				ul = isListItem && (p [0] == U'•' || (p [0] == U'\\' && p [1] == U'b' && p [2] == U'u'));
				MelderString_append (buffer, ul ? U"<ul>\n" : U"<dl>\n");
				inList = true;
			}
			if (ul) {
				if (p [0] == U'•'  && p [1] == U' ') p += 1;
				if (p [0] == U'\\' && p [1] == U'b' && p [2] == U'u' && p [3] == U' ') p += 3;
			}
			MelderString_append (buffer, ul ? U"<li>" : stylesInfo [paragraph -> type]. htmlIn, U"\n");
		} else {
			if (inList) {
				MelderString_append (buffer, ul ? U"</ul>\n" : U"</dl>\n");
				inList = ul = false;
			}
			MelderString_append (buffer, stylesInfo [paragraph -> type]. htmlIn, U"\n");
		}
		inTable = *p == U'\t';
		if (inTable) {
			MelderString_append (buffer, U"<table border=0 cellpadding=0 cellspacing=0><tr><td width=100 align=middle>");
			p ++;
		}
		/*
		 * Leading spaces should be visible (mainly used in code fragments).
		 */
		while (*p == U' ') {
			MelderString_append (buffer, U"&nbsp;");
			p ++;
		}
		while (*p) {
				if (wordItalic && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
				if (wordBold && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</b>"); wordBold = false; }
				if (wordCode && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</code>"); wordCode = false; }
			if (*p == U'@') {
				char32 link [301], linkText [301], *q = link;
				if (p [1] == U'@') {
					p += 2;
					while (*p != U'@' && *p != U'|' && *p != U'\0') *q++ = *p++;
					*q = U'\0';   // close link
					if (*p == U'|') {
						p ++;   // skip '|'
						q = linkText;
						while (*p != U'@' && *p != U'\0') *q++ = *p++;
						*q = U'\0';   // close link text
					} else {
						Melder_sprint (linkText,301, link);
					}
					if (*p) p ++;
				} else {
					p ++;
					while (isSingleWordCharacter (*p) && *p != U'\0') *q++ = *p++;
					*q = U'\0';   // close link
					Melder_sprint (linkText,301, link);
				}
				/*
				 * We write the link in the following format:
				 *     <a href="link.html">linkText</a>
				 * If "link" (initial lower case) is not in the manual, we write "Link.html" instead.
				 * All spaces and strange symbols in "link" are replaced by underscores,
				 * because it will be a file name (see ManPages_writeAllToHtmlDir).
				 * The file name will have no more than 30 or 60 characters, and no less than 1.
				 */
				MelderString_append (buffer, U"<a href=\"");
				if (str32nequ (link, U"\\FI", 3)) {
					MelderString_append (buffer, link + 3);   // file link
				} else {
					q = link;
					if (! ManPages_lookUp_caseSensitive (me, link)) {
						MelderString_appendCharacter (buffer, toupper32 (link [0]));
						if (*q) q ++;   // first letter already written
					}
					while (*q && q - link < LONGEST_FILE_NAME) {
						if (! isAllowedFileNameCharacter (*q)) MelderString_appendCharacter (buffer, U'_');
						else MelderString_appendCharacter (buffer, *q);
						q ++;
					}
					if (link [0] == U'\0') MelderString_appendCharacter (buffer, U'_');   /* Otherwise Mac problems or Unix invisibility. */
					MelderString_append (buffer, U".html");
				}
				MelderString_append (buffer, U"\">", linkText, U"</a>");
			} else if (*p == U'%') {
				if (inItalic) { MelderString_append (buffer, U"</i>"); inItalic = false; p ++; }
				else if (p [1] == U'%') { MelderString_append (buffer, U"<i>"); inItalic = true; p += 2; }
				else if (p [1] == U'#') { MelderString_append (buffer, U"<i><b>"); wordItalic = true; wordBold = true; p += 2; }
				else { MelderString_append (buffer, U"<i>"); wordItalic = true; p ++; }
			} else if (*p == U'_') {
				if (inSub) {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"</sub>"); inSub = false; p ++;
				} else if (p [1] == U'_') {
					if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }
					MelderString_append (buffer, U"<sub>"); inSub = true; p += 2;
				} else { MelderString_append (buffer, U"_"); p ++; }
			} else if (*p == U'#') {
				if (inBold) { MelderString_append (buffer, U"</b>"); inBold = false; p ++; }
				else if (p [1] == U'#') { MelderString_append (buffer, U"<b>"); inBold = true; p += 2; }
				else if (p [1] == U'%') { MelderString_append (buffer, U"<b><i>"); wordBold = true; wordItalic = true; p += 2; }
				else { MelderString_append (buffer, U"<b>"); wordBold = true; p ++; }
			} else if (*p == U'$') {
				if (inCode) { MelderString_append (buffer, U"</code>"); inCode = false; p ++; }
				else if (p [1] == U'$') { MelderString_append (buffer, U"<code>"); inCode = true; p += 2; }
				else { MelderString_append (buffer, U"<code>"); wordCode = true; p ++; }
			} else if (*p == U'^') {
				if (inSuper) {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"</sup>"); inSuper = false; p ++;
				} else if (p [1] == U'^') {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"<sup>"); inSuper = true; p += 2;
				} else {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"<sup>"); letterSuper = true; p ++;
				}
			} else if (*p == U'}') {
				if (inSmall) { MelderString_append (buffer, U"</font>"); inSmall = false; p ++; }
				else { MelderString_append (buffer, U"}"); p ++; }
			} else if (*p == U'\\' && p [1] == U's' && p [2] == U'{') {
				MelderString_append (buffer, U"<font size=-1>"); inSmall = true; p += 3;
			} else if (*p == U'\t' && inTable) {
				MelderString_append (buffer, U"<td width=100 align=middle>"); p ++;
			} else if (*p == U'<') {
				MelderString_append (buffer, U"&lt;"); p ++;
			} else if (*p == U'>') {
				MelderString_append (buffer, U"&gt;"); p ++;
			} else if (*p == U'&') {
				MelderString_append (buffer, U"&amp;"); p ++;
			} else {
				/*if (wordItalic && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
				if (wordBold && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</b>"); wordBold = false; }
				if (wordCode && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</code>"); wordCode = false; }*/
				if (*p == U'\\') {
					char32 kar1 = *++p, kar2 = *++p;
					Longchar_Info info = Longchar_getInfo (kar1, kar2);
					if (info -> unicode < 127) {
						MelderString_appendCharacter (buffer, info -> unicode ? info -> unicode : U'?');
					} else {
						MelderString_append (buffer, U"&#", info -> unicode, U";");
					}
					p ++;
				} else {
					if (*p < 127) {
						MelderString_appendCharacter (buffer, *p);
					} else {
						MelderString_append (buffer, U"&#", (int) *p, U";");
					}
					p ++;
				}
				if (letterSuper) {
					if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }
					MelderString_append (buffer, U"</sup>"); letterSuper = false;
				}
			}
		}
		if (inItalic || wordItalic) { MelderString_append (buffer, U"</i>"); inItalic = wordItalic = false; }
		if (inBold || wordBold) { MelderString_append (buffer, U"</b>"); inBold = wordBold = false; }
		if (inCode || wordCode) { MelderString_append (buffer, U"</code>"); inCode = wordCode = false; }
		if (inSub) { MelderString_append (buffer, U"</sub>"); inSub = false; }
		if (inSuper || letterSuper) { MelderString_append (buffer, U"</sup>"); inSuper = letterSuper = false; }
		if (inTable) { MelderString_append (buffer, U"</table>"); inTable = false; }
		MelderString_append (buffer, stylesInfo [paragraph -> type]. htmlOut, U"\n");
	}
	if (inList) { MelderString_append (buffer, ul ? U"</ul>\n" : U"</dl>\n"); inList = false; }
}

static const char32 *month [] =
	{ U"", U"January", U"February", U"March", U"April", U"May", U"June",
	  U"July", U"August", U"September", U"October", U"November", U"December" };

static void writePageAsHtml (ManPages me, MelderFile file, long ipage, MelderString *buffer) {
	ManPage page = my pages.at [ipage];
	ManPage_Paragraph paragraphs = page -> paragraphs;
	MelderString_append (buffer, U"<html><head><meta name=\"robots\" content=\"index,follow\">"
		U"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
		U"<title>", page -> title, U"</title></head><body bgcolor=\"#FFFFFF\">\n\n");
	MelderString_append (buffer, U"<table border=0 cellpadding=0 cellspacing=0><tr><td bgcolor=\"#CCCC00\">"
		U"<table border=4 cellpadding=9><tr><td align=middle bgcolor=\"#000000\">"
		U"<font face=\"Palatino,Times\" size=6 color=\"#999900\"><b>\n",
		page -> title, U"\n</b></font></table></table>\n");
	writeParagraphsAsHtml (me, file, paragraphs, buffer);
	if (ManPages_uniqueLinksHither (me, ipage)) {
		long ilink, jlink, lastParagraph = 0;
		while (page -> paragraphs [lastParagraph]. type != 0) lastParagraph ++;
		if (lastParagraph > 0) {
			const char32 *text = page -> paragraphs [lastParagraph - 1]. text;
			if (text && text [0] && text [str32len (text) - 1] != U':')
				MelderString_append (buffer, U"<h3>Links to this page</h3>\n");
		}
		MelderString_append (buffer, U"<ul>\n");
		for (ilink = 1; ilink <= page -> nlinksHither; ilink ++) {
			long link = page -> linksHither [ilink];
			bool alreadyShown = false;
			for (jlink = 1; jlink <= page -> nlinksThither; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = true;
			if (! alreadyShown) {
				const char32 *title = my pages.at [page -> linksHither [ilink]] -> title, *p;
				MelderString_append (buffer, U"<li><a href=\"");
				for (p = title; *p; p ++) {
					if (p - title >= LONGEST_FILE_NAME) break;
					if (! isAllowedFileNameCharacter (*p)) MelderString_append (buffer, U"_");
					else MelderString_appendCharacter (buffer, *p);
				}
				if (title [0] == U'\0') MelderString_append (buffer, U"_");
				MelderString_append (buffer, U".html\">", title, U"</a>\n");
			}
		}
		MelderString_append (buffer, U"</ul>\n");
	}
	MelderString_append (buffer, U"<hr>\n<address>\n\t<p>&copy; ", page -> author);
	if (page -> date) {
		long date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12) imonth = 0;
		MelderString_append (buffer, U", ", month [imonth], U" ", date % 100);
		MelderString_append (buffer, U", ", date / 10000);
	}
	MelderString_append (buffer, U"</p>\n</address>\n</body>\n</html>\n");
}

void ManPages_writeOneToHtmlFile (ManPages me, long ipage, MelderFile file) {
	static MelderString buffer { };
	MelderString_empty (& buffer);
	writePageAsHtml (me, file, ipage, & buffer);
	MelderFile_writeText (file, buffer.string, kMelder_textOutputEncoding_UTF8);
}

void ManPages_writeAllToHtmlDir (ManPages me, const char32 *dirPath) {
	structMelderDir dir { };
	Melder_pathToDir (dirPath, & dir);
	for (long ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		char32 fileName [256];
		Melder_assert (str32len (page -> title) < 256 - 100);
		trace (U"page ", ipage, U": ", page -> title);
		Melder_sprint (fileName,256,  page -> title);
		for (char32 *p = fileName; *p; p ++)
			if (! isAllowedFileNameCharacter (*p))
				*p = U'_';
		if (fileName [0] == U'\0')
			str32cpy (fileName, U"_");   // no empty file names please
		fileName [LONGEST_FILE_NAME] = U'\0';
		str32cpy (fileName + str32len (fileName), U".html");
		static MelderString buffer { };
		MelderString_empty (& buffer);
		structMelderFile file { };
		MelderDir_getFile (& dir, fileName, & file);
		writePageAsHtml (me, & file, ipage, & buffer);
		/*
		 * An optimization because reading is much faster than writing:
		 * we write the file only if the old file is different or doesn't exist.
		 */
		autostring32 oldText;
		try {
			oldText.reset (MelderFile_readText (& file));
		} catch (MelderError) {
			Melder_clearError ();
		}
		if (! oldText.peek()   // doesn't the file exist yet?
			|| str32cmp (buffer.string, oldText.peek()))   // isn't the old file identical to the new text?
		{
			MelderFile_writeText (& file, buffer.string, kMelder_textOutputEncoding_UTF8);   // then write the new text
		}
	}
}

/* End of file ManPages.cpp */
