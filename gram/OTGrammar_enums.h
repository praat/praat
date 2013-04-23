/* OTGrammar_enums.h
 *
 * Copyright (C) 2006-2011,2013 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

enums_begin (kOTGrammar_decisionStrategy, 0)
	enums_add (kOTGrammar_decisionStrategy, 0, OPTIMALITY_THEORY, L"OptimalityTheory")
	enums_add (kOTGrammar_decisionStrategy, 1, HARMONIC_GRAMMAR, L"HarmonicGrammar")
	enums_add (kOTGrammar_decisionStrategy, 2, LINEAR_OT, L"LinearOT")
	enums_add (kOTGrammar_decisionStrategy, 3, EXPONENTIAL_HG, L"ExponentialHG")
	enums_add (kOTGrammar_decisionStrategy, 4, MAXIMUM_ENTROPY, L"MaximumEntropy")
	enums_add (kOTGrammar_decisionStrategy, 5, POSITIVE_HG, L"PositiveHG")
	enums_add (kOTGrammar_decisionStrategy, 6, EXPONENTIAL_MAXIMUM_ENTROPY, L"ExponentialMaximumEntropy")
enums_end (kOTGrammar_decisionStrategy, 6, OPTIMALITY_THEORY)

enums_begin (kOTGrammar_rerankingStrategy, 0)
	enums_add (kOTGrammar_rerankingStrategy, 0, DEMOTION_ONLY, L"Demotion only")
	enums_add (kOTGrammar_rerankingStrategy, 1, SYMMETRIC_ONE, L"Symmetric one")
	enums_add (kOTGrammar_rerankingStrategy, 2, SYMMETRIC_ALL, L"Symmetric all")
	enums_add (kOTGrammar_rerankingStrategy, 3, WEIGHTED_UNCANCELLED, L"Weighted uncancelled")
	enums_add (kOTGrammar_rerankingStrategy, 4, WEIGHTED_ALL, L"Weighted all")
	enums_add (kOTGrammar_rerankingStrategy, 5, EDCD, L"EDCD")
	enums_add (kOTGrammar_rerankingStrategy, 6, EDCD_WITH_VACATION, L"EDCD with vacation")
	enums_add (kOTGrammar_rerankingStrategy, 7, WEIGHTED_ALL_UP_HIGHEST_DOWN, L"Weighted all up, highest down")
	enums_add (kOTGrammar_rerankingStrategy, 8, WEIGHTED_ALL_UP_HIGHEST_DOWN_2012, L"Weighted all up, highest down (2012)")
	enums_add (kOTGrammar_rerankingStrategy, 9, WEIGHTED_ALL_UP_HIGH_DOWN, L"Weighted all up, high down")
	enums_add (kOTGrammar_rerankingStrategy, 10, WEIGHTED_ALL_UP_HIGH_DOWN_2012, L"Weighted all up, high down (2012)")
enums_end (kOTGrammar_rerankingStrategy, 10, SYMMETRIC_ALL)

/* End of file OTGrammar_enums.h */
