/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "../third_party/json/json.h"
#include "type.h"

static bool rand_seed_inited = false;

double realRandom(double range);

int getIndex(Type *array, int size, Type type);
int getType(int *array, int size, int index);
int getOffset(int *array, int size, int index);

Json::Value testValue(Json::Value value, unsigned int idx);

real *loadArray(const char *filename, int size);
int saveArray(const char *filename, real *array, int size);

inline int upzero_else_set_one(int value) {
	if (value > 0) {
		return value;
	}

	return 1;
}

template<typename T>
T *getRandomArray(T range, int size) {
	if (!rand_seed_inited) {
		srand(time(NULL));
		rand_seed_inited = true;
	}

	T *res = new T[size];
	for (int i=0; i<size; i++) {
		res[i] = static_cast<T>(realRandom(static_cast<double>(range)));
	}

	return res;
}

template<typename T>
T *getConstArray(T value, int size)
{
	T *res = new T[size];
	for (int i=0; i<size; i++) {
		res[i] = value;
	}

	return res;
}


#endif /* UTILS_H */
