#ifndef _KlattGridEditors_h_
#define _KlattGridEditors_h_
/* KlattGridEditors.h
 *
 * Copyright (C) 2009-2011 David Weenink, 2017,2022 Paul Boersma
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

#include "KlattGridAreas.h"
#include "PitchTierArea.h"
#include "IntensityTier.h"
#include "RealTierEditor.h"
#include "FormantGridEditor.h"

Thing_define (KlattGrid_RealTierEditor, RealTierEditor) {
	KlattGrid klattgrid() { return static_cast <KlattGrid> (our data); }

	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime)
		override;
};

Thing_define (KlattGrid_IntensityTierEditor, KlattGrid_RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	conststring32 v_quantityText ()
		override { return U"Intensity (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set intensity range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum intensity (dB)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum intensity (dB)"; }
};
void KlattGrid_IntensityTierEditor_init (KlattGrid_IntensityTierEditor me, autoKlattGrid_IntensityTierArea area, conststring32 title, KlattGrid klattgrid);


# pragma mark - KlattGrid_PitchTierEditor

Thing_define (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	conststring32 v_quantityText ()
		override { return U"Frequency (Hz)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set frequency range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum frequency (Hz)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum frequency (Hz)"; }
};
Thing_define (KlattGrid_PitchTierArea, PitchTierArea) {
	Function v_function() override {
		return ((KlattGrid_PitchTierEditor) our _editor) -> klattgrid() -> phonation -> pitch.get();
	}
	#include "KlattGrid_PitchTierArea_prefs.h"
};
autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_VoicingAmplitudeTierEditor

Thing_define (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};
Thing_define (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		return ((KlattGrid_VoicingAmplitudeTierEditor) our _editor) -> klattgrid() -> phonation -> voicingAmplitude.get();
	}
};
autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FlutterTierEditor

Thing_define (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Flutter (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set flutter range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
Thing_define (KlattGrid_FlutterTierArea, RealTierArea) {
	Function v_function() override {
		return ((KlattGrid_FlutterTierEditor) our _editor) -> klattgrid() -> phonation -> flutter.get();
	}
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_FlutterTierArea_prefs.h"
};
autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_Power1TierEditor

Thing_define (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Power1"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set power1 range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum"; }
};
Thing_define (KlattGrid_Power1TierArea, RealTierArea) {
	Function v_function() override {
		return ((KlattGrid_Power1TierEditor) our _editor) -> klattgrid() -> phonation -> power1.get();
	}
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_Power1TierArea_prefs.h"
};
autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_Power2TierEditor

Thing_define (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Power2"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set power2 range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum"; }
};
Thing_define (KlattGrid_Power2TierArea, RealTierArea) {
	Function v_function() override {
		return ((KlattGrid_Power2TierEditor) our _editor) -> klattgrid() -> phonation -> power2.get();
	}
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_Power2TierArea_prefs.h"
};
autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_OpenPhaseTierEditor

Thing_define (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Open phase (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set open phase range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
Thing_define (KlattGrid_OpenPhaseTierArea, RealTierArea) {
	Function v_function() override {
		return ((KlattGrid_OpenPhaseTierEditor) our _editor) -> klattgrid() -> phonation -> openPhase.get();
	}
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_OpenPhaseTierArea_prefs.h"
};
autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_CollisionPhaseTierEditor

Thing_define (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Collision phase (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set collision phase range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
Thing_define (KlattGrid_CollisionPhaseTierArea, RealTierArea) {
	Function v_function() override {
		return ((KlattGrid_CollisionPhaseTierEditor) our _editor) -> klattgrid() -> phonation -> collisionPhase.get();
	}
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
};
autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_DoublePulsingTierEditor

Thing_define (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Double pulsing (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set double pulsing range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
Thing_define (KlattGrid_DoublePulsingTierArea, RealTierArea) {
	Function v_function() override {
		return ((KlattGrid_DoublePulsingTierEditor) our _editor) -> klattgrid() -> phonation -> doublePulsing.get();
	}
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_DoublePulsingTierArea_prefs.h"
};
autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_SpectralTiltTierEditor

Thing_define (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor) {
};
Thing_define (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		return ((KlattGrid_SpectralTiltTierEditor) our _editor) -> klattgrid() -> phonation -> spectralTilt.get();
	}
	#include "KlattGrid_SpectralTiltTierArea_prefs.h"
};
autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_AspirationAmplitudeTierEditor

Thing_define (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};
Thing_define (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		return ((KlattGrid_AspirationAmplitudeTierEditor) our _editor) -> klattgrid() -> phonation -> aspirationAmplitude.get();
	}
};
autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_BreathinessAmplitudeTierEditor

Thing_define (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};
Thing_define (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		return ((KlattGrid_BreathinessAmplitudeTierEditor) our _editor) -> klattgrid() -> phonation -> breathinessAmplitude.get();
	}
};
autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_DecibelTierEditor

Thing_define (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor) {
	kKlattGridFormantType formantType;
	integer iformant;

	conststring32 v_quantityText ()
		override { return U"Amplitude (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set amplitude range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum amplitude (dB)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum amplitude (dB)"; }
};
Thing_define (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		KlattGrid_DecibelTierEditor editor = (KlattGrid_DecibelTierEditor) our _editor;
		return KlattGrid_getAddressOfAmplitudes (editor -> klattgrid(), editor -> formantType)->at [editor -> iformant];
	}
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	#include "KlattGrid_DecibelTierArea_prefs.h"
};
autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (conststring32 title, KlattGrid klattgrid,
		kKlattGridFormantType formantType, integer iformant);


#pragma mark - KlattGrid_FricationBypassTierEditor

Thing_define (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor) {
};
Thing_define (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		return ((KlattGrid_FricationBypassTierEditor) our _editor) -> klattgrid() -> frication -> bypass.get();
	}
	#include "KlattGrid_FricationBypassTierArea_prefs.h"
};
autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FricationAmplitudeTierArea

Thing_define (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};
Thing_define (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
	Function v_function() override {
		return ((KlattGrid_FricationAmplitudeTierEditor) our _editor) -> klattgrid() -> frication -> fricationAmplitude.get();
	}
};
autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FormantGridEditor

Thing_define (KlattGrid_FormantGridEditor, FormantGridEditor) {
	KlattGrid klattgrid;   // BUG: shouldn't be here
	void v_play (double startTime, double endTime)
		override;
	bool v_hasSourceMenu ()
		override { return false; }
};
autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (conststring32 title, KlattGrid data, kKlattGridFormantType formantType);

/* End of file KlattGridEditors.h */
#endif
