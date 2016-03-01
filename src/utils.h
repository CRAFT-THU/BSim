/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "ID.h"

int id2idx(ID* array, int num, ID id) {
	for (int i=0; i<num; i++) {
		if (array[i] == id) {
			return i;
		}
	}
	printf("ERROR: Cannot find ID!!!\n");
	return 0;
}

#endif /* UTILS_H */

