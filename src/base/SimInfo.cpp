
#include "SimInfo.h"

int logFireInfo(FireInfo log, string type, string name) 
{
	string filename = name.append(".").append(type);

	FILE *file = fopen(filename.c_str(), "w+");
	if (file == NULL) {
		printf("ERROR: Open file %s failed\n", filename.c_str());
		return -1;
	}


	for (int j=0; j<log[type].size; j++) {
		if (type == "Y") {
			real *data = reinterpret_cast<real*>(log[type].data);
			fprintf(file, "%f ", data[j]);
		} else {
			int *data = reinterpret_cast<int*>(log[type].data);
			fprintf(file, "%d ", data[j]);
		}
	}
	fprintf(file, "\n");

	fflush(file);
	fclose(file);

	return 0;
}

int logFireInfo(FireInfo log, string type, int start, int batch, int size, string name) 
{
	string filename = name.append(".").append(type);

	FILE *file = fopen(filename.c_str(), "w+");
	if (file == NULL) {
		printf("ERROR: Open file %s failed\n", filename.c_str());
		return -1;
	}


	for (int i=0; i<batch; i++) {
		for (int j=0; j<size; j++) {
			if (type == "Y") {
				real *data = reinterpret_cast<real*>(log[type].data);
				fprintf(file, "%f ", data[start + i*size + j]);
			} else {
				int *data = reinterpret_cast<int*>(log[type].data);
				fprintf(file, "%d ", data[start + i*size + j]);
			}
		}
		fprintf(file, "\n");
	}

	fflush(file);
	fclose(file);

	return 0;
}
