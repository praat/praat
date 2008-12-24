/* KlattGrid_def.h
 *
 * Copyright (C) 2008 David Weenink
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
 * djmw 20081112
 */

#define ooSTRUCT PhonationGrid
oo_DEFINE_CLASS (PhonationGrid, Function)

	oo_OBJECT (PitchTier, 0, pitch)
	oo_OBJECT (RealTier, 0, flutter) // [0,1]
	oo_OBJECT (IntensityTier, 0, voicingAmplitude) // dB
	oo_OBJECT (RealTier, 0, doublePulsing) // [0,1]
	oo_OBJECT (RealTier, 0, openPhase) // (0,1)
	oo_OBJECT (RealTier, 0, collisionPhase) //
	oo_OBJECT (RealTier, 0, power1) // 2,3..
	oo_OBJECT (RealTier, 0, power2) // 3,4.. power2>power1
	oo_OBJECT (IntensityTier, 0, spectralTilt) // dB
	oo_OBJECT (IntensityTier, 0, aspirationAmplitude) // dB
	oo_OBJECT (IntensityTier, 0, breathinessAmplitude) // dB
	
oo_END_CLASS (PhonationGrid)
#undef ooSTRUCT

#define ooSTRUCT VocalTractGrid
oo_DEFINE_CLASS (VocalTractGrid, Function)

	oo_OBJECT (FormantGrid, 0, formants)
	oo_OBJECT (FormantGrid, 0, nasal_formants)
	oo_OBJECT (FormantGrid, 0, nasal_antiformants)
	// for parallel synthesis
	oo_COLLECTION (Ordered, formants_amplitudes, IntensityTier, 0)
	oo_COLLECTION (Ordered, nasal_formants_amplitudes, IntensityTier, 0)

oo_END_CLASS (VocalTractGrid)
#undef ooSTRUCT

#define ooSTRUCT CouplingGrid
oo_DEFINE_CLASS (CouplingGrid, Function)

	oo_OBJECT (FormantGrid, 0, tracheal_formants)
	oo_OBJECT (FormantGrid, 0, tracheal_antiformants)
	oo_COLLECTION (Ordered, tracheal_formants_amplitudes, IntensityTier, 0)
	oo_OBJECT (FormantGrid, 0, delta_formants)
	#if !oo_READING && !oo_WRITING
		oo_OBJECT (RealTier, 0, glottisOpenDurations)
	#endif

oo_END_CLASS (CouplingGrid)
#undef ooSTRUCT

#define ooSTRUCT FricationGrid
oo_DEFINE_CLASS (FricationGrid, Function)

	oo_OBJECT (IntensityTier, 0, noise_amplitude) // dB
	oo_OBJECT (FormantGrid, 0, formants)
	oo_COLLECTION (Ordered, formants_amplitudes, RealTier, 0)
	oo_OBJECT (IntensityTier, 0, bypass) // dB
	
oo_END_CLASS (FricationGrid)
#undef ooSTRUCT

#define ooSTRUCT KlattGrid
oo_DEFINE_CLASS (KlattGrid, Function)

	oo_OBJECT (PhonationGrid, 0, phonation) // Glottal source
	oo_OBJECT (VocalTractGrid, 0, vocalTract) // Filter
	oo_OBJECT (CouplingGrid, 0, coupling) // // Coupling between source and filter
	oo_OBJECT (FricationGrid, 0, frication) // Frication source
	oo_OBJECT (IntensityTier, 0, gain) // final scaling
	
oo_END_CLASS (KlattGrid)
#undef ooSTRUCT
