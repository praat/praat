/* VowelEditor.cpp
 *
 * Copyright (C) 2008-2020 David Weenink, 2015,2017,2018 Paul Boersma
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
  djmw 20080202, 20080330
  djmw 20090114 FormantTier_to_FormantGrid.
  djmw 20090613 Extract KlattGrid
  djmw 20110329 Table_get(Numeric|String)Value is now Table_get(Numeric|String)Value_Assert
  djmw 20110331 Corrected a typo
*/

/*
trajectory --> path ????
 The main part of the VowelEditor is a drawing area.
 In this drawing area a cursor can be moved around by a mouse.
 The position of the cursor is related to the F1 and F2 frequencies.
 On_mouse_down the position of the cursor is sampled (Not at fixed intervals!).
 This results in a series of (x,y) values that will be transformed to (F1,F2) values in Hertz
 The corresponding sound wil be made audible after the mouse is released.

 The user graphics area is the F1-F2 plane: here the origin is at the top-right with log(F2) on the 
 horizontal axis and log(F1) on the vertical axis (i.e. log (F1) top-down, log(F2) right-to-left)
 F1, F2 are always evaluated to herts values;
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

#include "EditorM.h"
#include "FormantGrid.h"
#include "KlattGrid.h"
#include "PitchTier_to_Sound.h"
#include "Polygon.h"
#include "Preferences.h"
#include "TableOfReal_extensions.h"
#include "Table_extensions.h"
#include "VowelEditor.h"
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

#define MICROSECPRECISION(x) (round((x)*1000000)/1000000)

#pragma mark - class TrajectoryPointTier

Thing_implement (TrajectoryPoint, AnyPoint, 0);

static autoTrajectoryPoint TrajectoryPoint_create (double time, double x, double y, MelderColour colour) {
	autoTrajectoryPoint thee = Thing_new (TrajectoryPoint);
	thy number = time;
	thy x =  x;
	thy y = y;
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
		Melder_throw (U"trajectory not created.");
	}
}

static void Trajectory_addPoint (Trajectory me, double time, double x, double y, MelderColour colour) {
	try {
		autoTrajectoryPoint point = TrajectoryPoint_create (time, x, y, colour);
		my points . addItem_move (std::move (& point));
	} catch (MelderError) {
		Melder_throw (me, U" no point added.");
	}
}

#pragma mark - class Vowel

Thing_implement (VowelSpecification, Function, 0);

static autoVowelSpecification VowelSpecification_create (double duration) {
	try {
		autoVowelSpecification me = Thing_new (VowelSpecification);
		Function_init (me.get(), 0.0, duration);
		my formantTier = FormantTier_create (0.0, duration);
		my pitchTier = PitchTier_create (0.0, duration);
		return me;
	} catch (MelderError) {
		Melder_throw (U"VowelSpecification not created.");
	}
}

static autoVowelSpecification VowelSpecification_create_twoFormantSchwa (double duration) {
	try {
		autoVowelSpecification me = VowelSpecification_create (duration);
		autoFormantPoint fp = FormantPoint_create (0.0, 2);
		fp -> formant [1] = 500.0;
		fp -> bandwidth [1] = 50.0;
		fp -> formant [2] = 1500.0;
		fp -> bandwidth [2] = 150.0;
		my formantTier -> points. addItem_move (fp.move());
		RealTier_addPoint (my pitchTier.get(), 0.0, 140.0);

		fp = FormantPoint_create (duration, 2);
		fp -> formant [1] = 500.0;
		fp -> bandwidth [1] = 50.0;
		fp -> formant [2] = 1500.0;
		fp -> bandwidth [2] = 150.0;
		my formantTier -> points. addItem_move (fp.move());
		RealTier_addPoint (my pitchTier.get(), duration, 140.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Schwa Vowel not created");
	}
}

static autoFormantGrid FormantTier_to_FormantGrid (FormantTier me) {
	try {
		const integer numberOfFormants = FormantTier_getMaxNumFormants (me);
		autoFormantGrid thee = FormantGrid_createEmpty (my xmin, my xmax, numberOfFormants);
		for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
			const FormantPoint fp = my points.at [ipoint];
			const double t = fp -> number;
			for (integer iformant = 1; iformant <= fp -> numberOfFormants; iformant ++) {
				FormantGrid_addFormantPoint (thee.get(), iformant, t, fp -> formant [iformant]);
				FormantGrid_addBandwidthPoint (thee.get(), iformant, t, fp -> bandwidth [iformant]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no FormantGrid created.");
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

#define REPRESENTNUMBER(x,i) (isundef (x) ? U" undef" : Melder_pad (6, Melder_fixed (x, 1)))
static void appendF1F2F0 (MelderString *statusInfo, conststring32 intro, double f1, double f2, double f0, conststring32 ending) {
	MelderString_append (statusInfo, intro, REPRESENTNUMBER (f1, 1), U", ", REPRESENTNUMBER (f2, 2), U", ", REPRESENTNUMBER (f0, 3), ending);
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

static void	clipXY (double *x, double *y) {
	Melder_clip (0.0, x, 1.0);
	Melder_clip (0.0, y, 1.0);
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
	if (isundef (extend) || extend <= my p_trajectory_minimumDuration || extend > my p_trajectory_maximumDuration)
		extend = my p_trajectory_minimumDuration;
	GuiText_setString (my extendTextField, Melder_double (extend));
	my pref_trajectory_extendDuration () = my p_trajectory_extendDuration = extend;
}

static double VowelEditor_updateFromDurationTextWidget (VowelEditor me) {
	double duration = getRealFromTextWidget (my durationTextField);
	if (isundef (duration) || duration < my p_trajectory_minimumDuration)
		duration = my p_trajectory_minimumDuration;
	my pref_trajectory_duration () = my p_trajectory_duration = duration;
	GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (duration)));
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
	if (numberOfSamples > my nx - istart + 1)
		numberOfSamples = my nx - istart + 1;

	for (integer i = 1; i <= numberOfSamples; i ++) {
		const double phase = NUMpi * (i - 1) / (numberOfSamples - 1);
		my z [1] [istart + i - 1] *= 0.5 * (1.0 - cos (phase));
	}
}

static void Sound_fadeOut (Sound me, double duration) {
	integer numberOfSamples = Melder_ifloor (duration / my dx);

	if (numberOfSamples > my nx)
		numberOfSamples = my nx;
	if (numberOfSamples < 2)
		return;
	const integer istart = my nx - numberOfSamples;
	/*
		Only one channel
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

static void VowelEditor_reverseFormantTier (VowelEditor me) {
	const FormantTier ft = my vowel -> formantTier.get();
	const double duration = ft -> xmax;
	const integer np = ft -> points.size, np_2 = np / 2;

	for (integer i = 1; i <= np_2; i ++) {
		FormantPoint fpt = ft -> points.at [i];
		ft -> points.at [i] = ft -> points.at [np - i + 1];
		ft -> points.at [np - i + 1] = fpt;
		fpt = ft -> points.at [i];
		fpt -> number = duration - fpt -> number;
		fpt = ft -> points.at [np - i + 1];
		fpt -> number = duration - fpt -> number;
	}
	if (np % 2 == 1) {
		const FormantPoint fpt = ft -> points.at [np_2 + 1];
		fpt -> number = duration - fpt -> number;
	}
}

static void VowelEditor_shiftF1F2 (VowelEditor me, double f1_st, double f2_st) {
	FormantTier ft = my vowel -> formantTier.get();
	for (integer i = 1; i <= ft -> points.size; i ++) {
		const FormantPoint fp = ft -> points.at [i];
		double f1 = fp -> formant [1], f2 = fp -> formant [2];

		f1 *= pow (2, f1_st / 12.0);
		Melder_clip (my p_window_f1min, & f1, my p_window_f1max);
		fp -> formant [1] = f1;
		fp -> bandwidth [1] = f1 / my p_synthesis_q1;

		f2 *= pow (2, f2_st / 12.0);
		Melder_clip (my p_window_f2min, & f2, my p_window_f2max);
		fp -> formant [2] = f2;
		fp -> bandwidth [2] = f2 / my p_synthesis_q2;
	}
}

static void FormantTier_newDuration (FormantTier me, double newDuration) {
	if (newDuration != my xmax) {
		const double multiplier = newDuration / my xmax;
		for (integer i = 1; i <= my points.size; i ++) {
			const FormantPoint fp = my points.at [i];
			fp -> number *= multiplier;
		}
		my xmax *= multiplier;
	}
}

static void VowelEditor_updateVowelSpecification (VowelEditor me) {
	/*
		Always update; GuiObject text might have changed
	*/
	const double newDuration = VowelEditor_updateFromDurationTextWidget (me);
	VowelEditor_updateFromF0StartAndSlopeTextWidgets (me);
	const VowelSpecification thee = my vowel.get();
	if (newDuration != thy xmax) {
		const double multiplier = newDuration / thy xmax;
		FormantTier_newDuration (thy formantTier.get(), newDuration);
		thy xmax *= multiplier;
	}
	const PitchTier him = my vowel -> pitchTier.get();
	const double multiplier = newDuration / his xmax;
	for (integer i = 1; i <= his points.size; i ++) {
		const RealPoint pp = his points.at [i];
		pp -> number *= multiplier;
		pp -> value = VowelEditor_getF0AtTime (me, pp -> number);
	}
	his xmax *= multiplier;
}

static autoSound VowelEditor_createTargetSound (VowelEditor me) {
	try {
		VowelEditor_updateVowelSpecification (me);   // update pitch and duration
		const autoFormantTier formantTier = Data_copy (my vowel -> formantTier.get());
		/* mutable return */ autoSound thee = PitchTier_to_Sound_pulseTrain (my vowel -> pitchTier.get(), my p_synthesis_samplingFrequency, 0.7, 0.05, 30, false);
		/*
			Modify the formant point size if p_numberOfFormants < size
		*/
		const integer numberOfExtraFormants = my p_synthesis_numberOfFormants - 2;
		for (integer ipoint = 1; ipoint <= formantTier -> points.size; ipoint ++) {
			const FormantPoint point = formantTier -> points.at [ipoint];
			Melder_clipRight (& point -> numberOfFormants, my p_synthesis_numberOfFormants);
			point -> formant. resize (point -> numberOfFormants);   // maintain invariant
			/*
				Since the time that the FormantTier was created, the synthesis preferences might
				have been changed by the user. E.g. the user moves the mouse, hears the sound and then
				changes the Preferences and hits the Play button again. We have to synthesize now according 
				to the new preferences.
			*/
			point -> bandwidth [1] = point -> formant [1] / my p_synthesis_q1;
			if (point -> numberOfFormants < 2)
				continue;
			point -> bandwidth [2] = point -> formant [2] / my p_synthesis_q2;
			for (integer ifor = 1; ifor <= numberOfExtraFormants; ifor ++) {
				if (point -> numberOfFormants < 2 + ifor)
					break;
				point -> formant [2 + ifor] = my extraFrequencyBandwidthPairs [2 * ifor - 1];
				point -> bandwidth [2 + ifor] = my extraFrequencyBandwidthPairs [2 * ifor];
			}
		}
		Sound_FormantTier_filter_inplace (thee.get(), formantTier.get());
		Vector_scale (thee.get(), 0.99);
		Sound_fadeIn (thee.get(), 0.005, true);
		Sound_fadeOut (thee.get(), 0.005);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Target Sound not created.");
	}
}

static void VowelEditor_VowelSpecification_addPoint (VowelEditor me, double time, double x, double y, bool transFromXYToFrequencies) {
	const VowelSpecification thee = my vowel.get();
	if (time > thy xmax) {
		thy xmax = time;
		thy formantTier -> xmax = time;
		thy pitchTier -> xmax = time;
	}
	const double f0 = VowelEditor_getF0AtTime (me, time);
	/*
		We reserve storage for all formants, during play we may filter.
	*/
	const integer numberOfExtraFormants = my extraFrequencyBandwidthPairs.size / 2;
	/* mutable move */ autoFormantPoint point = FormantPoint_create (time, 2 + numberOfExtraFormants);
	double f1 = x, f2 = y;
	if (transFromXYToFrequencies)
		VowelEditor_getF1F2FromXY (me, x, y, & f1, & f2);

	point -> formant [1] = f1;
	point -> bandwidth [1] = f1 / my p_synthesis_q1;
	point -> formant [2] = f2;
	point -> bandwidth [2] = f2 / my p_synthesis_q2;
	for (integer ifor = 1; ifor <= numberOfExtraFormants; ifor ++) {
		point -> formant [2 + ifor] = my extraFrequencyBandwidthPairs [2 * ifor - 1];
		point -> bandwidth [2 + ifor] = my extraFrequencyBandwidthPairs [2 * ifor];
	}
	thy formantTier -> points. addItem_move (point.move());
	RealTier_addPoint (thy pitchTier.get(), time, f0);
}

static void VowelEditor_drawF1F2Trajectory (VowelEditor me, Graphics g) {
// Our FormantTiers always have a FormantPoint at t=xmin and t=xmax;
	const FormantTier thee = my vowel -> formantTier.get();
	Melder_assert (thy points.size >= 2);

	const integer glt = Graphics_inqLineType (g);
	const double glw = Graphics_inqLineWidth (g);
	const MelderColour colour = Graphics_inqColour (g);
	const integer nfp = thy points.size;
	trace (U"number of points ", nfp);
	const FormantPoint fp1 = thy points.at [1];
	const double markLength = 0.01;

	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setColour (g, MelderColour_fromColourName (my p_trajectory_colour));
	if (thy xmax - thy xmin < 0.005)   // too short to hear?
		Graphics_setColour (g, Melder_RED);
	
	auto getx = [=](double f) { return log (f / my p_window_f2max) / log (my p_window_f2min / my p_window_f2max); };
	auto gety = [=](double f) { return log (f / my p_window_f1max) / log (my p_window_f1min / my p_window_f1max); };

	double x1 = getx (fp1 -> formant [2]);
	double y1 = gety (fp1 -> formant [1]);
	double x1p = x1, y1p = y1;
	double t1 = fp1 -> number;
	integer imark = 1;
	for (integer it = 2; it <= nfp; it ++) {
		const FormantPoint fp = thy points.at [it];
		const double x2 = getx (fp -> formant [2]);
		const double y2 = gety (fp -> formant [1]);
		const double t2 = fp -> number;
		Graphics_setLineWidth (g, 3);
		if (x1 == x2 && y1 == y2) {
			x1 = x1p;
			y1 = y1p;
		} else {
			Graphics_line (g, x1, y1, x2, y2);
		}
		double tm;
		while (my p_trajectory_markEvery > 0 && (tm = my p_trajectory_markEvery * imark) < t2) {
			// line orthogonal to y = (y1/x1)*x is y = -(x1/y1)*x
			const double fraction = (tm - t1) / (t2 - t1);
			const double dx = x2 - x1, dy = y2 - y1;
			const double xm = x1 + fraction * dx;
			const double ym = y1 + fraction * dy;
			const double d = sqrt (dx * dx + dy * dy);
			if (d > 0.0) {
				double xl1 = dy * markLength / d, xl2 = - xl1;
				double yl1 = dx * markLength / d, yl2 = - yl1;

				if (dx * dy > 0.0) {
					xl1 = -fabs (xl1);
					yl1 = fabs (yl1);
					xl2 = fabs (xl1);
					yl2 = -fabs (yl1);
				} else if (dx * dy < 0.0) {
					xl1 = -fabs (xl1);
					yl1 = -fabs (yl1);
					xl2 = fabs (xl1);
					yl2 = fabs (yl1);
				}
				Graphics_setLineWidth (g, 1);
				trace (xm, U" ", ym, U" ", xl1, U" ", xl2, U" ", yl1, U" ", yl2);
				Graphics_line (g, xm + xl1, ym + yl1, xm + xl2, ym + yl2);
			}
			imark ++;
		}
		x1p = x1;
		y1p = y1;
		x1 = x2;
		y1 = y2;
		t1 = t2;
	}
	// Arrow at end
	{
		const double gas = Graphics_inqArrowSize (g), arrowSize = 1.0;
		const double size = 10.0 * arrowSize * Graphics_getResolution (g) / 75.0 / my width;
		const double sizeSquared = size * size;
		Graphics_setArrowSize (g, arrowSize);
		integer it = 1;
		const FormantPoint fpn = thy points.at [nfp];
		FormantPoint fpi;
		while (it <= nfp - 1) {
			fpi = thy points.at [nfp - it];
			const double dx = getx (fpn -> formant [2]) - getx (fpi -> formant [2]);
			const double dy = gety (fpn -> formant [1]) - gety (fpi -> formant [1]);
			const double d2 = dx * dx + dy * dy;
			if (d2 > sizeSquared)
				break;
			it ++;
		}
		Graphics_arrow (g, getx (fpi -> formant [2]), gety (fpi -> formant [1]), getx (fpn -> formant [2]), gety (fpn -> formant [1]));
		Graphics_setArrowSize (g, gas);
	}
	Graphics_unsetInner (g);
	Graphics_setColour (g, colour);
	Graphics_setLineType (g, glt);
	Graphics_setLineWidth (g, glw);
}

static void Table_addColumnIfNotExists_size (Table me, double size) {
	const integer col_size = Table_findColumnIndexFromColumnLabel (me, U"Size");
	if (col_size == 0) {
		Table_appendColumn (me, U"Size");
		for (integer irow = 1; irow <= my rows.size; irow ++)
			Table_setNumericValue (me, irow, my numberOfColumns, size);
	}
}

static void Table_addColumnIfNotExists_colour (Table me, conststring32 colour) {
	integer col_colour = Table_findColumnIndexFromColumnLabel (me, U"Colour");
	if (col_colour == 0) {
		Table_appendColumn (me, U"Colour");
		for (integer irow = 1; irow <= my rows.size; irow ++)
			Table_setStringValue (me, irow, my numberOfColumns, colour);
	}
}

static void VowelEditor_getVowelMarksFromFile (VowelEditor me) {
	try {
		Melder_require (str32len (my p_marks_fileName) > 0,
			U"There is no file defined with vowel marks.");
		structMelderFile file { };
		Melder_pathToFile (my p_marks_fileName, & file);
		autoDaata data = Data_readFromFile (& file);
		Melder_require (Thing_isa (data.get(), classTable),
			U"\"", MelderFile_name (& file), U"\" is not a Table file");

		autoTable newMarks = data.static_cast_move <structTable> ();
		/*
			Require columns Vowel F1 & F2 to be present
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
	const char32 *speaker = ( my p_marks_speakerType == kVowelEditor_speakerType::MAN ? U"m" :
		my p_marks_speakerType == kVowelEditor_speakerType::WOMAN ? U"w" :
		my p_marks_speakerType == kVowelEditor_speakerType::CHILD ? U"c": U"m" );
	if (my p_marks_dataSet == kVowelEditor_marksDataSet::AMERICAN_ENGLISH) {   // American-English
		const autoTable thee = Table_create_petersonBarney1952 ();
		te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, speaker);
	} else if (my p_marks_dataSet == kVowelEditor_marksDataSet::DUTCH) {
		if (my p_marks_speakerType == kVowelEditor_speakerType::CHILD) {
			const autoTable thee = Table_create_weenink1983 ();
			te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, speaker);
		}
		else {   // male + female from Pols van Nierop
			const autoTable thee = Table_create_polsVanNierop1973 ();
			te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, speaker);
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
		Draw the marks
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
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
				Graphics_text (g, x1, y1, label);
			}
		}
	}
	Graphics_setFontSize (g, fontSize);
	Graphics_setGrey (g, 0.0); // black
	/*
		Draw the line F1=F2
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
			Graphics_setGrey (g, 0.0); // black
			Graphics_line (g, xl1, yl1, xl2, yl2);
		}
	}
	/*
		Draw the horizontal grid lines
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
		Graphics_setGrey (g, 0.0); // black
	}
	/*
		Draw the vertical grid lines
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
		Graphics_setGrey (g, 0.0); // black
	}
	Graphics_setLineWidth (g, 2.0);
	Graphics_rectangle (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_unsetInner (g);
	Graphics_setGrey (g, 0.0); // black
	Graphics_markLeft (g, 0.0, false, true, false, Melder_double (my p_window_f1max));
	Graphics_markLeft (g, 1.0, false, true, false, Melder_double (my p_window_f1min));
	Graphics_markTop (g, 0.0, false, true, false, Melder_double (my p_window_f2max));
	Graphics_markTop (g, 1.0, false, true, false, Melder_double (my p_window_f2min));

}

#pragma mark - menu methods

static void menu_cb_help (VowelEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"VowelEditor");
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
		Graphics_updateWs (my graphics.get());
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
	VowelEditor_updateVowelSpecification (me);
	autoFormantGrid publish = FormantTier_to_FormantGrid (my vowel -> formantTier.get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_KlattGrid (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateVowelSpecification (me);
	autoFormantGrid fg = FormantTier_to_FormantGrid (my vowel -> formantTier.get());
	autoKlattGrid publish = KlattGrid_create (fg -> xmin, fg -> xmax, fg -> formants.size, 0, 0, 0, 0, 0, 0);
	KlattGrid_addVoicingAmplitudePoint (publish.get(), fg -> xmin, 90.0);
	KlattGrid_replacePitchTier (publish.get(), my vowel -> pitchTier.get());
	KlattGrid_replaceFormantGrid (publish.get(), kKlattGridFormantType::ORAL, fg.get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_PitchTier (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateVowelSpecification (me);
	autoPitchTier publish = Data_copy (my vowel -> pitchTier.get());
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

static void menu_cb_showVowelMarks (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show vowel marks", nullptr);
		OPTIONMENU_ENUM (kVowelEditor_marksDataSet, dataSet, U"Data set", my default_marks_dataSet ())
		OPTIONMENU_ENUM (kVowelEditor_speakerType, speaker, U"Speaker", my default_marks_speakerType ())
		POSITIVE (fontSize, U"Font size (points)", my default_marks_fontSize ())
	EDITOR_OK
		SET_ENUM (dataSet, kVowelEditor_marksDataSet, my p_marks_dataSet)
		SET_ENUM (speaker, kVowelEditor_speakerType, my p_marks_speakerType)
		SET_REAL (fontSize, my p_marks_fontSize)
	EDITOR_DO
		my pref_marks_dataSet		() = my p_marks_dataSet = dataSet;
		my pref_marks_speakerType	() = my p_marks_speakerType = speaker;
		my pref_marks_fontSize		() = my p_marks_fontSize = fontSize;
		VowelEditor_getMarks (me);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_showVowelMarksFromTableFile (VowelEditor me, EDITOR_ARGS_FORM) {  // deprecated 20200404
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
	VowelEditor_reverseFormantTier (me);
	Graphics_updateWs (my graphics.get());
}

static void menu_cb_newTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"New Trajectory", nullptr);
		POSITIVE (startF1, U"Start F1 (Hz)", U"700.0")
		POSITIVE (startF2, U"Start F2 (Hz)", U"1200.0")
		POSITIVE (endF1, U"End F1 (Hz)", U"350.0")
		POSITIVE (endF2, U"End F2 (Hz)", U"800.0")
		POSITIVE (duration, U"Duration (s)", my default_trajectory_duration ())
	EDITOR_OK
		SET_REAL (duration, my p_trajectory_duration)
	EDITOR_DO
		my vowel = VowelSpecification_create (duration);
		clipF1F2 (me, & startF1, & startF2);
		VowelEditor_VowelSpecification_addPoint (me, 0.0, startF1, startF2, false);
		clipF1F2 (me, & endF1, & endF2);
		VowelEditor_VowelSpecification_addPoint (me, duration, endF1, endF2, false);
		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (duration)));
		my pref_trajectory_duration () = my p_trajectory_duration = duration;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_extendTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extend Trajectory", nullptr);
		POSITIVE (toF1, U"To F1 (Hz)", U"500.0")
		POSITIVE (toF2, U"To F2 (Hz)", U"1500.0")
		POSITIVE (extendDuration, U"Extend duration (s)", my default_trajectory_extendDuration ())
	EDITOR_OK
		SET_REAL (extendDuration, my p_trajectory_extendDuration)
	EDITOR_DO
		const double endTime = my vowel -> xmax + extendDuration;
		clipF1F2 (me, & toF1, & toF2);
		VowelEditor_VowelSpecification_addPoint (me, endTime, toF1, toF2, false);
		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (endTime)));
		GuiText_setString (my extendTextField, Melder_double (MICROSECPRECISION (extendDuration)));
		my pref_trajectory_extendDuration () = my p_trajectory_extendDuration = extendDuration;
		my pref_trajectory_duration () = my p_trajectory_duration = endTime;
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
		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (newDuration)));
	EDITOR_END
}

static void menu_cb_shiftTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Shift trajectory", nullptr);
		REAL (f1, U"F1 (semitones)", U"0.5")
		REAL (f2, U"F2 (semitones)", U"0.5")
	EDITOR_OK
	EDITOR_DO
		VowelEditor_shiftF1F2 (me, f1, f2);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_showTrajectoryTimeMarksEvery (VowelEditor me, EDITOR_ARGS_FORM) { // deprecated 20200404
	EDITOR_FORM (U"Show trajectory time marks every", nullptr);
		POSITIVE (distance, U"Distance (s)", my default_trajectory_markEvery ())
	EDITOR_OK
		SET_REAL (distance, my p_trajectory_markEvery)
	EDITOR_DO
		my pref_trajectory_markEvery () = my p_trajectory_markEvery = distance;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_trajectory_settings (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Trajectory settings", nullptr);
		POSITIVE (distance, U"Distance between time marks (s)", my default_trajectory_markEvery ())
		WORD (colour, U"Trajectory coulour", my default_trajectory_colour ())
	EDITOR_OK
		SET_REAL (distance, my p_trajectory_markEvery)
		SET_STRING (colour, my p_trajectory_colour)  // TODO SET_COLOUR
	EDITOR_DO
		my pref_trajectory_markEvery () = my p_trajectory_markEvery = distance;
		pref_str32cpy2 (my pref_trajectory_colour (), my p_trajectory_colour, colour);
		Graphics_updateWs (my graphics.get());
	EDITOR_END

}

#pragma mark - button methods

static void gui_button_cb_play (VowelEditor me, GuiButtonEvent /* event */) {
	autoSound thee = VowelEditor_createTargetSound (me);
	Sound_play (thee.get(), nullptr, nullptr);
	Graphics_updateWs (my graphics.get());
}

static void gui_button_cb_publish (VowelEditor me, GuiButtonEvent /* event */) {
	autoSound publish = VowelEditor_createTargetSound (me);
	Editor_broadcastPublication (me, publish.move());
}

static void gui_button_cb_reverse (VowelEditor me, GuiButtonEvent /* event */) {
	VowelEditor_reverseFormantTier (me);
	structGuiButtonEvent play_event { };
	gui_button_cb_play (me, & play_event);
}

static void gui_drawingarea_cb_expose (VowelEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	Melder_assert (me);
	Melder_assert (my vowel);
	double ts = my vowel -> xmin, te = my vowel -> xmax;
	const FormantTier ft = my vowel -> formantTier.get();
	Melder_assert (ft);
	static MelderString statusInfo;
	if (! my graphics)
		return;   // could be the case in the very beginning
	Graphics_clearWs (my graphics.get());

	appendF1F2F0 (& statusInfo, U"Start (F1,F2,f0) = (", FormantTier_getValueAtTime (ft, 1, ts),
		FormantTier_getValueAtTime (ft, 2, ts), VowelEditor_getF0AtTime (me, ts), U")");
	GuiLabel_setText (my startInfo, statusInfo.string);
	MelderString_empty (& statusInfo);

	appendF1F2F0 (& statusInfo, U"End (F1,F2,f0) = (", FormantTier_getValueAtTime (ft, 1, te),
		FormantTier_getValueAtTime (ft, 2, te), VowelEditor_getF0AtTime (me, te), U")");
	GuiLabel_setText (my endInfo, statusInfo.string);
	MelderString_empty (& statusInfo);

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
	Melder_assert (my vowel);
	Melder_assert (my vowel -> formantTier);
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

	/* Save the current shell size as the user's preference */

	my pref_shell_width () = my p_shell_width = GuiShell_getShellWidth  (my windowForm);
	my pref_shell_height () = my p_shell_height = GuiShell_getShellHeight (my windowForm);
}

// shift key always extends what already is.
// Special case : !soundFollowsMouse. The first click just defines the vowel's first f1f2-position,
static void gui_drawingarea_cb_click (VowelEditor me, GuiDrawingArea_ClickEvent event) {
	const double t0 = Melder_clock ();
	integer iskipped = 0;
	struct structGuiButtonEvent gb_event { 0 };
	Graphics_setInner (my graphics.get());

	double x, y, t, dt = 0.0;
	Graphics_getMouseLocation (my graphics.get(), & x, & y);
	clipXY (& x, & y);

	if (event -> shiftKeyPressed) {
		VowelEditor_updateFromExtendDurationTextWidget (me);
		(my shiftKeyPressed) ++;
		dt = my vowel -> xmax + my p_trajectory_extendDuration;
		t = 0.0 + dt;
		VowelEditor_VowelSpecification_addPoint (me, t, x, y, true);
		GuiText_setString (my durationTextField, Melder_double (t));
		goto end;
	} else {
		t = 0.0;
		my shiftKeyPressed = 0;
		my vowel = VowelSpecification_create (my p_trajectory_minimumDuration);
		VowelEditor_VowelSpecification_addPoint (me, t, x, y, true);
		GuiText_setString (my durationTextField, Melder_double (t));
		if (! my p_soundFollowsMouse) {
			VowelEditor_VowelSpecification_addPoint (me, my p_trajectory_minimumDuration, x, y, true);
			goto end;
		}
	}

	Graphics_xorOn (my graphics.get(), Melder_BLUE);
	while (Graphics_mouseStillDown (my graphics.get())) {
		const double xb = x, yb = y, tb = t;
		t = Melder_clock () - t0 + dt; // Get relative time in seconds from the clock
		Graphics_getMouseLocation (my graphics.get(), & x, & y);
		clipXY (& x, & y);
		/*
			If the new point equals the previous one: no tier update
		*/
		if (xb == x && yb == y) {
			iskipped ++;
			continue;
		}
		/*
			Add previous point only if at least one previous event was skipped...
		*/
		if (iskipped > 0)
			VowelEditor_VowelSpecification_addPoint (me, tb, xb, yb, true);
		iskipped = 0;
		Graphics_line (my graphics.get(), xb, yb, x, y);

		VowelEditor_VowelSpecification_addPoint (me, t, x, y, true);
		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (t)));
	}
	t = Melder_clock () - t0;
	/*
		To prevent ultra short clicks we set a minimum duration
	*/
	Melder_clipLeft (my p_trajectory_minimumDuration, & t);
	t += dt;
	GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (t)));
	VowelEditor_VowelSpecification_addPoint (me, t, x, y, true);

	Graphics_xorOff (my graphics.get());

end:
	Graphics_unsetInner (my graphics.get());

	//if (! my shiftKeyPressed)
	//	my vowel = athee.move();
	//Melder_assert (! athee);
	gui_button_cb_play (me, & gb_event);
}

#if 0
static void gui_drawingarea_cb_key (VowelEditor /* me */, GuiDrawingArea_KeyEvent /* event */) {
}
#endif

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
	Editor_addCommand (this, U"View", U"F1 & F2 range...", 0, menu_cb_ranges_f1f2);
	Editor_addCommand (this, U"View", U"--show vowel marks--", 0, nullptr);
	Editor_addCommand (this, U"View", U"Show one vowel mark...", Editor_HIDDEN, menu_cb_showOneVowelMark);
	Editor_addCommand (this, U"View", U"Show vowel marks...", Editor_HIDDEN, menu_cb_showVowelMarks);
	Editor_addCommand (this, U"View", U"Show vowel marks from fixed set...", 0, menu_cb_showVowelMarks);
	Editor_addCommand (this, U"View", U"Show vowel marks from Table file...", 0, menu_cb_showVowelMarksFromTableFile);
	Editor_addCommand (this, U"View", U"--show trajectory settings--", 0, nullptr);
	Editor_addCommand (this, U"View", U"Trajectory settings...", 0, menu_cb_trajectory_settings);
	Editor_addCommand (this, U"View", U"Show trajectory time marks every...", Editor_HIDDEN, menu_cb_showTrajectoryTimeMarksEvery);
}

void structVowelEditor :: v_createHelpMenuItems (EditorMenu menu) {
	VowelEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"VowelEditor help", '?', menu_cb_help);
}

void structVowelEditor :: v_createChildren ()
{
	const int button_width = 90, text_width = 95, status_info_width = 330;
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
		Four Text widgets with the label on top: Duration, Extend, f0, Slope
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
	left = MARGIN_LEFT;
	right = left + status_info_width;
	startInfo = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);

	left = right;
	right = left + status_info_width;
	endInfo = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);
	/*
		Create drawing area.
		Approximately square because for our defaults: f1min=200, f1max=1000 and f2min = 500, f2mx = 2500,
		log distances are equal (log (1000/200) == log (2500/500) ).
		drawingArea = GuiDrawingArea_createShown (our windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
	*/
	drawingArea = GuiDrawingArea_createShown (our windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, nullptr, gui_drawingarea_cb_resize, this, 0);
	width  = GuiControl_getWidth  (drawingArea);
	height = GuiControl_getHeight (drawingArea);
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
		Editor_init (me.get(), 0, 0, my p_shell_width, my p_shell_height, title, data);
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
		Melder_assert (my extraFrequencyBandwidthPairs.size >= 4); // For deprecated Set F3 & F4
		my p_soundFollowsMouse = true; // No real preference yet
		if (my p_synthesis_samplingFrequency <= 0.0)
			my p_synthesis_samplingFrequency = Melder_atof (my default_synthesis_samplingFrequency ());
		if (my p_trajectory_minimumDuration <= 0.0 || my p_trajectory_maximumDuration <= 0.0) {
			my p_trajectory_minimumDuration = Melder_atof (my default_trajectory_minimumDuration ());
			my p_trajectory_maximumDuration = Melder_atof (my default_trajectory_maximumDuration ());
		}
		if (my p_trajectory_extendDuration <= 0.0 || my p_trajectory_markEvery <= 0.0) {
			my p_trajectory_extendDuration = Melder_atof (my default_trajectory_extendDuration ());
			my p_trajectory_markEvery = Melder_atof (my default_trajectory_markEvery ());
		}
		if (my data)
			my vowel = Data_copy (static_cast<VowelSpecification> (data));
		else
			my vowel = VowelSpecification_create_twoFormantSchwa (0.2);
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
		GuiText_setString (my durationTextField, U"0.2"); // Source has been created
		GuiText_setString (my extendTextField, Melder_double (my p_trajectory_extendDuration));
		if (my p_grid_df1 <= 0)
			my p_grid_df1 = Melder_atof (my default_grid_df1 ());
		if (my p_grid_df2 <= 0)
			my p_grid_df2 = Melder_atof (my default_grid_df2 ());
{
	/*
		This exdents because it's a hack:
	*/
	struct structGuiDrawingArea_ResizeEvent event { my drawingArea, 0 };
	event. width  = GuiControl_getWidth  (my drawingArea);
	event. height = GuiControl_getHeight (my drawingArea);
	gui_drawingarea_cb_resize (me.get(), & event);
}
		updateWidgets (me.get());
		trace (U"exit");
		return me;
	} catch (MelderError) {
		Melder_throw (U"VowelEditor not created.");
	}
}

/* End of file VowelEditor.cpp */
