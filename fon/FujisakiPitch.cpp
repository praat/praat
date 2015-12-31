/* FujisakiPitch.cpp
 *
 * Copyright (C) 2002-2011,2015 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2007/10/01 can write as encoding
 * pb 2011/05/27 C++
 */

#include "FujisakiPitch.h"

#include "oo_DESTROY.h"
#include "FujisakiPitch_def.h"
#include "oo_COPY.h"
#include "FujisakiPitch_def.h"
#include "oo_EQUAL.h"
#include "FujisakiPitch_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FujisakiPitch_def.h"
#include "oo_WRITE_TEXT.h"
#include "FujisakiPitch_def.h"
#include "oo_READ_TEXT.h"
#include "FujisakiPitch_def.h"
#include "oo_WRITE_BINARY.h"
#include "FujisakiPitch_def.h"
#include "oo_READ_BINARY.h"
#include "FujisakiPitch_def.h"
#include "oo_DESCRIPTION.h"
#include "FujisakiPitch_def.h"

Thing_implement (FujisakiCommand, Function, 0);

autoFujisakiCommand FujisakiCommand_create (double tmin, double tmax, double amplitude) {
	try {
		autoFujisakiCommand me = Thing_new (FujisakiCommand);
		Function_init (me.peek(), tmin, tmax);
		my amplitude = amplitude;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Fujisaki command not created.");
	}
}

Thing_implement (FujisakiPitch, Function, 0);

autoFujisakiPitch FujisakiPitch_create (double tmin, double tmax,
	double baseFrequency, double alpha, double beta, double gamma)
{
	try {
		autoFujisakiPitch me = Thing_new (FujisakiPitch);
		Function_init (me.peek(), tmin, tmax);
		my baseFrequency = baseFrequency;
		my alpha = alpha;
		my beta = beta;
		my gamma = gamma;
		return me;
	} catch (MelderError) {
		Melder_throw (U"FujisakiPitch not created.");
	}
}

autoFujisakiPitch Pitch_to_FujisakiPitch (Pitch me, double gamma, double /* timeResolution */,
	autoFujisakiPitch *intermediate1, autoFujisakiPitch *intermediate2, autoFujisakiPitch *intermediate3)
{
	try {
		autoFujisakiPitch thee = FujisakiPitch_create (my xmin, my xmax, 0, 0, 0, gamma);
		/*
		 * Get phrase commands.
		 */
		while (/* ... */ 0) {
			double onsetTime = /* ... */ 0.0;
			double offsetTime = /* ... */ 3.0;
			double amplitude = /* ... */ 1.0;
			autoFujisakiCommand phraseCommand = FujisakiCommand_create (onsetTime, offsetTime, amplitude);
			thy phraseCommands. addItem_move (phraseCommand.move());
		}
		if (intermediate1) *intermediate1 = Data_copy (thee.peek());
		/*
		 * Get accent commands.
		 */
		while (/* ... */ 0) {
			double onsetTime = /* ... */ 0.0;
			double offsetTime = /* ... */ 3.0;
			double amplitude = /* ... */ 1.0;
			autoFujisakiCommand accentCommand = FujisakiCommand_create (onsetTime, offsetTime, amplitude);
			thy accentCommands. addItem_move (accentCommand.move());
		}
		if (intermediate2) *intermediate2 = Data_copy (thee.peek());
		/*
		 * Do some extra processing.
		 */
		/* ... */
		if (intermediate3) *intermediate3 = Data_copy (thee.peek());
		/*
		 * Tidy up.
		 */
		for (long i = 1; i <= thy phraseCommands.size(); i ++) {
			FujisakiCommand phraseCommand = thy phraseCommands.at [i];
			/* ... */
		}
		for (long i = 1; i <= thy accentCommands.size(); i ++) {
			FujisakiCommand accentCommand = thy accentCommands.at [i];
			/* ... */
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to FujisakiPitch.");
	}
}

/* End of file FujisakiPitch.cpp */
