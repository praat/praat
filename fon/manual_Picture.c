/* manual_Picture.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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

static void drawOneIpaSymbol (Graphics g, double x, double y, const wchar_t *symbol) {
	wchar_t buffer [30], *p = & buffer [0];
	int fontSize = Graphics_inqFontSize (g);
	Graphics_rectangle (g, x - 0.5, x + 0.5, y - 0.5, y + 0.5);
	if (! symbol) return;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFont (g, Graphics_FONT_TIMES);
	Graphics_setFontSize (g, fontSize * 3 / 2);
	Graphics_text (g, x, y + 0.25, symbol);
	while (*symbol) {
		if (*symbol == '\\') { *p++ = '\\'; *p++ = 'b'; *p++ = 's'; }   /* Visualize backslash (\bs). */
		else { *p++ = '\\'; *p++ = *symbol; *p++ = ' '; }   /* Visualize special symbols (% ^ _ #). */
		symbol ++;
	}
	*p = '\0';   /* Trailing null byte. */
	Graphics_setFont (g, Graphics_FONT_HELVETICA);
	Graphics_setFontSize (g, fontSize * 5 / 6);
	Graphics_text (g, x, y - 0.25, buffer);
	Graphics_setFontSize (g, fontSize);
}

static void draw_IPA_consonant_chart (Graphics graphics) {
	Graphics_setWindow (graphics, 0, 20, 6, 19.5);
	Graphics_setFontStyle (graphics, Graphics_ITALIC);
	Graphics_setTextRotation (graphics, 60);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (graphics, 4, 17.6, L"bilabial");
	Graphics_text (graphics, 5, 17.6, L"labiodental");
	Graphics_text (graphics, 6, 17.6, L"dental");
	Graphics_text (graphics, 7, 17.6, L"alveolar");
	Graphics_text (graphics, 8, 17.6, L"alv. lateral");
	Graphics_text (graphics, 9, 17.6, L"postalveolar");
	Graphics_text (graphics, 10, 17.6, L"retroflex");
	Graphics_text (graphics, 11, 17.6, L"alveolo-palatal");
	Graphics_text (graphics, 12, 17.6, L"palatal");
	Graphics_text (graphics, 13, 17.6, L"labial-palatal");
	Graphics_text (graphics, 14, 17.6, L"labial-velar");
	Graphics_text (graphics, 15, 17.6, L"velar");
	Graphics_text (graphics, 16, 17.6, L"uvular");
	Graphics_text (graphics, 17, 17.6, L"pharyngeal");
	Graphics_text (graphics, 18, 17.6, L"epiglottal");
	Graphics_text (graphics, 19, 17.6, L"glottal");
	Graphics_setTextRotation (graphics, 0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 3.3, 17, L"voiceless plosive");
	Graphics_text (graphics, 3.3, 16, L"voiced plosive");
	Graphics_text (graphics, 3.3, 15, L"nasal");
	Graphics_text (graphics, 3.3, 14, L"voiceless fricative");
	Graphics_text (graphics, 3.3, 13, L"voiced fricative");
	Graphics_text (graphics, 3.3, 12, L"approximant");
	Graphics_text (graphics, 3.3, 11, L"trill");
	Graphics_text (graphics, 3.3, 10, L"tap or flap");
	Graphics_text (graphics, 3.3, 9, L"lateral approx.");
	Graphics_text (graphics, 3.3, 8, L"implosive");
	Graphics_text (graphics, 3.3, 7, L"click");
	Graphics_setFontStyle (graphics, Graphics_NORMAL);
	{
		static struct { float x, y; wchar_t *string; } symbols [] = {
{ 4, 17, L"p" }, { 4, 16, L"b" }, { 4, 15, L"m" }, { 4, 14, L"\\ff" }, { 4, 13, L"\\bf" }, { 4, 11, L"\\bc" }, { 4, 8, L"\\b^" }, { 4, 7, L"\\O." },
{ 5, 15, L"\\mj" }, { 5, 14, L"f" }, { 5, 13, L"v" }, { 5, 12, L"\\vs" },
{ 6, 14, L"\\tf" }, { 6, 13, L"\\dh" }, { 6, 7, L"\\|1" },
{ 7, 17, L"t" }, { 7, 16, L"d" }, { 7, 15, L"n" }, { 7, 14, L"s" }, { 7, 13, L"z" }, { 7, 12, L"\\rt" }, { 7, 11, L"r" }, { 7, 10, L"\\fh" }, { 7, 9, L"l" }, { 7, 8, L"\\d^" },
{ 8, 17, L"t^l" }, { 8, 16, L"d^l" }, { 8, 14, L"\\l-" }, { 8, 13, L"\\lz" }, { 8, 12, L"l" }, { 8, 10, L"\\rl" }, { 8, 9, L"l" }, { 8, 7, L"\\|2" },
{ 9, 14, L"\\sh" }, { 9, 13, L"\\zh" }, { 9, 7, L"\\|-" },
{ 10, 17, L"\\t." }, { 10, 16, L"\\d." }, { 10, 15, L"\\n." }, { 10, 14, L"\\s." }, { 10, 13, L"\\z." }, { 10, 12, L"\\r." }, { 10, 10, L"\\f." }, { 10, 9, L"\\l." }, { 10, 7, L"!" },
{ 11, 14, L"\\cc" }, { 11, 13, L"\\zc" },
{ 12, 17, L"c" }, { 12, 16, L"\\j-" }, { 12, 15, L"\\nj" }, { 12, 14, L"\\c," }, { 12, 13, L"\\jc" }, { 12, 12, L"j" }, { 12, 9, L"\\yt" }, { 12, 8, L"\\j^" },
{ 13, 12, L"\\ht" },
{ 14, 14, L"\\wt" }, { 14, 12, L"w" },
{ 15, 17, L"k" }, { 15, 16, L"\\gs" }, { 15, 15, L"\\ng" }, { 15, 14, L"x" }, { 15, 13, L"\\gf" }, { 15, 12, L"\\ml" }, { 15, 9, L"\\lc" }, { 15, 8, L"\\g^" },
{ 16, 17, L"q" }, { 16, 16, L"\\gc" }, { 16, 15, L"\\nc" }, { 16, 14, L"\\cf" }, { 16, 13, L"\\ri" }, { 16, 11, L"\\rc" }, { 16, 8, L"\\G^" },
{ 17, 14, L"\\h-" }, { 17, 13, L"\\9e" },
{ 18, 17, L"\\?-" }, { 18, 14, L"\\hc" }, { 18, 13, L"\\9-" },
{ 19, 17, L"\\?g" }, { 19, 14, L"h" }, { 19, 13, L"\\h^" },
		{ 0, 0, NULL } };
		for (int i = 0; symbols [i]. string != NULL; i ++)
			drawOneIpaSymbol (graphics, symbols [i]. x, symbols [i]. y, symbols [i]. string);
	}
}

static void draw_IPA_vowel_chart (Graphics graphics) {
	Graphics_setWindow (graphics, 0, 19, -0.5, 7.5);
	Graphics_setFontStyle (graphics, Graphics_ITALIC);
	Graphics_setTextRotation (graphics, 60);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (graphics, 4.5, 6.6, L"front");
	Graphics_text (graphics, 7, 6.6, L"central");
	Graphics_text (graphics, 9.5, 6.6, L"back");
	Graphics_setTextRotation (graphics, 0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 3.3, 6, L"close");
	Graphics_text (graphics, 3.3, 5, L"close centralized");
	Graphics_text (graphics, 3.3, 4, L"close-mid");
	Graphics_text (graphics, 3.3, 2, L"open-mid");
	Graphics_text (graphics, 3.3, 0, L"open");
	Graphics_setFontStyle (graphics, Graphics_NORMAL);
	{
		static struct { float x, y; wchar_t *string; } symbols [] = {
{ 4, 6, L"i" }, { 5, 6, L"y" }, { 6.5, 6, L"\\i-" }, { 7.5, 6, L"\\u-" }, { 9, 6, L"\\mt" }, { 10, 6, L"u" },
{ 4.2, 5, L"\\ic" }, { 5.2, 5, L"\\yc" }, { 9.8, 5, L"\\hs" },
{ 4, 4, L"e" }, { 5, 4, L"\\o/" }, { 6.5, 4, L"\\e-" }, { 7.5, 4, L"\\o-" }, { 9, 4, L"\\rh" }, { 10, 4, L"o" },
{ 7, 3, L"\\sw" }, { 7, 1, L"\\at" },
{ 4, 2, L"\\ef" }, { 5, 2, L"\\oe" }, { 6.5, 2, L"\\er" }, { 7.5, 2, L"\\kb" }, { 9, 2, L"\\vt" }, { 10, 2, L"\\ct" },
{ 4, 1, L"\\ae" },
{ 4, 0, L"a" }, { 5, 0, L"\\Oe" }, { 9, 0, L"\\as" }, { 10, 0, L"\\ab" },
		{ 0, 0, NULL } };
		for (int i = 0; symbols [i]. string != NULL; i ++)
			drawOneIpaSymbol (graphics, symbols [i]. x, symbols [i]. y, symbols [i]. string);
	}
}

void manual_Picture_init (ManPages me);
void manual_Picture_init (ManPages me) {

MAN_BEGIN (L"Special symbols", L"ppgb", 20070924)
INTRO (L"When drawing text into the @@Picture window@ or into the @@TextGridEditor@, "
	"you can use %%backslash sequences% to display various kinds of special symbols. "
	"You can usually also type the characters themselves, if your computer has an input method for them.")

ENTRY (L"European symbols")
NORMAL (L"To get the symbol \"\\a^\" (a-circumflex), you type \"\\bsa\\^ \", i.e., "
	"a sequence of backslash + a + circumflex. In this way, you can get a hundred "
	"non-ASCII symbols that are used in the alphabets of many European languages.")
LIST_ITEM (L"\\a\" \\bsa\\\"   \\e\" \\bse\\\"   \\i\" \\bsi\\\"   \\o\" \\bso\\\"   "
	"\\u\" \\bsu\\\"   \\y\" \\bsy\\\"  "
	"\\A\" \\bsA\\\"   \\E\" \\bsE\\\"   \\I\" \\bsI\\\"   \\O\" \\bsO\\\"   \\U\" \\bsU\\\"   "
	"\\Y\" \\bsY\\\" ")
LIST_ITEM (L"\\a\' \\bsa\'  \\c\' \\bsc\'  \\e\' \\bse\'  \\i\' \\bsi\'  \\n\' \\bsn\'  \\o\' \\bso\'  \\s\' \\bss\'  \\u\' \\bsu\'  \\y\' \\bsy\'  \\z\' \\bsz\'  "
	"\\A\' \\bsA\'  \\C\' \\bsC\'  \\E\' \\bsE\'  \\I\' \\bsI\'  \\N\' \\bsN\'  \\O\' \\bsO\'  \\S\' \\bsS\'  \\U\' \\bsU\'  \\Y\' \\bsY\'  \\Z\' \\bsZ\'")
LIST_ITEM (L"\\o: \\bso:  \\u: \\bsu:  \\O: \\bsO:  \\U: \\bsU:")
LIST_ITEM (L"\\a` \\bsa`  \\e` \\bse`  \\i` \\bsi`  \\o` \\bso`  \\u` \\bsu`  "
	"\\A` \\bsA`  \\E` \\bsE`  \\I` \\bsI`  \\O` \\bsO`  \\U` \\bsU`")
LIST_ITEM (L"\\a^ \\bsa\\^   \\e^ \\bse\\^   \\i^ \\bsi\\^   \\o^ \\bso\\^   \\u^ \\bsu\\^   "
	"\\A^ \\bsA\\^   \\E^ \\bsE\\^   \\I^ \\bsI\\^   \\O^ \\bsO\\^   \\U^ \\bsU\\^ ")
LIST_ITEM (L"\\a~ \\bsa\\~   \\n~ \\bsn\\~   \\o~ \\bso\\~   \\A~ \\bsA\\~   \\N~ \\bsN\\~   \\O~ \\bsO\\~  ")
LIST_ITEM (L"\\c< \\bsc<  \\d< \\bsd<  \\e< \\bse<  \\g< \\bsg<  \\n< \\bsn<  \\r< \\bsr<  \\s< \\bss<  \\t< \\bst<  \\z< \\bsz<  "
	"\\C< \\bsC<  \\D< \\bsD<  \\E< \\bsE<  \\G< \\bsG<  \\N< \\bsN<  \\R< \\bsR<  \\S< \\bsS<  \\T< \\bsT<  \\Z< \\bsZ<")
LIST_ITEM (L"\\ao \\bsao  \\uo \\bsuo  \\Ao \\bsAo  \\Uo \\bsUo")
LIST_ITEM (L"\\ae \\bsae  \\Ae \\bsAe  \\o/ \\bso/  \\O/ \\bsO/  "
	"\\c, \\bsc,  \\C, \\bsC,  \\l/ \\bsl/  \\L/ \\bsL/  \\ss \\bsss  \\th \\bsth  \\z! \\bsz!  \\Z! \\bsZ!")
LIST_ITEM (L"\\!d \\bs!d  \\?d \\bs?d")
LIST_ITEM (L"\\eu \\bseu %euro, \\Lp \\bsLp %%pound sterling%, \\Y= \\bsY= %yen, "
	"\\fd \\bsfd %florin, \\c/ \\bsc/ %cent")
LIST_ITEM (L"\\SS \\bsSS %section, \\|| \\bs|| %pilcrow (%paragraph)")
LIST_ITEM (L"\\co \\bsco %copyright, \\re \\bsre %registered, \\tm \\bstm %trademark")
LIST_ITEM (L"\\a_ \\bsa_ %%feminine ordinal%, \\o_ \\bso_ %%masculine ordinal%")
LIST_ITEM (L"\\<< \\bs<< %%left-pointing guillemet%, \\>> \\bs>> %%right-pointing guillemet%")

ENTRY (L"Mathematical symbols")
LIST_ITEM (L"\\.c \\bs.c %%middle dot%, \\xx \\bsxx %multiplication, \\:- \\bs:- %division, \\/d \\bs/d %%division slash%")
LIST_ITEM (L"\\dg \\bsdg %degree, \\\'p \\bs\'p %prime (%minute), \\\"p \\bs\\\" p %%double prime% (%second)")
LIST_ITEM (L"\\-m \\bs-m %minus, \\-- \\bs-- %%en-dash%, \\+- \\bs+- %%plus-minus%")
LIST_ITEM (L"\\<_ \\bs<_ %%less than or equal to%, \\>_ \\bs>_ %%greater than or equal to%, \\=/ \\bs=/ %%not equal to%")
LIST_ITEM (L"\\no \\bsno %%logical not%, \\an \\bsan %%logical and%, \\or \\bsor %%logical or%")
LIST_ITEM (L"\\At \\bsAt %%for all%, \\Er \\bsEr %%there exists%, \\.3 \\bs.3 %therefore")
LIST_ITEM (L"\\oc \\bsoc %%proportional to%, \\=3 \\bs=3 %%defined as% (or %%congruent modulo%), \\~~ \\bs~~ %%approximately equal to%")
LIST_ITEM (L"\\Vr \\bsVr %%square root%")
LIST_ITEM (L"\\<- \\bs<-, \\-> \\bs->, \\<> \\bs<>")
LIST_ITEM (L"\\<= \\bs<=, \\=> \\bs=>, \\eq \\bseq")
LIST_ITEM (L"\\^| \\bs\\^ |, \\=~ \\bs=~ %%congruent to%, \\_| \\bs_|")
LIST_ITEM (L"\\oo \\bsoo %infinity, \\Tt \\bsTt %%up tack% (%%perpendicular to%)")
LIST_ITEM (L"\\O| \\bsO| %%empty set%, \\ni \\bsni %intersection, \\uu \\bsuu %union, "
	"\\c= \\bsc= %%subset of%, \\e= \\bse= %%element of%")
LIST_ITEM (L"\\dd \\bsdd %%partial differential%")
LIST_ITEM (L"\\ox \\bsox %%circled times%, \\o+ \\bso+ %%circled plus%")
LIST_ITEM (L"\\su \\bssu %summation, \\in \\bsin %integral")

ENTRY (L"Greek letters")
NORMAL (L"To get \\ep\\up\\ro\\et\\ka\\al, you type \\bsep\\bsup\\bsro\\bset\\bska\\bsal.")
LIST_ITEM (L"\t\\al \\bsal \t\\Al \\bsAl %alpha")
LIST_ITEM (L"\t\\be \\bsbe \t\\Be \\bsBe %beta")
LIST_ITEM (L"\t\\ga \\bsga \t\\Ga \\bsGa %gamma")
LIST_ITEM (L"\t\\de \\bsde \t\\De \\bsDe %delta")
LIST_ITEM (L"\t\\ep \\bsep \t\\Ep \\bsEp %epsilon")
LIST_ITEM (L"\t\\ze \\bsze \t\\Ze \\bsZe %zeta")
LIST_ITEM (L"\t\\et \\bset \t\\Et \\bsEt %eta")
LIST_ITEM (L"\t\\te \\bste \t\\Te \\bsTe %theta \t\\t2 \\bst2")
LIST_ITEM (L"\t\\io \\bsio \t\\Io \\bsIo %iota")
LIST_ITEM (L"\t\\ka \\bska \t\\Ka \\bsKa %kappa")
LIST_ITEM (L"\t\\la \\bsla \t\\La \\bsLa %lambda")
LIST_ITEM (L"\t\\mu \\bsmu \t\\Mu \\bsMu %mu")
LIST_ITEM (L"\t\\nu \\bsnu \t\\Nu \\bsNu %nu")
LIST_ITEM (L"\t\\xi \\bsxi \t\\Xi \\bsXi %xi")
LIST_ITEM (L"\t\\on \\bson \t\\On \\bsOn %omicron")
LIST_ITEM (L"\t\\pi \\bspi \t\\Pi \\bsPi %pi")
LIST_ITEM (L"\t\\ro \\bsro \t\\Ro \\bsRo %rho")
LIST_ITEM (L"\t\\si \\bssi \t\\Si \\bsSi %sigma \t\\s2 \\bss2")
LIST_ITEM (L"\t\\ta \\bsta \t\\Ta \\bsTa %tau")
LIST_ITEM (L"\t\\up \\bsup \t\\Up \\bsUp %upsilon")
LIST_ITEM (L"\t\\fi \\bsfi \t\\Fi \\bsFi %phi \t\\f2 \\bsf2")
LIST_ITEM (L"\t\\ci \\bsci \t\\Ci \\bsCi %chi")
LIST_ITEM (L"\t\\ps \\bsps \t\\Ps \\bsPs %psi")
LIST_ITEM (L"\t\\om \\bsom \t\\Om \\bsOm %omega \t\\o2 \\bso2")

ENTRY (L"Hebrew letters")
NORMAL (L"To get an alef, you type \\bs?+. To type a longer Hebrew text, you %may have to type the letters "
	"in reverse order (this can happen on Windows and is a known bug). "
	"If you mix Hebrew and Latin text, the Latin text may appear in reverse order "
	"(this can happen on the Mac and is another known bug).")
/*
 * The purpose of the "$$ $" sequences below is to prevent the trigraph text from reversing (on the Mac).
 */
LIST_ITEM (L"\t\\?+$$ $\\bs?+\t%alef")
LIST_ITEM (L"\t\\B+$$ $\\bsB+\t%bet")
LIST_ITEM (L"\t\\G+$$ $\\bsG+\t%gimel")
LIST_ITEM (L"\t\\D+$$ $\\bsD+\t%dalet")
LIST_ITEM (L"\t\\H+$$ $\\bsH+\t%he")
LIST_ITEM (L"\t\\V+$$ $\\bsV+\t%vav")
LIST_ITEM (L"\t\\Z+$$ $\\bsZ+\t%zayin")
LIST_ITEM (L"\t\\X+$$ $\\bsX+\t%het")
LIST_ITEM (L"\t\\Y+$$ $\\bsY+\t%tet")
LIST_ITEM (L"\t\\J+$$ $\\bsJ+\t%yod")
LIST_ITEM (L"\t\\K+$$ $\\bsK+\t%kaf\t\\K%$$ $\\bsK\\% ")
LIST_ITEM (L"\t\\L+$$ $\\bsL+\t%lamed")
LIST_ITEM (L"\t\\M+$$ $\\bsM+\t%mem\t\\M%$$ $\\bsM\\% ")
LIST_ITEM (L"\t\\N+$$ $\\bsN+\t%nun\t\\N%$$ $\\bsN\\% ")
LIST_ITEM (L"\t\\S+$$ $\\bsS+\t%samekh")
LIST_ITEM (L"\t\\9+$$ $\\bs9+\t%ayin")
LIST_ITEM (L"\t\\P+$$ $\\bsP+\t%pe\t\\P%$$ $\\bsP\\% ")
LIST_ITEM (L"\t\\C+$$ $\\bsC+\t%tsadi\t\\C%$$ $\\bsC\\% ")
LIST_ITEM (L"\t\\Q+$$ $\\bsQ+\t%qof")
LIST_ITEM (L"\t\\R+$$ $\\bsR+\t%resh")
LIST_ITEM (L"\t\\W+$$ $\\bsW+\t%shin")
LIST_ITEM (L"\t\\T+$$ $\\bsT+\t%tav")
LIST_ITEM (L"\t\\?+\\hI$$ $\\bs?+\\bshI\t%hiriq")
LIST_ITEM (L"\t\\?+\\sE$$ $\\bs?+\\bssE\t%segol\t\\?+\\cE$$ $\\bs?+\\bscE\t%tsere")
LIST_ITEM (L"\t\\?+\\qA$$ $\\bs?+\\bsqA\t%qamats\t\\?+\\pA$$ $\\bs?+\\bspA\t%patah")
LIST_ITEM (L"\t\\?+\\hO$$ $\\bs?+\\bshO\t%holam")
LIST_ITEM (L"\t\\?+\\qU$$ $\\bs?+\\bsqU\t%qubuts")
LIST_ITEM (L"\t\\dq$$ $\\bsdq\t%dagesh or %maliq")
NORMAL (L"Combinations:")
LIST_ITEM (L"\t\\vO$$ $\\bsvO\t%%vav holam")
LIST_ITEM (L"\t\\sU$$ $\\bssU\t%shuruq")

ENTRY (L"Phonetic symbols")
NORMAL (L"See @@Phonetic symbols@")

ENTRY (L"Miscellaneous")
LIST_ITEM (L"\\bs \\bsbs %backslash")
LIST_ITEM (L"\\bu \\bsbu %bullet")
LIST_ITEM (L"\\cl \\bscl (%club), \\di \\bsdi (%diamond), \\he \\bshe (%heart), \\sp \\bssp (%spade)")
LIST_ITEM (L"\\pf \\bspf %%pointing finger%, \\f5 \\bsf5 %%flower-five%")

ENTRY (L"See also")
NORMAL (L"@@Text styles@")
MAN_END

MAN_BEGIN (L"Text styles", L"ppgb", 19961012)
INTRO (L"When drawing text into the @@Picture window@ or into an editor, "
	"you can use text styles other than regular Roman.")
ENTRY (L"Italic, bold, superscript, subscript")
NORMAL (L"With the following symbols, you introduce stretches of text drawn in special styles:")
LIST_ITEM (L"\\% : the following letter will be italic.")
LIST_ITEM (L"\\# : the following letter will be bold.")
LIST_ITEM (L"\\# \\% : the following letter will be bold-italic.")
LIST_ITEM (L"\\^ : the following letter will be superscript: \\% m\\% c\\^ 2 gives %mc^2.")
LIST_ITEM (L"\\_ : the following letter will be subscript. Example: %F__0_ is typed as \\% F_0.")
LIST_ITEM (L"\\% \\% : the following letters will be italic, until the following \\% :")
LIST_ITEM (L"    $$Now \\% \\% you\\%  try$   gives:   Now %%you% try.")
LIST_ITEM (L"The same goes for \\# \\# , \\^ \\^ , and \\_ \\_ .")
LIST_ITEM (L"\\bss{...}: small:")
LIST_ITEM (L"    $$W\\bss{ARP}$   gives:   W\\s{ARP}")
NORMAL (L"To draw a \\% , \\# , \\^ , or \\_  symbol, you type \"$$\\bs\\%  $\", "
	"\"$$\\bs\\#  $\", \"$$\\bs\\^  $\", or \"$$\\bs\\_  $\": a backslash, the symbol, and a space.")
NORMAL (L"On Xwindows machines, the font `Symbol' will never look bold or italic, "
	"but it will be printed correctly.")
ENTRY (L"See also")
NORMAL (L"@@Special symbols@")
MAN_END

MAN_BEGIN (L"Phonetic symbols", L"ppgb", 20070825)
NORMAL (L"To draw phonetic symbols in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can then use backslash sequences as described in:")
LIST_ITEM (L"\\bu @@Phonetic symbols: consonants")
LIST_ITEM (L"\\bu @@Phonetic symbols: vowels")
LIST_ITEM (L"\\bu @@Phonetic symbols: diacritics")
MAN_END

MAN_BEGIN (L"Phonetic symbols: consonants", L"ppgb", 20070825)
NORMAL (L"To draw phonetic symbols for consonants in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can then use the backslash sequences in the following table.")
PICTURE (6.0, 7.0, draw_IPA_consonant_chart)
NORMAL (L"Other consonant symbols:")
LIST_ITEM (L"\\l~ \\bsl~ (%%l with tilde%): velarized %l")
LIST_ITEM (L"\\hj \\bshj (%%heng with hooktop%): the Swedish rounded post-alveolar & velar fricative")
ENTRY (L"How to remember the codes")
NORMAL (L"For most of the codes, the first letter tells you the most similar letter of the English alphabet. "
	"The second letter can be %t (%turned), %c (%capital or %curled), %s (%script), - (%barred), %l (%%with leg%), "
	"%i (%inverted), or %j (%%left tail%). Some phonetic symbols are similar to Greek letters but have special "
	"phonetic (%f) versions with serifs (\\ff, \\bf, \\gf) or are otherwise slightly different (\\tf, \\cf). "
	"The codes for \\ng (%engma), \\dh (%eth), \\sh (%esh), and \\zh (%yogh) are traditional alternative spellings. "
	"The retroflexes have a period in the second place, because an alternative traditional spelling is to write a dot under them. "
	"The code for \\fh is an abbreviation for %fishhook.")
MAN_END

MAN_BEGIN (L"Phonetic symbols: diacritics", L"ppgb", 20070914)
NORMAL (L"To draw phonetic diacritical symbols in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can then use the backslash sequences in the following list.")
NORMAL (L"In line:")
LIST_ITEM (L"\\:f \\bs:f the phonetic length sign")
LIST_ITEM (L"\\\'1 \\bs\'1 primary stress")
LIST_ITEM (L"\\\'2 \\bs\'2 secondary stress")
LIST_ITEM (L"\\|f \\bs|f the phonetic stroke")
LIST_ITEM (L"t\\cn t\\bscn (%%combining left angle above%, %corner): unreleased plosive")
NORMAL (L"Understrikes:")
LIST_ITEM (L"n\\|v n\\bs|v (%%combining vertical line below%): syllabic consonant")
LIST_ITEM (L"b\\0v b\\bs0v (%%combining ring below%): voiceless (e.g. lenis voiceless plosive, voiceless nasal or approximant)")
LIST_ITEM (L"o\\Tv o\\bsTv (%%combining down tack below%, %lowering): lowered vowel; or turns a fricative into an approximant")
LIST_ITEM (L"o\\T^ o\\bsT\\^  (%%combining up tack below%, %raising): raised vowel; or turns an approximant into a fricative")
LIST_ITEM (L"e\\-v e\\bs-v (%%combining macron below%): backed")
LIST_ITEM (L"o\\+v o\\bs+v (%%combining plus sign below%): fronted")
LIST_ITEM (L"o\\:v o\\bs:v (%%combining diaeresis below%): breathy voice")
LIST_ITEM (L"o\\~v o\\bs~v (%%combining tilde below%): creaky voice")
LIST_ITEM (L"t\\Nv t\\bsNv (%%combining bridge below%): dental (as opposed to alveolar)")
LIST_ITEM (L"u\\nv u\\bsnv (%%combining inverted breve below%): nonsyllabic")
LIST_ITEM (L"e\\3v e\\bs3v (%%combining right half ring below%): slightly rounded")
LIST_ITEM (L"u\\cv u\\bscv (%%combining left half ring below%): slightly unrounded")
NORMAL (L"Overstrikes:")
LIST_ITEM (L"\\gf\\0^ \\bsgf\\bs0\\^  (%%combining ring above%): voiceless")
LIST_ITEM (L"\\ep\\\'^ \\bsep\\bs\'\\^  (%%combining acute accent%): high tone")
LIST_ITEM (L"\\ep\\`^ \\bsep\\bs`\\^  (%%combining grave accent%): low tone")
LIST_ITEM (L"\\ep\\-^ \\bsep\\bs-\\^  (%%combining macron%): mid tone (or so)")
LIST_ITEM (L"\\ep\\~^ \\bsep\\bs~\\^  (%%combining tilde%): nasalized")
LIST_ITEM (L"\\ep\\v^ \\bsep\\bsv\\^  (%%combining caron%, %hac\\v^ek, %wedge): rising tone")
LIST_ITEM (L"\\ep\\^^ \\bsep\\bs\\^ \\^  (%%combining circumflex accent%): falling tone")
LIST_ITEM (L"o\\:^ o\\bs:\\^  (%%combining diaeresis%): centralized")
LIST_ITEM (L"k\\lip t\\lis k\\bslip (%%character tie%, %ligature): simultaneous articulation, or single segment")
MAN_END

MAN_BEGIN (L"Phonetic symbols: vowels", L"ppgb", 20070825)
NORMAL (L"To draw phonetic symbols for vowels in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can then use the backslash sequences in the following table.")
PICTURE (6.0, 5.0, draw_IPA_vowel_chart)
NORMAL (L"Other vowel symbols are:")
LIST_ITEM (L"\\sr \\bssr (%%schwa with right hook%): rhotacized schwa")
ENTRY (L"How to remember the codes")
NORMAL (L"For most of the codes, the first letter tells you the most similar letter of the English alphabet. "
	"The second letter can be %t (%turned), %c (%capital), %s (%script), %r (%reversed), - (%barred or %retracted), or / (%slashed). "
	"One symbol (\\ef) is a phonetic version of a Greek letter. "
	"The codes for \\sw, \\rh, \\hs and \\kb are abbreviations for %schwa, %%ram's horn%, %horseshoe, and %%kidney bean%.")
MAN_END

MAN_BEGIN (L"Axes...", L"ppgb", 19970330)
INTRO (L"One of the commands in the #Margins and #World menus of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To view and change the current world coordinates of the horizontal and vertical axes.")
ENTRY (L"Usage")
NORMAL (L"The axes are normally changed by every drawing operation in the dynamic menu, "
	"i.e., by object-specific drawing commands with titles like ##Draw...# and ##Paint...# "
	"(the drawing commands in the Picture window, like ##Paint rectangle...#, do not change the axes).")
NORMAL (L"You would use the ##Axes...# command if your data are not in an object.")
ENTRY (L"Example")
NORMAL (L"The following script would draw a person's vowel triangle:")
CODE (L"\\#  Put F1 (between 300 and 800 Hz) along the horizontal axis,")
CODE (L"\\#  and F2 (between 600 and 3600 Hz) along the vertical axis.")
CODE (L"##Axes...# 300 800 600 3600")
CODE (L"\\#  Draw a rectangle inside the current viewport (selected area),")
CODE (L"\\#  with text in the margins, and tick marks in steps of 100 Hz along the F1 axis,")
CODE (L"\\#  and in steps of 200 Hz along the F2 axis.")
CODE (L"Draw inner box")
CODE (L"Text top... no Dani\\bse\\\" l's Dutch vowel triangle")
CODE (L"Text bottom... yes \\% F_1 (Hz)")
CODE (L"Text left... yes \\% F_2 (Hz)")
CODE (L"Marks bottom every... 1 100 yes yes yes")
CODE (L"Marks left every... 1 200 yes yes yes")
CODE (L"\\#  Draw large phonetic symbols at the vowel points.")
CODE (L"Text special... 340 Centre 688 Half Times 24 0 u")
CODE (L"Text special... 481 Centre 1195 Half Times 24 0 \\bso/")
CODE (L"\\#  Etcetera")
NORMAL (L"This example would draw the texts \"Dani\\e\"l's Dutch vowel triangle\", "
	"\"%F__1_ (Hz)\", and \"%F__2_ (Hz)\" in the margins, "
	"and the texts \"u\" and \"\\o/\" at the appropriate positions inside the drawing area.")
MAN_END

MAN_BEGIN (L"Copy to clipboard", L"ppgb", 20041130)   /* Not Unix. */
INTRO (L"A command in the File menu of the @@Picture window@.")
NORMAL (L"It copies the selected part of the picture to the clipboard. "
	"You can then `Paste' it into any program that knows pictures.")
ENTRY (L"Behaviour")
NORMAL (L"Though all the picture data will be written to the clipboard, "
	"only the part that corresponds to the selected part of the Picture window (the %viewport) will be visible.")
#if defined (macintosh)
ENTRY (L"Usage")
NORMAL (L"If you have a PostScript printer, you will want to use @@Write to EPS file...@ instead. "
	"If the picture is too large, e.g. a spectrogram that does not seem to fit into the clipboard, "
	"you can try @@Write to Mac PICT file...@ instead.")
#endif
#if defined (_WIN32)
ENTRY (L"Usage")
NORMAL (L"If you have a PostScript printer, you will want to use @@Write to EPS file...@ instead. "
	"If the picture is too large, e.g. a spectrogram that does not seem to fit into the clipboard, "
	"you can try @@Write to Windows metafile...@ instead.")
#endif
MAN_END

MAN_BEGIN (L"Draw inner box", L"ppgb", 19970330)
INTRO (L"One of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To draw a rectangle inside the drawing area, "
	"leaving @margins on all four sides for drawing text and marks.")
ENTRY (L"Behaviour")
NORMAL (L"The widths of the margins depend on the current font size.")
MAN_END

MAN_BEGIN (L"Encapsulated PostScript", L"ppgb", 20001010)
INTRO (L"a kind of PostScript file that can easily be imported into word processors and drawing programs. "
	"In Praat, you can create an Encapsulated PostScript (EPS) file by choosing @@Write to EPS file...@.")
MAN_END

MAN_BEGIN (L"Erase all", L"ppgb", 19980825)
INTRO (L"A command in the #Edit menu of the @@Picture window@.")
NORMAL (L"It erases all your drawings.")
MAN_END

MAN_BEGIN (L"Font menu", L"ppgb", 20070815)
INTRO (L"One of the menus of the @@Picture window@.")
NORMAL (L"It allows you to choose the default font of the text "
	"to be used in subsequent drawing, and its size (character height).")
ENTRY (L"Sizes")
NORMAL (L"You can choose any of the sizes 10, 12, 14, 18, or 24 directly from this menu, "
	"or fill in any other size in the ##Font size...# form.")
TAG (L"Unix:")
DEFINITION (L"the font size will be rounded to the nearest size available on Xwindows, "
	"which is one from 10, 12, 14, 18, or 24 points; "
	"PostScript-printing a picture where you specified a font size of 100, however, "
	"will still give the correct 100-point character height.")
TAG (L"Macintosh and Windows:")
DEFINITION (L"all sizes are drawn correctly (what you see on the screen "
	"is what you get on your printer).")
NORMAL (L"The widths of the margins depend on the current font size, "
	"so if you want to change the font size, "
	"do so before making your drawing.")
ENTRY (L"Fonts")
NORMAL (L"With these commands, you set the font in which subsequent text will de drawn: "
	"Times, Helvetica, Palatino, or Courier.")
NORMAL (L"You can mix the Symbol and IPA alphabets with the normal Roman alphabets "
	"and use sequences of backslash + digraph for @@special symbols@ (see also @@phonetic symbols@).")
NORMAL (L"For instance, you can get an \\e\" by typing \\bse\\\" , or a \\ss by typing \\bsss; "
	"you can get an \\ep by typing \\bsep, or a \\ct, which is a turned c, by typing \\bsct.")
NORMAL (L"If you print to a PostScript printer, all fonts will be correct.")
ENTRY (L"Styles")
NORMAL (L"You can use all graphical @@text styles@ in the Picture window.")
MAN_END

MAN_BEGIN (L"Logarithmic marks left/right/top/bottom...", L"ppgb", 19970330)
INTRO (L"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To draw a specified number of marks per decade "
	"into the @margins around the drawing area, along a logarithmic axis.")
ENTRY (L"Arguments")
TAG (L"%%Marks per decade")
DEFINITION (L"the number of marks that will be drawn for every decade.")
TAG (L"%%Write numbers")
DEFINITION (L"if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (L"%%Draw ticks")
DEFINITION (L"if on, short line pieces will be drawn in the margin.")
TAG (L"%%Draw dotted lines")
DEFINITION (L"if on, dotted lines will be drawn through your drawing.")
ENTRY (L"Behaviour")
LIST_ITEM (L"If your vertical logarithmic axis runs from 10 to 100, "
	"and %%Marks per decade% is 1, marks will only be drawn at 10 and 100;")
LIST_ITEM (L"if %%Marks per decade% is 2, marks will be drawn at 10, 30, and 100;")
LIST_ITEM (L"if it is 3, marks will be drawn at 10, 20, 50, and 100;")
LIST_ITEM (L"if it is 4, marks will be drawn at 10, 20, 30, 50, and 100;")
LIST_ITEM (L"if it is 5, marks will be drawn at 10, 20, 30, 50, 70, and 100;")
LIST_ITEM (L"if it is 6, marks will be drawn at 10, 15, 20, 30, 50, 70, and 100;")
LIST_ITEM (L"if it is 7 (the maximum), marks will be drawn at 10, 15, 20, 30, 40, 50, 70, and 100.")
MAN_END

MAN_BEGIN (L"Margins", L"ppgb", 19970405)
INTRO (L"The space around most of your drawings in the @@Picture window@.")
ENTRY (L"World coordinates")
NORMAL (L"With the commands in the #Margins menu, "
	"you draw text, ticks, numbers, or a rectangle, "
	"in the margins around the latest drawing that you made, "
	"or you draw dotted lines through or text inside this last drawing.")
NORMAL (L"You specify the positions of these things in world coordinates, "
	"i.e., in coordinates that refer to the natural coordinate system of your last drawing.")
NORMAL (L"The numbers that you can mark around your drawing also refer to these coordinates. "
	"For instance, after drawing a spectrum with ##Spectrum: Draw...#, "
	"you can draw a dotted line at 2000 Hz or at 60 dB "
	"by choosing ##One mark bottom...# or ##One mark left...# "
	"and typing \"2000\" or \"60\", respectively.")
ENTRY (L"Usage")
NORMAL (L"The margin commands work with all the drawings that leave margins around themselves, "
	"such as ##Sound: Draw...#, ##Spectrogram: Paint...#, ##Polygon: Paint...#, and more. "
	"They do not work properly, however, with the commands that draw vocal tract shapes, "
	"like ##Art & Speaker: Draw...# and ##Artword & Speaker: Draw...#, "
	"because these can only be drawn correctly into a square viewport.")
ENTRY (L"Limited validity")
NORMAL (L"The margin commands work only on the latest drawing that you made "
	"(unless you @Undo that drawing).")
ENTRY (L"Margin size")
NORMAL (L"The size of the margins depends on the font size, "
	"so be sure that you have the font size of your choice before you make your drawing. "
	"You can set the font size with the @@Font menu@.")
MAN_END

MAN_BEGIN (L"Marks left/right/top/bottom...", L"ppgb", 19970330)
INTRO (L"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To draw any number of equally spaced marks into the @margins around the drawing area.")
ENTRY (L"Arguments")
TAG (L"%%Number of marks%")
DEFINITION (L"the number of equally spaced marks (2 or more) that will be drawn; "
	"there will always be marks at the beginning and end of the domain or range.")
TAG (L"%%Write numbers")
DEFINITION (L"if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (L"%%Draw ticks")
DEFINITION (L"if on, short line pieces will be drawn in the margin.")
TAG (L"%%Draw dotted lines")
DEFINITION (L"if on, dotted lines will be drawn through your drawing.")
ENTRY (L"Example")
NORMAL (L"If you draw a Sound with a domain between 0 and 1 seconds "
	"to an amplitude range between -1 and 1, "
	"choosing ##Marks left...# with a number of 3 and %%Draw dotted lines% on, "
	"will give you horizontal marks and horizontal dotted lines at -1, 0, and 1; "
	"choosing ##Marks bottom...# with a number of 6 and %%Draw dotted lines% off, "
	"will give you vertical marks at 0, 0.2, 0.4, 0.6, 0.8, and 1.")
MAN_END

MAN_BEGIN (L"Marks left/right/top/bottom every...", L"ppgb", 19970330)
INTRO (L"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To draw a number of equally spaced marks into the @margins around the drawing area.")
ENTRY (L"Arguments")
TAG (L"%%Units")
DEFINITION (L"the units, relative to the standard units, "
	"for writing the numbers; for example, if you want time in milliseconds "
	"instead of seconds (which is always the standard), "
	"%Units should be 0.001.")
TAG (L"%%Distance")
DEFINITION (L"the distance between the equally spaced marks that will be drawn, "
	"expressed in %Units; for example, if you want marks every 20 milliseconds, "
	"and %Units is 0.001, this argument should be 20.")
TAG (L"%%Write numbers")
DEFINITION (L"if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (L"%%Draw ticks")
DEFINITION (L"if on, short line pieces will be drawn in the margin.")
TAG (L"%%Draw dotted lines")
DEFINITION (L"if on, dotted lines will be drawn through your drawing.")
MAN_END

MAN_BEGIN (L"One logarithmic mark left/right/top/bottom...", L"ppgb", 19970330)
INTRO (L"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To draw one mark into one of the four @margins "
	"around the drawing area, along a logarithmic axis.")
ENTRY (L"Arguments")
TAG (L"%%Position")
DEFINITION (L"the %x (for top or bottom) or %y (for left or right) position of the mark, "
	"expressed in the logarithmic domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (L"%%Write number")
DEFINITION (L"if on, a real number equal to %Position will be written in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG (L"%%Draw tick")
DEFINITION (L"if on, a short line piece will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG (L"%%Draw dotted line")
DEFINITION (L"if on, a dotted line will be drawn through your drawing, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG (L"%%Draw text")
DEFINITION (L"if not empty, this text will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
ENTRY (L"Example")
NORMAL (L"After you draw a Pitch logarithmically in a range between 100 and 400 Hz, "
	"choosing ##One logarithmic mark left...# with a position of 200 and %%Draw dotted line% on, "
	"will give you a horizontal mark \"200\" and a horizontal dotted line at a %y position of 200, "
	"which is exactly halfway between 100 and 400 Hz.")
MAN_END

MAN_BEGIN (L"One mark left/right/top/bottom...", L"ppgb", 19970330)
INTRO (L"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To draw one mark into one of the four @margins around the drawing area.")
ENTRY (L"Arguments")
TAG (L"%%Position")
DEFINITION (L"the %x (for top or bottom) or %y (for left or right) position of the mark, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (L"%%Write number")
DEFINITION (L"if on, a real number equal to `Position' will be written in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG (L"%%Draw tick")
DEFINITION (L"if on, a short line piece will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG (L"%%Draw dotted line")
DEFINITION (L"if on, a dotted line will be drawn through your drawing, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
TAG (L"%%Draw text")
DEFINITION (L"if not empty, this text will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to %Position.")
ENTRY (L"Example")
NORMAL (L"If you draw a Sound to an amplitude range between -1 and 1, "
	"choosing ##One mark left...# with a position of 0.0 and %%Draw dotted line% on, "
	"will give you a horizontal mark \"0\" and a horizontal dotted line at a y position of 0.")
MAN_END

MAN_BEGIN (L"Pen menu", L"ppgb", 20061122)
INTRO (L"One of the menus of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To choose the line type and colour "
	"to be used in subsequent drawing of lines and painted areas.")
ENTRY (L"Behaviour")
NORMAL (L"The line type used by @@Draw inner box@ (solid), "
	"and the line type of the dotted lines in the ##Mark...# commands "
	"will not be affected.")
NORMAL (L"The commands in the @Margins menu will always draw in black.")
MAN_END

MAN_BEGIN (L"Picture window", L"ppgb", 20041130)
INTRO (L"One of the two main windows in P\\s{RAAT}.")
TAG (L"File menu")
LIST_ITEM (L"\\bu @@Read from Praat picture file...@, @@Write to Praat picture file...")
#if defined (macintosh)
LIST_ITEM (L"\\bu @@Copy to clipboard@, @@Write to Mac PICT file...@")
#elif defined (_WIN32)
LIST_ITEM (L"\\bu @@Copy to clipboard@, @@Write to Windows metafile...@")
#else
LIST_ITEM (L"\\bu @@Copy to clipboard@")
#endif
LIST_ITEM (L"\\bu @@PostScript settings...")
LIST_ITEM (L"\\bu @@Write to EPS file...")
LIST_ITEM (L"\\bu @@Print...")
TAG (L"Edit menu")
LIST_ITEM (L"\\bu @@Undo@, @@Erase all")
TAG (L"@Margins menu")
LIST_ITEM (L"\\bu @@Draw inner box")
LIST_ITEM (L"\\bu @@Text left/right/top/bottom...")
LIST_ITEM (L"\\bu @@Marks left/right/top/bottom every...")
LIST_ITEM (L"\\bu @@One mark left/right/top/bottom...")
LIST_ITEM (L"\\bu @@Marks left/right/top/bottom...")
LIST_ITEM (L"\\bu @@Logarithmic marks left/right/top/bottom...")
LIST_ITEM (L"\\bu @@One logarithmic mark left/right/top/bottom...")
LIST_ITEM (L"\\bu @@Axes...")
TAG (L"World menu")
LIST_ITEM (L"\\bu @@Text...")
LIST_ITEM (L"\\bu @@Axes...")
TAG (L"Select menu")
LIST_ITEM (L"\\bu @@Select inner viewport...@, @@Select outer viewport...@, @@Viewport text...")
TAG (L"@@Pen menu")
TAG (L"@@Font menu")
MAN_END

MAN_BEGIN (L"PostScript settings...", L"ppgb", 20040925)
INTRO (L"One of the commands in the File menus of many windows. "
	"The PostScript settings influence @Printing and writing to @@Encapsulated PostScript@ files.")
ENTRY (L"Arguments")
TAG (L"%%Allow direct PostScript printing% (Windows and Macintosh only)")
DEFINITION (L"this determines whether Praat prints explicit PostScript commands to your printer "
	"if it is a PostScript printer. This is what you will usually want. However, if you find "
	"that some of the options that you choose in the printing dialog seem not to be supported "
	"(e.g. scaling, printing two-up...), you may switch this off; Praat will then send native "
	"Windows or Macintosh drawing commands, which the printer driver will try to translate "
	"to PostScript. If your printer does not support PostScript, this switch is ignored. "
	"On Unix, this switch is superfluous, since all printing is done directly in PostScript.")
TAG (L"%%Grey resolution")
DEFINITION (L"you can choose from two image qualities:")
LIST_ITEM1 (L"\\bu the %finest quality for grey plots (106 spots per inch), "
	"which gives the best results directly from the printer;")
LIST_ITEM1 (L"\\bu a %photocopyable quality, which has fewer spots per inch (85) and "
	"gives the best results after photocopying.")
DEFINITION (L"Your choice of the grey resolution influences direct PostScript printing "
	"and writing to @@Encapsulated PostScript@ files.")
TAG (L"%%Paper size% (Unix only)")
DEFINITION (L"you can choose from A4 (210 \\xx 297 mm), A3 (297 \\xx 420 mm) or US Letter (8.5 \\xx 11\\\"p). "
	"This choice applies to Unix only; on Windows, you choose the paper size in the ##Print...# dialog; "
	"on Macintosh, you choose the paper size in the ##Page setup...# dialog.")
TAG (L"%%Orientation% (Unix only)")
DEFINITION (L"you can choose between %portrait (e.g., 297 mm high and 210 mm wide) "
	"and %landscape (e.g., 210 mm high and 297 mm wide). "
	"This choice applies to Unix only; on Windows, you choose the orientation in the ##Print...# dialog; "
	"on Macintosh, you choose the orientation in the ##Page setup...# dialog.")
TAG (L"%%Magnification% (Unix only)")
DEFINITION (L"the relative size with which your picture will be printed; normally 1.0. "
	"This choice applies to Unix only; on Windows, you choose the scaling in the ##Print...# dialog; "
	"on Macintosh, you choose the scaling in the ##Page setup...# dialog.")
TAG (L"%%Print command% (Unix only)")
DEFINITION (L"When printing on Unix, a temporary PostScript^\\re file is created in the \"/tmp\" directory; "
	"it will have a name like \"picXXXXXX\", and is automatically removed after printing. "
	"This file is sent to the printer with the print command, which will often look like $$lp -c \\% s$, "
	"where $$\\% s$ stands for the file name.")
MAN_END

MAN_BEGIN (L"Print...", L"ppgb", 20001010)
INTRO (L"One of the commands in the File menu of the @@Picture window@.")
NORMAL (L"With this command, you send your entire picture immediately to the printer. "
	"See the @Printing tutorial for details.")
MAN_END

MAN_BEGIN (L"Printing", L"ppgb", 20071016)
NORMAL (L"The best results will be obtained on PostScript printers, since these have built-in "
	"facilities for images (e.g. spectrograms) and rotated text. "
	"However, the printed page will look reasonable on colour inkjet printers as well.")
#if defined (UNIX)
ENTRY (L"1. Printing on Unix")
NORMAL (L"Most Unix networks (i.e. SGI, Solaris, HPUX) are traditionally connected to a PostScript printer. "
	"When you tell Praat to print a picture or manual page, Praat will write the picture to a temporary PostScript file "
	"and send this file to a printer with the %%print command% (typically $lp), which you can change "
	"with @@PostScript settings...@.")
NORMAL (L"On Linux, you do not need a PostScript printer to print PostScript directly, "
	"because the #lpr program sends PostScript files through the GhostScript program, "
	"which is a part of all modern Linux distributions. The print command is typically "
	"$$lpr \\% s$. By changing the print command (with @@PostScript settings...@), "
	"you can change it to something fancier. For instance, if you want to save the woods "
	"and print two pages on one sheet of paper, you change it to $$cat \\% s | mpage -2 -o -f -m0 | lpr$.")
#elif defined (macintosh)
ENTRY (L"1. Printing on Macintosh")
NORMAL (L"If you are on a Mac and a PostScript printer is available, "
	"Praat will usually write direct PostScript commands to that printer "
	"(see @@PostScript settings...@ if you want to switch this off). "
	"P\\s{RAAT} also supports non-PostScript printers, such as most colour inkjet printers.")
#elif defined (_WIN32)
ENTRY (L"1. Printing on Windows")
NORMAL (L"If you are on a Windows computer and a PostScript printer is available, "
	"Praat will usually write direct PostScript commands to that printer "
	"(see @@PostScript settings...@ if you want to switch this off). "
	"P\\s{RAAT} also supports non-PostScript printers, such as most colour inkjet printers.")
#endif
ENTRY (L"2. Indirect printing with GhostView")
NORMAL (L"If you don't have a PostScript printer, and you still want PostScript quality, "
	"you can save the picture to an EPS file (@@Write to EPS file...@). "
	"You can then view this file with the freely available "
	"GhostView^\\tm program, which you can download from ##http://pages.cs.wisc.edu/~ghost/#.")
ENTRY (L"3. Indirect printing with your word processor")
NORMAL (L"If you save your picture to an EPS file, you will be able to include it as a picture in your favourite "
	"word processor (Microsoft^\\re Word^\\tm, LaTeX...). See @@Write to EPS file...@.")
NORMAL (L"If you don't have a PostScript printer, you could again use GhostView^\\tm to print your document "
	"to any printer, after you printed your document to a PostScript file. You can do the same if you are "
	"the lucky owner of Adobe^\\re Acrobat^\\tm Distiller^\\tm, which is more reliable than GhostView "
	"but is also expensive.")
ENTRY (L"4. Creating a PDF file")
NORMAL (L"If you have Distiller or GhostView, you can print the entire Word^\\tm or LaTeX document to a PostScript file, "
	"and convert this to a PDF file, which anyone in the world can view and print with the free Adobe^\\re Acrobat^\\tm Reader program.")
#ifdef _WIN32
NORMAL (L"Note: when creating a PDF file on Windows if you have Acrobat, ##do not use PDFWriter#, but choose Distiller as your printer. "
	"also, ##do not use \"Print to PDF\"# from your Microsoft Word File menu; otherwise, "
	"your EPS files will not show in the PDF file.")
#endif
#ifdef macintosh
NORMAL (L"Note: when creating a PDF file on MacOS X, ##do not use \"Save as PDF...\"# in your printing dialog, "
	"but choose Adobe PDF as your printer or save the document to a PostScript file and convert it with "
	"Distiller or GhostView; otherwise, your EPS files will not show in the PDF file.")
#endif
#ifdef _WIN32
ENTRY (L"Indirect printing without PostScript")
NORMAL (L"Pictures included in your word processor via @@Copy to clipboard@ or @@Write to Windows metafile...@ "
	"will print fine, though not as nicely as EPS files.")
#endif
#ifdef macintosh
ENTRY (L"Indirect printing without PostScript")
NORMAL (L"Pictures included in your word processor via @@Copy to clipboard@ or @@Write to Mac PICT file...@ "
	"will print fine, though not as nicely as EPS files.")
#endif
MAN_END

MAN_BEGIN (L"Read from Praat picture file...", L"ppgb", 19960908)
INTRO (L"One of the commands in the File menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To read a picture that you saved earlier "
	"with @@Write to Praat picture file...@.")
ENTRY (L"Behaviour")
NORMAL (L"The picture will be drawn across whatever is currently visible in the Picture window.")
ENTRY (L"Usage")
NORMAL (L"With the help of this command, you can transfer a picture from a Unix machine to a Macintosh. "
	"Praat for Macintosh can write the picture to an @@Encapsulated PostScript@ file "
	"with a screen preview.")
MAN_END

MAN_BEGIN (L"Text...", L"ppgb", 19970330)
INTRO (L"One of the commands in the #World menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To write text inside the drawing area.")
ENTRY (L"Scope")
NORMAL (L"This works with all the drawings that leave @margins around themselves.")
ENTRY (L"Arguments")
TAG (L"%x")
DEFINITION (L"horizontal position, expressed in the horizontal domain of your latest drawing.")
TAG (L"%y")
DEFINITION (L"vertical position, expressed in the vertical range or domain of your latest drawing.")
TAG (L"%%Horizontal alignment")
DEFINITION (L"determines the horizontal alignment of the text relative to %x.")
TAG (L"%%Vertical alignment")
DEFINITION (L"determines the vertical alignment of the text relative to %y.")
TAG (L"%%Text")
DEFINITION (L"will be drawn in the current font and font size "
	"that you set with the @@Font menu@.")
ENTRY (L"Usage")
NORMAL (L"With the ##Text...# command, you can use all @@special symbols@ and @@text styles@.")
MAN_END

MAN_BEGIN (L"Text left/right/top/bottom...", L"ppgb", 19970330)
INTRO (L"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To write text into the @margins around the drawing area.")
ENTRY (L"Behaviour")
NORMAL (L"The text will be centred along the side. "
	"Text at the left or right will be turned by 90 degrees "
	"and written up and down, respectively.")
MAN_END

MAN_BEGIN (L"Undo", L"ppgb", 20041108)
INTRO (L"One of the commands in the #Edit menu of the @@Picture window@.")
NORMAL (L"It erases your most recently created drawing, which could have come from a command in the Objects window "
	"or from one of the drawing commands in the World and @Margins menus.")
ENTRY (L"Behaviour")
NORMAL (L"This command will erase some drawings, but it will not change your settings in the #Pen and #Font menus "
	"(line type, line width, font, font size, and colour).")
NORMAL (L"The world window will be what it was after the latest-but-one drawing, "
	"so that you can use the @Margins menu as if the latest drawing had never happened.")
MAN_END

MAN_BEGIN (L"Select inner viewport...", L"ppgb", 20041108)
INTRO (L"One of the commands in the #Select menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To determine where your next drawing will occur.")
ENTRY (L"The viewport")
#ifdef macintosh
NORMAL (L"The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"You can set the colour of its margins with ##System Preferences \\-> Appearance \\-> Highlight Colour#. ")
#else
NORMAL (L"The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"Its margins are pink.")
#endif
NORMAL (L"The %inner viewport does not include the margins, the %outer viewport does (see @@Select outer viewport...@).")
NORMAL (L"Normally, you select the viewport by dragging your mouse across the Picture window. "
	"However, you would use this explicit command:")
LIST_ITEM (L"\\bu from a script;")
LIST_ITEM (L"\\bu if you want a viewport that cannot be expressed in halves of an inch.")
MAN_END

MAN_BEGIN (L"Select outer viewport...", L"ppgb", 20041108)
INTRO (L"One of the commands in the #Select menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To determine where your next drawing will occur.")
ENTRY (L"The viewport")
#ifdef macintosh
NORMAL (L"The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"You can set the colour of its margins with ##System Preferences \\-> Appearance \\-> Highlight Colour#. ")
#else
NORMAL (L"The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"Its margins are pink.")
#endif
NORMAL (L"The %outer viewport includes the margins, the %inner viewport does not (see @@Select inner viewport...@).")
NORMAL (L"Normally, you select the viewport by dragging your mouse across the Picture window. "
	"However, you would use this explicit command:")
LIST_ITEM (L"\\bu from a script;")
LIST_ITEM (L"\\bu if you want a viewport that cannot be expressed in halves of an inch.")
MAN_END

MAN_BEGIN (L"Viewport text...", L"ppgb", 19970330)
INTRO (L"One of the commands in the #Select menu of the @@Picture window@.")
ENTRY (L"Purpose")
NORMAL (L"To write text inside the viewport, at nine different places, "
	"with a rotation between 0 to 360 degrees.")
ENTRY (L"Arguments:")
TAG (L"%%Horizontal alignment")
DEFINITION (L"determines the horizontal alignment of the text:")
LIST_ITEM (L"    \\bu %Left means pushed against the left edge of the viewport;")
LIST_ITEM (L"    \\bu %Right means pushed against the right edge of the viewport;")
LIST_ITEM (L"    \\bu %Centre means horizontally centred in the viewport.")
TAG (L"%%Vertical alignment")
DEFINITION (L"determines the vertical alignment of the text:")
LIST_ITEM (L"    \\bu %Top means pushed against the top of the viewport;")
LIST_ITEM (L"    \\bu %Bottom means pushed against the bottom of the viewport;")
LIST_ITEM (L"    \\bu %Half means vertically centred in the viewport.")
TAG (L"%%Text")
DEFINITION (L"will be drawn in the current font and font size "
	"that you set with the @@Font menu@.")
ENTRY (L"Behaviour")
NORMAL (L"For rotated text, the alignment arguments will not only determine "
	"the position inside the viewport, "
	"but also the alignment in the rotated coordinate system. "
	"This gives surprises now and then; "
	"so, if you want several rotated texts that align with each other, "
	"you should do this by varying the viewport, not the alignment.")
ENTRY (L"Usage")
NORMAL (L"You can use all @@special symbols@ and @@text styles@.")
MAN_END

MAN_BEGIN (L"Write to EPS file...", L"ppgb", 20041130)
INTRO (L"A command in the File menu of the @@Picture window@.")
NORMAL (L"It saves the picture to an @@Encapsulated PostScript@ (EPS) file, "
	"which can be imported by many other programs, such as Microsoft^\\re Word^\\tm.")
ENTRY (L"PostScript = highest possible quality!")
NORMAL (L"With EPS files you can use high-quality graphics in your word-processor documents. "
	"The quality is higher than if you use @@Copy to clipboard@.")
#ifdef _WIN32
ENTRY (L"The big limitation")
NORMAL (L"EPS pictures imported in Word for Windows will show correctly only on PostScript printers, or with GhostView, or in PDF files "
	"created by Adobe^\\re Acrobat^\\tm Distiller^\\tm. "
	"To print EPS pictures on non-PostScript printers, use a Linux or Macintosh computer.")
#endif
#ifdef macintosh
ENTRY (L"The big limitation")
NORMAL (L"If you have MacOS X 10.3 or higher, you are lucky. "
	"These system versions can write EPS (and other PostScript files) to non-PostScript printers, such as most inkjet printers. "
	"With older system versions, however, EPS pictures imported in Word for Macintosh will show correctly only on PostScript printers, "
	"or with GhostView, or in PDF files created by Adobe^\\re Acrobat^\\tm Distiller^\\tm.")
#endif
#ifdef macintosh
ENTRY (L"Usage")
NORMAL (L"To import an EPS file in Word 5.1, choose ##File...# or ##Picture...# from the #Insert menu. "
	"To do it in Word X, choose #Insert \\-> #Picture \\-> ##From file...#. "
	"Word will create a picture with the same size as the originally selected part of the Picture window (the %viewport).")
NORMAL (L"Word will show you a screen preview in a mediocre quality, but you will see the high-quality PostScript version when you print.")
#endif
#ifdef _WIN32
ENTRY (L"Usage")
NORMAL (L"If Word cannot read your EPS files, you may have to install EPS support from the Office^\\tm CD, "
	"because the standard installation of Office may not support EPS files. "
	"When you import an EPS file in an older version of Word, you may only see the file name and the date and time of creation, "
	"and a message that the picture will print correctly to a PostScript printer (it will, if the PostScript driver has been selected).")
#endif
ENTRY (L"Behaviour")
NORMAL (L"Though all the contents of the Picture window are written to the EPS file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in Word (or another program).")
ENTRY (L"Settings")
NORMAL (L"The EPS picture is saved with the grey resolution and fonts that you specified with @@PostScript settings...@.")
#ifdef macintosh
ENTRY (L"Technical information")
NORMAL (L"On the Macintosh, the EPS file will consist of PostScript text (for the printer, in the `data fork') "
	"plus a bitmapped screen preview (in the `resource fork'). The screen preview is lost if you transfer the EPS file "
	"to a Windows or Unix computer, although an EPS picture imported in a Word for Macintosh document retains its preview "
	"when the Word document is transferred to a Windows computer.")
#endif
MAN_END

MAN_BEGIN (L"Write to Praat picture file...", L"ppgb", 20041130)
INTRO (L"A command in the File menu of the @@Picture window@.")
NORMAL (L"It saves a picture in a format that can be imported into P\\s{RAAT} later with @@Read from Praat picture file...@.")
ENTRY (L"Usage")
NORMAL (L"With the help of this command, you can transfer the contents of the picture window between computers or even between platforms, "
	"for instance from a Macintosh to a Windows computer.")
MAN_END

#ifdef macintosh
MAN_BEGIN (L"Write to Mac PICT file...", L"ppgb", 20041130)
INTRO (L"A command in the File menu of the @@Picture window@.")
NORMAL (L"It saves the selected part of the picture in an \"extended PICT2\" format, "
	"which can be imported by many programs on the Mac, like MacDraw^\\tm "
	"(Microsoft^\\re Word^\\tm 5.1 unfortunately rounds the high resolution down to screen pixels).")
ENTRY (L"Behaviour")
NORMAL (L"Though all the picture data will be written to the PICT file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in the other program.")
ENTRY (L"Usage")
NORMAL (L"You will not use this command very often, "
	"because it is usually easier to copy the selection to the clipboard with the @@Copy to clipboard@ command, "
	"and `Paste' it into the other program. You may use a PICT file instead of the clipboard if the clipboard is too large "
	"for the other program to read, or if you want to transfer the picture to another computer.")
NORMAL (L"If you have a PostScript printer (or MacOS X 10.3 with %any printer), you would use @@Write to EPS file...@ instead "
	"for best printing results.")
MAN_END
#endif

#ifdef _WIN32
MAN_BEGIN (L"Write to Windows metafile...", L"ppgb", 20041130)
INTRO (L"A command in the File menu of the @@Picture window@.")
NORMAL (L"It saves the selected part of the picture in an \"enhanced metafile\" (.EMF) format, "
	"which can be imported by many Windows programs, like Adobe^\\re Illustrator^\\tm or Microsoft^\\re Word^\\tm.")
ENTRY (L"Behaviour")
NORMAL (L"Though all the picture data will be written to the file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in the other program.")
ENTRY (L"Usage")
NORMAL (L"You will not use this command very often, "
	"because it is usually easier to copy the selection to the clipboard with the @@Copy to clipboard@ command, "
	"and `Paste' it into the other program. You may use a metafile instead of the clipboard if the clipboard is too large "
	"for the other program to read, or if you want to transfer the picture to another computer.")
NORMAL (L"If you have a PostScript printer, you would use @@Write to EPS file...@ instead "
	"for best printing results.")
MAN_END
#endif

}

/* End of file manual_Picture.c */
