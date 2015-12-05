#ifndef _RunnerMFC_h_
#define _RunnerMFC_h_
/* RunnerMFC.h
 *
 * Copyright (C) 2001-2011,2012,2015 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Editor.h"
#include "ExperimentMFC.h"

Thing_define (RunnerMFC, Editor) {
	GuiDrawingArea d_drawingArea;
	autoOrdered experiments;
	long iexperiment;
	autoGraphics graphics;
	long numberOfReplays;

	void v_destroy ()
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

autoRunnerMFC RunnerMFC_create (const char32 *title, autoOrdered experiments);

/* End of file RunnerMFC.h */
#endif
