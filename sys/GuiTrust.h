#ifndef _UiTrust_h_
#define _UiTrust_h_
/* UiTrust.h
 *
 * Copyright (C) 2024 Paul Boersma
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

integer GuiTrust_get (GuiWindow optionalParent, Editor optionalTrustWindowOwningEditor,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5,
	Interpreter interpreter);

GuiDialog GuiTrust_createDialog (GuiWindow optionalParent,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5);

/* End of file UiPause.h */
#endif
