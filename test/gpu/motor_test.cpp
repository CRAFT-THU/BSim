
#include <stdio.h>

#include "../../src/utils/utils.h"
#include "../../src/utils/motor.h"

int main(int argc, char **argv)
{

	int * tmp = new int[10];

	for (int i=0; i<10; i++) {
		tmp[i] = i;
	}

	printf("Input:\t");
	for (int i=0; i<10; i++) {
		printf("%d ", tmp[i]);
	}
	printf("\n");

	motion(tmp, 10);


	return 0;
}
