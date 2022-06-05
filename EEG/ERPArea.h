#ifndef _ERPArea_h_
#define _ERPArea_h_
/* EEGArea.h
 *
 * Copyright (C) 2022 Paul Boersma
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

#include "SoundArea.h"
#include "ERP.h"

Thing_define (ERPArea, SoundArea) {
	ERP erp;

	virtual conststring32 v_rightTickUnits () { return U"V"; }

	conststring32 v_getChannelName (integer /* channelNumber */)
		override;

	#include "ERPArea_prefs.h"
};

inline void ERPArea_init (ERPArea me, FunctionEditor editor) {
	FunctionArea_init (me, editor);
}

inline autoERPArea ERPArea_create (FunctionEditor editor) {
	autoERPArea me = Thing_new (ERPArea);
	ERPArea_init (me.get(), editor);
	return me;
}

/* End of file ERPArea.h */
#endif
