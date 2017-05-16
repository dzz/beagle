//define OGL_3_3 or OGL_4_0
#define OGL_4_0


#ifdef _WIN32
#include <conio.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/wglext.h>
#endif
#ifdef __linux__
//todo: tablet / vsync
#endif
/*undef _DEBUG to get around py linking issues */
#undef _DEBUG
#include <Python.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_keycode.h>
#include "system/ctt2.h"
#include "system/ctt2_host.h"
#include "drawing/drawingSurfaces.h"
#include "system/extended_video.h"
#include "system/wm_handler.h"
#include "system/log.h"
#include "hardware/hf_timer.h"
#include "hardware/gamepad.h"
#include "hardware/audio.h"
#include "hwgfx/context.h"
#include "hwgfx/text.h"
//#include "hwgfx/primitive.h"
//#include "hwgfx/shader.h"

extern void hwgfx_render_test(); //{

    /*
    const gfx_float verts[3][2] = {
        { -1.0, -1.0 },
        { 1.0,-1.0},
        { 1.0,1.0}
    };
    const gfx_float uvs[3][2] = {
        { 0.0, 0.0 },
        { 1.0,0.0},
        { 1.0,1.0}
    };
    gfx_coordinate_uv_primitive primitive;
    gfx_shader shader;
   
    primitive_create_coordinate_uv_primitive( &primitive, (gfx_float*)verts, (gfx_float*)uvs, 3, 2);
    shader_load(&shader, "shaders/test/vert.glsl", "shaders/test/pixel.glsl");
    shader_bind(&shader);

    primitive_render_coordinate_uv_primitive( &primitive );
    primitive_destroy_coordinate_uv_primitive( &primitive );

    shader_drop(&shader);*/
//}

//not static due to reference in the host api for
//host_set_title
SDL_Window* opengl_window; 

static SDL_GLContext gl_context;

static CTT2_RT_FLAG fullscreen = 0;
static unsigned int initialized_modules;

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 700;

static FILE streams[3];
unsigned int streams_init = 0;
FILE* __iob_func() {
    if (streams_init == 0) {
        streams[0] = *stdin;
        streams[1] = *stdout;
        streams[2] = *stderr;
        streams_init = 1;
    }
    return streams;
}

char * ctt2_module_from_code( unsigned int module) {
    switch(module) {
        case CTT2_RT_MODULE_LOG:
            return  "LOG";
        case CTT2_RT_MODULE_DISPLAY:
            return  "DISPLAY";
        case CTT2_RT_MODULE_OPENGL:
            return  "OPENGL";
        case CTT2_RT_MODULE_AUDIO:
            return  "AUDIO";
        case CTT2_RT_MODULE_WINDOW_MSGS:
            return "WINMSG";
        case CTT2_RT_MODULE_TEXT_INPUT:
            return "TEXTINPUT";
        case CTT2_RT_MODULE_TIMER:
            return "HF_TIMER";
        case CTT2_RT_MODULE_GAMEPAD:
            return "GAMEPAD";
        case CTT2_RT_MODULE_PYTHON:
            return "PYTHON";
        case CTT2_RT_MODULE_CORE:
            return "RT_CORE";
        case CTT2_CLIENT_APPLICATION:
            return "CLIENT";
        case CTT2_INT_API_BRIDGE:
            return "BRIDGE";
        case CTT2_INT_HWGFX:
            return "HWGFX";
        case CTT2_INT_HWGFX_OGLOBJ:
            return "HWGFX_OBJ";
    }
    return "UNKNOWN";
}

/**************************************/

static void updateViewingSurface() {
    SDL_GL_SwapWindow( opengl_window );
    //glClear(GL_COLOR_BUFFER_BIT);
}

/**************************************/

#define VSYNC_DISABLED 0
#define VSYNC_ENABLED 1

static void requestVsyncMode(unsigned int mode)
{    
#ifdef _WIN32
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
    PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
    wglSwapIntervalEXT = 
        (PFNWGLSWAPINTERVALPROC)SDL_GL_GetProcAddress( "wglSwapIntervalEXT" );
    wglSwapIntervalEXT(mode);
#endif
#ifdef __linux__
    log_message(CTT2_RT_CORE, LOG_LEVEL_WARNING, "linux unimplemented vsync()");
#endif
}


unsigned int initWinMsgs() {
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    SDL_ShowCursor(0);
    return 1;
}

void dropWinMsgs() {
    SDL_EventState(SDL_SYSWMEVENT, SDL_DISABLE);
    SDL_ShowCursor(1);
}

/**************************************/


unsigned int initCore() {
    if( SDL_Init(0) < 0)
        return MODULE_FAILURE;
    return MODULE_LOADED;
}

void dropCore() {
    SDL_Quit();
}

unsigned int host_get_screen_width() {
    return SCREEN_WIDTH;
}

unsigned int host_get_screen_height() {
    return SCREEN_HEIGHT;
}


unsigned int initDisplay() {
    if( SDL_InitSubSystem( SDL_INIT_VIDEO ) < 0 ) {
        log_message( CTT2_RT_MODULE_DISPLAY, LOG_LEVEL_ERROR, "Could not initialize display: %s", SDL_GetError() );
        return MODULE_FAILURE;
    } 

    #ifdef OGL_3_3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    #endif

    #ifdef OGL_4_0
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    #endif

    if(fullscreen == 1 ) {
        opengl_window = SDL_CreateWindow( "ctt2_hw", 64, 64, 
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP );
    } else {
        opengl_window = SDL_CreateWindow( "ctt2_hw", 64, 64, 
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN  );
    }

    if( opengl_window == NULL ) {
        log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_ERROR, "No OpenGL Window Created");
        return MODULE_FAILURE;
    }

    log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_INFO, "Created window %-3x", opengl_window );

    return MODULE_LOADED;
}

void dropDisplay() {
    SDL_DestroyWindow( opengl_window);
}


/**************************************/

unsigned initOpenGL() {

    
    log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_INFO, "trying to acquire GL_context..." );
    
    gl_context = NULL;
    gl_context = SDL_GL_CreateContext(opengl_window);    
    if(!gl_context) {
        log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_ERROR, "Could not get valid GL context" );
        log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_ERROR, "--- error (proxied) : %s", SDL_GetError());
        return MODULE_FAILURE;
    }

    log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_INFO, "attempting to init extended video...");

    initExtendedVideo();

    requestVsyncMode( VSYNC_ENABLED );
    if(gl_context) {
        log_message( CTT2_RT_MODULE_OPENGL, LOG_LEVEL_INFO, "Initialized OpenGL Context: %x", gl_context );
        return MODULE_LOADED;
    }
    return MODULE_FAILURE;
}

void dropOpenGL() {
    SDL_GL_DeleteContext(gl_context);
}

/**************************************/

void dropPython(){
    if(PyErr_Occurred()) {
        PyErr_Print();
        log_message( CTT2_RT_MODULE_PYTHON, LOG_LEVEL_ERROR, "Python exiting under error condition");
    }
    api_drop();
    log_message( CTT2_RT_MODULE_PYTHON, LOG_LEVEL_INFO, "Dropped application");
    Py_Finalize();
    log_message( CTT2_RT_MODULE_PYTHON, LOG_LEVEL_INFO, "Dropped Python");
}

unsigned int initPython() {
    api_set_screensize( SCREEN_WIDTH, SCREEN_HEIGHT );
    //Py_SetProgramName("ctt2_py");
    if( api_init() == API_FAILURE ) {
        log_message( CTT2_RT_MODULE_PYTHON, LOG_LEVEL_ERROR, "Error booting python environment.");
        if(PyErr_Occurred()) {
            PyErr_Print();
        }
        return MODULE_FAILURE;
    } 
    return MODULE_LOADED;
}


/*****************************************************************************/

unsigned int initTextInput() {
    SDL_StartTextInput();
    return MODULE_LOADED;
}

void dropTextInput() {
    SDL_StopTextInput();
}


#define CTT2_EVT_POLL_EVENTS         0
#define CTT2_EVT_TICK                 1
#define CTT2_EVT_RENDER                 2
#define CTT2_EVT_SYNC_GFX             3

#define CTT2_MAX_RT_MODULES         64

#define CTT2_RT_ERROR               1

typedef unsigned int(*module_initializer)();
typedef void(*module_destructor)();

static unsigned int rt_module_count = 0;
static module_destructor rt_module_destructors[CTT2_MAX_RT_MODULES];
static unsigned int rt_module_ids[CTT2_MAX_RT_MODULES];

void dropRuntimeModules(unsigned int error) {
    while( rt_module_count!= 0 ) {
        rt_module_count-=1;
        {
            module_destructor destructor = rt_module_destructors[rt_module_count];
            (*destructor)();
            log_message(CTT2_RT_MODULE_CORE, LOG_LEVEL_INFO, "Released runtime module: %s", ctt2_module_from_code(rt_module_ids[rt_module_count]));
        }
    }
    exit(error);
}

void DIRTY_DISPLAY_ABORT() {
    dropRuntimeModules(MODULE_FAILURE);
}

void loadRuntimeModule( module_initializer moduleInitializer, module_destructor moduleDestructor, unsigned int RT_MODULE_ID  ) {

    if(rt_module_count==CTT2_MAX_RT_MODULES) {
        log_message(CTT2_RT_MODULE_CORE, LOG_LEVEL_ERROR, "Exceeded maximum RT module count.");
        dropRuntimeModules(CTT2_RT_ERROR);
    }

    if((*moduleInitializer)() == MODULE_FAILURE) {
        log_message(CTT2_RT_MODULE_CORE, LOG_LEVEL_ERROR, "Could not initialize module: %s", ctt2_module_from_code(RT_MODULE_ID));
        dropRuntimeModules(CTT2_RT_ERROR);
    } else {
        log_message(CTT2_RT_MODULE_CORE, LOG_LEVEL_INFO, "Loaded runtime module: %s", ctt2_module_from_code(RT_MODULE_ID));
        initialized_modules = initialized_modules | RT_MODULE_ID;
        rt_module_destructors[rt_module_count] = moduleDestructor;
        rt_module_ids[rt_module_count] = RT_MODULE_ID;
        rt_module_count++;
    }
}

CTT2_RT_SIGNAL finished                         = 0;
void host_signal_exit() {
    dropRuntimeModules(1);
}


void print_banner() {
    printf(" ____     ___   ____   ____  _        ___ \n");
    printf("|    \\   /  _] /    | /    || |      /  _]\n");
    printf("|  o  ) /  [_ |  o  ||   __|| |     /  [_ \n");
    printf("|     ||    _]|     ||  |  || |___ |    _]\n");
    printf("|  O  ||   [_ |  _  ||  |_ ||     ||   [_ \n");
    printf("|     ||     ||  |  ||     ||     ||     |\n");
    printf("|     ||     ||  |  ||     ||     ||     |\n");
    printf("|_____||_____||__|__||___,_||_____||_____|\n");
    printf("                                          \n");
    printf("          ~ * - B E A G L E - * ~\n");
    printf("\n");
    printf("an experiment in the medium of game engines\n");
    printf("\n");
    printf("                   v0.0\n");
    printf("\n");
}

void print_usage() {
    #ifdef _WIN32
    printf("    usage: bin\beagle_runtime {width} {height} {fullscreen} {fps} {path to app}\n");
    #endif

    #ifdef __linux__
    printf("    usage: ./bin/beagle_runtime {width} {height} {fullscreen} {fps} {path to app}\n");
    #endif
    printf("\n");


}
char* beagle_application_path = 0;

char* get_user_specified_application_folder() {
    return beagle_application_path;
}

int main(int argc, char **argv){ 
    
    int fps                                         = -1;
    double frame_millis                             = -1;
    double init_millis                              = 0;
    double tick_millis                              = 0;
    double frame_overflow                           = 0;
    double spf                                      = 0.0;
    int tick_next                                   = 0;
    unsigned int ctt2_state                         = CTT2_EVT_POLL_EVENTS;
    unsigned int render_test = 0;
        finished = 0;
    initialized_modules = 0;


    print_banner();

    if(argc==5 || argc==6) {
        SCREEN_WIDTH    = atoi( argv[1] );
        SCREEN_HEIGHT   = atoi( argv[2] );
        fullscreen      = atoi( argv[3] );
        fps             = atoi( argv[4] );
        spf = 1.0/(double)fps;
        frame_millis    = (double)1000/(double)fps;
        if(argc==6) {
            beagle_application_path = argv[5]; 
            printf("Application path: `%s`\n", beagle_application_path );
        }
    } else {

        print_usage();
        return;
    }


    loadRuntimeModule( &initLog,        &dropLog,           CTT2_RT_MODULE_LOG );
    loadRuntimeModule( &initCore,       &dropCore,          CTT2_RT_MODULE_CORE );
    loadRuntimeModule( &initDisplay,    &dropDisplay,       CTT2_RT_MODULE_DISPLAY );
    loadRuntimeModule( &initOpenGL,     &dropOpenGL,        CTT2_RT_MODULE_OPENGL );

    loadRuntimeModule( &initAudio,      &dropAudio,         CTT2_RT_MODULE_AUDIO );
    loadRuntimeModule( &initWinMsgs,    &dropWinMsgs,       CTT2_RT_MODULE_WINDOW_MSGS );
    loadRuntimeModule( &initTextInput,  &dropTextInput,     CTT2_RT_MODULE_TEXT_INPUT );
    loadRuntimeModule( &initTimer,      &dropTimer,         CTT2_RT_MODULE_TIMER );
    loadRuntimeModule( &initGamepad,    &dropGamepad,       CTT2_RT_MODULE_GAMEPAD);
    loadRuntimeModule( &initPython,     &dropPython,        CTT2_RT_MODULE_PYTHON);

    int run = 1;
    /** MAIN DISPATCH LOOP **/
    if(run) {
        SDL_Event event;
        double base_millis = timer_get_ms();
        tick_millis = timer_get_ms();


        while(finished != CTT2_RT_TERMINATED ) {
            switch(ctt2_state) {
                    case CTT2_EVT_TICK:
                        if(api_tick() == API_FAILURE) { 
                                finished = 1; 
                            } else {
                                tick_millis += frame_millis;
                                if( (timer_get_ms() - tick_millis) > frame_millis ) {
                                    ctt2_state = CTT2_EVT_TICK;
                                } else {
                                ctt2_state = CTT2_EVT_RENDER;
                                }
                            }
                         break;
                    case CTT2_EVT_RENDER:
                         if(render_test == 0) {
                            api_render();
                         } 
                         if(render_test==1) {
                             hwgfx_render_test();
                         }
                         if(render_test==2) {
                            api_render_test();
                         }
                         ctt2_state = CTT2_EVT_SYNC_GFX;
                         break;
                    case CTT2_EVT_SYNC_GFX:
                        updateViewingSurface();  
                        ctt2_state = CTT2_EVT_POLL_EVENTS;
                        break;
                    case CTT2_EVT_POLL_EVENTS:
                        if( (timer_get_ms() - tick_millis) > frame_millis ) {
                            ctt2_state = CTT2_EVT_TICK;
                         } 
                          break;
            }


            while(SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_CONTROLLERDEVICEADDED:
                        dropGamepad();
                        initGamepad();
                        break;
                    case SDL_CONTROLLERDEVICEREMOVED:
                        dropGamepad();
                        initGamepad();
                        break;
                    case SDL_JOYAXISMOTION:
                        GamepadHandleEvent( &event );
                        break; 
                    case SDL_TEXTINPUT:
                        api_dispatch_text( event.text.text );
                        break;
                    case SDL_QUIT:
                        finished = CTT2_RT_TERMINATED;
                        break;
                    case SDL_SYSWMEVENT:
                        #ifdef _WIN32
                            #ifdef WACOM_ENABLED
                                handle_wm_event(event); //used for the wacom tablet module, but currently missing...
                            #endif
                        #endif
                        break;
                    case SDL_KEYDOWN:
                        if( api_dispatch_key(event.key.keysym.sym,1) == API_FAILURE ) finished = CTT2_RT_TERMINATED;
                        if( event.key.keysym.sym == SDLK_F5 && (event.key.keysym.mod & KMOD_CTRL) ) {
                            dropPython();
                            initPython();
                        }
                        if( event.key.keysym.sym == SDLK_F6 && (event.key.keysym.mod & KMOD_CTRL) ) {
                            render_test = (render_test+1)%3;
                        }
                        if( event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod & KMOD_ALT) ) {
                            finished = CTT2_RT_TERMINATED;
                        }
                        break;
                    case SDL_KEYUP:
                        if( api_dispatch_key(event.key.keysym.sym,0) 
                                == API_FAILURE ) finished = CTT2_RT_TERMINATED;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if(api_dispatch_mousedown(
                                    event.button.button, 
                                    event.button.x, 
                                    event.button.y) == API_FAILURE ) 
                                        finished = CTT2_RT_TERMINATED ;
                        break;
                    case SDL_MOUSEBUTTONUP:
                        if(api_dispatch_mouseup(
                                    event.button.button, 
                                    event.button.x, 
                                    event.button.y) == API_FAILURE ) 
                                        finished = CTT2_RT_TERMINATED;
                        break;
                    case SDL_MOUSEMOTION:
                        if(api_dispatch_mousemotion(
                                    event.motion.x, 
                                    event.motion.y) == API_FAILURE ) 
                                        finished = CTT2_RT_TERMINATED;
                        break;
                }
            }
        }
    }
    
    sequencer_halt();
    dropRuntimeModules(0);
    return 0;
}
