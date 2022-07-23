/* TimeSoundEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "TimeSoundEditor.h"

Thing_implement_pureVirtual (TimeSoundEditor, FunctionEditor, 0);

/*
	TimeSoundEditor is designed to be removed.
	Most of its remaining functionality is to just delegate everything to SoundArea.
	At some point, editors will just have to include a SoundArea instead of deriving from TimeSoundEditor.
*/

/* End of file TimeSoundEditor.cpp */
