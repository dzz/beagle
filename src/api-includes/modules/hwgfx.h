
 /* ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~
 * import hwgfx
 * =============
 */

#define PRIMITIVE_FLOAT_ERROR log_message( CTT2_INT_API_BRIDGE, LOG_LEVEL_ERROR,"Failed to convert input to floating point");api_fail_hard();
/**
 *  rect
 */
MODULE_FUNC hwgfx_rect_draw
DEF_ARGS  {
    int x,y,w,h;

    if(!INPUT_ARGS(args,"iiii",&x,&y,&w,&h)) 
        return NULL;
    rect_draw(x,y,w,h);
    Py_RETURN_NONE;
}

/**
 *  rect_tex
 */
MODULE_FUNC hwgfx_rect_draw_tex
DEF_ARGS  {
    int x,y,w,h;
    float u,v,tw,th;

    if(!INPUT_ARGS(args,"iiiiffff",&x,&y,&w,&h,&u,&v,&tw,&th)) 
        return NULL;
    rect_draw_tex(x,y,w,h,u,v,tw,th);
    Py_RETURN_NONE;
}

/**
 * rect_tex_array
 */

MODULE_FUNC hwgfx_rect_draw_tex_array
DEF_ARGS {
    
    PyObject *array;
    int i;
    int size;

    size = 10;

    if(!INPUT_ARGS(args,"O", &array))
        return NULL;
    if(!PyList_Check(array))
        return NULL;
    
    size = PyList_Size(array);
    for(i=0; i< size; ++i) {
        int x,y,w,h;
        int t_size;
        float u,v,tw,th;
        PyObject *rect = PyList_GET_ITEM(array,i);
        t_size = PyList_Size(rect);
        if(t_size!= 8) {
            return NULL;
        }
        x = (int)PyLong_AS_LONG( PyList_GET_ITEM(rect,0));
        y = (int)PyLong_AS_LONG( PyList_GET_ITEM(rect,1));
        w = (int)PyLong_AS_LONG( PyList_GET_ITEM(rect,2));
        h = (int)PyLong_AS_LONG( PyList_GET_ITEM(rect,3));
        u = (float)PyFloat_AS_DOUBLE( PyList_GET_ITEM(rect,4));
        v = (float)PyFloat_AS_DOUBLE( PyList_GET_ITEM(rect,5));
        tw = (float)PyFloat_AS_DOUBLE( PyList_GET_ITEM(rect,6));
        th = (float)PyFloat_AS_DOUBLE( PyList_GET_ITEM(rect,7));
        rect_draw_tex(x,y,w,h,u,v,tw,th);

    }
    Py_RETURN_NONE;
}

/**
 * text
 */
MODULE_FUNC hwgfx_text_render
DEF_ARGS {
    float x,y,r,g,b;
    char* py_str_txt;
    if(!INPUT_ARGS(args, "fffffs", &x, &y, &r, &g, &b, &py_str_txt))
        return NULL;

    text_render(x,y,r,g,b,py_str_txt);

    Py_RETURN_NONE;
}
/**
 * label
 */
MODULE_FUNC hwgfx_label_generate 
DEF_ARGS {
    gfx_label* label = (gfx_label*)malloc(sizeof(gfx_label));

    label_generate(label);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)label);
}

MODULE_FUNC hwgfx_label_drop 
DEF_ARGS {
    marshalled_pointer ptr; 
    gfx_label* label;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    label = (gfx_label*)ptr;
    label_drop (label);
    free(label);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_label_set_text 
DEF_ARGS {
    marshalled_pointer ptr; 
    char *text; 
    gfx_label* label;
    char* py_str_txt;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "s",&ptr, &py_str_txt)) 
        return NULL;
    label   = (gfx_label*)ptr;
    text    = py_str_txt;
    label_set_text(label,text);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_label_render
DEF_ARGS {
    marshalled_pointer ptr; 
    float x,y,r,g,b;
    gfx_label* label;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "fffff",&ptr,&x,&y,&r,&g,&b)) 
        return NULL;
    label = (gfx_label*)ptr;

    label_render (label, x, y,r,g,b);
    Py_RETURN_NONE;
}
/**
 * blend
 */
MODULE_FUNC hwgfx_blend_enter
DEF_ARGS{
    int mode;
    if(!INPUT_ARGS(args,"i",&mode))
        return NULL;
    blend_enter(mode);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_blend_exit
DEF_ARGS{
    blend_exit();
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_manual_blend_enter
DEF_ARGS{
    int mode;
    if(!INPUT_ARGS(args,"i",&mode))
        return NULL;
    manual_blend_enter(mode);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_manual_blend_exit
DEF_ARGS{
    manual_blend_exit();
    Py_RETURN_NONE;
}

/**
 * framebuffer
 */

MODULE_FUNC hwgfx_framebuffer_create
DEF_ARGS {
    gfx_framebuffer* framebuffer;
    framebuffer = (gfx_framebuffer*)malloc(sizeof(framebuffer));
    framebuffer_create_framebuffer(framebuffer);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)framebuffer);
}

MODULE_FUNC hwgfx_framebuffer_drop
DEF_ARGS {
    marshalled_pointer ptr; 
    gfx_framebuffer* framebuffer;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    framebuffer = (gfx_framebuffer*)ptr;
    framebuffer_drop(framebuffer);
    //free(framebuffer);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_framebuffer_bind_texture
DEF_ARGS {
    marshalled_pointer ptr_fb,ptr_tex; 
    gfx_framebuffer* framebuffer;
    gfx_texture* texture;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT PYTHON_POINTER_INT,&ptr_fb,&ptr_tex)) 
        return NULL;
    framebuffer = (gfx_framebuffer*)ptr_fb;
    texture = (gfx_texture*)ptr_tex;
    framebuffer_bind_texture(framebuffer, texture);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_framebuffer_render_start
DEF_ARGS {
    marshalled_pointer ptr; 
    gfx_framebuffer* framebuffer;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    framebuffer = (gfx_framebuffer*)ptr;
    framebuffer_render_start(framebuffer);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_framebuffer_render_end
DEF_ARGS {
    marshalled_pointer ptr; 
    gfx_framebuffer* framebuffer;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    framebuffer = (gfx_framebuffer*)ptr;
    framebuffer_render_end(framebuffer);
    Py_RETURN_NONE;
}

/**
 * texture
 */

MODULE_FUNC hwgfx_texture_generate
DEF_ARGS {
    gfx_texture* texture;
    int w,h,filtered;
    if(!INPUT_ARGS(args, "iip", &w, &h, &filtered))
        return NULL;

    texture = (gfx_texture*) malloc(sizeof(gfx_texture));
    if( filtered == 0) 
        texture_generate(texture,w,h);
    else
        texture_generate_filtered(texture,w,h);

    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)texture);
}


//serious hacking here with the dunder for testing x64 windows builds...
float* __fp_data(int w,int h) {
        float* texture_data;
        int i;
        int addr=0;

        texture_data = malloc( w*h*4 * sizeof(float));
        for(i=0; i<(w*h);++i) {
            texture_data[addr++]=0.0f;
            texture_data[addr++]=0.0f;
            texture_data[addr++]=0.0f;
            texture_data[addr++]=0.0f;
        }
        return texture_data;
}


MODULE_FUNC hwgfx_texture_generate_fp
DEF_ARGS {
    gfx_texture* texture;
    PyObject* float_list;
    PyObject* flObj;
    int num_floats;
    int filtered;

    int w,h;
    if(!INPUT_ARGS(args, "iiO!p", &w, &h, &PyList_Type, &float_list, &filtered))
        return NULL;

    float *texture_data = __fp_data(w,h);
    num_floats = PyList_Size(float_list);

    int x = 0;
    int y = 0;

    float parsed;
    for(int i=0; i<num_floats;++i) {
        if(x==w) { x = 0; y+=1; }
        if(y==h) { break; }
        flObj = PyList_GetItem(float_list,i);
        if(PyFloat_Check(flObj)) {
            parsed = (float)PyFloat_AsDouble(flObj);
        } else {
            PRIMITIVE_FLOAT_ERROR;
        }
        texture_data[i] = parsed;
    }

    texture = (gfx_texture*) malloc(sizeof(gfx_texture));
    if(filtered) {
        texture_generate_fp_data_filtered(texture,w,h,texture_data);
    } else {
        texture_generate_fp_data(texture,w,h,texture_data);
    }
    free(texture_data);

    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)texture);
}


MODULE_FUNC hwgfx_pointlight_render_scene
DEF_ARGS {
    gfx_pointlight_context context;
    gfx_pointlight_context* ctxt = &context;

    PyObject* encoded_lines;
    PyObject* encoded_lights;

    if(!INPUT_ARGS(args, "O!O!fffff" PYTHON_POINTER_INT "ii",
            &encoded_lines, 
            &encoded_lights,
            &(ctxt->view_x),
            &(ctxt->view_y),
            &(ctxt->camera_x),
            &(ctxt->camera_y),
            &(ctxt->camera_scale),
            &(ctxt->composite_target),
            &(ctxt->composite_target_w),
            &(ctxt->composite_target_h)
        ))
        return NULL;
    

    {
        int num_floats = PyList_Size(encoded_lines);
        PyObject* flObj;
        ctxt->encoded_lines = malloc(sizeof(float)*num_floats);
        for(int i=0; i< num_floats; ++i) {
            flObj = PyList_GetItem(encoded_lines,i);
            ctxt->encoded_lines[i] = (float)PyFloat_AsDouble(flObj);
        }
    }

    {
        int num_lights = PyList_Size(encoded_lights) / 7;
        ctxt->lights = malloc(sizeof(gfx_pointlight)*num_lights);
        for(int i=0; i< num_lights; ++i) {
            int origin = i*7;
            gfx_pointlight* light = &(ctxt->lights[i]);
            light->x = (float)PyFloat_AsDouble(PyList_GetItem(encoded_lights,origin));
            light->y = (float)PyFloat_AsDouble(PyList_GetItem(encoded_lights,origin+1));
            light->r = (float)PyFloat_AsDouble(PyList_GetItem(encoded_lights,origin+2));
            light->g = (float)PyFloat_AsDouble(PyList_GetItem(encoded_lights,origin+3));
            light->b = (float)PyFloat_AsDouble(PyList_GetItem(encoded_lights,origin+4));
            light->radius = (float)PyFloat_AsDouble(PyList_GetItem(encoded_lights,origin+5));
            light->style = (int)PyLong_AsLong(PyList_GetItem(encoded_lights,origin+6));
        }
    }

    free(ctxt->encoded_lines);
    free(ctxt->lights);
}

MODULE_FUNC hwgfx_texture_drop
DEF_ARGS {
    marshalled_pointer ptr; 
    gfx_texture* texture;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    texture = (gfx_texture*)ptr;
    texture_drop(texture);
    //free(texture);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_texture_bind
DEF_ARGS {
    marshalled_pointer ptr; 
    int tex_unit;
    gfx_texture* texture;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "i",&ptr, &tex_unit)) 
        return NULL;
    texture = (gfx_texture*)ptr;
    texture_bind(texture, tex_unit);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_texture_upload 
DEF_ARGS {
    marshalled_pointer tptr,iptr;
    gfx_texture* texture;
    DRAWING_SURFACE ds;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT PYTHON_POINTER_INT,&tptr,&iptr))
        return NULL;
    texture = (gfx_texture*)tptr;
    ds      = (DRAWING_SURFACE)iptr;
    texture_from_SDL_surface(texture,ds);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_texture_download 
DEF_ARGS {
    marshalled_pointer tptr,iptr;
    gfx_texture* texture;
    DRAWING_SURFACE ds;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT PYTHON_POINTER_INT,&tptr,&iptr))
        return NULL;
    texture = (gfx_texture*)tptr;
    ds      = (DRAWING_SURFACE)iptr;
    texture_download(texture,ds);
    Py_RETURN_NONE;
}

/**
 * shader
 */
MODULE_FUNC hwgfx_shader_load
DEF_ARGS {
    gfx_shader* shader;
    char*       vert;
    char*       frag;

    if(!INPUT_ARGS(args, "ss", &vert, &frag)){
        return NULL;
    }
    shader = (gfx_shader*)malloc(sizeof(gfx_shader));
    shader_load(shader, vert, frag);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)shader);
}

MODULE_FUNC hwgfx_shader_compile
DEF_ARGS {
    gfx_shader* shader;
    char*       vert;
    char*       frag;

    if(!INPUT_ARGS(args, "ss", &vert, &frag)){
        return NULL;
    }
    shader = malloc(sizeof(gfx_shader));
    shader_compile(shader, vert, frag, "","");
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)shader);
}

MODULE_FUNC hwgfx_shader_drop
DEF_ARGS    {
    marshalled_pointer ptr;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr))
        return NULL;
    shader_drop ((gfx_shader*)ptr);    
    //free        ((gfx_shader*)ptr);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_shader_bind
DEF_ARGS    {
   marshalled_pointer ptr;
   if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr))
       return NULL;
   shader_bind  ((gfx_shader*)ptr);
   Py_RETURN_NONE;
}


MODULE_FUNC hwgfx_shader_bind_float
DEF_ARGS {
    float x;
    marshalled_pointer ptr;
    char* param;
    gfx_shader* shader;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "sf",&ptr,&param,&x))
        return NULL;
    shader = (gfx_shader*)ptr;
    shader_bind_float(shader,param,x);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_shader_bind_int
DEF_ARGS {
    int v;
    marshalled_pointer ptr;
    char* param;
    gfx_shader* shader;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "si",&ptr,&param,&v))
        return NULL;
    shader = (gfx_shader*)ptr;
    shader_bind_int(shader,param,v);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_shader_bind_floats
DEF_ARGS {
    unsigned int i;
    PyObject* float_list;
    gfx_float* float_buffer;
    int num_floats;
    float parsed;
    marshalled_pointer ptr;
    char* param;
    PyObject* flObj;
    gfx_shader* shader;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "sO!",&ptr, &param, &PyList_Type, &float_list ))
        return NULL;

    num_floats = PyList_Size(float_list);
    float_buffer = (gfx_float*)malloc(sizeof(gfx_float)*num_floats);
 
    for(i=0; i<num_floats;++i) {
        flObj = PyList_GetItem(float_list,i);
        if(PyFloat_Check(flObj)) {
            parsed = (float)PyFloat_AsDouble(flObj);
        } else {
            PRIMITIVE_FLOAT_ERROR;
        }
        float_buffer[i] = parsed;
    }
    shader = (gfx_shader*)ptr;
    shader_bind_floats( shader, param, float_buffer, num_floats );
    free(float_buffer);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_shader_bind_vec2
DEF_ARGS {
    float x,y;
    marshalled_pointer ptr;
    char* param;
    gfx_shader* shader;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "sff",&ptr,&param,&x,&y))
        return NULL;
    shader = (gfx_shader*)ptr;
    shader_bind_vec2(shader,param,x,y);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_shader_bind_vec3
DEF_ARGS {
    float x,y,z;
    marshalled_pointer ptr;
    char* param;
    gfx_shader* shader;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "sfff",&ptr,&param,&x,&y,&z))
        return NULL;
    shader = (gfx_shader*)ptr;
    shader_bind_vec3(shader,param,x,y,z);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_shader_bind_vec4
DEF_ARGS {
    float x,y,z,w;
    marshalled_pointer ptr;
    char* param;
    gfx_shader* shader;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "sffff",&ptr,&param,&x,&y,&z,&w))
        return NULL;
    shader = (gfx_shader*)ptr;
    shader_bind_vec4(shader,param,x,y,z,w);
    Py_RETURN_NONE;
}


MODULE_FUNC hwgfx_shader_bind_texture
DEF_ARGS {
    marshalled_pointer ptr;
    marshalled_pointer tex_ptr;
    char* param;
    gfx_shader* shader;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "s" PYTHON_POINTER_INT,&ptr,&param,&tex_ptr))
        return NULL;
    shader = (gfx_shader*)ptr;
    shader_bind_texture(shader,param,(gfx_texture*)tex_ptr);
    Py_RETURN_NONE;
}
/**
 * drawmode_map
 */
MODULE_FUNC hwgfx_drawmode_get_map
DEF_ARGS {
    return Py_BuildValue( 
    /* primitive.py */
    "[i,i,i,i,i,i,i]",  GL_POINTS,
                        GL_LINE_STRIP,
                        GL_LINE_LOOP,
                        GL_LINES,
                        GL_TRIANGLE_STRIP,
                        GL_TRIANGLE_FAN,
                        GL_TRIANGLES 
                        );
}
/**
 * primitive
 */

MODULE_FUNC hwgfx_primitive_create_coordinate_primitive
DEF_ARGS {
    int                             i;
    gfx_coordinate_primitive*       primitive;
    PyObject*                       coord_float_list;
    int                             vlen;
    int                             num_coord_floats;
    gfx_float*                      coord_float_buffer;
    float                           parsed;
    PyObject*                       flObj;
    int                             draw_mode;
    //see python c API docs for how O! and &PyList_Type 
    //works to validate list
    if (!INPUT_ARGS(args,"O!ii",&PyList_Type, &coord_float_list,
                &vlen, &draw_mode))
        return NULL;

    num_coord_floats       = PyList_Size(coord_float_list);
    coord_float_buffer    = (gfx_float*)malloc(sizeof(gfx_float)*num_coord_floats);
    for(i=0; i<num_coord_floats;++i) {
        flObj = PyList_GetItem(coord_float_list,i);
        if(PyFloat_Check(flObj)) {
            parsed=(float)PyFloat_AsDouble(flObj);
        } else {
            PRIMITIVE_FLOAT_ERROR;
        }
        coord_float_buffer[i] = parsed;
    }
    primitive = (gfx_coordinate_primitive*)malloc(sizeof(gfx_coordinate_primitive));
    primitive_create_coordinate_primitive(  primitive,
                                            coord_float_buffer,
                                            num_coord_floats / vlen ,
                                            vlen);
    free(coord_float_buffer);
    primitive->mode = draw_mode;
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)primitive);
}


MODULE_FUNC hwgfx_primitive_create_channel_primitive
DEF_ARGS {

    gfx_channel_primitive*  primitive;
    PyObject* channel_list;
    PyObject* chanFloatPerVert_list;
    gfx_float** float_buffers;
    int* chanFloatPerVerts;
    int ChannelCount;
    int computedNumVerts = 0;

    if(!INPUT_ARGS(args,"O!O!", &PyList_Type, &channel_list, &PyList_Type, &chanFloatPerVert_list)) {
        return NULL;
    }


    ChannelCount = PyList_Size(chanFloatPerVert_list);
    float_buffers = malloc( sizeof(gfx_float*)*ChannelCount);
    chanFloatPerVerts = malloc( sizeof(int)*ChannelCount);
    

	//printf("%d ChannelCount\n",ChannelCount);

    for(int i=0;i<ChannelCount;++i) {
        //printf("parsing channel %d\n",i);
        {
            int chanFloatPerVert;
            int chansize;
            int chanTotalFloatCount;
            PyObject* chan = PyList_GetItem(channel_list, i);
            PyObject* spec = PyList_GetItem(chanFloatPerVert_list, i);
            chanFloatPerVert = (int)PyLong_AsLong(spec);
            chansize = PyList_Size(chan);
            float_buffers[i] = malloc(sizeof(gfx_float)*chanFloatPerVert*chansize);
            chanFloatPerVerts[i] = chanFloatPerVert; 
            chanTotalFloatCount = chansize;

            if(computedNumVerts == 0) {
                computedNumVerts = chansize/chanFloatPerVert;
            } else {
                int validateVertCount = chansize/chanFloatPerVert;
                if(validateVertCount!=computedNumVerts) {
                    printf("HARD FAIL!!!! BAD CHANNEL PRIMITIVE DEFINITION SUBMITTED FROM USERSPACE\n");
                    exit(1);
                }
            }

            for(int j=0;j < chanTotalFloatCount; ++j) {
                PyObject* flObj = PyList_GetItem(chan,j);
                gfx_float parsed = (gfx_float)PyFloat_AsDouble(flObj);
                float_buffers[i][j] = parsed;
            }
        }
    }


    primitive = (gfx_channel_primitive*)malloc(sizeof(gfx_channel_primitive));
    primitive_create_channel_primitive((void*)primitive, ChannelCount, float_buffers, chanFloatPerVerts, computedNumVerts);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)primitive);
}

MODULE_FUNC hwgfx_primitive_update_channel_primitive
DEF_ARGS {

    gfx_channel_primitive*  primitive;
    PyObject* channel_list;
    PyObject* chanFloatPerVert_list;
    gfx_float** float_buffers;
    int* chanFloatPerVerts;
    int ChannelCount;
    int computedNumVerts = 0;
    marshalled_pointer ptr;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "O!O!", &ptr, &PyList_Type, &channel_list, &PyList_Type, &chanFloatPerVert_list)) {
        return NULL;
    }


    ChannelCount = PyList_Size(chanFloatPerVert_list);
    float_buffers = malloc( sizeof(gfx_float*)*ChannelCount);
    chanFloatPerVerts = malloc( sizeof(int)*ChannelCount);
    

	//printf("%d ChannelCount\n",ChannelCount);

    for(int i=0;i<ChannelCount;++i) {
        //printf("parsing channel %d\n",i);
        {
            int chanFloatPerVert;
            int chansize;
            int chanTotalFloatCount;
            PyObject* chan = PyList_GetItem(channel_list, i);
            PyObject* spec = PyList_GetItem(chanFloatPerVert_list, i);
            chanFloatPerVert = (int)PyLong_AsLong(spec);
            chansize = PyList_Size(chan);
            float_buffers[i] = malloc(sizeof(gfx_float)*chanFloatPerVert*chansize);
            chanFloatPerVerts[i] = chanFloatPerVert; 
            chanTotalFloatCount = chansize;

            if(computedNumVerts == 0) {
                computedNumVerts = chansize/chanFloatPerVert;
            } else {
                int validateVertCount = chansize/chanFloatPerVert;
                if(validateVertCount!=computedNumVerts) {
                    printf("HARD FAIL!!!! BAD CHANNEL PRIMITIVE DEFINITION SUBMITTED FROM USERSPACE\n");
                    exit(1);
                }
            }

            for(int j=0;j < chanTotalFloatCount; ++j) {
                PyObject* flObj = PyList_GetItem(chan,j);
                gfx_float parsed = (gfx_float)PyFloat_AsDouble(flObj);
                float_buffers[i][j] = parsed;
            }
        }
    }


    primitive = (gfx_channel_primitive*)ptr;
    primitive_update_channel_primitive((void*)primitive, float_buffers, chanFloatPerVerts, computedNumVerts);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_primitive_destroy_channel_primitive
DEF_ARGS {

    marshalled_pointer ptr;
    gfx_channel_primitive* primitive;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;

    primitive = (gfx_channel_primitive*)ptr;
    primitive_destroy_channel_primitive(primitive);

    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_primitive_create_coordinate_uv_primitive
DEF_ARGS {
    int                             i;
    gfx_coordinate_uv_primitive*    primitive;
    PyObject*                       coord_float_list;
    PyObject*                       uv_float_list;
    int                             vlen;
    int                             num_coord_floats; 
    int                             num_uv_floats;
    gfx_float*                      coord_float_buffer;
    gfx_float*                      uv_float_buffer;
    float                           parsed;
    PyObject*                       flObj;
    int                             draw_mode;
    //see python c API docs for how O! and &PyList_Type 
    //works to validate list
    if(!INPUT_ARGS(args,"O!O!ii",
                                &PyList_Type, &coord_float_list, 
                                &PyList_Type, &uv_float_list,
                                &vlen, &draw_mode))
        return NULL;
    num_coord_floats       = PyList_Size(coord_float_list);
    num_uv_floats          = PyList_Size(uv_float_list);
    
    coord_float_buffer     = (gfx_float*)malloc(sizeof(gfx_float)*num_coord_floats);
    uv_float_buffer        = (gfx_float*)malloc(sizeof(gfx_float)*num_uv_floats);

    //coords
    for(i=0; i<num_coord_floats;++i) {
        flObj           = PyList_GetItem(coord_float_list,i);
        if(PyFloat_Check(flObj)) {
            parsed=(float)PyFloat_AsDouble(flObj);
        } else {
            PRIMITIVE_FLOAT_ERROR;
        }
        coord_float_buffer[i] = parsed;
    }
    //uvs
    for(i=0; i<num_uv_floats;++i) {
        flObj           = PyList_GetItem(uv_float_list,i);
        if(PyFloat_Check(flObj)) {
            parsed=(float)PyFloat_AsDouble(flObj);
        } else {
            PRIMITIVE_FLOAT_ERROR;
        }
        uv_float_buffer[i] = parsed;
    }

    primitive = (gfx_coordinate_uv_primitive*)malloc(sizeof(gfx_coordinate_uv_primitive));

    primitive_create_coordinate_uv_primitive
            (  primitive,
               coord_float_buffer,
               uv_float_buffer,
               num_coord_floats / vlen ,
               vlen );

    free(coord_float_buffer);
    free(uv_float_buffer);

    primitive->mode = draw_mode;
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)primitive);
}

MODULE_FUNC hwgfx_primitive_render
DEF_ARGS {
    marshalled_pointer ptr;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr))
        return NULL;

    primitive_render((gfx_coordinate_primitive*)ptr);

    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_primitive_destroy_coordinate_primitive
DEF_ARGS {
    marshalled_pointer ptr;
    gfx_coordinate_primitive* primitive;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    primitive = (gfx_coordinate_primitive*)ptr;
    primitive_destroy_coordinate_primitive(primitive);
    //free(primitive);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_primitive_destroy_coordinate_uv_primitive
DEF_ARGS {
    marshalled_pointer ptr;
    gfx_coordinate_uv_primitive* primitive;

    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr)) 
        return NULL;
    primitive = (gfx_coordinate_uv_primitive*)ptr;
    primitive_destroy_coordinate_uv_primitive(primitive);
    //free(primitive);
    Py_RETURN_NONE;
}
/**
 * viewport
 */

MODULE_FUNC hwgfx_viewport_set
DEF_ARGS {
    viewport_dims dims;
    if(!INPUT_ARGS(args,"iiii", &dims.x, &dims.y, &dims.w, &dims.h))
        return NULL;

    gfx_viewport_set_dims(dims);
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_viewport_reset
DEF_ARGS {
    root_gfx_size rs = gfx_get_root_gfx_size();
    viewport_dims def_dims;
    def_dims.x=0;
    def_dims.y=0;
    def_dims.w = rs.w;
    def_dims.h = rs.h;
    gfx_viewport_set_dims(def_dims);
    Py_RETURN_NONE;
}
/**
 * misc
 **/
MODULE_FUNC hwgfx_clear
DEF_ARGS {
    gfx_clear();
    Py_RETURN_NONE;
}

MODULE_FUNC hwgfx_set_clear_color
DEF_ARGS {
    float r,g,b,a;
    if(!INPUT_ARGS(args,"ffff", &r,&g,&b,&a))
        return NULL;
    gfx_set_clear_color(r,g,b,a);
    Py_RETURN_NONE;
}
/**
 * debug
 */
MODULE_FUNC hwgfx_debug_displaykill
DEF_ARGS {
    Py_RETURN_NONE;
}


/*~=`=`=`=`=`=`=`=`=`=`==`=`=`=`=`=`=`=`=`=`=`=`=``=`=`=`=`=`=`=`=`=`=`=`=`=*/

static PyMethodDef hwgfx_methods[] = {

    /*rect*/
    {"rect_draw",           hwgfx_rect_draw,            METH_VARARGS, NULL},
    {"rect_draw_tex",       hwgfx_rect_draw_tex,        METH_VARARGS, NULL},
    {"rect_draw_tex_array", hwgfx_rect_draw_tex_array,  METH_VARARGS, NULL},

    /*text*/
    {"text_render",         hwgfx_text_render,          METH_VARARGS, NULL},

    /*label*/
    {"label_generate",      hwgfx_label_generate,       METH_VARARGS, NULL},
    {"label_drop",          hwgfx_label_drop,           METH_VARARGS, NULL},
    {"label_set_text",      hwgfx_label_set_text,       METH_VARARGS, NULL},
    {"label_render",        hwgfx_label_render,         METH_VARARGS, NULL},

    /*blend*/
    {"blend_enter" ,        hwgfx_blend_enter,          METH_VARARGS, NULL},
    {"blend_exit" ,         hwgfx_blend_exit,           METH_VARARGS, NULL},
    {"manual_blend_enter" , hwgfx_manual_blend_enter,   METH_VARARGS, NULL},
    {"manual_blend_exit" ,  hwgfx_manual_blend_exit,    METH_VARARGS, NULL},

    /*shader*/
    {"shader_load" ,        hwgfx_shader_load,          METH_VARARGS, NULL},
    {"shader_compile" ,     hwgfx_shader_compile,          METH_VARARGS, NULL},
    {"shader_drop" ,        hwgfx_shader_drop,          METH_VARARGS, NULL},
    {"shader_bind" ,        hwgfx_shader_bind,          METH_VARARGS, NULL},
    {"shader_bind_float" ,  hwgfx_shader_bind_float,    METH_VARARGS, NULL},
    {"shader_bind_floats" ,  hwgfx_shader_bind_floats,    METH_VARARGS, NULL},
    {"shader_bind_int" ,  hwgfx_shader_bind_int,    METH_VARARGS, NULL},
    {"shader_bind_vec2" ,   hwgfx_shader_bind_vec2,     METH_VARARGS, NULL},
    {"shader_bind_vec3" ,   hwgfx_shader_bind_vec3,     METH_VARARGS, NULL},
    {"shader_bind_vec4" ,   hwgfx_shader_bind_vec4,     METH_VARARGS, NULL},
    {"shader_bind_texture" ,   hwgfx_shader_bind_texture,     METH_VARARGS, NULL},

    /*texture*/
    {"texture_generate",    hwgfx_texture_generate,     METH_VARARGS, NULL},
    {"texture_generate_fp", hwgfx_texture_generate_fp,     METH_VARARGS, NULL},
    {"texture_bind",        hwgfx_texture_bind,         METH_VARARGS, NULL},
    {"texture_drop",        hwgfx_texture_drop,         METH_VARARGS, NULL},
    {"texture_upload",      hwgfx_texture_upload,       METH_VARARGS, NULL},
    {"texture_download",    hwgfx_texture_download,     METH_VARARGS, NULL},

    /*drawmode_map*/
    {"drawmode_get_map",    hwgfx_drawmode_get_map,     METH_VARARGS, NULL},

    /*primitive*/
    {"primitive_create_coordinate_primitive", hwgfx_primitive_create_coordinate_primitive, METH_VARARGS, NULL},
    {"primitive_destroy_coordinate_primitive", hwgfx_primitive_destroy_coordinate_primitive, METH_VARARGS, NULL},
    {"primitive_create_coordinate_uv_primitive", hwgfx_primitive_create_coordinate_uv_primitive, METH_VARARGS, NULL},
    {"primitive_destroy_coordinate_uv_primitive", hwgfx_primitive_destroy_coordinate_uv_primitive, METH_VARARGS, NULL},
    {"primitive_create_channel_primitive", hwgfx_primitive_create_channel_primitive, METH_VARARGS, NULL},
    {"primitive_update_channel_primitive", hwgfx_primitive_update_channel_primitive, METH_VARARGS, NULL},
    {"primitive_destroy_channel_primitive", hwgfx_primitive_destroy_channel_primitive, METH_VARARGS, NULL},
    {"primitive_render",    hwgfx_primitive_render,     METH_VARARGS, NULL},
    /*frame_buffer*/
    {"framebuffer_create",        hwgfx_framebuffer_create,         METH_VARARGS, NULL},
    {"framebuffer_drop",          hwgfx_framebuffer_drop,           METH_VARARGS, NULL},
    {"framebuffer_bind_texture",  hwgfx_framebuffer_bind_texture,   METH_VARARGS, NULL},
    {"framebuffer_render_start",  hwgfx_framebuffer_render_start,   METH_VARARGS, NULL},
    {"framebuffer_render_end",    hwgfx_framebuffer_render_end,     METH_VARARGS, NULL},

    /*pointlight*/
    {"pointlight_render_scene", hwgfx_pointlight_render_scene, METH_VARARGS, NULL},

    /*viewport*/
    {"viewport_set",        hwgfx_viewport_set,         METH_VARARGS, NULL},
    {"viewport_reset",      hwgfx_viewport_reset,       METH_VARARGS, NULL},
    /*misc*/
    {"clear",               hwgfx_clear,                METH_VARARGS, NULL},
    {"set_clear_color",     hwgfx_set_clear_color,      METH_VARARGS, NULL},
    /*debug*/
    {"debug_displaykill",   hwgfx_debug_displaykill,    METH_VARARGS, NULL},
    
    {NULL,NULL,0,NULL } /*terminator record*/
};

static PyModuleDef hwgfx_module = {
    PyModuleDef_HEAD_INIT, "hwgfx", NULL, -1, hwgfx_methods,
    NULL,NULL,NULL,NULL
};

static PyObject* PyInit_hwgfx(void) {
    return PyModule_Create(&hwgfx_module);
}
