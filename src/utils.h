/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "utils/json/json.h"
#include "ID.h"

double realRandom(double range);

int getType(int *array, int size, int index);
int getOffset(int *array, int size, int index);

Json::Value testValue(Json::Value value, unsigned int idx);

template<typename T>
T *getRandomArray(T range, int size);
template<typename T>
T *getConstArray(T value, int size);


template<typename T>
T *getRandomArray(T range, int size) {
	srand(time(NULL));
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
