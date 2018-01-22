#include "CPPGFXTest.h"
#include <malloc.h>

#include <utility>


#define MAX_TREES 3000

CPPGFXTest::CPPGFXTest():
    time ( 0.0f ),
    terrain( 1024 ),
    tree_locations( terrain.getTreeLocations( MAX_TREES ) ),
    tree_texture("terrain/tree_icon.png", true),
    cursor_texture("terrain/cursor.png", true),
    camera(),
    sprite_renderer()
{
        bgl::keyboard::register_keydown_handler( bgl::keys::kc_a, [&](){
            this->camera.move(std::make_pair(-2.0,0.0));
        } );
        bgl::keyboard::register_keydown_handler( bgl::keys::kc_d, [&](){
            this->camera.move(std::make_pair(2.0,0.0));
        } );
        bgl::keyboard::register_keydown_handler( bgl::keys::kc_w, [&](){
            this->camera.move(std::make_pair(0.0,-2.0));
        } );
        bgl::keyboard::register_keydown_handler( bgl::keys::kc_s, [&](){
            this->camera.move(std::make_pair(0.0,2.0));
        } );
        bgl::keyboard::register_keydown_handler( bgl::keys::kc_minus, [&](){
            auto camdata = this->camera.get_state();
            camdata.zoom *= 0.8;
            this->camera.update(camdata);
        } );
        bgl::keyboard::register_keydown_handler( bgl::keys::kc_equals, [&](){
            auto camdata = this->camera.get_state();
            camdata.zoom *= 1.2;
            this->camera.update(camdata);
        } );
        bgl::mouse::register_click_handler( bgl::mouse::buttons::right, [&]() {
            auto camdata = this->camera.get_state();
            auto mousedata = bgl::mouse::get_worldspace(this->camera);
            camdata.p.first = mousedata.first;
            camdata.p.second = mousedata.second;
            this->camera.update(camdata);
        });
}

CPPGFXTest::~CPPGFXTest() { }

void CPPGFXTest::tick() {
    time += 0.006f;    
}

void CPPGFXTest::render() {

    terrain.render(camera);

    float rotation = 0.0f;

    unsigned int t = 0;


    for( auto tree_loc: tree_locations ) {
            sprite_renderer.add_sprite({0.0, { 0.0,0.0}, { 4.0, 4.0 }, rotation , tree_loc, { 1.0,1.0 }, { 1.0,1.0,1.0,1.0}, {0.0,0.0,0.0,0.0}, &tree_texture });
    }

    sprite_renderer.add_sprite({1.0, { 1.0,1.0}, { 4.0, 4.0 }, rotation , bgl::mouse::get_worldspace(camera), { 1.0,1.0 }, { 1.0,1.0,1.0,1.0}, {0.0,0.0,0.0,0.0}, &cursor_texture });

    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
            sprite_renderer.render(camera); 
    });
}
