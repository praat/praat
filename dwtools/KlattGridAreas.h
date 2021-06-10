#ifndef _KlattGridAreas_h_
#define _KlattGridAreas_h_
/* KlattGridAreas.h
 *
 * Copyright (C) 2009-2011 David Weenink, 2017,2021 Paul Boersma
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

#include "RealTierArea.h"
#include "KlattGrid.h"

Thing_define (KlattGrid_RealTierArea, RealTierArea) {
};

Thing_define (KlattGrid_OpenPhaseTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_OpenPhaseTierArea_prefs.h"
};

Thing_define (KlattGrid_CollisionPhaseTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
};

Thing_define (KlattGrid_Power1TierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_Power1TierArea_prefs.h"
};

Thing_define (KlattGrid_Power2TierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_Power2TierArea_prefs.h"
};

Thing_define (KlattGrid_DoublePulsingTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_DoublePulsingTierArea_prefs.h"
};

Thing_define (KlattGrid_PitchTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_PitchTierArea_prefs.h"
};

Thing_define (KlattGrid_FlutterTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_FlutterTierArea_prefs.h"
};

Thing_define (KlattGrid_IntensityTierArea, KlattGrid_RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_IntensityTierArea_prefs.h"
};

Thing_define (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_DecibelTierArea_prefs.h"
};

Thing_define (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea) {
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_SpectralTiltTierArea_prefs.h"
};

Thing_define (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea) {
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }
	#include "KlattGrid_FricationBypassTierArea_prefs.h"
};

Thing_define (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

/* End of file KlattGridAreas.h */
#endif
