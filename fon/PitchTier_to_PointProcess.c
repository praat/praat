/* PitchTier_to_PointProcess.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2007/08/12 wchar_t
 */

#include "PitchTier_to_PointProcess.h"
#include "Pitch_to_PitchTier.h"

PointProcess PitchTier_to_PointProcess (PitchTier me) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 1000);
	double area = 0.5;   /* Imagine an event half a period before the beginning. */
	long interval, size = my points -> size;
	if (! thee) goto error;
	if (size == 0) return thee;
	for (interval = 0; interval <= size; interval ++) {
		double t1 = interval == 0 ? my xmin : ((RealPoint) my points -> item [interval]) -> time;
		double t2 = interval == size ? my xmax : ((RealPoint) my points -> item [interval + 1]) -> time;
		double f1 = ((RealPoint) my points -> item [interval == 0 ? 1 : interval]) -> value;
		double f2 = ((RealPoint) my points -> item [interval == size ? size : interval + 1]) -> value;
		area += (t2 - t1) * 0.5 * (f1 + f2);
		while (area >= 1.0) {
			double slope = (f2 - f1) / (t2 - t1), discriminant;
			area -= 1.0;
			discriminant = f2 * f2 - 2.0 * area * slope;
			if (discriminant < 0.0) discriminant = 0.0;   /* Catch rounding errors. */
			if (! PointProcess_addPoint (thee, t2 - 2.0 * area / (f2 + sqrt (discriminant)))) goto error;
		}
	}
	return thee;
error:
	forget (thee);
	return Melder_errorp ("(PitchTier_to_PointProcess:) Not performed.");
}

PointProcess PitchTier_Pitch_to_PointProcess (PitchTier me, Pitch vuv) {
	PointProcess fullPoint = NULL, thee = NULL;
	long i;
	if ((fullPoint = PitchTier_to_PointProcess (me)) == NULL) goto cleanUp;
	if ((thee = PointProcess_create (my xmin, my xmax, fullPoint -> maxnt)) == NULL) goto cleanUp;

	/*
	 * Copy only voiced parts to result.
	 */
	for (i = 1; i <= fullPoint -> nt; i ++) {
		double t = fullPoint -> t [i];
		if (Pitch_isVoiced_t (vuv, t) && ! PointProcess_addPoint (thee, t)) goto cleanUp;
	}

cleanUp:
	forget (fullPoint);
	if (Melder_hasError ()) {
		forget (thee);
		return Melder_errorp1 (L"(Pitch_to_PointProcess:) Not performed.");
	}
	return thee;
}

static int PointProcess_isVoiced_t (PointProcess me, double t, double maxT) {
	long imid = PointProcess_getNearestIndex (me, t);
	double tmid;
	int leftVoiced, rightVoiced;
	if (imid == 0) return 0;
	tmid = my t [imid];
	leftVoiced = imid > 1 && tmid - my t [imid - 1] <= maxT;
	rightVoiced = imid < my nt && my t [imid + 1] - tmid <= maxT;
	if ((leftVoiced && t <= tmid) || (rightVoiced && t >= tmid)) return 1;
	if (leftVoiced && t < 1.5 * tmid - 0.5 * my t [imid - 1]) return 1;
	if (rightVoiced && t > 1.5 * tmid - 0.5 * my t [imid + 1]) return 1;
	return 0;
}

PointProcess PitchTier_Point_to_PointProcess (PitchTier me, PointProcess vuv, double maxT) {
	PointProcess fullPoint = NULL, thee = NULL;
	long i;
	if ((fullPoint = PitchTier_to_PointProcess (me)) == NULL) goto cleanUp;
	if ((thee = PointProcess_create (my xmin, my xmax, fullPoint -> maxnt)) == NULL) goto cleanUp;

	/*
	 * Copy only voiced parts to result.
	 */
	for (i = 1; i <= fullPoint -> nt; i ++) {
		double t = fullPoint -> t [i];
		if (PointProcess_isVoiced_t (vuv, t, maxT) && ! PointProcess_addPoint (thee, t)) goto cleanUp;
	}

cleanUp:
	forget (fullPoint);
	if (Melder_hasError ()) {
		forget (thee);
		return Melder_errorp1 (L"(Pitch_to_PointProcess:) Not performed.");
	}
	return thee;
}

PitchTier PointProcess_to_PitchTier (PointProcess me, double maximumInterval) {
	PitchTier thee = NULL;
	long i;
	thee = PitchTier_create (my xmin, my xmax);
	if (! thee) goto end;
	for (i = 1; i < my nt; i ++) {
		double interval = my t [i + 1] - my t [i];
		if (interval <= maximumInterval) {
			if (! RealTier_addPoint (thee, my t [i] + 0.5 * interval, 1.0 / interval))
				goto end;
		}
	}
end:
	if (Melder_hasError ()) { forget (thee); return Melder_errorp
		("(PointProcess_to_PitchTier:) Not performed."); }
	return thee;
}

PitchTier Pitch_PointProcess_to_PitchTier (Pitch me, PointProcess pp) {
	PitchTier temp = Pitch_to_PitchTier (me), thee = NULL;
	if (! temp) return NULL;
	thee = PitchTier_PointProcess_to_PitchTier (temp, pp);
	forget (temp);
	iferror forget (thee);
	return thee;
}

PitchTier PitchTier_PointProcess_to_PitchTier (PitchTier me, PointProcess pp) {
	long i;
	PitchTier thee = NULL;
	if (my points -> size == 0) return Melder_errorp ("No pitch points.");
	thee = PitchTier_create (pp -> xmin, pp -> xmax); cherror;
	for (i = 1; i <= pp -> nt; i ++) {
		double time = pp -> t [i];
		double value = RealTier_getValueAtTime (me, time);
		RealTier_addPoint (thee, time, value); cherror
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal PitchTier_downto_TableOfReal (PitchTier me, int useSemitones) {
	TableOfReal thee = RealTier_downto_TableOfReal (me, L"Time", L"F0");
	long i;
	if (! thee) return NULL;
	if (useSemitones) for (i = 1; i <= thy numberOfRows; i ++)
		thy data [i] [2] = NUMhertzToSemitones (thy data [i] [2]);
	return thee;
}

/* End of file PitchTier_to_PointProcess.c */
