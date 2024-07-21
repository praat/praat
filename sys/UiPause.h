#ifndef _UiPause_h_
#define _UiPause_h_
/* UiPause.h
 *
 * Copyright (C) 2009-2012,2015,2016,2018,2020,2022-2024 Paul Boersma
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

#include "Ui.h"

void UiPause_begin (GuiWindow topShell, Editor optionalPauseWindowOwningEditor, conststring32 title, Interpreter interpreter);

void UiPause_real           (conststring32 label, conststring32 defaultValue);
void UiPause_positive       (conststring32 label, conststring32 defaultValue);
void UiPause_integer        (conststring32 label, conststring32 defaultValue);
void UiPause_natural        (conststring32 label, conststring32 defaultValue);
void UiPause_word           (conststring32 label, conststring32 defaultValue);
void UiPause_sentence       (conststring32 label, conststring32 defaultValue);
void UiPause_text           (conststring32 label, conststring32 defaultValue, integer numberOfLines);
void UiPause_boolean        (conststring32 label, bool defaultValue);
void UiPause_infile         (conststring32 label, conststring32 defaultValue, integer numberOfLines);
void UiPause_outfile        (conststring32 label, conststring32 defaultValue, integer numberOfLines);
void UiPause_folder         (conststring32 label, conststring32 defaultValue, integer numberOfLines);
void UiPause_realvector     (conststring32 label, kUi_realVectorFormat defaultFormat, conststring32 defaultValue, integer numberOfLines);
void UiPause_positivevector (conststring32 label, kUi_realVectorFormat defaultFormat, conststring32 defaultValue, integer numberOfLines);
void UiPause_integervector  (conststring32 label, kUi_integerVectorFormat defaultFormat, conststring32 defaultValue, integer numberOfLines);
void UiPause_naturalvector  (conststring32 label, kUi_integerVectorFormat defaultFormat, conststring32 defaultValue, integer numberOfLines);
void UiPause_choice         (conststring32 label, int defaultValue);
void UiPause_optionmenu     (conststring32 label, int defaultValue);
void UiPause_option         (conststring32 label);
void UiPause_heading        (conststring32 label);
void UiPause_comment        (conststring32 label);
void UiPause_caption        (conststring32 label);

int UiPause_end (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	conststring32 continueText1, conststring32 continueText2, conststring32 continueText3,
	conststring32 continueText4, conststring32 continueText5, conststring32 continueText6,
	conststring32 continueText7, conststring32 continueText8, conststring32 continueText9,
	conststring32 continueText10, Interpreter interpreter);

/* End of file UiPause.h */
#endif
