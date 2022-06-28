#ifndef _SoundAnalysisArea_h_
#define _SoundAnalysisArea_h_
/* SoundAnalysisArea.h
 *
 * Copyright (C) 1992-2005,2007-2022 Paul Boersma
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

#include "FunctionArea.h"
#include "Preferences.h"
#include "Sound.h"

//#include "SoundAnalysisArea_enums.h"

Thing_define (SoundAnalysisArea, FunctionArea) {
	Sound sound() { return static_cast <Sound> (our function()); }
};

DEFINE_FunctionArea_create (SoundAnalysisArea, Sound)

/* End of file SoundAnalysisArea.h */
#endif
