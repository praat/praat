/* VowelGenerator.c
 *
 * Copyright (C) 2008 David Weenink
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
 djmw 20080111
*/

/*
 The main part of the VowelGenerator is a drawing area. In the drawing area a cursor can be moved around by a mouse.
 The position of the cursor is related to the F1 and F2 frequencies.
 On_mouse_down the position of the cursor is sampled at a rate of VG_Cursor_Sampling_Rate (default say 1000 Hz).
 This results in a series of (x,y) values that will be transformed to (f1,f2) values in Hertz 
 (according to the scale of the x- and y-axes).
 The corresponding sound wil be made audible until the mouse is released.
 The third and higher formant frequencies can also be set indirectly by defining then as functions on the f1,f2 plane
 (for example, by an object of type Matrix).
 Graphics area id F1-F2 plane. Axis orientation from topright: F1 down, F2 to the left.
 f1, f2 in Hz;
 coordinate positions x,y in range [0,1]
 log(fmin) -> 1; log(fmax)-> 0
  For x direction F2 from right to left
   1 = a * log(f2min) +b 
   0 = a * log(f2max) +b
   x' = a (log(f2)-log(f2max))
	
   1 = a * log(f1min) +b
   0 = a * log(f1max) +b
   y' = a (log(f1)-log(f1max))
Watch out: the coordinates of a trajectory are absolute coordiates. this means that changing the domain of the
F1 and F2 axis changes the sound!!!!!!	
*/


#include "PitchTier_to_Sound.h"
#include "Polygon.h"
#include "TableOfReal_extensions.h"
#include "Table_extensions.h"
#include "VowelGenerator.h"
#include "machine.h"
#include "Preferences.h"
#include "EditorM.h"
#include "time.h"

// Male, Female, Child speaker
#define VG_SPEAKER_M 0
#define VG_SPEAKER_F 1
#define VG_SPEAKER_C 2

#define MARGIN_RIGHT 10
#define MARGIN_LEFT 50
#define MARGIN_TOP 50
#define MARGIN_BOTTOM 70
#define BUFFER_SIZE_SEC 4
#define SAMPLING_FREQUENCY 44100
// Too prevent the generation of inaudible short Sounds we set a minimum duration
#define MINIMUM_SOUND_DURATION 0.01
// forward declarations 
bool praat_new1 (I, const wchar_t *s1); // We cannot include "praat.h"
void praat_updateSelection (void);

void Sound_fadeIn (Sound me, double duration, int fromFirstNonZeroSample);
void Sound_fadeOut (Sound me, double duration);
FormantTier VowelGenerator_to_FormantTier (VowelGenerator me);
PitchTier VowelGenerator_to_PitchTier (VowelGenerator me);
void VowelGenerator_getPostionFromFrequencies (VowelGenerator me, double f1, double f2, double *x, double *y);
void VowelGenerator_getFrequenciesFromPostion (VowelGenerator me, double x, double y, double *f1, double *f2);
void VowelGenerator_reverseTrajectory (VowelGenerator me);
void VowelGenerator_modifyTrajectoryDuration (VowelGenerator me, double newDuration);
void VowelGenerator_shiftTrajectory (VowelGenerator me, double f1_st, double f2_st);
void VowelGenerator_updateTrajectoryDuration (VowelGenerator me);
void VowelGenerator_updateTrajectory (VowelGenerator me);
void VowelGenerator_updateF0 (VowelGenerator me);
void VowelGenerator_drawTrajectory (VowelGenerator me);
TableOfReal getDutchVowelAverages (int speakerType);
int VowelGenerator_setSource (VowelGenerator me, double f0, double slope);
Sound VowelGenerator_createTarget (VowelGenerator me);
int VowelGenerator_setMarks (VowelGenerator me, int dataset, int speakerType);
// forward declarations end

void Sound_fadeIn (Sound me, double duration, int fromFirstNonZeroSample)
{
	long istart = 1, numberOfSamples = duration / my dx;
	
	if (numberOfSamples < 2) return;
	if (fromFirstNonZeroSample != 0)
	{
		// If the first part of the sound is very low level we put sample values to zero and 
		// start windowing from the position where the amplitude is above the minimum level.
		// WARNING: this part is special for the artificial vowels because
		// 1. They have no offset
		// 2. They are already scaled to a maximum amplitude of 0.99
		// 3. For 16 bit precision
		double zmin = 0.5 / pow(2, 16);
		while (fabs (my z[1][istart]) < zmin)
		{
			my z[1][istart] = 0; // To make sure 
			istart++;
		}
	}
	if (numberOfSamples > my nx - istart + 1) numberOfSamples = my nx - istart + 1;
	
	for (long i = 1; i <= numberOfSamples; i++)
	{
		double phase = NUMpi * (i - 1) / (numberOfSamples - 1);
			
		my z[1][istart + i -1] *= 0.5 * (1 - cos (phase));	
	}
}

void Sound_fadeOut (Sound me, double duration)
{
	long istart, numberOfSamples = duration / my dx;
	
	if (numberOfSamples < 2) return;
	if (numberOfSamples > my nx) numberOfSamples = my nx;
	istart = my nx - numberOfSamples;
	// only one channel
	for (long i = 1; i <= numberOfSamples; i++)
	{
		double phase = NUMpi * (i - 1)/ (numberOfSamples - 1);
			
		my z[1][istart + i] *= 0.5 * (1 + cos (phase));	
	}
}

void VowelGenerator_reverseTrajectory (VowelGenerator me)
{
	double duration = my ptrace[my nptrace].t;
	long nptrace_2 = my nptrace / 2;
	
	for (long it = 1; it <= nptrace_2; it++)
	{
		struct structPosTrace tmp = my ptrace[it];
		my ptrace[it] = my ptrace[my nptrace - it + 1];
		my ptrace[my nptrace - it + 1] = tmp;
		my ptrace[my nptrace - it + 1].t = duration - my ptrace[my nptrace - it + 1].t;
		my ptrace[it].t = duration - my ptrace[it].t;
	}
	if (my nptrace % 2 == 1) my ptrace[nptrace_2 + 1].t = duration - my ptrace[nptrace_2 + 1].t;
}

void VowelGenerator_shiftTrajectory (VowelGenerator me, double f1_st, double f2_st)
{
	double octaves_x = NUMlog2(my f2max  / my f2min);
	double octaves_y = NUMlog2(my f1max  / my f1min);
	double dy = - f1_st / 12 / octaves_y;
	double dx = - f2_st / 12 / octaves_x;
	for (long i = 1; i <= my nptrace; i++)
	{
		my ptrace[i].x += dx;
		if (my ptrace[i].x < 0) my ptrace[i].x = 0;
		if (my ptrace[i].x > 1) my ptrace[i].x = 1;
		my ptrace[i].y += dy;
		if (my ptrace[i].y < 0) my ptrace[i].y = 0;
		if (my ptrace[i].y > 1) my ptrace[i].y = 1;
	}
}

void VowelGenerator_modifyTrajectoryDuration (VowelGenerator me, double duration)
{
	if (duration != my ptrace[my nptrace].t)
	{
		double factor = duration / my ptrace[my nptrace].t;
		for (long it = 1; it <= my nptrace; it++)
		{
			my ptrace[it].t *= factor;
		}
	}
}

static void drawArrowAtEnd (VowelGenerator me)
{
	long it = 1;
	double gas = Graphics_inqArrowSize (my g), arrowSize = 1;
	// size = 10.0 * my arrowSize * my resolution (my g) / 75.0 (From Graphics: arrowHead)
	// Our window is (0,1)x(0,1)
	double size = 10.0 * arrowSize * Graphics_getResolution (my g) / 75.0 / my width, size2 = size * size;
	Graphics_setArrowSize (my g, arrowSize);
	
	while (it < (my nptrace -1))
	{ 
		double dx = my ptrace[my nptrace].x - my ptrace[my nptrace - it].x;
		double dy = my ptrace[my nptrace].y - my ptrace[my nptrace - it].y;
		double d2 = dx * dx + dy * dy;
		if (d2 > size2) break;
		it++;
	}
	Graphics_arrow (my g, my ptrace[my nptrace-it].x, my ptrace[my nptrace-it].y, my ptrace[my nptrace].x, my ptrace[my nptrace].y);
	Graphics_setArrowSize (my g, gas);	
}

void VowelGenerator_drawTrajectory (VowelGenerator me)
{
	int it, imark = 1, glt = Graphics_inqLineType (my g);
	double glw = Graphics_inqLineWidth (my g), x1, x2, y1, y2, t1, t2;
	int colour = Graphics_inqColour (my g);

	Graphics_setLineType (my g, Graphics_DRAWN);
	// Too short too hear ?
	x1 = my ptrace[1].x; y1 = my ptrace[1].y; t1 = my ptrace[1].t;
	if ((my ptrace[my nptrace].t - t1) < 0.005) Graphics_setColour (my g, Graphics_RED);
	for (it = 2; it <= my nptrace; it++)
	{
		double tm, markLength = 0.01;
		x2 = my ptrace[it].x; y2 = my ptrace[it].y; t2 = my ptrace[it].t;
		Graphics_setLineWidth (my g, 3);
		Graphics_line (my g, x1, y1, x2, y2);
		while (my markTraceEvery > 0 && (tm = imark * my markTraceEvery) < t2)
		{
			// line orthogonal to y = (y1/x1)*x is y = -(x1/y1)*x
			double fraction = (tm - t1) / (t2 - t1);
			double dx = x2 - x1, dy = y2 - y1;
			double xm = x1 + fraction * dx, ym = y1 + fraction * dy;
			double xl1 = dy * markLength / sqrt (dx * dx + dy * dy), xl2 = - xl1;
			double yl1 = dx * markLength / sqrt (dx * dx + dy * dy), yl2 = - yl1;
		
			if (dx * dy > 0)
			{
				xl1 = -fabs (xl1); yl1 = fabs(yl1);
				xl2 = fabs (xl1); yl2 = -fabs(yl1);
			}
			else if (dx * dy < 0)
			{
				xl1 = -fabs (xl1); yl1 = -fabs(yl1);
				xl2 = fabs (xl1); yl2 = fabs(yl1);
			}
			Graphics_setLineWidth (my g, 1);
			Graphics_line (my g, xm + xl1, ym + yl1, xm + xl2, ym + yl2);
		
			imark++;
		}
		x1 = x2; y1 = y2; t1 = t2;
	}
	drawArrowAtEnd (me);
	Graphics_setColour (my g, colour);
	Graphics_setLineType (my g, glt);
	Graphics_setLineWidth (my g, glw);
}

FormantTier VowelGenerator_to_FormantTier (VowelGenerator me)
{
	FormantTier thee;
	
	if (my nptrace <= 0) return NULL;
	
	thee = FormantTier_create (0, my ptrace[my nptrace].t);
	if (thee == NULL) return NULL;
	for (long itime = 1; itime <= my nptrace; itime++)
	{
		double f1, f2;
		FormantPoint point = FormantPoint_create (my ptrace[itime].t);
		if (point == NULL) goto end;
		VowelGenerator_getFrequenciesFromPostion (me, my ptrace[itime].x, my ptrace[itime].y, &f1, &f2);
		point -> formant[0] = f1;
		point -> bandwidth[0] = f1 / 10;
		point -> formant[1] = f2;
		point -> bandwidth[1] = f2 / 10;
		point -> numberOfFormants = 2;
		
		if (! Collection_addItem (thy points, point)) goto end;
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

PitchTier VowelGenerator_to_PitchTier (VowelGenerator me)
{
//	double t_end = my ptrace[my nptrace].t;
	double t_end = my maximumDuration;
	double f0_end = my f0.start * pow (2, my f0.slopeOctPerSec * t_end);
	PitchTier thee = PitchTier_create (0, t_end);
	
	if (thee == NULL) return NULL;
	if (! RealTier_addPoint (thee, 0, my f0.start)) goto end;
	if (my f0.slopeOctPerSec < 0)
	{
		if (f0_end < my f0.minimum)
		{
			t_end = log2 (my f0.minimum / my f0.start) / my f0.slopeOctPerSec;
			f0_end = my f0.minimum;
		}
	}
	else if (my f0.slopeOctPerSec > 0)
	{
		if (f0_end > my f0.maximum)
		{
			t_end = log2 (my f0.maximum / my f0.start) / my f0.slopeOctPerSec;
			f0_end = my f0.maximum;
		}
	}
	RealTier_addPoint (thee, t_end, f0_end);
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

static struct {
	double f1min, f1max, f2min, f2max;
	int frequencyScale;
	int axisOrientation;
	int speakerType;
} prefs;

static struct structF0 f0default = { 140.0, 0.0, 50.0, 2000.0, SAMPLING_FREQUENCY, 1, 0.0, 2000 };
struct structGrid griddefault = { 200, 500, 0, 1, 0, 1, 0.5 };
 
void VowelGenerator_prefs (void)
{
	Preferences_addDouble (L"VowelGenerator.f1min", &prefs.f1min, 200);
	Preferences_addDouble (L"VowelGenerator.f1max", &prefs.f1max, 1200);
	Preferences_addDouble (L"VowelGenerator.f2min", &prefs.f2min, 500);
	Preferences_addDouble (L"VowelGenerator.f2max", &prefs.f2max, 3500);
	Preferences_addInt (L"VowelGenerator.frequencyScale", &prefs.frequencyScale, 0);
	Preferences_addInt (L"VowelGenerator.axisOrientation", &prefs.axisOrientation, 0);
	Preferences_addInt (L"VowelGenerator.speakerType", &prefs.speakerType, 1);
}

void VowelGenerator_getPostionFromFrequencies (VowelGenerator me, double f1, double f2, double *x, double *y)
{
	*x = log (f2 / my f2max) / log (my f2min / my f2max);
	*y = log (f1 / my f1max) / log (my f1min / my f1max); 
}

//Graphics_DCtoWC ????
void VowelGenerator_getFrequenciesFromPostion (VowelGenerator me, double x, double y, double *f1, double *f2)
{
	*f2 = my f2min * pow (my f2max / my f2min, 1 - x);
	*f1 = my f1min * pow (my f1max / my f1min, 1 - y);
}

int VowelGenerator_setMarks (VowelGenerator me, int dataset, int speakerType)
{
	TableOfReal thee = NULL;
	
	if (dataset == 1) // American-English
	{
		wchar_t *labels[4] = { L"", L"m", L"w", L"c" };
		Table me = Table_createFromPetersonBarneyData ();
		if (me == NULL) return 0;
		Table te = Table_extractRowsWhereColumn_string (me, 1, kMelder_string_EQUAL_TO, labels[speakerType]);
		forget (me);
		me = Table_collapseRows (te, L"IPA", L"", L"F1 F2", L"", L"", L"");
		thee = Table_to_TableOfReal (me, 1);
		forget (me);
		if (thee == NULL) return 0;
	}
	else if (dataset == 2) // Dutch
	{
		TableOfReal me = speakerType == 1 ? TableOfReal_createFromPolsData_50males (0) :
			speakerType == 2 ? TableOfReal_createFromVanNieropData_25females (0) :
			TableOfReal_createFromWeeninkData (3);
		if (me == NULL) return 0;
		thee = TableOfReal_meansByRowLabels (me, 0, 0);
		forget (me);
		if (thee == NULL) return 0;
	}
	if (my marks != NULL) forget (my marks);
	my marks = thee;
	return 1;
}

TableOfReal getDutchVowelAverages (int speakerType)
{
	TableOfReal thee, me;
	me = speakerType == 1 ? TableOfReal_createFromPolsData_50males (0) :
		speakerType == 2 ? TableOfReal_createFromVanNieropData_25females (0) :
		TableOfReal_createFromWeeninkData (3);
	if (me == NULL) return NULL;
	thee = TableOfReal_meansByRowLabels (me, 0, 0);
	forget (me);
	return thee;
}

static void VowelGenerator_drawBackground (VowelGenerator me)
{
	TableOfReal thee = my marks;
	double x1, y1, x2, y2, f1, f2;
	
	Graphics_setWindow (my g, 0, 1, 0, 1);
	Graphics_setLineType (my g, Graphics_DRAWN);
	Graphics_setLineWidth (my g, 2);
	Graphics_rectangle (my g, 0, 1, 0, 1);
	Graphics_setLineWidth (my g, 1);
	Graphics_setGrey (my g, 0.5);
	
	// draw the markers
	if (thee != NULL)
	{
		for (int i = 1; i <= thy numberOfRows; i++)
		{
			f1 = thy data[i][1];
			f2 = thy data[i][2];
			wchar_t *label = thy rowLabels[i];
			if (f1 >= my f1min && f1 <= my f1max && f2 >= my f2min && f2 <= my f2max)
			{
				VowelGenerator_getPostionFromFrequencies (me, f1, f2, &x1, &y1);
				Graphics_text (my g, x1, y1, label);
			}
		}
	}
	// Draw the line F1=F2
	// 
	VowelGenerator_getPostionFromFrequencies (me, my f2min, my f2min, &x1, &y1);
	if (y1 >= 0 && y1 <=1)
	{
		VowelGenerator_getPostionFromFrequencies (me, my f1max, my f1max, &x2, &y2);
		if (x2 >= 0 && x2 <= 1)
		{
			Polygon p = Polygon_create (4);
			p -> x[1] = x1; p -> x[2] = x2;
			p -> y[1] = y1; p -> y[2] = y2;
			p -> x[3] =  1; p -> x[4] = x1;
			p -> y[3] =  0; p -> y[4] = y1;
			Graphics_fillArea (my g, p -> numberOfPoints, & p -> x[1], & p -> y[1]);
			// Polygon_paint does not work because of use of Graphics_setInner.
			forget (p);
			Graphics_line (my g, x1, y1, x2, y2);
		}
	}
	//
	Graphics_text (my g, 1, 1, Melder_double (my f2min));
	// Draw the grid
	if (my grid.df1 < (my f1max - my f1min)) // Horizontal lines
	{
		long iline = (my f1min + my grid.df1) / my grid.df1;
		Graphics_setGrey (my g, 0.5);
		Graphics_setLineType (my g, Graphics_DOTTED);
		while ((f1 = iline * my grid.df1) < my f1max)
		{
			if (f1 > my f1min)
			{
				VowelGenerator_getPostionFromFrequencies (me, f1, my f2min, &x1, &y1);
				VowelGenerator_getPostionFromFrequencies (me, f1, my f2max, &x2, &y2);
				Graphics_line (my g, x1, y1, x2, y2);
			}
			iline++; 
		}
		Graphics_setLineType (my g, Graphics_DRAWN);
		Graphics_setGrey (my g, 0); // black
	}	
	if (my grid.df2 < (my f2max - my f2min))
	{
		long iline = (my f2min + my grid.df2) / my grid.df2;
		Graphics_setGrey (my g, 0.5);
		Graphics_setLineType (my g, Graphics_DOTTED);
		while ((f2 = iline * my grid.df2) < my f2max) // vert line
		{
			if (f2 > my f2min)
			{
				VowelGenerator_getPostionFromFrequencies (me, my f1min, f2, &x1, &y1);
				VowelGenerator_getPostionFromFrequencies (me, my f1max, f2, &x2, &y2);
				Graphics_line (my g, x1, y1, x2, y2);
			}
			iline++; 
		}
		Graphics_setLineType (my g, Graphics_DRAWN);
		Graphics_setGrey (my g, 0); // black
	}
	
	Graphics_setGrey (my g, 0); // black
}

typedef struct
{
	long some_check_value;
	long istart;
	float *z;
} *paVowelData;
/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
 
static int paCallback (const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, I)
{
	iam (paVowelData);
	float *out = (float*) outputBuffer;
	unsigned int i;
	(void) inputBuffer; /* Prevent unused variable warning. */
	(void) timeInfo;
	(void) statusFlags;

	for (i = 0; i < framesPerBuffer; i++)
	{
		*out++ = my z[my istart+i];  /* left */
		*out++ = my z[my istart+i];  /* right */
	}
	my istart += framesPerBuffer;
	return 0;
}

/********** MENU METHODS **********/

static int menu_cb_help (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	(void) me;
	Melder_help (L"VowelGenerator");
	return 1;
}

static int menu_cb_prefs (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Preferences", 0);
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

static int menu_cb_publishSound (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	VowelGenerator_updateTrajectory (me);
	Sound publish = VowelGenerator_createTarget (me);
	if (publish == NULL) return 0;
	if (my publishCallback)	my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_extract_FormantTier (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	VowelGenerator_updateTrajectory (me);
	FormantTier publish = VowelGenerator_to_FormantTier (me);
	if (publish == NULL) return 0;
	if (my publishCallback)	my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_extract_PitchTier (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	VowelGenerator_updateTrajectory (me);
	PitchTier publish = VowelGenerator_to_PitchTier (me);
	if (publish == NULL) return 0;	
	if (my publishCallback)	my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_showOneVowelMark (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Show one vowel mark", 0);
		POSITIVE (L"F1 (Hz)", L"300.0")
		POSITIVE (L"F2 (Hz)", L"600.0")
		WORD (L"Mark", L"u")
	EDITOR_OK
	EDITOR_DO
		double f1 = GET_REAL (L"F1");
		double f2 = GET_REAL (L"F2");
		wchar_t *label = GET_STRING (L"Mark");
		if (f1 >= my f1min && f1 <= my f1max && f2 >= my f2min && f2 <= my f2max)
		{
			long irow = 1;
			if (my marks == NULL)
			{
				my marks = TableOfReal_create (1, 2);
				if (my marks == NULL) return 0;
			}
			else
			{
				irow = my marks -> numberOfRows + 1;
				if (! TableOfReal_insertRow (my marks, irow)) return 0;
			}
			TableOfReal_setRowLabel (my marks, irow, label);
			my marks -> data[irow][1] = f1;
			my marks -> data[irow][2] = f2;
			Graphics_updateWs (my g);
		}
	EDITOR_END
}

static int menu_cb_showVowelMarks (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Show vowel marks", 0);
		OPTIONMENU (L"Data set:", 1)
		OPTION (L"American-English")
		OPTION (L"Dutch")
		OPTION (L"None")
		OPTIONMENU (L"Speaker:", 1)
		OPTION (L"Man")
		OPTION (L"Women")
		OPTION (L"Child")
	EDITOR_OK
	EDITOR_DO
		if (! VowelGenerator_setMarks (me, GET_INTEGER (L"Data set"), GET_INTEGER (L"Speaker"))) return 0;
		Graphics_updateWs (my g);
	EDITOR_END
}

static int menu_cb_setF0 (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Set F0", 0);
		POSITIVE (L"Start F0 (Hz)", L"150.0")
		REAL (L"Slope (oct/s)", L"0.0")
	EDITOR_OK
	EDITOR_DO
		if (! VowelGenerator_setSource (me, GET_REAL (L"Start F0"), GET_REAL (L"Slope"))) return 0;
		GuiText_setString (my f0TextField, Melder_double (my f0.start));
		GuiText_setString (my f0SlopeTextField, Melder_double (my f0.slopeOctPerSec));
	EDITOR_END
}

static int menu_cb_reverseTrajectory (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	
	VowelGenerator_reverseTrajectory (me);
	Graphics_updateWs (my g);
	return 1;
}

static int menu_cb_newTrajectory (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"New Trajectory", 0);
		POSITIVE (L"Start F1 (Hz)", L"700.0")
		POSITIVE (L"Start F2 (Hz)", L"1200.0")
		POSITIVE (L"End F1 (Hz)", L"350.0")
		POSITIVE (L"End F2 (Hz)", L"800.0")
		POSITIVE (L"Duration (s)", L"0.25")
	EDITOR_OK
	EDITOR_DO
		double x, y;
		
		VowelGenerator_getPostionFromFrequencies (me, GET_REAL (L"Start F1"), 
			GET_REAL (L"Start F2"), &x, &y);
		my nptrace = 1;
		my ptrace[my nptrace].t = 0;
		my ptrace[my nptrace].x = x;
		my ptrace[my nptrace].y = y;
		
		VowelGenerator_getPostionFromFrequencies (me, GET_REAL (L"End F1"), 
			GET_REAL (L"End F2"), &x, &y);
		my nptrace = 2;	
		my ptrace[my nptrace].t = GET_REAL (L"Duration");
		my ptrace[my nptrace].x = x;
		my ptrace[my nptrace].y = y;
		GuiText_setString (my durationTextField, Melder_double (GET_REAL (L"Duration")));
		Graphics_updateWs (my g);	
	EDITOR_END
}

static int menu_cb_extendTrajectory (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Extend Trajectory", 0);
		POSITIVE (L"To F1 (Hz)", L"500.0")
		POSITIVE (L"To F2 (Hz)", L"1500.0")
		POSITIVE (L"Extra duration (s)", L"0.1")
	EDITOR_OK
	EDITOR_DO
		double x, y;
		
		VowelGenerator_getPostionFromFrequencies (me, GET_REAL (L"To F1"), 
			GET_REAL (L"To F2"), &x, &y);
		if (my nptrace < my maximumPosTrace)
		{
			my nptrace ++;
			my ptrace[my nptrace].t = my ptrace[my nptrace - 1].t + GET_REAL (L"Extra duration");
			my ptrace[my nptrace].x = x;
			my ptrace[my nptrace].y = y;
		
			GuiText_setString (my durationTextField, Melder_double (my ptrace[my nptrace].t));
			Graphics_updateWs (my g);
		}	
	EDITOR_END
}

static int menu_cb_modifyTrajectoryDuration (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Modify duration", 0);
		POSITIVE (L"New duration (s)", L"0.5")
	EDITOR_OK
	EDITOR_DO
		VowelGenerator_modifyTrajectoryDuration (me, GET_REAL (L"New duration"));
		GuiText_setString (my durationTextField, Melder_double (my ptrace[my nptrace].t));
	EDITOR_END
}

static int menu_cb_shiftTrajectory (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Shift trajectory", 0);
		REAL (L"F1 (semitones)", L"0.5")
		REAL (L"F2 (semitones)", L"0.5")
	EDITOR_OK
	EDITOR_DO
		VowelGenerator_shiftTrajectory (me, GET_REAL (L"F1"), GET_REAL (L"F2"));
		Graphics_updateWs (my g);
	EDITOR_END
}

static int menu_cb_showTrajectoryTimeMarkersEvery (EDITOR_ARGS)
{
	EDITOR_IAM (VowelGenerator);
	EDITOR_FORM (L"Show trajectory time markers every", 0);
		REAL (L"Distance (s)", L"0.05")
	EDITOR_OK
	EDITOR_DO
		my markTraceEvery = GET_REAL (L"Distance");
		if (my markTraceEvery < 0) my markTraceEvery = 0;
		Graphics_updateWs (my g);
	EDITOR_END
}

static void gui_button_cb_play (I, GuiButtonEvent event)
{
	(void) event;
	iam (VowelGenerator);
	forget (my target);
	VowelGenerator_updateTrajectory (me);
	my target = VowelGenerator_createTarget (me);
	Graphics_updateWs (my g);
	if (my target != NULL) Sound_play (my target, 0, 0);
}

static void gui_button_cb_publish (I, GuiButtonEvent event)
{
	(void) event;
	iam (VowelGenerator);
	VowelGenerator_updateTrajectory(me);
	Sound publish = VowelGenerator_createTarget (me);
	if (publish == NULL) return;
	if (my publishCallback) my publishCallback (me, my publishClosure, publish);
}

static void gui_button_cb_reverse (I, GuiButtonEvent event)
{
	(void) event;
	iam (VowelGenerator);
	VowelGenerator_reverseTrajectory (me);
	Graphics_updateWs (my g);
}

/* Motif methods Dit is de eigenlijke tekenroutine: wordt aangeroepen na een expose-event (dat gegenereerd kan worden via Graphics_updateWs (g)) */
static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (VowelGenerator);
	(void) event;
	if (my g == NULL) return;   // Could be the case in the very beginning.
	Graphics_clearWs (my g);
	VowelGenerator_drawBackground (me);
	VowelGenerator_drawTrajectory (me);	
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event)
{
	iam (VowelGenerator);
	(void) me;
	(void) event;
	if (me == NULL || my g == NULL) return;
	my height = GuiObject_getHeight (my drawingArea);
	my width = GuiObject_getWidth (my drawingArea);
	Graphics_setWsViewport (my g, 0, my width , 0, my height);
	Graphics_setWsWindow (my g, 0, my width, 0, my height);
	Graphics_setViewport (my g, 0, my width, 0, my height);
	Graphics_updateWs (my g);
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event)
{
	iam (VowelGenerator);
	(void) event;
	double x, y, xb, yb, t;
	//struct timespec tp, tpb;
	my clicking = 1;
	//(void) clock_gettime(CLOCK_REALTIME, &tpb);
	// Only draw mouse down positions
	Graphics_xorOn (my g, Graphics_BLUE);
	Graphics_getMouseLocation (my g, & x, & y);
	my nptrace = 1;
	my ptrace[my nptrace].t = 0;
	my ptrace[my nptrace].x = x;
	my ptrace[my nptrace].y = y;
	while (Graphics_mouseStillDown (my g) && my nptrace < (my maximumPosTrace - 1))
	{
		xb = x, yb = y;
		//(void) clock_gettime(CLOCK_REALTIME, &tp);
		// Get relative time in seconds from the nano-second clock
		//t = (((double)(tp.tv_sec - tpb.tv_sec))*1000000000 + (tp.tv_nsec - tpb.tv_nsec))/1000000000;
		Graphics_getMouseLocation (my g, & x, & y);
		Graphics_line (my g, xb, yb, x, y);
		(my nptrace)++;
		my ptrace[my nptrace].t = t;
		my ptrace[my nptrace].x = x;
		my ptrace[my nptrace].y = y;
		GuiText_setString (my durationTextField, Melder_double (t));
	}
	//(void) clock_gettime(CLOCK_REALTIME, &tp);
	//t = (((double)(tp.tv_sec - tpb.tv_sec))*1000000000 + (tp.tv_nsec - tpb.tv_nsec))/1000000000;
	// To prevent ultra short clicks we set a minimum of 0.01 s duration
	if (t < MINIMUM_SOUND_DURATION) t = MINIMUM_SOUND_DURATION;
	GuiText_setString (my durationTextField, Melder_double (t));
	(my nptrace)++;
	my ptrace[my nptrace].t = t;
	my ptrace[my nptrace].x = x;
	my ptrace[my nptrace].y = y;
	
	Graphics_xorOff (my g);
	my clicking = 0;
	
	struct structGuiButtonEvent gb_event = { 0 };
	gb_event.button = my drawingArea;
	gui_button_cb_play (me, & gb_event);
}

static void gui_drawingarea_cb_key (I, GuiDrawingAreaKeyEvent event)
{
	iam (VowelGenerator);
	(void) me;
	(void) event;
}

static void cb_publish (Any editor, void *closure, Any publish)
{
	(void) editor;
	(void) closure;
	if (! praat_new1 (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
}

static void updateWidgets (I)
{
	iam (VowelGenerator);
	(void) me;
}

static void destroy (I)
{
	iam (VowelGenerator);
	NUMstructvector_free (PosTrace, my ptrace, 1);
	forget (my g);
	forget (my marks);
	forget (my source);
	forget (my target);
	inherited (VowelGenerator) destroy (me);
}

static void createMenus (I)
{
	iam (VowelGenerator);
	inherited (VowelGenerator) createMenus (me);

	Editor_addCommand (me, L"File", L"Preferences...", 0, menu_cb_prefs);
	Editor_addCommand (me, L"File", L"-- publish data --", 0, NULL);
	Editor_addCommand (me, L"File", L"Publish Sound", 0, menu_cb_publishSound);
	Editor_addCommand (me, L"File", L"Extract formant tier", 0, menu_cb_extract_FormantTier);
	Editor_addCommand (me, L"File", L"Extract pitch tier", 0, menu_cb_extract_PitchTier);
	Editor_addCommand (me, L"File", L"-- script stuff --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Show one vowel mark...", 0, menu_cb_showOneVowelMark);
	Editor_addCommand (me, L"Edit", L"Show vowel marks...", 0, menu_cb_showVowelMarks);
	Editor_addCommand (me, L"Edit", L"-- f0 --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Set F0...", 0, menu_cb_setF0);
	Editor_addCommand (me, L"Edit", L"-- trajectory commands --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Reverse trajectory", 0, menu_cb_reverseTrajectory);
	Editor_addCommand (me, L"Edit", L"Modify trajectory duration...", 0, menu_cb_modifyTrajectoryDuration);
	Editor_addCommand (me, L"Edit", L"New trajectory...", 0, menu_cb_newTrajectory);
	Editor_addCommand (me, L"Edit", L"Extend trajectory...", 0, menu_cb_extendTrajectory);
	Editor_addCommand (me, L"Edit", L"Shift trajectory...", 0, menu_cb_shiftTrajectory);
	Editor_addCommand (me, L"Edit", L"Show trajectory time markers every...", 0, menu_cb_showTrajectoryTimeMarkersEvery);
	Editor_addCommand (me, L"Help", L"VowelGenerator help", '?', menu_cb_help);
}

static void createChildren (I)
{
	iam (VowelGenerator);
	double text_left,text_right, text_width = 100, text_dx = 10;
	Widget form;

	form = XmCreateForm (my dialog, "buttons", NULL, 0);
	XtVaSetValues (form,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
		XmNbottomAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget?? */
		NULL);
		
	my playButton = GuiButton_createShown (form, 10, 100, -MARGIN_BOTTOM +10, -10, L"Play", gui_button_cb_play, me, 0);
	my publishButton = GuiButton_createShown (form, 110, 200, -MARGIN_BOTTOM +10, -10, L"Publish", gui_button_cb_publish, me, 0);
	my reverseButton = GuiButton_createShown (form, 210, 300, -MARGIN_BOTTOM +10, -10, L"Reverse", gui_button_cb_reverse, me, 0);
	
	text_left = 330;
	text_right = text_left + text_width;
	GuiLabel_createShown (form, text_left, text_right, -MARGIN_BOTTOM +10 , -MARGIN_BOTTOM +30, L"Duration:", 0);
	my durationTextField = GuiText_createShown (form, text_left, text_right, -MARGIN_BOTTOM +30, -10, 0);
	text_left = text_right + text_dx;
	text_right = text_left + text_width;
	GuiLabel_createShown (form, text_left, text_right, -MARGIN_BOTTOM +10 , -MARGIN_BOTTOM +30, L"Start F0 (Hz):", 0);
	my f0TextField = GuiText_createShown (form, text_left, text_right, -MARGIN_BOTTOM +30, -10, 0);
	text_left = text_right + text_dx;
	text_right = text_left + text_width ;
	GuiLabel_createShown (form, text_left, text_right + text_dx, -MARGIN_BOTTOM +10 , -MARGIN_BOTTOM +30, L"F0 slope (oct/s):", 0);
	my f0SlopeTextField = GuiText_createShown (form, text_left, text_right, -MARGIN_BOTTOM +30, -10, 0);
	
	//f0TextField, f0SlopeTextField
	//	Do not use a callback from a Text widget. It will get called multiple times during the editing 
	// of the text. Better to have all editing done and then query the widget for its value!
	
	/***** Create drawing area. *****/
	// Approximately square because for our defaults: f1min=200, f1max=1000 and f2min = 500, f2mx = 2500,
	// log distances are equal (log (1000/200) == log (2500/500) ).
	
	my drawingArea = GuiDrawingArea_createShown (form, MARGIN_LEFT, -MARGIN_RIGHT, MARGIN_TOP, -MARGIN_BOTTOM,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, me, 0);
	my height = GuiObject_getHeight (my drawingArea);
	my width = GuiObject_getWidth (my drawingArea);
	
	GuiObject_show (form);
}

static void dataChanged (I)
{
	iam (VowelGenerator);
	(void) me;
}

class_methods (VowelGenerator, Editor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createMenus)
class_methods_end


int VowelGenerator_setSource (VowelGenerator me, double f0, double slope)
{
	PitchTier pt = NULL; Sound thee = NULL;
	
	if (f0 == my f0.start && slope == my f0.slopeOctPerSec && my source != NULL) return 1;
	
	my f0.start = f0;
	my f0.slopeOctPerSec = slope;
	pt = VowelGenerator_to_PitchTier (me);
	if (pt == NULL) return 0;
	thee = PitchTier_to_Sound_pulseTrain (pt, my f0.samplingFrequency, my f0.adaptFactor, my f0.adaptTime, my f0.interpolationDepth, 0);
	if (thee == NULL) goto end;
	
	if (my source != NULL) forget (my source);
	my source = thee;

end:
	forget (pt); 
	return ! Melder_hasError ();
}

void VowelGenerator_updateF0 (VowelGenerator me)
{
	wchar_t *f0Text = GuiText_getString (my f0TextField);
	wchar_t *f0SlopeText = GuiText_getString (my f0SlopeTextField);
	double f0 = Melder_atof (f0Text);
	double slope = Melder_atof (f0SlopeText);
	Melder_free (f0Text);
	Melder_free (f0SlopeText);
	
	(void) VowelGenerator_setSource (me, f0, slope);
}

void VowelGenerator_updateTrajectory (VowelGenerator me)
{
	VowelGenerator_updateTrajectoryDuration (me);
	VowelGenerator_updateF0 (me);
}

void VowelGenerator_updateTrajectoryDuration (VowelGenerator me)
{
	wchar_t *durationText = GuiText_getString (my durationTextField);
	double duration = Melder_atof (durationText);
	Melder_free (durationText);
	VowelGenerator_modifyTrajectoryDuration (me, duration);
}

Sound VowelGenerator_createTarget (VowelGenerator me)
{	
	Sound thee = NULL; int do_klatt = 0;
	
	if (do_klatt)
	{/*
		double t_end = my ptrace[my nptrace].t;
		//Klatt him = Klatt_create (0, t_end);
		if (him == NULL) return NULL;
		PitchTier ptt = VowelGenerator_to_PitchTier (me); // improve
		for (long ip = 1; ip <= ptt -> points ->size; ip++)
		{
			RealPoint pp = ptt -> points -> item[ip];
			if (pp->time <= t_end) RealTier_addPoint (his pt, pp->time, pp->value);
		}
		// Could still be empty if ...
		forget (ptt);
		forget (his ft);
		his ft = VowelGenerator_to_FormantTier (me);
		//thee = Klatt_to_Sound_pulses (him, 0, 0, my f0. samplingFrequency, my f0.adaptFactor, my f0. adaptTime, my f0.interpolationDepth);
		forget (him);
	*/}
	else
	{
	double t_end = my ptrace[my nptrace].t;
	if (t_end <= 0) return NULL; // too short
	FormantTier ft = VowelGenerator_to_FormantTier (me);
	if (ft == NULL) goto end;
	thee = Sound_extractPart (my source, 0, t_end, kSound_windowShape_RECTANGULAR, 1, 0);
	Melder_clearError (); // We don't care if thee is too short;
	if (thee == NULL) goto end;
	Sound_FormantTier_filter_inline (thee, ft);
	Vector_scale (thee, 0.99);
	Sound_fadeIn (thee, 0.005, 1);
	Sound_fadeOut(thee, 0.005);
	forget (ft);
	}
end:
	return thee;
}

VowelGenerator VowelGenerator_create (Widget parent, wchar_t *title, Any data)
{
	VowelGenerator me = new (VowelGenerator);
	
	if (me == NULL || ! Editor_init (me, parent, 20, 40, 650, 650, title, data)) goto end;
	Melder_assert (XtWindow (my drawingArea));
	my g = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setFontSize (my g, 10);
	VowelGenerator_prefs ();
	Editor_setPublishCallback (me, cb_publish, NULL);
	
	my f1min = prefs.f1min;
	my f1max = prefs.f1max;
	my f2min = prefs.f2min;
	my f2max = prefs.f2max;
	my frequencyScale = prefs.frequencyScale;
	my axisOrientation = prefs.axisOrientation;
	my speakerType = prefs.speakerType;
	my marks = getDutchVowelAverages (my speakerType);
	my maximumDuration = BUFFER_SIZE_SEC;
	my maximumPosTrace = my maximumDuration * CLOCKS_PER_SEC;
	my ptrace = NUMstructvector (PosTrace, 1, my maximumPosTrace);
	if (my ptrace == NULL) goto end;
	my ptrace[1].t = 0;
	my ptrace[1].x = 0.5 + 0.001;
	my ptrace[1].y = 0.5;
	my ptrace[2].t = my maximumDuration; // Hack: ecause source signal must be long enough
	my ptrace[2].x = 0.5 - 0.001;
	my ptrace[2].y = 0.5;
	my nptrace = 2;
	my markTraceEvery = 0.05; 
	my f0 = f0default;
	if (! VowelGenerator_setSource (me, my f0.start, my f0.slopeOctPerSec)) goto end;
	my target = Sound_createSimple (1, my maximumDuration, my f0.samplingFrequency);
	if (my ptrace == NULL) goto end;
	GuiText_setString (my f0TextField, Melder_double (my f0.start));
	GuiText_setString (my f0SlopeTextField, Melder_double (my f0.slopeOctPerSec));
	GuiText_setString (my durationTextField, Melder_double (0.2)); // Source has been created
	my grid = griddefault;
	struct structGuiDrawingAreaResizeEvent event = { 0 };
	event.widget = my drawingArea;
	gui_drawingarea_cb_resize (me, & event);
	updateWidgets (me);

end:
	if (Melder_hasError ()) forget (me);
	return me;
}

int VowelGenerator_and_TableOfReal_setMarks (VowelGenerator me, TableOfReal thee)
{
	TableOfReal him;
	if (thy numberOfColumns < 2) return Melder_error ("The table must contain at least two colums with formant frequency values.");
	him = my marks;
	my marks = Data_copy (thee);
	if (my marks == NULL)
	{
		my marks = him;
		return 0;
	}
	forget (him);
	Graphics_updateWs (my g);
	return 1;
}

/* End of file VowelGenerator.c */
