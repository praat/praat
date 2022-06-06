#ifndef _KlattGridAreas_h_
#define _KlattGridAreas_h_
/* KlattGridAreas.h
 *
 * Copyright (C) 2009-2011 David Weenink, 2017,2021,2022 Paul Boersma
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

#include "RealTierArea.h"
#include "KlattGrid.h"

Thing_define (KlattGrid_RealTierArea, RealTierArea) {
};
inline void KlattGrid_RealTierArea_init (KlattGrid_RealTierArea me, FunctionEditor editor, RealTier realTier) {
	RealTierArea_init (me, editor, realTier);
}

Thing_define (KlattGrid_OpenPhaseTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_OpenPhaseTierArea_prefs.h"
};
inline autoKlattGrid_OpenPhaseTierArea KlattGrid_OpenPhaseTierArea_create (FunctionEditor editor, RealTier openPhase) {
	autoKlattGrid_OpenPhaseTierArea me = Thing_new (KlattGrid_OpenPhaseTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, openPhase);
	return me;
}

Thing_define (KlattGrid_CollisionPhaseTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_CollisionPhaseTierArea_prefs.h"
};
inline autoKlattGrid_CollisionPhaseTierArea KlattGrid_CollisionPhaseTierArea_create (FunctionEditor editor, RealTier collisionPhase) {
	autoKlattGrid_CollisionPhaseTierArea me = Thing_new (KlattGrid_CollisionPhaseTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, collisionPhase);
	return me;
}

Thing_define (KlattGrid_Power1TierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_Power1TierArea_prefs.h"
};
inline autoKlattGrid_Power1TierArea KlattGrid_Power1TierArea_create (FunctionEditor editor, RealTier power1) {
	autoKlattGrid_Power1TierArea me = Thing_new (KlattGrid_Power1TierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, power1);
	return me;
}

Thing_define (KlattGrid_Power2TierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_Power2TierArea_prefs.h"
};
inline autoKlattGrid_Power2TierArea KlattGrid_Power2TierArea_create (FunctionEditor editor, RealTier power2) {
	autoKlattGrid_Power2TierArea me = Thing_new (KlattGrid_Power2TierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, power2);
	return me;
}

Thing_define (KlattGrid_DoublePulsingTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_DoublePulsingTierArea_prefs.h"
};
inline autoKlattGrid_DoublePulsingTierArea KlattGrid_DoublePulsingTierArea_create (FunctionEditor editor, RealTier doublePulsing) {
	autoKlattGrid_DoublePulsingTierArea me = Thing_new (KlattGrid_DoublePulsingTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, doublePulsing);
	return me;
}

Thing_define (KlattGrid_PitchTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	#include "KlattGrid_PitchTierArea_prefs.h"
};
inline autoKlattGrid_PitchTierArea KlattGrid_PitchTierArea_create (FunctionEditor editor, PitchTier pitch) {
	autoKlattGrid_PitchTierArea me = Thing_new (KlattGrid_PitchTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, pitch);
	return me;
}

Thing_define (KlattGrid_FlutterTierArea, KlattGrid_RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	double v_maximumLegalY ()
		override { return 1.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	#include "KlattGrid_FlutterTierArea_prefs.h"
};
inline autoKlattGrid_FlutterTierArea KlattGrid_FlutterTierArea_create (FunctionEditor editor, RealTier flutter) {
	autoKlattGrid_FlutterTierArea me = Thing_new (KlattGrid_FlutterTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, flutter);
	return me;
}

Thing_define (KlattGrid_IntensityTierArea, KlattGrid_RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	#include "KlattGrid_IntensityTierArea_prefs.h"
};
inline autoKlattGrid_IntensityTierArea KlattGrid_IntensityTierArea_create (FunctionEditor editor, IntensityTier intensityTier) {
	autoKlattGrid_IntensityTierArea me = Thing_new (KlattGrid_IntensityTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, intensityTier);
	return me;
}

Thing_define (KlattGrid_DecibelTierArea, KlattGrid_IntensityTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	#include "KlattGrid_DecibelTierArea_prefs.h"
};
inline autoKlattGrid_DecibelTierArea KlattGrid_DecibelTierArea_create (FunctionEditor editor, IntensityTier decibelTier) {
	autoKlattGrid_DecibelTierArea me = Thing_new (KlattGrid_DecibelTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, decibelTier);
	return me;
}

Thing_define (KlattGrid_VoicingAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
inline autoKlattGrid_VoicingAmplitudeTierArea KlattGrid_VoicingAmplitudeTierArea_create (FunctionEditor editor, IntensityTier voicingAmplitude) {
	autoKlattGrid_VoicingAmplitudeTierArea me = Thing_new (KlattGrid_VoicingAmplitudeTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, voicingAmplitude);
	return me;
}

Thing_define (KlattGrid_AspirationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
inline autoKlattGrid_AspirationAmplitudeTierArea KlattGrid_AspirationAmplitudeTierArea_create (FunctionEditor editor, IntensityTier aspirationAmplitude) {
	autoKlattGrid_AspirationAmplitudeTierArea me = Thing_new (KlattGrid_AspirationAmplitudeTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, aspirationAmplitude);
	return me;
}

Thing_define (KlattGrid_BreathinessAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
inline autoKlattGrid_BreathinessAmplitudeTierArea KlattGrid_BreathinessAmplitudeTierArea_create (FunctionEditor editor, IntensityTier breathinessAmplitude) {
	autoKlattGrid_BreathinessAmplitudeTierArea me = Thing_new (KlattGrid_BreathinessAmplitudeTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, breathinessAmplitude);
	return me;
}

Thing_define (KlattGrid_SpectralTiltTierArea, KlattGrid_IntensityTierArea) {
	#include "KlattGrid_SpectralTiltTierArea_prefs.h"
};
inline autoKlattGrid_SpectralTiltTierArea KlattGrid_SpectralTiltTierArea_create (FunctionEditor editor, IntensityTier spectralTilt) {
	autoKlattGrid_SpectralTiltTierArea me = Thing_new (KlattGrid_SpectralTiltTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, spectralTilt);
	return me;
}

Thing_define (KlattGrid_FricationBypassTierArea, KlattGrid_IntensityTierArea) {
	#include "KlattGrid_FricationBypassTierArea_prefs.h"
};
inline autoKlattGrid_FricationBypassTierArea KlattGrid_FricationBypassTierArea_create (FunctionEditor editor, IntensityTier bypass) {
	autoKlattGrid_FricationBypassTierArea me = Thing_new (KlattGrid_FricationBypassTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, bypass);
	return me;
}

Thing_define (KlattGrid_FricationAmplitudeTierArea, KlattGrid_IntensityTierArea) {
};
inline autoKlattGrid_FricationAmplitudeTierArea KlattGrid_FricationAmplitudeTierArea_create (FunctionEditor editor, IntensityTier fricationAmplitude) {
	autoKlattGrid_FricationAmplitudeTierArea me = Thing_new (KlattGrid_FricationAmplitudeTierArea);
	KlattGrid_RealTierArea_init (me.get(), editor, fricationAmplitude);
	return me;
}

/* End of file KlattGridAreas.h */
#endif
