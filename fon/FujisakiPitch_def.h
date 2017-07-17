/* FujisakiPitch_def.h
 *
 * Copyright (C) 2002 Paul Boersma & Hansjoerg Mixdorff
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


#define ooSTRUCT FujisakiCommand
oo_DEFINE_CLASS (FujisakiCommand, Function)

	oo_DOUBLE (amplitude)

oo_END_CLASS (FujisakiCommand)
#undef ooSTRUCT


#define ooSTRUCT FujisakiPitch
oo_DEFINE_CLASS (FujisakiPitch, Function)

	oo_DOUBLE (baseFrequency)
	oo_DOUBLE (alpha)
	oo_DOUBLE (beta)
	oo_DOUBLE (gamma)
	oo_COLLECTION_OF (SortedSetOfDoubleOf, phraseCommands, FujisakiCommand, 0)
	oo_COLLECTION_OF (SortedSetOfDoubleOf, accentCommands, FujisakiCommand, 0)

oo_END_CLASS (FujisakiPitch)
#undef ooSTRUCT


/* End of file FujisakiPitch_def.h */
