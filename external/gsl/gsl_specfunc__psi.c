/* specfunc/psi.c
 * 
 * Copyright (C) 2007 Brian Gough
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2005, 2006 Gerard Jungman
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

/* Author: G. Jungman */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_zeta.h"
#include "gsl_sf_psi.h"
#include "gsl_complex_math.h"

#include <stdio.h>

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/


/* Chebyshev fit for f(y) = Re(Psi(1+Iy)) + M_EULER - y^2/(1+y^2) - y^2/(2(4+y^2))
 * 1 < y < 10
 *   ==>
 * y(x) = (9x + 11)/2,  -1 < x < 1
 * x(y) = (2y - 11)/9
 *
 * g(x) := f(y(x))
 */
static double r1py_data[] = {
   1.59888328244976954803168395603,
   0.67905625353213463845115658455,
  -0.068485802980122530009506482524,
  -0.005788184183095866792008831182,
   0.008511258167108615980419855648,
  -0.004042656134699693434334556409,
   0.001352328406159402601778462956,
  -0.000311646563930660566674525382,
   0.000018507563785249135437219139,
   0.000028348705427529850296492146,
  -0.000019487536014574535567541960,
   8.0709788710834469408621587335e-06,
  -2.2983564321340518037060346561e-06,
   3.0506629599604749843855962658e-07,
   1.3042238632418364610774284846e-07,
  -1.2308657181048950589464690208e-07,
   5.7710855710682427240667414345e-08,
  -1.8275559342450963966092636354e-08,
   3.1020471300626589420759518930e-09,
   6.8989327480593812470039430640e-10,
  -8.7182290258923059852334818997e-10,
   4.4069147710243611798213548777e-10,
  -1.4727311099198535963467200277e-10,
   2.7589682523262644748825844248e-11,
   4.1871826756975856411554363568e-12,
  -6.5673460487260087541400767340e-12,
   3.4487900886723214020103638000e-12,
  -1.1807251417448690607973794078e-12,
   2.3798314343969589258709315574e-13,
   2.1663630410818831824259465821e-15
};
static cheb_series r1py_cs = {
  r1py_data,
  29,
  -1,1,
  18
};


/* Chebyshev fits from SLATEC code for psi(x)

 Series for PSI        on the interval  0.         to  1.00000D+00
                                       with weighted error   2.03E-17
                                        log weighted error  16.69
                              significant figures required  16.39
                                   decimal places required  17.37

 Series for APSI       on the interval  0.         to  2.50000D-01
                                       with weighted error   5.54E-17
                                        log weighted error  16.26
                              significant figures required  14.42
                                   decimal places required  16.86

*/

static double psics_data[23] = {
  -.038057080835217922,
   .491415393029387130, 
  -.056815747821244730,
   .008357821225914313,
  -.001333232857994342,
   .000220313287069308,
  -.000037040238178456,
   .000006283793654854,
  -.000001071263908506,
   .000000183128394654,
  -.000000031353509361,
   .000000005372808776,
  -.000000000921168141,
   .000000000157981265,
  -.000000000027098646,
   .000000000004648722,
  -.000000000000797527,
   .000000000000136827,
  -.000000000000023475,
   .000000000000004027,
  -.000000000000000691,
   .000000000000000118,
  -.000000000000000020
};
static cheb_series psi_cs = {
  psics_data,
  22,
  -1, 1,
  17
};

static double apsics_data[16] = {    
  -.0204749044678185,
  -.0101801271534859,
   .0000559718725387,
  -.0000012917176570,
   .0000000572858606,
  -.0000000038213539,
   .0000000003397434,
  -.0000000000374838,
   .0000000000048990,
  -.0000000000007344,
   .0000000000001233,
  -.0000000000000228,
   .0000000000000045,
  -.0000000000000009,
   .0000000000000002,
  -.0000000000000000 
};    
static cheb_series apsi_cs = {
  apsics_data,
  15,
  -1, 1,
  9
};

#define PSI_TABLE_NMAX 100
static double psi_table[PSI_TABLE_NMAX+1] = {
  0.0,  /* Infinity */              /* psi(0) */
 -M_EULER,                          /* psi(1) */
  0.42278433509846713939348790992,  /* ...    */
  0.92278433509846713939348790992,
  1.25611766843180047272682124325,
  1.50611766843180047272682124325,
  1.70611766843180047272682124325,
  1.87278433509846713939348790992,
  2.01564147795560999653634505277,
  2.14064147795560999653634505277,
  2.25175258906672110764745616389,
  2.35175258906672110764745616389,
  2.44266167997581201673836525479,
  2.52599501330914535007169858813,
  2.60291809023222227314862166505,
  2.67434666166079370172005023648,
  2.74101332832746036838671690315,
  2.80351332832746036838671690315,
  2.86233685773922507426906984432,
  2.91789241329478062982462539988,
  2.97052399224214905087725697883,
  3.02052399224214905087725697883,
  3.06814303986119666992487602645,
  3.11359758531574212447033057190,
  3.15707584618530734186163491973,
  3.1987425128519740085283015864,
  3.2387425128519740085283015864,
  3.2772040513135124700667631249,
  3.3142410883505495071038001619,
  3.3499553740648352213895144476,
  3.3844381326855248765619282407,
  3.4177714660188582098952615740,
  3.4500295305349872421533260902,
  3.4812795305349872421533260902,
  3.5115825608380175451836291205,
  3.5409943255438998981248055911,
  3.5695657541153284695533770196,
  3.5973435318931062473311547974,
  3.6243705589201332743581818244,
  3.6506863483938174848844976139,
  3.6763273740348431259101386396,
  3.7013273740348431259101386396,
  3.7257176179372821503003825420,
  3.7495271417468059598241920658,
  3.7727829557002943319172153216,
  3.7955102284275670591899425943,
  3.8177324506497892814121648166,
  3.8394715810845718901078169905,
  3.8607481768292527411716467777,
  3.8815815101625860745049801110,
  3.9019896734278921969539597029,
  3.9219896734278921969539597029,
  3.9415975165651470989147440166,
  3.9608282857959163296839747858,
  3.9796962103242182164764276160,
  3.9982147288427367349949461345,
  4.0163965470245549168131279527,
  4.0342536898816977739559850956,
  4.0517975495308205809735289552,
  4.0690389288411654085597358518,
  4.0859880813835382899156680552,
  4.1026547480502049565823347218,
  4.1190481906731557762544658694,
  4.1351772229312202923834981274,
  4.1510502388042361653993711433,
  4.1666752388042361653993711433,
  4.1820598541888515500147557587,
  4.1972113693403667015299072739,
  4.2121367424746950597388624977,
  4.2268426248276362362094507330,
  4.2413353784508246420065521823,
  4.2556210927365389277208378966,
  4.2697055997787924488475984600,
  4.2835944886676813377364873489,
  4.2972931188046676391063503626,
  4.3108066323181811526198638761,
  4.3241399656515144859531972094,
  4.3372978603883565912163551041,
  4.3502848733753695782293421171,
  4.3631053861958823987421626300,
  4.3757636140439836645649474401,
  4.3882636140439836645649474401,
  4.4006092930563293435772931191,
  4.4128044150075488557724150703,
  4.4248526077786331931218126607,
  4.4367573696833950978837174226,
  4.4485220755657480390601880108,
  4.4601499825424922251066996387,
  4.4716442354160554434975042364,
  4.4830078717796918071338678728,
  4.4942438268358715824147667492,
  4.5053549379469826935258778603,
  4.5163439489359936825368668713,
  4.5272135141533849868846929582,
  4.5379662023254279976373811303,
  4.5486045001977684231692960239,
  4.5591308159872421073798223397,
  4.5695474826539087740464890064,
  4.5798567610044242379640147796,
  4.5900608426370772991885045755,
  4.6001618527380874001986055856
};


#define PSI_1_TABLE_NMAX 100
static double psi_1_table[PSI_1_TABLE_NMAX+1] = {
  0.0,  /* Infinity */              /* psi(1,0) */
  M_PI*M_PI/6.0,                    /* psi(1,1) */
  0.644934066848226436472415,       /* ...      */
  0.394934066848226436472415,
  0.2838229557371153253613041,
  0.2213229557371153253613041,
  0.1813229557371153253613041,
  0.1535451779593375475835263,
  0.1331370146940314251345467,
  0.1175120146940314251345467,
  0.1051663356816857461222010,
  0.0951663356816857461222010,
  0.0869018728717683907503002,
  0.0799574284273239463058557,
  0.0740402686640103368384001,
  0.0689382278476838062261552,
  0.0644937834032393617817108,
  0.0605875334032393617817108,
  0.0571273257907826143768665,
  0.0540409060376961946237801,
  0.0512708229352031198315363,
  0.0487708229352031198315363,
  0.0465032492390579951149830,
  0.0444371335365786562720078,
  0.0425467743683366902984728,
  0.0408106632572255791873617,
  0.0392106632572255791873617,
  0.0377313733163971768204978,
  0.0363596312039143235969038,
  0.0350841209998326909438426,
  0.0338950603577399442137594,
  0.0327839492466288331026483,
  0.0317433665203020901265817,
  0.03076680402030209012658168,
  0.02984853037475571730748159,
  0.02898347847164153045627052,
  0.02816715194102928555831133,
  0.02739554700275768062003973,
  0.02666508681283803124093089,
  0.02597256603721476254286995,
  0.02531510384129102815759710,
  0.02469010384129102815759710,
  0.02409521984367056414807896,
  0.02352832641963428296894063,
  0.02298749353699501850166102,
  0.02247096461137518379091722,
  0.02197713745088135663042339,
  0.02150454765882086513703965,
  0.02105185413233829383780923,
  0.02061782635456051606003145,
  0.02020133322669712580597065,
  0.01980133322669712580597065,
  0.01941686571420193164987683,
  0.01904704322899483105816086,
  0.01869104465298913508094477,
  0.01834810912486842177504628,
  0.01801753061247172756017024,
  0.01769865306145131939690494,
  0.01739086605006319997554452,
  0.01709360088954001329302371,
  0.01680632711763538818529605,
  0.01652854933985761040751827,
  0.01625980437882562975715546,
  0.01599965869724394401313881,
  0.01574770606433893015574400,
  0.01550356543933893015574400,
  0.01526687904880638577704578,
  0.01503731063741979257227076,
  0.01481454387422086185273411,
  0.01459828089844231513993134,
  0.01438824099085987447620523,
  0.01418415935820681325171544,
  0.01398578601958352422176106,
  0.01379288478501562298719316,
  0.01360523231738567365335942,
  0.01342261726990576130858221,
  0.01324483949212798353080444,
  0.01307170929822216635628920,
  0.01290304679189732236910755,
  0.01273868124291638877278934,
  0.01257845051066194236996928,
  0.01242220051066194236996928,
  0.01226978472038606978956995,
  0.01212106372098095378719041,
  0.01197590477193174490346273,
  0.01183418141592267460867815,
  0.01169577311142440471248438,
  0.01156056489076458859566448,
  0.01142844704164317229232189,
  0.01129931481023821361463594,
  0.01117306812421372175754719,
  0.01104961133409026496742374,
  0.01092885297157366069257770,
  0.01081070552355853781923177,
  0.01069508522063334415522437,
  0.01058191183901270133041676,
  0.01047110851491297833872701,
  0.01036260157046853389428257,
  0.01025632035036012704977199,  /* ...        */
  0.01015219706839427948625679,  /* psi(1,99)  */
  0.01005016666333357139524567   /* psi(1,100) */
};


/* digamma for x both positive and negative; we do both
 * cases here because of the way we use even/odd parts
 * of the function
 */
static int
psi_x(const double x, gsl_sf_result * result)
{
  const double y = fabs(x);

  if(x == 0.0 || x == -1.0 || x == -2.0) {
    DOMAIN_ERROR(result);
  }
  else if(y >= 2.0) {
    const double t = 8.0/(y*y)-1.0;
    gsl_sf_result result_c;
    cheb_eval_e(&apsi_cs, t, &result_c);
    if(x < 0.0) {
      const double s = sin(M_PI*x);
      const double c = cos(M_PI*x);
      if(fabs(s) < 2.0*GSL_SQRT_DBL_MIN) {
        DOMAIN_ERROR(result);
      }
      else {
        result->val  = log(y) - 0.5/x + result_c.val - M_PI * c/s;
        result->err  = M_PI*fabs(x)*GSL_DBL_EPSILON/(s*s);
        result->err += result_c.err;
        result->err += GSL_DBL_EPSILON * fabs(result->val);
        return GSL_SUCCESS;
      }
    }
    else {
      result->val  = log(y) - 0.5/x + result_c.val;
      result->err  = result_c.err;
      result->err += GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
  }
  else { /* -2 < x < 2 */
    gsl_sf_result result_c;

    if(x < -1.0) { /* x = -2 + v */
      const double v  = x + 2.0;
      const double t1 = 1.0/x;
      const double t2 = 1.0/(x+1.0);
      const double t3 = 1.0/v;
      cheb_eval_e(&psi_cs, 2.0*v-1.0, &result_c);
      
      result->val  = -(t1 + t2 + t3) + result_c.val;
      result->err  = GSL_DBL_EPSILON * (fabs(t1) + fabs(x/(t2*t2)) + fabs(x/(t3*t3)));
      result->err += result_c.err;
      result->err += GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
    else if(x < 0.0) { /* x = -1 + v */
      const double v  = x + 1.0;
      const double t1 = 1.0/x;
      const double t2 = 1.0/v;
      cheb_eval_e(&psi_cs, 2.0*v-1.0, &result_c);
      
      result->val  = -(t1 + t2) + result_c.val;
      result->err  = GSL_DBL_EPSILON * (fabs(t1) + fabs(x/(t2*t2)));
      result->err += result_c.err;
      result->err += GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
    else if(x < 1.0) { /* x = v */
      const double t1 = 1.0/x;
      cheb_eval_e(&psi_cs, 2.0*x-1.0, &result_c);
      
      result->val  = -t1 + result_c.val;
      result->err  = GSL_DBL_EPSILON * t1;
      result->err += result_c.err;
      result->err += GSL_DBL_EPSILON * fabs(result->val);
      return GSL_SUCCESS;
    }
    else { /* x = 1 + v */
      const double v = x - 1.0;
      return cheb_eval_e(&psi_cs, 2.0*v-1.0, result);
    }
  }
}


/* psi(z) for large |z| in the right half-plane; [Abramowitz + Stegun, 6.3.18] */
static
gsl_complex
psi_complex_asymp(gsl_complex z)
{
  /* coefficients in the asymptotic expansion for large z;
   * let w = z^(-2) and write the expression in the form
   *
   *   ln(z) - 1/(2z) - 1/12 w (1 + c1 w + c2 w + c3 w + ... )
   */
  static const double c1 = -0.1;
  static const double c2 =  1.0/21.0;
  static const double c3 = -0.05;

  gsl_complex zi = gsl_complex_inverse(z);
  gsl_complex w  = gsl_complex_mul(zi, zi);
  gsl_complex cs;

  /* Horner method evaluation of term in parentheses */
  gsl_complex sum;
  sum = gsl_complex_mul_real(w, c3/c2);
  sum = gsl_complex_add_real(sum, 1.0);
  sum = gsl_complex_mul_real(sum, c2/c1);
  sum = gsl_complex_mul(sum, w);
  sum = gsl_complex_add_real(sum, 1.0);
  sum = gsl_complex_mul_real(sum, c1);
  sum = gsl_complex_mul(sum, w);
  sum = gsl_complex_add_real(sum, 1.0);

  /* correction added to log(z) */
  cs = gsl_complex_mul(sum, w);
  cs = gsl_complex_mul_real(cs, -1.0/12.0);
  cs = gsl_complex_add(cs, gsl_complex_mul_real(zi, -0.5));

  return gsl_complex_add(gsl_complex_log(z), cs);
}



/* psi(z) for complex z in the right half-plane */
static int
psi_complex_rhp(
  gsl_complex z,
  gsl_sf_result * result_re,
  gsl_sf_result * result_im
  )
{
  int n_recurse = 0;
  int i;
  gsl_complex a;

  if(GSL_REAL(z) == 0.0 && GSL_IMAG(z) == 0.0)
  {
    result_re->val = 0.0;
    result_im->val = 0.0;
    result_re->err = 0.0;
    result_im->err = 0.0;
    return GSL_EDOM;
  }

  /* compute the number of recurrences to apply */
  if(GSL_REAL(z) < 20.0 && fabs(GSL_IMAG(z)) < 20.0)
  {
    const double sp = sqrt(20.0 + GSL_IMAG(z));
    const double sn = sqrt(20.0 - GSL_IMAG(z));
    const double rhs = sp*sn - GSL_REAL(z);
    if(rhs > 0.0) n_recurse = ceil(rhs);
  }

  /* compute asymptotic at the large value z + n_recurse */
  a = psi_complex_asymp(gsl_complex_add_real(z, n_recurse));

  result_re->err = 2.0 * GSL_DBL_EPSILON * fabs(GSL_REAL(a));
  result_im->err = 2.0 * GSL_DBL_EPSILON * fabs(GSL_IMAG(a));

  /* descend recursively, if necessary */
  for(i = n_recurse; i >= 1; --i)
  {
    gsl_complex zn = gsl_complex_add_real(z, i - 1.0);
    gsl_complex zn_inverse = gsl_complex_inverse(zn);
    a = gsl_complex_sub(a, zn_inverse);

    /* accumulate the error, to catch cancellations */
    result_re->err += 2.0 * GSL_DBL_EPSILON * fabs(GSL_REAL(zn_inverse));
    result_im->err += 2.0 * GSL_DBL_EPSILON * fabs(GSL_IMAG(zn_inverse));
  }

  result_re->val = GSL_REAL(a);
  result_im->val = GSL_IMAG(a);

  result_re->err += 2.0 * GSL_DBL_EPSILON * fabs(result_re->val);
  result_im->err += 2.0 * GSL_DBL_EPSILON * fabs(result_im->val);

  return GSL_SUCCESS;
}



/* generic polygamma; assumes n >= 0 and x > 0
 */
static int
psi_n_xg0(const int n, const double x, gsl_sf_result * result)
{
  if(n == 0) {
    return gsl_sf_psi_e(x, result);
  }
  else {
    /* Abramowitz + Stegun 6.4.10 */
    gsl_sf_result ln_nf;
    gsl_sf_result hzeta;
    int stat_hz = gsl_sf_hzeta_e(n+1.0, x, &hzeta);
    int stat_nf = gsl_sf_lnfact_e((unsigned int) n, &ln_nf);
    int stat_e  = gsl_sf_exp_mult_err_e(ln_nf.val, ln_nf.err,
                                           hzeta.val, hzeta.err,
                                           result);
    if(GSL_IS_EVEN(n)) result->val = -result->val;
    return GSL_ERROR_SELECT_3(stat_e, stat_nf, stat_hz);
  }
}



/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_psi_int_e(const int n, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(n <= 0) {
    DOMAIN_ERROR(result);
  }
  else if(n <= PSI_TABLE_NMAX) {
    result->val = psi_table[n];
    result->err = GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    /* Abramowitz+Stegun 6.3.18 */
    const double c2 = -1.0/12.0;
    const double c3 =  1.0/120.0;
    const double c4 = -1.0/252.0;
    const double c5 =  1.0/240.0;
    const double ni2 = (1.0/n)*(1.0/n);
    const double ser = ni2 * (c2 + ni2 * (c3 + ni2 * (c4 + ni2*c5)));
    result->val  = log(n) - 0.5/n + ser;
    result->err  = GSL_DBL_EPSILON * (fabs(log(n)) + fabs(0.5/n) + fabs(ser));
    result->err += GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}


int gsl_sf_psi_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */
  return psi_x(x, result);
}


int
gsl_sf_psi_1piy_e(const double y, gsl_sf_result * result)
{
  const double ay = fabs(y);

  /* CHECK_POINTER(result) */

  if(ay > 1000.0) {
    /* [Abramowitz+Stegun, 6.3.19] */
    const double yi2 = 1.0/(ay*ay);
    const double lny = log(ay);
    const double sum = yi2 * (1.0/12.0 + 1.0/120.0 * yi2 + 1.0/252.0 * yi2*yi2);
    result->val = lny + sum;
    result->err = 2.0 * GSL_DBL_EPSILON * (fabs(lny) + fabs(sum));
    return GSL_SUCCESS;
  }
  else if(ay > 10.0) {
    /* [Abramowitz+Stegun, 6.3.19] */
    const double yi2 = 1.0/(ay*ay);
    const double lny = log(ay);
    const double sum = yi2 * (1.0/12.0 +
                         yi2 * (1.0/120.0 +
                           yi2 * (1.0/252.0 +
                             yi2 * (1.0/240.0 +
                               yi2 * (1.0/132.0 + 691.0/32760.0 * yi2)))));
    result->val = lny + sum;
    result->err = 2.0 * GSL_DBL_EPSILON * (fabs(lny) + fabs(sum));
    return GSL_SUCCESS;
  }
  else if(ay > 1.0){
    const double y2 = ay*ay;
    const double x  = (2.0*ay - 11.0)/9.0;
    const double v  = y2*(1.0/(1.0+y2) + 0.5/(4.0+y2));
    gsl_sf_result result_c;
    cheb_eval_e(&r1py_cs, x, &result_c);
    result->val  = result_c.val - M_EULER + v;
    result->err  = result_c.err;
    result->err += 2.0 * GSL_DBL_EPSILON * (fabs(v) + M_EULER + fabs(result_c.val));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    result->err *= 5.0; /* FIXME: losing a digit somewhere... maybe at x=... ? */
    return GSL_SUCCESS;
  }
  else {
    /* [Abramowitz+Stegun, 6.3.17]
     *
     * -M_EULER + y^2 Sum[1/n 1/(n^2 + y^2), {n,1,M}]
     *   +     Sum[1/n^3, {n,M+1,Infinity}]
     *   - y^2 Sum[1/n^5, {n,M+1,Infinity}]
     *   + y^4 Sum[1/n^7, {n,M+1,Infinity}]
     *   - y^6 Sum[1/n^9, {n,M+1,Infinity}]
     *   + O(y^8)
     *
     * We take M=50 for at least 15 digit precision.
     */
    const int M = 50;
    const double y2 = y*y;
    const double c0 = 0.00019603999466879846570;
    const double c2 = 3.8426659205114376860e-08;
    const double c4 = 1.0041592839497643554e-11;
    const double c6 = 2.9516743763500191289e-15;
    const double p  = c0 + y2 *(-c2 + y2*(c4 - y2*c6));
    double sum = 0.0;
    double v;
    
    int n;
    for(n=1; n<=M; n++) {
      sum += 1.0/(n * (n*n + y*y));
    }

    v = y2 * (sum + p);
    result->val  = -M_EULER + v;
    result->err  = GSL_DBL_EPSILON * (M_EULER + fabs(v));
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
}


int gsl_sf_psi_1_int_e(const int n, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */
  if(n <= 0) {
    DOMAIN_ERROR(result);
  }
  else if(n <= PSI_1_TABLE_NMAX) {
    result->val = psi_1_table[n];
    result->err = GSL_DBL_EPSILON * result->val;
    return GSL_SUCCESS;
  }
  else {
    /* Abramowitz+Stegun 6.4.12
     * double-precision for n > 100
     */
    const double c0 = -1.0/30.0;
    const double c1 =  1.0/42.0;
    const double c2 = -1.0/30.0;
    const double ni2 = (1.0/n)*(1.0/n);
    const double ser =  ni2*ni2 * (c0 + ni2*(c1 + c2*ni2));
    result->val = (1.0 + 0.5/n + 1.0/(6.0*n*n) + ser) / n;
    result->err = GSL_DBL_EPSILON * result->val;
    return GSL_SUCCESS;
  }
}


int gsl_sf_psi_1_e(const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x == 0.0 || x == -1.0 || x == -2.0) {
    DOMAIN_ERROR(result);
  }
  else if(x > 0.0)
  {
    return psi_n_xg0(1, x, result);
  }
  else if(x > -5.0)
  {
    /* Abramowitz + Stegun 6.4.6 */
    int M = -floor(x);
    double fx = x + M;
    double sum = 0.0;
    int m;

    if(fx == 0.0)
      DOMAIN_ERROR(result);

    for(m = 0; m < M; ++m)
      sum += 1.0/((x+m)*(x+m));

    {
      int stat_psi = psi_n_xg0(1, fx, result);
      result->val += sum;
      result->err += M * GSL_DBL_EPSILON * sum;
      return stat_psi;
    }
  }
  else
  {
    /* Abramowitz + Stegun 6.4.7 */
    const double sin_px = sin(M_PI * x);
    const double d = M_PI*M_PI/(sin_px*sin_px);
    gsl_sf_result r;
    int stat_psi = psi_n_xg0(1, 1.0-x, &r);
    result->val = d - r.val;
    result->err = r.err + 2.0*GSL_DBL_EPSILON*d;
    return stat_psi;
  }
}


int gsl_sf_psi_n_e(const int n, const double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(n == 0)
  {
    return gsl_sf_psi_e(x, result);
  }
  else if(n == 1)
  {
    return gsl_sf_psi_1_e(x, result);
  }
  else if(n < 0 || x <= 0.0) {
    DOMAIN_ERROR(result);
  }
  else {
    gsl_sf_result ln_nf;
    gsl_sf_result hzeta;
    int stat_hz = gsl_sf_hzeta_e(n+1.0, x, &hzeta);
    int stat_nf = gsl_sf_lnfact_e((unsigned int) n, &ln_nf);
    int stat_e  = gsl_sf_exp_mult_err_e(ln_nf.val, ln_nf.err,
                                           hzeta.val, hzeta.err,
                                           result);
    if(GSL_IS_EVEN(n)) result->val = -result->val;
    return GSL_ERROR_SELECT_3(stat_e, stat_nf, stat_hz);
  }
}


int
gsl_sf_complex_psi_e(
  const double x,
  const double y,
  gsl_sf_result * result_re,
  gsl_sf_result * result_im
  )
{
  if(x >= 0.0)
  {
    gsl_complex z = gsl_complex_rect(x, y);
    return psi_complex_rhp(z, result_re, result_im);
  }
  else
  {
    /* reflection formula [Abramowitz+Stegun, 6.3.7] */
    gsl_complex z = gsl_complex_rect(x, y);
    gsl_complex omz = gsl_complex_rect(1.0 - x, -y);
    gsl_complex zpi = gsl_complex_mul_real(z, M_PI);
    gsl_complex cotzpi = gsl_complex_cot(zpi);
    int ret_val = psi_complex_rhp(omz, result_re, result_im);

    if(GSL_IS_REAL(GSL_REAL(cotzpi)) && GSL_IS_REAL(GSL_IMAG(cotzpi)))
    {
      result_re->val -= M_PI * GSL_REAL(cotzpi);
      result_im->val -= M_PI * GSL_IMAG(cotzpi);
      return ret_val;
    }
    else
    {
      GSL_ERROR("singularity", GSL_EDOM);
    }
  }
}



/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_psi_int(const int n)
{
  EVAL_RESULT(gsl_sf_psi_int_e(n, &result));
}

double gsl_sf_psi(const double x)
{
  EVAL_RESULT(gsl_sf_psi_e(x, &result));
}

double gsl_sf_psi_1piy(const double x)
{
  EVAL_RESULT(gsl_sf_psi_1piy_e(x, &result));
}

double gsl_sf_psi_1_int(const int n)
{
  EVAL_RESULT(gsl_sf_psi_1_int_e(n, &result));
}

double gsl_sf_psi_1(const double x)
{
  EVAL_RESULT(gsl_sf_psi_1_e(x, &result));
}

double gsl_sf_psi_n(const int n, const double x)
{
  EVAL_RESULT(gsl_sf_psi_n_e(n, x, &result));
}
