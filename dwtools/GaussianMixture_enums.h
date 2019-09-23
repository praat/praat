/* GaussianMixture_enums.h
 *
 * Copyright (C) 2019 David Weenink
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

enums_begin (kGaussianMixtureStorage, 1)
	enums_add (kGaussianMixtureStorage, 1, Complete, U"Complete")
	enums_add (kGaussianMixtureStorage, 2, Diagonals, U"Diagonals")
enums_end (kGaussianMixtureStorage, 2, Diagonals)

enums_begin (kGaussianMixtureCriterion, 1)
	enums_add (kGaussianMixtureCriterion, 1, Likelihood, U"Likelihood")
	enums_add (kGaussianMixtureCriterion, 2, MessageLength, U"MessageLength")
	enums_add (kGaussianMixtureCriterion, 3, BayesInformation, U"Bayes information")
	enums_add (kGaussianMixtureCriterion, 4, AkaikeInformation, U"Akaike information")
	enums_add (kGaussianMixtureCriterion, 5, AkaikeCorrected, U"Akaike corrected")
	enums_add (kGaussianMixtureCriterion, 6, CompleteDataML, U"Complete-data ML")
enums_end (kGaussianMixtureCriterion, 6, CompleteDataML)

/* End of file GaussianMixture_enums.h */
