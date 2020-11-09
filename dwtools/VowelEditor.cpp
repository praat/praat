/* VowelEditor.cpp
 *
 * Copyright (C) 2008-2020 David Weenink, 2015-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
trajectory --> path ????
 The main part of the VowelEditor is a drawing area.
 In this drawing area a cursor can be moved around by a mouse.
 The position of the cursor is related to the F1 and F2 frequencies.
 On_mouse_down the position of the cursor is sampled (Not at fixed intervals!).
 This results in a series of (x,y) values that will be transformed to (F1,F2) values in Hertz.
 The corresponding sound is made audible after the mouse is released.

 The user graphics area is the F1-F2 plane: here the origin is at the top-right with log(F2) on the 
 horizontal axis and log(F1) on the vertical axis (i.e. log (F1) top-down, log(F2) right-to-left)
 F1, F2 are always evaluated to hertz values;
 On the contrary the Graphics_window has it origin at the bottom left (0,0) and the top-right is at (1,1).
 We use a transformation such that log(fmin) -> 1 and  log(fmax)-> 0
 Transformations XY <=> F1F2: VowelEditor_getXYFromF1F2(...) and VowelEditor_getF1F2FromXY(...)
  For x direction F2 from right to left
   1 = a * log(f2min) +b
   0 = a * log(f2max) +b
   x' = a (log(f2)-log(f2max))

   1 = a * log(f1min) +b
   0 = a * log(f1max) +b
   y' = a (log(f1)-log(f1max))
 TO DO:
 Make sound-follows-mouse real time!
*/

#include "FormantGrid.h"
#include "KlattGrid.h"
#include "PitchTier_to_Sound.h"
#include "Polygon.h"
#include "Preferences.h"
#include "TableOfReal_extensions.h"
#include "Table_extensions.h"
#include "VowelEditor.h"   // this includes windows.h
#include "EditorM.h"   // include this *after* including windows.h (it redefines BOOLEAN)
#include "machine.h"
#include <time.h>

#if defined (macintosh)
	#include <sys/time.h>
#elif defined (linux)
	#include <sys/time.h>
	#include <signal.h>
#endif

#include "enums_getText.h"
#include "VowelEditor_enums.h"
#include "enums_getValue.h"
#include "VowelEditor_enums.h"

Thing_implement (VowelEditor, Editor, 0);

#include "prefs_define.h"
#include "VowelEditor_prefs.h"
#include "prefs_install.h"
#include "VowelEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "VowelEditor_prefs.h"

#define STATUS_INFO (3*Gui_LABEL_HEIGHT/2)
#define MARGIN_RIGHT 10
#define MARGIN_LEFT 50
#define MARGIN_TOP 50
#define MARGIN_BOTTOM (60+STATUS_INFO)

#pragma mark - class TrajectoryPointTier

Thing_implement (TrajectoryPoint, AnyPoint, 0);

static autoTrajectoryPoint TrajectoryPoint_create (double time, double f1, double f2, MelderColour colour) {
	autoTrajectoryPoint thee = Thing_new (TrajectoryPoint);
	thy number = time;
	thy f1 =  f1;
	thy f2 = f2;
	thy colour = colour;
	return thee;
}

Thing_implement (Trajectory, Function, 0);

static autoTrajectory Trajectory_create (double duration) {
	try {
		autoTrajectory me = Thing_new (Trajectory);
		Function_init (me.get(), 0.0, duration);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Trajectory not created.");
	}
}

static void Trajectory_addPoint (Trajectory me, double time, double f1, double f2, MelderColour colour) {
	try {
		autoTrajectoryPoint point = TrajectoryPoint_create (time, f1, f2, colour);
		my xmax = std::max (my xmax, time);
		my points . addItem_move (point.move());
	} catch (MelderError) {
		Melder_throw (me, U" no point added.");
	}
}

static void Trajectory_newDuration (Trajectory me, double newDuration) {
	if (newDuration != my xmax) {
		const double multiplier = newDuration / my xmax;
		for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
			TrajectoryPoint point = my points.at [ipoint];
			point -> number *= multiplier;
		}
		my xmax = newDuration;
	}
}

static void Trajectory_reverse (Trajectory me) {
	const double duration = my xmax;
	const integer np = my points.size, np_2 = np / 2;

	for (integer ipoint = 1; ipoint <= np_2; ipoint ++) {
		std::swap (my points.at [ipoint], my points.at [np - ipoint + 1]);
		TrajectoryPoint point = my points.at [ipoint];
		point -> number = duration - point -> number;
		point = my points.at [np - ipoint + 1];
		point -> number = duration - point -> number;
	}
	if (np % 2 == 1) {
		TrajectoryPoint point = my points.at [np_2 + 1];
		point -> number = duration - point -> number;
	}
}

static void Trajectory_shift_semitones (Trajectory me, double f1_st, double f2_st) {
	for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
		TrajectoryPoint point = my points.at [ipoint];
		double f1 = point -> f1, f2 = point -> f2;
		f1 *= pow (2.0, f1_st / 12.0);
		point -> f1 = f1;	
		f2 *= pow (2.0, f2_st / 12.0);
		point -> f2 = f2;
	}
}

static void Trajectory_setColour (Trajectory me, double startTime, double endTime, MelderColour colour) {
	Melder_require (Function_intersectRangeWithDomain (me, & startTime, & endTime),
		U"The time interval is not part of the trajectory.");
	/*
		If a segment runs from p1 to p2 then its colour is detemined by p1.
	*/
	integer startIndex = AnyTier_timeToHighIndex (my asAnyTier (), startTime);
	integer endIndex = AnyTier_timeToLowIndex (my asAnyTier (), endTime);
			
	for (integer ipoint = startIndex; ipoint <= endIndex; ipoint ++) {
		TrajectoryPoint point = my points.at [ipoint];
		point -> colour = colour;
	}
	/*
		We just add the points to the tier.
		If the startTime and endTime are too far away from a trajectory point,
		as could happen if a trajectory was created by the 'New trajectory' and/or
		'Extend trajectory' menu items, then we have to include a new trajectory point.
		The (time,f1,f2) numbers are obtained by linear interpolation, where we 
		use that two points in a trajectory never bear the same time.
		
		We also have to guard against rounding down in the forms. For example an endTime
		of 0.4632200000000007 might be shown in the form as '0.46322'.
	*/
	TrajectoryPoint p2, p1 = my points.at [endIndex];
	if (p1 -> number == endTime) {
		if (endIndex < my points.size) {
			p2 = my points.at [endIndex + 1];
			p1 -> colour = p2 -> colour;
		}
	} else {
		p2 = my points.at [endIndex + 1];
		double t = (endTime - p1 -> number) / (p2 -> number - p1 -> number);
		double f2 = p1 -> f2 + (p2 -> f2 - p1 -> f2) * t;
		double f1 = p1 -> f1 + (p2 -> f1 - p1 -> f1) * t;
		Trajectory_addPoint (me, endTime, f1, f2, ( startIndex > endIndex ? p1 -> colour : p2 -> colour ));
	}
	p2 = my points.at [startIndex];
	if (p2 -> number == startTime) {
		if (startIndex > 1) {
			p1 = my points.at [startIndex - 1];
			p2 -> colour = colour;
		}
	} else {
		p1 = my points.at [startIndex - 1];
		double t = (startTime - p1 -> number) / (p2 -> number - p1 -> number);
		double f2 = p1 -> f2 + (p2 -> f2 - p1 -> f2) * t;
		double f1 = p1 -> f1 + (p2 -> f1 - p1 -> f1) * t;
		Trajectory_addPoint (me, startTime, f1, f2, colour);
	}
}

#pragma mark - class Vowel

static void VowelEditor_create_twoFormantSchwa (VowelEditor me) {
	try {
		my trajectory = Trajectory_create (my p_trajectory_minimumDuration);
		MelderColour colour = MelderColour_fromColourNameOrRGBString (my p_trajectory_colour);
		Trajectory_addPoint (my trajectory.get (), 0.0, 500.0, 1500.0, colour);
		Trajectory_addPoint (my trajectory.get (), my p_trajectory_minimumDuration, 500.0, 1500.0, colour);
	} catch (MelderError) {
		Melder_throw (U"Schwa Vowel not created");
	}
}

static inline double VowelEditor_getXFromF2 (VowelEditor me, double f2) {
	return log (f2 / my p_window_f2max) / log (my p_window_f2min / my p_window_f2max);
}

static inline double VowelEditor_getYFromF1 (VowelEditor me, double f1) {
	return log (f1 / my p_window_f1max) / log (my p_window_f1min / my p_window_f1max);
}

static void VowelEditor_getXYFromF1F2 (VowelEditor me, double f1, double f2, double *x, double *y) {
	*y = VowelEditor_getYFromF1 (me, f1);
	*x = VowelEditor_getXFromF2 (me, f2);
}

static void VowelEditor_getF1F2FromXY (VowelEditor me, double x, double y, double *f1, double *f2) {
	*f2 = my p_window_f2min * pow (my p_window_f2max / my p_window_f2min, 1.0 - x);
	*f1 = my p_window_f1min * pow (my p_window_f1max / my p_window_f1min, 1.0 - y);
}

static double getRealFromTextWidget (GuiText me) {
	double value = undefined;
	autostring32 dirty = GuiText_getString (me);
	try {
		Interpreter_numericExpression (nullptr, dirty.get(), & value);
	} catch (MelderError) {
		Melder_clearError ();
		value = undefined;
	}
	return value;
}

static void clipF1F2 (VowelEditor me, double *f1, double *f2) {
	Melder_clip (my p_window_f1min, f1, my p_window_f1max);
	Melder_clip (my p_window_f2min, f2, my p_window_f2max);
}

static void VowelEditor_updateFromF0StartAndSlopeTextWidgets (VowelEditor me) {
	double f0 = getRealFromTextWidget (my f0TextField);
	Melder_clip (my p_f0_minimum, & f0, my p_f0_maximum);
	GuiText_setString (my f0TextField, Melder_double (f0));
	my pref_f0_start () = my p_f0_start = f0;
	double slope = getRealFromTextWidget (my f0SlopeTextField);
	if (isundef (slope))
		slope = 0.0;
	my pref_f0_slope () = my p_f0_slope = slope;
	GuiText_setString (my f0SlopeTextField, Melder_double (slope));
}

static void VowelEditor_updateFromExtendDurationTextWidget (VowelEditor me) {
	double extend = getRealFromTextWidget (my extendTextField);
	if (isundef (extend) || extend <= my p_trajectory_minimumDuration)
		extend = my p_trajectory_minimumDuration;
	GuiText_setString (my extendTextField, Melder_double (extend));
	my pref_trajectory_extendDuration () = my p_trajectory_extendDuration = extend;
}

static double VowelEditor_updateFromDurationTextWidget (VowelEditor me) {
	double duration = getRealFromTextWidget (my durationTextField);
	if (isundef (duration) || duration < my p_trajectory_minimumDuration)
		duration = my p_trajectory_minimumDuration;
	my pref_trajectory_duration () = my p_trajectory_duration = duration;
	GuiText_setString (my durationTextField, Melder_fixed (duration, 6));
	Trajectory_newDuration (my trajectory.get(), duration);
	return duration;
}

static void Sound_fadeIn (Sound me, double duration, bool fromFirstNonZeroSample) {
	integer istart = 1, numberOfSamples = Melder_ifloor (duration / my dx);   // ppgb: waarom afronden naar beneden?
	if (numberOfSamples < 2)
		return;
	if (fromFirstNonZeroSample) {
		/*
			If the first part of the sound is very low level we put sample values to zero and
			start windowing from the position where the amplitude is above the minimum level.
			WARNING: this part is special for the artificial vowels because
			1. They have no offset
			2. They are already scaled to a maximum amplitude of 0.99
			3. For 16 bit precision
		*/
		const double zmin = 0.5 / pow (2.0, 16.0);
		while (fabs (my z [1] [istart]) < zmin && istart < my nx) {
			my z [1] [istart] = 0.0; // To make sure
			istart ++;
		}
	}
	Melder_clipRight (& numberOfSamples, my nx - istart + 1);
	for (integer i = 1; i <= numberOfSamples; i ++) {
		const double phase = NUMpi * (i - 1) / (numberOfSamples - 1);
		my z [1] [istart + i - 1] *= 0.5 * (1.0 - cos (phase));
	}
}

static void Sound_fadeOut (Sound me, double duration) {
	integer numberOfSamples = Melder_ifloor (duration / my dx);
	Melder_clipRight (& numberOfSamples, my nx);
	if (numberOfSamples < 2)
		return;
	const integer istart = my nx - numberOfSamples;
	/*
		Only one channel.
	*/
	for (integer i = 1; i <= numberOfSamples; i ++) {
		const double phase = NUMpi * (i - 1) / (numberOfSamples - 1);
		my z [1] [istart + i] *= 0.5 * (1.0 + cos (phase));
	}
}

static double VowelEditor_getF0AtTime (VowelEditor me, double time) {
	double f0 = my p_f0_start * pow (2.0, my p_f0_slope * time);
	Melder_clip (my p_f0_minimum, & f0, my p_f0_maximum);
	return f0;
}

static void VowelEditor_updateTrajectorySpecification (VowelEditor me) {
	/*
		Always update; GuiObject text might have changed.
	*/
	VowelEditor_updateFromDurationTextWidget (me);
	VowelEditor_updateFromF0StartAndSlopeTextWidgets (me);
}

static autoFormantGrid VowelEditor_to_FormantGrid (VowelEditor me) {
	try {
		autoFormantGrid thee = FormantGrid_createEmpty (my trajectory -> xmin, my trajectory -> xmax, my p_synthesis_numberOfFormants);
		for (integer ipoint = 1; ipoint <= my trajectory -> points.size; ipoint ++) {
			const TrajectoryPoint tp = my trajectory -> points.at [ipoint];
			const double time = tp -> number;
			FormantGrid_addFormantPoint (thee.get(), 1, time, tp -> f1);
			FormantGrid_addBandwidthPoint (thee.get(), 1, time, tp -> f1 / my p_synthesis_q1);
			if (my p_synthesis_numberOfFormants < 2)
				continue;
			FormantGrid_addFormantPoint (thee.get(), 2, time, tp -> f2);
			FormantGrid_addBandwidthPoint (thee.get(), 2, time, tp -> f2 / my p_synthesis_q2);
			for (integer ifor = 1; ifor <= my p_synthesis_numberOfFormants - 2; ifor ++) {
				if (my p_synthesis_numberOfFormants < 2 + ifor)
					break;
				FormantGrid_addFormantPoint (thee.get(), 2 + ifor, time, my extraFrequencyBandwidthPairs [2 * ifor - 1]);
				FormantGrid_addBandwidthPoint (thee.get(), 2 + ifor, time, my extraFrequencyBandwidthPairs [2 * ifor]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No FormantGrid created.");
	}
}

static autoPitchTier VowelEditor_to_PitchTier (VowelEditor me) {
	try {
		autoPitchTier thee = PitchTier_create (my trajectory -> xmin, my trajectory -> xmax);
		for (integer ipoint = 1; ipoint <= my trajectory -> points.size; ipoint ++) {
			const TrajectoryPoint point = my trajectory -> points.at [ipoint];
			const double f0 = VowelEditor_getF0AtTime (me, point -> number);
			RealTier_addPoint (thee.get(), point -> number, f0);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"PitchTier not created.");
	}	
}
static autoSound VowelEditor_createTargetSound (VowelEditor me) {
	try {
		VowelEditor_updateTrajectorySpecification (me);   // update pitch and duration
		autoFormantGrid formantGrid = VowelEditor_to_FormantGrid (me);
		autoPitchTier pitchTier = VowelEditor_to_PitchTier (me);
		autoSound thee = PitchTier_to_Sound_pulseTrain (pitchTier.get(), my p_synthesis_samplingFrequency, 0.7, 0.05, 30, false);
		Sound_FormantGrid_filter_inplace (thee.get(), formantGrid.get());
		Vector_scale (thee.get(), 0.99);
		Sound_fadeIn (thee.get(), 0.005, true);
		Sound_fadeOut (thee.get(), 0.005);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Target Sound not created.");
	}
}

/* Precondition: trajectory points are all different */
static void VowelEditor_drawF1F2Trajectory (VowelEditor me, Graphics g) {
	if (my trajectory -> points.size < 2)
		return;

	const int savedLineType = Graphics_inqLineType (g);
	const double savedLineWidth = Graphics_inqLineWidth (g);
	const MelderColour savedColour = Graphics_inqColour (g);

	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	if (my trajectory -> xmax - my trajectory -> xmin < 0.005)   // too short to hear?
		Graphics_setColour (g, Melder_RED);
	
	auto getx = [=](double f) { return log (f / my p_window_f2max) / log (my p_window_f2min / my p_window_f2max); };
	auto gety = [=](double f) { return log (f / my p_window_f1max) / log (my p_window_f1min / my p_window_f1max); };

	const TrajectoryPoint firstPoint = my trajectory -> points.at [1];
	double x1 = getx (firstPoint -> f2);
	double y1 = gety (firstPoint -> f1);
	double t1 = firstPoint -> number;
	MelderColour colour = firstPoint -> colour;   // first point determines colour of segment
	Graphics_setColour (g, colour);
	integer imark = 1;
	for (integer it = 2; it <= my trajectory -> points.size; it ++) {
		const TrajectoryPoint point = my trajectory -> points.at [it];
		const double x2 = getx (point -> f2);
		const double y2 = gety (point -> f1);
		const double t2 = point -> number;
		Graphics_setLineWidth (g, my p_trajectory_lineWidth);
		Graphics_line (g, x1, y1, x2, y2);
		/*
			Draw line orthogonal to the trajectory at regular points.
		*/
		double markTime;
		while ((markTime = my p_trajectory_markEvery * imark) < t2) {
			/*
				The parametrization of the line running from (x1,y1) to (x2,y2) is
				(1)   (x1, y1) + s * (x2 - x1, y2 - y1), where 0 <= s <= 1;
				The mark position (xm,ym) is at s = (markTime - t1) / (t2 - t1).
				The line perpendicular to this line can be parametrized as
				(2)   (xm, ym) + v * (-(y2 - y1), x2 - x1). 
				We need the startpoint, M1, and endpoint, M2, of the mark line. These are parametrized as
				(xl1,yl1) = (xm,ym) + V * (y1 - y2, x2 - x1) and (xl2,yl2) = (xm , ym) - V * (y1 - y2, x2 - x1).
				The length of the vector mark line constrains the value of V.
			*/
			const double s = (markTime - t1) / (t2 - t1);
			const double dx = x2 - x1, dy = y2 - y1;
			const double xm = x1 + s * dx;
			const double ym = y1 + s * dy;
			const double d = sqrt (dx * dx + dy * dy);
			const double v = my p_trajectory_markLength / (2.0 * d); // d > 0
			const double xl1 = xm - v * dy, yl1 = ym + v * dx;
			const double xl2 = xm + v * dy, yl2 = ym - v * dx;
			Graphics_setLineWidth (g, 1);
			Graphics_line (g, xl1, yl1, xl2, yl2);
			imark ++;
		}
		x1 = x2;
		y1 = y2;
		t1 = t2;
		if (! MelderColour_equal (colour, point -> colour)) {
			Graphics_setColour (g, point -> colour);
			colour = point -> colour;
		}
	}
	/*
		Arrow at end.
	*/
	{
		const integer n = my trajectory -> points.size;
		const double savedArrowSize = Graphics_inqArrowSize (g), arrowSize = 1.0;
		double resolution = Graphics_getResolution (g);
		Graphics_setArrowSize (g, arrowSize);
		integer it = 0;
		const TrajectoryPoint lastPoint = my trajectory -> points.at [n];
		TrajectoryPoint point;
		Melder_assert (n >= 2);   // otherwise `point` will stay undefined
		while (++ it <= n - 1) {
			point = my trajectory -> points.at [n - it];
			const double dx = resolution * (getx (lastPoint -> f2) - getx (point -> f2));
			const double dy = resolution * (gety (lastPoint -> f1) - gety (point -> f1));
			const double d2 = dx * dx + dy * dy;
			if (sqrt (d2) > arrowSize)
				break;
		}
		Graphics_arrow (g, getx (point -> f2), gety (point -> f1), getx (lastPoint -> f2), gety (lastPoint -> f1));
		Graphics_setArrowSize (g, savedArrowSize);
	}
	Graphics_unsetInner (g);
	Graphics_setLineType (g, savedLineType);
	Graphics_setLineWidth (g, savedLineWidth);
	Graphics_setColour (g, savedColour);
}

static void Table_addColumnIfNotExists_size (Table me, double size) {
	const integer sizeColumn = Table_findColumnIndexFromColumnLabel (me, U"Size");
	if (sizeColumn == 0) {
		Table_appendColumn (me, U"Size");
		for (integer irow = 1; irow <= my rows.size; irow ++)
			Table_setNumericValue (me, irow, my numberOfColumns, size);
	}
}

static void Table_addColumnIfNotExists_colour (Table me, conststring32 colour) {
	const integer colourColumn = Table_findColumnIndexFromColumnLabel (me, U"Colour");
	if (colourColumn == 0) {
		Table_appendColumn (me, U"Colour");
		for (integer irow = 1; irow <= my rows.size; irow ++)
			Table_setStringValue (me, irow, my numberOfColumns, colour);
	}
}

static void VowelEditor_getVowelMarksFromFile (VowelEditor me) {
	try {
		Melder_require (str32len (my p_marks_fileName) > 0,
			U"No file with vowel marks has been defined.");
		structMelderFile file { };
		Melder_pathToFile (my p_marks_fileName, & file);
		autoDaata data = Data_readFromFile (& file);
		Melder_require (Thing_isa (data.get(), classTable),
			U"\"", MelderFile_name (& file), U"\" is not a Table file");

		autoTable newMarks = data.static_cast_move <structTable> ();
		/*
			Require columns Vowel F1 and F2 to be present.
		*/
		Table_getColumnIndexFromColumnLabel (newMarks.get(), U"Vowel");
		Table_getColumnIndexFromColumnLabel (newMarks.get(), U"F1");
		Table_getColumnIndexFromColumnLabel (newMarks.get(), U"F2");
		Table_addColumnIfNotExists_size (newMarks.get(), my p_marks_fontSize);
		my marks = newMarks.move();
	} catch (MelderError) {
		Melder_throw (U"Vowel marks from file cannot be shown.");
	}
}

static void VowelEditor_getMarks (VowelEditor me) {
	autoTable te;
	if (my p_marks_dataSet == kVowelEditor_marksDataSet::AMERICAN_ENGLISH) {
		const autoTable thee = Table_create_petersonBarney1952 ();
		te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO,
		  ( my p_marks_speakerType == kVowelEditor_speakerType::MAN ? U"m" :
			my p_marks_speakerType == kVowelEditor_speakerType::WOMAN ? U"w" :
			my p_marks_speakerType == kVowelEditor_speakerType::CHILD ? U"c": U"m" ));
	} else if (my p_marks_dataSet == kVowelEditor_marksDataSet::DUTCH) {
		if (my p_marks_speakerType == kVowelEditor_speakerType::CHILD) {
			const autoTable thee = Table_create_weenink1983 ();
			te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, U"c");
		}
		else {   // male + female from Pols van Nierop
			const autoTable thee = Table_create_polsVanNierop1973 ();
			te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, ( my p_marks_speakerType == kVowelEditor_speakerType::MAN ? U"m" : my p_marks_speakerType == kVowelEditor_speakerType::WOMAN ? U"f" : U"f") );
		}
	} else if (my p_marks_dataSet == kVowelEditor_marksDataSet::NONE) {   // none
		my marks.reset();
		return;
	} else {  // other
		VowelEditor_getVowelMarksFromFile (me);
		return;
	}
	/* mutable move */ autoTable newMarks = Table_collapseRows (te.get(), U"IPA", U"", U"F1 F2", U"", U"", U"");
	const integer col_ipa = Table_findColumnIndexFromColumnLabel (newMarks.get(), U"IPA");
	Table_setColumnLabel (newMarks.get(), col_ipa, U"Vowel");

	Table_addColumnIfNotExists_size (newMarks.get(), my p_marks_fontSize);
	Table_addColumnIfNotExists_colour (newMarks.get(), my p_marks_colour);
	my marks = newMarks.move();
}

static void VowelEditor_drawBackground (VowelEditor me, Graphics g) {
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (g, 0.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 1.0);
	Graphics_setGrey (g, 0.5);
	const double fontSize = Graphics_inqFontSize (g);
	/*
		Draw the marks.
	*/
	if (my marks) {
		const integer col_vowel = Table_getColumnIndexFromColumnLabel (my marks.get(), U"Vowel");
		const integer col_f1 = Table_getColumnIndexFromColumnLabel (my marks.get(), U"F1");
		const integer col_f2 = Table_getColumnIndexFromColumnLabel (my marks.get(), U"F2");
		const integer col_size = Table_findColumnIndexFromColumnLabel (my marks.get(), U"Size");
		const integer col_colour = Table_findColumnIndexFromColumnLabel (my marks.get(), U"Colour");
		for (integer irow = 1; irow <= my marks -> rows.size; irow ++) {
			const conststring32 label = Table_getStringValue_Assert (my marks.get(), irow, col_vowel);
			const double f1 = Table_getNumericValue_Assert (my marks.get(), irow, col_f1);
			const double f2 = Table_getNumericValue_Assert (my marks.get(), irow, col_f2);
			if (f1 >= my p_window_f1min && f1 <= my p_window_f1max && f2 >= my p_window_f2min && f2 <= my p_window_f2max) {
				double x1, y1;
				VowelEditor_getXYFromF1F2 (me, f1, f2, & x1, & y1);
				double size = my p_marks_fontSize;
				if (col_size != 0)
					size = Table_getNumericValue_Assert (my marks.get(), irow, col_size);
				if (col_colour != 0) {
					conststring32 colourString = Table_getStringValue_Assert (my marks.get(), irow, col_colour);
					MelderColour colour = MelderColour_fromColourNameOrNumberStringOrRGBString (colourString);
					if (! colour. valid())
						colour = MelderColour_fromColourName (my p_marks_colour);
					Graphics_setColour (g, colour);
				}
				Graphics_setFontSize (g, size);
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
				Graphics_text (g, x1, y1, label);
			}
		}
	}
	Graphics_setFontSize (g, fontSize);
	Graphics_setColour (g, Melder_BLACK);
	/*
		Draw the line F1=F2.
	*/
	double xl1, yl1, xl2, yl2;
	VowelEditor_getXYFromF1F2 (me, my p_window_f2min, my p_window_f2min, & xl1, & yl1);
	if (yl1 >= 0.0 && yl1 <= 1.0) {
		VowelEditor_getXYFromF1F2 (me, my p_window_f1max, my p_window_f1max, & xl2, & yl2);
		if (xl2 >= 0.0 && xl2 <= 1.0) {
			double x [] = { xl1, xl2, 1.0 };
			double y [] = { yl1, yl2, 0.0 };
			Graphics_setGrey (g, 0.6);
			Graphics_fillArea (g, 3, x , y);
			Graphics_setColour (g, Melder_BLACK);
			Graphics_line (g, xl1, yl1, xl2, yl2);
		}
	}
	/*
		Draw the horizontal grid lines.
	*/
	if (my p_grid_df1 < (my p_window_f1max - my p_window_f1min)) {
		integer iline = Melder_iroundDown ((my p_window_f1min + my p_grid_df1) / my p_grid_df1);
		Graphics_setGrey (g, 0.5);
		Graphics_setLineType (g, Graphics_DOTTED);
		double f1_gridLine, xg1, yg1, xg2, yg2;
		while ( (f1_gridLine = iline * my p_grid_df1) < my p_window_f1max) {
			if (f1_gridLine > my p_window_f1min) {
				VowelEditor_getXYFromF1F2 (me, f1_gridLine, my p_window_f2min, & xg1, & yg1);
				VowelEditor_getXYFromF1F2 (me, f1_gridLine, my p_window_f2max, & xg2, & yg2);
				Graphics_line (g, xg1, yg1, xg2, yg2);
			}
			iline ++;
		}
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setColour (g, Melder_BLACK);
	}
	/*
		Draw the vertical grid lines.
	*/
	if (my p_grid_df2 < (my p_window_f2max - my p_window_f2min)) {
		integer iline = Melder_iroundDown ((my p_window_f2min + my p_grid_df2) / my p_grid_df2);
		Graphics_setGrey (g, 0.5);
		Graphics_setLineType (g, Graphics_DOTTED);
		double f2_gridLine, xg1, yg1, xg2, yg2;
		while ( (f2_gridLine = iline * my p_grid_df2) < my p_window_f2max) { // vert line
			if (f2_gridLine > my p_window_f2min) {
				VowelEditor_getXYFromF1F2 (me, my p_window_f1min, f2_gridLine, & xg1, & yg1);
				VowelEditor_getXYFromF1F2 (me, my p_window_f1max, f2_gridLine, & xg2, & yg2);
				Graphics_line (g, xg1, yg1, xg2, yg2);
			}
			iline ++;
		}
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setColour (g, Melder_BLACK);
	}
	Graphics_setLineWidth (g, 2.0);
	Graphics_rectangle (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_unsetInner (g);
	Graphics_setGrey (g, 0.0);   // black
	Graphics_markLeft (g, 0.0, false, true, false, Melder_double (my p_window_f1max));
	Graphics_markLeft (g, 1.0, false, true, false, Melder_double (my p_window_f1min));
	Graphics_markTop (g, 0.0, false, true, false, Melder_double (my p_window_f2max));
	Graphics_markTop (g, 1.0, false, true, false, Melder_double (my p_window_f2min));
}

#pragma mark - menu methods

static void updateInfoLabels (VowelEditor me) {
	const double startF0 = VowelEditor_getF0AtTime (me, my trajectory -> xmin);
	const double endF0 = VowelEditor_getF0AtTime (me, my trajectory -> xmax);
	const TrajectoryPoint startPoint = my trajectory -> points.at [1];
	const TrajectoryPoint endPoint = my trajectory -> points.at [my trajectory -> points.size];
	MelderString statusInfo;
	if (! my graphics)
		return;   // could be the case in the very beginning
	MelderString_append (& statusInfo, U"Start (F1,F2,f0) = (", Melder_fixed (startPoint -> f1, 1), U", ", 
		Melder_fixed (startPoint -> f2, 1), U", ", startF0, U")");
	GuiLabel_setText (my startInfo, statusInfo.string);
	MelderString_empty (& statusInfo);
	MelderString_append (& statusInfo, U"End (F1,F2,f0) = (", Melder_fixed (endPoint -> f1, 1), U", ", 
		Melder_fixed (endPoint -> f2, 1), U", ", endF0, U")");
	GuiLabel_setText (my endInfo, statusInfo.string);	
}

static void menu_cb_help (VowelEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"VowelEditor");
}

static void menu_cb_trajectoryInfo (VowelEditor me, EDITOR_ARGS_FORM) {
	MelderInfo_open ();
	MelderInfo_writeLine (U"Trajectory info:");
	MelderInfo_writeLine (U"Number of points: ", my trajectory -> points.size);
	MelderInfo_writeLine (U"Start time: ", my trajectory -> xmin, U" s");
	TrajectoryPoint p1 = my trajectory -> points.at [1];
	MelderInfo_writeLine (U"    F1: ", p1 -> f1, U" Hz");
	MelderInfo_writeLine (U"    F2: ", p1 -> f2, U" Hz");
	MelderInfo_writeLine (U"End time: ", my trajectory -> xmax, U" s");
	TrajectoryPoint p2 = my trajectory -> points.at [my trajectory -> points.size];
	MelderInfo_writeLine (U"    F1: ", p2 -> f1, U" Hz");
	MelderInfo_writeLine (U"    F2: ", p2 -> f2, U" Hz");
	MelderInfo_close ();
}

static void menu_cb_prefs (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Preferences", nullptr);
		BOOLEAN (soundFollowsMouse, U"Sound follows mouse", my default_soundFollowsMouse ())
		LABEL (U"F1 and F2 frequencies are specified by the trajectory.")
		LABEL (U"The bandwidths of a formant can be specified by its Q-value")
		LABEL (U"which defines the sharpness of the peak: Q = frequency / bandwidth. ")
		POSITIVE (q1, U"F1 sharpness", my default_synthesis_q1 ())
		POSITIVE (q2, U"F2 sharpness", my default_synthesis_q2 ())
		LABEL (U"You can define extra fixed formants for the synthesis by supplying")
		LABEL (U"formant frequency bandwidth pairs.")
		SENTENCE (fbpairs, U"Frequency bandwidth pairs", my default_synthesis_extraFBPairs ())
		LABEL (U"The total number of formants used for synthesis")
		NATURAL (numberOfFormants, U"Number of formants for synthesis", my default_synthesis_numberOfFormants ())
	EDITOR_OK
		SET_BOOLEAN (soundFollowsMouse, my p_soundFollowsMouse)
		SET_REAL (q1, my p_synthesis_q1)
		SET_REAL (q2, my p_synthesis_q2)
		SET_STRING (fbpairs, my p_synthesis_extraFBPairs)
		SET_INTEGER (numberOfFormants, my p_synthesis_numberOfFormants)
	EDITOR_DO
		my pref_soundFollowsMouse () = my p_soundFollowsMouse = soundFollowsMouse;
		my pref_synthesis_q1 () = my p_synthesis_q1 = q1;
		my pref_synthesis_q2 () = my p_synthesis_q2 = q2;
		autoVEC extraFrequencyBandwidthPairs = newVECfromString (fbpairs);

		Melder_require (extraFrequencyBandwidthPairs.size % 2 == 0,
			U"There should be an even number of values in the \"Frequencies and bandwidths pairs\" list.");
		/*
			All items should be positive numbers and frequencies should be lower than the Nyquist.
			Bandwidths should be greater than zero.
		*/
		for (integer item = 1; item <= extraFrequencyBandwidthPairs.size; item ++) {
			Melder_require (extraFrequencyBandwidthPairs [item] > 0,
				U"All values frequency bandwidth values should be positive.");
			if (item % 2 == 1)
				Melder_require (extraFrequencyBandwidthPairs [item] < 0.5 * my p_synthesis_samplingFrequency,
					U"All formant frequencies should be below the Nyquist frequency (",
					0.5 * my p_synthesis_samplingFrequency, U" Hz.");
		}
		const integer numberOfPairs = extraFrequencyBandwidthPairs.size / 2;
		Melder_require (numberOfFormants <= numberOfPairs + 2,
			U"The \"Number of formants for synthesis\" should not exceed the number of formants specified (",
			numberOfPairs + 2, U"). Either lower the number of formants for synthesis or specify more "
			"frequency bandwidth pairs.");
		/*
			Formants and bandwidths are valid. It is safe to copy them.
		*/
		pref_str32cpy2 (my pref_synthesis_extraFBPairs (), my p_synthesis_extraFBPairs, fbpairs);
		my pref_synthesis_numberOfFormants () = my p_synthesis_numberOfFormants = numberOfFormants;
		my extraFrequencyBandwidthPairs = extraFrequencyBandwidthPairs.move();
	EDITOR_END
}

static void menu_cb_ranges_f1f2 (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"F1 (vert) and F2 (hor) view ranges", nullptr);
		POSITIVE (f1min, U"left F1 range (Hz)", my default_window_f1min ())
		POSITIVE (f1max, U"right F1 range (Hz)", my default_window_f1max ())
		POSITIVE (f2min, U"left F2 range (Hz)", my default_window_f2min ())
		POSITIVE (f2max, U"right F2 range (Hz)", my default_window_f2max ())
	EDITOR_OK
		SET_REAL (f1min, my p_window_f1min)
		SET_REAL (f1max, my p_window_f1max)
		SET_REAL (f2min, my p_window_f2min)
		SET_REAL (f2max, my p_window_f2max)
	EDITOR_DO
		my pref_window_f1min () = my p_window_f1min = f1min;
		my pref_window_f1max () = my p_window_f1max = f1max;
		my pref_window_f2min () = my p_window_f2min = f2min;
		my pref_window_f2max () = my p_window_f2max = f2max;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_publishSound (VowelEditor me, EDITOR_ARGS_DIRECT) {
	autoSound publish = VowelEditor_createTargetSound (me);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_FormantGrid (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateTrajectorySpecification (me);
	autoFormantGrid publish = VowelEditor_to_FormantGrid (me);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_KlattGrid (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateTrajectorySpecification (me);
	autoFormantGrid fg = VowelEditor_to_FormantGrid (me);
	autoKlattGrid publish = KlattGrid_create (fg -> xmin, fg -> xmax, fg -> formants.size, 0, 0, 0, 0, 0, 0);
	KlattGrid_addVoicingAmplitudePoint (publish.get(), fg -> xmin, 90.0);
	autoPitchTier pitchTier = VowelEditor_to_PitchTier (me);
	KlattGrid_replacePitchTier (publish.get(), pitchTier.get());
	KlattGrid_replaceFormantGrid (publish.get(), kKlattGridFormantType::ORAL, fg.get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_PitchTier (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateTrajectorySpecification (me);
	autoPitchTier publish = VowelEditor_to_PitchTier (me);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_TrajectoryAsTable (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateTrajectorySpecification (me);
	autoTable publish = Table_createWithColumnNames (my trajectory -> points.size, U"Time F1 F2 Colour");
	for (integer ipoint = 1; ipoint <= my trajectory -> points.size; ipoint ++) {
		TrajectoryPoint point = my trajectory -> points.at [ipoint];
		Table_setNumericValue (publish.get(), ipoint, 1, point -> number);
		Table_setNumericValue (publish.get(), ipoint, 2, point -> f1);
		Table_setNumericValue (publish.get(), ipoint, 3, point -> f2);
		Table_setStringValue  (publish.get(), ipoint, 4, MelderColour_namePrettyOrNull (point -> colour));
	}
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_drawTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw trajectory", nullptr)
		my v_form_pictureWindow (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		Editor_openPraatPicture (me);
		if (garnish)
			VowelEditor_drawBackground (me, my pictureGraphics);
		VowelEditor_drawF1F2Trajectory (me, my pictureGraphics);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_showOneVowelMark (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show one vowel mark", nullptr);
		WORD (mark, U"Mark", U"u")
		POSITIVE (f1, U"F1 (Hz)", U"300.0")
		POSITIVE (f2, U"F2 (Hz)", U"600.0")
		POSITIVE (fontSize, U"Font size", U"14.0")
		COLOUR (colour, U"Colour", U"Green")
	EDITOR_OK
	EDITOR_DO
		Melder_require (f1 >= my p_window_f1min && f1 <= my p_window_f1max,
			U"The first formant should be in the range from ", my p_window_f1min, U" to ", my p_window_f1max, U" Hz.");
		Melder_require (f2 >= my p_window_f2min && f2 <= my p_window_f1max,
			U"The second formant should be in the range from ", my p_window_f2min, U" to ", my p_window_f2max, U" Hz.");
		if (! my marks)
			my marks = Table_createWithColumnNames (1, U"IPA F1 F2 Size Colour");
		else
			Table_appendRow (my marks.get());
		integer irow = my marks -> rows.size;
		Table_setStringValue (my marks.get(), irow, 1, mark);
		Table_setNumericValue (my marks.get(), irow, 2, f1);
		Table_setNumericValue (my marks.get(), irow, 3, f2);
		Table_setNumericValue (my marks.get(), irow, 4, fontSize);
		Table_setStringValue (my marks.get(), irow, 5, MelderColour_name (colour));
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_vowelMarks (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show vowel marks", nullptr);
		OPTIONMENU_ENUM (kVowelEditor_marksDataSet, dataSet, U"Data set", my default_marks_dataSet ())
		OPTIONMENU_ENUM (kVowelEditor_speakerType, speaker, U"Speaker", my default_marks_speakerType ())
		POSITIVE (fontSize, U"Font size (points)", my default_marks_fontSize ())
		WORD (colour_string, U"Colour", my default_marks_colour ());
	EDITOR_OK
		SET_ENUM (dataSet, kVowelEditor_marksDataSet, my p_marks_dataSet)
		SET_ENUM (speaker, kVowelEditor_speakerType, my p_marks_speakerType)
		SET_REAL (fontSize, my p_marks_fontSize)
		SET_STRING (colour_string, my p_trajectory_colour)
	EDITOR_DO
		my pref_marks_dataSet		() = my p_marks_dataSet = dataSet;
		my pref_marks_speakerType	() = my p_marks_speakerType = speaker;
		my pref_marks_fontSize		() = my p_marks_fontSize = fontSize;
		pref_str32cpy2 (my pref_marks_colour (), my p_marks_colour, colour_string);
		VowelEditor_getMarks (me);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_vowelMarksFromTableFile (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_READ (U"VowelEditor: Show vowel marks from Table file", U"VowelEditor: Show vowel marks from Table file...");
	EDITOR_DO_READ
		pref_str32cpy2 (my pref_marks_fileName (), my p_marks_fileName, Melder_fileToPath (file));
		my pref_marks_speakerType () = my p_marks_speakerType = kVowelEditor_speakerType::UNKNOWN;
		my pref_marks_dataSet () = my p_marks_dataSet = kVowelEditor_marksDataSet::OTHER;
		VowelEditor_getVowelMarksFromFile (me);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_setF0 (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set f0", nullptr);
		POSITIVE (f0Start, U"Start f0 (Hz)", my default_f0_start ())
		REAL (f0Slope, U"Slope (oct/s)", my default_f0_slope ())
	EDITOR_OK
		SET_REAL (f0Start, my p_f0_start)
		SET_REAL (f0Slope, my p_f0_slope)
	EDITOR_DO
		Melder_clip (my p_f0_minimum, & f0Start, my p_f0_maximum);
		my pref_f0_start () = my p_f0_start = f0Start;
		my pref_f0_slope () = my p_f0_slope = f0Slope;
		GuiText_setString (my f0TextField, Melder_double (f0Start));
		GuiText_setString (my f0SlopeTextField, Melder_double (f0Slope));
	EDITOR_END
}

static void menu_cb_setF3F4 (VowelEditor me, EDITOR_ARGS_FORM) { // deprecated 20200404
	EDITOR_FORM (U"Set F3 & F4", nullptr);
		POSITIVE (f3, U"F3 (Hz)", U"2500.0")
		POSITIVE (b3, U"B3 (Hz)", U"250.0")
		POSITIVE (f4, U"F4 (Hz)", U"3500.0")
		POSITIVE (b4, U"B4 (Hz)", U"350.0")
	EDITOR_OK
	EDITOR_DO
		Melder_require (f3 < f4,
			U"F4 should be larger than F3.");
		my pref_synthesis_numberOfFormants () = my p_synthesis_numberOfFormants = 4;
		my extraFrequencyBandwidthPairs [1] = f3;
		my extraFrequencyBandwidthPairs [2] = b3;
		my extraFrequencyBandwidthPairs [3] = f4;
		my extraFrequencyBandwidthPairs [4] = b4;
	EDITOR_END
}

static void menu_cb_reverseTrajectory (VowelEditor me, EDITOR_ARGS_DIRECT) {
	Trajectory_reverse (my trajectory.get());
	updateInfoLabels (me);
	Graphics_updateWs (my graphics.get());
}

static void menu_cb_newTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"New Trajectory", nullptr);
		POSITIVE (startF1, U"Start F1 (Hz)", U"700.0")
		POSITIVE (startF2, U"Start F2 (Hz)", U"1200.0")
		POSITIVE (endF1, U"End F1 (Hz)", U"350.0")
		POSITIVE (endF2, U"End F2 (Hz)", U"800.0")
		POSITIVE (newDuration, U"Duration (s)", my default_trajectory_newDuration ())
		WORD (colour_string, U"Colour", my default_trajectory_colour ());
	EDITOR_OK
		SET_REAL (newDuration, my p_trajectory_newDuration)
		SET_STRING (colour_string, my p_trajectory_colour)  // TODO SET_COLOUR
	EDITOR_DO
		clipF1F2 (me, & startF1, & startF2);
		MelderColour colour = MelderColour_fromColourNameOrRGBString (colour_string);
		my trajectory = Trajectory_create (newDuration);
		Trajectory_addPoint (my trajectory.get(), 0.0, startF1, startF2, colour);
		clipF1F2 (me, & endF1, & endF2);
		Trajectory_addPoint (my trajectory.get(), newDuration, endF1, endF2, colour);
		GuiText_setString (my durationTextField, Melder_fixed (newDuration, 6));
		my pref_trajectory_newDuration () = my p_trajectory_newDuration = newDuration;
		pref_str32cpy2 (my pref_trajectory_colour (), my p_trajectory_colour, colour_string);
		updateInfoLabels (me);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_extendTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extend Trajectory", nullptr);
		POSITIVE (toF1, U"To F1 (Hz)", U"500.0")
		POSITIVE (toF2, U"To F2 (Hz)", U"1500.0")
		POSITIVE (extendDuration, U"Extend duration (s)", my default_trajectory_extendDuration ())
		WORD (colour_string, U"Colour", my default_trajectory_colour ());
	EDITOR_OK
		SET_REAL (extendDuration, my p_trajectory_extendDuration)
		SET_STRING (colour_string, my p_trajectory_colour)  // TODO SET_COLOUR
	EDITOR_DO
		MelderColour colour = MelderColour_fromColourNameOrRGBString (colour_string);
		TrajectoryPoint startPoint = my trajectory -> points.at [my trajectory -> points.size];
		startPoint -> colour = colour;
		const double startTime = startPoint -> number;
		const double endTime = startTime + extendDuration;
		clipF1F2 (me, & toF1, & toF2);
		Trajectory_addPoint (my trajectory.get(), endTime, toF1, toF2, colour);
		GuiText_setString (my durationTextField, Melder_fixed (endTime, 6));
		GuiText_setString (my extendTextField, Melder_fixed (extendDuration, 6));
		my pref_trajectory_extendDuration () = my p_trajectory_extendDuration = extendDuration;
		my pref_trajectory_duration () = my p_trajectory_duration = endTime;
		pref_str32cpy2 (my pref_trajectory_colour (), my p_trajectory_colour, colour_string);
		updateInfoLabels (me);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_modifyTrajectoryDuration (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Modify duration", nullptr);
		POSITIVE (newDuration, U"New duration (s)", my default_trajectory_duration ())
	EDITOR_OK
		SET_REAL (newDuration, my p_trajectory_duration)
	EDITOR_DO
		Melder_require (newDuration > my p_trajectory_minimumDuration,
			U"The duration should be larger than ", my p_trajectory_minimumDuration, U" s.");
		my pref_trajectory_duration () = my p_trajectory_duration = newDuration;
		Trajectory_newDuration (my trajectory.get(), newDuration);
		GuiText_setString (my durationTextField, Melder_fixed (newDuration, 6));
	EDITOR_END
}

static void menu_cb_shiftTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Shift trajectory", nullptr);
		REAL (f1_st, U"F1 (semitones)", U"0.5")
		REAL (f2_st, U"F2 (semitones)", U"0.5")
	EDITOR_OK
	EDITOR_DO
		Trajectory_shift_semitones (my trajectory.get(), f1_st, f2_st);
		updateInfoLabels (me);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_trajectoryTimeMarksEvery (VowelEditor me, EDITOR_ARGS_FORM) { // deprecated 20200404
	EDITOR_FORM (U"Trajectory time marks every", nullptr);
		POSITIVE (distance, U"Distance (s)", my default_trajectory_markEvery ())
	EDITOR_OK
		SET_REAL (distance, my p_trajectory_markEvery)
	EDITOR_DO
		my pref_trajectory_markEvery () = my p_trajectory_markEvery = distance;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_trajectory_colour (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Trajectory colour settings", nullptr);
		REAL (startTime, U"left Time range (s)", U"0.0")
		REAL (endTime, U"right Time range (s)", my default_trajectory_duration ())
		WORD (colour_string, U"Colour", my default_trajectory_colour ())
	EDITOR_OK
		SET_REAL (endTime, my p_trajectory_duration)
		SET_STRING (colour_string, my p_trajectory_colour)  // TODO SET_COLOUR
	EDITOR_DO
		pref_str32cpy2 (my pref_trajectory_colour (), my p_trajectory_colour, colour_string);
		Trajectory_setColour (my trajectory.get(), startTime, endTime, MelderColour_fromColourNameOrRGBString(colour_string));
		Graphics_updateWs (my graphics.get());
	EDITOR_END

}

#pragma mark - button methods

static void playTrajectory (VowelEditor me) {
	autoSound thee = VowelEditor_createTargetSound (me);
	Sound_play (thee.get(), nullptr, nullptr);
}

static void gui_button_cb_play (VowelEditor me, GuiButtonEvent /* event */) {
	playTrajectory (me);
}

static void gui_button_cb_publish (VowelEditor me, GuiButtonEvent /* event */) {
	autoSound publish = VowelEditor_createTargetSound (me);
	Editor_broadcastPublication (me, publish.move());
}

static void gui_button_cb_reverse (VowelEditor me, GuiButtonEvent /* event */) {
	Trajectory_reverse (my trajectory.get());
	playTrajectory (me);
	updateInfoLabels (me);
	Graphics_updateWs (my graphics.get());
}

static void gui_drawingarea_cb_expose (VowelEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	Melder_assert (me);
	Melder_assert (my trajectory);
	if (! my graphics)
		return;   // could be the case in the very beginning

	Graphics_setGrey (my graphics.get(), 0.9);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setInner (my graphics.get());
	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (my graphics.get(), 1.0);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_unsetInner (my graphics.get());
	Graphics_setGrey (my graphics.get(), 0.0);

	VowelEditor_drawBackground (me, my graphics.get());
	Melder_assert (me);
	Melder_assert (my trajectory);
	VowelEditor_drawF1F2Trajectory (me, my graphics.get());
}

static void gui_drawingarea_cb_resize (VowelEditor me, GuiDrawingArea_ResizeEvent event) {
	Melder_assert (me);
	if (! my graphics)
		return;   // could be the case in the very beginning
	my width = event -> width;
	my height = event -> height;
	Graphics_setWsViewport (my graphics.get(), 0.0, my width, 0.0, my height);
	Graphics_setWsWindow (my graphics.get(), 0.0, my width, 0.0, my height);
	Graphics_setViewport (my graphics.get(), 0.0, my width, 0.0, my height);
	Graphics_updateWs (my graphics.get());
	/*
		Save the current shell size as the user's preference.
	*/
	my pref_shell_width() = my p_shell_width = GuiShell_getShellWidth  (my windowForm);
	my pref_shell_height() = my p_shell_height = GuiShell_getShellHeight (my windowForm);
}

// shift key always extends what already is.
// Special case : !soundFollowsMouse. The first click just defines the vowel's first f1f2-position,
static void gui_drawingarea_cb_mouse (VowelEditor me, GuiDrawingArea_MouseEvent event) {
	static double anchorTime;
	static double previousX;
	static double previousY;
	static double dt;
	Graphics_setInner (my graphics.get());
	double mouseX, mouseY;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & mouseX, & mouseY);
	Melder_clip (0.0, & mouseX, 1.0);
	Melder_clip (0.0, & mouseY, 1.0);
	double f1, f2;
	VowelEditor_getF1F2FromXY (me, mouseX, mouseY, & f1, & f2);
	MelderColour colour = MelderColour_fromColourNameOrRGBString (my p_trajectory_colour);
	if (event -> isClick()) {
		anchorTime = Melder_clock ();
		if (event -> shiftKeyPressed) {
			VowelEditor_updateFromExtendDurationTextWidget (me);
			const double duration = dt = my trajectory -> xmax + my p_trajectory_extendDuration;
			Trajectory_addPoint (my trajectory.get(), duration, f1, f2, colour);
			GuiText_setString (my durationTextField, Melder_double (duration));
		} else {
			const double duration = dt = 0.0;
			my trajectory = Trajectory_create (my p_trajectory_minimumDuration);
			Trajectory_addPoint (my trajectory.get(), duration, f1, f2, colour);
			GuiText_setString (my durationTextField, Melder_double (duration));
			if (! my p_soundFollowsMouse)
				Trajectory_addPoint (my trajectory.get(), my p_trajectory_minimumDuration, f1, f2, colour);
		}
		previousX = mouseX;
		previousY = mouseY;
	} else {
		double duration = Melder_clock () - anchorTime + dt;
		if (mouseX != previousX || mouseY != previousY) {
			Trajectory_addPoint (my trajectory.get(), duration, f1, f2, colour);
			GuiText_setString (my durationTextField, Melder_fixed (duration, 6));
			previousX = mouseX;
			previousY = mouseY;
		}
		if (event -> isDrop()) {
			if (my trajectory -> points.size == 1) {
				/*
					Add a point with a slightly modified second formant because successive points should not have equal f1 and f2 values.
				*/
				Melder_clipLeft (my p_trajectory_minimumDuration, & duration);
				GuiText_setString (my durationTextField, Melder_fixed (duration, 6));
				Trajectory_addPoint (my trajectory.get(), duration, f1, 1.00001 * f2, colour);   // points have to be different
			}
			autoSound sound = VowelEditor_createTargetSound (me);
			Sound_play (sound.get(), nullptr, nullptr);
		}
	}
	Graphics_unsetInner (my graphics.get());
	updateInfoLabels (me);
	Graphics_updateWs (my graphics.get());
}

static void updateWidgets (void *void_me) {
	iam (VowelEditor);
	(void) me;
}

void structVowelEditor :: v_destroy () noexcept {
	VowelEditor_Parent :: v_destroy ();
}

void structVowelEditor :: v_createMenus () {
	VowelEditor_Parent :: v_createMenus ();

	Editor_addMenu (this, U"View", 0);
	Editor_addCommand (this, U"File", U"Preferences...", 0, menu_cb_prefs);
	Editor_addCommand (this, U"File", U"-- publish data --", 0, nullptr);
	Editor_addCommand (this, U"File", U"Publish Sound", 0, menu_cb_publishSound);
	Editor_addCommand (this, U"File", U"Extract KlattGrid", 0, menu_cb_extract_KlattGrid);
	Editor_addCommand (this, U"File", U"Extract FormantGrid", 0, menu_cb_extract_FormantGrid);
	Editor_addCommand (this, U"File", U"Extract PitchTier", 0, menu_cb_extract_PitchTier);
	Editor_addCommand (this, U"File", U"Extract Trajectory as Table", 0, menu_cb_extract_TrajectoryAsTable);
	Editor_addCommand (this, U"File", U"-- drawing --", 0, nullptr);
	Editor_addCommand (this, U"File", U"Draw trajectory...", 0, menu_cb_drawTrajectory);
	Editor_addCommand (this, U"File", U"-- scripting --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"-- f0 --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Set f0...", 0, menu_cb_setF0);
	Editor_addCommand (this, U"Edit", U"Set F3 & F4...", Editor_HIDDEN, menu_cb_setF3F4);
	Editor_addCommand (this, U"Edit", U"-- trajectory commands --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Reverse trajectory", 0, menu_cb_reverseTrajectory);
	Editor_addCommand (this, U"Edit", U"Modify trajectory duration...", 0, menu_cb_modifyTrajectoryDuration);
	Editor_addCommand (this, U"Edit", U"New trajectory...", 0, menu_cb_newTrajectory);
	Editor_addCommand (this, U"Edit", U"Extend trajectory...", 0, menu_cb_extendTrajectory);
	Editor_addCommand (this, U"Edit", U"Shift trajectory...", 0, menu_cb_shiftTrajectory);
	Editor_addCommand (this, U"Query", U"Trajectory info", 0, menu_cb_trajectoryInfo);
	Editor_addCommand (this, U"View", U"F1 & F2 range...", 0, menu_cb_ranges_f1f2);
	Editor_addCommand (this, U"View", U"--show vowel marks--", 0, nullptr);
	Editor_addCommand (this, U"View", U"Show one vowel mark...", Editor_HIDDEN, menu_cb_showOneVowelMark);
	Editor_addCommand (this, U"View", U"Vowel marks...", Editor_HIDDEN, menu_cb_vowelMarks);
	Editor_addCommand (this, U"View", U"Vowel marks from fixed set...", 0, menu_cb_vowelMarks);
	Editor_addCommand (this, U"View", U"Show vowel marks from Table file...", 0, menu_cb_vowelMarksFromTableFile);
	Editor_addCommand (this, U"View", U"--show trajectory settings--", 0, nullptr);
	Editor_addCommand (this, U"View", U"Trajectory colour...", 0, menu_cb_trajectory_colour);
	Editor_addCommand (this, U"View", U"Trajectory time marks every...", 0, menu_cb_trajectoryTimeMarksEvery);
}

void structVowelEditor :: v_createHelpMenuItems (EditorMenu menu) {
	VowelEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"VowelEditor help", '?', menu_cb_help);
}

void structVowelEditor :: v_createChildren ()
{
	const int button_width = 90, text_width = 110, status_info_width = 400;
	int top, bottom, bottom_widgets_top, bottom_widgets_bottom, bottom_widgets_halfway;

	// Three buttons on a row: Play, Reverse, Publish
	int left = 10, right = left + button_width;
	bottom_widgets_top = top = -MARGIN_BOTTOM + 10;
	bottom_widgets_bottom = bottom = -STATUS_INFO;
	playButton = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Play", gui_button_cb_play, this, 0);
	left = right + 10;
	right = left + button_width;
	reverseButton = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Reverse", gui_button_cb_reverse, this, 0);
	left = right + 10;
	right = left + button_width;
	publishButton = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Publish", gui_button_cb_publish, this, 0);
	/*
		Four Text widgets with the label on top: Duration, Extend, f0, Slope.
		Make the f0 slope button 10 wider to accomodate the text
		We wil not use a callback from a Text widget. It will get called multiple times during the editing
		of the text. Better to have all editing done and then query the widget for its value!
	*/
	left = right + 10;
	right = left + text_width;
	bottom_widgets_halfway = bottom = (top + bottom) / 2;
	top = bottom_widgets_top;
	GuiLabel_createShown (our windowForm, left, right, top , bottom, U"Duration (s):", 0);
	top = bottom;
	bottom = bottom_widgets_bottom;
	durationTextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	left = right + 10;
	right = left + text_width;
	top = bottom_widgets_top;
	bottom = bottom_widgets_halfway;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Extend (s):", 0);
	top = bottom;
	bottom = bottom_widgets_bottom;
	extendTextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	left = right + 10;
	right = left + text_width;
	top = bottom_widgets_top;
	bottom = bottom_widgets_halfway;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Start f0 (Hz):", 0);
	top = bottom;
	bottom = bottom_widgets_bottom;
	f0TextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	left = right + 10;
	right = left + text_width + 10;
	top = bottom_widgets_top;
	bottom = bottom_widgets_halfway;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Slope f0 (oct/s):", 0);
	top = bottom;
	bottom = bottom_widgets_bottom;
	f0SlopeTextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);
	/*
		The status startInfo and endInfo widget at the bottom:
	*/
	bottom = - (STATUS_INFO - Gui_LABEL_HEIGHT) / 2;
	top = bottom - Gui_LABEL_HEIGHT;
	left = 10; //MARGIN_LEFT;
	right = left + status_info_width;
	startInfo = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);

	left = right;
	right = left + status_info_width;
	endInfo = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);
	/*
		Create drawing area.
		Approximately square because for our defaults: f1min=200, f1max=1000 and f2min = 500, f2mx = 2500,
		log distances are equal (log (1000/200) == log (2500/500) ).
	*/
	drawingArea = GuiDrawingArea_createShown (our windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_mouse,   // TODO: mouse-dragged and mouse-up events
		nullptr, gui_drawingarea_cb_resize, this, 0
	);
	our width  = GuiControl_getWidth  (drawingArea);
	our height = GuiControl_getHeight (drawingArea);
}

autoVowelEditor VowelEditor_create (conststring32 title, Daata data) {
	try {
		trace (U"enter");
		autoVowelEditor me = Thing_new (VowelEditor);
		Melder_assert (me.get());
		if (my p_shell_width <= 0 || my p_shell_height <= 0) {
			my p_shell_width = Melder_atof (my default_shell_width ());
			my p_shell_height = Melder_atof (my default_shell_height ());
		}
		Editor_init (me.get(), 0, 0, my pref_shell_width(), my pref_shell_height(), title, data);
#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
#endif
		my graphics = Graphics_create_xmdrawingarea (my drawingArea);
		Melder_assert (my graphics);
		Graphics_setFontSize (my graphics.get(), 12);

		if (my p_window_f1min >= my p_window_f1max) {
			my p_window_f1min = Melder_atof (my default_window_f1min ());
			my p_window_f1max = Melder_atof (my default_window_f1max ());
		}
		if (my p_window_f2min >= my p_window_f2max) {
			my p_window_f2min = Melder_atof (my default_window_f2min ());
			my p_window_f2max = Melder_atof (my default_window_f2max ());
		}
		if (my p_marks_fontSize <= 0)
			my p_marks_fontSize = Melder_atof (my default_marks_fontSize ());
		if (Melder_equ (my p_marks_fileName, U"") && my p_marks_dataSet < kVowelEditor_marksDataSet::MIN) {
			my p_marks_dataSet = my default_marks_dataSet ();
			my p_marks_speakerType = my default_marks_speakerType ();
		}
		VowelEditor_getMarks (me.get());
		if (my p_synthesis_numberOfFormants <= 0)
			my p_synthesis_numberOfFormants = Melder_atoi (my default_synthesis_numberOfFormants ());
		if (my p_synthesis_q1 <= 0 || my p_synthesis_q2 <= 0) {
			my p_synthesis_q1 = Melder_atof (my default_synthesis_q1 ());
			my p_synthesis_q2 = Melder_atof (my default_synthesis_q2 ());
		}
		if (str32len (my p_synthesis_extraFBPairs) == 0)
			pref_str32cpy (my p_synthesis_extraFBPairs, my default_synthesis_extraFBPairs ());
		my extraFrequencyBandwidthPairs = newVECfromString (my p_synthesis_extraFBPairs);
		Melder_assert (my extraFrequencyBandwidthPairs.size >= 4);   // for deprecated Set F3 & F4
		my p_soundFollowsMouse = true;   // no real preference yet
		if (my p_synthesis_samplingFrequency <= 0.0)
			my p_synthesis_samplingFrequency = Melder_atof (my default_synthesis_samplingFrequency ());
		if (my p_trajectory_minimumDuration <= 0.0)
			my p_trajectory_minimumDuration = Melder_atof (my default_trajectory_minimumDuration ());
		if (my p_trajectory_extendDuration <= 0.0 || my p_trajectory_markEvery <= 0.0) {
			my p_trajectory_extendDuration = Melder_atof (my default_trajectory_extendDuration ());
			my p_trajectory_markEvery = Melder_atof (my default_trajectory_markEvery ());
		}
		VowelEditor_create_twoFormantSchwa (me.get());
		if (my p_f0_start <= 0)
			my p_f0_start = Melder_atof (my default_f0_start ());
		GuiText_setString (my f0TextField, Melder_double (my p_f0_start));
		if (my p_f0_slope <= 0)
			my p_f0_slope = Melder_atof (my default_f0_slope ());
		if (my p_f0_minimum <= 0 || my p_f0_maximum <= 0) {
			my p_f0_minimum = Melder_atof (my default_f0_minimum ());
			my p_f0_maximum = Melder_atof (my default_f0_maximum ());
		}
		GuiText_setString (my f0SlopeTextField, Melder_double (my p_f0_slope));
		GuiText_setString (my durationTextField, U"0.2");   // source has been created
		GuiText_setString (my extendTextField, Melder_double (my p_trajectory_extendDuration));
		if (my p_grid_df1 <= 0)
			my p_grid_df1 = Melder_atof (my default_grid_df1 ());
		if (my p_grid_df2 <= 0)
			my p_grid_df2 = Melder_atof (my default_grid_df2 ());
		updateWidgets (me.get());
		updateInfoLabels (me.get());
		trace (U"exit");
		return me;
	} catch (MelderError) {
		Melder_throw (U"VowelEditor not created.");
	}
}

/* End of file VowelEditor.cpp */
