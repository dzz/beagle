#include "uniform_map.h"

#include <tuple>
#include <unordered_map>
#include <string>
#include <iostream>

std::unordered_map<GLuint, std::unordered_map<std::string, GLint >> uniform_cache;

GLint mapUniformLocation(GLuint program, const GLchar *name) {

    std::string uniform_name(name);
    std::unordered_map<std::string, GLint> *shader_map;

    if (uniform_cache.count(program) == 1 ){
        //std::cout<<"reused map"<<"\n";
    } else {
        uniform_cache[program] = std::unordered_map<std::string, GLint>();
    }
    shader_map = &(uniform_cache[program]);

    auto it = shader_map->find(uniform_name);

    if(it == shader_map->end() ) {
        //std::cout<<"bound:"<<program<<uniform_name<<"\n";
        GLint location = glGetUniformLocation(program,name);
        shader_map->insert(std::make_pair(uniform_name, location));
        return location;
    } else {
        return it->second;
    }
}

void invalidateShaderId(GLuint program) {
    uniform_cache.erase(program);
}


