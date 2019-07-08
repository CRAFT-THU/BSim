/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <string.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../../src/utils/python.h"
#include "../../include/BSim.h"

using namespace std;

using std::vector;
using ::testing::AtLeast;
using ::testing::ElementsAreArray;

TEST(MultiGPUTest, ResultTest) {
	Py_Initialize();
	ASSERT_TRUE(Py_IsInitialized());

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('../../script')");
	PyRun_SimpleString("sys.path.append('../script')");

	PyObject * pModule1 = PyModule("column_merge");
	PyObject * find_files = PyFunc(pModule1, "find_series_files");
	PyObject * filename = PyTuple("s", "v.data");
	PyObject * files = PyCall(find_files, filename);

	PyObject * column_merge = PyObject_GetAttrString(pModule1, "column_merge");
	PyEval_CallObject(column_merge, files);

	PyObject * pModule2 = PyModule("data_compare");
	PyObject * column_sub = PyFunc(pModule2, "column_sub");
	PyObject * delta = PyCall(column_sub, Py_BuildValue("ss", "v.data", "v_merge.data"));
	double res = PyFloat_AsDouble(delta);
	ASSERT_LT(res, 1e-7);

	Py_DECREF(filename);
	Py_DECREF(files);
	Py_DECREF(find_files);
	Py_DECREF(column_merge);
	Py_DECREF(pModule1);
	Py_DECREF(delta);
	Py_DECREF(column_sub);
	Py_DECREF(pModule2);
	Py_Finalize();
}

int main(int argc, char **argv)
{
	bool load = false;

	if ((argc > 1) && (0==strcmp(argv[1], "load"))) {
		load = true;
	}

	const int N = 5;
	Network c;
	Population *pn0 = c.createPopulation(N, LIF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 0e-1), 1.0, 1.0));
	Population *pn1 = c.createPopulation(N, LIF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 10e-1), 1.0, 1.0));
	Population *pn2 = c.createPopulation(N, LIF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 0.0e-3), 1.0, 1.0));

	real * weight0 = NULL;
	real * weight1 = NULL;
	real * delay = NULL;

	if (load) { 
		printf("LOAD DATA...\n");
		weight0 = loadArray("weight0.csv", N*N);
		weight1 = loadArray("weight1.csv", N*N);
		delay = loadArray("delay.csv", N*N);
		printf("LOAD DATA FINISHED\n");
	} else {
		printf("GENERATE DATA...\n");
		weight0 = getRandomArray((real)3e-3, N*N);
		weight1 = getRandomArray((real)4e-3, N*N);
		delay = getConstArray((real)1e-3, N*N);
		printf("GENERATE DATA FINISHED\n");
	}

	c.connect(pn0, pn1, weight0, delay, NULL, N*N);
	c.connect(pn1, pn2, weight1, delay, NULL, N*N);

	SGSim sg(&c, 1.0e-3);
	sg.run(0.1);
	MGSim mg(&c, 1.0e-3);
	mg.run(0.1);

	if (!load) {
		printf("SAVE DATA...\n");
		saveArray("weight0.csv", weight0, N*N);
		saveArray("weight1.csv", weight1, N*N);
		saveArray("delay.csv", delay, N*N);
		printf("SAVE DATA FINISHED\n");
	}

	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
	
	return 0;
} 
