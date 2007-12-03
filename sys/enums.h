#ifndef _enums_h_
#define _enums_h_
/* enums.h
 *
 * Copyright (C) 2007 Paul Boersma
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
 * pb 2007/12/02
 */

#define enums_begin(type,minimum)  enum { type##_MIN = minimum,
#define enums_add(type,which,text,value)  type##_##which = value,
#define enums_alt(type,which,text)
#define enums_end(type,def,maximum) type##_MAX = maximum, \
	type##_DEFAULT = type##_##def }; \
	const wchar_t * type##_getText (int value); \
	int type##_getValue (const wchar_t *text);

/* End of file enums.h */
#endif

