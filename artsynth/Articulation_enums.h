/* Articulation_enums.h
 *
 * Copyright (C) 1992-2005,2009,2013,2015-2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kArt_muscle, 0)
	enums_add (kArt_muscle, 0, _, U"_")
	enums_add (kArt_muscle, 1, LUNGS, U"Lungs")

	enums_add (kArt_muscle, 2, INTERARYTENOID, U"Interarytenoid")   // constriction of larynx; 0 = breathing, 1 = constricted glottis
	enums_add (kArt_muscle, 3, CRICOTHYROID, U"Cricothyroid")   // vocal-cord tension
	enums_add (kArt_muscle, 4, VOCALIS, U"Vocalis")   // vocal-cord tension
	enums_add (kArt_muscle, 5, THYROARYTENOID, U"Thyroarytenoid")
	enums_add (kArt_muscle, 6, POSTERIOR_CRICOARYTENOID, U"PosteriorCricoarytenoid")   // opening of glottis
	enums_add (kArt_muscle, 7, LATERAL_CRICOARYTENOID, U"LateralCricoarytenoid")   // opening of glottis

	enums_add (kArt_muscle, 8, STYLOHYOID, U"Stylohyoid")   // up movement of hyoid bone
	enums_add (kArt_muscle, 9, STERNOHYOID, U"Sternohyoid")   // down movement of hyoid bone

	enums_add (kArt_muscle, 10, THYROPHARYNGEUS, U"Thyropharyngeus")   // constriction of ventricular folds
	enums_add (kArt_muscle, 11, LOWER_CONSTRICTOR, U"LowerConstrictor")
	enums_add (kArt_muscle, 12, MIDDLE_CONSTRICTOR, U"MiddleConstrictor")
	enums_add (kArt_muscle, 13, UPPER_CONSTRICTOR, U"UpperConstrictor")
	enums_add (kArt_muscle, 14, SPHINCTER, U"Sphincter")   // constriction of pharynx

	enums_add (kArt_muscle, 15, HYOGLOSSUS, U"Hyoglossus")   // down movement of tongue body
	enums_add (kArt_muscle, 16, STYLOGLOSSUS, U"Styloglossus")   // up movement of tongue body
	enums_add (kArt_muscle, 17, GENIOGLOSSUS, U"Genioglossus")   // forward movement of tongue body

	enums_add (kArt_muscle, 18, UPPER_TONGUE, U"UpperTongue")   // up curling of the tongue tip
	enums_add (kArt_muscle, 19, LOWER_TONGUE, U"LowerTongue")   // down curling of the tongue
	enums_add (kArt_muscle, 20, TRANSVERSE_TONGUE, U"TransverseTongue")   // thickening of tongue
	enums_add (kArt_muscle, 21, VERTICAL_TONGUE, U"VerticalTongue")   // thinning of tongue

	enums_add (kArt_muscle, 22, RISORIUS, U"Risorius")   // spreading of lips
	enums_add (kArt_muscle, 23, ORBICULARIS_ORIS, U"OrbicularisOris")   // rounding of lips

	enums_add (kArt_muscle, 24, LEVATOR_PALATINI, U"LevatorPalatini")   // closing of velo-pharyngeal port; 0 = open ("nasal"), 1 = closed ("oral")
	enums_add (kArt_muscle, 25, TENSOR_PALATINI, U"TensorPalatini")

	enums_add (kArt_muscle, 26, MASSETER, U"Masseter")   // closing of jaw; 0 = open, 1 = closed
	enums_add (kArt_muscle, 27, MYLOHYOID, U"Mylohyoid")   // opening of jaw
	enums_add (kArt_muscle, 28, LATERAL_PTERYGOID, U"LateralPterygoid")   // horizontal jaw position

	enums_add (kArt_muscle, 29, BUCCINATOR, U"Buccinator")   // oral wall tension
enums_end (kArt_muscle, 29, LUNGS)

/* End of file Articulation.enums */
