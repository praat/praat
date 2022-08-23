/* RealTierEditor.cpp
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

#include "RealTierEditor.h"
#include "EditorM.h"

Thing_implement (RealTierEditor, FunctionEditor, 0);

autoRealTierEditor RealTierEditor_create (conststring32 title, RealTier realTier, Sound optionalSoundToCopy) {
	try {
		autoRealTierEditor me = Thing_new (RealTierEditor);
		my realTierArea() = RealTierArea_create (true, nullptr, me.get());
		if (optionalSoundToCopy)
			my soundArea() = SoundArea_create (false, optionalSoundToCopy, me.get());
		FunctionEditor_init (me.get(), title, realTier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"RealTier window not created.");
	}
}

/* End of file RealTierEditor.cpp */
