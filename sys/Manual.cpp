/* Manual.cpp
 *
 * Copyright (C) 1996-2011,2014,2015,2016,2017 Paul Boersma
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
#include "Manual.h"
#include "Printer.h"
#include "machine.h"
#include "site.h"
#include <time.h>
#include "EditorM.h"
#include "praat_script.h"
#include "praatP.h"

Thing_implement (Manual, HyperPage, 0);

/* Remaining BUGS: HTML writer does not recognize "\s{". */

#define SEARCH_PAGE  0

static const char32 *month [] =
	{ U"", U"January", U"February", U"March", U"April", U"May", U"June",
	  U"July", U"August", U"September", U"October", U"November", U"December" };

static void menu_cb_writeOneToHtmlFile (Manual me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save as HTML file", nullptr)
		ManPages manPages = (ManPages) my data;
		autoMelderString buffer;
		MelderString_copy (& buffer, manPages -> pages.at [my path] -> title);
		char32 *p = buffer.string;
		while (*p) { if (! isalnum ((int) *p) && *p != U'_') *p = U'_'; p ++; }
		MelderString_append (& buffer, U".html");
		Melder_sprint (defaultName,300, buffer.string);
	EDITOR_DO_SAVE
		ManPages_writeOneToHtmlFile ((ManPages) my data, my path, file);
	EDITOR_END
}

static void menu_cb_writeAllToHtmlDir (Manual me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Save all pages as HTML files", nullptr)
		LABEL (U"", U"Type a directory name:")
		TEXTFIELD (U"directory", U"")
	EDITOR_OK
		structMelderDir currentDirectory { };
		Melder_getDefaultDir (& currentDirectory);
		SET_STRING (U"directory", Melder_dirToPath (& currentDirectory))
	EDITOR_DO
		char32 *directory = GET_STRING (U"directory");
		ManPages_writeAllToHtmlDir ((ManPages) my data, directory);
	EDITOR_END
}

static void menu_cb_searchForPageList (Manual me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Search for page", nullptr)
		ManPages manPages = (ManPages) my data;
		long numberOfPages;
		const char32 **pages = ManPages_getTitles (manPages, & numberOfPages);
		LIST (U"Page", manPages -> pages.size, pages, 1)
	EDITOR_OK
	EDITOR_DO
		HyperPage_goToPage_i (me, GET_INTEGER (U"Page"));
	EDITOR_END
}

void structManual :: v_draw () {
	ManPages manPages = (ManPages) our data;
	ManPage page;
	ManPage_Paragraph paragraph;
	#if motif
	Graphics_clearWs (our graphics.get());
	#endif
	if (our path == SEARCH_PAGE) {
		HyperPage_pageTitle (this, U"Best matches");
		HyperPage_intro (this, U"The best matches to your query seem to be:");
		for (int i = 1; i <= our numberOfMatches; i ++) {
			char32 link [300];
			page = manPages -> pages.at [matches [i]];
			Melder_sprint (link,300, U"• @@", page -> title);
			HyperPage_listItem (this, link);
		}
		return;
	}
	page = manPages -> pages.at [path];
	if (! our paragraphs) return;
	HyperPage_pageTitle (this, page -> title);
	for (paragraph = & page -> paragraphs [0]; paragraph -> type != 0; paragraph ++) {
		switch (paragraph -> type) {
			case  kManPage_type_INTRO: HyperPage_intro (this, paragraph -> text); break;
			case  kManPage_type_ENTRY: HyperPage_entry (this, paragraph -> text); break;
			case  kManPage_type_NORMAL: HyperPage_paragraph (this, paragraph -> text); break;
			case  kManPage_type_LIST_ITEM: HyperPage_listItem (this, paragraph -> text); break;
			case  kManPage_type_TAG: HyperPage_listTag (this, paragraph -> text); break;
			case  kManPage_type_DEFINITION: HyperPage_definition (this, paragraph -> text); break;
			case  kManPage_type_CODE: HyperPage_code (this, paragraph -> text); break;
			case  kManPage_type_PROTOTYPE: HyperPage_prototype (this, paragraph -> text); break;
			case  kManPage_type_FORMULA: HyperPage_formula (this, paragraph -> text); break;
			case  kManPage_type_PICTURE: HyperPage_picture (this, paragraph -> width,
				paragraph -> height, paragraph -> draw); break;
			case  kManPage_type_SCRIPT: HyperPage_script (this, paragraph -> width,
				paragraph -> height, paragraph -> text); break;
			case  kManPage_type_LIST_ITEM1: HyperPage_listItem1 (this, paragraph -> text); break;
			case  kManPage_type_LIST_ITEM2: HyperPage_listItem2 (this, paragraph -> text); break;
			case  kManPage_type_LIST_ITEM3: HyperPage_listItem3 (this, paragraph -> text); break;
			case  kManPage_type_TAG1: HyperPage_listTag1 (this, paragraph -> text); break;
			case  kManPage_type_TAG2: HyperPage_listTag2 (this, paragraph -> text); break;
			case  kManPage_type_TAG3: HyperPage_listTag3 (this, paragraph -> text); break;
			case  kManPage_type_DEFINITION1: HyperPage_definition1 (this, paragraph -> text); break;
			case  kManPage_type_DEFINITION2: HyperPage_definition2 (this, paragraph -> text); break;
			case  kManPage_type_DEFINITION3: HyperPage_definition3 (this, paragraph -> text); break;
			case  kManPage_type_CODE1: HyperPage_code1 (this, paragraph -> text); break;
			case  kManPage_type_CODE2: HyperPage_code2 (this, paragraph -> text); break;
			case  kManPage_type_CODE3: HyperPage_code3 (this, paragraph -> text); break;
			case  kManPage_type_CODE4: HyperPage_code4 (this, paragraph -> text); break;
			case  kManPage_type_CODE5: HyperPage_code5 (this, paragraph -> text); break;
			default: break;
		}
	}
	if (ManPages_uniqueLinksHither (manPages, our path)) {
		long ilink, jlink, lastParagraph = 0;
		bool goAhead = true;
		while (page -> paragraphs [lastParagraph]. type != 0) lastParagraph ++;
		if (lastParagraph > 0) {
			const char32 *text = page -> paragraphs [lastParagraph - 1]. text;
			if (! text || text [0] == U'\0' || text [str32len (text) - 1] != U':') {
				if (our printing && our suppressLinksHither)
					goAhead = false;
				else
					HyperPage_entry (this, U"Links to this page");
			}
		}
		if (goAhead) for (ilink = 1; ilink <= page -> nlinksHither; ilink ++) {
			long link = page -> linksHither [ilink];
			bool alreadyShown = false;
			for (jlink = 1; jlink <= page -> nlinksThither; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = true;
			if (! alreadyShown) {
				const char32 *title = manPages -> pages.at [page -> linksHither [ilink]] -> title;
				char32 linkText [304];
				Melder_sprint (linkText, 304, U"@@", title, U"@");
				HyperPage_listItem (this, linkText);
			}
		}
	}
	if (! our printing && page -> date) {
		char32 signature [100];
		long date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12) imonth = 0;
		Melder_sprint (signature,100,
			U"© ", str32equ (page -> author, U"ppgb") ? U"Paul Boersma" :
			       str32equ (page -> author, U"djmw") ? U"David Weenink" : page -> author,
			U", ", date % 100,
			U" ", month [imonth],
			U" ", date / 10000);
		HyperPage_any (this, U"", our p_font, our p_fontSize, 0, 0.0,
			0.0, 0.0, 0.1, 0.1, HyperPage_ADD_BORDER);
		HyperPage_any (this, signature, our p_font, our p_fontSize, Graphics_ITALIC, 0.0,
			0.03, 0.0, 0.1, 0.0, 0);
	}
}

/********** PRINTING **********/

static void print (void *void_me, Graphics graphics) {
	iam (Manual);
	ManPages manPages = (ManPages) my data;
	long numberOfPages = manPages -> pages.size, savePage = my path;
	my ps = graphics;
	Graphics_setDollarSignIsCode (my ps, true);
	Graphics_setAtSignIsLink (my ps, true);
	my printing = true;
	HyperPage_initSheetOfPaper ((HyperPage) me);
	for (long ipage = 1; ipage <= numberOfPages; ipage ++) {
		ManPage page = manPages -> pages.at [ipage];
		if (my printPagesStartingWith == nullptr ||
		    Melder_stringMatchesCriterion (page -> title, kMelder_string_STARTS_WITH, my printPagesStartingWith))
		{
			ManPage_Paragraph par;
			my path = ipage;
			my paragraphs = page -> paragraphs;
			my numberOfParagraphs = 0;
			par = my paragraphs;
			while ((par ++) -> type) my numberOfParagraphs ++;
			Melder_free (my currentPageTitle);
			my currentPageTitle = Melder_dup_f (page -> title);
			my v_goToPage_i (ipage);
			my v_draw ();
			my v_goToPage_i (savePage);
		}
	}
	my printing = false;
	my printPagesStartingWith = nullptr;
}

static void menu_cb_printRange (Manual me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Print range", 0)
		SENTENCE (U"Left or inside header", U"")
		SENTENCE (U"Middle header", U"")
		SENTENCE (U"Right or outside header", U"Manual")
		SENTENCE (U"Left or inside footer", U"")
		SENTENCE (U"Middle footer", U"")
		SENTENCE (U"Right or outside footer", U"")
		BOOLEAN (U"Mirror even/odd headers", true)
		LABEL (U"", U"Print all pages whose title starts with:")
		TEXTFIELD (U"Print pages starting with", U"Intro")
		INTEGER (U"First page number", U"1")
		BOOLEAN (U"Suppress \"Links to this page\"", false)
	EDITOR_OK
		ManPages manPages = (ManPages) my data;
		time_t today = time (nullptr);
		char dateA [50];
		#ifdef UNIX
			struct tm *tm = localtime (& today);
			strftime (dateA, 50, "%B %e, %Y", tm);
		#else
			strcpy (dateA, ctime (& today));
		#endif
		char32 *date = Melder_peek8to32 (dateA), *newline;
		newline = str32chr (date, U'\n'); if (newline) *newline = U'\0';
		SET_STRING (U"Left or inside header", date)
		SET_STRING (U"Right or outside header", my name)
		if (my d_printingPageNumber) SET_INTEGER (U"First page number", my d_printingPageNumber + 1)
		if (my path >= 1 && my path <= manPages -> pages.size) {
			ManPage page = manPages -> pages.at [my path];
			SET_STRING (U"Print pages starting with", page -> title);
		}
	EDITOR_DO
		my insideHeader = GET_STRING (U"Left or inside header");
		my middleHeader = GET_STRING (U"Middle header");
		my outsideHeader = GET_STRING (U"Right or outside header");
		my insideFooter = GET_STRING (U"Left or inside footer");
		my middleFooter = GET_STRING (U"Middle footer");
		my outsideFooter = GET_STRING (U"Right or outside footer");
		my mirror = GET_INTEGER (U"Mirror even/odd headers");
		my printPagesStartingWith = GET_STRING (U"Print pages starting with");
		my d_printingPageNumber = GET_INTEGER (U"First page number");
		my suppressLinksHither = GET_INTEGER (U"Suppress \"Links to this page\"");
		Printer_print (print, me);
	EDITOR_END
}

/********** SEARCHING **********/

static double *goodnessOfMatch;

static double searchToken (ManPages me, long ipage, char32 *token) {
	double goodness = 0.0;
	ManPage page = my pages.at [ipage];
	struct structManPage_Paragraph *par = & page -> paragraphs [0];
	if (! token [0]) return 1.0;
	/*
	 * Try to find a match in the title, case insensitively.
	 */
	static MelderString buffer { };
	MelderString_copy (& buffer, page -> title);
	for (char32 *p = & buffer.string [0]; *p != U'\0'; p ++) *p = tolower32 (*p);
	if (str32str (buffer.string, token)) {
		goodness += 300.0;   // lots of points for a match in the title!
		if (str32equ (buffer.string, token))
			goodness += 10000.0;   // even more points for an exact match!
	}
	/*
	 * Try to find a match in the paragraphs, case-insensitively.
	 */
	while (par -> type) {
		if (par -> text) {
			char32 *ptoken;
			MelderString_copy (& buffer, par -> text);
			for (char32 *p = & buffer.string [0]; *p != '\0'; p ++) *p = tolower32 (*p);
			ptoken = str32str (buffer.string, token);
			if (ptoken) {
				goodness += 10.0;   // ten points for every paragraph with a match!
				if (str32str (ptoken + str32len (token), token)) {
					goodness += 1.0;   // one point for every second occurrence in a paragraph!
				}
			}
		}
		par ++;
	}
	return goodness;
}

static void search (Manual me, const char32 *query) {
	ManPages manPages = (ManPages) my data;
	long numberOfPages = manPages -> pages.size;
	static MelderString searchText { };
	MelderString_copy (& searchText, query);
	for (char32 *p = & searchText.string [0]; *p != U'\0'; p ++) {
		if (*p == U'\n') *p = U' ';
		*p = tolower32 (*p);
	}
	if (! goodnessOfMatch)
		goodnessOfMatch = NUMvector <double> (1, numberOfPages);
	for (long ipage = 1; ipage <= numberOfPages; ipage ++) {
		char32 *token = searchText.string;
		goodnessOfMatch [ipage] = 1.0;
		for (;;) {
			char32 *space = str32chr (token, U' ');
			if (space) *space = U'\0';
			goodnessOfMatch [ipage] *= searchToken (manPages, ipage, token);
			if (! space) break;
			*space = U' ';   // restore
			token = space + 1;
		}
	}
	/*
	 * Find the 20 best matches.
	 */
	my numberOfMatches = 0;
	for (long imatch = 1; imatch <= 20; imatch ++) {
		long imax = 0;
		double max = 0.0;
		for (long ipage = 1; ipage <= numberOfPages; ipage ++) {
			if (goodnessOfMatch [ipage] > max) {
				max = goodnessOfMatch [ipage];
				imax = ipage;
			}
		}
		if (! imax) break;
		my matches [++ my numberOfMatches] = imax;
		goodnessOfMatch [imax] = 0.0;   // skip next time
	}
	HyperPage_goToPage_i (me, SEARCH_PAGE);
}

void Manual_search (Manual me, const char32 *query) {
	GuiText_setString (my searchText, query);
	search (me, query);
}

static void gui_button_cb_home (Manual me, GuiButtonEvent /* event */) {
	ManPages pages = (ManPages) my data;
	long iHome = ManPages_lookUp (pages, U"Intro");
	HyperPage_goToPage_i (me, iHome ? iHome : 1);
}
 
static void gui_button_cb_record (Manual me, GuiButtonEvent /* event */) {
	ManPages manPages = (ManPages) my data;
	ManPage manPage = ( my path < 1 ? nullptr : manPages -> pages.at [my path] );
	GuiThing_setSensitive (my recordButton,  false);
	GuiThing_setSensitive (my playButton,    false);
	GuiThing_setSensitive (my publishButton, false);
	#if defined (_WIN32)
		GdiFlush ();
	#endif
	if (! Melder_record (manPage == nullptr ? 1.0 : manPage -> recordingTime)) Melder_flushError ();
	GuiThing_setSensitive (my recordButton,  true);
	GuiThing_setSensitive (my playButton,    true);
	GuiThing_setSensitive (my publishButton, true);
}

static void gui_button_cb_play (Manual me, GuiButtonEvent /* event */) {
	GuiThing_setSensitive (my recordButton,  false);
	GuiThing_setSensitive (my playButton,    false);
	GuiThing_setSensitive (my publishButton, false);
	#if defined (_WIN32)
		GdiFlush ();
	#endif
	Melder_play ();
	GuiThing_setSensitive (my recordButton,  true);
	GuiThing_setSensitive (my playButton,    true);
	GuiThing_setSensitive (my publishButton, true);
}

static void gui_button_cb_publish (Manual /* me */, GuiButtonEvent /* event */) {
	Melder_publishPlayed ();
}

static void do_search (Manual me) {
	char32 *query = GuiText_getString (my searchText);
	search (me, query);
	Melder_free (query);
}

static void gui_button_cb_search (Manual me, GuiButtonEvent /* event */) {
	do_search (me);
}

void structManual :: v_createChildren () {
	ManPages pages = (ManPages) our data;   // has been installed here by Editor_init ()
	our d_hasExtraRowOfTools = pages -> dynamic;
	Manual_Parent :: v_createChildren ();
	#if defined (macintosh)
		#define STRING_SPACING 8
	#else
		#define STRING_SPACING 2
	#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	our homeButton = GuiButton_createShown (our windowForm, 104, 168, y, y + height,
		U"Home", gui_button_cb_home, this, 0);
	if (pages -> dynamic) {
		our recordButton = GuiButton_createShown (our windowForm, 4, 79, y+height+8, y+height+8 + height,
			U"Record", gui_button_cb_record, this, 0);
		our playButton = GuiButton_createShown (our windowForm, 85, 160, y+height+8, y+height+8 + height,
			U"Play", gui_button_cb_play, this, 0);
		our publishButton = GuiButton_createShown (our windowForm, 166, 166 + 175, y+height+8, y+height+8 + height,
			U"Copy last played to list", gui_button_cb_publish, this, 0);
	}
	GuiButton_createShown (our windowForm, 274, 274 + 69, y, y + height,
		U"Search:", gui_button_cb_search, this, GuiButton_DEFAULT);
	our searchText = GuiText_createShown (our windowForm, 274+69 + STRING_SPACING, 452 + STRING_SPACING - 2, y, y + Gui_TEXTFIELD_HEIGHT, 0);
}

static void menu_cb_help (Manual me, EDITOR_ARGS_DIRECT) { HyperPage_goToPage (me, U"Manual"); }

void structManual :: v_createMenus () {
	Manual_Parent :: v_createMenus ();

	Editor_addCommand (this, U"File", U"Print manual...", 0, menu_cb_printRange);
	Editor_addCommand (this, U"File", U"Save page as HTML file...", 0, menu_cb_writeOneToHtmlFile);
	Editor_addCommand (this, U"File", U"Save manual to HTML directory...", 0, menu_cb_writeAllToHtmlDir);
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);

	Editor_addCommand (this, U"Go to", U"Search for page (list)...", 0, menu_cb_searchForPageList);
}

void structManual :: v_createHelpMenuItems (EditorMenu menu) {
	Manual_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"Manual help", '?', menu_cb_help);
}

void structManual :: v_defaultHeaders (EditorCommand cmd) {
	Manual me = (Manual) cmd -> d_editor;
	ManPages manPages = (ManPages) my data;
	if (my path) {
		char32 string [400];
		static const char32 *shortMonth [] =
			{ U"Jan", U"Feb", U"Mar", U"Apr", U"May", U"Jun", U"Jul", U"Aug", U"Sep", U"Oct", U"Nov", U"Dec" };
		ManPage page = manPages -> pages.at [my path];
		long date = page -> date;
		SET_STRING (U"Right or outside header", page -> title)
		SET_STRING (U"Left or inside footer", page -> author)
		if (date) {
			Melder_sprint (string,400, shortMonth [date % 10000 / 100 - 1], U" ", date % 100, U", ", date / 10000);
			SET_STRING (U"Left or inside header", string)
		}
	}
}

long structManual :: v_getNumberOfPages () {
	ManPages manPages = (ManPages) our data;
	return manPages -> pages.size;
}

long structManual :: v_getCurrentPageNumber () {
	return our path ? our path : 1;
}

void structManual :: v_goToPage_i (long pageNumber) {
	ManPages manPages = (ManPages) our data;
	if (pageNumber < 1 || pageNumber > manPages -> pages.size) {
		if (pageNumber == SEARCH_PAGE) {
			our path = SEARCH_PAGE;
			Melder_free (our currentPageTitle);
			return;
		} else Melder_throw (U"Page ", pageNumber, U" not found.");
	}
	our path = pageNumber;
	ManPage page = manPages -> pages.at [path];
	our paragraphs = page -> paragraphs;
	our numberOfParagraphs = 0;
	ManPage_Paragraph par = paragraphs;
	while ((par ++) -> type) our numberOfParagraphs ++;
	Melder_free (our currentPageTitle);
	our currentPageTitle = Melder_dup_f (page -> title);
}

int structManual :: v_goToPage (const char32 *title) {
	ManPages manPages = (ManPages) our data;
	if (title [0] == U'\\' && title [1] == U'F' && title [2] == U'I') {
		structMelderFile file { };
		MelderDir_relativePathToFile (& manPages -> rootDirectory, title + 3, & file);
		Melder_recordFromFile (& file);
		return -1;
	} else if (title [0] == U'\\' && title [1] == U'S' && title [2] == U'C') {
		autoMelderSetDefaultDir dir (& manPages -> rootDirectory);
		autoPraatBackground background;
		try {
			autostring32 fileNameWithArguments = Melder_dup (title + 3);
			praat_executeScriptFromFileNameWithArguments (fileNameWithArguments.peek());
		} catch (MelderError) {
			Melder_flushError ();
		}
		return 0;
	} else {
		long i = ManPages_lookUp (manPages, title);
		if (! i)
			Melder_throw (U"Page \"", title, U"\" not found.");
		our v_goToPage_i (i);
		return 1;
	}
}

void Manual_init (Manual me, const char32 *title, Daata data, bool ownData) {
	ManPages manPages = (ManPages) data;
	char32 windowTitle [101];
	long i;
	ManPage page;
	ManPage_Paragraph par;
	if (! (i = ManPages_lookUp (manPages, title)))
		Melder_throw (U"Page \"", title, U"\" not found.");
	my path = i;
	page = manPages -> pages.at [i];
	my paragraphs = page -> paragraphs;
	my numberOfParagraphs = 0;
	par = my paragraphs;
	while ((par ++) -> type) my numberOfParagraphs ++;

	if (manPages -> pages.at [1] -> title [0] == U'-') {
		Melder_sprint (windowTitle,101, & manPages -> pages.at [1] -> title [1]);
		if (windowTitle [str32len (windowTitle) - 1] == U'-') windowTitle [str32len (windowTitle) - 1] = U'\0';
	} else {
		Melder_sprint (windowTitle,101, U"Manual");
	}
	my ownData = ownData;
	HyperPage_init (me, windowTitle, data);
	MelderDir_copy (& manPages -> rootDirectory, & my rootDirectory);
	my history [0]. page = Melder_dup_f (title);   // BAD
}

autoManual Manual_create (const char32 *title, Daata data, bool ownData) {
	try {
		autoManual me = Thing_new (Manual);
		Manual_init (me.get(), title, data, ownData);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Manual window not created.");
	}
}

/* End of file Manual.cpp */
