/* Pitch_to_PointProcess.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Pitch.h"
#include "PointProcess.h"
#include "Sound.h"

autoPointProcess Pitch_to_PointProcess (Pitch pitch);

autoPointProcess Sound_Pitch_to_PointProcess_cc (Sound sound, Pitch pitch);

autoPointProcess Sound_Pitch_to_PointProcess_peaks (Sound sound, Pitch pitch, int includeMaxima, int includeMinima);

/* End of file Pitch_to_PointProcess.h */
