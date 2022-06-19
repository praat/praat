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
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> pitch.get());
	}
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
autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_VoicingAmplitudeTierEditor

Thing_define (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> voicingAmplitude.get());
	}
};
autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FlutterTierEditor

Thing_define (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> flutter.get());
	}
	conststring32 v_quantityText ()
		override { return U"Flutter (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set flutter range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_Power1TierEditor

Thing_define (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> power1.get());
	}
	conststring32 v_quantityText ()
		override { return U"Power1"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set power1 range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum"; }
};
autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_Power2TierEditor

Thing_define (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> power2.get());
	}
	conststring32 v_quantityText ()
		override { return U"Power2"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set power2 range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum"; }
};
autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_OpenPhaseTierEditor

Thing_define (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> openPhase.get());
	}
	conststring32 v_quantityText ()
		override { return U"Open phase (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set open phase range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_CollisionPhaseTierEditor

Thing_define (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> collisionPhase.get());
	}
	conststring32 v_quantityText ()
		override { return U"Collision phase (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set collision phase range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_DoublePulsingTierEditor

Thing_define (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> doublePulsing.get());
	}
	conststring32 v_quantityText ()
		override { return U"Double pulsing (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set double pulsing range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum (0..1)"; }
};
autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_SpectralTiltTierEditor

Thing_define (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> spectralTilt.get());
	}
};
autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_AspirationAmplitudeTierEditor

Thing_define (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> aspirationAmplitude.get());
	}
};
autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_BreathinessAmplitudeTierEditor

Thing_define (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> phonation -> breathinessAmplitude.get());
	}
};
autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_DecibelTierEditor

Thing_define (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor) {
	kKlattGridFormantType formantType;
	integer iformant;
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (
				KlattGrid_getAddressOfAmplitudes (our klattgrid(), our formantType)->at [our iformant]);
	}
	conststring32 v_quantityText ()
		override { return U"Amplitude (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set amplitude range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum amplitude (dB)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum amplitude (dB)"; }
};
autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (conststring32 title, KlattGrid klattgrid,
		kKlattGridFormantType formantType, integer iformant);


#pragma mark - KlattGrid_FricationBypassTierEditor

Thing_define (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> frication -> bypass.get());
	}
};
autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FricationAmplitudeTierArea

Thing_define (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our realTierArea -> functionChanged (our klattgrid() -> frication -> fricationAmplitude.get());
	}
};
autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FormantGridEditor

Thing_define (KlattGrid_FormantGridEditor, FormantGridEditor) {
	KlattGrid klattgrid() { return static_cast <KlattGrid> (our data); }
	kKlattGridFormantType formantType;

	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our formantGridArea -> _formantGrid = KlattGrid_getAddressOfFormantGrid (our klattgrid(), our formantType)->get();
		OrderedOf<structRealTier>* tiers =
				( our formantGridArea -> editingBandwidths ? & our formantGridArea -> _formantGrid -> bandwidths : & our formantGridArea -> _formantGrid -> formants );
		RealTier tier = tiers->at [our formantGridArea -> selectedFormant];
		our formantGridArea -> functionChanged (tier);
	}
	void v_play (double startTime, double endTime)
		override;
	bool v_hasSourceMenu ()
		override { return false; }
};
autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (conststring32 title, KlattGrid data, kKlattGridFormantType formantType);

/* End of file KlattGridEditors.h */
#endif
