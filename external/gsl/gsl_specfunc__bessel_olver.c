/* specfunc/bessel_olver.c
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

#include "gsl_specfunc__error.h"

#include "gsl_specfunc__bessel.h"
#include "gsl_specfunc__bessel_olver.h"

#include "gsl_specfunc__chebyshev.h"
#include "gsl_specfunc__cheb_eval.c"

/* fit for f(x) = zofmzeta((x+1)/2), 0 <= mzeta <= 1 */
static double zofmzeta_a_data[20] = {
  2.9332563730829348990,
  0.4896518224847036624,
  0.0228637617355380860,
 -0.0001715731377284693,
 -0.0000105927538148751,
  1.0595602530419e-6,
 -4.68016051691e-8,
  5.8310020e-12,
  1.766537581e-10,
 -1.45034640e-11,
  4.357772e-13,
  4.60971e-14,
 -2.57571e-14,
  2.26468e-14,
 -2.22053e-14,
  2.08593e-14,
 -1.84454e-14,
  1.50150e-14,
 -1.06506e-14,
  5.5375e-15
};
static cheb_series zofmzeta_a_cs = {
  zofmzeta_a_data,
  19,
  -1,1,
  8
};


/* fit for f(x) = zofmzeta((9x+11)/2), 1 <= mzeta <= 10 */
static double zofmzeta_b_data[30] = {
  22.40725276466303489,
  10.39808258825165581,
  1.092050144486018425,
 -0.071111274777921604,
  0.008990125336059704,
 -0.001201950338088875,
  0.000106686807968315,
  0.000017406491576830,
 -0.000014946669657805,
  6.189984487752e-6,
 -2.049466715178e-6,
  5.87189458020e-7,
 -1.46077514157e-7,
  2.9803936132e-8,
 -3.817692108e-9,
 -4.66980416e-10,
  5.83860334e-10,
 -2.78825299e-10,
  1.01682688e-10,
 -3.1209928e-11,
  8.111122e-12,
 -1.663986e-12,
  1.81364e-13,
  5.3414e-14,
 -4.7234e-14,
  2.1689e-14,
 -7.815e-15,
  2.371e-15,
 -6.04e-16,
  1.20e-16
};
static cheb_series zofmzeta_b_cs = {
  zofmzeta_b_data,
  29,
  -1,1,
  15
};


/* fit for f(x) = zofmzeta(mz(x))/mz(x)^(3/2),
 * mz(x) = (2/(x+1))^(2/3) 10
 * 10 <= mzeta <= Inf
 */
static double zofmzeta_c_data[11] = {
  1.3824761227122911500,
  0.0244856101686774245,
 -0.0000842866496282540,
  1.4656076569771e-6,
 -3.14874099476e-8,
  7.561134833e-10,
 -1.94531643e-11,
  5.245878e-13,
 -1.46380e-14,
  4.192e-16,
 -1.23e-17
};
static cheb_series zofmzeta_c_cs = {
  zofmzeta_c_data,
  10,
  -1,1,
  6
};


/* Invert [Abramowitz+Stegun, 9.3.39].
 * Assumes minus_zeta >= 0.
 */
double
gsl_sf_bessel_Olver_zofmzeta(double minus_zeta)
{
  if(minus_zeta < 1.0) {
    const double x = 2.0*minus_zeta - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&zofmzeta_a_cs, x, &c);
    return c.val;
  }
  else if(minus_zeta < 10.0) {
    const double x = (2.0*minus_zeta - 11.0)/9.0;
    gsl_sf_result c;
    cheb_eval_e(&zofmzeta_b_cs, x, &c);
    return c.val;
  }
  else {
    const double TEN_32 = 31.62277660168379332; /* 10^(3/2) */
    const double p = pow(minus_zeta, 3.0/2.0);
    const double x = 2.0*TEN_32/p - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&zofmzeta_c_cs, x, &c);
    return c.val * p;
  }
}


/* Chebyshev fit for f(x) = z(x)^6 A_3(z(x)),  z(x) = 22/(10(x+1)) */
static double A3_gt1_data[31] = {
  -0.123783199829515294670493131190,
   0.104636462534700704670877382304,
  -0.067500816575851826744877535903,
   0.035563362418888483652711005520,
  -0.0160738524035979408472979609051,
   0.0064497878252851092073278056238,
  -0.00235408261133449663958121821593,
   0.00079545702851302155411892534965,
  -0.00025214920745855079895784825637,
   0.00007574004596069392921153301833,
  -0.00002172917966339623434407978263,
   5.9914810727868915476543145465e-06,
  -1.5958781571808992162953719817e-06,
   4.1232986512903717525448312012e-07,
  -1.0369725993417659101913919101e-07,
   2.5457982304266541145999235022e-08,
  -6.1161715053791743082427422443e-09,
   1.4409346199138658887871461320e-09,
  -3.3350445956255561668232014995e-10,
   7.5950686572918996453336138108e-11,
  -1.7042296334409430377389900278e-11,
   3.7723525020626230919721640081e-12,
  -8.2460237635733980528416501227e-13,
   1.7816961527997797696251868875e-13,
  -3.8084101506541792942694560802e-14,
   8.0593669930916099079755351563e-15,
  -1.6896565961641739017452636964e-15,
   3.5115651805888443184822853595e-16,
  -7.2384771938569255638904297651e-17,
   1.4806598977677176106283840244e-17,
  -3.0069285750787303634897997963e-18
};
static cheb_series A3_gt1_cs = {
  A3_gt1_data,
  30,
  -1,1,
  17
};

/* chebyshev expansion for f(x) = z(x)^8 A_4(z(x)), z(x) = 12/(5(x+1)) */
static double A4_gt1_data[30] = {
  1.15309329391198493586724229008,
 -1.01812701728669338904729927846,
  0.71964022270555684403652781941,
 -0.42359963977172689685150061355,
  0.215024488759339557817435404261,
 -0.096751915348145944032096342479,
  0.039413982058824310099856035361,
 -0.014775225692561697963781115014,
  0.005162114514159370516947823271,
 -0.00169783446445524322560925166335,
  0.00052995667873006847211519193478,
 -0.00015802027574996477115667974856,
  0.000045254366680989687988902825193,
 -0.000012503722965474638015488600967,
  3.3457656998119148699124716204e-06,
 -8.6981575241150758412492331833e-07,
  2.2030895484325645640823940625e-07,
 -5.4493369492600677068285936533e-08,
  1.3190457281724829107139385556e-08,
 -3.1301560183377379158951191769e-09,
  7.2937802527123344842593076131e-10,
 -1.6712080137945140407348940109e-10,
  3.7700053248213600430503521194e-11,
 -8.3824538848817227637828899571e-12,
  1.8388741910049766865274037194e-12,
 -3.9835919980753778560117573063e-13,
  8.5288827136546615604290389711e-14,
 -1.8060227869114416998653266836e-14,
  3.7849342199690728470461022877e-15,
 -7.8552867468122209577151823365e-16
};
static cheb_series A4_gt1_cs = {
  A4_gt1_data,
  17, /* 29, */
  -1, 1,
  17
};

/* Chebyshev fit for f(x) = z(x)^3 B_2(z(x)), z(x) = 12/(5(x+1)) */
static double B2_gt1_data[40] = {
  0.00118587147272683864479328868589,
  0.00034820459990648274622193981840,
 -0.00030411304425639768103075864567,
  0.00002812066284012343531484682886,
  0.00004493525295901613184489898748,
 -0.00003037629997093072196779489677,
  0.00001125979647123875721949743970,
 -2.4832533969517775991951008218e-06,
 -9.9003813640537799587086928278e-08,
  4.9259859656183110299492296029e-07,
 -3.7644120964426705960749504975e-07,
  2.2887828521334625189639122509e-07,
 -1.3202687370822203731489855050e-07,
  7.7019669092537400811434860763e-08,
 -4.6589706973010511603890144294e-08,
  2.9396476233013923711978522963e-08,
 -1.9293230611988282919101954538e-08,
  1.3099107013728717842406906896e-08,
 -9.1509111940885962831104149355e-09,
  6.5483472971925614347299375295e-09,
 -4.7831253582139967461241674569e-09,
  3.5562625457426178152760148639e-09,
 -2.6853389444008414186916562103e-09,
  2.0554738667134200145781857289e-09,
 -1.5923172019517426277886522758e-09,
  1.2465923213464381457319481498e-09,
 -9.8494846881180588507969988989e-10,
  7.8438674499372126663957464312e-10,
 -6.2877567918342950225937136855e-10,
  5.0662318868755257959686944117e-10,
 -4.0962270881243451160378710952e-10,
  3.3168684677374908553161911299e-10,
 -2.6829406619847450633596163305e-10,
  2.1603988122184568375561077873e-10,
 -1.7232373309560278402012124481e-10,
  1.3512709089611470626617830434e-10,
 -1.0285354732538663013167579792e-10,
  7.4211345443901713467637018423e-11,
 -4.8124980266864320351456993068e-11,
  2.3666534694476306077416831958e-11
};
static cheb_series B2_gt1_cs = {
  B2_gt1_data,
  39,
  -1, 1,
  30
};


/* Chebyshev fit for f(x) = z(x)^6 B_3(z(x)), z(x) = 12/(5(x+1)) */
static double B3_gt1_data[30] = {
 -0.0102445379362695740863663926486,
  0.0036618484329295342954730801917,
  0.0026154252498599303282569321117,
 -0.0036187389410353156728771706336,
  0.0021878564157692275944613452462,
 -0.0008219952303590803584426516821,
  0.0001281773889155631494321316520,
  0.0001000944653368032985720548637,
 -0.0001288293344663774273453147788,
  0.00010136264202696513867821487205,
 -0.00007000275849659556221916572733,
  0.00004694886396757430431607955146,
 -0.00003190003869717837686356945696,
  0.00002231453668447775219665947479,
 -0.00001611102197712439539300336438,
  0.00001196634424990735214466633513,
 -9.0986920398931223804111374679e-06,
  7.0492613694235423068926562567e-06,
 -5.5425216624642184684300615394e-06,
  4.4071884714230296614449244106e-06,
 -3.5328595506791663127928952625e-06,
  2.84594975572077091520522824686e-06,
 -2.29592697828824392391071619788e-06,
  1.84714740375289956396370322228e-06,
 -1.47383331248116454652025598620e-06,
  1.15687781098593231076084710267e-06,
 -8.8174688524627071175315084910e-07,
  6.3705856964426840441434605593e-07,
 -4.1358791499961929237755474814e-07,
  2.0354151158738819867477996807e-07
};
static cheb_series B3_gt1_cs = {
  B3_gt1_data,
  29,
  -1, 1,
  29
};


/* Chebyshev fit for f(x) = z(x) B_2(z(x)), z(x) = 2(x+1)/5 */
static double B2_lt1_data[40] = {
  0.00073681565841337130021924199490,
  0.00033803599647571227535304316937,
 -0.00008251723219239754024210552679,
 -0.00003390879948656432545900779710,
  0.00001961398056848881816694014889,
 -2.35593745904151401624656805567e-06,
 -1.79055017080406086541563835433e-06,
  1.33129571185610681090725934031e-06,
 -5.38879444715436544130673956170e-07,
  1.49603056041381416881299945557e-07,
 -1.83377228267274327911131293091e-08,
 -1.33191430762944336526965187651e-08,
  1.60642096463700438411396889489e-08,
 -1.28932576330421806740136816643e-08,
  9.6169275086179165484403221944e-09,
 -7.1818502280703532276832887290e-09,
  5.4744009217215145730697754561e-09,
 -4.2680446690508456935030086136e-09,
  3.3941665009266174865683284781e-09,
 -2.7440714072221673882163135170e-09,
  2.2488361522108255229193038962e-09,
 -1.8638240716608748862087923337e-09,
  1.5592350940805373500866440401e-09,
 -1.3145743937732330609242633070e-09,
  1.1153716777215047842790244968e-09,
 -9.5117576805266622854647303110e-10,
  8.1428799553234876296804561100e-10,
 -6.9893770813548773664326279169e-10,
  6.0073113636087448745018831981e-10,
 -5.1627434258513453901420776514e-10,
  4.4290993195074905891788459756e-10,
 -3.7852978599966867611179315200e-10,
  3.2143959338863177145307610452e-10,
 -2.7025926680620777594992221143e-10,
  2.2384857772457918539228234321e-10,
 -1.8125071664276678046551271701e-10,
  1.4164870008713668767293008546e-10,
 -1.0433101857132782485813325981e-10,
  6.8663910168392483929411418190e-11,
 -3.4068313177952244040559740439e-11
};
static cheb_series B2_lt1_cs = {
  B2_lt1_data,
  39,
  -1, 1,
  39
};


/* Chebyshev fit for f(x) = B_3(2(x+1)/5) */
static double B3_lt1_data[40] = {
 -0.00137160820526992057354001614451,
 -0.00025474937951101049982680561302,
  0.00024762975547895881652073467771,
  0.00005229657281480196749313930265,
 -0.00007488354272621512385016593760,
  0.00001416880012891046449980449746,
  0.00001528986060172183690742576230,
 -0.00001668672297078590514293325326,
  0.00001061765189536459018739585094,
 -5.8220577442406209989680801335e-06,
  3.3322423743855900506302033234e-06,
 -2.23292405803003860894449897815e-06,
  1.74816651036678291794777245325e-06,
 -1.49581306041395051804547535093e-06,
  1.32759146107893129050610165582e-06,
 -1.19376077392564467408373553343e-06,
  1.07878303863211630544654040875e-06,
 -9.7743335011819134006676476250e-07,
  8.8729318903693324226127054792e-07,
 -8.0671146292125665050876015280e-07,
  7.3432860378667354971042255937e-07,
 -6.6897926072697370325310483359e-07,
  6.0966619703735610352576581485e-07,
 -5.5554095284507959561958605420e-07,
  5.0588335673197236002812826526e-07,
 -4.6008146297767601862670079590e-07,
  4.1761348515688145911438168306e-07,
 -3.7803230006989446874174476515e-07,
  3.4095248501364300041684648230e-07,
 -3.0603959751354749520615015472e-07,
  2.7300134179365690589640458993e-07,
 -2.4158028250762304756044254231e-07,
  2.1154781038298751985689113868e-07,
 -1.8269911328756771201465223313e-07,
  1.5484895085808513749026173074e-07,
 -1.2782806851555809369226440495e-07,
  1.0148011725394892565174207341e-07,
 -7.5658969771439627809239950461e-08,
  5.0226342286491286957075289622e-08,
 -2.5049645660259882970547555831e-08
};
static cheb_series B3_lt1_cs = {
  B3_lt1_data,
  39,
  -1, 1,
  39
};


/* Chebyshev fit for f(x) = A_3(9(x+1)/20) */
static double A3_lt1_data[40] = {
  -0.00017982561472134418587634980117,
  -0.00036558603837525275836608884064,
  -0.00002819398055929628850294406363,
   0.00016704539863875736769812786067,
  -0.00007098969970347674307623044850,
  -8.4470843942344237748899879940e-06,
   0.0000273413090343147765148014327150,
  -0.0000199073838489821681991178018081,
   0.0000100004176278235088881096950105,
  -3.9739852013143676487867902026e-06,
   1.2265357766449574306882693267e-06,
  -1.88755584306424047416914864854e-07,
  -1.37482206060161206336523452036e-07,
   2.10326379301853336795686477738e-07,
  -2.05583778245412633433934301948e-07,
   1.82377384812654863038691147988e-07,
  -1.58130247846381041027699152436e-07,
   1.36966982725588978654041029615e-07,
  -1.19250280944620257443805710485e-07,
   1.04477169029350256435316644493e-07,
  -9.2064832489437534542041040184e-08,
   8.1523798290458784610230199344e-08,
  -7.2471794980050867512294061891e-08,
   6.4614432955971132569968860233e-08,
  -5.7724095125560946811081322985e-08,
   5.1623107567436835158110947901e-08,
  -4.6171250746798606260216486042e-08,
   4.1256621998650164023254101585e-08,
  -3.6788925543159819135102047082e-08,
   3.2694499457951844422299750661e-08,
  -2.89125899697964696586521743928e-08,
   2.53925288725374047626589488217e-08,
  -2.20915707933726481321465184207e-08,
   1.89732166352720474944407102940e-08,
  -1.60058977893259856012119939554e-08,
   1.31619294542205876946742394494e-08,
  -1.04166651771938038563454275883e-08,
   7.7478015858156185064152078434e-09,
  -5.1347942579352613057675111787e-09,
   2.5583541594586723967261504321e-09
};
static cheb_series A3_lt1_cs = {
  A3_lt1_data,
  39,
  -1, 1,
  39
};

/* chebyshev fit for f(x) = A_4(2(x+1)/5) */
static double A4_lt1_data[30] = {
  0.00009054703770051610946958226736,
  0.00033066000498098017589672988293,
  0.00019737453734363989127226073272,
 -0.00015490809725932037720034762889,
 -0.00004514948935538730085479280454,
  0.00007976881782603940889444573924,
 -0.00003314566154544740986264993251,
 -1.88212148790135672249935711657e-06,
  0.0000114788756505519986352882940648,
 -9.2263039911196207101468331210e-06,
  5.1401128250377780476084336340e-06,
 -2.38418218951722002658891397905e-06,
  1.00664292214481531598338960828e-06,
 -4.23224678096490060264249970540e-07,
  2.00132031535793489976535190025e-07,
 -1.18689501178886741400633921047e-07,
  8.7819524319114212999768013738e-08,
 -7.3964150324206644900787216386e-08,
  6.5780431507637165113885884236e-08,
 -5.9651053193022652369837650411e-08,
  5.4447762662767276209052293773e-08,
 -4.9802057381568863702541294988e-08,
  4.5571368194694340198117635845e-08,
 -4.1682117173547642845382848197e-08,
  3.8084701352766049815367147717e-08,
 -3.4740302885185237434662649907e-08,
  3.1616557064701510611273692060e-08,
 -2.8685739487689556252374879267e-08,
  2.5923752117132254429002796600e-08,
 -2.3309428552190587304662883477e-08
};
static cheb_series A4_lt1_cs = {
  A4_lt1_data,
  29,
  -1, 1,
  29
};


static double olver_B0(double z, double abs_zeta)
{
  if(z < 0.98) {
    const double t = 1.0/sqrt(1.0-z*z);
    return -5.0/(48.0*abs_zeta*abs_zeta) + t*(-3.0 + 5.0*t*t)/(24.0*sqrt(abs_zeta));
  }
  else if(z < 1.02) {
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
    const double t = 1.0/(z*sqrt(1.0 - 1.0/(z*z)));
    return -5.0/(48.0*abs_zeta*abs_zeta) + t*(3.0 + 5.0*t*t)/(24.0*sqrt(abs_zeta));
  }
}


static double olver_B1(double z, double abs_zeta)
{
  if(z < 0.88) {
    const double t   = 1.0/sqrt(1.0-z*z);
    const double t2  = t*t;
    const double rz  = sqrt(abs_zeta);
    const double z32 = rz*rz*rz;
    const double z92 = z32*z32*z32;
    const double term1 = t*t*t * (30375.0 - 369603.0*t2 + 765765.0*t2*t2 - 425425.0*t2*t2*t2)/414720.0;
    const double term2 = 85085.0/(663552.0*z92);
    const double term3 = 385.0/110592.*t*(3.0-5.0*t2)/(abs_zeta*abs_zeta*abs_zeta);
    const double term4 = 5.0/55296.0*t2*(81.0 - 462.0*t2 + 385.0*t2*t2)/z32;
    return -(term1 + term2 + term3 + term4)/rz;
  }
  else if(z < 1.12) {
    const double a = 1.0-z;
    const double c0  = -0.00149282953213429172050073403334;
    const double c1  = -0.00175640941909277865678308358128;
    const double c2  = -0.00113346148874174912576929663517;
    const double c3  = -0.00034691090981382974689396961817;
    const double c4  =  0.00022752516104839243675693256916;
    const double c5  =  0.00051764145724244846447294636552;
    const double c6  =  0.00058906174858194233998714243010;
    const double c7  =  0.00053485514521888073087240392846;
    const double c8  =  0.00042891792986220150647633418796;
    const double c9  =  0.00031639765900613633260381972850;
    const double c10 =  0.00021908147678699592975840749194;
    return c0+a*(c1+a*(c2+a*(c3+a*(c4+a*(c5+a*(c6+a*(c7+a*(c8+a*(c9+a*c10)))))))));
  }
  else {
    const double t   = 1.0/(z*sqrt(1.0 - 1.0/(z*z)));
    const double t2  = t*t;
    const double rz  = sqrt(abs_zeta);
    const double z32 = rz*rz*rz;
    const double z92 = z32*z32*z32;
    const double term1 = -t2*t * (30375.0 + 369603.0*t2 + 765765.0*t2*t2 + 425425.0*t2*t2*t2)/414720.0;
    const double term2 = 85085.0/(663552.0*z92);
    const double term3 = -385.0/110592.0*t*(3.0+5.0*t2)/(abs_zeta*abs_zeta*abs_zeta);
    const double term4 = 5.0/55296.0*t2*(81.0 + 462.0*t2 + 385.0*t2*t2)/z32;
    return (term1 + term2 + term3 + term4)/rz;
  }
}


static double olver_B2(double z, double abs_zeta)
{
  if(z < 0.8) {
    const double x = 5.0*z/2.0 - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&B2_lt1_cs, x, &c);
    return  c.val / z;
  }
  else if(z <= 1.2) {
    const double a = 1.0-z;
    const double c0 = 0.00055221307672129279005986982501;
    const double c1 = 0.00089586516310476929281129228969;
    const double c2 = 0.00067015003441569770883539158863;
    const double c3 = 0.00010166263361949045682945811828;
    const double c4 = -0.00044086345133806887291336488582;
    const double c5 = -0.00073963081508788743392883072523;
    const double c6 = -0.00076745494377839561259903887331;
    const double c7 = -0.00060829038106040362291568012663;
    const double c8 = -0.00037128707528893496121336168683;
    const double c9 = -0.00014116325105702609866850307176;
    return c0+a*(c1+a*(c2+a*(c3+a*(c4+a*(c5+a*(c6+a*(c7+a*(c8+a*c9))))))));
  }
  else {
    const double zi = 1.0/z;
    const double x  = 12.0/5.0 * zi - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&B2_gt1_cs, x, &c);
    return c.val * zi*zi*zi;
  }
}


static double olver_B3(double z, double abs_zeta)
{
  if(z < 0.8) {
    const double x = 5.0*z/2.0 - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&B3_lt1_cs, x, &c);
    return c.val;
  }
  else if(z < 1.2) {
    const double a = 1.0-z;
    const double c0 = -0.00047461779655995980754441833105;
    const double c1 = -0.00095572913429464297452176811898;
    const double c2 = -0.00080369634512082892655558133973;
    const double c3 = -0.00000727921669154784138080600339;
    const double c4 =  0.00093162500331581345235746518994;
    const double c5 =  0.00149848796913751497227188612403;
    const double c6 =  0.00148406039675949727870390426462;
    return c0 + a*(c1 + a*(c2 + a*(c3 + a*(c4 + a*(c5 + a*c6)))));
  }
  else {
    const double x   = 12.0/(5.0*z) - 1.0;
    const double zi2 = 1.0/(z*z);
    gsl_sf_result c;
    cheb_eval_e(&B3_gt1_cs, x, &c);
    return  c.val * zi2*zi2*zi2;
  }
}


static double olver_A1(double z, double abs_zeta, double * err)
{
  if(z < 0.98) {
    double t = 1.0/sqrt(1.0-z*z);
    double rz = sqrt(abs_zeta);
    double t2 = t*t;
    double term1 =  t2*(81.0 - 462.0*t2 + 385.0*t2*t2)/1152.0;
    double term2 = -455.0/(4608.0*abs_zeta*abs_zeta*abs_zeta);
    double term3 =  7.0*t*(-3.0 + 5.0*t2)/(1152.0*rz*rz*rz);
    *err = 2.0 * GSL_DBL_EPSILON * (fabs(term1) + fabs(term2) + fabs(term3));
    return term1 + term2 + term3;
  }
  else if(z < 1.02) {
    const double a = 1.0-z;
    const double c0 = -0.00444444444444444444444444444444;
    const double c1 = -0.00184415584415584415584415584416;
    const double c2 =  0.00056812076812076812076812076812;
    const double c3 =  0.00168137865661675185484709294233;
    const double c4 =  0.00186744042139000122193399504324;
    const double c5 =  0.00161330105833747826430066790326;
    const double c6 =  0.00123177312220625816558607537838;
    const double c7 =  0.00087334711007377573881689318421;
    const double c8 =  0.00059004942455353250141217015410;
    const double sum = c0+a*(c1+a*(c2+a*(c3+a*(c4+a*(c5+a*(c6+a*(c7+a*c8)))))));
    *err = 2.0 * GSL_DBL_EPSILON * fabs(sum);
    return sum;
  }
  else {
    const double t = 1.0/(z*sqrt(1.0 - 1.0/(z*z)));
    const double rz = sqrt(abs_zeta);
    const double t2 = t*t;
    const double term1 = -t2*(81.0 + 462.0*t2 + 385.0*t2*t2)/1152.0;
    const double term2 =  455.0/(4608.0*abs_zeta*abs_zeta*abs_zeta);
    const double term3 = -7.0*t*(3.0 + 5.0*t2)/(1152.0*rz*rz*rz);
    *err = 2.0 * GSL_DBL_EPSILON * (fabs(term1) + fabs(term2) + fabs(term3));
    return term1 + term2 + term3;
  }
}


static double olver_A2(double z, double abs_zeta)
{
  if(z < 0.88) {
    double t  = 1.0/sqrt(1.0-z*z);
    double t2 = t*t;
    double t4 = t2*t2;
    double t6 = t4*t2;
    double t8 = t4*t4;
    double rz = sqrt(abs_zeta);
    double z3 = abs_zeta*abs_zeta*abs_zeta;
    double z32 = rz*rz*rz;
    double z92 = z3*z32;
    double term1 = t4*(4465125.0 - 94121676.0*t2 + 349922430.0*t4 - 446185740.0*t6  + 185910725.0*t8)/39813120.0;
    double term2 = -40415375.0/(127401984.0*z3*z3);
    double term3 = -95095.0/15925248.0*t*(3.0-5.0*t2)/z92;
    double term4 = -455.0/5308416.0 *t2*(81.0 - 462.0*t2 + 385.0*t4)/z3;
    double term5 = -7.0/19906560.0*t*t2*(30375.0 - 369603.0*t2  + 765765.0*t4  - 425425.0*t6)/z32;
    return term1 + term2 + term3 + term4 + term5;
  }
  else if(z < 1.12) {
    double a = 1.0-z;
    const double c0  =  0.000693735541354588973636592684210;
    const double c1  =  0.000464483490365843307019777608010;
    const double c2  = -0.000289036254605598132482570468291;
    const double c3  = -0.000874764943953712638574497548110;
    const double c4  = -0.001029716376139865629968584679350;
    const double c5  = -0.000836857329713810600584714031650;
    const double c6  = -0.000488910893527218954998270124540;
    const double c7  = -0.000144236747940817220502256810151;
    const double c8  =  0.000114363800986163478038576460325;
    const double c9  =  0.000266806881492777536223944807117;
    const double c10 = -0.011975517576151069627471048587000;
    return c0+a*(c1+a*(c2+a*(c3+a*(c4+a*(c5+a*(c6+a*(c7+a*(c8+a*(c9+a*c10)))))))));
  }
  else {
    const double t  = 1.0/(z*sqrt(1.0 - 1.0/(z*z)));
    const double t2 = t*t;
    const double t4 = t2*t2;
    const double t6 = t4*t2;
    const double t8 = t4*t4;
    const double rz = sqrt(abs_zeta);
    const double z3 = abs_zeta*abs_zeta*abs_zeta;
    const double z32 = rz*rz*rz;
    const double z92 = z3*z32;
    const double term1 = t4*(4465125.0 + 94121676.0*t2 + 349922430.0*t4 + 446185740.0*t6  + 185910725.0*t8)/39813120.0;
    const double term2 = -40415375.0/(127401984.0*z3*z3);
    const double term3 =  95095.0/15925248.0*t*(3.0+5.0*t2)/z92;
    const double term4 = -455.0/5308416.0 *t2*(81.0 + 462.0*t2 + 385.0*t4)/z3;
    const double term5 =  7.0/19906560.0*t*t2*(30375.0 + 369603.0*t2  + 765765.0*t4  + 425425.0*t6)/z32;
    return term1 + term2 + term3 + term4 + term5;
  }
}


static double olver_A3(double z, double abs_zeta)
{
  if(z < 0.9) {
    const double x = 20.0*z/9.0 - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&A3_lt1_cs, x, &c);
    return c.val;
  }
  else if(z < 1.1) {
    double a = 1.0-z;
    const double c0 = -0.000354211971457743840771125759200;
    const double c1 = -0.000312322527890318832782774881353;
    const double c2 =  0.000277947465383133980329617631915;
    const double c3 =  0.000919803044747966977054155192400;
    const double c4 =  0.001147600388275977640983696906320;
    const double c5 =  0.000869239326123625742931772044544;
    const double c6 =  0.000287392257282507334785281718027;
    return c0 + a*(c1 + a*(c2 + a*(c3 + a*(c4 + a*(c5 + a*c6)))));
  }
  else {
    const double x   = 11.0/(5.0*z) - 1.0;
    const double zi2 = 1.0/(z*z);
    gsl_sf_result c;
    cheb_eval_e(&A3_gt1_cs, x, &c);
    return  c.val * zi2*zi2*zi2;
  }
}


static double olver_A4(double z, double abs_zeta)
{
  if(z < 0.8) {
    const double x = 5.0*z/2.0 - 1.0;
    gsl_sf_result c;
    cheb_eval_e(&A4_lt1_cs, x, &c);
    return c.val;
  }
  else if(z < 1.2) {
    double a = 1.0-z;
    const double c0 =  0.00037819419920177291402661228437;
    const double c1 =  0.00040494390552363233477213857527;
    const double c2 = -0.00045764735528936113047289344569;
    const double c3 = -0.00165361044229650225813161341879;
    const double c4 = -0.00217527517983360049717137015539;
    const double c5 = -0.00152003287866490735107772795537;
    return c0 + a*(c1 + a*(c2 + a*(c3 + a*(c4 + a*c5))));
  }
  else {
    const double x   = 12.0/(5.0*z) - 1.0;
    const double zi2 = 1.0/(z*z);
    gsl_sf_result c;
    cheb_eval_e(&A4_gt1_cs, x, &c);
    return c.val * zi2*zi2*zi2*zi2;
  }
}

inline
static double olver_Asum(double nu, double z, double abs_zeta, double * err)
{
  double nu2 = nu*nu;
  double A1_err;
  double A1 = olver_A1(z, abs_zeta, &A1_err);
  double A2 = olver_A2(z, abs_zeta);
  double A3 = olver_A3(z, abs_zeta);
  double A4 = olver_A4(z, abs_zeta);
  *err = A1_err/nu2 + GSL_DBL_EPSILON;
  return 1.0 + A1/nu2 + A2/(nu2*nu2) + A3/(nu2*nu2*nu2) + A4/(nu2*nu2*nu2*nu2);
}

inline
static double olver_Bsum(double nu, double z, double abs_zeta)
{
  double nu2 = nu*nu;
  double B0 = olver_B0(z, abs_zeta);
  double B1 = olver_B1(z, abs_zeta);
  double B2 = olver_B2(z, abs_zeta);
  double B3 = olver_B3(z, abs_zeta);
  return B0 + B1/nu2 + B2/(nu2*nu2) + B3/(nu2*nu2*nu2*nu2);
}


/* uniform asymptotic, nu -> Inf, [Abramowitz+Stegun, 9.3.35]
 *
 * error:
 *    nu =  2: uniformly good to >  6D
 *    nu =  5: uniformly good to >  8D
 *    nu = 10: uniformly good to > 10D
 *    nu = 20: uniformly good to > 13D
 *
 */
int gsl_sf_bessel_Jnu_asymp_Olver_e(double nu, double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0 || nu <= 0.0) {
    DOMAIN_ERROR(result);
  }  
  else {
    double zeta, abs_zeta;
    double arg;
    double pre;
    double asum, bsum, asum_err;
    gsl_sf_result ai;
    gsl_sf_result aip;
    double z = x/nu;
    double crnu = pow(nu, 1.0/3.0);
    double nu3  = nu*nu*nu;
    double nu11 = nu3*nu3*nu3*nu*nu;
    int stat_a, stat_ap;

    if(fabs(1.0-z) < 0.02) {
      const double a = 1.0-z;
      const double c0 = 1.25992104989487316476721060728;
      const double c1 = 0.37797631496846194943016318218;
      const double c2 = 0.230385563409348235843147082474;
      const double c3 = 0.165909603649648694839821892031;
      const double c4 = 0.12931387086451008907;
      const double c5 = 0.10568046188858133991;
      const double c6 = 0.08916997952268186978;
      const double c7 = 0.07700014900618802456;
      pre = c0 + a*(c1 + a*(c2 + a*(c3 + a*(c4 + a*(c5 + a*(c6 + a*c7))))));
      zeta = a * pre;
      pre  = sqrt(2.0*sqrt(pre/(1.0+z)));
      abs_zeta = fabs(zeta);
    }
    else if(z < 1.0) {
      double rt   = sqrt(1.0 - z*z);
      abs_zeta = pow(1.5*(log((1.0+rt)/z) - rt), 2.0/3.0);
      zeta = abs_zeta;
      pre  = sqrt(2.0*sqrt(abs_zeta/(rt*rt)));
    }
    else {
      /* z > 1 */
      double rt = z * sqrt(1.0 - 1.0/(z*z));
      abs_zeta = pow(1.5*(rt - acos(1.0/z)), 2.0/3.0);
      zeta = -abs_zeta;
      pre  = sqrt(2.0*sqrt(abs_zeta/(rt*rt)));
    }

    asum = olver_Asum(nu, z, abs_zeta, &asum_err);
    bsum = olver_Bsum(nu, z, abs_zeta);

    arg  = crnu*crnu * zeta;
    stat_a  = gsl_sf_airy_Ai_e(arg, GSL_MODE_DEFAULT, &ai);
    stat_ap = gsl_sf_airy_Ai_deriv_e(arg, GSL_MODE_DEFAULT, &aip);

    result->val  = pre * (ai.val*asum/crnu + aip.val*bsum/(nu*crnu*crnu));
    result->err  = pre * (ai.err * fabs(asum/crnu));
    result->err += pre * fabs(ai.val) * asum_err / crnu;
    result->err += pre * fabs(ai.val * asum) / (crnu*nu11);
    result->err += 8.0 * GSL_DBL_EPSILON * fabs(result->val);

    return GSL_ERROR_SELECT_2(stat_a, stat_ap);
  }
}


/* uniform asymptotic, nu -> Inf,  [Abramowitz+Stegun, 9.3.36]
 *
 * error:
 *    nu =  2: uniformly good to >  6D
 *    nu =  5: uniformly good to >  8D
 *    nu = 10: uniformly good to > 10D
 *    nu = 20: uniformly good to > 13D
 */
int gsl_sf_bessel_Ynu_asymp_Olver_e(double nu, double x, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(x <= 0.0 || nu <= 0.0) {
    DOMAIN_ERROR(result);
  }  
  else {
    double zeta, abs_zeta;
    double arg;
    double pre;
    double asum, bsum, asum_err;
    gsl_sf_result bi;
    gsl_sf_result bip;
    double z = x/nu;
    double crnu = pow(nu, 1.0/3.0);
    double nu3  = nu*nu*nu;
    double nu11 = nu3*nu3*nu3*nu*nu;
    int stat_b, stat_d;

    if(fabs(1.0-z) < 0.02) {
      const double a = 1.0-z;
      const double c0 = 1.25992104989487316476721060728;
      const double c1 = 0.37797631496846194943016318218;
      const double c2 = 0.230385563409348235843147082474;
      const double c3 = 0.165909603649648694839821892031;
      const double c4 = 0.12931387086451008907;
      const double c5 = 0.10568046188858133991;
      const double c6 = 0.08916997952268186978;
      const double c7 = 0.07700014900618802456;
      pre = c0 + a*(c1 + a*(c2 + a*(c3 + a*(c4 + a*(c5 + a*(c6 + a*c7))))));
      zeta = a * pre;
      pre  = sqrt(2.0*sqrt(pre/(1.0+z)));
      abs_zeta = fabs(zeta);
    }
    else if(z < 1.0) {
      double rt   = sqrt(1.0 - z*z);
      abs_zeta = pow(1.5*(log((1.0+rt)/z) - rt), 2.0/3.0);
      zeta = abs_zeta;
      pre  = sqrt(2.0*sqrt(abs_zeta/(rt*rt)));
    }
    else {
      /* z > 1 */
      double rt = z * sqrt(1.0 - 1.0/(z*z));
      double ac = acos(1.0/z);
      abs_zeta = pow(1.5*(rt - ac), 2.0/3.0);
      zeta = -abs_zeta;
      pre  = sqrt(2.0*sqrt(abs_zeta)/rt);
    }

    asum = olver_Asum(nu, z, abs_zeta, &asum_err);
    bsum = olver_Bsum(nu, z, abs_zeta);

    arg  = crnu*crnu * zeta;
    stat_b = gsl_sf_airy_Bi_e(arg, GSL_MODE_DEFAULT, &bi);
    stat_d = gsl_sf_airy_Bi_deriv_e(arg, GSL_MODE_DEFAULT, &bip);

    result->val  = -pre * (bi.val*asum/crnu + bip.val*bsum/(nu*crnu*crnu));
    result->err  =  pre * (bi.err * fabs(asum/crnu));
    result->err +=  pre * fabs(bi.val) * asum_err / crnu;
    result->err +=  pre * fabs(bi.val*asum) / (crnu*nu11);
    result->err +=  8.0 * GSL_DBL_EPSILON * fabs(result->val);

    return GSL_ERROR_SELECT_2(stat_b, stat_d);
  }
}
