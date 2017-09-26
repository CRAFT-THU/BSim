
#include <iostream>
#include <string>
#include <python2.7/Python.h>

using std::string;
using std::cout;
using std::endl;

int vision(float ** fire_rate, int argc, char **argv)
{
	Py_Initialize();
	PySys_SetArgv(argc, argv);

	string path = "/home/yhx/BSim/script";
	string chdir_cmd = string("sys.path.append(\"") + path + "\")";
	const char* cstr_cmd = chdir_cmd.c_str();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(cstr_cmd);

	PyObject* moduleName = PyString_FromString("vision"); //模块名，不是文件名
	PyObject* pModule = PyImport_Import(moduleName);
	if (!pModule) // 加载模块失败
	{
		PyErr_Print();
		cout << "[ERROR] Python get module " << "vision" << " failed." << endl;
		return 0;
	}
	//cout << "[INFO] Python get module succeed." << endl;

	// 加载函数
	PyObject* pv = PyObject_GetAttrString(pModule, "test");
	if (!pv || !PyCallable_Check(pv))  // 验证是否加载成功
	{
		PyErr_Print();
		cout << "[ERROR] Can't find funftion (test)" << endl;
		return 0;
	}
	//cout << "[INFO] Get function (test) succeed." << endl;

	// 设置参数
	PyObject* args = Py_BuildValue("()");

	// 调用函数
	PyObject* pRet = PyObject_CallObject(pv, args);

	int ret = 0;
	// 获取参数
	if (pRet)  // 验证是否调用成功
	{
		if (!PyList_Check(pRet)) {
			cout << "Wrong Func! Should return a list" << endl;
			PyErr_Print();
			return -1;
		}

		ret = PyList_Size(pRet);
		float *tmp = (float*)malloc(sizeof(float)*ret);

		if (tmp != NULL) {
			*fire_rate = tmp;
		}

		for (int i=0; i<ret; i++) {
			tmp[i] = (float)PyFloat_AsDouble(PyList_GetItem(pRet, i));
		}
	} else {
		PyErr_Print();
	}

	Py_Finalize();  

	return ret;
}


