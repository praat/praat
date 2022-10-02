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
	KlattGrid klattgrid() { return static_cast <KlattGrid> (our data()); }

	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime)
		override;
};

Thing_define (KlattGrid_IntensityTierEditor, KlattGrid_RealTierEditor) {
	void v_createMenuItems_help (EditorMenu menu)
		override;
};
void KlattGrid_IntensityTierEditor_init (KlattGrid_IntensityTierEditor me, autoKlattGrid_IntensityTierArea area, conststring32 title, KlattGrid klattgrid);


# pragma mark - KlattGrid_PitchTierEditor

Thing_define (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> pitch.get());
	}
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable pitch tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_VoicingAmplitudeTierEditor

Thing_define (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> voicingAmplitude.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable voicing amplitude tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FlutterTierEditor

Thing_define (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> flutter.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable flutter tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_Power1TierEditor

Thing_define (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> power1.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable power1 tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_Power2TierEditor

Thing_define (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> power2.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable power2 tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_OpenPhaseTierEditor

Thing_define (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> openPhase.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable open phase tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_CollisionPhaseTierEditor

Thing_define (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> collisionPhase.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable collision phase tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_DoublePulsingTierEditor

Thing_define (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> doublePulsing.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable double pulsing tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_SpectralTiltTierEditor

Thing_define (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> spectralTilt.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable spectral tilt tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_AspirationAmplitudeTierEditor

Thing_define (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> aspirationAmplitude.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable aspiration amplitude tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_BreathinessAmplitudeTierEditor

Thing_define (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> phonation -> breathinessAmplitude.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable breathiness amplitude tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_DecibelTierEditor

Thing_define (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor) {
	kKlattGridFormantType formantType;
	integer iformant;
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (
				KlattGrid_getAddressOfAmplitudes (our klattgrid(), our formantType)->at [our iformant]);
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable formant amplitude tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)   // BUG: make more specific
		);
	}
};
autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (conststring32 title, KlattGrid klattgrid,
		kKlattGridFormantType formantType, integer iformant);


#pragma mark - KlattGrid_FricationBypassTierEditor

Thing_define (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> frication -> bypass.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable frication bypass tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FricationAmplitudeTierArea

Thing_define (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (our klattgrid() -> frication -> fricationAmplitude.get());
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable frication amplitude tier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
	}
};
autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


#pragma mark - KlattGrid_FormantGridEditor

Thing_define (KlattGrid_FormantGridEditor, FormantGridEditor) {
	KlattGrid klattgrid() { return static_cast <KlattGrid> (our data()); }
	kKlattGridFormantType formantType;

	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our formantGridArea() -> _formantGrid = KlattGrid_getAddressOfFormantGrid (our klattgrid(), our formantType)->get();
		OrderedOf<structRealTier>* tiers =
				( our formantGridArea() -> editingBandwidths ? & our formantGridArea() -> _formantGrid -> bandwidths : & our formantGridArea() -> _formantGrid -> formants );
		RealTier tier = tiers->at [our formantGridArea() -> selectedFormant];
		our formantGridArea() -> functionChanged (tier);
	}
	void v_play (double startTime, double endTime)
		override;
	void v_drawLegends () override {
		FunctionArea_drawLegend (our formantGridArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable formant grid",
			DataGui_defaultForegroundColour (our formantGridArea().get(), false)   // BUG: should split up
		);
	}
};
autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (conststring32 title, KlattGrid data, kKlattGridFormantType formantType);

/* End of file KlattGridEditors.h */
#endif
