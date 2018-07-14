#ifndef _RunnerMFC_h_
#define _RunnerMFC_h_
/* RunnerMFC.h
 *
 * Copyright (C) 2001-2011,2012,2015,2017 Paul Boersma
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

#include "Editor.h"
#include "ExperimentMFC.h"

Thing_define (RunnerMFC, Editor) {
	GuiDrawingArea d_drawingArea;
	autoExperimentMFCList experiments;
	integer iexperiment;
	autoGraphics graphics;
	integer numberOfReplays;
	bool blanked;

	void v_destroy () noexcept
		override;
	bool v_editable ()
		override { return false; }
	bool v_scriptable ()
		override { return false; }
	void v_createChildren ()
		override;
	void v_dataChanged ()
		override;
};

autoRunnerMFC RunnerMFC_create (conststring32 title, autoExperimentMFCList experiments);

/* End of file RunnerMFC.h */
#endif
