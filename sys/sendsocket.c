/* sendsocket.c
 *
 * Copyright (C) 1999-2006 Paul Boersma
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
 * pb 1999/12/21
 * pb 2002/03/07 GPL
 * pb 2003/12/19 return NULL if no error
 * pb 2004/10/11 re-included windows.h include file (undoes fix of CW5 include-file bug)
 * pb 2004/10/11 user-readable error messages
 * pb 2006/10/28 erased MacOS 9 stuff
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined (UNIX) || defined (macintosh)
	#if defined (macintosh)
		/* Prevent some warnings with MacOS X 10.4.2 / CodeWarrior 9.6 / Xcode 2.0 */
		#define GATEWAY 0
		#define SENDFILE 0
	#endif
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
#include "sendsocket.h"

char * sendsocket (const char *hostNameAndPort, const char *command) {
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
	struct sockaddr_in his_addr;   /* Address of remote host. */
	int stat, sokket = -1;
	long bytesSent;
	result [0] = '\0';   /* No error yet. */

	#ifdef _WIN32
	{
		static int initialized;
		if (! initialized) {
			WSADATA wsaData;
			WORD requestedWinsockVersion = MAKEWORD (1,1);   /* Require version 1.1. */
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
		Melder_casual ("Connected to port %d on host %s.", ntohs (his_addr. sin_port), inet_ntoa (his_addr. sin_addr));
	*/
   
	bytesSent = send (sokket, command, strlen (command) + 1, 0);   /* Include null byte. */
	if (bytesSent < 0) {
		strcpy (result, "Data not sent to socket.");
		goto end;
	}
	/*
		Melder_casual ("sendsocket: %d bytes written.", bytesSent);
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
}

/* End of file sendsocket.c */
