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
	// new data:
	public:
		KlattGrid klattgrid;
	// overridden methods:
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_play (double tmin, double tmax);
};

void KlattGrid_realTierEditor_init (KlattGrid_realTierEditor me, const wchar_t *title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual double v_maximumLegalValue () { return 1.0; }
		virtual const wchar_t * v_quantityText () { return L"Open phase (0..1)"; }
		virtual const wchar_t * v_quantityKey () { return L"Open phase"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 1.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set open phase range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"1.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum (0..1)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum (0..1)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }
};

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual double v_maximumLegalValue () { return 1.0; }
		virtual const wchar_t * v_quantityText () { return L"Collision phase (0..1)"; }
		virtual const wchar_t * v_quantityKey () { return L"Collision phase"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 0.1; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set collision phase range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"0.1"; }
		virtual const wchar_t * v_yminText () { return L"Minimum (0..1)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum (0..1)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }
};

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_power1TierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual const wchar_t * v_quantityText () { return L"Power1"; }
		virtual const wchar_t * v_quantityKey () { return L"Power1"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 4.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set power1 range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"4"; }
		virtual const wchar_t * v_yminText () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }
};

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_power2TierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual const wchar_t * v_quantityText () { return L"Power2"; }
		virtual const wchar_t * v_quantityKey () { return L"Power2"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 5.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set power2 range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"5"; }
		virtual const wchar_t * v_yminText () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }
};

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual double v_maximumLegalValue () { return 1.0; }
		virtual const wchar_t * v_quantityText () { return L"Double pulsing (0..1)"; }
		virtual const wchar_t * v_quantityKey () { return L"Double pulsing"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 1.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set double pulsing range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"1.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum (0..1)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum (0..1)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }
};

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		void v_createHelpMenuItems (EditorMenu menu);
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual const wchar_t * v_quantityText () { return L"Frequency (Hz)"; }
		virtual const wchar_t * v_quantityKey () { return L"Frequency"; }
		virtual const wchar_t * v_rightTickUnits () { return L" Hz"; }
		virtual double v_defaultYmin () { return 50.0; }
		virtual double v_defaultYmax () { return 600.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set frequency range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"50.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"600.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum frequency (Hz)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum frequency (Hz)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum frequency"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum frequency"; }
};

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual double v_maximumLegalValue () { return 1.0; }
		virtual const wchar_t * v_quantityText () { return L"Flutter (0..1)"; }
		virtual const wchar_t * v_quantityKey () { return L"Flutter"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 1.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set flutter range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"1.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum (0..1)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum (0..1)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }
};

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor) {
	// overridden methods:
		void v_createHelpMenuItems (EditorMenu menu);
		virtual const wchar_t * v_quantityText () { return L"Intensity (dB)"; }
		virtual const wchar_t * v_quantityKey () { return L"Intensity"; }
		virtual const wchar_t * v_rightTickUnits () { return L" dB"; }
		virtual double v_defaultYmin () { return 50.0; }
		virtual double v_defaultYmax () { return 100.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set intensity range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"50.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"100.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum intensity (dB)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum intensity (dB)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum intensity"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum intensity"; }
};

void KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, const wchar_t *title, KlattGrid klattgrid, RealTier tier);


Thing_define (KlattGrid_decibelTierEditor, KlattGrid_intensityTierEditor) {
	// overridden methods:
		virtual const wchar_t * v_quantityText () { return L"Amplitude (dB)"; }
		virtual const wchar_t * v_quantityKey () { return L"Amplitude"; }
		virtual const wchar_t * v_rightTickUnits () { return L" dB"; }
		virtual double v_defaultYmin () { return -30.0; }
		virtual double v_defaultYmax () { return 30.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set amplitude range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"-30.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"30.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum amplitude (dB)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum amplitude (dB)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum amplitude"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum amplitude"; }
};

KlattGrid_decibelTierEditor KlattGrid_decibelTierEditor_create (const wchar_t *title, KlattGrid klattgrid, RealTier data);


Thing_define (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor) {
	// overridden methods:
		virtual double v_defaultYmin () { return -50.0; }
		virtual double v_defaultYmax () { return 10.0; }
		virtual const wchar_t * v_defaultYminText () { return L"-50.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"10.0"; }
};

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor) {
	// overridden methods:
		virtual double v_defaultYmin () { return -50.0; }
		virtual double v_defaultYmax () { return 10.0; }
		virtual const wchar_t * v_defaultYminText () { return L"-50.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"10.0"; }
};

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor) {
};

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (const wchar_t *title, KlattGrid klattgrid);


Thing_define (KlattGrid_formantGridEditor, FormantGridEditor) {
	// new data:
	public:
		KlattGrid klattgrid;
	// overridden methods:
		virtual void v_play (double tmin, double tmax);
		virtual bool v_hasSourceMenu () { return false; }
};

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (const wchar_t *title, KlattGrid data, int formantType);


#endif // _KlattGridEditors_h_