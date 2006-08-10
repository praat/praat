/* ManPages.c
 *
 * Copyright (C) 1996-2004 Paul Boersma
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
 */

#include <ctype.h>
#include "ManPages.h"
#include "longchar.h"
#if defined (macintosh) && ! defined (__MACH__)
	#define LONGEST_FILE_NAME  25
#else
	#define LONGEST_FILE_NAME  55
#endif

static int isAllowedFileNameCharacter (int c) {
	return isalnum (c) || c == '_' || c == '-' || c == '+';
}
static int isSingleWordCharacter (int c) {
	return isalnum (c) || c == '_';
}

static long lookUp_unsorted (ManPages me, const char *title);

static void classManPages_destroy (I) { iam (ManPages);
	if (my dynamic && my pages) {
		long ipage;
		for (ipage = 1; ipage <= my pages -> size; ipage ++) {
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
	forget (my pages);
	NUMpvector_free ((void **) my titles, 1);
	inherited (ManPages) destroy (me);
}

static const char *extractLink (const char *text, const char *p, char *link) {
	char *to = link, *max = link + 300;
	if (p == NULL) p = text;
	/*
	 * Search for next '@' that is not in a backslash sequence.
	 */
	for (;;) {
		p = strchr (p, '@');
		if (! p) return NULL;   /* No more '@'. */
		if (p - text <= 0 || p [-1] != '\\' && (p - text <= 1 || p [-2] != '\\')) break;
		p ++;
	}
	Melder_assert (*p == '@');
	if (p [1] == '@') {
		const char *from = p + 2;
		while (*from != '@' && *from != '|' && *from != '\0') {
		if (to >= max) return Melder_errorp ("(ManPages::grind:) Link starting with \"@@\" is too long:\n%s", text);
			*to ++ = *from ++;
		}
		if (*from == '|') { from ++; while (*from != '@' && *from != '\0') from ++; }
		if (*from) p = from + 1; else p = from;   /* Skip '@' but not '\0'. */
	} else {
		const char *from = p + 1;
		while (isSingleWordCharacter (*from)) {
			if (to >= max) return Melder_errorp ("(ManPages::grind:) Link starting with \"@@\" is too long:\n%s", text);
			*to ++ = *from ++;
		}
		p = from;
	}
	*to = '\0';
	return p;
}

static int readOnePage (ManPages me, FILE *f) {
	ManPage page;
	ManPage_Paragraph par;
	char *title = ascgets2 (f);
	if (! title) return Melder_error ("Cannot find page title.");

	/*
	 * Check whether a page with this title is already present.
	 */
	if (lookUp_unsorted (me, title)) {
		return 1;
	}

	page = new (ManPage);
	page -> title = title;

	/*
	 * Add the page early, so that lookUp can find it.
	 */
	if (! Collection_addItem (my pages, page)) return 0;

	page -> author = ascgets2 (f);
	if (! page -> author) return Melder_error ("Cannot find author.");
	page -> date = ascgetu4 (f);
	iferror return Melder_error ("Cannot find date.");
	page -> recordingTime = ascgetr8 (f);
	iferror return Melder_error ("Cannot find recording time.");
	page -> paragraphs = NUMvector (sizeof (struct structManPage_Paragraph), 0, 500);
	if (! page -> paragraphs) return 0;
	for (par = page -> paragraphs;; par ++) {
		char link [501], fileName [256];
		const char *p;
		par -> type = ascgete1 (f, & enum_ManPage_TYPE);
		if (Melder_hasError ()) {
			if (strstr (Melder_buffer1, "end of file")) {
				clearerr (f);
				Melder_clearError ();
				break;
			} else {
				return 0;
			}
		}
		par -> text = ascgets2 (f);
		if (! par -> text) return Melder_error ("Cannot find text.");
		for (p = extractLink (par -> text, NULL, link); p != NULL; p = extractLink (par -> text, p, link)) {
			/*
			 * Now, `link' contains the link text, with spaces and all.
			 * Transform it into a file name.
			 */
			structMelderFile file;
			if (link [0] == '\\' && link [1] == 'F' && link [2] == 'I') {
				/*
				 * A link to a sound file: see if it exists.
				 */
				MelderDir_relativePathToFile (& my rootDirectory, link + 3, & file);
				if (Melder_hasError ()) {
					Melder_clearError ();
					Melder_warning ("Cannot find sound file \"%s\".", link + 3);
				} else if (! MelderFile_exists (& file)) {
					Melder_warning ("Cannot find sound file \"%s\".", MelderFile_messageName (& file));
				}
			} else if (link [0] == '\\' && link [1] == 'S' && link [2] == 'C') {
				/*
				 * A link to a script: see if it exists.
				 */
				char *p = link + 3;
				if (*p == '\"') {
					char *q = fileName;
					p ++;
					while (*p != '\"' && *p != '\0') * q ++ = * p ++;
					*q = '\0';
				} else {
					sscanf (p, "%s", fileName);   /* One word, up to the next space. */
				}
				MelderDir_relativePathToFile (& my rootDirectory, fileName, & file);
				if (Melder_hasError ()) {
					Melder_clearError ();
					Melder_warning ("Cannot find script \"%s\".", fileName);
				} else if (! MelderFile_exists (& file)) {
					Melder_warning ("Cannot find script \"%s\".", MelderFile_messageName (& file));
				}
				my executable = TRUE;
			} else {
				FILE *f;
				char *q;
				/*
				 * A link to another page: follow it.
				 */
				for (q = link; *q; q ++) if (! isAllowedFileNameCharacter (*q)) *q = '_';
				strcpy (fileName, link);
				strcat (fileName, ".man");
				MelderDir_getFile (& my rootDirectory, fileName, & file);
				f = Melder_fopen (& file, "r");
				if (f) {
					if (! readOnePage (me, f)) { fclose (f); return Melder_error ("File \"%s\".", MelderFile_messageName (& file)); }
				} else {
					/*
					 * Second try: with upper case.
					 */
					Melder_clearError ();
					link [0] = toupper (link [0]);
					strcpy (fileName, link);
					strcat (fileName, ".man");
					MelderDir_getFile (& my rootDirectory, fileName, & file);
					if ((f = Melder_fopen (& file, "r")) == NULL) return 0;
					if (! readOnePage (me, f)) { fclose (f); return Melder_error ("File \"%s\".", MelderFile_messageName (& file)); }
				}
				fclose (f);
			}
		}
		iferror return 0;
	}
	++ par;   /* Leave room for zero. */
	Melder_realloc (page -> paragraphs, sizeof (struct structManPage_Paragraph) * (par - page -> paragraphs));
	return 1;
}
static int readOnePageFromFile (ManPages me, MelderFile file) {
	FILE *f = Melder_fopen (file, "r");
	if (! f) return 0;
	if (! readOnePage (me, f)) {
		Melder_error ("File \"%s\".", MelderFile_messageName (file));
		Melder_fclose (file, f);
		return 0;
	}
	return Melder_fclose (file, f);
}
static int classManPages_readAscii (I, FILE *f) { iam (ManPages);
	my dynamic = TRUE;
	my pages = Ordered_create ();
	MelderFile_getParentDir (& Data_fileBeingRead, & my rootDirectory);
	return readOnePage (me, f);
}

class_methods (ManPages, Data)
	class_method_local (ManPages, destroy)
	class_method_local (ManPages, readAscii)
class_methods_end

ManPages ManPages_create (void) {
	ManPages me = new (ManPages);
	my pages = Ordered_create ();
	return me;
}

int ManPages_addPage (ManPages me, const char *title, const char *author, long date,
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
	const char *p = my title, *q = thy title;
	for (;;) {
		int plower = tolower (*p), qlower = tolower (*q);
		if (plower < qlower) return -1;
		if (plower > qlower) return 1;
		if (plower == '\0') return strcmp (my title, thy title);
		p ++, q ++;
	}
	return 0;   /* Should not occur. */
}

static long lookUp_unsorted (ManPages me, const char *title) {
	long i;

	/*
	 * First try to match an unaltered 'title' with the titles of the man pages.
	 */
	for (i = 1; i <= my pages -> size; i ++) {
		ManPage page = my pages -> item [i];
		if (strequ (page -> title, title)) return i;
	}

	/*
	 * If that fails, try to find the upper-case variant.
	 */
	if (islower (title [0])) {
		char upperTitle [300];
		strcpy (upperTitle, title);
		upperTitle [0] = toupper (upperTitle [0]);
		for (i = 1; i <= my pages -> size; i ++) {
			ManPage page = my pages -> item [i];
			if (strequ (page -> title, upperTitle)) return i;
		}
	}
	return 0;
}

static long lookUp_sorted (ManPages me, const char *title) {
	static ManPage dummy;
	ManPage *page;
	if (! dummy) dummy = new (ManPage);
	dummy -> title = title;
	page = bsearch (& dummy, & my pages -> item [1], my pages -> size, sizeof (ManPage), pageCompare);
	if (page) return (page - (ManPage *) & my pages -> item [1]) + 1;
	if (islower (title [0]) || isupper (title [0])) {
		char caseSwitchedTitle [300];
		strcpy (caseSwitchedTitle, title);
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
			const char *text = page -> paragraphs [ipar]. text, *p;
			char link [301];
			if (text) for (p = extractLink (text, NULL, link); p != NULL; p = extractLink (text, p, link)) {
				if (link [0] == '\\' && (link [1] == 'F' && link [2] == 'I' || link [1] == 'S' && link [2] == 'C'))
					continue;   /* Ignore "FILE" links. */
				if ((jpage = lookUp_sorted (me, link)) != 0) {
					page -> nlinksThither ++;
					((ManPage) my pages -> item [jpage]) -> nlinksHither ++;
					grandNlinks ++;
				} else {
					Melder_info ("Page \"%s\" contains a dangling link to \"%s\".",
						page -> title, link);
					ndangle ++;
				}
			}
			iferror Melder_flushError (NULL);
		}
	}
	if (ndangle)
		Melder_warning ("(ManPages::grind:) %ld dangling links encountered. See console window.", ndangle);

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
			const char *text = page -> paragraphs [ipar]. text, *p;
			char link [301];
			if (text) for (p = extractLink (text, NULL, link); p != NULL; p = extractLink (text, p, link)) {
				if (link [0] == '\\' && (link [1] == 'F' && link [2] == 'I' || link [1] == 'S' && link [2] == 'C'))
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

long ManPages_lookUp (ManPages me, const char *title) {
	if (! my ground) grind (me);
	return lookUp_sorted (me, title);
}

static long ManPages_lookUp_caseSensitive (ManPages me, const char *title) {
	long i;
	if (! my ground) grind (me);
	for (i = 1; i <= my pages -> size; i ++) {
		ManPage page = my pages -> item [i];
		if (strequ (page -> title, title)) return i;
	}
	return 0;
}

const char **ManPages_getTitles (ManPages me, long *numberOfTitles) {
	if (! my ground) grind (me);
	if (! my titles) {
		int i;
		my titles = (const char **) NUMpvector (1, my pages -> size);
		for (i = 1; i <= my pages -> size; i ++) {
			ManPage page = my pages -> item [i];
			my titles [i] = page -> title;
		}
	}
	*numberOfTitles = my pages -> size;
	return my titles;
}

static CACHE *theCache;

static struct stylesInfo {
	const char *htmlIn, *htmlOut;
} stylesInfo [] = {
{ 0 },
/* INTRO: */ { "<p>", "</p>" },
/* ENTRY: */ { "<h3>", "</h3>" },
/* NORMAL: */ { "<p>", "</p>" },
/* LIST_ITEM: */ { "<dd>", "" },
/* TAG: */ { "<dt>", "" },
/* DEFINITION: */ { "<dd>", "" },
/* CODE: */ { "<code>", "<br></code>" },
/* PROTOTYPE: */ { "<p>", "</p>" },
/* FORMULA: */ { "<table width=\"100%\"><tr><td align=middle>", "</table>" },
/* PICTURE: */ { "<p>", "</p>" },
/* SCRIPT: */ { "<p>", "</p>" },
/* LIST_ITEM1: */ { "<dd>&nbsp;&nbsp;&nbsp;", "" },
/* LIST_ITEM2: */ { "<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "" },
/* LIST_ITEM3: */ { "<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "" },
/* TAG1: */ { "<dt>&nbsp;&nbsp;&nbsp;", "" },
/* TAG2: */ { "<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "" },
/* TAG3: */ { "<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "" },
/* DEFINITION1: */ { "<dd>&nbsp;&nbsp;&nbsp;", "" },
/* DEFINITION2: */ { "<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "" },
/* DEFINITION3: */ { "<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "" },
/* CODE1: */ { "<code>&nbsp;&nbsp;&nbsp;", "<br></code>" },
/* CODE2: */ { "<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "<br></code>" },
/* CODE3: */ { "<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "<br></code>" },
/* CODE4: */ { "<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "<br></code>" },
/* CODE5: */ { "<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", "<br></code>" }
};

static void writeParagraphsAsHtml (ManPages me, ManPage_Paragraph paragraphs) {
	int inList = FALSE, inItalic = FALSE, inBold = FALSE;
	int inSub = FALSE, inCode = FALSE, inSuper = FALSE, ul = FALSE, inSmall = FALSE;
	int wordItalic = FALSE, wordBold = FALSE, wordCode = FALSE, letterSuper = FALSE;
	ManPage_Paragraph paragraph;
	for (paragraph = paragraphs; paragraph -> type != 0; paragraph ++) {
		const char *p = paragraph -> text;
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
		if (! p) { memprint1 (theCache, "<p><font size=-2>[sorry, no pictures yet in the web version of this manual]</font></p>\n"); continue; }

		if (isListItem || isTag || isDefinition) {
			if (! inList) {
				ul = isListItem && p [0] == '\\' && p [1] == 'b' && p [2] == 'u';
				memprint1 (theCache, ul ? "<ul>\n" : "<dl>\n");
				inList = TRUE;
			}
			if (ul && p [0] == '\\' && p [1] == 'b' && p [2] == 'u' && p [3] == ' ') p += 3;
			memprint2 (theCache, ul ? "<li>" : stylesInfo [paragraph -> type]. htmlIn, "\n");
		} else {
			if (inList) { memprint1 (theCache, ul ? "</ul>\n" : "</dl>\n"); inList = ul = FALSE; }
			memprint2 (theCache, stylesInfo [paragraph -> type]. htmlIn, "\n");
		}
		inTable = *p == '\t';
		if (inTable) {
			memprint1 (theCache, "<table border=0 cellpadding=0 cellspacing=0><tr><td width=100 align=middle>"); p ++;
		}
		/*
		 * Leading spaces should be visible (mainly used in code fragments).
		 */
		while (*p == ' ') { memprint1 (theCache, "&nbsp;"); p ++; }
		while (*p) {
				if (wordItalic && ! isSingleWordCharacter (*p)) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
				if (wordBold && ! isSingleWordCharacter (*p)) { memprint1 (theCache, "</b>"); wordBold = FALSE; }
				if (wordCode && ! isSingleWordCharacter (*p)) { memprint1 (theCache, "</code>"); wordCode = FALSE; }
			if (*p == '@') {
				char link [301], linkText [301], *q = link;
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
						strcpy (linkText, link);
					}
					if (*p) p ++;
				} else {
					p ++;
					while (isSingleWordCharacter (*p) && *p != '\0') *q++ = *p++;
					*q = '\0';   /* Close link. */
					strcpy (linkText, link);
				}
				/*
				 * We write the link in the following format:
				 *     <a href="link.html">linkText</a>
				 * If "link" (initial lower case) is not in the manual, we write "Link.html" instead.
				 * All spaces and strange symbols in "link" are replaced by underscores,
				 * because it will be a file name (see ManPages_writeAllToHtmlDir).
				 * The file name will have no more than 30 or 60 characters, and no less than 1.
				 */
				memprint1 (theCache, "<a href=\"");
				if (strnequ (link, "\\FI", 3)) {
					memprint1 (theCache, link + 3);   /* File link. */
				} else {
					q = link;
					if (! ManPages_lookUp_caseSensitive (me, link)) {
						char upperCase [2] = "";
						upperCase [0] = toupper (link [0]);
						memwrite (upperCase, 1, 1, theCache);
						if (*q) q ++;   /* First letter already written. */
					}
					while (*q && q - link < LONGEST_FILE_NAME) {
						if (! isAllowedFileNameCharacter (*q)) memprint1 (theCache, "_");
						else memwrite (q, 1, 1, theCache);
						q ++;
					}
					if (link [0] == '\0') memprint1 (theCache, "_");   /* Otherwise Mac problems or Unix invisibility. */
					memprint1 (theCache, ".html");
				}
				memprint3 (theCache, "\">", linkText, "</a>");
			} else if (*p == '%') {
				if (inItalic) { memprint1 (theCache, "</i>"); inItalic = FALSE; p ++; }
				else if (p [1] == '%') { memprint1 (theCache, "<i>"); inItalic = TRUE; p += 2; }
				else if (p [1] == '#') { memprint1 (theCache, "<i><b>"); wordItalic = TRUE; wordBold = TRUE; p += 2; }
				else { memprint1 (theCache, "<i>"); wordItalic = TRUE; p ++; }
			} else if (*p == '_') {
				if (inSub) {
					/*if (wordItalic) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
					if (wordBold) { memprint1 (theCache, "</b>"); wordBold = FALSE; }*/
					memprint1 (theCache, "</sub>"); inSub = FALSE; p ++;
				} else if (p [1] == '_') {
					if (wordItalic) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
					if (wordBold) { memprint1 (theCache, "</b>"); wordBold = FALSE; }
					memprint1 (theCache, "<sub>"); inSub = TRUE; p += 2;
				} else { memprint1 (theCache, "_"); p ++; }
			} else if (*p == '#') {
				if (inBold) { memprint1 (theCache, "</b>"); inBold = FALSE; p ++; }
				else if (p [1] == '#') { memprint1 (theCache, "<b>"); inBold = TRUE; p += 2; }
				else if (p [1] == '%') { memprint1 (theCache, "<b><i>"); wordBold = TRUE; wordItalic = TRUE; p += 2; }
				else { memprint1 (theCache, "<b>"); wordBold = TRUE; p ++; }
			} else if (*p == '$') {
				if (inCode) { memprint1 (theCache, "</code>"); inCode = FALSE; p ++; }
				else if (p [1] == '$') { memprint1 (theCache, "<code>"); inCode = TRUE; p += 2; }
				else { memprint1 (theCache, "<code>"); wordCode = TRUE; p ++; }
			} else if (*p == '^') {
				if (inSuper) {
					/*if (wordItalic) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
					if (wordBold) { memprint1 (theCache, "</b>"); wordBold = FALSE; }*/
					memprint1 (theCache, "</sup>"); inSuper = FALSE; p ++;
				} else if (p [1] == '^') {
					/*if (wordItalic) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
					if (wordBold) { memprint1 (theCache, "</b>"); wordBold = FALSE; }*/
					memprint1 (theCache, "<sup>"); inSuper = TRUE; p += 2;
				} else {
					/*if (wordItalic) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
					if (wordBold) { memprint1 (theCache, "</b>"); wordBold = FALSE; }*/
					memprint1 (theCache, "<sup>"); letterSuper = TRUE; p ++;
				}
			} else if (*p == '}') {
				if (inSmall) { memprint1 (theCache, "</font>"); inSmall = FALSE; p ++; }
				else { memprint1 (theCache, "}"); p ++; }
			} else if (*p == '\\' && p [1] == 's' && p [2] == '{') {
				memprint1 (theCache, "<font size=-1>"); inSmall = TRUE; p += 3;
			} else if (*p == '\t' && inTable) {
				memprint1 (theCache, "<td width=100 align=middle>"); p ++;
			} else if (*p == '<') {
				memprint1 (theCache, "&lt;"); p ++;
			} else if (*p == '>') {
				memprint1 (theCache, "&gt;"); p ++;
			} else if (*p == '&') {
				memprint1 (theCache, "&amp;"); p ++;
			} else {
				/*if (wordItalic && ! isSingleWordCharacter (*p)) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
				if (wordBold && ! isSingleWordCharacter (*p)) { memprint1 (theCache, "</b>"); wordBold = FALSE; }
				if (wordCode && ! isSingleWordCharacter (*p)) { memprint1 (theCache, "</code>"); wordCode = FALSE; }*/
				if (*p == '\\') {
					int kar1 = *++p, kar2 = *++p;
					Longchar_Info info = Longchar_getInfo (kar1, kar2);
					if (info -> alphabet == Longchar_ROMAN) {
						if (info -> winEncoding < 127) {
							char letter [2] = "";
							letter [0] = info -> winEncoding ? info -> winEncoding : '?';
							memwrite (letter, 1, 1, theCache);
						} else {
							memprint3 (theCache, "&#", Melder_integer (info -> winEncoding), ";");
						}
					} else if (info -> alphabet == Longchar_PHONETIC || info -> alphabet == Longchar_DINGBATS) {
						memprint3 (theCache, "<sub><img src=\"symbols/", info -> ps. name + 1, ".xbm\"></sub>");
					} else {
						const char *greek = NULL, *symbol = NULL, *replace = NULL;
						switch (kar1) {
						case 'A': switch (kar2) {
							case 'l': replace = "A"; break;
							default: break; } break;
						case 'B': switch (kar2) {
							case 'e': replace = "B"; break;
							default: break; } break;
						case 'C': switch (kar2) {
							case 'i': symbol = "Chi"; break;
							default: break; } break;
						case 'D': switch (kar2) {
							case 'e': symbol = "Delta"; break;
							default: break; } break;
						case 'E': switch (kar2) {
							case 'p': replace = "E"; break;
							case 't': replace = "H"; break;
							default: break; } break;
						case 'F': switch (kar2) {
							case 'i': symbol = "Phi"; break;
							default: break; } break;
						case 'G': switch (kar2) {
							case 'a': symbol = "Gamma"; break;
							default: break; } break;
						case 'I': switch (kar2) {
							case 'o': replace = "I"; break;
							default: break; } break;
						case 'K': switch (kar2) {
							case 'a': replace = "K"; break;
							default: break; } break;
						case 'L': switch (kar2) {
							case 'a': symbol = "Lambda"; break;
							default: break; } break;
						case 'M': switch (kar2) {
							case 'u': replace = "M"; break;
							default: break; } break;
						case 'N': switch (kar2) {
							case 'u': replace = "N"; break;
							default: break; } break;
						case 'O': switch (kar2) {
							case 'm': symbol = "Omega"; break;
							case 'n': replace = "O"; break;
							case '|': replace = "&Oslash;"; break;
							default: break; } break;
						case 'P': switch (kar2) {
							case 'i': symbol = "Pi"; break;
							case 's': symbol = "Psi"; break;
							default: break; } break;
						case 'R': switch (kar2) {
							case 'o': replace = "P"; break;
							default: break; } break;
						case 'S': switch (kar2) {
							case 'i': symbol = "Sigma"; break;
							default: break; } break;
						case 'T': switch (kar2) {
							case 'M': replace = "<sup>TM</sup>"; break;
							case 'a': replace = "T"; break;
							case 'e': symbol = "Theta"; break;
							case 't': symbol = "perp"; break;
							default: break; } break;
						case 'U': switch (kar2) {
							case 'p': symbol = "Upsilon"; break;
							default: break; } break;
						case 'V': switch (kar2) {
							case 'r': symbol = "sqrt"; break;
							default: break; } break;
						case 'X': switch (kar2) {
							case 'i': symbol = "Xi"; break;
							default: break; } break;
						case 'Z': switch (kar2) {
							case 'e': replace = "Zeta"; break;
							default: break; } break;
						case 'a': switch (kar2) {
							case 'l': symbol = "alpha"; break;
							default: break; } break;
						case 'b': switch (kar2) {
							case 'c': replace = "B"; break;
							case 'e': symbol = "beta2"; break;
							case 'u': replace = "<b>o</b>"; break;
							default: break; } break;
						case 'c': switch (kar2) {
							case 'i': symbol = "chi"; break;
							default: break; } break;
						case 'd': switch (kar2) {
							case 'd': symbol = "delta"; break;   /* Not accurate. */
							case 'e': symbol = "delta"; break;
							case 'i': symbol = "diamond"; break;
							default: break; } break;
						case 'e': switch (kar2) {
							case 'p': symbol = "epsilon"; break;
							case 'q': replace = "<=>"; break;
							case 't': symbol = "eta2"; break;
							case '=': symbol = "epsilon"; break;
							default: break; } break;
						case 'f': switch (kar2) {
							case '2': greek = "phi2"; break;
							case 'd': replace = "<i>f</i>"; break;
							case 'i': greek = "phi"; break;
							default: break; } break;
						case 'g': switch (kar2) {
							case 'a': symbol = "gamma"; break;
							default: break; } break;
						case 'h': switch (kar2) {
							case 'c': replace = "H"; break;
							default: break; } break;
						case 'i': switch (kar2) {
							case 'n': symbol = "integ2"; break;
							case 'o': symbol = "iota"; break;
							default: break; } break;
						case 'j': switch (kar2) {
							default: break; } break;
						case 'k': switch (kar2) {
							case 'a': symbol = "kappa"; break;
							default: break; } break;
						case 'l': switch (kar2) {
							case 'a': symbol = "lambda"; break;
							case 'c': replace = "L"; break;
							default: break; } break;
						case 'm': switch (kar2) {
							case 'u': replace = "&#181;"; break;
							default: break; } break;
						case 'n': switch (kar2) {
							case 'u': symbol = "nu"; break;
							default: break; } break;
						case 'o': switch (kar2) {
							case '2': greek = "omega2"; break;
							case 'c': symbol = "propto"; break;
							case 'm': symbol = "omega"; break;
							case 'n': symbol = "omicron"; break;
							case 'o': symbol = "infty"; break;
							case 'x': symbol = "otimes"; break;
							default: break; } break;
						case 'p': switch (kar2) {
							case 'i': symbol = "pi"; break;
							case 's': symbol = "psi"; break;
							default: break; } break;
						case 'r': switch (kar2) {
							case 'o': symbol = "rho2"; break;
							default: break; } break;
						case 's': switch (kar2) {
							case '2': greek = "sigma2"; break;
							case 'i': symbol = "sigma"; break;
							case 'u': symbol = "sum"; break;
							default: break; } break;
						case 't': switch (kar2) {
							case '2': symbol = "theta"; break;
							case 'a': symbol = "tau"; break;
							case 'e': symbol = "theta"; break;
							case 'm': replace = "<sup>TM</sup>"; break;
							default: break; } break;
						case 'u': switch (kar2) {
							case 'p': symbol = "upsilon"; break;
							default: break; } break;
						case 'x': switch (kar2) {
							case 'i': symbol = "xi"; break;
							case 'x': replace = "&#215;"; break;
							default: break; } break;
						case 'z': switch (kar2) {
							case 'e': symbol = "zeta2"; break;
							default: break; } break;
						case '-': switch (kar2) {
							case '-': replace = "-"; break;
							case '>': replace = "-&gt;"; break;
							default: break; } break;
						case '+': switch (kar2) {
							case '-': replace = "&#177;"; break;
							default: break; } break;
						case '<': switch (kar2) {
							case '<': symbol = "ll"; break;
							case '=': replace = "&lt;="; break;
							case '>': replace = "&lt;-&gt;"; break;
							case '-': replace = "&lt;-"; break;
							case '_': symbol = "leq"; break;
							default: break; } break;
						case '=': switch (kar2) {
							case '>': replace = "=&gt;"; break;
							case '/': symbol = "neq"; break;
							case '3': symbol = "ident"; break;
							case '~': symbol = "appeq"; break;   /* Not accurate. */
							default: break; } break;
						case '>': switch (kar2) {
							case '>': symbol = "gg"; break;
							case '_': symbol = "geq"; break;
							default: break; } break;
						case '\'': switch (kar2) {
							case 'p': replace = "\'"; break;
							default: break; } break;
						case '\"': switch (kar2) {
							case 'p': replace = "\""; break;
							default: break; } break;
						case '/': switch (kar2) {
							case 'd': replace = "/"; break;
							default: break; } break;
						case ':': switch (kar2) {
							case '-': symbol = "div"; break;
							default: break; } break;
						case '~': switch (kar2) {
							case '~': symbol = "approx"; break;
							default: break; } break;
						default: break;
						}
						if (symbol) {
							if (symbol [strlen (symbol) - 1] == '2') {
								memprint3 (theCache, "<img align=middle src=\"symbols/", symbol, ".gif\">");
							} else {
								memprint3 (theCache, "<img src=\"symbols/", symbol, ".gif\">");
							}
						} else if (greek) {
							memprint3 (theCache, "<img src=\"greekGIF/", greek, ".GIF\">");
						} else if (replace) {
							memprint1 (theCache, replace);
						} else {
							char backslash [4] = "\\";
							backslash [1] = kar1;
							backslash [2] = kar2;
							memprint1 (theCache, backslash);
						}
					}
					p ++;
				} else {
					memwrite (p, 1, 1, theCache);
					p ++;
				}
				if (letterSuper) {
					if (wordItalic) { memprint1 (theCache, "</i>"); wordItalic = FALSE; }
					if (wordBold) { memprint1 (theCache, "</b>"); wordBold = FALSE; }
					memprint1 (theCache, "</sup>"); letterSuper = FALSE;
				}
			}
		}
		if (inItalic || wordItalic) { memprint1 (theCache, "</i>"); inItalic = wordItalic = FALSE; }
		if (inBold || wordBold) { memprint1 (theCache, "</b>"); inBold = wordBold = FALSE; }
		if (inCode || wordCode) { memprint1 (theCache, "</code>"); inCode = wordCode = FALSE; }
		if (inSub) { memprint1 (theCache, "</sub>"); inSub = FALSE; }
		if (inSuper || letterSuper) { memprint1 (theCache, "</sup>"); inSuper = letterSuper = FALSE; }
		if (inTable) { memprint1 (theCache, "</table>"); inTable = FALSE; }
		memprint2 (theCache, stylesInfo [paragraph -> type]. htmlOut, "\n");
	}
	if (inList) { memprint1 (theCache, ul ? "</ul>\n" : "</dl>\n"); inList = FALSE; }
}

static const char *month [] =
	{ "", "January", "February", "March", "April", "May", "June",
	  "July", "August", "September", "October", "November", "December" };

static void writePageAsHtml (ManPages me, long ipage) {
	ManPage page = my pages -> item [ipage];
	ManPage_Paragraph paragraphs = page -> paragraphs;
	memprint3 (theCache, "<html><head><meta name=\"robots\" content=\"index,follow\">\n"
		"<title>", page -> title, "</title></head><body bgcolor=\"#FFFFFF\">\n\n");
	memprint3 (theCache, "<table border=0 cellpadding=0 cellspacing=0><tr><td bgcolor=\"#CCCC00\">"
		"<table border=4 cellpadding=9><tr><td align=middle bgcolor=\"#000000\">"
		"<font face=\"Palatino,Times\" size=6 color=\"#999900\"><b>\n",
		page -> title, "\n</b></font></table></table>\n");
	writeParagraphsAsHtml (me, paragraphs);
	if (ManPages_uniqueLinksHither (me, ipage)) {
		long ilink, jlink, lastParagraph = 0;
		while (page -> paragraphs [lastParagraph]. type != 0) lastParagraph ++;
		if (lastParagraph > 0) {
			const char *text = page -> paragraphs [lastParagraph - 1]. text;
			if (text && text [0] && text [strlen (text) - 1] != ':')
				memprint1 (theCache, "<h3>Links to this page</h3>\n");
		}
		memprint1 (theCache, "<ul>\n");
		for (ilink = 1; ilink <= page -> nlinksHither; ilink ++) {
			long link = page -> linksHither [ilink];
			int alreadyShown = FALSE;
			for (jlink = 1; jlink <= page -> nlinksThither; jlink ++)
				if (page -> linksThither [jlink] == link)
					alreadyShown = TRUE;
			if (! alreadyShown) {
				const char *title = ((ManPage) my pages -> item [page -> linksHither [ilink]]) -> title, *p;
				memprint1 (theCache, "<li><a href=\"");
				for (p = title; *p; p ++) {
					if (p - title >= LONGEST_FILE_NAME) break;
					if (! isAllowedFileNameCharacter (*p)) memprint1 (theCache, "_");
					else memwrite (p, 1, 1, theCache);
				}
				if (title [0] == '\0') memprint1 (theCache, "_");
				memprint3 (theCache, ".html\">", title, "</a>\n");
			}
		}
		memprint1 (theCache, "</ul>\n");
	}
	memprint2 (theCache, "<hr>\n<address>\n\t<p>&copy; ", page -> author);
	if (page -> date) {
		long date = page -> date;
		int imonth = date % 10000 / 100;
		if (imonth < 0 || imonth > 12) imonth = 0;
		memprint4 (theCache, ", ", month [imonth], " ", Melder_integer (date % 100));
		memprint2 (theCache, ", ", Melder_integer (date / 10000));
	}
	memprint1 (theCache, "</p>\n</address>\n</body>\n</html>\n");
}

int ManPages_writeOneToHtmlFile (ManPages me, long ipage, MelderFile file) {
	if (! theCache) theCache = memopen (100000);
	if (! theCache) return 0;
	memrewind (theCache);
	writePageAsHtml (me, ipage);
	memwrite ("", 1, 1, theCache);   /* Closing null byte. */
	if (! MelderFile_writeText (file, (char *) theCache -> base)) return 0;
	return 1;
}

int ManPages_writeAllToHtmlDir (ManPages me, const char *dirPath) {
	structMelderDir dir;
	long ipage;
	if (! theCache) theCache = memopen (100000);
	if (! theCache) return 0;
	Melder_pathToDir (dirPath, & dir);
	for (ipage = 1; ipage <= my pages -> size; ipage ++) {
		ManPage page = my pages -> item [ipage];
		char fileName [256], *p, *oldText;
		structMelderFile file;
		strcpy (fileName, page -> title);
		for (p = fileName; *p; p ++) if (! isAllowedFileNameCharacter (*p)) *p = '_';
		if (fileName [0] == '\0') strcpy (fileName, "_");   /* Otherwise Mac problems and Unix invisibility. */
		fileName [LONGEST_FILE_NAME] = '\0';   /* Longest file name will be 30 characters on Mac. */
		strcat (fileName, ".html");
		memrewind (theCache);
		writePageAsHtml (me, ipage);
		memwrite ("", 1, 1, theCache);   /* Closing null byte. */
		MelderDir_getFile (& dir, fileName, & file);
		oldText = MelderFile_readText (& file);
		Melder_clearError ();
		if (oldText == NULL || strncmp ((char *) theCache -> base, oldText, memtell (theCache))) {
			if (! MelderFile_writeText (& file, (char *) theCache -> base)) return 0;
		}
		Melder_free (oldText);
	}
	return 1;
}

/* End of file ManPages.c */
