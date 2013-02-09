/* VowelEditor.c
 *
 * Copyright (C) 2008-2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
#include "praat.h"
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
#define STATUS_INFO (1.5*Gui_LABEL_HEIGHT)
#define MARGIN_RIGHT 10
#define MARGIN_LEFT 50
#define MARGIN_TOP 50
#define MARGIN_BOTTOM (60+STATUS_INFO)
#define BUFFER_SIZE_SEC 4
#define SAMPLING_FREQUENCY 44100

#define STATUSINFO_STARTINTR0 L"Start (F1,F2,F0) = ("
#define STATUSINFO_ENDINTR0 L"End (F1,F2,F0) = ("
#define STATUSINFO_ENDING L")"
#define MICROSECPRECISION(x) (round((x)*1000000)/1000000)

// To prevent the generation of inaudible short Sounds we set a minimum duration
#define MINIMUM_SOUND_DURATION 0.01

// maximum number of marks
#define VowelEditor_MAXIMUM_MARKERS 30

// prototypes: button callbacks
static void gui_button_cb_publish (I, GuiButtonEvent event);
static void gui_button_cb_play (I, GuiButtonEvent event);
static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event);
static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event);
static void gui_button_cb_reverse (I, GuiButtonEvent event);
static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event);
//  prototypes: helpers
static double getRealFromTextWidget (GuiText me);
static double getCoordinate (double fmin, double fmax, double f);
static double getF0 (structVowelEditor_F0 *f0p, double time);
static void checkF1F2 (VowelEditor me, double *f1, double *f2);
static void checkF0 (structVowelEditor_F0 *f0p, double *f0);
static void	checkXY (double *x, double *y);
static void Sound_fadeIn (Sound me, double duration, int fromFirstNonZeroSample);
static void Sound_fadeOut (Sound me, double duration);
static void PitchTier_newDuration (PitchTier me, structVowelEditor_F0 *f0p, double newDuration);
static void FormantTier_newDuration (FormantTier me, double newDuration);
static void FormantTier_drawF1F2Trajectory (FormantTier me, Graphics g, double f1min, double f1max, double f2min, double f2max, double markTraceEvery, double width);
static FormantGrid FormantTier_to_FormantGrid (FormantTier me);
static PitchTier VowelEditor_to_PitchTier (VowelEditor me, double duration);
static void VowelEditor_updateF0Info (VowelEditor me);
static void VowelEditor_updateExtendDuration (VowelEditor me);
static double VowelEditor_updateDurationInfo (VowelEditor me);
static void VowelEditor_Vowel_updateTiers (VowelEditor me, Vowel thee, double time, double x, double y);
static void VowelEditor_Vowel_addData (VowelEditor me, Vowel thee, double time, double f1, double f2, double f0);
static void VowelEditor_getXYFromF1F2 (VowelEditor me, double f1, double f2, double *x, double *y);
static void VowelEditor_getF1F2FromXY (VowelEditor me, double x, double y, double *f1, double *f2);
static void VowelEditor_updateVowel (VowelEditor me);
static Sound VowelEditor_createTarget (VowelEditor me);
static void VowelEditor_Vowel_reverseFormantTier (VowelEditor me);
static void VowelEditor_shiftF1F2 (VowelEditor me, double f1_st, double f2_st);
static void VowelEditor_setSource (VowelEditor me);
static void VowelEditor_setMarks (VowelEditor me, int marksDataset, int speakerType, int fontSize);
static void VowelEditor_setF3F4 (VowelEditor me, double f3, double b3, double f4, double b4);
static void VowelEditor_getF3F4 (VowelEditor me, double f1, double f2, double *f3, double *b3, double *f4, double *b4);
static void VowelEditor_getVowelMarksFromTableFile (VowelEditor me, MelderFile file);
static void VowelEditor_createTableFromVowelMarksInPreferences (VowelEditor me);
static void Table_addColumn_size (Table me, int size);
static double Matrix_getValue (Matrix me, double x, double y);
static void VowelEditor_drawBackground (VowelEditor me, Graphics g);
static void createPersistentVowelMarks ();
static void copyVowelMarksInPreferences_volatile (Table me);
static Vowel Vowel_create (double duration);
static Vowel Vowel_create_twoFormantSchwa (double duration);
static void Vowel_newDuration (Vowel me, structVowelEditor_F0 *f0p, double newDuration);
static Sound Vowel_to_Sound_pulses (Vowel me, double samplingFrequency, double adaptFactor, double adaptTime, long interpolationDepth);
// forward declarations end

static struct structVowelEditor_F0 f0default = { 140.0, 0.0, 40.0, 2000.0, SAMPLING_FREQUENCY, 1, 0.0, 2000 };
static struct structVowelEditor_F1F2Grid griddefault = { 200, 500, 0, 1, 0, 1, 0.5 };

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

// Preferences structs

struct markInfo {
	double f1, f2;
	int size;
	wchar_t vowel [Preferences_STRING_BUFFER_SIZE];
};

static struct {
	int shellWidth, shellHeight;
	int soundFollowsMouse;
	double f1min, f1max, f2min, f2max;
	double f3, b3, f4, b4;
	double markTraceEvery, extendDuration;
	int frequencyScale;
	int axisOrientation;
	int speakerType, marksDataset, numberOfMarks, marksFontSize;
	wchar_t mark[VowelEditor_MAXIMUM_MARKERS][Preferences_STRING_BUFFER_SIZE];
} prefs;


Thing_implement (Vowel, Function, 0);

static Vowel Vowel_create (double duration) {
	try {
		autoVowel me = Thing_new (Vowel);
		Function_init (me.peek(), 0, duration);
		my ft = FormantTier_create (0, duration);
		my pt = PitchTier_create (0, duration);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Vowel not created.");
	}
}

static Vowel Vowel_create_twoFormantSchwa (double duration) {
	try {
		autoVowel me = Vowel_create (duration);
		autoFormantPoint fp =  FormantPoint_create (0);
		fp -> formant [0] = 500;
		fp -> bandwidth[0] = 50;
		fp -> formant [1] = 1500;
		fp -> bandwidth[1] = 150;
		fp -> numberOfFormants = 2;
		Collection_addItem (my ft -> points, fp.transfer());
		RealTier_addPoint (my pt, 0, 140);

		fp.reset (FormantPoint_create (duration));
		fp -> formant [0] = 500;
		fp -> bandwidth[0] = 50;
		fp -> formant [1] = 1500;
		fp -> bandwidth[1] = 150;
		fp -> numberOfFormants = 2;
		Collection_addItem (my ft -> points, fp.transfer());
		RealTier_addPoint (my pt, duration, 140);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Schwa Vowel not created");
	}
}

static Sound Vowel_to_Sound_pulses (Vowel me, double samplingFrequency, double adaptFactor, double adaptTime, long interpolationDepth) {
	try {
		autoPointProcess pp = PitchTier_to_PointProcess (my pt);
		autoSound thee = PointProcess_to_Sound_pulseTrain (pp.peek(), samplingFrequency, adaptFactor, adaptTime, interpolationDepth);
		Sound_FormantTier_filter_inline (thee.peek(), my ft);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Sound with pulses not created.");
	}
}

static FormantGrid FormantTier_to_FormantGrid (FormantTier me) {
	try {
		long numberOfFormants = FormantTier_getMaxNumFormants (me);
		autoFormantGrid thee = FormantGrid_createEmpty (my xmin, my xmax, numberOfFormants);
		for (long ipoint = 1; ipoint <= my points -> size; ipoint++) {
			FormantPoint fp = (FormantPoint) my points -> item[ipoint];
			double t = fp -> time;
			for (long iformant = 1; iformant <= fp -> numberOfFormants; iformant++) {
				FormantGrid_addFormantPoint (thee.peek(), iformant, t, fp -> formant[iformant - 1]);
				FormantGrid_addBandwidthPoint (thee.peek(), iformant, t, fp -> bandwidth[iformant - 1]);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no FormantGrod created.");
	}
}

static void VowelEditor_getXYFromF1F2 (VowelEditor me, double f1, double f2, double *x, double *y) {
	*x = log (f2 / my f2max) / log (my f2min / my f2max);
	*y = log (f1 / my f1max) / log (my f1min / my f1max);
}

//Graphics_DCtoWC ????
static void VowelEditor_getF1F2FromXY (VowelEditor me, double x, double y, double *f1, double *f2) {
	*f2 = my f2min * pow (my f2max / my f2min, 1 - x);
	*f1 = my f1min * pow (my f1max / my f1min, 1 - y);
}

#define REPRESENTNUMBER(x,i) (((x) == NUMundefined) ? L" undef" : ((swprintf(buffer[i], 7, L"%6.1f",x)), buffer[i]))
static void appendF1F2F0 (MelderString *statusInfo, const wchar_t *intro, double f1, double f2, double f0, const wchar_t *ending) {
	const wchar_t *komma = L", ";
	wchar_t buffer[4][10];
	MelderString_append (statusInfo, intro, REPRESENTNUMBER (f1, 1), komma, REPRESENTNUMBER (f2, 2), komma, REPRESENTNUMBER (f0, 3), ending);
}

static double getRealFromTextWidget (GuiText me) {
	double value = NUMundefined;
	wchar_t *dirty = my f_getString ();
	try {
		Interpreter_numericExpression (NULL, dirty, & value);
	} catch (MelderError) {
		Melder_clearError (); value = NUMundefined;
	}
	Melder_free (dirty);
	return value;
}

static void VowelEditor_updateF0Info (VowelEditor me) {
	double f0 = getRealFromTextWidget (my f0TextField);
	checkF0 (&my f0, &f0);
	my f0TextField -> f_setString (Melder_double (f0));
	my f0.start = f0;
	double slopeOctPerSec = getRealFromTextWidget (my f0SlopeTextField);
	if (slopeOctPerSec == NUMundefined) {
		slopeOctPerSec = f0default.slopeOctPerSec;
	}
	my f0.slopeOctPerSec = slopeOctPerSec;
	my f0SlopeTextField -> f_setString (Melder_double (my f0.slopeOctPerSec));
}

static void VowelEditor_updateExtendDuration (VowelEditor me) {
	double extend = getRealFromTextWidget (my extendTextField);
	if (extend == NUMundefined || extend <= MINIMUM_SOUND_DURATION || extend > my maximumDuration) {
		extend = MINIMUM_SOUND_DURATION;
	}
	my extendTextField -> f_setString (Melder_double (extend));
	my extendDuration = prefs.extendDuration = extend;
}

static double VowelEditor_updateDurationInfo (VowelEditor me) {
	double duration = getRealFromTextWidget (my durationTextField);
	if (duration == NUMundefined || duration < MINIMUM_SOUND_DURATION) {
		duration = MINIMUM_SOUND_DURATION;
	}
	my durationTextField -> f_setString (Melder_double (MICROSECPRECISION (duration)));
	return duration;
}

static void Sound_fadeIn (Sound me, double duration, int fromFirstNonZeroSample) {
	long istart = 1, numberOfSamples = duration / my dx;

	if (numberOfSamples < 2) {
		return;
	}
	if (fromFirstNonZeroSample != 0) {
		// If the first part of the sound is very low level we put sample values to zero and
		// start windowing from the position where the amplitude is above the minimum level.
		// WARNING: this part is special for the artificial vowels because
		// 1. They have no offset
		// 2. They are already scaled to a maximum amplitude of 0.99
		// 3. For 16 bit precision
		double zmin = 0.5 / pow (2, 16);
		while (fabs (my z[1][istart]) < zmin && istart < my nx) {
			my z[1][istart] = 0; // To make sure
			istart++;
		}
	}
	if (numberOfSamples > my nx - istart + 1) {
		numberOfSamples = my nx - istart + 1;
	}

	for (long i = 1; i <= numberOfSamples; i++) {
		double phase = NUMpi * (i - 1) / (numberOfSamples - 1);

		my z[1][istart + i - 1] *= 0.5 * (1 - cos (phase));
	}
}

static void Sound_fadeOut (Sound me, double duration) {
	long istart, numberOfSamples = duration / my dx;

	if (numberOfSamples < 2) {
		return;
	}
	if (numberOfSamples > my nx) {
		numberOfSamples = my nx;
	}
	istart = my nx - numberOfSamples;
	// only one channel
	for (long i = 1; i <= numberOfSamples; i++) {
		double phase = NUMpi * (i - 1) / (numberOfSamples - 1);

		my z[1][istart + i] *= 0.5 * (1 + cos (phase));
	}
}

static double getF0 (structVowelEditor_F0 *f0p, double time) {
	double f0 = f0p -> start * pow (2, f0p -> slopeOctPerSec * time);
	if (f0 < f0p -> minimum) {
		f0 = f0p -> minimum;
	} else if (f0 > f0p -> maximum) {
		f0 = f0p -> maximum;
	}
	return f0;
}

static void VowelEditor_Vowel_reverseFormantTier (VowelEditor me) {
	FormantTier ft = (FormantTier) my vowel -> ft;
	double duration = ft -> xmax;
	long np = ft -> points -> size, np_2 = np / 2;

	for (long i = 1; i <= np_2; i++) {
		FormantPoint fpt = (FormantPoint) ft -> points -> item[i];
		ft -> points -> item[i] =  ft -> points -> item[np - i + 1];
		ft -> points -> item[np - i + 1] = fpt;
		fpt = (FormantPoint) ft -> points -> item[i];
		fpt -> time = duration - fpt -> time;
		fpt = (FormantPoint) ft -> points -> item[np - i + 1];
		fpt -> time = duration - fpt -> time;
	}
	if (np % 2 == 1) {
		FormantPoint fpt = (FormantPoint) ft -> points -> item[np_2 + 1];
		fpt -> time = duration - fpt -> time;
	}
}

static void VowelEditor_shiftF1F2 (VowelEditor me, double f1_st, double f2_st) {
	FormantTier ft = my vowel -> ft;
	for (long i = 1; i <= ft -> points -> size; i++) {
		FormantPoint fp = (FormantPoint) ft -> points -> item[i];
		double f1 = fp -> formant[0], f2 = fp -> formant[1];

		f1 *= pow (2, f1_st / 12);
		if (f1 < my f1min) {
			f1 = my f1min;
		}
		if (f1 > my f1max) {
			f1 = my f1max;
		}
		fp -> formant[0] = f1;
		fp -> bandwidth[0] = f1 / 10;

		f2 *= pow (2, f2_st / 12);
		if (f2 < my f2min) {
			f2 = my f2min;
		}
		if (f2 > my f2max) {
			f2 = my f2max;
		}
		fp -> formant[1] = f2;
		fp -> bandwidth[1] = f2 / 10;
		double f3, b3, f4, b4;
		VowelEditor_getF3F4 (me, f1, f2, &f3, &b3, &f4, &b4);
		fp -> formant[2] = f3;
		fp -> bandwidth[2] = b3;
		fp -> formant[3] = f4;
		fp -> bandwidth[3] = b4;
	}
}

static void Vowel_newDuration (Vowel me, structVowelEditor_F0 *f0p, double newDuration) {
	if (newDuration != my xmax) {
		double multiplier = newDuration / my xmax;
		FormantTier_newDuration (my ft, newDuration);
		my xmax *= multiplier;
	}
	PitchTier_newDuration (my pt, f0p, newDuration); // always update

}

static void FormantTier_newDuration (FormantTier me, double newDuration) {
	if (newDuration != my xmax) {
		double multiplier = newDuration / my xmax;

		for (long i = 1; i <= my points -> size; i++) {
			FormantPoint fp = (FormantPoint) my points -> item[i];
			fp -> time *= multiplier;
		}
		my xmax *= multiplier;
	}
}

static void PitchTier_newDuration (PitchTier me, structVowelEditor_F0 *f0p, double newDuration) {
	// Always update; GuiObject text might have changed
	double multiplier = newDuration / my xmax;
	for (long i = 1; i <= my points -> size; i++) {
		RealPoint pp = (RealPoint) my points -> item[i];
		pp -> number *= multiplier;
		pp -> value = getF0 (f0p, pp -> number);
	}
	my xmax *= multiplier;
}

static void VowelEditor_updateVowel (VowelEditor me) {
	double newDuration = VowelEditor_updateDurationInfo (me); // Get new duration from TextWidget
	VowelEditor_updateF0Info (me); // Get new pitch and slope values from TextWidgets
	Vowel_newDuration (my vowel, & my f0, newDuration);
}

static double getCoordinate (double fmin, double fmax, double f) {
	return log (f / fmax) / log (fmin / fmax);
}

#define GETX(x) (getCoordinate (f2min, f2max, x))
#define GETY(y) (getCoordinate (f1min, f1max, y))
// Our FormantTiers always have a FormantPoint at t=xmin and t=xmax;
static void FormantTier_drawF1F2Trajectory (FormantTier me, Graphics g, double f1min, double f1max, double f2min, double f2max, double markTraceEvery, double width) {
	int it, imark = 1, glt = Graphics_inqLineType (g);
	double glw = Graphics_inqLineWidth (g), x1, x1p, y1, y1p, t1;
	Graphics_Colour colour = Graphics_inqColour (g);
	long nfp = my points -> size;
	FormantPoint fp = (FormantPoint) my points -> item[1];
	FormantPoint fpn = (FormantPoint) my points -> item[nfp];
	double tm, markLength = 0.01;

	Graphics_setInner (g);
	Graphics_setWindow (g, 0, 1, 0, 1);
	Graphics_setLineType (g, Graphics_DRAWN);
	// Too short too hear ?
	if ( (my xmax - my xmin) < 0.005) {
		Graphics_setColour (g, Graphics_RED);
	}
	x1p = x1 = GETX (fp->formant[1]); y1p = y1 = GETY (fp->formant[0]); t1 = fp->time;
	for (it = 2; it <= nfp; it++) {
		fp = (FormantPoint) my points -> item[it];
		double x2 = GETX (fp->formant[1]), y2 = GETY (fp->formant[0]), t2 = fp->time;
		Graphics_setLineWidth (g, 3);
		if (x1 == x2 && y1 == y2) {
			x1 = x1p; y1 = y1p;
		} else {
			Graphics_line (g, x1, y1, x2, y2);
		}
		while (markTraceEvery > 0 && (tm = markTraceEvery * imark) < t2) {
			// line orthogonal to y = (y1/x1)*x is y = -(x1/y1)*x
			double fraction = (tm - t1) / (t2 - t1);
			double dx = x2 - x1, dy = y2 - y1;
			double xm = x1 + fraction * dx, ym = y1 + fraction * dy;
			double xl1 = dy * markLength / sqrt (dx * dx + dy * dy), xl2 = - xl1;
			double yl1 = dx * markLength / sqrt (dx * dx + dy * dy), yl2 = - yl1;

			if (dx * dy > 0) {
				xl1 = -fabs (xl1); yl1 = fabs (yl1);
				xl2 = fabs (xl1); yl2 = -fabs (yl1);
			} else if (dx * dy < 0) {
				xl1 = -fabs (xl1); yl1 = -fabs (yl1);
				xl2 = fabs (xl1); yl2 = fabs (yl1);
			}
			Graphics_setLineWidth (g, 1);
			Graphics_line (g, xm + xl1, ym + yl1, xm + xl2, ym + yl2);

			imark++;
		}
		x1p = x1; y1p = y1;
		x1 = x2; y1 = y2; t1 = t2;
	}
	// Arrow at end
	{
		double gas = Graphics_inqArrowSize (g), arrowSize = 1;
		double size = 10.0 * arrowSize * Graphics_getResolution (g) / 75.0 / width, size2 = size * size;
		Graphics_setArrowSize (g, arrowSize);
		it = 1;
		while (it <= (nfp - 1)) {
			fp = (FormantPoint) my points -> item[nfp - it];
			double dx = GETX (fpn->formant[1]) - GETX (fp->formant[1]);
			double dy = GETY (fpn->formant[0]) - GETY (fp->formant[0]);
			double d2 = dx * dx + dy * dy;
			if (d2 > size2) {
				break;
			}
			it++;
		}
		Graphics_arrow (g, GETX (fp->formant[1]), GETY (fp->formant[0]), GETX (fpn->formant[1]), GETY (fpn->formant[0]));
		Graphics_setArrowSize (g, gas);
	}
	Graphics_unsetInner (g);
	Graphics_setColour (g, colour);
	Graphics_setLineType (g, glt);
	Graphics_setLineWidth (g, glw);
}
#undef GETX
#undef GETY

static PitchTier VowelEditor_to_PitchTier (VowelEditor me, double duration) {
	try {
		double t_end = duration;
		double f0_end = my f0.start * pow (2, my f0.slopeOctPerSec * t_end);
		autoPitchTier thee = PitchTier_create (0, t_end);

		RealTier_addPoint (thee.peek(), 0, my f0.start);
		if (my f0.slopeOctPerSec < 0) {
			if (f0_end < my f0.minimum) {
				t_end = log2 (my f0.minimum / my f0.start) / my f0.slopeOctPerSec;
				f0_end = my f0.minimum;
			}
		} else if (my f0.slopeOctPerSec > 0) {
			if (f0_end > my f0.maximum) {
				t_end = log2 (my f0.maximum / my f0.start) / my f0.slopeOctPerSec;
				f0_end = my f0.maximum;
			}
		}
		RealTier_addPoint (thee.peek(), t_end, f0_end);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PitchTier created.");
	}
}

void VowelEditor_prefs () {
	Preferences_addInt (L"VowelEditor.shellWidth", &prefs.shellWidth, 500);
	Preferences_addInt (L"VowelEditor.shellHeight", &prefs.shellHeight, 500);
	Preferences_addInt (L"VowelEditor.soundFollowsMouse", &prefs.soundFollowsMouse, 1);
	Preferences_addDouble (L"VowelEditor.f1min", &prefs.f1min, 200);
	Preferences_addDouble (L"VowelEditor.f1max", &prefs.f1max, 1200);
	Preferences_addDouble (L"VowelEditor.f2min", &prefs.f2min, 500);
	Preferences_addDouble (L"VowelEditor.f2max", &prefs.f2max, 3500);
	Preferences_addDouble (L"VowelEditor.f3", &prefs.f3, 2500);
	Preferences_addDouble (L"VowelEditor.b3", &prefs.b3, 250);
	Preferences_addDouble (L"VowelEditor.f4", &prefs.f4, 3500);
	Preferences_addDouble (L"VowelEditor.b4", &prefs.b4, 350);
	Preferences_addDouble (L"VowelEditor.markTraceEvery", &prefs.markTraceEvery, 0.05);
	Preferences_addDouble (L"VowelEditor.extendDuration", &prefs.extendDuration, 0.05);
	Preferences_addInt (L"VowelEditor.frequencyScale", &prefs.frequencyScale, 0);
	Preferences_addInt (L"VowelEditor.axisOrientation", &prefs.axisOrientation, 0);
	Preferences_addInt (L"VowelEditor.speakerType", &prefs.speakerType, 1);
	Preferences_addInt (L"VowelEditor.marksDataset", &prefs.marksDataset, 2);
	Preferences_addInt (L"VowelEditor.marksFontsize", &prefs.marksFontSize, 14);
	createPersistentVowelMarks ();
}

static void copyVowelMarksInPreferences_volatile (Table me) {
	long numberOfRows = prefs.numberOfMarks = my rows -> size;
	if (numberOfRows > 0) {
		long col_vowel = Table_getColumnIndexFromColumnLabel (me, L"Vowel");
		long col_f1 = Table_getColumnIndexFromColumnLabel (me, L"F1");
		long col_f2 = Table_getColumnIndexFromColumnLabel (me, L"F2");
		long col_size = Table_getColumnIndexFromColumnLabel (me, L"Size");
		autoMelderString mark, markID;
		for (long i = 1; i <= VowelEditor_MAXIMUM_MARKERS; i++) {
			if (i <= numberOfRows) {
				MelderString_append (&mark, Table_getStringValue_Assert (me, i, col_vowel), L"\t",
					Table_getStringValue_Assert (me, i, col_f1), L"\t",
					Table_getStringValue_Assert (me, i, col_f2), L"\t",
					Table_getStringValue_Assert (me, i, col_size));
				long length = wcslen (mark.string);
				if (length > Preferences_STRING_BUFFER_SIZE) Melder_throw ("Preference mark", i, "too many characters");
				wcscpy (prefs.mark[i-1], mark.string);
				MelderString_empty (&mark);
			} else {
				wcscpy (prefs.mark[i-1], L"x");
			}
		}
		// The following code removes the superfluous mark preferences,
		// *if* we had access to the thePreferences set (but we don't)
		//	autoMelderString markID;
		// for (long i = VowelEditor_MAXIMUM_MARKERS; i > numberOfRows; i--) {
		//		MelderString_append (&markID, L"VowelEditor.mark", (i < 10 ? L"0" : L""), Melder_integer (i));
		//		long index = SortedSetOfString_lookUp (thePreferences, markID.string);
		//		Collection_removeItem (thePreferences, index);
		//		MelderString_empty (&markID);
		// }
	}
}

static void createPersistentVowelMarks ()
{
	autoMelderString markID;
	// Deadlock:
	// This function is executed before the preferences are read from file and before we know how
	// many vowel marks the user wants.
	// However, to succesfully read the preferences from file we need the names of the preferences otherwise
	// they will not be assigned a value.
	// We therefore create fake names first and later fill them with data.
	long numberOfMarks = VowelEditor_MAXIMUM_MARKERS;
	Preferences_addInt (L"VowelEditor.numberOfMarks", &prefs.numberOfMarks, numberOfMarks);
	for (long i = 1; i <= numberOfMarks; i++) {
		MelderString_append (&markID, L"VowelEditor.mark", (i < 10 ? L"0" : L""), Melder_integer (i));
		Preferences_addString (markID.string, & prefs.mark[i-1][0], L"x");
		MelderString_empty (&markID);
	}
}

void VowelEditor_createTableFromVowelMarksInPreferences (VowelEditor me)
{
	long numberOfRows = VowelEditor_MAXIMUM_MARKERS;
	try {
		autoTable marks = Table_createWithColumnNames (0, L"Vowel F1 F2 Size");
		long nmarksFound = 0;
		for (long i = 1; i <= numberOfRows; i++) {
			long numberOfTokens;
			autoMelderTokens rowi (prefs.mark[i-1], &numberOfTokens);
			if (numberOfTokens < 4) { // we are done
				break;
			}
			Table_appendRow (marks.peek());
			for (long j = 1; j <= 4; j++) {
				Table_setStringValue (marks.peek(), i, j, rowi[j]);
			}
			nmarksFound++;
		}
		if (nmarksFound == 0) {
			my speakerType = prefs.speakerType = 1;
			my marksDataset = prefs.marksDataset = 1;
			VowelEditor_setMarks (me, my marksDataset, my speakerType, prefs.marksFontSize);
		} else {
			my marks = marks.transfer(); // my marks == NULL;
		}
	} catch (MelderError) {
		Melder_throw (L"Cannot create Table from preferences. Default marks set.");
		my speakerType = prefs.speakerType = 1;
		my marksDataset = prefs.marksDataset = 1;
		VowelEditor_setMarks (me, my marksDataset, my speakerType, prefs.marksFontSize);
	}
}

static void Table_addColumn_size (Table me, int size) {
	long col_size = Table_findColumnIndexFromColumnLabel (me, L"Size");
	if (col_size == 0) {
		Table_appendColumn (me, L"Size");
		for (long i = 1; i <= my rows -> size; i++) {
			Table_setNumericValue (me, i, my numberOfColumns, size);
		}
	}
}

static void VowelEditor_setMarks (VowelEditor me, int marksDataset, int speakerType, int fontSize) {
	autoTable te = 0;
	const wchar_t *Type[4] = { L"", L"m", L"w", L"c" };
	const wchar_t *Sex[3] = { L"", L"m", L"f"};
	if (marksDataset == 1) { // American-English
		autoTable thee = Table_createFromPetersonBarneyData ();
		te.reset (Table_extractRowsWhereColumn_string (thee.peek(), 1, kMelder_string_EQUAL_TO, Type[speakerType]));
	} else if (marksDataset == 2) { // Dutch
		if (speakerType == 1 || speakerType == 2) { // male + female from Pols van Nierop
			autoTable thee = Table_createFromPolsVanNieropData ();
			te.reset (Table_extractRowsWhereColumn_string (thee.peek(), 1, kMelder_string_EQUAL_TO, Sex[speakerType]));
		} else {
			autoTable thee = Table_createFromWeeninkData ();
			te.reset (Table_extractRowsWhereColumn_string (thee.peek(), 1, kMelder_string_EQUAL_TO, Type[speakerType]));
		}
	} else if (marksDataset == 3) { // None
		forget (my marks);
		return;
	} else { // Leave as is
		return;
	}
	autoTable thee = Table_collapseRows (te.peek(), L"IPA", L"", L"F1 F2", L"", L"", L"");
	long col_ipa = Table_findColumnIndexFromColumnLabel (thee.peek(), L"IPA");
	Table_setColumnLabel (thee.peek(), col_ipa, L"Vowel");
	Table_addColumn_size (thee.peek(), fontSize);
	forget (my marks);
	my marks = thee.transfer();
	copyVowelMarksInPreferences_volatile (my marks);
}

static void VowelEditor_getVowelMarksFromTableFile (VowelEditor me, MelderFile file)
{
	try {
		autoData data =  (Data) Data_readFromFile (file);
		if (! Thing_member ((Thing) data.peek(), classTable)) Melder_throw (L"\"", MelderFile_name (file), L"\" is not a Table file");
		autoTable marks = (Table) data.transfer();
		// check if columns Vowel F1 & F2 are present
		Table_getColumnIndexFromColumnLabel (marks.peek(), L"Vowel");
		Table_getColumnIndexFromColumnLabel (marks.peek(), L"F1");
		Table_getColumnIndexFromColumnLabel (marks.peek(), L"F2");
		Table_addColumn_size (marks.peek(), prefs.marksFontSize);
		forget (my marks);
		my marks = marks.transfer();
		my marksDataset = prefs.marksDataset = 9999;
		copyVowelMarksInPreferences_volatile (my marks);
		// our marks preferences are dynamic, save each time
	} catch (MelderError) {
		Melder_throw ("Vowel marks from Table not shown.");
	}
}

static void VowelEditor_setF3F4 (VowelEditor me, double f3, double b3, double f4, double b4) {
	double xmin = my f2min, xmax = my f2max, dx = my f2max - my f2min, x1 = dx / 2;
	double dy = my f1max - my f1min, y1 = dy / 2;

	if (my f3 == 0) {
		my f3 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	if (my b3 == 0) {
		my b3 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	if (my f4 == 0) {
		my f4 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	if (my b4 == 0) {
		my b4 = Matrix_create (xmin, xmax, 1, dx, x1, my f1min, my f1max, 1, dy, y1);
	}
	my f3 -> z[1][1] = f3; my b3 -> z[1][1] = b3;
	my f4 -> z[1][1] = f4; my b4 -> z[1][1] = b4;
}

static double Matrix_getValue (Matrix me, double x, double y) {
	(void) x;
	(void) y;
	return my z[1][1];
}

static void VowelEditor_getF3F4 (VowelEditor me, double f1, double f2, double *f3, double *b3, double *f4, double *b4) {
	*f3 = Matrix_getValue (my f3, f2, f1);
	*b3 = Matrix_getValue (my b3, f2, f1);
	*f4 = Matrix_getValue (my f4, f2, f1);
	*b4 = Matrix_getValue (my b4, f2, f1);
}

static void VowelEditor_drawBackground (VowelEditor me, Graphics g) {
	double x1, y1, x2, y2, f1, f2;

	Graphics_setInner (g);
	Graphics_setWindow (g, 0, 1, 0, 1);
	Graphics_setGrey (g, 0);
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 2);
	Graphics_rectangle (g, 0, 1, 0, 1);
	Graphics_setLineWidth (g, 1);
	Graphics_setGrey (g, 0.5);
	int fontSize = Graphics_inqFontSize (g);
	// draw the marks
	if (my marks != 0) {
		long col_vowel = Table_getColumnIndexFromColumnLabel (my marks, L"Vowel");
		long col_f1 = Table_getColumnIndexFromColumnLabel (my marks, L"F1");
		long col_f2 = Table_getColumnIndexFromColumnLabel (my marks, L"F2");
		long col_fs = Table_findColumnIndexFromColumnLabel (my marks, L"Size");
		for (long i = 1; i <= my marks -> rows -> size; i++) {
			const wchar_t *label = Table_getStringValue_Assert (my marks, i, col_vowel);
			f1 = Table_getNumericValue_Assert (my marks, i, col_f1);
			f2 = Table_getNumericValue_Assert (my marks, i, col_f2);
			if (f1 >= my f1min && f1 <= my f1max && f2 >= my f2min && f2 <= my f2max) {
				VowelEditor_getXYFromF1F2 (me, f1, f2, &x1, &y1);
				int size = prefs.marksFontSize;
				if (col_fs != 0) {
					size = Table_getNumericValue_Assert (my marks, i, col_fs);
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
	VowelEditor_getXYFromF1F2 (me, my f2min, my f2min, &x1, &y1);
	if (y1 >= 0 && y1 <= 1) {
		VowelEditor_getXYFromF1F2 (me, my f1max, my f1max, &x2, &y2);
		if (x2 >= 0 && x2 <= 1) {
			Polygon p = Polygon_create (3);
			p -> x[1] = x1; p -> y[1] = y1;
			p -> x[2] = x2; p -> y[2] = y2;
			p -> x[3] =  1; p -> y[3] =  0;
			Graphics_fillArea (g, p -> numberOfPoints, & p -> x[1], & p -> y[1]);
			// Polygon_paint does not work because of use of Graphics_setInner.
			forget (p);
			Graphics_line (g, x1, y1, x2, y2);
		}
	}
	// Draw the grid
	if (my grid.df1 < (my f1max - my f1min)) { // Horizontal lines
		long iline = (my f1min + my grid.df1) / my grid.df1;
		Graphics_setGrey (g, 0.5);
		Graphics_setLineType (g, Graphics_DOTTED);
		while ( (f1 = iline * my grid.df1) < my f1max) {
			if (f1 > my f1min) {
				VowelEditor_getXYFromF1F2 (me, f1, my f2min, &x1, &y1);
				VowelEditor_getXYFromF1F2 (me, f1, my f2max, &x2, &y2);
				Graphics_line (g, x1, y1, x2, y2);
			}
			iline++;
		}
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setGrey (g, 0); // black
	}
	if (my grid.df2 < (my f2max - my f2min)) {
		long iline = (my f2min + my grid.df2) / my grid.df2;
		Graphics_setGrey (g, 0.5);
		Graphics_setLineType (g, Graphics_DOTTED);
		while ( (f2 = iline * my grid.df2) < my f2max) { // vert line
			if (f2 > my f2min) {
				VowelEditor_getXYFromF1F2 (me, my f1min, f2, &x1, &y1);
				VowelEditor_getXYFromF1F2 (me, my f1max, f2, &x2, &y2);
				Graphics_line (g, x1, y1, x2, y2);
			}
			iline++;
		}
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setGrey (g, 0); // black
	}
	Graphics_unsetInner (g);
	Graphics_setGrey (g, 0); // black
	Graphics_markLeft (g, 0, 0, 1, 0, Melder_double (my f1max));
	Graphics_markLeft (g, 1, 0, 1, 0, Melder_double (my f1min));
	Graphics_markTop (g, 0, 0, 1, 0, Melder_double (my f2max));
	Graphics_markTop (g, 1, 0, 1, 0, Melder_double (my f2min));

}

typedef struct {
	long some_check_value;
	long istart;
	float *z;
} *paVowelData;
/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/

static int paCallback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, I) {
	iam (paVowelData);
	float *out = (float *) outputBuffer;
	unsigned int i;
	(void) inputBuffer; /* Prevent unused variable warning. */
	(void) timeInfo;
	(void) statusFlags;

	for (i = 0; i < framesPerBuffer; i++) {
		*out++ = my z[my istart + i]; /* left */
		*out++ = my z[my istart + i]; /* right */
	}
	my istart += framesPerBuffer;
	return 0;
}

/********** MENU METHODS **********/

static void menu_cb_help (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	(void) me;
	Melder_help (L"VowelEditor");
}

static void menu_cb_prefs (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Preferences", 0);
		BOOLEAN (L"Sound-follows-mouse", 1)
	EDITOR_OK
		SET_INTEGER (L"Sound-follows-mouse", prefs.soundFollowsMouse)
	EDITOR_DO
		my frequencyScale = prefs.frequencyScale;
		my axisOrientation = prefs.axisOrientation;
		my soundFollowsMouse = prefs.soundFollowsMouse = GET_INTEGER (L"Sound-follows-mouse");
		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_ranges_f1f2 (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"F1 (vert) and F2 (hor) view ranges", 0);
		POSITIVE (L"left F1 range (Hz)", L"200.0")
		POSITIVE (L"right F1 range (Hz)", L"1000.0")
		POSITIVE (L"left F2 range (Hz)", L"500.0")
		POSITIVE (L"right F2 range (Hz)", L"2500.0")
	EDITOR_OK
		SET_REAL (L"left F1 range", prefs.f1min)
		SET_REAL (L"right F1 range", prefs.f1max)
		SET_REAL (L"left F2 range", prefs.f2min)
		SET_REAL (L"right F2 range", prefs.f2max)
	EDITOR_DO
		my frequencyScale = prefs.frequencyScale;
		my axisOrientation = prefs.axisOrientation;
		my f1min = prefs.f1min = GET_REAL (L"left F1 range");
		my f1max = prefs.f1max = GET_REAL (L"right F1 range");
		my f2min = prefs.f2min = GET_REAL (L"left F2 range");
		my f2max = prefs.f2max = GET_REAL (L"right F2 range");
		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_publishSound (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	autoSound publish = VowelEditor_createTarget (me);
	my broadcastPublication (publish.transfer());
}

static void menu_cb_extract_FormantGrid (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	VowelEditor_updateVowel (me);
	autoFormantGrid publish = FormantTier_to_FormantGrid (my vowel -> ft);
	my broadcastPublication (publish.transfer());
}

static void menu_cb_extract_KlattGrid (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	VowelEditor_updateVowel (me);
	autoFormantGrid fg = FormantTier_to_FormantGrid (my vowel -> ft);
	autoKlattGrid publish = KlattGrid_create (fg -> xmin, fg -> xmax, fg -> formants -> size, 0, 0, 0, 0, 0, 0);
	KlattGrid_addVoicingAmplitudePoint (publish.peek(), fg -> xmin, 90);
	KlattGrid_replacePitchTier (publish.peek(), my vowel -> pt);
	KlattGrid_replaceFormantGrid (publish.peek(), KlattGrid_ORAL_FORMANTS, fg.peek());
	my broadcastPublication (publish.transfer());
}

static void menu_cb_extract_PitchTier (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	VowelEditor_updateVowel (me);
	autoPitchTier publish = Data_copy (my vowel -> pt);
	my broadcastPublication (publish.transfer());
}

static void menu_cb_drawTrajectory (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Draw trajectory", 0)
		my v_form_pictureWindow (cmd);
		BOOLEAN (L"Garnish", 1)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
	EDITOR_DO
		int garnish = GET_INTEGER (L"Garnish");
		my v_do_pictureWindow (cmd);
		Editor_openPraatPicture (me);
		if (garnish) {
			VowelEditor_drawBackground (me, my pictureGraphics);
		}
		FormantTier_drawF1F2Trajectory (my vowel -> ft, my pictureGraphics, my f1min, my f1max, my f2min, my f2max, my markTraceEvery, my width);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_showOneVowelMark (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Show one vowel mark", 0);
		POSITIVE (L"F1 (Hz)", L"300.0")
		POSITIVE (L"F2 (Hz)", L"600.0")
		WORD (L"Mark", L"u")
	EDITOR_OK
	EDITOR_DO
		double f1 = GET_REAL (L"F1");
		double f2 = GET_REAL (L"F2");
		wchar_t *label = GET_STRING (L"Mark");
		if (f1 >= my f1min && f1 <= my f1max && f2 >= my f2min && f2 <= my f2max) {
			long irow = 1;
			if (my marks == NULL) {
				my marks = Table_createWithColumnNames (1, L"IPA F1 F2 Colour");
			} else {
				Table_appendRow (my marks);
			}
			irow = my marks -> rows -> size;
			Table_setStringValue (my marks, irow, 1, label);
			Table_setNumericValue (my marks, irow, 2, f1);
			Table_setNumericValue (my marks, irow, 3, f2);
			Graphics_updateWs (my g);
		}
	EDITOR_END
}

static void menu_cb_showVowelMarks (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Show vowel marks", 0);
		LABEL (L"note", L"")
		OPTIONMENU (L"Data set", 1)
			OPTION (L"American English")
			OPTION (L"Dutch")
			OPTION (L"None")
		OPTIONMENU (L"Speaker", 1)
			OPTION (L"Man")
			OPTION (L"Woman")
			OPTION (L"Child")
		NATURAL (L"Font size (points)", L"14")
	EDITOR_OK
		if (my marksDataset == 9999) SET_STRING (L"note", L"(Warning: current vowel marks are not from one of these data sets.)")
		SET_INTEGER (L"Data set", my marksDataset);
		SET_INTEGER (L"Speaker", my speakerType);
		SET_INTEGER (L"Font size", my marksFontSize);
	EDITOR_DO
		my marksDataset = prefs.marksDataset = GET_INTEGER (L"Data set");
		my speakerType = prefs.speakerType = GET_INTEGER (L"Speaker");
		my marksFontSize = prefs.marksFontSize = GET_INTEGER (L"Font size");
		VowelEditor_setMarks (me, my marksDataset, my speakerType, my marksFontSize);
		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_showVowelMarksFromTableFile (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM_READ (L"VowelEditor: Show vowel marks from Table file", L"VowelEditor: Show vowel marks from Table file...");
	EDITOR_DO_READ
		VowelEditor_getVowelMarksFromTableFile (me, file);
		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_setF0 (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Set F0", 0);
		POSITIVE (L"Start F0 (Hz)", L"150.0")
		REAL (L"Slope (oct/s)", L"0.0")
	EDITOR_OK
	EDITOR_DO
		double f0 = GET_REAL (L"Start F0");
		checkF0 (&my f0, &f0);
		my f0.start = f0;
		my f0.slopeOctPerSec = GET_REAL (L"Slope");
		VowelEditor_setSource (me);
		my f0TextField -> f_setString (Melder_double (my f0.start));
		my f0SlopeTextField -> f_setString (Melder_double (my f0.slopeOctPerSec));
	EDITOR_END
}

static void menu_cb_setF3F4 (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Set F3 & F4", 0);
		POSITIVE (L"F3 (Hz)", L"2500.0")
		POSITIVE (L"B3 (Hz)", L"250.0")
		POSITIVE (L"F4 (Hz)", L"3500.0")
		POSITIVE (L"B4 (Hz)", L"350.0")
	EDITOR_OK
	EDITOR_DO
		double f3 = GET_REAL (L"F3"), b3 = GET_REAL (L"B3");
		double f4 = GET_REAL (L"F4"), b4 = GET_REAL (L"B4");
		if (f3 >= f4) {
			Melder_throw ("F4 must be larger than F3.");
		}
		VowelEditor_setF3F4 (me, f3, b3, f4, b4);
	EDITOR_END
}
static void menu_cb_reverseTrajectory (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	VowelEditor_Vowel_reverseFormantTier (me);
	Graphics_updateWs (my g);
}

static void VowelEditor_Vowel_addData (VowelEditor me, Vowel thee, double time, double f1, double f2, double f0) {
	autoFormantPoint fp = FormantPoint_create (time);
	double f3, b3, f4, b4;
	fp -> formant[0] = f1;
	fp -> bandwidth[0] = f1 / 10;
	fp -> formant[1] = f2;
	fp -> bandwidth[1] = f2 / 10;
	VowelEditor_getF3F4 (me, f1, f2, &f3, &b3, &f4, &b4);
	fp -> formant[2] = f3;
	fp -> bandwidth[2] = b3;
	fp -> formant[3] = f4;
	fp -> bandwidth[3] = b4;
	fp -> numberOfFormants = 4;

	Collection_addItem (thy ft -> points, fp.transfer());
	RealTier_addPoint (thy pt, time, f0);
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
	if (*f0 == NUMundefined) {
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
	if (*x < 0) {
		*x = 0;
	} else if (*x > 1) {
		*x = 1;
	}
	if (*y < 0) {
		*y = 0;
	} else if (*y > 1) {
		*y = 1;
	}
}

static void menu_cb_newTrajectory (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"New Trajectory", 0);
		POSITIVE (L"Start F1 (Hz)", L"700.0")
		POSITIVE (L"Start F2 (Hz)", L"1200.0")
		POSITIVE (L"End F1 (Hz)", L"350.0")
		POSITIVE (L"End F2 (Hz)", L"800.0")
		POSITIVE (L"Duration (s)", L"0.25")
	EDITOR_OK
	EDITOR_DO
		double f0, f1, f2, time, duration = GET_REAL (L"Duration");
		autoVowel vowel = Vowel_create (duration);
		time = 0;
		f0 =  getF0 (&my f0, time);
		f1 = GET_REAL (L"Start F1");
		f2 = GET_REAL (L"Start F2");
		checkF1F2 (me, &f1, &f2);
		VowelEditor_Vowel_addData (me, vowel.peek(), time, f1, f2, f0);
		time = duration;
		f0 =  getF0 (&my f0, time);
		f1 = GET_REAL (L"End F1");
		f2 = GET_REAL (L"End F2");
		checkF1F2 (me, &f1, &f2);
		VowelEditor_Vowel_addData (me, vowel.peek(), time, f1, f2, f0);

		my durationTextField -> f_setString (Melder_double (MICROSECPRECISION (duration)));
		forget (my vowel);
		my vowel = vowel.transfer();

		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_extendTrajectory (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Extend Trajectory", 0);
		POSITIVE (L"To F1 (Hz)", L"500.0")
		POSITIVE (L"To F2 (Hz)", L"1500.0")
		POSITIVE (L"Extra duration (s)", L"0.1")
	EDITOR_OK
	EDITOR_DO
		Vowel thee = my vowel;
		double newDuration = thy xmax + GET_REAL (L"Extra duration");
		double f0 =  getF0 (&my f0, newDuration);
		double f1 = GET_REAL (L"To F1");
		double f2 = GET_REAL (L"To F2");
		thy xmax = thy pt -> xmax = thy ft -> xmax = newDuration;
		checkF1F2 (me, &f1, &f2);
		VowelEditor_Vowel_addData (me, thee, newDuration, f1, f2, f0);

		my durationTextField -> f_setString (Melder_double (MICROSECPRECISION (newDuration)));
		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_modifyTrajectoryDuration (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Modify duration", 0);
		POSITIVE (L"New duration (s)", L"0.5")
	EDITOR_OK
	EDITOR_DO
		my durationTextField -> f_setString (Melder_double (MICROSECPRECISION (GET_REAL (L"New duration"))));
	EDITOR_END
}

static void menu_cb_shiftTrajectory (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Shift trajectory", 0);
		REAL (L"F1 (semitones)", L"0.5")
		REAL (L"F2 (semitones)", L"0.5")
	EDITOR_OK
	EDITOR_DO
		VowelEditor_shiftF1F2 (me, GET_REAL (L"F1"), GET_REAL (L"F2"));
		Graphics_updateWs (my g);
	EDITOR_END
}

static void menu_cb_showTrajectoryTimeMarksEvery (EDITOR_ARGS) {
	EDITOR_IAM (VowelEditor);
	EDITOR_FORM (L"Show trajectory time marks every", 0);
		REAL (L"Distance (s)", L"0.05")
	EDITOR_OK
		SET_REAL (L"Distance", my markTraceEvery)
	EDITOR_DO
		my markTraceEvery = GET_REAL (L"Distance");
		if (my markTraceEvery < 0) {
			my markTraceEvery = 0;
		}
		Graphics_updateWs (my g);
	EDITOR_END
}

/********** BUTTON METHODS **********/

static void gui_button_cb_play (I, GuiButtonEvent event) {
	(void) event;
	iam (VowelEditor);
	autoSound thee = VowelEditor_createTarget (me);
	Sound_play (thee.peek(), 0, 0);
	Graphics_updateWs (my g);
}

static void gui_button_cb_publish (I, GuiButtonEvent event) {
	(void) event;
	iam (VowelEditor);
	autoSound publish = VowelEditor_createTarget (me);
	my broadcastPublication (publish.transfer());
}

static void gui_button_cb_reverse (I, GuiButtonEvent event) {
	(void) event;
	iam (VowelEditor);

	VowelEditor_Vowel_reverseFormantTier (me);
	struct structGuiButtonEvent play_event = { 0 };
	gui_button_cb_play (me, &play_event);
}

/* Main drawing routine: it's been called after every call to Graphics_updateWs (g) */
static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (VowelEditor);
	(void) event;
	double ts = my vowel -> xmin, te = my vowel -> xmax;
	FormantTier ft = (FormantTier) my vowel -> ft;
	static MelderString statusInfo = { 0 };
	if (my g == 0) {
		return;    // Could be the case in the very beginning.
	}
	Graphics_clearWs (my g);

	appendF1F2F0 (&statusInfo, STATUSINFO_STARTINTR0, FormantTier_getValueAtTime (ft, 1, ts),
	              FormantTier_getValueAtTime (ft, 2, ts), getF0 (&my f0, ts), STATUSINFO_ENDING);
	my startInfo -> f_setString (statusInfo.string);
	MelderString_empty (&statusInfo);

	appendF1F2F0 (&statusInfo, STATUSINFO_ENDINTR0, FormantTier_getValueAtTime (ft, 1, te),
	              FormantTier_getValueAtTime (ft, 2, te), getF0 (&my f0, te), STATUSINFO_ENDING);
	my endInfo -> f_setString (statusInfo.string);
	MelderString_empty (&statusInfo);

	Graphics_setGrey (my g, 0.9);
	Graphics_fillRectangle (my g, 0, 1, 0, 1);
	Graphics_setInner (my g);
	Graphics_setWindow (my g, 0, 1, 0, 1);
	Graphics_setGrey (my g, 1);
	Graphics_fillRectangle (my g, 0, 1, 0, 1);
	Graphics_unsetInner (my g);
	Graphics_setGrey (my g, 0);

	VowelEditor_drawBackground (me, my g);
	FormantTier_drawF1F2Trajectory (my vowel -> ft, my g, my f1min, my f1max, my f2min, my f2max, my markTraceEvery, my width);
}

static void gui_drawingarea_cb_resize__ (I, GuiDrawingAreaResizeEvent event) {
	iam (VowelEditor);
	(void) me;
	(void) event;
	if (me == NULL || my g == NULL) {
		return;
	}
	my height = my drawingArea -> f_getHeight ();
	my width = my drawingArea -> f_getWidth ();
	Graphics_setWsViewport (my g, 0, my width , 0, my height);
	Graphics_setWsWindow (my g, 0, my width, 0, my height);
	Graphics_setViewport (my g, 0, my width, 0, my height);
	Graphics_updateWs (my g);
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (VowelEditor);
	if (my g == NULL) {
		return;    // Could be the case in the very beginning.
	}
	Graphics_setWsViewport (my g, 0, event -> width, 0, event -> height);
	my width = event -> width;
	my height = event -> height;
	Graphics_setWsWindow (my g, 0, my width, 0, my height);
	Graphics_setViewport (my g, 0, my width, 0, my height);
	Graphics_updateWs (my g);

	/* Save the current shell size as the user's preference for a new FunctionEditor. */

	prefs.shellWidth = my d_windowForm -> f_getShellWidth ();
	prefs.shellHeight = my d_windowForm -> f_getShellHeight ();
}

static void VowelEditor_Vowel_updateTiers (VowelEditor me, Vowel thee, double time, double x, double y) {
	double f3, b3, f4, b4;
	if (time > thy xmax) {
		thy xmax = time;
		thy ft -> xmax = time;
		thy pt -> xmax = time;
	}
	double f0 = getF0 (& my f0, time), f1, f2;
	autoFormantPoint point = FormantPoint_create (time);

	VowelEditor_getF1F2FromXY (me, x, y, &f1, &f2);
	VowelEditor_getF3F4 (me, f1, f2, &f3, &b3, &f4, &b4);

	point -> formant[0] = f1;
	point -> bandwidth[0] = f1 / 10;
	point -> formant[1] = f2;
	point -> bandwidth[1] = f2 / 10;
	point -> formant[2] = f3;
	point -> bandwidth[2] = b3;
	point -> formant[3] = f4;
	point -> bandwidth[3] = b4;
	point -> numberOfFormants = 4;
	Collection_addItem (thy ft -> points, point.transfer());
	RealTier_addPoint (thy pt, time, f0);
}

// shift key always extends what already is.
// Special case : !soundFollowsMouse. The first click just defines the vowel's first f1f2-position,
static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (VowelEditor);
	(void) event;
	Vowel thee = 0; autoVowel athee = 0;
	double x, y, xb, yb, tb, t, dt = 0;
	double t0 = Melder_clock ();
	long iskipped = 0;
	struct structGuiButtonEvent gb_event = { 0 };
	Graphics_setInner (my g);

	Graphics_getMouseLocation (my g, & x, & y);
	checkXY (&x, &y);

	if (event -> shiftKeyPressed) {
		VowelEditor_updateExtendDuration (me);
		(my shiftKeyPressed) ++;
		thee = my vowel;
		dt = thy xmax + my extendDuration;
		t = 0 + dt;
		VowelEditor_Vowel_updateTiers (me, thee, t, x, y);
		my durationTextField -> f_setString (Melder_double (t));
		if (! my soundFollowsMouse) {
			goto end;
		}
	} else {
		t = 0;
		my shiftKeyPressed = 0;
		athee.reset (Vowel_create (MINIMUM_SOUND_DURATION));
		thee = athee.peek();
		VowelEditor_Vowel_updateTiers (me, thee, t, x, y);
		my durationTextField -> f_setString (Melder_double (t));
		if (! my soundFollowsMouse) {
			VowelEditor_Vowel_updateTiers (me, thee, MINIMUM_SOUND_DURATION, x, y);
			goto end;
		}
	}

	Graphics_xorOn (my g, Graphics_BLUE);
	while (Graphics_mouseStillDown (my g)) {
		xb = x, yb = y, tb = t;
		t = Melder_clock () - t0 + dt; // Get relative time in seconds from the clock
		Graphics_getMouseLocation (my g, & x, & y);
		checkXY (&x, &y);
		// If the new point equals the previous one: no tier update
		if (xb == x && yb == y) {
			iskipped++;
			continue;
		}
		// Add previous point only if at least one previous event was skipped...
		if (iskipped > 0) {
			VowelEditor_Vowel_updateTiers (me, thee, tb, xb, yb);
		}
		iskipped = 0;
		Graphics_line (my g, xb, yb, x, y);

		VowelEditor_Vowel_updateTiers (me, thee, t, x, y);
		my durationTextField -> f_setString (Melder_double (MICROSECPRECISION (t)));
	}
	t = Melder_clock () - t0;
	// To prevent ultra short clicks we set a minimum of 0.01 s duration
	if (t < MINIMUM_SOUND_DURATION) {
		t = MINIMUM_SOUND_DURATION;
	}
	t += dt;
	my durationTextField -> f_setString (Melder_double (MICROSECPRECISION (t)));
	VowelEditor_Vowel_updateTiers (me, thee, t, x, y);

	Graphics_xorOff (my g);

end:
	Graphics_unsetInner (my g);

	if (my shiftKeyPressed == 0) {
		forget (my vowel);
		my vowel = athee.transfer();
	}
	gui_button_cb_play (me, & gb_event);
}

static void gui_drawingarea_cb_key (I, GuiDrawingAreaKeyEvent event) {
	iam (VowelEditor);
	(void) me;
	(void) event;
}

static void cb_publish (Editor editor, void *closure, Data publish) {
	(void) editor;
	(void) closure;
	try {
		praat_new (publish, 0);
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError (0);
	}
}

static void updateWidgets (I) {
	iam (VowelEditor);
	(void) me;
}

void structVowelEditor :: v_destroy () {
	forget (g);
	forget (marks);
	forget (source);
	forget (target);
	forget (vowel);
	forget (f3); forget (b3);
	forget (f4); forget (b4);
	VowelEditor_Parent :: v_destroy ();
}

void structVowelEditor :: v_createMenus () {
	VowelEditor_Parent :: v_createMenus ();

	Editor_addMenu (this, L"View", 0);
	Editor_addCommand (this, L"File", L"Preferences...", 0, menu_cb_prefs);
	Editor_addCommand (this, L"File", L"-- publish data --", 0, NULL);
	Editor_addCommand (this, L"File", L"Publish Sound", 0, menu_cb_publishSound);
	Editor_addCommand (this, L"File", L"Extract KlattGrid", 0, menu_cb_extract_KlattGrid);
	Editor_addCommand (this, L"File", L"Extract FormantGrid", 0, menu_cb_extract_FormantGrid);
	Editor_addCommand (this, L"File", L"Extract PitchTier", 0, menu_cb_extract_PitchTier);
	Editor_addCommand (this, L"File", L"-- drawing --", 0, NULL);
	Editor_addCommand (this, L"File", L"Draw trajectory...", 0, menu_cb_drawTrajectory);
	Editor_addCommand (this, L"File", L"-- scripting --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"-- f0 --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Set F0...", 0, menu_cb_setF0);
	Editor_addCommand (this, L"Edit", L"Set F3 & F4...", 0, menu_cb_setF3F4);
	Editor_addCommand (this, L"Edit", L"-- trajectory commands --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Reverse trajectory", 0, menu_cb_reverseTrajectory);
	Editor_addCommand (this, L"Edit", L"Modify trajectory duration...", 0, menu_cb_modifyTrajectoryDuration);
	Editor_addCommand (this, L"Edit", L"New trajectory...", 0, menu_cb_newTrajectory);
	Editor_addCommand (this, L"Edit", L"Extend trajectory...", 0, menu_cb_extendTrajectory);
	Editor_addCommand (this, L"Edit", L"Shift trajectory...", 0, menu_cb_shiftTrajectory);
	Editor_addCommand (this, L"View", L"F1 & F2 range...", 0, menu_cb_ranges_f1f2);
	Editor_addCommand (this, L"View", L"--show vowel marks--", 0, NULL);
	Editor_addCommand (this, L"View", L"Show one vowel mark...", Editor_HIDDEN, menu_cb_showOneVowelMark);
	Editor_addCommand (this, L"View", L"Show vowel marks...", Editor_HIDDEN, menu_cb_showVowelMarks);
	Editor_addCommand (this, L"View", L"Show vowel marks from fixed set...", 0, menu_cb_showVowelMarks);
	Editor_addCommand (this, L"View", L"Show vowel marks from Table file...", 0, menu_cb_showVowelMarksFromTableFile);
	Editor_addCommand (this, L"View", L"--show trajectory time marks--", 0, NULL);
	Editor_addCommand (this, L"View", L"Show trajectory time marks every...", 0, menu_cb_showTrajectoryTimeMarksEvery);
}

void structVowelEditor :: v_createHelpMenuItems (EditorMenu menu) {
	VowelEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"VowelEditor help", '?', menu_cb_help);
}

void structVowelEditor :: v_createChildren ()
{
	double button_width = 90, text_width = 95, status_info_width = 290;
	double left, right, top, bottom, bottom_widgets_top, bottom_widgets_bottom, bottom_widgets_halfway;

	// Three buttons on a row: Play, Reverse, Publish
	left = 10; right = left + button_width;
	bottom_widgets_top = top = -MARGIN_BOTTOM + 10; bottom_widgets_bottom = bottom = -STATUS_INFO;
	playButton = GuiButton_createShown (d_windowForm, left, right, top, bottom, L"Play", gui_button_cb_play, this, 0);
	left = right + 10; right = left + button_width;
	reverseButton = GuiButton_createShown (d_windowForm, left, right, top, bottom, L"Reverse", gui_button_cb_reverse, this, 0);
	left = right + 10; right = left + button_width;
	publishButton = GuiButton_createShown (d_windowForm, left, right, top, bottom, L"Publish", gui_button_cb_publish, this, 0);
	// Four Text widgets with the label on top: Duration, Extend, F0, Slope
	// Make the F0 slope button 10 wider to accomodate the text
	// We wil not use a callback from a Text widget. It will get called multiple times during the editing
	// of the text. Better to have all editing done and then query the widget for its value!
	left = right + 10; right = left + text_width; bottom_widgets_halfway = bottom = (top + bottom) / 2; top = bottom_widgets_top;
	GuiLabel_createShown (d_windowForm, left, right, top , bottom, L"Duration (s):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	durationTextField = GuiText_createShown (d_windowForm, left, right, top, bottom, 0);

	left = right + 10; right = left + text_width; top = bottom_widgets_top; bottom = bottom_widgets_halfway;
	GuiLabel_createShown (d_windowForm, left, right, top, bottom, L"Extend (s):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	extendTextField = GuiText_createShown (d_windowForm, left, right, top, bottom, 0);

	left = right + 10; right = left + text_width; top = bottom_widgets_top; bottom = bottom_widgets_halfway;
	GuiLabel_createShown (d_windowForm, left, right, top, bottom, L"Start F0 (Hz):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	f0TextField = GuiText_createShown (d_windowForm, left, right, top, bottom, 0);

	left = right + 10; right = left + text_width + 10; top = bottom_widgets_top; bottom = bottom_widgets_halfway;
	GuiLabel_createShown (d_windowForm, left, right, top, bottom, L"F0 slope (oct/s):", 0);
	top = bottom; bottom = bottom_widgets_bottom;
	f0SlopeTextField = GuiText_createShown (d_windowForm, left, right, top, bottom, 0);

	// The status startInfo and endInfo widget at the bottom:

	bottom = - (STATUS_INFO - Gui_LABEL_HEIGHT) / 2; top = bottom - Gui_LABEL_HEIGHT; left = MARGIN_LEFT; right = left + status_info_width;
	startInfo = GuiLabel_createShown (d_windowForm, left, right, top, bottom, L"", 0);

	left = right; right = left + status_info_width;
	endInfo = GuiLabel_createShown (d_windowForm, left, right, top, bottom, L"", 0);

	/***** Create drawing area. *****/
	// Approximately square because for our defaults: f1min=200, f1max=1000 and f2min = 500, f2mx = 2500,
	// log distances are equal (log (1000/200) == log (2500/500) ).
	//drawingArea = GuiDrawingArea_createShown (d_windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
	//	gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
	drawingArea = GuiDrawingArea_createShown (d_windowForm, 0, 0, Machine_getMenuBarHeight (), -MARGIN_BOTTOM,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, this, 0);
	height = drawingArea -> f_getHeight ();
	width = drawingArea -> f_getWidth ();
}

static void VowelEditor_setSource (VowelEditor me) {
	autoPitchTier pt = VowelEditor_to_PitchTier (me, my maximumDuration);
	autoSound thee = PitchTier_to_Sound_pulseTrain (pt.peek(), my f0.samplingFrequency, my f0.adaptFactor, my f0.adaptTime, my f0.interpolationDepth, 0);
	if (my source != 0) {
		forget (my source);
	}
	my source = thee.transfer();
}
//
static Sound VowelEditor_createTarget (VowelEditor me) {
	try {
		VowelEditor_updateVowel (me); // update pitch and duration
		autoSound thee = Vowel_to_Sound_pulses (my vowel, 44100, 0.7, 0.05, 30);
		Vector_scale (thee.peek(), 0.99);
		Sound_fadeIn (thee.peek(), 0.005, 1);
		Sound_fadeOut (thee.peek(), 0.005);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Target Sound not created.");
	}
}

VowelEditor VowelEditor_create (const wchar_t *title, Data data) {
	try {
		autoVowelEditor me = Thing_new (VowelEditor);
		Editor_init (me.peek(), 0, 0, prefs.shellWidth, prefs.shellHeight, title, data);
#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
#endif
		my g = Graphics_create_xmdrawingarea (my drawingArea);
		Graphics_setFontSize (my g, 12);
		my setPublicationCallback (cb_publish, NULL);

		my f1min = prefs.f1min;
		my f1max = prefs.f1max;
		my f2min = prefs.f2min;
		my f2max = prefs.f2max;
		my frequencyScale = prefs.frequencyScale;
		my axisOrientation = prefs.axisOrientation;
		my speakerType = prefs.speakerType;
		my marksDataset = prefs.marksDataset;
		my marksFontSize = prefs.marksFontSize;
		my soundFollowsMouse = prefs.soundFollowsMouse;
		if (my marksDataset > 3) { // TODO variable??
			VowelEditor_createTableFromVowelMarksInPreferences (me.peek());
		} else {
			VowelEditor_setMarks (me.peek(), my marksDataset, my speakerType, 14);
		}
		VowelEditor_setF3F4 (me.peek(), prefs.f3, prefs.b3, prefs.f4, prefs.b4);
		my maximumDuration = BUFFER_SIZE_SEC;
		my extendDuration = prefs.extendDuration;
		if (my data != 0) {
			my vowel = Data_copy ( (Vowel) data);
		} else {
			my vowel = Vowel_create_twoFormantSchwa (0.2);
		}
		my markTraceEvery = prefs.markTraceEvery;
		my f0 = f0default;
		VowelEditor_setSource (me.peek());
		my target = Sound_createSimple (1, my maximumDuration, my f0.samplingFrequency);
		my f0TextField -> f_setString (Melder_double (my f0.start));
		my f0SlopeTextField -> f_setString (Melder_double (my f0.slopeOctPerSec));
		my durationTextField -> f_setString (L"0.2"); // Source has been created
		my extendTextField -> f_setString (Melder_double (my extendDuration));
		my grid = griddefault;
		{
			// This exdents because it's a hack:
			struct structGuiDrawingAreaResizeEvent event = { my drawingArea, 0 };
			event. width = my drawingArea -> f_getWidth ();
			event. height = my drawingArea -> f_getHeight ();
			gui_drawingarea_cb_resize (me.peek(), & event);
		}
		//struct structGuiDrawingAreaResizeEvent event = { 0 };
		//event.widget = my drawingArea;
		//gui_drawingarea_cb_resize (me, & event);
		updateWidgets (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("VowelEditor not created.");
	}
}

/* End of file VowelEditor.cpp */
