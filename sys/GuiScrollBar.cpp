/* GuiScrollBar.cpp
 *
 * Copyright (C) 1993-2011,2012,2013,2014,2015,2016,2017 Paul Boersma, 2013 Tom Naughton
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

#include "GuiP.h"

Thing_implement (GuiScrollBar, GuiControl, 0);

#if motif
	#define iam_scrollbar \
		Melder_assert (widget -> widgetClass == xmScrollBarWidgetClass); \
		GuiScrollBar me = (GuiScrollBar) widget -> userData
#else
	#define iam_scrollbar \
		GuiScrollBar me = (GuiScrollBar) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkScrollBar_destroyCallback (GuiObject /* widget */, gpointer void_me) {
		iam (GuiScrollBar);
		forget (me);
	}
	static void _GuiGtkScrollBar_valueChangedCallback (GuiObject widget, gpointer void_me) {
		iam (GuiScrollBar);
		trace (U"enter: blocked ", my d_blockValueChangedCallbacks);
		trace (U"_GuiGtkScrollBar_valueChangedCallback: ", GuiScrollBar_getValue (me));
		if (my d_blockValueChangedCallbacks) {
			my d_blockValueChangedCallbacks = false;
		} else if (my d_valueChangedCallback) {
			struct structGuiScrollBarEvent event { me };
			try {
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your action in a scroll bar was not completely handled.");
			}
		}
	}
#elif motif
	void _GuiWinScrollBar_destroy (GuiObject widget) {
		_GuiNativeControl_destroy (widget);
		iam_scrollbar;
		forget (me);   // NOTE: my widget is not destroyed here
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
		trace (U"deleting a scroll bar");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiScrollBar));
		d_userData = static_cast <GuiScrollBar> (userData);
	}
	- (void) setMinimum: (double)minimum maximum:(double)maximum value:(double)value sliderSize:(double)sliderSize increment:(double)increment pageIncrement:(double)pageIncrement {
		Melder_assert (isdefined (minimum));
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
	- (void) _update {
		GuiScrollBar me = (GuiScrollBar) self -> d_userData;
		[self setMinimum: _m_minimum maximum: _m_maximum value: _m_value sliderSize: _m_sliderSize increment: _m_increment pageIncrement: _m_pageIncrement];
		if (my d_valueChangedCallback) {
			struct structGuiScrollBarEvent event { me };
			try {
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Scroll not completely handled.");
			}
		}
	}
	- (void) scrollBy: (double) step {
		trace (U"step ", step);
		if (step == 0) return;
		_m_value -= 0.3 * step * _m_increment;
		if (_m_value < _m_minimum)
			_m_value = _m_minimum;
		if (_m_value > _m_maximum - _m_sliderSize)
			_m_value = _m_maximum - _m_sliderSize;
		[self _update];
	}
	- (void) magnifyBy: (double) step {
		trace (U"step ", step);
		double increase = _m_sliderSize * (exp (- step) - 1.0);
		_m_sliderSize += increase;
		if (_m_sliderSize > _m_maximum - _m_minimum)
			_m_sliderSize = _m_maximum - _m_minimum;
		_m_value -= 0.5 * increase;
		if (_m_value < _m_minimum)
			_m_value = _m_minimum;
		if (_m_value > _m_maximum - _m_sliderSize)
			_m_value = _m_maximum - _m_sliderSize;
		[self _update];
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
			struct structGuiScrollBarEvent event { me };
			try {
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Scroll not completely handled.");
			}
		}
	}
	@end
#endif
#if motif
	static void _GuiMotifScrollBar_valueChangedCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		iam (GuiScrollBar);
		if (my d_valueChangedCallback) {
			struct structGuiScrollBarEvent event { me };
			try {
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Scroll not completely handled.");
			}
		}
	}
#endif

GuiScrollBar GuiScrollBar_create (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	GuiScrollBarCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags)
{
	autoGuiScrollBar me = Thing_new (GuiScrollBar);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_valueChangedCallback = valueChangedCallback;
	my d_valueChangedBoss = valueChangedBoss;
	#if gtk
		GtkAdjustment *adjustment = gtk_adjustment_new (value, minimum, maximum, increment, pageIncrement, sliderSize);
		GtkOrientation orientation = ( flags & GuiScrollBar_HORIZONTAL ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL );
		my d_widget = gtk_scrollbar_new (orientation, adjustment);
		// TODO: figure out whether we have to delete the adjustment
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		g_signal_connect (G_OBJECT (my d_widget), "value-changed", G_CALLBACK (_GuiGtkScrollBar_valueChangedCallback), me.get());
	#elif motif
		my d_widget = XtVaCreateWidget (flags & GuiScrollBar_HORIZONTAL ? "horizontalScrollBar" : "verticalScrollBar",   // the name is checked for deciding the orientation...
			xmScrollBarWidgetClass, parent -> d_widget,
			XmNorientation, flags & GuiScrollBar_HORIZONTAL ? XmHORIZONTAL : XmVERTICAL,
			XmNminimum, (int) minimum,
			XmNmaximum, (int) maximum,
			XmNvalue, (int) value,
			XmNsliderSize, (int) sliderSize,
			XmNincrement, (int) increment,
			XmNpageIncrement, (int) pageIncrement,
			nullptr);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtAddCallback (my d_widget, XmNvalueChangedCallback, _GuiMotifScrollBar_valueChangedCallback, (XtPointer) me.get());
		XtAddCallback (my d_widget, XmNdragCallback, _GuiMotifScrollBar_valueChangedCallback, (XtPointer) me.get());
	#elif cocoa
		NSRect dummyFrame = flags & GuiScrollBar_HORIZONTAL ? NSMakeRect (20, 20, 100, [NSScroller scrollerWidth]) : NSMakeRect (20, 20, [NSScroller scrollerWidth], 100);
		GuiCocoaScrollBar *scroller = [[GuiCocoaScrollBar alloc] initWithFrame: dummyFrame];
		my d_widget = scroller;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[scroller setUserData: me.get()];
		[scroller setEnabled: YES];
		[scroller   setMinimum: minimum   maximum: maximum   value: value   sliderSize: sliderSize   increment: increment   pageIncrement: pageIncrement];
        //[scroller setScrollerStyle: NSScrollerStyleOverlay];
        [scroller setTarget: scroller];
        [scroller setAction: @selector (valueChanged)];
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiScrollBar GuiScrollBar_createShown (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	GuiScrollBarCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags)
{
	GuiScrollBar me = GuiScrollBar_create (parent, left, right, top, bottom,
		minimum, maximum, value, sliderSize, increment, pageIncrement,
		valueChangedCallback, valueChangedBoss, flags);
	GuiThing_show (me);
	return me;
}

void GuiScrollBar_set (GuiScrollBar me, double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement) {
	/*
	 * This function calls the native scroll bar modification function.
	 *
	 * Note:
	 * On almost all platforms, using the native scroll bar modification function sends a value-changed notification to the scroll bar.
	 * This will call our own d_valueChangedCallback if we don't prevent it.
	 * We have to prevent that, because our d_valueChangedCallback is only for user-initiated modifications.
	 */
	trace (U"enter ", minimum, U" ", maximum, U" ", value, U" ", sliderSize, U" ", increment, U" ", pageIncrement);
	#if gtk
		/*
		 * We're going to modify the scroll bar with gtk_adjustment_configure ().
		 * This function sends a *slow* value-changed notification to the scroll bar.
		 * We have to make sure that our own d_valueChangedCallback is not called.
		 */
		my d_blockValueChangedCallbacks = true;
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (my d_widget));
		gtk_adjustment_configure (GTK_ADJUSTMENT (adj),
			isdefined (value)         ? value         : gtk_adjustment_get_value          (GTK_ADJUSTMENT (adj)),
			isdefined (minimum)       ? minimum       : gtk_adjustment_get_lower          (GTK_ADJUSTMENT (adj)),
			isdefined (maximum)       ? maximum       : gtk_adjustment_get_upper          (GTK_ADJUSTMENT (adj)),
			isdefined (increment)     ? increment     : gtk_adjustment_get_step_increment (GTK_ADJUSTMENT (adj)),
			isdefined (pageIncrement) ? pageIncrement : gtk_adjustment_get_page_increment (GTK_ADJUSTMENT (adj)),
			isdefined (sliderSize)    ? sliderSize    : gtk_adjustment_get_page_size      (GTK_ADJUSTMENT (adj)));
		/*
		 * We don't set d_blockValueChangedCallbacks back to false yet, because GTK calls the valueChangedCallback with a delay.
		 */
	#elif motif
		if (isdefined (minimum))
			XtVaSetValues (my d_widget, XmNminimum, (int) minimum, nullptr);
		if (isdefined (maximum))
			XtVaSetValues (my d_widget, XmNmaximum, (int) maximum, nullptr);
		int oldValue, oldSliderSize, oldIncrement, oldPageIncrement;
		XmScrollBarGetValues (my d_widget, & oldValue, & oldSliderSize, & oldIncrement, & oldPageIncrement);
		XmScrollBarSetValues (my d_widget,
			isdefined (value)         ? value         : oldValue,
			isdefined (sliderSize)    ? sliderSize    : oldSliderSize,
			isdefined (increment)     ? increment     : oldIncrement,
			isdefined (pageIncrement) ? pageIncrement : oldPageIncrement,
			False);
	#elif cocoa
		/*
		 * We're going to modify the scroll bar with setMinimum:maximum:...
		 * This function sends a *synchronous* value-changed notification to the scroll bar.
		 * We have to make sure that our own d_valueChangedCallback is not called.
		 */
		GuiControlBlockValueChangedCallbacks block (me);
		GuiCocoaScrollBar *scroller = (GuiCocoaScrollBar *) my d_widget;
		[scroller
			setMinimum:    isdefined (minimum)       ? minimum       : [scroller m_minimum]
			maximum:       isdefined (maximum)       ? maximum       : [scroller m_maximum]
			value:         isdefined (value)         ? value         : [scroller m_value]
			sliderSize:    isdefined (sliderSize)    ? sliderSize    : [scroller m_sliderSize]
			increment:     isdefined (increment)     ? increment     : [scroller m_increment]
			pageIncrement: isdefined (pageIncrement) ? pageIncrement : [scroller m_pageIncrement]];
	#endif
	trace (U"exit");
}

double GuiScrollBar_getValue (GuiScrollBar me) {
	#if gtk
		return gtk_range_get_value (GTK_RANGE (my d_widget));
	#elif motif
		int value, slider, incr, pincr;
		XmScrollBarGetValues (my d_widget, & value, & slider, & incr, & pincr);
		return value;
	#elif cocoa
		GuiCocoaScrollBar *scroller = (GuiCocoaScrollBar *) my d_widget;
		return [scroller m_value];
	#else
		return 0;
	#endif
}

double GuiScrollBar_getSliderSize (GuiScrollBar me) {
	#if gtk
		return 1;   // NYI
	#elif motif
		int value, slider, incr, pincr;
		XmScrollBarGetValues (my d_widget, & value, & slider, & incr, & pincr);
		return slider;
	#elif cocoa
		GuiCocoaScrollBar *scroller = (GuiCocoaScrollBar *) my d_widget;
		return [scroller   m_sliderSize];
	#else
		return 0;
	#endif
}

/* End of file GuiScrollBar.cpp */
