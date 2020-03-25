/* FormantAnalyzer_prefs.h
 *
 * Copyright (C) 2020 David Weenink
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

prefs_begin (FormantAnalyzer)

	prefs_override_int         (FormantAnalyzer, shellWidth,				1, U"800")
	prefs_override_int         (FormantAnalyzer, shellHeight,				1, U"600")
	prefs_override_bool        (FormantAnalyzer, picture_garnish,			1, true)
	prefs_override_bool        (FormantAnalyzer, showSelectionViewer,		1, true)	
	prefs_add_double_with_data (FormantAnalyzer, modeler_minimumCeiling_Hz,	1, U"4000.0")
	prefs_add_double_with_data (FormantAnalyzer, modeler_maximumCeiling_Hz,	1, U"6000.0")
	prefs_add_int_with_data    (FormantAnalyzer, modeler_numberOfModels,	1, U"9")
	prefs_add_int_with_data    (FormantAnalyzer, modeler_numberOfParametersF1Track,	1, U"3")
	prefs_add_int_with_data    (FormantAnalyzer, modeler_numberOfParametersF2Track,	1, U"3")
	prefs_add_int_with_data    (FormantAnalyzer, modeler_numberOfParametersF3Track,	1, U"2")
	prefs_add_int_with_data    (FormantAnalyzer, modeler_numberOfParametersF4Track,	1, U"2")
	prefs_add_int_with_data    (FormantAnalyzer, modeler_numberOfParametersF5Track,	1, U"2")

prefs_end (FormantAnalyzer)

/* End of file FormantAnalyzer_prefs.h */
