#ifndef _UiPause_h_
#define _UiPause_h_
/* UiPause.h
 *
 * Copyright (C) 2009-2011,2012,2015 Paul Boersma
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

#include "Gui.h"
Thing_declare (Interpreter);

void UiPause_begin (GuiWindow topShell, const char32 *title, Interpreter interpreter);

void UiPause_real       (const char32 *label, const char32 *defaultValue);
void UiPause_positive   (const char32 *label, const char32 *defaultValue);
void UiPause_integer    (const char32 *label, const char32 *defaultValue);
void UiPause_natural    (const char32 *label, const char32 *defaultValue);
void UiPause_word       (const char32 *label, const char32 *defaultValue);
void UiPause_sentence   (const char32 *label, const char32 *defaultValue);
void UiPause_text       (const char32 *label, const char32 *defaultValue);
void UiPause_boolean    (const char32 *label, int defaultValue);
void UiPause_choice     (const char32 *label, int defaultValue);
void UiPause_optionMenu (const char32 *label, int defaultValue);
void UiPause_option     (const char32 *label);
void UiPause_comment    (const char32 *label);

int UiPause_end (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const char32 *continueText1, const char32 *continueText2, const char32 *continueText3,
	const char32 *continueText4, const char32 *continueText5, const char32 *continueText6,
	const char32 *continueText7, const char32 *continueText8, const char32 *continueText9,
	const char32 *continueText10, Interpreter interpreter);

/* End of file UiPause.h */
#endif
