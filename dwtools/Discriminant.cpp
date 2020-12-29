/* Discriminant.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20011016 removed some causes for compiler warnings
 djmw 20020313 removed obsolete TableOfReal_sortByLabels method
 djmw 20020314 +Discriminant_extractWithinGroupSSCP,
 	+Discriminant_extractGroupLabels, +Discriminant_setGroupLabels.
 djmw 20020327 modified Discriminant_TableOfReal_to_Configuration
 djmw 20020418 Removed some causes for compiler warnings
 djmw 20020502 modified call Eigen_TableOfReal_project_into
 djmw 20030801 Discriminant_drawConcentrationEllipses extra argument
 djmw 20050405 Modified column label: eigenvector->Eigenvector
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071009 wchar
 djmw 20071012 Added: oo_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20081119 Check in TableOfReal_to_Discriminant if TableOfReal_areAllCellsDefined
 djmw 20100107 +Discriminant_TableOfReal_mahalanobis
 djmw 20110304 Thing_new
*/

#include "Discriminant.h"
#include "SSCP.h"
#include "Eigen_and_SSCP.h"
#include "NUM2.h"
#include "TableOfReal_extensions.h"

#include "oo_DESTROY.h"
#include "Discriminant_def.h"
#include "oo_COPY.h"
#include "Discriminant_def.h"
#include "oo_EQUAL.h"
#include "Discriminant_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Discriminant_def.h"
#include "oo_WRITE_TEXT.h"
#include "Discriminant_def.h"
#include "oo_READ_TEXT.h"
#include "Discriminant_def.h"
#include "oo_WRITE_BINARY.h"
#include "Discriminant_def.h"
#include "oo_READ_BINARY.h"
#include "Discriminant_def.h"
#include "oo_DESCRIPTION.h"
#include "Discriminant_def.h"

Thing_implement (Discriminant, Daata, 1);

void structDiscriminant :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of groups: ", numberOfGroups);
	MelderInfo_writeLine (U"Number of eigenvalues: ", eigen -> numberOfEigenvalues);
	MelderInfo_writeLine (U"Dimension of eigenvector: ", eigen -> dimension);
	MelderInfo_writeLine (U"Number of discriminant functions: ", Discriminant_getNumberOfFunctions (this));
	MelderInfo_writeLine (U"Number of observations (total): ", Discriminant_getNumberOfObservations (this, 0));
}

autoDiscriminant Discriminant_create (integer numberOfGroups, integer numberOfEigenvalues, integer dimension) {
	try {
		autoDiscriminant me = Thing_new (Discriminant);
		my eigen = Eigen_create (numberOfEigenvalues, dimension);
		my numberOfGroups = numberOfGroups;
		my groups = SSCPList_create ();
		my total = SSCP_create (dimension);
		my aprioriProbabilities = raw_VEC (numberOfGroups);
		my costs = raw_MAT (numberOfGroups, numberOfGroups);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Discriminant not created.");
	}
}

integer Discriminant_groupLabelToIndex (Discriminant me, conststring32 label) {
	for (integer i = 1; i <= my numberOfGroups; i ++) {
		const conststring32 name = Thing_getName (my groups -> at [i]);
		if (name && str32equ (name, label))
			return i;
	}
	return 0;
}

integer Discriminant_getNumberOfGroups (Discriminant me) {
	return my numberOfGroups;
}

integer Discriminant_getNumberOfObservations (Discriminant me, integer group) {
	if (group == 0) {
		return Melder_ifloor (my total -> numberOfObservations);
	} else if (group >= 1 && group <= my numberOfGroups) {
		SSCP sscp = my groups->at [group];
		return Melder_ifloor (sscp -> numberOfObservations);
	} else {
		return -1;
	}
}

void Discriminant_setAprioriProbability (Discriminant me, integer group, double p) {
	Melder_require (group > 0 && group <= my numberOfGroups,
		U"The group number (", group, U") should be in the interval [1, ", my numberOfGroups, U"]; the supplied value (", group, U") falls outside it.");
	Melder_require (p >= 0.0 && p <= 1.0,
		U"The probability should be in the interval [0, 1]");

	my aprioriProbabilities [group] = p;
}

integer Discriminant_getNumberOfFunctions (Discriminant me) {
	integer numberOfFunctions = std::min (my numberOfGroups - 1, my eigen -> dimension);
	numberOfFunctions = std::min (numberOfFunctions, my eigen ->  numberOfEigenvalues);
	return numberOfFunctions;
}

void Discriminant_setGroupLabels (Discriminant me, Strings thee) {
	Melder_require (my numberOfGroups == thy numberOfStrings,
		U"The number of strings should equal the number of groups.");
	for (integer i = 1; i <= my numberOfGroups; i ++) {
		const conststring32 name = thy strings [i].get();
		Thing_setName ( my groups->at [i], name ? name : U"" );
	}
}

autoStrings Discriminant_extractGroupLabels (Discriminant me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my numberOfGroups);
		thy numberOfStrings = my numberOfGroups;
		for (integer i = 1; i <= my numberOfGroups; i ++) {
			const conststring32 name = Thing_getName (my groups->at [i]);
			thy strings [i] = Melder_dup (name);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": group labels not extracted.");
	}
}

autoTableOfReal Discriminant_extractGroupCentroids (Discriminant me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my groups -> size, my eigen -> dimension);

		for (integer i = 1; i <= my groups -> size; i ++) {
			const SSCP sscp = my groups->at [i];
			TableOfReal_setRowLabel (thee.get(), i, Thing_getName (sscp));
			thy data.row (i) <<= sscp -> centroid.all();
		}
		thy columnLabels.all() <<= my groups->at [my groups -> size] -> columnLabels.part (1, my eigen -> dimension);
		// The elements in my groups always have my eigen -> dimension columns
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": group centroids not extracted.");
	}
}

autoTableOfReal Discriminant_extractGroupStandardDeviations (Discriminant me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my groups->size, my eigen -> dimension);

		for (integer i = 1; i <= my groups->size; i ++) {
			const SSCP sscp = my groups->at [i];
			TableOfReal_setRowLabel (thee.get(), i, Thing_getName (sscp));
			const integer numberOfObservationsm1 = Melder_ifloor (sscp -> numberOfObservations) - 1;
			for (integer j = 1; j <= my eigen -> dimension; j ++) {
				thy data [i] [j] = ( numberOfObservationsm1 > 0.0 ? sqrt (sscp -> data [j] [j] / numberOfObservationsm1) : undefined );
			}
		}
		thy columnLabels.all() <<= my groups->at [my groups->size] -> columnLabels.part (1, my eigen -> dimension);
		// The elements in my groups always have my eigen -> dimension columns

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": group standard deviations not extracted.");
	}
}

double Discriminant_getWilksLambda (Discriminant me, integer from) {
	integer numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	if (from >= numberOfFunctions)
		return 1;
	if (from < 1)
		from = 1;
	return NUMwilksLambda (my eigen -> eigenvalues.get(), 1 + from, numberOfFunctions);
}

/*
	raw r [j]: eigenvec [i] [j]
	unstandardized u [j]: sqrt(N-g) * r [j]
	standardized s [j]: u [j] sqrt (w [i] [i] / (N-g))
*/
autoTableOfReal Discriminant_extractCoefficients (Discriminant me, integer choice) {
	try {
		bool raw = choice == 0, standardized = choice == 2;
		const integer nx = my eigen -> dimension, ny = my eigen -> numberOfEigenvalues;

		const SSCP total = my total.get();
		autoTableOfReal thee = TableOfReal_create (ny, nx + 1);
		thy columnLabels.part (1, nx) <<= my total -> columnLabels.part (1, nx);
		// The elements in my groups always have my eigen -> dimension columns

		autoSSCP within;
		if (standardized)
			within = Discriminant_extractPooledWithinGroupsSSCP (me);

		TableOfReal_setColumnLabel (thee.get(), nx + 1, U"constant");
		TableOfReal_setSequentialRowLabels (thee.get(), 1, ny, U"function_", 1, 1);

		double scale = sqrt (total -> numberOfObservations - my numberOfGroups);
		//double *centroid = my total -> centroid.at;
		for (integer i = 1; i <= ny; i ++) {
			longdouble u0 = 0.0;
			for (integer j = 1; j <= nx; j ++) {
				if (standardized)
					scale = sqrt (within -> data [j] [j]);
				const double ui = scale * my eigen -> eigenvectors [i] [j];
				thy data [i] [j] = ui;
				u0 += ui * my total -> centroid [j];
			}
			thy data [i] [nx + 1] = ( raw ? 0.0 : - (double) u0 );
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": coefficients not extracted.");
	}
}

static double Discriminant_getDegreesOfFreedom (Discriminant me) {
	double ndf = 0.0;
	for (integer i = 1; i <= my groups->size; i ++) {
		ndf += SSCP_getDegreesOfFreedom (my groups->at [i]);
	}
	return ndf;
}

void Discriminant_getPartialDiscriminationProbability (Discriminant me, integer numberOfWantedDimensions, double *out_prob, double *out_chisq, double *out_df)
{
	const integer eigendimension = my eigen -> dimension, numberOfGroups = my numberOfGroups;
	const integer numberOfFunctions = Discriminant_getNumberOfFunctions (me);
	const double degreesOfFreedom = Discriminant_getDegreesOfFreedom (me);

	double prob = undefined, chisq = undefined, df = undefined;

	if (numberOfWantedDimensions < numberOfFunctions) {
		const double lambda = NUMwilksLambda (my eigen -> eigenvalues.get(), numberOfWantedDimensions + 1, numberOfFunctions);
		if (lambda != 1.0) {
			chisq = - (degreesOfFreedom + (numberOfGroups - eigendimension) / 2.0 - 1.0) * log (lambda);
			df = (eigendimension - numberOfWantedDimensions) * (numberOfGroups - numberOfWantedDimensions - 1);
			if (out_prob)
				prob =  NUMchiSquareQ (chisq, df);
		}
	}
	if (out_prob)
		*out_prob = prob;
	if (out_chisq)
		*out_chisq = chisq;
	if (out_df)
		*out_df = df;
}

double Discriminant_getConcentrationEllipseArea (Discriminant me, integer groupNumber, double scale, bool confidence, bool discriminantDirections, integer d1, integer d2) {
	double area = undefined;

	if (groupNumber < 1 || groupNumber > my numberOfGroups)
		return area;

	if (discriminantDirections) {
		autoSSCP thee = Eigen_SSCP_project (my eigen.get(), my groups->at [groupNumber]);
		area = SSCP_getConcentrationEllipseArea (thee.get(), scale, confidence, d1, d2);
	} else {
		area = SSCP_getConcentrationEllipseArea (my groups->at [groupNumber], scale, confidence, d1, d2);
	}
	return area;
}

double Discriminant_getLnDeterminant_group (Discriminant me, integer groupNumber) {
	if (groupNumber < 1 || groupNumber > my numberOfGroups)
		return undefined;

	autoCovariance c = SSCP_to_Covariance (my groups->at [groupNumber], 1);
	const double ln_d = SSCP_getLnDeterminant (c.get());
	return ln_d;
}

double Discriminant_getLnDeterminant_total (Discriminant me) {
	autoCovariance c = SSCP_to_Covariance (my total.get(), 1);
	const double ln_d = SSCP_getLnDeterminant (c.get());
	return ln_d;
}

autoSSCP Discriminant_extractPooledWithinGroupsSSCP (Discriminant me) {
	return SSCPList_to_SSCP_pool (my groups.get());
}

autoSSCP Discriminant_extractWithinGroupSSCP (Discriminant me, integer groupNumber) {
	try {
		Melder_require (groupNumber >= 1 && groupNumber <= my numberOfGroups,
			U"The group number should be between 1 and ", my numberOfGroups, U".");
		autoSSCP thee = Data_copy (my groups->at [groupNumber]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": within-group SSCP not created.");
	}
}

autoSSCP Discriminant_extractBetweenGroupsSSCP (Discriminant me) {
	try {
		autoSSCP between = Data_copy (my total.get());   // for the moment, `between` contains the total sums of squares
		autoSSCP within = SSCPList_to_SSCP_pool (my groups.get());
		between -> data.get()  -=  within -> data.get();   // now, `between` does contain the between-groups sums of squares
		return between;
	} catch (MelderError) {
		Melder_throw (me, U": between-groups SSCP not created.");
	}
}

void Discriminant_drawTerritorialMap (Discriminant me, Graphics g, bool discriminantDirections,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, double fontSize, bool poolCovarianceMatrices, bool garnish)
{
	(void) me; (void) g; (void) discriminantDirections; (void) d1; (void) d2;
	(void) xmin; (void) xmax; (void) ymin;
	(void) ymax; (void) fontSize; (void) poolCovarianceMatrices; (void) garnish;

}

void Discriminant_drawConcentrationEllipses (Discriminant me, Graphics g, double scale, bool confidence,
	conststring32 label, bool discriminantDirections,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, double fontSize, bool garnish) {
	const integer numberOfFunctions = Discriminant_getNumberOfFunctions (me);

	if (! discriminantDirections) {
		SSCPList_drawConcentrationEllipses (my groups.get(), g, scale, confidence, label, d1, d2, xmin, xmax, ymin, ymax, fontSize, garnish);
		return;
	}

	Melder_require (numberOfFunctions > 1,
		U"Nothing drawn because there is only one dimension in the discriminant space.");

	// Project SSCPs on eigenvectors.

	if (d1 == 0 && d2 == 0) {
		d1 = 1;
		d2 = std::min (numberOfFunctions, d1 + 1);
	} else if (d1 < 0 || d2 > numberOfFunctions) {
		return;
	}

	autoSSCPList thee = SSCPList_toTwoDimensions (my groups.get(), my eigen -> eigenvectors.row (d1), my eigen -> eigenvectors.row (d2));

	SSCPList_drawConcentrationEllipses (thee.get(), g, scale, confidence, label, 1, 2, xmin, xmax, ymin, ymax, fontSize, 0);

	if (garnish) {
		char32 llabel [40];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"function ", d2);
		Graphics_textLeft (g, true, llabel);
		Graphics_marksBottom (g, 2, true, true, false);
		Melder_sprint (llabel,40, U"function ", d1);
		Graphics_textBottom (g, true, llabel);
	}
}

/* End of file Discriminant.cpp */
