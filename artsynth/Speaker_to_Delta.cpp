/* Speaker_to_Delta.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Art_Speaker.h"
#include "Speaker_to_Delta.h"
#define SMOOTH_LUNGS  TRUE
#define FIRST_TUBE  7

Delta Speaker_to_Delta (Speaker me) {
	double f = my relativeSize * 1e-3;   /* We shall use millimetres and grams. */
	double xe [30], ye [30], xi [30], yi [30], xmm [30], ymm [30], dx, dy;
	int closed [40];
	int itube;
	autoDelta thee = Delta_create (89);
	Melder_assert (my cord.numberOfMasses == 1 || my cord.numberOfMasses == 2 || my cord.numberOfMasses == 10);

	/* Lungs: tubes 1..23. */

	for (itube = 1; itube <= 23; itube ++) {
		Delta_Tube t = thy tube + itube;
		t -> Dx = t -> Dxeq = 10 * f;
		t -> Dy = t -> Dyeq = 100 * f;
		t -> Dz = t -> Dzeq = 230 * f;
		t -> mass = 10 * my relativeSize * t -> Dx * t -> Dz;   /* 80 * f; 35 * Dx * Dz */
		t -> k1 = 200;   /* 90000 * Dx * Dz; Newtons per metre */
		t -> k3 = 0.0;
		t -> Brel = 0.8;
		t -> parallel = 1000;
	}

	/* Bronchi: tubes 24..29. */

	for (itube = 24; itube <= 29; itube ++) {
		Delta_Tube t = thy tube + itube;
		t -> Dx = t -> Dxeq = 10 * f;
		t -> Dy = t -> Dyeq = 15 * f;
		t -> Dz = t -> Dzeq = 30 * f;
		t -> mass = 10 * f;
		t -> k1 = 40;   /* 125000 * Dx * Dz; Newtons per metre */
		t -> k3 = 0.0;
		t -> Brel = 0.8;
	}

	/* Trachea: tubes 30..35; four of these may be replaced by conus elasticus (see below). */

	for (itube = 30; itube <= 35; itube ++) {
		Delta_Tube t = thy tube + itube;
		t -> Dx = t -> Dxeq = 10 * f;
		t -> Dy = t -> Dyeq = 15 * f;
		t -> Dz = t -> Dzeq = 16 * f;
		t -> mass = 5 * f;
		t -> k1 = 160;   /* 100000 * Dx * Dz; Newtons per metre */
		t -> k3 = 0.0;
		t -> Brel = 0.8;
	}

	if (SMOOTH_LUNGS) {
		struct { int itube; double Dy, Dz, parallel; } data [] = {
			{  7, 120, 240, 5000 }, {  8, 120, 240, 5000 }, {  9, 120, 240, 5000 },
			{ 10, 120, 240, 5000 }, { 11, 120, 240, 5000 }, { 12, 120, 240, 5000 },
			{ 13, 120, 240, 2500 }, { 14, 120, 240, 1250 }, { 15, 120, 240,  640 },
			{ 16, 120, 240,  320 }, { 17, 120, 240,  160 }, { 18, 120, 140,   80 },
			{ 19,  70,  70,   40 }, { 20,  35,  35,   20 }, { 21,  18,  18,   10 },
			{ 22,  12,  12,    5 }, { 23,  12,  12,    3 }, { 24,  18,   9,    2 },
			{ 25,  18,  19,    2 }, { 0 } };
		int i;
		for (i = 0; data [i]. itube; i ++) {
			Delta_Tube t = thy tube + data [i]. itube;
			t -> Dy = t -> Dyeq = data [i]. Dy * f;
			t -> Dz = t -> Dzeq = data [i]. Dz * f;
			t -> parallel = data [i]. parallel;
		}
		for (itube = 26; itube <= 35; itube ++) {
			Delta_Tube t = thy tube + itube;
			t -> Dy = t -> Dyeq = 11 * f;
			t -> Dz = t -> Dzeq = 14 * f;
			t -> parallel = 1;
		}
		for (itube = FIRST_TUBE; itube <= 18; itube ++) {
			Delta_Tube t = thy tube + itube;
			t -> Dx = t -> Dxeq = 10 * f;
			t -> mass = 10 * my relativeSize * t -> Dx * t -> Dz;   /* 10 mm */
			t -> k1 = 1e5 * t -> Dx * t -> Dz;   /* elastic tissue: 1 mbar/mm */
			t -> k3 = 0.0;
			t -> Brel = 1;
		}
		for (itube = 19; itube <= 35; itube ++) {
			Delta_Tube t = thy tube + itube;
			t -> Dx = t -> Dxeq = 10 * f;
			t -> mass = 3 * my relativeSize * t -> Dx * t -> Dz;   /* 3 mm */
			t -> k1 = 10e5 * t -> Dx * t -> Dz;   /* cartilage: 10 mbar/mm */
			t -> k3 = 0.0;
			t -> Brel = 1;
		}
	}

	/* Glottis: tubes 36 and 37; the last one may be disconnected (see below). */
	{
		Delta_Tube t = thy tube + 36;
		t -> Dx = t -> Dxeq = my lowerCord.thickness;
		t -> Dy = t -> Dyeq = 0.0;
		t -> Dz = t -> Dzeq = my cord.length;
		t -> mass = my lowerCord.mass;
		t -> k1 = my lowerCord.k1;
		t -> k3 = t -> k1 * (20 / t -> Dz) * (20 / t -> Dz);
		t -> Brel = 0.2;
	}

	/*
	 * Fill in the values for the upper part of the glottis (tube 37) only if there is no one-mass model.
	 */
	if (my cord.numberOfMasses >= 2) {
		Delta_Tube t = thy tube + 37;
		t -> Dx = t -> Dxeq = my upperCord.thickness;
		t -> Dy = t -> Dyeq = 0.0;
		t -> Dz = t -> Dzeq = my cord.length;
		t -> mass = my upperCord.mass;
		t -> k1 = my upperCord.k1;
		t -> k3 = t -> k1 * (20 / t -> Dz) * (20 / t -> Dz);
		t -> Brel = 0.2;

		/* Couple spring with lower cord. */
		t -> k1left1 = thy tube [36]. k1right1 = 1.0;
	}

	/*
	 * Fill in the values for the conus elasticus (tubes 79..86) only if we want to model it.
	 */
	if (my cord.numberOfMasses == 10) {
		thy tube [79]. Dx = thy tube [79]. Dxeq = 8 * f;
		thy tube [80]. Dx = thy tube [80]. Dxeq = 7 * f;
		thy tube [81]. Dx = thy tube [81]. Dxeq = 6 * f;
		thy tube [82]. Dx = thy tube [82]. Dxeq = 5 * f;
		thy tube [83]. Dx = thy tube [83]. Dxeq = 4 * f;
		thy tube [84]. Dx = thy tube [84]. Dxeq = 0.75 * 4 * f + 0.25 * my lowerCord.thickness;
		thy tube [85]. Dx = thy tube [85]. Dxeq = 0.50 * 4 * f + 0.50 * my lowerCord.thickness;
		thy tube [86]. Dx = thy tube [86]. Dxeq = 0.25 * 4 * f + 0.75 * my lowerCord.thickness;

		thy tube [79]. Dy = thy tube [79]. Dyeq = 11 * f;
		thy tube [80]. Dy = thy tube [80]. Dyeq = 7 * f;
		thy tube [81]. Dy = thy tube [81]. Dyeq = 4 * f;
		thy tube [82]. Dy = thy tube [82]. Dyeq = 2 * f;
		thy tube [83]. Dy = thy tube [83]. Dyeq = 1 * f;
		thy tube [84]. Dy = thy tube [84]. Dyeq = 0.75 * f;
		thy tube [85]. Dy = thy tube [85]. Dyeq = 0.50 * f;
		thy tube [86]. Dy = thy tube [86]. Dyeq = 0.25 * f;

		thy tube [79]. Dz = thy tube [79]. Dzeq = 16 * f;
		thy tube [80]. Dz = thy tube [80]. Dzeq = 16 * f;
		thy tube [81]. Dz = thy tube [81]. Dzeq = 16 * f;
		thy tube [82]. Dz = thy tube [82]. Dzeq = 16 * f;
		thy tube [83]. Dz = thy tube [83]. Dzeq = 16 * f;
		thy tube [84]. Dz = thy tube [84]. Dzeq = 0.75 * 16 * f + 0.25 * my cord.length;
		thy tube [85]. Dz = thy tube [85]. Dzeq = 0.50 * 16 * f + 0.50 * my cord.length;
		thy tube [86]. Dz = thy tube [86]. Dzeq = 0.25 * 16 * f + 0.75 * my cord.length;

		thy tube [79]. k1 = 160;
		thy tube [80]. k1 = 160;
		thy tube [81]. k1 = 160;
		thy tube [82]. k1 = 160;
		thy tube [83]. k1 = 160;
		thy tube [84]. k1 = 0.75 * 160 * f + 0.25 * my lowerCord.k1;
		thy tube [85]. k1 = 0.50 * 160 * f + 0.50 * my lowerCord.k1;
		thy tube [86]. k1 = 0.25 * 160 * f + 0.75 * my lowerCord.k1;

		thy tube [79]. Brel = 0.7;
		thy tube [80]. Brel = 0.6;
		thy tube [81]. Brel = 0.5;
		thy tube [82]. Brel = 0.4;
		thy tube [83]. Brel = 0.3;
		thy tube [84]. Brel = 0.2;
		thy tube [85]. Brel = 0.2;
		thy tube [86]. Brel = 0.2;

		for (itube = 79; itube <= 86; itube ++) {
			Delta_Tube t = thy tube + itube;
			t -> mass = t -> Dx * t -> Dz / (30 * f);
			t -> k3 = t -> k1 * (20 / t -> Dz) * (20 / t -> Dz);
			t -> k1left1 = t -> k1right1 = 1.0;
		}
		thy tube [79]. k1left1 = 0.0;
		thy tube [36]. k1left1 = 1.0;   /* The essence: couple spring with lower vocal cords. */
	}

	/*
	 * Fill in the values of the glottal shunt only if we want to model it.
	 */
	if (my shunt.Dx != 0.0) {
		for (itube = 87; itube <= 89; itube ++) {
			Delta_Tube t = thy tube + itube;
			t -> Dx = t -> Dxeq = my shunt.Dx;
			t -> Dy = t -> Dyeq = my shunt.Dy;
			t -> Dz = t -> Dzeq = my shunt.Dz;
			t -> mass = 3 * my upperCord.mass;   /* Heavy... */
			t -> k1 = 3 * my upperCord.k1;   /* ...and stiff... */
			t -> k3 = t -> k1 * (20 / t -> Dz) * (20 / t -> Dz);
			t -> Brel = 3.0;   /* ...and inelastic, so that the walls will not vibrate. */
		}
	}

	/* Vocal tract from neutral articulation. */
	{
		autoArt art = Art_create ();
		Art_Speaker_meshVocalTract (art.peek(), me, xi, yi, xe, ye, xmm, ymm, closed);
	}

	/* Pharynx and mouth: tubes 38..64. */

	for (itube = 38; itube <= 64; itube ++) {
		Delta_Tube t = thy tube + itube;
		int i = itube - 37;
		t -> Dx = t -> Dxeq = sqrt (( dx = xmm [i] - xmm [i + 1], dx * dx ) + ( dy = ymm [i] - ymm [i + 1], dy * dy ));
		t -> Dyeq = sqrt (( dx = xe [i] - xi [i], dx * dx ) + ( dy = ye [i] - yi [i], dy * dy ));
		if (closed [i]) t -> Dyeq = - t -> Dyeq;
		t -> Dy = t -> Dyeq;
		t -> Dz = t -> Dzeq = 0.015;
		t -> mass = 0.006;
		t -> k1 = 30;
		t -> k3 = 0.0;
		t -> Brel = 1.0;
	}
  /* For tongue-tip vibration [r]:  thy tube [60]. Brel = 0.1; thy tube [60]. k1 = 3; */

	/* Nose: tubes 65..78. */

	for (itube = 65; itube <= 78; itube ++) {
		Delta_Tube t = thy tube + itube;
		t -> Dx = t -> Dxeq = my nose.Dx;
		t -> Dy = t -> Dyeq = my nose.weq [itube - 65];
		t -> Dz = t -> Dzeq = my nose.Dz;
		t -> mass = 0.006;
		t -> k1 = 100;
		t -> k3 = 0.0;
		t -> Brel = 1.0;
	}
	thy tube [65]. Dy = thy tube [65]. Dyeq = 0.0;   /* Override: nasopharyngeal port closed. */

	/* The default structure:
	 * every tube is connected on the left to the previous tube (index one lower).
	 * This corresponds to a two-mass model of the vocal cords without shunt.
	 */
	for (itube = SMOOTH_LUNGS ? FIRST_TUBE : 1; itube <= thy numberOfTubes; itube ++) {
		Delta_Tube t = thy tube + itube;
		t -> s1 = 5e6 * t -> Dx * t -> Dz;
		t -> s3 = t -> s1 / (0.9e-3 * 0.9e-3);
		t -> dy = 1e-5;
		t -> left1 = t - 1;   /* Connect to the previous tube on the left. */
		t -> right1 = t + 1;   /* Connect to the next tube on the right. */
	}

	/***** Connections: boundaries and interfaces. *****/

	/* The leftmost boundary: the diaphragm (tube 1).
	 * Disconnect on the left.
	 */
	thy tube [SMOOTH_LUNGS ? FIRST_TUBE : 1]. left1 = NULL;   /* Closed at diaphragm. */

	/* Optional one-mass model of the vocal cords.
	 * Short-circuit over tube 37 (upper glottis).
	 */
	if (my cord.numberOfMasses == 1) {

		/* Connect the right side of tube 36 to the left side of tube 38. */
		thy tube [36]. right1 = thy tube + 38;
		thy tube [38]. left1 = thy tube + 36;

		/* Disconnect tube 37 on both sides. */
		thy tube [37]. left1 = thy tube [37]. right1 = NULL;
	}

	/* Optionally couple vocal cords with conus elasticus.
	 * Replace tubes 32..35 (upper trachea) by tubes 79..86 (conus elasticus).
	 */
	if (my cord.numberOfMasses == 10) {

		/* Connect the right side of tube 31 to the left side of tube 79. */
		thy tube [31]. right1 = thy tube + 79;
		thy tube [79]. left1 = thy tube + 31;

		/* Connect the right side of tube 86 to the left side of tube 36. */
		thy tube [86]. right1 = thy tube + 36;
		thy tube [36]. left1 = thy tube + 86;

		/* Disconnect tubes 32..35 on both sides. */
		thy tube [32]. left1 = thy tube [32]. right1 = NULL;
		thy tube [33]. left1 = thy tube [33]. right1 = NULL;
		thy tube [34]. left1 = thy tube [34]. right1 = NULL;
		thy tube [35]. left1 = thy tube [35]. right1 = NULL;
	} else {

		/* Disconnect tubes 79..86 on both sides. */
		for (itube = 79; itube <= 86; itube ++)
			thy tube [itube]. left1 = thy tube [itube]. right1 = NULL;
	}

	/* Optionally add a shunt parallel to the glottis.
	 * Create a side branch from tube 34/35 (or 85/86) to tube 38/39 with tubes 87..89.
	 */
	if (my shunt.Dx != 0.0) {
		int topOfTrachea = my cord.numberOfMasses == 10 ? 86 : 35;

		/* Create a three-way interface below the shunt.
		 * Connect lowest shunt tube (87) with top of trachea (34/35 or 85/86).
		 */
		thy tube [topOfTrachea - 1]. right2 = thy tube + 87;   /* Trachea to shunt. */
		thy tube [87]. left1 = thy tube + topOfTrachea - 1;   /* Shunt to trachea. */
		thy tube [87]. Dxeq = thy tube [topOfTrachea - 1]. Dxeq = thy tube [topOfTrachea]. Dxeq;   /* Equal length. */
		thy tube [87]. Dx = thy tube [topOfTrachea - 1]. Dx = thy tube [topOfTrachea]. Dx;

		/* Create a three-way interface above the shunt.
		 * Connect highest shunt tube (89) with bottom of pharynx (38/39).
		 */
		thy tube [89]. right1 = thy tube + 39;   /* Shunt to pharynx. */
		thy tube [39]. left2 = thy tube + 89;   /* Pharynx to shunt. */
		thy tube [89]. Dxeq = thy tube [39]. Dxeq = thy tube [38]. Dxeq;   /* All three of equal length. */
		thy tube [89]. Dx = thy tube [39]. Dx = thy tube [38]. Dx;
	} else {

		/* Disconnect tubes 87..89 on both sides. */
		for (itube = 87; itube <= 89; itube ++)
			thy tube [itube]. left1 = thy tube [itube]. right1 = NULL;
	}

	/* Create a three-way interface at the nasopharyngeal port.
	 * Connect tubes 50 (pharynx), 51 (mouth), and 65 (nose).
	 */
	thy tube [50]. right2 = thy tube + 65;   /* Pharynx to nose. */
	thy tube [65]. left1 = thy tube + 50;   /* Nose to pharynx. */
	thy tube [65]. Dxeq = thy tube [51]. Dxeq = thy tube [50]. Dxeq;   /* All three must be of equal length. */
	thy tube [65]. Dx = thy tube [51]. Dx = thy tube [50]. Dx;

	/* The rightmost boundaries: the lips (tube 64) and the nostrils (tube 78).
	 * Disconnect on the right.
	 */
	thy tube [64]. right1 = NULL;   /* Radiation at the lips. */
	thy tube [78]. right1 = NULL;   /* Radiation at the nostrils. */

	for (itube = 1; itube <= thy numberOfTubes; itube ++) {
		Delta_Tube t = thy tube + itube;
		Melder_assert (! t->left1 || t->left1->right1 == t || t->left1->right2 == t);
		Melder_assert (! t->left2 || t->left2->right1 == t);
		Melder_assert (! t->right1 || t->right1->left1 == t || t->right1->left2 == t);
		Melder_assert (! t->right2 || t->right2->left1 == t);
	}
	return thee.transfer();
}

/* End of file Speaker_to_Delta.cpp */
