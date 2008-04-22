/* glplib12.c (shared library support) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000, 01, 02, 03, 04, 05, 06, 07, 08 Andrew Makhorin,
*  Department for Applied Informatics, Moscow Aviation Institute,
*  Moscow, Russia. All rights reserved. E-mail: <mao@mai2.rcnet.ru>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "glplib.h"

/**********************************************************************/

#if defined(HAVE_LTDL)

/* GNU version */

#include <ltdl.h>

void *xdlopen(const char *module)
{     void *h = NULL;
      if (lt_dlinit() != 0)
      {  lib_err_msg(lt_dlerror());
         goto done;
      }
      h = lt_dlopen(module);
      if (h == NULL)
      {  lib_err_msg(lt_dlerror());
         if (lt_dlexit() != 0)
            xerror("xdlopen: %s\n", lt_dlerror());
      }
done: return h;
}

void *xdlsym(void *h, const char *symbol)
{     void *ptr;
      xassert(h != NULL);
      ptr = lt_dlsym(h, symbol);
      if (ptr == NULL)
         xerror("xdlsym: %s: %s\n", symbol, lt_dlerror());
      return ptr;
}

void xdlclose(void *h)
{     xassert(h != NULL);
      if (lt_dlclose(h) != 0)
         xerror("xdlclose: %s\n", lt_dlerror());
      if (lt_dlexit() != 0)
         xerror("xdlclose: %s\n", lt_dlerror());
      return;
}

/**********************************************************************/

#elif defined(HAVE_DLFCN)

/* POSIX version */

#include <dlfcn.h>

void *xdlopen(const char *module)
{     void *h;
      h = dlopen(module, RTLD_NOW);
      if (h == NULL)
         lib_err_msg(dlerror());
      return h;
}

void *xdlsym(void *h, const char *symbol)
{     void *ptr;
      xassert(h != NULL);
      ptr = dlsym(h, symbol);
      if (ptr == NULL)
         xerror("xdlsym: %s: %s\n", symbol, dlerror());
      return ptr;
}

void xdlclose(void *h)
{     xassert(h != NULL);
      if (dlclose(h) != 0)
         xerror("xdlclose: %s\n", dlerror());
      return;
}

/**********************************************************************/

#elif defined(__WOE32__)

/* Windows version */

#include <windows.h>

void *xdlopen(const char *module)
{     void *h;
      h = LoadLibrary(module);
      if (h == NULL)
      {  char msg[20];
         sprintf(msg, "Error %d", GetLastError());
         lib_err_msg(msg);
      }
      return h;
}

void *xdlsym(void *h, const char *symbol)
{     void *ptr;
      xassert(h != NULL);
      ptr = GetProcAddress(h, symbol);
      if (ptr == NULL)
         xerror("xdlsym: %s: Error %d\n", symbol, GetLastError());
      return ptr;
}

void xdlclose(void *h)
{     xassert(h != NULL);
      if (!FreeLibrary(h))
         xerror("xdlclose: Error %d\n", GetLastError());
      return;
}

/**********************************************************************/

#else

/* NULL version */

void *xdlopen(const char *module)
{     xassert(module == module);
      lib_err_msg("Shared libraries not supported");
      return NULL;
}

void *xdlsym(void *h, const char *symbol)
{     xassert(h != h);
      xassert(symbol != symbol);
      return NULL;
}

void xdlclose(void *h)
{     xassert(h != h);
      return;
}

#endif

/* eof */
