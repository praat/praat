#ifndef _FunctionArea_h_
#define _FunctionArea_h_
/* FunctionArea.h
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

#include "FunctionEditor.h"

Thing_define (FunctionArea, Thing) {
	void init (FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
		_editor = editor;
		_ymin_fraction = ymin_fraction;
		_ymax_fraction = ymax_fraction;
	}
	double startWindow() const { return _editor -> startWindow; }
	double endWindow() const { return _editor -> endWindow; }
	double startSelection() const { return _editor -> startSelection; }
	double endSelection() const { return _editor -> endSelection; }
	double tmin() const { return _editor -> tmin; }
	double tmax() const { return _editor -> tmax; }
	Graphics graphics() const { return _editor -> graphics.get(); }
	void setViewport() const {
		Graphics_setViewport (our graphics(), our left_pxlt(), our right_pxlt(), our bottom_pxlt(), our top_pxlt());
	}
	void setSelection (double startSelection, double endSelection) {
		_editor -> startSelection = startSelection;
		_editor -> endSelection = endSelection;
	}
	bool defaultMouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
		_editor -> viewDataAsWorldByFraction ();
		return _editor -> structFunctionEditor :: v_mouseInWideDataView (event, x_world, y_fraction);
	}
	void save (conststring32 undoText) {
		Editor_save (_editor, undoText);
	}
	void broadcastDataChanged () {
		Editor_broadcastDataChanged (_editor);
	}
	bool y_fraction_globalIsInside (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return y_pxlt >= our bottom_pxlt() && y_pxlt <= our top_pxlt();
	}
	double y_fraction_globalToLocal (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return (y_pxlt - our bottom_pxlt()) / (our top_pxlt() - our bottom_pxlt());
	}
protected:
private:
	FunctionEditor _editor;
	double _ymin_fraction, _ymax_fraction;
	double globalY_fraction_to_pxlt (double globalY_fraction) const {
		return _editor -> dataBottom_pxlt() +
				globalY_fraction * (_editor -> dataTop_pxlt() - _editor -> dataBottom_pxlt());
	}
	double left_pxlt() const { return _editor -> dataLeft_pxlt(); }
	double right_pxlt() const { return _editor -> dataRight_pxlt(); }
	double verticalSpacing_pxlt() const { return 11; }
	double bottom_pxlt() const {
		const double bottomSpacing_pxlt = ( _ymin_fraction == 0.0 ? 0.0 : our verticalSpacing_pxlt() );
		return globalY_fraction_to_pxlt (_ymin_fraction) + bottomSpacing_pxlt;
	}
	double top_pxlt() const {
		return globalY_fraction_to_pxlt (_ymax_fraction) - our verticalSpacing_pxlt();
	}
};

inline void FunctionArea_init (FunctionArea me, FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	my init (editor, ymin_fraction, ymax_fraction);
}

/* End of file FunctionArea.h */
#endif
