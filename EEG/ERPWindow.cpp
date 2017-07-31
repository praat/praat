/* ERPWindow.cpp
 *
 * Copyright (C) 2012,2013,2014,2015,2016,2017 Paul Boersma
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

#include "ERPWindow.h"
#include "EditorM.h"
#include "Preferences.h"

Thing_implement (ERPWindow, SoundEditor, 0);

#include "prefs_define.h"
#include "ERPWindow_prefs.h"
#include "prefs_install.h"
#include "ERPWindow_prefs.h"
#include "prefs_copyToInstance.h"
#include "ERPWindow_prefs.h"

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

void ERP_drawScalp_garnish (Graphics graphics, double vmin, double vmax, enum kGraphics_colourScale colourScale) {
	long n = 201;
	autoNUMmatrix <double> legend (1, n, 1, 2);
	for (long irow = 1; irow <= n; irow ++) {
		for (long icol = 1; icol <= 2; icol ++) {
			legend [irow] [icol] = (irow - 1) / (n - 1.0);
		}
	}
	Graphics_setColourScale (graphics, colourScale);
	Graphics_image (graphics, legend.peek(), 1, 2, 0.85, 0.98, 1, n, -0.8, +0.8, 0.0, 1.0);
	Graphics_setColourScale (graphics, kGraphics_colourScale_GREY);
	Graphics_rectangle (graphics, 0.85, 0.98, -0.8, +0.8);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_text (graphics, 1.0, -0.8,   vmin * 1e6, U" μV");
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_BOTTOM);
	Graphics_text (graphics, 1.0, +0.8,   vmax * 1e6, U" μV");
}

void ERP_drawScalp (ERP me, Graphics graphics, double tmin, double tmax, double vmin, double vmax, enum kGraphics_colourScale colourScale, bool garnish) {
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, -1.0, 1.0, -1.0, 1.0);
	//Graphics_setGrey (graphics, 1.0);
	//Graphics_fillRectangle (graphics, -1.1, 1.1, -1.01, 1.19);
	//Graphics_setColour (graphics, Graphics_BLACK);
	long numberOfDrawableChannels =
			my ny >= 64 && Melder_equ (my channelNames [64], U"O2") ? 64 :
			my ny >= 32 && Melder_equ (my channelNames [32], U"Cz") ? 32 :
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
				Sampled_getValueAtX (me, tmin, ichan, 0, true) :
				Vector_getMean (me, tmin, tmax, ichan);
	}
	autoNUMmatrix <double> image (1, n, 1, n);
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y <= 1.0) {
				double value = undefined, sum = 0.0, weight = 0.0;
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
				if (isundef (value))
					value = ( sum == 0.0 ? 0.0 : sum / weight );
				image [irow] [icol] = value;
			}
		}
	}
	double whiteValue = colourScale == kGraphics_colourScale_BLUE_TO_RED ? 0.5 * (vmin + vmax) : vmin;
	Graphics_setColourScale (graphics, colourScale);
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y > 1.0) {
				image [irow] [icol] = whiteValue;
			}
		}
	}
	Graphics_image (graphics, image.peek(), 1, n, -1.0-0.5/n, 1.0+0.5/n, 1, n, -1.0-0.5/n, 1.0+0.5/n, vmin, vmax);
	Graphics_setColourScale (graphics, kGraphics_colourScale_GREY);
	Graphics_setLineWidth (graphics, 2.0);
	/*
	 * Nose.
	 */
	Graphics_setGrey (graphics, colourScale == kGraphics_colourScale_BLUE_TO_RED ? 1.0 : 0.5);
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
	Graphics_setGrey (graphics, colourScale == kGraphics_colourScale_BLUE_TO_RED ? 1.0 : 0.5);
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
		ERP_drawScalp_garnish (graphics, vmin, vmax, colourScale);
	}
}

void structERPWindow :: v_drawSelectionViewer () {
	ERP erp = (ERP) our data;
	Graphics_setWindow (our graphics.get(), -1.1, 1.1, -1.01, 1.19);
	Graphics_setColour (our graphics.get(), Graphics_WINDOW_BACKGROUND_COLOUR);
	Graphics_fillRectangle (our graphics.get(), -1.1, 1.1, -1.01, 1.19);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	long numberOfDrawableChannels =
			erp -> ny >= 64 && Melder_equ (erp -> channelNames [64], U"O2") ? 64 :
			erp -> ny >= 32 && Melder_equ (erp -> channelNames [32], U"Cz") ? 32 :
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
	autoNUMvector <double> means (1, numberOfDrawableChannels);
	for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
		means [ichan] =
			our startSelection == our endSelection ?
				Sampled_getValueAtX (erp, our startSelection, ichan, 0, true) :
				Vector_getMean (erp, our startSelection, our endSelection, ichan);
	}
	autoNUMmatrix <double> image (1, n, 1, n);
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y <= 1.0) {
				double value = undefined, sum = 0.0, weight = 0.0;
				for (long ichan = 1; ichan <= numberOfDrawableChannels; ichan ++) {
					double dx = x - biosemiLocationData [ichan]. topX;
					double dy = y - biosemiLocationData [ichan]. topY;
					double distance = sqrt (dx * dx + dy * dy);
					if (distance < 1e-12) {
						value = means [ichan];
						break;
					}
					distance = distance * distance * distance * distance * distance * distance;
					sum += means [ichan] / distance;
					weight += 1.0 / distance;
				}
				if (isundef (value))
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
	if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_FIXED_RANGE) {
		minimum = p_sound_scaling_minimum;
		maximum = p_sound_scaling_maximum;
	} else if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_FIXED_HEIGHT) {
		double mean = 0.5 * (minimum + maximum);
		minimum = mean - 0.5 * p_sound_scaling_height;
		maximum = mean + 0.5 * p_sound_scaling_height;
	} else {
		minimum = - absoluteExtremum;
		maximum = absoluteExtremum;
	}
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y > 1.0) {
				image [irow] [icol] = minimum +
					( our p_scalp_colourScale == kGraphics_colourScale_BLUE_TO_RED ? 0.46 : 0.1875 ) * (maximum - minimum);
					   // -0.625 * absoluteExtremum;
			}
		}
	}
	Graphics_setColourScale (our graphics.get(), our p_scalp_colourScale);
	Graphics_image (our graphics.get(), image.peek(), 1, n, -1.0-0.5/n, 1.0+0.5/n, 1, n, -1.0-0.5/n, 1.0+0.5/n, minimum, maximum);
	Graphics_setColourScale (our graphics.get(), kGraphics_colourScale_GREY);
	Graphics_setLineWidth (our graphics.get(), 2.0);
	/*
	 * Nose.
	 */
	Graphics_setGrey (our graphics.get(), our p_scalp_colourScale == kGraphics_colourScale_BLUE_TO_RED ? 1.0 : 0.5);
	{// scope
		double x [3] = { -0.08, 0.0, 0.08 }, y [3] = { 0.99, 1.18, 0.99 };
		Graphics_fillArea (our graphics.get(), 3, x, y);
	}
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	Graphics_line (our graphics.get(), -0.08, 0.99, 0.0, 1.18);
	Graphics_line (our graphics.get(), 0.08, 0.99, 0.0, 1.18);
	/*
	 * Ears.
	 */
	Graphics_setGrey (our graphics.get(), our p_scalp_colourScale == kGraphics_colourScale_BLUE_TO_RED ? 1.0 : 0.5);
	Graphics_fillRectangle (our graphics.get(), -1.09, -1.00, -0.08, 0.08);
	Graphics_fillRectangle (our graphics.get(), 1.09, 1.00, -0.08, 0.08);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	Graphics_line (our graphics.get(), -0.99, 0.08, -1.09, 0.08);
	Graphics_line (our graphics.get(), -1.09, 0.08, -1.09, -0.08);
	Graphics_line (our graphics.get(), -1.09, -0.08, -0.99, -0.08);
	Graphics_line (our graphics.get(), 0.99, 0.08, 1.09, 0.08);
	Graphics_line (our graphics.get(), 1.09, 0.08, 1.09, -0.08);
	Graphics_line (our graphics.get(), 1.09, -0.08, 0.99, -0.08);
	/*
	 * Scalp.
	 */
	Graphics_ellipse (our graphics.get(), -1.0, 1.0, -1.0, 1.0);
	Graphics_setLineWidth (our graphics.get(), 1.0);
}

void structERPWindow :: v_prefs_addFields (EditorCommand cmd) {
	UiField radio;
	OPTIONMENU_ENUM (U"Scalp colour space", kGraphics_colourScale, kGraphics_colourScale_BLUE_TO_RED)
}
void structERPWindow :: v_prefs_setValues (EditorCommand cmd) {
	SET_ENUM (U"Scalp colour space", kGraphics_colourScale, p_scalp_colourScale)
}
void structERPWindow :: v_prefs_getValues (EditorCommand cmd) {
	pref_scalp_colourScale () = p_scalp_colourScale = GET_ENUM (kGraphics_colourScale, U"Scalp colour space");
	FunctionEditor_redraw (this);
}

autoERPWindow ERPWindow_create (const char32 *title, ERP data) {
	Melder_assert (data);
	try {
		autoERPWindow me = Thing_new (ERPWindow);
		SoundEditor_init (me.get(), title, data);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ERP window not created.");
	}
}

/* End of file ERPWindow.cpp */
