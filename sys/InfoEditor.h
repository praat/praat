#ifndef _InfoEditor_h_
#define _InfoEditor_h_
/* InfoEditor.h
 *
 * Copyright (C) 2004-2005,2011,2015,2016,2018 Paul Boersma
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

#include "TextEditor.h"

Thing_define (InfoEditor, TextEditor) {
	void v_destroy () noexcept
		override;
	bool v_scriptable ()
		override { return false; }
	bool v_fileBased ()
		override { return false; }
	void v_clear ()
		override;
};

InfoEditor InfoEditor_getTheReferenceToTheOnlyInstance ();

void InfoEditor_injectInformationProc ();

/* End of file InfoEditor.h */
#endif
