/* Graphics_text.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2004/01/28 Windows: for PostScript, download Times New Roman instead of Times
 * pb 2004/02/02 Windows: my metafile instead of my printer for high resolution in metafiles and clipboards
 * pb 2004/02/04 EPS files: secondary fonts
 * pb 2004/03/13 EPS files: secondary font for Palatino as well
 * pb 2004/08/20 Windows EPS files: if "include fonts" is off, take SILDoulosIPA rather than SILDoulosIPA-Regular
 * pb 2004/10/24 better slant correction
 * pb 2004/11/28 Melder_debug == 15 to force bitmapped IPA
 * pb 2004/12/02 Linux: xipa support
 * pb 2005/02/03 TeX-xipa10-Praat-Regular
 * pb 2005/03/08 psEncoding; SILIPA93 encoding for Windows and Mac
 * pb 2005/03/15 find PostScript Courier with -p-
 * pb 2005/09/18 useSilipaPS, including bold
 * pb 2005/10/27 corrected character width for Symbol (should not depend on SILIPA setting)
 * pb 2005/11/11 Windows: font sizes up to 500
 * pb 2006/10/20 links are recorded in DC (no longer WC)
 * pb 2007/06/11 wchar_t
 * pb 2007/08/01 reintroduced yIsZeroAtTheTop
 * pb 2007/08/25 use Charis SIL or Doulos SIL rather than SILDoulos IPA93
 * pb 2007/09/29 correct counting of UTF-8 bytes
 * pb 2007/12/09 enums
 * pb 2008/03/24 cairo
 * pb 2009/03/14 switched kerning off
 * pb 2009/09/17 made Quartz part resistant against missing QuickDraw IPA font
 * pb 2010/05/13 support for XOR mode via GDK
 * pb 2010/06/29 Mac: handle missing phonetic fonts better
 * pb 2010/07/13 cairo: rotated text
 */

#include <ctype.h>
#include "UnicodeData.h"
#include "GraphicsP.h"
#include "longchar.h"
#include "Printer.h"

extern char * ipaSerifRegularPS [];
extern char * ipaSerifRegular24 [1 + 255-33+1 + 1] [24 + 1];

/*
 * When computing the width of a text by adding the widths of the separate characters,
 * we will make a correction for systems that make slanted characters overlap the character box to their right.
 * The effect is especially strong on Mac (older versions).
 * The slant correction is taken relative to the font size.
 */
#define POSTSCRIPT_SLANT_CORRECTION  0.1
#define SLANT_CORRECTION  POSTSCRIPT_SLANT_CORRECTION

/*
 * The Praat PostScript and the Mac character encodings have fi and fl ligature symbols.
 * The ISO8859-1 character encoding, which is used on Xwin and Win, has not.
 */
#if mac
	#define HAS_FI_AND_FL_LIGATURES  TRUE
#else
	#define HAS_FI_AND_FL_LIGATURES  ( my postScript == TRUE )
#endif

#if xwin
	static XFontStruct * fontInfos [1 + kGraphics_font_DINGBATS] [5] [1 + Graphics_BOLD_ITALIC];
	#define IPA_ID  -16012112   /* Magic number. */
	static struct { Window window; GC gc; Pixmap pixmap; XImage *image; } rotate;
#elif win
	#define win_MAXIMUM_FONT_SIZE  500
	static HFONT screenFonts [1 + kGraphics_font_DINGBATS] [1+win_MAXIMUM_FONT_SIZE] [1 + Graphics_BOLD_ITALIC];
	static HFONT printerFonts [1 + kGraphics_font_DINGBATS] [1+win_MAXIMUM_FONT_SIZE] [1 + Graphics_BOLD_ITALIC];
	static int ipaAvailable = FALSE;
	static int win_size2isize (int size) { return size > win_MAXIMUM_FONT_SIZE ? win_MAXIMUM_FONT_SIZE : size; }
	static int win_isize2size (int isize) { return isize; }
#elif mac
	#include "macport_on.h"
	#include <Fonts.h>
	#include <Gestalt.h>
	#include <QDOffscreen.h>
	#include "macport_off.h"
	static int ipaAvailable = FALSE, ipaInited = FALSE;
	static short int theTimesFont, theHelveticaFont, theCourierFont, theSymbolFont,
		thePalatinoFont, theIpaTimesFont, theZapfDingbatsFont;
	static ATSFontRef theTimesAtsuiFont, theHelveticaAtsuiFont, theCourierAtsuiFont, theSymbolAtsuiFont,
		thePalatinoAtsuiFont, theIpaTimesAtsuiFont, theIpaPalatinoAtsuiFont, theZapfDingbatsAtsuiFont, theArabicAtsuiFont;
	static RGBColor theBlackColour = { 0, 0, 0 }, theWhiteColour = { 0xFFFF, 0xFFFF, 0xFFFF }, theBlueColour = { 0, 0, 0xFFFF };
#endif

#if xwin
static XFontStruct * loadFont (I, int font, int size, int style) {
	iam (GraphicsScreen);
	XFontStruct *fontInfo;
	char name [100];
	/*
	 * An Xwindows font name looks like this:
	 * -foundry-                     // Replaced with a wild card: *
	 * family-                       // times-, courier-, etc
	 * weight-                       // medium-, bold-
	 * slant-                        // r-, i- (italic), o- (oblique)
	 * setWidth--                    // Always normal--
	 * pixels-                       // Wildcarded because of resolution independence: *-
	 * height_decipoints-            // 100-, 120-, 140-, 180-, 240-
	 * horizontalResolution_dpi-     // 75- or 100-
	 * verticalResolution_dpi-       // 75- or 100-
	 * spacing-                      // m- for Courier, else p-
	 * averageWidth_decipixel-       // Wildcarded: *-
	 * characterSet                  // iso8859-1 or adobe-fontspecific
	 */
	sprintf (name, "-*-%s-%s-normal--*-%d0-%d-%d-%s-*-%s",
		font == kGraphics_font_TIMES ? "times" :
		font == kGraphics_font_COURIER ? "courier" :
		font == kGraphics_font_PALATINO ? "palatino" :
		font == kGraphics_font_IPATIMES ? "xipadontusebecausethishasthewrongencoding" :
		font == kGraphics_font_DINGBATS ? "itc zapf dingbats" :
		font == kGraphics_font_SYMBOL ? "symbol" : "helvetica",
		font == kGraphics_font_SYMBOL || font == kGraphics_font_DINGBATS ? "medium-r" :
		style == Graphics_BOLD ? "bold-r" :
		style == Graphics_ITALIC ?
			font == kGraphics_font_TIMES || font == kGraphics_font_PALATINO ? "medium-i" : "medium-o" :
		style == Graphics_BOLD_ITALIC ?
			font == kGraphics_font_TIMES || font == kGraphics_font_PALATINO ? "bold-i" : "bold-o" : "medium-r",
		size == 0 ? 10 : size == 1 ? 12 : size == 2 ? 14 : size == 3 ? 18 : 24,
		font == kGraphics_font_PALATINO || font == kGraphics_font_DINGBATS ? 100 : my resolution < 100 ? 75 : 100,
		font == kGraphics_font_PALATINO || font == kGraphics_font_DINGBATS ? 100 : my resolution < 100 ? 75 : 100,
		"*" /*font == kGraphics_font_COURIER ? "m" : "p"*/,
		font == kGraphics_font_SYMBOL || font == kGraphics_font_IPATIMES || font == kGraphics_font_DINGBATS ?
			"adobe-fontspecific" : "iso8859-1");
	fontInfo = XLoadQueryFont (my display, name);
	if (! fontInfo) {
		/*
		 * Font not available. Is likely to happen with SIL Doulos IPA.
		 * For SIL Doulos IPA, we have a replacement in the form of Praat-internal bitmaps,
		 * encoded in the same way as the TeX-xipa-Praat PostScript font (so that xwin encoding equals ps encoding for IPA).
		 */
		if (font == kGraphics_font_IPATIMES) {
			int ichar;
			Melder_casual ("Font \"%s\" not found. Using bitmaps instead.", name);
			fontInfo = Melder_calloc (XFontStruct, 1);
			fontInfo -> fid = (Font) IPA_ID; 
			fontInfo -> min_char_or_byte2 = 33;
			fontInfo -> max_char_or_byte2 = 255;
			fontInfo -> per_char = Melder_calloc (XCharStruct, 255 - 33 + 1);
			for (ichar = 33; ichar <= 255; ichar ++) {
				int overstrike = ipaSerifRegular24 [ichar - 32] [0] [0] == 'o';
				if (overstrike)
					fontInfo -> per_char [ichar - 33]. lbearing = strlen (ipaSerifRegular24 [ichar - 32] [0]);
				else
					fontInfo -> per_char [ichar - 33]. width = strlen (ipaSerifRegular24 [ichar - 32] [0]);
			}
		} else {
			Melder_casual ("Font \"%s\" not found. Trying Courier instead.", name);
			sprintf (name, "-*-courier-medium-r-normal--*-%d0-%d-%d-*-*-iso8859-1",
				size == 0 ? 10 : size == 1 ? 12 : size == 2 ? 14 : size == 3 ? 18 : 24,
				my resolution < 100 ? 75 : 100, my resolution < 100 ? 75 : 100);
			fontInfo = XLoadQueryFont (my display, name);
			if (! fontInfo) {
				Melder_casual ("Praat requires the Adobe fonts Times, Helvetica, Courier and Symbol.");
				Melder_casual ("Praat may now crash.");
				return NULL;
			}
		}
	}
	fontInfos [font] [size] [style] = fontInfo;
	return fontInfo;
}
#elif win
static bool charisAvailable = false, doulosAvailable = false;
static int CALLBACK fontFuncEx_charis (const LOGFONTW *oldLogFont, const TEXTMETRICW *oldTextMetric, unsigned long fontType, LPARAM lparam) {
	const LPENUMLOGFONTW logFont = (LPENUMLOGFONTW) oldLogFont; (void) oldTextMetric; (void) fontType; (void) lparam;
	charisAvailable = TRUE;
	return 1;
}
static int CALLBACK fontFuncEx_doulos (const LOGFONTW *oldLogFont, const TEXTMETRICW *oldTextMetric, unsigned long fontType, LPARAM lparam) {
	const LPENUMLOGFONTW logFont = (LPENUMLOGFONTW) oldLogFont; (void) oldTextMetric; (void) fontType; (void) lparam;
	doulosAvailable = TRUE;
	return 1;
}
static HFONT loadFont (GraphicsScreen me, int font, int size, int style) {
	LOGFONTW spec;
	static int ipaInited;
	if (my printer || my metafile) {
		spec. lfHeight = - win_isize2size (size) * my resolution / 72.0;
	} else {
		spec. lfHeight = - win_isize2size (size) * my resolution / 72.0;
	}
	spec. lfWidth = 0;
	spec. lfEscapement = spec. lfOrientation = 0;
	spec. lfWeight = style & Graphics_BOLD ? FW_BOLD : 0;
	spec. lfItalic = style & Graphics_ITALIC ? 1 : 0;
	spec. lfUnderline = spec. lfStrikeOut = 0;
	spec. lfCharSet = font == kGraphics_font_SYMBOL ? SYMBOL_CHARSET : font >= kGraphics_font_IPATIMES ? DEFAULT_CHARSET : ANSI_CHARSET;
	spec. lfOutPrecision = spec. lfClipPrecision = spec. lfQuality = 0;
	spec. lfPitchAndFamily =
		( font == kGraphics_font_COURIER ? FIXED_PITCH : font == kGraphics_font_IPATIMES ? DEFAULT_PITCH : VARIABLE_PITCH ) |
		( font == kGraphics_font_HELVETICA ? FF_SWISS : font == kGraphics_font_COURIER ? FF_MODERN :
		  font >= kGraphics_font_IPATIMES ? FF_DONTCARE : FF_ROMAN );
	if (font == kGraphics_font_IPATIMES && ! ipaInited && Melder_debug != 15) {
		LOGFONTW logFont;
		logFont. lfCharSet = DEFAULT_CHARSET;
		logFont. lfPitchAndFamily = 0;
		wcscpy (logFont. lfFaceName, L"Charis SIL");
		EnumFontFamiliesExW (my dc, & logFont, fontFuncEx_charis, 0, 0);
		wcscpy (logFont. lfFaceName, L"Doulos SIL");
		EnumFontFamiliesExW (my dc, & logFont, fontFuncEx_doulos, 0, 0);
		ipaInited = TRUE;
		if (! charisAvailable && ! doulosAvailable) {
			/* BUG: The next warning may cause reentry of drawing (on window exposure) and lead to crash. Some code must be non-reentrant !! */
			Melder_warning1 (L"The phonetic font is not available.\nSeveral characters will not look correct.\nSee www.praat.org");
		} else {
			ipaAvailable = true;
		}
	}
	wcscpy (spec. lfFaceName,
		font == kGraphics_font_HELVETICA ? L"Arial" :
		font == kGraphics_font_TIMES ? L"Times New Roman" :
		font == kGraphics_font_COURIER ? L"Courier New" :
		font == kGraphics_font_PALATINO ? L"Book Antiqua" :
		font == kGraphics_font_SYMBOL ? L"Symbol" :
		font == kGraphics_font_IPATIMES ? ( doulosAvailable && style == 0 ? L"Doulos SIL" : charisAvailable ? L"Charis SIL" : L"Times New Roman" ) :
		font == kGraphics_font_DINGBATS ? L"Wingdings" :
		L"");
	return CreateFontIndirectW (& spec);
}
#endif

static void charSize (I, _Graphics_widechar *lc) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			if (my duringXor) {
				Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
				int normalSize = my fontSize * my resolution / 72.0;
				int smallSize = (3 * normalSize + 2) / 4;
				int size = lc -> size < 100 ? smallSize : normalSize;
				lc -> width = 10;
				lc -> baseline *= my fontSize * 0.01;
				lc -> code = lc -> kar;
				lc -> font.string = NULL;
				lc -> font.integer = 0;
				lc -> size = size;
			} else {
				if (my cr == NULL) return;
				Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
				int font, size, style;
				int normalSize = my fontSize * my resolution / 72.0;
				int smallSize = (3 * normalSize + 2) / 4;
				size = lc -> size < 100 ? smallSize : normalSize;
				cairo_text_extents_t extents;

				enum _cairo_font_slant slant   = (lc -> style & Graphics_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL);
				enum _cairo_font_weight weight = (lc -> style & Graphics_BOLD   ? CAIRO_FONT_WEIGHT_BOLD  : CAIRO_FONT_WEIGHT_NORMAL);

				cairo_set_font_size (my cr, size);
				
				font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
					   info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
					   info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : lc -> font.integer;

				switch (font) {
					case kGraphics_font_HELVETICA: cairo_select_font_face (my cr, "Helvetica", slant, weight); break;
					case kGraphics_font_TIMES:     cairo_select_font_face (my cr, "Times New Roman", slant, weight); break;
					case kGraphics_font_COURIER:   cairo_select_font_face (my cr, "Courier", slant, weight); break;
					case kGraphics_font_PALATINO:  cairo_select_font_face (my cr, "Palatino", slant, weight); break;
					case kGraphics_font_SYMBOL:    cairo_select_font_face (my cr, "Symbol", slant, weight); break;
					case kGraphics_font_IPATIMES:  cairo_select_font_face (my cr, "IPA Times", slant, weight); break;
					case kGraphics_font_DINGBATS:  cairo_select_font_face (my cr, "Dingbats", slant, weight); break;
					default:                       cairo_select_font_face (my cr, "Sans", slant, weight); break;
				}
				wchar_t buffer [2] = { lc -> kar, 0 };
				cairo_text_extents (my cr, Melder_peekWcsToUtf8 (buffer), & extents);
				lc -> width = extents.x_advance;
				lc -> baseline *= my fontSize * 0.01;
				lc -> code = lc -> kar;
				lc -> font.string = NULL;
				lc -> font.integer = font;
				lc -> size = size;
			}
		#elif xwin
			Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
			int font, size, style;
			XFontStruct *fontInfo;
			XCharStruct *charInfo;
			int normalSize = my fontSize < 11 ? 0 : my fontSize < 13 ? 1 :
			                 my fontSize < 16 ? 2 : my fontSize < 21 ? 3 : 4;
			int smallSize = normalSize == 0 ? 0 : normalSize - 1;
			font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
			       info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
			       info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : lc -> font.integer;
			size = lc -> size < 100 ? smallSize : normalSize;
			style = lc -> style & (Graphics_ITALIC | Graphics_BOLD);
			fontInfo = fontInfos [font] [size] [style];
			if (! fontInfo &&
			    (fontInfo = fontInfos [font] [size] [style] = loadFont (me, font, size, style)) == NULL) return;
			charInfo = & fontInfo -> per_char [info -> xwinEncoding - fontInfo -> min_char_or_byte2];
			lc -> width = charInfo -> width;
			lc -> baseline *= my fontSize * 0.01;
			lc -> code = info -> xwinEncoding;
			lc -> font.string = 0;
			lc -> font.integer = (long) fontInfo -> fid;
		#elif win
			Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
			int font, size, style;
			HFONT fontInfo;
			int normalSize = win_size2isize (my fontSize);
			int smallSize = (3 * normalSize + 2) / 4;
			font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
			       info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
			       info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : lc -> font.integer;
			size = lc -> size < 100 ? smallSize : normalSize;
			style = lc -> style & (Graphics_ITALIC | Graphics_BOLD);   // take out Graphics_CODE
			fontInfo = my printer || my metafile ? printerFonts [font] [size] [style] : screenFonts [font] [size] [style];
			if (! fontInfo) {
				fontInfo = loadFont (me, font, size, style);
				if (! fontInfo) return;
				if (my printer || my metafile) {
					printerFonts [font] [size] [style] = fontInfo;
				} else {
					screenFonts [font] [size] [style] = fontInfo;
				}
			}
			if (font == kGraphics_font_IPATIMES && ! ipaAvailable) {
				int overstrike = ipaSerifRegular24 [info -> psEncoding - 32] [0] [0] == 'o';
				lc -> code = info -> psEncoding;
				if (overstrike)
					lc -> width = 0;
				else
					lc -> width = strlen (ipaSerifRegular24 [info -> psEncoding - 32] [0]);
			} else {
				SIZE extent;
				wchar_t code;
				lc -> code = font == kGraphics_font_IPATIMES || font == kGraphics_font_TIMES || font == kGraphics_font_HELVETICA || font == kGraphics_font_COURIER ? (unsigned short) lc -> kar : info -> winEncoding;
				if (lc -> code == 0) {
					_Graphics_widechar *lc2;
					if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK) {
						info = Longchar_getInfo ('s', 'w');
						lc -> kar = info -> unicode;
						lc -> code = info -> winEncoding;
						for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
						lc2 [1]. kar = '\0';
						while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
						lc [1]. kar = UNICODE_MODIFIER_LETTER_RHOTIC_HOOK;
					} else if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE) {
						info = Longchar_getInfo ('l', ' ');
						lc -> kar = info -> unicode;
						lc -> code = info -> winEncoding;
						for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
						lc2 [1]. kar = '\0';
						while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
						lc [1]. kar = UNICODE_COMBINING_TILDE_OVERLAY;
					}
				}
				SelectFont (my dc, fontInfo);
				GetTextExtentPoint32W (my dc, (code = (unsigned short) lc -> code, & code), 1, & extent);   // UTF-32 BUG
				lc -> width = extent. cx;
			}
			lc -> baseline *= my fontSize * 0.01 * my resolution / 72.0;
			lc -> font.string = NULL;
			lc -> font.integer = font;   // kGraphics_font_HELVETICA .. kGraphics_font_DINGBATS
			lc -> size = size;   // 0..4 instead of 10..24
			lc -> style = style;   // without Graphics_CODE
		#elif mac
			if (my useQuartz) {
				Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
				int normalSize = my fontSize * my resolution / 72.0;
				lc -> size = lc -> size < 100 ? (3 * normalSize + 2) / 4 : /*lc -> size > 100 ? 1.2 * normalSize :*/ normalSize;
				lc -> baseline *= 0.01 * normalSize;
				long saveFont = lc -> font.integer;   // Save!
				lc -> font.string = NULL;   // This erases font.integer!
				ATSFontRef atsuiFont =
					info -> alphabet == Longchar_SYMBOL ? theSymbolAtsuiFont :
					info -> alphabet == Longchar_PHONETIC ? ( my font == kGraphics_font_TIMES && lc -> style == 0 ? theIpaTimesAtsuiFont : theIpaPalatinoAtsuiFont ) :
					lc -> kar == '/' ? thePalatinoAtsuiFont :   /* Override Courier. */
					info -> alphabet == Longchar_DINGBATS ? theZapfDingbatsAtsuiFont:
					saveFont == kGraphics_font_COURIER ? theCourierAtsuiFont :
					my font == kGraphics_font_TIMES ? theTimesAtsuiFont :
					my font == kGraphics_font_HELVETICA ? theHelveticaAtsuiFont :
					my font == kGraphics_font_COURIER ? theCourierAtsuiFont : theTimesAtsuiFont;
				Melder_assert (atsuiFont != 0);
				lc -> font.integer = (int) atsuiFont;   // BUG: not 64-bit compatible
				lc -> code = lc -> kar;
				if (lc -> code == 0) {
					_Graphics_widechar *lc2;
					if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK) {
						info = Longchar_getInfo ('s', 'w');
						lc -> kar = info -> unicode;
						lc -> code = info -> unicode;
						for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
						lc2 [1]. kar = '\0';
						while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
						lc [1]. kar = UNICODE_MODIFIER_LETTER_RHOTIC_HOOK;
					} else if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE) {
						info = Longchar_getInfo ('l', ' ');
						lc -> kar = info -> unicode;
						lc -> code = info -> unicode;
						for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
						lc2 [1]. kar = '\0';
						while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
						lc [1]. kar = UNICODE_COMBINING_TILDE_OVERLAY;
					}
				}
				/*
				 * Define the text layout.
				 */
				static ATSUTextLayout textLayout;
				if (textLayout == NULL) {
					OSStatus err = ATSUCreateTextLayout (& textLayout);
					Melder_assert (err == 0);
				}
				uint16_t code16 [2];
				if (lc -> kar <= 0xFFFF) {
					code16 [0] = lc -> kar;
					OSStatus err = ATSUSetTextPointerLocation (textLayout, & code16 [0], kATSUFromTextBeginning, kATSUToTextEnd, 1);   // BUG: not 64-bit
					Melder_assert (err == 0);
				} else {
					MelderUtf32 kar = lc -> kar - 0x10000;
					code16 [0] = 0xD800 + (kar >> 10);
					code16 [1] = 0xDC00 + (kar & 0x3FF);
					OSStatus err = ATSUSetTextPointerLocation (textLayout, & code16 [0], kATSUFromTextBeginning, kATSUToTextEnd, 2);   // BUG: not 64-bit
					Melder_assert (err == 0);
				}
				static ATSUFontFallbacks fontFallbacks = NULL;
				if (fontFallbacks == NULL) {
					ATSUCreateFontFallbacks (& fontFallbacks);
					ATSUSetObjFontFallbacks (fontFallbacks, 0, NULL, kATSUDefaultFontFallbacks);
				}
				ATSUAttributeTag attributeTags [] = { kATSUCGContextTag, kATSULineFontFallbacksTag };
				ByteCount valueSizes [] = { sizeof (CGContextRef), sizeof (ATSUFontFallbacks) };
				ATSUAttributeValuePtr values [] = { & my macGraphicsContext, & fontFallbacks };
				ATSUSetLayoutControls (textLayout, 2, attributeTags, valueSizes, values);
				ATSUSetTransientFontMatching (textLayout, true);
				/*
				 * Set styles: font, size, colour, bold, italic.
				 */
				static ATSUStyle atsuStyle;
				if (atsuStyle == NULL) {
					ATSUCreateStyle (& atsuStyle);
				}
				Fixed fontSize = lc -> size << 16;
				Boolean boldStyle = (lc -> style & Graphics_BOLD) != 0;
				Boolean italicStyle = (lc -> style & Graphics_ITALIC) != 0;
				ATSUAttributeTag styleAttributeTags [] = { kATSUFontTag, kATSUSizeTag, kATSUColorTag, kATSUQDBoldfaceTag, kATSUQDItalicTag };
				ByteCount styleValueSizes [] = { sizeof (ATSUFontID), sizeof (Fixed), sizeof (RGBColor), sizeof (Boolean), sizeof (Boolean) };
				ATSUAttributeValuePtr styleValues [] = { & atsuiFont, & fontSize, lc -> link ? & theBlueColour : & my macColour, & boldStyle, & italicStyle };
				ATSUSetAttributes (atsuStyle, 5, styleAttributeTags, styleValueSizes, styleValues);
				ATSUSetRunStyle (textLayout, atsuStyle, 0, 1);
				ATSUTextMeasurement textBefore, textAfter, ascent, descent;
				ATSUGetUnjustifiedBounds (textLayout, kATSUFromTextBeginning, kATSUToTextEnd, & textBefore, & textAfter, & ascent, & descent);
				lc -> width = textAfter / 65536.0;
			} else {
				Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
				int normalSize = my fontSize * my resolution / 72.0;
				lc -> size = lc -> size < 100 ? (3 * normalSize + 2) / 4 : /*lc -> size > 100 ? 1.2 * normalSize :*/ normalSize;
				lc -> baseline *= 0.01 * normalSize;
				long saveFont = lc -> font.integer;   // Save!
				lc -> font.string = NULL;   // This erases font.integer!
				lc -> font.integer =
					info -> alphabet == Longchar_SYMBOL ? theSymbolFont :
					info -> alphabet == Longchar_PHONETIC ? theIpaTimesFont :
					lc -> kar == '/' ? thePalatinoFont :   /* Override Courier. */
					info -> alphabet == Longchar_DINGBATS ? theZapfDingbatsFont:
					saveFont == kGraphics_font_COURIER ? theCourierFont :
					my macFont;
				lc -> style =
					(lc -> style & Graphics_ITALIC ? italic : 0) +
					(lc -> style & Graphics_BOLD ? bold : 0);
				if (lc -> font.integer == 0 && ! ipaInited && Melder_debug != 15) {   /* SIL Doulos IPA not initialized. */
					GetFNum ("\017SILDoulos IPA93", & theIpaTimesFont);   /* May be 0. */
					ipaInited = TRUE;
					if (theIpaTimesFont != 0) {
						ipaAvailable = TRUE;
					} else {
						Melder_warning1 (L"The phonetic font SILDoulos IPA93 is not available.\nPraat will use an ugly bitmap font instead.\nSee http://www.praat.org");
					}
					lc -> font.integer = theIpaTimesFont;
				}
				if (lc -> font.integer == 0) {   /* SIL Doulos IPA not available. */
					int overstrike = ipaSerifRegular24 [info -> psEncoding - 32] [0] [0] == 'o';
					if (overstrike)
						lc -> width = 0;
					else
						lc -> width = strlen (ipaSerifRegular24 [info -> psEncoding - 32] [0]);
					lc -> code = info -> psEncoding;
				} else {
					lc -> code = info -> macEncoding;
					if (lc -> code == 0) {
						_Graphics_widechar *lc2;
						if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK) {
							info = Longchar_getInfo ('s', 'w');
							lc -> kar = info -> unicode;
							lc -> code = info -> macEncoding;
							for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
							lc2 [1]. kar = '\0';
							while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
							lc [1]. kar = UNICODE_MODIFIER_LETTER_RHOTIC_HOOK;
						} else if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE) {
							info = Longchar_getInfo ('l', ' ');
							lc -> kar = info -> unicode;
							lc -> code = info -> macEncoding;
							for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
							lc2 [1]. kar = '\0';
							while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
							lc [1]. kar = UNICODE_COMBINING_TILDE_OVERLAY;
						}
					}
					lc -> code = info -> macEncoding;
					TextFont (lc -> font.integer);
					TextFace (lc -> style);
					TextSize (lc -> size);
					lc -> width = CharWidth (lc -> code);
				}
			}
		#endif
	} else if (my postScript) {
		iam (GraphicsPostscript);
		int normalSize = (int) ((double) my fontSize * (double) my resolution / 72.0);
		Longchar_Info info = Longchar_getInfoFromNative (lc -> kar);
		int font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
				info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
				info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : lc -> font.integer;
		int style = lc -> style == Graphics_ITALIC ? Graphics_ITALIC :
			lc -> style == Graphics_BOLD || lc -> link ? Graphics_BOLD :
			lc -> style == Graphics_BOLD_ITALIC ? Graphics_BOLD_ITALIC : 0;
		if (! my fontInfos [font] [style]) {
			char *fontInfo, *secondaryFontInfo = NULL, *tertiaryFontInfo = NULL;
			if (font == kGraphics_font_COURIER) {
				fontInfo = style == Graphics_BOLD ? "Courier-Bold" :
					style == Graphics_ITALIC ? "Courier-Oblique" :
					style == Graphics_BOLD_ITALIC ? "Courier-BoldOblique" : "Courier";
				secondaryFontInfo = style == Graphics_BOLD ? "CourierNewPS-BoldMT" :
					style == Graphics_ITALIC ? "CourierNewPS-ItalicMT" :
					style == Graphics_BOLD_ITALIC ? "CourierNewPS-BoldItalicMT" : "CourierNewPSMT";
				tertiaryFontInfo = style == Graphics_BOLD ? "CourierNew-Bold" :
					style == Graphics_ITALIC ? "CourierNew-Italic" :
					style == Graphics_BOLD_ITALIC ? "CourierNew-BoldItalic" : "CourierNew";
			} else if (font == kGraphics_font_TIMES) {
				fontInfo = style == Graphics_BOLD ? "Times-Bold" :
					style == Graphics_ITALIC ? "Times-Italic" :
					style == Graphics_BOLD_ITALIC ? "Times-BoldItalic" : "Times-Roman";
				secondaryFontInfo = style == Graphics_BOLD ? "TimesNewRomanPS-BoldMT" :
					style == Graphics_ITALIC ? "TimesNewRomanPS-ItalicMT" :
					style == Graphics_BOLD_ITALIC ? "TimesNewRomanPS-BoldItalicMT" : "TimesNewRomanPSMT";
				tertiaryFontInfo = style == Graphics_BOLD ? "TimesNewRoman-Bold" :
					style == Graphics_ITALIC ? "TimesNewRoman-Italic" :
					style == Graphics_BOLD_ITALIC ? "TimesNewRoman-BoldItalic" : "TimesNewRoman";
			} else if (font == kGraphics_font_PALATINO) {
				fontInfo = style == Graphics_BOLD ? "Palatino-Bold" :
					style == Graphics_ITALIC ? "Palatino-Italic" :
					style == Graphics_BOLD_ITALIC ? "Palatino-BoldItalic" : "Palatino-Roman";
				secondaryFontInfo = style == Graphics_BOLD ? "BookAntiquaPS-BoldMT" :
					style == Graphics_ITALIC ? "BookAntiquaPS-ItalicMT" :
					style == Graphics_BOLD_ITALIC ? "BookAntiquaPS-BoldItalicMT" : "BookAntiquaPSMT";
				tertiaryFontInfo = style == Graphics_BOLD ? "BookAntiqua-Bold" :
					style == Graphics_ITALIC ? "BookAntiqua-Italic" :
					style == Graphics_BOLD_ITALIC ? "BookAntiqua-BoldItalic" : "BookAntiqua";
			} else if (font == kGraphics_font_IPATIMES) {
				if (my includeFonts && ! my loadedXipa) {
					char **p;
					for (p = & ipaSerifRegularPS [0]; *p; p ++)
						my printf (my file, "%s", *p);
					my loadedXipa = TRUE;
				}
				fontInfo = my useSilipaPS ?
					(style == Graphics_BOLD || style == Graphics_BOLD_ITALIC ? "SILDoulosIPA93Bold" : "SILDoulosIPA93Regular") :
					"TeX-xipa10-Praat-Regular";
			} else if (font == kGraphics_font_SYMBOL) {
				fontInfo = "Symbol";
			} else if (font == kGraphics_font_DINGBATS) {
				fontInfo = "ZapfDingbats";
			} else {
				fontInfo = style == Graphics_BOLD ? "Helvetica-Bold" :
					style == Graphics_ITALIC ? "Helvetica-Oblique" :
					style == Graphics_BOLD_ITALIC ? "Helvetica-BoldOblique" : "Helvetica";
				secondaryFontInfo = style == Graphics_BOLD ? "Arial-BoldMT" :
					style == Graphics_ITALIC ? "Arial-ItalicMT" :
					style == Graphics_BOLD_ITALIC ? "Arial-BoldItalicMT" : "ArialMT";
				tertiaryFontInfo = style == Graphics_BOLD ? "Arial-Bold" :
					style == Graphics_ITALIC ? "Arial-Italic" :
					style == Graphics_BOLD_ITALIC ? "Arial-BoldItalic" : "Arial";
			}
			my fontInfos [font] [style] = Melder_malloc (char, 100);
			if (font == kGraphics_font_IPATIMES || font == kGraphics_font_SYMBOL || font == kGraphics_font_DINGBATS) {
				strcpy (my fontInfos [font] [style], fontInfo);
			} else {
				sprintf (my fontInfos [font] [style], "%s-Praat", fontInfo);
				if (thePrinter. fontChoiceStrategy == kGraphicsPostscript_fontChoiceStrategy_LINOTYPE) {
					my printf (my file, "/%s /%s-Praat PraatEncode\n", fontInfo, fontInfo);
				} else if (thePrinter. fontChoiceStrategy == kGraphicsPostscript_fontChoiceStrategy_MONOTYPE) {
					my printf (my file, "/%s /%s-Praat PraatEncode\n", tertiaryFontInfo, fontInfo);
				} else if (thePrinter. fontChoiceStrategy == kGraphicsPostscript_fontChoiceStrategy_PS_MONOTYPE) {
					my printf (my file, "/%s /%s-Praat PraatEncode\n", secondaryFontInfo, fontInfo);
				} else {
					/* Automatic font choice strategy. */
					if (secondaryFontInfo != NULL) {
						my printf (my file,
							"/%s /Font resourcestatus\n"
							"{ pop pop /%s /%s-Praat PraatEncode }\n"
							"{ /%s /%s-Praat PraatEncode }\n"
							"ifelse\n",
							secondaryFontInfo, secondaryFontInfo, fontInfo, fontInfo, fontInfo);
					} else {
						my printf (my file, "/%s /%s-Praat PraatEncode\n", fontInfo, fontInfo);
					}
				}
			}
		}
		lc -> font.integer = 0;
		lc -> font.string = my fontInfos [font] [style];

		/*
		 * Convert size and baseline information to device coordinates.
		 */
		lc -> size *= normalSize * 0.01;
		lc -> baseline *= normalSize * 0.01;

		if (font == kGraphics_font_COURIER) {
			lc -> width = 600;   /* Courier. */
		} else if (style == 0) {
			if (font == kGraphics_font_TIMES) lc -> width = info -> ps.times;
			else if (font == kGraphics_font_HELVETICA) lc -> width = info -> ps.helvetica;
			else if (font == kGraphics_font_PALATINO) lc -> width = info -> ps.palatino;
			else if (font == kGraphics_font_SYMBOL) lc -> width = info -> ps.times;
			else if (my useSilipaPS) lc -> width = info -> ps.timesItalic;
			else lc -> width = info -> ps.times;   /* XIPA. */
		} else if (style == Graphics_BOLD) {
			if (font == kGraphics_font_TIMES) lc -> width = info -> ps.timesBold;
			else if (font == kGraphics_font_HELVETICA) lc -> width = info -> ps.helveticaBold;
			else if (font == kGraphics_font_PALATINO) lc -> width = info -> ps.palatinoBold;
			else if (font == kGraphics_font_SYMBOL) lc -> width = info -> ps.times;
			else if (my useSilipaPS) lc -> width = info -> ps.timesBoldItalic;
			else lc -> width = info -> ps.times;   /* Symbol, IPA. */
		} else if (style == Graphics_ITALIC) {
			if (font == kGraphics_font_TIMES) lc -> width = info -> ps.timesItalic;
			else if (font == kGraphics_font_HELVETICA) lc -> width = info -> ps.helvetica;
			else if (font == kGraphics_font_PALATINO) lc -> width = info -> ps.palatinoItalic;
			else if (font == kGraphics_font_SYMBOL) lc -> width = info -> ps.times;
			else if (my useSilipaPS) lc -> width = info -> ps.timesItalic;
			else lc -> width = info -> ps.times;   /* Symbol, IPA. */
		} else if (style == Graphics_BOLD_ITALIC) {
			if (font == kGraphics_font_TIMES) lc -> width = info -> ps.timesBoldItalic;
			else if (font == kGraphics_font_HELVETICA) lc -> width = info -> ps.helveticaBold;
			else if (font == kGraphics_font_PALATINO) lc -> width = info -> ps.palatinoBoldItalic;
			else if (font == kGraphics_font_SYMBOL) lc -> width = info -> ps.times;
			else if (my useSilipaPS) lc -> width = info -> ps.timesBoldItalic;
			else lc -> width = info -> ps.times;   /* Symbol, IPA. */
		}
		lc -> width *= lc -> size / 1000.0;
		lc -> code = font == kGraphics_font_IPATIMES && my useSilipaPS ? info -> macEncoding : info -> psEncoding;
		if (lc -> code == 0) {
			_Graphics_widechar *lc2;
			if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK) {
				info = Longchar_getInfo ('s', 'w');
				lc -> kar = info -> unicode;
				lc -> code = info -> macEncoding;
				lc -> width = info -> ps.timesItalic * lc -> size / 1000.0;
				for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
				lc2 [1]. kar = '\0';
				while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
				lc [1]. kar = UNICODE_MODIFIER_LETTER_RHOTIC_HOOK;
			} else if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE) {
				info = Longchar_getInfo ('l', ' ');
				lc -> code = info -> macEncoding;
				lc -> kar = info -> unicode;
				lc -> width = info -> ps.timesItalic * lc -> size / 1000.0;
				for (lc2 = lc + 1; lc2 -> kar != '\0'; lc2 ++) { }
				lc2 [1]. kar = '\0';
				while (lc2 - lc > 0) { lc2 [0] = lc2 [-1]; lc2 --; }
				lc [1]. kar = UNICODE_COMBINING_TILDE_OVERLAY;
			}
		}
	}
}

static void charDraw (I, int xDC, int yDC, _Graphics_widechar *lc,
	const wchar_t *codes, const char *codes8, const MelderUtf16 *codes16, int nchars, int width)
{
	iam (Graphics);
	//Melder_casual ("nchars %d first %d %c rightToLeft %d", nchars, lc->kar, lc -> kar, lc->rightToLeft);
	if (my postScript) {
		iam (GraphicsPostscript);
		bool onlyRegular = lc -> font.string [0] == 'S' ||
			(lc -> font.string [0] == 'T' && lc -> font.string [1] == 'e');   /* Symbol & SILDoulos ! */
		int slant = (lc -> style & Graphics_ITALIC) && onlyRegular;
		int thick = (lc -> style & Graphics_BOLD) && onlyRegular;
		if (lc -> font.string != my lastFid || lc -> size != my lastSize)
			my printf (my file, my languageLevel == 1 ? "/%s %d FONT\n" : "/%s %d selectfont\n",
				my lastFid = lc -> font.string, my lastSize = lc -> size);
		if (lc -> link) my printf (my file, "0 0 1 setrgbcolor\n");
		for (int i = -3; i <= 3; i ++) {
			if (i != 0 && ! thick) continue;
			my printf (my file, "%d %d M ", xDC + i, yDC);
			if (my textRotation || slant) {
				my printf (my file, "gsave currentpoint translate ");
				if (my textRotation)
					my printf (my file, "%.6g rotate 0 0 M\n", (double) my textRotation);
				if (slant)
					my printf (my file, "[1 0 0.25 1 0 0] concat 0 0 M\n");
			}
			my printf (my file, "(");
			const char *kars = codes8;
			while (*kars) {
				if (*kars == '(' || *kars == ')' || *kars == '\\') {
					my printf (my file, "\\%c", *kars);
				} else if (*kars >= 32 && *kars <= 126) {
					my printf (my file, "%c", *kars);
				} else {
					my printf (my file, "\\%d%d%d", *(unsigned char*)kars / 64,
						(*(unsigned char*)kars % 64) / 8, *(unsigned char*)kars % 8);
				}
				kars ++;
			}
			my printf (my file, ") show\n");
			if (my textRotation || slant)
				my printf (my file, "grestore\n");
		}
		if (lc -> link) my printf (my file, "0 0 0 setrgbcolor\n");
	} else if (my screen) {
		iam (GraphicsScreen);
		#if cairo
			if (my duringXor) {
			} else {
				if (my cr == NULL) return;
				// TODO!
			}
			int font = lc -> font.integer;
			int needBitmappedIPA = 0;
		#elif xwin
			Font font = (Font) lc -> font.integer;
			int needBitmappedIPA = (long) font == IPA_ID;
		#elif win
			int font = lc -> font.integer;
			int needBitmappedIPA = font == kGraphics_font_IPATIMES && ! ipaAvailable;
		#elif mac
			if (my useQuartz) {
				ATSFontRef atsuiFont = (ATSFontRef) lc -> font.integer;
				Melder_assert (atsuiFont != 0);
				/*
				 * Define the text layout.
				 */
				static ATSUTextLayout theAtsuiTextLayout;
				if (theAtsuiTextLayout == NULL) {
					OSStatus err = ATSUCreateTextLayout (& theAtsuiTextLayout);
					Melder_assert (err == 0);
				}
				bool hasHighUnicodeValues = false;
				for (long i = 0; i < nchars; i ++) {
					hasHighUnicodeValues |= codes [i] > 0xFFFF;
				}
				if (hasHighUnicodeValues) {
					nchars = wcslen_utf16 (codes, 0);
					OSStatus err = ATSUSetTextPointerLocation (theAtsuiTextLayout, Melder_peekWcsToUtf16 (codes), kATSUFromTextBeginning, kATSUToTextEnd, nchars);
					Melder_assert (err == 0);
				} else {
					OSStatus err = ATSUSetTextPointerLocation (theAtsuiTextLayout, codes16, kATSUFromTextBeginning, kATSUToTextEnd, nchars);
					Melder_assert (err == 0);
				}
				static ATSUFontFallbacks fontFallbacks = NULL;
				if (fontFallbacks == NULL) {
					ATSUCreateFontFallbacks (& fontFallbacks);
					ATSUSetObjFontFallbacks (fontFallbacks, 0, NULL, kATSUDefaultFontFallbacks);
					ATSUSetObjFontFallbacks (fontFallbacks, 1, & theArabicAtsuiFont, kATSUSequentialFallbacksPreferred);
				}
				ATSUAttributeTag attributeTags [] = { kATSUCGContextTag, kATSULineFontFallbacksTag };
				ByteCount valueSizes [] = { sizeof (CGContextRef), sizeof (ATSUFontFallbacks) };
				ATSUAttributeValuePtr values [] = { & my macGraphicsContext, & fontFallbacks };
				ATSUSetLayoutControls (theAtsuiTextLayout, 2, attributeTags, valueSizes, values);
				ATSUSetTransientFontMatching (theAtsuiTextLayout, true);
				/*
				 * Set styles: font, size, colour, bold, italic.
				 */
				static ATSUStyle theAtsuStyle;
				if (theAtsuStyle == NULL) {
					ATSUCreateStyle (& theAtsuStyle);
				}
				Fixed fontSize = lc -> size << 16;
				Boolean boldStyle = (lc -> style & Graphics_BOLD) != 0;
				Boolean italicStyle = (lc -> style & Graphics_ITALIC) != 0;
				Fract kerningOff = FloatToFract (1.0);
				ATSUAttributeTag styleAttributeTags [] = { kATSUFontTag, kATSUSizeTag, kATSUColorTag, kATSUQDBoldfaceTag, kATSUQDItalicTag, kATSUKerningInhibitFactorTag };
				ByteCount styleValueSizes [] = { sizeof (ATSUFontID), sizeof (Fixed), sizeof (RGBColor), sizeof (Boolean), sizeof (Boolean), sizeof (Fract) };
				ATSUAttributeValuePtr styleValues [] = { & atsuiFont, & fontSize,
					lc -> link ? & theBlueColour : my duringXor ? & theWhiteColour : & my macColour, & boldStyle, & italicStyle, & kerningOff };
				ATSUSetAttributes (theAtsuStyle, 6, styleAttributeTags, styleValueSizes, styleValues);
				ATSUSetRunStyle (theAtsuiTextLayout, theAtsuStyle, 0, nchars);
				/*
				 * Draw.
				 */
				CGContextSaveGState (my macGraphicsContext);
				CGContextTranslateCTM (my macGraphicsContext, xDC, yDC);

				if (my yIsZeroAtTheTop) CGContextScaleCTM (my macGraphicsContext, 1.0, -1.0);
				CGContextRotateCTM (my macGraphicsContext, my textRotation * NUMpi / 180.0);
				if (my duringXor) {
					CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeDifference);
					CGContextSetAllowsAntialiasing (my macGraphicsContext, false);
					OSStatus err = ATSUDrawText (theAtsuiTextLayout, kATSUFromTextBeginning, kATSUToTextEnd, 0, 0);
					Melder_assert (err == 0);
					CGContextSetBlendMode (my macGraphicsContext, kCGBlendModeNormal);
					CGContextSetAllowsAntialiasing (my macGraphicsContext, true);
				} else {
					OSStatus err = ATSUDrawText (theAtsuiTextLayout, kATSUFromTextBeginning, kATSUToTextEnd, 0, 0);
					Melder_assert (err == 0);
				}
				CGContextRestoreGState (my macGraphicsContext);
				return;
			}
			long font = lc -> font.integer;
			int needBitmappedIPA = font == 0;
		#endif
		/*
		 * First handle the most common case: text without rotation.
		 */
		if (my textRotation == 0.0) {
			/*
			 * Unrotated text could be a link. If so, it will be blue.
			 */
			#if cairo
				if (my duringXor) {
				} else {
					if (lc -> link) _Graphics_setColour (me, Graphics_BLUE);
				}
			#elif xwin
				if (lc -> link) XSetForeground (my display, my gc, xwinColour_BLUE);
			#elif win
			#elif mac
				if (lc -> link) ForeColor (blueColor);
			#endif
			/*
			 * The most common case: a native font.
			 */
			if (! needBitmappedIPA) {
				#if cairo
					if (my duringXor) {
						static GdkFont *font = NULL;
						if (font == NULL) {
							font = gdk_font_load ("-*-courier-medium-r-normal--*-120-*-*-*-*-iso8859-1");
						}
						gdk_draw_text_wc (my window, font, my gc, xDC, yDC, codes, nchars);
					} else {
						enum _cairo_font_slant slant   = (lc -> style & Graphics_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL);
						enum _cairo_font_weight weight = (lc -> style & Graphics_BOLD   ? CAIRO_FONT_WEIGHT_BOLD  : CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size (my cr, lc -> size);
						switch (font) {
							case kGraphics_font_HELVETICA: cairo_select_font_face (my cr, "Helvetica", slant, weight); break;
							case kGraphics_font_TIMES:     cairo_select_font_face (my cr, "Times", slant, weight); break;
							case kGraphics_font_COURIER:   cairo_select_font_face (my cr, "Courier", slant, weight); break;
							case kGraphics_font_PALATINO:  cairo_select_font_face (my cr, "Palatino", slant, weight); break;
							case kGraphics_font_SYMBOL:    cairo_select_font_face (my cr, "Symbol", slant, weight); break;
							case kGraphics_font_IPATIMES:  cairo_select_font_face (my cr, "IPA Times", slant, weight); break;
							case kGraphics_font_DINGBATS:  cairo_select_font_face (my cr, "Dingbats", slant, weight); break;
							default:                       cairo_select_font_face (my cr, "Sans", slant, weight); break;
						}
						cairo_move_to (my cr, xDC, yDC);
						cairo_show_text (my cr, Melder_peekWcsToUtf8 (codes));
					}
				#elif xwin
					XSetFont (my display, my text.gc, font);
					XDrawString (my display, my text.window, my text.gc, xDC, yDC, (char *) codes8, nchars);
				#elif win
					if (my duringXor) {
						int descent = (1.0/216) * my fontSize * my resolution;
						int ascent = (1.0/72) * my fontSize * my resolution;
						int maxWidth = 800, maxHeight = 200;
						int baseline = 100, top = baseline - ascent - 1, bottom = baseline + descent + 1;
						static int inited = 0;
						static HDC dc;
						static HBITMAP bitmap;
						if (! inited) {
							dc = CreateCompatibleDC (my dc);
							bitmap = CreateCompatibleBitmap (my dc, maxWidth, maxHeight);
							SelectBitmap (dc, bitmap);
							SetBkMode (dc, TRANSPARENT);   // not the default!
							SelectPen (dc, GetStockPen (BLACK_PEN));
							SelectBrush (dc, GetStockBrush (BLACK_BRUSH));
							SetTextAlign (dc, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
							inited = 1;
						}
						width += 4;   // For slant.
						Rectangle (dc, 0, top, width, bottom);
						SelectFont (dc, my printer || my metafile ? printerFonts [font] [lc -> size] [lc -> style] :
							screenFonts [font] [lc -> size] [lc -> style]);
						SetTextColor (dc, my foregroundColour);
						TextOutW (dc, 0, baseline, codes16, nchars);
						BitBlt (my dc, xDC, yDC - ascent, width, bottom - top, dc, 0, top, SRCINVERT);
					} else {
						SelectPen (my dc, my pen), SelectBrush (my dc, my brush);
						if (lc -> link) SetTextColor (my dc, RGB (0, 0, 255)); else SetTextColor (my dc, my foregroundColour);
						SelectFont (my dc, my printer || my metafile ? printerFonts [font] [lc -> size] [lc -> style] :
							screenFonts [font] [lc -> size] [lc -> style]);
						TextOutW (my dc, xDC, yDC, codes16, nchars);
						if (lc -> link) SetTextColor (my dc, my foregroundColour);
						SelectPen (my dc, GetStockPen (BLACK_PEN)), SelectBrush (my dc, GetStockBrush (NULL_BRUSH));
					}
				#elif mac
					MoveTo (xDC, yDC);
					TextFont (lc -> font.integer);
					TextFace (lc -> style);
					TextSize (lc -> size);
					DrawText (codes8, 0, nchars);
				#endif
			} else {
				/*
				 * A non-rotated bitmap IPA font.
				 */
				for (int ichar = 0; ichar < nchars; ichar ++) {
					int irow, icol, code = ((unsigned char *) codes8) [ichar], ncol;
					int overstrike = ipaSerifRegular24 [code - 32] [0] [0] == 'o';
					ncol = strlen (ipaSerifRegular24 [code - 32] [0]);
					if (overstrike) 
						xDC -= 10;
					for (irow = 0; irow < 24; irow ++) {
						char *row = ipaSerifRegular24 [code - 32] [irow];
						int jrow = yDC - 18 + irow;
						for (icol = 0; icol < ncol; icol ++) {
							if (row [icol] == '#')
								#if cairo
									if (my duringXor) {
									} else {
										cairo_move_to (my cr, xDC, jrow);
										cairo_line_to (my cr, xDC, jrow);
									}
								#elif xwin
									XDrawPoint (my display, my window, my gc, xDC, jrow);
								#elif win
									SetPixel (my dc, xDC, jrow, my foregroundColour);
								#elif mac
									MoveTo (xDC, jrow), LineTo (xDC, jrow);
								#endif
							xDC ++;
						}
						row ++;
						xDC -= ncol;
					}
					xDC += overstrike ? 10 : ncol;
				}
				#if cairo
					if (my duringXor) {
					} else {
						cairo_stroke (my cr);
					}
				#endif
			}
			/*
			 * Back to normal colour.
			 */

			#if cairo
				if (my duringXor) {
				} else {
					if (lc -> link) _Graphics_setColour (me, my colour);
				}
			#elif xwin
				if (lc -> link) _Graphics_setColour (me, my colour);
			#elif win
			#elif mac
				if (lc -> link) RGBForeColor (& my macColour);
			#endif
		} else {
			/*
			 * Rotated text. This time, the easiest case is the bitmap IPA font;
			 */
			if (needBitmappedIPA) {
				int ichar, dx1 = 0;
				double cosa, sina;
				if (my textRotation == 90.0f) { cosa = 0.0; sina = 1.0; }
				else if (my textRotation == 270.0f) { cosa = 0.0; sina = -1.0; }
				else { double a = my textRotation * NUMpi / 180.0; cosa = cos (a); sina = sin (a); }
				for (ichar = 0; ichar < nchars; ichar ++) {
					int irow, icol, code = ((unsigned char *) codes8) [ichar];
					int ncol = strlen (ipaSerifRegular24 [code - 32] [0]);
					for (irow = 0; irow < 24; irow ++) {
						char *row = ipaSerifRegular24 [code - 32] [irow];
						double dy1 = irow - 18;
						for (icol = 0; icol < ncol; icol ++) {
							if (row [icol] == '#') {
								int xp = xDC + (int) (cosa * dx1 + sina * dy1);
								int yp = yDC - (int) (sina * dx1 - cosa * dy1);
								#if cairo
									if (my duringXor) {
									} else {
										cairo_move_to (my cr, xp, yp);
										cairo_line_to (my cr, xp, yp);
									}
								#elif xwin
									XDrawPoint (my display, my window, my gc, xp, yp);
								#elif win
									SetPixel (my dc, xp, yp, my foregroundColour);
								#elif mac
									MoveTo (xp, yp), LineTo (xp, yp);
								#endif
							}
							dx1 ++;
						}
						row ++;
						dx1 -= ncol;
					}
					dx1 += ncol;
				}
				#if cairo
					if (my duringXor) {
					} else {
						cairo_stroke (my cr);
					}
				#endif
			} else {
				/*
				 * Rotated native font.
				 */
				#if cairo
					enum _cairo_font_slant slant   = (lc -> style & Graphics_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL);
					enum _cairo_font_weight weight = (lc -> style & Graphics_BOLD   ? CAIRO_FONT_WEIGHT_BOLD  : CAIRO_FONT_WEIGHT_NORMAL);
					cairo_set_font_size (my cr, lc -> size);
					switch (font) {
						case kGraphics_font_HELVETICA: cairo_select_font_face (my cr, "Helvetica", slant, weight); break;
						case kGraphics_font_TIMES:     cairo_select_font_face (my cr, "Times", slant, weight); break;
						case kGraphics_font_COURIER:   cairo_select_font_face (my cr, "Courier", slant, weight); break;
						case kGraphics_font_PALATINO:  cairo_select_font_face (my cr, "Palatino", slant, weight); break;
						case kGraphics_font_SYMBOL:    cairo_select_font_face (my cr, "Symbol", slant, weight); break;
						case kGraphics_font_IPATIMES:  cairo_select_font_face (my cr, "IPA Times", slant, weight); break;
						case kGraphics_font_DINGBATS:  cairo_select_font_face (my cr, "Dingbats", slant, weight); break;
						default:                       cairo_select_font_face (my cr, "Sans", slant, weight); break;
					}
					cairo_save (my cr);
					cairo_translate (my cr, xDC, yDC);
					//cairo_scale (my cr, 1, -1);
					cairo_rotate (my cr, - my textRotation * NUMpi / 180.0);
					cairo_move_to (my cr, 0, 0);
					cairo_show_text (my cr, Melder_peekWcsToUtf8 (codes));
					cairo_restore (my cr);
					return;
				#elif win
					if (1) {
						SelectPen (my dc, my pen), SelectBrush (my dc, my brush);
						if (lc -> link) SetTextColor (my dc, RGB (0, 0, 255)); else SetTextColor (my dc, my foregroundColour);
						SelectFont (my dc, my printer || my metafile ? printerFonts [font] [lc -> size] [lc -> style] :
							screenFonts [font] [lc -> size] [lc -> style]);
						int restore = SaveDC (my dc);
						SetGraphicsMode (my dc, GM_ADVANCED);
						double a = my textRotation * NUMpi / 180.0, cosa = cos (a), sina = sin (a);
						XFORM rotate = { cosa, - sina, sina, cosa, 0, 0 };
						ModifyWorldTransform (my dc, & rotate, MWT_RIGHTMULTIPLY);
						XFORM translate = { 1, 0, 0, 1, xDC, yDC };
						ModifyWorldTransform (my dc, & translate, MWT_RIGHTMULTIPLY);
						TextOutW (my dc, 0 /*xDC*/, 0 /*yDC*/, codes16, nchars);
						RestoreDC (my dc, restore);
						if (lc -> link) SetTextColor (my dc, my foregroundColour);
						SelectPen (my dc, GetStockPen (BLACK_PEN)), SelectBrush (my dc, GetStockBrush (NULL_BRUSH));
						return;
					}
				#endif
				int ascent = (1.0/72) * my fontSize * my resolution;
				int descent = (1.0/216) * my fontSize * my resolution;
				int ix, iy /*, baseline = 1 + ascent * 2*/;
				double cosa, sina;
				#if xwin
					unsigned int height = 2 + (ascent + descent) * 2;
					if (! rotate.pixmap) {
						/* Create an off-screen bitmap in the "server". */
						rotate.pixmap = XCreatePixmap (my display, my rootWindow, 1000, 200, 1);
						if (! rotate.pixmap) return;
						if (! rotate.gc) rotate.gc = XCreateGC (my display, rotate.pixmap, 0, NULL);
						rotate.image = XGetImage (my display, rotate.pixmap, 0, 0, 1000, 200, AllPlanes, XYPixmap);
					}
				#elif win
					int maxWidth = 1000, maxHeight = 600;   /* BUG: printer??? */
					int baseline = maxHeight / 4, top = baseline - ascent - 1, bottom = baseline + descent + 1;
					static int inited = 0;
					static HDC dc;
					static HBITMAP bitmap;
					if (! inited) {
						dc = CreateCompatibleDC (my dc);
						bitmap = CreateBitmap (/*my dc,*/ maxWidth, maxHeight, 1, 1, NULL);
						SelectBitmap (dc, bitmap);
						inited = 1;
					}
				#elif mac
					int maxWidth = 1000, maxHeight = 600;
					int baseline = maxHeight / 4, top = baseline - ascent - 1, bottom = baseline + descent + 1;
					static int inited = 0;
					static Rect rect;
					static GWorldPtr offscreenWorld;
					CGrafPtr savePort;
					GDHandle saveDevice;
					//PixMapHandle offscreenPixMap;
					/*unsigned char *offscreenPixels;*/
					SetPort (my macPort);
					GetGWorld (& savePort, & saveDevice);
					Melder_assert (savePort == (CGrafPtr) my macPort);
					if (! inited) {
						SetRect (& rect, 0, 0, maxWidth, maxHeight);
						NewGWorld (& offscreenWorld, 24, & rect, NULL, NULL, keepLocal | (1L << 8));
						inited = 1;
					}
					SetGWorld (offscreenWorld, NULL);
					//offscreenPixMap = GetGWorldPixMap (offscreenWorld);
					//LockPixels (offscreenPixMap);
					/*offscreenPixels = (unsigned char *) GetPixBaseAddr (offscreenPixMap);*/
				#endif
				width += 4;   /* Leave room for slant. */
				#if xwin
					/* Clear the entire pixmap with "white". */
					XSetForeground (my display, rotate.gc, 0);
					XFillRectangle (my display, rotate.pixmap, rotate.gc, 0, 0, width, 200);

					/* Draw the text in "black". */
					XSetForeground (my display, rotate.gc, 1);
					XSetFont (my display, rotate.gc, font);
					XDrawString (my display, rotate.pixmap, rotate.gc, 0, 100, (char *) codes8, nchars);

					/* Copy bitmap from "server" to "client". */
					XGetSubImage (my display, rotate.pixmap, 0, 100 - ascent, width, height, AllPlanes, XYPixmap,
						rotate.image, 0, 0);
				#elif win
					SelectPen (dc, GetStockPen (WHITE_PEN));
					SelectBrush (dc, GetStockBrush (WHITE_BRUSH));
					SetTextAlign (dc, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
					Rectangle (dc, 0, top, maxWidth, bottom + 1);
					//Rectangle (dc, 0, 0, maxWidth, maxHeight);
					SelectPen (dc, GetStockPen (BLACK_PEN));
					SelectBrush (dc, GetStockBrush (NULL_BRUSH));
					SelectFont (dc, my printer || my metafile ? printerFonts [font] [lc -> size] [lc -> style] :
							screenFonts [font] [lc -> size] [lc -> style]);
					TextOutW (dc, 0, baseline, codes16, nchars);
				#elif mac
					SetRect (& rect, 0, top, width, bottom + 1);
					EraseRect (& rect);
					MoveTo (0, baseline);
					TextFont (lc -> font.integer);
					TextFace (lc -> style);
					TextSize (lc -> size);
					DrawText (codes8, 0, nchars);
				#endif
				if (my textRotation == 90.0) { cosa = 0.0; sina = 1.0; }
				else if (my textRotation == 270.0) { cosa = 0.0; sina = -1.0; }
				else { double a = my textRotation * NUMpi / 180.0; cosa = cos (a); sina = sin (a); }
				for (ix = 0; ix < width; ix ++) {
					double dx1 = ix;
					#if xwin
						for (iy = 0; iy < height; iy ++) {
							Pixel pixel = XGetPixel (rotate.image, ix, iy);
							if (pixel == 1) {   /* "Black". */
								double dy1 = iy - ascent;   /* Translate, rotate, translate. */
								int xp = xDC + (int) (cosa * dx1 + sina * dy1);
								int yp = yDC - (int) (sina * dx1 - cosa * dy1);
								XDrawPoint (my display, my window, my gc, xp, yp);
							}
						}
					#elif win
						for (iy = top; iy <= bottom; iy ++) {
							if (GetPixel (dc, ix, iy) == RGB (0, 0, 0)) {   /* Black? */
								int dy1 = iy - baseline;   /* Translate, rotate, translate. */
								int xp = xDC + (int) (cosa * dx1 + sina * dy1);
								int yp = yDC - (int) (sina * dx1 - cosa * dy1);
								SetPixel (my dc, xp, yp, my foregroundColour);
							}
						}
					#elif mac
						for (iy = top; iy <= bottom; iy ++) {
							if (GetPixel (ix, iy)) {   /* Black? */
								int dy1 = iy - baseline;   /* Translate, rotate, translate. */
								int xp = xDC + (int) (cosa * dx1 + sina * dy1);
								int yp = yDC - (int) (sina * dx1 - cosa * dy1);
								SetGWorld (savePort, saveDevice);
								SetPort (my macPort);
								MoveTo (xp, yp); LineTo (xp, yp);   /* Draw one pixel. */
								SetGWorld (offscreenWorld, NULL);
							}
						}
					#endif
				}
				#if mac
					SetGWorld (savePort, saveDevice);
					//UnlockPixels (offscreenPixMap);
					/*DisposeGWorld (offscreenWorld);*/
				end:
					SetPort (my macPort);   /* Superfluous? */
				#endif
			}
		}
	}
}

static void initText (I) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if mac
			if (my useQuartz) {
				// nothing
			} else {
				if (my drawingArea) GuiMac_clipOn (my drawingArea);
				SetPort (my macPort);
				if (my macColour.red != 0 || my macColour.green != 0 || my macColour.blue != 0) RGBForeColor (& my macColour);
				switch (my font) {
					case kGraphics_font_HELVETICA: my macFont = theHelveticaFont; break;   /* geneva? */
					case kGraphics_font_TIMES: my macFont = theTimesFont; break;
					case kGraphics_font_COURIER: my macFont = theCourierFont; break;
					case kGraphics_font_PALATINO: my macFont = thePalatinoFont; break;
					default: break;
				}
			}
		#else
			(void) me;
		#endif
	}
}

static void exitText (I) {
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if mac
			if (my useQuartz) {
				// nothing
			} else {
				motif_mac_defaultFont ();
				if (my macColour.red != 0 || my macColour.green != 0 || my macColour.blue != 0) RGBForeColor (& theBlackColour);
				if (my drawingArea) GuiMac_clipOff ();
			}
		#else
			(void) me;
		#endif
	}
}

#define MAX_LINK_LENGTH  300

static long bufferSize;
static _Graphics_widechar *widechar;
static wchar_t *charCodes;
static char *charCodes8;
static MelderUtf16 *charCodes16;
static int initBuffer (const wchar_t *txt) {
	long sizeNeeded = wcslen (txt) + 1;   /* It is true that some characters are split into two, but all of these are backslash sequences. */
	if (sizeNeeded > bufferSize) {
		sizeNeeded += sizeNeeded / 2 + 100;
		Melder_free (widechar);
		Melder_free (charCodes);
		Melder_free (charCodes8);
		Melder_free (charCodes16);
		if (! (widechar = Melder_calloc (_Graphics_widechar, sizeNeeded)))
			{ bufferSize = 0; Melder_flushError (NULL); return 0; }
		if (! (charCodes = Melder_calloc (wchar_t, sizeNeeded)))
			{ bufferSize = 0; Melder_flushError (NULL); return 0; }
		if (! (charCodes8 = Melder_calloc (char, sizeNeeded)))
			{ bufferSize = 0; Melder_flushError (NULL); return 0; }
		if (! (charCodes16 = Melder_calloc (MelderUtf16, sizeNeeded)))
			{ bufferSize = 0; Melder_flushError (NULL); return 0; }
		bufferSize = sizeNeeded;
	}
	return 1;
}

static int numberOfLinks = 0;
static Graphics_Link links [100];    /* A maximum of 100 links per string. */

static void charSizes (Graphics me, _Graphics_widechar string []) {
	/*
	 * Measure the size of each character.
	 */
	_Graphics_widechar *character;
	for (character = string; character -> kar > '\t'; character ++)
		charSize (me, character);
	/*
	 * Each character has been garnished with information about the character's width.
	 * Make a correction for systems that make slanted characters overlap the character box to their right.
	 * We must do this after the previous loop, because we query the size of the *next* character.
	 *
	 * Keep this in SYNC with psTextWidth.
	 */
	for (character = string; character -> kar > '\t'; character ++) {
		if ((character -> style & Graphics_ITALIC) != 0) {
			_Graphics_widechar *nextCharacter = character + 1;
			if (nextCharacter -> kar <= '\t') {
				character -> width += SLANT_CORRECTION / 72 * my fontSize * my resolution;
			} else if (((nextCharacter -> style & Graphics_ITALIC) == 0 && nextCharacter -> baseline >= character -> baseline)
				|| (character -> baseline == 0 && nextCharacter -> baseline > 0))
			{
				if (nextCharacter -> kar == '.' || nextCharacter -> kar == ',')
					character -> width += SLANT_CORRECTION / 144 * my fontSize * my resolution;
				else
					character -> width += SLANT_CORRECTION / 72 * my fontSize * my resolution;
			}
		}
	}
}

/*
 * The routine textWidth determines the fractional width of a text, in device coordinates.
 */
static double textWidth (_Graphics_widechar string []) {
	_Graphics_widechar *character;
	double width = 0;
	for (character = string; character -> kar > '\t'; character ++)
		width += character -> width;
	return width;
}

static void drawOneCell (Graphics me, int xDC, int yDC, _Graphics_widechar lc []) {
	int nchars = 0;
	double width = textWidth (lc), dx, dy;
	/*
	 * We must continue even if width is zero (for adjusting textY).
	 */
	_Graphics_widechar *plc, *lastlc;
	int inLink = 0;
	switch (my horizontalTextAlignment) {
		case Graphics_LEFT:      dx = 2; break;
		case Graphics_CENTRE:    dx = - width / 2; break;
		case Graphics_RIGHT:     dx = width ? - width - 1 : 0; break;   /* If width is zero, do not step left. */
		default:                 dx = 2; break;
	}
	switch (my verticalTextAlignment) {
		case Graphics_BOTTOM:    dy = (0.4/72) * my fontSize * my resolution; break;
		case Graphics_HALF:      dy = (-0.3/72) * my fontSize * my resolution; break;
		case Graphics_TOP:       dy = (-1.0/72) * my fontSize * my resolution; break;
		case Graphics_BASELINE:  dy = 0; break;
		default:                 dy = 0; break;
	}
	#if mac
		if (my screen && ((GraphicsScreen) me) -> useQuartz) {
			GraphicsQuartz_initDraw ((GraphicsScreen) me);
		}
	#endif
	if (my textRotation) {
		double xbegin = dx, x = xbegin, cosa, sina;
		if (my textRotation == 90.0f) { cosa = 0.0; sina = 1.0; }
		else if (my textRotation == 270.0f) { cosa = 0.0; sina = -1.0; }
		else { double a = my textRotation * NUMpi / 180.0; cosa = cos (a); sina = sin (a); }
		for (plc = lc; plc -> kar > '\t'; plc ++) {
			_Graphics_widechar *next = plc + 1;
			charCodes [nchars] = plc -> code;   /* Buffer... */
			charCodes8 [nchars] = plc -> code;   /* Buffer... */
			charCodes16 [nchars ++] = plc -> code;   /* Buffer... */
			x += plc -> width;
			/*
			 * We can draw stretches of characters:
			 * they have different styles, baselines, sizes, or fonts,
			 * or if there is a break between them,
			 * or if we cannot rotate multiple characters,
			 * which is the case on bitmap printers.
			 */
			if (next->kar < ' ' || next->style != plc->style ||
				next->baseline != plc->baseline || next->size != plc->size ||
				next->font.integer != plc->font.integer || next->font.string != plc->font.string ||
				next->rightToLeft != plc->rightToLeft ||
				(my screen && my resolution > 150))
			{
				double dy2 = dy + plc -> baseline;
				double xr = cosa * xbegin - sina * dy2;
				double yr = sina * xbegin + cosa * dy2;
				charCodes [nchars] = '\0';   /* ...and flush. */
				charCodes8 [nchars] = '\0';   /* ...and flush. */
				charCodes16 [nchars] = '\0';   /* ...and flush. */
				charDraw (me, xDC + xr, my yIsZeroAtTheTop ? yDC - yr : yDC + yr,
					plc, charCodes, charCodes8, charCodes16, nchars, x - xbegin);
				nchars = 0;
				xbegin = x;
			}
		}
	} else {
		double xbegin = xDC + dx, x = xbegin, y = my yIsZeroAtTheTop ? yDC - dy : yDC + dy;
		lastlc = lc;
		if (my wrapWidth) {
			/*
			 * Replace some spaces with new-line symbols.
			 */
			int xmax = xDC + my wrapWidth * my scaleX;
			for (plc = lc; plc -> kar >= ' '; plc ++) {
				if (x > xmax) {   /* Wrap (if wrapWidth is too small, each word will be on a separate line). */
					while (plc >= lastlc) {
						if (plc -> kar == ' ' && ! plc -> link)   /* Keep links contiguous. */
							break;
						plc --;
					}
					if (plc <= lastlc) break;   /* Hopeless situation: no spaces; get over it. */
					lastlc = plc;
					plc -> kar = '\n';   // Replace space with newline.
					x = xDC + dx + my secondIndent * my scaleX;
				} else {
					x += plc -> width;
				}
			}
			xbegin = x = xDC + dx;   /* Re-initialize for second pass. */
		}
		for (plc = lc; plc -> kar > '\t'; plc ++) {
			_Graphics_widechar *next = plc + 1;
			if (plc -> link) {
				if (! inLink) {
					double descent = ( my yIsZeroAtTheTop ? -(0.3/72) : (0.3/72) ) * my fontSize * my resolution;
					links [++ numberOfLinks]. x1 = x;
					links [numberOfLinks]. y1 = y - descent;
					links [numberOfLinks]. y2 = y + 3 * descent;
					inLink = TRUE;
				}
			} else if (inLink) {
				links [numberOfLinks]. x2 = x;
				inLink = FALSE;
			}
			if (plc -> kar == '\n') {
				xbegin = x = xDC + dx + my secondIndent * my scaleX;
				y = my yIsZeroAtTheTop ? y + (1.2/72) * my fontSize * my resolution : y - (1.2/72) * my fontSize * my resolution;
			} else {
				charCodes [nchars] = plc -> code;   /* Buffer... */
				charCodes8 [nchars] = plc -> code;   /* Buffer... */
				charCodes16 [nchars ++] = plc -> code;   /* Buffer... */
				x += plc -> width;
				if (next->kar < ' ' || next->style != plc->style ||
					next->baseline != plc->baseline || next->size != plc->size || next->link != plc->link ||
					next->font.integer != plc->font.integer || next->font.string != plc->font.string ||
					next->rightToLeft != plc->rightToLeft)
				{
					charCodes [nchars] = '\0';   /* ...and flush. */
					charCodes8 [nchars] = '\0';   /* ...and flush. */
					charCodes16 [nchars] = '\0';   /* ...and flush. */
					charDraw (me, xbegin, my yIsZeroAtTheTop ? y - plc -> baseline : y + plc -> baseline,
						plc, charCodes, charCodes8, charCodes16, nchars, x - xbegin);
					nchars = 0;
					xbegin = x;
				}
			}
		}
		if (inLink) {
			links [numberOfLinks]. x2 = x;
			inLink = FALSE;
		}
		my textX = (x - my deltaX) / my scaleX;
		my textY = (( my yIsZeroAtTheTop ? y + dy : y - dy ) - my deltaY) / my scaleY;
	}
	#if mac
		if (my screen && ((GraphicsScreen) me) -> useQuartz) {
			GraphicsQuartz_exitDraw ((GraphicsScreen) me);
		}
	#endif
}

static struct { double width; short alignment; } tabs [1 + 20] = { { 0, Graphics_CENTRE },
	{ 1, Graphics_CENTRE }, { 1, Graphics_CENTRE }, { 1, Graphics_CENTRE }, { 1, Graphics_CENTRE },
	{ 1, Graphics_CENTRE }, { 1, Graphics_CENTRE }, { 1, Graphics_CENTRE }, { 1, Graphics_CENTRE } };

/*
 * The routine 'drawCells' handles table and layout.
 */
static void drawCells (Graphics me, double xWC, double yWC, _Graphics_widechar lc []) {
	_Graphics_widechar *plc;
	int itab = 0, saveTextAlignment = my horizontalTextAlignment;
	double saveWrapWidth = my wrapWidth;
	numberOfLinks = 0;
	for (plc = lc; /* No stop condition. */ ; plc ++) {
		charSizes (me, plc);
		drawOneCell (me, xWC * my scaleX + my deltaX, yWC * my scaleY + my deltaY, plc);
		while (plc -> kar != '\0' && plc -> kar != '\t') plc ++;   // Find end of cell.
		if (plc -> kar == '\0') break;   // End of text?
		if (plc -> kar == '\t') {   // Go to next cell.
			xWC += ( tabs [itab]. alignment == Graphics_LEFT ? tabs [itab]. width :
			       tabs [itab]. alignment == Graphics_CENTRE ? 0.5 * tabs [itab]. width : 0 ) * my fontSize / 12.0;
			itab ++;
			xWC += ( tabs [itab]. alignment == Graphics_LEFT ? 0 :
			       tabs [itab]. alignment == Graphics_CENTRE ? 0.5 * tabs [itab]. width : tabs [itab]. width ) * my fontSize / 12.0;
			my horizontalTextAlignment = tabs [itab]. alignment;
			my wrapWidth = tabs [itab]. width * my fontSize / 12.0;
		}
	}
	my horizontalTextAlignment = saveTextAlignment;
	my wrapWidth = saveWrapWidth;
}

static void parseTextIntoCellsLinesRuns (Graphics me, const wchar_t *txt, _Graphics_widechar widechar []) {
	wchar_t kar;
	const wchar_t *in = txt;
	int nquote = 0;
	_Graphics_widechar *out = & widechar [0];
	unsigned int charSuperscript = 0, charSubscript = 0, charItalic = 0, charBold = 0;
	unsigned int wordItalic = 0, wordBold = 0, wordCode = 0, wordLink = 0;
	unsigned int globalSuperscript = 0, globalSubscript = 0, globalItalic = 0, globalBold = 0, globalCode = 0, globalLink = 0;
	unsigned int globalSmall = 0;
	numberOfLinks = 0;
	while ((kar = *in++) != '\0') {
		if (kar == '^' && my circumflexIsSuperscript) {
			if (globalSuperscript) globalSuperscript = 0;
			else if (in [0] == '^') { globalSuperscript = 1; in ++; }
			else charSuperscript = 1;
			wordItalic = wordBold = wordCode = 0;
			continue;
		} else if (kar == '_' && my underscoreIsSubscript) {
			if (globalSubscript) { globalSubscript = 0; wordItalic = wordBold = wordCode = 0; continue; }
			else if (in [0] == '_') { globalSubscript = 1; in ++; wordItalic = wordBold = wordCode = 0; continue; }
			else if (! my dollarSignIsCode) { charSubscript = 1; wordItalic = wordBold = wordCode = 0; continue; }   // Not in manuals.
			else
				;   // A normal underscore in manuals.
		} else if (kar == '%' && my percentSignIsItalic) {
			if (globalItalic) globalItalic = 0;
			else if (in [0] == '%') { globalItalic = 1; in ++; }
			else if (my dollarSignIsCode) wordItalic = 1;   // In manuals.
			else charItalic = 1;
			continue;
		} else if (kar == '#' && my numberSignIsBold) {
			if (globalBold) globalBold = 0;
			else if (in [0] == '#') { globalBold = 1; in ++; }
			else if (my dollarSignIsCode) wordBold = 1;   // In manuals.
			else charBold = 1;
			continue;
		} else if (kar == '$' && my dollarSignIsCode) {
			if (globalCode) globalCode = 0;
			else if (in [0] == '$') { globalCode = 1; in ++; }
			else wordCode = 1;
			continue;
		} else if (kar == '@' && my atSignIsLink   /* Recognize links. */
		           && ! my textRotation)   /* No links allowed in rotated text, because links are identified by 2-point rectangles. */
		{
			wchar_t *to, *max;
			/*
			 * We will distinguish:
			 * 1. The link text: the text shown to the user, drawn in blue.
			 * 2. The link info: the information saved in the Graphics object when the user clicks the link;
			 *    this may be a page title in a manual or any other information.
			 * The link info is equal to the link text in the following cases:
			 * 1. A single-word link: "this is a @Link that consists of one word".
			 * 2. Longer links without '|' in them: "@@Link with spaces@".
			 * The link info is unequal to the link text in the following case:
			 * 3. Longer links with '|' in them: "@@Page linked to|Text shown in blue@"
			 */
			if (globalLink) {
				/*
				 * Detected the third '@' in strings like "@@Link with spaces@".
				 * This closes the link text (which will be shown in blue).
				 */
				globalLink = 0;   /* Close the drawn link text (the normal colour will take over). */
				continue;   /* The '@' must not be drawn. */
			} else if (in [0] == '@') {
				/*
				 * Detected the second '@' in strings like "@@Link with spaces@".
				 * A format like "@@Page linked to|Text shown in blue@" is permitted.
				 * First step: collect the page text (the link information);
				 * it is everything between "@@" and "|" or "@" or end of string.
				 */
				const wchar_t *from = in + 1;   /* Start with first character after "@@". */
				if (! links [++ numberOfLinks]. name)   /* Make room for saving link info. */
					links [numberOfLinks]. name = Melder_calloc (wchar_t, MAX_LINK_LENGTH + 1);
				to = links [numberOfLinks]. name, max = to + MAX_LINK_LENGTH;
				while (*from && *from != '@' && *from != '|' && to < max)   /* Until end-of-string or '@' or '|'... */
					* to ++ = * from ++;   /* ... copy one character. */
				*to = '\0';   /* Close saved link info. */
				/*
				 * Second step: collect the link text that is to be drawn.
				 * Its characters will be collected during the normal cycles of the loop.
				 * If the link info is equal to the link text, no action is needed.
				 * If, on the other hand, there is a separate link info, this will have to be skipped.
				 */
				if (*from == '|')
					in += to - links [numberOfLinks]. name + 1;   /* Skip link info + '|'. */
				/*
				 * We are entering the link-text-collection mode.
				 */
				globalLink = 1;
				/*
				 * Both '@' must be skipped and must not be drawn.
				 */
				in ++;   /* Skip second '@'. */
				continue;   /* Do not draw. */
			} else {
				/*
				 * Detected a single-word link, like in "this is a @Link that consists of one word".
				 * First step: collect the page text: letters, digits, and underscores.
				 */
				const wchar_t *from = in;   /* Start with first character after "@". */
				if (! links [++ numberOfLinks]. name)   /* Make room for saving link info. */
					links [numberOfLinks]. name = Melder_calloc (wchar_t, MAX_LINK_LENGTH + 1);
				to = links [numberOfLinks]. name, max = to + MAX_LINK_LENGTH;
				while (*from && (isalnum (*from) || *from == '_') && to < max)   /* Until end-of-word... */
					*to ++ = *from++;   /* ... copy one character. */
				*to = '\0';   /* Close saved link info. */
				/*
				 * Second step: collect the link text that is to be drawn.
				 * Its characters will be collected during the normal cycles of the loop.
				 * The link info is equal to the link text, so no skipping is needed.
				 */
				wordLink = 1;   /* Enter the single-word link-text-collection mode. */
			}
			continue;
		} else if (kar == '\\') {
			/*
			 * Detected backslash sequence: backslash + kar1 + kar2...
			 */
			wchar_t kar1, kar2;
			/*
			 * ... except if kar1 or kar2 is null: in that case, draw the backslash.
			 */
			if (! (kar1 = in [0]) || ! (kar2 = in [1])) {
				;   // Normal backslash symbol.
			/*
			 * Catch "\s{", which means: small characters until corresponding '}'.
			 */
			} else if (kar2 == '{') {
				if (kar1 == 's') globalSmall = 1;
				in += 2;
				continue;
			/*
			 * Default action: translate the backslash sequence into the long character 'kar1,kar2'.
			 */
			} else {
				kar = Longchar_getInfo (kar1, kar2) -> unicode;
				in += 2;
			}
		} else if (kar == '\"') {
			kar = ++nquote & 1 ? UNICODE_LEFT_DOUBLE_QUOTATION_MARK : UNICODE_RIGHT_DOUBLE_QUOTATION_MARK;
		} else if (kar == '\'') {
			kar = UNICODE_RIGHT_SINGLE_QUOTATION_MARK;
		} else if (kar == '`') {
			kar = UNICODE_LEFT_SINGLE_QUOTATION_MARK;
		} else if (kar >= 32 && kar <= 126) {
			if (kar == 'f') {
				if (in [0] == 'i' && HAS_FI_AND_FL_LIGATURES) {
					kar = UNICODE_LATIN_SMALL_LIGATURE_FI;
					in ++;
				} else if (in [0] == 'l' && HAS_FI_AND_FL_LIGATURES) {
					kar = UNICODE_LATIN_SMALL_LIGATURE_FL;
					in ++;
				}
			} else if (kar == '}') {
				if (globalSmall) { globalSmall = 0; continue; }
			}
		} else if (kar == '\t') {
			out -> kar = '\t';
			out -> rightToLeft = false;
			wordItalic = wordBold = wordCode = wordLink = 0;
			globalSubscript = globalSuperscript = globalItalic = globalBold = globalCode = globalLink = globalSmall = 0;
			charItalic = charBold = charSuperscript = charSubscript = 0;
			out ++;
			continue;   /* Do not draw. */
		} else if (kar == '\n') {
			kar = ' ';
		}
		if (wordItalic | wordBold | wordCode | wordLink) {
			if (! isalnum (kar) && kar != '_')   // FIXME: this test could be more precise.
				wordItalic = wordBold = wordCode = wordLink = 0;
		}
		out -> style =
			(wordLink | globalLink) && my fontStyle != Graphics_CODE ? Graphics_BOLD :
			((my fontStyle & Graphics_ITALIC) | charItalic | wordItalic | globalItalic ? Graphics_ITALIC : 0) +
			((my fontStyle & Graphics_BOLD) | charBold | wordBold | globalBold ? Graphics_BOLD : 0);
		out -> font.string = NULL;
		out -> font.integer = my fontStyle == Graphics_CODE || wordCode || globalCode ||
			kar == '/' || kar == '|' ? kGraphics_font_COURIER : my font;
		out -> link = wordLink | globalLink;
		out -> baseline = charSuperscript | globalSuperscript ? 34 : charSubscript | globalSubscript ? -25 : 0;
		out -> size = globalSmall || out -> baseline != 0 ? 80 : 100;
		if (kar == '/') {
			out -> baseline -= out -> size / 12;
			out -> size += out -> size / 10;
		}
		out -> code = '?';   // Does this have any meaning?
		out -> kar = kar;
		out -> rightToLeft =
			(kar >= 0x0590 && kar <= 0x06FF) ||
			(kar >= 0xFE70 && kar <= 0xFEFF) ||
			(kar >= 0xFB1E && kar <= 0xFDFF);
		charItalic = charBold = charSuperscript = charSubscript = 0;
		out ++;
	}
	out -> kar = '\0';   // End of text.
	out -> rightToLeft = false;
}

double Graphics_textWidth (I, const wchar_t *txt) {
	iam (Graphics);
	double width;
	if (! initBuffer (txt)) return 0.0;
	initText (me);
	parseTextIntoCellsLinesRuns (me, txt, widechar);
	charSizes (me, widechar);
	width = textWidth (widechar);
	exitText (me);
	return width / my scaleX;
}

void Graphics_textRect (I, double x1, double x2, double y1, double y2, const wchar_t *txt) {
	iam (Graphics);
	_Graphics_widechar *plc, *startOfLine;
	double width = 0.0, lineHeight = (1.1 / 72) * my fontSize * my resolution;
	short x1DC = x1 * my scaleX + my deltaX + 2, x2DC = x2 * my scaleX + my deltaX - 2;
	short y1DC = y1 * my scaleY + my deltaY, y2DC = y2 * my scaleY + my deltaY;
	int availableHeight = my yIsZeroAtTheTop ? y1DC - y2DC : y2DC - y1DC, availableWidth = x2DC - x1DC;
	int linesAvailable = availableHeight / lineHeight, linesNeeded = 1, lines, iline;
	if (linesAvailable <= 0) linesAvailable = 1;
	if (availableWidth <= 0) return;
	if (! initBuffer (txt)) return;
	initText (me);
	parseTextIntoCellsLinesRuns (me, txt, widechar);
	charSizes (me, widechar);
	for (plc = widechar; plc -> kar > '\t'; plc ++) {
		width += plc -> width;
		if (width > availableWidth) {
			if (++ linesNeeded > linesAvailable) break;
			width = 0.0;
		}	
	}
	lines = linesNeeded > linesAvailable ? linesAvailable : linesNeeded;
	startOfLine = widechar;
	for (iline = 1; iline <= lines; iline ++) {
		width = 0.0;
		for (plc = startOfLine; plc -> kar > '\t'; plc ++) {
			int flush = FALSE;
			width += plc -> width;
			if (width > availableWidth) flush = TRUE;
			/*
			 * Trick for incorporating end-of-text.
			 */
			if (! flush && plc [1]. kar <= '\t') {
				Melder_assert (iline == lines);
				plc ++;   /* Brr. */
				flush = TRUE;
			}
			if (flush) {
				int saveKar = plc -> kar, direction = my yIsZeroAtTheTop ? -1 : 1;
				int x = my horizontalTextAlignment == Graphics_LEFT ? x1DC :
					my horizontalTextAlignment == Graphics_RIGHT ? x2DC :
					0.5 * (x1 + x2) * my scaleX + my deltaX;
				int y = my verticalTextAlignment == Graphics_BOTTOM ?
					y1DC + direction * (lines - iline) * lineHeight :
					my verticalTextAlignment == Graphics_TOP ?
					y2DC - direction * (iline - 1) * lineHeight :
					0.5 * (y1 + y2) * my scaleY + my deltaY + 0.5 * direction * (lines - iline*2 + 1) * lineHeight;
				plc -> kar = '\0';
				drawOneCell (me, x, y, startOfLine);
				plc -> kar = saveKar;
				startOfLine = plc;
				break;
			}
		}
	}
	exitText (me);
}

void Graphics_text (I, double xWC, double yWC, const wchar_t *txt) {
	iam (Graphics);
	if (my wrapWidth == 0.0 && wcschr (txt, '\n')) {
		double lineSpacingWC = (1.2/72) * my fontSize * my resolution / fabs (my scaleY);
		long numberOfLines = 1;
		for (const wchar_t *p = & txt [0]; *p != '\0'; p ++) {
			if (*p == '\n') {
				numberOfLines ++;
			}
		}
		yWC +=
			my verticalTextAlignment == Graphics_TOP ? 0 :
			my verticalTextAlignment == Graphics_HALF ? 0.5 * (numberOfLines - 1) * lineSpacingWC:
			(numberOfLines - 1) * lineSpacingWC;
		wchar_t *linesToDraw = Melder_wcsdup (txt), *p = & linesToDraw [0];
		for (;;) {
			wchar_t *newline = wcschr (p, '\n');
			if (newline != NULL) *newline = '\0';
			Graphics_text (me, xWC, yWC, p);
			yWC -= lineSpacingWC;
			if (newline != NULL) {
				p = newline + 1;
			} else {
				break;
			}
		}
		Melder_free (linesToDraw);
		return;
	}
	if (! initBuffer (txt)) return;
	initText (me);
	parseTextIntoCellsLinesRuns (me, txt, widechar);
	drawCells (me, xWC, yWC, widechar);
	exitText (me);
	if (my recording) {
		char *txt_utf8 = Melder_peekWcsToUtf8 (txt);
		int length = strlen (txt_utf8) / sizeof (double) + 1;
		op (TEXT, 3 + length); put (xWC); put (yWC); sput (txt_utf8, length)
	}
}

double Graphics_inqTextX (I) { iam (Graphics); return my textX; }
double Graphics_inqTextY (I) { iam (Graphics); return my textY; }

int Graphics_getLinks (Graphics_Link **plinks) { *plinks = & links [0]; return numberOfLinks; }

static double psTextWidth (_Graphics_widechar string [], int useSilipaPS) {
	_Graphics_widechar *character;
	/*
	 * The following has to be kept IN SYNC with GraphicsPostscript::charSize.
	 */
	double textWidth = 0;
	for (character = string; character -> kar > '\t'; character ++) {
		Longchar_Info info = Longchar_getInfoFromNative (character -> kar);
		int font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
				info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
				info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : character -> font.integer;
		int style = character -> style == Graphics_ITALIC ? Graphics_ITALIC :
			character -> style == Graphics_BOLD || character -> link ? Graphics_BOLD :
			character -> style == Graphics_BOLD_ITALIC ? Graphics_BOLD_ITALIC : 0;
		double size = character -> size * 0.01;
		double charWidth = 600;   /* Courier. */
		if (font == kGraphics_font_COURIER) {
			charWidth = 600;
		} else if (style == 0) {
			if (font == kGraphics_font_TIMES) charWidth = info -> ps.times;
			else if (font == kGraphics_font_HELVETICA) charWidth = info -> ps.helvetica;
			else if (font == kGraphics_font_PALATINO) charWidth = info -> ps.palatino;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS) charWidth = info -> ps.timesItalic;
			else charWidth = info -> ps.times;   /* Symbol, IPA. */
		} else if (style == Graphics_BOLD) {
			if (font == kGraphics_font_TIMES) charWidth = info -> ps.timesBold;
			else if (font == kGraphics_font_HELVETICA) charWidth = info -> ps.helveticaBold;
			else if (font == kGraphics_font_PALATINO) charWidth = info -> ps.palatinoBold;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS) charWidth = info -> ps.timesBoldItalic;
			else charWidth = info -> ps.times;
		} else if (style == Graphics_ITALIC) {
			if (font == kGraphics_font_TIMES) charWidth = info -> ps.timesItalic;
			else if (font == kGraphics_font_HELVETICA) charWidth = info -> ps.helvetica;
			else if (font == kGraphics_font_PALATINO) charWidth = info -> ps.palatinoItalic;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS) charWidth = info -> ps.timesItalic;
			else charWidth = info -> ps.times;
		} else if (style == Graphics_BOLD_ITALIC) {
			if (font == kGraphics_font_TIMES) charWidth = info -> ps.timesBoldItalic;
			else if (font == kGraphics_font_HELVETICA) charWidth = info -> ps.helveticaBold;
			else if (font == kGraphics_font_PALATINO) charWidth = info -> ps.palatinoBoldItalic;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS) charWidth = info -> ps.timesBoldItalic;
			else charWidth = info -> ps.times;
		}
		charWidth *= size / 1000.0;
		textWidth += charWidth;
	}
	/*
	 * The following has to be kept IN SYNC with charSizes ().
	 */
	for (character = string; character -> kar > '\t'; character ++) {
		if ((character -> style & Graphics_ITALIC) != 0) {
			_Graphics_widechar *nextCharacter = character + 1;
			if (nextCharacter -> kar <= '\t') {
				textWidth += POSTSCRIPT_SLANT_CORRECTION;
			} else if (((nextCharacter -> style & Graphics_ITALIC) == 0 && nextCharacter -> baseline >= character -> baseline)
				|| (character -> baseline == 0 && nextCharacter -> baseline > 0))
			{
				if (nextCharacter -> kar == '.' || nextCharacter -> kar == ',')
					textWidth += 0.5 * POSTSCRIPT_SLANT_CORRECTION;
				else
					textWidth += POSTSCRIPT_SLANT_CORRECTION;
			}
		}
	}
	return textWidth;
}

double Graphics_textWidth_ps_mm (I, const wchar_t *txt, bool useSilipaPS) {
	iam (Graphics);
	if (! initBuffer (txt)) return 0.0;
	parseTextIntoCellsLinesRuns (me, txt, widechar);
	return psTextWidth (widechar, useSilipaPS) * (double) my fontSize * (25.4 / 72.0);
}

double Graphics_textWidth_ps (I, const wchar_t *txt, bool useSilipaPS) {
	iam (Graphics);
	return Graphics_dxMMtoWC (me, Graphics_textWidth_ps_mm (me, txt, useSilipaPS));
}

#if mac
bool _GraphicsMac_tryToInitializeAtsuiFonts (void) {
	if (theTimesAtsuiFont != 0) return true;   // once
	theTimesAtsuiFont = ATSFontFindFromName (CFSTR ("Times"), kATSOptionFlagsDefault);
	if (! theTimesAtsuiFont) theTimesAtsuiFont = ATSFontFindFromName (CFSTR ("Times New Roman"), kATSOptionFlagsDefault);
	theHelveticaAtsuiFont = ATSFontFindFromName (CFSTR ("Helvetica"), kATSOptionFlagsDefault);
	if (! theHelveticaAtsuiFont) theHelveticaAtsuiFont = ATSFontFindFromName (CFSTR ("Arial"), kATSOptionFlagsDefault);
	theCourierAtsuiFont = ATSFontFindFromName (CFSTR ("Courier"), kATSOptionFlagsDefault);
	if (! theCourierAtsuiFont) theCourierAtsuiFont = ATSFontFindFromName (CFSTR ("Courier New"), kATSOptionFlagsDefault);
	theSymbolAtsuiFont = ATSFontFindFromName (CFSTR ("Symbol"), kATSOptionFlagsDefault);
	thePalatinoAtsuiFont = ATSFontFindFromName (CFSTR ("Palatino"), kATSOptionFlagsDefault);
	if (! thePalatinoAtsuiFont) thePalatinoAtsuiFont = ATSFontFindFromName (CFSTR ("Book Antiqua"), kATSOptionFlagsDefault);
	if (! thePalatinoAtsuiFont) thePalatinoAtsuiFont = theTimesAtsuiFont;
	theZapfDingbatsAtsuiFont = ATSFontFindFromName (CFSTR ("Zapf Dingbats"), kATSOptionFlagsDefault);
	if (! theZapfDingbatsAtsuiFont) theZapfDingbatsAtsuiFont = theTimesAtsuiFont;
	if (! theTimesAtsuiFont || ! theHelveticaAtsuiFont || ! theCourierAtsuiFont || ! theSymbolAtsuiFont) {
		Melder_warning1 (L"Praat cannot find one or more of the fonts Times (or Times New Roman), "
			"Helvetica (or Arial), Courier (or Courier New), and Symbol. "
			"Praat will have limited capabilities for international text.");
		return false;
	}
	theIpaTimesAtsuiFont = ATSFontFindFromName (CFSTR ("Doulos SIL"), kATSOptionFlagsDefault);
	theIpaPalatinoAtsuiFont = ATSFontFindFromName (CFSTR ("Charis SIL"), kATSOptionFlagsDefault);
	if (! theIpaTimesAtsuiFont) {
		if (theIpaPalatinoAtsuiFont) {
			theIpaTimesAtsuiFont = theIpaPalatinoAtsuiFont;
		} else {
			Melder_warning1 (L"Praat cannot find the Charis SIL or Doulos SIL font.\n"
				"Phonetic characters will not look well.");   // because ATSUI will use the "last resort font"
			theIpaTimesAtsuiFont = theTimesAtsuiFont;
			theIpaPalatinoAtsuiFont = thePalatinoAtsuiFont;
		}
	} else if (! theIpaPalatinoAtsuiFont) {
		theIpaPalatinoAtsuiFont = theIpaTimesAtsuiFont;
	}
	Melder_assert (theTimesAtsuiFont != 0);
	ATSUFindFontFromName (NULL, 0, 0, 0, 0, kFontArabicLanguage, & theArabicAtsuiFont);
	return true;
}
#endif

void _Graphics_text_init (I) {   /* BUG: should be done as late as possible. */
	iam (Graphics);
	if (my screen) {
		iam (GraphicsScreen);
		#if mac
			if (! thePalatinoFont) {
				GetFNum ("\005Times", & theTimesFont);
				GetFNum ("\011Helvetica", & theHelveticaFont);
				GetFNum ("\007Courier", & theCourierFont);
				GetFNum ("\006Symbol", & theSymbolFont);
				GetFNum ("\008Palatino", & thePalatinoFont);
				if (! thePalatinoFont) thePalatinoFont = theTimesFont;
				GetFNum ("\015Zapf Dingbats", & theZapfDingbatsFont);
				if (! theZapfDingbatsFont) theZapfDingbatsFont = theTimesFont;
				if (! theTimesFont || ! theHelveticaFont || ! theCourierFont || ! theSymbolFont) {
					Melder_fatal ("Praat cannot start up because it cannot find one or more of the fonts Times, Helvetica, Courier and Symbol. "
						"Please install these fonts from your system CD. Praat will now quit.");
				}
			}
			if (my useQuartz && theTimesAtsuiFont == 0) {
				Melder_assert (_GraphicsMac_tryToInitializeAtsuiFonts ());   // should have been handled when setting my useQuartz to true
			}
		#elif win
			int font, size, style;
			if (my printer || my metafile)
				for (font = kGraphics_font_MIN; font <= kGraphics_font_DINGBATS; font ++)
					for (size = 0; size <= 4; size ++)
						for (style = 0; style <= Graphics_BOLD_ITALIC; style ++)
							if (printerFonts [font] [size] [style]) {
								DeleteObject (printerFonts [font] [size] [style]);
								printerFonts [font] [size] [style] = 0;
							}
		#endif
	}
}

/* Output attributes. */

void Graphics_setTextAlignment (I, int hor, int vert) {
	iam (Graphics);
	if (hor != Graphics_NOCHANGE) my horizontalTextAlignment = hor;
	if (vert != Graphics_NOCHANGE) my verticalTextAlignment = vert;
	if (my recording) { op (SET_TEXT_ALIGNMENT, 2); put (hor); put (vert); }
}

void Graphics_setFont (I, enum kGraphics_font font) {
	iam (Graphics);
	my font = font;
	if (my recording) { op (SET_FONT, 1); put (font); }
}

void Graphics_setFontSize (I, int size) {
	iam (Graphics);
	my fontSize = size;
	if (my recording) { op (SET_FONT_SIZE, 1); put (size); }
}

void Graphics_setFontStyle (I, int style) {
	iam (Graphics);
	my fontStyle = style;
	if (my recording) { op (SET_FONT_STYLE, 1); put (style); }
}

void Graphics_setItalic (I, bool onoff) {
	iam (Graphics);
	if (onoff) my fontStyle |= Graphics_ITALIC; else my fontStyle &= ~ Graphics_ITALIC;
}

void Graphics_setBold (I, bool onoff) {
	iam (Graphics);
	if (onoff) my fontStyle |= Graphics_BOLD; else my fontStyle &= ~ Graphics_BOLD;
}

void Graphics_setCode (I, bool onoff) {
	iam (Graphics);
	if (onoff) my fontStyle |= Graphics_CODE; else my fontStyle &= ~ Graphics_CODE;
}

void Graphics_setTextRotation (I, double angle) {
	iam (Graphics);
	my textRotation = angle;
	if (my recording) { op (SET_TEXT_ROTATION, 1); put (angle); }
}

void Graphics_setWrapWidth (I, double wrapWidth) {
	iam (Graphics);
	my wrapWidth = wrapWidth;
	if (my recording) { op (SET_WRAP_WIDTH, 1); put (wrapWidth); }
}

void Graphics_setSecondIndent (I, double indent) {
	iam (Graphics);
	my secondIndent = indent;
	if (my recording) { op (SET_SECOND_INDENT, 1); put (indent); }
}

void Graphics_setPercentSignIsItalic (I, bool isItalic) {
	iam (Graphics);
	my percentSignIsItalic = isItalic;
	if (my recording) { op (SET_PERCENT_SIGN_IS_ITALIC, 1); put (isItalic); }
}

void Graphics_setNumberSignIsBold (I, bool isBold) {
	iam (Graphics);
	my numberSignIsBold = isBold;
	if (my recording) { op (SET_NUMBER_SIGN_IS_BOLD, 1); put (isBold); }
}

void Graphics_setCircumflexIsSuperscript (I, bool isSuperscript) {
	iam (Graphics);
	my circumflexIsSuperscript = isSuperscript;
	if (my recording) { op (SET_CIRCUMFLEX_IS_SUPERSCRIPT, 1); put (isSuperscript); }
}

void Graphics_setUnderscoreIsSubscript (I, bool isSubscript) {
	iam (Graphics);
	my underscoreIsSubscript = isSubscript;
	if (my recording) { op (SET_UNDERSCORE_IS_SUBSCRIPT, 1); put (isSubscript); }
}

void Graphics_setDollarSignIsCode (I, bool isCode) {
	iam (Graphics);
	my dollarSignIsCode = isCode;
	if (my recording) { op (SET_DOLLAR_SIGN_IS_CODE, 1); put (isCode); }
}

void Graphics_setAtSignIsLink (I, bool isLink) {
	iam (Graphics);
	my atSignIsLink = isLink;
	if (my recording) { op (SET_AT_SIGN_IS_LINK, 1); put (isLink); }
}

/* Inquiries. */

int Graphics_inqFont (I) { iam (Graphics); return my font; }
int Graphics_inqFontSize (I) { iam (Graphics); return my fontSize; }
int Graphics_inqFontStyle (I) { iam (Graphics); return my fontStyle; }

/* End of file Graphics_text.c */
