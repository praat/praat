/* GuiScrollBar.cpp
 *
 * Copyright (C) 1993-2011,2012,2013 Paul Boersma, 2013 Tom Naughton
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

#include "GuiP.h"

Thing_implement (GuiScrollBar, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_scrollbar \
		Melder_assert (widget -> widgetClass == xmScrollBarWidgetClass); \
		GuiScrollBar me = (GuiScrollBar) widget -> userData
#else
	#define iam_scrollbar \
		GuiScrollBar me = (GuiScrollBar) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkScrollBar_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiScrollBar);
		forget (me);
	}
	static void _GuiGtkScrollBar_valueChangedCallback (GuiObject widget, gpointer void_me) {
		iam (GuiScrollBar);
		trace ("enter");
		if (my d_valueChangedCallback != NULL && ! my d_blockValueChangedCallbacks) {
			struct structGuiScrollBarEvent event = { me };
			try {
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your action in a scroll bar was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
#elif cocoa
@interface GuiCocoaScrollBar ()
@property (nonatomic, assign) double m_minimum;
@property (nonatomic, assign) double m_maximum;
@property (nonatomic, assign) double m_value;
@property (nonatomic, assign) double m_sliderSize;
@property (nonatomic, assign) double m_increment;
@property (nonatomic, assign) double m_pageIncrement;
@end

// http://www.lucernesys.com/blog/2010/02/11/nsscroller/

@implementation GuiCocoaScrollBar {
    GuiScrollBar d_userData;
}
- (void) dealloc {   // override
    GuiScrollBar me = d_userData;
    forget (me);
    trace ("deleting a scroll bar");
    [super dealloc];
}
- (GuiThing) userData {
    return d_userData;
}
- (void) setUserData: (GuiThing) userData {
	Melder_assert (userData == NULL || Thing_member (userData, classGuiScrollBar));
	d_userData = static_cast <GuiScrollBar> (userData);
}
- (void) setMinimum:(double)minimum maximum:(double)maximum value:(double)value sliderSize:(double)sliderSize increment:(double)increment pageIncrement:(double)pageIncrement {
	Melder_assert (NUMdefined (minimum));
	_m_minimum = minimum;
	_m_maximum = maximum;
	_m_value = value;
	_m_sliderSize = sliderSize;
	_m_increment = increment;
	_m_pageIncrement = pageIncrement;
	double spaceLeft = (maximum - minimum) - sliderSize;
	if (spaceLeft <= 0.0) {
		[self setKnobProportion: 1.0];
		[self setDoubleValue: 0.5];
	} else {
		[self setKnobProportion: sliderSize / (maximum - minimum)];
		[self setDoubleValue: (value - minimum) / spaceLeft];
	}
}
- (void) valueChanged {
	GuiScrollBar me = (GuiScrollBar) d_userData;
	switch ([self hitPart]) {
        case NSScrollerIncrementLine: {
            // Include code here for the case where the down arrow is pressed
			_m_value += _m_increment;
			if (_m_value > _m_maximum - _m_sliderSize)
				_m_value = _m_maximum - _m_sliderSize;
			[self setMinimum: _m_minimum maximum: _m_maximum value: _m_value sliderSize: _m_sliderSize increment: _m_increment pageIncrement: _m_pageIncrement];
		} break;
        case NSScrollerIncrementPage: {
            // Include code here for the case where CTRL + down arrow is pressed, or the space the scroll knob moves in is pressed
			_m_value += _m_pageIncrement;
			if (_m_value > _m_maximum - _m_sliderSize)
				_m_value = _m_maximum - _m_sliderSize;
			[self setMinimum: _m_minimum maximum: _m_maximum value: _m_value sliderSize: _m_sliderSize increment: _m_increment pageIncrement: _m_pageIncrement];
		} break;
        case NSScrollerDecrementLine: {
            // Include code here for the case where the up arrow is pressed
			_m_value -= _m_increment;
			if (_m_value < _m_minimum)
				_m_value = _m_minimum;
			[self setMinimum: _m_minimum maximum: _m_maximum value: _m_value sliderSize: _m_sliderSize increment: _m_increment pageIncrement: _m_pageIncrement];
		} break;
        case NSScrollerDecrementPage: {
            // Include code here for the case where CTRL + up arrow is pressed, or the space the scroll knob moves in is pressed
			_m_value -= _m_pageIncrement;
			if (_m_value < _m_minimum)
				_m_value = _m_minimum;
			[self setMinimum: _m_minimum maximum: _m_maximum value: _m_value sliderSize: _m_sliderSize increment: _m_increment pageIncrement: _m_pageIncrement];
		} break;
        case NSScrollerKnob: {
            // This case is when the knob itself is pressed
			double spaceLeft = (_m_maximum - _m_minimum) - _m_sliderSize;
    		_m_value = _m_minimum + [self doubleValue] * (spaceLeft <= 0.0 ? 0.0 : spaceLeft);
		} break;
        default: {
		} break;
    }
    if (my d_valueChangedCallback) {
        struct structGuiScrollBarEvent event = { me };
        try {
            my d_valueChangedCallback (my d_valueChangedBoss, & event);
        } catch (MelderError) {
            Melder_flushError ("Scroll not completely handled.");
        }
    }
}
@end
#elif win
	void _GuiWinScrollBar_destroy (GuiObject widget) {
		_GuiNativeControl_destroy (widget);
		iam_scrollbar;
		forget (me);   // NOTE: my widget is not destroyed here
	}
#elif mac
	void _GuiMacScrollBar_destroy (GuiObject widget) {
		_GuiNativeControl_destroy (widget);
		iam_scrollbar;
		forget (me);   // NOTE: my widget is not destroyed here
	}
#endif
#if motif
	static void _GuiMotifScrollBar_valueChangedCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		iam (GuiScrollBar);
		if (my d_valueChangedCallback) {
			struct structGuiScrollBarEvent event = { me };
			try {
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Scroll not completely handled.");
			}
		}
	}
#endif

GuiScrollBar GuiScrollBar_create (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	void (*valueChangedCallback) (void *boss, GuiScrollBarEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiScrollBar me = Thing_new (GuiScrollBar);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_valueChangedCallback = valueChangedCallback;
	my d_valueChangedBoss = valueChangedBoss;
	#if gtk
		GtkObject *adj = gtk_adjustment_new (value, minimum, maximum, increment, pageIncrement, sliderSize);
		my d_widget = flags & GuiScrollBar_HORIZONTAL ? gtk_hscrollbar_new (GTK_ADJUSTMENT (adj)) : gtk_vscrollbar_new (GTK_ADJUSTMENT (adj));
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		g_signal_connect (G_OBJECT (my d_widget), "value-changed", G_CALLBACK (_GuiGtkScrollBar_valueChangedCallback), me);
	#elif cocoa
		NSRect dummyFrame = flags & GuiScrollBar_HORIZONTAL ? NSMakeRect (20, 20, 100, [NSScroller scrollerWidth]) : NSMakeRect (20, 20, [NSScroller scrollerWidth], 100);
		GuiCocoaScrollBar *scroller = [[GuiCocoaScrollBar alloc] initWithFrame: dummyFrame];
		my d_widget = scroller;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[scroller setUserData: me];
		[scroller setEnabled: YES];
		[scroller   setMinimum: minimum   maximum: maximum   value: value   sliderSize: sliderSize   increment: increment   pageIncrement: pageIncrement];
        //[scroller setScrollerStyle: NSScrollerStyleOverlay];
        [scroller setTarget: scroller];
        [scroller setAction: @selector (valueChanged)];
	#elif win
		my d_widget = XtVaCreateWidget (flags & GuiScrollBar_HORIZONTAL ? "horizontalScrollBar" : "verticalScrollBar",   // the name is checked for deciding the orientation...
			xmScrollBarWidgetClass, parent -> d_widget,
			XmNorientation, flags & GuiScrollBar_HORIZONTAL ? XmHORIZONTAL : XmVERTICAL,
			XmNminimum, (int) minimum,
			XmNmaximum, (int) maximum,
			XmNvalue, (int) value,
			XmNsliderSize, (int) sliderSize,
			XmNincrement, (int) increment,
			XmNpageIncrement, (int) pageIncrement,
			NULL);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtAddCallback (my d_widget, XmNvalueChangedCallback, _GuiMotifScrollBar_valueChangedCallback, (XtPointer) me);
		XtAddCallback (my d_widget, XmNdragCallback, _GuiMotifScrollBar_valueChangedCallback, (XtPointer) me);
	#elif mac
		my d_widget = XtVaCreateWidget ("scrollBar",
			xmScrollBarWidgetClass, parent -> d_widget,
			XmNorientation, flags & GuiScrollBar_HORIZONTAL ? XmHORIZONTAL : XmVERTICAL,
			XmNminimum, (int) minimum,
			XmNmaximum, (int) maximum,
			XmNvalue, (int) value,
			XmNsliderSize, (int) sliderSize,
			XmNincrement, (int) increment,
			XmNpageIncrement, (int) pageIncrement,
			NULL);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtAddCallback (my d_widget, XmNvalueChangedCallback, _GuiMotifScrollBar_valueChangedCallback, (XtPointer) me);
		XtAddCallback (my d_widget, XmNdragCallback, _GuiMotifScrollBar_valueChangedCallback, (XtPointer) me);
	#endif
	return me;
}

GuiScrollBar GuiScrollBar_createShown (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	void (*valueChangedCallback) (void *boss, GuiScrollBarEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiScrollBar me = GuiScrollBar_create (parent, left, right, top, bottom,
		minimum, maximum, value, sliderSize, increment, pageIncrement,
		valueChangedCallback, valueChangedBoss, flags);
	my f_show ();
	return me;
}

void structGuiScrollBar :: f_set (double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement) {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (d_widget));
		gtk_adjustment_configure (GTK_ADJUSTMENT (adj),
			NUMdefined (value)         ? value         : gtk_adjustment_get_value          (GTK_ADJUSTMENT (adj)),
			NUMdefined (minimum)       ? minimum       : gtk_adjustment_get_lower          (GTK_ADJUSTMENT (adj)),
			NUMdefined (maximum)       ? maximum       : gtk_adjustment_get_upper          (GTK_ADJUSTMENT (adj)),
			NUMdefined (increment)     ? increment     : gtk_adjustment_get_step_increment (GTK_ADJUSTMENT (adj)),
			NUMdefined (pageIncrement) ? pageIncrement : gtk_adjustment_get_page_increment (GTK_ADJUSTMENT (adj)),
			NUMdefined (sliderSize)    ? sliderSize    : gtk_adjustment_get_page_size      (GTK_ADJUSTMENT (adj)));
	#elif cocoa
		GuiCocoaScrollBar *scroller = (GuiCocoaScrollBar *) d_widget;
		[scroller
			setMinimum:    NUMdefined (minimum)       ? minimum       : [scroller m_minimum]
			maximum:       NUMdefined (maximum)       ? maximum       : [scroller m_maximum]
			value:         NUMdefined (value)         ? value         : [scroller m_value]
			sliderSize:    NUMdefined (sliderSize)    ? sliderSize    : [scroller m_sliderSize]
			increment:     NUMdefined (increment)     ? increment     : [scroller m_increment]
			pageIncrement: NUMdefined (pageIncrement) ? pageIncrement : [scroller m_pageIncrement]];
	#elif motif
		if (NUMdefined (minimum))
			XtVaSetValues (d_widget, XmNminimum, (int) minimum, NULL);
		if (NUMdefined (maximum))
			XtVaSetValues (d_widget, XmNmaximum, (int) maximum, NULL);
		int oldValue, oldSliderSize, oldIncrement, oldPageIncrement;
		XmScrollBarGetValues (d_widget, & oldValue, & oldSliderSize, & oldIncrement, & oldPageIncrement);
		XmScrollBarSetValues (d_widget,
			NUMdefined (value)         ? value         : oldValue,
			NUMdefined (sliderSize)    ? sliderSize    : oldSliderSize,
			NUMdefined (increment)     ? increment     : oldIncrement,
			NUMdefined (pageIncrement) ? pageIncrement : oldPageIncrement,
			False);
	#endif
}

int structGuiScrollBar :: f_getValue () {
	#if gtk
		return gtk_range_get_value (GTK_RANGE (d_widget));
	#elif cocoa
		GuiCocoaScrollBar *scroller = (GuiCocoaScrollBar *) d_widget;
		return [scroller m_value];
	#elif motif
		int value, slider, incr, pincr;
		XmScrollBarGetValues (d_widget, & value, & slider, & incr, & pincr);
		return value;
	#endif
}

/* End of file GuiScrollBar.cpp */
