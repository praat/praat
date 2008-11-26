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

#define ooSTRUCT FormantGridP
oo_DEFINE_CLASS (FormantGridP, FormantGrid)

	oo_COLLECTION (Ordered, amplitudes, RealTier, 0)
	
oo_END_CLASS (FormantGridP)
#undef ooSTRUCT

#define ooSTRUCT PhonationGrid
oo_DEFINE_CLASS (PhonationGrid, Function)

	oo_OBJECT (PitchTier, 0, f0)
	oo_OBJECT (IntensityTier, 0, amplitudeOfVoicing) // dB
	oo_OBJECT (RealTier, 0, openPhase) // (0,1)
	oo_OBJECT (RealTier, 0, collisionPhase) //
	oo_OBJECT (RealTier, 0, power1)
	oo_OBJECT (RealTier, 0, power2)
	oo_OBJECT (RealTier, 0, flutter) // [0,1]
	oo_OBJECT (RealTier, 0, doublePulsing) // [0,1]
	oo_OBJECT (IntensityTier, 0, extraSpectralTilt) // dB
	oo_OBJECT (IntensityTier, 0, aspirationAmplitude) // dB
	oo_OBJECT (IntensityTier, 0, breathyness) // dB
	
oo_END_CLASS (PhonationGrid)
#undef ooSTRUCT


#define ooSTRUCT KlattGrid
oo_DEFINE_CLASS (KlattGrid, Function)
	// Filter
	oo_INT (klatt88)
	oo_OBJECT (FormantGridP, 0, formants)
	oo_OBJECT (FormantGridP, 0, nasal_formants)
	oo_OBJECT (FormantGrid, 0, nasal_antiformants)
	oo_OBJECT (FormantGridP, 0, tracheal_formants)
	oo_OBJECT (FormantGrid, 0, tracheal_antiformants)
	oo_OBJECT (FormantGridP, 0, frication_formants)
	// Coupling between source and filter
	oo_OBJECT (RealTier, 0, open_glottis_delta_f1) // Hz
	oo_OBJECT (RealTier, 0, open_glottis_delta_b1) // Hz
	// Glottal source
	oo_OBJECT (PhonationGrid, 0, source)
	// Frication source
	oo_OBJECT (IntensityTier, 0, fricationAmplitude) // dB
	oo_OBJECT (IntensityTier, 0, bypassAmplitude) // dB
	// Output
	oo_OBJECT (IntensityTier, 0, gain) // dB
	
oo_END_CLASS (KlattGrid)
#undef ooSTRUCT
