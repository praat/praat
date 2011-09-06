#ifndef _ContingencyTable_h_
#define _ContingencyTable_h_

#include "TableOfReal.h"
#include "Confusion.h"

Thing_define (ContingencyTable, TableOfReal) {
	// overridden methods:
		virtual void v_info ();
};

// entries must be nonnegative numbers

ContingencyTable ContingencyTable_create (long numberOfRows, long numberOfColumns);

double ContingencyTable_chisqProbability (ContingencyTable me);
double ContingencyTable_cramersStatistic (ContingencyTable me);
double ContingencyTable_contingencyCoefficient (ContingencyTable me);
void ContingencyTable_chisq (ContingencyTable me, double *chisq, long *df);
void ContingencyTable_entropies (ContingencyTable me, double *h, double *hx, double *hy,
	double *hygx, double *hxgy, double *uygx, double *uxgy, double *uxy);
ContingencyTable Confusion_to_ContingencyTable (Confusion me);
ContingencyTable TableOfReal_to_ContingencyTable (I);

#endif // _ContingencyTable_h_
