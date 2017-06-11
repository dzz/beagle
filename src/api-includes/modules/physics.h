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

MODULE_FUNC physics_fixed_segment_create
DEF_ARGS {
    marshalled_pointer space_ptr;
    float x1,y1,x2,y2,fric;
    if(!INPUT_ARGS(args, PYTHON_POINTER_INT "fffff",&space_ptr, &x1,&y1,&x2,&y2,&fric))
        return NULL;
   
    cpSpace *space = (cpSpace*)space_ptr; 
    cpShape *segment = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(x1,y1),cpv(x2,y2),0);
    cpShapeSetFriction( segment, fric );
    cpSpaceAddShape(space,segment);
    
    return Py_BuildValue( PYTHON_POINTER_INT, (marshalled_pointer)segment);
}

MODULE_FUNC physics_shape_drop
DEF_ARGS {
    marshalled_pointer ptr; 
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    cpShape *shape=(cpShape*)ptr;
    cpShapeFree(shape);
    Py_RETURN_NONE;
}

MODULE_FUNC physics_create_circle_body
DEF_ARGS {
    marshalled_pointer ptr; 
    float radius, mass,x,y, friction;
 
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "fffff",&ptr, &radius, &mass, &x, &y, &friction)) 
        return NULL;
    cpSpace *space=(cpSpace*)ptr;
    cpFloat moment = cpMomentForCircle( mass,0, radius, cpvzero );
    cpBody *circleBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    
    cpBodySetPosition( circleBody, cpv(x, y));

    cpShape *circleShape = cpSpaceAddShape(space, cpCircleShapeNew( circleBody, radius, cpvzero));
    cpShapeSetFriction( circleShape, friction );

    return Py_BuildValue(PYTHON_POINTER_INT PYTHON_POINTER_INT, circleBody, circleShape );
}

MODULE_FUNC physics_body_drop
DEF_ARGS {
    marshalled_pointer ptr; 
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    cpBody *body=(cpBody*)ptr;
    cpBodyFree(body);
    Py_RETURN_NONE;
}


/*~=`=`=`=`=`=`=`=`=`=`==`=`=`=`=`=`=`=`=`=`=`=`=``=`=`=`=`=`=`=`=`=`=`=`=`=*/

static PyMethodDef physics_methods[] = {

    /*space*/
    {"space_create",physics_space_create,METH_VARARGS, NULL},
    {"space_drop",physics_space_drop,METH_VARARGS, NULL},
    /*shape*/
    {"fixed_segment_create",physics_fixed_segment_create,METH_VARARGS, NULL},
    {"shape_drop",physics_shape_drop,METH_VARARGS, NULL},
    /*body*/
    {"create_circle_body", physics_create_circle_body, METH_VARARGS, NULL},
    {"body_drop", physics_body_drop, METH_VARARGS,NULL},
    
    {NULL,NULL,0,NULL } /*terminator record*/
};

static PyModuleDef physics_module = {
    PyModuleDef_HEAD_INIT, "physics", NULL, -1, physics_methods,
    NULL,NULL,NULL,NULL
};

static PyObject* PyInit_physics(void) {
    return PyModule_Create(&physics_module);
}
