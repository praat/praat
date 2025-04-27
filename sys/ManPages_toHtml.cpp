/* ManPages_toHtml.cpp
 *
 * Copyright (C) 1996-2025 Paul Boersma
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
#include "praat.h"

#define LONGEST_FILE_NAME  55
#define PNG_RESOLUTION  300 /* dpi */

static const struct stylesInfo {
	conststring32 htmlIn, htmlOut;
} stylesInfo [] = {
{ nullptr, nullptr },
/* INTRO: */ { U"<p>", U"</p>" },
/* ENTRY: */ { U"<h2>", U"</h2>" },
/* NORMAL: */ { U"<p>", U"</p>" },
/* LIST_ITEM: */ { U"<dd style=\"position:relative;padding-left:1em;text-indent:-2em\">", U"</dd>" },
/* TERM: */ { U"<dt>", U"" },
/* DEFINITION: */ { U"<dd>", U"</dd>" },
/* CODE: */ { U"<code>   ", U"<br></code>" },
/* PROTOTYPE: */ { U"<p>", U"</p>" },
/* EQUATION: */ { U"<table width=\"100%\" style=\"white-space:pre-wrap\"><tr><td align=middle>", U"</table>" },
/* PICTURE: */ { U"<p>", U"</p>" },
/* SCRIPT: */ { U"<p>", U"</p>" },
/* LIST_ITEM1: */ { U"<dd>&nbsp;&nbsp;&nbsp;", U"</dd>" },
/* LIST_ITEM2: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"</dd>" },
/* LIST_ITEM3: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"</dd>" },
/* TERM1: */ { U"<dt>&nbsp;&nbsp;&nbsp;", U"" },
/* TERM2: */ { U"<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* TERM3: */ { U"<dt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"" },
/* DEFINITION1: */ { U"<dd>&nbsp;&nbsp;&nbsp;", U"</dd>" },
/* DEFINITION2: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"</dd>" },
/* DEFINITION3: */ { U"<dd>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", U"</dd>" },
/* CODE1: */ { U"<code>      ", U"<br></code>" },
/* CODE2: */ { U"<code>         ", U"<br></code>" },
/* CODE3: */ { U"<code>            ", U"<br></code>" },
/* CODE4: */ { U"<code>               ", U"<br></code>" },
/* CODE5: */ { U"<code>                  ", U"<br></code>" },
/* CAPTION: */ { U"<p style=\"position:relative;padding-left:4em;text-indent:-2em;font-size:86%\">", U"</font></p>" },
/* QUOTE: */ { U"<p style=\"position:relative;padding-left:4em;font-size:86%\">", U"</font></p>" },
/* QUOTE1: */ { U"<p style=\"position:relative;padding-left:8em;font-size:86%\">", U"</font></p>" },
/* QUOTE2: */ { U"<p style=\"position:relative;padding-left:12em;font-size:86%\">", U"</font></p>" },
/* QUOTE3: */ { U"<p style=\"position:relative;padding-left:16em;font-size:86%\">", U"</font></p>" },
/* SUBHEADER: */ { U"<h3>", U"</h3>" },
};

static void writeLinkAsHtml (ManPages me, mutablestring32 link, conststring32 linkText, MelderString *buffer, conststring32 pageTitle) {
	/*
		We write the link in the following format:
			<a href="link.html">linkText</a>
	*/
	MelderString_append (buffer, U"<a href=\"");
	if (str32nequ (link, U"\\FI", 3)) {
		MelderString_append (buffer, link + 3);   // file link
	} else {
		/*
			If "link" (initial lower case) is not in the manual, we write "Link.html" instead.
			All spaces and strange symbols in "link" are replaced by underscores,
			because it will be a file name (see ManPages_writeAllToHtmlDir).
			The file name will have no more than 30 or 60 characters, and no less than 1.

			The first character of the link text can have the wrong case.
		*/
		integer linkPageNumber = ManPages_lookUp (me, link);
		if (linkPageNumber == 0)
			Melder_throw (U"No such manual page: ", link, U" (from page “", pageTitle, U"”).");
		link [0] = my pages.at [linkPageNumber] -> title [0];
		char32 *q = link;
		if (! ManPages_lookUp_caseSensitive (me, link)) {
			MelderString_appendCharacter (buffer, Melder_toUpperCase (link [0]));
			if (*q)
				q ++;   // first letter already written
		}
		while (*q && q - link < LONGEST_FILE_NAME) {
			if (isAllowedFileNameCharacter (*q))
				MelderString_appendCharacter (buffer, *q);
			else if (*q == U'#')
				MelderString_append (buffer, U"-H");
			else if (*q == U'$')
				MelderString_append (buffer, U"-S");
			else if (*q == U'@')
				MelderString_append (buffer, U"-C");
			else
				MelderString_appendCharacter (buffer, U'_');
			q ++;
		}
		if (link [0] == U'\0')
			MelderString_appendCharacter (buffer, U'_');   // otherwise Mac problems or Unix invisibility
		MelderString_append (buffer, U".html");
	}
	MelderString_append (buffer, U"\">", linkText, U"</a>");
}

static void writeParagraphsAsHtml (ManPages me, Interpreter optionalInterpreterReference, MelderFile file, ManPage page, MelderString *buffer) {
	static structPraatApplication praatApplication;
	static structPraatObjects praatObjects;
	static structPraatPicture praatPicture;

	ManPage_runAllChunksToCache (page, optionalInterpreterReference, kGraphics_font::TIMES, 12.0,
			& praatApplication, & praatObjects, & praatPicture, & my rootDirectory);
	integer chunkNumber = 0;
	autostring32 theErrorThatOccurred;

	integer numberOfPictures = 0;
	bool inList = false, inItalic = false, inBold = false;
	bool inSub = false, inCode = false, inSuper = false, ul = false, inSmall = false;
	bool wordItalic = false, wordBold = false, wordCode = false, letterSub = false, letterSuper = false;
	for (integer ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
		const bool verbatimAware = true;   //page -> verbatimAware;
			// we ignore saving old-fashioned man pages to HTML (was undocumented anyway) 2023-07-19
		const structManPage_Paragraph *paragraph = & page -> paragraphs [ipar];
		const bool paragraphIsVerbatim = ( paragraph -> couldVerbatim () );
		const char32 *p = & paragraph -> text [0];
		const bool isListItem = paragraph -> type == kManPage_type::LIST_ITEM ||
			(paragraph -> type >= kManPage_type::LIST_ITEM1 && paragraph -> type <= kManPage_type::LIST_ITEM3);
		const bool isTag = paragraph -> type == kManPage_type::TERM ||
			(paragraph -> type >= kManPage_type::TERM1 && paragraph -> type <= kManPage_type::TERM3);
		const bool isDefinition = paragraph -> type == kManPage_type::DEFINITION ||
			(paragraph -> type >= kManPage_type::DEFINITION1 && paragraph -> type <= kManPage_type::DEFINITION3);
		/*const bool isCode = paragraph -> type == kManPage_type::CODE ||
			(paragraph -> type >= kManPage_type::CODE1 && paragraph -> type <= kManPage_type::CODE5);*/

		if (paragraph -> type == kManPage_type::PICTURE) {
			numberOfPictures ++;
			structMelderFile pngFile { };
			MelderFile_copy (file, & pngFile);
			pngFile. path [Melder_length (pngFile. path) - 5] = U'\0';   // delete extension ".html"
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
			MelderString_append (buffer,
				U"<p align=middle><img height=", paragraph -> height * 100,
				U" width=", paragraph -> width * 100, U" src=", MelderFile_name (& pngFile), U"></p>"
			);
			continue;
		}
		if (paragraph -> type == kManPage_type::SCRIPT) {
			chunkNumber += 1;
			if (paragraph -> cacheInfo.string && paragraph -> cacheInfo.string [0] != U'\0') {
				trace (U"text output");
				MelderString_append (buffer, U"<code style=\"color:red\">=&gt;</code><br>\n");   // show a red arrow "=>"
				static MelderString lineBuffer;
				MelderString_empty (& lineBuffer);
				bool inOutputTable = false;
				integer outputTableRowNumber = 0;
				bool hasError = false;
				for (const char32 *paragraphPointer = & paragraph -> cacheInfo.string [0]; *paragraphPointer != U'\0'; paragraphPointer ++) {
					if (Melder_isEndOfLine (*paragraphPointer)) {
						hasError |=
							str32str (lineBuffer.string, U"**AN ERROR OCCURRED IN THIS CODE CHUNK:**") ||
							str32str (lineBuffer.string, U"**ERROR** This code chunk was not run,");
						const bool isTableRow = str32chr (lineBuffer.string, U'\t');
						if (hasError)
							MelderString_append (buffer, U"<code style=\"color:red\">   ");
						else if (isTableRow) {
							if (! inOutputTable) {
								/*
									A new table in the chunk output.
								*/
								MelderString_append (buffer, U"<code><table cellpadding=0 style='text-align:center;border-spacing:0'>\n"
										"<tr><th>");
								inOutputTable = true;
								outputTableRowNumber = 1;
							} else {
								/*
									A new row in an existing table in the chunk output.
								*/
								MelderString_append (buffer, U"<tr>"
										"<td>");
								outputTableRowNumber += 1;
							}
						} else {
							if (inOutputTable) {
								MelderString_append (buffer, U"</table></code>\n");
								inOutputTable = false;
								outputTableRowNumber = 0;
							}
							MelderString_append (buffer, U"<code>   ");
						}
						for (const char32 *plineBuffer = & lineBuffer.string [0]; *plineBuffer != U'\0'; plineBuffer ++) {
							/*if (plineBuffer [0] == U' ' && plineBuffer [1] == U' ') {
								MelderString_append (buffer, U" &nbsp;");
								plineBuffer += 1;
							} else*/
							if (plineBuffer [0] == U' ') {
								MelderString_append (buffer, U" ");
							} else if (plineBuffer [0] == U'\t') {
								if (inBold) {
									inBold = false;
									MelderString_append (buffer, U"</b>");
								} else if (inItalic) {
									inItalic = false;
									MelderString_append (buffer, U"</i>");
								}
								if (outputTableRowNumber == 1)
									MelderString_append (buffer, U"<th>");
								else
									MelderString_append (buffer, U"<td>");
							} else if (plineBuffer [0] == U'\\' && plineBuffer [1] == U'#' && plineBuffer [2] == U'{') {
								inBold = true;
								MelderString_append (buffer, U"<b>");
								plineBuffer += 2;
							} else if (plineBuffer [0] == U'\\' && plineBuffer [1] == U'%' && plineBuffer [2] == U'{') {
								inItalic = true;
								MelderString_append (buffer, U"<i>");
								plineBuffer += 2;
							} else if (plineBuffer [0] == U'}') {
								if (inBold) {
									inBold = false;
									MelderString_append (buffer, U"</b>");
								} else if (inItalic) {
									inItalic = false;
									MelderString_append (buffer, U"</i>");
								} else
									MelderString_appendCharacter (buffer, plineBuffer [0]);
							} else
								MelderString_appendCharacter (buffer, plineBuffer [0]);
						}
						if (inBold) {
							inBold = false;
							MelderString_append (buffer, U"</b>");
						}
						if (inItalic) {
							inItalic = false;
							MelderString_append (buffer, U"</i>");
						}
						if (isTableRow)
							MelderString_append (buffer, U"</tr>\n");
						else
							MelderString_append (buffer, U"<br></code>\n");
						MelderString_empty (& lineBuffer);
					} else {
						MelderString_appendCharacter (& lineBuffer, *paragraphPointer);
					}
				}
				if (hasError)
					continue;   // no longer show any graphics
				if (inOutputTable) {
					MelderString_append (buffer, U"</table></code>\n");
					inOutputTable = false;
					outputTableRowNumber = 0;
				}
			}
			if (paragraph -> height == 0.001)
				continue;
			numberOfPictures ++;
			structMelderFile pngFile { };
			MelderFile_copy (file, & pngFile);
			pngFile. path [Melder_length (pngFile. path) - 5] = U'\0';   // delete extension ".html"
			str32cat (pngFile. path, Melder_cat (U"_", numberOfPictures, U".png"));
			{// scope
				autoGraphics graphics = Graphics_create_pngfile (& pngFile, PNG_RESOLUTION, 0.0, paragraph -> width, 0.0, paragraph -> height);
				Graphics_setFont (graphics.get(), kGraphics_font::TIMES);
				Graphics_setFontStyle (graphics.get(), 0);
				Graphics_setFontSize (graphics.get(), 12.0);
				Graphics_setWrapWidth (graphics.get(), 0);
				theCurrentPraatApplication = & praatApplication;
				theCurrentPraatApplication -> batch = true;
				theCurrentPraatApplication -> topShell = theForegroundPraatApplication. topShell;   // needed for UiForm_create () in dialogs
				theCurrentPraatObjects = & praatObjects;
				theCurrentPraatPicture = & praatPicture;
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
				Graphics_setViewport (graphics.get(),
					theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC,
					theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC
				);
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
				Graphics_setViewport (graphics.get(),
					theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC,
					theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC
				);
				{// scope
					autoMelderProgressOff progress;
					autoMelderWarningOff nowarn;
					autoMelderSaveCurrentFolder saveFolder;
					if (! MelderFolder_isNull (& my rootDirectory))
						Melder_setCurrentFolder (& my rootDirectory);
					const bool dollarSignWasCode = graphics -> dollarSignIsCode;
					const bool backquoteWasVerbatim = graphics -> backquoteIsVerbatim;
					const bool atSignWasLink = graphics -> atSignIsLink;
					Graphics_setDollarSignIsCode (graphics.get(), false);
					Graphics_setBackquoteIsVerbatim (graphics.get(), false);
					Graphics_setAtSignIsLink (graphics.get(), false);
					Graphics_play (paragraph -> cacheGraphics.get(), graphics.get());
					if (dollarSignWasCode)
						Graphics_setDollarSignIsCode (graphics.get(), true);
					if (backquoteWasVerbatim)
						Graphics_setBackquoteIsVerbatim (graphics.get(), true);
					if (atSignWasLink)
						Graphics_setAtSignIsLink (graphics.get(), true);
				}
				Graphics_setViewport (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				Graphics_setWindow (graphics.get(), 0.0, 1.0, 0.0, 1.0);
				Graphics_setTextAlignment (graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
			}
			MelderString_append (buffer,
				U"<p align=middle><img height=", paragraph -> height * 100,
				U" width=", paragraph -> width * 100, U" src=", MelderFile_name (& pngFile), U"></p>"
			);
			theCurrentPraatApplication = & theForegroundPraatApplication;
			theCurrentPraatObjects = & theForegroundPraatObjects;
			theCurrentPraatPicture = & theForegroundPraatPicture;
			continue;
		}

		if (isListItem || isTag || isDefinition) {
			if (! inList) {
				ul = ( isListItem && (p [0] == U'•' || (p [0] == U'\\' && p [1] == U'b' && p [2] == U'u')) );
				ul = false;   // TODO
				MelderString_append (buffer, ul ? U"<ul>\n" : U"<dl>\n");
				inList = true;
			}
			if (ul) {
				if (p [0] == U'•'  && p [1] == U' ')
					p += 1;
				if (p [0] == U'\\' && p [1] == U'b' && p [2] == U'u' && p [3] == U' ')
					p += 3;
			}
			MelderString_append (buffer, ul ? U"<li>" : stylesInfo [(int) paragraph -> type]. htmlIn/*, U"\n"*/);
		} else {
			if (inList) {
				MelderString_append (buffer, ul ? U"</ul>\n" : U"</dl>\n");
				inList = ul = false;
			}
			MelderString_append (buffer, stylesInfo [(int) paragraph -> type]. htmlIn,
					paragraphIsVerbatim ? U"" : U"");   // in a verbatim paragraph we would actually get a newline
		}
		/* mutable */ bool inTable = !! str32chr (p, U'\t');
		/* mutable */ bool inPromptedTable = false;
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
			Leading spaces should be visible, so we convert them to a non-breaking space ("&nbsp;").
			This is not needed in code fragments, because we have already have a "whitespace:pre-wrap" style there.
		*/
		if (! paragraphIsVerbatim)
			while (*p == U' ') {
				MelderString_append (buffer, U"&nbsp;");
				p ++;
			}
		if (paragraphIsVerbatim) {
			while (*p != U'\0') {
				if (*p == U'\\' && p [1] == U'#' && p [2] == U'{') {
					MelderString_append (buffer, U"<b>");
					p += 3;
					while (*p != U'}' && *p != U'\0')
						MelderString_append (buffer, *p ++);
					MelderString_append (buffer, U"</b>");
					if (*p)
						p ++;   // skip closing brace
				} else if (*p == U'\\' && p [1] == U'%' && p [2] == U'{') {
					MelderString_append (buffer, U"<i>");
					p += 3;
					while (*p != U'}' && *p != U'\0')
						MelderString_append (buffer, *p ++);
					MelderString_append (buffer, U"</i>");
					if (*p)
						p ++;   // skip closing brace
				} else if (
					*p == U'\\' && p [1] == U'@' && p [2] == U'{' ||
					*p == U'\\' && p [1] == U'#' && p [2] == U'@' && p [3] == U'{' ||
					*p == U'\\' && p [1] == U'`' && p [2] == U'{' ||
					*p == U'\\' && p [1] == U'#' && p [2] == U'`' && p [3] == U'{'
				) {
					const bool isBold = ( p [1] == U'#' );
					p += 3 + isBold;
					if (isBold)
						MelderString_append (buffer, U"<b>");
					const bool isVerbatim = ( p [-2] == U'`' );
					static MelderString link, linkText;
					MelderString_empty (& link);
					if (isVerbatim)
						MelderString_appendCharacter (& link, U'`');
					MelderString_empty (& linkText);
					while (*p != U'}' && *p != U'\0') {
						if (*p == U'|' && p [1] == U'|') {
							p += 2;
							MelderString_empty (& linkText);
						} else {
							MelderString_append (& link, *p);
							MelderString_append (& linkText, *p);
							p ++;
						}
					}
					if (*p)
						p ++;   // skip closing brace
					if (link.length > 0 && link.string [link.length - 1] == U':') {
						link.string [link.length - 1] = U'.';
						MelderString_append (& link, U"..");
					}
					if (isVerbatim)
						MelderString_appendCharacter (& link, U'`');
					writeLinkAsHtml (me, link.string, linkText.string, buffer, page -> title.get());
					if (isBold)
						MelderString_append (buffer, U"</b>");
				} else if (*p == U'<') {
					MelderString_append (buffer, U"&lt;");
					p ++;
				} else if (*p == U'>') {
					MelderString_append (buffer, U"&gt;");
					p ++;
				} else if (*p == U'&') {
					MelderString_append (buffer, U"&amp;");
					p ++;
				} else
					MelderString_append (buffer, *p ++);
			}
			MelderString_append (buffer, stylesInfo [(int) paragraph -> type]. htmlOut, U"\n");
			continue;
		}
		while (*p) {
			if (wordItalic && ! isSingleWordCharacter (*p)) {
				if (*p == U'\\' && p [1] != U'\0' && p [2] != U'\0') {
					Longchar_Info info = Longchar_getInfo (p [1], p [2]);
					if (! isSingleWordCharacter (info -> unicode)) {
						MelderString_append (buffer, U"</i>");
						wordItalic = false;
					}
				} else {
					MelderString_append (buffer, U"</i>");
					wordItalic = false;
				}
			}
			if (wordBold && ! isSingleWordCharacter (*p)) {
				if (*p == U'\\' && p [1] != U'\0' && p [2] != U'\0') {
					Longchar_Info info = Longchar_getInfo (p [1], p [2]);
					if (! isSingleWordCharacter (info -> unicode)) {
						MelderString_append (buffer, U"</b>");
						wordBold = false;
					}
				} else {
					MelderString_append (buffer, U"</b>");
					wordBold = false;
				}
			}
			if (wordCode && ! isSingleWordCharacter (*p)) {   // TODO: remove, because wordCode should no longer exist
				if (*p == U'\\' && p [1] != U'\0' && p [2] != U'\0') {
					Longchar_Info info = Longchar_getInfo (p [1], p [2]);
					if (! isSingleWordCharacter (info -> unicode)) {
						MelderString_append (buffer, U"</font></code>");
						wordCode = false;
					}
				} else {
					MelderString_append (buffer, U"</font></code>");
					wordCode = false;
				}
			}
			if (*p == U'@') {
				static MelderString link, linkText;
				MelderString_empty (& link);
				MelderString_empty (& linkText);
				if (p [1] == U'@' && ! paragraphIsVerbatim) {
					p += 2;
					while (*p != U'@' && *p != U'|' && *p != U'\0')
						MelderString_appendCharacter (& link, * p ++);
					if (*p == U'|') {
						if (p [1] == U'|') {
							const char32 *p2 = p + 2;
							while (*p2 != U'@' && *p2 != U'\0')
								MelderString_appendCharacter (& link, * p2 ++);
							p += 2;   // skip "||"
						} else
							p += 1;   // skip "|"
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
									const char32 kar1 = *++p, kar2 = *++p;
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
				} else if (p [1] == U'`') {
					p += 2;
					MelderString_append (& link, U'`');
					MelderString_append (& linkText, U"<b><code><font size=+1>");
					while (*p != U'`' && *p != U'\0') {
						MelderString_append (& link, * p);
						MelderString_append (& linkText, * p);
						p ++;
					}
					if (*p)
						p ++;   // skip closing backquote
					MelderString_append (& linkText, U"</font></code></b>");
					MelderString_append (& link, U'`');
				} else {
					p ++;
					while (isSingleWordCharacter (*p) && *p != U'\0')
						MelderString_append (& link, *p++);
					MelderString_copy (& linkText, link.string);
				}
				writeLinkAsHtml (me, link.string, linkText.string, buffer, page -> title.get());
			} else if (*p == U'%') {
				if (p [1] == U'`') {
					p += 2;
					MelderString_append (buffer, U"<i><code><font size=+1>");
					while (*p != U'`' && *p != U'\0')
						MelderString_append (buffer, * p ++);
					if (*p)
						p ++;
					MelderString_append (buffer, U"</font></code></i>");
				} else if (inItalic) {
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
					if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold)   { MelderString_append (buffer, U"</b>"); wordBold   = false; }
					MelderString_append (buffer, U"</sub>");
					inSub = false;
					p ++;
				} else if (p [1] == U'_') {
					if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
					if (wordBold)   { MelderString_append (buffer, U"</b>"); wordBold   = false; }
					MelderString_append (buffer, U"<sub>");
					inSub = true;
					p += 2;
				} else {
					if (verbatimAware) {
						if (wordItalic) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
						if (wordBold)   { MelderString_append (buffer, U"</b>"); wordBold   = false; }
						MelderString_append (buffer, U"<sub>");
						letterSub = true;
					} else
						MelderString_append (buffer, U"_");
					p ++;
				}
			} else if (*p == U'#') {
				if (p [1] == U'`') {
					p += 2;
					MelderString_append (buffer, U"<b><code><font size=+1>");
					while (*p != U'`' && *p != U'\0')
						MelderString_append (buffer, * p ++);
					if (*p)
						p ++;
					MelderString_append (buffer, U"</font></code></b>");
				} else if (inBold) {
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
					MelderString_append (buffer, U"</font></code>");
					inCode = false;
					p ++;
				} else if (p [1] == U'$') {
					MelderString_append (buffer, U"<code><font size=+1>");
					inCode = true;
					p += 2;
				} else if (p [1] == U'%') {
					MelderString_append (buffer, U"<code><font size=+1><i>");
					wordCode = true;
					wordItalic = true;
					p += 2;
				} else if (p [1] == U'#') {
					MelderString_append (buffer, U"<code><font size=+1><b>");
					wordCode = true;
					wordBold = true;
					p += 2;
				} else {
					MelderString_append (buffer, U"<code><font size=+1>");
					wordCode = true;
					p ++;
				}
			} else if (verbatimAware && *p == U'`') {
				MelderString_append (buffer, U"<code><font size=+1>");
				++ p;
				while (*p != U'\0' && *p != U'`')
					MelderString_append (buffer, *p ++);
				if (*p)
					++ p;   // skip closing backquote
				MelderString_append (buffer, U"</font></code>");
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
				//if (wordItalic && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</i>"); wordItalic = false; }
				//if (wordBold && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</b>"); wordBold = false; }
				//if (wordCode && ! isSingleWordCharacter (*p)) { MelderString_append (buffer, U"</code>"); wordCode = false; }
				if (*p == U'\\' && p [1] != U'\0' && p [2] != U'\0') {
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
				if (letterSub) {
					if (wordItalic) {
						MelderString_append (buffer, U"</i>");
						wordItalic = false;
					}
					if (wordBold) {
						MelderString_append (buffer, U"</b>");
						wordBold = false;
					}
					MelderString_append (buffer, U"</sub>");
					letterSub = false;
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
			MelderString_append (buffer, U"</font></code>");
			inCode = wordCode = false;
		}
		if (inSub || letterSub) {
			MelderString_append (buffer, U"</sub>");
			inSub = letterSub = false;
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
	praatObjects. reset();
}

static void writeTitleAsHtml (conststring32 title, MelderString *buffer) {
	static MelderString titleBuffer;
	const bool titleHasToBeVerbatim = ( title [0] == U'`' );
	if (titleHasToBeVerbatim)
		MelderString_ncopy (& titleBuffer, & title [1], Melder_length (& title [1]) - 1);
	else
		MelderString_copy (& titleBuffer, title);
	MelderString_append (buffer,
		U"<html><head><meta name=\"robots\" content=\"index,follow\">"
		U"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
		U"<title>", titleBuffer.string, U"</title>\n"
		U"<style>\n"
		U"   td { padding-left: 5pt; padding-right: 5pt; }\n"
		U"   th { padding-left: 5pt; padding-right: 5pt; }\n"
		U"   code { white-space: pre-wrap; }\n"
		U"   dd { white-space: pre-wrap; }\n"
		U"</style>\n"
		U"</head><body bgcolor=\"#FFFFFF\">\n\n"
	);
	MelderString_append (buffer,
		U"<table border=0 cellpadding=0 cellspacing=0><tr><td bgcolor=\"#CCCC00\">"
		U"<table border=4 cellpadding=9><tr><td align=middle bgcolor=\"#000000\">",
		titleHasToBeVerbatim ?
			U"<font face=\"Monaco,Courier\" size=6 color=\"#999900\"><b>\n" :
			U"<font face=\"Palatino,Times\" size=6 color=\"#999900\"><b>\n",
		titleBuffer.string, U"\n</b></font></table></table>\n"
	);
}

static void writePageAsHtml (ManPages me, Interpreter optionalInterpreterReference, MelderFile file, integer ipage, MelderString *buffer) {
	ManPage page = my pages.at [ipage];
	writeTitleAsHtml (page -> title.get(), buffer);
	writeParagraphsAsHtml (me, optionalInterpreterReference, file, page, buffer);
	if (ManPages_uniqueLinksHither (me, ipage)) {
		integer ilink, jlink;
		if (page -> paragraphs.size > 0) {
			conststring32 text = page -> paragraphs [page -> paragraphs.size]. text;
			if (text && text [0] != U'\0' && text [Melder_length (text) - 1] != U':')
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
					if (isAllowedFileNameCharacter (*p))
						MelderString_appendCharacter (buffer, *p);
					else if (*p == U'#')
						MelderString_append (buffer, U"-H");
					else if (*p == U'$')
						MelderString_append (buffer, U"-S");
					else if (*p == U'@')
						MelderString_append (buffer, U"-C");
					else
						MelderString_appendCharacter (buffer, U'_');
				}
				if (title [0] == U'\0')
					MelderString_append (buffer, U"_");
				if (title [0] == U'`') {
					static MelderString visibleTitle;
					MelderString_copy (& visibleTitle, title);
					if (visibleTitle.string [visibleTitle.length - 1] == U'`')
						visibleTitle.string [visibleTitle.length - 1] = U'\0';
					MelderString_append (buffer, U".html\"><code><font size=+1>",
							& visibleTitle.string [1], U"</font></code></a>\n");
				} else
					MelderString_append (buffer, U".html\">", title, U"</a>\n");
			}
		}
		MelderString_append (buffer, U"</ul>\n");
	}
	MelderString_append (buffer, U"<hr>\n<address>\n\t<p>", page -> signature.get());
	MelderString_append (buffer, U"</p>\n</address>\n</body>\n</html>\n");
}

void ManPages_writeOneToHtmlFile (ManPages me, Interpreter optionalInterpreterReference, integer ipage, MelderFile file) {
	static MelderString buffer;
	MelderString_empty (& buffer);
	writePageAsHtml (me, optionalInterpreterReference, file, ipage, & buffer);
	MelderFile_writeText (file, buffer.string, kMelder_textOutputEncoding::UTF8);
}

void ManPages_writeAllToHtmlDir (ManPages me, Interpreter optionalInterpreterReference, conststring32 dirPath) {
	structMelderFolder dir { };
	Melder_pathToFolder (dirPath, & dir);
	for (integer ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		char32 fileName [ManPages_FILENAME_BUFFER_SIZE];
		Melder_assert (Melder_length (page -> title.get()) < ManPages_FILENAME_BUFFER_SIZE - 100);
		trace (U"page ", ipage, U": ", page -> title.get());
		char32 *to = fileName, *max = fileName + ManPages_FILENAME_BUFFER_SIZE - (8 + 1);
		for (const char32 *from = & page -> title [0]; *from != U'\0'; from ++) {
			if (isAllowedFileNameCharacter (*from)) {
				if (to < max)
					*to ++ = *from;
			} else if (*from == U'#') {
				if (to < max)
					*to ++ = U'-';
				if (to < max)
					*to ++ = U'H';
			} else if (*from == U'$') {
				if (to < max)
					*to ++ = U'-';
				if (to < max)
					*to ++ = U'S';
			} else if (*from == U'@') {
				if (to < max)
					*to ++ = U'-';
				if (to < max)
					*to ++ = U'C';
			} else {
				if (to < max)
					*to ++ = U'_';
			}
		}
		if (to == fileName)
			*to ++ = U'_';   // no empty file names please
		*to = U'\0';
		fileName [LONGEST_FILE_NAME] = U'\0';   //
		str32cat (fileName, U".html");
		static MelderString buffer;
		MelderString_empty (& buffer);
		structMelderFile file { };
		MelderFolder_getFile (& dir, fileName, & file);
		writePageAsHtml (me, optionalInterpreterReference, & file, ipage, & buffer);
		/*
			An optimization because reading is much faster than writing:
			we write the file only if the old file is different or doesn't exist.
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

/* End of file ManPages_toHtml.cpp */
