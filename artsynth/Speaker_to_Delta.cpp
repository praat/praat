/* Speaker_to_Delta.cpp
 *
 * Copyright (C) 1992-2005,2006,2011,2015-2019 Paul Boersma
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

#include "Art_Speaker.h"
#include "Speaker_to_Delta.h"
#define SMOOTH_LUNGS  true
#define FIRST_TUBE  7

autoDelta Speaker_to_Delta (Speaker me) {
	double f = my relativeSize * 1e-3;   // we shall use millimetres and grams
	double xe [30], ye [30], xi [30], yi [30], xmm [30], ymm [30], dx, dy;
	autoDelta thee = Delta_create (89);
	Melder_assert (my cord.numberOfMasses == 1 || my cord.numberOfMasses == 2 || my cord.numberOfMasses == 10);

	/* Lungs: tubes 1..23. */

	for (integer itube = 1; itube <= 23; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		t -> Dx = t -> Dxeq = 10.0 * f;
		t -> Dy = t -> Dyeq = 100.0 * f;
		t -> Dz = t -> Dzeq = 230.0 * f;
		t -> mass = 10.0 * my relativeSize * t -> Dx * t -> Dz;   // 80 * f; 35 * Dx * Dz
		t -> k1 = 200.0;   // 90000 * Dx * Dz; Newtons per metre
		t -> k3 = 0.0;
		t -> Brel = 0.8;
		t -> parallel = 1000;
	}

	/* Bronchi: tubes 24..29. */

	for (integer itube = 24; itube <= 29; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		t -> Dx = t -> Dxeq = 10.0 * f;
		t -> Dy = t -> Dyeq = 15.0 * f;
		t -> Dz = t -> Dzeq = 30.0 * f;
		t -> mass = 10.0 * f;
		t -> k1 = 40.0;   // 125000 * Dx * Dz; Newtons per metre
		t -> k3 = 0.0;
		t -> Brel = 0.8;
	}

	/* Trachea: tubes 30..35; four of these may be replaced by conus elasticus (see below). */

	for (integer itube = 30; itube <= 35; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		t -> Dx = t -> Dxeq = 10.0 * f;
		t -> Dy = t -> Dyeq = 15.0 * f;
		t -> Dz = t -> Dzeq = 16.0 * f;
		t -> mass = 5.0 * f;
		t -> k1 = 160.0;   // 100000 * Dx * Dz; Newtons per metre
		t -> k3 = 0.0;
		t -> Brel = 0.8;
	}

	if (SMOOTH_LUNGS) {
		struct { integer itube; double Dy, Dz, parallel; } data [] = {
			{  7, 120.0, 240.0, 5000.0 }, {  8, 120.0, 240.0, 5000.0 }, {  9, 120.0, 240.0, 5000.0 },
			{ 10, 120.0, 240.0, 5000.0 }, { 11, 120.0, 240.0, 5000.0 }, { 12, 120.0, 240.0, 5000.0 },
			{ 13, 120.0, 240.0, 2500.0 }, { 14, 120.0, 240.0, 1250.0 }, { 15, 120.0, 240.0,  640.0 },
			{ 16, 120.0, 240.0,  320.0 }, { 17, 120.0, 240.0,  160.0 }, { 18, 120.0, 140.0,   80.0 },
			{ 19,  70.0,  70.0,   40.0 }, { 20,  35.0,  35.0,   20.0 }, { 21,  18.0,  18.0,   10.0 },
			{ 22,  12.0,  12.0,    5.0 }, { 23,  12.0,  12.0,    3.0 }, { 24,  18.0,   9.0,    2.0 },
			{ 25,  18.0,  19.0,    2.0 }, { } };
		for (integer i = 0; data [i]. itube; i ++) {
			Delta_Tube t = & thy tubes [data [i]. itube];
			t -> Dy = t -> Dyeq = data [i]. Dy * f;
			t -> Dz = t -> Dzeq = data [i]. Dz * f;
			t -> parallel = data [i]. parallel;
		}
		for (integer itube = 26; itube <= 35; itube ++) {
			Delta_Tube t = & thy tubes [itube];
			t -> Dy = t -> Dyeq = 11.0 * f;
			t -> Dz = t -> Dzeq = 14.0 * f;
			t -> parallel = 1;
		}
		for (integer itube = FIRST_TUBE; itube <= 18; itube ++) {
			Delta_Tube t = & thy tubes [itube];
			t -> Dx = t -> Dxeq = 10.0 * f;
			t -> mass = 10.0 * my relativeSize * t -> Dx * t -> Dz;   // 10 mm
			t -> k1 = 1e5 * t -> Dx * t -> Dz;   // elastic tissue: 1 mbar/mm
			t -> k3 = 0.0;
			t -> Brel = 1.0;
		}
		for (integer itube = 19; itube <= 35; itube ++) {
			Delta_Tube t = & thy tubes [itube];
			t -> Dx = t -> Dxeq = 10.0 * f;
			t -> mass = 3.0 * my relativeSize * t -> Dx * t -> Dz;   // 3 mm
			t -> k1 = 10e5 * t -> Dx * t -> Dz;   // cartilage: 10 mbar/mm
			t -> k3 = 0.0;
			t -> Brel = 1.0;
		}
	}

	/* Glottis: tubes 36 and 37; the last one may be disconnected (see below). */
	{
		Delta_Tube t = & thy tubes [36];
		t -> Dx = t -> Dxeq = my lowerCord.thickness;
		t -> Dy = t -> Dyeq = 0.0;
		t -> Dz = t -> Dzeq = my cord.length;
		t -> mass = my lowerCord.mass;
		t -> k1 = my lowerCord.k1;
		t -> k3 = t -> k1 * (20.0 / t -> Dz) * (20.0 / t -> Dz);
		t -> Brel = 0.2;
	}

	/*
	 * Fill in the values for the upper part of the glottis (tube 37) only if there is no one-mass model.
	 */
	if (my cord.numberOfMasses >= 2) {
		Delta_Tube t = & thy tubes [37];
		t -> Dx = t -> Dxeq = my upperCord.thickness;
		t -> Dy = t -> Dyeq = 0.0;
		t -> Dz = t -> Dzeq = my cord.length;
		t -> mass = my upperCord.mass;
		t -> k1 = my upperCord.k1;
		t -> k3 = t -> k1 * (20.0 / t -> Dz) * (20.0 / t -> Dz);
		t -> Brel = 0.2;

		/* Couple spring with lower cord. */
		t -> k1left1 = thy tubes [36]. k1right1 = 1.0;
	}

	/*
	 * Fill in the values for the conus elasticus (tubes 79..86) only if we want to model it.
	 */
	if (my cord.numberOfMasses == 10) {
		thy tubes [79]. Dx = thy tubes [79]. Dxeq = 8.0 * f;
		thy tubes [80]. Dx = thy tubes [80]. Dxeq = 7.0 * f;
		thy tubes [81]. Dx = thy tubes [81]. Dxeq = 6.0 * f;
		thy tubes [82]. Dx = thy tubes [82]. Dxeq = 5.0 * f;
		thy tubes [83]. Dx = thy tubes [83]. Dxeq = 4.0 * f;
		thy tubes [84]. Dx = thy tubes [84]. Dxeq = 0.75 * 4.0 * f + 0.25 * my lowerCord.thickness;
		thy tubes [85]. Dx = thy tubes [85]. Dxeq = 0.50 * 4.0 * f + 0.50 * my lowerCord.thickness;
		thy tubes [86]. Dx = thy tubes [86]. Dxeq = 0.25 * 4.0 * f + 0.75 * my lowerCord.thickness;

		thy tubes [79]. Dy = thy tubes [79]. Dyeq = 11.0 * f;
		thy tubes [80]. Dy = thy tubes [80]. Dyeq = 7.0 * f;
		thy tubes [81]. Dy = thy tubes [81]. Dyeq = 4.0 * f;
		thy tubes [82]. Dy = thy tubes [82]. Dyeq = 2.0 * f;
		thy tubes [83]. Dy = thy tubes [83]. Dyeq = 1.0 * f;
		thy tubes [84]. Dy = thy tubes [84]. Dyeq = 0.75 * f;
		thy tubes [85]. Dy = thy tubes [85]. Dyeq = 0.50 * f;
		thy tubes [86]. Dy = thy tubes [86]. Dyeq = 0.25 * f;

		thy tubes [79]. Dz = thy tubes [79]. Dzeq = 16.0 * f;
		thy tubes [80]. Dz = thy tubes [80]. Dzeq = 16.0 * f;
		thy tubes [81]. Dz = thy tubes [81]. Dzeq = 16.0 * f;
		thy tubes [82]. Dz = thy tubes [82]. Dzeq = 16.0 * f;
		thy tubes [83]. Dz = thy tubes [83]. Dzeq = 16.0 * f;
		thy tubes [84]. Dz = thy tubes [84]. Dzeq = 0.75 * 16.0 * f + 0.25 * my cord.length;
		thy tubes [85]. Dz = thy tubes [85]. Dzeq = 0.50 * 16.0 * f + 0.50 * my cord.length;
		thy tubes [86]. Dz = thy tubes [86]. Dzeq = 0.25 * 16.0 * f + 0.75 * my cord.length;

		thy tubes [79]. k1 = 160.0;
		thy tubes [80]. k1 = 160.0;
		thy tubes [81]. k1 = 160.0;
		thy tubes [82]. k1 = 160.0;
		thy tubes [83]. k1 = 160.0;
		thy tubes [84]. k1 = 0.75 * 160.0 * f + 0.25 * my lowerCord.k1;
		thy tubes [85]. k1 = 0.50 * 160.0 * f + 0.50 * my lowerCord.k1;
		thy tubes [86]. k1 = 0.25 * 160.0 * f + 0.75 * my lowerCord.k1;

		thy tubes [79]. Brel = 0.7;
		thy tubes [80]. Brel = 0.6;
		thy tubes [81]. Brel = 0.5;
		thy tubes [82]. Brel = 0.4;
		thy tubes [83]. Brel = 0.3;
		thy tubes [84]. Brel = 0.2;
		thy tubes [85]. Brel = 0.2;
		thy tubes [86]. Brel = 0.2;

		for (integer itube = 79; itube <= 86; itube ++) {
			Delta_Tube t = & thy tubes [itube];
			t -> mass = t -> Dx * t -> Dz / (30.0 * f);
			t -> k3 = t -> k1 * (20.0 / t -> Dz) * (20.0 / t -> Dz);
			t -> k1left1 = t -> k1right1 = 1.0;
		}
		thy tubes [79]. k1left1 = 0.0;
		thy tubes [36]. k1left1 = 1.0;   // the essence: couple spring with lower vocal cords
	}

	/*
	 * Fill in the values of the glottal shunt only if we want to model it.
	 */
	if (my shunt.Dx != 0.0) {
		for (integer itube = 87; itube <= 89; itube ++) {
			Delta_Tube t = & thy tubes [itube];
			t -> Dx = t -> Dxeq = my shunt.Dx;
			t -> Dy = t -> Dyeq = my shunt.Dy;
			t -> Dz = t -> Dzeq = my shunt.Dz;
			t -> mass = 3.0 * my upperCord.mass;   // heavy...
			t -> k1 = 3.0 * my upperCord.k1;   // ...and stiff...
			t -> k3 = t -> k1 * (20.0 / t -> Dz) * (20.0 / t -> Dz);
			t -> Brel = 3.0;   // ...and inelastic, so that the walls will not vibrate
		}
	}

	/* Vocal tract from neutral articulation. */
	bool closed [40];
	{
		autoArt art = Art_create ();
		Art_Speaker_meshVocalTract (art.get(), me, xi, yi, xe, ye, xmm, ymm, closed);
	}

	/* Pharynx and mouth: tubes 38..64. */

	for (integer itube = 38; itube <= 64; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		integer i = itube - 37;
		dx = xmm [i] - xmm [i + 1];
		dy = ymm [i] - ymm [i + 1];
		t -> Dx = t -> Dxeq = sqrt (dx * dx + dy * dy);
		dx = xe [i] - xi [i];
		dy = ye [i] - yi [i];
		t -> Dyeq = sqrt (dx * dx + dy * dy);
		if (closed [i]) t -> Dyeq = - t -> Dyeq;
		t -> Dy = t -> Dyeq;
		t -> Dz = t -> Dzeq = 0.015;
		t -> mass = 0.006;
		t -> k1 = 30.0;
		t -> k3 = 0.0;
		t -> Brel = 1.0;
	}
  /* For tongue-tip vibration [r]:  thy tube [60]. Brel = 0.1; thy tube [60]. k1 = 3; */

	/* Nose: tubes 65..78. */

	for (integer itube = 65; itube <= 78; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		t -> Dx = t -> Dxeq = my nose.Dx;
		t -> Dy = t -> Dyeq = my nose.weq [itube - 64];
		t -> Dz = t -> Dzeq = my nose.Dz;
		t -> mass = 0.006;
		t -> k1 = 100.0;
		t -> k3 = 0.0;
		t -> Brel = 1.0;
	}
	thy tubes [65]. Dy = thy tubes [65]. Dyeq = 0.0;   // override: nasopharyngeal port closed

	/* The default structure:
	 * every tube is connected on the left to the previous tube (index one lower).
	 * This corresponds to a two-mass model of the vocal cords without shunt.
	 */
	for (integer itube = SMOOTH_LUNGS ? FIRST_TUBE : 1; itube <= thy numberOfTubes; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		t -> s1 = 5e6 * t -> Dx * t -> Dz;
		t -> s3 = t -> s1 / (0.9e-3 * 0.9e-3);
		t -> dy = 1e-5;
		t -> left1 = t - 1;   // connect to the previous tube on the left
		t -> right1 = t + 1;   // connect to the next tube on the right
	}

	/***** Connections: boundaries and interfaces. *****/

	/* The leftmost boundary: the diaphragm (tube 1).
	 * Disconnect on the left.
	 */
	thy tubes [SMOOTH_LUNGS ? FIRST_TUBE : 1]. left1 = nullptr;   // closed at diaphragm

	/* Optional one-mass model of the vocal cords.
	 * Short-circuit over tube 37 (upper glottis).
	 */
	if (my cord.numberOfMasses == 1) {

		/* Connect the right side of tube 36 to the left side of tube 38. */
		thy tubes [36]. right1 = & thy tubes [38];
		thy tubes [38]. left1 = & thy tubes [36];

		/* Disconnect tube 37 on both sides. */
		thy tubes [37]. left1 = thy tubes [37]. right1 = nullptr;
	}

	/* Optionally couple vocal cords with conus elasticus.
	 * Replace tubes 32..35 (upper trachea) by tubes 79..86 (conus elasticus).
	 */
	if (my cord.numberOfMasses == 10) {

		/* Connect the right side of tube 31 to the left side of tube 79. */
		thy tubes [31]. right1 = & thy tubes [79];
		thy tubes [79]. left1 = & thy tubes [31];

		/* Connect the right side of tube 86 to the left side of tube 36. */
		thy tubes [86]. right1 = & thy tubes [36];
		thy tubes [36]. left1 = & thy tubes [86];

		/* Disconnect tubes 32..35 on both sides. */
		thy tubes [32]. left1 = thy tubes [32]. right1 = nullptr;
		thy tubes [33]. left1 = thy tubes [33]. right1 = nullptr;
		thy tubes [34]. left1 = thy tubes [34]. right1 = nullptr;
		thy tubes [35]. left1 = thy tubes [35]. right1 = nullptr;
	} else {

		/* Disconnect tubes 79..86 on both sides. */
		for (integer itube = 79; itube <= 86; itube ++)
			thy tubes [itube]. left1 = thy tubes [itube]. right1 = nullptr;
	}

	/* Optionally add a shunt parallel to the glottis.
	 * Create a side branch from tube 34/35 (or 85/86) to tube 38/39 with tubes 87..89.
	 */
	if (my shunt.Dx != 0.0) {
		integer topOfTrachea = ( my cord.numberOfMasses == 10 ? 86 : 35 );

		/* Create a three-way interface below the shunt.
		 * Connect lowest shunt tube (87) with top of trachea (34/35 or 85/86).
		 */
		thy tubes [topOfTrachea - 1]. right2 = & thy tubes [87];   // trachea to shunt
		thy tubes [87]. left1 = & thy tubes [topOfTrachea - 1];   // shunt to trachea
		thy tubes [87]. Dxeq = thy tubes [topOfTrachea - 1]. Dxeq = thy tubes [topOfTrachea]. Dxeq;   // equal length
		thy tubes [87]. Dx = thy tubes [topOfTrachea - 1]. Dx = thy tubes [topOfTrachea]. Dx;

		/* Create a three-way interface above the shunt.
		 * Connect highest shunt tube (89) with bottom of pharynx (38/39).
		 */
		thy tubes [89]. right1 = & thy tubes [39];   // shunt to pharynx
		thy tubes [39]. left2 = & thy tubes [89];   // pharynx to shunt
		thy tubes [89]. Dxeq = thy tubes [39]. Dxeq = thy tubes [38]. Dxeq;   // all three of equal length
		thy tubes [89]. Dx = thy tubes [39]. Dx = thy tubes [38]. Dx;
	} else {

		/* Disconnect tubes 87..89 on both sides. */
		for (integer itube = 87; itube <= 89; itube ++)
			thy tubes [itube]. left1 = thy tubes [itube]. right1 = nullptr;
	}

	/* Create a three-way interface at the nasopharyngeal port.
	 * Connect tubes 50 (pharynx), 51 (mouth), and 65 (nose).
	 */
	thy tubes [50]. right2 = & thy tubes [65];   // pharynx to nose
	thy tubes [65]. left1 = & thy tubes [50];   // nose to pharynx
	thy tubes [65]. Dxeq = thy tubes [51]. Dxeq = thy tubes [50]. Dxeq;   // all three must be of equal length
	thy tubes [65]. Dx = thy tubes [51]. Dx = thy tubes [50]. Dx;

	/* The rightmost boundaries: the lips (tube 64) and the nostrils (tube 78).
	 * Disconnect on the right.
	 */
	thy tubes [64]. right1 = nullptr;   // radiation at the lips
	thy tubes [78]. right1 = nullptr;   // radiation at the nostrils

	for (integer itube = 1; itube <= thy numberOfTubes; itube ++) {
		Delta_Tube t = & thy tubes [itube];
		Melder_assert (! t->left1 || t->left1->right1 == t || t->left1->right2 == t);
		Melder_assert (! t->left2 || t->left2->right1 == t);
		Melder_assert (! t->right1 || t->right1->left1 == t || t->right1->left2 == t);
		Melder_assert (! t->right2 || t->right2->left1 == t);
	}
	return thee;
}

/* End of file Speaker_to_Delta.cpp */
