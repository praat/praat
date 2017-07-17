/* sendsocket.c
 *
 * Copyright (C) 1999-2006,2016 Paul Boersma
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

#include <stdlib.h>
#ifdef NO_NETWORK
	/*
		If Praat is to be linked statically for reasons of having old C and C++ libraries
		on the target system, then it cannot use networking functions, because these tend
		to require the library version that was used for linking (on Linux).
	*/
#else
	#include <string.h>
	#include <stdio.h>
	#if defined (UNIX) || defined (macintosh)
		#include <sys/types.h>
		#include <unistd.h>
		#include <ctype.h>
		#include <netdb.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
	#elif defined (_WIN32)
		#include <windows.h>
		#include <winsock.h>
	#endif
#endif
#include "sendsocket.h"

char * sendsocket (const char *hostNameAndPort, const char *command) {
#ifdef NO_NETWORK
	return NULL;
#else
	static char result [200];
	char hostName [61], *colon;
	int port;
	if (strlen (hostNameAndPort) > 60)
		return "Cannot send to socket because the host-name-and-port string is too long.";
	strcpy (hostName, hostNameAndPort);
	colon = strchr (hostName, ':');
	if (colon == NULL)
		return "Cannot send to socket because a colon is missing.\nHost name and port should be in the format \"hostName:port\".";
	*colon = '\0';
	port = atoi (colon + 1);
{
	struct hostent *host;
	struct sockaddr_in his_addr;   // address of remote host
	int stat, sokket = -1;
	long bytesSent;
	result [0] = '\0';   // no error yet

	#ifdef _WIN32
	{
		static int initialized;
		if (! initialized) {
			WSADATA wsaData;
			WORD requestedWinsockVersion = MAKEWORD (1,1);   // require version 1.1
			if (WSAStartup (requestedWinsockVersion, & wsaData)) {
				return "Cannot send to socket because the socket library (WINSOCK.DLL) is not available, "
					"too old, or otherwise unusable.";
			}
			initialized = 1;
		}
	}
	#endif

	if (isdigit (hostName [0])) {
		if ((his_addr. sin_addr.s_addr = inet_addr ((char *) hostName)) == 0xFFFFFFFF) {
			sprintf (result, "Cannot send to socket because the hostname \"%s\" is invalid.", hostName);
			return result;
		}
		his_addr. sin_family = AF_INET;
	} else if ((host = gethostbyname (hostName)) != NULL) {
		his_addr. sin_family = host -> h_addrtype;
		memcpy (& his_addr. sin_addr, host -> h_addr, host -> h_length);
	} else {
		sprintf (result, "Cannot send to socket because the host \"%s\" is unknown.", hostName);
		return result;
	}
	his_addr. sin_port = htons (port);

	sokket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sokket < 0) {
		strcpy (result, "Cannot send to socket because the socket cannot be created.");
		goto end;
	}

	stat = connect (sokket, (struct sockaddr *) & his_addr, sizeof (struct sockaddr_in));
	if (stat != 0) {
		strcpy (result, "Cannot send to socket because the connection cannot be made.");
		goto end;
	}

	/*
		Melder_casual (U"Connected to port ", ntohs (his_addr. sin_port), U" on host ", Melder_peek8to32 (inet_ntoa (his_addr. sin_addr)), U".");
	*/
   
	bytesSent = send (sokket, command, strlen (command) + 1, 0);   /* Include null byte. */
	if (bytesSent < 0) {
		strcpy (result, "Data not sent to socket.");
		goto end;
	}
	/*
		Melder_casual (U"sendsocket: ", bytesSent, U" bytes written.");
	*/
end:
	if (sokket >= 0) {
		#if defined (UNIX) || defined (macintosh)
			close (sokket);
		#elif defined (_WIN32)
			closesocket (sokket);
		#endif
	}
}
	return result [0] == '\0' ? NULL: result;
#endif
}

/* End of file sendsocket.c */
