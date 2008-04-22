/* glpapi13.c (stand-alone LP/MIP solver) */

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

#include "glpapi.h"
#include "glpcli.h"

/***********************************************************************
*  exec_tty - execute commands interactively
*
*  The routine exec_tty reads commands from the terminal and passes
*  them to the GLPK command processor. */

static int func_tty(void *_info)
{     int *info = _info;
      int c = *info;
      if (c)
         *info = 0;
      else
      {  c = xgetc();
         if (c < 0) c = '\n';
      }
      return c == '\n' ? '\0' : c;
}

static int exec_tty(CLI *cli, const char *prompt)
{     int c;
      for (;;)
      {  xprintf("%.10s> ", prompt);
         c = xgetc();
         if (c < 0) goto done;
         switch (cli_execute_cmd(cli, func_tty, &c))
         {  case 0:
               break;
            case CLI_EXIT:
               goto done;
            case CLI_ERROR:
               xprintf("GLPK: %s\n", xerrmsg());
               break;
            default:
               xassert(cli != cli);
         }
      }
done: return 0;
}

/***********************************************************************
*  exec_str - execute command specified as character string
*
*  The routine exec_str passes a command specified as the character
*  string to the GLPK command processor. */

struct info_str
{     const char *cmd;
      /* command to be executed */
      int j;
      /* position of current character */
};

static int func_str(void *_info)
{     struct info_str *info = _info;
      int c;
      c = (unsigned char)info->cmd[info->j];
      if (c != '\0') info->j++;
      return c;
}

static int exec_str(CLI *cli, const char *cmd)
{     struct info_str _info, *info = &_info;
      int ret;
      xprintf("GLPK> %s\n", cmd);
      info->cmd = cmd;
      info->j = 0;
      switch (cli_execute_cmd(cli, func_str, info))
      {  case 0:
         case CLI_EXIT:
            ret = 0;
            break;
         case CLI_ERROR:
            xprintf("GLPK: %s\n", xerrmsg());
            ret = 1;
            break;
         default:
            xassert(cli != cli);
      }
      return ret;
}

/***********************************************************************
*  exec_file - execute commands from script file
*
*  The routine exec_file reads commands from the specified text file
*  and passes them to the GLPK command processor. */

struct info_file
{     const char *fname;
      /* name of script file */
      XFILE *fp;
      /* pointer to script file */
      int count;
      /* line count */
      int c;
      /* first character on the line, then 0 */
};

static int func_file(void *_info)
{     struct info_file *info = _info;
      int c = info->c;
      if (c)
         info->c = 0;
      else
      {  c = xfgetc(info->fp);
         if (c < 0) c = '\n';
      }
      xputc(c);
      return c == '\n' ? '\0' : c;
}

static int exec_file(CLI *cli, const char *fname)
{     struct info_file _info, *info = &_info;
      int ret;
      info->fname = fname;
      info->fp = xfopen(fname, "r");
      if (info->fp == NULL)
      {  xprintf("GLPK: unable to open %s - %s\n", fname, xerrmsg());
         ret = 1;
         goto done;
      }
      info->count = 0;
      info->c = 0;
      for (;;)
      {  info->count++;
         info->c = xfgetc(info->fp);
         if (info->c < 0)
         {  ret = 0;
            break;
         }
         xprintf("GLPK> ");
         switch (cli_execute_cmd(cli, func_file, info))
         {  case 0:
               break;
            case CLI_EXIT:
               ret = 0;
               goto done;
            case CLI_ERROR:
               xprintf("%s:%d: %s\n", fname, info->count, xerrmsg());
               ret = 1;
               goto done;
            default:
               xassert(cli != cli);
         }
      }
done: if (info->fp != NULL) xfclose(info->fp);
      return ret;
}

/***********************************************************************
*  simple_option - check for simple mode option
*
*  If the specified option is allowed in the simple mode, the routine
*  simple_option returns non-zero, otherwise zero. */

static int simple_option(const char *opt)
{     return
         strcmp(opt, "--mps")    == 0 ||
         strcmp(opt, "--deck")   == 0 ||
         strcmp(opt, "--lp")     == 0 ||
         strcmp(opt, "--math")   == 0 ||
         strcmp(opt, "-m")       == 0 ||
         strcmp(opt, "--model")  == 0 ||
         strcmp(opt, "-d")       == 0 ||
         strcmp(opt, "--data")   == 0 ||
         strcmp(opt, "--min")    == 0 ||
         strcmp(opt, "--max")    == 0 ||
         strcmp(opt, "-o")       == 0 ||
         strcmp(opt, "--output") == 0;
}

/***********************************************************************
*  script_option - check for script mode option
*
*  If the specified option is allowed in the script mode, the routine
*  script_option returns non-zero, otherwise zero. */

static int script_option(const char *opt)
{     return
         strcmp(opt, "-i")       == 0 ||
         strcmp(opt, "-p")       == 0 ||
         strcmp(opt, "-e")       == 0 ||
         strcmp(opt, "-f")       == 0;
}

/***********************************************************************
*  info_option - check for informational option
*
*  If the specified option is informational, the routine info_option
*  returns non-zero, otherwise zero. */

static int info_option(const char *opt)
{     return
         strcmp(opt, "-h")       == 0 ||
         strcmp(opt, "--help")   == 0 ||
         strcmp(opt, "-v")       == 0 ||
         strcmp(opt, "--version") == 0;
}

/***********************************************************************
*  simple_mode - run the solver in simple mode
*
*  The routine simple_mode implements the simple mode of the solver,
*  that includes:
*
*  1) reading problem data;
*  2) change optimization direction, if required;
*  3) solve the problem;
*  4) write solution to a text file, if required. */

static int simple_mode(int argc, const char *argv[])
{     int in_fmt = 0;
      /* input format:
         0 - not specified
         1 - free mps
         2 - fixed mps
         3 - cplex lp
         4 - gnu mathprog */
      const char *in_file = NULL;
      /* name of input file */
      const char *in_data = NULL;
      /* name of data file */
      int dir = 0;
      /* optimization direction:
         0 - not specified
         1 - minimization
         2 - maximization */
      const char *out_file = NULL;
      /* name of output file */
      CLI *cli;
      int k, ret;
      char *cmd;
      /* create command-line interface */
      cli = cli_create_it();
      /* parse command-line options */
      for (k = 1; k < argc; k++)
      {  if (strcmp(argv[k], "--mps") == 0)
         {  if (in_fmt != 0)
err1:       {  xprintf("GLPK: at most one format option allowed\n");
               goto err6;
            }
            in_fmt = 1;
         }
         else if (strcmp(argv[k], "--deck") == 0)
         {  if (in_fmt != 0) goto err1;
            in_fmt = 2;
         }
         else if (strcmp(argv[k], "--lp") == 0)
         {  if (in_fmt != 0) goto err1;
            in_fmt = 3;
         }
         else if (strcmp(argv[k], "--math") == 0)
         {  if (in_fmt != 0) goto err1;
            in_fmt = 4;
         }
         else if (strcmp(argv[k], "-m") == 0 ||
                  strcmp(argv[k], "--model") == 0)
         {  if (!(in_fmt == 0 || in_fmt == 4))
err2:       {  xprintf("GLPK: option `%s' conflicts with previously spe"
                  "cified format option\n", argv[k]);
               goto err6;
            }
            in_fmt = 4; /* assumed */
            k++;
            if (k == argc || argv[k][0] == '-')
err3:       {  xprintf("GLPK: option `%s' requires filename\n",
                  argv[k-1]);
               goto err6;
            }
            if (in_file != NULL)
err4:       {  xprintf("GLPK: only one problem file allowed\n");
               goto err6;
            }
            in_file = argv[k];
         }
         else if (strcmp(argv[k], "-d") == 0 ||
                  strcmp(argv[k], "--data") == 0)
         {  if (!(in_fmt == 0 || in_fmt == 4)) goto err2;
            in_fmt = 4; /* assumed */
            k++;
            if (k == argc || argv[k][0] == '-')
               goto err3;
            if (in_data != NULL)
            {  xprintf("GLPK: at most one data file allowed\n");
               goto err6;
            }
            in_data = argv[k];
         }
         else if (strcmp(argv[k], "--min") == 0)
         {  if (dir != 0)
err5:       {  xprintf("GLPK: at most one option `--min' or `--max' all"
                  "owed\n");
               goto err6;
            }
            dir = 1;
         }
         else if (strcmp(argv[k], "--max") == 0)
         {  if (dir != 0) goto err5;
            dir = 2;
         }
         else if (strcmp(argv[k], "-o") == 0 ||
                  strcmp(argv[k], "--output") == 0)
         {  k++;
            if (k == argc || argv[k][0] == '-')
               goto err3;
            if (out_file != NULL)
            {  xprintf("GLPK: at most one output file allowed\n");
               goto err6;
            }
            out_file = argv[k];
         }
         else if (script_option(argv[k]) || info_option(argv[k]))
         {  xprintf("GLPK: option `%s' not allowed in simple mode\n",
               argv[k]);
            goto err6;
         }
         else if (argv[k][0] == '-')
         {  xprintf("GLPK: invalid option `%s'\n", argv[k]);
err6:       xprintf("GLPK: try `%s --help' for more information\n",
               argv[0]);
            ret = EXIT_FAILURE;
            goto done;
         }
         else
         {  if (in_file != NULL) goto err4;
            in_file = argv[k];
         }
      }
      if (in_file == NULL)
      {  xprintf("GLPK: problem file not specified\n");
         goto err6;
      }
      /* read problem data */
      xassert(in_file != NULL);
      cmd = xmalloc(50 + strlen(in_file) + (in_data == NULL ? 0 :
         strlen(in_data)));
      switch (in_fmt)
      {  case 0:
         case 1:
            strcpy(cmd, "read mps ");
            strcat(cmd, in_file);
            break;
         case 2:
            strcpy(cmd, "read deck ");
            strcat(cmd, in_file);
            break;
         case 3:
            strcpy(cmd, "read lp ");
            strcat(cmd, in_file);
            break;
         case 4:
            strcpy(cmd, "read model ");
            strcat(cmd, in_file);
            if (in_data != NULL)
            {  strcat(cmd, " data ");
               strcat(cmd, in_data);
            }
            break;
         default:
            xassert(in_fmt != in_fmt);
      }
      ret = exec_str(cli, cmd);
      xfree(cmd);
      if (ret != 0)
      {  ret = EXIT_FAILURE;
         goto done;
      }
      /* change optimization direction, if necessary */
      switch (dir)
      {  case 0:
            ret = 0;
            break;
         case 1:
            ret = exec_str(cli, "set dir min");
            break;
         case 2:
            ret = exec_str(cli, "set dir max");
            break;
         default:
            xassert(dir != dir);
      }
      if (ret != 0)
      {  ret = EXIT_FAILURE;
         goto done;
      }
      /* solve the problem */
      ret = exec_str(cli, "solve");
      if (ret != 0)
      {  ret = EXIT_FAILURE;
         goto done;
      }
      /* write problem solution, if necessary */
      if (out_file != NULL)
      {  cmd = xmalloc(50 + strlen(out_file));
         strcpy(cmd, "print * ");
         strcat(cmd, out_file);
         ret = exec_str(cli, cmd);
         xfree(cmd);
         if (ret != 0)
         {  ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* all seems ok */
      ret = EXIT_SUCCESS;
done: /* delete command line interface */
      if (cli != NULL) cli_delete_it(cli);
      /* return to the main routine */
      return ret;
}

/***********************************************************************
*  script_mode - run the solver in script mode
*
*  The routine script_mode implements the script mode of the solver.
*  It reads commands either from the terminal or from a command string
*  or from a script file and passes them to the GLPK command-line
*  interpreter. */

static int script_mode(int argc, const char *argv[])
{     /* run the solver in script mode */
      CLI *cli;
      int k, ret;
      /* create command-line interface */
      cli = cli_create_it();
      /* parse command-line options and execute commands */
      for (k = 1; k < argc; k++)
      {  if (strcmp(argv[k], "-i") == 0)
            ret = exec_tty(cli, "GLPK");
         else if (strcmp(argv[k], "-p") == 0)
         {  k++;
            if (k == argc || argv[k][0] == '-')
            {  xprintf("GLPK: option `%s' requires prompt string\n",
                  argv[k-1]);
               goto err6;
            }
            ret = exec_tty(cli, argv[k]);
         }
         else if (strcmp(argv[k], "-e") == 0)
         {  k++;
            if (k == argc || argv[k][0] == '-')
            {  xprintf("GLPK: option `%s' requires command string\n",
                  argv[k-1]);
               goto err6;
            }
            ret = exec_str(cli, argv[k]);
         }
         else if (strcmp(argv[k], "-f") == 0)
         {  k++;
            if (k == argc || argv[k][0] == '-')
            {  xprintf("GLPK: option `%s' requires filename\n",
                  argv[k-1]);
               goto err6;
            }
            ret = exec_file(cli, argv[k]);
         }
         else if (simple_option(argv[k]) || info_option(argv[k]))
         {  xprintf("GLPK: option `%s' not allowed in script mode\n",
               argv[k]);
            goto err6;
         }
         else
         {  xprintf("GLPK: invalid option `%s'\n", argv[k]);
err6:       xprintf("GLPK: try `%s --help' for more information\n",
               argv[0]);
            ret = EXIT_FAILURE;
            goto done;
         }
         if (ret != 0)
         {  ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* all seems ok */
      ret = EXIT_SUCCESS;
done: /* delete command line interface */
      if (cli != NULL) cli_delete_it(cli);
      /* return to the main routine */
      return ret;
}

/***********************************************************************
*  help_info - display help information
*
*  The routine help_info displays help information according to the
*  GNU Coding Standards, Section 4.7. */

static void help_info(int argc, const char *argv[])
{     xassert(argc == argc);
      xprintf("Usage:\n");
      xprintf("   %s [simple mode options...] filename\n", argv[0]);
      xprintf("   %s [script mode options...]\n", argv[0]);
      xprintf("\n");
      xprintf("Simple mode options:\n");
      xprintf("   --mps             read problem data in free MPS forma"
         "t (default)\n");
      xprintf("   --deck            read problem data in fixed MPS form"
         "at\n");
      xprintf("   --lp              read problem data in CPLEX LP forma"
         "t\n");
      xprintf("   --math            read model written in GNU MathProg "
         "language\n");
      xprintf("   -m filename, --model filename\n");
      xprintf("                     read model section and optional dat"
         "a section from\n");
      xprintf("                     filename (assumes --math)\n");
      xprintf("   -d filename, --data filename\n");
      xprintf("                     read data section from filename (as"
         "sumes --math);\n");
      xprintf("                     if model file also has data section"
         ", that section\n");
      xprintf("                     is ignored\n");
      xprintf("   --min             minimization\n");
      xprintf("   --max             maximization\n");
      xprintf("   -o filename, --output filename\n");
      xprintf("                     write solution to filename in print"
         "able format\n");
      xprintf("\n");
      xprintf("Script mode options:\n");
      xprintf("   -i                execute commands interactively (def"
         "ault)\n");
      xprintf("   -p string         execute commands interactively usin"
         "g string as the\n");
      xprintf("                     prompt\n");
      xprintf("   -e command        execute command\n");
      xprintf("   -f filename       execute commands from filename\n");
      xprintf("\n");
      xprintf("Multiple -i, -p, -e, and -f options may be specified, in"
         " which case all\n");
      xprintf("commands are executed in the corresponding order.\n");
      xprintf("\n");
      xprintf("Informational options:\n");
      xprintf("   -h, --help        display this help information and e"
         "xit\n");
      xprintf("   -v, --version     display program version and exit\n")
         ;
      xprintf("\n");
      xprintf("See GLPK web page at <http://www.gnu.org/software/glpk/g"
         "lpk.html>.\n");
      xprintf("\n");
      xprintf("Please report bugs to <bug-glpk@gnu.org>.\n");
      return;
}

/***********************************************************************
*  version_info - display version information
*
*  The routine version_info displays version information according to
*  the GNU Coding Standards, Section 4.7. */

static void version_info(void)
{     xprintf("GLPK LP/MIP Solver %s\n", glp_version());
      xprintf("\n");
      xprintf("Copyright (C) 2008 Andrew Makhorin, Department for Appli"
         "ed Informatics,\n");
      xprintf("Moscow Aviation Institute, Moscow, Russia. All rights re"
         "served.\n");
      xprintf("\n");
      xprintf("This program has ABSOLUTELY NO WARRANTY.\n");
      xprintf("\n");
      xprintf("This program is free software; you may re-distribute it "
         "under the terms\n");
      xprintf("of the GNU General Public License version 3 or later.\n")
         ;
      return;
}

/***********************************************************************
*  glp_main - stand-alone LP/MIP solver
*
*  The routine glp_main is a main routine of the LP/MIP solver. */

int glp_main(int argc, const char *argv[])
{     const char *_argv[3];
      int ret, count;
      /* if no parameters are specified in the command line, simulate
         -i option to enter the interactive mode */
      if (argc < 2)
      {  argc = 2;
         _argv[0] = argv[0];
         _argv[1] = "-i";
         _argv[2] = NULL;
         argv = _argv;
         version_info();
         xprintf("\n");
      }
      /* enter the mode of operation depending of the first parameter
         specified in the command line */
      if (simple_option(argv[1]) || argv[1][0] != '-')
         ret = simple_mode(argc, argv);
      else if (script_option(argv[1]))
         ret = script_mode(argc, argv);
      else if (strcmp(argv[1], "-h") == 0 ||
               strcmp(argv[1], "--help") == 0)
      {  help_info(argc, argv);
         ret = EXIT_SUCCESS;
      }
      else if (strcmp(argv[1], "-v") == 0 ||
               strcmp(argv[1], "--version") == 0)
      {  version_info();
         ret = EXIT_SUCCESS;
      }
      else
      {  xprintf("GLPK: invalid option `%s'\n", argv[1]);
         xprintf("GLPK: try `%s --help' for more information\n",
            argv[0]);
         ret = EXIT_FAILURE;
      }
      /* check that no memory blocks are still allocated */
      lib_mem_usage(&count, NULL, NULL, NULL);
      xassert(count == 0);
      /* return to the control program */
      return ret;
}

/* eof */
