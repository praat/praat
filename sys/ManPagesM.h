#ifndef _ManPagesM_h_
#define _ManPagesM_h_
/* ManPagesM.h
 *
 * Copyright (C) 1996-2005,2007,2009,2011,2014-2018 Paul Boersma
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

/* ManPages macros. */

#ifndef _ManPages_h_
	#include "ManPages.h"
#endif

#define MAN_BEGIN(t,a,d)  { conststring32 title = t, author = a; integer date = d; \
	static struct structManPage_Paragraph page [] = {
#define INTRO(text)  { kManPage_type::INTRO, text, 0.0, 0.0, nullptr },
#define ENTRY(text)  { kManPage_type::ENTRY, text, 0.0, 0.0, nullptr },
#define NORMAL(text)  { kManPage_type::NORMAL, text, 0.0, 0.0, nullptr },
#define LIST_ITEM(text)  { kManPage_type::LIST_ITEM, text, 0.0, 0.0, nullptr },
#define LIST_ITEM1(text)  { kManPage_type::LIST_ITEM1, text, 0.0, 0.0, nullptr },
#define LIST_ITEM2(text)  { kManPage_type::LIST_ITEM2, text, 0.0, 0.0, nullptr },
#define LIST_ITEM3(text)  { kManPage_type::LIST_ITEM3, text, 0.0, 0.0, nullptr },
#define TAG(text)  { kManPage_type::TAG, text, 0.0, 0.0, nullptr },
#define TAG1(text)  { kManPage_type::TAG1, text, 0.0, 0.0, nullptr },
#define TAG2(text)  { kManPage_type::TAG2, text, 0.0, 0.0, nullptr },
#define TAG3(text)  { kManPage_type::TAG3, text, 0.0, 0.0, nullptr },
#define DEFINITION(text)  { kManPage_type::DEFINITION, text, 0.0, 0.0, nullptr },
#define DEFINITION1(text)  { kManPage_type::DEFINITION1, text, 0.0, 0.0, nullptr },
#define DEFINITION2(text)  { kManPage_type::DEFINITION2, text, 0.0, 0.0, nullptr },
#define DEFINITION3(text)  { kManPage_type::DEFINITION3, text, 0.0, 0.0, nullptr },
#define CODE(text)  { kManPage_type::CODE, text, 0.0, 0.0, nullptr },
#define CODE1(text)  { kManPage_type::CODE1, text, 0.0, 0.0, nullptr },
#define CODE2(text)  { kManPage_type::CODE2, text, 0.0, 0.0, nullptr },
#define CODE3(text)  { kManPage_type::CODE3, text, 0.0, 0.0, nullptr },
#define CODE4(text)  { kManPage_type::CODE4, text, 0.0, 0.0, nullptr },
#define CODE5(text)  { kManPage_type::CODE5, text, 0.0, 0.0, nullptr },
#define PROTOTYPE(text)  { kManPage_type::PROTOTYPE, text, 0.0, 0.0, nullptr },
#define FORMULA(text)  { kManPage_type::FORMULA, text, 0.0, 0.0, nullptr },
#define PICTURE(width,height,draw)  { kManPage_type::PICTURE, nullptr, width, height, draw },
#define SCRIPT(width,height,text)  { kManPage_type::SCRIPT, text, width, height, nullptr },
#define MAN_END  { } }; ManPages_addPage (me, title, author, date, page); }

#define Manual_DRAW_WINDOW(height,title,menu) \
	"Select inner viewport... 0.2 5.8 0.2 " #height "-0.2\n" \
	"Axes... 0 100*5.6 100*(" #height "-0.4) 0\n" \
	"Paint rectangle... 0.8 0 560 0 30\n" \
	"Paint circle... {1,0.5,0.5} 15 15 8\n" \
	"Draw line... 15-5 10 15+5 20\n" \
	"Draw line... 15+5 10 15-5 20\n" \
	"Paint circle... {1,1,0.25} 40 15 8\n" \
	"Draw line... 40-7 15 40+7 15\n" \
	"Paint circle... {0.25,1,0.25} 65 15 8\n" \
	"Draw rectangle... 65-5 65+5 15-5 15+5\n" \
	"Helvetica\n" \
	"Text... 280 centre 15 half " title "\n" \
	"Paint rectangle... 0.9 0 560 30 60\n" \
	"Text... 5 left 45 half " menu "\n" \
	"Draw line... 0 30 560 30\n" \
	"info$ = Picture info\n" \
	"fontSize = extractNumber (info$, \"Font size: \")\n"

#define Manual_SETTINGS_WINDOW_HEIGHT(numberOfVerticalFields)  1.4+numberOfVerticalFields*0.4
#define Manual_DRAW_SETTINGS_WINDOW(title,numberOfVerticalFields) \
	"Select inner viewport... 0.1 5.3 0.2 1.2+" #numberOfVerticalFields "*0.4\n" \
	"height = 100*(1+" #numberOfVerticalFields "*0.4)\n" \
	"Axes... 0 100*5.2 height 0\n" \
	"Paint rectangle... 0.8 0 520 0 30\n" \
	"Paint circle... {1,0.5,0.5} 15 15 8\n" \
	"Draw line... 15-5 10 15+5 20\n" \
	"Draw line... 15+5 10 15-5 20\n" \
	"Helvetica\n" \
	"Text... 260 centre 15 half " title "\n" \
	"Paint rectangle... 0.9 0 520 30 height\n" \
	"Draw line... 0 30 520 30\n" \
	"buttonColour$ = \"0.95\"\n" \
	"Paint rounded rectangle... 'buttonColour$' 15 85 height-10 height-34 2.0\n" \
	"Draw rounded rectangle... 15 85 height-10 height-34 2.0\n" \
	"Text... 50 centre height-21 half Help\n" \
	"Paint rounded rectangle... 'buttonColour$' 95 195 height-10 height-34 2.0\n" \
	"Draw rounded rectangle... 95 195 height-10 height-34 2.0\n" \
	"Text... 145 centre height-21 half Standards\n" \
	"Paint rounded rectangle... 'buttonColour$' 275 345 height-10 height-34 2.0\n" \
	"Draw rounded rectangle... 275 345 height-10 height-34 2.0\n" \
	"Text... 310 centre height-21 half Cancel\n" \
	"Paint rounded rectangle... 'buttonColour$' 355 425 height-10 height-34 2.0\n" \
	"Draw rounded rectangle... 355 425 height-10 height-34 2.0\n" \
	"Text... 390 centre height-21 half Apply\n" \
	"Line width... 2\n" \
	"Paint rounded rectangle... {0.8,0.8,1} 435 505 height-10 height-34 2.0\n" \
	"Draw rounded rectangle... 435 505 height-10 height-34 2.0\n" \
	"Line width... 1\n" \
	"Text... 470 centre height-21 half OK\n" \
	"Draw rectangle... 0 520 0 height\n" \
	"info$ = Picture info\n" \
	"fontSize = extractNumber (info$, \"Font size: \")\n" \
	"y = 55\n"

#define Manual_DRAW_SETTINGS_WINDOW_FIELD(label,text) \
	"Text... 255 right y half " label ":\n" \
	"Paint rectangle... white 265 505 y-12 y+12\n" \
	"Draw rectangle... 265 505 y-12 y+12\n" \
	";Courier\n" \
	"Text... 265 left y half " text "\n" \
	"Helvetica\n" \
	"y += 40\n"
#define Manual_DRAW_SETTINGS_WINDOW_RANGE(label,text1,text2) \
	"Text... 255 right y half " label ":\n" \
	"Paint rectangle... white 265 370 y-12 y+12\n" \
	"Draw rectangle... 265 370 y-12 y+12\n" \
	"Paint rectangle... white 380 505 y-12 y+12\n" \
	"Draw rectangle... 380 505 y-12 y+12\n" \
	";Courier\n" \
	"Text... 265 left y half " text1 "\n" \
	"Text... 380 left y half " text2 "\n" \
	"Helvetica\n" \
	"y += 40\n"
#define Manual_DRAW_SETTINGS_WINDOW_BOOLEAN(label,on) \
	"if " #on "\n" \
	"    Paint rectangle... yellow 265 279 y-7 y+7\n" \
	"    Text special... 272 centre y half Times fontSize*1.2 0 ##√\n" \
	"else\n" \
	"    Paint rectangle... white 265 279 y-7 y+7\n" \
	"endif\n" \
	"Draw rectangle... 265 279 y-7 y+7\n" \
	"Text... 281 left y half " label "\n" \
	"y += 40\n"
#define Manual_DRAW_SETTINGS_WINDOW_RADIO(label,text,on) \
	"if \"" label "\" <> \"\"\n" \
	"    Text... 255 right y half " label ":\n" \
	"endif\n" \
	"if " #on "\n" \
	"    Paint circle... yellow 272 y 7\n" \
	"    Paint circle... black 272 y 3\n" \
	"else\n" \
	"    Paint circle... white 272 y 7\n" \
	"endif\n" \
	"Draw circle... 272 y 7\n" \
	"Text... 281 left y half " text "\n" \
	"y += 40\n"
#define Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU(label,text) \
	"Text... 255 right y half " label ":\n" \
	"Paint rounded rectangle... 'buttonColour$' 265 505 y-12 y+12 1.0\n" \
	"Draw rounded rectangle... 265 505 y-12 y+12 2.0\n" \
	"Text... 270 left y half " text "\n" \
	"Helvetica\n" \
	"y += 40\n"
#define Manual_DRAW_SETTINGS_WINDOW_TEXT(label,text) \
	"Text... 12 left y half " label ":\n" \
	"y += 24\n" \
	"Paint rectangle... white 15 505 y-12 y+12\n" \
	"Draw rectangle... 15 505 y-12 y+12\n" \
	";Courier\n" \
	"Text... 15 left y half " text "\n" \
	"Helvetica\n" \
	"y += 40\n"

/* End of file ManPagesM.h */
#endif
