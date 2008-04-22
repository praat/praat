/* glplpx16.c */

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

#define _GLPSTD_ERRNO
#define _GLPSTD_STDIO
#include "glpapi.h"
#define xfault xerror

/*----------------------------------------------------------------------
-- lpx_read_cpxlp - read problem data in CPLEX LP format.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- LPX *lpx_read_cpxlp(char *fname);
--
-- *Description*
--
-- The routine lpx_read_cpxlp reads LP/MIP problem data in CPLEX LP
-- format from an input text file whose name is the character string
-- fname.
--
-- *Returns*
--
-- If no error occurred, the routine returns a pointer to the created
-- problem object. Otherwise the routine returns NULL. */

struct dsa
{     /* working area used by lpx_read_cpxlp routine */
      jmp_buf jump;
      /* label used for non-local go to in case of error */
      LPX *lp;
      /* LP/MIP problem object */
      const char *fname;
      /* name of input text file */
      FILE *fp;
      /* stream assigned to input text file */
      int count;
      /* line count */
      int c;
      /* current character or EOF */
      int token;
      /* code of current token: */
#define T_EOF        0  /* end of file */
#define T_MINIMIZE   1  /* keyword 'minimize' */
#define T_MAXIMIZE   2  /* keyword 'maximize' */
#define T_SUBJECT_TO 3  /* keyword 'subject to' */
#define T_BOUNDS     4  /* keyword 'bounds' */
#define T_GENERAL    5  /* keyword 'general' */
#define T_INTEGER    6  /* keyword 'integer' */
#define T_BINARY     7  /* keyword 'binary' */
#define T_END        8  /* keyword 'end' */
#define T_NAME       9  /* symbolic name */
#define T_NUMBER     10 /* numeric constant */
#define T_PLUS       11 /* delimiter '+' */
#define T_MINUS      12 /* delimiter '-' */
#define T_COLON      13 /* delimiter ':' */
#define T_LE         14 /* delimiter '<=' */
#define T_GE         15 /* delimiter '>=' */
#define T_EQ         16 /* delimiter '=' */
      char image[255+1];
      /* image of current token */
      int imlen;
      /* length of token image */
      double value;
      /* value of numeric constant */
      int n_max;
      /* length of the following five arrays (enlarged automatically,
         if necessary) */
      int *map; /* int map[1+n_max]; */
      int *ind; /* int ind[1+n_max]; */
      double *val; /* double val[1+n_max]; */
      /* working arrays used for constructing linear forms */
      double *lb; /* double lb[1+n_max]; */
      double *ub; /* double ub[1+n_max]; */
      /* lower and upper bounds of variables (columns) */
};

#define CHAR_SET "!\"#$%&()/,.;?@_`'{}|~"
/* characters which may appear in symbolic names */

static void fatal(struct dsa *dsa, char *fmt, ...)
{     /* print error message and terminate processing */
      va_list arg;
      char msg[4095+1];
      va_start(arg, fmt);
      vsprintf(msg, fmt, arg);
      xassert(strlen(msg) <= 4095);
      va_end(arg);
      xprintf("%s:%d: %s\n", dsa->fname, dsa->count, msg);
      longjmp(dsa->jump, 1);
      /* no return */
}

static void read_char(struct dsa *dsa)
{     /* read next character from input file */
      int c;
      xassert(dsa->c != EOF);
      if (dsa->c == '\n') dsa->count++;
      c = fgetc(dsa->fp);
      if (ferror(dsa->fp))
         fatal(dsa, "read error - %s", strerror(errno));
      if (feof(dsa->fp))
      {  if (dsa->c == '\n')
         {  dsa->count--;
            c = EOF;
         }
         else
         {  xprintf("%s:%d: warning: missing final LF\n",
               dsa->fname, dsa->count);
            c = '\n';
         }
      }
      else if (c == '\n')
         ;
      else if (isspace(c))
         c = ' ';
      else if (iscntrl(c))
         fatal(dsa, "invalid control character 0x%02X", c);
      dsa->c = c;
      return;
}

static void add_char(struct dsa *dsa)
{     /* append current character to current token */
      if (dsa->imlen == sizeof(dsa->image) - 1)
         fatal(dsa, "token `%.15s...' too long", dsa->image);
      dsa->image[dsa->imlen++] = (char)dsa->c;
      dsa->image[dsa->imlen] = '\0';
      read_char(dsa);
      return;
}

static int the_same(char *s1, char *s2)
{     /* compare two character strings without case sensitivity */
      for (; *s1 != '\0'; s1++, s2++)
         if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return 0;
      return 1;
}

static void scan_token(struct dsa *dsa)
{     /* scan next token */
      int flag;
      dsa->token = -1;
      dsa->image[0] = '\0';
      dsa->imlen = 0;
      dsa->value = 0.0;
loop: flag = 0;
      /* skip non-significant characters */
      while (dsa->c == ' ') read_char(dsa);
      /* recognize and scan current token */
      if (dsa->c == EOF)
         dsa->token = T_EOF;
      else if (dsa->c == '\n')
      {  read_char(dsa);
         /* if the next character is letter, it may begin a keyword */
         if (isalpha(dsa->c))
         {  flag = 1;
            goto name;
         }
         goto loop;
      }
      else if (dsa->c == '\\')
      {  /* comment; ignore everything until end-of-line */
         while (dsa->c != '\n') read_char(dsa);
         goto loop;
      }
      else if (isalpha(dsa->c) || dsa->c != '.' && strchr(CHAR_SET,
         dsa->c) != NULL)
name: {  /* symbolic name */
         dsa->token = T_NAME;
         while (isalnum(dsa->c) || strchr(CHAR_SET, dsa->c) != NULL)
            add_char(dsa);
         if (flag)
         {  /* check for keyword */
            if (the_same(dsa->image, "minimize"))
               dsa->token = T_MINIMIZE;
            else if (the_same(dsa->image, "minimum"))
               dsa->token = T_MINIMIZE;
            else if (the_same(dsa->image, "min"))
               dsa->token = T_MINIMIZE;
            else if (the_same(dsa->image, "maximize"))
               dsa->token = T_MAXIMIZE;
            else if (the_same(dsa->image, "maximum"))
               dsa->token = T_MAXIMIZE;
            else if (the_same(dsa->image, "max"))
               dsa->token = T_MAXIMIZE;
            else if (the_same(dsa->image, "subject"))
            {  if (dsa->c == ' ')
               {  read_char(dsa);
                  if (tolower(dsa->c) == 't')
                  {  dsa->token = T_SUBJECT_TO;
                     dsa->image[dsa->imlen++] = ' ';
                     dsa->image[dsa->imlen] = '\0';
                     add_char(dsa);
                     if (tolower(dsa->c) != 'o')
                        fatal(dsa, "keyword `subject to' incomplete");
                     add_char(dsa);
                     if (isalpha(dsa->c))
                        fatal(dsa, "keyword `%s%c...' not recognized",
                           dsa->image, dsa->c);
                  }
               }
            }
            else if (the_same(dsa->image, "such"))
            {  if (dsa->c == ' ')
               {  read_char(dsa);
                  if (tolower(dsa->c) == 't')
                  {  dsa->token = T_SUBJECT_TO;
                     dsa->image[dsa->imlen++] = ' ';
                     dsa->image[dsa->imlen] = '\0';
                     add_char(dsa);
                     if (tolower(dsa->c) != 'h')
err:                    fatal(dsa, "keyword `such that' incomplete");
                     add_char(dsa);
                     if (tolower(dsa->c) != 'a') goto err;
                     add_char(dsa);
                     if (tolower(dsa->c) != 't') goto err;
                     add_char(dsa);
                     if (isalpha(dsa->c))
                        fatal(dsa, "keyword `%s%c...' not recognized",
                           dsa->image, dsa->c);
                  }
               }
            }
            else if (the_same(dsa->image, "st"))
               dsa->token = T_SUBJECT_TO;
            else if (the_same(dsa->image, "s.t."))
               dsa->token = T_SUBJECT_TO;
            else if (the_same(dsa->image, "st."))
               dsa->token = T_SUBJECT_TO;
            else if (the_same(dsa->image, "bounds"))
               dsa->token = T_BOUNDS;
            else if (the_same(dsa->image, "bound"))
               dsa->token = T_BOUNDS;
            else if (the_same(dsa->image, "general"))
               dsa->token = T_GENERAL;
            else if (the_same(dsa->image, "generals"))
               dsa->token = T_GENERAL;
            else if (the_same(dsa->image, "gen"))
               dsa->token = T_GENERAL;
            else if (the_same(dsa->image, "integer"))
               dsa->token = T_INTEGER;
            else if (the_same(dsa->image, "integers"))
               dsa->token = T_INTEGER;
            else if (the_same(dsa->image, "int"))
              dsa->token = T_INTEGER;
            else if (the_same(dsa->image, "binary"))
               dsa->token = T_BINARY;
            else if (the_same(dsa->image, "binaries"))
               dsa->token = T_BINARY;
            else if (the_same(dsa->image, "bin"))
               dsa->token = T_BINARY;
            else if (the_same(dsa->image, "end"))
               dsa->token = T_END;
         }
      }
      else if (isdigit(dsa->c) || dsa->c == '.')
      {  /* numeric constant */
         dsa->token = T_NUMBER;
         /* scan integer part */
         while (isdigit(dsa->c)) add_char(dsa);
         /* scan optional fractional part (it is mandatory, if there is
            no integer part) */
         if (dsa->c == '.')
         {  add_char(dsa);
            if (dsa->imlen == 1 && !isdigit(dsa->c))
               fatal(dsa, "invalid use of decimal point");
            while (isdigit(dsa->c)) add_char(dsa);
         }
         /* scan optional decimal exponent */
         if (dsa->c == 'e' || dsa->c == 'E')
         {  add_char(dsa);
            if (dsa->c == '+' || dsa->c == '-') add_char(dsa);
            if (!isdigit(dsa->c))
               fatal(dsa, "numeric constant `%s' incomplete",
                  dsa->image);
            while (isdigit(dsa->c)) add_char(dsa);
         }
         /* convert the numeric constant to floating-point */
         if (str2num(dsa->image, &dsa->value))
            fatal(dsa, "numeric constant `%s' out of range",
               dsa->image);
      }
      else if (dsa->c == '+')
         dsa->token = T_PLUS, add_char(dsa);
      else if (dsa->c == '-')
         dsa->token = T_MINUS, add_char(dsa);
      else if (dsa->c == ':')
         dsa->token = T_COLON, add_char(dsa);
      else if (dsa->c == '<')
      {  dsa->token = T_LE, add_char(dsa);
         if (dsa->c == '=') add_char(dsa);
      }
      else if (dsa->c == '>')
      {  dsa->token = T_GE, add_char(dsa);
         if (dsa->c == '=') add_char(dsa);
      }
      else if (dsa->c == '=')
      {  dsa->token = T_EQ, add_char(dsa);
         if (dsa->c == '<')
            dsa->token = T_LE, add_char(dsa);
         else if (dsa->c == '>')
            dsa->token = T_GE, add_char(dsa);
      }
      else
         fatal(dsa, "character `%c' not recognized", dsa->c);
      /* skip non-significant characters */
      while (dsa->c == ' ') read_char(dsa);
      return;
}

static int find_col(struct dsa *dsa, char *name)
{     /* find column by its symbolic name */
      int j;
      j = lpx_find_col(dsa->lp, name);
      if (j == 0)
      {  /* not found; create new column */
         j = lpx_add_cols(dsa->lp, 1);
         lpx_set_col_name(dsa->lp, j, name);
         /* enlarge auxiliary arrays, if necessary */
         if (dsa->n_max < j)
         {  int n_max = dsa->n_max;
            int *map = dsa->map;
            int *ind = dsa->ind;
            double *val = dsa->val;
            double *lb = dsa->lb;
            double *ub = dsa->ub;
            dsa->n_max += dsa->n_max;
            dsa->map = xcalloc(1+dsa->n_max, sizeof(int));
            memset(&dsa->map[1], 0, dsa->n_max * sizeof(int));
            memcpy(&dsa->map[1], &map[1], n_max * sizeof(int));
            xfree(map);
            dsa->ind = xcalloc(1+dsa->n_max, sizeof(int));
            memcpy(&dsa->ind[1], &ind[1], n_max * sizeof(int));
            xfree(ind);
            dsa->val = xcalloc(1+dsa->n_max, sizeof(double));
            memcpy(&dsa->val[1], &val[1], n_max * sizeof(double));
            xfree(val);
            dsa->lb = xcalloc(1+dsa->n_max, sizeof(double));
            memcpy(&dsa->lb[1], &lb[1], n_max * sizeof(double));
            xfree(lb);
            dsa->ub = xcalloc(1+dsa->n_max, sizeof(double));
            memcpy(&dsa->ub[1], &ub[1], n_max * sizeof(double));
            xfree(ub);
         }
         dsa->lb[j] = +DBL_MAX, dsa->ub[j] = -DBL_MAX;
      }
      return j;
}

/*----------------------------------------------------------------------
-- parse_linear_form - parse linear form.
--
-- This routine parses linear form using the following syntax:
--
-- <variable> ::= <symbolic name>
-- <coefficient> ::= <numeric constant>
-- <term> ::= <variable> | <numeric constant> <variable>
-- <linear form> ::= <term> | + <term> | - <term> |
--    <linear form> + <term> | <linear form> - <term>
--
-- The routine returns the number of terms in the linear form. */

static int parse_linear_form(struct dsa *dsa)
{     int j, k, len = 0, newlen;
      double s, coef;
loop: /* parse an optional sign */
      if (dsa->token == T_PLUS)
         s = +1.0, scan_token(dsa);
      else if (dsa->token == T_MINUS)
         s = -1.0, scan_token(dsa);
      else
         s = +1.0;
      /* parse an optional coefficient */
      if (dsa->token == T_NUMBER)
         coef = dsa->value, scan_token(dsa);
      else
         coef = 1.0;
      /* parse a variable name */
      if (dsa->token != T_NAME)
         fatal(dsa, "missing variable name");
      /* find the corresponding column */
      j = find_col(dsa, dsa->image);
      /* check if the variable is already used in the linear form */
      if (dsa->map[j])
         fatal(dsa, "multiple use of variable `%s' not allowed",
            dsa->image);
      /* mark that the variable is used in the linear form */
      dsa->map[j] = 1;
      /* add new term to the linear form */
      len++, dsa->ind[len] = j, dsa->val[len] = s * coef;
      scan_token(dsa);
      /* if the next token is a sign, there is another term */
      if (dsa->token == T_PLUS || dsa->token == T_MINUS) goto loop;
      /* clear marks of the variables used in the linear form */
      for (k = 1; k <= len; k++) dsa->map[dsa->ind[k]] = 0;
      /* remove zero coefficients */
      newlen = 0;
      for (k = 1; k <= len; k++)
      {  if (dsa->val[k] != 0.0)
         {  newlen++;
            dsa->ind[newlen] = dsa->ind[k];
            dsa->val[newlen] = dsa->val[k];
         }
      }
      return newlen;
}

/*----------------------------------------------------------------------
-- parse_objective - parse objective function.
--
-- This routine parses definition of the objective function using the
-- following syntax:
--
-- <obj sense> ::= minimize | minimum | min | maximize | maximum | max
-- <obj name> ::= <empty> | <symbolic name> :
-- <obj function> ::= <obj sense> <obj name> <linear form> */

static void parse_objective(struct dsa *dsa)
{     /* parse objective sense */
      int k, len;
      /* parse the keyword 'minimize' or 'maximize' */
      if (dsa->token == T_MINIMIZE)
         lpx_set_obj_dir(dsa->lp, LPX_MIN);
      else if (dsa->token == T_MAXIMIZE)
         lpx_set_obj_dir(dsa->lp, LPX_MAX);
      else
         xassert(dsa != dsa);
      scan_token(dsa);
      /* parse objective name */
      if (dsa->token == T_NAME && dsa->c == ':')
      {  /* objective name is followed by a colon */
         lpx_set_obj_name(dsa->lp, dsa->image);
         scan_token(dsa);
         xassert(dsa->token == T_COLON);
         scan_token(dsa);
      }
      else
      {  /* objective name is not specified; use default */
         lpx_set_obj_name(dsa->lp, "obj");
      }
      /* parse linear form */
      len = parse_linear_form(dsa);
      for (k = 1; k <= len; k++)
         lpx_set_obj_coef(dsa->lp, dsa->ind[k], dsa->val[k]);
      return;
}

/*----------------------------------------------------------------------
-- parse_constraints - parse constraints section.
--
-- This routine parses the constraints section using the following
-- syntax:
--
-- <row name> ::= <empty> | <symbolic name> :
-- <row sense> ::= < | <= | =< | > | >= | => | =
-- <right-hand side> ::= <numeric constant> | + <numeric constant> |
--    - <numeric constant>
-- <constraint> ::= <row name> <linear form> <row sense>
--    <right-hand side>
-- <subject to> ::= subject to | such that | st | s.t. | st.
-- <constraints section> ::= <subject to> <constraint> |
--    <constraints section> <constraint> */

static void parse_constraints(struct dsa *dsa)
{     int i, len, type;
      double s;
      /* parse the keyword 'subject to' */
      xassert(dsa->token == T_SUBJECT_TO);
      scan_token(dsa);
loop: /* create new row (constraint) */
      i = lpx_add_rows(dsa->lp, 1);
      /* parse row name */
      if (dsa->token == T_NAME && dsa->c == ':')
      {  /* row name is followed by a colon */
         if (lpx_find_row(dsa->lp, dsa->image) != 0)
            fatal(dsa, "constraint `%s' multiply defined", dsa->image);
         lpx_set_row_name(dsa->lp, i, dsa->image);
         scan_token(dsa);
         xassert(dsa->token == T_COLON);
         scan_token(dsa);
      }
      else
      {  /* row name is not specified; use default */
         char name[50];
         sprintf(name, "r.%d", dsa->count);
         lpx_set_row_name(dsa->lp, i, name);
      }
      /* parse linear form */
      len = parse_linear_form(dsa);
      lpx_set_mat_row(dsa->lp, i, len, dsa->ind, dsa->val);
      /* parse constraint sense */
      if (dsa->token == T_LE)
         type = LPX_UP, scan_token(dsa);
      else if (dsa->token == T_GE)
         type = LPX_LO, scan_token(dsa);
      else if (dsa->token == T_EQ)
         type = LPX_FX, scan_token(dsa);
      else
         fatal(dsa, "missing constraint sense");
      /* parse right-hand side */
      if (dsa->token == T_PLUS)
         s = +1.0, scan_token(dsa);
      else if (dsa->token == T_MINUS)
         s = -1.0, scan_token(dsa);
      else
         s = +1.0;
      if (dsa->token != T_NUMBER)
         fatal(dsa, "missing right-hand side");
      switch (type)
      {  case LPX_LO:
            lpx_set_row_bnds(dsa->lp, i, LPX_LO, s * dsa->value, 0.0);
            break;
         case LPX_UP:
            lpx_set_row_bnds(dsa->lp, i, LPX_UP, 0.0, s * dsa->value);
            break;
         case LPX_FX:
            lpx_set_row_bnds(dsa->lp, i, LPX_FX, s * dsa->value, 0.0);
            break;
      }
      /* the rest of the current line must be empty */
      if (!(dsa->c == '\n' || dsa->c == EOF))
         fatal(dsa, "invalid symbol(s) beyond right-hand side");
      scan_token(dsa);
      /* if the next token is a sign, numeric constant, or a symbolic
         name, here is another constraint */
      if (dsa->token == T_PLUS || dsa->token == T_MINUS ||
         dsa->token == T_NUMBER || dsa->token == T_NAME) goto loop;
      return;
}

static void set_lower_bound(struct dsa *dsa, int j, double lb)
{     /* set upper bound of j-th variable */
      if (dsa->lb[j] != +DBL_MAX)
         xprintf(
            "%s:%d: warning: lower bound of variable `%s' redefined\n",
            dsa->fname, dsa->count, lpx_get_col_name(dsa->lp, j));
      dsa->lb[j] = lb;
      return;
}

static void set_upper_bound(struct dsa *dsa, int j, double ub)
{     /* set upper bound of j-th variable */
      if (dsa->ub[j] != -DBL_MAX)
         xprintf(
            "%s:%d: warning: upper bound of variable `%s' redefined\n",
            dsa->fname, dsa->count, lpx_get_col_name(dsa->lp, j));
      dsa->ub[j] = ub;
      return;
}

/*----------------------------------------------------------------------
-- parse_bounds - parse bounds section.
--
-- This routine parses the bounds section using the following syntax:
--
-- <variable> ::= <symbolic name>
-- <infinity> ::= infinity | inf
-- <bound> ::= <numeric constant> | + <numeric constant> |
--    - <numeric constant> | + <infinity> | - <infinity>
-- <lt> ::= < | <= | =<
-- <gt> ::= > | >= | =>
-- <bound definition> ::= <bound> <lt> <variable> <lt> <bound> |
--    <bound> <lt> <variable> | <variable> <lt> <bound> |
--    <variable> <gt> <bound> | <variable> = <bound> | <variable> free
-- <bounds> ::= bounds | bound
-- <bounds section> ::= <bounds> |
--    <bounds section> <bound definition> */

static void parse_bounds(struct dsa *dsa)
{     int j, lb_flag;
      double lb, s;
      /* parse the keyword 'bounds' */
      xassert(dsa->token == T_BOUNDS);
      scan_token(dsa);
loop: /* bound definition can start with a sign, numeric constant, or
         a symbolic name */
      if (!(dsa->token == T_PLUS || dsa->token == T_MINUS ||
            dsa->token == T_NUMBER || dsa->token == T_NAME)) goto done;
      /* parse bound definition */
      if (dsa->token == T_PLUS || dsa->token == T_MINUS)
      {  /* parse signed lower bound */
         lb_flag = 1;
         s = (dsa->token == T_PLUS ? +1.0 : -1.0);
         scan_token(dsa);
         if (dsa->token == T_NUMBER)
            lb = s * dsa->value, scan_token(dsa);
         else if (the_same(dsa->image, "infinity") ||
                  the_same(dsa->image, "inf"))
         {  if (s > 0.0)
               fatal(dsa, "invalid use of `+inf' as lower bound");
            lb = -DBL_MAX, scan_token(dsa);
         }
         else
            fatal(dsa, "missing lower bound");
      }
      else if (dsa->token == T_NUMBER)
      {  /* parse unsigned lower bound */
         lb_flag = 1;
         lb = dsa->value, scan_token(dsa);
      }
      else
      {  /* lower bound is not specified */
         lb_flag = 0;
      }
      /* parse the token that should follow the lower bound */
      if (lb_flag)
      {  if (dsa->token != T_LE)
            fatal(dsa, "missing `<', `<=', or `=<' after lower bound");
         scan_token(dsa);
      }
      /* parse variable name */
      if (dsa->token != T_NAME)
         fatal(dsa, "missing variable name");
      j = find_col(dsa, dsa->image);
      /* set lower bound */
      if (lb_flag) set_lower_bound(dsa, j, lb);
      scan_token(dsa);
      /* parse the context that follows the variable name */
      if (dsa->token == T_LE)
      {  /* parse upper bound */
         scan_token(dsa);
         if (dsa->token == T_PLUS || dsa->token == T_MINUS)
         {  /* parse signed upper bound */
            s = (dsa->token == T_PLUS ? +1.0 : -1.0);
            scan_token(dsa);
            if (dsa->token == T_NUMBER)
            {  set_upper_bound(dsa, j, s * dsa->value);
               scan_token(dsa);
            }
            else if (the_same(dsa->image, "infinity") ||
                     the_same(dsa->image, "inf"))
            {  if (s < 0.0)
                  fatal(dsa, "invalid use of `-inf' as upper bound");
               set_upper_bound(dsa, j, +DBL_MAX);
               scan_token(dsa);
            }
            else
               fatal(dsa, "missing upper bound");
         }
         else if (dsa->token == T_NUMBER)
         {  /* parse unsigned upper bound */
            set_upper_bound(dsa, j, dsa->value);
            scan_token(dsa);
         }
         else
            fatal(dsa, "missing upper bound");
      }
      else if (dsa->token == T_GE)
      {  /* parse lower bound */
         if (lb_flag)
         {  /* the context '... <= x >= ...' is invalid */
            fatal(dsa, "invalid bound definition");
         }
         scan_token(dsa);
         if (dsa->token == T_PLUS || dsa->token == T_MINUS)
         {  /* parse signed lower bound */
            s = (dsa->token == T_PLUS ? +1.0 : -1.0);
            scan_token(dsa);
            if (dsa->token == T_NUMBER)
            {  set_lower_bound(dsa, j, s * dsa->value);
               scan_token(dsa);
            }
            else if (the_same(dsa->image, "infinity") ||
                     the_same(dsa->image, "inf") == 0)
            {  if (s > 0.0)
                  fatal(dsa, "invalid use of `+inf' as lower bound");
               set_lower_bound(dsa, j, -DBL_MAX);
               scan_token(dsa);
            }
            else
               fatal(dsa, "missing lower bound");
         }
         else if (dsa->token == T_NUMBER)
         {  /* parse unsigned lower bound */
            set_lower_bound(dsa, j, dsa->value);
            scan_token(dsa);
         }
         else
            fatal(dsa, "missing lower bound");
      }
      else if (dsa->token == T_EQ)
      {  /* parse fixed value */
         if (lb_flag)
         {  /* the context '... <= x = ...' is invalid */
            fatal(dsa, "invalid bound definition");
         }
         scan_token(dsa);
         if (dsa->token == T_PLUS || dsa->token == T_MINUS)
         {  /* parse signed fixed value */
            s = (dsa->token == T_PLUS ? +1.0 : -1.0);
            scan_token(dsa);
            if (dsa->token == T_NUMBER)
            {  set_lower_bound(dsa, j, s * dsa->value);
               set_upper_bound(dsa, j, s * dsa->value);
               scan_token(dsa);
            }
            else
               fatal(dsa, "missing fixed value");
         }
         else if (dsa->token == T_NUMBER)
         {  /* parse unsigned fixed value */
            set_lower_bound(dsa, j, dsa->value);
            set_upper_bound(dsa, j, dsa->value);
            scan_token(dsa);
         }
         else
            fatal(dsa, "missing fixed value");
      }
      else if (the_same(dsa->image, "free"))
      {  /* parse the keyword 'free' */
         if (lb_flag)
         {  /* the context '... <= x free ...' is invalid */
            fatal(dsa, "invalid bound definition");
         }
         set_lower_bound(dsa, j, -DBL_MAX);
         set_upper_bound(dsa, j, +DBL_MAX);
         scan_token(dsa);
      }
      else if (!lb_flag)
      {  /* neither lower nor upper bounds are specified */
         fatal(dsa, "invalid bound definition");
      }
      goto loop;
done: return;
}

/*----------------------------------------------------------------------
-- parse_integer - parse general, integer, or binary section.
--
-- <variable> ::= <symbolic name>
-- <general> ::= general | generals | gen
-- <integer> ::= integer | integers | int
-- <binary> ::= binary | binaries | bin
-- <section head> ::= <general> <integer> <binary>
-- <additional section> ::= <section head> |
--    <additional section> <variable> */

static void parse_integer(struct dsa *dsa)
{     int j, binary;
      /* parse the keyword 'general', 'integer', or 'binary' */
      if (dsa->token == T_GENERAL)
         binary = 0, scan_token(dsa);
      else if (dsa->token == T_INTEGER)
         binary = 0, scan_token(dsa);
      else if (dsa->token == T_BINARY)
         binary = 1, scan_token(dsa);
      else
         xassert(dsa != dsa);
      /* parse list of variables (may be empty) */
      while (dsa->token == T_NAME)
      {  /* find the corresponding column */
         j = find_col(dsa, dsa->image);
         /* change kind of the variable */
#if 0
         lpx_set_class(dsa->lp, LPX_MIP);
#endif
         lpx_set_col_kind(dsa->lp, j, LPX_IV);
         /* set 0-1 bounds for the binary variable */
         if (binary)
         {  set_lower_bound(dsa, j, 0.0);
            set_upper_bound(dsa, j, 1.0);
         }
         scan_token(dsa);
      }
      return;
}

LPX *lpx_read_cpxlp(const char *fname)
{     /* read problem data in CPLEX LP format */
      struct dsa _dsa, *dsa = &_dsa;
      if (setjmp(dsa->jump)) goto fail;
      dsa->lp = NULL;
      dsa->fname = fname;
      dsa->fp = NULL;
      dsa->count = 0;
      dsa->c = '\n';
      dsa->token = T_EOF;
      dsa->image[0] = '\0';
      dsa->imlen = 0;
      dsa->value = 0.0;
      dsa->n_max = 100;
      dsa->map = xcalloc(1+dsa->n_max, sizeof(int));
      memset(&dsa->map[1], 0, dsa->n_max * sizeof(int));
      dsa->ind = xcalloc(1+dsa->n_max, sizeof(int));
      dsa->val = xcalloc(1+dsa->n_max, sizeof(double));
      dsa->lb = xcalloc(1+dsa->n_max, sizeof(double));
      dsa->ub = xcalloc(1+dsa->n_max, sizeof(double));
      xprintf("lpx_read_cpxlp: reading problem data from `%s'...\n",
         dsa->fname);
      dsa->fp = fopen(dsa->fname, "r");
      if (dsa->fp == NULL)
      {  xprintf("lpx_read_cpxlp: unable to open `%s' - %s\n",
            dsa->fname, strerror(errno));
         goto fail;
      }
      dsa->lp = lpx_create_prob();
      lpx_create_index(dsa->lp);
#if 0
      /* read very first character */
      read_char(dsa);
#endif
      /* scan very first token */
      scan_token(dsa);
      /* parse definition of the objective function */
      if (!(dsa->token == T_MINIMIZE || dsa->token == T_MAXIMIZE))
         fatal(dsa, "`minimize' or `maximize' keyword missing");
      parse_objective(dsa);
      /* parse constraints section */
      if (dsa->token != T_SUBJECT_TO)
         fatal(dsa, "constraints section missing");
      parse_constraints(dsa);
      /* parse optional bounds section */
      if (dsa->token == T_BOUNDS) parse_bounds(dsa);
      /* parse optional general, integer, and binary sections */
      while (dsa->token == T_GENERAL ||
             dsa->token == T_INTEGER ||
             dsa->token == T_BINARY) parse_integer(dsa);
      /* check for the keyword 'end' */
      if (dsa->token == T_END)
         scan_token(dsa);
      else if (dsa->token == T_EOF)
         xprintf("%s:%d: warning: keyword `end' missing\n",
            dsa->fname, dsa->count);
      else
         fatal(dsa, "symbol `%s' in wrong position", dsa->image);
      /* nothing must follow the keyword 'end' (except comments) */
      if (dsa->token != T_EOF)
         fatal(dsa, "extra symbol(s) detected beyond `end'");
      /* set bounds of variables */
      {  int j, type;
         double lb, ub;
         for (j = lpx_get_num_cols(dsa->lp); j >= 1; j--)
         {  lb = dsa->lb[j];
            ub = dsa->ub[j];
            if (lb == +DBL_MAX) lb = 0.0;      /* default lb */
            if (ub == -DBL_MAX) ub = +DBL_MAX; /* default ub */
            if (lb == -DBL_MAX && ub == +DBL_MAX)
               type = LPX_FR;
            else if (ub == +DBL_MAX)
               type = LPX_LO;
            else if (lb == -DBL_MAX)
               type = LPX_UP;
            else if (lb != ub)
               type = LPX_DB;
            else
               type = LPX_FX;
            lpx_set_col_bnds(dsa->lp, j, type, lb, ub);
         }
      }
      /* print some statistics */
      {  int m = lpx_get_num_rows(dsa->lp);
         int n = lpx_get_num_cols(dsa->lp);
         int nnz = lpx_get_num_nz(dsa->lp);
         xprintf(
            "lpx_read_cpxlp: %d row%s, %d column%s, %d non-zero%s\n",
            m, m == 1 ? "" : "s", n, n == 1 ? "" : "s", nnz, nnz == 1 ?
            "" : "s");
      }
      if (lpx_get_class(dsa->lp) == LPX_MIP)
      {  int ni = lpx_get_num_int(dsa->lp);
         int nb = lpx_get_num_bin(dsa->lp);
         char s[50];
         if (nb == 0)
            strcpy(s, "none of");
         else if (ni == 1 && nb == 1)
            strcpy(s, "");
         else if (nb == 1)
            strcpy(s, "one of");
         else if (nb == ni)
            strcpy(s, "all of");
         else
            sprintf(s, "%d of", nb);
         xprintf(
            "lpx_read_cpxlp: %d integer column%s, %s which %s binary\n"
            , ni, ni == 1 ? "" : "s", s, nb == 1 ? "is" : "are");
      }
      xprintf("lpx_read_cpxlp: %d lines were read\n", dsa->count);
      fclose(dsa->fp);
      xfree(dsa->map);
      xfree(dsa->ind);
      xfree(dsa->val);
      xfree(dsa->lb);
      xfree(dsa->ub);
      lpx_delete_index(dsa->lp);
      lpx_order_matrix(dsa->lp);
      return dsa->lp;
fail: if (dsa->lp != NULL) lpx_delete_prob(dsa->lp);
      if (dsa->fp != NULL) fclose(dsa->fp);
      if (dsa->map != NULL) xfree(dsa->map);
      if (dsa->ind != NULL) xfree(dsa->ind);
      if (dsa->val != NULL) xfree(dsa->val);
      if (dsa->lb != NULL) xfree(dsa->lb);
      if (dsa->ub != NULL) xfree(dsa->ub);
      return NULL;
}

/*----------------------------------------------------------------------
-- lpx_write_cpxlp - write problem data in CPLEX LP format.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- int lpx_write_cpxlp(LPX *lp, char *fname);
--
-- *Description*
--
-- The routine lpx_write_cpxlp writes problem data in CPLEX LP format
-- to an output text file whose name is the character string fname.
--
-- *Returns*
--
-- If the operation was successful, the routine returns zero. Otherwise
-- the routine prints an error message and returns non-zero. */

static int check_name(char *name)
{     /* check if given name is valid for CPLEX LP format */
      int k;
      if (isdigit((unsigned char)name[0])) return 1;
      if (name[0] == '.') return 1;
      for (k = 0; name[k] != '\0'; k++)
         if (!isalnum((unsigned char)name[k]) &&
            strchr(CHAR_SET, (unsigned char)name[k]) == NULL) return 1;
      return 0; /* name is ok */
}

static void adjust_name(char *name)
{     /* try changing given name to make it valid for CPLEX LP format */
      int k;
      for (k = 0; name[k] != '\0'; k++)
      {  if (name[k] == ' ')
            name[k] = '_';
         else if (name[k] == '-')
            name[k] = '~';
         else if (name[k] == '[')
            name[k] = '(';
         else if (name[k] == ']')
            name[k] = ')';
      }
      return;
}

static char *row_name(LPX *lp, int i, char rname[255+1])
{     /* construct symbolic name of i-th row (constraint) */
      char *name;
      if (i == 0)
         name = (void *)lpx_get_obj_name(lp);
      else
         name = (void *)lpx_get_row_name(lp, i);
      if (name == NULL) goto fake;
      strcpy(rname, name);
      adjust_name(rname);
      if (check_name(rname)) goto fake;
      return rname;
fake: if (i == 0)
         strcpy(rname, "obj");
      else
         sprintf(rname, "r_%d", i);
      return rname;
}

static char *col_name(LPX *lp, int j, char cname[255+1])
{     /* construct symbolic name of j-th column (variable) */
      const char *name;
      name = lpx_get_col_name(lp, j);
      if (name == NULL) goto fake;
      strcpy(cname, name);
      adjust_name(cname);
      if (check_name(cname)) goto fake;
      return cname;
fake: sprintf(cname, "x_%d", j);
      return cname;
}

int lpx_write_cpxlp(LPX *lp, const char *fname)
{     /* write problem data in CPLEX LP format */
      FILE *fp;
      int nrows, ncols, i, j, t, len, typx, flag, kind, *ind;
      double lb, ub, temp, *val;
      char line[1023+1], term[1023+1], rname[255+1], cname[255+1];
      xprintf("lpx_write_cpxlp: writing problem data to `%s'...\n",
         fname);
      /* open the output text file */
      fp = fopen(fname, "w");
      if (fp == NULL)
      {  xprintf("lpx_write_cpxlp: unable to create `%s' - %s\n",
            fname, strerror(errno));
         goto fail;
      }
      /* determine the number of rows and columns */
      nrows = lpx_get_num_rows(lp);
      ncols = lpx_get_num_cols(lp);
      /* the problem should contain at least one row and one column */
      if (!(nrows > 0 && ncols > 0))
         xfault("lpx_write_cpxlp: problem has no rows/columns\n");
      /* write problem name */
      {  const char *name = lpx_get_prob_name(lp);
         if (name == NULL) name = "Unknown";
         fprintf(fp, "\\* Problem: %s *\\\n", name);
         fprintf(fp, "\n");
      }
      /* allocate working arrays */
      ind = xcalloc(1+ncols, sizeof(int));
      val = xcalloc(1+ncols, sizeof(double));
      /* write the objective function definition and the constraints
         section */
      for (i = 0; i <= nrows; i++)
      {  if (i == 0)
         {  switch (lpx_get_obj_dir(lp))
            {  case LPX_MIN:
                  fprintf(fp, "Minimize\n");
                  break;
               case LPX_MAX:
                  fprintf(fp, "Maximize\n");
                  break;
               default:
                  xassert(lp != lp);
            }
         }
         else if (i == 1)
         {  temp = lpx_get_obj_coef(lp, 0);
            if (temp != 0.0)
               fprintf(fp, "\\* constant term = %.*g *\\\n", DBL_DIG,
                  temp);
            fprintf(fp, "\n");
            fprintf(fp, "Subject To\n");
         }
         row_name(lp, i, rname);
         if (i == 0)
         {  len = 0;
            for (j = 1; j <= ncols; j++)
            {  temp = lpx_get_obj_coef(lp, j);
               if (temp != 0.0)
                  len++, ind[len] = j, val[len] = temp;
            }
         }
         else
         {  lpx_get_row_bnds(lp, i, &typx, &lb, &ub);
            if (typx == LPX_FR) continue;
            len = lpx_get_mat_row(lp, i, ind, val);
         }
         flag = 0;
more:    if (!flag)
            sprintf(line, " %s:", rname);
         else
            sprintf(line, " %*s ", strlen(rname), "");
         for (t = 1; t <= len; t++)
         {  col_name(lp, ind[t], cname);
            if (val[t] == +1.0)
               sprintf(term, " + %s", cname);
            else if (val[t] == -1.0)
               sprintf(term, " - %s", cname);
            else if (val[t] > 0.0)
               sprintf(term, " + %.*g %s", DBL_DIG, +val[t], cname);
            else if (val[t] < 0.0)
               sprintf(term, " - %.*g %s", DBL_DIG, -val[t], cname);
            else
               xassert(lp != lp);
            if (strlen(line) + strlen(term) > 72)
               fprintf(fp, "%s\n", line), line[0] = '\0';
            strcat(line, term);
         }
         if (len == 0)
         {  /* empty row */
            sprintf(term, " 0 %s", col_name(lp, 1, cname));
            strcat(line, term);
         }
         if (i > 0)
         {  switch (typx)
            {  case LPX_LO:
               case LPX_DB:
                  sprintf(term, " >= %.*g", DBL_DIG, lb);
                  break;
               case LPX_UP:
                  sprintf(term, " <= %.*g", DBL_DIG, ub);
                  break;
               case LPX_FX:
                  sprintf(term, " = %.*g", DBL_DIG, lb);
                  break;
               default:
                  xassert(typx != typx);
            }
            if (strlen(line) + strlen(term) > 72)
               fprintf(fp, "%s\n", line), line[0] = '\0';
            strcat(line, term);
         }
         fprintf(fp, "%s\n", line);
         if (i > 0 && typx == LPX_DB)
         {  /* double-bounded row needs a copy for its upper bound */
            flag = 1;
            typx = LPX_UP;
            goto more;
         }
      }
      /* free working arrays */
      xfree(ind);
      xfree(val);
      /* write the bounds section */
      flag = 0;
      for (j = 1; j <= ncols; j++)
      {  col_name(lp, j, cname);
         lpx_get_col_bnds(lp, j, &typx, &lb, &ub);
         if (typx == LPX_LO && lb == 0.0) continue;
         if (!flag)
         {  fprintf(fp, "\n");
            fprintf(fp, "Bounds\n");
            flag = 1;
         }
         switch (typx)
         {  case LPX_FR:
               fprintf(fp, " %s free\n", cname);
               break;
            case LPX_LO:
               fprintf(fp, " %s >= %.*g\n", cname, DBL_DIG, lb);
               break;
            case LPX_UP:
               fprintf(fp, " -inf <= %s <= %.*g\n", cname, DBL_DIG, ub);
               break;
            case LPX_DB:
               fprintf(fp, " %.*g <= %s <= %.*g\n", DBL_DIG, lb, cname,
                  DBL_DIG, ub);
               break;
            case LPX_FX:
               fprintf(fp, " %s = %.*g\n", cname, DBL_DIG, lb);
               break;
            default:
               xassert(typx != typx);
         }
      }
      /* write the general section */
      if (lpx_get_class(lp) == LPX_MIP)
      {  flag = 0;
         for (j = 1; j <= ncols; j++)
         {  kind = lpx_get_col_kind(lp, j);
            if (kind == LPX_CV) continue;
            xassert(kind == LPX_IV);
            if (!flag)
            {  fprintf(fp, "\n");
               fprintf(fp, "Generals\n");
               flag = 1;
            }
            fprintf(fp, " %s\n", col_name(lp, j, cname));
         }
      }
      /* write the end keyword */
      fprintf(fp, "\n");
      fprintf(fp, "End\n");
      /* close the output text file */
      fflush(fp);
      if (ferror(fp))
      {  xprintf("lpx_write_cpxlp: write error on `%s' - %s\n",
            fname, strerror(errno));
         goto fail;
      }
      fclose(fp);
      /* return to the calling program */
      return 0;
fail: /* the operation failed */
      if (fp != NULL) fclose(fp);
      return 1;
}

/* eof */
