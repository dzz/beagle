#version 330 core

uniform sampler2D texBuffer;
uniform vec2 view;
uniform vec2 translation_world;
uniform vec2 scale_world;

in vec2 uv;

void main() {
    vec4 encoded_line = texture(texBuffer,uv);
    vec2 line_a = (vec2( encoded_line.r, encoded_line.g ) + translation_world) * scale_world * view;
    vec2 line_b = (vec2( encoded_line.b, encoded_line.a ) + translation_world) * scale_world * view;
    vec4 transformed_encoded_line = vec4(
        line_a.x,line_a.y,
        line_b.x,line_b.y
    );
    gl_FragColor = transformed_encoded_line;
}
