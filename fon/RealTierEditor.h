#ifndef _RealTierEditor_h_
#define _RealTierEditor_h_
/* RealTierEditor.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015-2018,2020 Paul Boersma
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
#include "RealTierArea.h"

Thing_define (RealTierEditor, TimeSoundEditor) {
	/*
		Access inherited attributes by their derived type.
	*/
	RealTier & realTier() { return * reinterpret_cast <RealTier *> (& our data); }

	autoRealTierArea realTierArea;
	constexpr static double SOUND_HEIGHT = 0.382;

	void v_createMenus ()
		override;
	void v_dataChanged ()
		override;
	void v_draw ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction)
		override;
	void v_play (double tmin, double tmax)
		override;
	void v_createMenuItems_view (EditorMenu menu)
		override;

	virtual conststring32 v_quantityText () { return U"Y"; }   // normally includes units
	virtual conststring32 v_setRangeTitle () { return U"Set range..."; }
	virtual conststring32 v_defaultYminText () { return U"0.0"; }
	virtual conststring32 v_defaultYmaxText () { return U"1.0"; }
	virtual conststring32 v_yminText () { return U"Minimum"; }   // normally includes units
	virtual conststring32 v_ymaxText () { return U"Maximum"; }   // normally includes units
};

void RealTierEditor_updateScaling (RealTierEditor me);
/*
	Computes the ymin and ymax values on the basis of the data.
	Call after every change in the data.
*/

/*
	Of the following two, the first is better, because the second could do incomplete initialization.
*/
void RealTierEditor_init (RealTierEditor me, autoRealTierArea realTierArea, conststring32 title, RealTier data, Sound sound, bool ownSound);
void RealTierEditor_init (RealTierEditor me, ClassInfo realTierAreaClass, conststring32 title, RealTier data, Sound sound, bool ownSound);
/*
	`sound` may be null;
	if `ownSound` is `true`, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file RealTierEditor.h */
#endif
