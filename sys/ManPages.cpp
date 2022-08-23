/* ManPages.cpp
 *
 * Copyright (C) 1996-2021 Paul Boersma
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

#include "ManPages.h"
#include "../kar/longchar.h"
#include "Interpreter.h"
#include "praat.h"

Thing_implement (ManPages, Daata, 0);

#define LONGEST_FILE_NAME  55

static bool isAllowedFileNameCharacter (char32 c) {
	return Melder_isWordCharacter (c) || c == U'_' || c == U'-' || c == U'+';
}
static bool isSingleWordCharacter (char32 c) {
	return Melder_isWordCharacter (c) || c == U'_';
}

static integer lookUp_unsorted (ManPages me, conststring32 title);

void structManPages :: v9_destroy () noexcept {
	for (integer ipage = 1; ipage <= our pages.size; ipage ++) {
		ManPage page = our pages.at [ipage];
		for (integer ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
			ManPage_Paragraph par = & page -> paragraphs [ipar];
			Melder_free (par -> text);   // not an autostring32, because it can be a string literal (if not dynamic)
		}
		page -> linksHither. reset();   // TODO automate
		page -> linksThither. reset();
	}
	ManPages_Parent :: v9_destroy ();
}

static conststring32 extractLink (conststring32 text, const char32 *p, char32 *link) {
	char32 *to = link, *max = link + 300;
	if (! p)
		p = text;
	/*
		Search for next '@' that is not in a backslash sequence.
	*/
	for (;;) {
		p = str32chr (p, U'@');
		if (! p)
			return nullptr;   // no more '@'
		if (p - text <= 0 || (p [-1] != U'\\' && (p - text <= 1 || p [-2] != U'\\')))
			break;
		p ++;
	}
	Melder_assert (*p == U'@');
	if (p [1] == U'@') {
		const char32 *from = p + 2;
		while (*from != U'@' && *from != U'|' && *from != U'\0') {
			if (to >= max)
				Melder_throw (U"(ManPages::grind:) Link starting with \"@@\" is too long:\n", text);
			*to ++ = *from ++;
		}
		if (*from == U'|') {
			from ++;
			while (*from != U'@' && *from != U'\0')
				from ++;
		}
		p = from + ( *from == U'@' );   // add bool to pointer: skip '@' but not '\0'
	} else {
		const char32 *from = p + 1;
		while (isSingleWordCharacter (*from)) {
			if (to >= max)
				Melder_throw (U"(ManPages::grind:) Link starting with \"@@\" is too long:\n", text);
			*to ++ = *from ++;
		}
		p = from;
	}
	*to = U'\0';
	return p;
}

static void readOnePage (ManPages me, MelderReadText text) {
	autostring32 title;
	try {
		title = texgetw16 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find page title.");
	}

	/*
		Check whether a page with this title is already present.
	*/
	if (lookUp_unsorted (me, title.get()))
		return;

	autoManPage autopage = Thing_new (ManPage);
	autopage -> title = title.move();

	/*
		Add the page early, so that lookUp can find it.
	*/
	ManPage page = my pages. addItem_move (autopage.move());

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

	for (;;) {
		enum kManPage_type type;
		char32 link [501], fileName [256];
		try {
			type = (kManPage_type) texgete8 (text, (enum_generic_getValue) kManPage_type_getValue);
		} catch (MelderError) {
			if (Melder_hasError (U"end of text")) {
				Melder_clearError ();
				break;
			} else {
				throw;
			}
		}
		ManPage_Paragraph par = page -> paragraphs. append ();
		par -> type = type;
		if (par -> type == kManPage_type::SCRIPT) {
			par -> width = texgetr64 (text);
			par -> height = texgetr64 (text);
		}
		try {
			par -> text = texgetw16 (text).transfer();
		} catch (MelderError) {
			Melder_throw (U"Cannot find text.");
		}
		for (const char32 *plink = extractLink (par -> text, nullptr, link); plink != nullptr; plink = extractLink (par -> text, plink, link)) {
			/*
				Now, `link' contains the link text, with spaces and all.
				Transform it into a file name.
			*/
			structMelderFile file2 { };
			if (link [0] == U'\\' && link [1] == U'F' && link [2] == U'I') {
				/*
					A link to a sound file: see if it exists.
				*/
				MelderDir_relativePathToFile (& my rootDirectory, link + 3, & file2);
				if (! MelderFile_exists (& file2))
					Melder_warning (U"Cannot find sound file ", MelderFile_messageName (& file2), U".");
			} else if (link [0] == U'\\' && link [1] == U'S' && link [2] == U'C') {
				/*
					A link to a script: see if it exists.
				s*/
				char32 *p = link + 3;
				if (*p == U'\"') {
					char32 *q = fileName;
					p ++;
					while (*p != U'\"' && *p != U'\0')
						* q ++ = * p ++;
					*q = U'\0';
				} else {
					char32 *q = fileName;
					while (*p != U' ' && *p != U'\0') * q ++ = * p ++;   // one word, up to the next space
					*q = U'\0';
				}
				MelderDir_relativePathToFile (& my rootDirectory, fileName, & file2);
				if (! MelderFile_exists (& file2))
					Melder_warning (U"Cannot find script ", MelderFile_messageName (& file2), U".");
				my executable = true;
			} else {
				char32 *q;
				/*
					A link to another page: follow it.
				*/
				for (q = link; *q; q ++)
					if (! isAllowedFileNameCharacter (*q))
						*q = U'_';
				Melder_sprint (fileName,256, link, U".man");
				MelderDir_getFile (& my rootDirectory, fileName, & file2);
				try {
					autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
					try {
						readOnePage (me, text2.get());
					} catch (MelderError) {
						Melder_throw (U"File ", & file2, U".");
					}
				} catch (MelderError) {
					/*
						Second try: with upper case.
					*/
					Melder_clearError ();
					link [0] = Melder_toUpperCase (link [0]);
					Melder_sprint (fileName,256, link, U".man");
					MelderDir_getFile (& my rootDirectory, fileName, & file2);
					autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
					try {
						readOnePage (me, text2.get());
					} catch (MelderError) {
						Melder_throw (U"File ", & file2, U".");
					}
				}
			}
		}
	}
}
void structManPages :: v1_readText (MelderReadText text, int /*formatVersion*/) {
	our dynamic = true;
	MelderDir_copy (& Data_directoryBeingRead, & our rootDirectory);
	readOnePage (this, text);
}

autoManPages ManPages_create () {
	autoManPages me = Thing_new (ManPages);
	return me;
}

void ManPages_addPage (ManPages me, conststring32 title, conststring32 author, integer date,
	structManPage_Paragraph paragraphs [])
{
	autoManPage page = Thing_new (ManPage);
	page -> title = Melder_dup (title);
	for (ManPage_Paragraph par = & paragraphs [0]; (int) par -> type != 0; par ++) {
		ManPage_Paragraph targetParagraph = page -> paragraphs. append ();
		targetParagraph -> type = par -> type;
		targetParagraph -> text = par -> text;   // static string
		targetParagraph -> width = par -> width;
		targetParagraph -> height = par -> height;
		targetParagraph -> draw = par -> draw;
	}
	page -> author = Melder_dup (author);
	page -> date = date;
	my pages. addItem_move (page.move());
}

static bool pageCompare (ManPage me, ManPage thee) {
	const char32 *p = & my title [0], *q = & thy title [0];
	for (;;) {
		const char32 plower = Melder_toLowerCase (*p), qlower = Melder_toLowerCase (*q);
		if (plower < qlower)
			return true;
		if (plower > qlower)
			return false;
		if (plower == U'\0')
			return str32cmp (my title.get(), thy title.get()) < 0;
		p ++;
		q ++;
	}
	return false;   // should not occur
}

static integer lookUp_unsorted (ManPages me, conststring32 title) {
	/*
		First try to match an unaltered 'title' with the titles of the man pages.
	*/
	for (integer i = 1; i <= my pages.size; i ++) {
		ManPage page = my pages.at [i];
		if (str32equ (page -> title.get(), title))
			return i;
	}
	/*
		If that fails, try to find the upper-case variant.
	*/
	if (Melder_isLowerCaseLetter (title [0])) {
		char32 upperTitle [300];
		Melder_sprint (upperTitle,300, title);
		upperTitle [0] = Melder_toUpperCase (upperTitle [0]);
		for (integer i = 1; i <= my pages.size; i ++) {
			ManPage page = my pages.at [i];
			if (str32equ (page -> title.get(), upperTitle))
				return i;
		}
	}
	return 0;
}

static integer lookUp_sorted (ManPages me, conststring32 title) {
	static autoManPage dummy;
	if (! dummy)
		dummy = Thing_new (ManPage);
	dummy -> title = Melder_dup (title);
	ManPage *page = std::lower_bound (my pages.begin(), my pages.end(), dummy.get(), pageCompare);   // noexcept
	if (page != my pages.end() && Melder_equ ((*page) -> title.get(), dummy -> title.get()))
		return (page - my pages.begin()) + 1;
	if (Melder_isLowerCaseLetter (title [0]) || Melder_isUpperCaseLetter (title [0])) {
		char32 caseSwitchedTitle [300];
		Melder_sprint (caseSwitchedTitle,300, title);
		caseSwitchedTitle [0] = Melder_isLowerCaseLetter (title [0]) ? Melder_toUpperCase (caseSwitchedTitle [0]) : Melder_toLowerCase (caseSwitchedTitle [0]);
		dummy -> title = Melder_dup (caseSwitchedTitle);
		page = std::lower_bound (my pages.begin(), my pages.end(), dummy.get(), pageCompare);   // noexcept
		if (page != my pages.end() && Melder_equ ((*page) -> title.get(), dummy -> title.get()))
			return (page - my pages.begin()) + 1;
	}
	return 0;
}

static void grind (ManPages me) {
	std::sort (my pages.begin(), my pages.end(), pageCompare);
	for (integer ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		page -> linksHither = zero_INTVEC (0);   // superfluous if not ground twice
		page -> linksThither = zero_INTVEC (0);   // superfluous if not ground twice
	}
	integer ndangle = 0;
	for (integer ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		for (int ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
			conststring32 text = page -> paragraphs [ipar]. text;
			const char32 *p;
			char32 link [301];
			if (text) for (p = extractLink (text, nullptr, link); p != nullptr; p = extractLink (text, p, link)) {
				if (link [0] == U'\\' && ((link [1] == U'F' && link [2] == U'I') || (link [1] == U'S' && link [2] == U'C')))
					continue;   // ignore "FILE" links
				integer jpage = lookUp_sorted (me, link);
				if (jpage == 0) {
					MelderInfo_writeLine (U"Page \"", page -> title.get(), U"\" contains a dangling link to \"", link, U"\".");
					ndangle ++;
				} else {
					bool alreadyPresent = false;
					for (int ilink = 1; ilink <= page -> linksThither.size; ilink ++) {
						if (page -> linksThither [ilink] == jpage) {
							alreadyPresent = true;
							break;
						}
					}
					if (! alreadyPresent) {
						ManPage otherPage = my pages.at [jpage];
						* page -> linksThither. append () = jpage;
						* otherPage -> linksHither. append () = ipage;
					}
				}
			}
		}
	}
	if (ndangle) {
		MelderInfo_close ();
		Melder_warning (U"(ManPages::grind:) ", ndangle, U" dangling links encountered. See console window.");
	}
	my ground = true;
}

integer ManPages_uniqueLinksHither (ManPages me, integer ipage) {
	ManPage page = my pages.at [ipage];
	integer result = page -> linksHither.size;
	for (integer ilinkHither = 1; ilinkHither <= page -> linksHither.size; ilinkHither ++) {
		integer link = page -> linksHither [ilinkHither];
		for (integer ilinkThither = 1; ilinkThither <= page -> linksThither.size; ilinkThither ++) {
			if (page -> linksThither [ilinkThither] == link) {
				result --;
				break;
			}
		}
	}
	return result;
}

integer ManPages_lookUp (ManPages me, conststring32 title) {
	if (! my ground) {
		//Melder_stopwatch ();
		grind (me);
		//Melder_information (U"grinding lasted ", Melder_stopwatch (), U" seconds.");
	}
	return lookUp_sorted (me, title);
}

static integer ManPages_lookUp_caseSensitive (ManPages me, conststring32 title) {
	if (! my ground)
		grind (me);
	for (integer i = 1; i <= my pages.size; i ++) {
		ManPage page = my pages.at [i];
		if (str32equ (page -> title.get(), title))
			return i;
	}
	return 0;
}

constSTRVEC ManPages_getTitles (ManPages me) {
	if (! my ground)
		grind (me);
	if (! my titles) {
		my titles = autoSTRVEC (my pages.size);
		for (integer i = 1; i <= my pages.size; i ++) {
			ManPage page = my pages.at [i];
			my titles [i] = Melder_dup (page -> title.get());
		}
	}
	return my titles.get();
}

static const struct stylesInfo {
	conststring32 htmlIn, htmlOut;
} stylesInfo [] = {
{ nullptr, nullptr },
/* INTRO: */ { U"<p>", U"</p>" },
/* ENTRY: */ { U"<h3>", U"</h3>" },
/* NORMAL: */ { U"<p>", U"</p>" },
/* LIST_ITEM: */ { U"<dd>", U"" },
/* TAG: */ { U"<dt>", U"" },
/* DEFINITION: */ { U"<dd>", U"" },
/* CODE: */ { U"<code>", U"<br></code>" },
/* PROTOTYPE: */ { U"<p>", U"</p>" },
/* EQUATION: */ { U"<table width=\"100%\"><tr><td align=middle>", U"</table>" },
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

static void writeParagraphsAsHtml (ManPages me, MelderFile file, constvector <structManPage_Paragraph> const& paragraphs, MelderString *buffer) {
	integer numberOfPictures = 0;
	bool inList = false, inItalic = false, inBold = false;
	bool inSub = false, inCode = false, inSuper = false, ul = false, inSmall = false;
	bool wordItalic = false, wordBold = false, wordCode = false, letterSuper = false;
	for (integer ipar = 1; ipar <= paragraphs.size; ipar ++) {
		const structManPage_Paragraph *paragraph = & paragraphs [ipar];
		const char32 *p = & paragraph -> text [0];
		const bool isListItem = paragraph -> type == kManPage_type::LIST_ITEM ||
			(paragraph -> type >= kManPage_type::LIST_ITEM1 && paragraph -> type <= kManPage_type::LIST_ITEM3);
		const bool isTag = paragraph -> type == kManPage_type::TAG ||
			(paragraph -> type >= kManPage_type::TAG1 && paragraph -> type <= kManPage_type::TAG3);
		const bool isDefinition = paragraph -> type == kManPage_type::DEFINITION ||
			(paragraph -> type >= kManPage_type::DEFINITION1 && paragraph -> type <= kManPage_type::DEFINITION3);
		/*const bool isCode = paragraph -> type == kManPage_type::CODE ||
			(paragraph -> type >= kManPage_type::CODE1 && paragraph -> type <= kManPage_type::CODE5);*/

		if (paragraph -> type == kManPage_type::PICTURE) {
			numberOfPictures ++;
			structMelderFile pngFile;
			MelderFile_copy (file, & pngFile);
			pngFile. path [str32len (pngFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cat (pngFile. path, Melder_cat (U"_", numberOfPictures, U".png"));
			{// scope
				autoGraphics graphics = Graphics_create_pngfile (& pngFile, 300, 0.0, paragraph -> width, 0.0, paragraph -> height);
				Graphics_setFont (graphics.get(), kGraphics_font::TIMES);
				Graphics_setFontStyle (graphics.get(), 0);
				Graphics_setFontSize (graphics.get(), 12);
				Graphics_setWrapWidth (graphics.get(), 0);
				Graphics_setViewport (graphics.get(), 0.0, paragraph -> width, 0.0, paragraph -> height);
				paragraph -> draw (graphics.get());
				Graphics_setViewport (graphics.get(), 0, 1, 0, 1);
				Graphics_setWindow (graphics.get(), 0, 1, 0, 1);
				Graphics_setTextAlignment (graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
			}
			MelderString_append (buffer, Melder_cat (U"<p align=middle><img height=", paragraph -> height * 100,
				U" width=", paragraph -> width * 100, U" src=", MelderFile_name (& pngFile), U"></p>"));
			continue;
		}
		if (paragraph -> type == kManPage_type::SCRIPT) {
			autoInterpreter interpreter = Interpreter_createFromEnvironment (nullptr);
			numberOfPictures ++;
			structMelderFile pngFile;
			MelderFile_copy (file, & pngFile);
			pngFile. path [str32len (pngFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cat (pngFile. path, Melder_cat (U"_", numberOfPictures, U".png"));
			{// scope
				autoGraphics graphics = Graphics_create_pngfile (& pngFile, 300, 0.0, paragraph -> width, 0.0, paragraph -> height);
				Graphics_setFont (graphics.get(), kGraphics_font::TIMES);
				Graphics_setFontStyle (graphics.get(), 0);
				Graphics_setFontSize (graphics.get(), 12.0);
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
				theCurrentPraatPicture -> font = kGraphics_font::TIMES;
				theCurrentPraatPicture -> fontSize = 12.0;
				theCurrentPraatPicture -> lineType = Graphics_DRAWN;
				theCurrentPraatPicture -> colour = Melder_BLACK;
				theCurrentPraatPicture -> lineWidth = 1.0;
				theCurrentPraatPicture -> arrowSize = 1.0;
				theCurrentPraatPicture -> speckleSize = 1.0;
				theCurrentPraatPicture -> x1NDC = 0.0;
				theCurrentPraatPicture -> x2NDC = paragraph -> width;
				theCurrentPraatPicture -> y1NDC = 0.0;
				theCurrentPraatPicture -> y2NDC = paragraph -> height;
				Graphics_setViewport (graphics.get(), theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
				Graphics_setWindow (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				integer x1DC, y1DC, x2DC, y2DC;
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
					if (! MelderDir_isNull (& my rootDirectory))
						Melder_setDefaultDir (& my rootDirectory);
					try {
						autostring32 text = Melder_dup (p);
						Interpreter_run (interpreter.get(), text.get());
					} catch (MelderError) {
						trace (U"interpreter fails on ", pngFile. path);
						Melder_flushError ();
					}
				}
				Graphics_setViewport (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				Graphics_setWindow (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				Graphics_setTextAlignment (graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
			}
			MelderString_append (buffer, U"<p align=middle><img height=", paragraph -> height * 100,
				U" width=", paragraph -> width * 100, U" src=", MelderFile_name (& pngFile), U"></p>");
			theCurrentPraatApplication = & theForegroundPraatApplication;
			theCurrentPraatObjects = & theForegroundPraatObjects;
			theCurrentPraatPicture = & theForegroundPraatPicture;
			continue;
		}

		if (isListItem || isTag || isDefinition) {
			if (! inList) {
				ul = ( isListItem && (p [0] == U'•' || (p [0] == U'\\' && p [1] == U'b' && p [2] == U'u')) );
				MelderString_append (buffer, ul ? U"<ul>\n" : U"<dl>\n");
				inList = true;
			}
			if (ul) {
				if (p [0] == U'•'  && p [1] == U' ')
					p += 1;
				if (p [0] == U'\\' && p [1] == U'b' && p [2] == U'u' && p [3] == U' ')
					p += 3;
			}
			MelderString_append (buffer, ul ? U"<li>" : stylesInfo [(int) paragraph -> type]. htmlIn, U"\n");
		} else {
			if (inList) {
				MelderString_append (buffer, ul ? U"</ul>\n" : U"</dl>\n");
				inList = ul = false;
			}
			MelderString_append (buffer, stylesInfo [(int) paragraph -> type]. htmlIn, U"\n");
		}
		bool inTable = !! str32chr (p, U'\t'), inPromptedTable = false;
		if (inTable) {
			if (*p == U'\t') {
				MelderString_append (buffer, U"<table border=0 cellpadding=0 cellspacing=0><tr><td width=100 align=middle>");
				p ++;
			} else {
				MelderString_append (buffer, U"<table border=0 cellpadding=0 cellspacing=0><tr><td width=100 align=left>");
				inPromptedTable = true;
			}
		}
		/*
		 * Leading spaces should be visible (mainly used in code fragments).
		 */
		while (*p == U' ') {
			MelderString_append (buffer, U"&nbsp;");
			p ++;
		}
		while (*p) {
				if (wordItalic && ! isSingleWordCharacter (*p)) {
					MelderString_append (buffer, U"</i>");
					wordItalic = false;
				}
				if (wordBold && ! isSingleWordCharacter (*p)) {
					MelderString_append (buffer, U"</b>");
					wordBold = false;
				}
				if (wordCode && ! isSingleWordCharacter (*p)) {
					MelderString_append (buffer, U"</code>");
					wordCode = false;
				}
			if (*p == U'@') {
				static MelderString link, linkText;
				MelderString_empty (& link);
				MelderString_empty (& linkText);
				if (p [1] == U'@') {
					p += 2;
					while (*p != U'@' && *p != U'|' && *p != U'\0')
						MelderString_append (& link, * p ++);
					if (*p == U'|') {
						p ++;   // skip '|'
						while (*p != U'@' && *p != U'\0') {
							if (*p == U'^') {
								if (inSuper) {
									MelderString_append (& linkText, U"</sup>");
									inSuper = false;
									p ++;
								} else if (p [1] == U'^') {
									MelderString_append (& linkText, U"<sup>");
									inSuper = true;
									p += 2;
								} else {
									MelderString_append (& linkText, U"<sup>");
									letterSuper = true;
									p ++;
								}
							} else {
								if (*p == U'\\') {
									char32 kar1 = *++p, kar2 = *++p;
									Longchar_Info info = Longchar_getInfo (kar1, kar2);
									if (info -> unicode < 127) {
										MelderString_appendCharacter (& linkText, info -> unicode ? info -> unicode : U'?');
									} else {
										MelderString_append (& linkText, U"&#", (int) info -> unicode, U";");
									}
									p ++;
								} else {
									if (*p < 127) {
										MelderString_appendCharacter (& linkText, *p);
									} else {
										MelderString_append (& linkText, U"&#", (int) *p, U";");
									}
									p ++;
								}
								if (letterSuper) {
									//if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
									//if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }
									MelderString_append (& linkText, U"</sup>");
									letterSuper = false;
								}
							}
						}
					} else {
						MelderString_copy (& linkText, link.string);
					}
					if (*p)
						p ++;
				} else {
					p ++;
					while (isSingleWordCharacter (*p) && *p != U'\0') MelderString_append (& link, *p++);
					MelderString_copy (& linkText, link.string);
				}
				/*
					The first character of the link text can have the wrong case.
				*/
				integer linkPageNumber = ManPages_lookUp (me, link.string);
				if (linkPageNumber == 0)
					Melder_throw (U"No such manual page: ", link.string);
				link.string [0] = my pages.at [linkPageNumber] -> title [0];
				/*
				 * We write the link in the following format:
				 *     <a href="link.html">linkText</a>
				 * If "link" (initial lower case) is not in the manual, we write "Link.html" instead.
				 * All spaces and strange symbols in "link" are replaced by underscores,
				 * because it will be a file name (see ManPages_writeAllToHtmlDir).
				 * The file name will have no more than 30 or 60 characters, and no less than 1.
				 */
				MelderString_append (buffer, U"<a href=\"");
				if (str32nequ (link.string, U"\\FI", 3)) {
					MelderString_append (buffer, link.string + 3);   // file link
				} else {
					char32 *q = link.string;
					if (! ManPages_lookUp_caseSensitive (me, link.string)) {
						MelderString_appendCharacter (buffer, Melder_toUpperCase (link.string [0]));
						if (*q)
							q ++;   // first letter already written
					}
					while (*q && q - link.string < LONGEST_FILE_NAME) {
						if (! isAllowedFileNameCharacter (*q))
							MelderString_appendCharacter (buffer, U'_');
						else
							MelderString_appendCharacter (buffer, *q);
						q ++;
					}
					if (link.string [0] == U'\0')
						MelderString_appendCharacter (buffer, U'_');   // otherwise Mac problems or Unix invisibility
					MelderString_append (buffer, U".html");
				}
				MelderString_append (buffer, U"\">", linkText.string, U"</a>");
			} else if (*p == U'%') {
				if (inItalic) {
					MelderString_append (buffer, U"</i>");
					inItalic = false;
					p ++;
				} else if (p [1] == U'%') {
					MelderString_append (buffer, U"<i>");
					inItalic = true;
					p += 2;
				} else if (p [1] == U'#') {
					MelderString_append (buffer, U"<i><b>");
					wordItalic = true;
					wordBold = true;
					p += 2;
				} else {
					MelderString_append (buffer, U"<i>");
					wordItalic = true;
					p ++;
				}
			} else if (*p == U'_') {
				if (inSub) {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"</sub>");
					inSub = false;
					p ++;
				} else if (p [1] == U'_') {
					if (wordItalic) {
						MelderString_append (buffer, U"</i>");
						wordItalic = false;
					}
					if (wordBold) {
						MelderString_append (buffer, U"</b>");
						wordBold = false;
					}
					MelderString_append (buffer, U"<sub>");
					inSub = true;
					p += 2;
				} else {
					MelderString_append (buffer, U"_");
					p ++;
				}
			} else if (*p == U'#') {
				if (inBold) {
					MelderString_append (buffer, U"</b>");
					inBold = false;
					p ++;
				} else if (p [1] == U'#') {
					MelderString_append (buffer, U"<b>");
					inBold = true;
					p += 2;
				} else if (p [1] == U'%') {
					MelderString_append (buffer, U"<b><i>");
					wordBold = true;
					wordItalic = true;
					p += 2;
				} else {
					MelderString_append (buffer, U"<b>");
					wordBold = true;
					p ++;
				}
			} else if (*p == U'$') {
				if (inCode) {
					MelderString_append (buffer, U"</code>");
					inCode = false;
					p ++;
				} else if (p [1] == U'$') {
					MelderString_append (buffer, U"<code>");
					inCode = true;
					p += 2;
				} else {
					MelderString_append (buffer, U"<code>");
					wordCode = true;
					p ++;
				}
			} else if (*p == U'^') {
				if (inSuper) {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"</sup>");
					inSuper = false;
					p ++;
				} else if (p [1] == U'^') {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"<sup>");
					inSuper = true;
					p += 2;
				} else {
					/*if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold) { MelderString_append (buffer, U"</b>"); wordBold = false; }*/
					MelderString_append (buffer, U"<sup>");
					letterSuper = true;
					p ++;
				}
			} else if (*p == U'}') {
				if (inSmall) {
					MelderString_append (buffer, U"</font>");
					inSmall = false;
					p ++;
				} else {
					MelderString_append (buffer, U"}");
					p ++;
				}
			} else if (*p == U'\\' && p [1] == U's' && p [2] == U'{') {
				MelderString_append (buffer, U"<font size=-1>");
				inSmall = true;
				p += 3;
			} else if (*p == U'\t' && inTable) {
				if (inPromptedTable) {
					inPromptedTable = false;
					p ++;   // skip one tab
				} else {
					MelderString_append (buffer, U"<td width=100 align=middle>");
					p ++;
				}
			} else if (*p == U'<') {
				MelderString_append (buffer, U"&lt;");
				p ++;
			} else if (*p == U'>') {
				MelderString_append (buffer, U"&gt;");
				p ++;
			} else if (*p == U'&') {
				MelderString_append (buffer, U"&amp;");
				p ++;
			} else {
				/*if (wordItalic && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
				if (wordBold && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</b>"); wordBold = false; }
				if (wordCode && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</code>"); wordCode = false; }*/
				if (*p == U'\\') {
					char32 kar1 = *++p, kar2 = *++p;
					Longchar_Info info = Longchar_getInfo (kar1, kar2);
					if (info -> unicode < 127)
						MelderString_appendCharacter (buffer, info -> unicode ? info -> unicode : U'?');
					else
						MelderString_append (buffer, U"&#", (int) info -> unicode, U";");
					p ++;
				} else {
					if (*p < 127)
						MelderString_appendCharacter (buffer, *p);
					else
						MelderString_append (buffer, U"&#", (int) *p, U";");
					p ++;
				}
				if (letterSuper) {
					if (wordItalic) {
						MelderString_append (buffer, U"</i>");
						wordItalic = false;
					}
					if (wordBold) {
						MelderString_append (buffer, U"</b>");
						wordBold = false;
					}
					MelderString_append (buffer, U"</sup>");
					letterSuper = false;
				}
			}
		}
		if (inItalic || wordItalic) {
			MelderString_append (buffer, U"</i>");
			inItalic = wordItalic = false;
		}
		if (inBold || wordBold) {
			MelderString_append (buffer, U"</b>");
			inBold = wordBold = false;
		}
		if (inCode || wordCode) {
			MelderString_append (buffer, U"</code>");
			inCode = wordCode = false;
		}
		if (inSub) {
			MelderString_append (buffer, U"</sub>");
			inSub = false;
		}
		if (inSuper || letterSuper) {
			MelderString_append (buffer, U"</sup>");
			inSuper = letterSuper = false;
		}
		if (inTable) {
			MelderString_append (buffer, U"</table>");
			inTable = false;
		}
		MelderString_append (buffer, stylesInfo [(int) paragraph -> type]. htmlOut, U"\n");
	}
	if (inList) {
		MelderString_append (buffer, ul ? U"</ul>\n" : U"</dl>\n");
		inList = false;
	}
}

static const conststring32 month [] =
	{ U"", U"January", U"February", U"March", U"April", U"May", U"June",
	  U"July", U"August", U"September", U"October", U"November", U"December" };

static void writePageAsHtml (ManPages me, MelderFile file, integer ipage, MelderString *buffer) {
	ManPage page = my pages.at [ipage];
	MelderString_append (buffer, U"<html><head><meta name=\"robots\" content=\"index,follow\">"
		U"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
		U"<title>", page -> title.get(), U"</title></head><body bgcolor=\"#FFFFFF\">\n\n");
	MelderString_append (buffer, U"<table border=0 cellpadding=0 cellspacing=0><tr><td bgcolor=\"#CCCC00\">"
		U"<table border=4 cellpadding=9><tr><td align=middle bgcolor=\"#000000\">"
		U"<font face=\"Palatino,Times\" size=6 color=\"#999900\"><b>\n",
		page -> title.get(), U"\n</b></font></table></table>\n");
	writeParagraphsAsHtml (me, file, page -> paragraphs.get(), buffer);
	if (ManPages_uniqueLinksHither (me, ipage)) {
		integer ilink, jlink;
		if (page -> paragraphs.size > 0) {
			conststring32 text = page -> paragraphs [page -> paragraphs.size]. text;
			if (text && text [0] != U'\0' && text [str32len (text) - 1] != U':')
				MelderString_append (buffer, U"<h3>Links to this page</h3>\n");
		}
		MelderString_append (buffer, U"<ul>\n");
		for (ilink = 1; ilink <= page -> linksHither.size; ilink ++) {
			integer link = page -> linksHither [ilink];
			bool alreadyShown = false;
			for (jlink = 1; jlink <= page -> linksThither.size; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = true;
			if (! alreadyShown) {
				ManPage linkingPage = my pages.at [page -> linksHither [ilink]];
				conststring32 title = linkingPage -> title.get();
				const char32 *p;
				MelderString_append (buffer, U"<li><a href=\"");
				for (p = & title [0]; *p; p ++) {
					if (p - title >= LONGEST_FILE_NAME)
						break;
					if (! isAllowedFileNameCharacter (*p))
						MelderString_append (buffer, U"_");
					else
						MelderString_appendCharacter (buffer, *p);
				}
				if (title [0] == U'\0')
					MelderString_append (buffer, U"_");
				MelderString_append (buffer, U".html\">", title, U"</a>\n");
			}
		}
		MelderString_append (buffer, U"</ul>\n");
	}
	MelderString_append (buffer, U"<hr>\n<address>\n\t<p>&copy; ", page -> author.get());
	if (page -> date) {
		integer date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12)
			imonth = 0;
		MelderString_append (buffer, U", ", month [imonth], U" ", date % 100);
		MelderString_append (buffer, U", ", date / 10000);
	}
	MelderString_append (buffer, U"</p>\n</address>\n</body>\n</html>\n");
}

void ManPages_writeOneToHtmlFile (ManPages me, integer ipage, MelderFile file) {
	static MelderString buffer;
	MelderString_empty (& buffer);
	writePageAsHtml (me, file, ipage, & buffer);
	MelderFile_writeText (file, buffer.string, kMelder_textOutputEncoding::UTF8);
}

void ManPages_writeAllToHtmlDir (ManPages me, conststring32 dirPath) {
	structMelderDir dir { };
	Melder_pathToDir (dirPath, & dir);
	for (integer ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		char32 fileName [256];
		Melder_assert (str32len (page -> title.get()) < 256 - 100);
		trace (U"page ", ipage, U": ", page -> title.get());
		Melder_sprint (fileName,256,  page -> title.get());
		for (char32 *p = fileName; *p; p ++)
			if (! isAllowedFileNameCharacter (*p))
				*p = U'_';
		if (fileName [0] == U'\0')
			str32cpy (fileName, U"_");   // no empty file names please
		fileName [LONGEST_FILE_NAME] = U'\0';
		str32cat (fileName, U".html");
		static MelderString buffer;
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
			oldText = MelderFile_readText (& file);
		} catch (MelderError) {
			Melder_clearError ();
		}
		if (! oldText   // doesn't the file exist yet?
			|| str32cmp (buffer.string, oldText.get()))   // isn't the old file identical to the new text?
		{
			MelderFile_writeText (& file, buffer.string, kMelder_textOutputEncoding::UTF8);   // then write the new text
		}
	}
}

/* End of file ManPages.cpp */
