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

/*
  djmw 20090123, 20090420, 20090630
  djmw 20110306 Latest modification.
*/

#include "KlattGrid.h"
#include "PitchTier.h"
#include "IntensityTier.h"
#include "RealTierEditor.h"
#include "FormantGridEditor.h"


Thing_declare1cpp (KlattGrid_realTierEditor);
struct structKlattGrid_realTierEditor : public structRealTierEditor {
	// new data:
		KlattGrid klattgrid;
	// overridden methods:
		void v_createHelpMenuItems (EditorMenu menu);
};
#define KlattGrid_realTierEditor__methods(Klas) RealTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_realTierEditor, RealTierEditor);

void KlattGrid_realTierEditor_init (KlattGrid_realTierEditor me, GuiObject parent, const wchar_t *title, KlattGrid klattgrid, RealTier data);


Thing_declare1cpp (KlattGrid_openPhaseTierEditor);
struct structKlattGrid_openPhaseTierEditor : public structKlattGrid_realTierEditor {
};
#define KlattGrid_openPhaseTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor);

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_collisionPhaseTierEditor);
struct structKlattGrid_collisionPhaseTierEditor : public structKlattGrid_realTierEditor {
};
#define KlattGrid_collisionPhaseTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor);

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_power1TierEditor);
struct structKlattGrid_power1TierEditor : public structKlattGrid_realTierEditor {
};
#define KlattGrid_power1TierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_power1TierEditor, KlattGrid_realTierEditor);

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_power2TierEditor);
struct structKlattGrid_power2TierEditor : public structKlattGrid_realTierEditor {
};
#define KlattGrid_power2TierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_power2TierEditor, KlattGrid_realTierEditor);

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_doublePulsingTierEditor);
struct structKlattGrid_doublePulsingTierEditor : public structKlattGrid_realTierEditor {
};
#define KlattGrid_doublePulsingTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor);

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_pitchTierEditor);
struct structKlattGrid_pitchTierEditor : public structKlattGrid_realTierEditor {
	// overridden methods:
		void v_createHelpMenuItems (EditorMenu menu);
};
#define KlattGrid_pitchTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor);

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_flutterTierEditor);
struct structKlattGrid_flutterTierEditor : public structKlattGrid_realTierEditor {
};
#define KlattGrid_flutterTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor);

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_intensityTierEditor);
struct structKlattGrid_intensityTierEditor : public structKlattGrid_realTierEditor {
	// overridden methods:
		void v_createHelpMenuItems (EditorMenu menu);
};
#define KlattGrid_intensityTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor);

void KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, GuiObject parent, const wchar_t *title, KlattGrid klattgrid, RealTier tier);


Thing_declare1cpp (KlattGrid_decibelTierEditor);
struct structKlattGrid_decibelTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_decibelTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_decibelTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_decibelTierEditor KlattGrid_decibelTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid, RealTier data);


Thing_declare1cpp (KlattGrid_voicingAmplitudeTierEditor);
struct structKlattGrid_voicingAmplitudeTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_voicingAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_aspirationAmplitudeTierEditor);
struct structKlattGrid_aspirationAmplitudeTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_aspirationAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_breathinessAmplitudeTierEditor);
struct structKlattGrid_breathinessAmplitudeTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_breathinessAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_spectralTiltTierEditor);
struct structKlattGrid_spectralTiltTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_spectralTiltTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_fricationBypassTierEditor);
struct structKlattGrid_fricationBypassTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_fricationBypassTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_fricationAmplitudeTierEditor);
struct structKlattGrid_fricationAmplitudeTierEditor : public structKlattGrid_intensityTierEditor {
};
#define KlattGrid_fricationAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (GuiObject parent, const wchar_t *title, KlattGrid klattgrid);


Thing_declare1cpp (KlattGrid_formantGridEditor);
struct structKlattGrid_formantGridEditor : public structFormantGridEditor {
	KlattGrid klattgrid;
};
#define KlattGrid_formantGridEditor__methods(Klas) FormantGridEditor__methods(Klas)
Thing_declare2cpp (KlattGrid_formantGridEditor, FormantGridEditor);

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (GuiObject parent, const wchar_t *title, KlattGrid data, int formantType);


#endif // _KlattGridEditors_h_
