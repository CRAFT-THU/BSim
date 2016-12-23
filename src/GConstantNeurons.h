/* This header file is writen by qp09
 * usually just for fun
 * Fri December 23 2016
 */
#ifndef GCONSTANTNEURONS_H
#define GCONSTANTNEURONS_H

#include "constant.h"

struct GConstantNeurons {
	real *fire_rate;
	int *fire_count;

	int *pSynapsesNum;
	int *pSynapsesLoc;
	int *pSynapsesIdx;
	int num;
	int synapsesNum;
};

#endif /* GCONSTANTNEURONS_H */

