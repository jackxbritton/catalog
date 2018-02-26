#version 330

in vec2 point;
in vec2 uv;

out vec2 uv_frag;

void main() {

    gl_Position = vec4(point.xy, 0.0, 1.0);

    uv_frag = uv;
    uv_frag.y = 1.0 - uv_frag.y;
}
