/* ERPWindow.cpp
 *
 * Copyright (C) 2012 Paul Boersma
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

#include "ERPWindow.h"
#include "Preferences.h"

Thing_implement (ERPWindow, SoundEditor, 0);

bool                             structERPWindow :: s_showSelectionViewer;     // overridden
kTimeSoundEditor_scalingStrategy structERPWindow :: s_sound_scalingStrategy;   // overridden
define_preference (ERPWindow, double, sound_scaling_height,   L"20e-6")
define_preference (ERPWindow, double, sound_scaling_minimum, L"-10e-6")
define_preference (ERPWindow, double, sound_scaling_maximum,  L"10e-6")
FunctionEditor_spectrogram       structERPWindow :: s_spectrogram;             // overridden

void structERPWindow :: f_preferences (void) {
	Preferences_addBool   (L"ERPWindow.showSelectionViewer",   & s_showSelectionViewer,   true);    // overridden
	Preferences_addEnum   (L"ERPWindow.sound.scalingStrategy", & s_sound_scalingStrategy, kTimeSoundEditor_scalingStrategy, DEFAULT);   // overridden
	Preferences_addDouble (L"ERPWindow.sound.scaling.height",  & s_sound_scaling_height,  Melder_atof (sdefault_sound_scaling_height));   // overridden
	Preferences_addDouble (L"ERPWindow.sound.scaling.minimum", & s_sound_scaling_minimum, Melder_atof (sdefault_sound_scaling_minimum));   // overridden
	Preferences_addDouble (L"ERPWindow.sound.scaling.maximum", & s_sound_scaling_maximum, Melder_atof (sdefault_sound_scaling_maximum));   // overridden
	Preferences_addBool   (L"ERPWindow.spectrogram.show",               & s_spectrogram.show, false);
	Preferences_addDouble (L"ERPWindow.spectrogram.viewFrom",           & s_spectrogram.viewFrom, 0.0);   // Hz
	Preferences_addDouble (L"ERPWindow.spectrogram.viewTo",             & s_spectrogram.viewTo, 60.0);   // Hz
	Preferences_addDouble (L"ERPWindow.spectrogram.windowLength",       & s_spectrogram.windowLength, 0.5);   // seconds
	Preferences_addDouble (L"ERPWindow.spectrogram.dynamicRange",       & s_spectrogram.dynamicRange, 40.0);   // dB
	Preferences_addLong   (L"ERPWindow.spectrogram.timeSteps",          & s_spectrogram.timeSteps, 1000);
	Preferences_addLong   (L"ERPWindow.spectrogram.frequencySteps",     & s_spectrogram.frequencySteps, 250);
	Preferences_addEnum   (L"ERPWindow.spectrogram.method",             & s_spectrogram.method, kSound_to_Spectrogram_method, DEFAULT);
	Preferences_addEnum   (L"ERPWindow.spectrogram.windowShape",        & s_spectrogram.windowShape, kSound_to_Spectrogram_windowShape, DEFAULT);
	Preferences_addBool   (L"ERPWindow.spectrogram.autoscaling",        & s_spectrogram.autoscaling, true);
	Preferences_addDouble (L"ERPWindow.spectrogram.maximum",            & s_spectrogram.maximum, 100.0);   // dB/Hz
	Preferences_addDouble (L"ERPWindow.spectrogram.preemphasis",        & s_spectrogram.preemphasis, 0.0);   // dB/octave
	Preferences_addDouble (L"ERPWindow.spectrogram.dynamicCompression", & s_spectrogram.dynamicCompression, 0.0);
	Preferences_addBool   (L"ERPWindow.spectrogram.picture.garnish",    & s_spectrogram.picture.garnish, true);
}

typedef struct { int inclination, azimuth; double topX, topY; } BiosemiLocationData;

static BiosemiLocationData biosemiCapCoordinates64 [1+64] =
{
	/*
	 * BioSemi says:
	 *    "Spherical coordinates in degrees,
	 *     by inclination (from Cz, pos is right hemisphere, neg is left hemisphere),
     *     and azimuth (from T7 for left hemisphere, and from T8 for the right hemisphere, pos is anti-clockwise, neg is clockwise)"
	 */
	{0,0},
	{ -92, -72 },   //  1 Fp1
	{ -92, -54 },   //  2 AF7
	{ -74, -65 },   //  3 AF3
	{ -50, -68 },   //  4 F1
	{ -60, -51 },   //  5 F3
	{ -75, -41 },   //  6 F5
	{ -92, -36 },   //  7 F7
	{ -92, -18 },   //  8 FT7
	{ -72, -21 },   //  9 FC5
	{ -50, -28 },   // 10 FC3
	{ -32, -45 },   // 11 FC1
	{ -23,   0 },   // 12 C1
	{ -46,   0 },   // 13 C3
	{ -69,   0 },   // 14 C5
	{ -92,   0 },   // 15 T7
	{ -92,  18 },   // 16 TP7
	{ -72,  21 },   // 17 CP5
	{ -50,  28 },   // 18 CP3
	{ -32,  45 },   // 19 CP1
	{ -50,  68 },   // 20 P1
	{ -60,  51 },   // 21 P3
	{ -75,  41 },   // 22 P5
	{ -92,  36 },   // 23 P7
	{-115,  40 },   // 24 P9
	{ -92,  54 },   // 25 PO7
	{ -74,  65 },   // 26 PO3
	{ -92,  72 },   // 27 O1
	{ 115, -90 },   // 28 Iz
	{  92, -90 },   // 29 Oz
	{  69, -90 },   // 30 POz
	{  46, -90 },   // 31 Pz
	{  23, -90 },   // 32 CPz
	{  92,  90 },   // 33 Fpz
	{  92,  72 },   // 34 Fp2
	{  92,  54 },   // 35 AF8
	{  74,  65 },   // 36 AF4
	{  69,  90 },   // 37 AFz
	{  46,  90 },   // 38 Fz
	{  50,  68 },   // 39 F2
	{  60,  51 },   // 40 F4
	{  75,  41 },   // 41 F6
	{  92,  36 },   // 42 F8
	{  92,  18 },   // 43 FT8
	{  72,  21 },   // 44 FC6
	{  50,  28 },   // 45 FC4
	{  32,  45 },   // 46 FC2
	{  23,  90 },   // 47 FCz
	{   0,   0 },   // 48 Cz
	{  23,   0 },   // 49 C2
	{  46,   0 },   // 50 C4
	{  69,   0 },   // 51 C6
	{  92,   0 },   // 52 T8
	{  92, -18 },   // 53 TP8
	{  72, -21 },   // 54 CP6
	{  50, -28 },   // 55 CP4
	{  32, -45 },   // 56 CP2
	{  50, -68 },   // 57 P2
	{  60, -51 },   // 58 P4
	{  75, -41 },   // 59 P6
	{  92, -36 },   // 60 P8
	{ 115, -40 },   // 61 P10
	{  92, -54 },   // 62 PO8
	{  74, -65 },   // 63 PO4
	{  92, -72 }    // 64 O2
};

static BiosemiLocationData biosemiCapCoordinates32 [1+32] =
{
	/*
	 * BioSemi says:
	 *    "Spherical coordinates in degrees,
	 *     by inclination (from Cz, pos is right hemisphere, neg is left hemisphere),
     *     and azimuth (from T7 for left hemisphere, and from T8 for the right hemisphere, pos is anti-clockwise, neg is clockwise)"
	 */
	{0,0},
	{ -92, -72 },   //  1 Fp1
	{ -74, -65 },   //  2 AF3
	{ -92, -36 },   //  3 F7
	{ -60, -51 },   //  4 F3
	{ -32, -45 },   //  5 FC1
	{ -72, -21 },   //  6 FC5
	{ -92,   0 },   //  7 T7
	{ -46,   0 },   //  8 C3
	{ -32,  45 },   //  9 CP1
	{ -72,  21 },   // 10 CP5
	{ -92,  36 },   // 11 P7
	{ -60,  51 },   // 12 P3
	{  46, -90 },   // 13 Pz
	{ -74,  65 },   // 14 PO3
	{ -92,  72 },   // 15 O1
	{  92, -90 },   // 16 Oz
	{  92, -72 },   // 17 O2
	{  74, -65 },   // 18 PO4
	{  60, -51 },   // 19 P4
	{  92, -36 },   // 20 P8
	{  72, -21 },   // 21 CP6
	{  32, -45 },   // 22 CP2
	{  46,   0 },   // 23 C4
	{  92,   0 },   // 24 T8
	{  72,  21 },   // 25 FC6
	{  32,  45 },   // 26 FC2
	{  60,  51 },   // 27 F4
	{  92,  36 },   // 28 F8
	{  74,  65 },   // 29 AF4
	{  92,  72 },   // 30 Fp2
	{  46,  90 },   // 31 Fz
	{   0,   0 },   // 32 Cz
};

void structERP :: f_drawScalp (Graphics graphics, double tmin, double tmax, double vmin, double vmax, bool garnish) {
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, -1.0, 1.0, -1.0, 1.0);
	//Graphics_setGrey (graphics, 1.0);
	//Graphics_fillRectangle (graphics, -1.1, 1.1, -1.01, 1.19);
	//Graphics_setColour (graphics, Graphics_BLACK);
	long numberOfDrawableChannels =
			this -> ny >= 64 && Melder_wcsequ (this -> d_channelNames [64], L"O2") ? 64 :
			this -> ny >= 32 && Melder_wcsequ (this -> d_channelNames [32], L"Cz") ? 32 :
			0;
	BiosemiLocationData *biosemiLocationData = numberOfDrawableChannels == 64 ? biosemiCapCoordinates64 : numberOfDrawableChannels == 32 ? biosemiCapCoordinates32 : 0;
	for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
		double inclination = (double) biosemiLocationData [ichan]. inclination;
		double azimuth = (double) biosemiLocationData [ichan]. azimuth;
		bool rightHemisphere = inclination >= 0.0;
		double r = fabs (inclination / 115.0);
		double theta = rightHemisphere ? azimuth * (NUMpi / 180.0) : (azimuth + 180.0) * (NUMpi / 180.0);
		biosemiLocationData [ichan]. topX = r * cos (theta);
		biosemiLocationData [ichan]. topY = r * sin (theta);
	}
	long n = 201;
	double d = 2.0 / (n - 1);
	autoNUMvector <double> mean (1, numberOfDrawableChannels);
	for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
		mean [ichan] = tmin == tmax ?
				Sampled_getValueAtX (this, tmin, ichan, 0, true) :
				Vector_getMean (this, tmin, tmax, ichan);
	}
	autoNUMmatrix <double> image (1, n, 1, n);
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y <= 1.0) {
				double value = NUMundefined, sum = 0.0, weight = 0.0;
				for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
					double dx = x - biosemiLocationData [ichan]. topX;
					double dy = y - biosemiLocationData [ichan]. topY;
					double distance = sqrt (dx * dx + dy * dy);
					if (distance < 1e-12) {
						value = mean [ichan];
						break;
					}
					distance = distance * distance * distance * distance * distance * distance;
					sum += mean [ichan] / distance;
					weight += 1.0 / distance;
				}
				if (value == NUMundefined)
					value = ( sum == 0.0 ? 0.0 : sum / weight );
				image [irow] [icol] = value;
			}
		}
	}
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y > 1.0) {
				image [irow] [icol] = vmin;
			}
		}
	}
	Graphics_image (graphics, image.peek(), 1, n, -1.0-0.5/n, 1.0+0.5/n, 1, n, -1.0-0.5/n, 1.0+0.5/n, vmin, vmax);
	Graphics_setLineWidth (graphics, 2.0);
	/*
	 * Nose.
	 */
	Graphics_setGrey (graphics, 0.5);
	{// scope
		double x [3] = { -0.08, 0.0, 0.08 }, y [3] = { 0.99, 1.18, 0.99 };
		Graphics_fillArea (graphics, 3, x, y);
	}
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_line (graphics, -0.08, 0.99, 0.0, 1.18);
	Graphics_line (graphics, 0.08, 0.99, 0.0, 1.18);
	/*
	 * Ears.
	 */
	Graphics_setGrey (graphics, 0.5);
	Graphics_fillRectangle (graphics, -1.09, -1.00, -0.08, 0.08);
	Graphics_fillRectangle (graphics, 1.09, 1.00, -0.08, 0.08);
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_line (graphics, -0.99, 0.08, -1.09, 0.08);
	Graphics_line (graphics, -1.09, 0.08, -1.09, -0.08);
	Graphics_line (graphics, -1.09, -0.08, -0.99, -0.08);
	Graphics_line (graphics, 0.99, 0.08, 1.09, 0.08);
	Graphics_line (graphics, 1.09, 0.08, 1.09, -0.08);
	Graphics_line (graphics, 1.09, -0.08, 0.99, -0.08);
	/*
	 * Scalp.
	 */
	Graphics_ellipse (graphics, -1.0, 1.0, -1.0, 1.0);
	Graphics_setLineWidth (graphics, 1.0);
	Graphics_unsetInner (graphics);
	if (garnish) {
		autoNUMmatrix <double> legend (1, n, 1, 2);
		for (long irow = 1; irow <= n; irow ++) {
			for (long icol = 1; icol <= 2; icol ++) {
				legend [irow] [icol] = (irow - 1) / (n - 1.0);
			}
		}
		Graphics_image (graphics, legend.peek(), 1, 2, 0.78, 0.98, 1, n, -0.8, +0.8, 0.0, 1.0);
		Graphics_rectangle (graphics, 0.78, 0.98, -0.8, +0.8);
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_TOP);
		Graphics_text2 (graphics, 1.0, -0.8, Melder_double (vmin * 1e6), L" \\muV");
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_BOTTOM);
		Graphics_text2 (graphics, 1.0, +0.8, Melder_double (vmax * 1e6), L" \\muV");
	}
}

void structERPWindow :: v_drawSelectionViewer () {
	ERP erp = (ERP) data;
	Graphics_setWindow (d_graphics, -1.1, 1.1, -1.01, 1.19);
	Graphics_setGrey (d_graphics, 0.85);
	Graphics_fillRectangle (d_graphics, -1.1, 1.1, -1.01, 1.19);
	Graphics_setColour (d_graphics, Graphics_BLACK);
	long numberOfDrawableChannels =
			erp -> ny >= 64 && Melder_wcsequ (erp -> d_channelNames [64], L"O2") ? 64 :
			erp -> ny >= 32 && Melder_wcsequ (erp -> d_channelNames [32], L"Cz") ? 32 :
			0;
	BiosemiLocationData *biosemiLocationData = numberOfDrawableChannels == 64 ? biosemiCapCoordinates64 : numberOfDrawableChannels == 32 ? biosemiCapCoordinates32 : 0;
	for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
		double inclination = (double) biosemiLocationData [ichan]. inclination;
		double azimuth = (double) biosemiLocationData [ichan]. azimuth;
		bool rightHemisphere = inclination >= 0.0;
		double r = fabs (inclination / 115.0);
		double theta = rightHemisphere ? azimuth * (NUMpi / 180.0) : (azimuth + 180.0) * (NUMpi / 180.0);
		biosemiLocationData [ichan]. topX = r * cos (theta);
		biosemiLocationData [ichan]. topY = r * sin (theta);
	}
	long n = 201;
	double d = 2.0 / (n - 1);
	autoNUMvector <double> mean (1, numberOfDrawableChannels);
	for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
		mean [ichan] =
			d_startSelection == d_endSelection ?
				Sampled_getValueAtX (erp, d_startSelection, ichan, 0, true) :
				Vector_getMean (erp, d_startSelection, d_endSelection, ichan);
	}
	autoNUMmatrix <double> image (1, n, 1, n);
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y <= 1.0) {
				double value = NUMundefined, sum = 0.0, weight = 0.0;
				for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
					double dx = x - biosemiLocationData [ichan]. topX;
					double dy = y - biosemiLocationData [ichan]. topY;
					double distance = sqrt (dx * dx + dy * dy);
					if (distance < 1e-12) {
						value = mean [ichan];
						break;
					}
					distance = distance * distance * distance * distance * distance * distance;
					sum += mean [ichan] / distance;
					weight += 1.0 / distance;
				}
				if (value == NUMundefined)
					value = ( sum == 0.0 ? 0.0 : sum / weight );
				image [irow] [icol] = value;
			}
		}
	}
	double minimum = 0.0, maximum = 0.0;
	for (long irow = 1; irow <= n; irow ++) {
		for (long icol = 1; icol <= n; icol ++) {
			double value = image [irow] [icol];
			if (value < minimum) minimum = value;
			else if (value > maximum) maximum = value;
		}
	}
	double absoluteExtremum = - minimum > maximum ? - minimum : maximum;
	if (d_sound.scalingStrategy == kTimeSoundEditor_scalingStrategy_FIXED_RANGE) {
		minimum = d_sound.scaling_minimum;
		maximum = d_sound.scaling_maximum;
	} else if (d_sound.scalingStrategy == kTimeSoundEditor_scalingStrategy_FIXED_HEIGHT) {
		double mean = 0.5 * (minimum + maximum);
		minimum = mean - 0.5 * d_sound.scaling_height;
		maximum = mean + 0.5 * d_sound.scaling_height;
	} else {
		minimum = - absoluteExtremum;
		maximum = absoluteExtremum;
	}
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y > 1.0) {
				image [irow] [icol] = minimum + 0.1875 * (maximum - minimum);   // -0.625 * absoluteExtremum;
			}
		}
	}
	Graphics_image (d_graphics, image.peek(), 1, n, -1.0-0.5/n, 1.0+0.5/n, 1, n, -1.0-0.5/n, 1.0+0.5/n, minimum, maximum);
	Graphics_setLineWidth (d_graphics, 2.0);
	/*
	 * Nose.
	 */
	Graphics_setGrey (d_graphics, 0.5);
	{// scope
		double x [3] = { -0.08, 0.0, 0.08 }, y [3] = { 0.99, 1.18, 0.99 };
		Graphics_fillArea (d_graphics, 3, x, y);
	}
	Graphics_setColour (d_graphics, Graphics_BLACK);
	Graphics_line (d_graphics, -0.08, 0.99, 0.0, 1.18);
	Graphics_line (d_graphics, 0.08, 0.99, 0.0, 1.18);
	/*
	 * Ears.
	 */
	Graphics_setGrey (d_graphics, 0.5);
	Graphics_fillRectangle (d_graphics, -1.09, -1.00, -0.08, 0.08);
	Graphics_fillRectangle (d_graphics, 1.09, 1.00, -0.08, 0.08);
	Graphics_setColour (d_graphics, Graphics_BLACK);
	Graphics_line (d_graphics, -0.99, 0.08, -1.09, 0.08);
	Graphics_line (d_graphics, -1.09, 0.08, -1.09, -0.08);
	Graphics_line (d_graphics, -1.09, -0.08, -0.99, -0.08);
	Graphics_line (d_graphics, 0.99, 0.08, 1.09, 0.08);
	Graphics_line (d_graphics, 1.09, 0.08, 1.09, -0.08);
	Graphics_line (d_graphics, 1.09, -0.08, 0.99, -0.08);
	/*
	 * Scalp.
	 */
	Graphics_ellipse (d_graphics, -1.0, 1.0, -1.0, 1.0);
	Graphics_setLineWidth (d_graphics, 1.0);
}

ERPWindow ERPWindow_create (const wchar_t *title, ERP data) {
	Melder_assert (data != NULL);
	try {
		autoERPWindow me = Thing_new (ERPWindow);
		me -> structSoundEditor :: f_init (title, data);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("ERP window not created.");
	}
}

/* End of file ERPWindow.cpp */
