#ifndef _FunctionArea_h_
#define _FunctionArea_h_
/* FunctionArea.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015-2018,2020-2022 Paul Boersma
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

#include "DataGui.h"
#include "FunctionEditor.h"

Thing_define (FunctionArea, DataGui) {
	/*
		Accessors.
	*/
public:   // all readonly
	Function function() const { Thing_cast (Function, function, our data()); return function; }
	FunctionEditor functionEditor() const { Thing_cast (FunctionEditor, functionEditor, our boss()); return functionEditor; }
	double startWindow() const { return functionEditor() -> startWindow; }
	double endWindow() const { return functionEditor() -> endWindow; }
	double startSelection() const { return functionEditor() -> startSelection; }
	double endSelection() const { return functionEditor() -> endSelection; }
	double tmin() const { return functionEditor() -> tmin; }
	double tmax() const { return functionEditor() -> tmax; }
	Graphics graphics() const { return functionEditor() -> graphics.get(); }

	/*
		Initialization.
	*/
public:
	friend void FunctionArea_init (FunctionArea me, bool editable, Function optionalFunctionToCopy, FunctionEditor boss) {
		DataGui_init (me, nullptr, editable, boss);
		my _optionalFunctionCopy = Data_copy (optionalFunctionToCopy);
	}
	bool functionHasBeenCopied () {
		return !! _optionalFunctionCopy;
	}
private:
	autoFunction _optionalFunctionCopy;

public:
	void functionChanged (Function newFunction) {
		our setData (our _optionalFunctionCopy ? our _optionalFunctionCopy.get() : newFunction);
		our v_invalidateAllDerivedDataCaches ();
		our v_computeAuxiliaryData ();
	}
	void invalidateAllDerivedDataCaches () {
		our v_invalidateAllDerivedDataCaches ();
	}

protected:
	virtual void v_invalidateAllDerivedDataCaches () { }   // derived classes can call inherited at end
	virtual void v_computeAuxiliaryData () { }   // derived classes can call inherited at start

public:
	/*
		To be called just before drawing or tracking:
	*/
	void setGlobalYRange_fraction (double ymin_fraction, double ymax_fraction) {
		_ymin_fraction = ymin_fraction;
		_ymax_fraction = ymax_fraction;
	}
	/*
		FunctionArea_setViewport() is used both for drawing and for mousing.
	*/
	friend void FunctionArea_setViewport (constFunctionArea me) {
		Graphics_setViewport (my graphics(), my left_pxlt(), my right_pxlt(), my bottom_pxlt(), my top_pxlt());
	}
	friend void FunctionArea_drawOne (FunctionArea me) {
		FunctionArea_prepareCanvas (me);
		FunctionArea_drawInside (me);
	}
	friend void FunctionArea_drawTwo (FunctionArea me, FunctionArea you) {
		FunctionArea_prepareCanvas (me);
		FunctionArea_drawInside (me);
		FunctionArea_drawInside (you);
	}
	friend void FunctionArea_prepareCanvas (FunctionArea me) {
		FunctionArea_setViewport (me);
		FunctionArea_drawBackground (me);
	}
	friend void FunctionArea_drawBackground (constFunctionArea me) {
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (my graphics(), DataGuiColour_BACKGROUND);
		Graphics_fillRectangle (my graphics(), 0.0, 1.0, 0.0, 1.0);
		if (my editable()) {
			Graphics_setLineWidth (my graphics(), 2.0);
			Graphics_setColour (my graphics(), DataGuiColour_EDITABLE);
			Graphics_rectangle (my graphics(), 0.0, 1.0, 0.0, 1.0);
		}
		Graphics_setColour (my graphics(), DataGuiColour_DEFAULT_FOREGROUND);
		Graphics_setLineWidth (my graphics(), 1.0);
	}
	friend void FunctionArea_drawInside (FunctionArea me) {
		my v_drawInside ();
	}
protected:
	virtual void v_drawInside () { }
public:
	void setSelection (double startSelection, double endSelection) {
		Melder_sort (& startSelection, & endSelection);
		functionEditor() -> startSelection = startSelection;
		functionEditor() -> endSelection = endSelection;
	}
	bool defaultMouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
		functionEditor() -> viewDataAsWorldByFraction ();
		return functionEditor() -> structFunctionEditor :: v_mouseInWideDataView (event, x_world, y_fraction);
	}
	void save (conststring32 undoText) {
		Editor_save (functionEditor(), undoText);
	}
	void broadcastDataChanged () {
		Editor_broadcastDataChanged (functionEditor());
	}
	bool y_fraction_globalIsInside (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return y_pxlt >= our bottom_pxlt() && y_pxlt < our top_pxlt();
	}
	double y_fraction_globalToLocal (double globalY_fraction) const {
		const double y_pxlt = globalY_fraction_to_pxlt (globalY_fraction);
		return (y_pxlt - our bottom_pxlt()) / (our top_pxlt() - our bottom_pxlt());
	}
private:
	double _ymin_fraction, _ymax_fraction;
	double globalY_fraction_to_pxlt (double globalY_fraction) const {
		return functionEditor() -> dataBottom_pxlt() +
				globalY_fraction * (functionEditor() -> dataTop_pxlt() - functionEditor() -> dataBottom_pxlt());
	}
	double left_pxlt() const { return functionEditor() -> dataLeft_pxlt(); }
	double right_pxlt() const { return functionEditor() -> dataRight_pxlt(); }
	virtual double v_verticalSpacing_pxlt() const { return 11; }
	double bottom_pxlt() const {
		const double bottomSpacing_pxlt = ( _ymin_fraction == 0.0 ? 0.0 : our v_verticalSpacing_pxlt() );
		return globalY_fraction_to_pxlt (_ymin_fraction) + bottomSpacing_pxlt;
	}
	double top_pxlt() const {
		return globalY_fraction_to_pxlt (_ymax_fraction) - our v_verticalSpacing_pxlt();
	}
public:
	virtual void v_form_pictureSelection (EditorCommand);
	virtual void v_ok_pictureSelection (EditorCommand);
	virtual void v_do_pictureSelection (EditorCommand);

	#include "FunctionArea_prefs.h"
};

#define DEFINE_FunctionArea_create(FunctionAreaType, FunctionType) \
inline auto##FunctionAreaType FunctionAreaType##_create (bool editable, FunctionType functionToCopy, FunctionEditor boss) { \
	auto##FunctionAreaType me = Thing_new (FunctionAreaType); \
	FunctionArea_init (me.get(), editable, functionToCopy, boss); \
	return me; \
}

typedef MelderCallback <void, structFunctionArea, EditorCommand, UiForm, integer /*narg*/, Stackel /*args*/, conststring32,
		Interpreter> FunctionAreaCommandCallback;

GuiMenuItem FunctionAreaMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
		FunctionAreaCommandCallback commandCallback, FunctionArea commandBoss);

/* End of file FunctionArea.h */
#endif
