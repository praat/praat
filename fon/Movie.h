#ifndef _Movie_h_
#define _Movie_h_
/* Movie.h
 *
 * Copyright (C) 2011,2014,2015,2017 Paul Boersma
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

#include "Sampled.h"
#include "Graphics.h"
#include "Sound.h"
#include "Strings_.h"

#include "Movie_def.h"

autoMovie Movie_openFromSoundFile (MelderFile file);

void Movie_init (Movie me, autoSound sound, conststring32 folderName, autoStrings fileNames);

void Movie_paintOneImageInside (Movie me, Graphics graphics, integer frameNumber, double xmin, double xmax, double ymin, double ymax);

void Movie_paintOneImage (Movie me, Graphics graphics, integer frameNumber, double xmin, double xmax, double ymin, double ymax);

void Movie_play (Movie me, Graphics graphics, double tmin, double tmax, Sound_PlayCallback callback, Thing boss);

/* End of file Movie.h */
#endif
