#ifndef _PowerCepstrogramToMatrixWorkspace_h_
#define _PowerCepstrogramToMatrixWorkspace_h_
/* PowerCepstrogramToMatrixWorkspace.h
 *
 * Copyright (C) 2025 David Weenink
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

#include "SampledToSampledWorkspace.h"
#include "PowerCepstrum.h"
#include "PowerCepstrogram.h"
#include "Matrix.h"
#include "SlopeSelector.h"

#include "PowerCepstrogramToMatrixWorkspace_def.h"

autoPowerCepstrogramToMatrixWorkspace PowerCepstrogramToMatrixWorkspace_create (constPowerCepstrogram thee, mutableMatrix him);

#endif /* _PowerCepstrogramToMatrixWorkspace_h_ */
