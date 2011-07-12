#ifndef _UiPause_h_
#define _UiPause_h_
/* UiPause.h
 *
 * Copyright (C) 2009-2011 Paul Boersma
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
 * pb 2011/07/12
 */

#include "Gui.h"
#include "Interpreter_decl.h"

void UiPause_begin (GuiObject topShell, const wchar *title, Interpreter interpreter);

void UiPause_real (const wchar *label, const wchar *defaultValue);
void UiPause_positive (const wchar *label, const wchar *defaultValue);
void UiPause_integer (const wchar *label, const wchar *defaultValue);
void UiPause_natural (const wchar *label, const wchar *defaultValue);
void UiPause_word (const wchar *label, const wchar *defaultValue);
void UiPause_sentence (const wchar *label, const wchar *defaultValue);
void UiPause_text (const wchar *label, const wchar *defaultValue);
void UiPause_boolean (const wchar *label, int defaultValue);
void UiPause_choice (const wchar *label, int defaultValue);
void UiPause_optionMenu (const wchar *label, int defaultValue);
void UiPause_option (const wchar *label);
void UiPause_comment (const wchar *label);

int UiPause_end (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const wchar *continueText1, const wchar *continueText2, const wchar *continueText3,
	const wchar *continueText4, const wchar *continueText5, const wchar *continueText6,
	const wchar *continueText7, const wchar *continueText8, const wchar *continueText9,
	const wchar *continueText10, Interpreter interpreter);

/* End of file UiPause.h */
#endif
