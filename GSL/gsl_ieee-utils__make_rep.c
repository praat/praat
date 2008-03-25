/* ieee-utils/make_rep.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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

#include "gsl__config.h"
#include "gsl_ieee_utils.h"

#include "gsl_ieee-utils__endian.c"
#include "gsl_ieee-utils__standardize.c"

static void sprint_nybble(int i, char *s) ;
static void sprint_byte(int i, char *s) ;
static int determine_ieee_type (int non_zero, int exponent, int max_exponent);


/* For the IEEE float format the bits are found from the following
   masks,
   
   sign      = 0x80000000  
   exponent  = 0x7f800000 
   mantisssa = 0x007fffff  

   For the IEEE double format the masks are,

   sign      = 0x8000000000000000  
   exponent  = 0x7ff0000000000000 
   mantissa  = 0x000fffffffffffff

   */

void 
gsl_ieee_float_to_rep (const float * x, gsl_ieee_float_rep * r)
{
  int e, non_zero;

  union { 
    float f;
    struct  { 
      unsigned char byte[4] ;
    } ieee ;
  } u;
  
  u.f = *x ; 

  if (little_endian_p())
    make_float_bigendian(&(u.f)) ;
  
  /* note that r->sign is signed, u.ieee.byte is unsigned */

  if (u.ieee.byte[3]>>7)
    {
      r->sign = 1 ;
    }
  else
    {
      r->sign = 0 ;
    }

  e = (u.ieee.byte[3] & 0x7f) << 1 | (u.ieee.byte[2] & 0x80)>>7 ; 
  
  r->exponent = e - 127 ;

  sprint_byte((u.ieee.byte[2] & 0x7f) << 1,r->mantissa) ;
  sprint_byte(u.ieee.byte[1],r->mantissa + 7) ;
  sprint_byte(u.ieee.byte[0],r->mantissa + 15) ;

  r->mantissa[23] = '\0' ;

  non_zero = u.ieee.byte[0] || u.ieee.byte[1] || (u.ieee.byte[2] & 0x7f);

  r->type = determine_ieee_type (non_zero, e, 255) ;
}

void 
gsl_ieee_double_to_rep (const double * x, gsl_ieee_double_rep * r)
{

  int e, non_zero;

  union 
  { 
    double d;
    struct  { 
      unsigned char byte[8];
    } ieee ;
  } u;

  u.d= *x ; 
  
  if (little_endian_p())
    make_double_bigendian(&(u.d)) ;
  
  /* note that r->sign is signed, u.ieee.byte is unsigned */

  if (u.ieee.byte[7]>>7)
    {
      r->sign = 1 ;
    }
  else
    {
      r->sign = 0 ;
    }


  e =(u.ieee.byte[7] & 0x7f)<<4 ^ (u.ieee.byte[6] & 0xf0)>>4 ;
  
  r->exponent = e - 1023 ;

  sprint_nybble(u.ieee.byte[6],r->mantissa) ;
  sprint_byte(u.ieee.byte[5],r->mantissa + 4) ;
  sprint_byte(u.ieee.byte[4],r->mantissa + 12) ;
  sprint_byte(u.ieee.byte[3],r->mantissa + 20) ; 
  sprint_byte(u.ieee.byte[2],r->mantissa + 28) ;
  sprint_byte(u.ieee.byte[1],r->mantissa + 36) ;
  sprint_byte(u.ieee.byte[0],r->mantissa + 44) ;

  r->mantissa[52] = '\0' ;

  non_zero = (u.ieee.byte[0] || u.ieee.byte[1] || u.ieee.byte[2]
              || u.ieee.byte[3] || u.ieee.byte[4] || u.ieee.byte[5] 
              || (u.ieee.byte[6] & 0x0f)) ;

  r->type = determine_ieee_type (non_zero, e, 2047) ;
}

/* A table of character representations of nybbles */

static char nybble[16][5]={ /* include space for the \0 */
  "0000", "0001", "0010", "0011",
  "0100", "0101", "0110", "0111",
  "1000", "1001", "1010", "1011",
  "1100", "1101", "1110", "1111"
}  ;
          
static void
sprint_nybble(int i, char *s)
{
  char *c ;
  c=nybble[i & 0x0f ];
  *s=c[0] ;  *(s+1)=c[1] ;  *(s+2)=c[2] ;  *(s+3)=c[3] ;
} 

static void
sprint_byte(int i, char *s)
{
  char *c ;
  c=nybble[(i & 0xf0)>>4];
  *s=c[0] ;  *(s+1)=c[1] ;  *(s+2)=c[2] ;  *(s+3)=c[3] ;
  c=nybble[i & 0x0f];
  *(s+4)=c[0] ;  *(s+5)=c[1] ;  *(s+6)=c[2] ;  *(s+7)=c[3] ;
} 

static int 
determine_ieee_type (int non_zero, int exponent, int max_exponent)
{
  if (exponent == max_exponent)
    {
      if (non_zero)
        {
          return GSL_IEEE_TYPE_NAN ;
        }
      else
        {
          return GSL_IEEE_TYPE_INF ;
        }
    }
  else if (exponent == 0)
    {
      if (non_zero)
        {
          return GSL_IEEE_TYPE_DENORMAL ;
        }
      else
        {
          return GSL_IEEE_TYPE_ZERO ;
        }
    }
  else
    {
      return GSL_IEEE_TYPE_NORMAL ;
    }
}
