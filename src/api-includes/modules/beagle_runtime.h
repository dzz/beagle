/* ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~
 * import beagle_runtime
 * =============
 */

extern SDL_Window* opengl_window;
extern char* get_user_specified_application_folder();


MODULE_FUNC host_abort
DEF_ARGS {
    exit(1);    
}

MODULE_FUNC host_get_char_dims
DEF_ARGS {
    return Py_BuildValue("i", api_host_get_char_dims() );
}

MODULE_FUNC host_get_user_specified_application_folder 
DEF_ARGS{
    if( get_user_specified_application_folder() ) {
        return Py_BuildValue("s", get_user_specified_application_folder() );
    } else Py_RETURN_NONE;
}

MODULE_FUNC host_get_gamepad_count
DEF_ARGS {
    return Py_BuildValue("i",SDL_NumJoysticks());
}

MODULE_FUNC host_dequeue_gamepad_dirty
DEF_ARGS {
    return Py_BuildValue("i",GamepadDequeueIsDirty() );
}

MODULE_FUNC host_get_hf_timer
DEF_ARGS {
    return Py_BuildValue("f",timer_get_ms() );
}

MODULE_FUNC _host_get_screen_width
DEF_ARGS {
    return Py_BuildValue("I",host_get_screen_width() );
}
MODULE_FUNC _host_get_screen_height
DEF_ARGS {
    return Py_BuildValue("I",host_get_screen_height() );
}


MODULE_FUNC host_set_title
DEF_ARGS {
    char* title;
    if(!INPUT_ARGS(args,"s",&title))
        return NULL;
    SDL_SetWindowTitle(opengl_window, title);
    Py_RETURN_NONE;
}

MODULE_FUNC host_log_stdout
DEF_ARGS {
    char* output;
    if(!INPUT_ARGS(args,"s",&output))
        return NULL;
    log_message( CTT2_CLIENT_APPLICATION, LOG_LEVEL_INFO, output);
    Py_RETURN_NONE;
}
MODULE_FUNC host_log_client_message
DEF_ARGS {
    char* output;
    unsigned int level;
    if(!INPUT_ARGS(args,"Is",&level, &output))
        return NULL;
    log_client_message( level, output);
    Py_RETURN_NONE;
}

MODULE_FUNC host_log_set_level
DEF_ARGS {
    unsigned int level;
    if(!INPUT_ARGS(args,"I",&level ))
        return NULL;
    log_set_level( level );
    Py_RETURN_NONE;
}

MODULE_FUNC host_get_gamepad_sticks
DEF_ARGS {
    int i;
    hw_gamepad* gp;

    if(!INPUT_ARGS(args,"i",&i))
        return NULL;

    if(i>=MAX_PADS) 
        return NULL;

     gp = getGamepad(i);

    return Py_BuildValue("ffffff",    gp->left_x,
                                      gp->left_y,
                                      gp->right_x,
                                      gp->right_y,
                                      gp->left_trigger,
                                      gp->right_trigger );
}

MODULE_FUNC host_get_gamepad_button
DEF_ARGS {
    int i;
    unsigned int btn;
    hw_gamepad* gp;

    if(!INPUT_ARGS(args,"iI",&i,&btn))
        return NULL;

    if(i>=MAX_PADS) 
        return NULL;

     gp = getGamepad(i);

    return Py_BuildValue("i", gamepad_get_button( gp,btn ) );
}

/*~=`=`=`=`=`=`=`=`=`=`==`=`=`=`=`=`=`=`=`=`=`=`=``=`=`=`=`=`=`=`=`=`=`=`=`=*/

static PyMethodDef host_methods[] = {
    {"abort",               host_abort,                 METH_VARARGS, NULL},
    {"get_user_specified_application_folder",       host_get_user_specified_application_folder,         
                                                        METH_VARARGS, NULL},
    {"get_char_dims",       host_get_char_dims,         METH_VARARGS, NULL},
    {"get_gamepad_sticks",  host_get_gamepad_sticks,    METH_VARARGS, NULL},
    {"get_gamepad_count",   host_get_gamepad_count,     METH_VARARGS, NULL},
    {"get_gamepad_button",  host_get_gamepad_button,     METH_VARARGS, NULL},
    {"dequeue_gamepad_dirty",   
                            host_dequeue_gamepad_dirty, METH_VARARGS, NULL},
    {"get_hf_timer",        host_get_hf_timer,          METH_VARARGS, NULL},
    {"set_title",           host_set_title,             METH_VARARGS, NULL},
    {"log_stdout",          host_log_stdout,            METH_VARARGS, NULL},
    {"log_client_message",  host_log_client_message,    METH_VARARGS, NULL},
    {"log_set_level",  host_log_set_level,    METH_VARARGS, NULL},
    {"get_screen_width",  _host_get_screen_width,    METH_VARARGS, NULL},
    {"get_screen_height",  _host_get_screen_height,    METH_VARARGS, NULL},

    {NULL,NULL,0,NULL } /*terminator record*/
};

static PyModuleDef host_module = {
    PyModuleDef_HEAD_INIT, "host", NULL, -1, host_methods,
    NULL,NULL,NULL,NULL
};

static PyObject* PyInit_host(void) {
    return PyModule_Create(&host_module);
}


