/* EEGAnalysisArea.cpp
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

#include "EEGAnalysisArea.h"

Thing_implement (EEGAnalysisArea, SoundAnalysisArea, 0);

#include "Prefs_define.h"
#include "EEGAnalysisArea_prefs.h"
#include "Prefs_install.h"
#include "EEGAnalysisArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "EEGAnalysisArea_prefs.h"

/* End of file EEGAnalysisArea.cpp */
