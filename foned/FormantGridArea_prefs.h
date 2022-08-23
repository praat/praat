/* FormantGridArea_prefs.h
 *
 * Copyright (C) 2022 Paul Boersma
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

Prefs_begin (FormantGridArea)

	InstancePrefs_addDouble  (FormantGridArea, formantFloor,                    1, U"0.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, formantCeiling,                  1, U"11000.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, bandwidthFloor,                  1, U"0.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, bandwidthCeiling,                1, U"1000.0")   // Hz

	InstancePrefs_addDouble  (FormantGridArea, play_samplingFrequency,          1, U"44100.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, source_pitch_tStart,             1, U"0.0%")   // relative time
	InstancePrefs_addDouble  (FormantGridArea, source_pitch_f0Start,            1, U"150.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, source_pitch_tMid,               1, U"25.0%")   // relative time
	InstancePrefs_addDouble  (FormantGridArea, source_pitch_f0Mid,              1, U"180.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, source_pitch_tEnd,               1, U"100.0%")   // relative time
	InstancePrefs_addDouble  (FormantGridArea, source_pitch_f0End,              1, U"120.0")   // Hz
	InstancePrefs_addDouble  (FormantGridArea, source_phonation_adaptFactor,    1, U"1.0")
	InstancePrefs_addDouble  (FormantGridArea, source_phonation_maximumPeriod,  1, U"0.05")
	InstancePrefs_addDouble  (FormantGridArea, source_phonation_openPhase,      1, U"0.7")
	InstancePrefs_addDouble  (FormantGridArea, source_phonation_collisionPhase, 1, U"0.03")
	InstancePrefs_addDouble  (FormantGridArea, source_phonation_power1,         1, U"3.0")
	InstancePrefs_addDouble  (FormantGridArea, source_phonation_power2,         1, U"4.0")

Prefs_end (FormantGridArea)

/* End of file FormantGridArea_prefs.h */
