#ifndef _KlattGridEditors_h_
#define _KlattGridEditors_h_
/* KlattGridEditors.h
 *
 * Copyright (C) 2009-2011 David Weenink
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

#include "KlattGrid.h"
#include "PitchTier.h"
#include "IntensityTier.h"
#include "RealTierEditor.h"
#include "FormantGridEditor.h"


Thing_define (KlattGrid_realTierEditor, RealTierEditor) {
	KlattGrid klattgrid;

	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double tmin, double tmax)
		override;
};

void KlattGrid_realTierEditor_init (KlattGrid_realTierEditor me, const char32 *title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Open phase (0..1)"; }
	const char32 * v_quantityKey ()
		override { return U"Open phase"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum"; }
};

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Collision phase (0..1)"; }
	const char32 * v_quantityKey ()
		override { return U"Collision phase"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum"; }
};

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_power1TierEditor, KlattGrid_realTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	const char32 * v_quantityText ()
		override { return U"Power1"; }
	const char32 * v_quantityKey ()
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
	const char32 * v_yminKey ()
		override { return U"Minimum"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum"; }
};

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_power2TierEditor, KlattGrid_realTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	const char32 * v_quantityText ()
		override { return U"Power2"; }
	const char32 * v_quantityKey ()
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
	const char32 * v_yminKey ()
		override { return U"Minimum"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum"; }
};

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Double pulsing (0..1)"; }
	const char32 * v_quantityKey ()
		override { return U"Double pulsing"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum"; }
};

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	double v_minimumLegalValue ()
		override { return 0.0; }
	const char32 * v_quantityText ()
		override { return U"Frequency (Hz)"; }
	const char32 * v_quantityKey ()
		override { return U"Frequency"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum frequency"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum frequency"; }
};

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor) {
	double v_minimumLegalValue ()
		override { return 0.0; }
	double v_maximumLegalValue ()
		override { return 1.0; }
	const char32 * v_quantityText ()
		override { return U"Flutter (0..1)"; }
	const char32 * v_quantityKey ()
		override { return U"Flutter"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum"; }
};

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	const char32 * v_quantityText ()
		override { return U"Intensity (dB)"; }
	const char32 * v_quantityKey ()
		override { return U"Intensity"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum intensity"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum intensity"; }
};

void KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, const char32 *title, KlattGrid klattgrid, RealTier tier);


Thing_define (KlattGrid_decibelTierEditor, KlattGrid_intensityTierEditor) {
	const char32 * v_quantityText ()
		override { return U"Amplitude (dB)"; }
	const char32 * v_quantityKey ()
		override { return U"Amplitude"; }
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
	const char32 * v_yminKey ()
		override { return U"Minimum amplitude"; }
	const char32 * v_ymaxKey ()
		override { return U"Maximum amplitude"; }
};

KlattGrid_decibelTierEditor KlattGrid_decibelTierEditor_create (const char32 *title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor) {
	double v_defaultYmin ()
		override { return -50.0; }
	double v_defaultYmax ()
		override { return 10.0; }
	const char32 * v_defaultYminText ()
		override { return U"-50.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"10.0"; }
};

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor) {
	double v_defaultYmin ()
		override { return -50.0; }
	double v_defaultYmax ()
		override { return 10.0; }
	const char32 * v_defaultYminText ()
		override { return U"-50.0"; }
	const char32 * v_defaultYmaxText ()
		override { return U"10.0"; }
};

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (const char32 *title, KlattGrid klattgrid);


Thing_define (KlattGrid_formantGridEditor, FormantGridEditor) {
	KlattGrid klattgrid;

	void v_play (double tmin, double tmax)
		override;
	bool v_hasSourceMenu ()
		override { return false; }
};

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (const char32 *title, KlattGrid data, int formantType);


#endif // _KlattGridEditors_h_