#ifndef _PitchArea_h_
#define _PitchArea_h_
/* PitchArea.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015,2016,2018,2020,2022 Paul Boersma
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
#include "Pitch.h"

Thing_define (PitchArea, FunctionArea) {
	Pitch pitch() { return static_cast <Pitch> (our function()); }

	void v_drawInside ()
		override;
	void v_createMenus ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
		override;
};
DEFINE_FunctionArea_create (PitchArea, Pitch)

/* End of file PitchArea.h */
#endif
