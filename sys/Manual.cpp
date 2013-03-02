/* Manual.cpp
 *
 * Copyright (C) 1996-2011 Paul Boersma
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
 * pb 2003/03/09 searching: more points for multiple occurrences within a paragraph
 * pb 2003/10/03 praat_executeFromFile without arguments
 * pb 2003/11/26 use recording time from file
 * pb 2003/11/30 removed newline from date
 * pb 2004/02/08 allow arguments in scripts
 * pb 2005/05/08 embedded scripts (for pictures)
 * pb 2005/07/19 moved navigation buttons to the top, removed page label and horizontal scroll bar
 * pb 2006/10/20 embedded scripts: not on opening page
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2008/01/19 double
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 * pb 2011/04/06 C++
 * pb 2011/07/05 C++
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

static const wchar_t *month [] =
	{ L"", L"January", L"February", L"March", L"April", L"May", L"June",
	  L"July", L"August", L"September", L"October", L"November", L"December" };

static void menu_cb_writeOneToHtmlFile (EDITOR_ARGS) {
	EDITOR_IAM (Manual);
	EDITOR_FORM_WRITE (L"Save as HTML file", 0)
		ManPages manPages = (ManPages) my data;
		wchar_t *p = defaultName;
		wcscpy (p, ((ManPage) manPages -> pages -> item [my path]) -> title);
		while (*p) { if (! isalnum (*p) && *p != '_') *p = '_'; p ++; }
		wcscat (defaultName, L".html");
	EDITOR_DO_WRITE
		ManPages_writeOneToHtmlFile ((ManPages) my data, my path, file);
	EDITOR_END
}

static void menu_cb_writeAllToHtmlDir (EDITOR_ARGS) {
	EDITOR_IAM (Manual);
	EDITOR_FORM (L"Save all pages as HTML files", 0)
		LABEL (L"", L"Type a directory name:")
		TEXTFIELD (L"directory", L"")
	EDITOR_OK
		structMelderDir currentDirectory = { { 0 } };
		Melder_getDefaultDir (& currentDirectory);
		SET_STRING (L"directory", Melder_dirToPath (& currentDirectory))
	EDITOR_DO
		wchar_t *directory = GET_STRING (L"directory");
		ManPages_writeAllToHtmlDir ((ManPages) my data, directory);
	EDITOR_END
}

static void menu_cb_searchForPageList (EDITOR_ARGS) {
	EDITOR_IAM (Manual);
	EDITOR_FORM (L"Search for page", 0)
		ManPages manPages = (ManPages) my data;
		long numberOfPages;
		const wchar_t **pages = ManPages_getTitles (manPages, & numberOfPages);
		LIST (L"Page", manPages -> pages -> size, pages, 1)
	EDITOR_OK
	EDITOR_DO
		HyperPage_goToPage_i (me, GET_INTEGER (L"Page"));
	EDITOR_END
}

void structManual :: v_draw () {
	ManPages manPages = (ManPages) data;
	ManPage page;
	ManPage_Paragraph paragraph;
	#if motif
	Graphics_clearWs (g);
	#endif
	if (path == SEARCH_PAGE) {
		HyperPage_pageTitle (this, L"Best matches");
		HyperPage_intro (this, L"The best matches to your query seem to be:");
		for (int i = 1; i <= numberOfMatches; i ++) {
			wchar_t link [300];
			page = (ManPage) manPages -> pages -> item [matches [i]];
			swprintf (link, 300, L"\\bu @@%ls", page -> title);
			HyperPage_listItem (this, link);
		}
		return;
	}
	page = (ManPage) manPages -> pages -> item [path];
	if (! paragraphs) return;
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
	if (ManPages_uniqueLinksHither (manPages, path)) {
		long ilink, jlink, lastParagraph = 0;
		int goAhead = TRUE;
		while (page -> paragraphs [lastParagraph]. type != 0) lastParagraph ++;
		if (lastParagraph > 0) {
			const wchar_t *text = page -> paragraphs [lastParagraph - 1]. text;
			if (text == NULL || text [0] == '\0' || text [wcslen (text) - 1] != ':') {
				if (printing && suppressLinksHither)
					goAhead = FALSE;
				else
					HyperPage_entry (this, L"Links to this page");
			}
		}
		if (goAhead) for (ilink = 1; ilink <= page -> nlinksHither; ilink ++) {
			long link = page -> linksHither [ilink];
			int alreadyShown = FALSE;
			for (jlink = 1; jlink <= page -> nlinksThither; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = TRUE;
			if (! alreadyShown) {
				const wchar_t *title = ((ManPage) manPages -> pages -> item [page -> linksHither [ilink]]) -> title;
				wchar_t linkText [304];
				swprintf (linkText, 304, L"@@%ls@", title);
				HyperPage_listItem (this, linkText);
			}
		}
	}
	if (! printing && page -> date) {
		wchar_t signature [100];
		long date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12) imonth = 0;
		swprintf (signature, 100, L"\\co %ls, %ld %ls %ld",
			wcsequ (page -> author, L"ppgb") ? L"Paul Boersma" :
			wcsequ (page -> author, L"djmw") ? L"David Weenink" : page -> author,
			date % 100, month [imonth], date / 10000);
		HyperPage_any (this, L"", p_font, p_fontSize, 0, 0.0,
			0.0, 0.0, 0.1, 0.1, HyperPage_ADD_BORDER);
		HyperPage_any (this, signature, p_font, p_fontSize, Graphics_ITALIC, 0.0,
			0.03, 0.0, 0.1, 0.0, 0);
	}
}

/********** PRINTING **********/

static void print (I, Graphics graphics) {
	iam (Manual);
	ManPages manPages = (ManPages) my data;
	long numberOfPages = manPages -> pages -> size, savePage = my path;
	my ps = graphics;
	Graphics_setDollarSignIsCode (my ps, TRUE);
	Graphics_setAtSignIsLink (my ps, TRUE);
	my printing = TRUE;
	HyperPage_initSheetOfPaper ((HyperPage) me);
	for (long ipage = 1; ipage <= numberOfPages; ipage ++) {
		ManPage page = (ManPage) manPages -> pages -> item [ipage];
		if (my printPagesStartingWith == NULL ||
		    Melder_stringMatchesCriterion (page -> title, kMelder_string_STARTS_WITH, my printPagesStartingWith))
		{
			ManPage_Paragraph par;
			my path = ipage;
			my paragraphs = page -> paragraphs;
			my numberOfParagraphs = 0;
			par = my paragraphs;
			while ((par ++) -> type) my numberOfParagraphs ++;
			Melder_free (my currentPageTitle);
			my currentPageTitle = Melder_wcsdup_f (page -> title);
			my v_goToPage_i (ipage);
			my v_draw ();
			my v_goToPage_i (savePage);
		}
	}
	my printing = FALSE;
	my printPagesStartingWith = NULL;
}

static void menu_cb_printRange (EDITOR_ARGS) {
	EDITOR_IAM (Manual);
	EDITOR_FORM (L"Print range", 0)
		SENTENCE (L"Left or inside header", L"")
		SENTENCE (L"Middle header", L"")
		SENTENCE (L"Right or outside header", L"Manual")
		SENTENCE (L"Left or inside footer", L"")
		SENTENCE (L"Middle footer", L"")
		SENTENCE (L"Right or outside footer", L"")
		BOOLEAN (L"Mirror even/odd headers", TRUE)
		LABEL (L"", L"Print all pages whose title starts with:")
		TEXTFIELD (L"Print pages starting with", L"Intro")
		INTEGER (L"First page number", L"1")
		BOOLEAN (L"Suppress \"Links to this page\"", FALSE)
	EDITOR_OK
		ManPages manPages = (ManPages) my data;
		time_t today = time (NULL);
		char dateA [50];
		#ifdef UNIX
			struct tm *tm = localtime (& today);
			strftime (dateA, 50, "%B %e, %Y", tm);
		#else
			strcpy (dateA, ctime (& today));
		#endif
		wchar_t *date = Melder_peekUtf8ToWcs (dateA), *newline;
		newline = wcschr (date, '\n'); if (newline) *newline = '\0';
		SET_STRING (L"Left or inside header", date)
		SET_STRING (L"Right or outside header", my name)
		if (my d_printingPageNumber) SET_INTEGER (L"First page number", my d_printingPageNumber + 1)
		if (my path >= 1 && my path <= manPages -> pages -> size) {
			ManPage page = (ManPage) manPages -> pages -> item [my path];
			SET_STRING (L"Print pages starting with", page -> title);
		}
	EDITOR_DO
		my insideHeader = GET_STRING (L"Left or inside header");
		my middleHeader = GET_STRING (L"Middle header");
		my outsideHeader = GET_STRING (L"Right or outside header");
		my insideFooter = GET_STRING (L"Left or inside footer");
		my middleFooter = GET_STRING (L"Middle footer");
		my outsideFooter = GET_STRING (L"Right or outside footer");
		my mirror = GET_INTEGER (L"Mirror even/odd headers");
		my printPagesStartingWith = GET_STRING (L"Print pages starting with");
		my d_printingPageNumber = GET_INTEGER (L"First page number");
		my suppressLinksHither = GET_INTEGER (L"Suppress \"Links to this page\"");
		Printer_print (print, me);
	EDITOR_END
}

/********** SEARCHING **********/

static double *goodnessOfMatch;

static double searchToken (ManPages me, long ipage, wchar_t *token) {
	double goodness = 0.0;
	ManPage page = (ManPage) my pages -> item [ipage];
	struct structManPage_Paragraph *par = & page -> paragraphs [0];
	if (! token [0]) return 1.0;
	/*
	 * Try to find a match in the title, case insensitively.
	 */
	static MelderString buffer = { 0 };
	MelderString_copy (& buffer, page -> title);
	for (wchar_t *p = & buffer.string [0]; *p != '\0'; p ++) *p = tolower (*p);
	if (wcsstr (buffer.string, token)) {
		goodness += 300.0;   /* Lots of points for a match in the title! */
		if (wcsequ (buffer.string, token))
			goodness += 10000.0;   /* Even more points for an exact match! */
	}
	/*
	 * Try to find a match in the paragraphs, case-insensitively.
	 */
	while (par -> type) {
		if (par -> text) {
			wchar_t *ptoken;
			MelderString_copy (& buffer, par -> text);
			for (wchar_t *p = & buffer.string [0]; *p != '\0'; p ++) *p = tolower (*p);
			ptoken = wcsstr (buffer.string, token);
			if (ptoken) {
				goodness += 10.0;   /* Ten points for every paragraph with a match! */
				if (wcsstr (ptoken + wcslen (token), token)) {
					goodness += 1.0;   /* One point for every second occurrence in a paragraph! */
				}
			}
		}
		par ++;
	}
	return goodness;
}

static void search (Manual me, const wchar_t *query) {
	ManPages manPages = (ManPages) my data;
	long numberOfPages = manPages -> pages -> size;
	static MelderString searchText = { 0 };
	MelderString_copy (& searchText, query);
	for (wchar_t *p = & searchText.string [0]; *p != '\0'; p ++) {
		if (*p == '\n') *p = ' ';
		*p = tolower (*p);
	}
	if (! goodnessOfMatch)
		goodnessOfMatch = NUMvector <double> (1, numberOfPages);
	for (long ipage = 1; ipage <= numberOfPages; ipage ++) {
		wchar_t *token = searchText.string;
		goodnessOfMatch [ipage] = 1.0;
		for (;;) {
			wchar_t *space = wcschr (token, ' ');
			if (space) *space = '\0';
			goodnessOfMatch [ipage] *= searchToken (manPages, ipage, token);
			if (! space) break;
			*space = ' ';   // restore
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

void Manual_search (Manual me, const wchar_t *query) {
	my searchText -> f_setString (query);
	search (me, query);
}

static void gui_button_cb_home (I, GuiButtonEvent event) {
	(void) event;
	iam (Manual);
	ManPages pages = (ManPages) my data;
	long iHome = ManPages_lookUp (pages, L"Intro");
	HyperPage_goToPage_i (me, iHome ? iHome : 1);
}
 
static void gui_button_cb_record (I, GuiButtonEvent event) {
	(void) event;
	iam (Manual);
	ManPages manPages = (ManPages) my data;
	ManPage manPage = (ManPage) (my path < 1 ? NULL : manPages -> pages -> item [my path]);
	my recordButton  -> f_setSensitive (false);
	my playButton    -> f_setSensitive (false);
	my publishButton -> f_setSensitive (false);
	#if motif
		XmUpdateDisplay (my d_windowForm -> d_xmShell);
	#endif
	if (! Melder_record (manPage == NULL ? 1.0 : manPage -> recordingTime)) Melder_flushError (NULL);
	my recordButton  -> f_setSensitive (true);
	my playButton    -> f_setSensitive (true);
	my publishButton -> f_setSensitive (true);
}

static void gui_button_cb_play (I, GuiButtonEvent event) {
	(void) event;
	iam (Manual);
	my recordButton  -> f_setSensitive (false);
	my playButton    -> f_setSensitive (false);
	my publishButton -> f_setSensitive (false);
	#if motif
		XmUpdateDisplay (my d_windowForm -> d_xmShell);
	#endif
	Melder_play ();
	my recordButton  -> f_setSensitive (true);
	my playButton    -> f_setSensitive (true);
	my publishButton -> f_setSensitive (true);
}

static void gui_button_cb_publish (I, GuiButtonEvent event) {
	(void) event;
	iam (Manual);
	(void) me;
	Melder_publishPlayed ();
}

static void do_search (Manual me) {
	wchar_t *query = my searchText -> f_getString ();
	search (me, query);
	Melder_free (query);
}

static void gui_button_cb_search (I, GuiButtonEvent event) {
	(void) event;
	iam (Manual);
	do_search (me);
}

static void gui_cb_search (GUI_ARGS) {
	GUI_IAM (Manual);
	do_search (me);
}

void structManual :: v_createChildren () {
	Manual_Parent :: v_createChildren ();
	ManPages pages = (ManPages) data;   // has been installed here by Editor_init ()
	#if defined (macintosh)
		#define STRING_SPACING 8
	#else
		#define STRING_SPACING 2
	#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	homeButton = GuiButton_createShown (d_windowForm, 104, 168, y, y + height,
		L"Home", gui_button_cb_home, this, 0);
	if (pages -> dynamic) {
		#if motif
			XtVaSetValues (drawingArea -> d_widget, XmNtopOffset, y + height * 2 + 16, NULL);
			XtVaSetValues (verticalScrollBar -> d_widget, XmNtopOffset, y + height * 2 + 16, NULL);
		#endif
		recordButton = GuiButton_createShown (d_windowForm, 4, 79, y+height+8, y+height+8 + height,
			L"Record", gui_button_cb_record, this, 0);
		playButton = GuiButton_createShown (d_windowForm, 85, 160, y+height+8, y+height+8 + height,
			L"Play", gui_button_cb_play, this, 0);
		publishButton = GuiButton_createShown (d_windowForm, 166, 166 + 175, y+height+8, y+height+8 + height, 
			L"Copy last played to list", gui_button_cb_publish, this, 0);
	}
	GuiButton_createShown (d_windowForm, 274, 274 + 69, y, y + height,
		L"Search:", gui_button_cb_search, this, GuiButton_DEFAULT);
	searchText = GuiText_createShown (d_windowForm, 274+69 + STRING_SPACING, 452 + STRING_SPACING - 2, y, y + Gui_TEXTFIELD_HEIGHT, 0);
}

static void menu_cb_help (EDITOR_ARGS) { EDITOR_IAM (Manual); HyperPage_goToPage (me, L"Manual"); }

void structManual :: v_createMenus () {
	Manual_Parent :: v_createMenus ();

	Editor_addCommand (this, L"File", L"Print manual...", 0, menu_cb_printRange);
	Editor_addCommand (this, L"File", L"Save page as HTML file...", 0, menu_cb_writeOneToHtmlFile);
	Editor_addCommand (this, L"File", L"Save manual to HTML directory...", 0, menu_cb_writeAllToHtmlDir);
	Editor_addCommand (this, L"File", L"-- close --", 0, NULL);

	Editor_addCommand (this, L"Go to", L"Search for page (list)...", 0, menu_cb_searchForPageList);
}

void structManual :: v_createHelpMenuItems (EditorMenu menu) {
	Manual_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"Manual help", '?', menu_cb_help);
}

void structManual :: v_defaultHeaders (EditorCommand cmd) {
	Manual me = (Manual) cmd -> d_editor;
	ManPages manPages = (ManPages) my data;
	if (my path) {
		wchar_t string [400];
		static const wchar_t *shortMonth [] =
			{ L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
		ManPage page = (ManPage) manPages -> pages -> item [my path];
		long date = page -> date;
		SET_STRING (L"Right or outside header", page -> title)
		SET_STRING (L"Left or inside footer", page -> author)
		if (date) {
			swprintf (string, 400, L"%ls %ld, %ld", shortMonth [date % 10000 / 100 - 1], date % 100, date / 10000);
			SET_STRING (L"Left or inside header", string)
		}
	}
}

long structManual :: v_getNumberOfPages () {
	ManPages manPages = (ManPages) data;
	return manPages -> pages -> size;
}

long structManual :: v_getCurrentPageNumber () {
	return path ? path : 1;
}

void structManual :: v_goToPage_i (long pageNumber) {
	ManPages manPages = (ManPages) data;
	if (pageNumber < 1 || pageNumber > manPages -> pages -> size) {
		if (pageNumber == SEARCH_PAGE) {
			path = SEARCH_PAGE;
			Melder_free (currentPageTitle);
			return;
		} else Melder_throw ("Page ", pageNumber, " not found.");
	}
	path = pageNumber;
	ManPage page = (ManPage) manPages -> pages -> item [path];
	paragraphs = page -> paragraphs;
	numberOfParagraphs = 0;
	ManPage_Paragraph par = paragraphs;
	while ((par ++) -> type) numberOfParagraphs ++;
	Melder_free (currentPageTitle);
	currentPageTitle = Melder_wcsdup_f (page -> title);
}

int structManual :: v_goToPage (const wchar_t *title) {
	ManPages manPages = (ManPages) data;
	if (title [0] == '\\' && title [1] == 'F' && title [2] == 'I') {
		structMelderFile file = { 0 };
		MelderDir_relativePathToFile (& manPages -> rootDirectory, title + 3, & file);
		Melder_recordFromFile (& file);
		return -1;
	} else if (title [0] == '\\' && title [1] == 'S' && title [2] == 'C') {
		autoMelderSetDefaultDir dir (& manPages -> rootDirectory);
		autoPraatBackground background;
		try {
			praat_executeScriptFromFileNameWithArguments (title + 3);
		} catch (MelderError) {
			Melder_flushError (NULL);
		}
		return 0;
	} else {
		long i = ManPages_lookUp (manPages, title);
		if (! i)
			Melder_throw ("Page \"", title, "\" not found.");
		v_goToPage_i (i);
		return 1;
	}
}

void Manual_init (Manual me, const wchar_t *title, Data data, bool ownData) {
	ManPages manPages = (ManPages) data;
	wchar_t windowTitle [101];
	long i;
	ManPage page;
	ManPage_Paragraph par;
	if (! (i = ManPages_lookUp (manPages, title)))
		Melder_throw ("Page \"", title, "\" not found.");
	my path = i;
	page = (ManPage) manPages -> pages -> item [i];
	my paragraphs = page -> paragraphs;
	my numberOfParagraphs = 0;
	par = my paragraphs;
	while ((par ++) -> type) my numberOfParagraphs ++;

	if (((ManPage) manPages -> pages -> item [1]) -> title [0] == '-') {
		wcscpy (windowTitle, & ((ManPage) manPages -> pages -> item [1]) -> title [1]);
		if (windowTitle [wcslen (windowTitle) - 1] == '-') windowTitle [wcslen (windowTitle) - 1] = '\0';
	} else {
		wcscpy (windowTitle, L"Manual");
	}
	my d_ownData = ownData;
	HyperPage_init (me, windowTitle, data);
	MelderDir_copy (& manPages -> rootDirectory, & my rootDirectory);
	my history [0]. page = Melder_wcsdup_f (title);   /* BAD */
}

Manual Manual_create (const wchar_t *title, Data data, bool ownData) {
	try {
		autoManual me = Thing_new (Manual);
		Manual_init (me.peek(), title, data, ownData);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Manual window not created.");
	}
}

/* End of file Manual.cpp */
