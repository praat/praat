/* manual_Picture.cpp
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

#include "ManPagesM.h"
#include "../kar/UnicodeData.h"

static void drawOneIpaSymbol (Graphics g, double x, double y, conststring32 symbol) {
	char32 buffer [30], *p = & buffer [0];
	const double fontSize = Graphics_inqFontSize (g);
	Graphics_rectangle (g, x - 0.5, x + 0.5, y - 0.5, y + 0.5);
	if (! symbol) return;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setFont (g, kGraphics_font::TIMES);
	Graphics_setFontSize (g, fontSize * 3.0 / 2.0);
	Graphics_text (g, x, y + 0.25, symbol);
	while (*symbol) {
		if (*symbol == '\\') { *p++ = '\\'; *p++ = 'b'; *p++ = 's'; }   // visualize backslash (\bs)
		else { *p++ = '\\'; *p++ = *symbol; *p++ = ' '; }   // visualize special symbols (% ^ _ #)
		symbol ++;
	}
	*p = '\0';   // trailing null byte
	Graphics_setFont (g, kGraphics_font::HELVETICA);
	Graphics_setFontSize (g, fontSize * 5.0 / 6.0);
	Graphics_text (g, x, y - 0.25, buffer);
	Graphics_setFontSize (g, fontSize);
}

static void draw_IPA_consonant_chart (Graphics graphics) {
	Graphics_setWindow (graphics, -0.8, 20.0, 6.0, 19.5);
	Graphics_setFontStyle (graphics, Graphics_ITALIC);
	Graphics_setTextRotation (graphics, 60.0);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (graphics, 4, 17.6, U"bilabial");
	Graphics_text (graphics, 5, 17.6, U"labiodental");
	Graphics_text (graphics, 6, 17.6, U"dental");
	Graphics_text (graphics, 7, 17.6, U"alveolar");
	Graphics_text (graphics, 8, 17.6, U"alv. lateral");
	Graphics_text (graphics, 9, 17.6, U"postalveolar");
	Graphics_text (graphics, 10, 17.6, U"retroflex");
	Graphics_text (graphics, 11, 17.6, U"alveolo-palatal");
	Graphics_text (graphics, 12, 17.6, U"palatal");
	Graphics_text (graphics, 13, 17.6, U"labial-palatal");
	Graphics_text (graphics, 14, 17.6, U"labial-velar");
	Graphics_text (graphics, 15, 17.6, U"velar");
	Graphics_text (graphics, 16, 17.6, U"uvular");
	Graphics_text (graphics, 17, 17.6, U"pharyngeal");
	Graphics_text (graphics, 18, 17.6, U"epiglottal");
	Graphics_text (graphics, 19, 17.6, U"glottal");
	Graphics_setTextRotation (graphics, 0.0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 3.3, 17, U"voiceless plosive");
	Graphics_text (graphics, 3.3, 16, U"voiced plosive");
	Graphics_text (graphics, 3.3, 15, U"nasal");
	Graphics_text (graphics, 3.3, 14, U"voiceless fricative");
	Graphics_text (graphics, 3.3, 13, U"voiced fricative");
	Graphics_text (graphics, 3.3, 12, U"approximant");
	Graphics_text (graphics, 3.3, 11, U"trill");
	Graphics_text (graphics, 3.3, 10, U"tap or flap");
	Graphics_text (graphics, 3.3, 9, U"lateral approx.");
	Graphics_text (graphics, 3.3, 8, U"implosive");
	Graphics_text (graphics, 3.3, 7, U"click");
	Graphics_setFontStyle (graphics, Graphics_NORMAL);
	{
		static const struct { double x, y; conststring32 string; } symbols [] = {
{ 4, 17, U"p" }, { 4, 16, U"b" }, { 4, 15, U"m" }, { 4, 14, U"\\ff" }, { 4, 13, U"\\bf" }, { 4, 11, U"\\bc" }, { 4, 8, U"\\b^" }, { 4, 7, U"\\O." },
{ 5, 15, U"\\mj" }, { 5, 14, U"f" }, { 5, 13, U"v" }, { 5, 12, U"\\vs" },
{ 6, 14, U"\\tf" }, { 6, 13, U"\\dh" }, { 6, 7, U"\\|1" },
{ 7, 17, U"t" }, { 7, 16, U"d" }, { 7, 15, U"n" }, { 7, 14, U"s" }, { 7, 13, U"z" }, { 7, 12, U"\\rt" }, { 7, 11, U"r" }, { 7, 10, U"\\fh" }, { 7, 9, U"l" }, { 7, 8, U"\\d^" },
{ 8, 17, U"t^l" }, { 8, 16, U"d^l" }, { 8, 14, U"\\l-" }, { 8, 13, U"\\lz" }, { 8, 12, U"l" }, { 8, 10, U"\\rl" }, { 8, 9, U"l" }, { 8, 7, U"\\|2" },
{ 9, 14, U"\\sh" }, { 9, 13, U"\\zh" }, { 9, 7, U"\\|-" },
{ 10, 17, U"\\t." }, { 10, 16, U"\\d." }, { 10, 15, U"\\n." }, { 10, 14, U"\\s." }, { 10, 13, U"\\z." }, { 10, 12, U"\\r." }, { 10, 10, U"\\f." }, { 10, 9, U"\\l." }, { 10, 7, U"!" },
{ 11, 14, U"\\cc" }, { 11, 13, U"\\zc" },
{ 12, 17, U"c" }, { 12, 16, U"\\j-" }, { 12, 15, U"\\nj" }, { 12, 14, U"\\c," }, { 12, 13, U"\\jc" }, { 12, 12, U"j" }, { 12, 9, U"\\yt" }, { 12, 8, U"\\j^" },
{ 13, 12, U"\\ht" },
{ 14, 14, U"\\wt" }, { 14, 12, U"w" },
{ 15, 17, U"k" }, { 15, 16, U"\\gs" }, { 15, 15, U"\\ng" }, { 15, 14, U"x" }, { 15, 13, U"\\gf" }, { 15, 12, U"\\ml" }, { 15, 9, U"\\lc" }, { 15, 8, U"\\g^" },
{ 16, 17, U"q" }, { 16, 16, U"\\gc" }, { 16, 15, U"\\nc" }, { 16, 14, U"\\cf" }, { 16, 13, U"\\ri" }, { 16, 11, U"\\rc" }, { 16, 8, U"\\G^" },
{ 17, 14, U"\\h-" }, { 17, 13, U"\\9e" },
{ 18, 17, U"\\?-" }, { 18, 14, U"\\hc" }, { 18, 13, U"\\9-" },
{ 19, 17, U"\\?g" }, { 19, 14, U"h" }, { 19, 13, U"\\h^" },
		{ 0, 0, nullptr } };
		for (int i = 0; symbols [i]. string != nullptr; i ++)
			drawOneIpaSymbol (graphics, symbols [i]. x, symbols [i]. y, symbols [i]. string);
	}
}

static void draw_IPA_vowel_chart (Graphics graphics) {
	Graphics_setWindow (graphics, -0.7, 19.0, -0.5, 7.5);
	Graphics_setFontStyle (graphics, Graphics_ITALIC);
	Graphics_setTextRotation (graphics, 60.0);
	Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (graphics, 4.5, 6.6, U"front");
	Graphics_text (graphics, 7, 6.6, U"central");
	Graphics_text (graphics, 9.5, 6.6, U"back");
	Graphics_setTextRotation (graphics, 0);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (graphics, 3.3, 6, U"close");
	Graphics_text (graphics, 3.3, 5, U"close centralized");
	Graphics_text (graphics, 3.3, 4, U"close-mid");
	Graphics_text (graphics, 3.3, 2, U"open-mid");
	Graphics_text (graphics, 3.3, 0, U"open");
	Graphics_setFontStyle (graphics, Graphics_NORMAL);
	{
		static const struct { double x, y; conststring32 string; } symbols [] = {
{ 4, 6, U"i" }, { 5, 6, U"y" }, { 6.5, 6, U"\\i-" }, { 7.5, 6, U"\\u-" }, { 9, 6, U"\\mt" }, { 10, 6, U"u" },
{ 4.2, 5, U"\\ic" }, { 5.2, 5, U"\\yc" }, { 9.8, 5, U"\\hs" },
{ 4, 4, U"e" }, { 5, 4, U"\\o/" }, { 6.5, 4, U"\\e-" }, { 7.5, 4, U"\\o-" }, { 9, 4, U"\\rh" }, { 10, 4, U"o" },
{ 7, 3, U"\\sw" }, { 7, 1, U"\\at" },
{ 4, 2, U"\\ef" }, { 5, 2, U"\\oe" }, { 6.5, 2, U"\\er" }, { 7.5, 2, U"\\kb" }, { 9, 2, U"\\vt" }, { 10, 2, U"\\ct" },
{ 4, 1, U"\\ae" },
{ 4, 0, U"a" }, { 5, 0, U"\\Oe" }, { 9, 0, U"\\as" }, { 10, 0, U"\\ab" },
		{ 0, 0, nullptr } };
		for (int i = 0; symbols [i]. string != nullptr; i ++)
			drawOneIpaSymbol (graphics, symbols [i]. x, symbols [i]. y, symbols [i]. string);
	}
}

void manual_Picture_init (ManPages me);
void manual_Picture_init (ManPages me) {

MAN_BEGIN (U"Special symbols", U"ppgb", 20080227)
INTRO (U"When drawing text into the @@Picture window@ or into the @@TextGridEditor@, "
	"you can use %%backslash sequences% to display various kinds of special symbols. "
	"You can usually also type the characters themselves, if your computer has an input method for them.")

ENTRY (U"European symbols")
NORMAL (U"To get the symbol \"\\a^\" (a-circumflex), you type \"\\bsa\\^ \", i.e., "
	"a sequence of backslash + a + circumflex. In this way, you can get a hundred "
	"non-ASCII symbols that are used in the alphabets of many European languages.")
LIST_ITEM (U"\\a\" \\bsa\\\"   \\e\" \\bse\\\"   \\i\" \\bsi\\\"   \\o\" \\bso\\\"   "
	"\\u\" \\bsu\\\"   \\y\" \\bsy\\\"  "
	"\\A\" \\bsA\\\"   \\E\" \\bsE\\\"   \\I\" \\bsI\\\"   \\O\" \\bsO\\\"   \\U\" \\bsU\\\"   "
	"\\Y\" \\bsY\\\" ")
LIST_ITEM (U"\\a\' \\bsa\'  \\c\' \\bsc\'  \\e\' \\bse\'  \\i\' \\bsi\'  \\n\' \\bsn\'  \\o\' \\bso\'  \\s\' \\bss\'  \\u\' \\bsu\'  \\y\' \\bsy\'  \\z\' \\bsz\'  "
	"\\A\' \\bsA\'  \\C\' \\bsC\'  \\E\' \\bsE\'  \\I\' \\bsI\'  \\N\' \\bsN\'  \\O\' \\bsO\'  \\S\' \\bsS\'  \\U\' \\bsU\'  \\Y\' \\bsY\'  \\Z\' \\bsZ\'")
LIST_ITEM (U"\\o: \\bso:  \\u: \\bsu:  \\O: \\bsO:  \\U: \\bsU:")
LIST_ITEM (U"\\a` \\bsa`  \\e` \\bse`  \\i` \\bsi`  \\o` \\bso`  \\u` \\bsu`  "
	"\\A` \\bsA`  \\E` \\bsE`  \\I` \\bsI`  \\O` \\bsO`  \\U` \\bsU`")
LIST_ITEM (U"\\a^ \\bsa\\^   \\e^ \\bse\\^   \\i^ \\bsi\\^   \\o^ \\bso\\^   \\u^ \\bsu\\^   "
	"\\A^ \\bsA\\^   \\E^ \\bsE\\^   \\I^ \\bsI\\^   \\O^ \\bsO\\^   \\U^ \\bsU\\^ ")
LIST_ITEM (U"\\a~ \\bsa\\~   \\n~ \\bsn\\~   \\o~ \\bso\\~   \\A~ \\bsA\\~   \\N~ \\bsN\\~   \\O~ \\bsO\\~  ")
LIST_ITEM (U"\\c< \\bsc<  \\d< \\bsd<  \\e< \\bse<  \\g< \\bsg<  \\n< \\bsn<  \\r< \\bsr<  \\s< \\bss<  \\t< \\bst<  \\z< \\bsz<  "
	"\\C< \\bsC<  \\D< \\bsD<  \\E< \\bsE<  \\G< \\bsG<  \\N< \\bsN<  \\R< \\bsR<  \\S< \\bsS<  \\T< \\bsT<  \\Z< \\bsZ<")
LIST_ITEM (U"\\d- \\bsd-  \\D- \\bsD-")
LIST_ITEM (U"\\ao \\bsao  \\uo \\bsuo  \\Ao \\bsAo  \\Uo \\bsUo")
LIST_ITEM (U"\\a; \\bsa;  \\e; \\bse;  \\A; \\bsA;  \\E; \\bsE;")
LIST_ITEM (U"\\ae \\bsae  \\Ae \\bsAe  \\o/ \\bso/  \\O/ \\bsO/  "
	"\\c, \\bsc,  \\C, \\bsC,  \\l/ \\bsl/  \\L/ \\bsL/  \\ss \\bsss  \\th \\bsth  \\z! \\bsz!  \\Z! \\bsZ!")
LIST_ITEM (U"\\!d \\bs!d  \\?d \\bs?d")
LIST_ITEM (U"\\eu \\bseu %euro, \\Lp \\bsLp %%pound sterling%, \\Y= \\bsY= %yen, "
	"\\fd \\bsfd %florin, \\c/ \\bsc/ %cent")
LIST_ITEM (U"\\SS \\bsSS %section, \\|| \\bs|| %pilcrow (%paragraph)")
LIST_ITEM (U"\\co \\bsco %copyright, \\re \\bsre %registered, \\tm \\bstm %trademark")
LIST_ITEM (U"\\a_ \\bsa_ %%feminine ordinal%, \\o_ \\bso_ %%masculine ordinal%")
LIST_ITEM (U"\\<< \\bs<< %%left-pointing guillemet%, \\>> \\bs>> %%right-pointing guillemet%")

ENTRY (U"Mathematical symbols")
LIST_ITEM (U"\\.c \\bs.c %%middle dot%, \\xx \\bsxx %multiplication, \\:- \\bs:- %division, \\/d \\bs/d %%division slash%")
LIST_ITEM (U"\\dg \\bsdg %degree, \\\'p \\bs\'p %prime (%minute), \\\"p \\bs\\\" p %%double prime% (%second)")
LIST_ITEM (U"\\-m \\bs-m %minus, \\-- \\bs-- %%en-dash%, \\+- \\bs+- %%plus-minus%")
LIST_ITEM (U"\\<_ \\bs<_ %%less than or equal to%, \\>_ \\bs>_ %%greater than or equal to%, \\=/ \\bs=/ %%not equal to%")
LIST_ITEM (U"\\no \\bsno %%logical not%, \\an \\bsan %%logical and%, \\or \\bsor %%logical or%")
LIST_ITEM (U"\\At \\bsAt %%for all%, \\Er \\bsEr %%there exists%, \\.3 \\bs.3 %therefore")
LIST_ITEM (U"\\oc \\bsoc %%proportional to%, \\=3 \\bs=3 %%defined as% (or %%congruent modulo%), \\~~ \\bs~~ %%approximately equal to%")
LIST_ITEM (U"\\Vr \\bsVr %%square root%")
LIST_ITEM (U"\\<- \\bs<-, \\-> \\bs->, \\<> \\bs<>")
LIST_ITEM (U"\\<= \\bs<=, \\=> \\bs=>, \\eq \\bseq")
LIST_ITEM (U"\\^| \\bs\\^ |, \\=~ \\bs=~ %%congruent to%, \\_| \\bs_|")
LIST_ITEM (U"\\oo \\bsoo %infinity, \\Tt \\bsTt %%up tack% (%%perpendicular to%)")
LIST_ITEM (U"\\O| \\bsO| %%empty set%, \\ni \\bsni %intersection, \\uu \\bsuu %union, "
	"\\c= \\bsc= %%subset of%, \\e= \\bse= %%element of%")
LIST_ITEM (U"\\dd \\bsdd %%partial differential%")
LIST_ITEM (U"\\ox \\bsox %%circled times%, \\o+ \\bso+ %%circled plus%")
LIST_ITEM (U"\\su \\bssu %summation, \\in \\bsin %integral")

ENTRY (U"Greek letters")
NORMAL (U"To get \\ep\\up\\ro\\et\\ka\\al, you type \\bsep\\bsup\\bsro\\bset\\bska\\bsal.")
LIST_ITEM (U"\t\\al \\bsal \t\\Al \\bsAl %alpha")
LIST_ITEM (U"\t\\be \\bsbe \t\\Be \\bsBe %beta")
LIST_ITEM (U"\t\\ga \\bsga \t\\Ga \\bsGa %gamma")
LIST_ITEM (U"\t\\de \\bsde \t\\De \\bsDe %delta")
LIST_ITEM (U"\t\\ep \\bsep \t\\Ep \\bsEp %epsilon")
LIST_ITEM (U"\t\\ze \\bsze \t\\Ze \\bsZe %zeta")
LIST_ITEM (U"\t\\et \\bset \t\\Et \\bsEt %eta")
LIST_ITEM (U"\t\\te \\bste \t\\Te \\bsTe %theta \t\\t2 \\bst2")
LIST_ITEM (U"\t\\io \\bsio \t\\Io \\bsIo %iota")
LIST_ITEM (U"\t\\ka \\bska \t\\Ka \\bsKa %kappa")
LIST_ITEM (U"\t\\la \\bsla \t\\La \\bsLa %lambda")
LIST_ITEM (U"\t\\mu \\bsmu \t\\Mu \\bsMu %mu")
LIST_ITEM (U"\t\\nu \\bsnu \t\\Nu \\bsNu %nu")
LIST_ITEM (U"\t\\xi \\bsxi \t\\Xi \\bsXi %xi")
LIST_ITEM (U"\t\\on \\bson \t\\On \\bsOn %omicron")
LIST_ITEM (U"\t\\pi \\bspi \t\\Pi \\bsPi %pi")
LIST_ITEM (U"\t\\ro \\bsro \t\\Ro \\bsRo %rho")
LIST_ITEM (U"\t\\si \\bssi \t\\Si \\bsSi %sigma \t\\s2 \\bss2")
LIST_ITEM (U"\t\\ta \\bsta \t\\Ta \\bsTa %tau")
LIST_ITEM (U"\t\\up \\bsup \t\\Up \\bsUp %upsilon")
LIST_ITEM (U"\t\\fi \\bsfi \t\\Fi \\bsFi %phi \t\\f2 \\bsf2")
LIST_ITEM (U"\t\\ci \\bsci \t\\Ci \\bsCi %chi")
LIST_ITEM (U"\t\\ps \\bsps \t\\Ps \\bsPs %psi")
LIST_ITEM (U"\t\\om \\bsom \t\\Om \\bsOm %omega \t\\o2 \\bso2")

ENTRY (U"Hebrew letters")
NORMAL (U"To get an alef, you type \\bs?+. "
	"On Windows, you have to install international support to get the left-to-right order correct.")
LIST_ITEM (U"\t\\?+ \\bs?+\t%alef")
LIST_ITEM (U"\t\\B+ \\bsB+\t%bet")
LIST_ITEM (U"\t\\G+ \\bsG+\t%gimel")
LIST_ITEM (U"\t\\D+ \\bsD+\t%dalet")
LIST_ITEM (U"\t\\H+ \\bsH+\t%he")
LIST_ITEM (U"\t\\V+ \\bsV+\t%vav")
LIST_ITEM (U"\t\\Z+ \\bsZ+\t%zayin")
LIST_ITEM (U"\t\\X+ \\bsX+\t%het")
LIST_ITEM (U"\t\\Y+ \\bsY+\t%tet")
LIST_ITEM (U"\t\\J+ \\bsJ+\t%yod")
LIST_ITEM (U"\t\\K+ \\bsK+\t%kaf\t\\K% \\bsK\\% ")
LIST_ITEM (U"\t\\L+ \\bsL+\t%lamed")
LIST_ITEM (U"\t\\M+ \\bsM+\t%mem\t\\M% \\bsM\\% ")
LIST_ITEM (U"\t\\N+ \\bsN+\t%nun\t\\N% \\bsN\\% ")
LIST_ITEM (U"\t\\S+ \\bsS+\t%samekh")
LIST_ITEM (U"\t\\9+ \\bs9+\t%ayin")
LIST_ITEM (U"\t\\P+ \\bsP+\t%pe\t\\P% \\bsP\\% ")
LIST_ITEM (U"\t\\C+ \\bsC+\t%tsadi\t\\C% \\bsC\\% ")
LIST_ITEM (U"\t\\Q+ \\bsQ+\t%qof")
LIST_ITEM (U"\t\\R+ \\bsR+\t%resh")
LIST_ITEM (U"\t\\W+ \\bsW+\t%shin")
LIST_ITEM (U"\t\\T+ \\bsT+\t%tav")
LIST_ITEM (U"\t\\?+\\hI \\bs?+\\bshI\t%hiriq")
LIST_ITEM (U"\t\\?+\\sE \\bs?+\\bssE\t%segol\t\\?+\\cE \\bs?+\\bscE\t%tsere")
LIST_ITEM (U"\t\\?+\\qA \\bs?+\\bsqA\t%qamats\t\\?+\\pA \\bs?+\\bspA\t%patah")
LIST_ITEM (U"\t\\?+\\hO \\bs?+\\bshO\t%holam")
LIST_ITEM (U"\t\\?+\\qU \\bs?+\\bsqU\t%qubuts")
LIST_ITEM (U"\t\\dq \\bsdq\t%dagesh or %maliq")
NORMAL (U"Combinations:")
LIST_ITEM (U"\t\\vO \\bsvO\t%%vav holam")
LIST_ITEM (U"\t\\sU \\bssU\t%shuruq")

ENTRY (U"Phonetic symbols")
NORMAL (U"See @@Phonetic symbols@")

ENTRY (U"Miscellaneous")
LIST_ITEM (U"\\bs \\bsbs %backslash")
LIST_ITEM (U"\\bu \\bsbu %bullet")
LIST_ITEM (U"\\cl \\bscl (%club), \\di \\bsdi (%diamond), \\he \\bshe (%heart), \\sp \\bssp (%spade)")
LIST_ITEM (U"\\pf \\bspf %%pointing finger%, \\f5 \\bsf5 %%flower-five%")

ENTRY (U"See also")
NORMAL (U"@@Text styles@")
MAN_END

MAN_BEGIN (U"Text styles", U"ppgb", 19961012)
INTRO (U"When drawing text into the @@Picture window@ or into an editor, "
	"you can use text styles other than regular Roman.")
ENTRY (U"Italic, bold, superscript, subscript")
NORMAL (U"With the following symbols, you introduce stretches of text drawn in special styles:")
LIST_ITEM (U"\\% : the following letter will be italic.")
LIST_ITEM (U"\\# : the following letter will be bold.")
LIST_ITEM (U"\\# \\% : the following letter will be bold-italic.")
LIST_ITEM (U"\\^ : the following letter will be superscript: \\% m\\% c\\^ 2 gives %mc^2.")
LIST_ITEM (U"\\_ : the following letter will be subscript. Example: %F__0_ is typed as \\% F_0.")
LIST_ITEM (U"\\% \\% : the following letters will be italic, until the following \\% :")
LIST_ITEM (U"    $$Now \\% \\% you\\%  try$   gives:   Now %%you% try.")
LIST_ITEM (U"The same goes for \\# \\# , \\^ \\^ , and \\_ \\_ .")
LIST_ITEM (U"\\bss{...}: small:")
LIST_ITEM (U"    $$W\\bss{ARP}$   gives:   W\\s{ARP}")
NORMAL (U"To draw a \\% , \\# , \\^ , or \\_  symbol, you type \"$$\\bs\\%  $\", "
	"\"$$\\bs\\#  $\", \"$$\\bs\\^  $\", or \"$$\\bs\\_  $\": a backslash, the symbol, and a space.")
NORMAL (U"On Xwindows machines, the font `Symbol' will never look bold or italic, "
	"but it will be printed correctly.")
ENTRY (U"See also")
NORMAL (U"@@Special symbols@")
MAN_END

MAN_BEGIN (U"Phonetic symbols", U"ppgb", 20090804)
NORMAL (U"To draw phonetic symbols in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can either type the symbols directly (if your computer has an input method for them), or use backslash sequences as described in:")
LIST_ITEM (U"\\bu @@Phonetic symbols: consonants")
LIST_ITEM (U"\\bu @@Phonetic symbols: vowels")
LIST_ITEM (U"\\bu @@Phonetic symbols: diacritics")
MAN_END

MAN_BEGIN (U"Phonetic symbols: consonants", U"ppgb", 20090804)
NORMAL (U"To draw phonetic symbols for consonants in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can either type the symbols directly (if your computer has an input method for them), or use the backslash sequences in the following table.")
PICTURE (6.0, 7.0, draw_IPA_consonant_chart)
NORMAL (U"Other consonant symbols:")
LIST_ITEM (U"\\l~ \\bsl~ (%%l with tilde%): velarized %l")
LIST_ITEM (U"\\hj \\bshj (%%heng with hooktop%): the Swedish rounded post-alveolar & velar fricative")
ENTRY (U"How to remember the codes")
NORMAL (U"For most of the codes, the first letter tells you the most similar letter of the English alphabet. "
	"The second letter can be %t (%turned), %c (%capital or %curled), %s (%script), - (%barred), %l (%%with leg%), "
	"%i (%inverted), or %j (%%left tail%). Some phonetic symbols are similar to Greek letters but have special "
	"phonetic (%f) versions with serifs (\\ff, \\bf, \\gf) or are otherwise slightly different (\\tf, \\cf). "
	"The codes for \\ng (%engma), \\dh (%eth), \\sh (%esh), and \\zh (%yogh) are traditional alternative spellings. "
	"The retroflexes have a period in the second place, because an alternative traditional spelling is to write a dot under them. "
	"The code for \\fh is an abbreviation for %fishhook.")
MAN_END

MAN_BEGIN (U"Phonetic symbols: diacritics", U"ppgb", 20130327)
NORMAL (U"To draw phonetic diacritical symbols in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can either type the symbols directly (if your computer has an input method for them), or use the backslash sequences in the following list.")
NORMAL (U"Understrikes:")
LIST_ITEM (U"n\\|v n\\bs|v (%%combining vertical line below%): syllabic consonant")
LIST_ITEM (U"b\\0v b\\bs0v (%%combining ring below%): voiceless (e.g. lenis voiceless plosive, voiceless nasal or approximant)")
LIST_ITEM (U"o\\Tv o\\bsTv (%%combining down tack below%, %lowering): lowered vowel; or turns a fricative into an approximant")
LIST_ITEM (U"o\\T^ o\\bsT\\^  (%%combining up tack below%, %raising): raised vowel; or turns an approximant into a fricative")
LIST_ITEM (U"o\\T( o\\bsT( (%%combining left tack below%, %atr): advanced tongue root")
LIST_ITEM (U"o\\T) o\\bsT) (%%combining right tack below%, %rtr): retracted tongue root")
LIST_ITEM (U"e\\-v e\\bs-v (%%combining macron below%): backed")
LIST_ITEM (U"o\\+v o\\bs+v (%%combining plus sign below%): fronted")
LIST_ITEM (U"o\\:v o\\bs:v (%%combining diaeresis below%): breathy voice")
LIST_ITEM (U"o\\~v o\\bs~v (%%combining tilde below%): creaky voice")
LIST_ITEM (U"d\\Nv d\\bsNv (%%combining bridge below%): dental (as opposed to alveolar)")
LIST_ITEM (U"d\\Uv d\\bsUv (%%combining inverted bridge below%): apical")
LIST_ITEM (U"d\\Dv d\\bsDv (%%combining square below%): laminal")
LIST_ITEM (U"u\\nv u\\bsnv (%%combining inverted breve below%): nonsyllabic")
LIST_ITEM (U"e\\3v e\\bs3v (%%combining right half ring below%): slightly rounded")
LIST_ITEM (U"u\\cv u\\bscv (%%combining left half ring below%): slightly unrounded")
NORMAL (U"Overstrikes:")
LIST_ITEM (U"\\gf\\0^ \\bsgf\\bs0\\^  (%%combining ring above%): voiceless")
LIST_ITEM (U"\\ef\\\'^ \\bsef\\bs\'\\^  (%%combining acute accent%): high tone")
LIST_ITEM (U"\\ef\\`^ \\bsef\\bs`\\^  (%%combining grave accent%): low tone")
LIST_ITEM (U"\\ef\\-^ \\bsef\\bs-\\^  (%%combining macron%): mid tone (or so)")
LIST_ITEM (U"\\ef\\~^ \\bsef\\bs~\\^  (%%combining tilde%): nasalized")
LIST_ITEM (U"\\ef\\v^ \\bsef\\bsv\\^  (%%combining caron%, %%háček%, %wedge): rising tone")   // UNITEXT_LATIN_SMALL_LETTER_C_WITH_CARON
LIST_ITEM (U"\\ef\\^^ \\bsef\\bs\\^ \\^  (%%combining circumflex accent%): falling tone")
LIST_ITEM (U"o\\:^ o\\bs:\\^  (%%combining diaeresis%): centralized")
LIST_ITEM (U"\\ef\\N^ \\bsef\\bsN\\^  (%%combining breve%): short")
LIST_ITEM (U"k\\lip t\\lis k\\bslip (%%combining double inverted breve%, %ligature): simultaneous articulation, or single segment")
NORMAL (U"In line:")
LIST_ITEM (U"\\:f \\bs:f the phonetic length sign")
LIST_ITEM (U"\\.f \\bs.f half length")
LIST_ITEM (U"\\\'1 \\bs\'1 primary stress")
LIST_ITEM (U"\\\'2 \\bs\'2 secondary stress")
LIST_ITEM (U"\\|f \\bs|f the phonetic stroke")
LIST_ITEM (U"t\\cn t\\bscn (%%combining left angle above%, %corner): unreleased plosive")
LIST_ITEM (U"\\er\\hr \\bser\\bshr (%%combining rhotic hook%): rhotacized vowel")
NORMAL (U"Not available in EPS files (i.e. only publishable with copy-paste or with PDF files):")
LIST_ITEM (U"t\\^h t\\bs\\^ h aspiration")
LIST_ITEM (U"b\\^H b\\bs\\^ H voiced aspiration (breathiness)")
LIST_ITEM (U"t\\^j t\\bs\\^ j palatalization")
LIST_ITEM (U"t\\^g t\\bs\\^ g, t\\^M t\\bs\\^ M, t\\^G t\\bs\\^ G velarization")
LIST_ITEM (U"k\\^w k\\bs\\^ w rounding")
LIST_ITEM (U"t\\^Y t\\bs\\^ Y rounding with palatalization")
LIST_ITEM (U"a\\^? a\\bs\\^ ? glottalization")
LIST_ITEM (U"t\\^9 t\\bs\\^ 9 pharyngealization")
LIST_ITEM (U"t\\^l t\\bs\\^ l lateral release")
LIST_ITEM (U"t\\^n t\\bs\\^ n, p\\^m p\\bs\\^ m, k\\^N k\\bs\\^ N nasal release")
LIST_ITEM (U"t\\^s t\\bs\\^ s, k\\^x k\\bs\\^ x, p\\^f p\\bs\\^ f affrication")
LIST_ITEM (U"\\^y \\bs\\^ y (palatalization in a deprecated American notation)")
LIST_ITEM (U"a\\_ub a\\bs\\_ ub undertie (liaison, if spaces don't mean breaks in your transcription)")
LIST_ITEM (U"\\ts \\bsts t\\--s ligature")
LIST_ITEM (U"\\tS \\bstS tesh ligature")
LIST_ITEM (U"p\\ap p\\bsap apostrophe (for ejectives)")
MAN_END

MAN_BEGIN (U"Phonetic symbols: vowels", U"ppgb", 20090804)
NORMAL (U"To draw phonetic symbols for vowels in the @@Picture window@ or in the @TextGridEditor, "
	"make sure that you have installed the Charis SIL and/or Doulos SIL font, for instance from www.sil.org or from www.praat.org. "
	"You can either type the symbols directly (if your computer has an input method for them), or use the backslash sequences in the following table.")
PICTURE (6.0, 5.0, draw_IPA_vowel_chart)
NORMAL (U"Other vowel symbols are:")
LIST_ITEM (U"\\sr \\bssr (%%schwa with right hook%): rhotacized schwa")
NORMAL (U"Not available in EPS files (i.e. only publishable with copy-paste or with PDF files):")
LIST_ITEM (U"\\id \\bsid syllabic dental approximant")
LIST_ITEM (U"\\ir \\bsir syllabic retroflex (or apico-postalveolar) approximant")
ENTRY (U"How to remember the codes")
NORMAL (U"For most of the codes, the first letter tells you the most similar letter of the English alphabet. "
	"The second letter can be %t (%turned), %c (%capital), %s (%script), %r (%reversed), - (%barred or %retracted), or / (%slashed). "
	"One symbol (\\ef) is a phonetic version of a Greek letter. "
	"The codes for \\sw, \\rh, \\hs and \\kb are abbreviations for %schwa, %%ram's horn%, %horseshoe, and %%kidney bean%.")
MAN_END

MAN_BEGIN (U"Axes...", U"ppgb", 20140107)
INTRO (U"One of the commands in the #Margins and #World menus of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To view and change the current world coordinates of the horizontal and vertical axes.")
ENTRY (U"Usage")
NORMAL (U"The axes are normally changed by every drawing operation in the dynamic menu, "
	"i.e., by object-specific drawing commands with titles like ##Draw...# and ##Paint...# "
	"(the drawing commands in the Picture window, like ##Paint rectangle...#, do not change the axes).")
NORMAL (U"You would use the ##Axes...# command if your data are not in an object.")
ENTRY (U"Example")
NORMAL (U"The following script would draw a person's vowel triangle:")
CODE (U"\\#  Put F1 (between 300 and 800 Hz) along the horizontal axis,")
CODE (U"\\#  and F2 (between 600 and 3600 Hz) along the vertical axis.")
CODE (U"##Axes:# 300, 800, 600, 3600")
CODE (U"\\#  Draw a rectangle inside the current viewport (selected area),")
CODE (U"\\#  with text in the margins, and tick marks in steps of 100 Hz along the F1 axis,")
CODE (U"\\#  and in steps of 200 Hz along the F2 axis.")
CODE (U"Draw inner box")
CODE (U"Text top: \"no\", \"Daniël's Dutch vowel triangle\"")
CODE (U"Text bottom: \"yes\", \"\\% F_1 (Hz)\"")
CODE (U"Text left: \"yes\", \"\\% F_2 (Hz)\"")
CODE (U"Marks bottom every: 1, 100, \"yes\", \"yes\", \"yes\", \"\"")
CODE (U"Marks left every: 1, 200, \"yes\", \"yes\", \"yes\", \"\"")
CODE (U"\\#  Draw large phonetic symbols at the vowel points.")
CODE (U"Text special: 340, \"Centre\", 688, \"Half\", \"Times\", 24, \"0\", \"u\"")
CODE (U"Text special: 481, \"Centre\", 1195, \"Half\", \"Times\", 24, \"0\", \"ø\"")
CODE (U"\\#  Etcetera")
NORMAL (U"This example would draw the texts \"Daniël's Dutch vowel triangle\", "
	"\"%F__1_ (Hz)\", and \"%F__2_ (Hz)\" in the margins, "
	"and the texts \"u\" and \"ø\" at the appropriate positions inside the drawing area.")
MAN_END

MAN_BEGIN (U"Copy to clipboard", U"ppgb", 20120430)   /* Not Unix. */
INTRO (U"A command in the File menu of the @@Picture window@ on Windows and Macintosh.")
NORMAL (U"It copies the selected part of the picture to the clipboard. "
	"You can then #Paste it into any program that knows how to display pictures.")
ENTRY (U"Behaviour")
NORMAL (U"Though all the picture data will be written to the clipboard, "
	"only the part that corresponds to the selected part of the Picture window (the %viewport) will be visible.")
ENTRY (U"Usage")
NORMAL (U"On Windows, if you have a PostScript printer, you may want to use @@Save as EPS file...@ instead (experiment with the results after converting to PDF or printing). "
	"If the picture is too large, e.g. a spectrogram that does not seem to fit into the clipboard, "
	"you can try @@Save as Windows metafile...@ instead.")
NORMAL (U"On the Mac, the picture will be put on the clipboard in PDF format, "
	"which is recognized by programs that know how to handle PDF pictures, such as Microsoft^\\re Word\\tm 2008; "
	"the quality of the picture will be exactly as good as when you use @@Save as PDF file...@ and read in the resulting PDF file. "
	"Please realize that Word will convert the PDF picture to a mediocre 300-dpi bitmap if you save the document as a ##.doc# file; "
	"therefore, you should save the document as a ##.docx# file instead. "
	"If you cannot work with ##.docx# files (for instance because your publisher does not accept that file type yet), "
	"consider using @@Save as EPS file...@ instead. In older programs, such as Microsoft^\\re Word\\tm 2004, you cannot paste Praat's PDF pictures; "
	"use @@Save as EPS file...@ instead.")
MAN_END

MAN_BEGIN (U"Draw inner box", U"ppgb", 19970330)
INTRO (U"One of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw a rectangle inside the drawing area, "
	"leaving @margins on all four sides for drawing text and marks.")
ENTRY (U"Behaviour")
NORMAL (U"The widths of the margins depend on the current font size.")
MAN_END

MAN_BEGIN (U"Encapsulated PostScript", U"ppgb", 20110129)
INTRO (U"a kind of PostScript file that can easily be imported into word processors and drawing programs. "
	"In Praat, you can create an Encapsulated PostScript (EPS) file by choosing @@Save as EPS file...@.")
MAN_END

MAN_BEGIN (U"Erase all", U"ppgb", 19980825)
INTRO (U"A command in the #Edit menu of the @@Picture window@.")
NORMAL (U"It erases all your drawings.")
MAN_END

MAN_BEGIN (U"Font menu", U"ppgb", 20101125)
INTRO (U"One of the menus of the @@Picture window@.")
NORMAL (U"It allows you to choose the font of the text "
	"to be used in subsequent drawing, as well as its size (character height).")
ENTRY (U"Sizes")
NORMAL (U"You can choose any of the sizes 10, 12, 14, 18, or 24 directly from this menu, "
	"or fill in any other size in the ##Font size...# form.")
NORMAL (U"The widths of the margins depend on the current font size, "
	"so if you want to change the font size, "
	"do so before making your drawing.")
ENTRY (U"Fonts")
NORMAL (U"With these commands, you set the font in which subsequent text will be drawn: "
	"Times, Helvetica, Palatino, or Courier.")
NORMAL (U"You can mix the Symbol and IPA alphabets with the normal Roman alphabets "
	"and use sequences of backslash + digraph for @@special symbols@ (see also @@phonetic symbols@).")
NORMAL (U"For instance, you can get an \\e\" by typing \\bse\\\" , or a \\ss by typing \\bsss; "
	"you can get an \\ep by typing \\bsep, or a \\ct, which is a turned c, by typing \\bsct.")
ENTRY (U"Styles")
NORMAL (U"There are no commands in the menu for text styles (bold, italic), "
	"but you can still use them in the Picture window: see @@text styles@.")
MAN_END

MAN_BEGIN (U"Logarithmic marks left/right/top/bottom...", U"ppgb", 19970330)
INTRO (U"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw a specified number of marks per decade "
	"into the @margins around the drawing area, along a logarithmic axis.")
ENTRY (U"Settings")
TAG (U"%%Marks per decade")
DEFINITION (U"the number of marks that will be drawn for every decade.")
TAG (U"%%Write numbers")
DEFINITION (U"if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (U"%%Draw ticks")
DEFINITION (U"if on, short line pieces will be drawn in the margin.")
TAG (U"%%Draw dotted lines")
DEFINITION (U"if on, dotted lines will be drawn through your drawing.")
ENTRY (U"Behaviour")
LIST_ITEM (U"If your vertical logarithmic axis runs from 10 to 100, "
	"and %%Marks per decade% is 1, marks will only be drawn at 10 and 100;")
LIST_ITEM (U"if %%Marks per decade% is 2, marks will be drawn at 10, 30, and 100;")
LIST_ITEM (U"if it is 3, marks will be drawn at 10, 20, 50, and 100;")
LIST_ITEM (U"if it is 4, marks will be drawn at 10, 20, 30, 50, and 100;")
LIST_ITEM (U"if it is 5, marks will be drawn at 10, 20, 30, 50, 70, and 100;")
LIST_ITEM (U"if it is 6, marks will be drawn at 10, 15, 20, 30, 50, 70, and 100;")
LIST_ITEM (U"if it is 7 (the maximum), marks will be drawn at 10, 15, 20, 30, 40, 50, 70, and 100.")
MAN_END

MAN_BEGIN (U"Margins", U"ppgb", 19970405)
INTRO (U"The space around most of your drawings in the @@Picture window@.")
ENTRY (U"World coordinates")
NORMAL (U"With the commands in the #Margins menu, "
	"you draw text, ticks, numbers, or a rectangle, "
	"in the margins around the latest drawing that you made, "
	"or you draw dotted lines through or text inside this last drawing.")
NORMAL (U"You specify the positions of these things in world coordinates, "
	"i.e., in coordinates that refer to the natural coordinate system of your last drawing.")
NORMAL (U"The numbers that you can mark around your drawing also refer to these coordinates. "
	"For instance, after drawing a spectrum with ##Spectrum: Draw...#, "
	"you can draw a dotted line at 2000 Hz or at 60 dB "
	"by choosing ##One mark bottom...# or ##One mark left...# "
	"and typing \"2000\" or \"60\", respectively.")
ENTRY (U"Usage")
NORMAL (U"The margin commands work with all the drawings that leave margins around themselves, "
	"such as ##Sound: Draw...#, ##Spectrogram: Paint...#, ##Polygon: Paint...#, and more. "
	"They do not work properly, however, with the commands that draw vocal tract shapes, "
	"like ##Art & Speaker: Draw...# and ##Artword & Speaker: Draw...#, "
	"because these can only be drawn correctly into a square viewport.")
ENTRY (U"Limited validity")
NORMAL (U"The margin commands work only on the latest drawing that you made "
	"(unless you @Undo that drawing).")
ENTRY (U"Margin size")
NORMAL (U"The size of the margins depends on the font size, "
	"so be sure that you have the font size of your choice before you make your drawing. "
	"You can set the font size with the @@Font menu@.")
MAN_END

MAN_BEGIN (U"Marks left/right/top/bottom...", U"ppgb", 19970330)
INTRO (U"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw any number of equally spaced marks into the @margins around the drawing area.")
ENTRY (U"Settings")
TAG (U"##Number of marks")
DEFINITION (U"the number of equally spaced marks (2 or more) that will be drawn; "
	"there will always be marks at the beginning and end of the domain or range.")
TAG (U"##Write numbers")
DEFINITION (U"if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (U"##Draw ticks")
DEFINITION (U"if on, short line pieces will be drawn in the margin.")
TAG (U"##Draw dotted lines")
DEFINITION (U"if on, dotted lines will be drawn through your drawing.")
ENTRY (U"Example")
NORMAL (U"If you draw a Sound with a domain between 0 and 1 seconds "
	"to an amplitude range between -1 and 1, "
	"choosing ##Marks left...# with a number of 3 and %%Draw dotted lines% on, "
	"will give you horizontal marks and horizontal dotted lines at -1, 0, and 1; "
	"choosing ##Marks bottom...# with a number of 6 and %%Draw dotted lines% off, "
	"will give you vertical marks at 0, 0.2, 0.4, 0.6, 0.8, and 1.")
MAN_END

MAN_BEGIN (U"Marks left/right/top/bottom every...", U"ppgb", 19970330)
INTRO (U"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw a number of equally spaced marks into the @margins around the drawing area.")
ENTRY (U"Settings")
TAG (U"##Units")
DEFINITION (U"the units, relative to the standard units, "
	"for writing the numbers; for example, if you want time in milliseconds "
	"instead of seconds (which is always the standard), "
	"#Units should be 0.001.")
TAG (U"##Distance")
DEFINITION (U"the distance between the equally spaced marks that will be drawn, "
	"expressed in #Units; for example, if you want marks every 20 milliseconds, "
	"and %Units is 0.001, this argument should be 20.")
TAG (U"##Write numbers")
DEFINITION (U"if on, real numbers will be written in the margin, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (U"##Draw ticks")
DEFINITION (U"if on, short line pieces will be drawn in the margin.")
TAG (U"##Draw dotted lines")
DEFINITION (U"if on, dotted lines will be drawn through your drawing.")
MAN_END

MAN_BEGIN (U"One logarithmic mark left/right/top/bottom...", U"ppgb", 19970330)
INTRO (U"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw one mark into one of the four @margins "
	"around the drawing area, along a logarithmic axis.")
ENTRY (U"Settings")
TAG (U"##Position")
DEFINITION (U"the %x (for top or bottom) or %y (for left or right) position of the mark, "
	"expressed in the logarithmic domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (U"##Write number")
DEFINITION (U"if on, a real number equal to #Position will be written in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
TAG (U"##Draw tick")
DEFINITION (U"if on, a short line piece will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
TAG (U"##Draw dotted line")
DEFINITION (U"if on, a dotted line will be drawn through your drawing, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
TAG (U"##Draw text")
DEFINITION (U"if not empty, this text will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
ENTRY (U"Example")
NORMAL (U"After you draw a Pitch logarithmically in a range between 100 and 400 Hz, "
	"choosing ##One logarithmic mark left...# with a position of 200 and %%Draw dotted line% on, "
	"will give you a horizontal mark \"200\" and a horizontal dotted line at a %y position of 200, "
	"which is exactly halfway between 100 and 400 Hz.")
MAN_END

MAN_BEGIN (U"One mark left/right/top/bottom...", U"ppgb", 19970330)
INTRO (U"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw one mark into one of the four @margins around the drawing area.")
ENTRY (U"Settings")
TAG (U"##Position")
DEFINITION (U"the %x (for top or bottom) or %y (for left or right) position of the mark, "
	"expressed in the domain or range of your latest drawing "
	"in the horizontal or vertical direction.")
TAG (U"##Write number")
DEFINITION (U"if on, a real number equal to `Position' will be written in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
TAG (U"##Draw tick")
DEFINITION (U"if on, a short line piece will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
TAG (U"##Draw dotted line")
DEFINITION (U"if on, a dotted line will be drawn through your drawing, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
TAG (U"##Draw text")
DEFINITION (U"if not empty, this text will be drawn in the margin, "
	"at an %x (for top or bottom) or %y (for left or right) position equal to #Position.")
ENTRY (U"Example")
NORMAL (U"If you draw a Sound to an amplitude range between -1 and 1, "
	"choosing ##One mark left...# with a position of 0.0 and %%Draw dotted line% on, "
	"will give you a horizontal mark \"0\" and a horizontal dotted line at a y position of 0.")
MAN_END

MAN_BEGIN (U"Pen menu", U"ppgb", 20091215)
INTRO (U"One of the menus of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To choose the line type and @colour "
	"to be used in subsequent drawing of lines and text.")
ENTRY (U"Behaviour")
NORMAL (U"The line type used by @@Draw inner box@ (solid), "
	"and the line type of the dotted lines in the ##Mark...# commands "
	"will not be affected.")
NORMAL (U"The commands in the @Margins menu will always draw in black.")
MAN_END

MAN_BEGIN (U"Colour", U"ppgb", 20091215)
INTRO (U"In windows that pop up when you choose ##Colour...# from the @@Pen menu@ "
	"or any of the #Paint commands in the ##World menu#, you can see the following field:")
CODE (U"Colour (0-1, name, or {r,g,b})")
NORMAL (U"This means that you can specify here a colour in any of three ways:")
TAG (U"\\bu a grey value between 0 and 1:")
DEFINITION (U"a value of 0 means black, 1 means white, 0.5 means grey, 0.75 means silver, and so on.")
TAG (U"\\bu a colour name:")
DEFINITION (U"you can choose from Black, White, Red, Green, Blue, Yellow, Cyan, Magenta, Maroon, Lime, Navy, Teal, "
	"Purple, Olive, Pink, Silver, Grey, i.e. from any of the colours in the Pen menu. "
	"You can write these either with a capital (Red) or in lowercase (red).")
TAG (U"\\bu an RGB colour:")
DEFINITION (U"you can specify a red-green-blue value as three values between 0 and 1, enclosed within braces "
	"and separated by commas, e.g. {0.8,0.1,0.2} is something reddish.")
MAN_END

MAN_BEGIN (U"Picture window", U"ppgb", 20140325)
INTRO (U"One of the two main windows in Praat.")
TAG (U"File menu")
LIST_ITEM (U"\\bu @@Save as PDF file...")
LIST_ITEM (U"\\bu @@Save as PNG file...")
LIST_ITEM (U"\\bu @@Save as EPS file...")
LIST_ITEM (U"\\bu @@Save as Windows metafile...@")
LIST_ITEM (U"\\bu @@Read from Praat picture file...@, @@Save as Praat picture file...")
LIST_ITEM (U"\\bu @@PostScript settings...")
LIST_ITEM (U"\\bu @@Print...")
TAG (U"Edit menu")
LIST_ITEM (U"\\bu @@Undo@")
LIST_ITEM (U"\\bu @@Copy to clipboard@")
LIST_ITEM (U"\\bu @@Erase all@")
TAG (U"@Margins menu")
LIST_ITEM (U"\\bu @@Draw inner box")
LIST_ITEM (U"\\bu @@Text left/right/top/bottom...")
LIST_ITEM (U"\\bu @@Marks left/right/top/bottom every...")
LIST_ITEM (U"\\bu @@One mark left/right/top/bottom...")
LIST_ITEM (U"\\bu @@Marks left/right/top/bottom...")
LIST_ITEM (U"\\bu @@Logarithmic marks left/right/top/bottom...")
LIST_ITEM (U"\\bu @@One logarithmic mark left/right/top/bottom...")
LIST_ITEM (U"\\bu @@Axes...")
TAG (U"World menu")
LIST_ITEM (U"\\bu @@Text...")
LIST_ITEM (U"\\bu @@Axes...")
TAG (U"Select menu")
LIST_ITEM (U"\\bu @@Select inner viewport...@, @@Select outer viewport...@, @@Viewport text...")
TAG (U"@@Pen menu")
TAG (U"@@Font menu")
MAN_END

MAN_BEGIN (U"PostScript settings...", U"ppgb", 20201229)
INTRO (U"One of the commands in the File menus of many windows. "
	"The PostScript settings influence @Printing and saving to @@Encapsulated PostScript@ files.")
ENTRY (U"Settings")
TAG (U"##Allow direct PostScript printing# (Windows only)")
DEFINITION (U"this determines whether Praat prints explicit PostScript commands to your printer "
	"if it is a PostScript printer. This is what you will usually want. However, if you find "
	"that some of the options that you choose in the #Print window seem not to be supported "
	"(e.g. scaling, printing two-up...), you may switch this off; Praat will then send native "
	"Windows drawing commands, which the printer driver will try to translate "
	"to PostScript. If your printer does not support PostScript, this switch is ignored. "
	"On Macintosh, this switch is ignored, because all printingis done in PDF. "
	"On Unix, this switch is superfluous, because all printing is done directly in PostScript.")
TAG (U"##Grey resolution")
DEFINITION (U"you can choose from two image qualities:")
LIST_ITEM1 (U"\\bu the %finest quality for grey plots (106 spots per inch), "
	"which gives the best results directly from the printer;")
LIST_ITEM1 (U"\\bu a %photocopyable quality, which has fewer spots per inch (85) and "
	"gives the best results after photocopying.")
DEFINITION (U"Your choice of the grey resolution influences direct PostScript printing "
	"and saving to @@Encapsulated PostScript@ files.")
TAG (U"##Paper size# (Unix only)")
DEFINITION (U"you can choose from A4 (210 \\xx 297 mm), A3 (297 \\xx 420 mm) or US Letter (8.5 \\xx 11\\\"p). "
	"This choice applies to Unix only; on Windows, you choose the paper size in the ##Print...# window; "
	"on Macintosh, you choose the paper size in the ##Page setup...# window.")
TAG (U"##Orientation# (Unix only)")
DEFINITION (U"you can choose between %portrait (e.g., 297 mm high and 210 mm wide) "
	"and %landscape (e.g., 210 mm high and 297 mm wide). "
	"This choice applies to Unix only; on Windows, you choose the orientation in the ##Print...# window; "
	"on Macintosh, you choose the orientation in the ##Page setup...# window.")
TAG (U"##Magnification# (Unix only)")
DEFINITION (U"the relative size with which your picture will be printed; normally 1.0. "
	"This choice applies to Unix only; on Windows, you choose the scaling in the ##Print...# window; "
	"on Macintosh, you choose the scaling in the ##Page setup...# window.")
TAG (U"##Print command# (Unix only)")
DEFINITION (U"When printing on Unix, a temporary PostScript^\\re file is created in the \"/tmp\" folder; "
	"it will have a name like \"picXXXXXX\", and is automatically removed after printing. "
	"This file is sent to the printer with the print command, which will often look like $$lp -c \\% s$, "
	"where $$\\% s$ stands for the file name.")
MAN_END

MAN_BEGIN (U"Print...", U"ppgb", 20001010)
INTRO (U"One of the commands in the File menu of the @@Picture window@.")
NORMAL (U"With this command, you send your entire picture immediately to the printer. "
	"See the @Printing tutorial for details.")
MAN_END

MAN_BEGIN (U"Printing", U"ppgb", 20120430)
ENTRY (U"1a. Printing on Windows")
NORMAL (U"On Windows, the best results will be obtained on PostScript printers, since these have built-in "
	"facilities for images (e.g. spectrograms) and rotated text. If a PostScript printer is available, "
	"Praat will usually write direct PostScript commands to that printer "
	"(see @@PostScript settings...@ if you want to switch this off). "
	"Praat also supports non-PostScript printers, such as most colour inkjet printers.")
NORMAL (U"If you don't have a PostScript printer, and you still want PostScript quality, "
	"you can save the picture to an EPS file (@@Save as EPS file...@). "
	"You can then view this file with the freely available "
	"GhostView^\\tm program, which you can download from ##http://pages.cs.wisc.edu/~ghost/#, "
	"or convert it to PDF with either GhostView or Adobe^\\re Acrobat^\\tm Distiller^\\tm, which is more reliable than GhostView "
	"but is also expensive.")
ENTRY (U"1b. Printing on Macintosh")
NORMAL (U"On the Mac, Praat will print in PDF, both to PostScript and non-PostScript printers.")
ENTRY (U"1c. Printing on Linux")
NORMAL (U"On Linux, when you tell Praat to print a picture or manual page, Praat will write the picture to a temporary PostScript file "
	"and send this file to a printer with the %%print command%, which you can change "
	"with @@PostScript settings...@. You do not need a PostScript printer to print PostScript directly, "
	"because the #lpr program sends PostScript files through the GhostScript program, "
	"which is a part of all modern Linux distributions. The print command is typically "
	"$$lpr \\% s$. By changing the print command (with @@PostScript settings...@), "
	"you can change it to something fancier. For instance, if you want to save the woods "
	"and print two pages on one sheet of paper, you change it to $$cat \\% s | mpage -2 -o -f -m0 | lpr$.")
ENTRY (U"2. Indirect printing with your word processor")
NORMAL (U"If you save your picture to an EPS file, you will be able to include it as a picture in your favourite "
	"word processor (Microsoft^\\re Word^\\tm, LaTeX...). See @@Save as EPS file...@.")
NORMAL (U"On the Mac it is better to save your picture to a PDF file, which both Microsoft^\\re Word^\\tm (when using the ##.docx# format) and LaTeX can process. "
	"See @@Save as PDF file...@.")
ENTRY (U"3. Indirect printing through the clipboard")
NORMAL (U"On Windows, pictures included in your word processor via @@Copy to clipboard@ or @@Save as Windows metafile...@ "
	"will print fine, though not as nicely as EPS files.")
NORMAL (U"On the Mac, pictures included in your word processor (post-2006 versions) via @@Copy to clipboard@ "
	"will print just as nicely as PDF files (use the ##.docx# format in Microsoft Word).")
MAN_END

MAN_BEGIN (U"Read from Praat picture file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the File menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To read a picture that you saved earlier "
	"with @@Save as Praat picture file...@.")
ENTRY (U"Behaviour")
NORMAL (U"The picture will be drawn across whatever is currently visible in the Picture window.")
ENTRY (U"Usage")
NORMAL (U"With the help of this command, you can transfer a picture from a Unix machine to a Macintosh. "
	"Praat for Macintosh can write the picture to an @@Encapsulated PostScript@ file "
	"with a screen preview.")
MAN_END

MAN_BEGIN (U"Text...", U"ppgb", 19970330)
INTRO (U"One of the commands in the #World menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To write text inside the drawing area.")
ENTRY (U"Scope")
NORMAL (U"This works with all the drawings that leave @margins around themselves.")
ENTRY (U"Settings")
TAG (U"%x")
DEFINITION (U"horizontal position, expressed in the horizontal domain of your latest drawing.")
TAG (U"%y")
DEFINITION (U"vertical position, expressed in the vertical range or domain of your latest drawing.")
TAG (U"%%Horizontal alignment")
DEFINITION (U"determines the horizontal alignment of the text relative to %x.")
TAG (U"%%Vertical alignment")
DEFINITION (U"determines the vertical alignment of the text relative to %y.")
TAG (U"%%Text")
DEFINITION (U"will be drawn in the current font and font size "
	"that you set with the @@Font menu@.")
ENTRY (U"Usage")
NORMAL (U"With the ##Text...# command, you can use all @@special symbols@ and @@text styles@.")
MAN_END

MAN_BEGIN (U"Insert picture from file...", U"ppgb", 20140608)
INTRO (U"A command in the #World menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To draw a picture file (PNG; on Mac and Windows also JPEG or TIFF; "
	"other picture formats may also work) into the Picture window (or into the Demo window).")
ENTRY (U"Settings")
TAG (U"##File name")
DEFINITION (U"the name of the picture file. If you use this command in a script, "
	"you can use a relative path name such as $$pictures/myface.png$ or $$~/Desktop/hello.png$.")
TAG (U"##From x")
TAG (U"##To x")
DEFINITION (U"The horizontal location (in world coordinates) where the picture will appear. "
	"Use the @@Axes...@ command to set world coordinates if they have not been set implicitly by an earlier #Draw or #Paint command. "
	"If you set ##From x# and ##To x# to the same value, the picture will be horizontally centred around this %x value "
	"and obtain a width that preserves the aspect ratio (width-to-height ratio) of the original picture.")
TAG (U"##From y")
TAG (U"##To y")
DEFINITION (U"The vertical location (in world coordinates) where the picture will appear. "
	"Use the @@Axes...@ command to set world coordinates if they have not been set implicitly by an earlier #Draw or #Paint command. "
	"If you set ##From y# and ##To y# to the same value, the picture will be vertically centred around this %y value "
	"and obtain a height that preserves the aspect ratio (width-to-height ratio) of the original picture.")
ENTRY (U"Behaviour")
NORMAL (U"From the description above, you see that if ##From x# is unequal to ##To x# and ##From y# is unequal to ##To y#, "
	"the picture will probably obtain an aspect ratio different from the original picture. "
	"To preserve the aspect ratio, either make ##To x# equal to ##From x# or make ##To y# equal to ##From y#.")
NORMAL (U"To show the picture on the same number of pixels as the original, make ##To x# equal to ##From x# and ##To y# equal to ##From y#. "
	"Praat will then show the picture with the original width and height (in pixels), centred around the given %x and %y values.")
ENTRY (U"Usage")
NORMAL (U"You will usually use this in a script, and often in a script that runs the @@Demo window@.")
MAN_END

MAN_BEGIN (U"Text left/right/top/bottom...", U"ppgb", 19970330)
INTRO (U"Four of the commands in the #Margins menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To write text into the @margins around the drawing area.")
ENTRY (U"Behaviour")
NORMAL (U"The text will be centred along the side. "
	"Text at the left or right will be turned by 90 degrees "
	"and written up and down, respectively.")
MAN_END

MAN_BEGIN (U"Undo", U"ppgb", 20091215)
INTRO (U"One of the commands in the #Edit menu of the @@Picture window@.")
NORMAL (U"It erases your most recently created drawing, which could have come from a command in the Objects window "
	"or from one of the drawing commands in the World and @Margins menus.")
ENTRY (U"Behaviour")
NORMAL (U"This command will erase some drawings, but it will not change your settings in the #Pen and #Font menus "
	"(line type, line width, font, font size, and @colour).")
NORMAL (U"The world window will be what it was after the latest-but-one drawing, "
	"so that you can use the @Margins menu as if the latest drawing had never happened.")
MAN_END

MAN_BEGIN (U"Select inner viewport...", U"ppgb", 20041108)
INTRO (U"One of the commands in the #Select menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To determine where your next drawing will occur.")
ENTRY (U"The viewport")
NORMAL (U"The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"Its margins are pink.")
NORMAL (U"The %inner viewport does not include the margins, the %outer viewport does (see @@Select outer viewport...@).")
NORMAL (U"Normally, you select the viewport by dragging your mouse across the Picture window. "
	"However, you would use this explicit command:")
LIST_ITEM (U"\\bu from a script;")
LIST_ITEM (U"\\bu if you want a viewport that cannot be expressed in halves of an inch.")
MAN_END

MAN_BEGIN (U"Select outer viewport...", U"ppgb", 20041108)
INTRO (U"One of the commands in the #Select menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To determine where your next drawing will occur.")
ENTRY (U"The viewport")
NORMAL (U"The \"viewport\" is the part of the Picture window where your next drawing will occur. "
	"Its margins are pink.")
NORMAL (U"The %outer viewport includes the margins, the %inner viewport does not (see @@Select inner viewport...@).")
NORMAL (U"Normally, you select the viewport by dragging your mouse across the Picture window. "
	"However, you would use this explicit command:")
LIST_ITEM (U"\\bu from a script;")
LIST_ITEM (U"\\bu if you want a viewport that cannot be expressed in halves of an inch.")
MAN_END

MAN_BEGIN (U"Viewport text...", U"ppgb", 19970330)
INTRO (U"One of the commands in the #Select menu of the @@Picture window@.")
ENTRY (U"Purpose")
NORMAL (U"To write text inside the viewport, at nine different places, "
	"with a rotation between 0 to 360 degrees.")
ENTRY (U"Settings:")
TAG (U"##Horizontal alignment")
DEFINITION (U"determines the horizontal alignment of the text:")
LIST_ITEM (U"    \\bu #Left means pushed against the left edge of the viewport;")
LIST_ITEM (U"    \\bu #Right means pushed against the right edge of the viewport;")
LIST_ITEM (U"    \\bu #Centre means horizontally centred in the viewport.")
TAG (U"##Vertical alignment")
DEFINITION (U"determines the vertical alignment of the text:")
LIST_ITEM (U"    \\bu #Top means pushed against the top of the viewport;")
LIST_ITEM (U"    \\bu #Bottom means pushed against the bottom of the viewport;")
LIST_ITEM (U"    \\bu #Half means vertically centred in the viewport.")
TAG (U"##Text")
DEFINITION (U"will be drawn in the current font and font size "
	"that you set with the @@Font menu@.")
ENTRY (U"Behaviour")
NORMAL (U"For rotated text, the alignment settings will not only determine "
	"the position inside the viewport, "
	"but also the alignment in the rotated coordinate system. "
	"This gives surprises now and then; "
	"so, if you want several rotated texts that align with each other, "
	"you should do this by varying the viewport, not the alignment.")
ENTRY (U"Usage")
NORMAL (U"You can use all @@special symbols@ and @@text styles@.")
MAN_END

MAN_BEGIN (U"Save as EPS file...", U"ppgb", 20140325)
INTRO (U"A command in the File menu of the @@Picture window@.")
NORMAL (U"It saves the picture to an @@Encapsulated PostScript@ (EPS) file, "
	"which can be imported by many other programs, such as Microsoft^\\re Word^\\tm.")
ENTRY (U"Usage")
NORMAL (U"EPS files are on the way out, mainly because they do not really support international text. "
	"On Macintosh and Linux, it is almost always better to use @@Save as PDF file...@ instead, "
	"or (on the Mac) to use @@Copy to clipboard@. On Windows, which does not really support PDF files yet, "
	"you may find that EPS files sometimes creates better quality than @@Copy to clipboard@; "
	"you should also check out @@Save as PNG file...@ on Windows. "
	"To import an EPS file in Word, choose #Insert \\-> #Picture \\-> ##From file...#. ")
ENTRY (U"Behaviour")
NORMAL (U"Though all the contents of the Picture window are written to the EPS file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in Word (or another program).")
ENTRY (U"Settings")
NORMAL (U"The EPS picture is saved with the grey resolution and fonts that you specified with @@PostScript settings...@.")
MAN_END

MAN_BEGIN (U"Save as PDF file...", U"ppgb", 20140325)
INTRO (U"A command in the File menu of the @@Picture window@, on Macintosh and Linux.")
NORMAL (U"It saves the picture to a PDF file, "
	"which can be imported by several other programs, such as modern versions of Microsoft^\\re Word\\tm.")
ENTRY (U"PDF means highest possible quality")
NORMAL (U"With PDF pictures you can use high-quality graphics in your word-processor documents. "
	"On the Mac, the quality is the same as if you use @@Copy to clipboard@.")
NORMAL (U"On Windows, use @@Save as PNG file...@ or @@Save as EPS file...@ instead.")
ENTRY (U"Behaviour")
NORMAL (U"Though all the contents of the Picture window are written to the PDF file, "
	"only the part that you selected in the Picture window (the %viewport) will become visible in Word (or another program).")
ENTRY (U"Usage")
NORMAL (U"To import a PDF file in Word, choose #Insert \\-> #Picture \\-> ##From file...#. "
	"Word will create a picture with the same size as the originally selected part of the Picture window (the %viewport).")
MAN_END

MAN_BEGIN (U"Save as PNG file...", U"ppgb", 20140325)
INTRO (U"A command in the File menu of the @@Picture window@, on all platforms.")
NORMAL (U"It saves the picture to a PNG (\"ping\") image file, "
	"which can be imported by several other programs, such as Microsoft^\\re Word\\tm. "
	"For the resolution you can choose between 600 dots per inch (very good quality even when printed) "
	"and 300 dpi (enough for all web sites, even on retina displays).")
ENTRY (U"Usage in text processors")
NORMAL (U"On Windows, PNG files may sometimes have the best quality that you can get, "
	"although you should also try @@Save as EPS file...@ and @@Copy to clipboard@. "
	"On Macintosh or Linux, @@Save as PDF file...@ or @@Copy to clipboard@ is almost always better.")
ENTRY (U"Usage for publication")
NORMAL (U"Some publishers do not accept PDF pictures. In such a case, "
	"they may accept 600-dpi or 300-dpi PNG pictures. If they accept TIFF pictures only, "
	"then you can easily convert your PNG picture to a TIFF picture with any graphics converter program.")
ENTRY (U"Behaviour")
NORMAL (U"Only the contents of the part of the Picture window that you selected (the %viewport) "
	"are written to the PNG file.")
ENTRY (U"Usage")
NORMAL (U"To import a PNG file in Word, choose #Insert \\-> #Picture \\-> ##From file...#. "
	"Word will create a picture with the same size as the originally selected part of the Picture window (the %viewport).")
MAN_END

MAN_BEGIN (U"Save as Praat picture file...", U"ppgb", 20110129)
INTRO (U"A command in the File menu of the @@Picture window@.")
NORMAL (U"It saves a picture in a format that can be imported into Praat later with @@Read from Praat picture file...@.")
ENTRY (U"Usage")
NORMAL (U"With the help of this command, you can transfer the contents of the picture window between computers or even between platforms, "
	"for instance from a Macintosh to a Windows computer.")
MAN_END

MAN_BEGIN (U"Save as Windows metafile...", U"ppgb", 20140325)
INTRO (U"A command in the File menu of the @@Picture window@, if you are on Windows.")
NORMAL (U"It saves the selected part of the picture in an \"enhanced metafile\" (.EMF) format, "
	"which can be imported by many Windows programs, like Adobe^\\re Illustrator^\\tm or Microsoft^\\re Word^\\tm.")
ENTRY (U"Behaviour")
NORMAL (U"Though all the picture data will be written to the file, "
	"only the part that you selected in the Picture window (the %viewport) may become visible in the other program.")
ENTRY (U"Usage")
NORMAL (U"You will not use this command very often, "
	"because it is usually easier to copy the selection to the clipboard with the @@Copy to clipboard@ command, "
	"and `Paste' it into the other program. You may use a metafile instead of the clipboard if the clipboard is too large "
	"for the other program to read, or if you want to transfer the picture to another computer.")
MAN_END

}

/* End of file manual_Picture.cpp */
