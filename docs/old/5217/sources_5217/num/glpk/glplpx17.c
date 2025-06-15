/* glplpx17.c */

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

/*----------------------------------------------------------------------
-- lpx_extract_prob - extract problem instance from MathProg model.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- LPX *lpx_extract_prob(void *mpl);
--
-- *Description*
--
-- The routine lpx_extract_prob extracts the problem instance from the
-- MathProg translator database.
--
-- *Returns*
--
-- The routine returns a pointer to the extracted problem object. */

LPX *lpx_extract_prob(void *_mpl)
{     MPL *mpl = _mpl;
      LPX *lp;
      int m, n, i, j, t, kind, type, len, *ind;
      double lb, ub, *val;
      /* create problem instance */
      lp = lpx_create_prob();
      /* set problem name */
      lpx_set_prob_name(lp, mpl_get_prob_name(mpl));
      /* build rows (constraints) */
      m = mpl_get_num_rows(mpl);
      if (m > 0) lpx_add_rows(lp, m);
      for (i = 1; i <= m; i++)
      {  /* set row name */
         lpx_set_row_name(lp, i, mpl_get_row_name(mpl, i));
         /* set row bounds */
         type = mpl_get_row_bnds(mpl, i, &lb, &ub);
         switch (type)
         {  case MPL_FR: type = LPX_FR; break;
            case MPL_LO: type = LPX_LO; break;
            case MPL_UP: type = LPX_UP; break;
            case MPL_DB: type = LPX_DB; break;
            case MPL_FX: type = LPX_FX; break;
            default: xassert(type != type);
         }
         if (type == LPX_DB && fabs(lb - ub) < 1e-9 * (1.0 + fabs(lb)))
         {  type = LPX_FX;
            if (fabs(lb) <= fabs(ub)) ub = lb; else lb = ub;
         }
         lpx_set_row_bnds(lp, i, type, lb, ub);
         /* warn about non-zero constant term */
         if (mpl_get_row_c0(mpl, i) != 0.0)
            xprintf(
               "lpx_read_model: row %s; constant term %.12g ignored\n",
               mpl_get_row_name(mpl, i), mpl_get_row_c0(mpl, i));
      }
      /* build columns (variables) */
      n = mpl_get_num_cols(mpl);
      if (n > 0) lpx_add_cols(lp, n);
      for (j = 1; j <= n; j++)
      {  /* set column name */
         lpx_set_col_name(lp, j, mpl_get_col_name(mpl, j));
         /* set column kind */
         kind = mpl_get_col_kind(mpl, j);
         switch (kind)
         {  case MPL_NUM:
               break;
            case MPL_INT:
            case MPL_BIN:
#if 0
               lpx_set_class(lp, LPX_MIP);
#endif
               lpx_set_col_kind(lp, j, LPX_IV);
               break;
            default:
               xassert(kind != kind);
         }
         /* set column bounds */
         type = mpl_get_col_bnds(mpl, j, &lb, &ub);
         switch (type)
         {  case MPL_FR: type = LPX_FR; break;
            case MPL_LO: type = LPX_LO; break;
            case MPL_UP: type = LPX_UP; break;
            case MPL_DB: type = LPX_DB; break;
            case MPL_FX: type = LPX_FX; break;
            default: xassert(type != type);
         }
         if (kind == MPL_BIN)
         {  if (type == LPX_FR || type == LPX_UP || lb < 0.0) lb = 0.0;
            if (type == LPX_FR || type == LPX_LO || ub > 1.0) ub = 1.0;
            type = LPX_DB;
         }
         if (type == LPX_DB && fabs(lb - ub) < 1e-9 * (1.0 + fabs(lb)))
         {  type = LPX_FX;
            if (fabs(lb) <= fabs(ub)) ub = lb; else lb = ub;
         }
         lpx_set_col_bnds(lp, j, type, lb, ub);
      }
      /* load the constraint matrix */
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
      for (i = 1; i <= m; i++)
      {  len = mpl_get_mat_row(mpl, i, ind, val);
         lpx_set_mat_row(lp, i, len, ind, val);
      }
      /* build objective function (the first objective is used) */
      for (i = 1; i <= m; i++)
      {  kind = mpl_get_row_kind(mpl, i);
         if (kind == MPL_MIN || kind == MPL_MAX)
         {  /* set objective name */
            lpx_set_obj_name(lp, mpl_get_row_name(mpl, i));
            /* set optimization direction */
            lpx_set_obj_dir(lp, kind == MPL_MIN ? LPX_MIN : LPX_MAX);
            /* set constant term */
            lpx_set_obj_coef(lp, 0, mpl_get_row_c0(mpl, i));
            /* set objective coefficients */
            len = mpl_get_mat_row(mpl, i, ind, val);
            for (t = 1; t <= len; t++)
               lpx_set_obj_coef(lp, ind[t], val[t]);
            break;
         }
      }
      /* free working arrays */
      xfree(ind);
      xfree(val);
      /* bring the problem object to the calling program */
      return lp;
}

/*----------------------------------------------------------------------
-- lpx_read_model - read LP/MIP model written in GNU MathProg language.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- LPX *lpx_read_model(char *model, char *data, char *output);
--
-- *Description*
--
-- The routine lpx_read_model reads and translates LP/MIP model written
-- in the GNU MathProg modeling language.
--
-- The character string model specifies name of input text file, which
-- contains model section and, optionally, data section. This parameter
-- cannot be NULL.
--
-- The character string data specifies name of input text file, which
-- contains data section. This parameter can be NULL. (If the data file
-- is specified and the model file also has data section, that section
-- is ignored.)
--
-- The character string output specifies name of output text file, to
-- which the output produced by display statement(s) should be written.
-- This parameter can be NULL, in which case the display output is sent
-- to stdout via the routine print.
--
-- *Returns*
--
-- If no errors occurred, the routine returns a pointer to the problem
-- object created. Otherwise the routine returns NULL. */

LPX *lpx_read_model(const char *model, const char *data,
      const char *output)
{     LPX *lp = NULL;
      MPL *mpl;
      int ret;
      /* create and initialize the translator database */
      mpl = mpl_initialize();
      /* read model section and optional data section */
      ret = mpl_read_model(mpl, (char *)model, data != NULL);
      if (ret == 4) goto done;
      xassert(ret == 1 || ret == 2);
      /* read data section, if necessary */
      if (data != NULL)
      {  xassert(ret == 1);
         ret = mpl_read_data(mpl, (char *)data);
         if (ret == 4) goto done;
         xassert(ret == 2);
      }
      /* generate model */
      ret = mpl_generate(mpl, (char *)output);
      if (ret == 4) goto done;
      xassert(ret == 3);
      /* extract problem instance */
      lp = lpx_extract_prob(mpl);
done: /* free resources used by the model translator */
      mpl_terminate(mpl);
      return lp;
}

/* eof */
