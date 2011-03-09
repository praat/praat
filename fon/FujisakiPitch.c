/* FujisakiPitch.c
 *
 * Copyright (C) 2002-2007 Paul Boersma
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

class_methods (FujisakiCommand, Function) {
	class_method_local (FujisakiCommand, destroy)
	class_method_local (FujisakiCommand, copy)
	class_method_local (FujisakiCommand, equal)
	class_method_local (FujisakiCommand, canWriteAsEncoding)
	class_method_local (FujisakiCommand, writeText)
	class_method_local (FujisakiCommand, readText)
	class_method_local (FujisakiCommand, writeBinary)
	class_method_local (FujisakiCommand, readBinary)
	class_method_local (FujisakiCommand, description)
	class_methods_end
}

FujisakiCommand FujisakiCommand_create (double tmin, double tmax, double amplitude) {
	FujisakiCommand me = Thing_new (FujisakiCommand);
	if (! me) return NULL;
	Function_init (me, tmin, tmax);
	my amplitude = amplitude;
	return me;
}

class_methods (FujisakiPitch, Function) {
	class_method_local (FujisakiPitch, destroy)
	class_method_local (FujisakiPitch, copy)
	class_method_local (FujisakiPitch, equal)
	class_method_local (FujisakiPitch, canWriteAsEncoding)
	class_method_local (FujisakiPitch, writeText)
	class_method_local (FujisakiPitch, readText)
	class_method_local (FujisakiPitch, writeBinary)
	class_method_local (FujisakiPitch, readBinary)
	class_method_local (FujisakiPitch, description)
	class_methods_end
}

FujisakiPitch FujisakiPitch_create (double tmin, double tmax,
	double baseFrequency, double alpha, double beta, double gamma)
{
	FujisakiPitch me = Thing_new (FujisakiPitch);
	if (! me) return NULL;
	Function_init (me, tmin, tmax);
	my baseFrequency = baseFrequency;
	my alpha = alpha;
	my beta = beta;
	my gamma = gamma;
	if ((my phraseCommands = SortedSetOfDouble_create ()) == NULL) return NULL;
	if ((my accentCommands = SortedSetOfDouble_create ()) == NULL) return NULL;
	return me;
}

FujisakiPitch Pitch_to_FujisakiPitch (Pitch me, double gamma, double timeResolution,
	FujisakiPitch *intermediate1, FujisakiPitch *intermediate2, FujisakiPitch *intermediate3)
{
	FujisakiPitch thee = NULL;
	long i;
	(void) timeResolution;

	thee = FujisakiPitch_create (my xmin, my xmax, 0, 0, 0, gamma); cherror
	/*
	 * Get phrase commands.
	 */
	while (/* ... */ 0) {
		double onsetTime = /* ... */ 0.0;
		double offsetTime = /* ... */ 3.0;
		double amplitude = /* ... */ 1.0;
		FujisakiCommand phraseCommand = FujisakiCommand_create (onsetTime, offsetTime, amplitude);
		Collection_addItem (thy phraseCommands, phraseCommand); cherror
	}
	if (intermediate1) *intermediate1 = Data_copy (thee);
	/*
	 * Get accent commands.
	 */
	while (/* ... */ 0) {
		double onsetTime = /* ... */ 0.0;
		double offsetTime = /* ... */ 3.0;
		double amplitude = /* ... */ 1.0;
		FujisakiCommand accentCommand = FujisakiCommand_create (onsetTime, offsetTime, amplitude);
		Collection_addItem (thy accentCommands, accentCommand); cherror
	}
	if (intermediate2) *intermediate2 = Data_copy (thee);
	/*
	 * Do some extra processing.
	 */
	/* ... */
	if (intermediate3) *intermediate3 = Data_copy (thee);
	/*
	 * Tidy up.
	 */
	for (i = 1; i <= thy phraseCommands -> size; i ++) {
		FujisakiCommand phraseCommand = thy phraseCommands -> item [i];
		/* ... */
	}
	for (i = 1; i <= thy accentCommands -> size; i ++) {
		FujisakiCommand accentCommand = thy accentCommands -> item [i];
		/* ... */
	}
end:
	iferror {
		forget (thee);
		return Melder_errorp ("(Pitch_to_FujisakiPitch:) Not performed.");
	}
	return thee;
}

/* End of file FujisakiPitch.c */
