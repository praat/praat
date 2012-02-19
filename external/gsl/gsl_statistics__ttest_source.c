/* statistics/ttest_source.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Jim Davies, Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

double 
FUNCTION(gsl_stats,ttest) (const BASE data1[], 
                           const size_t stride1, const size_t n1, 
                           const BASE data2[],
                           const size_t stride2, const size_t n2)
{
  /* runs a t-test between two datasets representing independent
     samples. Tests to see if the difference between means of the
     samples is different from zero */

  /* find means for the two samples */
  const double mean1 = FUNCTION(gsl_stats,mean) (data1, stride1, n1);
  const double mean2 = FUNCTION(gsl_stats,mean) (data2, stride2, n2);

  /* find pooled variance for the two samples */
  const double pv = FUNCTION(gsl_stats,pvariance) (data1, stride1, n1, data2, stride2, n2);

  /* calculate the t statistic */
  const double t = (mean1 - mean2) / (sqrt (pv * ((1.0 / n1) + (1.0 / n2))));

  return t;
}

