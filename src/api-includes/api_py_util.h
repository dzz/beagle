#include <stdint.h>

typedef intptr_t marshalled_pointer;
//#ifdef _WIN32
//    #define PYTHON_POINTER_INT "I"
//#else
    typedef intptr_t marshalled_pointer;
    #define PYTHON_POINTER_INT "L"
//#endif



void api_fail_hard() {
    if(PyErr_Occurred())
        PyErr_Print();
    host_signal_exit();
}

int api_checkfailure() {
    if(PyErr_Occurred())
        return API_FAILURE;
    return API_NOFAILURE;
}

#define PY_NOARGS 0
int _pycall_noargs(PyObject* func) {
    if(func && PyCallable_Check( func )) {
        PyObject_CallObject(func,PY_NOARGS);
        return api_checkfailure();
    } 
    return API_FAILURE;
}

int api_checkfailure_cleanargs(PyObject* args,PyObject** vals) {
    if(PyErr_Occurred())
        return API_FAILURE;
    Py_CLEAR(args);
    return API_NOFAILURE;
}

/* hax: should be variadic 
 *
 * ideal interface here would be to build calls
 * with variadic and the default py tuple
 * format e.g.
 * _pycall( client_if.{$myfunc}, "sii", mystr, myint, myint2)
 * */

#define MAX_ARGS 16
int _pycall_int_args(PyObject* func,int* args,int nargs) {
    int i;
    if(func && PyCallable_Check( func )) {
        PyObject* py_args = PyTuple_New(nargs);
        PyObject* py_vals [MAX_ARGS] = {0};
        for(i=0;i<nargs;++i) {
            py_vals[i] = PyLong_FromLong((long)args[i]);
            PyTuple_SetItem(py_args,i,py_vals[i]);
        }
        PyObject_CallObject(func,py_args);
        return api_checkfailure_cleanargs(py_args,py_vals);
    } 
    return API_FAILURE;
}

int _pycall_str_arg(PyObject* func, char* arg) {
    if(func && PyCallable_Check( func )) {
        PyObject* py_args = PyTuple_New(1);
        PyObject* py_vals[1] = {0};
        py_vals[0] = Py_BuildValue("s",arg);
        PyTuple_SetItem(py_args, 0, py_vals[0]);
        PyObject_CallObject(func, py_args);

        return api_checkfailure_cleanargs(py_args,py_vals);
    }
    return API_FAILURE;
}
