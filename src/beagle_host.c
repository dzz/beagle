
#include "memory.h"
//#undef _DEBUG
#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#endif

#ifdef __linux__
#include <stdlib.h>
#endif

#include <Python.h>
#include <SDL_image.h>

#include "drawing/drawingSurfaces.h"
#include "system/ctt2_host.h"
#include "system/ctt2.h"
#include "hwgfx/shader.h"
#include "hwgfx/label.h"
#include "hwgfx/rect.h"
#include "hwgfx/blend_control.h"
#include "hwgfx/texture.h"
#include "hwgfx/primitive.h"
#include "hwgfx/framebuffer.h"
#include "hwgfx/text.h"
#include "hwgfx/misc.h"
#include "hwgfx/context.h"
#include "hwgfx/pointlight.h"
#include "hwgfx/sprite_renderer.h"
#include "hardware/gamepad.h"
#include "hardware/audio.h"
#include "hardware/hf_timer.h"
#include "basicAudio.h"

//build a structure to hold required host callables inside of client
#define CLIENT_FUNCTION(x,y) PyObject* x;
typedef struct {
    PyObject* __module;
    #include "api-includes/client-handler-inventory.h"
} CLIENT_HANDLERS;
#undef CLIENT_FUNCTION
static CLIENT_HANDLERS client_if;


void embed_modules();                       // fwd declaration for use in init

#include "api-includes/api_py_util.h"       // error handling, function calls


//load the python client code:
#define FAIL_RETURN { api_fail_hard(); return 1; }
#ifdef _WIN32
    #define CLIENT_FUNCTION(x,y) client_if.##x = PyObject_GetAttrString\
    (client_if.__module,y); if(client_if.##x==0) FAIL_RETURN
#endif

#ifdef __linux__
    #define CLIENT_FUNCTION(x,y) client_if.x = PyObject_GetAttrString\
    (client_if.__module,y); if(client_if.x==0) FAIL_RETURN
#endif

int api_init() {
    char buffer[1024];
    embed_modules();
    Py_Initialize();

 
	//the nicest thing i can say about this is that it works
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append(os.getcwd())");
    PyRun_SimpleString("sys.path.append(\".\")");
	PyRun_SimpleString("print(os.getcwd())");

    client_if.__module = PyImport_ImportModule("client.beagle.main");
    if(client_if.__module == 0) FAIL_RETURN
    #include "api-includes/client-handler-inventory.h"
    #include "system/client_key_map.h"
    return _pycall_noargs(client_if.init);
}
#undef CLIENT_FUNCTION

//terminate
#ifdef _WIN32
    #define CLIENT_FUNCTION(x,y) Py_CLEAR(client_if.##x);
#endif

#ifdef __linux__
    #define CLIENT_FUNCTION(x,y) Py_CLEAR(client_if.x);
#endif

int api_drop() {
    int ret; 
    ret = _pycall_noargs(client_if.finalize);
    #include "api-includes/client-handler-inventory.h"
    Py_CLEAR(client_if.__module);
    return ret;
}

#include "api-includes/client-handlers.h"  //C function bindings for client_if

/*
 * implementations of modules
 */

#define MODULE_FUNC static PyObject* 
#define DEF_ARGS (PyObject *self, PyObject *args )
#define INPUT_ARGS PyArg_ParseTuple

#include "system/log.h"

#include "api-includes/modules/beagle_runtime.h"
#include "api-includes/modules/hwgfx.h"
#include "api-includes/modules/localgfx.h"
#include "api-includes/modules/audio.h"
#include "api-includes/modules/physics.h"
void embed_modules() {

    PyImport_AppendInittab("beagle_runtime",      &PyInit_host);
    PyImport_AppendInittab("hwgfx",     &PyInit_hwgfx);
    PyImport_AppendInittab("localgfx",  &PyInit_localgfx);
    PyImport_AppendInittab("audio",     &PyInit_audio);
    PyImport_AppendInittab("physics",     &PyInit_physics);
    api_checkfailure();
}
#undef CLIENT_FUNCTION
