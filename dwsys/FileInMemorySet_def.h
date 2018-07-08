/* FileInMemorySet_def.h
 *
 * Copyright (C) 2017 David Weenink
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

#define ooSTRUCT FileInMemorySet
oo_DEFINE_CLASS (FileInMemorySet, SortedSet)

	#if oo_DECLARING
		static int s_compareHook (Daata f1, Daata f2) noexcept {
			FileInMemory me = static_cast<FileInMemory> (f1), thee = static_cast<FileInMemory> (f2);
			return Melder_cmp (my d_path.get(), thy d_path.get());
		}
		CompareHook v_getCompareHook () override { return s_compareHook; }

		void v_info ()
			override;
	#endif

oo_END_CLASS (FileInMemorySet)
#undef ooSTRUCT

/* End of file FileInMemorySet_def.h */
