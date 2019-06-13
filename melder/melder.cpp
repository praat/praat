/* melder.cpp
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

#include "melder.h"

#include "enums_getText.h"
#include "melder_enums.h"
#include "enums_getValue.h"
#include "melder_enums.h"

#include "../dwsys/NUMmachar.h"
#include "../external/gsl/gsl_errno.h"
#ifdef macintosh
	#include <Carbon/Carbon.h>   // Gestalt
#endif

/********** Exported variables. **********/

bool Melder_batch;   // don't we have a GUI?- Set once at application start-up
bool Melder_backgrounding;   // are we running a script?- Set and unset dynamically
int32 Melder_systemVersion;

void Melder_init () {
	NUMmachar ();
	gsl_set_error_handler_off ();
	NUMrandom_init ();
	Melder_alloc_init ();
	Melder_message_init ();
	#ifdef macintosh
		SInt32 sys1, sys2, sys3;
		Gestalt ('sys1', & sys1);
		Gestalt ('sys2', & sys2);
		Gestalt ('sys3', & sys3);
		Melder_systemVersion = sys1 * 10000 + sys2 * 100 + sys3;
	#endif
}

/* End of file melder.cpp */
