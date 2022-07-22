#ifndef _FormantPathArea_h_
#define _FormantPathArea_h_
/* FormantPathEditor.h
 *
 * Copyright (C) 2020-2022 David Weenink, 2022 Paul Boersma
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

#include "FormantPath.h"
#include "SoundAnalysisArea.h"

/*
	The explanation following is for analyses with different maximum formant frequencies (i.e. different ceilings).
	The <name>-log tier can have multiple intervals. Each interval shows a particular analysis prefered by the user. It shows first the <ceiling> that was selected by the user, then a ';' separator and finally the number of parameters per track of the formant modeler. 
	Its content could be, for example '5000; 5 5 5', which means that the analysis with a ceiling of 5000 Hz was chosen and the Formant modeler used F1, F2, and F3 in the modelling and reserved 5 coefficients to model F1, 5 coefficients to model F2 and 5 coefficients to model F3. The <ceiling> number must match one of the possible ceilings (rounded to integer Hz values)
	An empty interval always implies the default analysis. Therefore only intervals where you want a non-default have to be specified.

	There is no need to permanently store the FormantModelers because they can easily
	be calculated whenever they are needed from the information in the tiers.
	
	Multichannel sounds don't make sense with respect to the analysis part. If both channels are the same sound, one is redundant.
	If two different sounds, then average (convert to mono), as in other editors.
*/

Thing_define (FormantPathArea, SoundAnalysisArea) {
	FormantPath formantPath() const { return (FormantPath) our function(); }

	void v_draw_analysis_formants ()
		override;
	void v_createMenuItems_formant (EditorMenu menu)
		override;
	void v_reset_analysis () override {
		our d_spectrogram. reset();
		our d_pitch. reset();
		our d_intensity. reset();
		our d_pulses. reset();
		// not our formant!
	}

	#include "FormantPathArea_prefs.h"
};
DEFINE_FunctionArea_create (FormantPathArea, FormantPath)

/* End of file FormantPathArea.h */
#endif
