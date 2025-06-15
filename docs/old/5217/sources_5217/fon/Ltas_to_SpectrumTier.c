/* Ltas_to_SpectrumTier.c
 *
 * Copyright (C) 2007 Paul Boersma
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
 * pb 2007/03/17
 */

#include "Ltas_to_SpectrumTier.h"

SpectrumTier Ltas_to_SpectrumTier_peaks (Ltas me) {
	SpectrumTier thee = (SpectrumTier) Vector_to_RealTier_peaks (me, 1); cherror
	Thing_overrideClass (thee, classSpectrumTier);
end:
	iferror forget (thee);
	return thee;
}

/* End of file Ltas_to_SpectrumTier.c */
