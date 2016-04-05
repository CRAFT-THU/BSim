/* This program is writen by qp09.
 * usually just for fun.
 * Mon March 14 2016
 */

#include <iostream>
#include "utils.h"

double realRandom(double range)
{
	long f = rand();
	return (double)(f/RAND_MAX)*range;
}

//int id2idx(ID* array, int num, ID id) {
//	for (int i=0; i<num; i++) {
//		if (array[i] == id) {
//			return i;
//		}
//	}
//	printf("ERROR: Cannot find ID!!!\n");
//	return 0;
//}

int getType(int *array, int size, int index)
{
	for (int i=0; i<size-1; i++) {
		if (array[i+1] > index) {
			return i;
		}
	}

	printf("ERROR: Cannot find index %d !!!\n", index);
	return 0;
}

int getOffset(int *array, int size, int index)
{
	for (int i=0; i<size-1; i++) {
		if (array[i+1] > index) {
			return (index - array[i]);
		}
	}

	printf("ERROR: Cannot find index %d !!!\n", index);
	return 0;
}

Json::Value testValue(Json::Value value, unsigned int idx)
{
	if (value.type() == Json::nullValue) {
		return 0;
	}

	if (value.type() == Json::arrayValue) {
		if (idx < value.size()) {
			return value[idx];
		} else {
			std::cout  << "Not enough parameters:" << value << "@" << idx << std::endl;
		}
	} 

	return value;
}
