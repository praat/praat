/* glpcli.h (command-line interface) */

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

#ifndef _GLPCLI_H
#define _GLPCLI_H

#include "glplib.h"

#ifndef _GLP_CLI
#define _GLP_CLI
typedef struct { double _cli; } CLI;
/* command-line interface object */
#endif

/* return codes: */
#define CLI_EXIT     1
#define CLI_ERROR    2

#define cli_create_it _glp_cli_create_it
CLI *cli_create_it(void);
/* create GLPK command-line interface */

#define cli_execute_cmd _glp_cli_execute_cmd
int cli_execute_cmd(CLI *cli, int (*func)(void *info), void *info);
/* execute GLPK command script (core routine) */

#define cli_delete_it _glp_cli_delete_it
void cli_delete_it(CLI *cli);
/* delete GLPK command-line interface */

#endif

/* eof */
