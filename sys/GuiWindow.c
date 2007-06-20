/* GuiWindow.c
 *
 * Copyright (C) 1993-2006 Paul Boersma
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
 * pb 2004/01/07 this file separated from Gui.c
 * pb 2004/02/12 don't trust window modification feedback on MacOS 9
 * pb 2004/04/06 GuiWindow_drain separated from XmUpdateDisplay
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/06/19 wchar_t
 */

#include "GuiP.h"
#include "UnicodeData.h"

void GuiWindow_setTitleW (Widget me, const wchar_t *titleW) {
	#if mac
		unsigned long length = wcslen (titleW);
		UniChar *titleUtf16 = Melder_calloc (wcslen (titleW), sizeof (UniChar));
		for (unsigned long i = 0; i < length; i ++)
			titleUtf16 [i] = titleW [i];
		CFStringRef titleCF = CFStringCreateWithCharacters (NULL, titleUtf16, length);
		Melder_free (titleUtf16);
		SetWindowTitleWithCFString (my nat.window.ptr, titleCF);
		CFRelease (titleCF);
	#elif win
		SetWindowTextW (my window, titleW);
	#else
		char *titleA = Melder_peekWcsToAscii (titleW);   // BANDAID
		unsigned long length = strlen (titleA);
		for (unsigned long i = 0; i < length; i ++) {   // BANDAID
			if (titleW [i] == UNICODE_LEFT_DOUBLE_QUOTATION_MARK || titleW [i] == UNICODE_RIGHT_DOUBLE_QUOTATION_MARK)
			    titleA [i] = '\"';
		}
		XtVaSetValues (me, XmNtitle, titleA, XmNiconName, titleA, NULL);
	#endif
}

int GuiWindow_setDirty (Widget me, int dirty) {
	#if mac
		SetWindowModified (my nat.window.ptr, dirty);
		return 1;
	#else
		(void) me;
		(void) dirty;
		return 0;
	#endif
}

void GuiWindow_drain (Widget me) {
	#if mac
		QDFlushPortBuffer (GetWindowPort (my macWindow), NULL);
		/*
		 * The following TRICK cost me half a day to work out.
		 * It turns out that after a call to QDFlushPortBuffer (),
		 * it takes MacOS ages to compute a new dirty region while
		 * the next graphics commands are executed. Such a dirty region
		 * could well be the region that includes all the pixels drawn by
		 * the graphics commands, and nothing else. One can imagine
		 * that such a thing takes five seconds when the graphics is
		 * a simple Graphics_function () of e.g. noise.
		 */
		{
			static Rect bounds = { -32768, -32768, 32767, 32767 };
			QDAddRectToDirtyRegion (GetWindowPort (my macWindow), & bounds);
		}
	#else
		(void) me;
	#endif
}

/* End of file GuiWindow.c */
