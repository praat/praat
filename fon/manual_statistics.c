/* manual_statistics.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 */

#include "ManPagesM.h"

void manual_statistics_init (ManPages me);
void manual_statistics_init (ManPages me) {

MAN_BEGIN ("Statistics", "ppgb", 20020624)
INTRO ("This is the tutorial about basic statistical techniques in Praat, which work "
	"with the @Table object. It assumes that you are familiar with the @Intro.")
NORMAL ("(Under construction..................)")
NORMAL ("For more sophisticated techniques, see:")
LIST_ITEM ("\\bu @@Principal component analysis@")
LIST_ITEM ("\\bu @@Multidimensional scaling@")
LIST_ITEM ("\\bu @@Discriminant analysis@")
MAN_END

}

/* End of file manual_statistics.c */
