/* Manipulation_def.h
 *
 * Copyright (C) 1992-2003 Paul Boersma
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
 * pb 2002/03/30
 * pb 2002/07/16 GPL
 * pb 2003/02/17 removed pitch analysis properties
 */

#define ooSTRUCT Manipulation
oo_DEFINE_CLASS (Manipulation, Function)

	oo_OBJECT (Sound, sound)
	oo_OBJECT (PointProcess, pulses)
	oo_OBJECT (PitchTier, pitch)

	oo_FROM (1)
		oo_OBJECT (IntensityTier, dummyIntensity)
	oo_ENDFROM

	oo_FROM (2)
		oo_OBJECT (DurationTier, duration)
	oo_ENDFROM

	oo_FROM (3)
		/*
		 * Make sure that the spectrogram is not written,
		 * but allow it to be read (a legacy of writing but not reading the version 3 stuff).
		 */
		#if oo_WRITING
			{ Image save = my dummySpectrogram; my dummySpectrogram = NULL;
		#endif
		oo_OBJECT (Image, dummySpectrogram)
		#if oo_WRITING
			my dummySpectrogram = save; }
		#endif
		oo_OBJECT (FormantTier, dummyFormantTier)
		oo_OBJECT (Data, dummy1)
		oo_OBJECT (Data, dummy2)
		oo_OBJECT (Data, dummy3)
	oo_ENDFROM

	oo_FROM (4)
		oo_DOUBLE (dummy10)
		oo_OBJECT (Pitch, dummyPitchAnalysis)
		oo_DOUBLE (dummy11)
		oo_DOUBLE (dummy12)
		oo_OBJECT (Intensity, dummyIntensityAnalysis)
		oo_VERSION (1) oo_OBJECT (Formant, dummyFormantAnalysis)
		oo_INT (dummy4)
		oo_DOUBLE (dummy5)
		oo_DOUBLE (dummy6)
		oo_DOUBLE (dummy7)
		oo_DOUBLE (dummy8)
		oo_DOUBLE (dummy9)
	oo_ENDFROM

	#if !oo_READING && !oo_WRITING
		oo_OBJECT (LPC, lpc)
	#endif

oo_END_CLASS (Manipulation)
#undef ooSTRUCT

/* End of file Manipulation_def.h */
