/* glplpx20.c */

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
#include "glpgmp.h"
#include "glpmpl.h"
#define fault old_fault

/*--------------------------------------------------------------------*/
/* This program is a stand-alone LP/MIP solver. For pure LP problems  */
/* either the simplex method or the primal-dual interior point method */
/* can be used. For MIP problems the branch-and-bound procedure based */
/* on the simplex method is used.                                     */
/*--------------------------------------------------------------------*/

static int format = 4;
/* type of input text file:
   0 - fixed MPS
   1 - CPLEX LP
   2 - GNU MathProg
   3 - GNU LP
   4 - free MPS */

static const char *in_file = NULL;
/* name of input text file */

static const char *in_data = NULL;
/* name of optional input text file, which contains data section; NULL
   means no separate data section is provided */

static const char *display = NULL;
/* name of optional output text file, to which display output is sent;
   NULL means the output is sent to stdout */

static const char *in_bas = NULL;
/* name of input text file, which contains initial LP basis in MPS
   format; NULL means no initial LP basis is provided */

static const char *out_bas = NULL;
/* name of output text file, to which final LP basis should be written
   in MPS format; NULL means no output */

static const char *in_res = NULL;
/* name of input text file, which contains solution in raw format;
   NULL means no solution is provided */

static const char *out_res = NULL;
/* name of output text file, to which final solution should be written
   in raw format; NULL means no output */

static int dir = 0;
/* optimization direction flag:
   0       - not specified
   LPX_MIN - minimization
   LPX_MAX - maximization */

static int scale = 1;
/* if this flag is set, automatic scaling before solving the problem is
   performed; otherwise scaling is not used */

static int method = 0;
/* which method should be used for solving the problem:
   0 - simplex
   1 - interior point
   2 - branch-and-bound */

static int exact = 0;
/* if this flag is set, use lpx_exact rather than lpx_simplex */

#if 0 /* 12/XI-2006 */
static int primal = 0;
/* if this flag is set, use lpx_primal rather than lpx_simplex */
#endif

static int intopt = 0;
/* if this flag is set, use lpx_intopt rather than lpx_integer */

static const char *out_sol = NULL;
/* name of output text file, to which the final solution should be sent
   in plain text format; NULL means no solution output */

static const char *out_bnds = NULL;
/* name of output text file, to which sensitivity bounds should be sent
   in plain text format; NULL means no sensitivity output */

static int tmlim = -1;
/* solution time limit, in seconds */

static int memlim = -1;
/* available memory limit, in megabytes */

static int check = 0;
/* if this flag is set, only input data checking is required */

static int orig = 0;
/* if this flag is set, try to use original names of rows and columns;
   otherwise use plain names */

static const char *out_mps = NULL;
/* name of output text file, to which the problem should be written in
   fixed MPS format; NULL means no MPS output */

static const char *out_freemps = NULL;
/* name of output text file, to which the problem should be written in
   free MPS format; NULL means no MPS output */

static const char *out_cpxlp = NULL;
/* name of output text file, to which the problem should be written in
   CPLEX LP format; NULL means no CPLEX LP output */

static const char *out_txt = NULL;
/* name of output text file, to which the problem should be written in
   plain text format; NULL means no plain text output */

static const char *out_glp = NULL;
/* name of output text file, to which the problem should be written in
   GNU LP format; NULL means no GNU LP output */

static const char *out_pb = NULL;
/* name of output text file, to which the problem should be written in
    OPB format; NULL means no OPB output */

static const char *out_npb = NULL;
/* name of output text file, to which the problem should be written in
    nomralized OPB format; NULL means no normalized OPB output */

static const char *log_file = NULL;
/* name of output text file, to which a hardcopy of all screen output
   should be written; NULL means no hardcopying */

static const char *newname = NULL;
/* new name which has to be assigned to the problem */

static bf_type = 1;
/* which LP basis factorization should be used:
   1 - LP basis factorization option:
   2 - LU + Schur complement + Bartels-Golub update
   3 - LU + Schur complement + Givens rotation update */

static int basis = 1;
/* which initial basis should be used:
   0 - standard initial basis
   1 - advanced initial basis
   2 - Bixby's initial basis */

static int price = 1;
/* which pricing technique should be used:
   0 - textbook pricing
   1 - steepest edge pricing */

static int relax = 1;
/* if this flag is set, the solver uses two-pass ratio test (for both
   primal and dual simplex) proposed by P.Harris; otherwise the solver
   uses the standard "textbook" ratio test */

static int presol = 1;
/* if this flag is set, the solver uses the LP presolver; otherwise the
   LP presolver is not used */

static int xcheck = 0;
/* if this flag is set, the solver checks the final basis using exact
   (bignum) arithmetic */

static int nomip = 0;
/* if this flag is set, the solver considers all integer variables as
   continuous (this allows solving MIP problem as pure LP) */

static int branch = 2;
/* which branching technique should be used:
   0 - branch on first variable
   1 - branch on last variable
   2 - branch using heuristic by Driebeck and Tomlin
   3 - branch on most fractional variable */

static int btrack = 3;
/* which backtracking technique should be used:
   0 - select most recent node (depth first search)
   1 - select earliest node (breadth first search)
   2 - select node using the best projection heuristic
   3 - select node with best local bound */

static double mip_gap = 0.0;
/* relative MIP gap tolerance */

static int binarize = 0;
/* if this flag is set, the solver replaces general integer variables
   by binary ones */

static int use_cuts = 0;
/* if this flag is set, the solver tries to generate cutting planes */

/*----------------------------------------------------------------------
-- display_help - display help.
--
-- This routine displays help information about the program as required
-- by the GNU Coding Standards. */

static void display_help(const char *my_name)
{     xprintf("Usage: %s [options...] filename\n", my_name);
      xprintf("\n");
      xprintf("General options:\n");
#if 0
      print("   --glp             read LP/MIP model in GNU LP format");
#endif
    xprintf("   --mps             read LP/MIP problem in Fixed MPS form"
         "at\n");
    xprintf("   --freemps         read LP/MIP problem in Free MPS forma"
         "t (default)\n");
    xprintf("   --cpxlp           read LP/MIP problem in CPLEX LP forma"
         "t\n");
    xprintf("   --math            read LP/MIP model written in GNU Math"
         "Prog modeling\n");
    xprintf("                     language\n");
    xprintf("   -m filename, --model filename\n");
    xprintf("                     read model section and optional data "
         "section from\n");
    xprintf("                     filename (the same as --math)\n");
    xprintf("   -d filename, --data filename\n");
    xprintf("                     read data section from filename (for "
         "--math only);\n");
    xprintf("                     if model file also has data section, "
         "that section\n");
    xprintf("                     is ignored\n");
    xprintf("   -y filename, --display filename\n");
    xprintf("                     send display output to filename (for "
         "--math only);\n");
    xprintf("                     by default the output is sent to term"
         "inal\n");
    xprintf("   -r filename, --read filename\n");
    xprintf("                     read solution from filename rather to"
         " find it with\n");
    xprintf("                     the solver\n");
    xprintf("   --min             minimization\n");
    xprintf("   --max             maximization\n");
    xprintf("   --scale           scale problem (default)\n");
    xprintf("   --noscale         do not scale problem\n");
    xprintf("   --simplex         use simplex method (default)\n");
    xprintf("   --interior        use interior point method (for pure L"
         "P only)\n");
    xprintf("   -o filename, --output filename\n");
    xprintf("                     write solution to filename in printab"
         "le format\n");
    xprintf("   -w filename, --write filename\n");
    xprintf("                     write solution to filename in plain t"
         "ext format\n");
    xprintf("   --bounds filename\n");
    xprintf("                     write sensitivity bounds to filename "
         "in printable\n");
    xprintf("                     format (LP only)\n");
    xprintf("   --tmlim nnn       limit solution time to nnn seconds\n")
         ;
    xprintf("   --memlim nnn      limit available memory to nnn megabyt"
         "es\n");
    xprintf("   --check           do not solve problem, check input dat"
         "a only\n");
    xprintf("   --name probname   change problem name to probname\n");
    xprintf("   --plain           use plain names of rows and columns ("
         "default)\n");
    xprintf("   --orig            try using original names of rows and "
         "columns\n");
    xprintf("                     (default for --mps)\n");
#if 0
      print("   --wglp filename   write problem to filename in GNU LP f"
         "ormat");
#endif
    xprintf("   --wmps filename   write problem to filename in Fixed MP"
         "S format\n");
    xprintf("   --wfreemps filename\n");
    xprintf("                     write problem to filename in Free MPS"
         " format\n");
    xprintf("   --wcpxlp filename write problem to filename in CPLEX LP"
         " format\n");
    xprintf("   --wtxt filename   write problem to filename in printabl"
         "e format\n");
    xprintf("   --wpb filename    write problem to filename in OPB form"
         "at\n");
    xprintf("   --wnpb filename   write problem to filename in normaliz"
         "ed OPB format\n");
    xprintf("   --log filename    write copy of terminal output to file"
         "name\n");
    xprintf("   -h, --help        display this help information and exi"
         "t\n");
    xprintf("   -v, --version     display program version and exit\n");
    xprintf("\n");
    xprintf("LP basis factorization option:\n");
    xprintf("   --luf             LU + Forrest-Tomlin update\n");
    xprintf("                     (faster, less stable; default)\n");
    xprintf("   --cbg             LU + Schur complement + Bartels-Golub"
         " update\n");
    xprintf("                     (slower, more stable)\n");
    xprintf("   --cgr             LU + Schur complement + Givens rotati"
         "on update\n");
    xprintf("                     (slower, more stable)\n");
    xprintf("\n");
    xprintf("Options specific to simplex method:\n");
    xprintf("   --std             use standard initial basis of all sla"
         "cks\n");
      xprintf("   --adv             use advanced initial basis (default"
         ")\n");
      xprintf("   --bib             use Bixby's initial basis\n");
    xprintf("   --bas filename    read initial basis from filename in M"
         "PS format\n");
      xprintf("   --steep           use steepest edge technique (defaul"
         "t)\n");
    xprintf("   --nosteep         use standard \"textbook\" pricing\n");
    xprintf("   --relax           use Harris' two-pass ratio test (defa"
         "ult)\n");
      xprintf("   --norelax         use standard \"textbook\" ratio tes"
         "t\n");
    xprintf("   --presol          use presolver (default; assumes --sca"
         "le and --adv)\n");
    xprintf("   --nopresol        do not use presolver\n");
    xprintf("   --exact           use simplex method based on exact ari"
         "thmetic\n");
    xprintf("   --xcheck          check final basis using exact arithme"
         "tic\n");
    xprintf("   --wbas filename   write final basis to filename in MPS "
         "format\n");
    xprintf("\n");
    xprintf("Options specific to MIP:\n");
    xprintf("   --nomip           consider all integer variables as con"
         "tinuous\n");
    xprintf("                     (allows solving MIP as pure LP)\n");
    xprintf("   --first           branch on first integer variable\n");
    xprintf("   --last            branch on last integer variable\n");
    xprintf("   --drtom           branch using heuristic by Driebeck an"
         "d Tomlin\n");
    xprintf("                     (default)\n");
    xprintf("   --mostf           branch on most fractional varaible\n")
         ;
    xprintf("   --dfs             backtrack using depth first search\n")
         ;
      xprintf("   --bfs             backtrack using breadth first searc"
         "h\n");
    xprintf("   --bestp           backtrack using the best projection h"
         "euristic\n");
    xprintf("   --bestb           backtrack using node with best local "
         "bound\n");
    xprintf("                     (default)\n");
    xprintf("   --mipgap tol      set relative gap tolerance to tol\n");
    xprintf("   --intopt          use advanced MIP solver\n");
    xprintf("   --binarize        replace general integer variables by "
         "binary ones\n");
    xprintf("                     (assumes --intopt)\n");
    xprintf("   --cover           generate mixed cover cuts\n");
    xprintf("   --clique          generate clique cuts\n");
      xprintf("   --gomory          generate Gomory's mixed integer cut"
         "s\n");
    xprintf("   --mir             generate MIR (mixed integer rounding)"
         " cuts\n");
    xprintf("   --cuts            generate all cuts above (assumes --in"
         "topt)\n");
#ifdef _GLP_USE_MIPOPT
      print("   --mipopt          use external MIP solver");
#endif
    xprintf("\n");
    xprintf("For description of the MPS and CPLEX LP formats see Refere"
         "nce Manual.\n");
    xprintf("For description of the modeling language see \"GLPK: Model"
         "ing Language\n");
    xprintf("GNU MathProg\". Both documents are included in the GLPK di"
         "stribution.\n");
    xprintf("\n");
    xprintf("See GLPK web page at <http://www.gnu.org/software/glpk/glp"
         "k.html>.\n");
    xprintf("\n");
    xprintf("Please report bugs to <bug-glpk@gnu.org>.\n");
      return;
}

/*----------------------------------------------------------------------
-- display_version - display version.
--
-- This routine displays version information for the program as required
-- by the GNU Coding Standards. */

static void display_version(void)
{     xprintf("GLPSOL: GLPK LP/MIP Solver %s\n", glp_version());
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

/*----------------------------------------------------------------------
-- parse_cmdline - parse command-line parameters.
--
-- This routine parses parameters specified in the command line. */

#define p(str) (strcmp(argv[k], str) == 0)

static int parse_cmdline(int argc, const char *argv[])
{     int k;
      for (k = 1; k < argc; k++)
      {  if (p("--mps"))
            format = 0;
         else if (p("--cpxlp") || p("--lpt"))
            format = 1;
         else if (p("--math") || p("-m") || p("--model"))
            format = 2;
         else if (p("--glp"))
            format = 3;
         else if (p("--freemps"))
            format = 4;
         else if (p("-d") || p("--data"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No data input file specifed\n");
               return 1;
            }
            if (in_data != NULL)
            {  xprintf("Only one data input file allowed\n");
               return 1;
            }
            in_data = argv[k];
         }
         else if (p("-y") || p("--display"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No display output file specifed\n");
               return 1;
            }
            if (display != NULL)
            {  xprintf("Only one display output file allowed\n");
               return 1;
            }
            display = argv[k];
         }
         else if (p("-r") || p("--read"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No input solution file specifed\n");
               return 1;
            }
            if (display != NULL)
            {  xprintf("Only one input solution file allowed\n");
               return 1;
            }
            in_res = argv[k];
         }
         else if (p("--min"))
            dir = LPX_MIN;
         else if (p("--max"))
            dir = LPX_MAX;
         else if (p("--scale"))
            scale = 1;
         else if (p("--noscale"))
            scale = 0;
         else if (p("--simplex"))
            method = 0;
         else if (p("--interior"))
            method = 1;
         else if (p("-o") || p("--output"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No solution output file specified\n");
               return 1;
            }
            if (out_sol != NULL)
            {  xprintf("Only one solution output file allowed\n");
               return 1;
            }
            out_sol = argv[k];
         }
         else if (p("-w") || p("--write"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No output solution file specifed\n");
               return 1;
            }
            if (display != NULL)
            {  xprintf("Only one output solution file allowed\n");
               return 1;
            }
            out_res = argv[k];
         }
         else if (p("--bounds"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No sensitivity bounds output file specified\n");
               return 1;
            }
            if (out_bnds != NULL)
            {  xprintf("Only one sensitivity bounds output file allowed"
                  "\n");
               return 1;
            }
            out_bnds = argv[k];
         }
         else if (p("--tmlim"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No time limit specified\n");
               return 1;
            }
            if (str2int(argv[k], &tmlim) || tmlim < 0)
            {  xprintf("Invalid time limit `%s'\n", argv[k]);
               return 1;
            }
         }
         else if (p("--memlim"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No memory limit specified\n");
               return 1;
            }
            if (str2int(argv[k], &memlim) || memlim < 0)
            {  xprintf("Invalid memory limit `%s'\n", argv[k]);
               return 1;
            }
         }
         else if (p("--check"))
            check = 1;
         else if (p("--name"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No problem name specified\n");
               return 1;
            }
            if (newname != NULL)
            {  xprintf("Only one problem name allowed\n");
               return 1;
            }
            newname = argv[k];
         }
         else if (p("--plain"))
            orig = 0;
         else if (p("--orig"))
            orig = 1;
         else if (p("--wmps"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No fixed MPS output file specified\n");
               return 1;
            }
            if (out_mps != NULL)
            {  xprintf("Only one fixed MPS output file allowed\n");
               return 1;
            }
            out_mps = argv[k];
         }
         else if (p("--wfreemps"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No free MPS output file specified\n");
               return 1;
            }
            if (out_mps != NULL)
            {  xprintf("Only one free MPS output file allowed\n");
               return 1;
            }
            out_freemps = argv[k];
         }
         else if (p("--wcpxlp") || p("--wlpt"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No CPLEX LP output file specified\n");
               return 1;
            }
            if (out_cpxlp != NULL)
            {  xprintf("Only one CPLEX LP output file allowed\n");
               return 1;
            }
            out_cpxlp = argv[k];
         }
         else if (p("--wtxt"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No problem output file specified\n");
               return 1;
            }
            if (out_txt != NULL)
            {  xprintf("Only one problem output file allowed\n");
               return 1;
            }
            out_txt = argv[k];
         }
         else if (p("--wpb"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No problem output file specified\n");
               return 1;
            }
            if (out_pb != NULL)
            {  xprintf("Only one OPB output file allowed\n");
               return 1;
            }
            out_pb = argv[k];
         }
         else if (p("--wnpb"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No problem output file specified\n");
               return 1;
            }
            if (out_npb != NULL)
            {  xprintf("Only one normalized OPB output file allowed\n");
               return 1;
            }
            out_npb = argv[k];
         }
         else if (p("--log"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No log file specified\n");
               return 1;
            }
            if (log_file != NULL)
            {  xprintf("Only one log file allowed\n");
               return 1;
            }
            log_file = argv[k];
         }
         else if (p("--wglp"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No problem output file specified\n");
               return 1;
            }
            if (out_glp != NULL)
            {  xprintf("Only one problem output file allowed\n");
               return 1;
            }
            out_glp = argv[k];
         }
         else if (p("-h") || p("--help"))
         {  display_help(argv[0]);
            return -1;
         }
         else if (p("-v") || p("--version"))
         {  display_version();
            return -1;
         }
         else if (p("--luf"))
            bf_type = 1;
         else if (p("--cbg"))
            bf_type = 2;
         else if (p("--cgr"))
            bf_type = 3;
#if 0 /* 12/XI-2006 */
         else if (p("--primal"))
            primal = 1;
#endif
         else if (p("--std"))
            basis = 0, presol = 0;
         else if (p("--adv"))
            basis = 1, presol = 0;
         else if (p("--bib"))
            basis = 2, presol = 0;
         else if (p("--bas"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No basis input file specifed\n");
               return 1;
            }
            if (in_bas != NULL)
            {  xprintf("Only one basis input file allowed\n");
               return 1;
            }
            in_bas = argv[k];
         }
         else if (p("--steep"))
            price = 1;
         else if (p("--nosteep"))
            price = 0;
         else if (p("--relax"))
            relax = 1;
         else if (p("--norelax"))
            relax = 0;
         else if (p("--presol"))
            presol = 1;
         else if (p("--nopresol"))
            presol = 0;
         else if (p("--exact"))
            exact = 1, presol = 0;
         else if (p("--xcheck"))
            xcheck = 1;
         else if (p("--wbas"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No basis output file specified\n");
               return 1;
            }
            if (out_bas != NULL)
            {  xprintf("Only one basis output file allowed\n");
               return 1;
            }
            out_bas = argv[k];
         }
         else if (p("--nomip"))
            nomip = 1;
         else if (p("--first"))
            branch = 0;
         else if (p("--last"))
            branch = 1;
         else if (p("--drtom"))
            branch = 2;
         else if (p("--mostf"))
            branch = 3;
         else if (p("--dfs"))
            btrack = 0;
         else if (p("--bfs"))
            btrack = 1;
         else if (p("--bestp"))
            btrack = 2;
         else if (p("--bestb"))
            btrack = 3;
         else if (p("--mipgap"))
         {  k++;
            if (k == argc || argv[k][0] == '\0' || argv[k][0] == '-')
            {  xprintf("No relative gap tolerance specified\n");
               return 1;
            }
            if (str2num(argv[k], &mip_gap) || mip_gap < 0.0)
            {  xprintf("Invalid relative gap tolerance `%s'\n",
                  argv[k]);
               return 1;
            }
         }
         else if (p("--intopt"))
            intopt = 1;
         else if (p("--binarize"))
            intopt = 1, binarize = 1;
         else if (p("--cover"))
            intopt = 1, use_cuts |= LPX_C_COVER;
         else if (p("--clique"))
            intopt = 1, use_cuts |= LPX_C_CLIQUE;
         else if (p("--gomory"))
            intopt = 1, use_cuts |= LPX_C_GOMORY;
         else if (p("--mir"))
            intopt = 1, use_cuts |= LPX_C_MIR;
         else if (p("--cuts"))
            intopt = 1, use_cuts |= LPX_C_ALL;
#ifdef _GLP_USE_MIPOPT
         else if (p("--mipopt"))
            intopt = 2;
#endif
         else if (argv[k][0] == '-' ||
                 (argv[k][0] == '-' && argv[k][1] == '-'))
         {  xprintf("Invalid option `%s'; try %s --help\n",
               argv[k], argv[0]);
            return 1;
         }
         else
         {  if (in_file != NULL)
            {  xprintf("Only one input file allowed\n");
               return 1;
            }
            in_file = argv[k];
         }
      }
      return 0;
}

/*----------------------------------------------------------------------
-- lpx_main - stand-alone LP/MIP solver.
--
-- This main program is called by the control program and manages the
-- solution process. */

int lpx_main(int argc, const char *argv[])
{     LPX *lp = NULL;
      MPL *mpl = NULL;
      int ret;
      xlong_t start;
      /* parse command line parameters */
      ret = parse_cmdline(argc, argv);
      if (ret < 0)
      {  ret = EXIT_SUCCESS;
         goto done;
      }
      if (ret > 0)
      {  ret = EXIT_FAILURE;
         goto done;
      }
      /* set available memory limit */
      if (memlim >= 0) glp_mem_limit(memlim);
      /* remove all output files specified in the command line */
      if (display != NULL) remove(display);
      if (out_bas != NULL) remove(out_bas);
      if (out_sol != NULL) remove(out_sol);
      if (out_res != NULL) remove(out_res);
      if (out_bnds != NULL) remove(out_bnds);
      if (out_mps != NULL) remove(out_mps);
      if (out_freemps != NULL) remove(out_freemps);
      if (out_cpxlp != NULL) remove(out_cpxlp);
      if (out_txt != NULL) remove(out_txt);
      if (out_glp != NULL) remove(out_glp);
      if (log_file != NULL) remove(log_file);
      /* open hardcopy file, if necessary */
      if (log_file != NULL)
      {  if (lib_open_log(log_file))
         {  xprintf("Unable to create log file\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* read problem data from the input file */
      if (in_file == NULL)
      {  xprintf("No input file specified; try %s --help\n", argv[0]);
         ret = EXIT_FAILURE;
         goto done;
      }
      switch (format)
      {  case 0:
            lp = lpx_read_mps(in_file);
            if (lp == NULL)
            {  xprintf("MPS file processing error\n");
               ret = EXIT_FAILURE;
               goto done;
            }
            orig = 1;
            break;
         case 1:
            lp = lpx_read_cpxlp(in_file);
            if (lp == NULL)
            {  xprintf("CPLEX LP file processing error\n");
               ret = EXIT_FAILURE;
               goto done;
            }
            break;
         case 2:
            /* initialize the translator database */
            mpl = mpl_initialize();
            /* read model section and optional data section */
            ret = mpl_read_model(mpl, (char *)in_file, in_data != NULL);
            if (ret == 4)
err:        {  xprintf("Model processing error\n");
               ret = EXIT_FAILURE;
               goto done;
            }
            xassert(ret == 1 || ret == 2);
            /* read data section, if necessary */
            if (in_data != NULL)
            {  xassert(ret == 1);
               ret = mpl_read_data(mpl, (char *)in_data);
               if (ret == 4) goto err;
               xassert(ret == 2);
            }
            /* generate model */
            ret = mpl_generate(mpl, (char *)display);
            if (ret == 4) goto err;
            /* extract problem instance */
            lp = lpx_extract_prob(mpl);
            xassert(lp != NULL);
            break;
         case 3:
            lp = lpx_read_prob((char *)in_file);
            if (lp == NULL)
            {  xprintf("GNU LP file processing error\n");
               ret = EXIT_FAILURE;
               goto done;
            }
            break;
         case 4:
            lp = lpx_read_freemps(in_file);
            if (lp == NULL)
            {  xprintf("MPS file processing error\n");
               ret = EXIT_FAILURE;
               goto done;
            }
            break;
         default:
            xassert(format != format);
      }
      /* order rows and columns of the constraint matrix */
      lpx_order_matrix(lp);
      /* change problem name (if required) */
      if (newname != NULL) lpx_set_prob_name(lp, newname);
      /* change optimization direction (if required) */
      if (dir != 0) lpx_set_obj_dir(lp, dir);
      /* write problem in fixed MPS format (if required) */
      if (out_mps != NULL)
      {  lpx_set_int_parm(lp, LPX_K_MPSORIG, orig);
         ret = lpx_write_mps(lp, out_mps);
         if (ret != 0)
         {  xprintf("Unable to write problem in fixed MPS format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem in free MPS format (if required) */
      if (out_freemps != NULL)
      {  ret = lpx_write_freemps(lp, out_freemps);
         if (ret != 0)
         {  xprintf("Unable to write problem in free MPS format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem in CPLEX LP format (if required) */
      if (out_cpxlp != NULL)
      {  ret = lpx_write_cpxlp(lp, out_cpxlp);
         if (ret != 0)
         {  xprintf("Unable to write problem in CPLEX LP format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem in plain text format (if required) */
      if (out_txt != NULL)
      {  lpx_set_int_parm(lp, LPX_K_LPTORIG, orig);
         ret = lpx_print_prob(lp, out_txt);
         if (ret != 0)
         {  xprintf("Unable to write problem in plain text format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem in GNU LP format (if required) */
      if (out_glp != NULL)
      {  ret = lpx_write_prob(lp, (char *)out_glp);
         if (ret != 0)
         {  xprintf("Unable to write problem in GNU LP format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem in OPB format (if required) */
      if (out_pb != NULL)
      {  ret = lpx_write_pb(lp, out_pb, 0);
         if (ret != 0)
         {  xprintf("Unable to write problem in OPB format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem in normalized OPB format (if required) */
      if (out_npb != NULL)
      {  ret = lpx_write_pb(lp, out_npb, 1);
         if (ret != 0)
         {  xprintf(
               "Unable to write problem in normalized OPB format\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* if only data check is required, skip computations */
      if (check) goto done;
      /* if solution is provided, read it and skip computations */
      if (in_res != NULL)
      {  switch (method)
         {  case 0:
               if (nomip || lpx_get_class(lp) == LPX_LP)
                  ret = glp_read_sol(lp, in_res);
               else
               {  method = 2;
                  ret = glp_read_mip(lp, in_res);
               }
               break;
            case 1:
               ret = glp_read_ipt(lp, in_res);
               break;
            default:
               xassert(method != method);
         }
         if (ret != 0)
         {  xprintf("Unable to read problem solution\n");
            ret = EXIT_FAILURE;
            goto done;
         }
         goto ssss;
      }
      /* scale the problem data (if required) */
      if (scale && (!presol || method == 1)) lpx_scale_prob(lp);
      /* build initial LP basis */
      if (method == 0 && !presol && in_bas == NULL)
      {  switch (basis)
         {  case 0:
               lpx_std_basis(lp);
               break;
            case 1:
               if (lpx_get_num_rows(lp) > 0 && lpx_get_num_cols(lp) > 0)
                  lpx_adv_basis(lp);
               break;
            case 2:
               if (lpx_get_num_rows(lp) > 0 && lpx_get_num_cols(lp) > 0)
                  lpx_cpx_basis(lp);
               break;
            default:
               xassert(basis != basis);
         }
      }
      /* or read initial basis from input text file in MPS format */
      if (in_bas != NULL)
      {  if (method != 0)
         {  xprintf("Initial LP basis is useless for interior-point sol"
               "ver and therefore ignored\n");
            goto nobs;
         }
         lpx_set_int_parm(lp, LPX_K_MPSORIG, orig);
         ret = lpx_read_bas(lp, in_bas);
         if (ret != 0)
         {  xprintf("Unable to read initial LP basis\n");
            ret = EXIT_FAILURE;
            goto done;
         }
         if (presol)
         {  presol = 0;
            xprintf("LP presolver disabled because initial LP basis has"
               " been provided\n");
         }
nobs:    ;
      }
      /* set some control parameters, which might be changed in the
         command line */
      lpx_set_int_parm(lp, LPX_K_BFTYPE, bf_type);
      lpx_set_int_parm(lp, LPX_K_PRICE, price);
      if (!relax) lpx_set_real_parm(lp, LPX_K_RELAX, 0.0);
      lpx_set_int_parm(lp, LPX_K_PRESOL, presol);
      lpx_set_int_parm(lp, LPX_K_BRANCH, branch);
      lpx_set_int_parm(lp, LPX_K_BTRACK, btrack);
      lpx_set_real_parm(lp, LPX_K_TMLIM, (double)tmlim);
      lpx_set_int_parm(lp, LPX_K_BINARIZE, binarize);
      lpx_set_int_parm(lp, LPX_K_USECUTS, use_cuts);
      lpx_set_real_parm(lp, LPX_K_MIPGAP, mip_gap);
      /* solve the problem */
      start = xtime();
      switch (method)
      {  case 0:
            if (nomip || lpx_get_class(lp) == LPX_LP)
            {  ret = (!exact ? lpx_simplex(lp) : lpx_exact(lp));
               if (xcheck)
               {  if (!presol || ret == LPX_E_OK)
                     lpx_exact(lp);
                  else
                     xprintf("If you need checking final basis for non-"
                        "optimal solution, use --nopresol\n");
               }
               if (presol && ret != LPX_E_OK && (out_bas != NULL ||
                  out_sol != NULL))
                  xprintf("If you need actual output for non-optimal so"
                     "lution, use --nopresol\n");
            }
            else
            {  method = 2;
               if (!intopt)
               {  ret = (!exact ? lpx_simplex(lp) : lpx_exact(lp));
                  if (xcheck && (!presol || ret == LPX_E_OK))
                     lpx_exact(lp);
                  lpx_integer(lp);
               }
               else if (intopt == 1)
                  lpx_intopt(lp);
#ifdef _GLP_USE_MIPOPT
               else
                  glp_mipopt(lp);
#endif
            }
            break;
         case 1:
            if (nomip || lpx_get_class(lp) == LPX_LP)
               lpx_interior(lp);
            else
            {  xprintf("Interior-point method is not able to solve MIP "
                  "problem; use --simplex\n");
               ret = EXIT_FAILURE;
               goto done;
            }
            break;
         default:
            xassert(method != method);
      }
      /* display statistics */
      xprintf("Time used:   %.1f secs\n", xdifftime(xtime(), start));
      {  xlong_t tpeak;
         char buf[50];
         lib_mem_usage(NULL, NULL, NULL, &tpeak);
         xprintf("Memory used: %.1f Mb (%s bytes)\n",
            xltod(tpeak) / 1048576.0, xltoa(tpeak, buf));
      }
ssss: if (mpl != NULL && mpl_has_solve_stmt(mpl))
      {  int n, j, round;
         /* store the solution to the translator database */
         n = lpx_get_num_cols(lp);
         round = lpx_get_int_parm(lp, LPX_K_ROUND);
         lpx_set_int_parm(lp, LPX_K_ROUND, 1);
         switch (method)
         {  case 0:
               for (j = 1; j <= n; j++)
                  mpl_put_col_value(mpl, j, lpx_get_col_prim(lp, j));
               break;
            case 1:
               for (j = 1; j <= n; j++)
                  mpl_put_col_value(mpl, j, lpx_ipt_col_prim(lp, j));
               break;
            case 2:
               for (j = 1; j <= n; j++)
                  mpl_put_col_value(mpl, j, lpx_mip_col_val(lp, j));
               break;
            default:
               xassert(method != method);
         }
         lpx_set_int_parm(lp, LPX_K_ROUND, round);
         /* perform postsolving */
         ret = mpl_postsolve(mpl);
         if (ret == 4)
         {  xprintf("Model postsolving error\n");
            ret = EXIT_FAILURE;
            goto done;
         }
         xassert(ret == 3);
      }
      /* write final LP basis (if required) */
      if (out_bas != NULL)
      {  lpx_set_int_parm(lp, LPX_K_MPSORIG, orig);
         ret = lpx_write_bas(lp, out_bas);
         if (ret != 0)
         {  xprintf("Unable to write final LP basis\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write problem solution found by the solver (if required) */
      if (out_sol != NULL)
      {  switch (method)
         {  case 0:
               ret = lpx_print_sol(lp, out_sol);
               break;
            case 1:
               ret = lpx_print_ips(lp, out_sol);
               break;
            case 2:
               ret = lpx_print_mip(lp, out_sol);
               break;
            default:
               xassert(method != method);
         }
         if (ret != 0)
         {  xprintf("Unable to write problem solution\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      if (out_res != NULL)
      {  switch (method)
         {  case 0:
               ret = glp_write_sol(lp, out_res);
               break;
            case 1:
               ret = glp_write_ipt(lp, out_res);
               break;
            case 2:
               ret = glp_write_mip(lp, out_res);
               break;
            default:
               xassert(method != method);
         }
         if (ret != 0)
         {  xprintf("Unable to write problem solution\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* write sensitivity bounds information (if required) */
      if (out_bnds != NULL)
      {  if (method != 0)
         {  xprintf("Cannot write sensitivity bounds information for in"
               "terior-point or MIP solution\n");
            ret = EXIT_FAILURE;
            goto done;
         }
         ret = lpx_print_sens_bnds(lp, out_bnds);
         if (ret != 0)
         {  xprintf("Unable to write sensitivity bounds information\n");
            ret = EXIT_FAILURE;
            goto done;
         }
      }
      /* all seems to be ok */
      ret = EXIT_SUCCESS;
done: /* delete the problem object */
      if (lp != NULL) lpx_delete_prob(lp);
      /* if the translator database exists, destroy it */
      if (mpl != NULL) mpl_terminate(mpl);
      xassert(gmp_pool_count() == 0);
      gmp_free_mem();
      /* close the hardcopy file */
      if (log_file != NULL) lib_close_log();
      /* check that no memory blocks are still allocated */
      {  int count;
         xlong_t total;
         lib_mem_usage(&count, NULL, &total, NULL);
         xassert(count == 0);
         xassert(total.lo == 0 && total.hi == 0);
      }
      /* free the library environment */
      lib_free_env();
      /* return to the control program */
      return ret;
}

/* eof */
