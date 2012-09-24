#ifndef _Movie_h_
#define _Movie_h_
/* Movie.h
 *
 * Copyright (C) 2011 Paul Boersma
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

#include "Sampled.h"
#include "Graphics.h"
#include "Sound.h"
#include "Strings_.h"

#include "Movie_def.h"
oo_CLASS_CREATE (Movie, Sampled);

Movie Movie_openFromSoundFile (MelderFile file);

/* End of file Movie.h */
#endif
