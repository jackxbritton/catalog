#version 330

uniform sampler2D texture;

in vec2 uv_frag;

void main() {
    gl_FragColor = texture2D(texture, uv_frag);
}
