

#include <Python.h>

void printPyErr();

PyObject * PyModule(const char * moduleName);

PyObject * PyFunc(PyObject * module, const char * funcName);

PyObject * PyCall(PyObject * func, PyObject *args);

PyObject * PyTuple(const char * types, ...);
