

#include <string.h>
#include <stdarg.h>
#include "python.h"

void printPyErr() 
{
	if (PyErr_Occurred())
		PyErr_Print();
}

PyObject * PyModule(const char * moduleName)
{
	PyObject * module = PyImport_ImportModule(moduleName);
	if (module == NULL) {
		printPyErr();
	}

	return module;
}

PyObject * PyFunc(PyObject * module, const char * funcName)
{
	PyObject * func = PyObject_GetAttrString(module, funcName);
	
	if (func == NULL || !PyCallable_Check(func)){
		printPyErr();
	}

	return func;
}

PyObject * PyCall(PyObject * func, PyObject *args)
{
	PyObject * ret = PyEval_CallObject(func, args);
	if (ret == NULL) {
		printPyErr();
	}

	return ret;
}

PyObject * PyTuple(const char * types, ...)
{
	int num = strlen(types);

	PyObject *tuple = PyTuple_New(num);
	if (tuple == NULL) {
		printPyErr();
		return NULL;
	}

	va_list valist;
	va_start(valist, num);
	for (int i=0; i<num; i++) {
		if (types[i] == 's') {
			PyTuple_SetItem(tuple, i, PyUnicode_FromString(va_arg(valist, char *)));
			printPyErr();
		} else if (types[i] == 'i') {
			PyTuple_SetItem(tuple, i, PyLong_FromLong((long)va_arg(valist, int)));
			printPyErr();
		} else if (types[i] == 'l') {
			PyTuple_SetItem(tuple, i, PyLong_FromLong(va_arg(valist, long)));
			printPyErr();
		} else if (types[i] == 'f') {
			PyTuple_SetItem(tuple, i, PyFloat_FromDouble(va_arg(valist, double)));
			printPyErr();
		} else if (types[i] == 'd') {
			PyTuple_SetItem(tuple, i, PyFloat_FromDouble(va_arg(valist, double)));
			printPyErr();
		} else {
			printf("Unsupported type\n");
			PyTuple_SetItem(tuple, i, Py_None);
			printPyErr();
		}
	}
	va_end(valist);

	return tuple;
}
