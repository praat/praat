#ifndef _UiPause_h_
#define _UiPause_h_
/* UiPause.h
 *
 * Copyright (C) 2009 Paul Boersma
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
 * pb 2009/01/20
 */

#include "Gui.h"
#ifndef _Interpreter_decl_h_
	#include "Interpreter_decl.h"
#endif

int UiPause_begin (Widget topShell, const wchar_t *title, Interpreter interpreter);

int UiPause_real (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_positive (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_integer (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_natural (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_word (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_sentence (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_text (const wchar_t *label, const wchar_t *defaultValue);
int UiPause_boolean (const wchar_t *label, int defaultValue);
int UiPause_choice (const wchar_t *label, int defaultValue);
int UiPause_optionMenu (const wchar_t *label, int defaultValue);
int UiPause_option (const wchar_t *label);
int UiPause_comment (const wchar_t *label);

int UiPause_end (int numberOfContinueButtons, int defaultContinueButton,
	const wchar_t *continueText1, const wchar_t *continueText2, const wchar_t *continueText3,
	const wchar_t *continueText4, const wchar_t *continueText5, const wchar_t *continueText6,
	const wchar_t *continueText7, const wchar_t *continueText8, const wchar_t *continueText9,
	const wchar_t *continueText10, Interpreter interpreter);

#endif
/* End of file UiPause.h */
