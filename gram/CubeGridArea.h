#ifndef _CubeGridArea_h_
#define _CubeGridArea_h_
/* CubeGridArea.h
 *
 * Copyright (C) 2023 Paul Boersma
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
#include "CubeGrid.h"

Thing_define (CubeGridArea, FunctionArea) {
	CubeGrid cubeGrid () { return static_cast <CubeGrid> (our function()); }

	void v_drawInside ()
		override;
	void v_createMenus ()
		override;
};
DEFINE_FunctionArea_create (CubeGridArea, CubeGrid)

/* End of file CubeGridArea.h */
#endif
