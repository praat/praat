/* site.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2004/10/21 simplified print command
 */

#include "Resources.h"
#include <string.h>
#include "site.h"

static char printCommand [200] = "lp -c %s";

char * Site_getPrintCommand (void) { return printCommand; }

void Site_setPrintCommand (const char *text) { strcpy (printCommand, text); }

void Site_prefs (void) {
	Resources_addString ("Site.printCommand", printCommand);
}

/* End of file site.c */

