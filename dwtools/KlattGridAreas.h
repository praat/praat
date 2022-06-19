#ifndef _KlattGridAreas_h_
#define _KlattGridAreas_h_
/* KlattGridAreas.h
 *
 * Copyright (C) 2009-2011 David Weenink, 2017,2021,2022 Paul Boersma
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

#include "PitchTierArea.h"
#include "FormantGridEditor.h"
#include "KlattGrid.h"


#pragma mark - generic areas

Thing_define (KlattGrid_IntensityTierArea, RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	#include "KlattGrid_IntensityTierArea_prefs.h"
};

Thing_define (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	#include "KlattGrid_DecibelTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_DecibelTierArea, IntensityTier)

#pragma mark - KlattGrid_PitchTierArea

Thing_define (KlattGrid_PitchTierArea, PitchTierArea) {
	#include "KlattGrid_PitchTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_PitchTierArea, PitchTier)


#pragma mark - KlattGrid_VoicingAmplitudeTierArea

Thing_define (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
DEFINE_FunctionArea_create (KlattGrid_VoicingAmplitudeTierArea, IntensityTier)


#pragma mark - KlattGrid_FlutterTierArea

Thing_define (KlattGrid_FlutterTierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_FlutterTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_FlutterTierArea, RealTier)


#pragma mark - KlattGrid_Power1TierArea

Thing_define (KlattGrid_Power1TierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_Power1TierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_Power1TierArea, RealTier)


#pragma mark - KlattGrid_Power2TierArea

Thing_define (KlattGrid_Power2TierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_Power2TierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_Power2TierArea, RealTier)


#pragma mark - KlattGrid_OpenPhaseTierArea

Thing_define (KlattGrid_OpenPhaseTierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_OpenPhaseTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_OpenPhaseTierArea, RealTier)


#pragma mark - KlattGrid_CollisionPhaseTierArea

Thing_define (KlattGrid_CollisionPhaseTierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_CollisionPhaseTierArea, RealTier)


#pragma mark - KlattGrid_DoublePulsingTierArea

Thing_define (KlattGrid_DoublePulsingTierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_DoublePulsingTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_DoublePulsingTierArea, RealTier)


#pragma mark - KlattGrid_SpectralTiltTierArea

Thing_define (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea) {
	#include "KlattGrid_SpectralTiltTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_SpectralTiltTierArea, IntensityTier)


#pragma mark - KlattGrid_AspirationAmplitudeTierArea

Thing_define (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
DEFINE_FunctionArea_create (KlattGrid_AspirationAmplitudeTierArea, IntensityTier)


#pragma mark - KlattGrid_BreathinessAmplitudeTierArea
Thing_define (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
DEFINE_FunctionArea_create (KlattGrid_BreathinessAmplitudeTierArea, IntensityTier)


#pragma mark - KlattGrid_FricationBypassTierArea

Thing_define (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea) {
	#include "KlattGrid_FricationBypassTierArea_prefs.h"
};
DEFINE_FunctionArea_create (KlattGrid_FricationBypassTierArea, IntensityTier)


#pragma mark - KlattGrid_FricationAmplitudeTierArea

Thing_define (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
DEFINE_FunctionArea_create (KlattGrid_FricationAmplitudeTierArea, IntensityTier)


#pragma mark - KlattGrid_FormantGridArea
Thing_define (KlattGrid_FormantGridArea, FormantGridArea) {
};
DEFINE_FunctionArea_create (KlattGrid_FormantGridArea, FormantGrid)


/* End of file KlattGridAreas.h */
#endif
