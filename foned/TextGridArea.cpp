/* TextGridArea.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "TextGridArea.h"
#include "TextGrid_Sound.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "LongSoundArea.h"   // for drawing TextGrid and Sound, or for aligning TextGrid to Sound
#include "SoundAnalysisArea.h"   // for drawing TextGrid and Pitch
#include "EditorM.h"

Thing_implement (TextGridArea, FunctionArea, 0);

#include "enums_getText.h"
#include "TextGridArea_enums.h"
#include "enums_getValue.h"
#include "TextGridArea_enums.h"

#include "Prefs_define.h"
#include "TextGridArea_prefs.h"
#include "Prefs_install.h"
#include "TextGridArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "TextGridArea_prefs.h"


static void timeToInterval (TextGridArea me, double t, integer tierNumber,
	double *out_tmin, double *out_tmax)
{
	Melder_assert (isdefined (t));
	const Function tier = my textGrid() -> tiers->at [tierNumber];
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (tier, & intervalTier, & textTier);
	if (intervalTier) {
		integer iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval == 0) {
			if (t < my tmin()) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals.size;
			}
		}
		Melder_assert (iinterval >= 1);
		Melder_assert (iinterval <= intervalTier -> intervals.size);
		const TextInterval interval = intervalTier -> intervals.at [iinterval];
		*out_tmin = interval -> xmin;
		*out_tmax = interval -> xmax;
	} else {
		const integer n = textTier -> points.size;
		if (n == 0) {
			*out_tmin = my tmin();
			*out_tmax = my tmax();
		} else {
			integer ipointleft = AnyTier_timeToLowIndex (textTier->asAnyTier(), t);
			*out_tmin = ( ipointleft == 0 ? my tmin() : textTier -> points.at [ipointleft] -> number );
			*out_tmax = ( ipointleft == n ? my tmax() : textTier -> points.at [ipointleft + 1] -> number );
		}
	}
	Melder_clipLeft (my tmin(), out_tmin);
	Melder_clipRight (out_tmax, my tmax());
}

static void insertBoundaryOrPoint (TextGridArea me, integer itier, double t1, double t2, bool insertSecond) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	if (itier < 1 || itier > numberOfTiers)
		Melder_throw (U"No tier ", itier, U".");
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [itier], & intervalTier, & textTier);
	Melder_assert (t1 <= t2);

	if (intervalTier) {
		autoTextInterval rightNewInterval, midNewInterval;
		const bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
		const bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
		if (t1 == t2 && t1IsABoundary)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t1, 6), U" seconds, because there is already a boundary there.");
		if (t1IsABoundary && t2IsABoundary)
			Melder_throw (U"Cannot add boundaries at ", Melder_fixed (t1, 6), U" and ", Melder_fixed (t2, 6), U" seconds, because there are already boundaries there.");
		const integer iinterval = IntervalTier_timeToIndex (intervalTier, t1);
		const integer iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
		if (iinterval == 0 || iinterval2 == 0)
			Melder_throw (U"The selection is outside the time domain of the intervals.");
		const integer correctedIinterval2 = ( t2IsABoundary && iinterval2 == intervalTier -> intervals.size ? iinterval2 + 1 : iinterval2 );
		if (correctedIinterval2 > iinterval + 1 || (correctedIinterval2 > iinterval && ! t2IsABoundary))
			Melder_throw (U"The selection straddles a boundary.");
		const TextInterval interval = intervalTier -> intervals.at [iinterval];

		if (t1 == t2) {
			FunctionArea_save (me, U"Add boundary");
		} else {
			FunctionArea_save (me, U"Add interval");
		}

		if (itier == my selectedTier) {
			/*
				Divide up the label text into left, mid and right, depending on where the text selection is.
			*/
			integer left, right;
			autostring32 text = GuiText_getStringAndSelectionPosition (my functionEditor() -> textArea, & left, & right);
			const bool wholeTextIsSelected = ( right - left == str32len (text.get()) );
			rightNewInterval = TextInterval_create (t2, interval -> xmax, text.get() + right);
			text [right] = U'\0';
			midNewInterval = TextInterval_create (t1, t2, text.get() + left);
			if (! wholeTextIsSelected || t1 != t2)
				text [left] = U'\0';
			TextInterval_setText (interval, text.get());
		} else {
			/*
				Move the text to the left of the boundary.
			*/
			rightNewInterval = TextInterval_create (t2, interval -> xmax, U"");
			midNewInterval = TextInterval_create (t1, t2, U"");
		}
		if (t1IsABoundary) {
			/*
				Merge mid with left interval.
			*/
			if (interval -> xmin != t1)
				Melder_fatal (U"Boundary unequal: ", interval -> xmin, U" versus ", t1, U".");
			interval -> xmax = t2;
			TextInterval_setText (interval, Melder_cat (interval -> text.get(), midNewInterval -> text.get()));
		} else if (t2IsABoundary) {
			/*
				Merge mid and right interval.
			*/
			if (interval -> xmax != t2)
				Melder_fatal (U"Boundary unequal: ", interval -> xmax, U" versus ", t2, U".");
			interval -> xmax = t1;
			Melder_assert (rightNewInterval -> xmin == t2);
			Melder_assert (rightNewInterval -> xmax == t2);
			rightNewInterval -> xmin = t1;
			TextInterval_setText (rightNewInterval.get(), Melder_cat (midNewInterval -> text.get(), rightNewInterval -> text.get()));
		} else {
			interval -> xmax = t1;
			if (t1 != t2)
				intervalTier -> intervals.addItem_move (midNewInterval.move());
		}
		intervalTier -> intervals.addItem_move (rightNewInterval.move());
		if (insertSecond && numberOfTiers >= 2 && t1 == t2) {
			/*
				Find the last time before t on another tier.
			*/
			double tlast = interval -> xmin;
			for (integer jtier = 1; jtier <= numberOfTiers; jtier ++) {
				if (jtier != itier) {
					double startInterval, endInterval;
					timeToInterval (me, t1, jtier, & startInterval, & endInterval);
					if (startInterval > tlast)
						tlast = startInterval;
				}
			}
			if (tlast > interval -> xmin && tlast < t1) {
				autoTextInterval newInterval = TextInterval_create (tlast, t1, U"");
				interval -> xmax = tlast;
				intervalTier -> intervals.addItem_move (newInterval.move());
			}
		}
	} else {
		Melder_assert (isdefined (t1));
		if (AnyTier_hasPoint (textTier->asAnyTier(), t1))
			Melder_throw (U"Cannot add a point at ", Melder_fixed (t1, 6), U" seconds, because there is already a point there.");

		FunctionArea_save (me, U"Add point");

		autoTextPoint newPoint = TextPoint_create (t1, U"");
		textTier -> points. addItem_move (newPoint.move());
	}
	my setSelection (t1, t1);
}


#pragma mark - TextGridArea info

void structTextGridArea :: v1_info () {
	MelderInfo_writeLine (U"Selected tier: ", our selectedTier);
	MelderInfo_writeLine (U"TextGrid uses text styles: ", our instancePref_useTextStyles());
	MelderInfo_writeLine (U"TextGrid font size: ", our instancePref_fontSize());
	MelderInfo_writeLine (U"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (our instancePref_alignment()));
}


#pragma mark - TextGridArea drawing

void structTextGridArea :: v_specializedHighlightBackground () const {
	Melder_assert (our textGrid());
	const integer numberOfTiers = our textGrid() -> tiers->size;
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		const Function anyTier = our textGrid() -> tiers->at [itier];
		if (anyTier -> classInfo != classIntervalTier)
			continue;
		IntervalTier tier = (IntervalTier) anyTier;
		/*
			Highlight interval: yellow (selected) or green (matching label).
		*/
		const integer selectedInterval = ( itier == our selectedTier ? getSelectedInterval (this) : 0 );
		const integer numberOfIntervals = tier -> intervals.size;
		for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			/* mutable clip */ double startInterval = interval -> xmin, endInterval = interval -> xmax;
			if (endInterval > our startWindow() && startInterval < our endWindow()) {   // interval visible?
				const bool intervalIsSelected = ( iinterval == selectedInterval );
				const bool labelDoesMatch = Melder_stringMatchesCriterion (interval -> text.get(),
						our instancePref_greenMethod(), our instancePref_greenString(), true);
				Melder_clipLeft (our startWindow(), & startInterval);
				Melder_clipRight (& endInterval, our endWindow());
				const double bottom = 1.0 - double (itier) / numberOfTiers;
				const double top = 1.0 - double (itier - 1) / numberOfTiers;
				if (labelDoesMatch) {
					Graphics_setColour (our graphics(), Melder_LIME);
					Graphics_fillRectangle (our graphics(), startInterval, endInterval, bottom, top);
				}
				if (intervalIsSelected) {
					Graphics_setColour (our graphics(), Melder_YELLOW);
					Graphics_fillRectangle (our graphics(),
						labelDoesMatch ? 0.85 * startInterval + 0.15 * endInterval : startInterval,
						labelDoesMatch ? 0.15 * startInterval + 0.85 * endInterval : endInterval,
						labelDoesMatch ? 0.85 * bottom + 0.15 * top : bottom,
						labelDoesMatch ? 0.15 * bottom + 0.85 * top : top
					);
				}
			}
		}
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
}
static void do_drawIntervalTier (TextGridArea me, IntervalTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	integer x1DC, x2DC, yDC;
	Graphics_WCtoDC (my graphics(), my startWindow(), 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics(), my endWindow(), 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics(), my instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (my graphics(), my instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (my graphics(), my instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (my graphics(), my instancePref_useTextStyles());

	const integer selectedInterval = ( itier == my selectedTier ? getSelectedInterval (me) : 0 );
	const integer numberOfIntervals = tier -> intervals.size;

	/*
		Draw a grey bar and a selection button at the cursor position.
	*/
	if (my startSelection() == my endSelection() && my startSelection() >= my startWindow() && my startSelection() <= my endWindow()) {
		/* mutable search */ bool cursorAtBoundary = false;
		for (integer iinterval = 2; iinterval <= numberOfIntervals; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			if (interval -> xmin == my startSelection())
				cursorAtBoundary = true;
		}
		if (! cursorAtBoundary) {
			const double dy = Graphics_dyMMtoWC (my graphics(), 1.5);
			Graphics_setGrey (my graphics(), 0.8);
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), my startSelection(), 0.0, my startSelection(), 1.0);
			Graphics_setLineWidth (my graphics(), 1.0);
			Graphics_setColour (my graphics(), Melder_BLUE);
			Graphics_circle_mm (my graphics(), my startSelection(), 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics(), my instancePref_alignment(), Graphics_HALF);
	for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
		const TextInterval interval = tier -> intervals.at [iinterval];
		/* mutable clip */ double startInterval = interval -> xmin, endInterval = interval -> xmax;
		Melder_clipLeft (my tmin(), & startInterval);
		Melder_clipRight (& endInterval, my tmax());
		if (startInterval >= endInterval)
			continue;
		const bool intervalIsSelected = ( selectedInterval == iinterval );

		/*
			Draw left boundary.
		*/
		if (startInterval >= my startWindow() && startInterval <= my endWindow() && iinterval > 1) {
			const bool boundaryIsSelected = ( my selectedTier == itier && startInterval == my startSelection() );
			Graphics_setColour (my graphics(), boundaryIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), startInterval, 0.0, startInterval, 1.0);

			/*
				Show alignment with cursor.
			*/
			if (startInterval == my startSelection()) {
				Graphics_setColour (my graphics(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics(), startInterval, 0.0, startInterval, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics(), 1.0);

		/*
			Draw label text.
		*/
		if (interval -> text && endInterval >= my startWindow() && startInterval <= my endWindow()) {
			const double t1 = std::max (my startWindow(), startInterval);
			const double t2 = std::min (my endWindow(), endInterval);
			Graphics_setColour (my graphics(), intervalIsSelected ? Melder_RED : Melder_BLACK);
			Graphics_textRect (my graphics(), t1, t2, 0.0, 1.0, interval -> text.get());
			Graphics_setColour (my graphics(), Melder_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics(), true);
	Graphics_setNumberSignIsBold (my graphics(), true);
	Graphics_setCircumflexIsSuperscript (my graphics(), true);
	Graphics_setUnderscoreIsSubscript (my graphics(), true);
}

static void do_drawTextTier (TextGridArea me, TextTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	const integer numberOfPoints = tier -> points.size;
	Graphics_setPercentSignIsItalic (my graphics(), my instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (my graphics(), my instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (my graphics(), my instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (my graphics(), my instancePref_useTextStyles());

	/*
		Draw a grey bar and a selection button at the cursor position.
	*/
	if (my startSelection() == my endSelection() && my startSelection() >= my startWindow() && my startSelection() <= my endWindow()) {
		bool cursorAtPoint = false;
		for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
			const TextPoint point = tier -> points.at [ipoint];
			if (point -> number == my startSelection())
				cursorAtPoint = true;
		}
		if (! cursorAtPoint) {
			const double dy = Graphics_dyMMtoWC (my graphics(), 1.5);
			Graphics_setGrey (my graphics(), 0.8);
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), my startSelection(), 0.0, my startSelection(), 1.0);
			Graphics_setLineWidth (my graphics(), 1.0);
			Graphics_setColour (my graphics(), Melder_BLUE);
			Graphics_circle_mm (my graphics(), my startSelection(), 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
	for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
		const TextPoint point = tier -> points.at [ipoint];
		const double t = point -> number;
		if (t >= my startWindow() && t <= my endWindow()) {
			const bool pointIsSelected = ( itier == my selectedTier && t == my startSelection() );
			Graphics_setColour (my graphics(), pointIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics(), t, 0.0, t, 0.2);
			Graphics_line (my graphics(), t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics(), 1.0);

			/*
				Wipe out the cursor where the text is going to be.
			*/
			Graphics_setColour (my graphics(), Melder_WHITE);
			Graphics_line (my graphics(), t, 0.2, t, 0.8);

			/*
				Show alignment with cursor.
			*/
			if (my startSelection() == my endSelection() && t == my startSelection()) {
				Graphics_setColour (my graphics(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics(), t, 0.0, t, 0.2);
				Graphics_line (my graphics(), t, 0.8, t, 1.0);
				Graphics_setLineWidth (my graphics(), 1.0);
			}
			Graphics_setColour (my graphics(), pointIsSelected ? Melder_RED : Melder_BLUE);
			if (point -> mark)
				Graphics_text (my graphics(), t, 0.5, point -> mark.get());
		}
	}
	Graphics_setPercentSignIsItalic (my graphics(), true);
	Graphics_setNumberSignIsBold (my graphics(), true);
	Graphics_setCircumflexIsSuperscript (my graphics(), true);
	Graphics_setUnderscoreIsSubscript (my graphics(), true);
}
void structTextGridArea :: v_drawInside () {
	Graphics_Viewport vp2;
	const integer numberOfTiers = our textGrid() -> tiers->size;
	const enum kGraphics_font oldFont = Graphics_inqFont (our graphics());
	const double oldFontSize = Graphics_inqFontSize (our graphics());
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, 1.0);
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		const Function anyTier = our textGrid() -> tiers->at [itier];
		const bool tierIsSelected = ( itier == our selectedTier );
		const bool isIntervalTier = ( anyTier -> classInfo == classIntervalTier );
		vp2 = Graphics_insetViewport (our graphics(), 0.0, 1.0,
				1.0 - (double) itier / (double) numberOfTiers,
				1.0 - (double) (itier - 1) / (double) numberOfTiers);
		Graphics_setColour (our graphics(), Melder_BLACK);
		if (itier != 1)
			Graphics_line (our graphics(), our startWindow(), 1.0, our endWindow(), 1.0);

		/*
			Show the number and the name of the tier.
		*/
		Graphics_setColour (our graphics(), tierIsSelected ? Melder_RED : Melder_BLACK);
		Graphics_setFont (our graphics(), oldFont);
		Graphics_setFontSize (our graphics(), 14);
		Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics(), our startWindow(), 0.5,   tierIsSelected ? U"☞ " : U"", itier);
		Graphics_setFontSize (our graphics(), oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (our graphics(), Graphics_LEFT,
					our instancePref_showNumberOf() == kTextGridArea_showNumberOf::NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (our graphics(), our endWindow(), 0.5, anyTier -> name.get());
		}
		if (our instancePref_showNumberOf() != kTextGridArea_showNumberOf::NOTHING) {
			Graphics_setTextAlignment (our graphics(), Graphics_LEFT, Graphics_TOP);
			if (our instancePref_showNumberOf() == kTextGridArea_showNumberOf::INTERVALS_OR_POINTS) {
				const integer count = ( isIntervalTier ? ((IntervalTier) anyTier) -> intervals.size : ((TextTier) anyTier) -> points.size );
				const integer position = ( itier == our selectedTier ? ( isIntervalTier ? getSelectedInterval (this) : getSelectedPoint (this) ) : 0 );
				if (position)
					Graphics_text (our graphics(), our endWindow(), 0.5,   U"(", position, U"/", count, U")");
				else
					Graphics_text (our graphics(), our endWindow(), 0.5,   U"(", count, U")");
			} else {
				Melder_assert (our instancePref_showNumberOf() == kTextGridArea_showNumberOf::NONEMPTY_INTERVALS_OR_POINTS);
				integer count = 0;
				if (isIntervalTier) {
					const IntervalTier tier = (IntervalTier) anyTier;
					const integer numberOfIntervals = tier -> intervals.size;
					for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
						const TextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> text && interval -> text [0] != U'\0')
							count ++;
					}
				} else {
					const TextTier tier = (TextTier) anyTier;
					const integer numberOfPoints = tier -> points.size;
					for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
						const TextPoint point = tier -> points.at [ipoint];
						if (point -> mark && point -> mark [0] != U'\0')
							count ++;
					}
				}
				Graphics_text (our graphics(), our endWindow(), 0.5,   U"(##", count, U"#)");
			}
		}

		Graphics_setColour (our graphics(), Melder_BLACK);
		Graphics_setFont (our graphics(), kGraphics_font::TIMES);
		Graphics_setFontSize (our graphics(), our instancePref_fontSize());
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (our graphics(), vp2);
	}
	Graphics_setColour (our graphics(), Melder_BLACK);
	Graphics_setFont (our graphics(), oldFont);
	Graphics_setFontSize (our graphics(), oldFontSize);

	if (isdefined (our draggingTime) && hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
		Graphics_xorOn (our graphics(), Melder_MAROON);
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			if (our draggingTiers [itier]) {
				const double ymin = 1.0 - (double) itier / numberOfTiers;
				const double ymax = 1.0 - (double) (itier - 1) / numberOfTiers;
				Graphics_setLineWidth (our graphics(), 7.0);
				Graphics_line (our graphics(), our draggingTime, ymin, our draggingTime, ymax);
			}
		}
		our functionEditor() -> viewDataAsWorldByFraction ();
		Graphics_setLineWidth (our graphics(), 1.0);
		Graphics_line (our graphics(), our draggingTime, 0.0, our draggingTime, 1.0);
		Graphics_text (our graphics(), our draggingTime, 1.0, Melder_fixed (our draggingTime, 6));
		Graphics_xorOff (our graphics());
	}
}


#pragma mark - TextGridArea tracking

static integer localY_fraction_toTier (TextGridArea me, double localY_fraction) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	integer tierNumber = numberOfTiers - Melder_ifloor (localY_fraction * (double) numberOfTiers);
	Melder_clip (1_integer, & tierNumber, numberOfTiers);
	return tierNumber;
}

bool structTextGridArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	const integer numberOfTiers = our textGrid() -> tiers->size;
	const integer oldSelectedTier = our selectedTier;

	constexpr double clickingVicinityRadius_mm = 1.0;
	constexpr double draggingVicinityRadius_mm = clickingVicinityRadius_mm + 0.2;   // must be greater than `clickingVicinityRadius_mm`
	constexpr double droppingVicinityRadius_mm = 1.5;

	const integer mouseTier = localY_fraction_toTier (this, localY_fraction);

	our draggingTime = undefined;   // information to next expose event
	if (event -> isClick()) {
		if (isdefined (our anchorTime))   // sanity check for the fixed order click-drag-drop
			return false;
		Melder_assert (our clickedLeftBoundary == 0);
		Melder_assert (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce);
		our draggingTiers.reset();
		/*
			The user clicked in the grid part.
			We select the tier in which they clicked.
		*/
		our selectedTier = mouseTier;
		double startInterval, endInterval;
		timeToInterval (this, x_world, our selectedTier, & startInterval, & endInterval);

		if (event -> isLeftBottomFunctionKeyPressed()) {
			our setSelection (x_world - startInterval < endInterval - x_world ? startInterval : endInterval, our endSelection());   // to nearest boundary
			return FunctionEditor_UPDATE_NEEDED;
		}
		if (event -> isRightBottomFunctionKeyPressed()) {
			our setSelection (our startSelection(), x_world - startInterval < endInterval - x_world ? startInterval : endInterval);
			return FunctionEditor_UPDATE_NEEDED;
		}

		IntervalTier selectedIntervalTier;
		TextTier selectedTextTier;
		AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our selectedTier], & selectedIntervalTier, & selectedTextTier);

		if (x_world <= our startWindow() || x_world >= our endWindow())
			return FunctionEditor_UPDATE_NEEDED;

		/*
			Get the time of the nearest boundary or point.
		*/
		if (selectedIntervalTier) {
			const integer clickedIntervalNumber = IntervalTier_timeToIndex (selectedIntervalTier, x_world);
			const bool theyClickedOutsidetheTimeDomainOfTheIntervals = ( clickedIntervalNumber == 0 );
			if (theyClickedOutsidetheTimeDomainOfTheIntervals)
				return FunctionEditor_UPDATE_NEEDED;
			const TextInterval interval = selectedIntervalTier -> intervals.at [clickedIntervalNumber];
			if (x_world > 0.5 * (interval -> xmin + interval -> xmax)) {
				our anchorTime = interval -> xmax;
				our clickedLeftBoundary = clickedIntervalNumber + 1;
			} else {
				our anchorTime = interval -> xmin;
				our clickedLeftBoundary = clickedIntervalNumber;
			}
		} else {
			const integer clickedPointNumber = AnyTier_timeToNearestIndex (selectedTextTier->asAnyTier(), x_world);
			if (clickedPointNumber != 0) {
				const TextPoint point = selectedTextTier -> points.at [clickedPointNumber];
				our anchorTime = point -> number;
			}
		}
		Melder_assert (! (selectedIntervalTier && our clickedLeftBoundary == 0));

		const bool nearBoundaryOrPoint = ( isdefined (our anchorTime) &&
				fabs (Graphics_dxWCtoMM (our graphics(), x_world - our anchorTime)) < 1.5 );
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, 1.0);
		const double distanceToCursorCircle = ( our startSelection() != our endSelection() ? undefined :
			Graphics_distanceWCtoMM (our graphics(), x_world, localY_fraction,
				our startSelection(),
				(numberOfTiers + 1 - our selectedTier) / double (numberOfTiers) -
				Graphics_dyMMtoWC (our graphics(), 1.5))
		);
		trace (localY_fraction, U" ", distanceToCursorCircle);
		const bool nearCursorCircle = ( distanceToCursorCircle < 1.5 );

		if (nearBoundaryOrPoint) {
			/*
				Possibility 1: the user clicked near a boundary or point.
				Perhaps drag it.
			*/
			bool boundaryOrPointIsMovable = true;
			if (selectedIntervalTier) {
				const bool isLeftEdgeOfFirstInterval = ( our clickedLeftBoundary <= 1 );
				const bool isRightEdgeOfLastInterval = ( our clickedLeftBoundary > selectedIntervalTier -> intervals.size );
				boundaryOrPointIsMovable = ! isLeftEdgeOfFirstInterval && ! isRightEdgeOfLastInterval;
			}
			/*
				If the user clicked on an unselected boundary or point, we extend or shrink the selection to it.
			*/
			if (event -> shiftKeyPressed) {
				if (our anchorTime > 0.5 * (our startSelection() + our endSelection()))
					our setSelection (our startSelection(), our anchorTime);
				else
					our setSelection (our anchorTime, our endSelection());
			}
			if (! boundaryOrPointIsMovable) {
				our draggingTime = undefined;
				our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
				our anchorTime = undefined;
				our clickedLeftBoundary = 0;
				return FunctionEditor_UPDATE_NEEDED;
			}
			/*
				Determine the set of selected boundaries and points, and the dragging range.
			*/
			our draggingTiers = zero_BOOLVEC (numberOfTiers);
			our leftDraggingBoundary = our tmin();
			our rightDraggingBoundary = our tmax();
			for (int itier = 1; itier <= numberOfTiers; itier ++) {
				/*
					If the user has pressed the shift key, let her drag all the boundaries and points at this time.
					Otherwise, let her only drag the boundary or point on the clicked tier.
				*/
				if (itier == mouseTier || our functionEditor() -> clickWasModifiedByShiftKey == our instancePref_shiftDragMultiple()) {
					IntervalTier intervalTier;
					TextTier textTier;
					AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [itier], & intervalTier, & textTier);
					if (intervalTier) {
						const integer ibound = IntervalTier_hasBoundary (intervalTier, our anchorTime);
						if (ibound) {
							TextInterval leftInterval = intervalTier -> intervals.at [ibound - 1];
							TextInterval rightInterval = intervalTier -> intervals.at [ibound];
							our draggingTiers [itier] = true;
							/*
								Prevent the user from dragging the boundary past its left or right neighbours on the same tier.
							*/
							Melder_clipLeft (leftInterval -> xmin, & our leftDraggingBoundary);
							Melder_clipRight (& our rightDraggingBoundary, rightInterval -> xmax);
						}
					} else {
						Melder_assert (isdefined (our anchorTime));
						if (AnyTier_hasPoint (textTier->asAnyTier(), our anchorTime)) {
							/*
								Other than with boundaries on interval tiers,
								points on text tiers can be dragged past their neighbours.
							*/
							our draggingTiers [itier] = true;
						}
					}
				}
			}
		} else if (nearCursorCircle) {
			/*
				Possibility 2: the user clicked near the cursor circle.
				Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
				because we are not 'nearBoundaryOrPoint'.
			*/
			Melder_assert (isdefined (our startSelection()));   // precondition of v_updateText()
			if (our selectedTier != oldSelectedTier)
				our functionEditor() -> v_updateText();   // this puts the text of the newly clicked tier into the text area
			insertBoundaryOrPoint (this, mouseTier, our startSelection(), our startSelection(), false);
			//Melder_assert (isdefined (our startSelection));   // precondition of FunctionEditor_marksChanged()
			//FunctionEditor_marksChanged (this, true);
			Editor_broadcastDataChanged (our functionEditor());
		} else {
			/*
				Possibility 3: the user clicked in empty space.
				Select the interval, if any.
			*/
			if (selectedIntervalTier)
				our setSelection (startInterval, endInterval);
		}
	} else if (event -> isDrag ()) {
		if (isdefined (our anchorTime) && our draggingTiers.size > 0) {
			our draggingTime = x_world;
			if (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
				const double distanceToAnchor_mm = fabs (Graphics_dxWCtoMM (our graphics(), x_world - our anchorTime));
				if (distanceToAnchor_mm > draggingVicinityRadius_mm)
					our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = true;
			}
		}
	} else if (event -> isDrop ()) {
		if (isundef (our anchorTime) || our draggingTiers.size == 0) {   // TODO: figure out a circumstance under which anchorTime could be undefined
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			If the user shift-clicked, we extend the selection (this already happened during click()).
		*/
		if (event -> shiftKeyPressed && ! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			If the user dropped near an existing boundary in an unselected tier,
			we snap to that mark.
		*/
		const integer itierDrop = localY_fraction_toTier (this, localY_fraction);
		bool droppedOnABoundaryOrPointInsideAnUnselectedTier = false;
		if (x_world > 0.0 && ! our draggingTiers [itierDrop]) {   // dropped inside an unselected tier?
			const Function anyTierDrop = our textGrid() -> tiers->at [itierDrop];
			if (anyTierDrop -> classInfo == classIntervalTier) {
				const IntervalTier tierDrop = (IntervalTier) anyTierDrop;
				for (integer ibound = 1; ibound < tierDrop -> intervals.size; ibound ++) {
					const TextInterval left = tierDrop -> intervals.at [ibound];
					const double mouseDistanceToBoundary = fabs (Graphics_dxWCtoMM (our graphics(), x_world - left -> xmax));
					if (mouseDistanceToBoundary < droppingVicinityRadius_mm) {
						x_world = left -> xmax;   // snap to boundary
						droppedOnABoundaryOrPointInsideAnUnselectedTier = true;
					}
				}
			} else {
				const TextTier tierDrop = (TextTier) anyTierDrop;
				for (integer ipoint = 1; ipoint <= tierDrop -> points.size; ipoint ++) {
					const TextPoint point = tierDrop -> points.at [ipoint];
					const double mouseDistanceToPoint_mm = fabs (Graphics_dxWCtoMM (our graphics(), x_world - point -> number));
					if (mouseDistanceToPoint_mm < droppingVicinityRadius_mm) {
						x_world = point -> number;   // snap to point
						droppedOnABoundaryOrPointInsideAnUnselectedTier = true;
					}
				}
			}
		}
		/*
			If the user dropped near the cursor (outside the anchor),
			we snap to the cursor.
		*/
		if (our startSelection() == our endSelection() && our startSelection() != our anchorTime) {
			const double mouseDistanceToCursor = fabs (Graphics_dxWCtoMM (our graphics(), x_world - our startSelection()));
			if (mouseDistanceToCursor < droppingVicinityRadius_mm)
				x_world = our startSelection();
		}
		/*
			If the user wiggled near the anchor, we snap to the anchor and bail out
			(a boundary has been selected, but nothing has been dragged).
		*/
		if (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce && ! droppedOnABoundaryOrPointInsideAnUnselectedTier) {
			our setSelection (our anchorTime, our anchorTime);
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}

		/*
			We cannot move a boundary out of the dragging range.
		*/
		if (x_world <= our leftDraggingBoundary || x_world >= our rightDraggingBoundary) {
			Melder_beep ();
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}

		FunctionArea_save (this, U"Drag");

		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			if (our draggingTiers [itier]) {
				IntervalTier intervalTier;
				TextTier textTier;
				AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [itier], & intervalTier, & textTier);
				if (intervalTier) {
					const integer numberOfIntervals = intervalTier -> intervals.size;
					for (integer ibound = 2; ibound <= numberOfIntervals; ibound ++) {
						TextInterval left = intervalTier -> intervals.at [ibound - 1], right = intervalTier -> intervals.at [ibound];
						if (left -> xmax == our anchorTime) {   // boundary dragged?
							left -> xmax = right -> xmin = x_world;   // move boundary to drop site
							break;
						}
					}
				} else {
					Melder_assert (isdefined (our anchorTime));
					const integer iDraggedPoint = AnyTier_hasPoint (textTier->asAnyTier(), our anchorTime);
					if (iDraggedPoint) {
						Melder_assert (isdefined (x_world));
						const integer dropSiteHasPoint = AnyTier_hasPoint (textTier->asAnyTier(), x_world);
						if (dropSiteHasPoint != 0) {
							Melder_warning (U"Cannot drop point on an existing point.");
						} else {
							const TextPoint point = textTier -> points.at [iDraggedPoint];
							/*
								Move point to drop site. May have passed another point.
							*/
							autoTextPoint newPoint = Data_copy (point);
							newPoint -> number = x_world;   // move point to drop site
							textTier -> points. removeItem (iDraggedPoint);
							textTier -> points. addItem_move (newPoint.move());
						}
					}
				}
			}
		}

		/*
			Select the drop site.
		*/
		our setSelection (x_world, x_world);
		our draggingTime = undefined;
		our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
		our anchorTime = undefined;
		our clickedLeftBoundary = 0;
		//Melder_assert (isdefined (our startSelection));   // precondition of FunctionEditor_marksChanged()
		//FunctionEditor_marksChanged (this, true);
		Editor_broadcastDataChanged (our functionEditor());
	}
	return FunctionEditor_UPDATE_NEEDED;
}


#pragma mark - TextGridArea File menu

static void menu_cb_SaveWholeTextGridAsTextFile (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save whole TextGrid as text file", nullptr)
		Melder_sprint (defaultName,300, my textGrid() -> name.get(), U".TextGrid");
	EDITOR_DO_SAVE
		Data_writeToTextFile (my textGrid(), file);
	EDITOR_END
}
void structTextGridArea :: v_createMenuItems_file (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"Save TextGrid to disk:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Save whole TextGrid as text file...", 'S',
			menu_cb_SaveWholeTextGridAsTextFile, this);
	FunctionAreaMenu_addCommand (menu, U"-- after TextGrid save --", 0, nullptr, this);
}


#pragma mark - TextGridArea Query menu

static void QUERY_DATA_FOR_REAL__GetStartingPointOfInterval (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		checkTierSelection (me, U"query the starting point of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection());
		const double result = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmin );
	QUERY_DATA_FOR_REAL_END (U" seconds")
}

static void QUERY_DATA_FOR_REAL__GetEndPointOfInterval (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		checkTierSelection (me, U"query the end point of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection());
		const double result = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmax );
	QUERY_DATA_FOR_REAL_END (U" seconds")
}

static void QUERY_DATA_FOR_STRING__GetLabelOfInterval (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_STRING
		checkTierSelection (me, U"query the label of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection());
		const conststring32 result = ( iinterval < 1 || iinterval > tier -> intervals.size ? U"" :
				tier -> intervals.at [iinterval] -> text.get() );
	QUERY_DATA_FOR_STRING_END
}
void structTextGridArea :: v_createMenuItems_query (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- query interval --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Query interval:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Get starting point of interval", 0,
			QUERY_DATA_FOR_REAL__GetStartingPointOfInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Get end point of interval", 0,
			QUERY_DATA_FOR_REAL__GetEndPointOfInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Get label of interval", 0,
			QUERY_DATA_FOR_STRING__GetLabelOfInterval, this);
}


#pragma mark - TextGridArea changing

static void gui_text_cb_changed (TextGridArea me, GuiTextEvent /* event */) {
	trace (my suppressRedraw);
	if (my suppressRedraw)
		return;   // prevent infinite loop if 'draw' method or Editor_broadcastChange calls GuiText_setString
	if (my selectedTier) {
		autostring32 text = GuiText_getString (my functionEditor() -> textArea);
		IntervalTier intervalTier;
		TextTier textTier;
		AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer selectedInterval = getSelectedInterval (me);
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals.at [selectedInterval];
				TextInterval_setText (interval, text.get());
				Editor_broadcastDataChanged (my functionEditor());
			}
		} else {
			const integer selectedPoint = getSelectedPoint (me);
			if (selectedPoint) {
				TextPoint point = textTier -> points.at [selectedPoint];
				point -> mark. reset();
				if (Melder_findInk (text.get()))   // any visible characters?
					point -> mark = Melder_dup_f (text.get());
				Editor_broadcastDataChanged (my functionEditor());
			}
		}
	}
}


#pragma mark - TextGridArea View menu

static void do_selectAdjacentTier (TextGridArea me, bool previous) {
	const integer n = my textGrid() -> tiers->size;
	if (n >= 2) {
		my selectedTier = ( previous ?
				my selectedTier > 1 ? my selectedTier - 1 : n :
				my selectedTier < n ? my selectedTier + 1 : 1 );
		double startInterval, endInterval;
		timeToInterval (me, my startSelection(), my selectedTier, & startInterval, & endInterval);
		my setSelection (startInterval, endInterval);
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
	}
}
static void menu_cb_SelectPreviousTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, true);
}
static void menu_cb_SelectNextTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, false);
}
static void do_selectAdjacentInterval (TextGridArea me, bool previous, bool shift) {
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > my textGrid() -> tiers->size)
		return;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		const integer n = intervalTier -> intervals.size;
		if (n >= 2) {
			integer iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection());
			if (shift) {
				const integer binterval = IntervalTier_timeToIndex (intervalTier, my startSelection());
				integer einterval = IntervalTier_timeToIndex (intervalTier, my endSelection());
				if (my endSelection() == intervalTier -> xmax)
					einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					const TextInterval interval = intervalTier -> intervals.at [iinterval];
					my setSelection (interval -> xmin, interval -> xmax);
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							const TextInterval interval = intervalTier -> intervals.at [einterval - 1];
							my setSelection (my startSelection(), interval -> xmin);
						}
					} else if (binterval > 1) {
						const TextInterval interval = intervalTier -> intervals.at [binterval - 1];
						my setSelection (interval -> xmin, my endSelection());
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							const TextInterval interval = intervalTier -> intervals.at [binterval];
							my setSelection (interval -> xmax, my endSelection());
						}
					} else if (einterval <= n) {
						const TextInterval interval = intervalTier -> intervals.at [einterval];
						my setSelection (my startSelection(), interval -> xmax);
					}
				}
			} else {
				iinterval = ( previous ?
						iinterval > 1 ? iinterval - 1 : n :
						iinterval < n ? iinterval + 1 : 1 );
				const TextInterval interval = intervalTier -> intervals.at [iinterval];
				my setSelection (interval -> xmin, interval -> xmax);
			}
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToView()
			FunctionEditor_scrollToView (my functionEditor(), iinterval == n ? my startSelection() : iinterval == 1 ? my endSelection() : 0.5 * (my startSelection() + my endSelection()));
		}
	} else {
		const integer n = textTier -> points.size;
		if (n >= 2) {
			integer ipoint = AnyTier_timeToHighIndex (textTier->asAnyTier(), my startSelection());
			ipoint = ( previous ?
					ipoint > 1 ? ipoint - 1 : n :
					ipoint < n ? ipoint + 1 : 1 );
			const TextPoint point = textTier -> points.at [ipoint];
			my setSelection (point -> number, point -> number);
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_scrollToView()
			FunctionEditor_scrollToView (my functionEditor(), my startSelection());
		}
	}
}
static void menu_cb_SelectPreviousInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, false);
}
static void menu_cb_SelectNextInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, false);
}
static void menu_cb_ExtendSelectPreviousInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, true);
}
static void menu_cb_ExtendSelectNextInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, true);
}
void structTextGridArea :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- TextGrid view domain --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"View TextGrid time domain:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW,
			menu_cb_SelectPreviousTier, this);
	FunctionAreaMenu_addCommand (menu, U"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW,
			menu_cb_SelectNextTier, this);
	FunctionAreaMenu_addCommand (menu, U"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW,
			menu_cb_SelectPreviousInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW,
			menu_cb_SelectNextInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW,
			menu_cb_ExtendSelectPreviousInterval, this);
	FunctionAreaMenu_addCommand (menu, U"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW,
			menu_cb_ExtendSelectNextInterval, this);
}


#pragma mark - TextGridArea Draw menu

static void menu_cb_DrawVisibleTextGrid (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		Editor_openPraatPicture (my functionEditor());
		TextGrid_Sound_draw (my textGrid(), nullptr, my functionEditor() -> pictureGraphics,
				my startWindow(), my endWindow(), true, my instancePref_useTextStyles(), garnish);
		FunctionEditor_garnish (my functionEditor());
		Editor_closePraatPicture (my functionEditor());
	EDITOR_END
}
static void menu_cb_DrawVisibleSoundAndTextGrid (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible sound and TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish())
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		Editor_openPraatPicture (my functionEditor());
		{// scope
			autoSound sound = (
				my borrowedSoundArea -> longSound() ?
					LongSound_extractPart (my borrowedSoundArea -> longSound(),
							my startWindow(), my endWindow(), true)
				:	Sound_extractPart (my borrowedSoundArea -> sound(),
							my startWindow(), my endWindow(), kSound_windowShape::RECTANGULAR, 1.0, true)
			);
			TextGrid_Sound_draw (my textGrid(), sound.get(), my functionEditor() -> pictureGraphics,
					my startWindow(), my endWindow(), true, my instancePref_useTextStyles(), garnish);
		}
		FunctionEditor_garnish (my functionEditor());
		Editor_closePraatPicture (my functionEditor());
	EDITOR_END
}
static void menu_cb_DrawTextGridAndPitch (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw TextGrid and Pitch separately", nullptr)
		my v_form_pictureWindow (cmd);
		LABEL (U"TextGrid:")
		BOOLEAN (showBoundariesAndPoints, U"Show boundaries and points", my default_picture_showBoundaries ());
		LABEL (U"Pitch:")
		BOOLEAN (speckle, U"Speckle", my default_picture_pitch_speckle ());
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (showBoundariesAndPoints, my classPref_picture_showBoundaries())
		SET_BOOLEAN (speckle, my classPref_picture_pitch_speckle())
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my classPref_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my setClassPref_picture_showBoundaries (showBoundariesAndPoints);   // set prefs even if analyses are missing (it would be annoying not to)
		my setClassPref_picture_pitch_speckle (speckle);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setClassPref_picture_garnish (garnish);
		SoundAnalysisArea_haveVisiblePitch (my borrowedSoundAnalysisArea);
		Editor_openPraatPicture (my functionEditor());
		const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my borrowedSoundAnalysisArea -> d_pitch.get(),
				my borrowedSoundAnalysisArea -> instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) my borrowedSoundAnalysisArea -> instancePref_pitch_unit());
		const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my borrowedSoundAnalysisArea -> d_pitch.get(),
				my borrowedSoundAnalysisArea -> instancePref_pitch_ceiling(), Pitch_LEVEL_FREQUENCY, (int) my borrowedSoundAnalysisArea -> instancePref_pitch_unit());
		const double pitchFloor_overt = Function_convertToNonlogarithmic (my borrowedSoundAnalysisArea -> d_pitch.get(),
				pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) my borrowedSoundAnalysisArea -> instancePref_pitch_unit());
		const double pitchCeiling_overt = Function_convertToNonlogarithmic (my borrowedSoundAnalysisArea -> d_pitch.get(),
				pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) my borrowedSoundAnalysisArea -> instancePref_pitch_unit());
		const double pitchViewFrom_overt = ( my borrowedSoundAnalysisArea -> instancePref_pitch_viewFrom() < my borrowedSoundAnalysisArea -> instancePref_pitch_viewTo() ? my borrowedSoundAnalysisArea -> instancePref_pitch_viewFrom() : pitchFloor_overt );
		const double pitchViewTo_overt = ( my borrowedSoundAnalysisArea -> instancePref_pitch_viewFrom() < my borrowedSoundAnalysisArea -> instancePref_pitch_viewTo() ? my borrowedSoundAnalysisArea -> instancePref_pitch_viewTo() : pitchCeiling_overt );
		TextGrid_Pitch_drawSeparately (my textGrid(), my borrowedSoundAnalysisArea -> d_pitch.get(), my functionEditor() -> pictureGraphics, my startWindow(), my endWindow(),
			pitchViewFrom_overt, pitchViewTo_overt, showBoundariesAndPoints, my instancePref_useTextStyles(), garnish,
			speckle, my borrowedSoundAnalysisArea -> instancePref_pitch_unit()
		);
		FunctionEditor_garnish (my functionEditor());
		Editor_closePraatPicture (my functionEditor());
	EDITOR_END
}
static void addTextGridDrawMenu (TextGridArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- TextGrid draw --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Draw TextGrid to picture window:", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Draw visible TextGrid...", 0,
			menu_cb_DrawVisibleTextGrid, me);
	if (my borrowedSoundArea)
		FunctionAreaMenu_addCommand (menu, U"Draw visible sound and TextGrid...", 0,
				menu_cb_DrawVisibleSoundAndTextGrid, me);
	if (my borrowedSoundAnalysisArea)
		FunctionAreaMenu_addCommand (menu, U"Draw visible pitch contour and TextGrid...", 0,
				menu_cb_DrawTextGridAndPitch, me);
}


#pragma mark - TextGridArea Extract menu

static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (my endSelection() <= my startSelection())
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection(), my endSelection(), true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (my endSelection() <= my startSelection())
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection(), my endSelection(), false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void addTextGridExtractMenu (TextGridArea me, EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- TextGrid extract --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Extract TextGrid to objects window:", 0, nullptr, me);
	my extractSelectedTextGridPreserveTimesButton = FunctionAreaMenu_addCommand (menu, U"Extract selected TextGrid (preserve times)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes, me);
	my extractSelectedTextGridTimeFromZeroButton = FunctionAreaMenu_addCommand (menu, U"Extract selected TextGrid (time from 0)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero, me);
}


#pragma mark - TextGridArea Interval menu

static void menu_cb_AlignInterval (TextGridArea me, EDITOR_ARGS_DIRECT) {
	checkTierSelection (me, U"align words");
	const AnyTier tier = static_cast <AnyTier> (my textGrid() -> tiers->at [my selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Alignment works only for interval tiers, whereas tier ", my selectedTier, U" is a point tier.\nSelect an interval tier instead.");
	const integer intervalNumber = getSelectedInterval (me);
	if (! intervalNumber)
		Melder_throw (U"Select an interval first");
	if (! my instancePref_align_includeWords() && ! my instancePref_align_includePhonemes())
		Melder_throw (U"Nothing to be done.\nPlease switch on \"Include words\" and/or \"Include phonemes\" in the \"Alignment settings\".");
	{// scope
		const autoMelderProgressOff noprogress;
		FunctionArea_save (me, U"Align interval");
		TextGrid_anySound_alignInterval (my textGrid(), my borrowedSoundArea -> soundOrLongSound(), my selectedTier, intervalNumber,
				my instancePref_align_language(), my instancePref_align_includeWords(), my instancePref_align_includePhonemes());
	}
	Editor_broadcastDataChanged (my functionEditor());
}
static void menu_cb_AlignmentSettings (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Alignment settings", nullptr)
		OPTIONMENU (language, U"Language", (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)"))
		for (integer i = 1; i <= espeakdata_languages_names -> numberOfStrings; i ++) {
			OPTION ((conststring32) espeakdata_languages_names -> strings [i].get());
		}
		BOOLEAN (includeWords,    U"Include words",    my default_align_includeWords ())
		BOOLEAN (includePhonemes, U"Include phonemes", my default_align_includePhonemes ())
		BOOLEAN (allowSilences,   U"Allow silences",   my default_align_allowSilences ())
	EDITOR_OK
		int prefVoice = (int) Strings_findString (espeakdata_languages_names.get(), my instancePref_align_language());
		if (prefVoice == 0)
			prefVoice = (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)");
		SET_OPTION (language, prefVoice)
		SET_BOOLEAN (includeWords, my instancePref_align_includeWords())
		SET_BOOLEAN (includePhonemes, my instancePref_align_includePhonemes())
		SET_BOOLEAN (allowSilences, my instancePref_align_allowSilences())
	EDITOR_DO
		my setInstancePref_align_language (espeakdata_languages_names -> strings [language].get());
		my setInstancePref_align_includeWords (includeWords);
		my setInstancePref_align_includePhonemes (includePhonemes);
		my setInstancePref_align_allowSilences (allowSilences);
	EDITOR_END
}
static void do_insertIntervalOnTier (TextGridArea me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my startSelection(),
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my endSelection(),
			true
		);
		my selectedTier = itier;
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
		Editor_broadcastDataChanged (my functionEditor());
	} catch (MelderError) {
		Melder_throw (U"Interval not inserted.");
	}
}
static void menu_cb_InsertIntervalOnTier1 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 1); }
static void menu_cb_InsertIntervalOnTier2 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 2); }
static void menu_cb_InsertIntervalOnTier3 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 3); }
static void menu_cb_InsertIntervalOnTier4 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 4); }
static void menu_cb_InsertIntervalOnTier5 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 5); }
static void menu_cb_InsertIntervalOnTier6 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 6); }
static void menu_cb_InsertIntervalOnTier7 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 7); }
static void menu_cb_InsertIntervalOnTier8 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 8); }

static void addIntervalMenu (TextGridArea me) {
	EditorMenu menu = Editor_addMenu (my functionEditor(), U"Interval", 0);
	if (my borrowedSoundArea) {
		FunctionAreaMenu_addCommand (menu, U"Align interval", 'D',
				menu_cb_AlignInterval, me);
		FunctionAreaMenu_addCommand (menu, U"Alignment settings...", 0,
				menu_cb_AlignmentSettings, me);
		FunctionAreaMenu_addCommand (menu, U"-- add interval --", 0, nullptr, me);
	}
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 1", GuiMenu_COMMAND | '1',
			menu_cb_InsertIntervalOnTier1, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 2", GuiMenu_COMMAND | '2',
			menu_cb_InsertIntervalOnTier2, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 3", GuiMenu_COMMAND | '3',
			menu_cb_InsertIntervalOnTier3, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 4", GuiMenu_COMMAND | '4',
			menu_cb_InsertIntervalOnTier4, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 5", GuiMenu_COMMAND | '5',
			menu_cb_InsertIntervalOnTier5, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 6", GuiMenu_COMMAND | '6',
			menu_cb_InsertIntervalOnTier6, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 7", GuiMenu_COMMAND | '7',
			menu_cb_InsertIntervalOnTier7, me);
	FunctionAreaMenu_addCommand (menu, U"Add interval on tier 8", GuiMenu_COMMAND | '8',
			menu_cb_InsertIntervalOnTier8, me);
}


#pragma mark - TextGridArea Boundary menu

static void menu_cb_RemovePointOrBoundary (TextGridArea me, EDITOR_ARGS_DIRECT) {
	checkTierSelection (me, U"remove a point or boundary");
	const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (selectedLeftBoundary == 0)
			Melder_throw (U"To remove a boundary, first click on it.");

		FunctionArea_save (me, U"Remove boundary");
		IntervalTier_removeLeftBoundary (tier, selectedLeftBoundary);
	} else {
		const TextTier tier = (TextTier) anyTier;
		const integer selectedPoint = getSelectedPoint (me);
		if (selectedPoint == 0)
			Melder_throw (U"To remove a point, first click on it.");

		FunctionArea_save (me, U"Remove point");
		tier -> points. removeItem (selectedPoint);
	}
	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (my functionEditor());
}
static void do_movePointOrBoundary (TextGridArea me, int where) {
	if (where == 0 && ! (my borrowedSoundArea && my borrowedSoundArea -> sound()))
		return;
	checkTierSelection (me, U"move a point or boundary");
	const Function anyTier = my textGrid() -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		static const conststring32 boundarySaveText [3] { U"Move boundary to zero crossing", U"Move boundary to B", U"Move boundary to E" };
		const integer selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (selectedLeftBoundary == 0)
			Melder_throw (U"To move a boundary, first click on it.");
		const TextInterval left = tier -> intervals.at [selectedLeftBoundary - 1];
		const TextInterval right = tier -> intervals.at [selectedLeftBoundary];
		const double position = ( where == 1 ? my startSelection() : where == 2 ? my endSelection() :
				Sound_getNearestZeroCrossing (my borrowedSoundArea -> sound(), left -> xmax, 1) );   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");
		if (position <= left -> xmin || position >= right -> xmax)
			Melder_throw (U"Cannot move a boundary past its neighbour.");

		FunctionArea_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = position;
		my setSelection (position, position);
	} else {
		TextTier tier = (TextTier) anyTier;
		static const conststring32 pointSaveText [3] { U"Move point to zero crossing", U"Move point to B", U"Move point to E" };
		const integer selectedPoint = getSelectedPoint (me);
		if (selectedPoint == 0)
			Melder_throw (U"To move a point, first click on it.");
		const TextPoint point = tier -> points.at [selectedPoint];
		const double position = ( where == 1 ? my startSelection() : where == 2 ? my endSelection() :
				Sound_getNearestZeroCrossing (my borrowedSoundArea -> sound(), point -> number, 1) );   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");

		FunctionArea_save (me, pointSaveText [where]);

		point -> number = position;
		my setSelection (position, position);
	}
	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
	FunctionEditor_marksChanged (my functionEditor(), true);   // because cursor has moved
	Editor_broadcastDataChanged (my functionEditor());
}
static void menu_cb_MoveToB (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 1);
}
static void menu_cb_MoveToE (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 2);
}
static void menu_cb_MoveToZero (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 0);
}
static void do_insertOnTier (TextGridArea me, integer itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my startSelection(),
			my functionEditor() -> duringPlay ? my functionEditor() -> playCursor : my endSelection(),
			false
		);
		my selectedTier = itier;
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (my functionEditor(), true);
		Editor_broadcastDataChanged (my functionEditor());
	} catch (MelderError) {
		Melder_throw (U"Boundary or point not inserted.");
	}
}
static void menu_cb_InsertOnSelectedTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	do_insertOnTier (me, my selectedTier);
}
static void menu_cb_InsertOnTier1 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 1); }
static void menu_cb_InsertOnTier2 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 2); }
static void menu_cb_InsertOnTier3 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 3); }
static void menu_cb_InsertOnTier4 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 4); }
static void menu_cb_InsertOnTier5 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 5); }
static void menu_cb_InsertOnTier6 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 6); }
static void menu_cb_InsertOnTier7 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 7); }
static void menu_cb_InsertOnTier8 (TextGridArea me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 8); }

static void menu_cb_InsertOnAllTiers (TextGridArea me, EDITOR_ARGS_DIRECT) {
	const integer saveTier = my selectedTier;
	for (integer itier = 1; itier <= my textGrid() -> tiers->size; itier ++)
		do_insertOnTier (me, itier);
	my selectedTier = saveTier;   // only if everything went right; otherwise, the tier where something went wrong will stand selected
}
static void addBoundaryMenu (TextGridArea me) {
	EditorMenu menu = Editor_addMenu (my functionEditor(), U"Boundary", 0);
	/*FunctionAreaMenu_addCommand (menu, U"Move to B", 0, menu_cb_MoveToB, me);
	FunctionAreaMenu_addCommand (menu, U"Move to E", 0, menu_cb_MoveToE);*/
	if (my borrowedSoundArea && ! Thing_isa (my borrowedSoundArea, classLongSoundArea)) {
		FunctionAreaMenu_addCommand (menu, U"Move to nearest zero crossing", 0,
				menu_cb_MoveToZero, me);
		FunctionAreaMenu_addCommand (menu, U"-- insert boundary --", 0, nullptr, me);
	}
	FunctionAreaMenu_addCommand (menu, U"Add on selected tier", GuiMenu_ENTER,
			menu_cb_InsertOnSelectedTier, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 1", GuiMenu_COMMAND | GuiMenu_F1,
			menu_cb_InsertOnTier1, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 2", GuiMenu_COMMAND | GuiMenu_F2,
			menu_cb_InsertOnTier2, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 3", GuiMenu_COMMAND | GuiMenu_F3,
			menu_cb_InsertOnTier3, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 4", GuiMenu_COMMAND | GuiMenu_F4,
			menu_cb_InsertOnTier4, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 5", GuiMenu_COMMAND | GuiMenu_F5,
			menu_cb_InsertOnTier5, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 6", GuiMenu_COMMAND | GuiMenu_F6,
			menu_cb_InsertOnTier6, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 7", GuiMenu_COMMAND | GuiMenu_F7,
			menu_cb_InsertOnTier7, me);
	FunctionAreaMenu_addCommand (menu, U"Add on tier 8", GuiMenu_COMMAND | GuiMenu_F8,
			menu_cb_InsertOnTier8, me);
	FunctionAreaMenu_addCommand (menu, U"Add on all tiers", GuiMenu_COMMAND | GuiMenu_F9,
			menu_cb_InsertOnAllTiers, me);
	FunctionAreaMenu_addCommand (menu, U"-- remove mark --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Remove", GuiMenu_OPTION | GuiMenu_BACKSPACE,
			menu_cb_RemovePointOrBoundary, me);
}


#pragma mark - TextGridArea Tier menu

static void menu_cb_RenameTier (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Rename tier", nullptr)
		SENTENCE (newName, U"New name", U"");
	EDITOR_OK
		checkTierSelection (me, U"rename a tier");
		const Daata tier = my textGrid() -> tiers->at [my selectedTier];
		SET_STRING (newName, tier -> name ? tier -> name.get() : U"")
	EDITOR_DO
		checkTierSelection (me, U"rename a tier");
		const Function tier = my textGrid() -> tiers->at [my selectedTier];

		FunctionArea_save (me, U"Rename tier");

		Thing_setName (tier, newName);

		Editor_broadcastDataChanged (my functionEditor());
	EDITOR_END
}
static void CONVERT_DATA_TO_ONE__PublishTier (TextGridArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		checkTierSelection (me, U"publish a tier");
		const Function tier = my textGrid() -> tiers->at [my selectedTier];
		autoTextGrid result = TextGrid_createWithoutTiers (1e30, -1e30);
		TextGrid_addTier_copy (result.get(), tier);
	CONVERT_DATA_TO_ONE_END (tier -> name.get())
}
static void menu_cb_RemoveAllTextFromTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	checkTierSelection (me, U"remove all text from a tier");
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my selectedTier], & intervalTier, & textTier);

	FunctionArea_save (me, U"Remove text from tier");
	if (intervalTier)
		IntervalTier_removeText (intervalTier);
	else
		TextTier_removeText (textTier);

	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (my functionEditor());
}
static void menu_cb_RemoveTier (TextGridArea me, EDITOR_ARGS_DIRECT) {
	if (my textGrid() -> tiers->size <= 1)
		Melder_throw (U"Sorry, I refuse to remove the last tier.");
	checkTierSelection (me, U"remove a tier");

	FunctionArea_save (me, U"Remove tier");
	my textGrid() -> tiers-> removeItem (my selectedTier);

	//my textGridArea -> selectedTier = 1; TRY OUT 2022-07-23
	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (my functionEditor());
}
static void menu_cb_AddIntervalTier (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add interval tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"")
	EDITOR_OK
		SET_INTEGER_AS_STRING (position, Melder_cat (my textGrid() -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (name, U"")
	EDITOR_DO
		{// scope
			autoIntervalTier tier = IntervalTier_create (my textGrid() -> xmin, my textGrid() -> xmax);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (tier.get(), name);

			FunctionArea_save (me, U"Add interval tier");
			my textGrid() -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my selectedTier = position;
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
		//FunctionEditor_updateText (me); TRY OUT 2022-07-23
		Editor_broadcastDataChanged (my functionEditor());
	EDITOR_END
}
static void menu_cb_AddPointTier (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"");
	EDITOR_OK
		SET_INTEGER_AS_STRING (position, Melder_cat (my textGrid() -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (name, U"")
	EDITOR_DO
		{// scope
			autoTextTier tier = TextTier_create (my textGrid() -> xmin, my textGrid() -> xmax);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (tier.get(), name);

			FunctionArea_save (me, U"Add point tier");
			my textGrid() -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my selectedTier = position;
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
		//FunctionEditor_updateText (me); TRY OUT 2022-07-23
		Editor_broadcastDataChanged (my functionEditor());
	EDITOR_END
}
static void menu_cb_DuplicateTier (TextGridArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Duplicate tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"")
	EDITOR_OK
		if (my selectedTier != 0) {
			SET_INTEGER (position, my selectedTier + 1)
			SET_STRING (name, my textGrid() -> tiers->at [my selectedTier] -> name.get())
		}
	EDITOR_DO
		checkTierSelection (me, U"duplicate a tier");
		const Function tier = my textGrid() -> tiers->at [my selectedTier];
		{// scope
			autoFunction newTier = Data_copy (tier);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (newTier.get(), name);

			FunctionArea_save (me, U"Duplicate tier");
			my textGrid() -> tiers -> addItemAtPosition_move (newTier.move(), position);
		}

		my selectedTier = position;
		Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_updateText()
		//FunctionEditor_updateText (me); TRY OUT 2022-07-23
		Editor_broadcastDataChanged (my functionEditor());
	EDITOR_END
}
static void addTierMenu (TextGridArea me) {
	EditorMenu menu = Editor_addMenu (my functionEditor(), U"Tier", 0);
	FunctionAreaMenu_addCommand (menu, U"Add interval tier...", 0,
			menu_cb_AddIntervalTier, me);
	FunctionAreaMenu_addCommand (menu, U"Add point tier...", 0,
			menu_cb_AddPointTier, me);
	FunctionAreaMenu_addCommand (menu, U"Duplicate tier...", 0,
			menu_cb_DuplicateTier, me);
	FunctionAreaMenu_addCommand (menu, U"Rename tier...", 0,
			menu_cb_RenameTier, me);
	FunctionAreaMenu_addCommand (menu, U"-- remove tier --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Remove all text from tier", 0,
			menu_cb_RemoveAllTextFromTier, me);
	FunctionAreaMenu_addCommand (menu, U"Remove entire tier", 0,
			menu_cb_RemoveTier, me);
	FunctionAreaMenu_addCommand (menu, U"-- extract tier --", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Extract to list of objects:", 0, nullptr, me);
	FunctionAreaMenu_addCommand (menu, U"Extract entire selected tier", 0,
			CONVERT_DATA_TO_ONE__PublishTier, me);
}


#pragma mark - TextGridArea all menus

void structTextGridArea :: v_createMenus () {
	EditorMenu textGridMenu = Editor_addMenu (our functionEditor(), U"TextGrid", 0);
	addTextGridDrawMenu (this, our functionEditor() -> drawMenu);
	addTextGridDrawMenu (this, textGridMenu);
	addTextGridExtractMenu (this, our functionEditor() -> extractMenu);
	addTextGridExtractMenu (this, textGridMenu);
	addIntervalMenu (this);
	addBoundaryMenu (this);
	addTierMenu (this);

	if (our functionEditor() -> textArea)
		GuiText_setChangedCallback (our functionEditor() -> textArea, gui_text_cb_changed, this);
}
void structTextGridArea :: v_updateMenuItems () {
	TextGridArea_Parent :: v_updateMenuItems ();
	GuiThing_setSensitive (extractSelectedTextGridPreserveTimesButton, our endSelection() > our startSelection());
	GuiThing_setSensitive (extractSelectedTextGridTimeFromZeroButton,  our endSelection() > our startSelection());
}

/* End of file TextGridArea.cpp */
