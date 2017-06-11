 /* ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~
 * import physics
 * =============
 */

#include <chipmunk/chipmunk.h>

MODULE_FUNC physics_space_create
DEF_ARGS {

    cpSpace *space = cpSpaceNew(); 
    cpVect gravity = cpv(0,0);
    cpSpaceSetGravity(space, gravity);
    return Py_BuildValue( PYTHON_POINTER_INT, (marshalled_pointer)space);
}

MODULE_FUNC physics_space_drop
DEF_ARGS {
    marshalled_pointer ptr; 
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    cpSpace *space=(cpSpace*)ptr;
    cpSpaceFree(space);
    Py_RETURN_NONE;
}


/*~=`=`=`=`=`=`=`=`=`=`==`=`=`=`=`=`=`=`=`=`=`=`=``=`=`=`=`=`=`=`=`=`=`=`=`=*/

static PyMethodDef physics_methods[] = {

    /*rect*/
    {"space_create",physics_space_create,METH_VARARGS, NULL},
    {"space_drop",physics_space_drop,METH_VARARGS, NULL},
    
    {NULL,NULL,0,NULL } /*terminator record*/
};

static PyModuleDef physics_module = {
    PyModuleDef_HEAD_INIT, "physics", NULL, -1, physics_methods,
    NULL,NULL,NULL,NULL
};

static PyObject* PyInit_physics(void) {
    return PyModule_Create(&physics_module);
}
