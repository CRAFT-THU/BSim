
#include <stdio.h>

#include "../../src/utils/vision.h"

int main(int argc, char **argv)
{

	float * tmp = NULL;
	int size = 0;

	size = vision(&tmp, argc, argv);

	for (int i=0; i<size; i++) {
		printf("%f ", tmp[i]);
	}

	printf("\n");

	return 0;
}
