#ifndef _LPC_and_LineSpectralFrequencies_h_
#define _LPC_and_LineSpectralFrequencies_h_
/* LPC_and_LineLineSpectralFrequencies.h
 *
 * Copyright (C) 2016 David Weenink
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

#include "LPC.h"
#include "LineSpectralFrequencies.h"

autoLineSpectralFrequencies LPC_to_LineSpectralFrequencies (LPC me, double gridSize);

autoLPC LineSpectralFrequencies_to_LPC (LineSpectralFrequencies me);


#endif /* _LPC_and_LineSpectralFrequencies_h_ */
