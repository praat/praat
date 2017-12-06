/* specfunc/fermi_dirac.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author:  G. Jungman */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_sf_exp.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_hyperg.h"
#include "gsl_sf_pow_int.h"
#include "gsl_sf_zeta.h"
#include "gsl_sf_fermi_dirac.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

#define locEPS  (1000.0*GSL_DBL_EPSILON)


/* Chebyshev fit for F_{1}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_1_a_data[22] = {
  1.8949340668482264365,
  0.7237719066890052793,
  0.1250000000000000000,
  0.0101065196435973942,
  0.0,
 -0.0000600615242174119,
  0.0,
  6.816528764623e-7,
  0.0,
 -9.5895779195e-9,
  0.0,
  1.515104135e-10,
  0.0,
 -2.5785616e-12,
  0.0,
  4.62270e-14,
  0.0,
 -8.612e-16,
  0.0,
  1.65e-17,
  0.0,
 -3.e-19
};
static cheb_series fd_1_a_cs = {
  fd_1_a_data,
  21,
  -1, 1,
  12
};


/* Chebyshev fit for F_{1}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_1_b_data[22] = {
  10.409136795234611872,
  3.899445098225161947,
  0.513510935510521222,
  0.010618736770218426,
 -0.001584468020659694,
  0.000146139297161640,
 -1.408095734499e-6,
 -2.177993899484e-6,
  3.91423660640e-7,
 -2.3860262660e-8,
 -4.138309573e-9,
  1.283965236e-9,
 -1.39695990e-10,
 -4.907743e-12,
  4.399878e-12,
 -7.17291e-13,
  2.4320e-14,
  1.4230e-14,
 -3.446e-15,
  2.93e-16,
  3.7e-17,
 -1.6e-17
};
static cheb_series fd_1_b_cs = {
  fd_1_b_data,
  21,
  -1, 1,
  11
};


/* Chebyshev fit for F_{1}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_1_c_data[23] = {
  56.78099449124299762,
  21.00718468237668011,
  2.24592457063193457,
  0.00173793640425994,
 -0.00058716468739423,
  0.00016306958492437,
 -0.00003817425583020,
  7.64527252009e-6,
 -1.31348500162e-6,
  1.9000646056e-7,
 -2.141328223e-8,
  1.23906372e-9,
  2.1848049e-10,
 -1.0134282e-10,
  2.484728e-11,
 -4.73067e-12,
  7.3555e-13,
 -8.740e-14,
  4.85e-15,
  1.23e-15,
 -5.6e-16,
  1.4e-16,
 -3.e-17
};
static cheb_series fd_1_c_cs = {
  fd_1_c_data,
  22,
  -1, 1,
  13
};


/* Chebyshev fit for F_{1}(x) / x^2
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 * x = 10(t+2)
 */
static double fd_1_d_data[30] = {
  1.0126626021151374442,
 -0.0063312525536433793,
  0.0024837319237084326,
 -0.0008764333697726109,
  0.0002913344438921266,
 -0.0000931877907705692,
  0.0000290151342040275,
 -8.8548707259955e-6,
  2.6603474114517e-6,
 -7.891415690452e-7,
  2.315730237195e-7,
 -6.73179452963e-8,
  1.94048035606e-8,
 -5.5507129189e-9,
  1.5766090896e-9,
 -4.449310875e-10,
  1.248292745e-10,
 -3.48392894e-11,
  9.6791550e-12,
 -2.6786240e-12,
  7.388852e-13,
 -2.032828e-13,
  5.58115e-14,
 -1.52987e-14,
  4.1886e-15,
 -1.1458e-15,
  3.132e-16,
 -8.56e-17,
  2.33e-17,
 -5.9e-18
};
static cheb_series fd_1_d_cs = {
  fd_1_d_data,
  29,
  -1, 1,
  14
};


/* Chebyshev fit for F_{1}(x) / x^2
 * 30 < x < Inf
 * -1 < t < 1
 * t = 60/x - 1
 * x = 60/(t+1)
 */
static double fd_1_e_data[10] = {
  1.0013707783890401683,
  0.0009138522593601060,
  0.0002284630648400133,
 -1.57e-17,
 -1.27e-17,
 -9.7e-18,
 -6.9e-18,
 -4.6e-18,
 -2.9e-18,
 -1.7e-18
};
static cheb_series fd_1_e_cs = {
  fd_1_e_data,
  9,
  -1, 1,
  4
};


/* Chebyshev fit for F_{2}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_2_a_data[21] = {
  2.1573661917148458336,
  0.8849670334241132182,
  0.1784163467613519713,
  0.0208333333333333333,
  0.0012708226459768508,
  0.0,
 -5.0619314244895e-6,
  0.0,
  4.32026533989e-8,
  0.0,
 -4.870544166e-10,
  0.0,
  6.4203740e-12,
  0.0,
 -9.37424e-14,
  0.0,
  1.4715e-15,
  0.0,
 -2.44e-17,
  0.0,
  4.e-19
};
static cheb_series fd_2_a_cs = {
  fd_2_a_data,
  20,
  -1, 1,
  12
};


/* Chebyshev fit for F_{2}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_2_b_data[22] = {
  16.508258811798623599,
  7.421719394793067988,
  1.458309885545603821,
  0.128773850882795229,
  0.001963612026198147,
 -0.000237458988738779,
  0.000018539661382641,
 -1.92805649479e-7,
 -2.01950028452e-7,
  3.2963497518e-8,
 -1.885817092e-9,
 -2.72632744e-10,
  8.0554561e-11,
 -8.313223e-12,
 -2.24489e-13,
  2.18778e-13,
 -3.4290e-14,
  1.225e-15,
  5.81e-16,
 -1.37e-16,
  1.2e-17,
  1.e-18
};
static cheb_series fd_2_b_cs = {
  fd_2_b_data,
  21,
  -1, 1,
  12
};


/* Chebyshev fit for F_{1}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_2_c_data[20] = {
  168.87129776686440711,
  81.80260488091659458,
  15.75408505947931513,
  1.12325586765966440,
  0.00059057505725084,
 -0.00016469712946921,
  0.00003885607810107,
 -7.89873660613e-6,
  1.39786238616e-6,
 -2.1534528656e-7,
  2.831510953e-8,
 -2.94978583e-9,
  1.6755082e-10,
  2.234229e-11,
 -1.035130e-11,
  2.41117e-12,
 -4.3531e-13,
  6.447e-14,
 -7.39e-15,
  4.3e-16
};
static cheb_series fd_2_c_cs = {
  fd_2_c_data,
  19,
  -1, 1,
  12
};


/* Chebyshev fit for F_{1}(x) / x^3
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 * x = 10(t+2)
 */
static double fd_2_d_data[30] = {
  0.3459960518965277589,
 -0.00633136397691958024,
  0.00248382959047594408,
 -0.00087651191884005114,
  0.00029139255351719932,
 -0.00009322746111846199,
  0.00002904021914564786,
 -8.86962264810663e-6,
  2.66844972574613e-6,
 -7.9331564996004e-7,
  2.3359868615516e-7,
 -6.824790880436e-8,
  1.981036528154e-8,
 -5.71940426300e-9,
  1.64379426579e-9,
 -4.7064937566e-10,
  1.3432614122e-10,
 -3.823400534e-11,
  1.085771994e-11,
 -3.07727465e-12,
  8.7064848e-13,
 -2.4595431e-13,
  6.938531e-14,
 -1.954939e-14,
  5.50162e-15,
 -1.54657e-15,
  4.3429e-16,
 -1.2178e-16,
  3.394e-17,
 -8.81e-18
};
static cheb_series fd_2_d_cs = {
  fd_2_d_data,
  29,
  -1, 1,
  14
};


/* Chebyshev fit for F_{2}(x) / x^3
 * 30 < x < Inf
 * -1 < t < 1
 * t = 60/x - 1
 * x = 60/(t+1)
 */
static double fd_2_e_data[4] = {
  0.3347041117223735227,
  0.00091385225936012645,
  0.00022846306484003205,
  5.2e-19
};
static cheb_series fd_2_e_cs = {
  fd_2_e_data,
  3,
  -1, 1,
  3
};


/* Chebyshev fit for F_{-1/2}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_mhalf_a_data[20] = {
  1.2663290042859741974,
  0.3697876251911153071,
  0.0278131011214405055,
 -0.0033332848565672007,
 -0.0004438108265412038,
  0.0000616495177243839,
  8.7589611449897e-6,
 -1.2622936986172e-6,
 -1.837464037221e-7,
  2.69495091400e-8,
  3.9760866257e-9,
 -5.894468795e-10,
 -8.77321638e-11,
  1.31016571e-11,
  1.9621619e-12,
 -2.945887e-13,
 -4.43234e-14,
  6.6816e-15,
  1.0084e-15,
 -1.561e-16
};
static cheb_series fd_mhalf_a_cs = {
  fd_mhalf_a_data,
  19,
  -1, 1,
  12
};


/* Chebyshev fit for F_{-1/2}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_mhalf_b_data[20] = {
  3.270796131942071484,
  0.5809004935853417887,
 -0.0299313438794694987,
 -0.0013287935412612198,
  0.0009910221228704198,
 -0.0001690954939688554,
  6.5955849946915e-6,
  3.5953966033618e-6,
 -9.430672023181e-7,
  8.75773958291e-8,
  1.06247652607e-8,
 -4.9587006215e-9,
  7.160432795e-10,
  4.5072219e-12,
 -2.3695425e-11,
  4.9122208e-12,
 -2.905277e-13,
 -9.59291e-14,
  3.00028e-14,
 -3.4970e-15
};
static cheb_series fd_mhalf_b_cs = {
  fd_mhalf_b_data,
  19,
  -1, 1,
  12
};


/* Chebyshev fit for F_{-1/2}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_mhalf_c_data[25] = {
  5.828283273430595507,
  0.677521118293264655,
 -0.043946248736481554,
  0.005825595781828244,
 -0.000864858907380668,
  0.000110017890076539,
 -6.973305225404e-6,
 -1.716267414672e-6,
  8.59811582041e-7,
 -2.33066786976e-7,
  4.8503191159e-8,
 -8.130620247e-9,
  1.021068250e-9,
 -5.3188423e-11,
 -1.9430559e-11,
  8.750506e-12,
 -2.324897e-12,
  4.83102e-13,
 -8.1207e-14,
  1.0132e-14,
 -4.64e-16,
 -2.24e-16,
  9.7e-17,
 -2.6e-17,
  5.e-18
};
static cheb_series fd_mhalf_c_cs = {
  fd_mhalf_c_data,
  24,
  -1, 1,
  13
};


/* Chebyshev fit for F_{-1/2}(x) / x^(1/2)
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 */
static double fd_mhalf_d_data[30] = {
  2.2530744202862438709,
  0.0018745152720114692,
 -0.0007550198497498903,
  0.0002759818676644382,
 -0.0000959406283465913,
  0.0000324056855537065,
 -0.0000107462396145761,
  3.5126865219224e-6,
 -1.1313072730092e-6,
  3.577454162766e-7,
 -1.104926666238e-7,
  3.31304165692e-8,
 -9.5837381008e-9,
  2.6575790141e-9,
 -7.015201447e-10,
  1.747111336e-10,
 -4.04909605e-11,
  8.5104999e-12,
 -1.5261885e-12,
  1.876851e-13,
  1.00574e-14,
 -1.82002e-14,
  8.6634e-15,
 -3.2058e-15,
  1.0572e-15,
 -3.259e-16,
  9.60e-17,
 -2.74e-17,
  7.6e-18,
 -1.9e-18
};
static cheb_series fd_mhalf_d_cs = {
  fd_mhalf_d_data,
  29,
  -1, 1,
  15
};


/* Chebyshev fit for F_{1/2}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_half_a_data[23] = {
  1.7177138871306189157,
  0.6192579515822668460,
  0.0932802275119206269,
  0.0047094853246636182,
 -0.0004243667967864481,
 -0.0000452569787686193,
  5.2426509519168e-6,
  6.387648249080e-7,
 -8.05777004848e-8,
 -1.04290272415e-8,
  1.3769478010e-9,
  1.847190359e-10,
 -2.51061890e-11,
 -3.4497818e-12,
  4.784373e-13,
  6.68828e-14,
 -9.4147e-15,
 -1.3333e-15,
  1.898e-16,
  2.72e-17,
 -3.9e-18,
 -6.e-19,
  1.e-19
};
static cheb_series fd_half_a_cs = {
  fd_half_a_data,
  22,
  -1, 1,
  11
};


/* Chebyshev fit for F_{1/2}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_half_b_data[20] = {
  7.651013792074984027,
  2.475545606866155737,
  0.218335982672476128,
 -0.007730591500584980,
 -0.000217443383867318,
  0.000147663980681359,
 -0.000021586361321527,
  8.07712735394e-7,
  3.28858050706e-7,
 -7.9474330632e-8,
  6.940207234e-9,
  6.75594681e-10,
 -3.10200490e-10,
  4.2677233e-11,
 -2.1696e-14,
 -1.170245e-12,
  2.34757e-13,
 -1.4139e-14,
 -3.864e-15,
  1.202e-15
};
static cheb_series fd_half_b_cs = {
  fd_half_b_data,
  19,
  -1, 1,
  12
};


/* Chebyshev fit for F_{1/2}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_half_c_data[23] = {
  29.584339348839816528,
  8.808344283250615592,
  0.503771641883577308,
 -0.021540694914550443,
  0.002143341709406890,
 -0.000257365680646579,
  0.000027933539372803,
 -1.678525030167e-6,
 -2.78100117693e-7,
  1.35218065147e-7,
 -3.3740425009e-8,
  6.474834942e-9,
 -1.009678978e-9,
  1.20057555e-10,
 -6.636314e-12,
 -1.710566e-12,
  7.75069e-13,
 -1.97973e-13,
  3.9414e-14,
 -6.374e-15,
  7.77e-16,
 -4.0e-17,
 -1.4e-17
};
static cheb_series fd_half_c_cs = {
  fd_half_c_data,
  22,
  -1, 1,
  13
};


/* Chebyshev fit for F_{1/2}(x) / x^(3/2)
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 */
static double fd_half_d_data[30] = {
  1.5116909434145508537,
 -0.0036043405371630468,
  0.0014207743256393359,
 -0.0005045399052400260,
  0.0001690758006957347,
 -0.0000546305872688307,
  0.0000172223228484571,
 -5.3352603788706e-6,
  1.6315287543662e-6,
 -4.939021084898e-7,
  1.482515450316e-7,
 -4.41552276226e-8,
  1.30503160961e-8,
 -3.8262599802e-9,
  1.1123226976e-9,
 -3.204765534e-10,
  9.14870489e-11,
 -2.58778946e-11,
  7.2550731e-12,
 -2.0172226e-12,
  5.566891e-13,
 -1.526247e-13,
  4.16121e-14,
 -1.12933e-14,
  3.0537e-15,
 -8.234e-16,
  2.215e-16,
 -5.95e-17,
  1.59e-17,
 -4.0e-18
};
static cheb_series fd_half_d_cs = {
  fd_half_d_data,
  29,
  -1, 1,
  15
};



/* Chebyshev fit for F_{3/2}(t);  -1 < t < 1, -1 < x < 1
 */
static double fd_3half_a_data[20] = {
  2.0404775940601704976,
  0.8122168298093491444,
  0.1536371165644008069,
  0.0156174323847845125,
  0.0005943427879290297,
 -0.0000429609447738365,
 -3.8246452994606e-6,
  3.802306180287e-7,
  4.05746157593e-8,
 -4.5530360159e-9,
 -5.306873139e-10,
  6.37297268e-11,
  7.8403674e-12,
 -9.840241e-13,
 -1.255952e-13,
  1.62617e-14,
  2.1318e-15,
 -2.825e-16,
 -3.78e-17,
  5.1e-18
};
static cheb_series fd_3half_a_cs = {
  fd_3half_a_data,
  19,
  -1, 1,
  11
};


/* Chebyshev fit for F_{3/2}(3/2(t+1) + 1);  -1 < t < 1, 1 < x < 4
 */
static double fd_3half_b_data[22] = {
  13.403206654624176674,
  5.574508357051880924,
  0.931228574387527769,
  0.054638356514085862,
 -0.001477172902737439,
 -0.000029378553381869,
  0.000018357033493246,
 -2.348059218454e-6,
  8.3173787440e-8,
  2.6826486956e-8,
 -6.011244398e-9,
  4.94345981e-10,
  3.9557340e-11,
 -1.7894930e-11,
  2.348972e-12,
 -1.2823e-14,
 -5.4192e-14,
  1.0527e-14,
 -6.39e-16,
 -1.47e-16,
  4.5e-17,
 -5.e-18
};
static cheb_series fd_3half_b_cs = {
  fd_3half_b_data,
  21,
  -1, 1,
  12
};


/* Chebyshev fit for F_{3/2}(3(t+1) + 4);  -1 < t < 1, 4 < x < 10
 */
static double fd_3half_c_data[21] = {
  101.03685253378877642,
  43.62085156043435883,
  6.62241373362387453,
  0.25081415008708521,
 -0.00798124846271395,
  0.00063462245101023,
 -0.00006392178890410,
  6.04535131939e-6,
 -3.4007683037e-7,
 -4.072661545e-8,
  1.931148453e-8,
 -4.46328355e-9,
  7.9434717e-10,
 -1.1573569e-10,
  1.304658e-11,
 -7.4114e-13,
 -1.4181e-13,
  6.491e-14,
 -1.597e-14,
  3.05e-15,
 -4.8e-16
};
static cheb_series fd_3half_c_cs = {
  fd_3half_c_data,
  20,
  -1, 1,
  12
};


/* Chebyshev fit for F_{3/2}(x) / x^(5/2)
 * 10 < x < 30 
 * -1 < t < 1
 * t = 1/10 (x-10) - 1 = x/10 - 2
 */
static double fd_3half_d_data[25] = {
  0.6160645215171852381,
 -0.0071239478492671463,
  0.0027906866139659846,
 -0.0009829521424317718,
  0.0003260229808519545,
 -0.0001040160912910890,
  0.0000322931223232439,
 -9.8243506588102e-6,
  2.9420132351277e-6,
 -8.699154670418e-7,
  2.545460071999e-7,
 -7.38305056331e-8,
  2.12545670310e-8,
 -6.0796532462e-9,
  1.7294556741e-9,
 -4.896540687e-10,
  1.380786037e-10,
 -3.88057305e-11,
  1.08753212e-11,
 -3.0407308e-12,
  8.485626e-13,
 -2.364275e-13,
  6.57636e-14,
 -1.81807e-14,
  4.6884e-15
};
static cheb_series fd_3half_d_cs = {
  fd_3half_d_data,
  24,
  -1, 1,
  16
};


/* Goano's modification of the Levin-u implementation.
 * This is a simplification of the original WHIZ algorithm.
 * See [Fessler et al., ACM Toms 9, 346 (1983)].
 */
static
int
fd_whiz(const double term, const int iterm,
        double * qnum, double * qden,
        double * result, double * s)
{
  if(iterm == 0) *s = 0.0;

  *s += term;

  qden[iterm] = 1.0/(term*(iterm+1.0)*(iterm+1.0));
  qnum[iterm] = *s * qden[iterm];

  if(iterm > 0) {
    double factor = 1.0;
    double ratio  = iterm/(iterm+1.0);
    int j;
    for(j=iterm-1; j>=0; j--) {
      double c = factor * (j+1.0) / (iterm+1.0);
      factor *= ratio;
      qden[j] = qden[j+1] - c * qden[j];
      qnum[j] = qnum[j+1] - c * qnum[j];
    }
  }

  *result = qnum[0] / qden[0];
  return GSL_SUCCESS;
}


/* Handle case of integer j <= -2.
 */
static
int
fd_nint(const int j, const double x, gsl_sf_result * result)
{
/*    const int nsize = 100 + 1; */
  enum {
    nsize = 100+1
  };
  double qcoeff[nsize];

  if(j >= -1) {
    result->val = 0.0;
    result->err = 0.0;
    GSL_ERROR ("error", GSL_ESANITY);
  }
  else if(j < -(nsize)) {
    result->val = 0.0;
    result->err = 0.0;
    GSL_ERROR ("error", GSL_EUNIMPL);
  }
  else {
    double a, p, f;
    int i, k;
    int n = -(j+1);

    qcoeff[1] = 1.0;

    for(k=2; k<=n; k++) {
      qcoeff[k] = -qcoeff[k-1];
      for(i=k-1; i>=2; i--) {
        qcoeff[i] = i*qcoeff[i] - (k-(i-1))*qcoeff[i-1];
      }
    }

    if(x >= 0.0) {
      a = exp(-x);
      f = qcoeff[1];
      for(i=2; i<=n; i++) {
        f = f*a + qcoeff[i];
      }
    }
    else {
      a = exp(x);
      f = qcoeff[n];
      for(i=n-1; i>=1; i--) {
        f = f*a + qcoeff[i];
      }
    }

    p = gsl_sf_pow_int(1.0+a, j);
    result->val = f*a*p;
    result->err = 3.0 * GSL_DBL_EPSILON * fabs(f*a*p);
    return GSL_SUCCESS;
  }
}


/* x < 0
 */
static
int
fd_neg(const double j, const double x, gsl_sf_result * result)
{
  enum {
    itmax = 100,
    qsize = 100+1
  };
/*    const int itmax = 100; */
/*    const int qsize = 100 + 1; */
  double qnum[qsize], qden[qsize];

  if(x < GSL_LOG_DBL_MIN) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else if(x < -1.0 && x < -fabs(j+1.0)) {
    /* Simple series implementation. Avoid the
     * complexity and extra work of the series
     * acceleration method below.
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100; n++) {
      double rat = (n-1.0)/n;
      double p   = pow(rat, j+1.0);
      term *= -ex * p;
      sum  += term;
      if(fabs(term/sum) < GSL_DBL_EPSILON) break;
    }
    result->val = sum;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(sum);
    return GSL_SUCCESS;
  }
  else {
    double s = 0.0;
    double xn = x;
    double ex  = -exp(x);
    double enx = -ex;
    double f = 0.0;
    double f_previous;
    int jterm;
    for(jterm=0; jterm<=itmax; jterm++) {
      double p = pow(jterm+1.0, j+1.0);
      double term = enx/p;
      f_previous = f;
      fd_whiz(term, jterm, qnum, qden, &f, &s);
      xn += x;
      if(fabs(f-f_previous) < fabs(f)*2.0*GSL_DBL_EPSILON || xn < GSL_LOG_DBL_MIN) break;
      enx *= ex;
    }

    result->val  = f;
    result->err  = fabs(f-f_previous);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(f);

    if(jterm == itmax)
      GSL_ERROR ("error", GSL_EMAXITER);
    else
      return GSL_SUCCESS;
  }
}


/* asymptotic expansion
 * j + 2.0 > 0.0
 */
static
int
fd_asymp(const double j, const double x, gsl_sf_result * result)
{
  const int j_integer = ( fabs(j - floor(j+0.5)) < 100.0*GSL_DBL_EPSILON );
  const int itmax = 200;
  gsl_sf_result lg;
  int stat_lg = gsl_sf_lngamma_e(j + 2.0, &lg);
  double seqn_val = 0.5;
  double seqn_err = 0.0;
  double xm2  = (1.0/x)/x;
  double xgam = 1.0;
  double add = GSL_DBL_MAX;
  double cos_term;
  double ln_x;
  double ex_term_1;
  double ex_term_2;
  gsl_sf_result fneg;
  gsl_sf_result ex_arg;
  gsl_sf_result ex;
  int stat_fneg;
  int stat_e;
  int n;
  for(n=1; n<=itmax; n++) {
    double add_previous = add;
    gsl_sf_result eta;
    gsl_sf_eta_int_e(2*n, &eta);
    xgam = xgam * xm2 * (j + 1.0 - (2*n-2)) * (j + 1.0 - (2*n-1));
    add  = eta.val * xgam;
    if(!j_integer && fabs(add) > fabs(add_previous)) break;
    if(fabs(add/seqn_val) < GSL_DBL_EPSILON) break;
    seqn_val += add;
    seqn_err += 2.0 * GSL_DBL_EPSILON * fabs(add);
  }
  seqn_err += fabs(add);

  stat_fneg = fd_neg(j, -x, &fneg);
  ln_x = log(x);
  ex_term_1 = (j+1.0)*ln_x;
  ex_term_2 = lg.val;
  ex_arg.val = ex_term_1 - ex_term_2; /*(j+1.0)*ln_x - lg.val; */
  ex_arg.err = GSL_DBL_EPSILON*(fabs(ex_term_1) + fabs(ex_term_2)) + lg.err;
  stat_e    = gsl_sf_exp_err_e(ex_arg.val, ex_arg.err, &ex);
  cos_term  = cos(j*M_PI);
  result->val  = cos_term * fneg.val + 2.0 * seqn_val * ex.val;
  result->err  = fabs(2.0 * ex.err * seqn_val);
  result->err += fabs(2.0 * ex.val * seqn_err);
  result->err += fabs(cos_term) * fneg.err;
  result->err += 4.0 * GSL_DBL_EPSILON * fabs(result->val);
  return GSL_ERROR_SELECT_3(stat_e, stat_fneg, stat_lg);
}


/* Series evaluation for small x, generic j.
 * [Goano (8)]
 */
#if 0
static
int
fd_series(const double j, const double x, double * result)
{
  const int nmax = 1000;
  int n;
  double sum = 0.0;
  double prev;
  double pow_factor = 1.0;
  double eta_factor;
  gsl_sf_eta_e(j + 1.0, &eta_factor);
  prev = pow_factor * eta_factor;
  sum += prev;
  for(n=1; n<nmax; n++) {
    double term;
    gsl_sf_eta_e(j+1.0-n, &eta_factor);
    pow_factor *= x/n;
    term = pow_factor * eta_factor;
    sum += term;
    if(fabs(term/sum) < GSL_DBL_EPSILON && fabs(prev/sum) < GSL_DBL_EPSILON) break;
    prev = term;
  }

  *result = sum;
  return GSL_SUCCESS;
}
#endif /* 0 */


/* Series evaluation for small x > 0, integer j > 0; x < Pi.
 * [Goano (8)]
 */
static
int
fd_series_int(const int j, const double x, gsl_sf_result * result)
{
  int n;
  double sum = 0.0;
  double del;
  double pow_factor = 1.0;
  gsl_sf_result eta_factor;
  gsl_sf_eta_int_e(j + 1, &eta_factor);
  del  = pow_factor * eta_factor.val;
  sum += del;

  /* Sum terms where the argument
   * of eta() is positive.
   */
  for(n=1; n<=j+2; n++) {
    gsl_sf_eta_int_e(j+1-n, &eta_factor);
    pow_factor *= x/n;
    del  = pow_factor * eta_factor.val;
    sum += del;
    if(fabs(del/sum) < GSL_DBL_EPSILON) break;
  }

  /* Now sum the terms where eta() is negative.
   * The argument of eta() must be odd as well,
   * so it is convenient to transform the series
   * as follows:
   *
   * Sum[ eta(j+1-n) x^n / n!, {n,j+4,Infinity}]
   * = x^j / j! Sum[ eta(1-2m) x^(2m) j! / (2m+j)! , {m,2,Infinity}]
   *
   * We do not need to do this sum if j is large enough.
   */
  if(j < 32) {
    int m;
    gsl_sf_result jfact;
    double sum2;
    double pre2;

    gsl_sf_fact_e((unsigned int)j, &jfact);
    pre2 = gsl_sf_pow_int(x, j) / jfact.val;

    gsl_sf_eta_int_e(-3, &eta_factor);
    pow_factor = x*x*x*x / ((j+4)*(j+3)*(j+2)*(j+1));
    sum2 = eta_factor.val * pow_factor;

    for(m=3; m<24; m++) {
      gsl_sf_eta_int_e(1-2*m, &eta_factor);
      pow_factor *= x*x / ((j+2*m)*(j+2*m-1));
      sum2 += eta_factor.val * pow_factor;
    }

    sum += pre2 * sum2;
  }

  result->val = sum;
  result->err = 2.0 * GSL_DBL_EPSILON * fabs(sum);

  return GSL_SUCCESS;
}


/* series of hypergeometric functions for integer j > 0, x > 0
 * [Goano (7)]
 */
static
int
fd_UMseries_int(const int j, const double x, gsl_sf_result * result)
{
  const int nmax = 2000;
  double pre;
  double lnpre_val;
  double lnpre_err;
  double sum_even_val = 1.0;
  double sum_even_err = 0.0;
  double sum_odd_val  = 0.0;
  double sum_odd_err  = 0.0;
  int stat_sum;
  int stat_e;
  int stat_h = GSL_SUCCESS;
  int n;

  if(x < 500.0 && j < 80) {
    double p = gsl_sf_pow_int(x, j+1);
    gsl_sf_result g;
    gsl_sf_fact_e(j+1, &g); /* Gamma(j+2) */
    lnpre_val = 0.0;
    lnpre_err = 0.0;
    pre   = p/g.val;
  }
  else {
    double lnx = log(x);
    gsl_sf_result lg;
    gsl_sf_lngamma_e(j + 2.0, &lg);
    lnpre_val = (j+1.0)*lnx - lg.val;
    lnpre_err = 2.0 * GSL_DBL_EPSILON * fabs((j+1.0)*lnx) + lg.err;
    pre = 1.0;
  }

  /* Add up the odd terms of the sum.
   */
  for(n=1; n<nmax; n+=2) {
    double del_val;
    double del_err;
    gsl_sf_result U;
    gsl_sf_result M;
    int stat_h_U = gsl_sf_hyperg_U_int_e(1, j+2, n*x, &U);
    int stat_h_F = gsl_sf_hyperg_1F1_int_e(1, j+2, -n*x, &M);
    stat_h = GSL_ERROR_SELECT_3(stat_h, stat_h_U, stat_h_F);
    del_val = ((j+1.0)*U.val - M.val);
    del_err = (fabs(j+1.0)*U.err + M.err);
    sum_odd_val += del_val;
    sum_odd_err += del_err;
    if(fabs(del_val/sum_odd_val) < GSL_DBL_EPSILON) break;
  }

  /* Add up the even terms of the sum.
   */
  for(n=2; n<nmax; n+=2) {
    double del_val;
    double del_err;
    gsl_sf_result U;
    gsl_sf_result M;
    int stat_h_U = gsl_sf_hyperg_U_int_e(1, j+2, n*x, &U);
    int stat_h_F = gsl_sf_hyperg_1F1_int_e(1, j+2, -n*x, &M);
    stat_h = GSL_ERROR_SELECT_3(stat_h, stat_h_U, stat_h_F);
    del_val = ((j+1.0)*U.val - M.val);
    del_err = (fabs(j+1.0)*U.err + M.err);
    sum_even_val -= del_val;
    sum_even_err += del_err;
    if(fabs(del_val/sum_even_val) < GSL_DBL_EPSILON) break;
  }

  stat_sum = ( n >= nmax ? GSL_EMAXITER : GSL_SUCCESS );
  stat_e   = gsl_sf_exp_mult_err_e(lnpre_val, lnpre_err,
                                      pre*(sum_even_val + sum_odd_val),
                                      pre*(sum_even_err + sum_odd_err),
                                      result);
  result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);

  return GSL_ERROR_SELECT_3(stat_e, stat_h, stat_sum);
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

/* [Goano (4)] */
int gsl_sf_fermi_dirac_m1_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < 0.0) {
    const double ex = exp(x);
    result->val = ex/(1.0+ex);
    result->err = 2.0 * (fabs(x) + 1.0) * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    double ex = exp(-x);
    result->val = 1.0/(1.0 + ex);
    result->err = 2.0 * GSL_DBL_EPSILON * (x + 1.0) * ex;
    return GSL_SUCCESS;
  }
}


/* [Goano (3)] */
int gsl_sf_fermi_dirac_0_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < -5.0) {
    double ex  = exp(x);
    double ser = 1.0 - ex*(0.5 - ex*(1.0/3.0 - ex*(1.0/4.0 - ex*(1.0/5.0 - ex/6.0))));
    result->val = ex * ser;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < 10.0) {
    result->val = log(1.0 + exp(x));
    result->err = fabs(x * GSL_DBL_EPSILON);
    return GSL_SUCCESS;
  }
  else {
    double ex = exp(-x);
    result->val = x + ex * (1.0 - 0.5*ex + ex*ex/3.0 - ex*ex*ex/4.0);
    result->err = (x + ex) * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
}


int gsl_sf_fermi_dirac_1_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * rat;
      sum  += term;
      if(fabs(term/sum) < GSL_DBL_EPSILON) break;
    }
    result->val = sum;
    result->err = 2.0 * fabs(sum) * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    return cheb_eval_e(&fd_1_a_cs, x, result);
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    return cheb_eval_e(&fd_1_b_cs, t, result);
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    return cheb_eval_e(&fd_1_c_cs, t, result);
  }
  else if(x < 30.0) {
    double t = 0.1*x - 2.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_1_d_cs, t, &c);
    result->val  = c.val * x*x;
    result->err  = c.err * x*x + GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < 1.0/GSL_SQRT_DBL_EPSILON) {
    double t = 60.0/x - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_1_e_cs, t, &c);
    result->val  = c.val * x*x;
    result->err  = c.err * x*x + GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < GSL_SQRT_DBL_MAX) {
    result->val = 0.5 * x*x;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    OVERFLOW_ERROR(result);
  }
}


int gsl_sf_fermi_dirac_2_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * rat * rat;
      sum  += term;
      if(fabs(term/sum) < GSL_DBL_EPSILON) break;
    }
    result->val = sum;
    result->err = 2.0 * GSL_DBL_EPSILON * fabs(sum);
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    return cheb_eval_e(&fd_2_a_cs, x, result);
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    return cheb_eval_e(&fd_2_b_cs, t, result);
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    return cheb_eval_e(&fd_2_c_cs, t, result);
  }
  else if(x < 30.0) {
    double t = 0.1*x - 2.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_2_d_cs, t, &c);
    result->val  = c.val * x*x*x;
    result->err  = c.err * x*x*x + 3.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < 1.0/GSL_ROOT3_DBL_EPSILON) {
    double t = 60.0/x - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_2_e_cs, t, &c);
    result->val = c.val * x*x*x;
    result->err = c.err * x*x*x + 3.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else if(x < GSL_ROOT3_DBL_MAX) {
    result->val = 1.0/6.0 * x*x*x;
    result->err = 3.0 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    OVERFLOW_ERROR(result);
  }
}


int gsl_sf_fermi_dirac_int_e(const int j, const double x, gsl_sf_result * result)
{
  if(j < -1) {
    return fd_nint(j, x, result);
  }
  else if (j == -1) {
    return gsl_sf_fermi_dirac_m1_e(x, result);
  }
  else if(j == 0) {
    return gsl_sf_fermi_dirac_0_e(x, result);
  }
  else if(j == 1) {
    return gsl_sf_fermi_dirac_1_e(x, result);
  }
  else if(j == 2) {
    return gsl_sf_fermi_dirac_2_e(x, result);
  }
  else if(x < 0.0) {
    return fd_neg(j, x, result);
  }
  else if(x == 0.0) {
    return gsl_sf_eta_int_e(j+1, result);
  }
  else if(x < 1.5) {
    return fd_series_int(j, x, result);
  }
  else {
    gsl_sf_result fasymp;
    int stat_asymp = fd_asymp(j, x, &fasymp);

    if(stat_asymp == GSL_SUCCESS) {
      result->val  = fasymp.val;
      result->err  = fasymp.err;
      result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return stat_asymp;
    }
    else {
      return fd_UMseries_int(j, x, result);
    }
  }
}


int gsl_sf_fermi_dirac_mhalf_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<200 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * sqrt(rat);
      sum  += term;
      if(fabs(term/sum) < GSL_DBL_EPSILON) break;
    }
    result->val = sum;
    result->err = 2.0 * fabs(sum) * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    return cheb_eval_e(&fd_mhalf_a_cs, x, result);
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    return cheb_eval_e(&fd_mhalf_b_cs, t, result);
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    return cheb_eval_e(&fd_mhalf_c_cs, t, result);
  }
  else if(x < 30.0) {
    double rtx = sqrt(x);
    double t = 0.1*x - 2.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_mhalf_d_cs, t, &c);
    result->val  = c.val * rtx;
    result->err  = c.err * rtx + 0.5 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    return fd_asymp(-0.5, x, result);
  }
}


int gsl_sf_fermi_dirac_half_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * sqrt(rat);
      sum  += term;
      if(fabs(term/sum) < GSL_DBL_EPSILON) break;
    }
    result->val = sum;
    result->err = 2.0 * fabs(sum) * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    return cheb_eval_e(&fd_half_a_cs, x, result);
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    return cheb_eval_e(&fd_half_b_cs, t, result);
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    return cheb_eval_e(&fd_half_c_cs, t, result);
  }
  else if(x < 30.0) {
    double x32 = x*sqrt(x);
    double t = 0.1*x - 2.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_half_d_cs, t, &c);
    result->val = c.val * x32;
    result->err = c.err * x32 + 1.5 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    return fd_asymp(0.5, x, result);
  }
}


int gsl_sf_fermi_dirac_3half_e(const double x, gsl_sf_result * result)
{
  if(x < GSL_LOG_DBL_MIN) {
    UNDERFLOW_ERROR(result);
  }
  else if(x < -1.0) {
    /* series [Goano (6)]
     */
    double ex   = exp(x);
    double term = ex;
    double sum  = term;
    int n;
    for(n=2; n<100 ; n++) {
      double rat = (n-1.0)/n;
      term *= -ex * rat * rat * sqrt(rat);
      sum  += term;
      if(fabs(term/sum) < GSL_DBL_EPSILON) break;
    }
    result->val = sum;
    result->err = 2.0 * fabs(sum) * GSL_DBL_EPSILON;
    return GSL_SUCCESS;
  }
  else if(x < 1.0) {
    return cheb_eval_e(&fd_3half_a_cs, x, result);
  }
  else if(x < 4.0) {
    double t = 2.0/3.0*(x-1.0) - 1.0;
    return cheb_eval_e(&fd_3half_b_cs, t, result);
  }
  else if(x < 10.0) {
    double t = 1.0/3.0*(x-4.0) - 1.0;
    return cheb_eval_e(&fd_3half_c_cs, t, result);
  }
  else if(x < 30.0) {
    double x52 = x*x*sqrt(x);
    double t = 0.1*x - 2.0;
    gsl_sf_result c;
    cheb_eval_e(&fd_3half_d_cs, t, &c);
    result->val = c.val * x52;
    result->err = c.err * x52 + 2.5 * GSL_DBL_EPSILON * fabs(result->val);
    return GSL_SUCCESS;
  }
  else {
    return fd_asymp(1.5, x, result);
  }
}

/* [Goano p. 222] */
int gsl_sf_fermi_dirac_inc_0_e(const double x, const double b, gsl_sf_result * result)
{
  if(b < 0.0) {
    DOMAIN_ERROR(result);
  }
  else {
    double arg = b - x;
    gsl_sf_result f0;
    int status = gsl_sf_fermi_dirac_0_e(arg, &f0);
    result->val = f0.val - arg;
    result->err = f0.err + GSL_DBL_EPSILON * (fabs(x) + fabs(b));
    return status;
  }
}



/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_fermi_dirac_m1(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_m1_e(x, &result));
}

double gsl_sf_fermi_dirac_0(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_0_e(x, &result));
}

double gsl_sf_fermi_dirac_1(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_1_e(x, &result));
}

double gsl_sf_fermi_dirac_2(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_2_e(x, &result));
}

double gsl_sf_fermi_dirac_int(const int j, const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_int_e(j, x, &result));
}

double gsl_sf_fermi_dirac_mhalf(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_mhalf_e(x, &result));
}

double gsl_sf_fermi_dirac_half(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_half_e(x, &result));
}

double gsl_sf_fermi_dirac_3half(const double x)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_3half_e(x, &result));
}

double gsl_sf_fermi_dirac_inc_0(const double x, const double b)
{
  EVAL_RESULT(gsl_sf_fermi_dirac_inc_0_e(x, b, &result));
}
