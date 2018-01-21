#include "cpp_keyboard.h"
#include <unordered_map>
//#include <stack>

namespace bgl { namespace keyboard {

static std::unordered_map<unsigned int, std::function<void()>> key_down_handlers;
static std::unordered_map<unsigned int, std::function<void()>> key_up_handlers;


void register_keydown_handler( unsigned int key, std::function<void()> handler ) {
    key_down_handlers[key] = handler;
}

void register_keyup_handler( unsigned int key, std::function<void()> handler ) {
    key_up_handlers[key] = handler;
}

void _dispatch_keydown( unsigned int key ) {
    auto handler = key_down_handlers.find(key);
    if(handler == key_down_handlers.end())
        return;
    handler->second();
}

void _dispatch_keyup( unsigned int key) {
    auto handler = key_up_handlers.find(key);
    if(handler == key_up_handlers.end())
        return;
    handler->second();
}

void register_keyhandler_pair( unsigned int key, std::function<void()> down_handler, std::function<void()> up_handler ) {
    bgl::keyboard::register_keydown_handler( key, down_handler);
    bgl::keyboard::register_keyup_handler( key, up_handler);
}

}}
