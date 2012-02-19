/* rng/schrage.c
 * Copyright (C) 2003 Carlo Perassi and Heiko Bauke.
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

static inline unsigned long int
schrage (unsigned long int a, unsigned long int b, unsigned long int m)
{
  /* This is a modified version of Schrage's method. It ensures that no
   * overflow or underflow occurs even if a=ceil(sqrt(m)). Usual 
   * Schrage's method works only until a=floor(sqrt(m)).
   */
  unsigned long int q, t;
  if (a == 0UL)
    return 0UL;
  q = m / a;
  t = 2 * m - (m % a) * (b / q);
  if (t >= m)
    t -= m;
  t += a * (b % q);
  return (t >= m) ? (t - m) : t;
}

static inline unsigned long int
schrage_mult (unsigned long int a, unsigned long int b,
              unsigned long int m,
              unsigned long int sqrtm)
{
  /* To multiply a and b use Schrage's method 3 times.
   * represent a in base ceil(sqrt(m))  a = a1*ceil(sqrt(m)) + a0  
   * a*b = (a1*ceil(sqrt(m)) + a0)*b = a1*ceil(sqrt(m))*b + a0*b   
   */
  unsigned long int t0 = schrage (sqrtm, b, m);
  unsigned long int t1 = schrage (a / sqrtm, t0, m);
  unsigned long int t2 = schrage (a % sqrtm, b, m);
  unsigned long int t = t1 + t2;
  return (t >= m) ? (t - m) : t;
}
