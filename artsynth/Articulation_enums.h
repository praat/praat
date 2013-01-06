/* Articulation_enums.h
 *
 * Copyright (C) 1992-2009 Paul Boersma
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

enums_begin (kArt_muscle, 0)
	enums_add (kArt_muscle, 0, _, L"_")
	enums_add (kArt_muscle, 1, LUNGS, L"Lungs")

	enums_add (kArt_muscle, 2, INTERARYTENOID, L"Interarytenoid")   // constriction of larynx; 0 = breathing, 1 = constricted glottis
	enums_add (kArt_muscle, 3, CRICOTHYROID, L"Cricothyroid")   // vocal-cord tension
	enums_add (kArt_muscle, 4, VOCALIS, L"Vocalis")   // vocal-cord tension
	enums_add (kArt_muscle, 5, THYROARYTENOID, L"Thyroarytenoid")
	enums_add (kArt_muscle, 6, POSTERIOR_CRICOARYTENOID, L"PosteriorCricoarytenoid")   // opening of glottis
	enums_add (kArt_muscle, 7, LATERAL_CRICOARYTENOID, L"LateralCricoarytenoid")   // opening of glottis

	enums_add (kArt_muscle, 8, STYLOHYOID, L"Stylohyoid")   // up movement of hyoid bone
	enums_add (kArt_muscle, 9, STERNOHYOID, L"Sternohyoid")   // down movement of hyoid bone

	enums_add (kArt_muscle, 10, THYROPHARYNGEUS, L"Thyropharyngeus")   // constriction of ventricular folds
	enums_add (kArt_muscle, 11, LOWER_CONSTRICTOR, L"LowerConstrictor")
	enums_add (kArt_muscle, 12, MIDDLE_CONSTRICTOR, L"MiddleConstrictor")
	enums_add (kArt_muscle, 13, UPPER_CONSTRICTOR, L"UpperConstrictor")
	enums_add (kArt_muscle, 14, SPHINCTER, L"Sphincter")   // constriction of pharynx

	enums_add (kArt_muscle, 15, HYOGLOSSUS, L"Hyoglossus")   // down movement of tongue body
	enums_add (kArt_muscle, 16, STYLOGLOSSUS, L"Styloglossus")   // up movement of tongue body
	enums_add (kArt_muscle, 17, GENIOGLOSSUS, L"Genioglossus")   // forward movement of tongue body

	enums_add (kArt_muscle, 18, UPPER_TONGUE, L"UpperTongue")   // up curling of the tongue tip
	enums_add (kArt_muscle, 19, LOWER_TONGUE, L"LowerTongue")   // down curling of the tongue
	enums_add (kArt_muscle, 20, TRANSVERSE_TONGUE, L"TransverseTongue")   // thickening of tongue
	enums_add (kArt_muscle, 21, VERTICAL_TONGUE, L"VerticalTongue")   // thinning of tongue

	enums_add (kArt_muscle, 22, RISORIUS, L"Risorius")   // spreading of lips
	enums_add (kArt_muscle, 23, ORBICULARIS_ORIS, L"OrbicularisOris")   // rounding of lips

	enums_add (kArt_muscle, 24, LEVATOR_PALATINI, L"LevatorPalatini")   // closing of velo-pharyngeal port; 0 = open ("nasal"), 1 = closed ("oral")
	enums_add (kArt_muscle, 25, TENSOR_PALATINI, L"TensorPalatini")

	enums_add (kArt_muscle, 26, MASSETER, L"Masseter")   // closing of jaw; 0 = open, 1 = closed
	enums_add (kArt_muscle, 27, MYLOHYOID, L"Mylohyoid")   // opening of jaw
	enums_add (kArt_muscle, 28, LATERAL_PTERYGOID, L"LateralPterygoid")   // horizontal jaw position

	enums_add (kArt_muscle, 29, BUCCINATOR, L"Buccinator")   // oral wall tension
enums_end (kArt_muscle, 29, LUNGS)

/* End of file Articulation.enums */
