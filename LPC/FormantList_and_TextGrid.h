#ifndef _FormantList_and_TextGrid_h_
#define _FormantList_and_TextGrid_h_
/* FormantList_and_TextGrid.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Formant.h"
#include "FormantList.h"
#include "TextGrid.h"

integer TextGrid_and_FormantList_findLogTier (TextGrid me, FormantList thee);

autoFormant FormantList_and_TextGrid_to_Formant (FormantList me, TextGrid thee, integer logTierNumber);

#endif /* _FormantList_and_TextGrid_h_ */
