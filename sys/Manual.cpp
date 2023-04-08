/* Manual.cpp
 *
 * Copyright (C) 1996-2023 Paul Boersma
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

#include "Manual.h"
#include "Printer.h"
#include "machine.h"
#include "site.h"
#include "EditorM.h"
#include "praat_script.h"
#include "praatP.h"

Thing_implement (Manual, HyperPage, 0);

void structManual :: v9_destroy () noexcept {
	if (our ownManPages)
		forget_nozero (our data());
	Manual_Parent :: v9_destroy ();
}

#define SEARCH_PAGE  0

static const conststring32 month [] =
	{ U"", U"January", U"February", U"March", U"April", U"May", U"June",
	  U"July", U"August", U"September", U"October", U"November", U"December" };

static void menu_cb_writeOneToHtmlFile (Manual me, EDITOR_ARGS) {
	EDITOR_FORM_SAVE (U"Save as HTML file", nullptr)
		autoMelderString buffer;
		MelderString_copy (& buffer, my manPages() -> pages.at [my visiblePageNumber] -> title.get());
		char32 *p = buffer.string;
		while (*p) {
			if (! isalnum ((int) *p) && *p != U'_')
				*p = U'_';
			p ++;
		}
		MelderString_append (& buffer, U".html");
		Melder_sprint (defaultName,300, buffer.string);
	EDITOR_DO_SAVE
		ManPages_writeOneToHtmlFile (my manPages(), my visiblePageNumber, file);
	EDITOR_END
}

static void menu_cb_writeAllToHtmlFolder (Manual me, EDITOR_ARGS) {
	EDITOR_FORM (U"Save all pages as HTML files", nullptr)
		FOLDER (folder, U"Folder", U"")
	EDITOR_OK
		SET_STRING (folder, Melder_dirToPath (& my rootDirectory))
	EDITOR_DO
		ManPages_writeAllToHtmlDir (my manPages(), folder);
	EDITOR_END
}

static void menu_cb_searchForPageList (Manual me, EDITOR_ARGS) {
	EDITOR_FORM (U"Search for page", nullptr)
		static ManPages manPages;   // BUG: why?
		static constSTRVEC pages;
		manPages = my manPages();
		pages = ManPages_getTitles (manPages);
		LIST (page, U"Page", pages, 1)
	EDITOR_OK
	EDITOR_DO
		HyperPage_goToPage_number (me, page);
	EDITOR_END
}

static autoMelderString *manualInfoProc_string;
static void manualInfoProc (conststring32 infoText) {
	MelderString_copy (manualInfoProc_string, infoText);
			// FIXME: this overrides a growing info buffer, which is an O(N^2) algorithm if in a loop
}

static void Manual_runAllChunksToCache (ManPage page, Manual me) {
	/*
		When this page is drawn for the first time,
		all the script parts have to be run,
		so that the outputs of drawing and info can be cached.
	*/
	autoInterpreter interpreter;
	integer chunkNumber = 0;
	bool anErrorHasOccurred = false;
	autostring32 theErrorThatOccurred;
	for (integer ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
		ManPage_Paragraph paragraph = & page -> paragraphs [ipar];
		if (paragraph -> type != kManPage_type::SCRIPT)
			continue;
		chunkNumber += 1;
		if (paragraph -> cacheGraphics)
			break;   // don't run the chunks again
		if (! interpreter)
			interpreter = Interpreter_create ();
		/*
			Divert info text from Info window to Manual window.
		*/
		autoMelderSetInformationProc divert (manualInfoProc);
		manualInfoProc_string = & paragraph -> cacheInfo;
		MelderInfo_open ();
		/*
			Divert graphics from Picture window to Manual window.
		*/
		const kGraphics_font font = my instancePref_font();
		const double fontSize = my instancePref_fontSize();
		TRACE
		trace (U"Creating a cache graphics for chunk ", chunkNumber);
		paragraph -> cacheGraphics = Graphics_create_screen (nullptr, nullptr, 100);
		Graphics_startRecording (paragraph -> cacheGraphics.get());
		Graphics_setFont (paragraph -> cacheGraphics.get(), font);
		Graphics_setFontStyle (paragraph -> cacheGraphics.get(), 0);
		Graphics_setFontSize (paragraph -> cacheGraphics.get(), fontSize);
		const double true_width_inches  = paragraph -> width  * ( paragraph -> width  < 0.0 ? -1.0 : fontSize / 12.0 );
		const double true_height_inches = paragraph -> height * ( paragraph -> height < 0.0 ? -1.0 : fontSize / 12.0 );
		Graphics_setWrapWidth (paragraph -> cacheGraphics.get(), 0.0);
		integer x1DCold, x2DCold, y1DCold, y2DCold;
		Graphics_inqWsViewport (paragraph -> cacheGraphics.get(), & x1DCold, & x2DCold, & y1DCold, & y2DCold);
		double x1NDCold, x2NDCold, y1NDCold, y2NDCold;
		Graphics_inqWsWindow (paragraph -> cacheGraphics.get(), & x1NDCold, & x2NDCold, & y1NDCold, & y2NDCold);
		{
			if (! my praatApplication)
				my praatApplication = Melder_calloc_f (structPraatApplication, 1);
			if (! my praatObjects)
				my praatObjects = Melder_calloc_f (structPraatObjects, 1);
			if (! my praatPicture)
				my praatPicture = Melder_calloc_f (structPraatPicture, 1);
			theCurrentPraatApplication = (PraatApplication) my praatApplication;
			theCurrentPraatApplication -> batch = true;   // prevent creation of editor windows
			theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
			theCurrentPraatObjects = (PraatObjects) my praatObjects;
			theCurrentPraatPicture = (PraatPicture) my praatPicture;
			theCurrentPraatPicture -> graphics = paragraph -> cacheGraphics.get();   // has to draw into HyperPage rather than Picture window
			theCurrentPraatPicture -> font = font;
			theCurrentPraatPicture -> fontSize = fontSize;
			theCurrentPraatPicture -> lineType = Graphics_DRAWN;
			theCurrentPraatPicture -> colour = Melder_BLACK;
			theCurrentPraatPicture -> lineWidth = 1.0;
			theCurrentPraatPicture -> arrowSize = 1.0;
			theCurrentPraatPicture -> speckleSize = 1.0;
			#if 1
			theCurrentPraatPicture -> x1NDC = 0.0;
			theCurrentPraatPicture -> x2NDC = true_width_inches;
			theCurrentPraatPicture -> y1NDC = 0.0;
			theCurrentPraatPicture -> y2NDC = true_height_inches;

			Graphics_setViewport (paragraph -> cacheGraphics.get(),
					theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
			#endif
			Graphics_setWindow (paragraph -> cacheGraphics.get(), 0.0, 1.0, 0.0, 1.0);
			#if 1
			integer x1DC, y1DC, x2DC, y2DC;
			Graphics_WCtoDC (paragraph -> cacheGraphics.get(), 0.0, 0.0, & x1DC, & y2DC);
			Graphics_WCtoDC (paragraph -> cacheGraphics.get(), 1.0, 1.0, & x2DC, & y1DC);
			Graphics_resetWsViewport (paragraph -> cacheGraphics.get(), x1DC, x2DC, y1DC, y2DC);
			Graphics_setWsWindow (paragraph -> cacheGraphics.get(), 0, paragraph -> width, 0, paragraph -> height);
			#endif
			theCurrentPraatPicture -> x1NDC = 0.0;
			theCurrentPraatPicture -> x2NDC = paragraph -> width;
			theCurrentPraatPicture -> y1NDC = 0.0;
			theCurrentPraatPicture -> y2NDC = paragraph -> height;
			Graphics_setViewport (paragraph -> cacheGraphics.get(),
					theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
			if (anErrorHasOccurred) {
				trace (U"Chunk ", chunkNumber, U" not run, because of an earlier error.");
				MelderInfo_writeLine (U"**ERROR** This code chunk was not run,\n    because an error occurred in an earlier chunk.");
				MelderInfo_close ();
			} else {
				autoMelderProgressOff progress;
				autoMelderWarningOff warning;
				autoMelderSaveDefaultDir saveDir;
				if (! MelderDir_isNull (& my rootDirectory))
					Melder_setDefaultDir (& my rootDirectory);
				try {
					autostring32 text = Melder_dup (paragraph -> text);
					Interpreter_run (interpreter.get(), text.get(), chunkNumber > 1);
				} catch (MelderError) {
					anErrorHasOccurred = true;
					theErrorThatOccurred = Melder_dup (Melder_getError ());
					trace (U"Error in chunk ", chunkNumber, U".");
					Melder_clearError ();
					MelderInfo_writeLine (U"**AN ERROR OCCURRED IN THIS CODE CHUNK:**\n\n", theErrorThatOccurred.get());
					MelderInfo_close ();
				}
			}
			Graphics_setLineType (paragraph -> cacheGraphics.get(), Graphics_DRAWN);
			Graphics_setLineWidth (paragraph -> cacheGraphics.get(), 1.0);
			Graphics_setArrowSize (paragraph -> cacheGraphics.get(), 1.0);
			Graphics_setSpeckleSize (paragraph -> cacheGraphics.get(), 1.0);
			Graphics_setColour (paragraph -> cacheGraphics.get(), Melder_BLACK);
			theCurrentPraatApplication = & theForegroundPraatApplication;
			theCurrentPraatObjects = & theForegroundPraatObjects;
			theCurrentPraatPicture = & theForegroundPraatPicture;
		}
		Graphics_resetWsViewport (paragraph -> cacheGraphics.get(), x1DCold, x2DCold, y1DCold, y2DCold);
		Graphics_setWsWindow (paragraph -> cacheGraphics.get(), x1NDCold, x2NDCold, y1NDCold, y2NDCold);
		Graphics_setViewport (paragraph -> cacheGraphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setWindow (paragraph -> cacheGraphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (paragraph -> cacheGraphics.get(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_stopRecording (paragraph -> cacheGraphics.get());
	}
	if (anErrorHasOccurred)
		Melder_flushError (theErrorThatOccurred.get());
}

void structManual :: v_draw () {
	TRACE
	if (our visiblePageNumber == SEARCH_PAGE) {
		HyperPage_pageTitle (this, U"Best matches");
		HyperPage_intro (this, U"The best matches to your query seem to be:");
		for (int i = 1; i <= our numberOfMatches; i ++) {
			char32 link [300];
			const ManPage page = our manPages() -> pages.at [matches [i]];
			Melder_sprint (link,300, U"• @@", page -> title.get());
			HyperPage_listItem (this, link);
		}
		return;
	}
	const ManPage page = our manPages() -> pages.at [our visiblePageNumber];
	HyperPage_pageTitle (this, page -> title.get());   // TODO: check appropriateness of file name
	integer chunkNumber = 0;
	for (integer ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
		ManPage_Paragraph paragraph = & page -> paragraphs [ipar];
		switch (paragraph -> type) {
			case kManPage_type::INTRO: HyperPage_intro (this, paragraph -> text); break;
			case kManPage_type::ENTRY: HyperPage_entry (this, paragraph -> text); break;
			case kManPage_type::NORMAL: HyperPage_paragraph (this, paragraph -> text); break;
			case kManPage_type::LIST_ITEM: HyperPage_listItem (this, paragraph -> text); break;
			case kManPage_type::TERM: HyperPage_listTag (this, paragraph -> text); break;
			case kManPage_type::DEFINITION: HyperPage_definition (this, paragraph -> text); break;
			case kManPage_type::CODE: HyperPage_code (this, paragraph -> text); break;
			case kManPage_type::PROTOTYPE: HyperPage_prototype (this, paragraph -> text); break;
			case kManPage_type::EQUATION: HyperPage_formula (this, paragraph -> text); break;
			case kManPage_type::PICTURE: HyperPage_picture (this, paragraph -> width,
					paragraph -> height, paragraph -> draw); break;
			case kManPage_type::SCRIPT:
				++ chunkNumber;
				trace (U"Drawing chunk ", chunkNumber);
				HyperPage_script (this, paragraph -> width, paragraph -> height,
						paragraph -> text, paragraph -> cacheGraphics.get(), paragraph -> cacheInfo.string); break;
			case kManPage_type::LIST_ITEM1: HyperPage_listItem1 (this, paragraph -> text); break;
			case kManPage_type::LIST_ITEM2: HyperPage_listItem2 (this, paragraph -> text); break;
			case kManPage_type::LIST_ITEM3: HyperPage_listItem3 (this, paragraph -> text); break;
			case kManPage_type::TERM1: HyperPage_listTag1 (this, paragraph -> text); break;
			case kManPage_type::TERM2: HyperPage_listTag2 (this, paragraph -> text); break;
			case kManPage_type::TERM3: HyperPage_listTag3 (this, paragraph -> text); break;
			case kManPage_type::DEFINITION1: HyperPage_definition1 (this, paragraph -> text); break;
			case kManPage_type::DEFINITION2: HyperPage_definition2 (this, paragraph -> text); break;
			case kManPage_type::DEFINITION3: HyperPage_definition3 (this, paragraph -> text); break;
			case kManPage_type::CODE1: HyperPage_code1 (this, paragraph -> text); break;
			case kManPage_type::CODE2: HyperPage_code2 (this, paragraph -> text); break;
			case kManPage_type::CODE3: HyperPage_code3 (this, paragraph -> text); break;
			case kManPage_type::CODE4: HyperPage_code4 (this, paragraph -> text); break;
			case kManPage_type::CODE5: HyperPage_code5 (this, paragraph -> text); break;
			default: break;
		}
	}
	if (ManPages_uniqueLinksHither (our manPages(), our visiblePageNumber)) {
		integer ilink, jlink;
		bool goAhead = true;
		if (page -> paragraphs.size > 0) {
			conststring32 text = page -> paragraphs [page -> paragraphs.size]. text;
			if (! text || text [0] == U'\0' || text [Melder_length (text) - 1] != U':') {
				if (our printing && our suppressLinksHither)
					goAhead = false;
				else
					HyperPage_entry (this, U"Links to this page");
			}
		}
		if (goAhead) for (ilink = 1; ilink <= page -> linksHither.size; ilink ++) {
			const integer link = page -> linksHither [ilink];
			bool alreadyShown = false;
			for (jlink = 1; jlink <= page -> linksThither.size; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = true;
			if (! alreadyShown) {
				conststring32 title = our manPages() -> pages.at [page -> linksHither [ilink]] -> title.get();
				char32 linkText [304];
				Melder_sprint (linkText, 304, U"@@", title, U"@");
				HyperPage_listItem (this, linkText);
			}
		}
	}
	if (! our printing && page -> date) {
		char32 signature [100];
		const integer date = page -> date;
		const integer imonth = Melder_clipped (0_integer, date % 10000 / 100, 12_integer);
		Melder_sprint (signature,100,
			U"© ", str32equ (page -> author.get(), U"ppgb") ? U"Paul Boersma" :
			       str32equ (page -> author.get(), U"djmw") ? U"David Weenink" : page -> author.get(),
			U", ", date % 100,
			U" ", month [imonth],
			U" ", date / 10000);
		HyperPage_any (this, U"", our instancePref_font(), our instancePref_fontSize(), 0, 0.0,
			0.0, 0.0, 0.1, 0.1, HyperPage_ADD_BORDER);
		HyperPage_any (this, signature, our instancePref_font(), our instancePref_fontSize(), Graphics_ITALIC, 0.0,
			0.03, 0.0, 0.1, 0.0, 0);
	}
}

/********** PRINTING **********/

static void print (void *void_me, Graphics graphics) {
	iam (Manual);
	const integer numberOfPages = my manPages() -> pages.size, saveVisiblePageNumber = my visiblePageNumber;
	my ps = graphics;
	Graphics_setDollarSignIsCode (my ps, true);
	Graphics_setAtSignIsLink (my ps, true);
	my printing = true;
	HyperPage_initSheetOfPaper ((HyperPage) me);
	for (integer ipage = 1; ipage <= numberOfPages; ipage ++) {
		ManPage page = my manPages() -> pages.at [ipage];
		if (my printPagesStartingWith == nullptr ||
		    Melder_stringMatchesCriterion (page -> title.get(), kMelder_string::STARTS_WITH, my printPagesStartingWith, true))
		{
			my visiblePageNumber = ipage;
			my optionalCurrentPageTitle = Melder_dup_f (page -> title.get());
			my v_goToPage_number (ipage);
			my v_draw ();
			my v_goToPage_number (saveVisiblePageNumber);
		}
	}
	my printing = false;
	my printPagesStartingWith = nullptr;
}

static void menu_cb_printRange (Manual me, EDITOR_ARGS) {
	EDITOR_FORM (U"Print range", nullptr)
		SENTENCE (leftOrInsideHeader, U"Left or inside header", U"")
		SENTENCE (middleHeader, U"Middle header", U"")
		SENTENCE (rightOrOutsideHeader, U"Right or outside header", U"Manual")
		SENTENCE (leftOrInsideFooter, U"Left or inside footer", U"")
		SENTENCE (middleFooter, U"Middle footer", U"")
		SENTENCE (rightOrOutsideFooter, U"Right or outside footer", U"")
		BOOLEAN (mirrorEvenOddHeaders, U"Mirror even/odd headers", true)
		TEXTFIELD (printAllPagesWhoseTitleStartsWith, U"Print all pages whose title starts with", U"Intro", 2)
		INTEGER (firstPageNumber, U"First page number", U"1")
		BOOLEAN (suppressLinksToThisPage, U"Suppress \"Links to this page\"", false)
	EDITOR_OK
		SET_STRING (leftOrInsideHeader, date_STR().get())
		SET_STRING (rightOrOutsideHeader, my name.get())
		if (my d_printingPageNumber)
			SET_INTEGER (firstPageNumber, my d_printingPageNumber + 1)
		if (my visiblePageNumber >= 1 && my visiblePageNumber <= my manPages() -> pages.size) {
			ManPage page = my manPages() -> pages.at [my visiblePageNumber];
			SET_STRING (printAllPagesWhoseTitleStartsWith, page -> title.get());
		}
	EDITOR_DO
		my insideHeader = leftOrInsideHeader;
		my middleHeader = middleHeader;
		my outsideHeader = rightOrOutsideHeader;
		my insideFooter = leftOrInsideFooter;
		my middleFooter = middleFooter;
		my outsideFooter = rightOrOutsideFooter;
		my mirror = mirrorEvenOddHeaders;
		my printPagesStartingWith = printAllPagesWhoseTitleStartsWith;
		my d_printingPageNumber = firstPageNumber;
		my suppressLinksHither = suppressLinksToThisPage;
		Printer_print (print, me);
	EDITOR_END
}

/********** SEARCHING **********/

static double searchToken (ManPages me, integer ipage, conststring32 token) {
	double goodness = 0.0;
	ManPage page = my pages.at [ipage];
	if (! token [0])
		return 1.0;
	/*
		Try to find a match in the title, case-insensitively.
	*/
	static MelderString buffer;
	MelderString_copy (& buffer, page -> title.get());
	for (char32 *p = & buffer.string [0]; *p != U'\0'; p ++)
		*p = Melder_toLowerCase (*p);
	if (str32str (buffer.string, token)) {
		goodness += 300.0;   // lots of points for a match in the title!
		if (str32equ (buffer.string, token))
			goodness += 10000.0;   // even more points for an exact match!
	}
	/*
		Try to find a match in the paragraphs, case-insensitively.
	*/
	for (integer ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
		ManPage_Paragraph par = & page -> paragraphs [ipar];
		if (par -> text) {
			char32 *ptoken;
			MelderString_copy (& buffer, par -> text);
			for (char32 *p = & buffer.string [0]; *p != '\0'; p ++)
				*p = Melder_toLowerCase (*p);
			ptoken = str32str (buffer.string, token);
			if (ptoken) {
				goodness += 10.0;   // ten points for every paragraph with a match!
				if (str32str (ptoken + Melder_length (token), token))
					goodness += 1.0;   // one point for every second occurrence in a paragraph!
			}
		}
	}
	return goodness;
}

static void search (Manual me, conststring32 query) {
	const integer numberOfPages = my manPages() -> pages.size;
	static MelderString searchText;
	MelderString_copy (& searchText, query);
	for (char32 *p = & searchText.string [0]; *p != U'\0'; p ++) {
		if (*p == U'\n')
			*p = U' ';
		*p = Melder_toLowerCase (*p);
	}
	static autoVEC goodnessOfMatch;
	if (NUMisEmpty (goodnessOfMatch.get()))
		goodnessOfMatch = zero_VEC (numberOfPages);
	for (integer ipage = 1; ipage <= numberOfPages; ipage ++) {
		char32 *token = searchText.string;
		goodnessOfMatch [ipage] = 1.0;
		for (;;) {
			char32 *space = str32chr (token, U' ');
			if (space)
				*space = U'\0';
			goodnessOfMatch [ipage] *= searchToken (my manPages(), ipage, token);
			if (! space)
				break;
			*space = U' ';   // restore
			token = space + 1;
		}
	}
	/*
		Find the 20 best matches.
	*/
	my numberOfMatches = 0;
	for (integer imatch = 1; imatch <= 20; imatch ++) {
		integer imax = 0;
		double max = 0.0;
		for (integer ipage = 1; ipage <= numberOfPages; ipage ++) {
			if (goodnessOfMatch [ipage] > max) {
				max = goodnessOfMatch [ipage];
				imax = ipage;
			}
		}
		if (! imax) break;
		my matches [++ my numberOfMatches] = imax;
		goodnessOfMatch [imax] = 0.0;   // skip next time
	}
	HyperPage_goToPage_number (me, SEARCH_PAGE);
}

void Manual_search (Manual me, conststring32 query) {
	GuiText_setString (my searchText, query);
	search (me, query);
}

static void gui_button_cb_home (Manual me, GuiButtonEvent /* event */) {
	integer iHome = ManPages_lookUp (my manPages(), U"Intro");
	HyperPage_goToPage_number (me, iHome ? iHome : 1);
}
 
static void gui_button_cb_record (Manual me, GuiButtonEvent /* event */) {
	ManPage optionalManPage = ( my visiblePageNumber < 1 ? nullptr : my manPages() -> pages.at [my visiblePageNumber] );
	GuiThing_setSensitive (my recordButton,  false);
	GuiThing_setSensitive (my playButton,    false);
	GuiThing_setSensitive (my publishButton, false);
	#if defined (_WIN32)
		GdiFlush ();
	#endif
	if (! Melder_record (optionalManPage ? optionalManPage -> recordingTime : 1.0))
		Melder_flushError ();
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
	autostring32 query = GuiText_getString (my searchText);
	search (me, query.get());
}

static void gui_button_cb_search (Manual me, GuiButtonEvent /* event */) {
	do_search (me);
}

void structManual :: v_createChildren () {
	const bool hasRecordingButtons = ( our manPages() -> dynamic && our manPages() -> pages.at [1] -> recordingTime > 0.0 );
	our d_hasExtraRowOfTools = hasRecordingButtons;
	Manual_Parent :: v_createChildren ();
	#if defined (macintosh)
		#define STRING_SPACING 8
	#else
		#define STRING_SPACING 2
	#endif
	const int height = Machine_getTextHeight (), y = Machine_getMenuBarBottom () + 4;
	our homeButton = GuiButton_createShown (our windowForm, 104, 168, y, y + height,
		U"Home", gui_button_cb_home, this, 0);
	if (hasRecordingButtons) {
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

static void menu_cb_help (Manual me, EDITOR_ARGS) { HyperPage_goToPage (me, U"Manual"); }

void structManual :: v_createMenus () {
	Manual_Parent :: v_createMenus ();

	Editor_addCommand (this, U"File", U"Print manual...", 0, menu_cb_printRange);
	Editor_addCommand (this, U"File", U"Save page as HTML file...", 0, menu_cb_writeOneToHtmlFile);
	Editor_addCommand (this, U"File", U"Save manual to HTML folder...", 0, menu_cb_writeAllToHtmlFolder);
	Editor_addCommand (this, U"File", U"Save manual to HTML directory...", GuiMenu_DEPRECATED_2020, menu_cb_writeAllToHtmlFolder);
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);

	Editor_addCommand (this, U"Go to", U"Search for page (list)...", 0, menu_cb_searchForPageList);
}

void structManual :: v_createMenuItems_help (EditorMenu menu) {
	Manual_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"Manual help", '?', menu_cb_help);
}

void structManual :: v_defaultHeaders (EditorCommand cmd) {
	Manual me = (Manual) cmd -> d_editor;
	if (my visiblePageNumber > 0) {
		char32 string [400];
		static const conststring32 shortMonth [] =
			{ U"Jan", U"Feb", U"Mar", U"Apr", U"May", U"Jun", U"Jul", U"Aug", U"Sep", U"Oct", U"Nov", U"Dec" };
		const ManPage page = our manPages() -> pages.at [my visiblePageNumber];
		const integer date = page -> date;
		SET_STRING (my outsideHeader, page -> title.get())
		SET_STRING (my insideFooter, page -> author.get())
		if (date) {
			Melder_sprint (string,400, shortMonth [date % 10000 / 100 - 1], U" ", date % 100, U", ", date / 10000);
			SET_STRING (my insideHeader, string)
		}
	}
}

integer structManual :: v_getNumberOfPages () {
	return our manPages() -> pages.size;
}

integer structManual :: v_getCurrentPageNumber () {
	return our visiblePageNumber > 0 ? our visiblePageNumber : 1;
}

void structManual :: v_goToPage_number (integer goToPageNumber) {
	if (goToPageNumber < 1 || goToPageNumber > our manPages() -> pages.size) {
		if (goToPageNumber == SEARCH_PAGE) {
			our visiblePageNumber = SEARCH_PAGE;
			our optionalCurrentPageTitle. reset();
			return;
		} else Melder_throw (U"Page ", goToPageNumber, U" not found.");
	}
	our visiblePageNumber = goToPageNumber;
	ManPage page = our manPages() -> pages.at [our visiblePageNumber];
	our optionalCurrentPageTitle = Melder_dup_f (page -> title.get());
	Melder_assert (our optionalCurrentPageTitle);
	our manPages() -> invalidateCache ();
	Manual_runAllChunksToCache (page, this);
}

int structManual :: v_goToPage (conststring32 title) {
	if (! title) {
		our v_goToPage_number (SEARCH_PAGE);
		return 1;
	}
	Melder_assert (title);
	if (title [0] == U'\\' && title [1] == U'F' && title [2] == U'I') {
		structMelderFile file { };
		MelderDir_relativePathToFile (& our manPages() -> rootDirectory, title + 3, & file);
		Melder_recordFromFile (& file);
		return -1;
	} else if (title [0] == U'\\' && title [1] == U'S' && title [2] == U'C') {
		autoMelderSetDefaultDir saveDir (& our manPages() -> rootDirectory);
		autoPraatBackground background;
		try {
			autostring32 fileNameWithArguments = Melder_dup (title + 3);
			praat_executeScriptFromFileNameWithArguments (fileNameWithArguments.get());
		} catch (MelderError) {
			Melder_flushError ();
		}
		return 0;
	} else {
		const integer i = ManPages_lookUp (our manPages(), title);
		if (i == 0)
			Melder_throw (U"Page “", title, U"” not found.");
		our v_goToPage_number (i);
		return 1;
	}
}

autoManual Manual_create (conststring32 openingPageTitle, ManPages manPages, bool ownManPages) {
	Melder_assert (openingPageTitle);
	try {
		autoManual me = Thing_new (Manual);
		const integer lookUpPageNumber = ManPages_lookUp (manPages, openingPageTitle);
		if (lookUpPageNumber == 0)
			Melder_throw (U"Page “", openingPageTitle, U"” not found.");
		my visiblePageNumber = lookUpPageNumber;
		my optionalCurrentPageTitle = Melder_dup_f (openingPageTitle);
		/*
			The title of the window is the title of the whole manual, not the title of the page.
			If the first page has a title that starts with "-", then that is the title;
			otherwise, the title is just "Praat Manual".
		*/
		char32 windowTitle [101];
		if (manPages -> pages.at [1] -> title [0] == U'-') {
			Melder_sprint (windowTitle,101, & manPages -> pages.at [1] -> title [1]);
			if (windowTitle [Melder_length (windowTitle) - 1] == U'-')
				windowTitle [Melder_length (windowTitle) - 1] = U'\0';
		} else {
			Melder_sprint (windowTitle,101, U"Praat Manual");
		}
		my ownManPages = ownManPages;
		HyperPage_init1 (me.get(), windowTitle, manPages);
		MelderDir_copy (& manPages -> rootDirectory, & my rootDirectory);
		my history [0]. page = Melder_dup_f (openingPageTitle);   // BAD
		/*
			Cache the output of the opening page.
		*/
		ManPage openingPage = manPages -> pages.at [my visiblePageNumber];
		Manual_runAllChunksToCache (openingPage, me.get());

		HyperPage_init2 (me.get(), windowTitle, manPages);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Manual window not created.");
	}
}

/* End of file Manual.cpp */
