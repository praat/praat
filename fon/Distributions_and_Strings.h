/* Distributions_and_Strings.h
 *
 * Copyright (C) 1997-2002 Paul Boersma
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
 * pb 1999/02/17
 * pb 2002/07/16 GPL
 */

#ifndef _Distributions_h_
	#include "Distributions.h"
#endif
#ifndef _Strings_h_
	#include "Strings.h"
#endif

Strings Distributions_to_Strings (Distributions me, long column, long numberOfStrings);
Strings Distributions_to_Strings_exact (Distributions me, long column);
Distributions Strings_to_Distributions (Strings me);

/* End of file Distributions_and_Strings.h */
