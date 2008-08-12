#ifndef _Misc_h_
#define _Misc_h_

/* Misc.h
 *
 * Copyright (C) 2007-2008 Ola Söder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $URL: svn://pegasos.dyndns.biz/praat/trunk/kNN/OlaP.h $
 * $Rev: 137 $
 * $Author: stix $
 * $Date: 2008-08-10 19:34:07 +0200 (Sun, 10 Aug 2008) $
 * $Id: OlaP.h 137 2008-08-10 17:34:07Z stix $
 */

/*
 * os 20080529 Initial release
 */

/////////////////////////////////////////
// Macros                              //
/////////////////////////////////////////

#define LONGARRAYSWAP(a, x, y) {long temp = a[x]; a[x] = a[y]; a[y] = temp;}
#define LONGSWAP(x, y) {long temp = x; x = y; y = temp;}
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define SQUARE(x) ((x) * (x))				

/////////////////////////////////////////
// Misc defines                        //
/////////////////////////////////////////

#define MINFLOAT 0.0000000000000000000001

#endif /* _Misc_h_ */
