/* Pitch_def.h
 *
 * Copyright (C) 1992-2006,2008,2011,2012,2015-2020 Paul Boersma
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

/* Attributes:
	xmin			// Start time (seconds).
	xmax > xmin		// End time (seconds).
	nx >= 1			// Number of time slices.
	dx > 0.0		// Time step (seconds).
	x1			// Centre of first time slice (seconds).
	ceiling			// Candidates with a higher frequency are unvoiced.
	maxnCandidates >= 1	// Maximum number of candidates per time slice.
	frame[1..nx].nCandidates	// Number of candidates in each time slice, including the unvoiced candidate.
	frame[1..nx].candidate[1..nCandidates].frequency
		// The frequency of each candidate (Hz), 0 means aperiodic or silent.
		// candidate[1].frequency is the frequency of the currently best candidate.
	frame[1..nx].candidate[1..nCandidates].strength
		// The strength of each candidate, a real number between 0 and 1:
		// 0 means not periodic at all, 1 means perfectly periodic;
		// if the frequency of the candidate is 0, its strength is a real number
		// that represents the maximum periodicity that
		// can still be considered to be due to noise (e.g., 0.4).
		// candidate[1].strength is the strength of the currently best candidate.
	frame[1..nx].intensity
		// The relative intensity of each frame, a real number between 0 and 1.
*/


#define ooSTRUCT Pitch_Candidate
oo_DEFINE_STRUCT (Pitch_Candidate)

	oo_DOUBLE (frequency)
	oo_DOUBLE (strength)

oo_END_STRUCT (Pitch_Candidate)
#undef ooSTRUCT


#define ooSTRUCT Pitch_Frame
oo_DEFINE_STRUCT (Pitch_Frame)

	#if oo_READING_BINARY
		oo_VERSION_UNTIL (0)
			oo_INT16 (nCandidates)
			oo_FLOAT (intensity)
		oo_VERSION_ELSE_UNTIL (1)
			oo_FLOAT (intensity)
			oo_INTEGER (nCandidates)
		oo_VERSION_ELSE
			oo_DOUBLE (intensity)
			oo_INTEGER (nCandidates)
		oo_VERSION_END
	#else
		oo_DOUBLE (intensity)
		oo_INTEGER (nCandidates)
	#endif

	oo_STRUCTVEC (Pitch_Candidate, candidates, nCandidates)

oo_END_STRUCT (Pitch_Frame)
#undef ooSTRUCT


#define ooSTRUCT Pitch
oo_DEFINE_CLASS (Pitch, Sampled)

	oo_DOUBLE (ceiling)
	oo_INT16 (maxnCandidates)
	oo_STRUCTVEC (Pitch_Frame, frames, nx)

	#if oo_DECLARING
		void v_info ()
			override;
		int v_domainQuantity ()
			override { return MelderQuantity_TIME_SECONDS; }
		int v_getMinimumUnit (integer level)
			override;
		int v_getMaximumUnit (integer level)
			override;
		conststring32 v_getUnitText (integer level, int unit, uint32 flags)
			override;
		bool v_isUnitLogarithmic (integer level, int unit)
			override;
		double v_convertStandardToSpecialUnit (double value, integer level, int unit)
			override;
		double v_convertSpecialToStandardUnit (double value, integer level, int unit)
			override;
		double v_getValueAtSample (integer sampleNumber, integer level, int unit)
			override;
		conststring32 v_getIndexText () const
			override { return U"frame number"; }
		conststring32 v_getNxText () const
			override { return U"the number of frames"; }
	#endif

oo_END_CLASS (Pitch)
#undef ooSTRUCT


/* End of file Pitch_def.h */
