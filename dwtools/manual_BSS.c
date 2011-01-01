/* manual_BSS.c
 *
 * Copyright (C) 2010 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
	djmw 20101227 Initial version
*/

#include "ManPagesM.h"

void manual_BSS (ManPages me);
void manual_BSS (ManPages me)
{
MAN_BEGIN (L"CrossCorrelationTable", L"djmw", 20101229)
INTRO (L"One of the types of objects in Praat. A CrossCorrelationTable represents the cross-correlations between "
	"a number of signals. Cell [%i,%j] of a CrossCorrelationTable contains the cross-correlation between the %i-th "
	"and the %j-th signal. For example, the CrossCorrelationTable of an %n-channel sound is a %n\\xx%n table where "
	"the number in cell [%i,%j] is the cross-correlation of channel %i with channel %j (for a particular lag time %\\ta).")
NORMAL (L"A CrossCorrelationTable has a square matrix whose cells contain the cross-correlations between "
	"the signals and a centroid vector with the average value of each signal.")
ENTRY (L"Remarks")
NORMAL (L"Sometimes in the statistical literature, the cross-correlation between signals is also called "
	"\"covariance\". However, the only thing a @@Covariance@ has in common with a CrossCorrelationTable is that "
	"both are symmetric matrices. The differences between a CrossCorrelationTable and a Covariance are:")
TAG (L"1. a Covariance matrix is always positive-definite; for a cross-correlation table this is only guaranteed if "
	"the lag time %\\ta = 0.")
TAG (L"2. The elements %%c__ij_% in a Covariance always satisfy |%%c__ij_%/\\Vr(%%c__ii_%\\.c%%c__jj_%)| \\<_ 1; this is "
	"generally not the case for cross-correlations.")
MAN_END

MAN_BEGIN (L"CrossCorrelationTables", L"djmw", 20101227)
INTRO (L"One of the types of objects in Praat. A CrossCorrelationTables represents a collection of @@CrossCorrelationTable@ objects.")
MAN_END

MAN_BEGIN (L"CrossCorrelationTables: Create test set...", L"djmw", 20101227)
INTRO (L"Create a collection of @@CrossCorrelationTable@s that are all derived from different diagonal matrices by the same transformation matrix.")
NORMAL (L"")
ENTRY (L"Settings")
TAG (L"##Matrix dimension")
DEFINITION (L"determines the size of the square matrix with cross-correlations.")
TAG (L"##Number of matrices")
DEFINITION (L"determines the number of matrices that have to be generated.")
TAG (L"##First is positive-definite")
DEFINITION (L"guarantees that the first matrix of the series is positive definite.")
TAG (L"##Sigma")
DEFINITION (L"the standard deviation of the noise that is added to each transformation matrix element. A value "
	"of zero makes all the cross-correlation matrices jointly diagonalizable. A value greater than zero "
	"makes each transformation matrix a little different and the collection not jointly "
	"diagonalizable anymore.")
ENTRY (L"Algorithm")
NORMAL (L"All the CrossCorrelationTable matrices are generated as #V\\'p\\.c#D__%k_\\.c #V, where #D__%k_ is a diagonal matrix "
	"with entries randomly choosen from the [-1,1] interval. The matrix #V is a \"random\" orthogonal matrix "
	"obtained from the singular value decomposition of a matrix #M = #U\\.c#D\\.c#V\\'p, where the cells of the "
	"matrix #M are random Gaussian numbers with mean 0 and standard deviation 1.")
NORMAL (L"If the first matrix has to be positive definite, the numbers on the diagonal of #D__1_ are randomly "
	"chosen from the [0.1,1] interval.")
MAN_END

MAN_BEGIN (L"Sound: To Sound (blind source separation)...", L"djmw", 20101229)
INTRO (L"Analyze the selected multi-channel sound into its independent components by an iteartive method.")
NORMAL (L"The method to find the independent components tries to simultaneously diagonalize a number of "
	"@@CrossCorrelationTable@s that are calculated from the multi-channel sound at different lag times.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"defines the time range over which the ##CrossCorrelationTable#s of the sound will be calculated.")
TAG (L"##Number of cross-correlations")
DEFINITION (L"defines the number of ##CrossCorrelationTable#s to be calculated.")
TAG (L"##Lag times")
DEFINITION (L"defines the lag time %\\ta__0_ for the ##CrossCorrelationTable#s. These tables "
	"are calculated at lag times %\\ta__k_=(%k - 1)%\\ta__0_, where %k runs from 1 to %%numberOfCrosscorrelations%.")
TAG (L"##Maximum number of iterations")
DEFINITION (L"defines a stopping criterion for the iteration. The iteration will stops when this number is reached.")
TAG (L"##Tolerance")
DEFINITION (L"defines another stopping criterion that depends on the method used.")
TAG (L"##Diagonalization method")
DEFINITION (L"defines the method to determine the independent components.")
ENTRY (L"Algorithm")
NORMAL (L"The ##qdiag# method is described in @@Vollgraf & Obermayer (2006)@, the ##ffdiag# algorithm in @@Ziehe et al. (2004)@.")
MAN_END

MAN_BEGIN (L"Vollgraf & Obermayer (2006)", L"djmw", 20101229)
NORMAL (L"Roland Vollgraf & Klaus Obermayer (2006): \"Quadratic optimization for simultaneous matrix "
	"diagonalization.\" %%IEEE Transactions On Signal Processing% #54: 3270\\--3278.")
MAN_END

MAN_BEGIN (L"Ziehe et al. (2004)", L"djmw", 20101229)
NORMAL (L"Andreas Ziehe, Pavel Laskov, Guido Nolte & Klaus-Robert Müller (2004): \"A fast algorithm for joint "
	"diagonalization with non-orthogonal transformations and its application to blind source separation.\" "
	"%%Journal of Machine Learning Research% #5: 777\\-–800.")
MAN_END

}

/* End of file manual_BSS.c */

