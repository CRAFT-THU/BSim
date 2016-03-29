/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "utils/json/json.h"
#include "ID.h"

int getType(int *array, int size, int index);
int getOffset(int *array, int size, int index);

Json::Value testValue(Json::Value value, unsigned int idx);

#endif /* UTILS_H */
