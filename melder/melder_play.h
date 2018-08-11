#ifndef _melder_play_h_
#define _melder_play_h_
/* melder_play.h
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

int Melder_record (double duration);
int Melder_recordFromFile (MelderFile file);
void Melder_play ();
void Melder_playReverse ();
int Melder_publishPlayed ();
void Melder_beep ();

void Melder_setRecordProc (int (*record) (double));
void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile));
void Melder_setPlayProc (void (*play) ());
void Melder_setPlayReverseProc (void (*playReverse) ());
void Melder_setPublishPlayedProc (int (*publishPlayed) ());

/* End of file melder_play.h */
#endif
