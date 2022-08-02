#ifndef _PointArea_h_
#define _PointArea_h_
/* PointArea.h
 *
 * Copyright (C) 1992-2011,2012,2015,2016,2018,2022 Paul Boersma
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
#include "PointProcess.h"

Thing_declare (SoundArea);

Thing_define (PointArea, FunctionArea) {
	PointProcess pointProcess() { return static_cast <PointProcess> (our function()); }
	SoundArea borrowedSoundArea = nullptr;

	GuiObject addPointAtDialog;

	void v_createMenus ()
		override;
	void v_drawInside ()
		override;
};
DEFINE_FunctionArea_create (PointArea, PointProcess)

/* End of file PointArea.h */
#endif
