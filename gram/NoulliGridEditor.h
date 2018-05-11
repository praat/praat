#ifndef _NoulliGridEditor_h_
#define _NoulliGridEditor_h_
/* NoulliGridEditor.h
 *
 * Copyright (C) 2018 Paul Boersma
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

#include "TimeSoundEditor.h"
#include "NoulliGrid.h"

Thing_define (NoulliGridEditor, TimeSoundEditor) {
	void v_draw ()
		override;
	void v_play (double tmin, double tmax)
		override;
	void v_drawSelectionViewer ()
		override;
	void v_drawRealTimeSelectionViewer (int phase, double time)
		override;
};

void NoulliGridEditor_init (NoulliGridEditor me, const char32 *title, NoulliGrid data, Sound sound, bool ownSound);
autoNoulliGridEditor NoulliGridEditor_create (const char32 *title, NoulliGrid grid, Sound sound, bool ownSound);
/*
	`sound` may be null;
	if `ownSound` is `true`, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file NoulliGridEditor.h */
#endif
