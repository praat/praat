#ifndef _sendsocket_h_
#define _sendsocket_h_
/* sendsocket.h
 *
 * Copyright (C) 1999-2011 Paul Boersma
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

#ifdef __cplusplus
	extern "C" {
#endif

char * sendsocket (const char *hostNameAndPort, const char *command);
/*
 * E.g. 'hostNameAndPort' could be "localhost:6667" or "fonsg19.hum.uva.nl:4711".
 */

#ifdef __cplusplus
	}
#endif

/* End of file sendsocket.h */
#endif
