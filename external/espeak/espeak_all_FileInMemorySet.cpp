/* espeak_all_FileInMemorySet.cpp
 *
 * Copyright (C) 2017 David Weenink, 2024 Paul Boersma
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
 * ainteger with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "espeak_praat.h"

autoFileInMemorySet create_espeak_all_FileInMemorySet () {
	try{
		OrderedOf <structFileInMemorySet> list;
		list. addItem_move (create_espeak_phon_FileInMemorySet ());
		list. addItem_move (create_espeak_russianDict_FileInMemorySet ());
		list. addItem_move (create_espeak_faroeseDict_FileInMemorySet ());
		list. addItem_move (create_espeak_otherDicts_FileInMemorySet ());
		list. addItem_move (create_espeak_languages_FileInMemorySet ());
		list. addItem_move (create_espeak_voices_FileInMemorySet ());
		return FileInMemorySets_merge (list);
	} catch (MelderError) {
		Melder_throw (U"FileInMemorySet for eSpeak-NG not created.");
	}
}

/* End of file espeak_all_FileInMemorySet.cpp */
