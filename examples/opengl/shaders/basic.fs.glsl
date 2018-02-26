#version 330

uniform sampler2D texture;

in vec2 uv_frag;

void main() {
    gl_FragColor = texture2D(texture, uv_frag);
    gl_FragColor = vec4(1, 0.5, 0, 1-texture2D(texture, uv_frag).r);
}
