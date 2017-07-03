 /* ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~
 * import physics
 * =============
 */

#include <chipmunk/chipmunk.h>

typedef struct {
    cpSpace *space;
    int iter;
    cpFloat ts;
    int release;
    int halt;
} cp_phys_job;

#define MAX_CP_JOBS (4096)
unsigned int cp_phys_job_read_ptr = 0;
unsigned int cp_phys_job_write_ptr = 0;
unsigned int CP_STOPPED = 0;
cp_phys_job cpjobs[MAX_CP_JOBS];

void CP_WAIT_FLUSH() {
    //printf("hang...");
    while(cp_phys_job_read_ptr!=cp_phys_job_write_ptr) printf("...");
    //printf("on!\n");
}

void CP_ISSUE(cp_phys_job job) {
    cpjobs[cp_phys_job_write_ptr] = job;
    cp_phys_job_write_ptr +=1;
    if(cp_phys_job_write_ptr>MAX_CP_JOBS) {
        cp_phys_job_write_ptr = 0;
    }
    //printf("..issue...");
}

void CP_HALT() {
    cp_phys_job job;
    job.halt =1;
    job.release = 0;
    CP_ISSUE(job);
    CP_WAIT_FLUSH();
}

void CP_THREAD() {
    while(!CP_STOPPED) {
        while(cp_phys_job_read_ptr!=cp_phys_job_write_ptr) {
            cp_phys_job job = cpjobs[cp_phys_job_read_ptr];

            if(job.halt==1) {
                CP_STOPPED = 1;
            } else
            if(!job.release) {
                //printf("...STEPPIN..");
                cpSpaceSetIterations( job.space,job.iter );
                cpSpaceStep(job.space,job.ts);
            } else {
                cpSpaceFree(job.space);
            }

            cp_phys_job_read_ptr+=1;
            if(cp_phys_job_read_ptr>MAX_CP_JOBS) {
                cp_phys_job_read_ptr = 0;
            }
        }
    }
}



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
    cp_phys_job job;
    job.space = space;
    job.release = 1;
    job.halt = 0;
    CP_ISSUE(job);

    //cpSpaceFree(space);
    Py_RETURN_NONE;
}

MODULE_FUNC physics_space_step
DEF_ARGS {
    marshalled_pointer ptr; 
    float ts;
    float it;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "ff",&ptr,&ts, &it)) 
        return NULL;
    cpSpace *space=(cpSpace*)ptr;

    cp_phys_job job;
    job.space=space;
    job.iter=(int)it;
    job.ts=ts;
    job.halt = 0;
    job.release = 0;
    CP_ISSUE(job);


    CP_WAIT_FLUSH();
    //cpSpaceSetIterations(space,(int)it);
    //cpSpaceStep(space, (cpFloat)ts);
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

MODULE_FUNC physics_body_get_state
DEF_ARGS {
    marshalled_pointer ptr; 
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    cpBody *body=(cpBody*)ptr;

    cpVect pos = cpBodyGetPosition(body);
    cpVect vel = cpBodyGetVelocity(body);

    return Py_BuildValue("ffff",(float)pos.x,(float)pos.y,(float)vel.x,(float)vel.y);
}

MODULE_FUNC physics_body_set_state
DEF_ARGS {
    marshalled_pointer ptr; 
    cpVect pos;
    cpVect vel;
    float pos_x,pos_y,vel_x,vel_y;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "ffff",&ptr,&pos_x,&pos_y,&vel_x,&vel_y)) 
        return NULL;
    cpBody *body=(cpBody*)ptr;

    pos.x = (cpFloat)pos_x;
    pos.y = (cpFloat)pos_y;
    vel.x = (cpFloat)vel_x;
    vel.y = (cpFloat)vel_y;
    cpBodySetPosition(body,pos);
    cpBodySetVelocity(body,vel);
    Py_RETURN_NONE;
}


/*~=`=`=`=`=`=`=`=`=`=`==`=`=`=`=`=`=`=`=`=`=`=`=``=`=`=`=`=`=`=`=`=`=`=`=`=*/

static PyMethodDef physics_methods[] = {

    /*space*/
    {"space_create",physics_space_create,METH_VARARGS, NULL},
    {"space_step",physics_space_step,METH_VARARGS, NULL},
    {"space_drop",physics_space_drop,METH_VARARGS, NULL},
    /*shape*/
    {"fixed_segment_create",physics_fixed_segment_create,METH_VARARGS, NULL},
    {"shape_drop",physics_shape_drop,METH_VARARGS, NULL},
    /*body*/
    {"create_circle_body", physics_create_circle_body, METH_VARARGS, NULL},
    {"body_drop", physics_body_drop, METH_VARARGS,NULL},
    {"body_set_state", physics_body_set_state, METH_VARARGS,NULL},
    {"body_get_state", physics_body_get_state, METH_VARARGS,NULL},
    
    {NULL,NULL,0,NULL } /*terminator record*/
};

static PyModuleDef physics_module = {
    PyModuleDef_HEAD_INIT, "physics", NULL, -1, physics_methods,
    NULL,NULL,NULL,NULL
};

static PyObject* PyInit_physics(void) {
    return PyModule_Create(&physics_module);
}
