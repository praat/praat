#ifndef _OlaP_h_
#define _OlaP_h_

/* OlaP.h
 *
 * Copyright (C) 2007-2008 Ola Söder
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * os 20080529 Initial release
 */

/////////////////////////////////////////
// Macros                              //
/////////////////////////////////////////

#define OlaSWAP(TYPE, X, Y) {TYPE temp = X; X = Y; Y = temp;}
#define OlaMAX(x,y) ((x) > (y) ? (x) : (y))
#define OlaMIN(x,y) ((x) < (y) ? (x) : (y))
#define OlaSQUARE(x) ((x) * (x))			

/////////////////////////////////////////
// DEBUG                               //
/////////////////////////////////////////


/////////////////////////////////////////
// Misc defines                        //
/////////////////////////////////////////

#define kOla_MINFLOAT 0.0000000000000000000001

#endif /* _Misc_h_ */
