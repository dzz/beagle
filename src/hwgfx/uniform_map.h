#ifndef UNIFORM_MAP
#define UNIFORM_MAP

#include <GLXW/glxw.h>


#ifdef __cplusplus
extern "C" {
#endif
    GLint mapUniformLocation(GLuint program, const GLchar *name);
    void invalidateShaderId(GLuint program);
#ifdef __cplusplus
}
#endif


#endif
