/* GaussianMixture.cpp
 *
 * Copyright (C) 2011-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
  djmw 20101021 Initial version
*/
#include "Distributions_and_Strings.h"
#include "GaussianMixture.h"
#include "NUMmachar.h"
#include "NUM2.h"
#include "Strings_extensions.h"

#include "oo_DESTROY.h"
#include "GaussianMixture_def.h"
#include "oo_COPY.h"
#include "GaussianMixture_def.h"
#include "oo_EQUAL.h"
#include "GaussianMixture_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "GaussianMixture_def.h"
#include "oo_WRITE_TEXT.h"
#include "GaussianMixture_def.h"
#include "oo_READ_TEXT.h"
#include "GaussianMixture_def.h"
#include "oo_WRITE_BINARY.h"
#include "GaussianMixture_def.h"
#include "oo_READ_BINARY.h"
#include "GaussianMixture_def.h"
#include "oo_DESCRIPTION.h"
#include "GaussianMixture_def.h"

#include "enums_getText.h"
#include "GaussianMixture_enums.h"
#include "enums_getValue.h"
#include "GaussianMixture_enums.h"

Thing_implement (GaussianMixture, Daata, 0);

void structGaussianMixture :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of components: ", our numberOfComponents);
	MelderInfo_writeLine (U"Dimension of component: ", our dimension);
	MelderInfo_writeLine (U"Mixing probabilities:");
	for (integer im = 1; im <= numberOfComponents; im ++) {
		MelderInfo_writeLine (U"  ", im, U": p = ", our mixingProbabilities [im], U"  Name =  \"", Thing_getName (our covariances->at [im]), U"\"");
	}
}

static integer GaussianMixture_getNumberOfParametersInComponent (GaussianMixture me) {
	Melder_assert (my covariances->size > 0);
	const Covariance thee = my covariances->at [1];
	return ( thy numberOfRows == 1 ? 2 * thy numberOfColumns : thy numberOfColumns * (thy numberOfColumns + 3) / 2 );
}

static double GaussianMixture_getLikelihoodValue (GaussianMixture me, constMAT const& probabilities, kGaussianMixtureCriterion criterion) {
	Melder_require (probabilities.ncol == my numberOfComponents,
		U"The number of columns in the probabilities should equal the number of components.");
	const integer numberOfData = probabilities.nrow;
	Melder_require (numberOfData > my numberOfComponents,
		U"The number of rows in the probabilities should be larger than the number of components.");
	if (criterion == kGaussianMixtureCriterion::COMPLETE_DATA_ML) {
		/*
			Bishop eq. 9.40 (we rewrote ln(a)+ln(b) = ln (a*b)):
			ln(p(X,Z|μ,S,π)= sum(n=1...N, sum (k=1...K, gamma [n][k])*ln (π [k]*N(x [n]|μ [k],S [k])),
			where gamma[n][k] = mixingProbablities[k]*probabilities[n][k]/sum(1...K, mixingProbablities[k]*probabilities[n][k])
		*/
		longdouble lnpcd = 0.0;
		for (integer irow = 1; irow <= numberOfData; irow ++) {
			longdouble psum = 0.0, lnsum = 0.0;
			for (integer icol = 1; icol <= my numberOfComponents; icol ++) {
				const longdouble pp = my mixingProbabilities [icol] * probabilities [irow] [icol];
				psum += pp;
				lnsum += pp * log (pp); // scaling outside the loop
			}
			if (psum > 0)
				lnpcd += lnsum / psum; // scaling: as reponsibilities
		}
		return lnpcd;
	}
	/*
		The common factor for the following criteria is the log(likelihood), Bishop eq. 9.28
	*/
	longdouble lnp = 0.0;
	for (integer irow = 1; irow <= numberOfData; irow ++) {
		double psum = NUMinner (my mixingProbabilities.get(), probabilities.row (irow));
		if (psum > 0.0)
			lnp += (longdouble) log (psum);
	}

	if (criterion == kGaussianMixtureCriterion::LIKELIHOOD)
		return lnp;

	const double numberOfParametersPerComponent = GaussianMixture_getNumberOfParametersInComponent (me);
	const double numberOfParametersTotal = numberOfParametersPerComponent * my numberOfComponents;
	if (criterion == kGaussianMixtureCriterion::MESSAGE_LENGTH) {
		/*
			Equation (15) in
			Figueiredo & Jain, Unsupervised Learning of Finite Mixture Models :
			IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 24, NO. 3, MARCH 2002

			L(theta,Y)= N/2*sum(m=1..k, log(n*alpha [m]/12)) +k/2*ln(n/12) +k(N+1)/2
				- log (sum(i=1..n, sum(m=1..k, alpha [k]*p(k))))
		*/
		longdouble logmpn = 0.0;
		integer numberOfNonZeroComponents = 0;
		for (integer ic = 1; ic <= my numberOfComponents; ic ++)
			if (my mixingProbabilities [ic] > 0) {
				logmpn += log (my mixingProbabilities [ic]);
				numberOfNonZeroComponents ++;
			}
		/*
			A rewritten L(theta,Y) is
		*/
		return lnp - 0.5 * numberOfNonZeroComponents * (numberOfParametersPerComponent + 1) * (log (numberOfData / 12.0) + 1.0)
		       - 0.5 * numberOfParametersPerComponent * logmpn;
	} else if (criterion == kGaussianMixtureCriterion::BAYES_INFORMATION)
		return 2.0 * lnp - numberOfParametersTotal * log (numberOfData);
	else if (criterion == kGaussianMixtureCriterion::AKAIKE_INFORMATION)
		return 2.0 * (lnp - numberOfParametersTotal);
	else if (criterion == kGaussianMixtureCriterion::AKAIKE_CORRECTED) {
		return 2.0 * (lnp - numberOfParametersTotal * (numberOfData / (numberOfData - numberOfParametersTotal - 1.0)));
	}
	return lnp;
}

static void GaussianMixture_getResponsibilities (GaussianMixture me, constMATVU const& probabilities, integer componentToUpdate, MAT const& responsibilities) {
	Melder_require (responsibilities.nrow == probabilities.nrow && responsibilities.ncol == probabilities.ncol,
			U"The responsibilities and the probabilities should have the same dimensions.");
	Melder_require (responsibilities.ncol == my numberOfComponents,
			U"The number of columns of the responsbilities should equal the number of components.");
	const integer fromComponent = componentToUpdate == 0 ? 1 : componentToUpdate;
	const integer toComponent = componentToUpdate == 0 ? my numberOfComponents : componentToUpdate;
	
	for (integer irow = 1; irow <= probabilities.nrow; irow ++) {
		responsibilities.row (irow).part (fromComponent, toComponent) <<= probabilities.row (irow).part (fromComponent, toComponent)  *  my mixingProbabilities.part (fromComponent, toComponent);
	}
	/*
		Maintain the invariant.
	*/
	MATnormalizeRows_inplace (responsibilities, 1.0, 1.0);
}

static void GaussianMixture_updateComponent (GaussianMixture me, integer component, MATVU const& data, MATVU const& responsibilities) {
	integer numberOfData = data.nrow;
	Melder_require (my dimension == data.ncol,
		U"The number of columns in the data and the dimension of the GaussianMixture should be equal.");
	Melder_require (my numberOfComponents == responsibilities.ncol,
		U"The number of components and the number of columns in the responsibilities should conform.");
	Melder_require (responsibilities.nrow == data.nrow,
		U"The number of rows in the data and the responsibilities should conform.");
	Melder_require (component > 0 && component <= my numberOfComponents,
		U"The component number should be in the range from 1 to ", my numberOfComponents, U".");
	
	const Covariance thee = my covariances->at [component];
	/*
		Update the means: Bishop eq. 9.24
	*/
	thy centroid.all()  <<=  0.0;
	for (integer irow = 1; irow <= numberOfData; irow ++)
		thy centroid.get()  +=  responsibilities [irow] [component]  *  data.row (irow);
	
	const double totalComponentResponsibility = NUMsum (responsibilities.column (component));
	thy centroid.get ()  /=  totalComponentResponsibility;
	/*
		update covariance with the new mean: Bishop eq. 9.25
	*/
	thy data.all()  <<=  0.0;
	autoVEC dif = raw_VEC (thy numberOfColumns);
	if (thy numberOfRows == 1) { // 1xn covariance
		autoVEC variance = raw_VEC (thy numberOfColumns);
		for (integer irow = 1; irow <= numberOfData; irow ++) {
			dif.all()  <<=  data.row (irow)  -  thy centroid.get();
			variance.all()   <<= dif.all()  *  dif.all();
			thy data.row (1)  +=  responsibilities [irow] [component]  *  variance.get();
		}
	} else { // nxn covariance
		autoMAT covar = raw_MAT (thy numberOfColumns, thy numberOfColumns);
		for (integer irow = 1; irow <= numberOfData; irow ++) {
			dif.all() <<= data.row (irow)  -  thy centroid.get();
			outer_MAT_out (covar.get(), dif.get(), dif.get());
			thy data.get()  +=  responsibilities [irow] [component]  *  covar.get();
		}
	}
	thy data.get()  /=  totalComponentResponsibility;
	thy numberOfObservations = my mixingProbabilities [component] * numberOfData;
}

static void GaussianMixture_setDefaultMixtureNames (GaussianMixture me) {
	for (integer component = 1; component <= my numberOfComponents; component ++) {
		Covariance cov = my covariances->at [component];
		Thing_setName (cov, Melder_cat (U"c", component));
	}
}

static void GaussianMixture_addCovarianceFraction (GaussianMixture me, integer component, Covariance him, double fraction) {
	Melder_require (component > 0 && component <= my numberOfComponents,
		U"The component number should be in the range from 1 to ", my numberOfComponents, U".");
	const Covariance thee = my covariances->at [component];
	Melder_require (thy numberOfColumns == his numberOfColumns,
		U"The dimension of the Covariance should equal the dimension of the covariances in the GaussianMixture.");
	/*
		Prevent instability: add lambda fraction of global covariances
	*/
	if (thy numberOfRows == 1)
		if (his numberOfRows == 1)
			thy data.row (1)  +=  fraction * his data.row (1);
		else
			thy data.row (1)  +=  fraction * his data.diagonal();
	else
		if (his numberOfRows == 1)
			thy data.diagonal()  +=  his data.row (1);
		else
			thy data.get()  +=  his data.get()  *  fraction;
}

static void GaussianMixture_setColumnLabelsFromTableOfReal (GaussianMixture me, TableOfReal thee) {
	for (integer component = 1; component <= my numberOfComponents; component ++) {
		const Covariance cov = my covariances->at [component];
		for (integer j = 1; j <= my dimension; j ++)
			TableOfReal_setColumnLabel (cov, j, thy columnLabels [j].get());
	}
}

// only from big to reduced or same
static void Covariance_into_Covariance (Covariance me, Covariance thee) {
	Melder_require (my numberOfColumns == thy numberOfColumns,
		U"Dimensions should be equal.");
	SSCP_unExpand (thee); // to its original state
	thy numberOfObservations = my numberOfObservations;
	thy centroid.all() <<= my centroid.all();
	thy columnLabels.all() <<= my columnLabels.all();

	if (my numberOfRows == thy numberOfRows) {
		thy rowLabels.all() <<= my rowLabels.all();
		thy data.all() <<= my data.all();
	} else {
		for (integer ir = 1; ir <= my numberOfRows; ir ++)
			for (integer ic = ir; ic <= my numberOfColumns; ic ++) {
				const integer dij = ic - ir;
				if (dij < thy numberOfRows)
					thy data [dij + 1] [ic] = my data [ir] [ic];
			}
	}
}

conststring32 GaussianMixture_criterionText (kGaussianMixtureCriterion criterion) {
	if (criterion == kGaussianMixtureCriterion::LIKELIHOOD)
		return U"(1/n)*Likelihood";
	else if (criterion == kGaussianMixtureCriterion::MESSAGE_LENGTH)
		return U"(1/n)*Messagelength";
	else if (criterion == kGaussianMixtureCriterion::BAYES_INFORMATION)
		return U"(1/n)*BayesInformation";
	else if (criterion == kGaussianMixtureCriterion::AKAIKE_INFORMATION)
		return U"(1/n)*AkaikeInformation";
	else if (criterion == kGaussianMixtureCriterion::AKAIKE_CORRECTED)
		return U"(1/n)*AkaikeCorrected";
	else if (criterion == kGaussianMixtureCriterion::COMPLETE_DATA_ML)
		return U"(1/n)*CompleteDataML";
	else
		return U"???";
}

void GaussianMixture_removeUnsupportedComponents (GaussianMixture me) {
	integer numberOfComponents = my numberOfComponents;
	for (integer component = numberOfComponents; component > 0; component --) {
		if (my mixingProbabilities [component] <= 0.0) {
			my covariances -> removeItem (component);
			my numberOfComponents --;
			for (integer ic = component; ic <= my numberOfComponents; ic ++)
				my mixingProbabilities [ic] = my mixingProbabilities [ic + 1];
		}
	}
	my mixingProbabilities. resize (my numberOfComponents); // they stay normalized
}

autoGaussianMixture GaussianMixture_create (integer numberOfComponents, integer dimension, kGaussianMixtureStorage storage) {
	try {
		autoGaussianMixture me = Thing_new (GaussianMixture);
		my numberOfComponents = numberOfComponents;
		my dimension = dimension;
		my mixingProbabilities = raw_VEC (numberOfComponents);
		my mixingProbabilities.all() <<= 1.0 / numberOfComponents;
		my covariances = CovarianceList_create ();
		kSSCPstorage sscpStorage = storage == kGaussianMixtureStorage::DIAGONALS ? kSSCPstorage::DIAGONAL : kSSCPstorage::COMPLETE;
		for (integer component = 1; component <= numberOfComponents; component ++) {
			autoCovariance cov = Covariance_create_reduceStorage (dimension, sscpStorage);
			my covariances -> addItemAtPosition_move (cov.move(), component);
		}
		GaussianMixture_setDefaultMixtureNames (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"GaussianMixture not created.");
	}
}

void GaussianMixture_generateOneVector_inline (GaussianMixture me, VEC const& c, autostring32 *out_covname, VEC const& buf) {
	try {
		const double p = NUMrandomUniform (0.0, 1.0);
		const integer component = NUMgetIndexFromProbability (my mixingProbabilities.get(), p);
		Covariance thee = my covariances->at [component];
		if (thy numberOfRows == 1) { // 1xn reduced form
			for (integer i = 1; i <= my dimension; i ++)
				c [i] = NUMrandomGauss (thy centroid [i], sqrt (thy data [1] [i]));
		} else { // nxn
			if (! thy pca)
				SSCP_expandPCA (thee);    // on demand expanding
			Covariance_PCA_generateOneVector_inline (thee, thy pca.get(), c, buf);
		}
		if (out_covname) {
			autostring32 name = Melder_dup (thy name.get()); 
			*out_covname = name.move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": vector not generated.");
	}
}

autoGaussianMixture TableOfReal_to_GaussianMixture_fromRowLabels (TableOfReal me, kGaussianMixtureStorage storage) {
	try {
		autoStrings rowLabels = TableOfReal_extractRowLabels (me);
		autoDistributions dist = Strings_to_Distributions (rowLabels.get());
		const integer numberOfComponents = dist -> numberOfRows;

		autoGaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);

		GaussianMixture_setColumnLabelsFromTableOfReal (thee.get(), me);

		for (integer component = 1; component <= numberOfComponents; component ++) {
			autoTableOfReal tab = TableOfReal_extractRowsWhereLabel (me, kMelder_string::EQUAL_TO, dist -> rowLabels [component].get());
			autoCovariance cov = TableOfReal_to_Covariance (tab.get());
			Covariance_into_Covariance (cov.get(), thy covariances->at [component]);
			Thing_setName (thy covariances->at [component], dist -> rowLabels [component].get());
		}
		thy mixingProbabilities.all()  <<=  dist -> data.column (1).part(1, dist -> numberOfRows);
		thy mixingProbabilities.all()  /=  my numberOfRows;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no GaussianMixture created.");
	}
}

autoCovariance GaussianMixture_to_Covariance_between (GaussianMixture me) {
	return CovarianceList_to_Covariance_between (my covariances.get());
}
autoCovariance GaussianMixture_to_Covariance_within (GaussianMixture me) {
	return CovarianceList_to_Covariance_within (my covariances.get());
}

autoCovariance GaussianMixture_to_Covariance_total (GaussianMixture me) {
	try {
		autoCovariance thee = GaussianMixture_to_Covariance_between (me);
		autoCovariance within = GaussianMixture_to_Covariance_within (me);
		thy data.all()  +=  within -> data.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (total) created.");
	}
}

autoCovariance GaussianMixture_extractComponent (GaussianMixture me, integer component) {
	try {
		Melder_require (component > 0 && component <= my numberOfComponents,
			U"The component should be in the range from 1 to ", my numberOfComponents, U".");
		autoCovariance thee = Data_copy (my covariances->at [component]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no component extracted.");
	}
}

autoTableOfReal GaussianMixture_extractMixingProbabilities (GaussianMixture me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfComponents, 2);
		TableOfReal_setColumnLabel (thee.get(), 1, U"componentProbability");
		TableOfReal_setColumnLabel (thee.get(), 2, U"numberOfObservations");
		for (integer im = 1; im <= my numberOfComponents; im ++) {
			const Covariance cov = my covariances->at [im];
			thy data [im] [1] = my mixingProbabilities [im];
			thy data [im] [2] = cov -> numberOfObservations;
			TableOfReal_setRowLabel (thee.get(), im, Thing_getName (cov));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no mixing probabilities extracted.");
	}
}

autoTableOfReal GaussianMixture_extractCentroids (GaussianMixture me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfComponents, my dimension);

		for (integer component = 1; component <= my numberOfComponents; component ++) {
			const Covariance cov = my covariances->at [component];
			if (component == 1)
				for (integer j = 1; j <= my dimension; j ++)
					TableOfReal_setColumnLabel (thee.get(), j, cov -> columnLabels [j].get());
			TableOfReal_setRowLabel (thee.get(), component, Thing_getName (cov));
			thy data.row (component) <<= cov -> centroid.get();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no centroid extracted.");
	}
}

autoPCA GaussianMixture_to_PCA (GaussianMixture me) {
	try {
		autoCovariance him = GaussianMixture_to_Covariance_total (me);
		autoPCA thee = SSCP_to_PCA (him.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PCA calculated.");
	}
}

void GaussianMixture_getIntervalsAlongDirections (GaussianMixture me, integer d1, integer d2, double nsigmas, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax) {
	Melder_require (d1 > 0 && d1 <= my dimension && d2 > 0 && d2 <= my dimension,
		U"The directions should be in the range from 1 to ", my dimension, U".");
	autoSSCPList sscps = SSCPList_extractTwoDimensions (my covariances->asSSCPList(), d1, d2);
	SSCPList_getEllipsesBoundingBoxCoordinates (sscps.get(), -nsigmas, 0, out_xmin, out_xmax, out_ymin, out_ymax);
}

void GaussianMixture_getIntervalAlongDirection (GaussianMixture me, integer d, double nsigmas, double *out_xmin, double *out_xmax) {
	GaussianMixture_getIntervalsAlongDirections (me, d, d, nsigmas, out_xmin, out_xmax, nullptr, nullptr);
}

void GaussianMixture_PCA_getIntervalsAlongDirections (GaussianMixture me, PCA thee, integer d1, integer d2, double nsigmas, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax) {
	Melder_require (my dimension == thy dimension,
		U"The dimensions should agree.");
	Melder_require (d1 >= 1 && d1 <= my dimension && d2 >= 1 && d2 <= my dimension,
		U"The dimensions should be in the range from 1 to ", my dimension, U".");
	
	autoSSCPList sscps = SSCPList_toTwoDimensions (my covariances->asSSCPList(), thy eigenvectors.row (d1), thy eigenvectors.row (d2));
	SSCPList_getEllipsesBoundingBoxCoordinates (sscps.get(), -nsigmas, 0, out_xmin, out_xmax, out_ymin, out_ymax);
}


void GaussianMixture_PCA_getIntervalAlongDirection (GaussianMixture me, PCA thee, integer d, double nsigmas, double *out_xmin, double *out_xmax) {
	GaussianMixture_PCA_getIntervalsAlongDirections (me, thee, d, d, nsigmas, out_xmin, out_xmax, nullptr, nullptr);
}

void GaussianMixture_PCA_drawMarginalPdf (GaussianMixture me, PCA thee, Graphics g, integer d, double xmin, double xmax, double ymin, double ymax, integer npoints, integer nbins, bool garnish) {
	Melder_require (my dimension == thy dimension,
		U"The dimensions should agree.");
	Melder_require (d >= 1 && d <= my dimension,
		U"The dimension must be in the range from 1 to ", my dimension, U".");
	if (npoints <= 1)
		npoints = 1000;
	constexpr double nsigmas = 2;
	if (xmax <= xmin)
		GaussianMixture_PCA_getIntervalAlongDirection (me, thee, d, nsigmas, & xmin, & xmax);
	const double dx = (xmax - xmin) / npoints, x1 = xmin + 0.5 * dx;
	const double scalef = ( nbins <= 0 ? 1.0 : 1.0 ); // TODO
	autoVEC p = raw_VEC (npoints);
	for (integer i = 1; i <= npoints; i ++) {
		const double x = x1 + (i - 1) * dx;
		Melder_assert (thy eigenvectors.ncol == thy dimension);
		p [i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, thy eigenvectors.row (d), x);
	}
	const double pmax = NUMmax (p.get());
	if (ymin >= ymax) {
		ymin = 0.0;
		ymax = pmax;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p.asArgumentToFunctionThatExpectsOneBasedArray(), 1, npoints, x1, xmax - 0.5 * dx);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
	}
}

void GaussianMixture_drawMarginalPdf (GaussianMixture me, Graphics g, integer d, double xmin, double xmax, double ymin, double ymax, integer npoints, integer /* nbins */, bool garnish) {
	Melder_require (d >= 1 && d <= my dimension,
		U"The dimension should be in the range from 1 to ", my dimension, U".");
	
	if (npoints <= 1)
		npoints = 1000;

	constexpr double nsigmas = 2.0;
	if (xmax <= xmin)
		GaussianMixture_getIntervalAlongDirection (me, d, nsigmas, & xmin, & xmax);

	const double dx = (xmax - xmin) / (npoints - 1);
	const double scalef = 1.0; // TODO
	autoVEC p = raw_VEC (npoints);
	autoVEC pos = raw_VEC (my dimension);
	
	for (integer k = 1; k <= my dimension; k++)
		pos [k] = ( k == d ? 1.0 : 0.0 );
	for (integer i = 1; i <= npoints; i++) {
		const double x = xmin + (i - 1) * dx;
		p [i] = scalef * GaussianMixture_getMarginalProbabilityAtPosition (me, pos.get(), x);
	}
	const double pmax = NUMmax (p.get());
	if (ymin >= ymax) {
		ymin = 0;
		ymax = pmax;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, p.asArgumentToFunctionThatExpectsOneBasedArray(), 1, npoints, xmin, xmax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
	}
}

void GaussianMixture_PCA_drawConcentrationEllipses (GaussianMixture me, PCA him, Graphics g, double scale,
	bool confidence, char32 *label, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, double fontSize, bool garnish) {
	Melder_require (my dimension == his dimension,
		U"The numbers of dimensions should agree.");
	const integer dim1 = integer_abs (d1), dim2 = integer_abs (d2);
	Melder_require (dim1 >= 1 && dim1 <= my dimension && dim2 >= 1 && dim2 <= my dimension,
		U"The dimension numbers should be in the range from 1 to ", my dimension, U" (or the negative of this value for a reversed axis).");
	autoVEC eigenvector1 = copy_VEC (his eigenvectors.row (dim1));
	autoVEC eigenvector2 = copy_VEC (his eigenvectors.row (dim2));
	if (d1 < 0)
		eigenvector1.all()  *=  -1.0;
	if (d2 < 0)
		eigenvector2.all()  *=  -1.0;

	autoSSCPList thee = SSCPList_toTwoDimensions (my covariances->asSSCPList(), eigenvector1.all(), eigenvector2.all());

	SSCPList_drawConcentrationEllipses (thee.get(), g, -scale, confidence, label, 1, 2, xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish) {
		char32 llabel [40];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"pc ", d2);
		Graphics_textLeft (g, true, llabel);
		Graphics_marksBottom (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"pc ", d1);
		Graphics_textBottom (g, true, llabel);
	}
}

void GaussianMixture_drawConcentrationEllipses (GaussianMixture me, Graphics g, double scale, bool confidence, char32 *label,
	bool pcaDirections, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, double fontSize, bool garnish) {
	const integer dim1 = integer_abs (d1), dim2 = integer_abs (d2);
	Melder_require (dim1 >= 1 && dim1 <= my dimension && dim2 >= 1 && dim2 <= my dimension,
		U"The dimensions should be in the range from 1 to ", my dimension, U" (or the negative of this value for a reversed axis).");
	if (! pcaDirections) {
		SSCPList_drawConcentrationEllipses (my covariances->asSSCPList(), g, -scale, confidence, label,
			dim1, dim2, xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	autoPCA him = GaussianMixture_to_PCA (me);

	GaussianMixture_PCA_drawConcentrationEllipses (me, him.get(), g, scale, confidence, label, d1, d2,
	        xmin, xmax, ymin, ymax, fontSize, garnish);
}

void GaussianMixture_initialGuess (GaussianMixture me, TableOfReal thee) {
	try {
		autoCovariance cov_t = TableOfReal_to_Covariance (thee);
		for (integer icol = 1; icol <= thy numberOfColumns; icol ++) {
			const double min = NUMmin (thy data.column(icol));
			const double max = NUMmax (thy data.column(icol));
			for (integer component = 1; component <= my numberOfComponents; component ++) {
				const Covariance covi = my covariances->at [component];
				covi -> centroid [icol] = NUMrandomUniform (min, max);
			}
		}
		const double varianceScaleFactor = 1.0 / (10 * thy numberOfColumns);
		for (integer component = 1; component <= my numberOfComponents; component ++) {
			const Covariance cov = my covariances->at [component];
			if (cov -> numberOfRows == 1)
				cov -> data.row(1) <<= varianceScaleFactor  *  cov_t -> data.diagonal();
			else
				cov -> data.diagonal() <<= varianceScaleFactor  *  cov_t -> data.diagonal();
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no initial guess possible.");
	}
}
	
void GaussianMixture_initialGuess2 (GaussianMixture me, TableOfReal thee, double nSigmas, double ru_range) {
	try {
		autoCovariance cov_t = TableOfReal_to_Covariance (thee);
		/*
			Assume equal probabilities for mixture
			Assume equal covariance matrices
			Spread centroids on an ellips in pc1-pc2 plane?
		*/
		if (my dimension == 1) {
			const double dm = 2.0 * sqrt (cov_t -> data [1] [1]) / my numberOfComponents;
			double m1 = cov_t -> centroid [1] - dm;
			for (integer im = 1; im <= my numberOfComponents; im ++) {
				const Covariance covi = my covariances->at [im];
				covi -> centroid [1] = m1;
				covi -> data [1] [1] = dm * dm;
				m1 += dm;
				covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
			}
		} else {
			autoPCA pca = SSCP_to_PCA (cov_t.get());
			autoSSCP s2d = SSCP_toTwoDimensions (cov_t.get(), pca -> eigenvectors.row (1), pca -> eigenvectors.row (2));
			autoMAT means2d = raw_MAT (my numberOfComponents, 2);

			double a, b, cs, sn;
			NUMeigencmp22 (s2d -> data [1] [1], s2d -> data [1] [2], s2d -> data [2] [2], & a, & b, & cs, & sn);

			const double a_scaled = nSigmas * sqrt (a);
			const double b_scaled = nSigmas * sqrt (b);
			const double angle_inc = NUM2pi / my numberOfComponents;
			double angle = 0.0;
			for (integer im = 1; im <= my numberOfComponents; im++, angle += angle_inc) {
				const double xc = a_scaled * (1.0 + NUMrandomUniform (-ru_range, ru_range)) * cos (angle);
				const double yc = b_scaled * (1.0 + NUMrandomUniform (-ru_range, ru_range)) * sin (angle);
				means2d [im] [1] = s2d -> centroid [1] + xc * cs - yc * sn;
				means2d [im] [2] = s2d -> centroid [2] + xc * sn + yc * cs;
			}
			/*
				Reconstruct the n-dimensional means from the 2-d pc's and the 2 eigenvectors
			*/
			autoMAT means = mul_MAT (means2d.get(), pca -> eigenvectors.horizontalBand (1, 2));

			for (integer im = 1; im <= my numberOfComponents; im ++) {
				const Covariance covi = my covariances->at [im];
				covi -> centroid.all() <<= means.row (im);
				covi -> numberOfObservations = thy numberOfRows / my numberOfComponents;
			}
			/*
				Trick: use the new means to get the between SSCP,
				if there is only one component the cov_b will be zero!
			*/
			autoCovariance cov_b = GaussianMixture_to_Covariance_between (me);
			const double var_t = SSCP_getTotalVariance (cov_t.get());
			const double var_b = SSCP_getTotalVariance (cov_b.get());
			if (var_b >= var_t) { // we have chosen our initial values too far out
				const double scale = 0.9 * sqrt (var_t / var_b);
				for (integer im = 1; im <= my numberOfComponents; im ++) {
					const Covariance covi = my covariances->at [im];
					for (integer ic = 1; ic <= my dimension; ic ++)
						covi -> centroid [ic] -= (1.0 - scale) * (covi -> centroid [ic] - cov_b -> centroid [ic]);
				}
				cov_b = GaussianMixture_to_Covariance_between (me);
			}
			/*
				Within variances are now (total - between) / numberOfComponents;
			*/
			if (my numberOfComponents > 1)
				cov_t -> data.all()  *=  var_b / var_t / my numberOfComponents;
			/*
				Copy them
			*/
			for (integer im = 1; im <= my numberOfComponents; im ++) {
				const Covariance cov = my covariances->at [im];
				if (cov -> numberOfRows == 1)
					cov -> data.row(1) <<= cov_t -> data.diagonal();
				 else
					cov -> data.all() <<= cov_t -> data.all();
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no initial guess possible.");
	}
}

autoClassificationTable GaussianMixture_TableOfReal_to_ClassificationTable (GaussianMixture me, TableOfReal thee) {
	try {
		autoClassificationTable him = ClassificationTable_create (thy numberOfRows, my numberOfComponents);
		for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
			const Covariance cov = my covariances->at [ic];
			SSCP_expandLowerCholeskyInverse (cov);
			TableOfReal_setColumnLabel (him.get(), ic, Thing_getName (cov));
		}

		const double ln2pid = -0.5 * my dimension * log (NUM2pi);
		autoVEC lnN = raw_VEC (my numberOfComponents);
		for (integer irow = 1; irow <=  thy numberOfRows; irow ++) {
			longdouble psum = 0.0;
			for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
				const Covariance cov = my covariances->at [ic];
				const double dsq = NUMmahalanobisDistanceSquared (cov -> lowerCholeskyInverse.get(), thy data.row (irow), cov -> centroid.get());
				lnN [ic] = ln2pid - 0.5 * (cov -> lnd + dsq);
				psum += his data [irow] [ic] = my mixingProbabilities [ic] * exp (lnN [ic]);
			}
			if (psum == 0.0) { // p's might be too small (underflow), make the largest equal to sfmin
				double lnmax = -1e308;
				integer imm = 1;
				for (integer ic = 1; ic <= my numberOfComponents; ic ++) {
					if (lnN [ic] > lnmax)
						lnmax = lnN [ic];
					imm = ic;
				}
				his data [irow] [imm] = NUMfpp -> sfmin;
			}

			// for (integer im = 1; im <= my numberOfComponents; im++) { his data [i] [im] /= psum; }

			TableOfReal_setRowLabel (him.get(), irow, thy rowLabels [irow].get());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"No ClassificationTable created from GaussianMixture & TableOfReal.");
	}
}

void GaussianMixture_splitComponent (GaussianMixture me, integer component) {
	try {
		Melder_require (component > 0 && component <= my numberOfComponents,
			U"The component should be in [1, ", my numberOfComponents, U"].");
		
		Covariance thee = my covariances->at [component];
		// Always new PCA because we cannot be sure of data unchanged.
		SSCP_expandPCA (thee);
		autoCovariance cov1 = Data_copy (thee);
		autoCovariance cov2 = Data_copy (thee);
		SSCP_unExpandPCA (cov1.get());
		SSCP_unExpandPCA (cov2.get());

		// Eventually cov1 replaces component, cov2 at end

		autoVEC mixingProbabilities = raw_VEC (my numberOfComponents + 1);
		for (integer i = 1; i <= my numberOfComponents; i ++)
			mixingProbabilities [i] = my mixingProbabilities [i];

		const double gamma = 0.5, lambda = 0.5, eta = 0.5, mu = 0.5;
		mixingProbabilities [component] = gamma * my mixingProbabilities [component];
		mixingProbabilities [my numberOfComponents + 1] = (1.0 - gamma) * my mixingProbabilities [component];
		const double mp12 =  mixingProbabilities [component] / mixingProbabilities [my numberOfComponents + 1];
		const double factor1 = (eta - eta * lambda * lambda - 1.0) / gamma + 1.0;
		const double factor2 = (eta * lambda * lambda - eta - lambda * lambda) / (1.0 - gamma) + 1.0;
		constVEC ev = thy pca -> eigenvectors.row (1);
		const double d2 = thy pca -> eigenvalues [1];

		for (integer i = 1; i <= my dimension; i ++) {
			cov1 -> centroid [i] -= (1.0 / sqrt (mp12)) * sqrt (d2) * mu * ev [i];
			cov2 -> centroid [i] +=        sqrt (mp12)  * sqrt (d2) * mu * ev [i];

			if (thy numberOfRows == 1) { // diagonal
				cov1 -> data [1] [i] = cov1 -> data [1] [i] / mp12 + factor1 * d2;
				cov1 -> data [1] [i] = cov2 -> data [i] [i] * mp12 + factor2 * d2;
			} else {
				for (integer j = i; j <= my dimension; j++) {
					cov1 -> data [j] [i] = cov1 -> data [i] [j] = cov1 -> data [i] [j] / mp12 + factor1 * d2 * ev [i] * ev [j];
					cov2 -> data [j] [i] = cov2 -> data [i] [j] = cov2 -> data [i] [j] * mp12 + factor2 * d2 * ev [i] * ev [j];
				}
			}
		}
		cov1 -> numberOfObservations *= gamma;
		cov2 -> numberOfObservations *= 1.0 - gamma;

		// Replace cov1 at component + add cov2. If something goes wrong we should be able to restore original!
		try {
			Thing_setName (cov2.get(), Melder_cat (Thing_getName (cov2.get()), U"-", my numberOfComponents + 1));
			my covariances -> addItem_move (cov2.move());
		} catch (MelderError) {
			Melder_throw (me, U" cannot add new component.");
		}

		my covariances -> replaceItem_move (cov1.move(), component);

		my numberOfComponents ++;
		my mixingProbabilities = mixingProbabilities.move();
	} catch (MelderError) {
		Melder_throw (me, U": component ", component, U" cannot be split.");
	}
}

void GaussianMixture_TableOfReal_getComponentProbabilities (GaussianMixture me, TableOfReal thee, integer componentToUpdate, MAT const& probabilities) {
	try {
		Melder_require (probabilities.nrow == thy numberOfRows,
			U"The number of rows in the TableOfReal and the probabilities should be equal.");
		Melder_require (probabilities.ncol == my numberOfComponents,
			U"The number of columns in the TableOfReal and the probabilities should be equal.");
		Melder_require (my dimension == thy numberOfColumns,
			U"The number of columns in the TableOfReal and the dimension of the GaussianMixture should be equal.");
		Melder_require (componentToUpdate >= 0 && componentToUpdate <= my numberOfComponents,
			U"The component number should be in the interval from 0 to ", my numberOfComponents);
		const double ln2pid = my dimension * log (NUM2pi);

		const integer fromComponent = componentToUpdate == 0 ? 1 : componentToUpdate;
		const integer toComponent = componentToUpdate == 0 ? my numberOfComponents : componentToUpdate;
		
		for (integer component = fromComponent; component <= toComponent; component ++) {
			const Covariance covi = my covariances->at [component];
			SSCP_expandLowerCholeskyInverse (covi);

			for (integer irow = 1; irow <= thy numberOfRows; irow++) {
				const double dsq = NUMmahalanobisDistanceSquared (covi -> lowerCholeskyInverse.get(), thy data.row (irow), covi -> centroid.get());
				probabilities [irow] [component] = std::max (1e-300, exp (- 0.5 * (ln2pid + covi -> lnd + dsq))); // prevent probabilities from being zero
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no component probabilies could be calculated.");
	}
}

void GaussianMixture_TableOfReal_getResponsilities (GaussianMixture me, TableOfReal thee, MAT const& responsibilities) {
		Melder_require (responsibilities.nrow == thy numberOfRows,
			U"The number of rows in the TableOfReal and the responsibilities should be equal.");
		Melder_require (responsibilities.ncol == my numberOfComponents,
			U"The number of columns in the TableOfReal and the responsibilities should be equal.");
		Melder_require (my dimension == thy numberOfColumns,
			U"The number of columns in the TableOfReal and the dimension of the GaussianMixture should be equal.");
		autoMAT probabilities = raw_MAT (responsibilities.nrow, responsibilities.ncol);
		GaussianMixture_TableOfReal_getComponentProbabilities (me, thee, 0, probabilities);
		GaussianMixture_getResponsibilities (me, probabilities.get(), 0, responsibilities);
}

autoTableOfReal GaussianMixture_TableOfReal_to_TableOfReal_probabilities (GaussianMixture me, TableOfReal thee) {
	try {
		Melder_require (my dimension == thy numberOfColumns,
			U"The number of columns in the TableOfReal and the dimension of the GaussianMixture should be equal.");
		autoTableOfReal him = TableOfReal_create (thy numberOfRows, my numberOfComponents);
		his rowLabels.all() <<= thy rowLabels.all();
		TableOfReal_setSequentialColumnLabels (him.get(), 1, my numberOfComponents, U"c", 1, 1);
		GaussianMixture_TableOfReal_getComponentProbabilities (me, thee, 0, his data.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no probabilies could be calculated.");
	}
}

autoTableOfReal GaussianMixture_TableOfReal_to_TableOfReal_responsibilities (GaussianMixture me, TableOfReal thee) {
	try {
		autoTableOfReal him = GaussianMixture_TableOfReal_to_TableOfReal_probabilities (me, thee);
		GaussianMixture_getResponsibilities (me, his data.get(), 0, his data.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no responsibilities could be calculated.");
	}
}

void GaussianMixture_expandPCA (GaussianMixture me) {
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		const Covariance him = my covariances->at [im];
		Melder_require (his numberOfRows > 1,
			U"Nothing to expand.");
		his pca = SSCP_to_PCA (him);
	}
}

void GaussianMixture_unExpandPCA (GaussianMixture me) {
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		SSCP_unExpandPCA (my covariances->at [im]);
	}
}

void GaussianMixture_TableOfReal_improveLikelihood (GaussianMixture me, TableOfReal thee, double delta_lnp, integer maxNumberOfIterations, double lambda, kGaussianMixtureCriterion criterion) {
	try {
		Melder_require (thy numberOfColumns == my dimension,
			U"The number of columns and the dimension of the model should agree.");
		Melder_require (my numberOfComponents < thy numberOfRows / 2,
			U"Not enough data points.");

		const conststring32 criterionText = GaussianMixture_criterionText (criterion);

		// The global covariance matrix is added with scaling coefficient lambda during updating the
		// mixture covariances to prevent numerical instabilities.

		autoCovariance covg = TableOfReal_to_Covariance (thee);
		autoMAT probabilities = raw_MAT (thy numberOfRows, my numberOfComponents);
		autoMAT responsibilities = raw_MAT (thy numberOfRows, my numberOfComponents);
		
		GaussianMixture_TableOfReal_getComponentProbabilities (me, thee, 0, probabilities);

		double lnp = GaussianMixture_getLikelihoodValue (me, probabilities.get(), criterion);
		integer iter = 0;
		autoMelderProgress progress (U"Improve likelihood...");
		try {
			double lnp_prev, lnp_start = lnp / thy numberOfRows;
			do {
				iter ++;
				/*
					E-step: get responsibilities (gamma) with current parameters
					See C. Bishop (2006), Pattern reconition and machine learning, Springer, page 439...
				*/
				GaussianMixture_getResponsibilities (me, probabilities.get(), 0, responsibilities.get());

				lnp_prev = lnp;
				
				/*
					M-step: 1. new means & covariances
				*/
				for (integer component = 1; component <= my numberOfComponents; component ++) {
					GaussianMixture_updateComponent (me, component, thy data.get(), responsibilities.get());
					GaussianMixture_addCovarianceFraction (me, component, covg.get(), lambda);
				}

				/*
					M-step: 2. new mixingProbabilities
				*/
				autoVEC totalResponsibilities = columnSums_VEC (responsibilities.get());
				my mixingProbabilities.all() <<= totalResponsibilities.get();
				my mixingProbabilities.all()  *=  1.0 / responsibilities.nrow;
				
				GaussianMixture_TableOfReal_getComponentProbabilities (me, thee, 0, probabilities);
				
				lnp = GaussianMixture_getLikelihoodValue (me, probabilities.get(), criterion);
				Melder_progress ((double) iter / (double) maxNumberOfIterations, criterionText, U": ", lnp / thy numberOfRows, U", L0: ", lnp_start);
			} while (fabs (lnp - lnp_prev) > std::max (fabs (delta_lnp * lnp_prev), NUMeps) && iter < maxNumberOfIterations);
		} catch (MelderError) {
			Melder_clearError ();
		}

		// During EM, covariances were underestimated by a factor of (n-1)/n. Correction now.

		for (integer component = 1; component <= my numberOfComponents; component ++) {
			const Covariance cov = my covariances->at [component];
			if (cov -> numberOfObservations > 1.5)
				cov -> data.row (1)  *=  cov -> numberOfObservations / (cov -> numberOfObservations - 1.0);
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": likelihood cannot be improved.");
	}
}


double GaussianMixture_TableOfReal_getLikelihoodValue (GaussianMixture me, TableOfReal thee, kGaussianMixtureCriterion criterion) {
	autoMAT probabilities = raw_MAT (thy numberOfRows, my numberOfComponents);
	GaussianMixture_TableOfReal_getComponentProbabilities (me, thee, 0, probabilities.get());
	return GaussianMixture_getLikelihoodValue (me, probabilities.get(), criterion);
}

autoMAT newMATremoveColumn (constMAT const& m, integer columnToRemove) {
	Melder_require (m.ncol > 1,
		U"We cannot remove the last column.");
	Melder_require (columnToRemove > 0 && columnToRemove <= m.ncol,
		U"The column number should be in the range from 1 to ",m.ncol, U".");
	autoMAT resized = raw_MAT (m.nrow, m.ncol - 1);
	resized.verticalBand (1, columnToRemove - 1) <<= m.verticalBand (1, columnToRemove - 1);
	if (columnToRemove < m.ncol)
		resized.verticalBand (columnToRemove, m.ncol - 1) <<= m.verticalBand (columnToRemove + 1, m.ncol);
	return resized;
}

autoGaussianMixture TableOfReal_to_GaussianMixture_CEMM (TableOfReal me, integer minimumNumberOfComponents, integer maximumNumberOfComponents, kGaussianMixtureStorage storage, integer maximumNumberOfIterations, double tolerance, bool info) {
	try {
		autoGaussianMixture thee = GaussianMixture_create (maximumNumberOfComponents, my numberOfColumns, storage);
		GaussianMixture_setColumnLabelsFromTableOfReal (thee.get(), me);
		GaussianMixture_initialGuess (thee.get(), me);
		if (maximumNumberOfIterations <= 0)
			return thee;
		return GaussianMixture_TableOfReal_to_GaussianMixture_CEMM (thee.get(), me, minimumNumberOfComponents, maximumNumberOfIterations, tolerance, info);
	} catch (MelderError) {
		Melder_throw (me, U"GaussianMixture not created from TableOfReal.");
	}	
}

autoGaussianMixture GaussianMixture_TableOfReal_to_GaussianMixture_CEMM (GaussianMixture me, TableOfReal thee, integer minimumNumberOfComponents, integer maxNumberOfIterations, double tolerance, bool info) {
	try {
		Melder_require (thy numberOfColumns == my dimension,
			U"The number of columns in the TableOfReal and the dimension of the model should agree.");
		Melder_require (my numberOfComponents < thy numberOfRows / 2,
			U"Not enough data points.");
		kGaussianMixtureCriterion criterion = kGaussianMixtureCriterion::MESSAGE_LENGTH;
		const conststring32 criterionText = GaussianMixture_criterionText (criterion);
		const bool deleteWeakComponents = minimumNumberOfComponents > 0;
		autoGaussianMixture him = Data_copy (me);
		autoMAT probabilities = zero_MAT (thy numberOfRows, his numberOfComponents);
		autoMAT responsibilities = zero_MAT (thy numberOfRows, his numberOfComponents);

		autoCovariance covg = TableOfReal_to_Covariance (thee);

		const double npars = GaussianMixture_getNumberOfParametersInComponent (him.get());
		const double nparsd2 = ( deleteWeakComponents ? npars / 2.0 : 0.0 );

		// Initial E-step: Update all component probabilities.

		GaussianMixture_TableOfReal_getComponentProbabilities (him.get(), thee, 0, probabilities.get());
		GaussianMixture_getResponsibilities (me, probabilities.get(), 0, responsibilities.get());

		double lnew = GaussianMixture_getLikelihoodValue (him.get(), probabilities.get(), criterion);

		autoMelderProgress progress (U"Gaussian mixture...");
		autoGaussianMixture best = Data_copy (me);

		double lstart = lnew / thy numberOfRows;
		integer iter = 0, numberOfNonzeroComponents = his numberOfComponents;
		double lmax = -1e308, lprev;
		if (info)
			MelderInfo_writeLine (U"iter = 0, ML = ", lstart, U" ", criterionText);
		while (numberOfNonzeroComponents >= minimumNumberOfComponents) {
			do {
				iter ++;
				lprev = lnew;
				for (integer icomponent = 1; icomponent <= his numberOfComponents; icomponent ++) {

					GaussianMixture_getResponsibilities (him.get(), probabilities.get(), icomponent, responsibilities.get());
					// Now check if enough support for a component exists
					
					const double componentSupport = NUMsum (responsibilities.column (icomponent)) - nparsd2;
					double totalSupport = 0.0;
					for (integer component = 1; component <= his numberOfComponents; component ++)
						totalSupport += std::max (0.0, NUMsum (responsibilities.column (component)) - nparsd2);

					his mixingProbabilities [icomponent] = std::max (0.0, componentSupport / totalSupport);

					VECnormalize_inplace (his mixingProbabilities.get(), 1.0, 1.0); // redistribute probability mass

					if (his mixingProbabilities [icomponent] > 0.0) {
						// update probabilities for component
						GaussianMixture_updateComponent (him.get(), icomponent, thy data.get(), responsibilities.get());
						//if (lambda > 0)
						//	GaussianMixture_addCovarianceFraction (him.get(), icomponent, covg.get(), lambda);
						GaussianMixture_TableOfReal_getComponentProbabilities (him.get(), thee, icomponent, probabilities.get());
					} else {
						/*
							"Remove" the component from GaussianMixture, the probabilities and responsibilities
						*/
						if (numberOfNonzeroComponents > minimumNumberOfComponents) {
							numberOfNonzeroComponents --;
							probabilities.column (icomponent) <<= 0.0;
							responsibilities.column (icomponent) <<= 0.0;
							MATnormalizeRows_inplace (responsibilities.get(), 1.0, 1.0); // Maintain invariant
							if (info)
								MelderInfo_writeLine (U"iter = ", iter, U", component ", icomponent, U" removed.");
						}
					}
				}

				// L(theta,Y)=N/2 sum(m=1..k, log(n*mixingP [m]/12))+k/2log(n/12)+k/2(N+1)-loglikelihood reduces to:
				// k/2 (N+1){log(n/12)+1}+N/2sum(m=1..k,mixingP [m]) - loglikelihood

				lnew = GaussianMixture_getLikelihoodValue (him.get(), probabilities.get(), criterion);
				if (info)
					MelderInfo_writeLine (U"iter = ", iter, U", ML = ", lnew);
			} while (lnew > lprev && fabs (lprev - lnew) > std::max (tolerance * fabs (lnew), NUMeps) && iter < maxNumberOfIterations);
			if (lnew > lmax) {
				best = Data_copy (him.get());
				lmax = lnew;
				if (! deleteWeakComponents) {
					break;    // TODO was goto end; is dat hetzelfde?
				}
			}
			if (numberOfNonzeroComponents > minimumNumberOfComponents) { // remove smallest component
				integer componentToDelete = 1;
				double mpmin = 1.1; // 
				for (integer component = 1; component <= his numberOfComponents; component ++) {
					if (his mixingProbabilities [component] > 0 && his mixingProbabilities [component] < mpmin) {
						mpmin = his mixingProbabilities [component];
						componentToDelete = component;
					}
				}
				his mixingProbabilities [componentToDelete] = 0.0;
				numberOfNonzeroComponents --;
				probabilities.column (componentToDelete) <<= 0.0;
				responsibilities.column (componentToDelete) <<= 0.0;
				MATnormalizeRows_inplace (responsibilities.get(), 1.0, 1.0); // Maintain invariant
				if (info)
					MelderInfo_writeLine (U"iter = ", iter, U", component ", componentToDelete, U" removed (after).");
			} else {
				break;
			}
		}
		if (info)
			MelderInfo_drain();
		GaussianMixture_removeUnsupportedComponents (best.get());
		return best;
	} catch (MelderError) {
		Melder_throw (U"GaussianMixture not improved.");
	}
}

// The numberOfElements per covariance needs to be updated later
void GaussianMixture_removeComponent (GaussianMixture me, integer component) {
	if (my numberOfComponents == 1)
		return;
	Melder_require (component > 0 && component <= my numberOfComponents,
		U"The component number should be in the range from 1 to ", my numberOfComponents, U".");

	my covariances -> removeItem (component);
	my numberOfComponents --;

	for (integer ic = component; ic <= my numberOfComponents; ic ++)
		my mixingProbabilities [ic] = my mixingProbabilities [ic + 1];
	
	my mixingProbabilities. resize (my numberOfComponents);
	
	VECnormalize_inplace (my mixingProbabilities.get(), 1.0, 1.0);
}

autoGaussianMixture TableOfReal_to_GaussianMixture (TableOfReal me, integer numberOfComponents, double delta_lnp, integer maxNumberOfIterations, double lambda, kGaussianMixtureStorage storage, kGaussianMixtureCriterion criterion) {
	try {
		Melder_require (my numberOfRows >= 2 * numberOfComponents,
			U"The number of rows should at least be twice the number of components.");
		autoGaussianMixture thee = GaussianMixture_create (numberOfComponents, my numberOfColumns, storage);
		GaussianMixture_setColumnLabelsFromTableOfReal (thee.get(), me);
		GaussianMixture_initialGuess (thee.get(), me);
		if (maxNumberOfIterations <= 0)
			return thee;
		GaussianMixture_TableOfReal_improveLikelihood (thee.get(), me, delta_lnp, maxNumberOfIterations, lambda, criterion);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no GaussianMixture created.");
	}
}

autoCorrelation GaussianMixture_TableOfReal_to_Correlation (GaussianMixture me, TableOfReal thee) {
	try {
		Melder_require (my dimension == thy numberOfColumns,
			U"Dimensions should be equal.");
		autoClassificationTable ct = GaussianMixture_TableOfReal_to_ClassificationTable (me, thee);
		autoCorrelation him = ClassificationTable_to_Correlation_columns (ct.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Correlation not created from GaussianMixture & TableOfReal.");
	}
}

double GaussianMixture_getProbabilityAtPosition_string (GaussianMixture me, conststring32 vector_string) {
	autoSTRVEC vector = splitByWhitespace_STRVEC (vector_string);
	autoVEC pos = zero_VEC (my dimension);
	for (integer i = 1; i <= vector.size; i ++) {
		pos [i] = Melder_atof (vector [i].get());
		if (i == my dimension)
			break;
	}
	const double p = GaussianMixture_getProbabilityAtPosition (me, pos.get());
	return p;
}

double GaussianMixture_getMarginalProbabilityAtPosition (GaussianMixture me, constVECVU const& pos, double x) {
	longdouble p = 0.0;
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		const double pim = Covariance_getMarginalProbabilityAtPosition (my covariances->at [im], pos, x);
		p += my mixingProbabilities [im] * pim;
	}
	return (double) p;
}

double GaussianMixture_getProbabilityAtPosition (GaussianMixture me, constVEC const& xpos) {
	longdouble p = 0.0;
	for (integer im = 1; im <= my numberOfComponents; im ++) {
		const double pim = Covariance_getProbabilityAtPosition (my covariances->at [im], xpos);
		p += my mixingProbabilities [im] * pim;
	}
	return (double) p;
}

autoMatrix GaussianMixture_PCA_to_Matrix_density (GaussianMixture me, PCA thee, integer d1, integer d2, double xmin, double xmax, integer nx, double ymin, double ymax, integer ny) {
	try {
		Melder_require (my dimension == thy dimension,
			U"Dimensions should be equal.");
		Melder_require (d1 <= thy numberOfEigenvalues && d2 <= thy numberOfEigenvalues,
			U"Direction index too high.");
		
		autoVEC v = zero_VEC (my dimension);
		if (xmax == xmin || ymax == ymin) {
			double xmind, xmaxd, ymind, ymaxd, nsigmas = 2.0;

			GaussianMixture_PCA_getIntervalsAlongDirections (me, thee, d1, d2, nsigmas, &xmind, &xmaxd, &ymind, &ymaxd);
			if (xmax == xmin) {
				xmin = xmind;
				xmax = xmaxd;
			}
			if (ymax == ymin) {
				ymin = ymind;
				ymax = ymaxd;
			}
		}

		// xmin,xmax and ymin,ymax are coordinates in the pc1 vs pc2 plane

		const double dx = fabs (xmax - xmin) / nx, dy = fabs (ymax - ymin) / ny;
		const double x1 = xmin + 0.5 * dx, y1 = ymin + 0.5 * dy;
		autoMatrix him = Matrix_create (xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);

		for (integer i = 1; i <= ny; i ++) {
			const double y = y1 + (i - 1) * dy;
			for (integer j = 1; j <= nx; j ++) {
				const double x = x1 + (j - 1) * dx;
				for (integer k = 1; k <= my dimension; k ++)
					v [k] = x * thy eigenvectors [d1] [k] + y * thy eigenvectors [d2] [k];
				his z [i] [j] = GaussianMixture_getProbabilityAtPosition (me, v.get());
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no Matrix density created.");
	}
}

autoTableOfReal GaussianMixture_to_TableOfReal_randomSampling (GaussianMixture me, integer numberOfPoints) {
	try {
		const Covariance cov = my covariances->at [1];
		autoTableOfReal thee = TableOfReal_create (numberOfPoints, my dimension);
		autoVEC buf = raw_VEC (my dimension);
		thy columnLabels.all() <<= cov -> columnLabels.part (1, my dimension);
		for (integer i = 1; i <= numberOfPoints; i ++) {
			autostring32 covname;
			GaussianMixture_generateOneVector_inline (me, thy data.row (i), & covname, buf.get());
			TableOfReal_setRowLabel (thee.get(), i, covname.get());
		}
		GaussianMixture_unExpandPCA (me);
		return thee;
	} catch (MelderError) {
		GaussianMixture_unExpandPCA (me); 
		Melder_throw (U"TableOfReal with random sampling not created.");
	}
}

autoTable GaussianMixture_TableOfReal_to_Table_BHEPNormalityTests (GaussianMixture me, TableOfReal thee, double h) {
	try {
		Melder_require (thy numberOfColumns == my dimension,
			U"Dimensions should agree.");
		
		autoMAT responsibilities = raw_MAT (thy numberOfRows, my numberOfComponents);
		GaussianMixture_TableOfReal_getResponsilities (me, thee, responsibilities);
		autoVEC numberOfData = columnSums_VEC (responsibilities.get());
		
		autoTable him = Table_createWithColumnNames (my numberOfComponents, U"component probability smoothing statistic lnmu lnvar numberOfData dimension singular");

		for (integer component = 1; component <= my numberOfComponents; component ++) {
			const Covariance cov = my covariances->at [component];
			Table_setStringValue (him.get(), component, 1, Thing_getName (cov));
		}

		for (integer component = 1; component <= my numberOfComponents; component ++) {
			const Covariance cov = my covariances->at [component];
			autoVEC componentResponsibilities = copy_VEC (responsibilities.column (component));
			double testStatistic, lnmu, lnvar;
			bool isSingular;
			const double probability = Covariance_TableOfReal_normalityTest_BHEP (cov, thee, componentResponsibilities.get(), & h, & testStatistic, & lnmu, & lnvar, & isSingular);
		
			Table_setNumericValue (him.get(), component, 2, probability);
			Table_setNumericValue (him.get(), component, 3, h);
			Table_setNumericValue (him.get(), component, 4, testStatistic);
			Table_setNumericValue (him.get(), component, 5, lnmu);
			Table_setNumericValue (him.get(), component, 6, lnvar);
			Table_setNumericValue (him.get(), component, 7, numberOfData [component]);
			Table_setNumericValue (him.get(), component, 8, my dimension);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Table for BHEP not created.");
	}
}

/* End of file GaussianMixture.cpp 1555*/
