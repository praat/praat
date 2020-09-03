#ifndef _KlattGridEditors_h_
#define _KlattGridEditors_h_
/* KlattGridEditors.h
 *
 * Copyright (C) 2009-2011 David Weenink, 2017 Paul Boersma
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

#include "KlattGrid.h"
#include "PitchTier.h"
#include "IntensityTier.h"
#include "RealTierEditor.h"
#include "FormantGridEditor.h"

Thing_define (KlattGrid_RealTierArea, RealTierArea) {
};

Thing_define (KlattGrid_RealTierEditor, RealTierEditor) {
	KlattGrid klattgrid;

	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime)
		override;
};

void KlattGrid_RealTierEditor_init (KlattGrid_RealTierEditor me, ClassInfo areaClass, conststring32 title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_OpenPhaseTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 1.0; }
};

Thing_define (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Open phase (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set open phase range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"1.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_CollisionPhaseTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 0.1; }
};

Thing_define (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Collision phase (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set collision phase range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"0.1"; }
	conststring32 v_yminText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_Power1TierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 4.0; }
};

Thing_define (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Power1"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set power1 range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"4"; }
	conststring32 v_yminText ()
		override { return U"Minimum"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum"; }
};

autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_Power2TierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 5.0; }
};

Thing_define (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Power2"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set power2 range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"5"; }
	conststring32 v_yminText ()
		override { return U"Minimum"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum"; }
};

autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_DoublePulsingTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 1.0; }
};

Thing_define (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Double pulsing (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set double pulsing range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"1.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_PitchTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	double v_defaultYmin ()
		override { return 50.0; }
	double v_defaultYmax ()
		override { return 600.0; }
};

Thing_define (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	conststring32 v_quantityText ()
		override { return U"Frequency (Hz)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set frequency range..."; }
	conststring32 v_defaultYminText ()
		override { return U"50.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"600.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum frequency (Hz)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum frequency (Hz)"; }
};

autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_FlutterTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 1.0; }
};

Thing_define (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Flutter (0..1)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set flutter range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"1.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum (0..1)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_IntensityTierArea, KlattGrid_RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultYmin ()
		override { return 50.0; }
	double v_defaultYmax ()
		override { return 100.0; }
};

Thing_define (KlattGrid_IntensityTierEditor, KlattGrid_RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	conststring32 v_quantityText ()
		override { return U"Intensity (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set intensity range..."; }
	conststring32 v_defaultYminText ()
		override { return U"50.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"100.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum intensity (dB)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum intensity (dB)"; }
};

void KlattGrid_IntensityTierEditor_init (KlattGrid_IntensityTierEditor me, ClassInfo viewClass, conststring32 title, KlattGrid klattgrid, RealTier tier);


Thing_define (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultYmin ()
		override { return -30.0; }
	double v_defaultYmax ()
		override { return 30.0; }
};

Thing_define (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor) {
	conststring32 v_quantityText ()
		override { return U"Amplitude (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set amplitude range..."; }
	conststring32 v_defaultYminText ()
		override { return U"-30.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"30.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum amplitude (dB)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum amplitude (dB)"; }
};

autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (conststring32 title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea) {
	double v_defaultYmin ()
		override { return -50.0; }
	double v_defaultYmax ()
		override { return 10.0; }
};

Thing_define (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor) {
	conststring32 v_defaultYminText ()
		override { return U"-50.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"10.0"; }
};

autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea) {
	double v_defaultYmin ()
		override { return -50.0; }
	double v_defaultYmax ()
		override { return 10.0; }
};

Thing_define (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor) {
	conststring32 v_defaultYminText ()
		override { return U"-50.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"10.0"; }
};

autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};

Thing_define (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (conststring32 title, KlattGrid klattgrid);


Thing_define (KlattGrid_FormantGridEditor, FormantGridEditor) {
	KlattGrid klattgrid;

	void v_play (double startTime, double endTime)
		override;
	bool v_hasSourceMenu ()
		override { return false; }
};

autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (conststring32 title, KlattGrid data, kKlattGridFormantType formantType);

/* End of file KlattGridEditors.h */
#endif
