/* Articulation_enums.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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

/*
 * pb 1996/06/11
 * pb 2002/07/16 GPL
 */

enum_begin (Art_MUSCLE, _)
	enum (Lungs)

	enum (Interarytenoid) /* Constriction of larynx; 0 = breathing, 1 = constricted glottis */
	enum (Cricothyroid) /* Vocal-cord tension. */
	enum (Vocalis) /* Vocal-cord tension. */
	enum (Thyroarytenoid)
	enum (PosteriorCricoarytenoid) /* Opening of glottis. */
	enum (LateralCricoarytenoid) /* Opening of glottis. */

	enum (Stylohyoid) /* Up movement of hyoid bone. */
	enum (Sternohyoid) /* Down movement of hyoid bone. */

	enum (Thyropharyngeus) /* Constriction of ventricular folds. */
	enum (LowerConstrictor)
	enum (MiddleConstrictor)
	enum (UpperConstrictor)
	enum (Sphincter) /* Constriction of pharynx. */

	enum (Hyoglossus) /* Down movement of tongue body. */
	enum (Styloglossus) /* Up movement of tongue body. */
	enum (Genioglossus) /* Forward movement of tongue body. */

	enum (UpperTongue) /* Up curling of the tongue tip. */
	enum (LowerTongue) /* Down curling of the tongue. */
	enum (TransverseTongue) /* Thickening of tongue. */
	enum (VerticalTongue) /* Thinning of tongue. */

	enum (Risorius) /* Spreading of lips. */
	enum (OrbicularisOris) /* Rounding of lips. */

	enum (LevatorPalatini) /* Closing of velo-pharyngeal port; 0 = open ("nasal"), 1 = closed ("oral") */
	enum (TensorPalatini)

	enum (Masseter) /* Closing of jaw; 0 = open, 1 = closed */
	enum (Mylohyoid) /* Opening of jaw. */
	enum (LateralPterygoid) /* Horizontal jaw position. */

	enum (Buccinator) /* Oral wall tension. */
enum_end (Art_MUSCLE)

/* End of file Articulation.enums */
