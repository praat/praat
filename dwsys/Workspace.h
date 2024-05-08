#ifndef _Workspace_h_
#define _GridTrace_h_
/* Workspace.h
 *
 * Copyright (C) 2024 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "melder.h"

typedef struct structWorkspace *Workspace;
struct structWorkspace {
private:
	
	integer start = 0, end = 0; // internal memory pointers
	autoVEC vworkspace; 		// the memory
	
public:
	
	void init (integer size) {
		vworkspace = raw_VEC (size);
		start = end = 0;
	}
	
	inline VEC rawVEC (integer size) {
		start = end + 1; end += size;
		Melder_require (end <= vworkspace.size,
			U"Workspace too small, you are asking for ", size, U" elements, available only ", vworkspace.size - start, U".");
		return vworkspace.part (start, end);
	}
	
	inline VEC zeroVEC (integer size) {
		VEC v = rawVEC (size);
		v  <<=  0.0;
		return v;
	}
	
	inline MAT rawMAT (integer nrow, ncol) {
		VEC v = rawVEC (nrow * ncol);
		return v.asmatrix (nrow, ncol);
	}
	inline MAT zeroMAT (integer nrow, ncol) {
		MAT m = rawMAT (nrow, ncol);
		m  <<=  0.0;
		return m;
	}
};

#endif /* Workspace.h */
