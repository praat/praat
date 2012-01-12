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

Thing_implement (ERPWindow, SoundEditor, 0);

static struct { int inclination, azimuth; double topX, topY; } biosemiCapCoordinates [1+64] =
{
	/*
	 * BioSemi says:
	 *    "Spherical coordinates in degrees,
	 *     by inclination (from Cz, pos is right hemisphere, neg is left hemisphere),
     *     and azimuth (from T7 for left hemisphere, and from T8 for the right hemisphere, pos is anti-clockwise, neg is clockwise)"
	 */
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

void structERPWindow :: v_drawSelectionViewer () {
	ERP erp = (ERP) data;
	long sampleNumber = Sampled_xToNearestIndex (erp, startSelection);
	Graphics_setWindow (graphics, -1.1, 1.1, -1.01, 1.19);
	Graphics_setGrey (graphics, 0.85);
	Graphics_fillRectangle (graphics, -1.1, 1.1, -1.01, 1.19);
	Graphics_setColour (graphics, Graphics_BLACK);
	for (long ichan = 1; ichan <= 64; ichan ++) {
		double inclination = (double) biosemiCapCoordinates [ichan]. inclination;
		double azimuth = (double) biosemiCapCoordinates [ichan]. azimuth;
		bool rightHemisphere = inclination >= 0.0;
		double r = fabs (inclination / 115.0);
		double theta = rightHemisphere ? azimuth * (NUMpi / 180.0) : (azimuth + 180.0) * (NUMpi / 180.0);
		biosemiCapCoordinates [ichan]. topX = r * cos (theta);
		biosemiCapCoordinates [ichan]. topY = r * sin (theta);
	}
	long n = 201;
	double d = 2.0 / (n - 1);
	autoNUMmatrix <double> image (1, n, 1, n);
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y <= 1.0) {
				double value = NUMundefined, sum = 0.0, weight = 0.0;
				for (long ichan = 1; ichan <= 64; ichan ++) {
					double dx = x - biosemiCapCoordinates [ichan]. topX;
					double dy = y - biosemiCapCoordinates [ichan]. topY;
					double distance = sqrt (dx * dx + dy * dy);
					if (distance < 1e-12) {
						value = erp -> z [ichan] [sampleNumber];
						break;
					}
					distance = distance * distance * distance;
					sum += erp -> z [ichan] [sampleNumber] / distance;
					weight += 1.0 / distance;
				}
				if (value == NUMundefined) value = sum / weight;
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
	for (long irow = 1; irow <= n; irow ++) {
		double y = -1.0 + (irow - 1) * d;
		for (long icol = 1; icol <= n; icol ++) {
			double x = -1.0 + (icol - 1) * d;
			if (x * x + y * y > 1.0) {
				image [irow] [icol] = -0.63 * absoluteExtremum;
			}
		}
	}
	Graphics_image (graphics, image.peek(), 1, n, -1.0-0.5/n, 1.0+0.5/n, 1, n, -1.0-0.5/n, 1.0+0.5/n, - absoluteExtremum, absoluteExtremum);
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
}

ERPWindow ERPWindow_create (GuiObject parent, const wchar *title, ERP data) {
	Melder_assert (data != NULL);
	try {
		autoERPWindow me = Thing_new (ERPWindow);
		me -> structSoundEditor :: f_init (parent, title, data);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("ERP window not created.");
	}
}

/* End of file ERPWindow.cpp */
