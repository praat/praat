/* OTMultiLevel.c
 *
 * Copyright (C) 2005 Paul Boersma
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
 * pb 2005/06/11 the very beginning of computational bidirectional multi-level OT
 */

#include "OTMultiLevel.h"

#include "oo_DESTROY.h"
#include "OTMultiLevel_def.h"
#include "oo_COPY.h"
#include "OTMultiLevel_def.h"
#include "oo_EQUAL.h"
#include "OTMultiLevel_def.h"
#include "oo_WRITE_BINARY.h"
#include "OTMultiLevel_def.h"
#include "oo_READ_BINARY.h"
#include "OTMultiLevel_def.h"
#include "oo_DESCRIPTION.h"
#include "OTMultiLevel_def.h"

static void classOTMultiLevel_info (I) {
	iam (OTMultiLevel);
	long numberOfViolations = 0, icand, icons;
	for (icand = 1; icand <= my numberOfCandidates; icand ++)
		for (icons = 1; icons <= my numberOfConstraints; icons ++)
			numberOfViolations += my candidates [icand]. marks [icons];
	Melder_info ("Number of constraints: %ld", my numberOfConstraints);
	Melder_info ("Number of candidates: %ld", my numberOfCandidates);
	Melder_info ("Number of violation marks: %ld", numberOfViolations);
}

static int writeAscii (I, FILE *f) {
	iam (OTMultiLevel);
	long icons, icand;
	const char *p;
	fprintf (f, "\n%ld constraints", my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		fprintf (f, "\nconstraint [%ld]: \"", icons);
		for (p = & constraint -> name [0]; *p; p ++) { if (*p =='\"') fputc (*p, f); fputc (*p, f); }
		fprintf (f, "\" %.17g %.17g ! ", constraint -> ranking, constraint -> disharmony);
		for (p = & constraint -> name [0]; *p; p ++) {
			if (*p == '\n') fputc (' ', f);
			else if (*p == '\\' && p [1] == 's' && p [2] == '{') p += 2;
			else if (*p == '}') { }
			else fputc (*p, f);
		}
	}
	fprintf (f, "\" %ld", my numberOfCandidates);
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		OTCandidate candidate = & my candidates [icand];
		fprintf (f, "\n   candidate [%ld]: \"", icand);
		for (p = & candidate -> string [0]; *p; p ++) { if (*p =='\"') fputc (*p, f); fputc (*p, f); }
		fprintf (f, "\"");
		for (icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
			fprintf (f, " %d", candidate -> marks [icons]);
	}
	return 1;
}

void OTMultiLevel_checkIndex (OTMultiLevel me) {
	int icons;
	if (my index) return;
	my index = NUMlvector (1, my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTMultiLevel_sort (me);
}

static int readAscii (I, FILE *f) {
	iam (OTMultiLevel);
	long icons, icand;
	if (! inherited (OTMultiLevel) readAscii (me, f)) return 0;
	if ((my numberOfConstraints = ascgeti4 (f)) < 1) return Melder_error ("No constraints.");
	if (! (my constraints = NUMstructvector (OTConstraint, 1, my numberOfConstraints))) return 0;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		if (! (constraint -> name = ascgets2 (f))) return 0;
		constraint -> ranking = ascgetr8 (f);
		constraint -> disharmony = ascgetr8 (f);
	}
	if ((my numberOfCandidates = ascgeti4 (f)) < 1) return Melder_error ("No candidates.");
	if (! (my candidates = NUMstructvector (OTCandidate, 1, my numberOfCandidates))) return 0;
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		OTCandidate candidate = & my candidates [icand];
		if (! (candidate -> string = ascgets2 (f))) return 0;
		candidate -> numberOfConstraints = my numberOfConstraints;   /* Redundancy, needed for writing binary. */
		if (! (candidate -> marks = NUMivector (1, candidate -> numberOfConstraints))) return 0;
		for (icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
			candidate -> marks [icons] = ascgeti2 (f);
	}
	OTMultiLevel_checkIndex (me);
	return 1;
}

class_methods (OTMultiLevel, Data)
	class_method_local (OTMultiLevel, destroy)
	class_method_local (OTMultiLevel, info)
	class_method_local (OTMultiLevel, description)
	class_method_local (OTMultiLevel, copy)
	class_method_local (OTMultiLevel, equal)
	class_method (writeAscii)
	class_method (readAscii)
	class_method_local (OTMultiLevel, writeBinary)
	class_method_local (OTMultiLevel, readBinary)
class_methods_end

void OTMultiLevel_sort (OTMultiLevel me) {
	long icons, jcons;
	for (icons = 1; icons < my numberOfConstraints; icons ++) {
		OTConstraint ci = & my constraints [my index [icons]];
		double maximum = ci -> disharmony;
		long jmax = icons, dummy;
		for (jcons = icons + 1; jcons <= my numberOfConstraints; jcons ++) {
			OTConstraint cj = & my constraints [my index [jcons]];
			double disharmonyj = cj -> disharmony;
			/*
			 * Sort only by disharmony; tied constraints should not exist.
			 */
			if (disharmonyj > maximum) {
				maximum = disharmonyj;
				jmax = jcons;
			}
		}
		dummy = my index [icons]; my index [icons] = my index [jmax]; my index [jmax] = dummy;   /* Swap. */
	}
}

void OTMultiLevel_newDisharmonies (OTMultiLevel me, double evaluationNoise) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, evaluationNoise);
	}
	OTMultiLevel_sort (me);
}

/* End of file OTMultiLevel.c */
