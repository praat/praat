/* VowelEditor.cpp
 *
 * Copyright (C) 2008-2017 David Weenink, 2015,2017 Paul Boersma
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
 On_mouse_down the position of the cursor is sampled (Not a fixed intervals!).
 This results in a series of (x,y) values that will be transformed to (F1,F2) values in Hertz
 The corresponding sound wil be made audible until the mouse is released.

 Graphics area is F1-F2 plane. Origin top-right with log(F2) horizontal and log(F1) vertical).
 Axis orientation from topright: F1 down, F2 to the left.
 F1, F2 are always evaluated to Hz;
 In the Graphics part, the Graphics_window (0, 1, 0, 1), i.e. origin is bottom-left.
 log(fmin) -> 1; log(fmax)-> 0
 Transformations XY <=> F1F2: VowelEditor_getXYFromF1F2(...) and VowelEditor_getF1F2FromXY(...)
  For x direction F2 from right to left
   1 = a * log(f2min) +b
   0 = a * log(f2max) +b
   x' = a (log(f2)-log(f2max))

   1 = a * log(f1min) +b
   0 = a * log(f1max) +b
   y' = a (log(f1)-log(f1max))
 TO DO:
 The third and higher formant frequencies can also be set indirectly by defining them as functions on the f1,f2 plane
 (for example, by an object of type Matrix).
 Make sound-follows-mouse real time!
*/

#include "FormantGrid.h"
#include "KlattGrid.h"
#include "../external/portaudio/portaudio.h"
#include "PitchTier_to_PointProcess.h"
#include "PitchTier_to_Sound.h"
#include "PointProcess_and_Sound.h"
#include "Polygon.h"
#include "TableOfReal_extensions.h"
#include "Table_extensions.h"
#include "VowelEditor.h"
#include "machine.h"
#include "Preferences.h"
#include "EditorM.h"
#include <time.h>

#if defined (macintosh)
	#include <sys/time.h>
#elif defined (linux)
	#include <sys/time.h>
	#include <signal.h>
#endif

Thing_implement (VowelEditor, Editor, 0);

// Male, Female, Child speaker
#define VG_SPEAKER_M 0
#define VG_SPEAKER_F 1
#define VG_SPEAKER_C 2

// STATUS_INFO >=Gui_LABEL_HEIGHT !!
#define STATUS_INFO (3*Gui_LABEL_HEIGHT/2)
#define MARGIN_RIGHT 10
#define MARGIN_LEFT 50
#define MARGIN_TOP 50
#define MARGIN_BOTTOM (60+STATUS_INFO)
#define BUFFER_SIZE_SEC 4
#define SAMPLING_FREQUENCY 44100

#define STATUSINFO_STARTINTR0 U"Start (F1,F2,F0) = ("
#define STATUSINFO_ENDINTR0 U"End (F1,F2,F0) = ("
#define STATUSINFO_ENDING U")"
#define MICROSECPRECISION(x) (round((x)*1000000)/1000000)

// To prevent the generation of inaudible short Sounds we set a minimum duration
#define MINIMUM_SOUND_DURATION 0.01

// maximum number of marks
#define VowelEditor_MAXIMUM_MARKERS 30

static structVowelEditor_F0 f0default { 140.0, 0.0, 40.0, 2000.0, SAMPLING_FREQUENCY, 1, 0.0, 2000 };
static structVowelEditor_F1F2Grid griddefault { 200.0, 500.0, 0, 1, 0, 1, 0.5 };

#include "oo_DESTROY.h"
#include "Vowel_def.h"
#include "oo_COPY.h"
#include "Vowel_def.h"
#include "oo_EQUAL.h"
#include "Vowel_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Vowel_def.h"
#include "oo_WRITE_TEXT.h"
#include "Vowel_def.h"
#include "oo_READ_TEXT.h"
#include "Vowel_def.h"
#include "oo_WRITE_BINARY.h"
#include "Vowel_def.h"
#include "oo_READ_BINARY.h"
#include "Vowel_def.h"
#include "oo_DESCRIPTION.h"
#include "Vowel_def.h"

#pragma mark - preferences

struct markInfo {
	double f1, f2;
	int size;
	char32 vowel [Preferences_STRING_BUFFER_SIZE];
};

static struct {
	int shellWidth, shellHeight;
	bool soundFollowsMouse;
	double f1min, f1max, f2min, f2max;
	double f3, b3, f4, b4;
	double markTraceEvery, extendDuration;
	int speakerType, marksDataset, numberOfMarks, marksFontSize;
	char32 mark [VowelEditor_MAXIMUM_MARKERS] [Preferences_STRING_BUFFER_SIZE];
} prefs;

void VowelEditor_prefs () {
	Preferences_addInt (U"VowelEditor.shellWidth", & prefs.shellWidth, 500);
	Preferences_addInt (U"VowelEditor.shellHeight", & prefs.shellHeight, 500);
	Preferences_addBool (U"VowelEditor.soundFollowsMouse", & prefs.soundFollowsMouse, true);
	Preferences_addDouble (U"VowelEditor.f1min", & prefs.f1min, 200.0);
	Preferences_addDouble (U"VowelEditor.f1max", & prefs.f1max, 1200.0);
	Preferences_addDouble (U"VowelEditor.f2min", & prefs.f2min, 500.0);
	Preferences_addDouble (U"VowelEditor.f2max", & prefs.f2max, 3500.0);
	Preferences_addDouble (U"VowelEditor.f3", & prefs.f3, 2500.0);
	Preferences_addDouble (U"VowelEditor.b3", & prefs.b3, 250.0);
	Preferences_addDouble (U"VowelEditor.f4", & prefs.f4, 3500.0);
	Preferences_addDouble (U"VowelEditor.b4", & prefs.b4, 350.0);
	Preferences_addDouble (U"VowelEditor.markTraceEvery", & prefs.markTraceEvery, 0.05);
	Preferences_addDouble (U"VowelEditor.extendDuration", & prefs.extendDuration, 0.05);
	Preferences_addInt (U"VowelEditor.speakerType", & prefs.speakerType, 1);   // TODO: replace with enum
	Preferences_addInt (U"VowelEditor.marksDataset", & prefs.marksDataset, 2);   // TODO: replace with enum
	Preferences_addInt (U"VowelEditor.marksFontsize", & prefs.marksFontSize, 14);
	Preferences_addInt (U"VowelEditor.numberOfMarks", & prefs.numberOfMarks, 12);   // 12 is the number of vowels in the default (Dutch) marksDataset
	/*
	 * We don't know how many markers there will be, so the prefs file needs to have the maximum number.
	 */
	for (integer i = 1; i <= VowelEditor_MAXIMUM_MARKERS; i ++) {
		Preferences_addString (Melder_cat (U"VowelEditor.mark", (i < 10 ? U"0" : U""), i), & prefs.mark [i - 1] [0], U"x");
	}
}

#pragma mark - class Vowel

Thing_implement (Vowel, Function, 0);

static autoVowel Vowel_create (double duration) {
	try {
		autoVowel me = Thing_new (Vowel);
		Function_init (me.get(), 0.0, duration);
		my ft = FormantTier_create (0.0, duration);
		my pt = PitchTier_create (0.0, duration);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Vowel not created.");
	}
}

static autoVowel Vowel_create_twoFormantSchwa (double duration) {
	try {
		autoVowel me = Vowel_create (duration);
		autoFormantPoint fp = FormantPoint_create (0.0);
		fp -> formant [0] = 500.0;
		fp -> bandwidth [0] = 50.0;
		fp -> formant [1] = 1500.0;
		fp -> bandwidth [1] = 150.0;
		fp -> numberOfFormants = 2;
		my ft -> points. addItem_move (fp.move());
		RealTier_addPoint (my pt.get(), 0.0, 140.0);

		fp = FormantPoint_create (duration);
		fp -> formant [0] = 500.0;
		fp -> bandwidth [0] = 50.0;
		fp -> formant [1] = 1500.0;
		fp -> bandwidth [1] = 150.0;
		fp -> numberOfFormants = 2;
		my ft -> points. addItem_move (fp.move());
		RealTier_addPoint (my pt.get(), duration, 140.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Schwa Vowel not created");
	}
}

static autoSound Vowel_to_Sound_pulses (Vowel me, double samplingFrequency, double adaptFactor, double adaptTime, integer interpolationDepth) {
	try {
		autoPointProcess pp = PitchTier_to_PointProcess (my pt.get());
		autoSound thee = PointProcess_to_Sound_pulseTrain (pp.get(), samplingFrequency, adaptFactor, adaptTime, interpolationDepth);
		Sound_FormantTier_filter_inplace (thee.get(), my ft.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Sound with pulses not created.");
	}
}

static autoFormantGrid FormantTier_to_FormantGrid (FormantTier me) {
	try {
		integer numberOfFormants = FormantTier_getMaxNumFormants (me);
		autoFormantGrid thee = FormantGrid_createEmpty (my xmin, my xmax, numberOfFormants);
		for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
			FormantPoint fp = my points.at [ipoint];
			double t = fp -> number;
			for (integer iformant = 1; iformant <= fp -> numberOfFormants; iformant ++) {
				FormantGrid_addFormantPoint (thee.get(), iformant, t, fp -> formant [iformant - 1]);
				FormantGrid_addBandwidthPoint (thee.get(), iformant, t, fp -> bandwidth [iformant - 1]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no FormantGrid created.");
	}
}

static void VowelEditor_getXYFromF1F2 (VowelEditor me, double f1, double f2, double *x, double *y) {
	*x = log (f2 / my f2max) / log (my f2min / my f2max);
	*y = log (f1 / my f1max) / log (my f1min / my f1max);
}

//Graphics_DCtoWC ????
static void VowelEditor_getF1F2FromXY (VowelEditor me, double x, double y, double *f1, double *f2) {
	*f2 = my f2min * pow (my f2max / my f2min, 1.0 - x);
	*f1 = my f1min * pow (my f1max / my f1min, 1.0 - y);
}

#define REPRESENTNUMBER(x,i) (isundef (x) ? U" undef" : Melder_pad (6, Melder_fixed (x, 1)))
static void appendF1F2F0 (MelderString *statusInfo, const char32 *intro, double f1, double f2, double f0, const char32 *ending) {
	MelderString_append (statusInfo, intro, REPRESENTNUMBER (f1, 1), U", ", REPRESENTNUMBER (f2, 2), U", ", REPRESENTNUMBER (f0, 3), ending);
}

static double getRealFromTextWidget (GuiText me) {
	double value = undefined;
	char32 *dirty = GuiText_getString (me);
	try {
		Interpreter_numericExpression (nullptr, dirty, & value);
	} catch (MelderError) {
		Melder_clearError ();
		value = undefined;
	}
	Melder_free (dirty);
	return value;
}

static void checkF1F2 (VowelEditor me, double *f1, double *f2) {
	if (*f1 < my f1min) {
		*f1 = my f1min;
	}
	if (*f1 > my f1max) {
		*f1 = my f1max;
	}
	if (*f2 < my f2min) {
		*f2 = my f2min;
	}
	if (*f2 > my f2max) {
		*f1 = my f2max;
	}
}

static void checkF0 (structVowelEditor_F0 *f0p, double *f0) {
	if (isundef (*f0)) {
		*f0 = f0p -> start;
	}
	if (*f0 > f0p -> maximum) {
		*f0 = f0p -> maximum;
	}
	if (*f0 < f0p -> minimum) {
		*f0 = f0p -> minimum;
	}
}

static void	checkXY (double *x, double *y) {
	if (*x < 0.0) {
		*x = 0.0;
	} else if (*x > 1.0) {
		*x = 1.0;
	}
	if (*y < 0.0) {
		*y = 0.0;
	} else if (*y > 1.0) {
		*y = 1.0;
	}
}

static double Matrix_getValue (Matrix me, double /* x */, double /* y */) {
	return my z [1] [1];
}

static void VowelEditor_getF3F4 (VowelEditor me, double f1, double f2, double *f3, double *b3, double *f4, double *b4) {
	*f3 = Matrix_getValue (my f3.get(), f2, f1);
	*b3 = Matrix_getValue (my b3.get(), f2, f1);
	*f4 = Matrix_getValue (my f4.get(), f2, f1);
	*b4 = Matrix_getValue (my b4.get(), f2, f1);
}

static void VowelEditor_updateF0Info (VowelEditor me) {
	double f0 = getRealFromTextWidget (my f0TextField);
	checkF0 (& my f0, & f0);
	GuiText_setString (my f0TextField, Melder_double (f0));
	my f0.start = f0;
	double slopeOctPerSec = getRealFromTextWidget (my f0SlopeTextField);
	if (isundef (slopeOctPerSec)) {
		slopeOctPerSec = f0default.slopeOctPerSec;
	}
	my f0.slopeOctPerSec = slopeOctPerSec;
	GuiText_setString (my f0SlopeTextField, Melder_double (my f0.slopeOctPerSec));
}

static void VowelEditor_updateExtendDuration (VowelEditor me) {
	double extend = getRealFromTextWidget (my extendTextField);
	if (isundef (extend) || extend <= MINIMUM_SOUND_DURATION || extend > my maximumDuration) {
		extend = MINIMUM_SOUND_DURATION;
	}
	GuiText_setString (my extendTextField, Melder_double (extend));
	my extendDuration = prefs.extendDuration = extend;
}

static double VowelEditor_updateDurationInfo (VowelEditor me) {
	double duration = getRealFromTextWidget (my durationTextField);
	if (isundef (duration) || duration < MINIMUM_SOUND_DURATION) {
		duration = MINIMUM_SOUND_DURATION;
	}
	GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (duration)));
	return duration;
}

static void Sound_fadeIn (Sound me, double duration, bool fromFirstNonZeroSample) {
	integer istart = 1, numberOfSamples = Melder_ifloor (duration / my dx);   // ppgb: waarom afronden naar beneden?

	if (numberOfSamples < 2) {
		return;
	}
	if (fromFirstNonZeroSample) {
		// If the first part of the sound is very low level we put sample values to zero and
		// start windowing from the position where the amplitude is above the minimum level.
		// WARNING: this part is special for the artificial vowels because
		// 1. They have no offset
		// 2. They are already scaled to a maximum amplitude of 0.99
		// 3. For 16 bit precision
		double zmin = 0.5 / pow (2.0, 16.0);
		while (fabs (my z [1] [istart]) < zmin && istart < my nx) {
			my z [1] [istart] = 0.0; // To make sure
			istart ++;
		}
	}
	if (numberOfSamples > my nx - istart + 1) {
		numberOfSamples = my nx - istart + 1;
	}

	for (integer i = 1; i <= numberOfSamples; i ++) {
		double phase = NUMpi * (i - 1) / (numberOfSamples - 1);

		my z [1] [istart + i - 1] *= 0.5 * (1.0 - cos (phase));
	}
}

static void Sound_fadeOut (Sound me, double duration) {
	integer istart, numberOfSamples = Melder_ifloor (duration / my dx);

	if (numberOfSamples < 2) {
		return;
	}
	if (numberOfSamples > my nx) {
		numberOfSamples = my nx;
	}
	istart = my nx - numberOfSamples;
	// only one channel
	for (integer i = 1; i <= numberOfSamples; i ++) {
		double phase = NUMpi * (i - 1) / (numberOfSamples - 1);

		my z [1] [istart + i] *= 0.5 * (1.0 + cos (phase));
	}
}

static double getF0 (structVowelEditor_F0 *f0p, double time) {
	double f0 = f0p -> start * pow (2.0, f0p -> slopeOctPerSec * time);
	if (f0 < f0p -> minimum) {
		f0 = f0p -> minimum;
	} else if (f0 > f0p -> maximum) {
		f0 = f0p -> maximum;
	}
	return f0;
}

static void VowelEditor_Vowel_reverseFormantTier (VowelEditor me) {
	FormantTier ft = my vowel -> ft.get();
	double duration = ft -> xmax;
	integer np = ft -> points.size, np_2 = np / 2;

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
		FormantPoint fpt = ft -> points.at [np_2 + 1];
		fpt -> number = duration - fpt -> number;
	}
}

static void VowelEditor_shiftF1F2 (VowelEditor me, double f1_st, double f2_st) {
	FormantTier ft = my vowel -> ft.get();
	for (integer i = 1; i <= ft -> points.size; i ++) {
		FormantPoint fp = ft -> points.at [i];
		double f1 = fp -> formant [0], f2 = fp -> formant [1];

		f1 *= pow (2, f1_st / 12.0);
		if (f1 < my f1min) {
			f1 = my f1min;
		}
		if (f1 > my f1max) {
			f1 = my f1max;
		}
		fp -> formant [0] = f1;
		fp -> bandwidth [0] = f1 / 10.0;

		f2 *= pow (2, f2_st / 12.0);
		if (f2 < my f2min) {
			f2 = my f2min;
		}
		if (f2 > my f2max) {
			f2 = my f2max;
		}
		fp -> formant [1] = f2;
		fp -> bandwidth [1] = f2 / 10.0;
		double f3, b3, f4, b4;
		VowelEditor_getF3F4 (me, f1, f2, & f3, & b3, & f4, & b4);
		fp -> formant [2] = f3;
		fp -> bandwidth [2] = b3;
		fp -> formant [3] = f4;
		fp -> bandwidth [3] = b4;
	}
}

static void FormantTier_newDuration (FormantTier me, double newDuration) {
	if (newDuration != my xmax) {
		double multiplier = newDuration / my xmax;

		for (integer i = 1; i <= my points.size; i ++) {
			FormantPoint fp = my points.at [i];
			fp -> number *= multiplier;
		}
		my xmax *= multiplier;
	}
}

static void PitchTier_newDuration (PitchTier me, structVowelEditor_F0 *f0p, double newDuration) {
	// Always update; GuiObject text might have changed
	double multiplier = newDuration / my xmax;
	for (integer i = 1; i <= my points.size; i ++) {
		RealPoint pp = my points.at [i];
		pp -> number *= multiplier;
		pp -> value = getF0 (f0p, pp -> number);
	}
	my xmax *= multiplier;
}

static void Vowel_newDuration (Vowel me, structVowelEditor_F0 *f0p, double newDuration) {
	if (newDuration != my xmax) {
		double multiplier = newDuration / my xmax;
		FormantTier_newDuration (my ft.get(), newDuration);
		my xmax *= multiplier;
	}
	PitchTier_newDuration (my pt.get(), f0p, newDuration);   // always update
}

static void VowelEditor_updateVowel (VowelEditor me) {
	double newDuration = VowelEditor_updateDurationInfo (me); // Get new duration from TextWidget
	VowelEditor_updateF0Info (me);   // get new pitch and slope values from TextWidgets
	Vowel_newDuration (my vowel.get(), & my f0, newDuration);
}

static double getCoordinate (double fmin, double fmax, double f) {
	return log (f / fmax) / log (fmin / fmax);
}

static autoSound VowelEditor_createTarget (VowelEditor me) {
	try {
		VowelEditor_updateVowel (me);   // update pitch and duration
		autoSound thee = Vowel_to_Sound_pulses (my vowel.get(), 44100.0, 0.7, 0.05, 30);
		Vector_scale (thee.get(), 0.99);
		Sound_fadeIn (thee.get(), 0.005, true);
		Sound_fadeOut (thee.get(), 0.005);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Target Sound not created.");
	}
}

#define GETX(x) (getCoordinate (f2min, f2max, x))
#define GETY(y) (getCoordinate (f1min, f1max, y))
static void FormantTier_drawF1F2Trajectory (FormantTier me, Graphics g, double f1min, double f1max, double f2min, double f2max, double markTraceEvery, double width) {
// Our FormantTiers always have a FormantPoint at t=xmin and t=xmax;
	Melder_assert (my points.size >= 2);

	int it, imark = 1, glt = Graphics_inqLineType (g);
	double glw = Graphics_inqLineWidth (g), x1, x1p, y1, y1p, t1;
	Graphics_Colour colour = Graphics_inqColour (g);
	integer nfp = my points.size;
	trace (U"number of points ", nfp);
	FormantPoint fp = my points.at [1];
	FormantPoint fpn = my points.at [nfp];
	double tm, markLength = 0.01;

	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	// Too short too hear ?
	if ( (my xmax - my xmin) < 0.005) {
		Graphics_setColour (g, Graphics_RED);
	}
	x1p = x1 = GETX (fp -> formant [1]);
	y1p = y1 = GETY (fp -> formant [0]);
	t1 = fp -> number;
	for (it = 2; it <= nfp; it ++) {
		fp = my points.at [it];
		double x2 = GETX (fp -> formant [1]);
		double y2 = GETY (fp -> formant [0]);
		double t2 = fp -> number;
		Graphics_setLineWidth (g, 3);
		if (x1 == x2 && y1 == y2) {
			x1 = x1p;
			y1 = y1p;
		} else {
			Graphics_line (g, x1, y1, x2, y2);
		}
		while (markTraceEvery > 0 && (tm = markTraceEvery * imark) < t2) {
			// line orthogonal to y = (y1/x1)*x is y = -(x1/y1)*x
			double fraction = (tm - t1) / (t2 - t1);
			double dx = x2 - x1, dy = y2 - y1;
			double xm = x1 + fraction * dx;
			double ym = y1 + fraction * dy;
			double d = sqrt (dx * dx + dy * dy);
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
		x1p = x1; y1p = y1;
		x1 = x2; y1 = y2; t1 = t2;
	}
	// Arrow at end
	{
		double gas = Graphics_inqArrowSize (g), arrowSize = 1.0;
		double size = 10.0 * arrowSize * Graphics_getResolution (g) / 75.0 / width, size2 = size * size;
		Graphics_setArrowSize (g, arrowSize);
		it = 1;
		while (it <= (nfp - 1)) {
			fp = my points.at [nfp - it];
			double dx = GETX (fpn -> formant [1]) - GETX (fp -> formant [1]);
			double dy = GETY (fpn -> formant [0]) - GETY (fp -> formant [0]);
			double d2 = dx * dx + dy * dy;
			if (d2 > size2) {
				break;
			}
			it ++;
		}
		Graphics_arrow (g, GETX (fp -> formant [1]), GETY (fp -> formant [0]), GETX (fpn -> formant [1]), GETY (fpn -> formant [0]));
		Graphics_setArrowSize (g, gas);
	}
	Graphics_unsetInner (g);
	Graphics_setColour (g, colour);
	Graphics_setLineType (g, glt);
	Graphics_setLineWidth (g, glw);
}
#undef GETX
#undef GETY

static void copyVowelMarksInPreferences_volatile (Table me) {
	integer numberOfRows = prefs.numberOfMarks = my rows.size;
	if (numberOfRows > 0) {
		integer col_vowel = Table_getColumnIndexFromColumnLabel (me, U"Vowel");
		integer col_f1 = Table_getColumnIndexFromColumnLabel (me, U"F1");
		integer col_f2 = Table_getColumnIndexFromColumnLabel (me, U"F2");
		integer col_size = Table_getColumnIndexFromColumnLabel (me, U"Size");
		autoMelderString mark;
		for (integer i = 1; i <= VowelEditor_MAXIMUM_MARKERS; i ++) {
			if (i <= numberOfRows) {
				MelderString_copy (& mark, Table_getStringValue_Assert (me, i, col_vowel), U"\t",
					Table_getStringValue_Assert (me, i, col_f1), U"\t",
					Table_getStringValue_Assert (me, i, col_f2), U"\t",
					Table_getStringValue_Assert (me, i, col_size));
				integer length = str32len (mark.string);
				Melder_require (length < Preferences_STRING_BUFFER_SIZE,
					U"Preference mark ", i, U" contains too many characters");
				str32cpy (prefs.mark [i - 1], mark.string);
			} else {
				str32cpy (prefs.mark [i - 1], U"x");
			}
		}
	}
}

static void Table_addColumn_size (Table me, int size) {
	integer col_size = Table_findColumnIndexFromColumnLabel (me, U"Size");
	if (col_size == 0) {
		Table_appendColumn (me, U"Size");
		for (integer i = 1; i <= my rows.size; i ++) {
			Table_setNumericValue (me, i, my numberOfColumns, size);
		}
	}
}

static void VowelEditor_setMarks (VowelEditor me, int marksDataset, int speakerType, int fontSize) {
	autoTable te;
	const char32 *Type [4] = { U"", U"m", U"w", U"c" };
	const char32 *Sex [3] = { U"", U"m", U"f"};
	if (marksDataset == 1) {   // American-English
		autoTable thee = Table_create_petersonBarney1952 ();
		te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, Type [speakerType]);
	} else if (marksDataset == 2) {   // Dutch
		if (speakerType == 1 || speakerType == 2) {   // male + female from Pols van Nierop
			autoTable thee = Table_create_polsVanNierop1973 ();
			te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, Sex [speakerType]);
		} else {
			autoTable thee = Table_create_weenink1983 ();
			te = Table_extractRowsWhereColumn_string (thee.get(), 1, kMelder_string::EQUAL_TO, Type [speakerType]);
		}
	} else if (marksDataset == 3) {   // none
		my marks.reset();
		return;
	} else {   // leave as is
		return;
	}
	autoTable newMarks = Table_collapseRows (te.get(), U"IPA", U"", U"F1 F2", U"", U"", U"");
	integer col_ipa = Table_findColumnIndexFromColumnLabel (newMarks.get(), U"IPA");
	Table_setColumnLabel (newMarks.get(), col_ipa, U"Vowel");
	Table_addColumn_size (newMarks.get(), fontSize);
	my marks = newMarks.move();
	copyVowelMarksInPreferences_volatile (my marks.get());
}

static void VowelEditor_createTableFromVowelMarksInPreferences (VowelEditor me)
{
	integer numberOfRows = VowelEditor_MAXIMUM_MARKERS;
	try {
		autoTable newMarks = Table_createWithColumnNames (0, U"Vowel F1 F2 Size");
		integer nmarksFound = 0;
		for (integer i = 1; i <= numberOfRows; i ++) {
			autoMelderTokens rowi (prefs.mark [i - 1]);
			integer numberOfTokens = rowi.count();
			if (numberOfTokens < 4) { // we are done
				break;
			}
			Table_appendRow (newMarks.get());
			for (integer j = 1; j <= 4; j ++) {
				Table_setStringValue (newMarks.get(), i, j, rowi [j]);
			}
			nmarksFound ++;
		}
		if (nmarksFound == 0) {
			my speakerType = prefs.speakerType = 1;
			my marksDataset = prefs.marksDataset = 1;
			VowelEditor_setMarks (me, my marksDataset, my speakerType, prefs.marksFontSize);
		} else {
			my marks = newMarks.move();
		}
	} catch (MelderError) {
		Melder_throw (U"Cannot create Table from preferences. Default marks set.");
		my speakerType = prefs.speakerType = 1;
		my marksDataset = prefs.marksDataset = 1;
		VowelEditor_setMarks (me, my marksDataset, my speakerType, prefs.marksFontSize);
	}
}

static void VowelEditor_getVowelMarksFromTableFile (VowelEditor me, MelderFile file)
{
	try {
		autoDaata data = Data_readFromFile (file);
		Melder_require (Thing_isa (data.get(), classTable),
			U"\"", MelderFile_name (file), U"\" is not a Table file");

		autoTable newMarks = data.static_cast_move <structTable> ();
		// check if columns Vowel F1 & F2 are present
		Table_getColumnIndexFromColumnLabel (newMarks.get(), U"Vowel");
		Table_getColumnIndexFromColumnLabel (newMarks.get(), U"F1");
		Table_getColumnIndexFromColumnLabel (newMarks.get(), U"F2");
		Table_addColumn_size (newMarks.get(), prefs.marksFontSize);
		my marks = newMarks.move();
		my marksDataset = prefs.marksDataset = 9999;
		copyVowelMarksInPreferences_volatile (my marks.get());
		// our marks preferences are dynamic, save each time
	} catch (MelderError) {
		Melder_throw (U"Vowel marks from Table not shown.");
	}
}

static void VowelEditor_setF3F4 (VowelEditor me, double f3, double b3, double f4, double b4) {
	double xmin = my f2min, xmax = my f2max, dx = my f2max - my f2min, x1 = dx / 2.0;
	double dy = my f1max - my f1min, y1 = dy / 2;

	if (! my f3) {
		my f3 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	if (! my b3) {
		my b3 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	if (! my f4) {
		my f4 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	if (! my b4) {
		my b4 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	my f3 -> z [1] [1] = f3;
	my b3 -> z [1] [1] = b3;
	my f4 -> z [1] [1] = f4;
	my b4 -> z [1] [1] = b4;
}

static void VowelEditor_drawBackground (VowelEditor me, Graphics g) {
	double x1, y1, x2, y2, f1, f2;

	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (g, 0.0);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 2.0);
	Graphics_rectangle (g, 0.0, 1.0, 0.0, 1.0);
	Graphics_setLineWidth (g, 1.0);
	Graphics_setGrey (g, 0.5);
	int fontSize = Graphics_inqFontSize (g);
	// draw the marks
	if (my marks) {
		integer col_vowel = Table_getColumnIndexFromColumnLabel (my marks.get(), U"Vowel");
		integer col_f1 = Table_getColumnIndexFromColumnLabel (my marks.get(), U"F1");
		integer col_f2 = Table_getColumnIndexFromColumnLabel (my marks.get(), U"F2");
		integer col_fs = Table_findColumnIndexFromColumnLabel (my marks.get(), U"Size");
		for (integer i = 1; i <= my marks -> rows.size; i ++) {
			const char32 *label = Table_getStringValue_Assert (my marks.get(), i, col_vowel);
			f1 = Table_getNumericValue_Assert (my marks.get(), i, col_f1);
			f2 = Table_getNumericValue_Assert (my marks.get(), i, col_f2);
			if (f1 >= my f1min && f1 <= my f1max && f2 >= my f2min && f2 <= my f2max) {
				VowelEditor_getXYFromF1F2 (me, f1, f2, & x1, & y1);
				int size = prefs.marksFontSize;
				if (col_fs != 0) {
					size = Melder_ifloor (Table_getNumericValue_Assert (my marks.get(), i, col_fs));
				}
				Graphics_setFontSize (g, size);
				Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
				Graphics_text (g, x1, y1, label);
			}
		}
	}
	Graphics_setFontSize (g, fontSize);
	// Draw the line F1=F2
	//
	VowelEditor_getXYFromF1F2 (me, my f2min, my f2min, & x1, & y1);
	if (y1 >= 0.0 && y1 <= 1.0) {
		VowelEditor_getXYFromF1F2 (me, my f1max, my f1max, & x2, & y2);
		if (x2 >= 0.0 && x2 <= 1.0) {
			autoPolygon p = Polygon_create (3);
			p -> x [1] = x1;    p -> y [1] = y1;
			p -> x [2] = x2;    p -> y [2] = y2;
			p -> x [3] = 1.0;   p -> y [3] = 0.0;
			Graphics_fillArea (g, p -> numberOfPoints, & p -> x [1], & p -> y [1]);
			// Polygon_paint does not work because of use of Graphics_setInner.
			Graphics_line (g, x1, y1, x2, y2);
		}
	}
	// Draw the grid
	if (my grid.df1 < (my f1max - my f1min)) { // Horizontal lines
		integer iline = (my f1min + my grid.df1) / my grid.df1;   // FIXME: if truncating down is deliberate, then do `floor`
		Graphics_setGrey (g, 0.5);
		Graphics_setLineType (g, Graphics_DOTTED);
		while ( (f1 = iline * my grid.df1) < my f1max) {
			if (f1 > my f1min) {
				VowelEditor_getXYFromF1F2 (me, f1, my f2min, & x1, & y1);
				VowelEditor_getXYFromF1F2 (me, f1, my f2max, & x2, & y2);
				Graphics_line (g, x1, y1, x2, y2);
			}
			iline ++;
		}
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setGrey (g, 0.0); // black
	}
	if (my grid.df2 < (my f2max - my f2min)) {
		integer iline = (my f2min + my grid.df2) / my grid.df2;   // FIXME: if truncating down is deliberate, then do `floor`
		Graphics_setGrey (g, 0.5);
		Graphics_setLineType (g, Graphics_DOTTED);
		while ( (f2 = iline * my grid.df2) < my f2max) { // vert line
			if (f2 > my f2min) {
				VowelEditor_getXYFromF1F2 (me, my f1min, f2, & x1, & y1);
				VowelEditor_getXYFromF1F2 (me, my f1max, f2, & x2, & y2);
				Graphics_line (g, x1, y1, x2, y2);
			}
			iline ++;
		}
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setGrey (g, 0.0); // black
	}
	Graphics_unsetInner (g);
	Graphics_setGrey (g, 0.0); // black
	Graphics_markLeft (g, 0.0, false, true, false, Melder_double (my f1max));
	Graphics_markLeft (g, 1.0, false, true, false, Melder_double (my f1min));
	Graphics_markTop (g, 0.0, false, true, false, Melder_double (my f2max));
	Graphics_markTop (g, 1.0, false, true, false, Melder_double (my f2min));

}

#if 0
typedef struct {
	integer some_check_value;
	integer istart;
	float *z;
} *paVowelData;
/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/

static int paCallback (const void* /*inputBuffer*/, void* outputBuffer, unsigned integer framesPerBuffer,
                       const PaStreamCallbackTimeInfo* /*timeInfo*/, PaStreamCallbackFlags /*statusFlags*/, void *void_me) {
	iam (paVowelData);
	float* out = (float*) outputBuffer;

	for (unsigned int i = 0; i < framesPerBuffer; i ++) {
		*out ++ = my z [my istart + i]; /* left */
		*out ++ = my z [my istart + i]; /* right */
	}
	my istart += framesPerBuffer;
	return 0;
}
#endif

#pragma mark - menu methods

static void menu_cb_help (VowelEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"VowelEditor");
}

static void menu_cb_prefs (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Preferences", nullptr);
		BOOLEAN (soundFollowsMouse, U"Sound follows mouse", true)
	EDITOR_OK
		SET_BOOLEAN (soundFollowsMouse, prefs.soundFollowsMouse)
	EDITOR_DO
		my soundFollowsMouse = prefs.soundFollowsMouse = soundFollowsMouse;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_ranges_f1f2 (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"F1 (vert) and F2 (hor) view ranges", nullptr);
		POSITIVE (f1min, U"left F1 range (Hz)", U"200.0")
		POSITIVE (f1max, U"right F1 range (Hz)", U"1000.0")
		POSITIVE (f2min, U"left F2 range (Hz)", U"500.0")
		POSITIVE (f2max, U"right F2 range (Hz)", U"2500.0")
	EDITOR_OK
		SET_REAL (f1min, prefs.f1min)
		SET_REAL (f1max, prefs.f1max)
		SET_REAL (f2min, prefs.f2min)
		SET_REAL (f2max, prefs.f2max)
	EDITOR_DO
		my f1min = prefs.f1min = f1min;
		my f1max = prefs.f1max = f1max;
		my f2min = prefs.f2min = f2min;
		my f2max = prefs.f2max = f2max;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_publishSound (VowelEditor me, EDITOR_ARGS_DIRECT) {
	autoSound publish = VowelEditor_createTarget (me);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_FormantGrid (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateVowel (me);
	autoFormantGrid publish = FormantTier_to_FormantGrid (my vowel -> ft.get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_KlattGrid (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateVowel (me);
	autoFormantGrid fg = FormantTier_to_FormantGrid (my vowel -> ft.get());
	autoKlattGrid publish = KlattGrid_create (fg -> xmin, fg -> xmax, fg -> formants.size, 0, 0, 0, 0, 0, 0);
	KlattGrid_addVoicingAmplitudePoint (publish.get(), fg -> xmin, 90.0);
	KlattGrid_replacePitchTier (publish.get(), my vowel -> pt.get());
	KlattGrid_replaceFormantGrid (publish.get(), KlattGrid_ORAL_FORMANTS, fg.get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extract_PitchTier (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_updateVowel (me);
	autoPitchTier publish = Data_copy (my vowel -> pt.get());
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
		if (garnish) {
			VowelEditor_drawBackground (me, my pictureGraphics);
		}
		FormantTier_drawF1F2Trajectory (my vowel -> ft.get(), my pictureGraphics, my f1min, my f1max, my f2min, my f2max, my markTraceEvery, my width);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_showOneVowelMark (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show one vowel mark", nullptr);
		POSITIVE (f1, U"F1 (Hz)", U"300.0")
		POSITIVE (f2, U"F2 (Hz)", U"600.0")
		WORD (mark, U"Mark", U"u")
	EDITOR_OK
	EDITOR_DO
		if (f1 >= my f1min && f1 <= my f1max && f2 >= my f2min && f2 <= my f2max) {
			integer irow = 1;
			if (! my marks) {
				my marks = Table_createWithColumnNames (1, U"IPA F1 F2 Colour");
			} else {
				Table_appendRow (my marks.get());
			}
			irow = my marks -> rows.size;
			Table_setStringValue (my marks.get(), irow, 1, mark);
			Table_setNumericValue (my marks.get(), irow, 2, f1);
			Table_setNumericValue (my marks.get(), irow, 3, f2);
			Graphics_updateWs (my graphics.get());
		}
	EDITOR_END
}

static void menu_cb_showVowelMarks (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show vowel marks", nullptr);
		MUTABLE_LABEL (note, U"")
		OPTIONMENU (dataSet, U"Data set", 1)
			OPTION (U"American English")
			OPTION (U"Dutch")
			OPTION (U"None")
		OPTIONMENU (speaker, U"Speaker", 1)
			OPTION (U"Man")
			OPTION (U"Woman")
			OPTION (U"Child")
		NATURAL (fontSize, U"Font size (points)", U"14")
	EDITOR_OK
		if (my marksDataset == 9999) SET_STRING (note, U"(Warning: the current vowel marks are not from one of these data sets.)")
		SET_OPTION (dataSet, my marksDataset)
		SET_OPTION (speaker, my speakerType)
		SET_INTEGER (fontSize, my marksFontSize)
	EDITOR_DO
		my marksDataset = prefs.marksDataset = dataSet;
		my speakerType = prefs.speakerType = speaker;
		my marksFontSize = prefs.marksFontSize = fontSize;
		VowelEditor_setMarks (me, my marksDataset, my speakerType, my marksFontSize);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_showVowelMarksFromTableFile (VowelEditor me, EDITOR_ARGS_FORM) {
	(void) narg;
	(void) interpreter;
	EDITOR_FORM_READ (U"VowelEditor: Show vowel marks from Table file", U"VowelEditor: Show vowel marks from Table file...");
	EDITOR_DO_READ
		VowelEditor_getVowelMarksFromTableFile (me, file);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_setF0 (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set F0", nullptr);
		POSITIVE (startF0, U"Start F0 (Hz)", U"150.0")
		REAL (slope, U"Slope (oct/s)", U"0.0")
	EDITOR_OK
	EDITOR_DO
		checkF0 (& my f0, & startF0);
		my f0.start = startF0;
		my f0.slopeOctPerSec = slope;
		GuiText_setString (my f0TextField, Melder_double (my f0.start));
		GuiText_setString (my f0SlopeTextField, Melder_double (my f0.slopeOctPerSec));
	EDITOR_END
}

static void menu_cb_setF3F4 (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set F3 & F4", nullptr);
		POSITIVE (f3, U"F3 (Hz)", U"2500.0")
		POSITIVE (b3, U"B3 (Hz)", U"250.0")
		POSITIVE (f4, U"F4 (Hz)", U"3500.0")
		POSITIVE (b4, U"B4 (Hz)", U"350.0")
	EDITOR_OK
	EDITOR_DO
		Melder_require (f3 < f4, U"F4 should be larger than F3.");
		VowelEditor_setF3F4 (me, f3, b3, f4, b4);
	EDITOR_END
}
static void menu_cb_reverseTrajectory (VowelEditor me, EDITOR_ARGS_DIRECT) {
	VowelEditor_Vowel_reverseFormantTier (me);
	Graphics_updateWs (my graphics.get());
}

static void VowelEditor_Vowel_addData (VowelEditor me, Vowel thee, double time, double f1, double f2, double f0) {
	autoFormantPoint fp = FormantPoint_create (time);
	double f3, b3, f4, b4;
	fp -> formant [0] = f1;
	fp -> bandwidth [0] = f1 / 10.0;
	fp -> formant [1] = f2;
	fp -> bandwidth [1] = f2 / 10.0;
	VowelEditor_getF3F4 (me, f1, f2, & f3, & b3, & f4, & b4);
	fp -> formant [2] = f3;
	fp -> bandwidth [2] = b3;
	fp -> formant [3] = f4;
	fp -> bandwidth [3] = b4;
	fp -> numberOfFormants = 4;

	thy ft -> points. addItem_move (fp.move());
	RealTier_addPoint (thy pt.get(), time, f0);
}

static void menu_cb_newTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"New Trajectory", nullptr);
		POSITIVE (startF1, U"Start F1 (Hz)", U"700.0")
		POSITIVE (startF2, U"Start F2 (Hz)", U"1200.0")
		POSITIVE (endF1, U"End F1 (Hz)", U"350.0")
		POSITIVE (endF2, U"End F2 (Hz)", U"800.0")
		POSITIVE (duration, U"Duration (s)", U"0.25")
	EDITOR_OK
	EDITOR_DO
		autoVowel newVowel = Vowel_create (duration);

		double startTime = 0.0;
		double startF0 = getF0 (& my f0, startTime);
		checkF1F2 (me, & startF1, & startF2);
		VowelEditor_Vowel_addData (me, newVowel.get(), startTime, startF1, startF2, startF0);

		double endTime = duration;
		double endF0 = getF0 (& my f0, endTime);
		checkF1F2 (me, & endF1, & endF2);
		VowelEditor_Vowel_addData (me, newVowel.get(), endTime, endF1, endF2, endF0);

		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (duration)));
		my vowel = newVowel.move();

		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_extendTrajectory (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extend Trajectory", nullptr);
		POSITIVE (toF1, U"To F1 (Hz)", U"500.0")
		POSITIVE (toF2, U"To F2 (Hz)", U"1500.0")
		POSITIVE (extraDuration, U"Extra duration (s)", U"0.1")
	EDITOR_OK
	EDITOR_DO
		Vowel thee = my vowel.get();
		double newDuration = thy xmax + extraDuration;
		double toF0 = getF0 (& my f0, newDuration);
		thy xmax = thy pt -> xmax = thy ft -> xmax = newDuration;
		checkF1F2 (me, & toF1, & toF2);
		VowelEditor_Vowel_addData (me, thee, newDuration, toF1, toF2, toF0);

		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (newDuration)));
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

static void menu_cb_modifyTrajectoryDuration (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Modify duration", nullptr);
		POSITIVE (newDuration, U"New duration (s)", U"0.5")
	EDITOR_OK
	EDITOR_DO
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

static void menu_cb_showTrajectoryTimeMarksEvery (VowelEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Show trajectory time marks every", nullptr);
		REAL (distance, U"Distance (s)", U"0.05")
	EDITOR_OK
		SET_REAL (distance, my markTraceEvery)
	EDITOR_DO
		my markTraceEvery = distance;
		if (my markTraceEvery < 0.0) {
			my markTraceEvery = 0.0;
		}
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

#pragma mark - button methods

static void gui_button_cb_play (VowelEditor me, GuiButtonEvent /* event */) {
	autoSound thee = VowelEditor_createTarget (me);
	Sound_play (thee.get(), nullptr, nullptr);
	Graphics_updateWs (my graphics.get());
}

static void gui_button_cb_publish (VowelEditor me, GuiButtonEvent /* event */) {
	autoSound publish = VowelEditor_createTarget (me);
	Editor_broadcastPublication (me, publish.move());
}

static void gui_button_cb_reverse (VowelEditor me, GuiButtonEvent /* event */) {
	VowelEditor_Vowel_reverseFormantTier (me);
	structGuiButtonEvent play_event { };
	gui_button_cb_play (me, & play_event);
}

static void gui_drawingarea_cb_expose (VowelEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	Melder_assert (me);
	Melder_assert (my vowel);
	double ts = my vowel -> xmin, te = my vowel -> xmax;
	FormantTier ft = my vowel -> ft.get();
	Melder_assert (ft);
	static MelderString statusInfo { };
	if (! my graphics) {
		return;   // could be the case in the very beginning
	}
	Graphics_clearWs (my graphics.get());

	appendF1F2F0 (& statusInfo, STATUSINFO_STARTINTR0, FormantTier_getValueAtTime (ft, 1, ts),
	              FormantTier_getValueAtTime (ft, 2, ts), getF0 (& my f0, ts), STATUSINFO_ENDING);
	GuiLabel_setText (my startInfo, statusInfo.string);
	MelderString_empty (& statusInfo);

	appendF1F2F0 (& statusInfo, STATUSINFO_ENDINTR0, FormantTier_getValueAtTime (ft, 1, te),
	              FormantTier_getValueAtTime (ft, 2, te), getF0 (& my f0, te), STATUSINFO_ENDING);
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
	Melder_assert (my vowel -> ft);
	FormantTier_drawF1F2Trajectory (my vowel -> ft.get(), my graphics.get(), my f1min, my f1max, my f2min, my f2max, my markTraceEvery, my width);
}

static void gui_drawingarea_cb_resize (VowelEditor me, GuiDrawingArea_ResizeEvent event) {
	Melder_assert (me);
	if (! my graphics) {
		return;   // could be the case in the very beginning
	}
	Graphics_setWsViewport (my graphics.get(), 0.0, event -> width, 0.0, event -> height);
	my width = event -> width;
	my height = event -> height;
	Graphics_setWsWindow (my graphics.get(), 0.0, my width, 0.0, my height);
	Graphics_setViewport (my graphics.get(), 0.0, my width, 0.0, my height);
	Graphics_updateWs (my graphics.get());

	/* Save the current shell size as the user's preference for a new VowelEditor. */

	prefs.shellWidth  = GuiShell_getShellWidth  (my windowForm);
	prefs.shellHeight = GuiShell_getShellHeight (my windowForm);
}

static void VowelEditor_Vowel_updateTiers (VowelEditor me, Vowel thee, double time, double x, double y) {
	if (time > thy xmax) {
		thy xmax = time;
		thy ft -> xmax = time;
		thy pt -> xmax = time;
	}
	double f0 = getF0 (& my f0, time), f1, f2;
	autoFormantPoint point = FormantPoint_create (time);

	VowelEditor_getF1F2FromXY (me, x, y, & f1, & f2);
	double f3, b3, f4, b4;
	VowelEditor_getF3F4 (me, f1, f2, & f3, & b3, & f4, & b4);

	point -> formant [0] = f1;
	point -> bandwidth [0] = f1 / 10.0;
	point -> formant [1] = f2;
	point -> bandwidth [1] = f2 / 10.0;
	point -> formant [2] = f3;
	point -> bandwidth [2] = b3;
	point -> formant [3] = f4;
	point -> bandwidth [3] = b4;
	point -> numberOfFormants = 4;
	thy ft -> points. addItem_move (point.move());
	RealTier_addPoint (thy pt.get(), time, f0);
}

// shift key always extends what already is.
// Special case : !soundFollowsMouse. The first click just defines the vowel's first f1f2-position,
static void gui_drawingarea_cb_click (VowelEditor me, GuiDrawingArea_ClickEvent event) {
	Vowel thee = nullptr; autoVowel athee;
	double x, y, xb, yb, tb, t, dt = 0.0;
	double t0 = Melder_clock ();
	integer iskipped = 0;
	struct structGuiButtonEvent gb_event { 0 };
	Graphics_setInner (my graphics.get());

	Graphics_getMouseLocation (my graphics.get(), & x, & y);
	checkXY (& x, & y);

	if (event -> shiftKeyPressed) {
		VowelEditor_updateExtendDuration (me);
		(my shiftKeyPressed) ++;
		thee = my vowel.get();
		dt = thy xmax + my extendDuration;
		t = 0.0 + dt;
		VowelEditor_Vowel_updateTiers (me, thee, t, x, y);
		GuiText_setString (my durationTextField, Melder_double (t));
		if (! my soundFollowsMouse) {
			goto end;
		}
	} else {
		t = 0.0;
		my shiftKeyPressed = 0;
		athee = Vowel_create (MINIMUM_SOUND_DURATION);
		thee = athee.get();
		VowelEditor_Vowel_updateTiers (me, thee, t, x, y);
		GuiText_setString (my durationTextField, Melder_double (t));
		if (! my soundFollowsMouse) {
			VowelEditor_Vowel_updateTiers (me, thee, MINIMUM_SOUND_DURATION, x, y);
			goto end;
		}
	}

	Graphics_xorOn (my graphics.get(), Graphics_BLUE);
	while (Graphics_mouseStillDown (my graphics.get())) {
		xb = x, yb = y, tb = t;
		t = Melder_clock () - t0 + dt; // Get relative time in seconds from the clock
		Graphics_getMouseLocation (my graphics.get(), & x, & y);
		checkXY (& x, & y);
		// If the new point equals the previous one: no tier update
		if (xb == x && yb == y) {
			iskipped ++;
			continue;
		}
		// Add previous point only if at least one previous event was skipped...
		if (iskipped > 0) {
			VowelEditor_Vowel_updateTiers (me, thee, tb, xb, yb);
		}
		iskipped = 0;
		Graphics_line (my graphics.get(), xb, yb, x, y);

		VowelEditor_Vowel_updateTiers (me, thee, t, x, y);
		GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (t)));
	}
	t = Melder_clock () - t0;
	// To prevent ultra short clicks we set a minimum of 0.01 s duration
	if (t < MINIMUM_SOUND_DURATION) {
		t = MINIMUM_SOUND_DURATION;
	}
	t += dt;
	GuiText_setString (my durationTextField, Melder_double (MICROSECPRECISION (t)));
	VowelEditor_Vowel_updateTiers (me, thee, t, x, y);

	Graphics_xorOff (my graphics.get());

end:
	Graphics_unsetInner (my graphics.get());

	if (! my shiftKeyPressed) {
		my vowel = athee.move();
	}
	Melder_assert (! athee);
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
	Editor_addCommand (this, U"Edit", U"Set F0...", 0, menu_cb_setF0);
	Editor_addCommand (this, U"Edit", U"Set F3 & F4...", 0, menu_cb_setF3F4);
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
	Editor_addCommand (this, U"View", U"--show trajectory time marks--", 0, nullptr);
	Editor_addCommand (this, U"View", U"Show trajectory time marks every...", 0, menu_cb_showTrajectoryTimeMarksEvery);
}

void structVowelEditor :: v_createHelpMenuItems (EditorMenu menu) {
	VowelEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"VowelEditor help", '?', menu_cb_help);
}

void structVowelEditor :: v_createChildren ()
{
	const int button_width = 90, text_width = 95, status_info_width = 290;
	int top, bottom, bottom_widgets_top, bottom_widgets_bottom, bottom_widgets_halfway;

	// Three buttons on a row: Play, Reverse, Publish
	int left = 10, right = left + button_width;
	bottom_widgets_top = top = -MARGIN_BOTTOM + 10; bottom_widgets_bottom = bottom = -STATUS_INFO;
	playButton = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Play", gui_button_cb_play, this, 0);
	left = right + 10; right = left + button_width;
	reverseButton = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Reverse", gui_button_cb_reverse, this, 0);
	left = right + 10; right = left + button_width;
	publishButton = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Publish", gui_button_cb_publish, this, 0);
	// Four Text widgets with the label on top: Duration, Extend, F0, Slope
	// Make the F0 slope button 10 wider to accomodate the text
	// We wil not use a callback from a Text widget. It will get called multiple times during the editing
	// of the text. Better to have all editing done and then query the widget for its value!
	left = right + 10; right = left + text_width; bottom_widgets_halfway = bottom = (top + bottom) / 2; top = bottom_widgets_top;
	GuiLabel_createShown (our windowForm, left, right, top , bottom, U"Duration (s):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	durationTextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	left = right + 10; right = left + text_width; top = bottom_widgets_top; bottom = bottom_widgets_halfway;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Extend (s):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	extendTextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	left = right + 10; right = left + text_width; top = bottom_widgets_top; bottom = bottom_widgets_halfway;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Start F0 (Hz):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	f0TextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	left = right + 10; right = left + text_width + 10; top = bottom_widgets_top; bottom = bottom_widgets_halfway;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"F0 slope (oct/s):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	f0SlopeTextField = GuiText_createShown (our windowForm, left, right, top, bottom, 0);

	// The status startInfo and endInfo widget at the bottom:

	bottom = - (STATUS_INFO - Gui_LABEL_HEIGHT) / 2; top = bottom - Gui_LABEL_HEIGHT; left = MARGIN_LEFT; right = left + status_info_width;
	startInfo = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);

	left = right; right = left + status_info_width;
	endInfo = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);

	/***** Create drawing area. *****/
	// Approximately square because for our defaults: f1min=200, f1max=1000 and f2min = 500, f2mx = 2500,
	// log distances are equal (log (1000/200) == log (2500/500) ).
	//drawingArea = GuiDrawingArea_createShown (our windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
	//	gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
	drawingArea = GuiDrawingArea_createShown (our windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, nullptr, gui_drawingarea_cb_resize, this, 0);
	width  = GuiControl_getWidth  (drawingArea);
	height = GuiControl_getHeight (drawingArea);
}

autoVowelEditor VowelEditor_create (const char32 *title, Daata data) {
	try {
		trace (U"enter");
		autoVowelEditor me = Thing_new (VowelEditor);
		Melder_assert (me.get());
		Editor_init (me.get(), 0, 0, prefs.shellWidth, prefs.shellHeight, title, data);
#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
#endif
		my graphics = Graphics_create_xmdrawingarea (my drawingArea);
		Melder_assert (my graphics);
		Graphics_setFontSize (my graphics.get(), 12);

		my f1min = prefs.f1min;
		my f1max = prefs.f1max;
		my f2min = prefs.f2min;
		my f2max = prefs.f2max;
		my speakerType = prefs.speakerType;
		my marksDataset = prefs.marksDataset;
		my marksFontSize = prefs.marksFontSize;
		my soundFollowsMouse = prefs.soundFollowsMouse;
		if (my marksDataset > 3) { // TODO variable??
			VowelEditor_createTableFromVowelMarksInPreferences (me.get());
		} else {
			VowelEditor_setMarks (me.get(), my marksDataset, my speakerType, 14);
		}
		VowelEditor_setF3F4 (me.get(), prefs.f3, prefs.b3, prefs.f4, prefs.b4);
		my maximumDuration = BUFFER_SIZE_SEC;
		my extendDuration = prefs.extendDuration;
		if (my data) {
			my vowel = Data_copy (static_cast<Vowel> (data));
		} else {
			my vowel = Vowel_create_twoFormantSchwa (0.2);
		}
		my markTraceEvery = prefs.markTraceEvery;
		my f0 = f0default;
		GuiText_setString (my f0TextField, Melder_double (my f0.start));
		GuiText_setString (my f0SlopeTextField, Melder_double (my f0.slopeOctPerSec));
		GuiText_setString (my durationTextField, U"0.2"); // Source has been created
		GuiText_setString (my extendTextField, Melder_double (my extendDuration));
		my grid = griddefault;
{
	// This exdents because it's a hack:
	struct structGuiDrawingArea_ResizeEvent event { my drawingArea, 0 };
	event. width  = GuiControl_getWidth  (my drawingArea);
	event. height = GuiControl_getHeight (my drawingArea);
	gui_drawingarea_cb_resize (me.get(), & event);
}
		//struct structGuiDrawingArea_ResizeEvent event { 0 };
		//event.widget = my drawingArea;
		//gui_drawingarea_cb_resize (me, & event);
		updateWidgets (me.get());
		trace (U"exit");
		return me;
	} catch (MelderError) {
		Melder_throw (U"VowelEditor not created.");
	}
}

/* End of file VowelEditor.cpp */
