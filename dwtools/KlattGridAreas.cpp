/* KlattGridAreas.cpp
 *
 * Copyright (C) 2009-2019 David Weenink, 2021,2022 Paul Boersma
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

Thing_implement (KlattGrid_RealTierArea, RealTierArea, 0);

Thing_implement (KlattGrid_PitchTierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_PitchTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_PitchTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_PitchTierArea_prefs.h"

Thing_implement (KlattGrid_IntensityTierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_IntensityTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_IntensityTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_IntensityTierArea_prefs.h"

Thing_implement (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_DecibelTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_DecibelTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_DecibelTierArea_prefs.h"

Thing_implement (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_SpectralTiltTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_SpectralTiltTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_SpectralTiltTierArea_prefs.h"

Thing_implement (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_FricationBypassTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_FricationBypassTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_FricationBypassTierArea_prefs.h"

Thing_implement (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea, 0);

Thing_implement (KlattGrid_OpenPhaseTierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_OpenPhaseTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_OpenPhaseTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_OpenPhaseTierArea_prefs.h"

Thing_implement (KlattGrid_CollisionPhaseTierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_CollisionPhaseTierArea_prefs.h"

Thing_implement (KlattGrid_Power1TierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_Power1TierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_Power1TierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_Power1TierArea_prefs.h"

Thing_implement (KlattGrid_Power2TierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_Power2TierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_Power2TierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_Power2TierArea_prefs.h"

Thing_implement (KlattGrid_FlutterTierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_FlutterTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_FlutterTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_FlutterTierArea_prefs.h"

Thing_implement (KlattGrid_DoublePulsingTierArea, KlattGrid_RealTierArea, 0);
#include "EditorPrefs_define.h"
#include "KlattGrid_DoublePulsingTierArea_prefs.h"
#include "EditorPrefs_install.h"
#include "KlattGrid_DoublePulsingTierArea_prefs.h"
#include "EditorPrefs_copyToInstance.h"
#include "KlattGrid_DoublePulsingTierArea_prefs.h"

/* End of file KlattGridAreas.cpp */
