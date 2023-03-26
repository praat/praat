/* FormantPathEditor_prefs.h
 *
 * Copyright (C) 2020-2023 David Weenink, 2022 Paul Boersma
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

Prefs_begin (FormantPathEditor)

	ClassPrefs_overrideInt        (FormantPathEditor, shellWidth,                       1, U"800")
	ClassPrefs_overrideInt        (FormantPathEditor, shellHeight,                      1, U"600")
	InstancePrefs_overrideBool    (FormantPathEditor, showSelectionViewer,              1, true)

	InstancePrefs_addInteger (FormantPathEditor, modeler_numberOfTracks,                1, U"3")
	InstancePrefs_addString  (FormantPathEditor, modeler_numberOfParametersPerTrack,    1, U"3 3 3")
	InstancePrefs_addDouble  (FormantPathEditor, modeler_varianceExponent,              1, U"1.25")
	InstancePrefs_addDouble  (FormantPathEditor, candidate_draw_maximumFrequency,       1, U"6200.0")
	InstancePrefs_addBool    (FormantPathEditor, candidate_draw_adjustSpectrogramView,  1, true)
	InstancePrefs_addBool    (FormantPathEditor, candidate_draw_estimatedModels,        1, true)
	InstancePrefs_addBool    (FormantPathEditor, candidate_draw_useMaximumCeiling,      1, true)
	InstancePrefs_addBool    (FormantPathEditor, candidate_draw_showBandwidths,         1, true)
	InstancePrefs_addDouble  (FormantPathEditor, candidate_draw_yGridLineEvery_Hz,      1, U"1000.0") // Hz
	
Prefs_end (FormantPathEditor)
	
/* End of file FormantPathEditor_prefs.h */
