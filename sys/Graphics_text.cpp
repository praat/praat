/* Graphics_text.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma, 2013 Tom Naughton, 2017 David Weenink
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

#include "../kar/UnicodeData.h"
#include "GraphicsP.h"
#include "../kar/longchar.h"
#include "Printer.h"

extern const char * ipaSerifRegularPS [];

/*
 * When computing the width of a text by adding the widths of the separate characters,
 * we will make a correction for systems that make slanted characters overlap the character box to their right.
 * The effect is especially strong on Mac (older versions).
 * The slant correction is taken relative to the font size.
 */
#define POSTSCRIPT_SLANT_CORRECTION  0.1
#define SCREEN_SLANT_CORRECTION  0.05

#define HAS_FI_AND_FL_LIGATURES  ( my postScript == true )

#if cairo
	PangoFontMap *thePangoFontMap;
	PangoContext *thePangoContext;
	static bool hasTimes, hasHelvetica, hasCourier, hasSymbol, hasPalatino, hasDoulos, hasCharis, hasIpaSerif;
#elif gdi
	#define win_MAXIMUM_FONT_SIZE  500
	static HFONT fonts [1 + (int) kGraphics_resolution::MAX] [1 + kGraphics_font_JAPANESE] [1+win_MAXIMUM_FONT_SIZE] [1 + Graphics_BOLD_ITALIC];
	static int win_size2isize (int size) { return size > win_MAXIMUM_FONT_SIZE ? win_MAXIMUM_FONT_SIZE : size; }
	static int win_isize2size (int isize) { return isize; }
#elif quartz
	static bool hasTimes, hasHelvetica, hasCourier, hasSymbol, hasPalatino, hasDoulos, hasCharis, hasIpaSerif;
	#define mac_MAXIMUM_FONT_SIZE  500
	static CTFontRef theScreenFonts [1 + kGraphics_font_DINGBATS] [1+mac_MAXIMUM_FONT_SIZE] [1 + Graphics_BOLD_ITALIC];
#endif

#if gdi
	#ifdef __CYGWIN__
		#define FONT_TYPE_TYPE  unsigned int
	#else
		#define FONT_TYPE_TYPE  unsigned long int
	#endif
	static bool charisAvailable = false, doulosAvailable = false;
	static int CALLBACK fontFuncEx_charis (const LOGFONTW *oldLogFont, const TEXTMETRICW *oldTextMetric, FONT_TYPE_TYPE fontType, LPARAM lparam) {
		const LPENUMLOGFONTW logFont = (LPENUMLOGFONTW) oldLogFont; (void) oldTextMetric; (void) fontType; (void) lparam;
		charisAvailable = true;
		return 1;
	}
	static int CALLBACK fontFuncEx_doulos (const LOGFONTW *oldLogFont, const TEXTMETRICW *oldTextMetric, FONT_TYPE_TYPE fontType, LPARAM lparam) {
		const LPENUMLOGFONTW logFont = (LPENUMLOGFONTW) oldLogFont; (void) oldTextMetric; (void) fontType; (void) lparam;
		doulosAvailable = true;
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
		spec. lfCharSet =
			font == kGraphics_font_SYMBOL ? SYMBOL_CHARSET :
			font == kGraphics_font_CHINESE ? DEFAULT_CHARSET :
			font == kGraphics_font_JAPANESE ? DEFAULT_CHARSET :
			font >= kGraphics_font_IPATIMES ? DEFAULT_CHARSET :
			ANSI_CHARSET;
		spec. lfOutPrecision = spec. lfClipPrecision = spec. lfQuality = 0;
		spec. lfPitchAndFamily =
			( font == (int) kGraphics_font::COURIER ? FIXED_PITCH : font == kGraphics_font_IPATIMES ? DEFAULT_PITCH : VARIABLE_PITCH ) |
			( font == (int) kGraphics_font::HELVETICA ? FF_SWISS : font == (int) kGraphics_font::COURIER ? FF_MODERN :
			  font == kGraphics_font_CHINESE ? FF_DONTCARE :
			  font == kGraphics_font_JAPANESE ? FF_DONTCARE :
			  font >= kGraphics_font_IPATIMES ? FF_DONTCARE : FF_ROMAN );
		if (font == kGraphics_font_IPATIMES && ! ipaInited && Melder_debug != 15) {
			LOGFONTW logFont;
			logFont. lfCharSet = DEFAULT_CHARSET;
			logFont. lfPitchAndFamily = 0;
			wcscpy (logFont. lfFaceName, L"Charis SIL");
			EnumFontFamiliesExW (my d_gdiGraphicsContext, & logFont, fontFuncEx_charis, 0, 0);
			wcscpy (logFont. lfFaceName, L"Doulos SIL");
			EnumFontFamiliesExW (my d_gdiGraphicsContext, & logFont, fontFuncEx_doulos, 0, 0);
			ipaInited = true;
			if (! charisAvailable && ! doulosAvailable) {
				/* BUG: The next warning may cause reentry of drawing (on window exposure) and lead to crash. Some code must be non-reentrant !! */
				Melder_warning (U"The phonetic font is not available.\nSeveral characters may not look correct.\nSee www.praat.org");
			}
		}
		wcscpy (spec. lfFaceName,
			font == (int) kGraphics_font::HELVETICA ? L"Arial" :
			font == (int) kGraphics_font::TIMES     ? L"Times New Roman" :
			font == (int) kGraphics_font::COURIER   ? L"Courier New" :
			font == (int) kGraphics_font::PALATINO  ? L"Book Antiqua" :
			font == kGraphics_font_SYMBOL    ? L"Symbol" :
			font == kGraphics_font_IPATIMES  ? ( doulosAvailable && style == 0 ? L"Doulos SIL" : charisAvailable ? L"Charis SIL" : L"Times New Roman" ) :
			font == kGraphics_font_DINGBATS  ? L"Wingdings" :
			font == kGraphics_font_CHINESE   ? L"SimSun" :
			font == kGraphics_font_JAPANESE  ? L"MS UI Gothic" :
			L"");
		return CreateFontIndirectW (& spec);
	}
#endif

#if cairo
	static PangoFontDescription *PangoFontDescription_create (int font, _Graphics_widechar *lc) {
		static PangoFontDescription *fontDescriptions [1 + kGraphics_font_DINGBATS];
		Melder_assert (font >= 0 && font <= kGraphics_font_DINGBATS);
		if (! fontDescriptions [font]) {
			const char *fontFace =
				font == (int) kGraphics_font::HELVETICA ? "Helvetica" :
				font == (int) kGraphics_font::TIMES ? "Times" :
				font == (int) kGraphics_font::COURIER ? "Courier New" :
				font == (int) kGraphics_font::PALATINO ? "Palatino" :
				font == kGraphics_font_IPATIMES ? "Doulos SIL" :
				font == kGraphics_font_IPAPALATINO ? "Charis SIL" :
				font == kGraphics_font_DINGBATS ? "Dingbats" : "Serif";
			fontDescriptions [font] = pango_font_description_from_string (fontFace);
		}

		PangoStyle slant = (lc -> style & Graphics_ITALIC ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
		pango_font_description_set_style (fontDescriptions [font], slant);
						
		PangoWeight weight = (lc -> style & Graphics_BOLD ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
		pango_font_description_set_weight (fontDescriptions [font], weight);
		pango_font_description_set_absolute_size (fontDescriptions [font], (int) (lc -> size * PANGO_SCALE));
		return fontDescriptions [font];
	}
#endif

inline static bool isDiacritic (Longchar_Info info, int font) {
	if (info -> isDiacritic == 0)
		return false;
	if (info -> isDiacritic == 1)
		return true;
	Melder_assert (info -> isDiacritic == 2);   // corner
	if (font == kGraphics_font_IPATIMES || font == kGraphics_font_IPAPALATINO)
		return false;   // Doulos or Charis
	return true;   // e.g. Times substitutes a zero-width corner
}

/*
	Most operating systems provide automatic font substitution nowadays,
	so that e.g. phonetic characters will be drawn recognizably even if the
	user-preferred font is e.g. Times, Helvetica, Courier or Palatino
	and the phonetic characters are not available in that user font.
	
	However, on some systems the substituted font might really be a last resort
	font that does not look at all similar to the user font. An example is
	the use of the sans-serif font Lucida Grande for phonetic characters
	within a stretch of a serif font such as Times or Palatino.
	
	This is not good enough for Praat. We need more control over the shape
	of phonetic characters. We therefore advise the use of Doulos SIL,
	which is Times-like, or Charis SIL, which is Palatino-like.
	For true continuity between non-phonetic and phonetic characters it is
	mandatory that the exact same font is used for both types of characters,
	so we use Doulos SIL to replace Times even for non-phonetic characters,
	and Charis SIL to replace Palatino even for non-phonetic characters.
	A technical issue that makes this even more important is that diacritics
	can look really weird if at the beginning of a Praat font stretch:
	a "b" followed by a ring below will not be aligned correctly if they
	are part of different Praat font stretches.
	
	Beside Praat-enforced visual font continuity, there are some more issues,
	such as that the "/" (slash) character should extend below the baseline
	whenever it is used in an equation or to demarcate a phonological
	representation.
*/
#if cairo || quartz
inline static int chooseFont (Graphics me, _Graphics_widechar *lc) {
	/*
		When we arrive here, the character's font is the "user-preferred font",
		which is Courier if the user asked for code style (e.g. between "$$" and "$"),
		or otherwise Times, Helvetica, Courier or Palatino, as chosen from a font menu.
		
		Exception: if the character is a slash, its font has already been converted to Courier.
	*/
	int font = lc -> font.integer_;
	Longchar_Info info = lc -> karInfo;
	int alphabet = info -> alphabet;

	if (font == (int) kGraphics_font::COURIER) {
		constexpr bool systemSubstitutesMonospacedSerifFontForIpaCourier = (quartz);
		if (systemSubstitutesMonospacedSerifFontForIpaCourier) {
			/*
				No need to check whether the character is phonetic or not.
			*/
			return (int) kGraphics_font::COURIER;
		}
		if (alphabet == Longchar_SYMBOL ||
			alphabet == Longchar_PHONETIC ||
			lc [1]. kar > U'\t' && lc [1]. karInfo -> isDiacritic)   // inspect next character to ensure diacritic continuity
		{
			/*
				Serif is more important than monospaced,
				and Charis looks slightly better within Courier than Doulos does.
			*/
			if (hasCharis)
				return kGraphics_font_IPAPALATINO;
			if (hasDoulos)
				return kGraphics_font_IPATIMES;
		}
		return (int) kGraphics_font::COURIER;
	}
	font =
		alphabet == Longchar_SYMBOL || // ? kGraphics_font_SYMBOL :
		alphabet == Longchar_PHONETIC ?
			( my font == kGraphics_font::TIMES ?
				( hasDoulos ?
					( lc -> style == 0 ?
						kGraphics_font_IPATIMES :
					  hasCharis ?
						kGraphics_font_IPAPALATINO :   // other styles in Charis, because Doulos has no bold or italic
						(int) kGraphics_font::TIMES
					) :
				  hasCharis ?
					kGraphics_font_IPAPALATINO :
					(int) kGraphics_font::TIMES   // on newer systems, Times and Times New Roman have a lot of phonetic characters
				) :
			  my font == kGraphics_font::HELVETICA ?
				(int) kGraphics_font::HELVETICA :   // sans serif, so fall back on Lucida Grande or so for phonetic characters
			  /* my font must be kGraphics_font_PALATINO */
			  hasCharis && Melder_debug != 900 ?
				kGraphics_font_IPAPALATINO :
			  hasDoulos && Melder_debug != 900 ?
				( lc -> style == 0 ?
					kGraphics_font_IPATIMES :
					(int) kGraphics_font::TIMES
				) :
				(int) kGraphics_font::PALATINO
			) :
		alphabet == Longchar_DINGBATS ?
			kGraphics_font_DINGBATS :
		my font == kGraphics_font::TIMES ?
			( hasDoulos ?
				( lc -> style == 0 ?
					kGraphics_font_IPATIMES :
				  lc -> style == Graphics_ITALIC ?
					( lc [1]. kar > U'\t' && lc [1]. karInfo -> isDiacritic && hasCharis ?
						kGraphics_font_IPAPALATINO : (int) kGraphics_font::TIMES ) :   // correct placement of diacritics
				  hasCharis ?
					kGraphics_font_IPAPALATINO :
					(int) kGraphics_font::TIMES
				) :
				(int) kGraphics_font::TIMES
			) :
		my font == kGraphics_font::HELVETICA ?
			(int) kGraphics_font::HELVETICA :
		my font == kGraphics_font::PALATINO ?
			( hasCharis && Melder_debug != 900 ?
				kGraphics_font_IPAPALATINO :
				(int) kGraphics_font::PALATINO
			) :
		(int) my font;   // why not lc -> font.integer_?
	Melder_assert (font >= 0 && font <= kGraphics_font_DINGBATS);
	return font;
}
#endif

static void charSize (Graphics anyGraphics, _Graphics_widechar *lc) {
	if (anyGraphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (anyGraphics);
		#if cairo
			Melder_assert (my duringXor);
			const Longchar_Info info = lc -> karInfo;
			const int normalSize = my fontSize * my resolution / 72.0;
			const int smallSize = (3 * normalSize + 2) / 4;
			const int size = ( lc -> size < 100 ? smallSize : normalSize );
			lc -> width = 7;
			lc -> baseline *= my fontSize * 0.01;
			lc -> code = lc -> kar;
			lc -> font.string = nullptr;
			lc -> font.integer_ = 0;
			lc -> size = size;
		#elif gdi
			Longchar_Info info = lc -> karInfo;
			const int normalSize = win_size2isize (my fontSize);
			const int smallSize = (3 * normalSize + 2) / 4;
			int font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
			       info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
			       info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : lc -> font.integer_;
			if ((unsigned int) lc -> kar >= 0x2E80 && (unsigned int) lc -> kar <= 0x9FFF)
				font = ( theGraphicsCjkFontStyle == kGraphics_cjkFontStyle::CHINESE ? kGraphics_font_CHINESE : kGraphics_font_JAPANESE );
			const int size = ( lc -> size < 100 ? smallSize : normalSize );
			const int style = lc -> style & (Graphics_ITALIC | Graphics_BOLD);   // take out Graphics_CODE
			HFONT fontInfo = fonts [(int) my resolutionNumber] [font] [size] [style];
			if (! fontInfo) {
				fontInfo = loadFont (me, font, size, style);
				if (! fontInfo)
					return;
				fonts [(int) my resolutionNumber] [font] [size] [style] = fontInfo;
			}
			SIZE extent;
			lc -> code =
				font == kGraphics_font_IPATIMES ||
				font == (int) kGraphics_font::TIMES ||
				font == (int) kGraphics_font::HELVETICA ||
				font == kGraphics_font_CHINESE ||
				font == kGraphics_font_JAPANESE ||
				font == (int) kGraphics_font::COURIER ? lc -> kar :
				info -> winEncoding;
			if (lc -> code == 0) {
				_Graphics_widechar *lc2;
				if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK) {
					info = Longchar_getInfo (U's', U'w');
					lc -> kar = info -> unicode;
					lc -> code = info -> winEncoding;
					for (lc2 = lc + 1; lc2 -> kar != U'\0'; lc2 ++) { }
					lc2 [1]. kar = U'\0';
					while (lc2 - lc > 0) {
						lc2 [0] = lc2 [-1];
						lc2 --;
					}
					lc [1]. kar = UNICODE_MODIFIER_LETTER_RHOTIC_HOOK;
				} else if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE) {
					info = Longchar_getInfo (U'l', U' ');
					lc -> kar = info -> unicode;
					lc -> code = info -> winEncoding;
					for (lc2 = lc + 1; lc2 -> kar != U'\0'; lc2 ++) { }
					lc2 [1]. kar = U'\0';
					while (lc2 - lc > 0) {
						lc2 [0] = lc2 [-1];
						lc2 --;
					}
					lc [1]. kar = UNICODE_COMBINING_TILDE_OVERLAY;
				}
			}
			SelectFont (my d_gdiGraphicsContext, fontInfo);
			if (lc -> code <= 0x00FFFF) {
				char16 code = (char16) lc -> code;
				GetTextExtentPoint32W (my d_gdiGraphicsContext, (WCHAR *) & code, 1, & extent);
			} else {
				char32 code [2] { lc -> code, U'\0' };
				GetTextExtentPoint32W (my d_gdiGraphicsContext, Melder_peek32toW (code), 2, & extent);
			}
			lc -> width = extent. cx;
			lc -> baseline *= my fontSize * 0.01 * my resolution / 72.0;
			lc -> font.string = nullptr;
			lc -> font.integer_ = font;   // kGraphics_font_HELVETICA .. kGraphics_font_DINGBATS
			lc -> size = size;   // 0..4 instead of 10..24
			lc -> style = style;   // without Graphics_CODE
		#elif quartz
		#endif
	} else if (anyGraphics -> postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (anyGraphics);
		const int normalSize = (int) ((double) my fontSize * (double) my resolution / 72.0);
		Longchar_Info info = lc -> karInfo;
		const int font = info -> alphabet == Longchar_SYMBOL ? kGraphics_font_SYMBOL :
				info -> alphabet == Longchar_PHONETIC ? kGraphics_font_IPATIMES :
				info -> alphabet == Longchar_DINGBATS ? kGraphics_font_DINGBATS : lc -> font.integer_;
		const int style = lc -> style == Graphics_ITALIC ? Graphics_ITALIC :
			lc -> style == Graphics_BOLD || lc -> link ? Graphics_BOLD :
			lc -> style == Graphics_BOLD_ITALIC ? Graphics_BOLD_ITALIC : 0;
		if (! my fontInfos [font] [style]) {
			const char *fontInfo, *secondaryFontInfo = nullptr, *tertiaryFontInfo = nullptr;
			if (font == (int) kGraphics_font::COURIER) {
				fontInfo = style == Graphics_BOLD ? "Courier-Bold" :
					style == Graphics_ITALIC ? "Courier-Oblique" :
					style == Graphics_BOLD_ITALIC ? "Courier-BoldOblique" : "Courier";
				secondaryFontInfo = style == Graphics_BOLD ? "CourierNewPS-BoldMT" :
					style == Graphics_ITALIC ? "CourierNewPS-ItalicMT" :
					style == Graphics_BOLD_ITALIC ? "CourierNewPS-BoldItalicMT" : "CourierNewPSMT";
				tertiaryFontInfo = style == Graphics_BOLD ? "CourierNew-Bold" :
					style == Graphics_ITALIC ? "CourierNew-Italic" :
					style == Graphics_BOLD_ITALIC ? "CourierNew-BoldItalic" : "CourierNew";
			} else if (font == (int) kGraphics_font::TIMES) {
				fontInfo = style == Graphics_BOLD ? "Times-Bold" :
					style == Graphics_ITALIC ? "Times-Italic" :
					style == Graphics_BOLD_ITALIC ? "Times-BoldItalic" : "Times-Roman";
				secondaryFontInfo = style == Graphics_BOLD ? "TimesNewRomanPS-BoldMT" :
					style == Graphics_ITALIC ? "TimesNewRomanPS-ItalicMT" :
					style == Graphics_BOLD_ITALIC ? "TimesNewRomanPS-BoldItalicMT" : "TimesNewRomanPSMT";
				tertiaryFontInfo = style == Graphics_BOLD ? "TimesNewRoman-Bold" :
					style == Graphics_ITALIC ? "TimesNewRoman-Italic" :
					style == Graphics_BOLD_ITALIC ? "TimesNewRoman-BoldItalic" : "TimesNewRoman";
			} else if (font == (int) kGraphics_font::PALATINO) {
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
					const char **p;
					for (p = & ipaSerifRegularPS [0]; *p; p ++)
						my d_printf (my d_file, "%s", *p);
					my loadedXipa = true;
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
			my fontInfos [font] [style] = Melder_malloc_f (char, 100);
			if (font == kGraphics_font_IPATIMES || font == kGraphics_font_SYMBOL || font == kGraphics_font_DINGBATS) {
				strcpy (my fontInfos [font] [style], fontInfo);
			} else {
				sprintf (my fontInfos [font] [style], "%s-Praat", fontInfo);
				if (thePrinter. fontChoiceStrategy == kGraphicsPostscript_fontChoiceStrategy::LINOTYPE) {
					my d_printf (my d_file, "/%s /%s-Praat PraatEncode\n", fontInfo, fontInfo);
				} else if (thePrinter. fontChoiceStrategy == kGraphicsPostscript_fontChoiceStrategy::MONOTYPE) {
					my d_printf (my d_file, "/%s /%s-Praat PraatEncode\n", tertiaryFontInfo, fontInfo);
				} else if (thePrinter. fontChoiceStrategy == kGraphicsPostscript_fontChoiceStrategy::PS_MONOTYPE) {
					my d_printf (my d_file, "/%s /%s-Praat PraatEncode\n", secondaryFontInfo, fontInfo);
				} else {
					/* Automatic font choice strategy. */
					if (secondaryFontInfo) {
						my d_printf (my d_file,
							"/%s /Font resourcestatus\n"
							"{ pop pop /%s /%s-Praat PraatEncode }\n"
							"{ /%s /%s-Praat PraatEncode }\n"
							"ifelse\n",
							secondaryFontInfo, secondaryFontInfo, fontInfo, fontInfo, fontInfo);
					} else {
						my d_printf (my d_file, "/%s /%s-Praat PraatEncode\n", fontInfo, fontInfo);
					}
				}
			}
		}
		lc -> font.integer_ = 0;
		lc -> font.string = my fontInfos [font] [style];

		/*
		 * Convert size and baseline information to device coordinates.
		 */
		lc -> size *= normalSize * 0.01;
		lc -> baseline *= normalSize * 0.01;

		if (font == (int) kGraphics_font::COURIER) {
			lc -> width = 600;   // Courier
		} else if (style == 0) {
			if (font == (int) kGraphics_font::TIMES)
				lc -> width = info -> ps.times;
			else if (font == (int) kGraphics_font::HELVETICA)
				lc -> width = info -> ps.helvetica;
			else if (font == (int) kGraphics_font::PALATINO)
				lc -> width = info -> ps.palatino;
			else if (font == kGraphics_font_SYMBOL)
				lc -> width = info -> ps.times;
			else if (my useSilipaPS)
				lc -> width = info -> ps.timesItalic;   // ?
			else
				lc -> width = info -> ps.times;   // XIPA
		} else if (style == Graphics_BOLD) {
			if (font == (int) kGraphics_font::TIMES)
				lc -> width = info -> ps.timesBold;
			else if (font == (int) kGraphics_font::HELVETICA)
				lc -> width = info -> ps.helveticaBold;
			else if (font == (int) kGraphics_font::PALATINO)
				lc -> width = info -> ps.palatinoBold;
			else if (font == kGraphics_font_SYMBOL)
				lc -> width = info -> ps.times;
			else if (my useSilipaPS)
				lc -> width = info -> ps.timesBoldItalic;   // ?
			else
				lc -> width = info -> ps.times;   // Symbol, IPA
		} else if (style == Graphics_ITALIC) {
			if (font == (int) kGraphics_font::TIMES)
				lc -> width = info -> ps.timesItalic;
			else if (font == (int) kGraphics_font::HELVETICA)
				lc -> width = info -> ps.helvetica;
			else if (font == (int) kGraphics_font::PALATINO)
				lc -> width = info -> ps.palatinoItalic;
			else if (font == kGraphics_font_SYMBOL)
				lc -> width = info -> ps.times;
			else if (my useSilipaPS)
				lc -> width = info -> ps.timesItalic;
			else
				lc -> width = info -> ps.times;   // Symbol, IPA
		} else if (style == Graphics_BOLD_ITALIC) {
			if (font == (int) kGraphics_font::TIMES)
				lc -> width = info -> ps.timesBoldItalic;
			else if (font == (int) kGraphics_font::HELVETICA)
				lc -> width = info -> ps.helveticaBold;
			else if (font == (int) kGraphics_font::PALATINO)
				lc -> width = info -> ps.palatinoBoldItalic;
			else if (font == kGraphics_font_SYMBOL)
				lc -> width = info -> ps.times;
			else if (my useSilipaPS)
				lc -> width = info -> ps.timesBoldItalic;
			else
				lc -> width = info -> ps.times;   // Symbol, IPA
		}
		lc -> width *= lc -> size / 1000.0;
		lc -> code = ( font == kGraphics_font_IPATIMES && my useSilipaPS ? info -> macEncoding : info -> psEncoding );
		if (lc -> code == 0) {
			_Graphics_widechar *lc2;
			if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK) {
				info = Longchar_getInfo (U's', U'w');
				lc -> kar = info -> unicode;
				lc -> code = info -> macEncoding;
				lc -> width = info -> ps.timesItalic * lc -> size / 1000.0;
				for (lc2 = lc + 1; lc2 -> kar != U'\0'; lc2 ++) { }
				lc2 [1]. kar = U'\0';
				while (lc2 - lc > 0) {
					lc2 [0] = lc2 [-1];
					lc2 --;
				}
				lc [1]. kar = UNICODE_MODIFIER_LETTER_RHOTIC_HOOK;
			} else if (lc -> kar == UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE) {
				info = Longchar_getInfo (U'l', U' ');
				lc -> code = info -> macEncoding;
				lc -> kar = info -> unicode;
				lc -> width = info -> ps.timesItalic * lc -> size / 1000.0;
				for (lc2 = lc + 1; lc2 -> kar != U'\0'; lc2 ++) { }
				lc2 [1]. kar = U'\0';
				while (lc2 - lc > 0) {
					lc2 [0] = lc2 [-1];
					lc2 --;
				}
				lc [1]. kar = UNICODE_COMBINING_TILDE_OVERLAY;
			}
		}
	}
}

#if quartz
static conststring32 quartz_getFontName (int font, int style) {
	if (Melder_systemVersion < 110000)
		style = 0;   // style extension in the name was not needed on macOS X
	switch (font) {
		case (int) kGraphics_font::TIMES:
			return
				style == 0 ? U"Times New Roman"
				: style == Graphics_BOLD ? U"Times New Roman Bold"
				: style == Graphics_ITALIC ? U"Times New Roman Italic"
				: U"Times New Roman Bold Italic";
		case (int) kGraphics_font::HELVETICA:
			return
				style == 0 ? U"Arial"
				: style == Graphics_BOLD ? U"Arial Bold"
				: style == Graphics_ITALIC ? U"Arial Italic"
				: U"Arial Bold Italic";
		case (int) kGraphics_font::COURIER:
			return
				style == 0 ? U"Courier New"
				: style == Graphics_BOLD ? U"Courier New Bold"
				: style == Graphics_ITALIC ? U"Courier New Italic"
				: U"Courier New Bold Italic";
		case (int) kGraphics_font::PALATINO:
			if (Melder_debug == 900)
				return U"DG Meta Serif Science";
			else
				return style == 0 ? U"Palatino"
				: style == Graphics_BOLD ? U"Palatino Bold"
				: style == Graphics_ITALIC ? U"Palatino Italic"
				: U"Palatino Bold Italic";
		case kGraphics_font_SYMBOL:
			return U"Symbol";
		case kGraphics_font_IPATIMES:
			return U"Doulos SIL";
		case kGraphics_font_IPAPALATINO:
			return
				style == 0 ? U"Charis SIL"
				: style == Graphics_BOLD ? U"Charis SIL Bold"
				: style == Graphics_ITALIC ? U"Charis SIL Italic"
				: U"Charis SIL Bold Italic";
		case kGraphics_font_DINGBATS:
			return U"Zapf Dingbats";
		default:
			return nullptr;
	}
}
static CTFontRef quartz_getFontRef (int font, int size, int style) {
	CTFontSymbolicTraits ctStyle = ( style & Graphics_BOLD ? kCTFontBoldTrait : 0 ) | ( style & Graphics_ITALIC ? kCTFontItalicTrait : 0 );
#if 1
	CFStringRef key = kCTFontSymbolicTrait;
	CFNumberRef value = CFNumberCreate (nullptr, kCFNumberIntType, & ctStyle);
	CFIndex numberOfValues = 1;
	CFDictionaryRef styleDict = CFDictionaryCreate (nullptr, (const void **) & key, (const void **) & value, numberOfValues,
		& kCFTypeDictionaryKeyCallBacks, & kCFTypeDictionaryValueCallBacks);
	CFRelease (value);
	CFStringRef keys [2];
	keys [0] = kCTFontTraitsAttribute;
	keys [1] = kCTFontNameAttribute;
	conststring32 fontName = quartz_getFontName (font, style);
	CFStringRef cfFont = (CFStringRef) Melder_peek32toCfstring (fontName);
	void *values [2] = { (void *) styleDict, (void *) cfFont };
	CFDictionaryRef attributes = CFDictionaryCreate (nullptr, (const void **) & keys, (const void **) & values, 2,
		& kCFTypeDictionaryKeyCallBacks, & kCFTypeDictionaryValueCallBacks);
	CFRelease (styleDict);
	CTFontDescriptorRef ctFontDescriptor = CTFontDescriptorCreateWithAttributes (attributes);
	CFRelease (attributes);
#else
	NSMutableDictionary *styleDict = [[NSMutableDictionary alloc] initWithCapacity: 1];
	[styleDict   setObject: [NSNumber numberWithUnsignedInt: ctStyle]   forKey: (id) kCTFontSymbolicTrait];
	NSMutableDictionary *attributes = [[NSMutableDictionary alloc] initWithCapacity: 2];
	[attributes   setObject: styleDict   forKey: (id) kCTFontTraitsAttribute];
	switch (font) {
		case (int) kGraphics_font::TIMES:       { [attributes   setObject: @"Times New Roman" forKey: (id) kCTFontNameAttribute]; } break;
		case (int) kGraphics_font::HELVETICA:   { [attributes   setObject: @"Arial"           forKey: (id) kCTFontNameAttribute]; } break;
		case (int) kGraphics_font::COURIER:     { [attributes   setObject: @"Courier New"     forKey: (id) kCTFontNameAttribute]; } break;
		case (int) kGraphics_font::PALATINO:    { if (Melder_debug == 900)
												[attributes   setObject: @"DG Meta Serif Science" forKey: (id) kCTFontNameAttribute];
										   else
												[attributes   setObject: @"Palatino"              forKey: (id) kCTFontNameAttribute];
										 } break;
		case kGraphics_font_SYMBOL:      { [attributes   setObject: @"Symbol"          forKey: (id) kCTFontNameAttribute]; } break;
		case kGraphics_font_IPATIMES:    { [attributes   setObject: @"Doulos SIL"      forKey: (id) kCTFontNameAttribute]; } break;
		case kGraphics_font_IPAPALATINO: { [attributes   setObject: @"Charis SIL"      forKey: (id) kCTFontNameAttribute]; } break;
		case kGraphics_font_DINGBATS:    { [attributes   setObject: @"Zapf Dingbats"   forKey: (id) kCTFontNameAttribute]; } break;
	}
	CTFontDescriptorRef ctFontDescriptor = CTFontDescriptorCreateWithAttributes ((CFMutableDictionaryRef) attributes);
	[styleDict release];
	[attributes release];
#endif
	CTFontRef ctFont = CTFontCreateWithFontDescriptor (ctFontDescriptor, size, nullptr);
	CFRelease (ctFontDescriptor);
	return ctFont;
}
#endif

static void charDraw (Graphics anyGraphics, int xDC, int yDC, _Graphics_widechar *lc,
	const char32 codes [], int nchars, int width)
{
	trace (U"nchars ", nchars, U" first ", (int) lc->kar, U" ", (char32) lc -> kar, U" rightToLeft ", lc->rightToLeft);
	if (anyGraphics -> postScript) {
		GraphicsPostscript me = static_cast <GraphicsPostscript> (anyGraphics);
		const bool onlyRegular = lc -> font.string [0] == 'S' ||
			(lc -> font.string [0] == 'T' && lc -> font.string [1] == 'e');   // Symbol & SILDoulos !
		const bool slant = (lc -> style & Graphics_ITALIC) && onlyRegular;
		const bool thick = (lc -> style & Graphics_BOLD) && onlyRegular;
		if (lc -> font.string != my lastFid || lc -> size != my lastSize)
			my d_printf (my d_file, my languageLevel == 1 ? "/%s %d FONT\n" : "/%s %d selectfont\n",
					my lastFid = lc -> font.string, my lastSize = lc -> size);
		if (lc -> link)
			my d_printf (my d_file, "0 0 1 setrgbcolor\n");
		for (int i = -3; i <= 3; i ++) {
			if (i != 0 && ! thick)
				continue;
			my d_printf (my d_file, "%d %d M ", xDC + i, yDC);
			if (my textRotation != 0.0 || slant) {
				my d_printf (my d_file, "gsave currentpoint translate ");
				if (my textRotation != 0.0)
					my d_printf (my d_file, "%.6g rotate 0 0 M\n", (double) my textRotation);
				if (slant)
					my d_printf (my d_file, "[1 0 0.25 1 0 0] concat 0 0 M\n");
			}
			my d_printf (my d_file, "(");
			const char32 *p = & codes [0];
			while (*p) {
				if (*p == U'(' || *p == U')' || *p == U'\\') {
					my d_printf (my d_file, "\\%c", (unsigned char) *p);
				} else if (*p >= 32 && *p <= 126) {
					my d_printf (my d_file, "%c", (unsigned char) *p);
				} else {
					my d_printf (my d_file, "\\%d%d%d", (unsigned char) *p / 64,
						((unsigned char) *p % 64) / 8, (unsigned char) *p % 8);
				}
				p ++;
			}
			my d_printf (my d_file, ") show\n");
			if (my textRotation != 0.0 || slant)
				my d_printf (my d_file, "grestore\n");
		}
		if (lc -> link)
			my d_printf (my d_file, "0 0 0 setrgbcolor\n");
	} else if (anyGraphics -> screen) {
		GraphicsScreen me = static_cast <GraphicsScreen> (anyGraphics);
		#if cairo
			if (! my d_cairoGraphicsContext)
				return;
			// TODO!
			if (lc -> link)
				_Graphics_setColour (me, Melder_BLUE);
			int font = lc -> font.integer_;
			cairo_save (my d_cairoGraphicsContext);
			cairo_translate (my d_cairoGraphicsContext, xDC, yDC);
			//cairo_scale (my d_cairoGraphicsContext, 1, -1);
			cairo_rotate (my d_cairoGraphicsContext, - my textRotation * NUMpi / 180.0);
			const char *codes8 = Melder_peek32to8 (codes);
			PangoFontDescription *font_description = PangoFontDescription_create (font, lc);
			PangoLayout *layout = pango_cairo_create_layout (my d_cairoGraphicsContext);
			pango_layout_set_font_description (layout, font_description);
			pango_layout_set_text (layout, codes8, -1);
			cairo_move_to (my d_cairoGraphicsContext, 0 /*xDC*/, 0 /*yDC*/);
			// instead of pango_cairo_show_layout we use pango_cairo_show_layout_line to
			// get the same text origin as cairo_show_text, i.e. baseline left, instead of Pango's top left!
			pango_cairo_show_layout_line (my d_cairoGraphicsContext, pango_layout_get_line_readonly (layout, 0));
			g_object_unref (layout);
			cairo_restore (my d_cairoGraphicsContext);
			if (lc -> link)
				_Graphics_setColour (me, my colour);
			return;
		#elif gdi
			const int font = lc -> font.integer_;
			conststringW codesW = Melder_peek32toW (codes);
			if (my duringXor) {
				/*
					On GDI, SetROP2 does not influence text drawing,
					so we have to create a bitmap in the background
					and use BitBlt with SRCINVERT as its ROP.
				*/
				const int descent = (1.0/216) * my fontSize * my resolution;
				const int ascent = (1.0/72) * my fontSize * my resolution;
				const int maxWidth = 800, maxHeight = 200;
				const int baseline = 100, top = baseline - ascent - 1, bottom = baseline + descent + 1;
				static bool inited = false;
				static HDC dc;
				static HBITMAP bitmap;
				if (! inited) {
					dc = CreateCompatibleDC (my d_gdiGraphicsContext);
					bitmap = CreateCompatibleBitmap (my d_gdiGraphicsContext, maxWidth, maxHeight);
					SelectBitmap (dc, bitmap);
					SetBkMode (dc, TRANSPARENT);   // not the default!
					SelectPen (dc, GetStockPen (BLACK_PEN));
					SelectBrush (dc, GetStockBrush (BLACK_BRUSH));
					SetTextAlign (dc, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
					inited = true;
				}
				width += 4;   // for slant
				Rectangle (dc, 0, top, width, bottom);
				SelectFont (dc, fonts [(int) my resolutionNumber] [font] [lc -> size] [lc -> style]);
				SetTextColor (dc, my d_winForegroundColour);
				TextOutW (dc, 0, baseline, codesW, str16len ((conststring16) codesW));
				BitBlt (my d_gdiGraphicsContext, xDC, yDC - ascent, width, bottom - top, dc, 0, top, SRCINVERT);
				return;
			}
			SelectPen (my d_gdiGraphicsContext, my d_winPen);
			SelectBrush (my d_gdiGraphicsContext, my d_winBrush);
			if (lc -> link)
				SetTextColor (my d_gdiGraphicsContext, RGB (0, 0, 255));
			else
				SetTextColor (my d_gdiGraphicsContext, my d_winForegroundColour);
			SelectFont (my d_gdiGraphicsContext, fonts [(int) my resolutionNumber] [font] [lc -> size] [lc -> style]);
			if (my textRotation == 0.0) {
				TextOutW (my d_gdiGraphicsContext, xDC, yDC, codesW, str16len ((const char16 *) codesW));
			} else {
				int restore = SaveDC (my d_gdiGraphicsContext);
				SetGraphicsMode (my d_gdiGraphicsContext, GM_ADVANCED);
				double a = my textRotation * NUMpi / 180.0, cosa = cos (a), sina = sin (a);
				XFORM rotate = { (float) cosa, (float) - sina, (float) sina, (float) cosa, 0.0, 0.0 };
				ModifyWorldTransform (my d_gdiGraphicsContext, & rotate, MWT_RIGHTMULTIPLY);
				XFORM translate = { 1.0, 0.0, 0.0, 1.0, (float) xDC, (float) yDC };
				ModifyWorldTransform (my d_gdiGraphicsContext, & translate, MWT_RIGHTMULTIPLY);
				TextOutW (my d_gdiGraphicsContext, 0, 0, codesW, str16len ((const char16 *) codesW));
				RestoreDC (my d_gdiGraphicsContext, restore);
			}
			if (lc -> link)
				SetTextColor (my d_gdiGraphicsContext, my d_winForegroundColour);
			SelectPen (my d_gdiGraphicsContext, GetStockPen (BLACK_PEN)), SelectBrush (my d_gdiGraphicsContext, GetStockBrush (NULL_BRUSH));
			return;
		#elif quartz
			/*
				Determine the font family.
			*/
			const int font = lc -> font.integer_;   // the font of the first character

			/*
				Determine the style.
			*/
			const int style = lc -> style;   // the style of the first character

			/*
				Determine the font-style combination.
			*/
			CTFontRef ctFont = theScreenFonts [font] [lc -> size] [style];
			if (! ctFont)
				theScreenFonts [font] [lc -> size] [style] = ctFont =
					quartz_getFontRef (font, lc -> size, style);

			const char16 *codes16 = Melder_peek32to16 (codes);
			#if 1
				CFStringRef s = CFStringCreateWithBytes (nullptr,
					(const UInt8 *) codes16, str16len (codes16) * 2,
					kCFStringEncodingUTF16LE, false);
				integer length = CFStringGetLength (s);
			#else
				NSString *s = [[NSString alloc]   initWithBytes: codes16   length: str16len (codes16) * 2   encoding: NSUTF16LittleEndianStringEncoding];
				integer length = [s length];
			#endif

			CGFloat descent = CTFontGetDescent (ctFont);

            CFMutableAttributedStringRef string = CFAttributedStringCreateMutable (kCFAllocatorDefault, length);
            CFAttributedStringReplaceString (string, CFRangeMake (0, 0), (CFStringRef) s);
            CFRange textRange = CFRangeMake (0, length);
            CFAttributedStringSetAttribute (string, textRange, kCTFontAttributeName, ctFont);

			/*
				We don't set kerning explicitly, so that Praat will use standard kerning.
			*/

			static CTParagraphStyleRef paragraphStyle;
			if (! paragraphStyle) {
				CTTextAlignment textAlignment = kCTLeftTextAlignment;
				CTParagraphStyleSetting paragraphSettings [1] = { { kCTParagraphStyleSpecifierAlignment, sizeof (CTTextAlignment), & textAlignment } };
				paragraphStyle = CTParagraphStyleCreate (paragraphSettings, 1);
				Melder_assert (paragraphStyle != nullptr);
			}
            CFAttributedStringSetAttribute (string, textRange, kCTParagraphStyleAttributeName, paragraphStyle);

            MelderColour colour = lc -> link ? Melder_BLUE : my colour;
            CGColorRef color = CGColorCreateGenericRGB (colour.red, colour.green, colour.blue, 1.0);
			Melder_assert (color != nullptr);
            CFAttributedStringSetAttribute (string, textRange, kCTForegroundColorAttributeName, color);

            /*
            	Draw.
			*/
    
            CGContextSetTextMatrix (my d_macGraphicsContext, CGAffineTransformIdentity);   // this could set the "current context" for CoreText
            CFRelease (color);

            CGContextSaveGState (my d_macGraphicsContext);
            CGContextTranslateCTM (my d_macGraphicsContext, xDC, yDC);
            if (my yIsZeroAtTheTop)
            	CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
            CGContextRotateCTM (my d_macGraphicsContext, my textRotation * NUMpi / 180.0);

			CTLineRef line = CTLineCreateWithAttributedString (string);
			CTLineDraw (line, my d_macGraphicsContext);
			CFRelease (line);
            CGContextRestoreGState (my d_macGraphicsContext);

            // Clean up
            CFRelease (string);
			CFRelease (s);
			//CFRelease (ctFont);
			return;
		#endif
	}
}

#define MAX_LINK_LENGTH  300

static integer bufferSize;
static _Graphics_widechar *theWidechar;
static char32 *charCodes;
static int initBuffer (conststring32 txt) {
	try {
		integer sizeNeeded = str32len (txt) + 1;
		if (sizeNeeded > bufferSize) {
			sizeNeeded += sizeNeeded / 2 + 100;
			Melder_free (theWidechar);
			Melder_free (charCodes);
			theWidechar = Melder_calloc (_Graphics_widechar, sizeNeeded);
			charCodes = Melder_calloc (char32, sizeNeeded);
			bufferSize = sizeNeeded;
		}
		return 1;
	} catch (MelderError) {
		bufferSize = 0;
		Melder_flushError ();
		return 0;
	}
}

static int numberOfLinks = 0;
static Graphics_Link links [100];    // a maximum of 100 links per string

static void charSizes (Graphics me, _Graphics_widechar string [], bool measureEachCharacterSeparately) {
	/*
		Ideally, this function should work even in cases where there is no screen.
		Example: a Praat script wants to draw a text inside a rectangle
		and determines the width of the rectangle by means of the "Text width (mm)" command.
		Ideally, this script should run correctly from the command line.
		On the Mac, `CTFramesetterSuggestFrameSizeWithConstraints` works correctly from the command line,
		but on Linux, `pango_layout_get_extents` does not work if there is no d_cairoGraphicsContext.
		(last checked 2020-07-17)
	*/
	if (my postScript || (cairo && ! my screen)) {   // TODO: use Pango measurements even without Cairo context (if no screen)
		for (_Graphics_widechar *character = string; character -> kar > U'\t'; character ++)
			charSize (me, character);
	} else {
	/*
		Measure the size of each character.
	*/
	_Graphics_widechar *character;
	#if quartz || cairo
		#if cairo
			if (! ((GraphicsScreen) me) -> d_cairoGraphicsContext)
				return;
		#endif
		int numberOfDiacritics = 0;
		for (_Graphics_widechar *lc = string; lc -> kar > U'\t'; lc ++) {
			/*
				Determine the font family.
			*/
			Longchar_Info info = lc -> karInfo;
			Melder_assert (info);
			int font = chooseFont (me, lc);
			lc -> font.string = nullptr;   // this erases font.integer_!

			/*
				Determine the style.
			*/
			int style = lc -> style;
			Melder_assert (style >= 0 && style <= Graphics_BOLD_ITALIC);

			int normalSize = my fontSize * my resolution / 72.0;
			int smallSize = (3 * normalSize + 2) / 4;
			int size = ( lc -> size < 100 ? smallSize : normalSize );
			lc -> size = size;
			lc -> baseline *= 0.01 * normalSize;
			lc -> code = lc -> kar;
			lc -> font.integer_ = font;
			if (Longchar_Info_isDiacritic (info))
				numberOfDiacritics ++;

			#if quartz
				/*
					Determine and store the font-style combination.
				*/
				CTFontRef ctFont = theScreenFonts [font] [size] [style];
				if (! ctFont)
					theScreenFonts [font] [size] [style] = ctFont =
						quartz_getFontRef (font, size, style);
			#endif
		}
		int nchars = 0;
		for (_Graphics_widechar *lc = string; lc -> kar > U'\t'; lc ++) {
			charCodes [nchars ++] = lc -> code;
			_Graphics_widechar *next = lc + 1;
			lc -> width = 0;
			if (measureEachCharacterSeparately ||
				next->kar <= U' ' || next->style != lc->style ||
				next->baseline != lc->baseline || next->size != lc->size || next->link != lc->link ||
				next->font.integer_ != lc->font.integer_ || next->font.string != lc->font.string ||
				next->rightToLeft != lc->rightToLeft ||
				(my textRotation != 0.0 && my screen && my resolution > 150))
			{
				charCodes [nchars] = U'\0';
				#if cairo
					const char *codes8 = Melder_peek32to8 (charCodes);
					int length = strlen (codes8);
					PangoFontDescription *fontDescription = PangoFontDescription_create (lc -> font.integer_, lc);

					/*
						Measuring the width of a text with a homogeneous Praat font
						should still allow for Pango's font substitution.
						Low-level sequences such as `pango_itemize--pango_shape--pango_glyph_string_get_width`
						or `pango_itemize--pango_shape--pango_font_map_load_font--pango_glyph_string_extents`
						don't accomplish this: they seem to compute the width solely on the
						basis of the (perhaps substituted) font of the *first* glyph. By contrast, a PangoLayout
						performs font substitution when drawing with `pango_cairo_show_layout_line`,
						and also when measuring the width with `pango_layout_get_extents`.
						Fortunately, a PangoLayout is 1.5 to 2 times faster than the two low-level methods
						(measured 20170527).
					*/
					Melder_assert (my screen);
					PangoLayout *layout = pango_cairo_create_layout (((GraphicsScreen) me) -> d_cairoGraphicsContext);
					pango_layout_set_font_description (layout, fontDescription);
					pango_layout_set_text (layout, codes8, -1);
					PangoRectangle inkRect, logicalRect;
					pango_layout_get_extents (layout, & inkRect, & logicalRect);
					lc -> width = logicalRect. width / PANGO_SCALE;
					Melder_assert (logicalRect.x == 0);
					g_object_unref (layout);
				#elif quartz
					const char16 *codes16 = Melder_peek32to16 (charCodes);
					int64 length = str16len (codes16);

					NSString *s = [[NSString alloc]
						initWithBytes: codes16
						length: (NSUInteger) (length * 2)
						encoding: NSUTF16LittleEndianStringEncoding   // BUG: should be NSUTF16NativeStringEncoding, except that that doesn't exist
						];

					CFRange textRange = CFRangeMake (0, (CFIndex) [s length]);

					CFMutableAttributedStringRef cfstring =
						CFAttributedStringCreateMutable (kCFAllocatorDefault, (CFIndex) [s length]);
					CFAttributedStringReplaceString (cfstring, CFRangeMake (0, 0), (CFStringRef) s);
					CFAttributedStringSetAttribute (cfstring, textRange, kCTFontAttributeName, theScreenFonts [lc -> font.integer_] [lc -> size] [lc -> style]);

					/*
					 * Measure.
					 */

					// Create a path to render text in
					CGMutablePathRef path = CGPathCreateMutable ();
					NSRect measureRect = NSMakeRect (0, 0, CGFLOAT_MAX, CGFLOAT_MAX);
					CGPathAddRect (path, nullptr, (CGRect) measureRect);
				
					CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString ((CFAttributedStringRef) cfstring);
					CFRange fitRange;
					CGSize targetSize = CGSizeMake (lc -> width, CGFLOAT_MAX);
					CGSize frameSize = CTFramesetterSuggestFrameSizeWithConstraints (framesetter, textRange, nullptr, targetSize, & fitRange);
					CFRelease (framesetter);
					CFRelease (cfstring);
					[s release];
					CFRelease (path);
					//Longchar_Info info = lc -> karInfo;
					//bool isDiacritic = ( info -> ps.times == 0 );
					//lc -> width = ( isDiacritic ? 0.0 : frameSize.width * lc -> size / 100.0 );
					lc -> width = frameSize.width /* * lc -> size / 100.0 */;
					if (Melder_systemVersion >= 101100) {
						/*
						 * If the text ends in a space, CTFramesetterSuggestFrameSizeWithConstraints() ignores the space.
						 * we correct for this.
						 */
						if (codes16 [length - 1] == u' ')
							lc -> width += 25.0 * lc -> size / 100.0;
					}
				#endif
				nchars = 0;
			}
		}
	#else
		for (character = string; character -> kar > U'\t'; character ++)
			charSize (me, character);
	#endif
	}
	/*
	 * Each character has been garnished with information about the character's width.
	 * Make a correction for systems that make slanted characters overlap the character box to their right.
	 * We must do this after the previous loop, because we query the size of the *next* character.
	 *
	 * Keep this in SYNC with psTextWidth.
	 */
	for (_Graphics_widechar *character = string; character -> kar > U'\t'; character ++) {
		if ((character -> style & Graphics_ITALIC) != 0) {
			_Graphics_widechar *nextCharacter = character + 1;
			if (nextCharacter -> kar <= U'\t') {
				character -> width += SCREEN_SLANT_CORRECTION / 72 * my fontSize * my resolution;
			} else if (((nextCharacter -> style & Graphics_ITALIC) == 0 && nextCharacter -> baseline >= character -> baseline)
				|| (character -> baseline == 0 && nextCharacter -> baseline > 0))
			{
				if (nextCharacter -> kar == U'.' || nextCharacter -> kar == U',')
					character -> width += SCREEN_SLANT_CORRECTION / 144 * my fontSize * my resolution;
				else
					character -> width += SCREEN_SLANT_CORRECTION / 72 * my fontSize * my resolution;
			}
		}
	}
}

/*
 * The routine textWidth determines the fractional width of a text, in device coordinates.
 */
static double textWidth (_Graphics_widechar string []) {
	double width = 0.0;
	for (_Graphics_widechar *character = string; character -> kar > U'\t'; character ++)
		width += character -> width;
	return width;
}

static void drawOneCell (Graphics me, int xDC, int yDC, _Graphics_widechar lc []) {
	int nchars = 0;
	const double width = textWidth (lc);
	/*
	 * We must continue even if width is zero (for adjusting textY).
	 */
	_Graphics_widechar *plc, *lastlc;
	bool inLink = false;
	double dx, dy;
	switch (my horizontalTextAlignment) {
		case (int) Graphics_LEFT:      dx = 1.0 + (0.1/72.0) * my fontSize * my resolution; break;
		case (int) Graphics_CENTRE:    dx = - width / 2.0; break;
		case (int) Graphics_RIGHT:     dx = ( width != 0.0 ? - width - (0.1/72.0) * my fontSize * my resolution : 0 ); break;   // if width is zero, do not step left
		default:                 dx = 1 + (0.1/72.0) * my fontSize * my resolution; break;
	}
	switch (my verticalTextAlignment) {
		case Graphics_BOTTOM:    dy = (0.4/72.0) * my fontSize * my resolution; break;
		case Graphics_HALF:      dy = (-0.3/72.0) * my fontSize * my resolution; break;
		case Graphics_TOP:       dy = (-1.0/72.0) * my fontSize * my resolution; break;
		case Graphics_BASELINE:  dy = 0.0; break;
		default:                 dy = 0.0; break;
	}
	if (my textRotation != 0.0) {
		double xbegin = dx, x = xbegin, cosa, sina;
		if (my textRotation == 90.0f) {
			cosa = 0.0;
			sina = 1.0;
		} else if (my textRotation == 270.0f) {
			cosa = 0.0;
			sina = -1.0;
		} else {
			const double angle = my textRotation * NUMpi / 180.0;
			cosa = cos (angle);
			sina = sin (angle);
		}
		for (plc = lc; plc -> kar > U'\t'; plc ++) {
			_Graphics_widechar *next = plc + 1;
			charCodes [nchars ++] = plc -> code;   // buffer...
			x += plc -> width;
			/*
			 * We can draw stretches of characters:
			 * they have different styles, baselines, sizes, or fonts,
			 * or if there is a break between them,
			 * or if we cannot rotate multiple characters,
			 * which is the case on bitmap printers.
			 */
			if (next->kar < U' ' || next->style != plc->style ||
				next->baseline != plc->baseline || next->size != plc->size ||
				next->font.integer_ != plc->font.integer_ || next->font.string != plc->font.string ||
				next->rightToLeft != plc->rightToLeft ||
				(my screen && my resolution > 150))
			{
				const double dy2 = dy + plc -> baseline;
				const double xr = cosa * xbegin - sina * dy2;
				const double yr = sina * xbegin + cosa * dy2;
				charCodes [nchars] = U'\0';   // ...and flush
				charDraw (me, xDC + xr, my yIsZeroAtTheTop ? yDC - yr : yDC + yr,
					plc, charCodes, nchars, x - xbegin);
				nchars = 0;
				xbegin = x;
			}
		}
	} else {
		double xbegin = xDC + dx, x = xbegin, y = ( my yIsZeroAtTheTop ? yDC - dy : yDC + dy );   // all mutable
		lastlc = lc;
		if (my wrapWidth != 0.0) {
			/*
			 * Replace some spaces with new-line symbols.
			 */
			int xmax = xDC + my wrapWidth * my scaleX;
			for (plc = lc; plc -> kar >= U' '; plc ++) {
				x += plc -> width;
				if (x > xmax) {   // wrap (if wrapWidth is too small, each word will be on a separate line)
					while (plc >= lastlc) {
						if (plc -> kar == U' ' && ! plc -> link)   // keep links contiguous
							break;
						plc --;
					}
					if (plc <= lastlc)
						break;   // hopeless situation: no spaces; get over it
					lastlc = plc;
					plc -> kar = U'\n';   // replace space with newline
					#if quartz || cairo
						if (my screen) {
							/*
								This part is needed when using the non-`charSize()` variant of `charSizes()`,
								because otherwise you'll see extra spaces
								before the first font switch on each non-initial line.
							*/
							_Graphics_widechar *next = plc + 1;
							if (next->style != plc->style ||
								next->baseline != plc->baseline || next->size != plc->size || next->link != plc->link ||
								next->font.integer_ != plc->font.integer_ || next->font.string != plc->font.string ||
								next->rightToLeft != plc->rightToLeft)
							{
								// nothing
							} else {
								next -> width -= 0.25 * my fontSize * my resolution / 72.0;   // subtract the width of one space
							}
						}
					#endif
					x = xDC + dx + my secondIndent * my scaleX;
				}
			}
			xbegin = x = xDC + dx;   // re-initialize for second pass
		}
		for (plc = lc; plc -> kar > U'\t'; plc ++) {
			_Graphics_widechar *next = plc + 1;
			if (plc -> link) {
				if (! inLink) {
					const double descent = ( my yIsZeroAtTheTop ? -(0.3/72) : (0.3/72) ) * my fontSize * my resolution;
					links [++ numberOfLinks]. x1 = x;
					links [numberOfLinks]. y1 = y - descent;
					links [numberOfLinks]. y2 = y + 3 * descent;
					inLink = true;
				}
			} else if (inLink) {
				links [numberOfLinks]. x2 = x;
				inLink = false;
			}
			if (plc -> kar == U'\n') {
				xbegin = x = xDC + dx + my secondIndent * my scaleX;
				y = ( my yIsZeroAtTheTop ? y + (1.2/72) * my fontSize * my resolution : y - (1.2/72) * my fontSize * my resolution );
			} else {
				charCodes [nchars ++] = plc -> code;   // buffer...
				x += plc -> width;
				if (next->kar < U' ' || next->style != plc->style ||
					next->baseline != plc->baseline || next->size != plc->size || next->link != plc->link ||
					next->font.integer_ != plc->font.integer_ || next->font.string != plc->font.string ||
					next->rightToLeft != plc->rightToLeft)
				{
					charCodes [nchars] = U'\0';   // ...and flush
					charDraw (me, xbegin, my yIsZeroAtTheTop ? y - plc -> baseline : y + plc -> baseline,
						plc, charCodes, nchars, x - xbegin);
					nchars = 0;
					xbegin = x;
				}
			}
		}
		if (inLink) {
			links [numberOfLinks]. x2 = x;
			inLink = false;
		}
		my textX = (x - my deltaX) / my scaleX;
		my textY = (( my yIsZeroAtTheTop ? y + dy : y - dy ) - my deltaY) / my scaleY;
	}
}

static struct { double width; kGraphics_horizontalAlignment alignment; } tabs [1 + 20] = { { 0, Graphics_CENTRE },
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
		charSizes (me, plc, false);
		drawOneCell (me, xWC * my scaleX + my deltaX, yWC * my scaleY + my deltaY, plc);
		while (plc -> kar != U'\0' && plc -> kar != U'\t') plc ++;   // find end of cell
		if (plc -> kar == U'\0')   // end of text?
			break;
		if (plc -> kar == U'\t') {   // go to next cell
			xWC += ( tabs [itab]. alignment == Graphics_LEFT ? tabs [itab]. width :
				tabs [itab]. alignment == Graphics_CENTRE ? 0.5 * tabs [itab]. width : 0 ) * my fontSize / 12.0;
			itab ++;
			xWC += ( tabs [itab]. alignment == Graphics_LEFT ? 0 :
				tabs [itab]. alignment == Graphics_CENTRE ? 0.5 * tabs [itab]. width : tabs [itab]. width ) * my fontSize / 12.0;
			my horizontalTextAlignment = (int) tabs [itab]. alignment;
			my wrapWidth = tabs [itab]. width * my fontSize / 12.0;
		}
	}
	my horizontalTextAlignment = saveTextAlignment;
	my wrapWidth = saveWrapWidth;
}

static void parseTextIntoCellsLinesRuns (Graphics me, conststring32 txt /* cattable */, _Graphics_widechar a_widechar []) {
	char32 kar;
	const char32 *in = & txt [0];
	int nquote = 0;
	_Graphics_widechar *out = & a_widechar [0];
	bool charSuperscript = false, charSubscript = false, charItalic = false, charBold = false;
	bool wordItalic = false, wordBold = false, wordCode = false, wordLink = false;
	bool globalSuperscript = false, globalSubscript = false, globalItalic = false, globalBold = false, globalCode = false, globalLink = false;
	bool globalSmall = 0;
	numberOfLinks = 0;
	while ((kar = *in++) != U'\0') {
		if (kar == U'^' && my circumflexIsSuperscript) {
			if (globalSuperscript) globalSuperscript = 0;
			else if (in [0] == U'^') { globalSuperscript = 1; in ++; }
			else charSuperscript = 1;
			wordItalic = wordBold = wordCode = false;
			continue;
		} else if (kar == U'_' && my underscoreIsSubscript) {
			if (globalSubscript) { globalSubscript = false; wordItalic = wordBold = wordCode = false; continue; }
			else if (in [0] == U'_') { globalSubscript = true; in ++; wordItalic = wordBold = wordCode = false; continue; }
			else if (! my dollarSignIsCode) { charSubscript = true; wordItalic = wordBold = wordCode = false; continue; }   // not in manuals
			else
				;   // a normal underscore in manuals
		} else if (kar == U'%' && my percentSignIsItalic) {
			if (globalItalic) globalItalic = false;
			else if (in [0] == U'%') { globalItalic = true; in ++; }
			else if (my dollarSignIsCode) wordItalic = true;   // in manuals
			else charItalic = true;
			continue;
		} else if (kar == U'#' && my numberSignIsBold) {
			if (globalBold) globalBold = false;
			else if (in [0] == U'#') { globalBold = true; in ++; }
			else if (my dollarSignIsCode) wordBold = true;   // in manuals
			else charBold = true;
			continue;
		} else if (kar == U'$' && my dollarSignIsCode) {
			if (globalCode) globalCode = false;
			else if (in [0] == U'$') { globalCode = true; in ++; }
			else wordCode = true;
			continue;
		} else if (kar == U'@' && my atSignIsLink   // recognize links
		           && my textRotation == 0.0)   // no links allowed in rotated text, because links are identified by 2-point rectangles
		{
			char32 *to, *max;
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
				globalLink = false;   // close the drawn link text (the normal colour will take over)
				continue;   // the '@' must not be drawn
			} else if (in [0] == U'@') {
				/*
				 * Detected the second '@' in strings like "@@Link with spaces@".
				 * A format like "@@Page linked to|Text shown in blue@" is permitted.
				 * First step: collect the page text (the link information);
				 * it is everything between "@@" and "|" or "@" or end of string.
				 */
				const char32 *from = in + 1;   // start with first character after "@@"
				if (! links [++ numberOfLinks]. name)   // make room for saving link info
					links [numberOfLinks]. name = Melder_calloc_f (char32, MAX_LINK_LENGTH + 1);
				to = links [numberOfLinks]. name, max = to + MAX_LINK_LENGTH;
				while (*from && *from != U'@' && *from != U'|' && to < max)   // until end-of-string or '@' or '|'...
					* to ++ = * from ++;   // ... copy one character
				*to = U'\0';   // close saved link info
				/*
				 * Second step: collect the link text that is to be drawn.
				 * Its characters will be collected during the normal cycles of the loop.
				 * If the link info is equal to the link text, no action is needed.
				 * If, on the other hand, there is a separate link info, this will have to be skipped.
				 */
				if (*from == U'|')
					in += to - links [numberOfLinks]. name + 1;   // skip link info + '|'
				/*
				 * We are entering the link-text-collection mode.
				 */
				globalLink = true;
				/*
				 * Both '@' must be skipped and must not be drawn.
				 */
				in ++;   // skip second '@'
				continue;   // do not draw
			} else {
				/*
				 * Detected a single-word link, like in "this is a @Link that consists of one word".
				 * First step: collect the page text: letters, digits, and underscores.
				 */
				const char32 *from = in;   // start with first character after "@"
				if (! links [++ numberOfLinks]. name)   // make room for saving link info
					links [numberOfLinks]. name = Melder_calloc_f (char32, MAX_LINK_LENGTH + 1);
				to = links [numberOfLinks]. name;
				max = to + MAX_LINK_LENGTH;
				while (*from && (Melder_isWordCharacter (*from) || *from == U'_') && to < max)   // until end-of-word...
					*to ++ = *from++;   // ... copy one character
				*to = U'\0';   // close saved link info
				/*
				 * Second step: collect the link text that is to be drawn.
				 * Its characters will be collected during the normal cycles of the loop.
				 * The link info is equal to the link text, so no skipping is needed.
				 */
				wordLink = true;   // enter the single-word link-text-collection mode
			}
			continue;
		} else if (kar == U'\\') {
			/*
			 * Detected backslash sequence: backslash + kar1 + kar2...
			 */
			char32 kar1, kar2;
			/*
			 * ... except if kar1 or kar2 is null: in that case, draw the backslash.
			 */
			if (! (kar1 = in [0]) || ! (kar2 = in [1])) {
				;   // normal backslash symbol
			/*
			 * Catch "\s{", which means: small characters until corresponding '}'.
			 */
			} else if (kar2 == U'{') {
				if (kar1 == U's')
					globalSmall = true;
				in += 2;
				continue;
			/*
			 * Default action: translate the backslash sequence into the long character 'kar1,kar2'.
			 */
			} else {
				kar = Longchar_getInfo (kar1, kar2) -> unicode;
				in += 2;
			}
		} else if (kar == U'\"') {
			if (! (my font == kGraphics_font::COURIER || my fontStyle == Graphics_CODE || wordCode || globalCode))
				kar = ++nquote & 1 ? UNICODE_LEFT_DOUBLE_QUOTATION_MARK : UNICODE_RIGHT_DOUBLE_QUOTATION_MARK;
		} else if (kar == U'\'') {
			kar = UNICODE_RIGHT_SINGLE_QUOTATION_MARK;
		} else if (kar == U'`') {
			kar = UNICODE_LEFT_SINGLE_QUOTATION_MARK;
		} else if (kar >= 32 && kar <= 126) {
			if (kar == U'f') {
				if (in [0] == U'i' && HAS_FI_AND_FL_LIGATURES && ! (my font == kGraphics_font::COURIER || my fontStyle == Graphics_CODE || wordCode || globalCode)) {
					kar = UNICODE_LATIN_SMALL_LIGATURE_FI;
					in ++;
				} else if (in [0] == U'l' && HAS_FI_AND_FL_LIGATURES && ! (my font == kGraphics_font::COURIER || my fontStyle == Graphics_CODE || wordCode || globalCode)) {
					kar = UNICODE_LATIN_SMALL_LIGATURE_FL;
					in ++;
				}
			} else if (kar == U'}') {
				if (globalSmall) { globalSmall = 0; continue; }
			}
		} else if (kar == U'\t') {
			out -> kar = U'\t';
			out -> rightToLeft = false;
			wordItalic = wordBold = wordCode = wordLink = false;
			globalSubscript = globalSuperscript = globalItalic = globalBold = globalCode = globalLink = globalSmall = false;
			charItalic = charBold = charSuperscript = charSubscript = false;
			out ++;
			continue;   // do not draw
		} else if (kar == U'\n') {
			kar = U' ';
		}
		if (wordItalic | wordBold | wordCode | wordLink) {
			if (! Melder_isWordCharacter (kar) && kar != U'_')
				wordItalic = wordBold = wordCode = wordLink = false;
		}
		out -> style =
			(wordLink | globalLink) && my fontStyle != Graphics_CODE ? Graphics_BOLD :
			((my fontStyle & Graphics_ITALIC) | charItalic | wordItalic | globalItalic ? Graphics_ITALIC : 0) +
			((my fontStyle & Graphics_BOLD) | charBold | wordBold | globalBold ? Graphics_BOLD : 0);
		out -> font.string = nullptr;
		out -> font.integer_ = my fontStyle == Graphics_CODE || wordCode || globalCode ||
			(kar == U'/' || kar == U'|') && my font != kGraphics_font::PALATINO ? (int) kGraphics_font::COURIER : (int) my font;
		out -> link = wordLink | globalLink;
		out -> baseline = charSuperscript | globalSuperscript ? 34 : charSubscript | globalSubscript ? -25 : 0;
		out -> size = globalSmall || out -> baseline != 0 ? 80 : 100;
		if (kar == U'/' && my font != kGraphics_font::PALATINO) {
			out -> baseline -= out -> size / 12;
			out -> size += out -> size / 10;
			if (my screen)
				out -> font.integer_ = (int) kGraphics_font::PALATINO;
		}
		out -> code = U'?';   // does this have any meaning?
		Melder_assert (kar != U'\0');
		out -> kar = kar;
		out -> karInfo = Longchar_getInfoFromNative (kar);
		Melder_assert (out -> karInfo);
		out -> rightToLeft =
			(kar >= 0x0590 && kar <= 0x06FF) ||
			(kar >= 0xFE70 && kar <= 0xFEFF) ||
			(kar >= 0xFB1E && kar <= 0xFDFF);
		charItalic = charBold = charSuperscript = charSubscript = false;
		out ++;
	}
	out -> kar = U'\0';   // end of text
	out -> karInfo = Longchar_getInfoFromNative (kar);
	Melder_assert (out -> karInfo);
	out -> rightToLeft = false;
}

double Graphics_textWidth (Graphics me, conststring32 txt) {
	if (! initBuffer (txt))
		return 0.0;
	//Melder_casual (U"Graphics_textWidth: workstation viewport ", my d_x1DC, U" ", my d_x2DC, U" ", my d_y1DC, U" ", my d_y2DC);
	//Melder_casual (U"Graphics_textWidth: workstation window ", my d_x1wNDC, U" ", my d_x2wNDC, U" ", my d_y1wNDC, U" ", my d_y2wNDC);
	//Melder_casual (U"Graphics_textWidth: viewport ", my d_x1NDC, U" ", my d_x2NDC, U" ", my d_y1NDC, U" ", my d_y2NDC);
	//Melder_casual (U"Graphics_textWidth: window ", my d_x1WC, U" ", my d_x2WC, U" ", my d_y1WC, U" ", my d_y2WC);
	#if cairo
		Melder_assert (Thing_isa (me, classGraphicsScreen));
		cairo_t *oldCairoGraphicsContext = ((GraphicsScreen) me) -> d_cairoGraphicsContext;
		cairo_surface_t *cairoSurface;
		if (! oldCairoGraphicsContext)
			#ifndef NO_GUI
			if (((GraphicsScreen) me) -> d_window) {
				//Melder_casual (U"Graphics_textWidth: creating a graphics context in an existing widget.");
				((GraphicsScreen) me) -> d_cairoGraphicsContext = gdk_cairo_create (((GraphicsScreen) me) -> d_window);
			} else
			#endif
			{
				//Melder_casual (U"Graphics_textWidth: creating a graphics context outside any existing widget.");
				cairoSurface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 1/*my d_x2DC - my d_x1DC*/, 1/*my d_y2DC - my d_y1DC*/);
				((GraphicsScreen) me) -> d_cairoGraphicsContext = cairo_create (cairoSurface);
			}
	#endif
	parseTextIntoCellsLinesRuns (me, txt, theWidechar);
	charSizes (me, theWidechar, false);
	const double width = textWidth (theWidechar);
	//Melder_casual (U"Graphics_textWidth: width ", width, U", scale ", my scaleX);
	#if cairo
		if (! oldCairoGraphicsContext) {
			#ifndef NO_GUI
			if (((GraphicsScreen) me) -> d_window) {
				cairo_destroy (((GraphicsScreen) me) -> d_cairoGraphicsContext);
			} else
			#endif
			{
				cairo_destroy (((GraphicsScreen) me) -> d_cairoGraphicsContext);
				cairo_surface_destroy (cairoSurface);
			}
			((GraphicsScreen) me) -> d_cairoGraphicsContext = nullptr;
		}
	#endif
	return width / my scaleX;
}

void Graphics_textRect (Graphics me, double x1, double x2, double y1, double y2, conststring32 txt) {
	_Graphics_widechar *plc, *startOfLine;
	double width = 0.0, lineHeight = (1.1 / 72) * my fontSize * my resolution;
	const integer x1DC = x1 * my scaleX + my deltaX + 2, x2DC = x2 * my scaleX + my deltaX - 2;
	const integer y1DC = y1 * my scaleY + my deltaY, y2DC = y2 * my scaleY + my deltaY;
	const int availableHeight = ( my yIsZeroAtTheTop ? y1DC - y2DC : y2DC - y1DC ), availableWidth = x2DC - x1DC;
	const int linesAvailable = Melder_clippedLeft (1, int (availableHeight / lineHeight));
	if (availableWidth <= 0)
		return;
	if (! initBuffer (txt))
		return;
	parseTextIntoCellsLinesRuns (me, txt, theWidechar);
	charSizes (me, theWidechar, true);
	int linesNeeded = 1;
	for (plc = theWidechar; plc -> kar > U'\t'; plc ++) {
		width += plc -> width;
		if (width > availableWidth) {
			if (++ linesNeeded > linesAvailable)
				break;
			width = 0.0;
		}	
	}
	const int lines = Melder_clippedRight (linesNeeded, linesAvailable);
	startOfLine = theWidechar;
	for (int iline = 1; iline <= lines; iline ++) {
		width = 0.0;
		for (plc = startOfLine; plc -> kar > U'\t'; plc ++) {
			bool flush = false;
			width += plc -> width;
			if (width > availableWidth)
				flush = true;
			/*
				Trick for incorporating end-of-text.
			*/
			if (! flush && plc [1]. kar <= U'\t') {
				Melder_assert (iline == lines);
				plc ++;   // brr
				flush = true;
			}
			if (flush) {
				const char32 saveKar = plc -> kar;
				const int direction = ( my yIsZeroAtTheTop ? -1 : 1 );
				const int x = (
					my horizontalTextAlignment == (int) Graphics_LEFT ?
						x1DC
					: my horizontalTextAlignment == (int) Graphics_RIGHT ?
						x2DC
					:
						0.5 * (x1 + x2) * my scaleX + my deltaX
				);
				const int y = (
					my verticalTextAlignment == Graphics_BOTTOM ?
						y1DC + direction * (lines - iline) * lineHeight
					: my verticalTextAlignment == Graphics_TOP ?
						y2DC - direction * (iline - 1) * lineHeight
					:
						0.5 * (y1 + y2) * my scaleY + my deltaY + 0.5 * direction * (lines - iline*2 + 1) * lineHeight
				);
				plc -> kar = U'\0';
				drawOneCell (me, x, y, startOfLine);
				plc -> kar = saveKar;
				startOfLine = plc;
				break;
			}
		}
	}
}

void Graphics_text (Graphics me, double xWC, double yWC, conststring32 txt) {
	if (my recording) {
		const conststring8 txt_utf8 = Melder_peek32to8 (txt);
		const int length = strlen (txt_utf8) / sizeof (double) + 1;
		op (TEXT, 3 + length); put (xWC); put (yWC); sput (txt_utf8, length)
	} else {
		if (my wrapWidth == 0.0 && str32chr (txt, U'\n') && my textRotation == 0.0) {
			const double lineSpacingWC = (1.2/72.0) * my fontSize * my resolution / fabs (my scaleY);
			integer numberOfLines = 1;
			for (const char32 *p = & txt [0]; *p != U'\0'; p ++) {
				if (*p == U'\n')
					numberOfLines ++;
			}
			yWC += (
				my verticalTextAlignment == Graphics_TOP ?
					0.0
				: my verticalTextAlignment == Graphics_HALF ?
					0.5 * (numberOfLines - 1) * lineSpacingWC
				:
					(numberOfLines - 1) * lineSpacingWC
			);
			autostring32 linesToDraw = Melder_dup_f (txt);
			const char32 *p = & linesToDraw [0];
			for (;;) {
				char32 * const newline = str32chr (p, U'\n');
				if (newline)
					*newline = U'\0';
				Graphics_text (me, xWC, yWC, p);
				yWC -= lineSpacingWC;
				if (newline)
					p = newline + 1;
				else
					break;
			}
			return;
		}
		if (! initBuffer (txt))
			return;
		parseTextIntoCellsLinesRuns (me, txt, theWidechar);
		drawCells (me, xWC, yWC, theWidechar);
	}
}

double Graphics_inqTextX (Graphics me) { return my textX; }
double Graphics_inqTextY (Graphics me) { return my textY; }

int Graphics_getLinks (Graphics_Link **plinks) { *plinks = & links [0]; return numberOfLinks; }

static double psTextWidth (_Graphics_widechar string [], bool useSilipaPS) {
	/*
		The following has to be kept IN SYNC with GraphicsPostscript::charSize.
	*/
	double textWidth = 0.0;
	for (_Graphics_widechar *character = & string [0]; character -> kar > U'\t'; character ++) {
		Longchar_Info info = character -> karInfo;
		const int font = (
			info -> alphabet == Longchar_SYMBOL ?
				kGraphics_font_SYMBOL
			: info -> alphabet == Longchar_PHONETIC ?
				kGraphics_font_IPATIMES
			: info -> alphabet == Longchar_DINGBATS ?
				kGraphics_font_DINGBATS
			:
				character -> font.integer_
		);
		const int style = (
			character -> style == Graphics_ITALIC ?
				Graphics_ITALIC
			: character -> style == Graphics_BOLD || character -> link ?
				Graphics_BOLD
			: character -> style == Graphics_BOLD_ITALIC ?
				Graphics_BOLD_ITALIC
			:
				0
		);
		const double size = character -> size * 0.01;
		double charWidth = 600;   // Courier
		if (font == (int) kGraphics_font::COURIER) {
			charWidth = 600;
		} else if (style == 0) {
			if (font == (int) kGraphics_font::TIMES)
				charWidth = info -> ps.times;
			else if (font == (int) kGraphics_font::HELVETICA)
				charWidth = info -> ps.helvetica;
			else if (font == (int) kGraphics_font::PALATINO)
				charWidth = info -> ps.palatino;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS)
				charWidth = info -> ps.timesItalic;
			else
				charWidth = info -> ps.times;   // Symbol, IPA
		} else if (style == Graphics_BOLD) {
			if (font == (int) kGraphics_font::TIMES)
				charWidth = info -> ps.timesBold;
			else if (font == (int) kGraphics_font::HELVETICA)
				charWidth = info -> ps.helveticaBold;
			else if (font == (int) kGraphics_font::PALATINO)
				charWidth = info -> ps.palatinoBold;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS)
				charWidth = info -> ps.timesBoldItalic;
			else
				charWidth = info -> ps.times;
		} else if (style == Graphics_ITALIC) {
			if (font == (int) kGraphics_font::TIMES)
				charWidth = info -> ps.timesItalic;
			else if (font == (int) kGraphics_font::HELVETICA)
				charWidth = info -> ps.helvetica;
			else if (font == (int) kGraphics_font::PALATINO)
				charWidth = info -> ps.palatinoItalic;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS)
				charWidth = info -> ps.timesItalic;
			else
				charWidth = info -> ps.times;
		} else if (style == Graphics_BOLD_ITALIC) {
			if (font == (int) kGraphics_font::TIMES)
				charWidth = info -> ps.timesBoldItalic;
			else if (font == (int) kGraphics_font::HELVETICA)
				charWidth = info -> ps.helveticaBold;
			else if (font == (int) kGraphics_font::PALATINO)
				charWidth = info -> ps.palatinoBoldItalic;
			else if (font == kGraphics_font_IPATIMES && useSilipaPS)
				charWidth = info -> ps.timesBoldItalic;
			else
				charWidth = info -> ps.times;
		}
		charWidth *= size / 1000.0;
		textWidth += charWidth;
	}
	/*
	 * The following has to be kept IN SYNC with charSizes ().
	 */
	for (_Graphics_widechar *character = & string [0]; character -> kar > U'\t'; character ++) {
		if ((character -> style & Graphics_ITALIC) != 0) {
			_Graphics_widechar *nextCharacter = character + 1;
			if (nextCharacter -> kar <= U'\t') {
				textWidth += POSTSCRIPT_SLANT_CORRECTION;
			} else if (((nextCharacter -> style & Graphics_ITALIC) == 0 && nextCharacter -> baseline >= character -> baseline)
				|| (character -> baseline == 0 && nextCharacter -> baseline > 0))
			{
				if (nextCharacter -> kar == U'.' || nextCharacter -> kar == U',')
					textWidth += 0.5 * POSTSCRIPT_SLANT_CORRECTION;
				else
					textWidth += POSTSCRIPT_SLANT_CORRECTION;
			}
		}
	}
	return textWidth;
}

double Graphics_textWidth_ps_mm (Graphics me, conststring32 txt, bool useSilipaPS) {
	if (! initBuffer (txt))
		return 0.0;
	parseTextIntoCellsLinesRuns (me, txt, theWidechar);
	return psTextWidth (theWidechar, useSilipaPS) * (double) my fontSize * (25.4 / 72.0);
}

double Graphics_textWidth_ps (Graphics me, conststring32 txt, bool useSilipaPS) {
	return Graphics_dxMMtoWC (me, Graphics_textWidth_ps_mm (me, txt, useSilipaPS));
}

#if quartz
	bool _GraphicsMac_tryToInitializeFonts () {
		static bool inited = false;
		if (inited)
			return true;
		NSArray *fontNames = [[NSFontManager sharedFontManager] availableFontFamilies];
		hasTimes = [fontNames containsObject: @"Times"];
		if (! hasTimes)
			hasTimes = [fontNames containsObject: @"Times New Roman"];
		hasHelvetica = [fontNames containsObject: @"Helvetica"];
		if (! hasHelvetica)
			hasHelvetica = [fontNames containsObject: @"Arial"];
		hasCourier = [fontNames containsObject: @"Courier"];
		if (! hasCourier)
			hasCourier = [fontNames containsObject: @"Courier New"];
		hasSymbol = [fontNames containsObject: @"Symbol"];
		hasPalatino = [fontNames containsObject: @"Palatino"];
		if (! hasPalatino)
			hasPalatino = [fontNames containsObject: @"Book Antiqua"];
		hasDoulos = [fontNames containsObject: @"Doulos SIL"];
		hasCharis = [fontNames containsObject: @"Charis SIL"];
		hasIpaSerif = hasDoulos || hasCharis;
		inited = true;
		return true;
	}
#endif

#if cairo
	static const char *testFont (const char *fontName) {
		PangoFontDescription *pangoFontDescription = pango_font_description_from_string (fontName);
		PangoFont *pangoFont = pango_font_map_load_font (thePangoFontMap, thePangoContext, pangoFontDescription);
		PangoFontDescription *pangoFontDescription2 = pango_font_describe (pangoFont);
		return pango_font_description_get_family (pangoFontDescription2);
	}
	bool _GraphicsLin_tryToInitializeFonts () {
		static bool inited = false;
		if (inited)
			return true;
		thePangoFontMap = pango_cairo_font_map_get_default ();
		thePangoContext = pango_font_map_create_context (thePangoFontMap);
		#if 0   /* For debugging: list all fonts. */
			PangoFontFamily **families;
			int numberOfFamilies;
			pango_font_map_list_families (thePangoFontMap, & families, & numberOfFamilies);
			for (int i = 0; i < numberOfFamilies; i ++)
				fprintf (stderr, "%d %s\n", i, pango_font_family_get_name (families [i]));
			g_free (families);
		#endif
		const char *trueName;
		trueName = testFont ("Times");
		hasTimes = !! strstr (trueName, "Times") || !! strstr (trueName, "Roman") || !! strstr (trueName, "Serif");
		trueName = testFont ("Helvetica");
		hasHelvetica = !! strstr (trueName, "Helvetica") || !! strstr (trueName, "Arial") || !! strstr (trueName, "Sans");
		trueName = testFont ("Courier");
		hasCourier = !! strstr (trueName, "Courier") || !! strstr (trueName, "Mono");
		trueName = testFont ("Palatino");
		hasPalatino = !! strstr (trueName, "Palatino") || !! strstr (trueName, "Palladio");
		trueName = testFont ("Doulos SIL");
		hasDoulos = !! strstr (trueName, "Doulos");
		trueName = testFont ("Charis SIL");
		hasCharis = !! strstr (trueName, "Charis");
		hasIpaSerif = hasDoulos || hasCharis;
		testFont ("Symbol");
		testFont ("Dingbats");
		#if 0   /* For debugging: list font availability. */
			fprintf (stderr, "times %d helvetica %d courier %d palatino %d doulos %d charis %d\n",
				hasTimes, hasHelvetica, hasCourier, hasPalatino, hasDoulos, hasCharis);
		#endif
		inited = true;
		return true;
	}
#endif

void _GraphicsScreen_text_init (GraphicsScreen me) {   // BUG: should be done as late as possible
	#if cairo
        (void) me;
		Melder_assert (_GraphicsLin_tryToInitializeFonts ());
	#elif gdi
		int font, size, style;
		if (my printer || my metafile)
			for (font = (int) kGraphics_font::MIN; font <= kGraphics_font_DINGBATS; font ++)
				for (size = 0; size <= 4; size ++)
					for (style = 0; style <= Graphics_BOLD_ITALIC; style ++)
						if (fonts [(int) my resolutionNumber] [font] [size] [style]) {
							//DeleteObject (fonts [my resolutionNumber] [font] [size] [style]);
							//fonts [my resolutionNumber] [font] [size] [style] = 0;
						}
	#elif quartz
        (void) me;
        Melder_assert (_GraphicsMac_tryToInitializeFonts ());   // should have been handled when setting my useQuartz to true
	#endif
}

/* Output attributes. */

void Graphics_setTextAlignment (Graphics me, kGraphics_horizontalAlignment hor, int vert) {
	if ((int) hor != Graphics_NOCHANGE)
		my horizontalTextAlignment = (int) hor;
	if (vert != Graphics_NOCHANGE)
		my verticalTextAlignment = vert;
	if (my recording) { op (SET_TEXT_ALIGNMENT, 2); put (hor); put (vert); }
}

void Graphics_setFont (Graphics me, enum kGraphics_font font) {
	my font = font;
	if (my recording) { op (SET_FONT, 1); put (font); }
}

void Graphics_setFontSize (Graphics me, double size) {
	my fontSize = size;
	if (my recording) { op (SET_FONT_SIZE, 1); put (size); }
}

void Graphics_setFontStyle (Graphics me, int style) {
	my fontStyle = style;
	if (my recording) { op (SET_FONT_STYLE, 1); put (style); }
}

void Graphics_setItalic (Graphics me, bool onoff) {
	Graphics_setFontStyle (me, ( onoff ? my fontStyle | Graphics_ITALIC : my fontStyle & ~ Graphics_ITALIC ));
}

void Graphics_setBold (Graphics me, bool onoff) {
	Graphics_setFontStyle (me, ( onoff ? my fontStyle | Graphics_BOLD : my fontStyle & ~ Graphics_BOLD ));
}

void Graphics_setCode (Graphics me, bool onoff) {
	Graphics_setFontStyle (me, ( onoff ? my fontStyle | Graphics_CODE : my fontStyle & ~ Graphics_CODE ));
}

void Graphics_setTextRotation (Graphics me, double angle) {
	my textRotation = angle;
	if (my recording) { op (SET_TEXT_ROTATION, 1); put (angle); }
}

void Graphics_setWrapWidth (Graphics me, double wrapWidth) {
	my wrapWidth = wrapWidth;
	if (my recording) { op (SET_WRAP_WIDTH, 1); put (wrapWidth); }
}

void Graphics_setSecondIndent (Graphics me, double indent) {
	my secondIndent = indent;
	if (my recording) { op (SET_SECOND_INDENT, 1); put (indent); }
}

void Graphics_setPercentSignIsItalic (Graphics me, bool isItalic) {
	my percentSignIsItalic = isItalic;
	if (my recording) { op (SET_PERCENT_SIGN_IS_ITALIC, 1); put (isItalic); }
}

void Graphics_setNumberSignIsBold (Graphics me, bool isBold) {
	my numberSignIsBold = isBold;
	if (my recording) { op (SET_NUMBER_SIGN_IS_BOLD, 1); put (isBold); }
}

void Graphics_setCircumflexIsSuperscript (Graphics me, bool isSuperscript) {
	my circumflexIsSuperscript = isSuperscript;
	if (my recording) { op (SET_CIRCUMFLEX_IS_SUPERSCRIPT, 1); put (isSuperscript); }
}

void Graphics_setUnderscoreIsSubscript (Graphics me, bool isSubscript) {
	my underscoreIsSubscript = isSubscript;
	if (my recording) { op (SET_UNDERSCORE_IS_SUBSCRIPT, 1); put (isSubscript); }
}

void Graphics_setDollarSignIsCode (Graphics me, bool isCode) {
	my dollarSignIsCode = isCode;
	if (my recording) { op (SET_DOLLAR_SIGN_IS_CODE, 1); put (isCode); }
}

void Graphics_setAtSignIsLink (Graphics me, bool isLink) {
	my atSignIsLink = isLink;
	if (my recording) { op (SET_AT_SIGN_IS_LINK, 1); put (isLink); }
}

/* Inquiries. */

enum kGraphics_font Graphics_inqFont (Graphics me) { return my font; }
double Graphics_inqFontSize (Graphics me) { return my fontSize; }
int Graphics_inqFontStyle (Graphics me) { return my fontStyle; }

/* End of file Graphics_text.cpp */
