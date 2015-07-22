/* Graphics_enums.h
 *
 * Copyright (C) 1992-2007,2013,2014,2015 Paul Boersma
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

enums_begin (kGraphics_font, 0)
	enums_add (kGraphics_font, 0, HELVETICA, U"Helvetica")
	enums_add (kGraphics_font, 1, TIMES, U"Times")
	enums_add (kGraphics_font, 2, COURIER, U"Courier")
	enums_add (kGraphics_font, 3, PALATINO, U"Palatino")
enums_end (kGraphics_font, 3, TIMES)

enums_begin (kGraphics_horizontalAlignment, 0)
	enums_add (kGraphics_horizontalAlignment, 0, LEFT, U"left")
	enums_add (kGraphics_horizontalAlignment, 1, CENTRE, U"centre")
	enums_alt (kGraphics_horizontalAlignment,    CENTRE, U"center")
	enums_add (kGraphics_horizontalAlignment, 2, RIGHT, U"right")
enums_end (kGraphics_horizontalAlignment, 2, CENTRE)

enums_begin (kGraphics_resolution, 0)
	enums_add (kGraphics_resolution, 0,  96, U"96 dpi")
	enums_add (kGraphics_resolution, 1, 100, U"100 dpi")
	enums_add (kGraphics_resolution, 2, 180, U"180 dpi")
	enums_add (kGraphics_resolution, 3, 200, U"200 dpi")
	enums_add (kGraphics_resolution, 4, 300, U"300 dpi")
	enums_add (kGraphics_resolution, 5, 360, U"360 dpi")
	enums_add (kGraphics_resolution, 6, 600, U"600 dpi")
	enums_add (kGraphics_resolution, 7, 720, U"720 dpi")
	enums_add (kGraphics_resolution, 8, 900, U"900 dpi")
	enums_add (kGraphics_resolution, 9, 1200, U"1200 dpi")
enums_end (kGraphics_resolution, 9, 100)

enums_begin (kGraphics_colourScale, 0)
	enums_add (kGraphics_colourScale, 0, GREY, U"grey")
	enums_add (kGraphics_colourScale, 1, BLUE_TO_RED, U"blue to red")
enums_end (kGraphics_colourScale, 1, GREY)

enums_begin (kGraphicsPostscript_spots, 0)
	enums_add (kGraphicsPostscript_spots, 0, FINE, U"finest")
	enums_add (kGraphicsPostscript_spots, 1, PHOTOCOPYABLE, U"photocopyable")
enums_end (kGraphicsPostscript_spots, 1, FINE)

enums_begin (kGraphicsPostscript_paperSize, 0)
	enums_add (kGraphicsPostscript_paperSize, 0, A4, U"A4")
	enums_add (kGraphicsPostscript_paperSize, 1, A3, U"A3")
	enums_add (kGraphicsPostscript_paperSize, 2, US_LETTER, U"US Letter")
enums_end (kGraphicsPostscript_paperSize, 2, A4)

enums_begin (kGraphicsPostscript_orientation, 0)
	enums_add (kGraphicsPostscript_orientation, 0, PORTRAIT, U"portrait")
	enums_add (kGraphicsPostscript_orientation, 1, LANDSCAPE, U"landscape")
enums_end (kGraphicsPostscript_orientation, 1, PORTRAIT)

enums_begin (kGraphicsPostscript_fontChoiceStrategy, 0)
	enums_add (kGraphicsPostscript_fontChoiceStrategy, 0, AUTOMATIC, U"automatic")
	enums_add (kGraphicsPostscript_fontChoiceStrategy, 1, LINOTYPE, U"Linotype")
	enums_add (kGraphicsPostscript_fontChoiceStrategy, 2, MONOTYPE, U"Monotype")
	enums_add (kGraphicsPostscript_fontChoiceStrategy, 3, PS_MONOTYPE, U"PS Monotype")
enums_end (kGraphicsPostscript_fontChoiceStrategy, 3, AUTOMATIC)

enums_begin (kGraphics_cjkFontStyle, 0)
	enums_add (kGraphics_cjkFontStyle, 0, CHINESE, U"Chinese")
	enums_add (kGraphics_cjkFontStyle, 1, JAPANESE, U"Japanese")
enums_end (kGraphics_cjkFontStyle, 1, CHINESE)

/* End of file Graphics_enums.h */
