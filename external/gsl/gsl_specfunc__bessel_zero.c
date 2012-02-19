/* specfunc/bessel_zero.c
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
#include "gsl_sf_airy.h"
#include "gsl_sf_pow_int.h"
#include "gsl_sf_bessel.h"

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel_olver.h"

/* For Chebyshev expansions of the roots as functions of nu,
 * see [G. Nemeth, Mathematical Approximation of Special Functions].
 * This gives the fits for all nu and s <= 10.
 * I made the fits for other values of s myself [GJ].
 */

/* Chebyshev expansion: j_{nu,1} = c_k T_k*(nu/2), nu <= 2 */
static const double coef_jnu1_a[] = {
  3.801775243633476,
  1.360704737511120,
 -0.030707710261106,
  0.004526823746202,
 -0.000808682832134,
  0.000159218792489,
 -0.000033225189761,
  0.000007205599763,
 -0.000001606110397,
  0.000000365439424,
 -0.000000084498039,
  0.000000019793815,
 -0.000000004687054,
  0.000000001120052,
 -0.000000000269767,
  0.000000000065420,
 -0.000000000015961,
  0.000000000003914,
 -0.000000000000965,
  0.000000000000239,
 -0.000000000000059,
  0.000000000000015,
 -0.000000000000004,
  0.000000000000001
};


/* Chebyshev expansion: j_{nu,1} = nu c_k T_k*((2/nu)^(2/3)), nu >= 2 */
static const double coef_jnu1_b[] = {
  1.735063412537096,
  0.784478100951978,
  0.048881473180370,
 -0.000578279783021,
 -0.000038984957864,
  0.000005758297879,
 -0.000000327583229,
 -0.000000003853878,
  0.000000002284653,
 -0.000000000153079,
 -0.000000000000895,
  0.000000000000283,
  0.000000000000043,
  0.000000000000010,
 -0.000000000000003
};


/* Chebyshev expansion: j_{nu,2} = c_k T_k*(nu/2), nu <= 2 */
static const double coef_jnu2_a[] = {
  6.992370244046161,
  1.446379282056534,
 -0.023458616207293,
  0.002172149448700,
 -0.000246262775620,
  0.000030990180959,
 -0.000004154183047,
  0.000000580766328,
 -0.000000083648175,
  0.000000012317355,
 -0.000000001844887,
  0.000000000280076,
 -0.000000000042986,
  0.000000000006658,
 -0.000000000001039,
  0.000000000000163,
 -0.000000000000026,
  0.000000000000004,
 -0.000000000000001
};


/* Chebyshev expansion: j_{nu,2} = nu c_k T_k*((2/nu)^(2/3)), nu >= 2 */
static const double coef_jnu2_b[] = {
  2.465611864263400,
  1.607952988471069,
  0.138758034431497,
 -0.003687791182054,
 -0.000051276007868,
  0.000045113570749,
 -0.000007579172152,
  0.000000736469208,
 -0.000000011118527,
 -0.000000011919884,
  0.000000002696788,
 -0.000000000314488,
  0.000000000008124,
  0.000000000005211,
 -0.000000000001292,
  0.000000000000158,
 -0.000000000000004,
 -0.000000000000003,
  0.000000000000001
};


/* Chebyshev expansion: j_{nu,3} = c_k T_k*(nu/3), nu <= 3 */
static const double coef_jnu3_a[] = {
  10.869647065239236,
   2.177524286141710,
  -0.034822817125293,
   0.003167249102413,
  -0.000353960349344,
   0.000044039086085,
  -0.000005851380981,
   0.000000812575483,
  -0.000000116463617,
   0.000000017091246,
  -0.000000002554376,
   0.000000000387335,
  -0.000000000059428,
   0.000000000009207,
  -0.000000000001438,
   0.000000000000226,
  -0.000000000000036,
   0.000000000000006,
  -0.000000000000001
};


/* Chebyshev expansion: j_{nu,3} = nu c_k T_k*((3/nu)^(2/3)), nu >= 3 */
static const double coef_jnu3_b[] = {
  2.522816775173244,
  1.673199424973720,
  0.146431617506314,
 -0.004049001763912,
 -0.000039517767244,
  0.000048781729288,
 -0.000008729705695,
  0.000000928737310,
 -0.000000028388244,
 -0.000000012927432,
  0.000000003441008,
 -0.000000000471695,
  0.000000000025590,
  0.000000000005502,
 -0.000000000001881,
  0.000000000000295,
 -0.000000000000020,
 -0.000000000000003,
  0.000000000000001
};


/* Chebyshev expansion: j_{nu,4} = c_k T_k*(nu/4), nu <= 4 */
static const double coef_jnu4_a[] = {
  14.750310252773009,
   2.908010932941708,
  -0.046093293420315,
   0.004147172321412,
  -0.000459092310473,
   0.000056646951906,
  -0.000007472351546,
   0.000001031210065,
  -0.000000147008137,
   0.000000021475218,
  -0.000000003197208,
   0.000000000483249,
  -0.000000000073946,
   0.000000000011431,
  -0.000000000001782,
   0.000000000000280,
  -0.000000000000044,
   0.000000000000007,
  -0.000000000000001
};


/* Chebyshev expansion: j_{nu,4} = nu c_k T_k*((4/nu)^(2/3)), nu >= 4 */
static const double coef_jnu4_b[] = {
  2.551681323117914,
  1.706177978336572,
  0.150357658406131,
 -0.004234001378590,
 -0.000033854229898,
  0.000050763551485,
 -0.000009337464057,
  0.000001029717834,
 -0.000000037474196,
 -0.000000013450153,
  0.000000003836180,
 -0.000000000557404,
  0.000000000035748,
  0.000000000005487,
 -0.000000000002187,
  0.000000000000374,
 -0.000000000000031,
 -0.000000000000003,
  0.000000000000001
};



/* Chebyshev expansion: j_{nu,5} = c_k T_k*(nu/5), nu <= 5 */
static const double coef_jnu5_a[] = {
  18.632261081028211,
   3.638249012596966,
  -0.057329705998828,
   0.005121709126820,
  -0.000563325259487,
   0.000069100826174,
  -0.000009066603030,
   0.000001245181383,
  -0.000000176737282,
   0.000000025716695,
  -0.000000003815184,
   0.000000000574839,
  -0.000000000087715,
   0.000000000013526,
  -0.000000000002104,
   0.000000000000330,
  -0.000000000000052,
   0.000000000000008,
  -0.000000000000001
};


/* Chebyshev expansion: j_{nu,5} = nu c_k T_k*((5/nu)^(2/3)), nu >= 5 */
/* FIXME: There is something wrong with this fit, in about the
 * 9th or 10th decimal place.
 */
static const double coef_jnu5_b[] = {
  2.569079487591442,
  1.726073360882134,
  0.152740776809531,
 -0.004346449660148,
 -0.000030512461856,
  0.000052000821080,
 -0.000009713343981,
  0.000001091997863,
 -0.000000043061707,
 -0.000000013779413,
  0.000000004082870,
 -0.000000000611259,
  0.000000000042242,
  0.000000000005448,
 -0.000000000002377,
  0.000000000000424,
 -0.000000000000038,
 -0.000000000000002,
  0.000000000000002
};


/* Chebyshev expansion: j_{nu,6} = c_k T_k*(nu/6), nu <= 6 */
static const double coef_jnu6_a[] = {
  22.514836143374042,
   4.368367257557198,
  -0.068550155285562,
   0.006093776505822,
  -0.000667152784957,
   0.000081486022398,
  -0.000010649011647,
   0.000001457089679,
  -0.000000206105082,
   0.000000029894724,
  -0.000000004422012,
   0.000000000664471,
  -0.000000000101140,
   0.000000000015561,
  -0.000000000002416,
   0.000000000000378,
  -0.000000000000060,
   0.000000000000009,
  -0.000000000000002
};


/* Chebyshev expansion: j_{nu,6} = nu c_k T_k*((6/nu)^(2/3)), nu >= 6 */
static const double coef_jnu6_b[] = {
  2.580710285494837,
  1.739380728566154,
  0.154340696401691,
 -0.004422028860168,
 -0.000028305272624,
  0.000052845975269,
 -0.000009968794373,
  0.000001134252926,
 -0.000000046841241,
 -0.000000014007555,
  0.000000004251816,
 -0.000000000648213,
  0.000000000046728,
  0.000000000005414,
 -0.000000000002508,
  0.000000000000459,
 -0.000000000000043,
 -0.000000000000002,
  0.000000000000002
};


/* Chebyshev expansion: j_{nu,7} = c_k T_k*(nu/7), nu <= 7 */
static const double coef_jnu7_a[] = {
  26.397760539730869,
   5.098418721711790,
  -0.079761896398948,
   0.007064521280487,
  -0.000770766522482,
   0.000093835449636,
  -0.000012225308542,
   0.000001667939800,
  -0.000000235288157,
   0.000000034040347,
  -0.000000005023142,
   0.000000000753101,
  -0.000000000114389,
   0.000000000017564,
  -0.000000000002722,
   0.000000000000425,
  -0.000000000000067,
   0.000000000000011,
  -0.000000000000002
};


/* Chebyshev expansion: j_{nu,7} = nu c_k T_k*((7/nu)^(2/3)), nu >= 7 */
static const double coef_jnu7_b[] = {
  2.589033335856773,
  1.748907007612678,
  0.155488900387653,
 -0.004476317805688,
 -0.000026737952924,
  0.000053459680946,
 -0.000010153699240,
  0.000001164804272,
 -0.000000049566917,
 -0.000000014175403,
  0.000000004374840,
 -0.000000000675135,
  0.000000000050004,
  0.000000000005387,
 -0.000000000002603,
  0.000000000000485,
 -0.000000000000047,
 -0.000000000000002,
  0.000000000000002
};


/* Chebyshev expansion: j_{nu,8} = c_k T_k*(nu/8), nu <= 8 */
static const double coef_jnu8_a[] = {
  30.280900001606662,
   5.828429205461221,
  -0.090968381181069,
   0.008034479731033,
  -0.000874254899080,
   0.000106164151611,
  -0.000013798098749,
   0.000001878187386,
  -0.000000264366627,
   0.000000038167685,
  -0.000000005621060,
   0.000000000841165,
  -0.000000000127538,
   0.000000000019550,
  -0.000000000003025,
   0.000000000000472,
  -0.000000000000074,
   0.000000000000012,
  -0.000000000000002
};


/* Chebyshev expansion: j_{nu,8} = nu c_k T_k*((8/nu)^(2/3)), nu >= 8 */
static const double coef_jnu8_b[] = {
  2.595283877150078,
  1.756063044986928,
  0.156352972371030,
 -0.004517201896761,
 -0.000025567187878,
  0.000053925472558,
 -0.000010293734486,
  0.000001187923085,
 -0.000000051625122,
 -0.000000014304212,
  0.000000004468450,
 -0.000000000695620,
  0.000000000052500,
  0.000000000005367,
 -0.000000000002676,
  0.000000000000505,
 -0.000000000000050,
 -0.000000000000002,
  0.000000000000002
};


/* Chebyshev expansion: j_{nu,9} = c_k T_k*(nu/9), nu <= 9 */
static const double coef_jnu9_a[] = {
  34.164181213238386,
   6.558412747925228,
  -0.102171455365016,
   0.009003934361201,
  -0.000977663914535,
   0.000118479876579,
  -0.000015368714220,
   0.000002088064285,
  -0.000000293381154,
   0.000000042283900,
  -0.000000006217033,
   0.000000000928887,
  -0.000000000140627,
   0.000000000021526,
  -0.000000000003326,
   0.000000000000518,
  -0.000000000000081,
   0.000000000000013,
  -0.000000000000002
};


/* Chebyshev expansion: j_{nu,9} = nu c_k T_k*((9/nu)^(2/3)), nu >= 9 */
static const double coef_jnu9_b[] = {
  2.600150240905079,
  1.761635491694032,
  0.157026743724010,
 -0.004549100368716,
 -0.000024659248617,
  0.000054291035068,
 -0.000010403464334,
  0.000001206027524,
 -0.000000053234089,
 -0.000000014406241,
  0.000000004542078,
 -0.000000000711728,
  0.000000000054464,
  0.000000000005350,
 -0.000000000002733,
  0.000000000000521,
 -0.000000000000052,
 -0.000000000000002,
  0.000000000000002
};


/* Chebyshev expansion: j_{nu,10} = c_k T_k*(nu/10), nu <= 10 */
static const double coef_jnu10_a[] = {
  38.047560766184647,
   7.288377637926008,
  -0.113372193277897,
   0.009973047509098,
  -0.001081019701335,
   0.000130786983847,
  -0.000016937898538,
   0.000002297699179,
  -0.000000322354218,
   0.000000046392941,
  -0.000000006811759,
   0.000000001016395,
  -0.000000000153677,
   0.000000000023486,
  -0.000000000003616,
   0.000000000000561,
  -0.000000000000095,
   0.000000000000027,
  -0.000000000000013,
   0.000000000000005
};


/* Chebyshev expansion: j_{nu,10} = nu c_k T_k*((10/nu)^(2/3)), nu >= 10 */
static const double coef_jnu10_b[] = {
  2.604046346867949,
  1.766097596481182,
  0.157566834446511,
 -0.004574682244089,
 -0.000023934500688,
  0.000054585558231,
 -0.000010491765415,
  0.000001220589364,
 -0.000000054526331,
 -0.000000014489078,
  0.000000004601510,
 -0.000000000724727,
  0.000000000056049,
  0.000000000005337,
 -0.000000000002779,
  0.000000000000533,
 -0.000000000000054,
 -0.000000000000002,
  0.000000000000002
};


/* Chebyshev expansion: j_{nu,11} = c_k T_k*(nu/22), nu <= 22 */
static const double coef_jnu11_a[] = {
  49.5054081076848637,
  15.33692279367165101,
 -0.33677234163517130,
  0.04623235772920729,
 -0.00781084960665093,
  0.00147217395434708,
 -0.00029695043846867,
  0.00006273356860235,
 -0.00001370575125628,
  3.07171282012e-6,
 -7.0235041249e-7,
  1.6320559339e-7,
 -3.843117306e-8,
  9.15083800e-9,
 -2.19957642e-9,
  5.3301703e-10,
 -1.3007541e-10,
  3.193827e-11,
 -7.88605e-12,
  1.95918e-12,
 -4.9020e-13,
  1.2207e-13,
 -2.820e-14,
  5.25e-15,
 -1.88e-15,
  2.80e-15,
 -2.45e-15
};


/* Chebyshev expansion: j_{nu,12} = c_k T_k*(nu/24), nu <= 24 */
static const double coef_jnu12_a[] = {
  54.0787833216641519,
  16.7336367772863598,
 -0.36718411124537953,
  0.05035523375053820,
 -0.00849884978867533,
  0.00160027692813434,
 -0.00032248114889921,
  0.00006806354127199,
 -0.00001485665901339,
  3.32668783672e-6,
 -7.5998952729e-7,
  1.7644939709e-7,
 -4.151538210e-8,
  9.87722772e-9,
 -2.37230133e-9,
  5.7442875e-10,
 -1.4007767e-10,
  3.437166e-11,
 -8.48215e-12,
  2.10554e-12,
 -5.2623e-13,
  1.3189e-13,
 -3.175e-14,
  5.73e-15,
  5.6e-16,
 -8.7e-16,
 -6.5e-16
};


/* Chebyshev expansion: j_{nu,13} = c_k T_k*(nu/26), nu <= 26 */
static const double coef_jnu13_a[] = {
  58.6521941921708890,
  18.1303398137970284,
 -0.39759381380126650,
  0.05447765240465494,
 -0.00918674227679980,
  0.00172835361420579,
 -0.00034800528297612,
  0.00007339183835188,
 -0.00001600713368099,
  3.58154960392e-6,
 -8.1759873497e-7,
  1.8968523220e-7,
 -4.459745253e-8,
  1.060304419e-8,
 -2.54487624e-9,
  6.1580214e-10,
 -1.5006751e-10,
  3.679707e-11,
 -9.07159e-12,
  2.24713e-12,
 -5.5943e-13,
  1.4069e-13,
 -3.679e-14,
  1.119e-14,
 -4.99e-15,
  3.43e-15,
 -2.85e-15,
  2.3e-15,
 -1.7e-15,
  8.7e-16
};


/* Chebyshev expansion: j_{nu,14} = c_k T_k*(nu/28), nu <= 28 */
static const double coef_jnu14_a[] = {
  63.2256329577315566,
  19.5270342832914901,
 -0.42800190567884337,
  0.05859971627729398,
 -0.00987455163523582,
  0.00185641011402081,
 -0.00037352439419968,
  0.00007871886257265,
 -0.00001715728110045,
  3.83632624437e-6,
 -8.7518558668e-7,
  2.0291515353e-7,
 -4.767795233e-8,
  1.132844415e-8,
 -2.71734219e-9,
  6.5714886e-10,
 -1.6005342e-10,
  3.922557e-11,
 -9.66637e-12,
  2.39379e-12,
 -5.9541e-13,
  1.4868e-13,
 -3.726e-14,
  9.37e-15,
 -2.36e-15,
  6.0e-16
};


/* Chebyshev expansion: j_{nu,15} = c_k T_k*(nu/30), nu <= 30 */
static const double coef_jnu15_a[] = {
  67.7990939565631635,
  20.9237219226859859,
 -0.45840871823085836,
  0.06272149946755639,
 -0.01056229551143042,
  0.00198445078693100,
 -0.00039903958650729,
  0.00008404489865469,
 -0.00001830717574922,
  4.09103745566e-6,
 -9.3275533309e-7,
  2.1614056403e-7,
 -5.075725222e-8,
  1.205352081e-8,
 -2.88971837e-9,
  6.9846848e-10,
 -1.7002946e-10,
  4.164941e-11,
 -1.025859e-11,
  2.53921e-12,
 -6.3128e-13,
  1.5757e-13,
 -3.947e-14,
  9.92e-15,
 -2.50e-15,
  6.3e-16
};


/* Chebyshev expansion: j_{nu,16} = c_k T_k*(nu/32), nu <= 32 */
static const double coef_jnu16_a[] = {
  72.3725729616724770,
  22.32040402918608585,
 -0.48881449782358690,
  0.06684305681828766,
 -0.01124998690363398,
  0.00211247882775445,
 -0.00042455166484632,
  0.00008937015316346,
 -0.00001945687139551,
  4.34569739281e-6,
 -9.9031173548e-7,
  2.2936247195e-7,
 -5.383562595e-8,
  1.277835103e-8,
 -3.06202860e-9,
  7.3977037e-10,
 -1.8000071e-10,
  4.407196e-11,
 -1.085046e-11,
  2.68453e-12,
 -6.6712e-13,
  1.6644e-13,
 -4.168e-14,
  1.047e-14,
 -2.64e-15,
  6.7e-16
};


/* Chebyshev expansion: j_{nu,17} = c_k T_k*(nu/34), nu <= 34 */
static const double coef_jnu17_a[] = {
  76.9460667535209549,
  23.71708159112252670,
 -0.51921943142405352,
  0.07096442978067622,
 -0.01193763559341369,
  0.00224049662974902,
 -0.00045006122941781,
  0.00009469477941684,
 -0.00002060640777107,
  4.60031647195e-6,
 -1.04785755046e-6,
  2.4258161247e-7,
 -5.691327087e-8,
  1.350298805e-8,
 -3.23428733e-9,
  7.8105847e-10,
 -1.8996825e-10,
  4.649350e-11,
 -1.144205e-11,
  2.82979e-12,
 -7.0294e-13,
  1.7531e-13,
 -4.388e-14,
  1.102e-14,
 -2.78e-15,
  7.0e-16
};


/* Chebyshev expansion: j_{nu,18} = c_k T_k*(nu/36), nu <= 36 */
static const double coef_jnu18_a[] = {
  81.5195728368096659,
  25.11375537470259305,
 -0.54962366347317668,
  0.07508565026117689,
 -0.01262524908033818,
  0.00236850602019778,
 -0.00047556873651929,
  0.00010001889347161,
 -0.00002175581482429,
  4.85490251239e-6,
 -1.10539483940e-6,
  2.5579853343e-7,
 -5.999033352e-8,
  1.422747129e-8,
 -3.40650521e-9,
  8.2233565e-10,
 -1.9993286e-10,
  4.891426e-11,
 -1.203343e-11,
  2.97498e-12,
 -7.3875e-13,
  1.8418e-13,
 -4.608e-14,
  1.157e-14,
 -2.91e-15,
  7.4e-16
};


/* Chebyshev expansion: j_{nu,19} = c_k T_k*(nu/38), nu <= 38 */
static const double coef_jnu19_a[] = {
  86.0930892477047512,
  26.51042598308271729,
 -0.58002730731948358,
  0.07920674321589394,
 -0.01331283320930301,
  0.00249650841778073,
 -0.00050107453900793,
  0.00010534258471335,
 -0.00002290511552874,
  5.10946148897e-6,
 -1.16292517157e-6,
  2.6901365037e-7,
 -6.306692473e-8,
  1.495183048e-8,
 -3.57869025e-9,
  8.6360410e-10,
 -2.0989514e-10,
  5.133439e-11,
 -1.262465e-11,
  3.12013e-12,
 -7.7455e-13,
  1.9304e-13,
 -4.829e-14,
  1.212e-14,
 -3.05e-15,
  7.7e-16
};


/* Chebyshev expansion: j_{nu,20} = c_k T_k*(nu/40), nu <= 40 */
static const double coef_jnu20_a[] = {
  90.6666144195163770,
  27.9070938975436823,
 -0.61043045315390591,
  0.08332772844325554,
 -0.01400039260208282,
  0.00262450494035660,
 -0.00052657891389470,
  0.00011066592304919,
 -0.00002405432778364,
  5.36399803946e-6,
 -1.22044976064e-6,
  2.8222728362e-7,
 -6.614312964e-8,
  1.567608839e-8,
 -3.75084856e-9,
  9.0486546e-10,
 -2.1985553e-10,
  5.375401e-11,
 -1.321572e-11,
  3.26524e-12,
 -8.1033e-13,
  2.0190e-13,
 -5.049e-14,
  1.267e-14,
 -3.19e-15,
  8.0e-16,
 -2.0e-16
};


static const double * coef_jnu_a[] = {
  0,
  coef_jnu1_a,
  coef_jnu2_a,
  coef_jnu3_a,
  coef_jnu4_a,
  coef_jnu5_a,
  coef_jnu6_a,
  coef_jnu7_a,
  coef_jnu8_a,
  coef_jnu9_a,
  coef_jnu10_a,
  coef_jnu11_a,
  coef_jnu12_a,
  coef_jnu13_a,
  coef_jnu14_a,
  coef_jnu15_a,
  coef_jnu16_a,
  coef_jnu17_a,
  coef_jnu18_a,
  coef_jnu19_a,
  coef_jnu20_a
};

static const size_t size_jnu_a[] = {
  0,
  sizeof(coef_jnu1_a)/sizeof(double),
  sizeof(coef_jnu2_a)/sizeof(double),
  sizeof(coef_jnu3_a)/sizeof(double),
  sizeof(coef_jnu4_a)/sizeof(double),
  sizeof(coef_jnu5_a)/sizeof(double),
  sizeof(coef_jnu6_a)/sizeof(double),
  sizeof(coef_jnu7_a)/sizeof(double),
  sizeof(coef_jnu8_a)/sizeof(double),
  sizeof(coef_jnu9_a)/sizeof(double),
  sizeof(coef_jnu10_a)/sizeof(double),
  sizeof(coef_jnu11_a)/sizeof(double),
  sizeof(coef_jnu12_a)/sizeof(double),
  sizeof(coef_jnu13_a)/sizeof(double),
  sizeof(coef_jnu14_a)/sizeof(double),
  sizeof(coef_jnu15_a)/sizeof(double),
  sizeof(coef_jnu16_a)/sizeof(double),
  sizeof(coef_jnu17_a)/sizeof(double),
  sizeof(coef_jnu18_a)/sizeof(double),
  sizeof(coef_jnu19_a)/sizeof(double),
  sizeof(coef_jnu20_a)/sizeof(double)
};


static const double * coef_jnu_b[] = {
  0,
  coef_jnu1_b,
  coef_jnu2_b,
  coef_jnu3_b,
  coef_jnu4_b,
  coef_jnu5_b,
  coef_jnu6_b,
  coef_jnu7_b,
  coef_jnu8_b,
  coef_jnu9_b,
  coef_jnu10_b
};

static const size_t size_jnu_b[] = {
  0,
  sizeof(coef_jnu1_b)/sizeof(double),
  sizeof(coef_jnu2_b)/sizeof(double),
  sizeof(coef_jnu3_b)/sizeof(double),
  sizeof(coef_jnu4_b)/sizeof(double),
  sizeof(coef_jnu5_b)/sizeof(double),
  sizeof(coef_jnu6_b)/sizeof(double),
  sizeof(coef_jnu7_b)/sizeof(double),
  sizeof(coef_jnu8_b)/sizeof(double),
  sizeof(coef_jnu9_b)/sizeof(double),
  sizeof(coef_jnu10_b)/sizeof(double)
};



/* Evaluate Clenshaw recurrence for
 * a T* Chebyshev series.
 * sizeof(c) = N+1
 */
static double
clenshaw(const double * c, int N, double u)
{
  double B_np1 = 0.0;
  double B_n   = c[N];
  double B_nm1;
  int n;
  for(n=N; n>0; n--) {
    B_nm1 = 2.0*(2.0*u-1.0) * B_n - B_np1 + c[n-1];
    B_np1 = B_n;
    B_n   = B_nm1;
  }
  return B_n - (2.0*u-1.0)*B_np1;
}



/* correction terms to leading McMahon expansion
 * [Abramowitz+Stegun 9.5.12]
 * [Olver, Royal Society Math. Tables, v. 7]
 * We factor out a beta, so that this is a multiplicative
 * correction:
 *   j_{nu,s} = beta(s,nu) * mcmahon_correction(nu, beta(s,nu))
 *   macmahon_correction --> 1 as s --> Inf
 */
static double
mcmahon_correction(const double mu, const double beta)
{
  const double eb   = 8.0*beta;
  const double ebsq = eb*eb;

  if(mu < GSL_DBL_EPSILON) {
    /* Prevent division by zero below. */
    const double term1 =  1.0/ebsq;
    const double term2 = -4.0*31.0/(3*ebsq*ebsq);
    const double term3 =  32.0*3779.0/(15.0*ebsq*ebsq*ebsq);
    const double term4 = -64.0*6277237.0/(105.0*ebsq*ebsq*ebsq*ebsq);
    const double term5 =  512.0*2092163573.0/(315.0*ebsq*ebsq*ebsq*ebsq*ebsq);
    return 1.0 + 8.0*(term1 + term2 + term3 + term4 + term5);
  }
  else {
    /* Here we do things in terms of 1/mu, which
     * is purely to prevent overflow in the very
     * unlikely case that mu is really big.
     */
    const double mi   = 1.0/mu;
    const double r  = mu/ebsq;
    const double n2 = 4.0/3.0    * (7.0 - 31.0*mi);
    const double n3 = 32.0/15.0  * (83.0 + (-982.0 + 3779.0*mi)*mi);
    const double n4 = 64.0/105.0 * (6949.0 + (-153855.0 + (1585743.0 - 6277237.0*mi)*mi)*mi);
    const double n5 = 512.0/315.0 * (70197.0 + (-2479316.0 + (48010494.0 + (-512062548.0 + 2092163573.0*mi)*mi)*mi)*mi);
    const double n6 = 2048.0/3465.0 * (5592657.0 + (-287149133.0 + (8903961290.0 + (-179289628602.0 + (1982611456181.0 - 8249725736393.0*mi)*mi)*mi)*mi)*mi);
    const double term1 = (1.0 - mi) * r;
    const double term2 = term1 * n2 * r;
    const double term3 = term1 * n3 * r*r;
    const double term4 = term1 * n4 * r*r*r;
    const double term5 = term1 * n5 * r*r*r*r;
    const double term6 = term1 * n6 * r*r*r*r*r;
    return 1.0 - 8.0*(term1 + term2 + term3 + term4 + term5 + term6);
  }
}


/* Assumes z >= 1.0 */
static double
olver_b0(double z, double minus_zeta)
{
  if(z < 1.02) {
    const double a = 1.0-z;
    const double c0 =  0.0179988721413553309252458658183;
    const double c1 =  0.0111992982212877614645974276203;
    const double c2 =  0.0059404069786014304317781160605;
    const double c3 =  0.0028676724516390040844556450173;
    const double c4 =  0.0012339189052567271708525111185;
    const double c5 =  0.0004169250674535178764734660248;
    const double c6 =  0.0000330173385085949806952777365;
    const double c7 = -0.0001318076238578203009990106425;
    const double c8 = -0.0001906870370050847239813945647;
    return c0 + a*(c1 + a*(c2 + a*(c3 + a*(c4 + a*(c5 + a*(c6 + a*(c7 + a*c8)))))));
  }
  else {
    const double abs_zeta = minus_zeta;
    const double t = 1.0/(z*sqrt(1.0 - 1.0/(z*z)));
    return -5.0/(48.0*abs_zeta*abs_zeta) + t*(3.0 + 5.0*t*t)/(24.0*sqrt(abs_zeta));
  }
}


inline
static double
olver_f1(double z, double minus_zeta)
{
  const double b0 = olver_b0(z, minus_zeta);
  const double h2 = sqrt(4.0*minus_zeta/(z*z-1.0)); /* FIXME */
  return 0.5 * z * h2 * b0;
}


int
gsl_sf_bessel_zero_J0_e(unsigned int s, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(s == 0){
    result->val = 0.0;
    result->err = 0.0;
    GSL_ERROR ("error", GSL_EINVAL);
  }
  else {
    /* See [F. Lether, J. Comp. Appl .Math. 67, 167 (1996)]. */

    static const double P[] = { 1567450796.0/12539606369.0,
                                8903660.0/2365861.0,
                                10747040.0/536751.0,
                                17590991.0/1696654.0
                              };
    static const double Q[] = { 1.0,
                                29354255.0/954518.0,
                                76900001.0/431847.0,
                                67237052.0/442411.0
                              };

    const double beta = (s - 0.25) * M_PI;
    const double bi2  = 1.0/(beta*beta);
    const double R33num = P[0] + bi2 * (P[1] + bi2 * (P[2] + P[3] * bi2));
    const double R33den = Q[0] + bi2 * (Q[1] + bi2 * (Q[2] + Q[3] * bi2));
    const double R33 = R33num/R33den;
    result->val = beta + R33/beta;
    result->err = fabs(3.0e-15 * result->val);
    return GSL_SUCCESS;
  }
}


int
gsl_sf_bessel_zero_J1_e(unsigned int s, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(s == 0) {
    result->val = 0.0;
    result->err = 0.0;
    return GSL_SUCCESS;
  }
  else {
    /* See [M. Branders et al., J. Comp. Phys. 42, 403 (1981)]. */

    static const double a[] = { -0.362804405737084,
                                 0.120341279038597,
                                 0.439454547101171e-01,
                                 0.159340088474713e-02
                              };
    static const double b[] = {  1.0,
                                -0.325641790801361,
                                -0.117453445968927,
                                -0.424906902601794e-02
                              };

    const double beta = (s + 0.25) * M_PI;
    const double bi2  = 1.0/(beta*beta);
    const double Rnum = a[3] + bi2 * (a[2] + bi2 * (a[1] + bi2 * a[0]));
    const double Rden = b[3] + bi2 * (b[2] + bi2 * (b[1] + bi2 * b[0]));
    const double R = Rnum/Rden;
    result->val = beta * (1.0 + R*bi2);
    result->err = fabs(2.0e-14 * result->val);
    return GSL_SUCCESS;
  }
}


int
gsl_sf_bessel_zero_Jnu_e(double nu, unsigned int s, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(nu <= -1.0) {
    DOMAIN_ERROR(result);
  }
  else if(s == 0) {
    result->val = 0.0;
    result->err = 0.0;
    if (nu == 0.0) {
      GSL_ERROR ("no zero-th root for nu = 0.0", GSL_EINVAL);
    }
    return GSL_SUCCESS;
  }
  else if(nu < 0.0) {
    /* This can be done, I'm just lazy now. */
    result->val = 0.0;
    result->err = 0.0;
    GSL_ERROR("unimplemented", GSL_EUNIMPL);
  }
  else if(s == 1) {
    /* Chebyshev fits for the first positive zero.
     * For some reason Nemeth made this different from the others.
     */
    if(nu < 2.0) {
      const double * c = coef_jnu_a[s];
      const size_t   L = size_jnu_a[s];
      const double arg = nu/2.0;
      const double chb = clenshaw(c, L-1, arg);
      result->val = chb;
      result->err = 2.0e-15 * result->val;
    }
    else {
      const double * c = coef_jnu_b[s];
      const size_t   L = size_jnu_b[s];
      const double arg = pow(2.0/nu, 2.0/3.0);
      const double chb = clenshaw(c, L-1, arg);
      result->val = nu * chb;
      result->err = 2.0e-15 * result->val;
    }
    return GSL_SUCCESS;
  }
  else if(s <= 10) {
    /* Chebyshev fits for the first 10 positive zeros. */
    if(nu < s) {
      const double * c = coef_jnu_a[s];
      const size_t   L = size_jnu_a[s];
      const double arg = nu/s;
      const double chb = clenshaw(c, L-1, arg);
      result->val = chb;
      result->err = 2.0e-15 * result->val;
    }
    else {
      const double * c = coef_jnu_b[s];
      const size_t   L = size_jnu_b[s];
      const double arg = pow(s/nu, 2.0/3.0);
      const double chb = clenshaw(c, L-1, arg);
      result->val = nu * chb;
      result->err = 2.0e-15 * result->val;

      /* FIXME: truth in advertising for the screwed up
       * s = 5 fit. Need to fix that.
       */
      if(s == 5) {
        result->err *= 5.0e+06;
      }
    }
    return GSL_SUCCESS;
  }
  else if(s > 0.5*nu && s <= 20) {
    /* Chebyshev fits for 10 < s <= 20. */
    const double * c = coef_jnu_a[s];
    const size_t   L = size_jnu_a[s];
    const double arg = nu/(2.0*s);
    const double chb = clenshaw(c, L-1, arg);
    result->val = chb;
    result->err = 4.0e-15 * chb;
    return GSL_SUCCESS;
  }
  else if(s > 2.0 * nu) {
    /* McMahon expansion if s is large compared to nu. */
    const double beta = (s + 0.5*nu - 0.25) * M_PI;
    const double mc   = mcmahon_correction(4.0*nu*nu, beta);
    gsl_sf_result rat12;
    gsl_sf_pow_int_e(nu/beta, 14, &rat12);
    result->val  = beta * mc;
    result->err  = 4.0 * fabs(beta) * rat12.val;
    result->err += 4.0 * fabs(GSL_DBL_EPSILON * result->val);
    return GSL_SUCCESS;
  }
  else {
    /* Olver uniform asymptotic. */
    gsl_sf_result as;
    const int stat_as = gsl_sf_airy_zero_Ai_e(s, &as);
    const double minus_zeta = -pow(nu,-2.0/3.0) * as.val;
    const double z  = gsl_sf_bessel_Olver_zofmzeta(minus_zeta);
    const double f1 = olver_f1(z, minus_zeta);
    result->val  = nu * (z + f1/(nu*nu));
    result->err  = 0.001/(nu*nu*nu);
    result->err += 2.0 * GSL_DBL_EPSILON * fabs(result->val);
    return stat_as;
  }
}


/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

#include "gsl_specfunc__eval.h"

double gsl_sf_bessel_zero_J0(unsigned int s)
{
  EVAL_RESULT(gsl_sf_bessel_zero_J0_e(s, &result));
}

double gsl_sf_bessel_zero_J1(unsigned int s)
{
  EVAL_RESULT(gsl_sf_bessel_zero_J1_e(s, &result));
}

double gsl_sf_bessel_zero_Jnu(double nu, unsigned int s)
{
  EVAL_RESULT(gsl_sf_bessel_zero_Jnu_e(nu, s, &result));
}
