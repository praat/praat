#ifndef _KlattGridEditors_h_
#define _KlattGridEditors_h_
/* KlattGridEditors.h
 *
 * Copyright (C) 2009 David Weenink
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
  djmw 20090123
*/

#ifndef _KlattGrid_h_
	#include "KlattGrid.h"
#endif
#ifndef _PitchTier_h_
	#include "PitchTier.h"
#endif
#ifndef _IntensityTier_h_
	#include "IntensityTier.h"
#endif
#ifndef _RealTierEditor_h_
	#include "RealTierEditor.h"
#endif
#ifndef _FormantGridEditor_h_
	#include "FormantGridEditor.h"
#endif

#define KlattGrid_realTierEditor__parents(Klas) RealTierEditor__parents(Klas) Thing_inherit (Klas, RealTierEditor)
Thing_declare1 (KlattGrid_realTierEditor);
#define KlattGrid_realTierEditor__members(Klas) RealTierEditor__members(Klas) \
	KlattGrid klattgrid;
#define KlattGrid_realTierEditor__methods(Klas) RealTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_realTierEditor, RealTierEditor);

int KlattGrid_realTierEditor_init (KlattGrid_realTierEditor, Widget parent, const wchar_t *title, KlattGrid klattgrid, RealTier data);

#define KlattGrid_openPhaseTierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_openPhaseTierEditor);
#define KlattGrid_openPhaseTierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_openPhaseTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_openPhaseTierEditor, KlattGrid_realTierEditor);

KlattGrid_openPhaseTierEditor KlattGrid_openPhaseTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_collisionPhaseTierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_collisionPhaseTierEditor);
#define KlattGrid_collisionPhaseTierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_collisionPhaseTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_collisionPhaseTierEditor, KlattGrid_realTierEditor);

KlattGrid_collisionPhaseTierEditor KlattGrid_collisionPhaseTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_power1TierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_power1TierEditor);
#define KlattGrid_power1TierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_power1TierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_power1TierEditor, KlattGrid_realTierEditor);

KlattGrid_power1TierEditor KlattGrid_power1TierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_power2TierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_power2TierEditor);
#define KlattGrid_power2TierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_power2TierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_power2TierEditor, KlattGrid_realTierEditor);

KlattGrid_power2TierEditor KlattGrid_power2TierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_doublePulsingTierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_doublePulsingTierEditor);
#define KlattGrid_doublePulsingTierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_doublePulsingTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_doublePulsingTierEditor, KlattGrid_realTierEditor);

KlattGrid_doublePulsingTierEditor KlattGrid_doublePulsingTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_pitchTierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_pitchTierEditor);
#define KlattGrid_pitchTierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_pitchTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_pitchTierEditor, KlattGrid_realTierEditor);

KlattGrid_pitchTierEditor KlattGrid_pitchTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_flutterTierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_flutterTierEditor);
#define KlattGrid_flutterTierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_flutterTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_flutterTierEditor, KlattGrid_realTierEditor);

KlattGrid_flutterTierEditor KlattGrid_flutterTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_intensityTierEditor__parents(Klas) KlattGrid_realTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_realTierEditor)
Thing_declare1 (KlattGrid_intensityTierEditor);
#define KlattGrid_intensityTierEditor__members(Klas) KlattGrid_realTierEditor__members(Klas)
#define KlattGrid_intensityTierEditor__methods(Klas) KlattGrid_realTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_intensityTierEditor, KlattGrid_realTierEditor);

int KlattGrid_intensityTierEditor_init (KlattGrid_intensityTierEditor me, Widget parent, const wchar_t *title, KlattGrid klattgrid, RealTier tier);

#define KlattGrid_voicingAmplitudeTierEditor__parents(Klas) KlattGrid_intensityTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_intensityTierEditor)
Thing_declare1 (KlattGrid_voicingAmplitudeTierEditor);
#define KlattGrid_voicingAmplitudeTierEditor__members(Klas) KlattGrid_intensityTierEditor__members(Klas)
#define KlattGrid_voicingAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_voicingAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_voicingAmplitudeTierEditor KlattGrid_voicingAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_aspirationAmplitudeTierEditor__parents(Klas) KlattGrid_intensityTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_intensityTierEditor)
Thing_declare1 (KlattGrid_aspirationAmplitudeTierEditor);
#define KlattGrid_aspirationAmplitudeTierEditor__members(Klas) KlattGrid_intensityTierEditor__members(Klas)
#define KlattGrid_aspirationAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_aspirationAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_aspirationAmplitudeTierEditor KlattGrid_aspirationAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_breathinessAmplitudeTierEditor__parents(Klas) KlattGrid_intensityTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_intensityTierEditor)
Thing_declare1 (KlattGrid_breathinessAmplitudeTierEditor);
#define KlattGrid_breathinessAmplitudeTierEditor__members(Klas) KlattGrid_intensityTierEditor__members(Klas)
#define KlattGrid_breathinessAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_breathinessAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_breathinessAmplitudeTierEditor KlattGrid_breathinessAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_spectralTiltTierEditor__parents(Klas) KlattGrid_intensityTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_intensityTierEditor)
Thing_declare1 (KlattGrid_spectralTiltTierEditor);
#define KlattGrid_spectralTiltTierEditor__members(Klas) KlattGrid_intensityTierEditor__members(Klas)
#define KlattGrid_spectralTiltTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_spectralTiltTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_spectralTiltTierEditor KlattGrid_spectralTiltTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_fricationBypassTierEditor__parents(Klas) KlattGrid_intensityTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_intensityTierEditor)
Thing_declare1 (KlattGrid_fricationBypassTierEditor);
#define KlattGrid_fricationBypassTierEditor__members(Klas) KlattGrid_intensityTierEditor__members(Klas)
#define KlattGrid_fricationBypassTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_fricationBypassTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_fricationBypassTierEditor KlattGrid_fricationBypassTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_fricationAmplitudeTierEditor__parents(Klas) KlattGrid_intensityTierEditor__parents(Klas) Thing_inherit (Klas, KlattGrid_intensityTierEditor)
Thing_declare1 (KlattGrid_fricationAmplitudeTierEditor);
#define KlattGrid_fricationAmplitudeTierEditor__members(Klas) KlattGrid_intensityTierEditor__members(Klas)
#define KlattGrid_fricationAmplitudeTierEditor__methods(Klas) KlattGrid_intensityTierEditor__methods(Klas)
Thing_declare2 (KlattGrid_fricationAmplitudeTierEditor, KlattGrid_intensityTierEditor);

KlattGrid_fricationAmplitudeTierEditor KlattGrid_fricationAmplitudeTierEditor_create (Widget parent, const wchar_t *title, KlattGrid klattgrid);

#define KlattGrid_formantGridEditor__parents(Klas) FormantGridEditor__parents(Klas) Thing_inherit (Klas, FormantGridEditor)
Thing_declare1 (KlattGrid_formantGridEditor);
#define KlattGrid_formantGridEditor__members(Klas) FormantGridEditor__members(Klas) \
	KlattGrid klattgrid;
#define KlattGrid_formantGridEditor__methods(Klas) FormantGridEditor__methods(Klas)
Thing_declare2 (KlattGrid_formantGridEditor, FormantGridEditor);

KlattGrid_formantGridEditor KlattGrid_formantGridEditor_create (Widget parent, const wchar_t *title, KlattGrid data, int formantType);

#endif /* _KlattGridEditors_h_ */

