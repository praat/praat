#ifndef _ManPage_h_
#define _ManPage_h_
/* ManPage.h
 *
 * Copyright (C) 1996-2008,2011,2012,2014-2020,2023 Paul Boersma
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

#include "Graphics.h"

#include "ManPage_enums.h"

typedef struct structManPage_Paragraph {
	enum kManPage_type type;
	conststring32 text;   // not an autostring32, because it could be a string literal (if not dynamic)
	double width, height;   // for machine-code pictures and script chunks with pictures
	void (*draw) (Graphics g);   // for machine-code pictures
	autoGraphics cacheGraphics;   // for script chunks with pictures
	autoMelderString cacheInfo;   // for script chunks with info text
	void invalidateCache () noexcept {
		our cacheGraphics. reset();
	}
} *ManPage_Paragraph;

Thing_define (ManPage, Thing) {
	autostring32 title, copyright;
	autovector <structManPage_Paragraph> paragraphs;
	autoINTVEC linksHither, linksThither;
	double recordingTime;
	void invalidateCache () noexcept {
		for (integer i = 1; i <= our paragraphs. size; i ++)
			our paragraphs [i]. invalidateCache ();
	}
};

Thing_declare (PraatApplication);
Thing_declare (PraatObjects);
Thing_declare (PraatPicture);

void ManPage_runAllChunksToCache (
	ManPage me,
	const kGraphics_font font,
	const double fontSize,
	PraatApplication praatApplication,
	PraatObjects praatObjects,
	PraatPicture praatPicture,
	MelderDir rootDirectory
);

/* End of file ManPage.h */
#endif
