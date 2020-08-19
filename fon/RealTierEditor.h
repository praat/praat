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
#include "RealTier.h"

Thing_define (FunctionView, Thing) {
	FunctionEditor editor;
	double ymin_fraction, ymax_fraction;
	Graphics graphics() const { return our editor -> graphics.get(); }
	double startWindow() const { return our editor -> startWindow; }
	double endWindow() const { return our editor -> endWindow; }
	double startSelection() const { return our editor -> startSelection; }
	double endSelection() const { return our editor -> endSelection; }
	void setViewport() const {
		Graphics_setViewport (our graphics(),
				our editor -> dataLeft_pxlt(), our editor -> dataRight_pxlt(), our editor -> dataBottom_pxlt(), our editor -> dataTop_pxlt());
		Graphics_insetViewport (our graphics(), 0.0, 1.0, our ymin_fraction, our ymax_fraction);
	}
};

inline static void FunctionView_init (FunctionView me, FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	my editor = editor;
	my ymin_fraction = ymin_fraction;
	my ymax_fraction = ymax_fraction;
}

Thing_define (RealTierView, FunctionView) {
	virtual double v_minimumLegalValue () { return undefined; }
	virtual double v_maximumLegalValue () { return undefined; }
	virtual conststring32 v_rightTickUnits () { return U""; }
	virtual double v_defaultYmin () { return 0.0; }
	virtual double v_defaultYmax () { return 1.0; }

	double ymin, ymax, ycursor;
	double anchorTime = undefined, anchorY;
	bool draggingSelection;
	double dt = 0.0, dy = 0.0;
	integer firstSelected, lastSelected;

	void viewRealTierAsWorldByWorld () const {
		our setViewport ();
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	}
};

Thing_define (RealTierEditor, TimeSoundEditor) {
	autoRealTierView view;
	RealTier tier() { return static_cast <RealTier> (our data); }
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

void RealTierEditor_init (RealTierEditor me, ClassInfo viewClass, conststring32 title, RealTier data, Sound sound, bool ownSound);
/*
	`sound` may be null;
	if `ownSound` is `true`, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file RealTierEditor.h */
#endif
