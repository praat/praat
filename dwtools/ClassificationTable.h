#ifndef _ClassificationTable_h_
#define _ClassificationTable_h_
/* ClassificationTable.h
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "TableOfReal.h"
#include "SSCP.h"
#include "Correlation.h"
#include "Confusion.h"

#include "Strings_.h"


Thing_define (ClassificationTable, TableOfReal) {
};

autoClassificationTable ClassificationTable_create (integer numberOfRows, integer numberOfColumns);

autoCategories ClassificationTable_to_Categories_maximumProbability (ClassificationTable me);

autoStrings ClassificationTable_to_Strings_maximumProbability (ClassificationTable me);

autoConfusion ClassificationTable_to_Confusion (ClassificationTable me, bool onlyClassLabels);

/* Correlations between the classes (columns) */
autoCorrelation ClassificationTable_to_Correlation_columns (ClassificationTable me);

void Confusion_ClassificationTable_increase (Confusion me, ClassificationTable thee);

#endif /* _ClassificationTable_h_ */

