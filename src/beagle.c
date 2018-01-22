#include "memory.h"

#define BEAGLE_GL_MAJOR 4
#define BEAGLE_GL_MINOR 0

//WINDOWS 7
#define WINVER 0x0601

#define BEAGLE_USE_SDL_AUDIO



//#define NO_PYTHON   // Turn on to disable the python interpreter entirely (no game for you)
//#define CPP_API  // Turn on to replace python api with a cpp version

#define RENDER_TEST_TEXT // Turn on to test text rendering
#define RENDER_TEST_SHADED_PRIMITIVES


#ifdef _WIN32
#include <conio.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <GLXW/glxw.h>
#include <WinSock2.h>
#include <windows.h>
#endif
#ifdef __linux__
//todo: tablet / vsync
#endif
/*undef _DEBUG to get around py linking issues */
//#undef _DEBUG
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
#include "hwgfx/pointlight.h"
//#include "hwgfx/primitive.h"
//#include "hwgfx/shader.h"
#include <chipmunk/chipmunk.h>
#include "hwgfx/command_message.h"

#include "basicAudio.h"

#include "args.h"

#ifdef CPP_API
#include "cpp_module/cpp_host.h"
#endif

void test_cp_integration() {

    // cpVect is a 2D vector and cpv() is a shortcut for initializing them.
    cpVect gravity = cpv(0, -100);

    // Create an empty space.
    cpSpace *space = cpSpaceNew();
    cpSpaceSetGravity(space, gravity);

    // Add a static line segment shape for the ground.
    // We'll make it slightly tilted so the ball will roll off.
    // We attach it to a static body to tell Chipmunk it shouldn't be movable.
    cpShape *ground = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(-20, 5), cpv(20, -5), 0);
    cpShapeSetFriction(ground, 1);
    cpSpaceAddShape(space, ground);

    // Now let's make a ball that falls onto the line and rolls off.
    // First we need to make a cpBody to hold the physical properties of the object.
    // These include the mass, position, velocity, angle, etc. of the object.
    // Then we attach collision shapes to the cpBody to give it a size and shape.

    cpFloat radius = 5;
    cpFloat mass = 1;

    // The moment of inertia is like mass for rotation
    // Use the cpMomentFor*() functions to help you approximate it.
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // The cpSpaceAdd*() functions return the thing that you are adding.
    // It's convenient to create and add an object in one line.
    cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(ballBody, cpv(0, 15));

    // Now we create the collision shape for the ball.
    // You can create multiple collision shapes that point to the same body.
    // They will all be attached to the body and move around to follow it.
    cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
    cpShapeSetFriction(ballShape, 0.7);

    // Now that it's all set up, we simulate all the objects in the space by
    // stepping forward through time in small increments called steps.
    // It is *highly* recommended to use a fixed size time step.
    cpFloat timeStep = 1.0/60.0;
    for(cpFloat time = 0; time < 2; time += timeStep){
        cpVect pos = cpBodyGetPosition(ballBody);
        cpVect vel = cpBodyGetVelocity(ballBody);
        printf(
                "Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
                time, pos.x, pos.y, vel.x, vel.y
              );

        cpSpaceStep(space, timeStep);
    }

    // Clean up our objects and exit!
    cpShapeFree(ballShape);
    cpBodyFree(ballBody);
    cpShapeFree(ground);
    cpSpaceFree(space);


}

extern void hwgfx_render_test();
//not static due to reference in the host api for
//host_set_title
SDL_Window* opengl_window; 

static SDL_GLContext gl_context;

static CTT2_RT_FLAG fullscreen = 0;
static unsigned int initialized_modules;
static unsigned int use_vsync = 1;
static unsigned int use_timing = 1;

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

void updateViewingSurface() {
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
    SDL_ShowCursor(1);//DONT COMMIT THIS DISABLE IT AGAIN
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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, BEAGLE_GL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, BEAGLE_GL_MINOR);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);


    if(fullscreen>100) {
        use_timing = 0;
        fullscreen -= 100;
    }
    if(fullscreen>10) {
        use_vsync = 0;
        fullscreen -= 10;
    }

    if(fullscreen == 1 ) {
        opengl_window = SDL_CreateWindow( "Beagle Engine", 64, 64, 
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN );
    } 
    else if (fullscreen == 2) {
        opengl_window = SDL_CreateWindow( "Beagle Engine", 64, 64, 
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED );
    }
    else {
        opengl_window = SDL_CreateWindow( "Beagle Engine", 64, 64, 
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

    initGLExtensions();

    requestVsyncMode( use_vsync );
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


#define CTT2_EVT_POLL_EVENTS 0
#define CTT2_EVT_TICK        1
#define CTT2_EVT_RENDER      2
#define CTT2_EVT_SYNC_GFX    3

#define CTT2_MAX_RT_MODULES  64

#define CTT2_RT_ERROR        1

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
    printf("    usage: bin\\beagle_runtime {width} {height} {fullscreen} {fps} {path to app}\n");
#endif

#ifdef __linux__
    printf("    usage: ./bin/beagle_runtime {width} {height} {fullscreen} {fps} {path to app}\n");
#endif
    printf("\n");


}
char* beagle_application_path = 0;
const char *beagle_default_config_string = "";
char* beagle_config_string = 0;

char* get_user_specified_application_folder() {
    return beagle_application_path;
}

char* get_user_specified_config_string() {

    if(beagle_config_string>0)
        return beagle_config_string;
    return beagle_default_config_string;
}

static double vfps;
double get_vfps() {
    return vfps;
}


///......seriously, SDL? 
#undef main
// ....why....



volatile static unsigned int GXC_READY = 0;

void GXC_Thread() {

    initOpenGL();
    initGLExtensions();
    //initExtendedVideo();
    GXC_READY = 1;
    GXC_main();
    dropOpenGL();

}

void test_pl_project() {

    float pX, pY;

    pl_project(10, -2,-2, &pX, &pY);
    printf("%f %f\n", pX, pY);
    pl_project(10, 4,-4, &pX, &pY);
    printf("%f %f\n", pX, pY);
    pl_project(10, 7,7, &pX, &pY);
    printf("%f %f\n", pX, pY);
    pl_project(10, 0,7, &pX, &pY);
    printf("%f %f\n", pX, pY);
    return;

}

#include "hwgfx/blend_control.h"
int cmain(int argc, char **argv){ 

    int fps                                         = 60;
    double frame_millis                             = -1;
    double init_millis                              = 0;
    double tick_millis                              = 0;
    double frame_overflow                           = 0;
    double spf                                      = 0.0;
    int tick_next                                   = 0;
    unsigned int ctt2_state                         = CTT2_EVT_POLL_EVENTS;
    unsigned int render_test = 0;
    unsigned int frames = 0;
    finished = 0;
    initialized_modules = 0;
    unsigned int systems_test = 0;

    SDL_Thread *gxc_thread;



    //test_pl_project();
    //return;

    //test_cp_integration();
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    print_banner();

    vfps = (double)fps;
    spf = 1.0/(double)fps;
    frame_millis = (double)1000/(double)fps;

    char * flag;
    char * arg;
    while (next_arg(argc, argv, &flag, &arg)) {
        if (!strcmp("--width", flag)) {
            SCREEN_WIDTH = atoi( arg );    
        } else if (!strcmp("--height", flag)) {
            SCREEN_HEIGHT = atoi( arg );
        } else if (!strcmp("--fullscreen", flag)) {
            fullscreen = atoi( arg );
        } else if (!strcmp("--fps", flag)) {
            fps = atoi( arg );
            vfps = (double)fps;
            spf = 1.0/(double)fps;
            frame_millis = (double)1000/(double)fps;
        } else if (!strcmp("--app-path", flag)) {
            if (!strcmp("system-test", arg)) {
                systems_test = 1;
            } else {
                beagle_application_path = arg; 
                printf("Application path: `%s`\n", beagle_application_path );
            }
        } else if (!strcmp("--config-string", flag)) {
            beagle_config_string = arg;
        } else if (!strcmp("--address", flag)) {
        } else if (!strcmp("-help", flag)) {
            print_usage();
            return 0;
        }
    }


    loadRuntimeModule( &initLog,        &dropLog,           CTT2_RT_MODULE_LOG );
    loadRuntimeModule( &initCore,       &dropCore,          CTT2_RT_MODULE_CORE );
    loadRuntimeModule( &initDisplay,    &dropDisplay,       CTT2_RT_MODULE_DISPLAY );
    //loadRuntimeModule( &initOpenGL,     &dropOpenGL,        CTT2_RT_MODULE_OPENGL );

    gxc_thread = SDL_CreateThread((SDL_ThreadFunction)GXC_Thread,"GXC",NULL);
    while(!GXC_READY) {
        //spppiiiinnn
    }

    {
        viewport_dims dims;
        dims.x = 0;
        dims.y = 0;
        dims.w = SCREEN_WIDTH;
        dims.h = SCREEN_HEIGHT;
        gfx_viewport_set_dims(dims);
    }
    initExtendedVideo();




    loadRuntimeModule( &initAudio,      &dropAudio,         CTT2_RT_MODULE_AUDIO );
#ifdef BEAGLE_USE_SDL_AUDIO
    BGLBasicMixer_Init();
#endif

    loadRuntimeModule( &initWinMsgs,    &dropWinMsgs,       CTT2_RT_MODULE_WINDOW_MSGS );
    loadRuntimeModule( &initTextInput,  &dropTextInput,     CTT2_RT_MODULE_TEXT_INPUT );
    loadRuntimeModule( &initTimer,      &dropTimer,         CTT2_RT_MODULE_TIMER );
    loadRuntimeModule( &initGamepad,    &dropGamepad,       CTT2_RT_MODULE_GAMEPAD);

#ifdef CPP_API
    cpp_api_init();
#else
#ifndef NO_PYTHON
    initPython();
#endif
#endif



    int run = 1;
    /** MAIN DISPATCH LOOP **/
    if(run) {
        unsigned int sync_ctr = 0;
        unsigned int sync_freq = 60;
        SDL_Event event;
        double base_millis = timer_get_ms();
        tick_millis = timer_get_ms();


        unsigned int frames_ticked = 0;
        unsigned int frames_tick_max = 8;

        GXC_WAIT_FLUSH(); //give the renderer time to catch up with anything triggered by initialization
        while(finished != CTT2_RT_TERMINATED ) {
            // api_immediate_cycle();
            switch(ctt2_state) {
                case CTT2_EVT_TICK:

                    if(sync_ctr == sync_freq) {
                        GXC_WAIT_FLUSH();
                        sync_ctr = 0;
                    }

                    #ifdef CPP_API
                    if(cpp_api_tick() == API_FAILURE) { 
                        finished = 1; 
                    } else 
                    #else
                    #ifndef NO_PYTHON
                    if(api_tick() == API_FAILURE) { 
                        finished = 1; 
                    } else 
                    #endif
                    #endif
                    {
                        sync_ctr+=1;
                        frames_ticked = frames_ticked + 1;
                        tick_millis += frame_millis;


                        if(use_timing == 0) {
                            ctt2_state = CTT2_EVT_RENDER;
                            break;
                        }
                        if( (timer_get_ms() - tick_millis) > frame_millis ) {
                            ctt2_state = CTT2_EVT_TICK;
                        } else {
                            frames_ticked = 0;
                            ctt2_state = CTT2_EVT_RENDER;
                        }
                        if(frames_ticked>frames_tick_max) {
                            frames_ticked = 0;
                            ctt2_state = CTT2_EVT_RENDER;
                        }
                    }
                    break;
                case CTT2_EVT_RENDER:

                    if(systems_test) {
                        hwgfx_render_test();
                        manual_blend_enter(0);
                        text_render(0,0,0.0,1.0,0.0, "01234567890abcdefghijklmnopqrstuv");
                        text_render(8,8,1.0,0.0,0.0, "01234567890abcdefghijklmnopqrstuv");
                        text_render(16,16,0.0,0.0,1.0, "01234567890abcdefghijklmnopqrstuv");
                        manual_blend_exit();
                    }

                    #ifdef CPP_API
                    cpp_api_render();
                    #else
                    #ifndef NO_PYTHON
                    api_render();
                    //GXC_WAIT_FLUSH(); //give the renderer time to catch up with anything triggered by initialization
                    #endif
                    #endif
                    ctt2_state = CTT2_EVT_SYNC_GFX;

                    break;
                case CTT2_EVT_SYNC_GFX:
                    //updateViewingSurface();  
                    {
                        gc_msg m;
                        m.cmd = GXC_COMMIT_FRAME;
                        GXC_ISSUE(m);
                    }
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
                        #ifdef CPP_API
                        cpp_api_dispatch_text( event.text.text );
                        #else
                        #ifndef NO_PYTHON
                        api_dispatch_text( event.text.text );
                        #endif
                        #endif
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
                        #ifdef CPP_API
                        if( cpp_api_dispatch_key(event.key.keysym.sym,1) == API_FAILURE ) finished = CTT2_RT_TERMINATED;
                        #else
                        #ifndef NO_PYTHON
                        if( api_dispatch_key(event.key.keysym.sym,1) == API_FAILURE ) finished = CTT2_RT_TERMINATED;
                        if( event.key.keysym.sym == SDLK_F5 && (event.key.keysym.mod & KMOD_CTRL) ) {
                            GXC_WAIT_FLUSH();
                            dropPython();
                            initPython();
                        }
                        #endif
                        #endif
                        if( event.key.keysym.sym == SDLK_F6 && (event.key.keysym.mod & KMOD_CTRL) ) {
                            render_test = (render_test+1)%3;
                        }

                        if( event.key.keysym.sym == SDLK_F4 && (event.key.keysym.mod & KMOD_ALT) ) {
                            printf("Received Exit Signal\n");
                            finished = CTT2_RT_TERMINATED;
                        }
                        break;
                    case SDL_KEYUP:
                        #ifdef CPP_API
                        if( cpp_api_dispatch_key(event.key.keysym.sym,0) 
                                == API_FAILURE ) finished = CTT2_RT_TERMINATED;
                        #else
                        #ifndef NO_PYTHON
                        if( api_dispatch_key(event.key.keysym.sym,0) 
                                == API_FAILURE ) finished = CTT2_RT_TERMINATED;
                        #endif
                        #endif
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        #ifdef CPP_API
                        if(cpp_api_dispatch_mousedown(
                                    event.button.button, 
                                    event.button.x, 
                                    event.button.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED ;
                        #else
                        #ifndef NO_PYTHON
                        if(api_dispatch_mousedown(
                                    event.button.button, 
                                    event.button.x, 
                                    event.button.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED ;
                        #endif
                        #endif
                        break;
                    case SDL_MOUSEBUTTONUP:
                        #ifdef CPP_API
                        if(cpp_api_dispatch_mouseup(
                                    event.button.button, 
                                    event.button.x, 
                                    event.button.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED;
                        #else
                        #ifndef NO_PYTHON
                        if(api_dispatch_mouseup(
                                    event.button.button, 
                                    event.button.x, 
                                    event.button.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED;
                        #endif
                        #endif
                        break;
                    case SDL_MOUSEMOTION:
                        #ifdef CPP_API
                        if(cpp_api_dispatch_mousemotion(
                                    event.motion.x, 
                                    event.motion.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED;
                        #else
                        #ifndef NO_PYTHON
                        if(api_dispatch_mousemotion(
                                    event.motion.x, 
                                    event.motion.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED;
                        #endif
                        #endif
                        break;
                    case SDL_MOUSEWHEEL:
                        #ifdef CPP_API
                        if(cpp_api_dispatch_mousewheel(
                                    event.wheel.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED;
                        #else
                        #ifndef NO_PYTHON
                        if(api_dispatch_mousewheel(
                                    event.wheel.y) == API_FAILURE ) 
                            finished = CTT2_RT_TERMINATED;
                        #endif
                        #endif
                        break;

                }
            }
        }
    }

#ifdef CPP_API
    cpp_api_drop();
#else
#ifndef NO_PYTHON
    dropPython();
#endif
#endif

    dropExtendedVideo();

    {
        gc_msg m;
        m.cmd = GXC_HALT;
        GXC_ISSUE(m);
        int gxc_result;
        SDL_WaitThread(gxc_thread, &gxc_result );
    }
    sequencer_halt();

#ifdef BEAGLE_USE_SDL_AUDIO
    BGLBasicMixer_Shutdown();
#endif

    dropRuntimeModules(0);
    return 0;
}
