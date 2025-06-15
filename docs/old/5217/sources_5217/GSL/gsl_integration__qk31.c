/* integration/qk31.c
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
#include "gsl_integration.h"

/* Gauss quadrature weights and kronrod quadrature abscissae and
   weights as evaluated with 80 decimal digit arithmetic by
   L. W. Fullerton, Bell Labs, Nov. 1981. */

static const double xgk[16] =   /* abscissae of the 31-point kronrod rule */
{
  0.998002298693397060285172840152271,
  0.987992518020485428489565718586613,
  0.967739075679139134257347978784337,
  0.937273392400705904307758947710209,
  0.897264532344081900882509656454496,
  0.848206583410427216200648320774217,
  0.790418501442465932967649294817947,
  0.724417731360170047416186054613938,
  0.650996741297416970533735895313275,
  0.570972172608538847537226737253911,
  0.485081863640239680693655740232351,
  0.394151347077563369897207370981045,
  0.299180007153168812166780024266389,
  0.201194093997434522300628303394596,
  0.101142066918717499027074231447392,
  0.000000000000000000000000000000000
};

/* xgk[1], xgk[3], ... abscissae of the 15-point gauss rule. 
   xgk[0], xgk[2], ... abscissae to optimally extend the 15-point gauss rule */

static const double wg[8] =     /* weights of the 15-point gauss rule */
{
  0.030753241996117268354628393577204,
  0.070366047488108124709267416450667,
  0.107159220467171935011869546685869,
  0.139570677926154314447804794511028,
  0.166269205816993933553200860481209,
  0.186161000015562211026800561866423,
  0.198431485327111576456118326443839,
  0.202578241925561272880620199967519
};

static const double wgk[16] =   /* weights of the 31-point kronrod rule */
{
  0.005377479872923348987792051430128,
  0.015007947329316122538374763075807,
  0.025460847326715320186874001019653,
  0.035346360791375846222037948478360,
  0.044589751324764876608227299373280,
  0.053481524690928087265343147239430,
  0.062009567800670640285139230960803,
  0.069854121318728258709520077099147,
  0.076849680757720378894432777482659,
  0.083080502823133021038289247286104,
  0.088564443056211770647275443693774,
  0.093126598170825321225486872747346,
  0.096642726983623678505179907627589,
  0.099173598721791959332393173484603,
  0.100769845523875595044946662617570,
  0.101330007014791549017374792767493
};

void
gsl_integration_qk31 (const gsl_function * f, double a, double b,
      double *result, double *abserr,
      double *resabs, double *resasc)
{
  double fv1[16], fv2[16];
  gsl_integration_qk (16, xgk, wg, wgk, fv1, fv2, f, a, b, result, abserr, resabs, resasc);
}
