/* Layer.cpp
 *
 * Copyright (C) 2017 Paul Boersma
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

#include "Layer.h"

#include "oo_DESTROY.h"
#include "Layer_def.h"
#include "oo_COPY.h"
#include "Layer_def.h"
#include "oo_EQUAL.h"
#include "Layer_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Layer_def.h"
#include "oo_WRITE_TEXT.h"
#include "Layer_def.h"
#include "oo_READ_TEXT.h"
#include "Layer_def.h"
#include "oo_WRITE_BINARY.h"
#include "Layer_def.h"
#include "oo_READ_BINARY.h"
#include "Layer_def.h"
#include "oo_DESCRIPTION.h"
#include "Layer_def.h"

#include "enums_getText.h"
#include "Layer_enums.h"
#include "enums_getValue.h"
#include "Layer_enums.h"

Thing_implement (Layer, Daata, 0);

Thing_implement (LayerList, Ordered, 0);

/* End of file Layer.cpp */
