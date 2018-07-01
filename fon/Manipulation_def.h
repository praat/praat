/* Manipulation_def.h
 *
 * Copyright (C) 1992-2006,2008,2011,2015-2018 Paul Boersma
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


#define ooSTRUCT Manipulation
oo_DEFINE_CLASS (Manipulation, Function)

	#if oo_READING
		oo_VERSION_UNTIL (4)
			oo_OBJECT (Sound, 0, sound)
		oo_VERSION_ELSE_UNTIL (5)
			oo_OBJECT (Sound, Melder_debug == 25 ? 2 : 0, sound)
		oo_VERSION_ELSE
			oo_OBJECT (Sound, 2, sound)
		oo_VERSION_END
	#else
		oo_OBJECT (Sound, 2, sound)
	#endif
	oo_OBJECT (PointProcess, 0, pulses)
	oo_OBJECT (PitchTier, 0, pitch)

	oo_FROM (1)
		oo_OBJECT (IntensityTier, 0, dummyIntensity)
	oo_ENDFROM

	oo_FROM (2)
		oo_OBJECT (DurationTier, 0, duration)
	oo_ENDFROM

	oo_FROM (3)
		/*
		 * Make sure that the spectrogram is not written,
		 * but allow it to be read (a legacy of writing but not reading the version 3 stuff).
		 */
		#if oo_WRITING
			{ autoImage save = dummySpectrogram.move();
		#endif
		oo_OBJECT (Image, 0, dummySpectrogram)
		#if oo_WRITING
			dummySpectrogram = save.move(); }
		#endif
		oo_OBJECT (FormantTier, 0, dummyFormantTier)
		oo_OBJECT (Daata, 0, dummy1)
		oo_OBJECT (Daata, 0, dummy2)
		oo_OBJECT (Daata, 0, dummy3)
	oo_ENDFROM

	oo_FROM (4)
		oo_DOUBLE (dummy10)
		oo_OBJECT (Pitch, 0, dummyPitchAnalysis)
		oo_DOUBLE (dummy11)
		oo_DOUBLE (dummy12)
		oo_OBJECT (Intensity, 0, dummyIntensityAnalysis)
		oo_OBJECT (Formant, 1, dummyFormantAnalysis)
		oo_INT16 (dummy4)
		oo_DOUBLE (dummy5)
		oo_DOUBLE (dummy6)
		oo_DOUBLE (dummy7)
		oo_DOUBLE (dummy8)
		oo_DOUBLE (dummy9)
	oo_ENDFROM

	#if ! oo_READING && ! oo_WRITING
		oo_OBJECT (LPC, 0, lpc)
	#endif

	#if oo_DECLARING
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		void v_shiftX (double xfrom, double xto)
			override;
		void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
			override;
	#endif

oo_END_CLASS (Manipulation)
#undef ooSTRUCT


/* End of file Manipulation_def.h */
