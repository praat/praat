/* FileInMemoryManager_def.h
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

#define ooSTRUCT FileInMemoryManager
oo_DEFINE_CLASS (FileInMemoryManager, Daata)

	oo_OBJECT (FileInMemorySet, 0, files)
	oo_OBJECT (FileInMemorySet, 0, openFiles)
	oo_INTEGER (errorNumber)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif
	
oo_END_CLASS (FileInMemoryManager)
#undef ooSTRUCT

/* End of file FileInMemoryManager_def.h */
