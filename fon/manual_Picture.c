/* manual_Picture.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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

#include "ManPagesM.h"

static void drawOneIpaSymbol (Graphics g, double x, double y, const char *symbol) {
	char buffer [30], *p = & buffer [0];
	int fontSize = Graphics_inqFontSize (g);
	Graphics_rectangle (g, x - 0.5, x + 0.5, y - 0.5, y + 0.5);
	if (! symbol) return;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFont (g, Graphics_TIMES);
	Graphics_setFontSize (g, fontSize * 3 / 2);
	Graphics_text (g, x, y + 0.25, symbol);
	while (*symbol) {
		if (*symbol == '\\') { *p++ = '\\'; *p++ = 'b'; *p++ = 's'; }   /* Visualize backslash (\bs). */
		else { *p++ = '\\'; *p++ = *symbol; *p++ = ' '; }   /* Visualize special symbols (% ^ _ #). */
		symbol ++;
	}
	*p = '\0';   /* Trailing null byte. */
	Graphics_setFont (g, Graphics_HELVETICA);
	Graphics_setFontSize (g, fontSize * 5 / 6);
	Graphics_text (g, x, y - 0.25, buffer);
	Graphics_setFontSize (g, fontSize);
}

static void draw_IPA_consonant_chart (Graphics graphics) {
	Graphics_setWindow (graphics, 0, 20, 6, 19.5);
	Graphics_setFontStyle (graphics, Graphics_ITALIC);
	Graphics_setTextRotation (graphics, 60);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (graphics, 4, 17.6, "bilabial");
	Graphics_text (graphics, 5, 17.6, "labiodental");
	Graphics_text (graphics, 6, 17.6, "dental");
	Graphics_text (graphics, 7, 17.6, "alveolar");
	Graphics_text (graphics, 8, 17.6, "alv. lateral");
	Graphics_text (graphics, 9, 17.6, "postalveolar");
	Graphics_text (graphics, 10, 17.6, "retroflex");
	Graphics_text (graphics, 11, 17.6, "alveolo-palatal");
	Graphics_text (graphics, 12, 17.6, "palatal");
	Graphics_text (graphics, 13, 17.6, "labial-palatal");
	Graphics_text (graphics, 14, 17.6, "labial-velar");
	Graphics_text (graphics, 15, 17.6, "velar");
	Graphics_text (graphics, 16, 17.6, "uvular");
	Graphics_text (graphics, 17, 17.6, "pharyngeal");
	Graphics_text (graphics, 18, 17.6, "epiglottal");
	Graphics_text (graphics, 19, 17.6, "glottal");
	Graphics_setTextRotation (graphics, 0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 3.3, 17, "voiceless plosive");
	Graphics_text (graphics, 3.3, 16, "voiced plosive");
	Graphics_text (graphics, 3.3, 15, "nasal");
	Graphics_text (graphics, 3.3, 14, "voiceless fricative");
	Graphics_text (graphics, 3.3, 13, "voiced fricative");
	Graphics_text (graphics, 3.3, 12, "approximant");
	Graphics_text (graphics, 3.3, 11, "trill");
	Graphics_text (graphics, 3.3, 10, "tap or flap");
	Graphics_text (graphics, 3.3, 9, "lateral approx.");
	Graphics_text (graphics, 3.3, 8, "implosive");
	Graphics_text (graphics, 3.3, 7, "click");
	Graphics_setFontStyle (graphics, Graphics_NORMAL);
	{
		static struct { float x, y; char *string; } symbols [] = {
{ 4, 17, "p" }, { 4, 16, "b" }, { 4, 15, "m" }, { 4, 14, "\\ff" }, { 4, 13, "\\bf" }, { 4, 11, "\\bc" }, { 4, 8, "\\b^" }, { 4, 7, "\\O." },
{ 5, 15, "\\mj" }, { 5, 14, "f" }, { 5, 13, "v" }, { 5, 12, "\\vs" },
{ 6, 14, "\\tf" }, { 6, 13, "\\dh" }, { 6, 7, "\\|1" },
{ 7, 17, "t" }, { 7, 16, "d" }, { 7, 15, "n" }, { 7, 14, "s" }, { 7, 13, "z" }, { 7, 12, "\\rt" }, { 7, 11, "r" }, { 7, 10, "\\fh" }, { 7, 9, "l" }, { 7, 8, "\\d^" },
{ 8, 17, "t^l" }, { 8, 16, "d^l" }, { 8, 14, "\\l-" }, { 8, 13, "\\lz" }, { 8, 12, "l" }, { 8, 10, "\\rl" }, { 8, 9, "l" }, { 8, 7, "\\|2" },
{ 9, 14, "\\sh" }, { 9, 13, "\\zh" }, { 9, 7, "\\|-" },
{ 10, 17, "\\t." }, { 10, 16, "\\d." }, { 10, 15, "\\n." }, { 10, 14, "\\s." }, { 10, 13, "\\z." }, { 10, 12, "\\r." }, { 10, 10, "\\f." }, { 10, 9, "\\l." }, { 10, 7, "!" },
{ 11, 14, "\\cc" }, { 11, 13, "\\zc" },
{ 12, 17, "c" }, { 12, 16, "\\j-" }, { 12, 15, "\\nj" }, { 12, 14, "\\c," }, { 12, 13, "\\jc" }, { 12, 12, "j" }, { 12, 9, "\\yt" }, { 12, 8, "\\j^" },
{ 13, 12, "\\ht" },
{ 14, 14, "\\wt" }, { 14, 12, "w" },
{ 15, 17, "k" }, { 15, 16, "\\gs" }, { 15, 15, "\\ng" }, { 15, 14, "x" }, { 15, 13, "\\gf" }, { 15, 12, "\\ml" }, { 15, 9, "\\lc" }, { 15, 8, "\\g^" },
{ 16, 17, "q" }, { 16, 16, "\\gc" }, { 16, 15, "\\nc" }, { 16, 14, "\\cf" }, { 16, 13, "\\ri" }, { 16, 11, "\\rc" }, { 16, 8, "\\G^" },
{ 17, 14, "\\h-" }, { 17, 13, "\\9e" },
{ 18, 17, "\\?-" }, { 18, 14, "\\hc" }, { 18, 13, "\\9-" },
{ 19, 17, "\\?g" }, { 19, 14, "h" }, { 19, 13, "\\h^" },
		{ 0, 0, NULL } };
		int i;
		for (i = 0; symbols [i]. string != NULL; i ++)
			drawOneIpaSymbol (graphics, symbols [i]. x, symbols [i]. y, symbols [i]. string);
	}
}

static void draw_IPA_vowel_chart (Graphics graphics) {
	Graphics_setWindow (graphics, 0, 19, -0.5, 7.5);
	Graphics_setFontStyle (graphics, Graphics_ITALIC);
	Graphics_setTextRotation (graphics, 60);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (graphics, 4.5, 6.6, "front");
	Graphics_text (graphics, 7, 6.6, "central");
	Graphics_text (graphics, 9.5, 6.6, "back");
	Graphics_setTextRotation (graphics, 0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 3.3, 6, "close");
	Graphics_text (graphics, 3.3, 5, "close centralized");
	Graphics_text (graphics, 3.3, 4, "close-mid");
	Graphics_text (graphics, 3.3, 2, "open-mid");
	Graphics_text (graphics, 3.3, 0, "open");
	Graphics_setFontStyle (graphics, Graphics_NORMAL);
	{
		static struct { float x, y; char *string; } symbols [] = {
{ 4, 6, "i" }, { 5, 6, "y" }, { 6.5, 6, "\\i-" }, { 7.5, 6, "\\u-" }, { 9, 6, "\\mt" }, { 10, 6, "u" },
{ 4.2, 5, "\\ic" }, { 5.2, 5, "\\yc" }, { 9.8, 5, "\\hs" },
{ 4, 4, "e" }, { 5, 4, "\\o/" }, { 6.5, 4, "\\e-" }, { 7.5, 4, "\\o-" }, { 9, 4, "\\rh" }, { 10, 4, "o" },
{ 7, 3, "\\sw" }, { 7, 1, "\\at" },
{ 4, 2, "\\ef" }, { 5, 2, "\\oe" }, { 6.5, 2, "\\er" }, { 7.5, 2, "\\kb" }, { 9, 2, "\\vt" }, { 10, 2, "\\ct" },
{ 4, 1, "\\ae" },
{ 4, 0, "a" }, { 5, 0, "\\Oe" }, { 9, 0, "\\as" }, { 10, 0, "\\ab" },
		{ 0, 0, NULL } };
		int i;
		for (i = 0; symbols [i]. string != NULL; i ++)
			drawOneIpaSymbol (graphics, symbols [i]. x, symbols [i]. y, symbols [i]. string);
	}
}

void manual_Picture_init (ManPages me);
void manual_Picture_init (ManPages me) {

MAN_BEGIN ("Special symbols", "ppgb", 20031102)
INTRO ("When drawing text into the @@Picture window@ or into an editor, "
	"you can use %%backslash sequences% to display various kinds of special symbols.")

ENTRY ("European symbols")
NORMAL ("To get the symbol \"\\a^\" (a-circumflex), you type \"\\bsa\\^ \", i.e., "
	"a sequence of backslash + a + circumflex. In this way, you can get a hundred "
	"non-ASCII symbols that are used in the alphabets of many European languages. "
	"You can also use these symbols in info messages sent from scripts.")
#ifdef macintosh
LIST_ITEM ("\\a\" \\bsa\\\"  \\e\" \\bse\\\"  \\i\" \\bsi\\\"  \\o\" \\bso\\\"  "
	"\\u\" \\bsu\\\"  \\y\" \\bsy\\\"  "
	"\\A\" \\bsA\\\"  \\E\" \\bsE\\\"  \\I\" \\bsI\\\"  \\O\" \\bsO\\\"  \\U\" \\bsU\\\" "
	" \\Y\" \\bsY\\\" ")
LIST_ITEM ("\\a\' \\bsa\' \\e\' \\bse\' \\i\' \\bsi\' \\o\' \\bso\' \\u\' \\bsu\' "
	"\\A\' \\bsA\' \\E\' \\bsE\' \\I\' \\bsI\' \\O\' \\bsO\' \\U\' \\bsU\'")
LIST_ITEM ("\\a` \\bsa` \\e` \\bse` \\i` \\bsi` \\o` \\bso` \\u` \\bsu` "
	"\\A` \\bsA` \\E` \\bsE` \\I` \\bsI` \\O` \\bsO` \\U` \\bsU`")
LIST_ITEM ("\\a^ \\bsa\\^  \\e^ \\bse\\^  \\i^ \\bsi\\^  \\o^ \\bso\\^  \\u^ \\bsu\\^  "
	"\\A^ \\bsA\\^  \\E^ \\bsE\\^  \\I^ \\bsI\\^  \\O^ \\bsO\\^  \\U^ \\bsU\\^ ")
LIST_ITEM ("\\a~ \\bsa\\~  \\n~ \\bsn\\~  \\o~ \\bso\\~  \\A~ \\bsA\\~  \\N~ \\bsN\\~  \\O~ \\bsO\\~ ")
LIST_ITEM ("\\ae \\bsae \\o/ \\bso/ \\ao \\bsao \\Ae \\bsAe \\O/ \\bsO/ \\Ao \\bsAo "
	"\\c, \\bsc, \\C, \\bsC, \\ss \\bsss")
#else
LIST_ITEM ("\\a\" \\bsa\\\"  \\e\" \\bse\\\"  \\i\" \\bsi\\\"  \\o\" \\bso\\\"  "
	"\\u\" \\bsu\\\"  \\y\" \\bsy\\\"  "
	"\\A\" \\bsA\\\"  \\E\" \\bsE\\\"  \\I\" \\bsI\\\"  \\O\" \\bsO\\\"  \\U\" \\bsU\\\" ")
LIST_ITEM ("\\a\' \\bsa\' \\e\' \\bse\' \\i\' \\bsi\' \\o\' \\bso\' \\u\' \\bsu\' "
	" \\y\' \\bsy\'"
	"\\A\' \\bsA\' \\E\' \\bsE\' \\I\' \\bsI\' \\O\' \\bsO\' \\U\' \\bsU\'"
	" \\Y\' \\bsY\'")
LIST_ITEM ("\\a` \\bsa` \\e` \\bse` \\i` \\bsi` \\o` \\bso` \\u` \\bsu` "
	"\\A` \\bsA` \\E` \\bsE` \\I` \\bsI` \\O` \\bsO` \\U` \\bsU`")
LIST_ITEM ("\\a^ \\bsa\\^  \\e^ \\bse\\^  \\i^ \\bsi\\^  \\o^ \\bso\\^  \\u^ \\bsu\\^  "
	"\\A^ \\bsA\\^  \\E^ \\bsE\\^  \\I^ \\bsI\\^  \\O^ \\bsO\\^  \\U^ \\bsU\\^ ")
LIST_ITEM ("\\a~ \\bsa\\~  \\n~ \\bsn\\~  \\o~ \\bso\\~  \\A~ \\bsA\\~  \\N~ \\bsN\\~  \\O~ \\bsO\\~ ")
LIST_ITEM ("\\ae \\bsae \\o/ \\bso/ \\ao \\bsao \\Ae \\bsAe \\O/ \\bsO/ \\Ao \\bsAo "
	"\\c, \\bsc, \\C, \\bsC, \\ss \\bsss"
	" \\th \\bsth \\Th \\bsTh")
#endif
LIST_ITEM ("\\!d \\bs!d \\?d \\bs?d")
LIST_ITEM ("\\cu \\bscu (%currency), \\Lp \\bsLp (%sterling), \\Y= \\bsY= (%yen), "
	"\\fd \\bsfd (Dutch %florin), \\c/ \\bsc/ (%cent)")
LIST_ITEM ("\\SS \\bsSS (%section), \\|| \\bs|| (%paragraph)")
LIST_ITEM ("\\co \\bsco (%copyright), \\re \\bsre (%registered), \\tm \\bstm (%trademark)")
LIST_ITEM ("\\a_ \\bsa_ (%ordfeminine), \\o_ \\bso_ (%ordmasculine)")
LIST_ITEM ("\\<< \\bs<< (%guillemotleft), \\>> \\bs>> (%guillemotright)")

ENTRY ("Mathematical symbols")
LIST_ITEM ("\\.c \\bs.c (%periodcentered), \\xx \\bsxx (%multiply), \\:- \\bs:- (%divide), \\/d \\bs/d (%fraction)")
LIST_ITEM ("\\dg \\bsdg (%degree), \\\'p \\bs\'p (%minute or %prime), \\\"p \\bs\\\" p (%second or %%double prime%)")
LIST_ITEM ("- - (%minus), \\-- \\bs-- (%endash), \\+- \\bs+- (%plusminus)")
LIST_ITEM ("\\<_ \\bs<_ (%lessequal), \\>_ \\bs>_ (%greaterequal), \\=/ \\bs=/ (%notequal)")
LIST_ITEM ("\\no \\bsno (%logicalnot), \\an \\bsan (%logicaland), \\or \\bsor (%logicalor)")
LIST_ITEM ("\\At \\bsAt (%universal), \\Er \\bsEr (%existential), \\.3 \\bs.3 (%therefore)")
LIST_ITEM ("\\oc \\bsoc (%proportional), \\=3 \\bs=3 (%equivalence), \\~~ \\bs~~ (%approxequal)")
LIST_ITEM ("\\Vr \\bsVr (%radical)")
LIST_ITEM ("\\<- \\bs<- (%arrowleft), \\-> \\bs-> (%arrowright), \\<> \\bs<> (%arrowboth)")
LIST_ITEM ("\\<= \\bs<= (%arrowdblleft), \\=> \\bs=> (%arrowdblright), \\eq \\bseq (%arrowdblboth)")
LIST_ITEM ("\\^| \\bs\\^ | (%arrowup), \\=~ \\bs=~ (%congruent), \\_| \\bs_| (%arrowdown)")
LIST_ITEM ("\\oo \\bsoo (%infinity), \\Tt \\bsTt (%perpendicular)")
LIST_ITEM ("\\O| \\bsO| (%emptyset), \\ni \\bsni (%intersection), \\uu \\bsuu (%union), "
	"\\c= \\bsc= (%propersubset), \\e= \\bse= (%element)")
LIST_ITEM ("\\dd \\bsdd (%partialdiff)")
LIST_ITEM ("\\ox \\bsox (%circlemultiply), \\o+ \\bso+ (%circleplus)")
LIST_ITEM ("\\su \\bssu (%summation), \\in \\bsin (%integral)")

ENTRY ("Greek letters")
NORMAL ("To get \\ep\\up\\ro\\et\\ka\\al, you type \\bsep\\bsup\\bsro\\bset\\bska\\bsal.")
LIST_ITEM ("\t\\al \\bsal \t\\Al \\bsAl %alpha")
LIST_ITEM ("\t\\be \\bsbe \t\\Be \\bsBe %beta")
LIST_ITEM ("\t\\ga \\bsga \t\\Ga \\bsGa %gamma")
LIST_ITEM ("\t\\de \\bsde \t\\De \\bsDe %delta")
LIST_ITEM ("\t\\ep \\bsep \t\\Ep \\bsEp %epsilon")
LIST_ITEM ("\t\\ze \\bsze \t\\Ze \\bsZe %zeta")
LIST_ITEM ("\t\\et \\bset \t\\Et \\bsEt %eta")
LIST_ITEM ("\t\\te \\bste \t\\Te \\bsTe %theta \t\\t2 \\bst2")
LIST_ITEM ("\t\\io \\bsio \t\\Io \\bsIo %iota")
LIST_ITEM ("\t\\ka \\bska \t\\Ka \\bsKa %kappa")
LIST_ITEM ("\t\\la \\bsla \t\\La \\bsLa %lambda")
LIST_ITEM ("\t\\mu \\bsmu \t\\Mu \\bsMu %mu")
LIST_ITEM ("\t\\nu \\bsnu \t\\Nu \\bsNu %nu")
LIST_ITEM ("\t\\xi \\bsxi \t\\Xi \\bsXi %xi")
LIST_ITEM ("\t\\on \\bson \t\\On \\bsOn %omicron")
LIST_ITEM ("\t\\pi \\bspi \t\\Pi \\bsPi %pi")
LIST_ITEM ("\t\\ro \\bsro \t\\Ro \\bsRo %rho")
LIST_ITEM ("\t\\si \\bssi \t\\Si \\bsSi %sigma \t\\s2 \\bss2")
LIST_ITEM ("\t\\ta \\bsta \t\\Ta \\bsTa %tau")
LIST_ITEM ("\t\\up \\bsup \t\\Up \\bsUp %upsilon")
LIST_ITEM ("\t\\fi \\bsfi \t\\Fi \\bsFi %phi \t\\f2 \\bsf2")
LIST_ITEM ("\t\\ci \\bsci \t\\Ci \\bsCi %chi")
LIST_ITEM ("\t\\ps \\bsps \t\\Ps \\bsPs %psi")
LIST_ITEM ("\t\\om \\bsom \t\\Om \\bsOm %omega \t\\o2 \\bso2")

ENTRY ("Phonetic symbols")
NORMAL ("See @@Phonetic symbols@")

ENTRY ("Miscellaneous")
LIST_ITEM ("\\bs \\bsbs %backslash")
LIST_ITEM ("\\bu \\bsbu %bullet")
LIST_ITEM ("\\cl \\bscl (%club), \\di \\bsdi (%diamond), \\he \\bshe (%heart), \\sp \\bssp (%spade)")

ENTRY ("See also")
NORMAL ("@@Text styles@")
MAN_END

MAN_BEGIN ("Text styles", "ppgb", 19961012)
INTRO ("When drawing text into the @@Picture window@ or into an editor, "
	"you can use text styles other than regular Roman.")
ENTRY ("Italic, bold, superscript, subscript")
NORMAL ("With the following symbols, you introduce stretches of text drawn in special styles:")
LIST_ITEM ("\\% : the following letter will be italic.")
LIST_ITEM ("\\# : the following letter will be bold.")
LIST_ITEM ("\\# \\% : the following letter will be bold-italic.")
LIST_ITEM ("\\^ : the following letter will be superscript: \\% m\\% c\\^ 2 gives %mc^2.")
LIST_ITEM ("\\_ : the following letter will be subscript. Example: %F__0_ is typed as \\% F_0.")
LIST_ITEM ("\\% \\% : the following letters will be italic, until the following \\% :")
LIST_ITEM ("    $$Now \\% \\% you\\%  try$   gives:   Now %%you% try.")
LIST_ITEM ("The same goes for \\# \\# , \\^ \\^ , and \\_ \\_ .")
LIST_ITEM ("\\bss{...}: small:")
LIST_ITEM ("    $$W\\bss{ARP}$   gives:   W\\s{ARP}")
NORMAL ("To draw a \\% , \\# , \\^ , or \\_  symbol, you type \"$$\\bs\\%  $\", "
	"\"$$\\bs\\#  $\", \"$$\\bs\\^  $\", or \"$$\\bs\\_  $\": a backslash, the symbol, and a space.")
NORMAL ("On Xwindows machines, the font `Symbol' will never look bold or italic, "
	"but it will be printed correctly.")
ENTRY ("See also")
NORMAL ("@@Special symbols@")
MAN_END

MAN_BEGIN ("Phonetic symbols", "ppgb", 20050308)
NORMAL ("To draw phonetic symbols in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the SIL Doulos IPA 1993 font, e.g. from www.praat.org. "
	"You can then use backslash sequences as described in:")
LIST_ITEM ("\\bu @@Phonetic symbols: consonants")
LIST_ITEM ("\\bu @@Phonetic symbols: vowels")
LIST_ITEM ("\\bu @@Phonetic symbols: diacritics")
MAN_END

MAN_BEGIN ("Phonetic symbols: consonants", "ppgb", 20050918)
NORMAL ("To draw phonetic symbols for consonants in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the SIL Doulos IPA 1993 font, e.g. from www.praat.org. "
	"You can then use the backslash sequences in the following table.")
PICTURE (6.0, 7.0, draw_IPA_consonant_chart)
NORMAL ("Other consonant symbols:")
LIST_ITEM ("\\l~ \\bsl~ (%%l with tilde%): velarized %l")
LIST_ITEM ("\\hj \\bshj (%%heng with hooktop%): the Swedish rounded post-alveolar & velar fricative")
ENTRY ("How to remember the codes")
NORMAL ("For most of the codes, the first letter tells you the most similar letter of the English alphabet. "
	"The second letter can be %t (%turned), %c (%capital or %curled), %s (%script), - (%barred), %l (%%with leg%), "
	"%i (%inverted), or %j (%%left tail%). Some phonetic symbols are similar to Greek letters but have special "
	"phonetic (%f) versions with serifs (\\ff, \\bf, \\gf) or are otherwise slightly different (\\tf, \\cf). "
	"The codes for \\ng (%engma), \\dh (%eth), \\sh (%esh), and \\zh (%yogh) are traditional alternative spellings. "
	"The retroflexes have a period in the second place, because an alternative traditional spelling is to write a dot under them. "
	"The code for \\fh is an abbreviation for %fishhook.")
MAN_END

MAN_BEGIN ("Phonetic symbols: diacritics", "ppgb", 20050918)
NORMAL ("To draw phonetic diacritical symbols in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the SIL Doulos IPA 1993 font, e.g. from www.praat.org. "
	"You can then use the backslash sequences in the following list.")
NORMAL ("In line:")
LIST_ITEM ("\\:f \\bs:f the phonetic length sign")
LIST_ITEM ("\\|f \\bs|f the phonetic stroke")
LIST_ITEM ("t\\cn t\\bscn (%corner): unreleased plosive")
NORMAL ("Understrikes:")
LIST_ITEM ("n\\|v n\\bs|v (%strokeunder): syllabic consonant")
LIST_ITEM ("b\\0v b\\bs0v (%ringunder): voiceless (e.g. lenis voiceless plosive, voiceless nasal or approximant)")
LIST_ITEM ("o\\Tv o\\bsTv (%lowering): lowered vowel; or turns a fricative into an approximant")
LIST_ITEM ("o\\T^ o\\bsT\\^  (%raising): raised vowel; or turns an approximant into a fricative")
LIST_ITEM ("e\\-v e\\bs-v (%minusunder): backed")
LIST_ITEM ("o\\+v o\\bs+v (%plusunder): fronted")
LIST_ITEM ("o\\:v o\\bs:v (%diaresisunder): breathy voice")
LIST_ITEM ("o\\~v o\\bs~v (%tildeunder): creaky voice")
LIST_ITEM ("t\\Nv t\\bsNv (%bridgeunder): dental (as opposed to alveolar)")
LIST_ITEM ("e\\3v e\\bs3v (%halfringright): slightly rounded")
LIST_ITEM ("u\\cv u\\bscv (%halfringleft): slightly unrounded")
NORMAL ("Overstrikes:")
LIST_ITEM ("\\gf\\0^ \\bsgf\\bs0\\^  (%ringover): voiceless")
LIST_ITEM ("\\ep\\\'^ \\bsep\\bs\'\\^  (%acuteover): high tone")
LIST_ITEM ("\\ep\\`^ \\bsep\\bs`\\^  (%graveover): low tone")
LIST_ITEM ("\\ep\\-^ \\bsep\\bs-\\^  (%minusover): mid tone (or so)")
LIST_ITEM ("\\ep\\~^ \\bsep\\bs~\\^  (%tildeover): nasalized")
LIST_ITEM ("\\ep\\v^ \\bsep\\bsv\\^  (%caronover, %hac\\v^ek, %wedge): rising tone")
LIST_ITEM ("\\ep\\^^ \\bsep\\bs\\^ \\^  (%circumover): falling tone")
LIST_ITEM ("o\\:^ o\\bs:\\^  (%diaresisover): centralized")
LIST_ITEM ("k\\lip t\\lis k\\bslip (%ligature): simultaneous articulation, or single segment")
MAN_END

MAN_BEGIN ("Phonetic symbols: vowels", "ppgb", 20050918)
NORMAL ("To draw phonetic symbols for vowels in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the SIL Doulos IPA 1993 font, e.g. from www.praat.org. "
	"You can then use the backslash sequences in the following table.")
PICTURE (6.0, 5.0, draw_IPA_vowel_chart)
NORMAL ("Other vowel symbols are:")
LIST_ITEM ("\\sr \\bssr (%%schwa with right hook%): rhotacized schwa")
ENTRY ("How to remember the codes")
NORMAL ("For most of the codes, the first letter tells you the most similar letter of the English alphabet. "
	"The second letter can be %t (%turned), %c (%capital), %s (%script), %r (%reversed), - (%barred or %retracted), or / (%slashed). "
	"One symbol (\\ef) is a phonetic version of a Greek letter. "
	"The codes for \\sw, \\rh, \\hs and \\kb are abbreviations for %schwa, %%ram's horn%, %horseshoe, and %%kidney bean%.")
MAN_END

MAN_BEGIN ("Axes...", "ppgb", 19970330)
INTRO ("One of the commands in the #Margins and #World menus of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To view and change the current world coordinates of the horizontal and vertical axes.")
ENTRY ("Usage")
NORMAL ("The axes are normally changed by every drawing operation in the dynamic menu, "
	"i.e., by object-specific drawing commands with titles like ##Draw...# and ##Paint...# "
	"(the drawing commands in the Picture window, like ##Paint rectangle...#, do not change the axes).")
NORMAL ("You would use the ##Axes...# command if your data are not in an object.")
ENTRY ("Example")
NORMAL ("The following script would draw a person's vowel triangle:")
CODE ("\\#  Put F1 (between 300 and 800 Hz) along the horizontal axis,")
CODE ("\\#  and F2 (between 600 and 3600 Hz) along the vertical axis.")
CODE ("##Axes...# 300 800 600 3600")
CODE ("\\#  Draw a rectangle inside the current viewport (selected area),")
CODE ("\\#  with text in the margins, and tick marks in steps of 100 Hz along the F1 axis,")
CODE ("\\#  and in steps of 200 Hz along the F2 axis.")
CODE ("Draw inner box")
CODE ("Text top... no Dani\\bse\\\" l's Dutch vowel triangle")
CODE ("Text bottom... yes \\% F_1 (Hz)")
CODE ("Text left... yes \\% F_2 (Hz)")
CODE ("Marks bottom every... 1 100 yes yes yes")
CODE ("Marks left every... 1 200 yes yes yes")
CODE ("\\#  Draw large phonetic symbols at the vowel points.")
CODE ("Text special... 340 Centre 688 Half Times 24 0 u")
CODE ("Text special... 481 Centre 1195 Half Times 24 0 \\bso/")
CODE ("\\#  Etcetera")
NORMAL ("This example would draw the texts \"Dani\\e\"l's Dutch vowel triangle\", "
	"\"%F__1_ (Hz)\", and \"%F__2_ (Hz)\" in the margins, "
	"and the texts \"u\" and \"\\o/\" at the appropriate positions inside the drawing area.")
MAN_END

MAN_BEGIN ("Copy to clipboard", "ppgb", 20041130)   /* Not Unix. */
INTRO ("A command in the File menu of the @@Picture window@.")
NORMAL ("It copies the selected part of the picture to the clipboard. "
	"You can then `Paste' it into any program that knows pictures.")
ENTRY ("Behaviour")
NORMAL ("Though all the picture data will be written to the clipboard, "
	"only the part that corresponds to the selected part of the Picture window (the %viewport) will be visible.")
#if defined (macintosh)
ENTRY ("Usage")
NORMAL ("If you have a PostScript printer, you will want to use @@Write to EPS file...@ instead. "
	"If the picture is too large, e.g. a spectrogram that does not seem to fit into the clipboard, "
	"you can try @@Write to Mac PICT file...@ instead.")
#endif
#if defined (_WIN32)
ENTRY ("Usage")
NORMAL ("If you have a PostScript printer, you will want to use @@Write to EPS file...@ instead. "
	"If the picture is too large, e.g. a spectrogram that does not seem to fit into the clipboard, "
	"you can try @@Write to Windows metafile...@ instead.")
#endif
MAN_END

MAN_BEGIN ("Draw inner box", "ppgb", 19970330)
INTRO ("One of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To draw a rectangle inside the drawing area, "
	"leaving @margins on all four sides for drawing text and marks.")
ENTRY ("Behaviour")
NORMAL ("The widths of the margins depend on the current font size.")
MAN_END

MAN_BEGIN ("Encapsulated PostScript", "ppgb", 20001010)
INTRO ("a kind of PostScript file that can easily be imported into word processors and drawing programs. "
	"In Praat, you can create an Encapsulated PostScript (EPS) file by choosing @@Write to EPS file...@.")
MAN_END

MAN_BEGIN ("Erase all", "ppgb", 19980825)
INTRO ("A command in the #Edit menu of the @@Picture window@.")
NORMAL ("It erases all your drawings.")
MAN_END

MAN_BEGIN ("Font menu", "ppgb", 20040211)
INTRO ("One of the menus of the @@Picture window@.")
NORMAL ("It allows you to choose the default font of the text "
	"to be used in subsequent drawing, and its size (character height).")
ENTRY ("Sizes")
NORMAL ("You can choose any of the sizes 10, 12, 14, 18, or 24 directly from this menu, "
	"or fill in any other size in the ##Font size...# form.")
TAG ("Unix:")
DEFINITION ("the font size will be rounded to the nearest size available on Xwindows, "
	"which is one from 10, 12, 14, 18, or 24 points; "
	"PostScript-printing a picture where you specified a font size of 100, however, "
	"will still give the correct 100-point character height.")
TAG ("Macintosh and Windows:")
DEFINITION ("all sizes are drawn correctly (what you see on the screen "
	"is what you get on your printer).")
NORMAL ("The widths of the margins depend on the current font size, "
	"so if you want to change the font size, "
	"do so before making your drawing.")
ENTRY ("Fonts")
NORMAL ("With these commands, you set the font in which subsequent text will de drawn: "
	"Times, Helvetica, New Century Schoolbook, Palatino, or Courier.")
NORMAL ("You can mix the Symbol and IPA alphabets with the normal Roman alphabets "
	"and use sequences of backslash + digraph for @@special symbols@ (see also @@phonetic symbols@).")
NORMAL ("For instance, you can get an \\e\" by typing \\bse\\\" , or a \\ss by typing \\bsss; "
	"you can get an \\ep by typing \\bsep, or a \\ct, which is a turned c, by typing \\bsct.")
NORMAL ("In a PostScript preview on the Indigo (double-click on a PostScript file), "
	"New Century Schoolbook will be replaced by Courier.")
NORMAL ("If you print to a PostScript printer, all fonts will be correct.")
ENTRY ("Styles")
NORMAL ("You can use all graphical @@text styles@ in the Picture window.")
MAN_END

MAN_BEGIN ("Logarithmic marks left/right/top/bottom...", "ppgb", 19970330)
INTRO ("Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To draw a specified number of marks per decade "
	"into the @margins around the drawing area, along a logarithmic axis.")
ENTRY ("Arguments")
TAG ("%%Marks per decade")
DEFINITION ("the number of marks that will be drawn for every decade.")
TAG ("%%Write numbers")
DEFINITION ("if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG ("%%Draw ticks")
DEFINITION ("if on, short line pieces will be drawn in the margin.")
TAG ("%%Draw dotted lines")
DEFINITION ("if on, dotted lines will be drawn through your drawing.")
ENTRY ("Behaviour")
LIST_ITEM ("If your vertical logarithmic axis runs from 10 to 100, "
	"and %%Marks per decade% is 1, marks will only be drawn at 10 and 100;")
LIST_ITEM ("if %%Marks per decade% is 2, marks will be drawn at 10, 30, and 100;")
LIST_ITEM ("if it is 3, marks will be drawn at 10, 20, 50, and 100;")
LIST_ITEM ("if it is 4, marks will be drawn at 10, 20, 30, 50, and 100;")
LIST_ITEM ("if it is 5, marks will be drawn at 10, 20, 30, 50, 70, and 100;")
LIST_ITEM ("if it is 6, marks will be drawn at 10, 15, 20, 30, 50, 70, and 100;")
LIST_ITEM ("if it is 7 (the maximum), marks will be drawn at 10, 15, 20, 30, 40, 50, 70, and 100.")
MAN_END

MAN_BEGIN ("Margins", "ppgb", 19970405)
INTRO ("The space around most of your drawings in the @@Picture window@.")
ENTRY ("World coordinates")
NORMAL ("With the commands in the #Margins menu, "
	"you draw text, ticks, numbers, or a rectangle, "
	"in the margins around the latest drawing that you made, "
	"or you draw dotted lines through or text inside this last drawing.")
NORMAL ("You specify the positions of these things in world coordinates, "
	"i.e., in coordinates that refer to the natural coordinate system of your last drawing.")
NORMAL ("The numbers that you can mark around your drawing also refer to these coordinates. "
	"For instance, after drawing a spectrum with ##Spectrum: Draw...#, "
	"you can draw a dotted line at 2000 Hz or at 60 dB "
	"by choosing ##One mark bottom...# or ##One mark left...# "
	"and typing \"2000\" or \"60\", respectively.")
ENTRY ("Usage")
NORMAL ("The margin commands work with all the drawings that leave margins around themselves, "
	"such as ##Sound: Draw...#, ##Spectrogram: Paint...#, ##Polygon: Paint...#, and more. "
	"They do not work properly, however, with the commands that draw vocal tract shapes, "
	"like ##Art & Speaker: Draw...# and ##Artword & Speaker: Draw...#, "
	"because these can only be drawn correctly into a square viewport.")
ENTRY ("Limited validity")
NORMAL ("The margin commands work only on the latest drawing that you made "
	"(unless you @Undo that drawing).")
ENTRY ("Margin size")
NORMAL ("The size of the margins depends on the font size, "
	"so be sure that you have the font size of your choice before you make your drawing. "
	"You can set the font size with the @@Font menu@.")
MAN_END

MAN_BEGIN ("Marks left/right/top/bottom...", "ppgb", 19970330)
INTRO ("Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To draw any number of equally spaced marks into the @margins around the drawing area.")
ENTRY ("Arguments")
TAG ("%%Number of marks%")
DEFINITION ("the number of equally spaced marks (2 or more) that will be drawn; "
	"there will always be marks at the beginning and end of the domain or range.")
TAG ("%%Write numbers")
DEFINITION ("if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG ("%%Draw ticks")
DEFINITION ("if on, short line pieces will be drawn in the margin.")
TAG ("%%Draw dotted lines")
DEFINITION ("if on, dotted lines will be drawn through your drawing.")
ENTRY ("Example")
NORMAL ("If you draw a Sound with a domain between 0 and 1 seconds "
	"to an amplitude range between -1 and 1, "
	"choosing ##Marks left...# with a number of 3 and %%Draw dotted lines% on, "
	"will give you horizontal marks and horizontal dotted lines at -1, 0, and 1; "
	"choosing ##Marks bottom...# with a number of 6 and %%Draw dotted lines% off, "
	"will give you vertical marks at 0, 0.2, 0.4, 0.6, 0.8, and 1.")
MAN_END

MAN_BEGIN ("Marks left/right/top/bottom every...", "ppgb", 19970330)
INTRO ("Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To draw a number of equally spaced marks into the @margins around the drawing area.")
ENTRY ("Arguments")
TAG ("%%Units")
DEFINITION ("the units, relative to the standard units, "
	"for writing the numbers; for example, if you want time in milliseconds "
	"instead of seconds (which is always the standard), "
	"%Units should be 0.001.")
TAG ("%%Distance")
DEFINITION ("the distance between the equally spaced marks that will be drawn, "
	"expressed in %Units; for example, if you want marks every 20 milliseconds, "
	"and %Units is 0.001, this argument should be 20.")
TAG ("%%Write numbers")
DEFINITION ("if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG ("%%Draw ticks")
DEFINITION ("if on, short line pieces will be drawn in the margin.")
TAG ("%%Draw dotted lines")
DEFINITION ("if on, dotted lines will be drawn through your drawing.")
MAN_END

MAN_BEGIN ("One logarithmic mark left/right/top/bottom...", "ppgb", 19970330)
INTRO ("Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To draw one mark into one of the four @margins "
	"around the drawing area, along a logarithmic axis.")
ENTRY ("Arguments")
TAG ("%%Position")
DEFINITION ("the %x (for top or bottom) or %y (for left or right) position of the mark, "
	"expressed in the logarithmic domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG ("%%Write number")
DEFINITION ("if on, a real number equal to %Position will be written in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG ("%%Draw tick")
DEFINITION ("if on, a short line piece will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG ("%%Draw dotted line")
DEFINITION ("if on, a dotted line will be drawn through your drawing, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG ("%%Draw text")
DEFINITION ("if not empty, this text will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
ENTRY ("Example")
NORMAL ("After you draw a Pitch logarithmically in a range between 100 and 400 Hz, "
	"choosing ##One logarithmic mark left...# with a position of 200 and %%Draw dotted line% on, "
	"will give you a horizontal mark \"200\" and a horizontal dotted line at a %y position of 200, "
	"which is exactly halfway between 100 and 400 Hz.")
MAN_END

MAN_BEGIN ("One mark left/right/top/bottom...", "ppgb", 19970330)
INTRO ("Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To draw one mark into one of the four @margins around the drawing area.")
ENTRY ("Arguments")
TAG ("%%Position")
DEFINITION ("the %x (for top or bottom) or %y (for left or right) position of the mark, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG ("%%Write number")
DEFINITION ("if on, a real number equal to `Position' will be written in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG ("%%Draw tick")
DEFINITION ("if on, a short line piece will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG ("%%Draw dotted line")
DEFINITION ("if on, a dotted line will be drawn through your drawing, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG ("%%Draw text")
DEFINITION ("if not empty, this text will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
ENTRY ("Example")
NORMAL ("If you draw a Sound to an amplitude range between -1 and 1, "
	"choosing ##One mark left...# with a position of 0.0 and %%Draw dotted line% on, "
	"will give you a horizontal mark \"0\" and a horizontal dotted line at a y position of 0.")
MAN_END

MAN_BEGIN ("Pen menu", "ppgb", 19961006)
INTRO ("One of the menus of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To choose the default line type and colour "
	"to be used in subsequent drawing of lines and painted areas.")
ENTRY ("Behaviour")
NORMAL ("The line type used by @@Draw inner box@ (plain), "
	"and the line type of the dotted lines in the ##Mark...# commands "
	"will not be affected.")
NORMAL ("The commands in the @Margins menu will always draw in black.")
MAN_END

MAN_BEGIN ("Picture window", "ppgb", 20041130)
INTRO ("One of the two main windows in P\\s{RAAT}.")
TAG ("File menu")
LIST_ITEM ("\\bu @@Read from Praat picture file...@, @@Write to Praat picture file...")
#if defined (macintosh)
LIST_ITEM ("\\bu @@Copy to clipboard@, @@Write to Mac PICT file...@")
#elif defined (_WIN32)
LIST_ITEM ("\\bu @@Copy to clipboard@, @@Write to Windows metafile...@")
#else
LIST_ITEM ("\\bu @@Copy to clipboard@")
#endif
LIST_ITEM ("\\bu @@PostScript settings...")
LIST_ITEM ("\\bu @@Write to EPS file...")
LIST_ITEM ("\\bu @@Print...")
TAG ("Edit menu")
LIST_ITEM ("\\bu @@Undo@, @@Erase all")
TAG ("@Margins menu")
LIST_ITEM ("\\bu @@Draw inner box")
LIST_ITEM ("\\bu @@Text left/right/top/bottom...")
LIST_ITEM ("\\bu @@Marks left/right/top/bottom every...")
LIST_ITEM ("\\bu @@One mark left/right/top/bottom...")
LIST_ITEM ("\\bu @@Marks left/right/top/bottom...")
LIST_ITEM ("\\bu @@Logarithmic marks left/right/top/bottom...")
LIST_ITEM ("\\bu @@One logarithmic mark left/right/top/bottom...")
LIST_ITEM ("\\bu @@Axes...")
TAG ("World menu")
LIST_ITEM ("\\bu @@Text...")
LIST_ITEM ("\\bu @@Axes...")
TAG ("Select menu")
LIST_ITEM ("\\bu @@Select inner viewport...@, @@Select outer viewport...@, @@Viewport text...")
TAG ("@@Pen menu")
TAG ("@@Font menu")
MAN_END

MAN_BEGIN ("PostScript settings...", "ppgb", 20040925)
INTRO ("One of the commands in the File menus of many windows. "
	"The PostScript settings influence @Printing and writing to @@Encapsulated PostScript@ files.")
ENTRY ("Arguments")
TAG ("%%Allow direct PostScript printing% (Windows and Macintosh only)")
DEFINITION ("this determines whether Praat prints explicit PostScript commands to your printer "
	"if it is a PostScript printer. This is what you will usually want. However, if you find "
	"that some of the options that you choose in the printing dialog seem not to be supported "
	"(e.g. scaling, printing two-up...), you may switch this off; Praat will then send native "
	"Windows or Macintosh drawing commands, which the printer driver will try to translate "
	"to PostScript. If your printer does not support PostScript, this switch is ignored. "
	"On Unix, this switch is superfluous, since all printing is done directly in PostScript.")
TAG ("%%Grey resolution")
DEFINITION ("you can choose from two image qualities:")
LIST_ITEM1 ("\\bu the %finest quality for grey plots (106 spots per inch), "
	"which gives the best results directly from the printer;")
LIST_ITEM1 ("\\bu a %photocopyable quality, which has fewer spots per inch (85) and "
	"gives the best results after photocopying.")
DEFINITION ("Your choice of the grey resolution influences direct PostScript printing "
	"and writing to @@Encapsulated PostScript@ files.")
TAG ("%%Paper size% (Unix only)")
DEFINITION ("you can choose from A4 (210 \\xx 297 mm), A3 (297 \\xx 420 mm) or US Letter (8.5 \\xx 11\\\"p). "
	"This choice applies to Unix only; on Windows, you choose the paper size in the ##Print...# dialog; "
	"on Macintosh, you choose the paper size in the ##Page setup...# dialog.")
TAG ("%%Orientation% (Unix only)")
DEFINITION ("you can choose between %portrait (e.g., 297 mm high and 210 mm wide) "
	"and %landscape (e.g., 210 mm high and 297 mm wide). "
	"This choice applies to Unix only; on Windows, you choose the orientation in the ##Print...# dialog; "
	"on Macintosh, you choose the orientation in the ##Page setup...# dialog.")
TAG ("%%Magnification% (Unix only)")
DEFINITION ("the relative size with which your picture will be printed; normally 1.0. "
	"This choice applies to Unix only; on Windows, you choose the scaling in the ##Print...# dialog; "
	"on Macintosh, you choose the scaling in the ##Page setup...# dialog.")
TAG ("%%Print command% (Unix only)")
DEFINITION ("When printing on Unix, a temporary PostScript^\\re file is created in the \"/tmp\" directory; "
	"it will have a name like \"picXXXXXX\", and is automatically removed after printing. "
	"This file is sent to the printer with the print command, which will often look like $$lp -c \\% s$, "
	"where $$\\% s$ stands for the file name.")
MAN_END

MAN_BEGIN ("Print...", "ppgb", 20001010)
INTRO ("One of the commands in the File menu of the @@Picture window@.")
NORMAL ("With this command, you send your entire picture immediately to the printer. "
	"See the @Printing tutorial for details.")
MAN_END

MAN_BEGIN ("Printing", "ppgb", 20041130)
NORMAL ("The best results will be obtained on PostScript printers, since these have built-in "
	"facilities for images (e.g. spectrograms) and rotated text. "
	"However, the printed page will look reasonable on colour inkjet printers as well.")
#if defined (UNIX)
ENTRY ("1. Printing on Unix")
NORMAL ("Most Unix networks (i.e. SGI, Solaris, HPUX) are traditionally connected to a PostScript printer. "
	"When you tell Praat to print a picture or manual page, Praat will write the picture to a temporary PostScript file "
	"and send this file to a printer with the %%print command% (typically $lp), which you can change "
	"with @@PostScript settings...@.")
NORMAL ("On Linux, you do not need a PostScript printer to print PostScript directly, "
	"because the #lpr program sends PostScript files through the GhostScript program, "
	"which is a part of all modern Linux distributions. The print command is typically "
	"$$lpr \\% s$. By changing the print command (with @@PostScript settings...@), "
	"you can change it to something fancier. For instance, if you want to save the woods "
	"and print two pages on one sheet of paper, you change it to $$cat \\% s | mpage -2 -o -f -m0 | lpr$.")
#elif defined (macintosh)
ENTRY ("1. Printing on Macintosh")
NORMAL ("Many Macintoshes are in a network that includes a PostScript printer (e.g. an Apple^\\re LaserWriter^\\re). "
	"If a PostScript printer is available, Praat will usually write direct PostScript commands to that printer "
	"(see @@PostScript settings...@ if you want to switch this off). "
	"P\\s{RAAT} also supports non-PostScript printers, such as most colour inkjet printers.")
#elif defined (_WIN32)
ENTRY ("1. Printing on Windows")
NORMAL ("Some Windows computers are in a network that includes a PostScript printer. "
	"If a PostScript printer is available, Praat will usually write direct PostScript commands to that printer "
	"(see @@PostScript settings...@ if you want to switch this off). "
	"P\\s{RAAT} also supports non-PostScript printers, such as most colour inkjet printers.")
#endif
ENTRY ("2. Indirect printing with GhostView")
NORMAL ("If you don't have a PostScript printer, and you still want PostScript quality, "
	"you can save the picture to an EPS file (@@Write to EPS file...@). "
	"You can then view this file with the freely available "
	"GhostView^\\tm program, which you can download from ##http://www.cs.wisc.edu/~ghost#.")
ENTRY ("3. Indirect printing with your word processor")
NORMAL ("If you save your picture to an EPS file, you will be able to include it as a picture in your favourite "
	"word processor (Microsoft^\\re Word^\\tm, LaTeX...). See @@Write to EPS file...@.")
NORMAL ("If you don't have a PostScript printer, you could again use GhostView^\\tm to print your document "
	"to any printer, after you printed your document to a PostScript file. You can do the same if you are "
	"the lucky owner of Adobe^\\re Acrobat^\\tm Distiller^\\tm, which is more reliable than GhostView "
	"but is also expensive.")
ENTRY ("4. Creating a PDF file")
NORMAL ("If you have Distiller or GhostView, you can print the entire Word^\\tm or LaTeX document to a PostScript file, "
	"and convert this to a PDF file, which anyone in the world can view and print with the free Adobe^\\re Acrobat^\\tm Reader program.")
#ifdef _WIN32
NORMAL ("Note: when creating a PDF file on Windows if you have Acrobat, ##do not use PDFWriter#, but choose Distiller as your printer. "
	"also, ##do not use \"Print to PDF\"# from your Microsoft Word File menu; otherwise, "
	"your EPS files will not show in the PDF file.")
#endif
#ifdef __MACH__
NORMAL ("Note: when creating a PDF file on MacOS X, ##do not use \"Save as PDF...\"# in your printing dialog, "
	"but choose Adobe PDF as your printer or save the document to a PostScript file and convert it with "
	"Distiller or GhostView; otherwise, your EPS files will not show in the PDF file.")
#endif
#ifdef _WIN32
ENTRY ("Indirect printing without PostScript")
NORMAL ("Pictures included in your word processor via @@Copy to clipboard@ or @@Write to Windows metafile...@ "
	"will print fine, though not as nicely as EPS files.")
#endif
#ifdef macintosh
ENTRY ("Indirect printing without PostScript")
NORMAL ("Pictures included in your word processor via @@Copy to clipboard@ or @@Write to Mac PICT file...@ "
	"will print fine, though not as nicely as EPS files.")
#endif
MAN_END

MAN_BEGIN ("Read from Praat picture file...", "ppgb", 19960908)
INTRO ("One of the commands in the File menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To read a picture that you saved earlier "
	"with @@Write to Praat picture file...@.")
ENTRY ("Behaviour")
NORMAL ("The picture will be drawn across whatever is currently visible in the Picture window.")
ENTRY ("Usage")
NORMAL ("With the help of this command, you can transfer a picture from a Unix machine to a Macintosh. "
	"Praat for Macintosh can write the picture to an @@Encapsulated PostScript@ file "
	"with a screen preview.")
MAN_END

MAN_BEGIN ("Text...", "ppgb", 19970330)
INTRO ("One of the commands in the #World menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To write text inside the drawing area.")
ENTRY ("Scope")
NORMAL ("This works with all the drawings that leave @margins around themselves.")
ENTRY ("Arguments")
TAG ("%x")
DEFINITION ("horizontal position, expressed in the horizontal domain of your latest drawing.")
TAG ("%y")
DEFINITION ("vertical position, expressed in the vertical range or domain of your latest drawing.")
TAG ("%%Horizontal alignment")
DEFINITION ("determines the horizontal alignment of the text relative to %x.")
TAG ("%%Vertical alignment")
DEFINITION ("determines the vertical alignment of the text relative to %y.")
TAG ("%%Text")
DEFINITION ("will be drawn in the current font and font size "
	"that you set with the @@Font menu@.")
ENTRY ("Usage")
NORMAL ("With the ##Text...# command, you can use all @@special symbols@ and @@text styles@.")
MAN_END

MAN_BEGIN ("Text left/right/top/bottom...", "ppgb", 19970330)
INTRO ("Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To write text into the @margins around the drawing area.")
ENTRY ("Behaviour")
NORMAL ("The text will be centred along the side. "
	"Text at the left or right will be turned by 90 degrees "
	"and written up and down, respectively.")
MAN_END

MAN_BEGIN ("Undo", "ppgb", 20041108)
INTRO ("One of the commands in the #Edit menu of the @@Picture window@.")
NORMAL ("It erases your most recently created drawing, which could have come from a command in the Objects window "
	"or from one of the drawing commands in the World and @Margins menus.")
ENTRY ("Behaviour")
NORMAL ("This command will erase some drawings, but it will not change your settings in the #Pen and #Font menus "
	"(line type, line width, font, font size, and colour).")
NORMAL ("The world window will be what it was after the latest-but-one drawing, "
	"so that you can use the @Margins menu as if the latest drawing had never happened.")
MAN_END

MAN_BEGIN ("Select inner viewport...", "ppgb", 20041108)
INTRO ("One of the commands in the #Select menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To determine where your next drawing will occur.")
ENTRY ("The viewport")
#ifdef macintosh
#ifdef __MACH__
NORMAL ("The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"You can set the colour of its margins with ##System Preferences \\-> Appearance \\-> Highlight Colour#. ")
#else
NORMAL ("The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"You can set the colour of its margins with ##Control Panel \\-> Appearance \\-> Appearance \\-> Highlight Colour#. ")
#endif
#else
NORMAL ("The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"Its margins are pink.")
#endif
NORMAL ("The %inner viewport does not include the margins, the %outer viewport does (see @@Select outer viewport...@).")
NORMAL ("Normally, you select the viewport by dragging your mouse across the Picture window. "
	"However, you would use this explicit command:")
LIST_ITEM ("\\bu from a script;")
LIST_ITEM ("\\bu if you want a viewport that cannot be expressed in halves of an inch.")
MAN_END

MAN_BEGIN ("Select outer viewport...", "ppgb", 20041108)
INTRO ("One of the commands in the #Select menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To determine where your next drawing will occur.")
ENTRY ("The viewport")
#ifdef macintosh
#ifdef __MACH__
NORMAL ("The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"You can set the colour of its margins with ##System Preferences \\-> Appearance \\-> Highlight Colour#. ")
#else
NORMAL ("The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"You can set the colour of its margins with ##Control Panel \\-> Appearance \\-> Appearance \\-> Highlight Colour#. ")
#endif
#else
NORMAL ("The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"Its margins are pink.")
#endif
NORMAL ("The %outer viewport includes the margins, the %inner viewport does not (see @@Select inner viewport...@).")
NORMAL ("Normally, you select the viewport by dragging your mouse across the Picture window. "
	"However, you would use this explicit command:")
LIST_ITEM ("\\bu from a script;")
LIST_ITEM ("\\bu if you want a viewport that cannot be expressed in halves of an inch.")
MAN_END

MAN_BEGIN ("Viewport text...", "ppgb", 19970330)
INTRO ("One of the commands in the #Select menu of the @@Picture window@.")
ENTRY ("Purpose")
NORMAL ("To write text inside the viewport, at nine different places, "
	"with a rotation between 0 to 360 degrees.")
ENTRY ("Arguments:")
TAG ("%%Horizontal alignment")
DEFINITION ("determines the horizontal alignment of the text:")
LIST_ITEM ("    \\bu %Left means pushed against the left edge of the viewport;")
LIST_ITEM ("    \\bu %Right means pushed against the right edge of the viewport;")
LIST_ITEM ("    \\bu %Centre means horizontally centred in the viewport.")
TAG ("%%Vertical alignment")
DEFINITION ("determines the vertical alignment of the text:")
LIST_ITEM ("    \\bu %Top means pushed against the top of the viewport;")
LIST_ITEM ("    \\bu %Bottom means pushed against the bottom of the viewport;")
LIST_ITEM ("    \\bu %Half means vertically centred in the viewport.")
TAG ("%%Text")
DEFINITION ("will be drawn in the current font and font size "
	"that you set with the @@Font menu@.")
ENTRY ("Behaviour")
NORMAL ("For rotated text, the alignment arguments will not only determine "
	"the position inside the viewport, "
	"but also the alignment in the rotated coordinate system. "
	"This gives surprises now and then; "
	"so, if you want several rotated texts that align with each other, "
	"you should do this by varying the viewport, not the alignment.")
ENTRY ("Usage")
NORMAL ("You can use all @@special symbols@ and @@text styles@.")
MAN_END

MAN_BEGIN ("Write to EPS file...", "ppgb", 20041130)
INTRO ("A command in the File menu of the @@Picture window@.")
NORMAL ("It saves the picture to an @@Encapsulated PostScript@ (EPS) file, "
	"which can be imported by many other programs, such as Microsoft^\\re Word^\\tm.")
ENTRY ("PostScript = highest possible quality!")
NORMAL ("With EPS files you can use high-quality graphics in your word-processor documents. "
	"The quality is higher than if you use @@Copy to clipboard@.")
#ifdef _WIN32
ENTRY ("The big limitation")
NORMAL ("EPS pictures imported in Word for Windows will show correctly only on PostScript printers, or with GhostView, or in PDF files "
	"created by Adobe^\\re Acrobat^\\tm Distiller^\\tm. "
	"To print EPS pictures on non-PostScript printers, use a Linux or Macintosh computer.")
#endif
#ifdef macintosh
ENTRY ("The big limitation")
NORMAL ("If you have MacOS X 10.3 or higher, you are lucky. "
	"These system versions can write EPS (and other PostScript files) to non-PostScript printers, such as most inkjet printers. "
	"With older system versions, however, EPS pictures imported in Word for Macintosh will show correctly only on PostScript printers, "
	"or with GhostView, or in PDF files created by Adobe^\\re Acrobat^\\tm Distiller^\\tm.")
#endif
#ifdef macintosh
ENTRY ("Usage")
NORMAL ("To import an EPS file in Word 5.1, choose ##File...# or ##Picture...# from the #Insert menu. "
	"To do it in Word X, choose #Insert \\-> #Picture \\-> ##From file...#. "
	"Word will create a picture with the same size as the originally selected part of the Picture window (the %viewport).")
NORMAL ("Word will show you a screen preview in a mediocre quality, but you will see the high-quality PostScript version when you print.")
#endif
#ifdef _WIN32
ENTRY ("Usage")
NORMAL ("If Word cannot read your EPS files, you may have to install EPS support from the Office^\\tm CD, "
	"because the standard installation of Office may not support EPS files. "
	"When you import an EPS file in an older version of Word, you may only see the file name and the date and time of creation, "
	"and a message that the picture will print correctly to a PostScript printer (it will, if the PostScript driver has been selected).")
#endif
ENTRY ("Behaviour")
NORMAL ("Though all the contents of the Picture window are written to the EPS file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in Word (or another program).")
ENTRY ("Settings")
NORMAL ("The EPS picture is saved with the grey resolution and fonts that you specified with @@PostScript settings...@.")
#ifdef macintosh
ENTRY ("Technical information")
NORMAL ("On the Macintosh, the EPS file will consist of PostScript text (for the printer, in the `data fork') "
	"plus a bitmapped screen preview (in the `resource fork'). The screen preview is lost if you transfer the EPS file "
	"to a Windows or Unix computer, although an EPS picture imported in a Word for Macintosh document retains its preview "
	"when the Word document is transferred to a Windows computer.")
#endif
MAN_END

MAN_BEGIN ("Write to Praat picture file...", "ppgb", 20041130)
INTRO ("A command in the File menu of the @@Picture window@.")
NORMAL ("It saves a picture in a format that can be imported into P\\s{RAAT} later with @@Read from Praat picture file...@.")
ENTRY ("Usage")
NORMAL ("With the help of this command, you can transfer the contents of the picture window between computers or even between platforms, "
	"for instance from a Macintosh to a Windows computer.")
MAN_END

#ifdef macintosh
MAN_BEGIN ("Write to Mac PICT file...", "ppgb", 20041130)
INTRO ("A command in the File menu of the @@Picture window@.")
NORMAL ("It saves the selected part of the picture in an \"extended PICT2\" format, "
	"which can be imported by many programs on the Mac, like MacDraw^\\tm "
	"(Microsoft^\\re Word^\\tm 5.1 unfortunately rounds the high resolution down to screen pixels).")
ENTRY ("Behaviour")
NORMAL ("Though all the picture data will be written to the PICT file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in the other program.")
ENTRY ("Usage")
NORMAL ("You will not use this command very often, "
	"because it is usually easier to copy the selection to the clipboard with the @@Copy to clipboard@ command, "
	"and `Paste' it into the other program. You may use a PICT file instead of the clipboard if the clipboard is too large "
	"for the other program to read, or if you want to transfer the picture to another computer.")
NORMAL ("If you have a PostScript printer (or MacOS X 10.3 with %any printer), you would use @@Write to EPS file...@ instead "
	"for best printing results.")
MAN_END
#endif

#ifdef _WIN32
MAN_BEGIN ("Write to Windows metafile...", "ppgb", 20041130)
INTRO ("A command in the File menu of the @@Picture window@.")
NORMAL ("It saves the selected part of the picture in an \"enhanced metafile\" (.EMF) format, "
	"which can be imported by many Windows programs, like Adobe^\\re Illustrator^\\tm or Microsoft^\\re Word^\\tm.")
ENTRY ("Behaviour")
NORMAL ("Though all the picture data will be written to the file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in the other program.")
ENTRY ("Usage")
NORMAL ("You will not use this command very often, "
	"because it is usually easier to copy the selection to the clipboard with the @@Copy to clipboard@ command, "
	"and `Paste' it into the other program. You may use a metafile instead of the clipboard if the clipboard is too large "
	"for the other program to read, or if you want to transfer the picture to another computer.")
NORMAL ("If you have a PostScript printer, you would use @@Write to EPS file...@ instead "
	"for best printing results.")
MAN_END
#endif

}

/* End of file manual_Picture.c */
