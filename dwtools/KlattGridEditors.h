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


Thing_define (KlattGrid_RealTierEditor, RealTierEditor) {
	KlattGrid klattgrid;

	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double tmin, double tmax)
		override;
};

void KlattGrid_RealTierEditor_init (KlattGrid_RealTierEditor me, const char32 *title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_OpenPhaseTierEditor, KlattGrid_RealTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Open phase (0..1)"; }
	const char32 * v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 1.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set open phase range..."; }
	const char32 * v_defaultYminText ()
		override { return U"0.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"1.0"; }
	const char32 * v_yminText ()
		override { return U"Minimum (0..1)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_OpenPhaseTierEditor KlattGrid_OpenPhaseTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_CollisionPhaseTierEditor, KlattGrid_RealTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Collision phase (0..1)"; }
	const char32 * v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 0.1; }
	const char32 * v_setRangeTitle ()
		override { return U"Set collision phase range..."; }
	const char32 * v_defaultYminText ()
		override { return U"0.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"0.1"; }
	const char32 * v_yminText ()
		override { return U"Minimum (0..1)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_CollisionPhaseTierEditor KlattGrid_CollisionPhaseTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_Power1TierEditor, KlattGrid_RealTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	const char32 * v_quantityText ()
		override { return U"Power1"; }
	const char32 * v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 4.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set power1 range..."; }
	const char32 * v_defaultYminText ()
		override { return U"0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"4"; }
	const char32 * v_yminText ()
		override { return U"Minimum"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum"; }
};

autoKlattGrid_Power1TierEditor KlattGrid_Power1TierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_Power2TierEditor, KlattGrid_RealTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	const char32 * v_quantityText ()
		override { return U"Power2"; }
	const char32 * v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 5.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set power2 range..."; }
	const char32 * v_defaultYminText ()
		override { return U"0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"5"; }
	const char32 * v_yminText ()
		override { return U"Minimum"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum"; }
};

autoKlattGrid_Power2TierEditor KlattGrid_Power2TierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_DoublePulsingTierEditor, KlattGrid_RealTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Double pulsing (0..1)"; }
	const char32 * v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 1.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set double pulsing range..."; }
	const char32 * v_defaultYminText ()
		override { return U"0.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"1.0"; }
	const char32 * v_yminText ()
		override { return U"Minimum (0..1)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_DoublePulsingTierEditor KlattGrid_DoublePulsingTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_PitchTierEditor, KlattGrid_RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	double v_minimumLegalValue ()
		override { return 0.0; }
	const char32 * v_quantityText ()
		override { return U"Frequency (Hz)"; }
	const char32 * v_rightTickUnits ()
		override { return U" Hz"; }
	double v_defaultYmin ()
		override { return 50.0; }
	double v_defaultYmax ()
		override { return 600.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set frequency range..."; }
	const char32 * v_defaultYminText ()
		override { return U"50.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"600.0"; }
	const char32 * v_yminText ()
		override { return U"Minimum frequency (Hz)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum frequency (Hz)"; }
};

autoKlattGrid_PitchTierEditor KlattGrid_PitchTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_FlutterTierEditor, KlattGrid_RealTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Flutter (0..1)"; }
	const char32 * v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.0; }
	double v_defaultYmax ()
		override { return 1.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set flutter range..."; }
	const char32 * v_defaultYminText ()
		override { return U"0.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"1.0"; }
	const char32 * v_yminText ()
		override { return U"Minimum (0..1)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum (0..1)"; }
};

autoKlattGrid_FlutterTierEditor KlattGrid_FlutterTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_IntensityTierEditor, KlattGrid_RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	const char32 * v_quantityText ()
		override { return U"Intensity (dB)"; }
	const char32 * v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultYmin ()
		override { return 50.0; }
	double v_defaultYmax ()
		override { return 100.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set intensity range..."; }
	const char32 * v_defaultYminText ()
		override { return U"50.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"100.0"; }
	const char32 * v_yminText ()
		override { return U"Minimum intensity (dB)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum intensity (dB)"; }
};

void KlattGrid_IntensityTierEditor_init (KlattGrid_IntensityTierEditor me, const char32 *title, KlattGrid klattgrid, RealTier tier);


Thing_define (KlattGrid_DecibelTierEditor, KlattGrid_IntensityTierEditor) {
	const char32 * v_quantityText ()
		override { return U"Amplitude (dB)"; }
	const char32 * v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultYmin ()
		override { return -30.0; }
	double v_defaultYmax ()
		override { return 30.0; }
	const char32 * v_setRangeTitle ()
		override { return U"Set amplitude range..."; }
	const char32 * v_defaultYminText ()
		override { return U"-30.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"30.0"; }
	const char32 * v_yminText ()
		override { return U"Minimum amplitude (dB)"; }
	const char32 * v_ymaxText ()
		override { return U"Maximum amplitude (dB)"; }
};

autoKlattGrid_DecibelTierEditor KlattGrid_DecibelTierEditor_create (const char32 *title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_VoicingAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_VoicingAmplitudeTierEditor KlattGrid_VoicingAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_AspirationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_AspirationAmplitudeTierEditor KlattGrid_AspirationAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_BreathinessAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_BreathinessAmplitudeTierEditor KlattGrid_BreathinessAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_SpectralTiltTierEditor, KlattGrid_IntensityTierEditor) {
	double v_defaultYmin ()
		override { return -50.0; }
	double v_defaultYmax ()
		override { return 10.0; }
	const char32 * v_defaultYminText ()
		override { return U"-50.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"10.0"; }
};

autoKlattGrid_SpectralTiltTierEditor KlattGrid_SpectralTiltTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_FricationBypassTierEditor, KlattGrid_IntensityTierEditor) {
	double v_defaultYmin ()
		override { return -50.0; }
	double v_defaultYmax ()
		override { return 10.0; }
	const char32 * v_defaultYminText ()
		override { return U"-50.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"10.0"; }
};

autoKlattGrid_FricationBypassTierEditor KlattGrid_FricationBypassTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_FricationAmplitudeTierEditor, KlattGrid_IntensityTierEditor) {
};

autoKlattGrid_FricationAmplitudeTierEditor KlattGrid_FricationAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_FormantGridEditor, FormantGridEditor) {
	KlattGrid klattgrid;

	void v_play (double tmin, double tmax)
		override;
	bool v_hasSourceMenu ()
		override { return false; }
};

autoKlattGrid_FormantGridEditor KlattGrid_FormantGridEditor_create (const char32 *title, KlattGrid data, int formantType);

/* End of file KlattGridEditors.h */
#endif
