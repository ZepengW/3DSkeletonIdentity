#include "python_api.h"
#include "Python.h"
#include <tuple>

JointMatch::JointMatch()
{
	//��ʼ��python����
	Py_Initialize();

	PyRun_SimpleString("import sys");

	//����ģ��
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
