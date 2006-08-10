/* OTMulti.c
 *
 * Copyright (C) 2005-2006 Paul Boersma
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
 * pb 2006/05/16 guarded against cells with many violations
 * pb 2006/05/17 draw disharmonies above tableau
 */

#include "OTMulti.h"

#include "oo_DESTROY.h"
#include "OTMulti_def.h"
#include "oo_COPY.h"
#include "OTMulti_def.h"
#include "oo_EQUAL.h"
#include "OTMulti_def.h"
#include "oo_WRITE_BINARY.h"
#include "OTMulti_def.h"
#include "oo_READ_BINARY.h"
#include "OTMulti_def.h"
#include "oo_DESCRIPTION.h"
#include "OTMulti_def.h"

static void classOTMulti_info (I) {
	iam (OTMulti);
	long numberOfViolations = 0, icand, icons;
	for (icand = 1; icand <= my numberOfCandidates; icand ++)
		for (icons = 1; icons <= my numberOfConstraints; icons ++)
			numberOfViolations += my candidates [icand]. marks [icons];
	Melder_info ("Number of constraints: %ld", my numberOfConstraints);
	Melder_info ("Number of candidates: %ld", my numberOfCandidates);
	Melder_info ("Number of violation marks: %ld", numberOfViolations);
}

static int writeAscii (I, FILE *f) {
	iam (OTMulti);
	long icons, icand;
	const char *p;
	fprintf (f, "\n%ld constraints", my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		fprintf (f, "\n\t\"");
		for (p = & constraint -> name [0]; *p; p ++) { if (*p =='\"') fputc (*p, f); fputc (*p, f); }
		fprintf (f, "\"   %.17g %.17g", constraint -> ranking, constraint -> disharmony);
	}
	fprintf (f, "\n\n%ld candidates", my numberOfCandidates);
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		OTCandidate candidate = & my candidates [icand];
		fprintf (f, "\n\t\"");
		for (p = & candidate -> string [0]; *p; p ++) { if (*p =='\"') fputc (*p, f); fputc (*p, f); }
		fprintf (f, "\"  ");
		for (icons = 1; icons <= candidate -> numberOfConstraints; icons ++)
			fprintf (f, " %d", candidate -> marks [icons]);
	}
	return 1;
}

void OTMulti_checkIndex (OTMulti me) {
	int icons;
	if (my index) return;
	my index = NUMlvector (1, my numberOfConstraints);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTMulti_sort (me);
}

static int readAscii (I, FILE *f) {
	iam (OTMulti);
	long icons, icand;
	if (! inherited (OTMulti) readAscii (me, f)) return 0;
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
	OTMulti_checkIndex (me);
	return 1;
}

class_methods (OTMulti, Data)
	class_method_local (OTMulti, destroy)
	class_method_local (OTMulti, info)
	class_method_local (OTMulti, description)
	class_method_local (OTMulti, copy)
	class_method_local (OTMulti, equal)
	class_method (writeAscii)
	class_method (readAscii)
	class_method_local (OTMulti, writeBinary)
	class_method_local (OTMulti, readBinary)
class_methods_end

void OTMulti_sort (OTMulti me) {
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

void OTMulti_newDisharmonies (OTMulti me, double evaluationNoise) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking + NUMrandomGauss (0, evaluationNoise);
	}
	OTMulti_sort (me);
}

int OTMulti_compareCandidates (OTMulti me, long icand1, long icand2) {
	int *marks1 = my candidates [icand1]. marks;
	int *marks2 = my candidates [icand2]. marks;
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		int numberOfMarks1 = marks1 [my index [icons]];
		int numberOfMarks2 = marks2 [my index [icons]];
		if (numberOfMarks1 < numberOfMarks2) return -1;   /* Candidate 1 is better than candidate 2. */
		if (numberOfMarks1 > numberOfMarks2) return +1;   /* Candidate 2 is better than candidate 1. */
	}
	return 0;   /* None of the comparisons found a difference between the two candidates. Hence, they are equally good. */
}

int OTMulti_candidateMatches (OTMulti me, long icand, const char *form1, const char *form2) {
	const char *string = my candidates [icand]. string;
	return (form1 [0] == '\0' || strstr (string, form1)) && (form2 [0] == '\0' || strstr (string, form2));
}

long OTMulti_getWinner (OTMulti me, const char *form1, const char *form2) {
	long icand_best = 0, icand, numberOfBestCandidates = 0;
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (me, icand, form1, form2)) {
			if (icand_best == 0) {
				icand_best = icand;
				numberOfBestCandidates = 1;
			} else {
				int comparison = OTMulti_compareCandidates (me, icand, icand_best);
				if (comparison == -1) {
					icand_best = icand;
					numberOfBestCandidates = 1;
				} else if (comparison == 0) {
					numberOfBestCandidates += 1;
					if (NUMrandomUniform (0.0, numberOfBestCandidates) < 1.0) {
						icand_best = icand;
					}
				}
			}
		}
	}
	if (icand_best == 0) {
		Melder_error ("The forms \"%s\" and \"%s\" do not match any candidate.", form1, form2);
		goto end;
	}
end:
	iferror return Melder_error ("(OTMulti: Get winner (two):) Not performed.");
	return icand_best;
}

static int OTMulti_modifyRankings (OTMulti me, long iwinner, long iloser,
	double plasticity, double relativePlasticityNoise)
{
	OTCandidate winner = & my candidates [iwinner], loser = & my candidates [iloser];
	long icons;
	double step = relativePlasticityNoise == 0.0 ? plasticity :
		NUMrandomGauss (plasticity, relativePlasticityNoise * plasticity);
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		int winnerMarks = winner -> marks [icons];
		int loserMarks = loser -> marks [icons];
		if (loserMarks > winnerMarks) {
			my constraints [icons]. ranking -= step;
		}
		if (winnerMarks > loserMarks) {
			my constraints [icons]. ranking += step;
		}
	}
end:
	iferror return 0;
	return 1;
}

int OTMulti_learnOne (OTMulti me, const char *form1, const char *form2,
	int direction, double plasticity, double relativePlasticityNoise)
{
	long iloser;
	iloser = OTMulti_getWinner (me, form1, form2); cherror
	if (direction & OTMulti_LEARN_FORWARD) {
		long iwinner = OTMulti_getWinner (me, form1, ""); cherror
		OTMulti_modifyRankings (me, iwinner, iloser, plasticity, relativePlasticityNoise); cherror
	}
	if (direction & OTMulti_LEARN_BACKWARD) {
		long iwinner = OTMulti_getWinner (me, form2, ""); cherror
		OTMulti_modifyRankings (me, iwinner, iloser, plasticity, relativePlasticityNoise); cherror
	}
end:
	iferror return 0;
	return 1;
}

int OTMulti_PairDistribution_learn (OTMulti me, PairDistribution thee, double evaluationNoise, int direction,
	double initialPlasticity, long replicationsPerPlasticity, double plasticityDecrement,
	long numberOfPlasticities, double relativePlasticityNoise)
{
	long iplasticity, ireplication, idatum = 0, numberOfData = numberOfPlasticities * replicationsPerPlasticity;
	double plasticity = initialPlasticity;
	Graphics graphics = Melder_monitor (0.0, "Learning with full knowledge...");
	if (graphics) {
		Graphics_clearWs (graphics);
	}
	for (iplasticity = 1; iplasticity <= numberOfPlasticities; iplasticity ++) {
		for (ireplication = 1; ireplication <= replicationsPerPlasticity; ireplication ++) {
			char *form1, *form2;
			if (! PairDistribution_peekPair (thee, & form1, & form2)) goto end;
			++ idatum;
			if (graphics && idatum % (numberOfData / 400 + 1) == 0) {
				long icons;
				Graphics_setWindow (graphics, 0, numberOfData, 50, 150);
				for (icons = 1; icons <= 14 && icons <= my numberOfConstraints; icons ++) {
					Graphics_setColour (graphics, icons + 1);
					Graphics_line (graphics, idatum, my constraints [icons]. ranking,
						idatum, my constraints [icons]. ranking+1);
				}
				Graphics_flushWs (graphics);   /* Because drawing is faster than progress loop. */
			}
			if (! Melder_monitor ((double) idatum / numberOfData,
				"Processing partial pair %ld out of %ld: { \"%s\", \"%s\" }", idatum, numberOfData, form1, form2))
			{
				Melder_flushError ("Only %ld partial pairs out of %ld were processed.", idatum - 1, numberOfData);
				goto end;
			}
			OTMulti_newDisharmonies (me, evaluationNoise);
			if (! OTMulti_learnOne (me, form1, form2, direction, plasticity, relativePlasticityNoise)) goto end;
		}
		plasticity *= plasticityDecrement;
	}
end:
	Melder_monitor (1.0, NULL);
	iferror return Melder_error ("OTMulti did not complete learning from partial pairs.");
	return 1;
}

static long OTMulti_crucialCell (OTMulti me, long icand, long iwinner, long numberOfOptimalCandidates, const char *form1, const char *form2) {
	long icons;
	if (my numberOfCandidates < 2) return 0;   /* If there is only one candidate, all cells can be greyed. */
	if (OTMulti_compareCandidates (me, icand, iwinner) == 0) {   /* Candidate equally good as winner? */
		if (numberOfOptimalCandidates > 1) {
			/* All cells are important. */
		} else {
			long jcand, secondBest = 0;
			for (jcand = 1; jcand <= my numberOfCandidates; jcand ++) {
				if (OTMulti_candidateMatches (me, jcand, form1, form2) && OTMulti_compareCandidates (me, jcand, iwinner) != 0) {   /* A non-optimal candidate? */
					if (secondBest == 0) {
						secondBest = jcand;   /* First guess. */
					} else if (OTMulti_compareCandidates (me, jcand, secondBest) < 0) {
						secondBest = jcand;   /* Better guess. */
					}
				}
			}
			if (secondBest == 0) return 0;   /* If all candidates are equally good, all cells can be greyed. */
			return OTMulti_crucialCell (me, secondBest, iwinner, 1, form1, form2);
		}
	} else {
		int *candidateMarks = my candidates [icand]. marks;
		int *winnerMarks = my candidates [iwinner]. marks;
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int numberOfCandidateMarks = candidateMarks [my index [icons]];
			int numberOfWinnerMarks = winnerMarks [my index [icons]];
			if (numberOfCandidateMarks > numberOfWinnerMarks)
				return icons;
		}
	}
	return my numberOfConstraints;   /* Nothing grey. */
}

static double OTMulti_constraintWidth (Graphics g, OTConstraint constraint, int showDisharmony) {
	char text [100], *newLine;
	double maximumWidth = showDisharmony ? 0.8 * Graphics_textWidth_ps (g, Melder_fixed (constraint -> disharmony, 1), TRUE) : 0.0,
		firstWidth, secondWidth;
	strcpy (text, constraint -> name);
	newLine = strchr (text, '\n');
	if (newLine) {
		*newLine = '\0';
		firstWidth = Graphics_textWidth_ps (g, text, TRUE);
		if (firstWidth > maximumWidth) maximumWidth = firstWidth;
		secondWidth = Graphics_textWidth_ps (g, newLine + 1, TRUE);
		if (secondWidth > maximumWidth) maximumWidth = secondWidth;
		return maximumWidth;
	}
	firstWidth = Graphics_textWidth_ps (g, text, TRUE);
	if (firstWidth > maximumWidth) maximumWidth = firstWidth;
	return maximumWidth;
}

void OTMulti_drawTableau (OTMulti me, Graphics g, const char *form1, const char *form2, int showDisharmonies) {
	long winner, winner1 = 0, winner2 = 0, icons, icand, numberOfMatchingCandidates;
	long numberOfOptimalCandidates, numberOfOptimalCandidates1, numberOfOptimalCandidates2, imark;
	double candWidth, margin, fingerWidth, doubleLineDx, doubleLineDy;
	double tableauWidth, rowHeight, headerHeight, descent, x, y, fontSize = Graphics_inqFontSize (g);
	char text [200];
	int bidirectional = form1 [0] != '\0' && form2 [0] != '\0';
	winner = OTMulti_getWinner (me, form1, form2);
	if (winner == 0) {
		Melder_clearError ();
		Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
		Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
		Graphics_rectangle (g, 0, 1, 0, 1);
		Graphics_text (g, 0.0, 0.5, "(no matching candidates)");
		return;
	}

	if (bidirectional) {
		winner1 = OTMulti_getWinner (me, form1, "");
		winner2 = OTMulti_getWinner (me, form2, "");
	}
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	margin = Graphics_dxMMtoWC (g, 1.0);
	fingerWidth = Graphics_dxMMtoWC (g, 7.0) * fontSize / 12.0;
	doubleLineDx = Graphics_dxMMtoWC (g, 0.9);
	doubleLineDy = Graphics_dyMMtoWC (g, 0.9);
	rowHeight = Graphics_dyMMtoWC (g, 1.5 * fontSize * 25.4 / 72);
	descent = rowHeight * 0.5;
	/*
	 * Compute height of header row.
	 */
	headerHeight = rowHeight;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		if (strchr (constraint -> name, '\n')) {
			headerHeight += 0.7 * rowHeight;
			break;
		}
	}
	/*
	 * Compute longest candidate string.
	 * Also count the number of optimal candidates (if there are more than one, the fingers will be drawn in red).
	 */
	candWidth = Graphics_textWidth_ps (g, form1, TRUE) + Graphics_textWidth_ps (g, form2, TRUE);
	numberOfMatchingCandidates = 0;
	numberOfOptimalCandidates = numberOfOptimalCandidates1 = numberOfOptimalCandidates2 = 0;
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		if ((form1 [0] != '\0' && OTMulti_candidateMatches (me, icand, form1, "")) ||
		    (form2 [0] != '\0' && OTMulti_candidateMatches (me, icand, form2, "")) ||
		    (form1 [0] == '\0' && form2 [0] == '\0'))
		{
			double width = Graphics_textWidth_ps (g, my candidates [icand]. string, TRUE);
			if (width > candWidth) candWidth = width;
			numberOfMatchingCandidates ++;
			if (OTMulti_compareCandidates (me, icand, winner) == 0) {
				numberOfOptimalCandidates ++;
			}
			if (winner1 != 0 && OTMulti_compareCandidates (me, icand, winner1) == 0) {
				numberOfOptimalCandidates1 ++;
			}
			if (winner2 != 0 && OTMulti_compareCandidates (me, icand, winner2) == 0) {
				numberOfOptimalCandidates2 ++;
			}
		}
	}
	candWidth += fingerWidth * (bidirectional ? 3 : 1) + margin * 3;
	/*
	 * Compute tableau width.
	 */
	tableauWidth = candWidth + doubleLineDx;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		tableauWidth += OTMulti_constraintWidth (g, constraint, showDisharmonies);
	}
	tableauWidth += margin * 2 * my numberOfConstraints;
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   /* Left side of tableau. */
	y = 1.0 - doubleLineDy;
	if (showDisharmonies) y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
	/*
	 * Draw input.
	 */
	y -= headerHeight;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_printf (g, x + 0.5 * candWidth, y + 0.5 * headerHeight, "%s%s", form1, form2);
	Graphics_rectangle (g, x, x + candWidth, y, y + headerHeight);
	/*
	 * Draw constraint names.
	 */
	x += candWidth + doubleLineDx;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [my index [icons]];
		double width = OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
		if (strchr (constraint -> name, '\n')) {
			char *newLine;
			strcpy (text, constraint -> name);
			newLine = strchr (text, '\n');
			*newLine = '\0';
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
			Graphics_text (g, x + 0.5 * width, y + headerHeight, text);
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			Graphics_text (g, x + 0.5 * width, y, newLine + 1);
		} else {
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
			Graphics_text (g, x + 0.5 * width, y + 0.5 * headerHeight, constraint -> name);
		}
		if (showDisharmonies) {
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			Graphics_setFontSize (g, 0.8 * fontSize);
			Graphics_text (g, x + 0.5 * width, y + headerHeight, Melder_fixed (constraint -> disharmony, 1));
			Graphics_setFontSize (g, fontSize);
		}
		Graphics_line (g, x, y, x, y + headerHeight);
		Graphics_line (g, x, y, x + width, y);
		x += width;
	}
	/*
	 * Draw candidates.
	 */
	y -= doubleLineDy;
	for (icand = 1; icand <= my numberOfCandidates; icand ++)
		if ((form1 [0] != '\0' && OTMulti_candidateMatches (me, icand, form1, "")) ||
		    (form2 [0] != '\0' && OTMulti_candidateMatches (me, icand, form2, "")) ||
		    (form1 [0] == '\0' && form2 [0] == '\0'))
	{
		long crucialCell = OTMulti_crucialCell (me, icand, winner, numberOfOptimalCandidates, form1, form2);
		int candidateIsOptimal = OTMulti_compareCandidates (me, icand, winner) == 0;
		int candidateIsOptimal1 = winner1 != 0 && OTMulti_compareCandidates (me, icand, winner1) == 0;
		int candidateIsOptimal2 = winner2 != 0 && OTMulti_compareCandidates (me, icand, winner2) == 0;
		/*
		 * Draw candidate transcription.
		 */
		x = doubleLineDx;
		y -= rowHeight;
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_text (g, x + candWidth - margin, y + descent, my candidates [icand]. string);
		if (candidateIsOptimal) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, (int) ((bidirectional ? 1.2 : 1.5) * fontSize));
			if (numberOfOptimalCandidates > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_text (g, x + margin, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, bidirectional ? "\\Vr" : "\\pf");
			Graphics_setColour (g, Graphics_BLACK);
			Graphics_setFontSize (g, (int) fontSize);
		}
		if (candidateIsOptimal1) {
			Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);
			Graphics_setFontSize (g, (int) (1.5 * fontSize));
			if (numberOfOptimalCandidates1 > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_text (g, x + margin + fingerWidth, y + descent - Graphics_dyMMtoWC (g, 0.5) * fontSize / 12.0, "\\pf");
			Graphics_setColour (g, Graphics_BLACK);
			Graphics_setFontSize (g, (int) fontSize);
		}
		if (candidateIsOptimal2) {
			Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
			Graphics_setFontSize (g, (int) (1.5 * fontSize));
			if (numberOfOptimalCandidates2 > 1) Graphics_setColour (g, Graphics_RED);
			Graphics_setTextRotation (g, 180);
			Graphics_text (g, x + margin + fingerWidth * 2, y + descent - Graphics_dyMMtoWC (g, 0.0) * fontSize / 12.0, "\\pf");
			Graphics_setTextRotation (g, 0);
			Graphics_setColour (g, Graphics_BLACK);
			Graphics_setFontSize (g, (int) fontSize);
		}
		Graphics_rectangle (g, x, x + candWidth, y, y + rowHeight);
		/*
		 * Draw grey cell backgrounds.
		 */
		if (! bidirectional) {
			x = candWidth + 2 * doubleLineDx;
			Graphics_setGrey (g, 0.9);
			for (icons = 1; icons <= my numberOfConstraints; icons ++) {
				int index = my index [icons];
				OTConstraint constraint = & my constraints [index];
				double width = OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
				if (icons > crucialCell)
					Graphics_fillRectangle (g, x, x + width, y, y + rowHeight);
				x += width;
			}
			Graphics_setGrey (g, 0.0);
		}
		/*
		 * Draw cell marks.
		 */
		x = candWidth + 2 * doubleLineDx;
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (icons = 1; icons <= my numberOfConstraints; icons ++) {
			int index = my index [icons];
			OTConstraint constraint = & my constraints [index];
			double width = OTMulti_constraintWidth (g, constraint, showDisharmonies) + margin * 2;
			char markString [40];
			markString [0] = '\0';
			if (bidirectional && my candidates [icand]. marks [index]) {
				if ((candidateIsOptimal1 || candidateIsOptimal2) && ! candidateIsOptimal) {
					strcat (markString, "\\<-");
				}
			}
			/*
			 * An exclamation mark can be drawn in this cell only if both of the following conditions are met:
			 * 1. the candidate is not optimal;
			 * 2. this is the crucial cell, i.e. the cells after it are drawn in grey.
			 */
			if (! bidirectional && icons == crucialCell && ! candidateIsOptimal) {
				int winnerMarks = my candidates [winner]. marks [index];
				if (winnerMarks + 1 > 5) {
					strcat (markString, Melder_integer (winnerMarks + 1));
				} else {
					for (imark = 1; imark <= winnerMarks + 1; imark ++)
						strcat (markString, "*");
				}
				strcat (markString, "!");
				if (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1 > 5) {
					strcat (markString, Melder_integer (my candidates [icand]. marks [index] - (winnerMarks + 2) + 1));
				} else {
					for (imark = winnerMarks + 2; imark <= my candidates [icand]. marks [index]; imark ++)
						strcat (markString, "*");
				}
			} else {
				if (my candidates [icand]. marks [index] > 5) {
					strcat (markString, Melder_integer (my candidates [icand]. marks [index]));
				} else {
					for (imark = 1; imark <= my candidates [icand]. marks [index]; imark ++)
						strcat (markString, "*");
				}
			}
			if (bidirectional && my candidates [icand]. marks [index]) {
				if (candidateIsOptimal && ! candidateIsOptimal1) {
					strcat (markString, "\\->");
				}
				if (candidateIsOptimal && ! candidateIsOptimal2) {
					strcat (markString, "\\->");
				}
			}
			Graphics_text (g, x + 0.5 * width, y + descent, markString);
			Graphics_setColour (g, Graphics_BLACK);
			Graphics_line (g, x, y, x, y + rowHeight);
			Graphics_line (g, x, y + rowHeight, x + width, y + rowHeight);
			x += width;
		}
	}
	/*
	 * Draw box.
	 */
	x = doubleLineDx;   /* Left side of tableau. */
	y = 1.0 - doubleLineDy;
	if (showDisharmonies) y -= 0.6 * rowHeight;
	Graphics_rectangle (g, x, x + tableauWidth,
		y - headerHeight - numberOfMatchingCandidates * rowHeight - doubleLineDy, y);
}

void OTMulti_reset (OTMulti me, double ranking) {
	long icons;
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		constraint -> disharmony = constraint -> ranking = ranking;
	}
	OTMulti_sort (me);
}

int OTMulti_setRanking (OTMulti me, long constraint, double ranking, double disharmony) {
	if (constraint < 1 || constraint > my numberOfConstraints)
		return Melder_error ("(OTGrammar_setRanking): No constraint %ld.", constraint);
	my constraints [constraint]. ranking = ranking;
	my constraints [constraint]. disharmony = disharmony;
	OTMulti_sort (me);
	return 1;
}

int OTMulti_removeConstraint (OTMulti me, const char *constraintName) {
	long icons, icand, removed = 0;

	if (my numberOfConstraints <= 1)
		return Melder_error ("Cannot remove last constraint.");

	/*
	 * Look for the constraint to be removed.
	 */
	for (icons = 1; icons <= my numberOfConstraints; icons ++) {
		OTConstraint constraint = & my constraints [icons];
		if (strequ (constraint -> name, constraintName)) {
			removed = icons;
			break;
		}
	}
	if (removed == 0)
		return Melder_error ("No constraint \"%s\".", constraintName);
	/*
	 * Remove the constraint while reusing the memory space.
	 */
	my numberOfConstraints -= 1;
	/*
	 * Shift constraints.
	 */
	Melder_free (my constraints [removed]. name);
	for (icons = removed; icons <= my numberOfConstraints; icons ++) {
		my constraints [icons] = my constraints [icons + 1];
	}
	/*
	 * Shift tableau rows.
	 */
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		OTCandidate candidate = & my candidates [icand];
		candidate -> numberOfConstraints -= 1;
		for (icons = removed; icons <= my numberOfConstraints; icons ++) {
			candidate -> marks [icons] = candidate -> marks [icons + 1];
		}
	}
	/*
	 * Rebuild index.
	 */
	for (icons = 1; icons <= my numberOfConstraints; icons ++) my index [icons] = icons;
	OTMulti_sort (me);
	return 1;
}

int OTMulti_generateOptimalForm (OTMulti me, const char *form1, const char *form2, char *optimalForm, double evaluationNoise) {
	long winner;
	OTMulti_newDisharmonies (me, evaluationNoise);
	winner = OTMulti_getWinner (me, form1, form2);
	if (! winner) { Melder_error ("No winner"); goto end; }
	strcpy (optimalForm, my candidates [winner]. string);
end:
	iferror return Melder_error ("(OTMulti_generateOptimalForm:) Not performed.");
	return 1;
}

Strings OTMulti_Strings_generateOptimalForms (OTMulti me, Strings forms, double evaluationNoise) {
	Strings outputs = new (Strings);
	long i, n = forms -> numberOfStrings;
	cherror
	outputs -> numberOfStrings = n;
	outputs -> strings = NUMpvector (1, n); cherror
	for (i = 1; i <= n; i ++) {
		char output [100];
		OTMulti_generateOptimalForm (me, forms -> strings [i], "", output, evaluationNoise); cherror
		outputs -> strings [i] = Melder_strdup (output); cherror
	}
end:
	iferror { forget (outputs); return Melder_errorp ("(OTMulti_Strings_generateOptimalForms:) Not performed."); }
	return outputs;
}

Strings OTMulti_generateOptimalForms (OTMulti me, const char *form1, const char *form2, long numberOfTrials, double evaluationNoise) {
	Strings outputs = new (Strings);
	long i;
	cherror
	outputs -> numberOfStrings = numberOfTrials;
	outputs -> strings = NUMpvector (1, numberOfTrials); cherror
	for (i = 1; i <= numberOfTrials; i ++) {
		char output [100];
		OTMulti_generateOptimalForm (me, form1, form2, output, evaluationNoise); cherror
		outputs -> strings [i] = Melder_strdup (output); cherror
	}
end:
	iferror return Melder_errorp ("(OTMulti_generateOptimalForms:) Not performed.");
	return outputs;
}

Distributions OTMulti_to_Distribution (OTMulti me, const char *form1, const char *form2,
	long numberOfTrials, double evaluationNoise)
{
	Distributions thee = NULL;
	long totalNumberOfOutputs = 0, iout = 0, icand, itrial, *index = NULL;
	/*
	 * Count the total number of outputs.
	 */
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (me, icand, form1, form2)) {
			totalNumberOfOutputs ++;
		}
	}
	/*
	 * Create the distribution. One row for every output form.
	 */
	thee = Distributions_create (totalNumberOfOutputs, 1); cherror
	index = NUMlvector (1, my numberOfCandidates); cherror
	/*
	 * Set the row labels to the output strings.
	 */
	iout = 0;
	for (icand = 1; icand <= my numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (me, icand, form1, form2)) {
			thy rowLabels [++ iout] = Melder_strdup (my candidates [icand]. string);
			index [icand] = iout;
		}
	}
	/*
	 * Compute a number of outputs and store the results.
	 */
	for (itrial = 1; itrial <= numberOfTrials; itrial ++) {
		long iwinner;
		OTMulti_newDisharmonies (me, evaluationNoise);
		iwinner = OTMulti_getWinner (me, form1, form2);
		thy data [index [iwinner]] [1] += 1;
	}
end:
	NUMlvector_free (index, 1);
	iferror forget (thee);
	return thee;
}

/* End of file OTMulti.c */
