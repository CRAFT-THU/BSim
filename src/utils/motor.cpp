
#include <stdio.h>

#include "motor.h"

void motion(int * fire_rate, int size)
{
	int max_idx = 0;
	int max_value = fire_rate[max_idx];

	for (int i=1; i<size; i++) {
		if (fire_rate[i] > max_value) {
			max_value = fire_rate[i];
			max_idx = i;
		}
	}

	printf("Result:\t%d\n", max_idx);
}
