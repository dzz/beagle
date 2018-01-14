#version 440 

in float height;

void main(void) {

    float vheight = (height+1.0)/2.0f;
    gl_FragColor = vec4( vheight, vheight, vheight, 1.0);
}

