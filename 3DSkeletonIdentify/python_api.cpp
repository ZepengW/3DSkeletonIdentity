#include "python_api.h"
#include "Python.h"
#include <tuple>

JointMatch::JointMatch()
{
	//初始化python环境
	Py_Initialize();

	PyRun_SimpleString("import sys");

	//导入模块
	//this->pModule = PyImport_ImportModule("test_w");

	PyObject* pFunc = NULL;
	//pFunc = PyObject_GetAttrString((PyObject*)this->pModule, "hello");
	//PyObject_CallNoArgs(pFunc);
}

JointMatch::~JointMatch()
{
	if (NULL != pModule)
	{
		
	}
}
