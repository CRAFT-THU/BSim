
#ifndef FILEOP_H
#define FILEOP_H

#include <stdio.h>
#include <iostream>
#include <assert.h>

inline FILE * openFile(const char *filename, const char *mode) {
	FILE * file = fopen(filename, mode);
	if (file == NULL) {
		printf("ERROR: Open file %s failed\n", filename);
	}
	assert(file != NULL);
	return file;
}

inline int closeFile(FILE *file) {
	return fclose(file);
}

#endif // FILEOP_H
