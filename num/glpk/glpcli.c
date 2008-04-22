/* glpcli.c */

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

#define _GLPSTD_STDIO
#include "glpapi.h"
#include "glpmpl.h"
#undef error
#undef read_char
typedef struct CLI CLI;
#define _GLP_CLI
#include "glpcli.h"

struct CLI
{     /* command-line interface */
      jmp_buf jump;
      /* label used for non-local jump in case of error */
      int (*func)(void *info);
      /* formal routine to read input characters */
      void *info;
      /* transit pointer passed to formal routine func */
      int c;
      /* current character ('\0' means end of command) */
      char word[255+1];
      /* current word (command name or parameter) */
      /*--------------------------------------------------------------*/
      MPL *mpl;
      /* model database */
      glp_prob *lp;
      /* lp/mip problem object */
      int sol;
      /* last solution flag:
         0 - problem not solved yet
         1 - basic solution
         2 - interior-point solution
         3 - integer solution */
};

static void error(CLI *cli, const char *fmt, ...)
{     /* format error message and terminate command execution */
      LIBENV *env = lib_link_env();
      va_list arg;
      /* format the error message and put it into internal buffer */
      va_start(arg, fmt);
      vsprintf(env->err_msg, fmt, arg);
      xassert(strlen(env->err_msg) < sizeof(env->err_msg));
      va_end(arg);
      /* terminate execution of the command script */
      longjmp(cli->jump, 0);
      /* no return */
}

static void read_char(CLI *cli)
{     /* read next character from the command script */
      int c;
      c = cli->func(cli->info);
      if (!(0x00 <= c && c <= 0xFF))
         error(cli, "character code out of range");
      else if (c == '\0')
         ;
      else if (isspace(c))
         c = ' ';
      else if (iscntrl(c))
         error(cli, "invalid control character 0x%02X", c);
      cli->c = (char)c;
      return;
}

static void read_cmd(CLI *cli)
{     /* read command mnemonic name */
      int len = 0;
      /* skip non-significant characters */
      while (cli->c == ' ') read_char(cli);
      /* read command name */
      while (!(cli->c == ' ' || cli->c == '\0'))
      {  if (!islower(cli->c))
            error(cli, "invalid command name");
         if (len == 7)
            error(cli, "command name too long");
         cli->word[len++] = (char)cli->c;
         read_char(cli);
      }
      /* the command name has been read */
      cli->word[len] = '\0';
      return;
}

static void read_word(CLI *cli)
{     /* read next command parameter */
      int len = 0;
      /* skip non-significant characters */
      while (cli->c == ' ') read_char(cli);
      /* read parameter */
      if (cli->c == '\'' || cli->c == '\"')
      {  /* quoted character string */
         int quote = cli->c;
         /* skip opening quote */
         read_char(cli);
         /* read characters within quotes */
         for (;;)
         {  /* check for closing quote and read it */
            if (cli->c == quote)
            {  read_char(cli);
               if (cli->c != quote) break;
            }
            /* check for end marker */
            if (cli->c == '\0')
               error(cli, "closing quote not found");
            /* add current character to the parameter */
            cli->word[len++] = (char)cli->c;
            if (len == sizeof(cli->word))
err:           error(cli, "parameter `%.15s...' too long", cli->word);
            /* read next character */
            read_char(cli);
         }
         if (len == 0)
            error(cli, "empty parameter not allowed");
      }
      else
      {  /* sequence of characters */
         while (!(cli->c == ' ' || cli->c == '\'' || cli->c == '\"' ||
                  cli->c == '\0'))
         {  /* add current character to the parameter */
            cli->word[len++] = (char)cli->c;
            if (len == sizeof(cli->word)) goto err;
            /* read next character */
            read_char(cli);
         }
      }
      /* the parameter has been read */
      cli->word[len] = '\0';
      return;
}

static void exec_print(CLI *cli)
{     /* print problem solution */
      read_word(cli);
      if (strcmp(cli->word, "*") == 0 ||
          strcmp(cli->word, "sol") == 0 ||
          strcmp(cli->word, "ipt") == 0 ||
          strcmp(cli->word, "mip") == 0)
      {  int what = cli->word[0], ret;
         char fname[sizeof(cli->word)];
         read_word(cli);
         if (cli->word[0] == '\0')
            error(cli, "filename required");
         strcpy(fname, cli->word);
         read_word(cli);
         if (cli->word[0] != '\0')
            error(cli, "too many parameters");
         if (cli->lp == NULL)
            error(cli, "problem does not exist");
         if (what == '*' && cli->sol == 0)
            error(cli, "problem not solved yet");
         else if (what == '*' && cli->sol == 1 || what == 's')
         {  /* basic solution */
            ret = lpx_print_sol(cli->lp, fname);
         }
         else if (what == '*' && cli->sol == 2 || what == 'i')
         {  /* interior-point solution */
            ret = lpx_print_ips(cli->lp, fname);
         }
         else if (what == '*' && cli->sol == 3 || what == 'm')
         {  /* mip solution */
            ret = lpx_print_mip(cli->lp, fname);
         }
         else
            xassert(cli != cli);
         if (ret)
            error(cli, "unable to write problem solution");
      }
      else
         error(cli, "command syntax error");
      return;
}

static void drop_problem(CLI *cli)
{     /* delete the current problem */
      if (cli->mpl != NULL)
         mpl_terminate(cli->mpl), cli->mpl = NULL;
      if (cli->lp != NULL)
         glp_delete_prob(cli->lp), cli->lp = NULL;
      cli->sol = 0;
      return;
}

static void exec_read(CLI *cli)
{     /* read problem data */
      read_word(cli);
      if (strcmp(cli->word, "mps") == 0 ||
          strcmp(cli->word, "deck") == 0 ||
          strcmp(cli->word, "lp") == 0)
      {  /* read problem data from a text file */
         int fmt = cli->word[0];
         char fname[sizeof(cli->word)];
         read_word(cli);
         if (cli->word[0] == '\0')
            error(cli, "filename required");
         strcpy(fname, cli->word);
         read_word(cli);
         if (cli->word[0] != '\0')
            error(cli, "too many parameters");
         drop_problem(cli);
         switch (fmt)
         {  case 'm':
               cli->lp = lpx_read_freemps(fname);
               break;
            case 'd':
               cli->lp = lpx_read_mps(fname);
               break;
            case 'l':
               cli->lp = lpx_read_cpxlp(fname);
               break;
            default:
               xassert(fmt != fmt);
         }
         if (cli->lp == NULL)
            error(cli, "unable to read problem data");
      }
      else if (strcmp(cli->word, "model") == 0)
      {  char fname[sizeof(cli->word)];
         char dname[sizeof(cli->word)];
         int ret;
         read_word(cli);
         if (cli->word[0] == '\0')
            error(cli, "model filename required");
         strcpy(fname, cli->word);
         read_word(cli);
         if (cli->word[0] == '\0')
         {  /* data filename not specified */
            dname[0] = '\0';
         }
         else if (strcmp(cli->word, "data") == 0)
         {  read_word(cli);
            if (cli->word[0] == '\0')
               error(cli, "data filename required");
            strcpy(dname, cli->word);
            read_word(cli);
         }
         else
            error(cli, "command syntax error");
         if (cli->word[0] != '\0')
            error(cli, "too many parameters");
         drop_problem(cli);
         /* initialize the translator database */
         cli->mpl = mpl_initialize();
         /* read model section and optional data section */
         ret = mpl_read_model(cli->mpl, fname, dname[0] != '\0');
         if (ret == 4)
err:     {  mpl_terminate(cli->mpl), cli->mpl = NULL;
            error(cli, "model processing error");
         }
         xassert(ret == 1 || ret == 2);
         /* read data section, if necessary */
         if (dname[0] != '\0')
         {  xassert(ret == 1);
            ret = mpl_read_data(cli->mpl, dname);
            if (ret == 4) goto err;
            xassert(ret == 2);
         }
         /* generate model */
         ret = mpl_generate(cli->mpl, NULL);
         if (ret == 4) goto err;
         /* extract problem instance */
         cli->lp = lpx_extract_prob(cli->mpl);
         xassert(cli->lp != NULL);
         /* if the model has no solve statement, delete it */
         if (!mpl_has_solve_stmt(cli->mpl))
            mpl_terminate(cli->mpl), cli->mpl = NULL;
      }
      else
         error(cli, "command syntax error");
      return;
}

static void put_solution(CLI *cli)
{     /* put solution to the translator database */
      int n, j, round, ret;
      xassert(cli->mpl != NULL);
      xassert(mpl_has_solve_stmt(cli->mpl));
      n = lpx_get_num_cols(cli->lp);
      round = lpx_get_int_parm(cli->lp, LPX_K_ROUND);
      lpx_set_int_parm(cli->lp, LPX_K_ROUND, 1);
      switch (cli->sol)
      {  case 1:
            /* basic solution */
            for (j = 1; j <= n; j++)
               mpl_put_col_value(cli->mpl, j,
                  lpx_get_col_prim(cli->lp, j));
            break;
         case 2:
            /* interior-point solution */
            for (j = 1; j <= n; j++)
               mpl_put_col_value(cli->mpl, j,
                  lpx_ipt_col_prim(cli->lp, j));
            break;
         case 3:
            /* integer solution */
            for (j = 1; j <= n; j++)
               mpl_put_col_value(cli->mpl, j,
                  lpx_mip_col_val(cli->lp, j));
            break;
         default:
            xassert(cli != cli);
      }
      lpx_set_int_parm(cli->lp, LPX_K_ROUND, round);
      /* perform postsolving and delete the model */
      ret = mpl_postsolve(cli->mpl);
      mpl_terminate(cli->mpl), cli->mpl = NULL;
      if (ret == 4)
         error(cli, "model postsolving error\n");
      xassert(ret == 3);
      return;
}

static void exec_solve(CLI *cli)
{     /* solve the current problem */
      read_word(cli);
      if (cli->word[0] != '\0')
         error(cli, "too many parameters");
      if (cli->lp == NULL)
         error(cli, "problem does not exist");
      /* solve lp (or lp relaxation to mip) */
      {  glp_smcp parm;
         glp_init_smcp(&parm);
         parm.presolve = GLP_ON;
         cli->lp->it_cnt = 0;
         glp_simplex(cli->lp, &parm);
         cli->sol = 1;
      }
      /* solve mip */
      if (glp_get_num_int(cli->lp) > 0)
      {  glp_iocp parm;
         glp_init_iocp(&parm);
         glp_intopt(cli->lp, &parm);
         cli->sol = 3;
      }
      /* put solution to the translator database */
      if (cli->mpl != NULL) put_solution(cli);
      return;
}

/***********************************************************************
*  NAME
*
*  cli_create_it - create GLPK command-line interface
*
*  SYNOPSIS
*
*  #include "glpcli.h"
*  CLI *cli_create_it(void);
*
*  DESCRIPTION
*
*  The routine cli_create_it creates a program object, which implements
*  the command-line interface to GLPK.
*
*  RETURNS
*
*  The routine cli_create_it returns a pointer to the object created. */

CLI *cli_create_it(void)
{     /* create command-line interface */
      CLI *cli;
      cli = xmalloc(sizeof(CLI));
      memset(cli->jump, 0, sizeof(cli->jump));
      cli->func = NULL;
      cli->info = NULL;
      cli->c = '\0';
      cli->word[0] = '\0';
      cli->lp = NULL;
      cli->mpl = NULL;
      cli->sol = 0;
      return cli;
}

/***********************************************************************
*  NAME
*
*  cli_execute_cmd - execute GLPK command script (core routine)
*
*  SYNOPSIS
*
*  #include "glpcli.h"
*  int cli_execute_cmd(CLI *cli, int (*func)(void *info), void *info);
*
*  DESCRIPTION
*
*  The routine cli_execute_cmd executes a command script passing it to
*  the command interpreter through the command-line interface specified
*  by the parameter cli.
*
*  RETURNS
*
*  0  The command has been successfully executed.
*
*  CLI_EXIT
*     The exit command has been executed.
*
*  CLI_ERROR
*     Either the specified command is invalid or an error occurred on
*     executing the command. */

int cli_execute_cmd(CLI *cli, int (*func)(void *info), void *info)
{     int ret;
      /* setup error handling */
      if (setjmp(cli->jump))
      {  /* skip command script until end of line */
         while (cli->c != '\0')
            cli->c = cli->func(cli->info);
         ret = CLI_ERROR;
         goto done;
      }
      /* initialize scanner */
      cli->func = func;
      cli->info = info;
      cli->c = ' ';
      cli->word[0] = '\0';
      /* read command name */
      read_cmd(cli);
      /* execute command script */
      if (cli->word[0] == '\0')
      {  /* empty command */
         ;
      }
      else if (strcmp(cli->word, "exec") == 0)
      {  /* execute operating system command */
         int len = 0;
         while (cli->c != '\0')
         {  cli->word[len++] = (char)cli->c;
            if (len == sizeof(cli->word))
               error(cli, "system command too long");
            read_char(cli);
         }
         cli->word[len] = '\0';
         system(cli->word);
         cli->word[0] = '\0';
      }
      else if (strcmp(cli->word, "exit") == 0)
      {  /* stop executing the script */
         read_word(cli);
         if (cli->word[0] != '\0')
            error(cli, "too many parameters");
         ret = CLI_EXIT;
         goto done;
      }
      else if (strcmp(cli->word, "print") == 0)
         exec_print(cli);
      else if (strcmp(cli->word, "read") == 0)
         exec_read(cli);
      else if (strcmp(cli->word, "solve") == 0)
         exec_solve(cli);
      else
         error(cli, "no such command `%s'", cli->word);
      /* there must be no unread parameters */
      xassert(cli->word[0] == '\0');
      ret = 0;
done: return ret;
}

/***********************************************************************
*  NAME
*
*  cli_delete_it - delete GLPK command-line interface
*
*  SYNOPSIS
*
*  #include "glpcli.h"
*  void cli_delete_it(CLI *cli);
*
*  DESCRIPTION
*
*  The routine cli_delete_it deletes the command-line interface to GLPK
*  specified by the parameter cli and frees all resources allocated to
*  this program object. */

void cli_delete_it(CLI *cli)
{     drop_problem(cli);
      xfree(cli);
      return;
}

/* eof */
