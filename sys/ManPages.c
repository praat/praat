/* ManPages.c
 *
 * Copyright (C) 1996-2007 Paul Boersma
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
 * pb 2002/12/19 corrected word-bold-italic for HTML files
 * pb 2003/09/14 replaced MelderDir_getFile with MelderDir_relativePathToFile to allow man pages in directory trees
 * pb 2004/09/27 corrected check for script files with arguments
 * pb 2004/10/16 C++ compatible structs
 * pb 2004/11/21 a link can have upper case while the man page has lower case
 * pb 2006/10/20 embedded scripts
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/10 MelderInfo
 * pb 2006/12/15 turned HTML special symbols into Unicode (and removed glyph pictures)
 * pb 2006/12/28 repaired a memory leak
 * pb 2007/06/11 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/08/25 added an extra count to "par" in readOnePage (awful bug)
 * pb 2007/10/01 made sure that non-ASCII characters in ManPage code are written as ASCII Unicode numbers
 */

#include <ctype.h>
#include "ManPages.h"
#include "longchar.h"
#define LONGEST_FILE_NAME  55

static int isAllowedFileNameCharacter (int c) {
	return isalnum (c) || c == '_' || c == '-' || c == '+';
}
static int isSingleWordCharacter (int c) {
	return isalnum (c) || c == '_';
}

static long lookUp_unsorted (ManPages me, const wchar_t *title);

static void classManPages_destroy (I) { iam (ManPages);
	if (my dynamic && my pages) {
		for (long ipage = 1; ipage <= my pages -> size; ipage ++) {
			ManPage page = my pages -> item [ipage];
			Melder_free (page -> title);
			Melder_free (page -> author);
			if (page -> paragraphs) {
				ManPage_Paragraph par;
				for (par = page -> paragraphs; par -> type; par ++)
					Melder_free (par -> text);
				NUMvector_free (sizeof (struct structManPage_Paragraph), page -> paragraphs, 0);
			}
			if (ipage == 1) {
				NUMlvector_free (page -> linksHither, 1);
				NUMlvector_free (page -> linksThither, 1);
			}
		}
	}
	if (my pages && my titles)
		for (long ipage = 1; ipage <= my pages -> size; ipage ++) {
			Melder_free (my titles [ipage]);
		}
	forget (my pages);
	NUMpvector_free ((void **) my titles, 1);
	inherited (ManPages) destroy (me);
}

static const wchar_t *extractLink (const wchar_t *text, const wchar_t *p, wchar_t *link) {
	wchar_t *to = link, *max = link + 300;
	if (p == NULL) p = text;
	/*
	 * Search for next '@' that is not in a backslash sequence.
	 */
	for (;;) {
		p = wcschr (p, '@');
		if (! p) return NULL;   /* No more '@'. */
		if (p - text <= 0 || (p [-1] != '\\' && (p - text <= 1 || p [-2] != '\\'))) break;
		p ++;
	}
	Melder_assert (*p == '@');
	if (p [1] == '@') {
		const wchar_t *from = p + 2;
		while (*from != '@' && *from != '|' && *from != '\0') {
			if (to >= max) {
				Melder_error2 (L"(ManPages::grind:) Link starting with \"@@\" is too long:\n", text);
				return NULL;
			}
			*to ++ = *from ++;
		}
		if (*from == '|') { from ++; while (*from != '@' && *from != '\0') from ++; }
		if (*from) p = from + 1; else p = from;   /* Skip '@' but not '\0'. */
	} else {
		const wchar_t *from = p + 1;
		while (isSingleWordCharacter (*from)) {
			if (to >= max) {
				Melder_error2 (L"(ManPages::grind:) Link starting with \"@@\" is too long:\n", text);
				return NULL;
			}
			*to ++ = *from ++;
		}
		p = from;
	}
	*to = '\0';
	return p;
}

static int readOnePage (ManPages me, MelderReadString *text) {
	ManPage page;
	ManPage_Paragraph par;
	wchar_t *title = texgetw2 (text);
	if (! title) return Melder_error1 (L"Cannot find page title.");

	/*
	 * Check whether a page with this title is already present.
	 */
	if (lookUp_unsorted (me, title)) {
		Melder_free (title);   // memory leak repaired, ppgb 20061228
		return 1;
	}

	page = new (ManPage);
	page -> title = title;

	/*
	 * Add the page early, so that lookUp can find it.
	 */
	if (! Collection_addItem (my pages, page)) return 0;

	page -> author = texgetw2 (text);
	if (! page -> author) return Melder_error1 (L"Cannot find author.");
	page -> date = texgetu4 (text);
	iferror return Melder_error1 (L"Cannot find date.");
	page -> recordingTime = texgetr8 (text);
	iferror return Melder_error1 (L"Cannot find recording time.");
	page -> paragraphs = NUMvector (sizeof (struct structManPage_Paragraph), 0, 500);
	if (! page -> paragraphs) return 0;
	for (par = page -> paragraphs;; par ++) {
		wchar_t link [501], fileName [256];
		const wchar_t *p;
		par -> type = texgete1 (text, & enum_ManPage_TYPE);
		if (Melder_hasError ()) {
			if (wcsstr (Melder_getError (), L"end of text")) {
				Melder_clearError ();
				break;
			} else {
				return 0;
			}
		}
		if (par -> type == enumi (ManPage_TYPE, script)) {
			par -> width = texgetr4 (text);
			par -> height = texgetr4 (text);
		}
		par -> text = texgetw2 (text);
		if (! par -> text) return Melder_error1 (L"Cannot find text.");
		for (p = extractLink (par -> text, NULL, link); p != NULL; p = extractLink (par -> text, p, link)) {
			/*
			 * Now, `link' contains the link text, with spaces and all.
			 * Transform it into a file name.
			 */
			structMelderFile file2 = { 0 };
			if (link [0] == '\\' && link [1] == 'F' && link [2] == 'I') {
				/*
				 * A link to a sound file: see if it exists.
				 */
				MelderDir_relativePathToFile (& my rootDirectory, link + 3, & file2);
				if (Melder_hasError ()) {
					Melder_clearError ();
					Melder_warning ("Cannot find sound file \"%ls\".", link + 3);
				} else if (! MelderFile_exists (& file2)) {
					Melder_warning ("Cannot find sound file \"%s\".", MelderFile_messageName (& file2));
				}
			} else if (link [0] == '\\' && link [1] == 'S' && link [2] == 'C') {
				/*
				 * A link to a script: see if it exists.
				 */
				wchar_t *p = link + 3;
				if (*p == '\"') {
					wchar_t *q = fileName;
					p ++;
					while (*p != '\"' && *p != '\0') * q ++ = * p ++;
					*q = '\0';
				} else {
					wchar_t *q = fileName;
					while (*p != ' ' && *p != '\0') * q ++ = * p ++;   // One word, up to the next space.
					*q = '\0';
				}
				MelderDir_relativePathToFile (& my rootDirectory, fileName, & file2);
				if (Melder_hasError ()) {
					Melder_clearError ();
					Melder_warning ("Cannot find script \"%ls\".", fileName);
				} else if (! MelderFile_exists (& file2)) {
					Melder_warning ("Cannot find script \"%ls\".", MelderFile_messageNameW (& file2));
				}
				my executable = TRUE;
			} else {
				wchar_t *q;
				/*
				 * A link to another page: follow it.
				 */
				for (q = link; *q; q ++) if (! isAllowedFileNameCharacter (*q)) *q = '_';
				wcscpy (fileName, link);
				wcscat (fileName, L".man");
				MelderDir_getFile (& my rootDirectory, fileName, & file2);
				wchar_t *string2 = MelderFile_readText (& file2);
				if (string2 != NULL) {
					MelderReadString text2 = { string2, string2 };
					if (! readOnePage (me, & text2)) {
						Melder_free (string2);
						return Melder_error3 (L"File \"", MelderFile_messageNameW (& file2), L"\".");
					}
				} else {
					/*
					 * Second try: with upper case.
					 */
					Melder_clearError ();
					link [0] = toupper (link [0]);
					wcscpy (fileName, link);
					wcscat (fileName, L".man");
					MelderDir_getFile (& my rootDirectory, fileName, & file2);
					string2 = MelderFile_readText (& file2);
					if (string2 == NULL) return 0;
					MelderReadString text2 = { string2, string2 };
					if (! readOnePage (me, & text2)) {
						Melder_free (string2);
						return Melder_error3 (L"File \"", MelderFile_messageNameW (& file2), L"\".");
					}
				}
				Melder_free (string2);
			}
		}
		iferror return 0;
	}
	++ par;   // Room for the last paragraph (because counting starts at 0).
	++ par;   // Room for the final zero-type paragraph.
	Melder_realloc (page -> paragraphs, sizeof (struct structManPage_Paragraph) * (par - page -> paragraphs));
	return 1;
}
static int classManPages_readText (I, MelderReadString *text) {
	iam (ManPages);
	my dynamic = TRUE;
	my pages = Ordered_create ();
	MelderDir_copy (& Data_directoryBeingRead, & my rootDirectory);
	return readOnePage (me, text);
}

class_methods (ManPages, Data)
	class_method_local (ManPages, destroy)
	class_method_local (ManPages, readText)
class_methods_end

ManPages ManPages_create (void) {
	ManPages me = new (ManPages);
	my pages = Ordered_create ();
	return me;
}

int ManPages_addPage (ManPages me, const wchar_t *title, const wchar_t *author, long date,
	struct structManPage_Paragraph paragraphs [])
{
	ManPage page = new (ManPage);
	page -> title = title;
	page -> paragraphs = & paragraphs [0];
	page -> author = author;
	page -> date = date;
	Collection_addItem (my pages, page);
	return ! Melder_hasError ();
}

static int pageCompare (const void *first, const void *second) {
	ManPage me = * (ManPage *) first, thee = * (ManPage *) second;
	const wchar_t *p = my title, *q = thy title;
	for (;;) {
		int plower = tolower (*p), qlower = tolower (*q);
		if (plower < qlower) return -1;
		if (plower > qlower) return 1;
		if (plower == '\0') return wcscmp (my title, thy title);
		p ++, q ++;
	}
	return 0;   /* Should not occur. */
}

static long lookUp_unsorted (ManPages me, const wchar_t *title) {
	long i;

	/*
	 * First try to match an unaltered 'title' with the titles of the man pages.
	 */
	for (i = 1; i <= my pages -> size; i ++) {
		ManPage page = my pages -> item [i];
		if (wcsequ (page -> title, title)) return i;
	}

	/*
	 * If that fails, try to find the upper-case variant.
	 */
	if (islower (title [0])) {
		wchar_t upperTitle [300];
		wcscpy (upperTitle, title);
		upperTitle [0] = toupper (upperTitle [0]);
		for (i = 1; i <= my pages -> size; i ++) {
			ManPage page = my pages -> item [i];
			if (wcsequ (page -> title, upperTitle)) return i;
		}
	}
	return 0;
}

static long lookUp_sorted (ManPages me, const wchar_t *title) {
	static ManPage dummy;
	ManPage *page;
	if (! dummy) dummy = new (ManPage);
	dummy -> title = title;
	page = bsearch (& dummy, & my pages -> item [1], my pages -> size, sizeof (ManPage), pageCompare);
	if (page) return (page - (ManPage *) & my pages -> item [1]) + 1;
	if (islower (title [0]) || isupper (title [0])) {
		wchar_t caseSwitchedTitle [300];
		wcscpy (caseSwitchedTitle, title);
		caseSwitchedTitle [0] = islower (title [0]) ? toupper (caseSwitchedTitle [0]) : tolower (caseSwitchedTitle [0]);
		dummy -> title = caseSwitchedTitle;
		page = bsearch (& dummy, & my pages -> item [1], my pages -> size, sizeof (ManPage), pageCompare);
		if (page) return (page - (ManPage *) & my pages -> item [1]) + 1;
	}
	return 0;
}

static void grind (ManPages me) {
	long ipage, ndangle = 0, jpage, grandNlinks, ilinkHither, ilinkThither;
	long *grandLinksHither, *grandLinksThither;

	qsort (& my pages -> item [1], my pages -> size, sizeof (ManPage), pageCompare);

	/*
	 * First pass: count and check links: fill in nlinksHither and nlinksThither.
	 */
	grandNlinks = 0;
	for (ipage = 1; ipage <= my pages -> size; ipage ++) {
		ManPage page = my pages -> item [ipage];
		int ipar;
		for (ipar = 0; page -> paragraphs [ipar]. type; ipar ++) {
			const wchar_t *text = page -> paragraphs [ipar]. text, *p;
			wchar_t link [301];
			if (text) for (p = extractLink (text, NULL, link); p != NULL; p = extractLink (text, p, link)) {
				if (link [0] == '\\' && ((link [1] == 'F' && link [2] == 'I') || (link [1] == 'S' && link [2] == 'C')))
					continue;   /* Ignore "FILE" links. */
				if ((jpage = lookUp_sorted (me, link)) != 0) {
					page -> nlinksThither ++;
					((ManPage) my pages -> item [jpage]) -> nlinksHither ++;
					grandNlinks ++;
				} else {
					MelderInfo_writeLine5 (L"Page \"", page -> title, L"\" contains a dangling link to \"", link, L"\".");
					ndangle ++;
				}
			}
			iferror Melder_flushError (NULL);
		}
	}
	if (ndangle) {
		MelderInfo_close ();
		Melder_warning ("(ManPages::grind:) %ld dangling links encountered. See console window.", ndangle);
	}

	/*
	 * Second pass: allocate memory: fill in linksHither and linksThither.
	 * Some optimization required: use only two mallocs.
	 * Forget nlinksHither and nlinksThither.
	 */
	if (grandNlinks == 0) { my ground = TRUE; return; }
	if (! (grandLinksHither = NUMlvector (1, grandNlinks)) || ! (grandLinksThither = NUMlvector (1, grandNlinks))) {
		Melder_flushError (NULL);
		return;
	}
	ilinkHither = ilinkThither = 0;
	for (ipage = 1; ipage <= my pages -> size; ipage ++) {
		ManPage page = my pages -> item [ipage];
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
	for (ipage = 1; ipage <= my pages -> size; ipage ++) {
		ManPage page = my pages -> item [ipage];
		int ipar;
		for (ipar = 0; page -> paragraphs [ipar]. type; ipar ++) {
			const wchar_t *text = page -> paragraphs [ipar]. text, *p;
			wchar_t link [301];
			if (text) for (p = extractLink (text, NULL, link); p != NULL; p = extractLink (text, p, link)) {
				if (link [0] == '\\' && ((link [1] == 'F' && link [2] == 'I') || (link [1] == 'S' && link [2] == 'C')))
					continue;   /* Ignore "FILE" links. */
				if ((jpage = lookUp_sorted (me, link)) != 0) {
					int ilink, alreadyPresent = FALSE;
					for (ilink = 1; ilink <= page -> nlinksThither; ilink ++) {
						if (page -> linksThither [ilink] == jpage) {
							alreadyPresent = TRUE;
							break;
						}
					}
					if (! alreadyPresent) {
						ManPage otherPage = my pages -> item [jpage];
						page -> linksThither [++ page -> nlinksThither] = jpage;
						otherPage -> linksHither [++ otherPage -> nlinksHither] = ipage;
					}
				}
			}
			iferror Melder_flushError (NULL);
		}
	}

	my ground = TRUE;
}

long ManPages_uniqueLinksHither (ManPages me, long ipage) {
	ManPage page = my pages -> item [ipage];
	long result = page -> nlinksHither, ilinkHither, ilinkThither;
	for (ilinkHither = 1; ilinkHither <= page -> nlinksHither; ilinkHither ++) {
		long link = page -> linksHither [ilinkHither];
		for (ilinkThither = 1; ilinkThither <= page -> nlinksThither; ilinkThither ++)
			if (page -> linksThither [ilinkThither] == link) { result --; break; }
	}
	return result;
}

long ManPages_lookUp (ManPages me, const wchar_t *title) {
	if (! my ground) grind (me);
	return lookUp_sorted (me, title);
}

static long ManPages_lookUp_caseSensitive (ManPages me, const wchar_t *title) {
	if (! my ground) grind (me);
	for (long i = 1; i <= my pages -> size; i ++) {
		ManPage page = my pages -> item [i];
		if (wcsequ (page -> title, title)) return i;
	}
	return 0;
}

const wchar_t **ManPages_getTitles (ManPages me, long *numberOfTitles) {
	if (! my ground) grind (me);
	if (! my titles) {
		my titles = (const wchar_t **) NUMpvector (1, my pages -> size);
		for (long i = 1; i <= my pages -> size; i ++) {
			ManPage page = my pages -> item [i];
			my titles [i] = Melder_wcsdup (page -> title);
		}
	}
	*numberOfTitles = my pages -> size;
	return my titles;
}

static struct stylesInfo {
	const wchar_t *htmlIn, *htmlOut;
} stylesInfo [] = {
{ 0 },
/* INTRO: */ { L"<p>", L"</p>" },
/* ENTRY: */ { L"<h3>", L"</h3>" },
/* NORMAL: */ { L"<p>", L"</p>" },
/* LIST_ITEM: */ { L"<dd>", L"" },
/* TAG: */ { L"<dt>", L"" },
/* DEFINITION: */ { L"<dd>", L"" },
/* CODE: */ { L"<code>", L"<br></code>" },
/* PROTOTYPE: */ { L"<p>", L"</p>" },
/* FORMULA: */ { L"<table width=\"100%\"><tr><td align=middle>", L"</table>" },
/* PICTURE: */ { L"<p>", L"</p>" },
/* SCRIPT: */ { L"<p>", L"</p>" },
/* LIST_ITEM1: */ { L"<dd>&nbsp;&nbsp;&nbsp;", L"" },
/* LIST_ITEM2: */ { L"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"" },
/* LIST_ITEM3: */ { L"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"" },
/* TAG1: */ { L"<dt>&nbsp;&nbsp;&nbsp;", L"" },
/* TAG2: */ { L"<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"" },
/* TAG3: */ { L"<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"" },
/* DEFINITION1: */ { L"<dd>&nbsp;&nbsp;&nbsp;", L"" },
/* DEFINITION2: */ { L"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"" },
/* DEFINITION3: */ { L"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"" },
/* CODE1: */ { L"<code>&nbsp;&nbsp;&nbsp;", L"<br></code>" },
/* CODE2: */ { L"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"<br></code>" },
/* CODE3: */ { L"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"<br></code>" },
/* CODE4: */ { L"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"<br></code>" },
/* CODE5: */ { L"<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", L"<br></code>" }
};

static void writeParagraphsAsHtml (ManPages me, ManPage_Paragraph paragraphs, MelderString *buffer) {
	int inList = FALSE, inItalic = FALSE, inBold = FALSE;
	int inSub = FALSE, inCode = FALSE, inSuper = FALSE, ul = FALSE, inSmall = FALSE;
	int wordItalic = FALSE, wordBold = FALSE, wordCode = FALSE, letterSuper = FALSE;
	ManPage_Paragraph paragraph;
	for (paragraph = paragraphs; paragraph -> type != 0; paragraph ++) {
		const wchar_t *p = paragraph -> text;
		int type = paragraph -> type, inTable;
		int isListItem = type == enumi (ManPage_TYPE, list_item) ||
			(type >= enumi (ManPage_TYPE, list_item1) && type <= enumi (ManPage_TYPE, list_item3));
		int isTag = type == enumi (ManPage_TYPE, tag) ||
			(type >= enumi (ManPage_TYPE, tag1) && type <= enumi (ManPage_TYPE, tag3));
		int isDefinition = type == enumi (ManPage_TYPE, definition) ||
			(type >= enumi (ManPage_TYPE, definition1) && type <= enumi (ManPage_TYPE, definition3));
		/*int isCode = type == enumi (ManPage_TYPE, code) ||
			(type >= enumi (ManPage_TYPE, code1) && type <= enumi (ManPage_TYPE, code5));*/

		/*
		 * We do not recognize pictures yet.
		 */
		if (! p) {
			MelderString_append1 (buffer, L"<p><font size=-2>[sorry, no pictures yet in the web version of this manual]</font></p>\n");
			continue;
		}

		if (isListItem || isTag || isDefinition) {
			if (! inList) {
				ul = isListItem && p [0] == '\\' && p [1] == 'b' && p [2] == 'u';
				MelderString_append1 (buffer, ul ? L"<ul>\n" : L"<dl>\n");
				inList = TRUE;
			}
			if (ul && p [0] == '\\' && p [1] == 'b' && p [2] == 'u' && p [3] == ' ') p += 3;
			MelderString_append2 (buffer, ul ? L"<li>" : stylesInfo [paragraph -> type]. htmlIn, L"\n");
		} else {
			if (inList) {
				MelderString_append1 (buffer, ul ? L"</ul>\n" : L"</dl>\n");
				inList = ul = FALSE;
			}
			MelderString_append2 (buffer, stylesInfo [paragraph -> type]. htmlIn, L"\n");
		}
		inTable = *p == '\t';
		if (inTable) {
			MelderString_append1 (buffer, L"<table border=0 cellpadding=0 cellspacing=0><tr><td width=100 align=middle>");
			p ++;
		}
		/*
		 * Leading spaces should be visible (mainly used in code fragments).
		 */
		while (*p == ' ') {
			MelderString_append1 (buffer, L"&nbsp;");
			p ++;
		}
		while (*p) {
				if (wordItalic && ! isSingleWordCharacter (*p)) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
				if (wordBold && ! isSingleWordCharacter (*p)) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }
				if (wordCode && ! isSingleWordCharacter (*p)) { MelderString_append1 (buffer, L"</code>"); wordCode = FALSE; }
			if (*p == '@') {
				wchar_t link [301], linkText [301], *q = link;
				if (p [1] == '@') {
					p += 2;
					while (*p != '@' && *p != '|' && *p != '\0') *q++ = *p++;
					*q = '\0';   /* Close link. */
					if (*p == '|') {
						p ++;   /* Skip '|'. */
						q = linkText;
						while (*p != '@' && *p != '\0') *q++ = *p++;
						*q = '\0';   /* Close link text. */
					} else {
						wcscpy (linkText, link);
					}
					if (*p) p ++;
				} else {
					p ++;
					while (isSingleWordCharacter (*p) && *p != '\0') *q++ = *p++;
					*q = '\0';   /* Close link. */
					wcscpy (linkText, link);
				}
				/*
				 * We write the link in the following format:
				 *     <a href="link.html">linkText</a>
				 * If "link" (initial lower case) is not in the manual, we write "Link.html" instead.
				 * All spaces and strange symbols in "link" are replaced by underscores,
				 * because it will be a file name (see ManPages_writeAllToHtmlDir).
				 * The file name will have no more than 30 or 60 characters, and no less than 1.
				 */
				MelderString_append1 (buffer, L"<a href=\"");
				if (wcsnequ (link, L"\\FI", 3)) {
					MelderString_append1 (buffer, link + 3);   /* File link. */
				} else {
					q = link;
					if (! ManPages_lookUp_caseSensitive (me, link)) {
						MelderString_appendCharacter (buffer, toupper (link [0]));
						if (*q) q ++;   /* First letter already written. */
					}
					while (*q && q - link < LONGEST_FILE_NAME) {
						if (! isAllowedFileNameCharacter (*q)) MelderString_appendCharacter (buffer, '_');
						else MelderString_appendCharacter (buffer, *q);
						q ++;
					}
					if (link [0] == '\0') MelderString_appendCharacter (buffer, '_');   /* Otherwise Mac problems or Unix invisibility. */
					MelderString_append1 (buffer, L".html");
				}
				MelderString_append3 (buffer, L"\">", linkText, L"</a>");
			} else if (*p == '%') {
				if (inItalic) { MelderString_append1 (buffer, L"</i>"); inItalic = FALSE; p ++; }
				else if (p [1] == '%') { MelderString_append1 (buffer, L"<i>"); inItalic = TRUE; p += 2; }
				else if (p [1] == '#') { MelderString_append1 (buffer, L"<i><b>"); wordItalic = TRUE; wordBold = TRUE; p += 2; }
				else { MelderString_append1 (buffer, L"<i>"); wordItalic = TRUE; p ++; }
			} else if (*p == '_') {
				if (inSub) {
					/*if (wordItalic) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
					if (wordBold) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }*/
					MelderString_append1 (buffer, L"</sub>"); inSub = FALSE; p ++;
				} else if (p [1] == '_') {
					if (wordItalic) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
					if (wordBold) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }
					MelderString_append1 (buffer, L"<sub>"); inSub = TRUE; p += 2;
				} else { MelderString_append1 (buffer, L"_"); p ++; }
			} else if (*p == '#') {
				if (inBold) { MelderString_append1 (buffer, L"</b>"); inBold = FALSE; p ++; }
				else if (p [1] == '#') { MelderString_append1 (buffer, L"<b>"); inBold = TRUE; p += 2; }
				else if (p [1] == '%') { MelderString_append1 (buffer, L"<b><i>"); wordBold = TRUE; wordItalic = TRUE; p += 2; }
				else { MelderString_append1 (buffer, L"<b>"); wordBold = TRUE; p ++; }
			} else if (*p == '$') {
				if (inCode) { MelderString_append1 (buffer, L"</code>"); inCode = FALSE; p ++; }
				else if (p [1] == '$') { MelderString_append1 (buffer, L"<code>"); inCode = TRUE; p += 2; }
				else { MelderString_append1 (buffer, L"<code>"); wordCode = TRUE; p ++; }
			} else if (*p == '^') {
				if (inSuper) {
					/*if (wordItalic) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
					if (wordBold) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }*/
					MelderString_append1 (buffer, L"</sup>"); inSuper = FALSE; p ++;
				} else if (p [1] == '^') {
					/*if (wordItalic) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
					if (wordBold) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }*/
					MelderString_append1 (buffer, L"<sup>"); inSuper = TRUE; p += 2;
				} else {
					/*if (wordItalic) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
					if (wordBold) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }*/
					MelderString_append1 (buffer, L"<sup>"); letterSuper = TRUE; p ++;
				}
			} else if (*p == '}') {
				if (inSmall) { MelderString_append1 (buffer, L"</font>"); inSmall = FALSE; p ++; }
				else { MelderString_append1 (buffer, L"}"); p ++; }
			} else if (*p == '\\' && p [1] == 's' && p [2] == '{') {
				MelderString_append1 (buffer, L"<font size=-1>"); inSmall = TRUE; p += 3;
			} else if (*p == '\t' && inTable) {
				MelderString_append1 (buffer, L"<td width=100 align=middle>"); p ++;
			} else if (*p == '<') {
				MelderString_append1 (buffer, L"&lt;"); p ++;
			} else if (*p == '>') {
				MelderString_append1 (buffer, L"&gt;"); p ++;
			} else if (*p == '&') {
				MelderString_append1 (buffer, L"&amp;"); p ++;
			} else {
				/*if (wordItalic && ! isSingleWordCharacter (*p)) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
				if (wordBold && ! isSingleWordCharacter (*p)) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }
				if (wordCode && ! isSingleWordCharacter (*p)) { MelderString_append1 (buffer, L"</code>"); wordCode = FALSE; }*/
				if (*p == '\\') {
					int kar1 = *++p, kar2 = *++p;
					Longchar_Info info = Longchar_getInfo (kar1, kar2);
					if (info -> unicode < 127) {
						MelderString_appendCharacter (buffer, info -> unicode ? info -> unicode : '?');
					} else {
						MelderString_append3 (buffer, L"&#", Melder_integer (info -> unicode), L";");
					}
					p ++;
				} else {
					if (*p < 127) {
						MelderString_appendCharacter (buffer, *p);
					} else {
						MelderString_append3 (buffer, L"&#", Melder_integer (*p), L";");
					}
					p ++;
				}
				if (letterSuper) {
					if (wordItalic) { MelderString_append1 (buffer, L"</i>"); wordItalic = FALSE; }
					if (wordBold) { MelderString_append1 (buffer, L"</b>"); wordBold = FALSE; }
					MelderString_append1 (buffer, L"</sup>"); letterSuper = FALSE;
				}
			}
		}
		if (inItalic || wordItalic) { MelderString_append1 (buffer, L"</i>"); inItalic = wordItalic = FALSE; }
		if (inBold || wordBold) { MelderString_append1 (buffer, L"</b>"); inBold = wordBold = FALSE; }
		if (inCode || wordCode) { MelderString_append1 (buffer, L"</code>"); inCode = wordCode = FALSE; }
		if (inSub) { MelderString_append1 (buffer, L"</sub>"); inSub = FALSE; }
		if (inSuper || letterSuper) { MelderString_append1 (buffer, L"</sup>"); inSuper = letterSuper = FALSE; }
		if (inTable) { MelderString_append1 (buffer, L"</table>"); inTable = FALSE; }
		MelderString_append2 (buffer, stylesInfo [paragraph -> type]. htmlOut, L"\n");
	}
	if (inList) { MelderString_append1 (buffer, ul ? L"</ul>\n" : L"</dl>\n"); inList = FALSE; }
}

static const wchar_t *month [] =
	{ L"", L"January", L"February", L"March", L"April", L"May", L"June",
	  L"July", L"August", L"September", L"October", L"November", L"December" };

static void writePageAsHtml (ManPages me, long ipage, MelderString *buffer) {
	ManPage page = my pages -> item [ipage];
	ManPage_Paragraph paragraphs = page -> paragraphs;
	MelderString_append3 (buffer, L"<html><head><meta name=\"robots\" content=\"index,follow\">\n"
		L"<title>", page -> title, L"</title></head><body bgcolor=\"#FFFFFF\">\n\n");
	MelderString_append3 (buffer, L"<table border=0 cellpadding=0 cellspacing=0><tr><td bgcolor=\"#CCCC00\">"
		L"<table border=4 cellpadding=9><tr><td align=middle bgcolor=\"#000000\">"
		L"<font face=\"Palatino,Times\" size=6 color=\"#999900\"><b>\n",
		page -> title, L"\n</b></font></table></table>\n");
	writeParagraphsAsHtml (me, paragraphs, buffer);
	if (ManPages_uniqueLinksHither (me, ipage)) {
		long ilink, jlink, lastParagraph = 0;
		while (page -> paragraphs [lastParagraph]. type != 0) lastParagraph ++;
		if (lastParagraph > 0) {
			const wchar_t *text = page -> paragraphs [lastParagraph - 1]. text;
			if (text && text [0] && text [wcslen (text) - 1] != ':')
				MelderString_append1 (buffer, L"<h3>Links to this page</h3>\n");
		}
		MelderString_append1 (buffer, L"<ul>\n");
		for (ilink = 1; ilink <= page -> nlinksHither; ilink ++) {
			long link = page -> linksHither [ilink];
			int alreadyShown = FALSE;
			for (jlink = 1; jlink <= page -> nlinksThither; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = TRUE;
			if (! alreadyShown) {
				const wchar_t *title = ((ManPage) my pages -> item [page -> linksHither [ilink]]) -> title, *p;
				MelderString_append1 (buffer, L"<li><a href=\"");
				for (p = title; *p; p ++) {
					if (p - title >= LONGEST_FILE_NAME) break;
					if (! isAllowedFileNameCharacter (*p)) MelderString_append1 (buffer, L"_");
					else MelderString_appendCharacter (buffer, *p);
				}
				if (title [0] == '\0') MelderString_append1 (buffer, L"_");
				MelderString_append3 (buffer, L".html\">", title, L"</a>\n");
			}
		}
		MelderString_append1 (buffer, L"</ul>\n");
	}
	MelderString_append2 (buffer, L"<hr>\n<address>\n\t<p>&copy; ", page -> author);
	if (page -> date) {
		long date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12) imonth = 0;
		MelderString_append4 (buffer, L", ", month [imonth], L" ", Melder_integer (date % 100));
		MelderString_append2 (buffer, L", ", Melder_integer (date / 10000));
	}
	MelderString_append1 (buffer, L"</p>\n</address>\n</body>\n</html>\n");
}

int ManPages_writeOneToHtmlFile (ManPages me, long ipage, MelderFile file) {
	static MelderString buffer = { 0 };
	MelderString_empty (& buffer);
	writePageAsHtml (me, ipage, & buffer);
	if (! MelderFile_writeText (file, buffer.string)) return 0;
	return 1;
}

int ManPages_writeAllToHtmlDir (ManPages me, const wchar_t *dirPath) {
	structMelderDir dir;
	Melder_pathToDir (dirPath, & dir);
	for (long ipage = 1; ipage <= my pages -> size; ipage ++) {
		ManPage page = my pages -> item [ipage];
		wchar_t fileName [256], *oldText;
		wcscpy (fileName, page -> title);
		for (wchar_t *p = fileName; *p; p ++) if (! isAllowedFileNameCharacter (*p)) *p = '_';
		if (fileName [0] == '\0') wcscpy (fileName, L"_");   /* Otherwise Mac problems and Unix invisibility. */
		fileName [LONGEST_FILE_NAME] = '\0';   /* Longest file name will be 30 characters on Mac. */
		wcscat (fileName, L".html");
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		writePageAsHtml (me, ipage, & buffer);
		structMelderFile file = { 0 };
		MelderDir_getFile (& dir, fileName, & file);
		oldText = MelderFile_readText (& file);
		Melder_clearError ();
		if (oldText == NULL || wcscmp (buffer.string, oldText)) {
			if (! MelderFile_writeText (& file, buffer.string)) return 0;
		}
		Melder_free (oldText);
	}
	return 1;
}

/* End of file ManPages.c */
