/* glplpx18.c */

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
#include "glplib.h"

/*----------------------------------------------------------------------
-- lpx_read_prob - read problem data in GNU LP format.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- LPX *lpx_read_prob(char *fname);
--
-- *Description*
--
-- The routine lpx_read_prob reads LP/MIP problem data in GNU LP format
-- from an input text file whose name is the character string fname.
--
-- *Returns*
--
-- If no error occurred, the routine returns a pointer to the created
-- problem object. Otherwise the routine returns NULL. */

struct dsa
{     char *fname;
      /* name of input text file */
      FILE *fp;
      /* stream assigned to input text file */
      int count;
      /* line count */
      int c;
      /* current character or EOF */
};

static int read_char(struct dsa *dsa)
{     int c;
      xassert(dsa->c != EOF);
      if (dsa->c == '\n') dsa->count++;
      c = fgetc(dsa->fp);
      if (ferror(dsa->fp))
      {  xprintf("%s:%d: read error - %s\n",
            dsa->fname, dsa->count, strerror(errno));
         return 1;
      }
      if (feof(dsa->fp))
         c = (dsa->c == '\n' ? EOF : '\n');
      else if (c == '\n')
         ;
      else if (isspace(c))
         c = ' ';
      else if (iscntrl(c))
      {  xprintf("%s:%d: invalid control character 0x%02X\n",
            dsa->fname, dsa->count, c);
         return 1;
      }
      dsa->c = c;
      return 0;
}

static int skip_comment(struct dsa *dsa)
{     while (dsa->c != '\n')
         if (read_char(dsa)) return 1;
      if (read_char(dsa)) return 1;
      return 0;
}

static int read_item(struct dsa *dsa, char item[255+1])
{     int len = 0;
      if (dsa->c == EOF)
      {  xprintf("%s:%d: unexpected end of file\n",
            dsa->fname, dsa->count);
         return 1;
      }
      while (dsa->c == ' ')
         if (read_char(dsa)) return 1;
      if (dsa->c == '\n')
      {  xprintf("%s:%d: unexpected end of line\n",
            dsa->fname, dsa->count);
         return 1;
      }
      while (!(dsa->c == ' ' || dsa->c == '\n'))
      {  if (len == 255)
         {  xprintf("%s:%d: data item `%.255s...' too long\n",
               dsa->fname, dsa->count, item);
            return 1;
         }
         item[len++] = (char)dsa->c;
         if (read_char(dsa)) return 1;
      }
      item[len] = '\0';
      return 0;
}

static int read_int(struct dsa *dsa, int *val)
{     char item[255+1];
      if (read_item(dsa, item)) return 1;
      switch (str2int(item, val))
      {  case 0:
            break;
         case 1:
            xprintf("%s:%d: integer value `%s' out of range\n",
               dsa->fname, dsa->count, item);
            return 1;
         case 2:
            xprintf("%s:%d: invalid integer value `%s'\n",
               dsa->fname, dsa->count, item);
            return 1;
         default:
            xassert(str2int != str2int);
      }
      return 0;
}

static int read_num(struct dsa *dsa, double *val)
{     char item[255+1];
      if (read_item(dsa, item)) return 1;
      switch (str2num(item, val))
      {  case 0:
            break;
         case 1:
            xprintf("%s:%d: floating-point value `%s' out of range\n",
               dsa->fname, dsa->count, item);
            return 1;
         case 2:
            xprintf("%s:%d: invalid floating-point value `%s'\n",
               dsa->fname, dsa->count, item);
            return 1;
         default:
            xassert(str2num != str2num);
      }
      return 0;
}

static int skip_until_nl(struct dsa *dsa)
{     while (dsa->c == ' ')
         if (read_char(dsa)) return 1;
      if (dsa->c != '\n')
      {  xprintf("%s:%d: extra data item(s) detected\n", dsa->fname,
            dsa->count);
         return 1;
      }
      if (read_char(dsa)) return 1;
      return 0;
}

LPX *lpx_read_prob(char *fname)
{     struct dsa _dsa, *dsa = &_dsa;
      LPX *lp = NULL;
      int m, n, nnz, loc, i, j, *ia = NULL, *ja = NULL;
      double lb, ub, coef, *ar = NULL;
      char item[255+1];
      dsa->fname = fname;
      dsa->fp = NULL;
      dsa->count = 0;
      dsa->c = '\n';
      xprintf("lpx_read_prob: reading problem data from `%s'...\n",
         dsa->fname);
      dsa->fp = fopen(dsa->fname, "r");
      if (dsa->fp == NULL)
      {  xprintf("lpx_read_prob: unable to open `%s' - %s\n",
            dsa->fname, strerror(errno));
         goto fail;
      }
      lp = lpx_create_prob();
      /* read very first character */
      if (read_char(dsa)) goto fail;
      /* skip optional comments */
      while (dsa->c == '*')
         if (skip_comment(dsa)) goto fail;
      /* scan problem line */
      if (read_item(dsa, item)) goto fail;
      if (strcmp(item, "P") != 0)
      {  xprintf("%s:%d: problem line missing\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      if (read_item(dsa, item)) goto fail;
      if (strcmp(item, "LP") == 0)
         /* lpx_set_class(lp, LPX_LP) */ ;
      else if (strcmp(item, "MIP") == 0)
         /* lpx_set_class(lp, LPX_MIP) */ ;
      else
      {  xprintf("%s:%d: unknown problem class\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      if (read_item(dsa, item)) goto fail;
      if (strcmp(item, "MIN") == 0)
         lpx_set_obj_dir(lp, LPX_MIN);
      else if (strcmp(item, "MAX") == 0)
         lpx_set_obj_dir(lp, LPX_MAX);
      else
      {  xprintf("%s:%d: invalid objective sense\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      if (read_int(dsa, &m)) goto fail;
      if (m < 0)
      {  xprintf("%s:%d: invalid number of rows\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      if (m > 0) lpx_add_rows(lp, m);
      if (read_int(dsa, &n)) goto fail;
      if (n < 0)
      {  xprintf("%s:%d: invalid number of columns\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      if (n > 0) lpx_add_cols(lp, n);
      if (read_int(dsa, &nnz)) goto fail;
      if (nnz < 0)
      {  xprintf("%s:%d: invalid number of non-zeros\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      xprintf("lpx_read_prob: %d row%s, %d column%s, %d non-zero%s\n",
         m, m == 1 ? "" : "s", n, n == 1 ? "" : "s", nnz, nnz == 1 ?
         "" : "s");
      ia = xcalloc(1+nnz, sizeof(int));
      ja = xcalloc(1+nnz, sizeof(int));
      ar = xcalloc(1+nnz, sizeof(double));
      loc = 0;
      if (skip_until_nl(dsa)) goto fail;
loop: /* skip optional comments */
      while (dsa->c == '*')
         if (skip_comment(dsa)) goto fail;
      /* determine line type */
      if (read_item(dsa, item)) goto fail;
      /* scan line data */
      if (strcmp(item, "N") == 0)
      {  /* scan problem name */
         if (read_item(dsa, item)) goto fail;
         lpx_set_prob_name(lp, item);
         if (skip_until_nl(dsa)) goto fail;
      }
      else if (strcmp(item, "R") == 0)
      {  /* scan row attributes */
         if (read_int(dsa, &i)) goto fail;
         if (!(1 <= i && i <= m))
         {  xprintf("%s:%d: row number out of range\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (read_item(dsa, item)) goto fail;
         if (strcmp(item, "F") == 0)
            lpx_set_row_bnds(lp, i, LPX_FR, 0.0, 0.0);
         else if (strcmp(item, "L") == 0)
         {  if (read_num(dsa, &lb)) goto fail;
            lpx_set_row_bnds(lp, i, LPX_LO, lb, 0.0);
         }
         else if (strcmp(item, "U") == 0)
         {  if (read_num(dsa, &ub)) goto fail;
            lpx_set_row_bnds(lp, i, LPX_UP, 0.0, ub);
         }
         else if (strcmp(item, "D") == 0)
         {  if (read_num(dsa, &lb)) goto fail;
            if (read_num(dsa, &ub)) goto fail;
            lpx_set_row_bnds(lp, i, LPX_DB, lb, ub);
         }
         else if (strcmp(item, "S") == 0)
         {  if (read_num(dsa, &lb)) goto fail;
            lpx_set_row_bnds(lp, i, LPX_FX, lb, 0.0);
         }
         else
         {  xprintf("%s:%d: unknown row type\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (skip_until_nl(dsa)) goto fail;
      }
      else if (strcmp(item, "C") == 0)
      {  /* scan column attributes */
         if (read_int(dsa, &j)) goto fail;
         if (!(1 <= j && j <= n))
         {  xprintf("%s:%d: column number out of range\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (lpx_get_class(lp) == LPX_MIP)
         {  if (read_item(dsa, item)) goto fail;
            if (strcmp(item, "C") == 0)
               lpx_set_col_kind(lp, j, LPX_CV);
            else if (strcmp(item, "I") == 0)
               lpx_set_col_kind(lp, j, LPX_IV);
            else
            {  xprintf("%s:%d: unknown column kind\n",
                  dsa->fname, dsa->count);
               goto fail;
            }
         }
         if (read_item(dsa, item)) goto fail;
         if (strcmp(item, "F") == 0)
            lpx_set_col_bnds(lp, j, LPX_FR, 0.0, 0.0);
         else if (strcmp(item, "L") == 0)
         {  if (read_num(dsa, &lb)) goto fail;
            lpx_set_col_bnds(lp, j, LPX_LO, lb, 0.0);
         }
         else if (strcmp(item, "U") == 0)
         {  if (read_num(dsa, &ub)) goto fail;
            lpx_set_col_bnds(lp, j, LPX_UP, 0.0, ub);
         }
         else if (strcmp(item, "D") == 0)
         {  if (read_num(dsa, &lb)) goto fail;
            if (read_num(dsa, &ub)) goto fail;
            lpx_set_col_bnds(lp, j, LPX_DB, lb, ub);
         }
         else if (strcmp(item, "S") == 0)
         {  if (read_num(dsa, &lb)) goto fail;
            lpx_set_col_bnds(lp, j, LPX_FX, lb, 0.0);
         }
         else
         {  xprintf("%s:%d: unknown column type\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (skip_until_nl(dsa)) goto fail;
      }
      else if (strcmp(item, "A") == 0)
      {  /* constraint or objective coefficient or constant term */
         if (read_int(dsa, &i)) goto fail;
         if (!(0 <= i && i <= m))
         {  xprintf("%s:%d: row number out of range\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (read_int(dsa, &j)) goto fail;
         if (!((i == 0 ? 0 : 1) <= j && j <= n))
         {  xprintf("%s:%d: column number out of range\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (read_num(dsa, &coef)) goto fail;
         if (i == 0)
            lpx_set_obj_coef(lp, j, coef);
         else
         {  loc++;
            if (loc > nnz)
            {  xprintf("%s:%d: too many constraint coefficients\n",
                  dsa->fname, dsa->count);
               goto fail;
            }
            ia[loc] = i, ja[loc] = j, ar[loc] = coef;
         }
         if (skip_until_nl(dsa)) goto fail;
      }
      else if (strcmp(item, "I") == 0)
      {  /* row or objective function name */
         if (read_int(dsa, &i)) goto fail;
         if (!(0 <= i && i <= m))
         {  xprintf("%s:%d: row number out of range\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (read_item(dsa, item)) goto fail;
         if (i == 0)
            lpx_set_obj_name(lp, item);
         else
            lpx_set_row_name(lp, i, item);
         if (skip_until_nl(dsa)) goto fail;
      }
      else if (strcmp(item, "J") == 0)
      {  /* column name */
         if (read_int(dsa, &j)) goto fail;
         if (!(1 <= j && j <= n))
         {  xprintf("%s:%d: column number out of range\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (read_item(dsa, item)) goto fail;
         lpx_set_col_name(lp, j, item);
         if (skip_until_nl(dsa)) goto fail;
      }
      else if (strcmp(item, "E") == 0)
      {  if (read_item(dsa, item)) goto fail;
         if (strcmp(item, "N") != 0)
boo:     {  xprintf("%s:%d: end line invalid or incomplete\n",
               dsa->fname, dsa->count);
            goto fail;
         }
         if (read_item(dsa, item)) goto fail;
         if (strcmp(item, "D") != 0) goto boo;
         if (skip_until_nl(dsa)) goto fail;
         goto fini;
      }
      else
      {  xprintf("%s:%d: unknown line type\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      goto loop;
fini: /* load constraint matrix */
      if (loc < nnz)
      {  xprintf("%s:%d: too few constraint coefficients\n",
            dsa->fname, dsa->count);
         goto fail;
      }
      xassert(loc == nnz);
      lpx_load_matrix(lp, nnz, ia, ja, ar);
      xfree(ia);
      xfree(ja);
      xfree(ar);
      xprintf("lpx_read_prob: %d lines were read\n", dsa->count-1);
      fclose(dsa->fp);
      return lp;
fail: if (dsa->fp != NULL) fclose(dsa->fp);
      if (lp != NULL) lpx_delete_prob(lp);
      if (ia != NULL) xfree(ia);
      if (ja != NULL) xfree(ja);
      if (ar != NULL) xfree(ar);
      return NULL;
}

/*----------------------------------------------------------------------
-- lpx_write_prob - write problem data in GNU LP format.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- int lpx_write_prob(LPX *lp, char *fname);
--
-- *Description*
--
-- The routine lpx_write_prob writes data from a problem object, which
-- the parameter lp points to, to an output text file, whose name is the
-- character string fname, in GNU LP format.
--
-- *Returns*
--
-- If the operation is successful, the routine returns zero. Otherwise
-- the routine prints an error message and returns non-zero. */

static void write_name(FILE *fp, char *name)
{     int k;
      xassert(name != NULL);
      for (k = 0; name[k] != '\0'; k++)
      {  if (isgraph((unsigned char)name[k]))
            fprintf(fp, "%c", name[k]);
         else
            fprintf(fp, "_");
      }
      return;
}

int lpx_write_prob(LPX *lp, char *fname)
{     FILE *fp;
      int m, n, klass, dir, i, j, t, type, len, *ind;
      double lb, ub, coef, *val;
      const char *name;
      xprintf("lpx_write_prob: writing problem data to `%s'...\n",
         fname);
      fp = fopen(fname, "w");
      if (fp == NULL)
      {  xprintf("lpx_write_prob: unable to create `%s' - %s\n",
            fname, strerror(errno));
         goto fail;
      }
      /* problem line */
      m = lpx_get_num_rows(lp);
      n = lpx_get_num_cols(lp);
      klass = lpx_get_class(lp);
      dir = lpx_get_obj_dir(lp);
      fprintf(fp, "P %s %s %d %d %d\n",
         klass == LPX_LP ? "LP" : klass == LPX_MIP ? "MIP" : "???",
         dir == LPX_MIN ? "MIN" : dir == LPX_MAX ? "MAX" : "???",
         m, n, lpx_get_num_nz(lp));
      /* problem name */
      name = (void *)lpx_get_prob_name(lp);
      if (name != NULL)
      {  fprintf(fp, "N ");
         write_name(fp, (void *)name);
         fprintf(fp, "\n");
      }
      /* rows (constraints) */
      for (i = 1; i <= m; i++)
      {  fprintf(fp, "R %d ", i);
         type = lpx_get_row_type(lp, i);
         lb = lpx_get_row_lb(lp, i);
         ub = lpx_get_row_ub(lp, i);
         switch (type)
         {  case LPX_FR:
               fprintf(fp, "F"); break;
            case LPX_LO:
               fprintf(fp, "L %.*g", DBL_DIG, lb); break;
            case LPX_UP:
               fprintf(fp, "U %.*g", DBL_DIG, ub); break;
            case LPX_DB:
               fprintf(fp, "D %.*g %.*g", DBL_DIG, lb, DBL_DIG, ub);
               break;
            case LPX_FX:
               fprintf(fp, "S %.*g", DBL_DIG, lb); break;
            default:
               xassert(type != type);
         }
         fprintf(fp, "\n");
      }
      /* columns (variables) */
      for (j = 1; j <= n; j++)
      {  fprintf(fp, "C %d ", j);
         type = lpx_get_col_type(lp, j);
         lb = lpx_get_col_lb(lp, j);
         ub = lpx_get_col_ub(lp, j);
         if (klass == LPX_MIP)
         {  switch (lpx_get_col_kind(lp, j))
            {  case LPX_CV:
                  fprintf(fp, "C "); break;
               case LPX_IV:
                  fprintf(fp, "I "); break;
               default:
                  xassert(lp != lp);
            }
         }
         switch (type)
         {  case LPX_FR:
               fprintf(fp, "F"); break;
            case LPX_LO:
               fprintf(fp, "L %.*g", DBL_DIG, lb); break;
            case LPX_UP:
               fprintf(fp, "U %.*g", DBL_DIG, ub); break;
            case LPX_DB:
               fprintf(fp, "D %.*g %.*g", DBL_DIG, lb, DBL_DIG, ub);
               break;
            case LPX_FX:
               fprintf(fp, "S %.*g", DBL_DIG, lb); break;
            default:
               xassert(type != type);
         }
         fprintf(fp, "\n");
      }
      /* objective coefficients */
      for (j = 0; j <= n; j++)
      {  coef = lpx_get_obj_coef(lp, j);
         if (coef != 0.0)
            fprintf(fp, "A 0 %d %.*g\n", j, DBL_DIG, coef);
      }
      /* constraint matrix */
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
      for (i = 1; i <= m; i++)
      {  len = lpx_get_mat_row(lp, i, ind, val);
         for (t = 1; t <= len; t++)
            fprintf(fp, "A %d %d %.*g\n", i, ind[t], DBL_DIG, val[t]);
      }
      xfree(ind);
      xfree(val);
      /* objective and row names */
      for (i = 0; i <= m; i++)
      {  if (i == 0)
            name = (void *)lpx_get_obj_name(lp);
         else
            name = (void *)lpx_get_row_name(lp, i);
         if (name != NULL)
         {  fprintf(fp, "I %d ", i);
            write_name(fp, (void *)name);
            fprintf(fp, "\n");
         }
      }
      /* column names */
      for (j = 1; j <= n; j++)
      {  name = lpx_get_col_name(lp, j);
         if (name != NULL)
         {  fprintf(fp, "J %d ", j);
            write_name(fp, (void *)name);
            fprintf(fp, "\n");
         }
      }
      /* end line */
      fprintf(fp, "E N D\n");
      fflush(fp);
      if (ferror(fp))
      {  xprintf("lpx_write_prob: write error on `%s' - %s\n",
            fname, strerror(errno));
         goto fail;
      }
      fclose(fp);
      return 0;
fail: if (fp != NULL) fclose(fp);
      return 1;
}

/* eof */
