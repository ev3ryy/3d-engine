#version 450

layout(location = 0) out vec2 outUV;

void main() {
    if (gl_VertexIndex == 0) {
        outUV = vec2(0.0, 0.0);
        gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
    } else if (gl_VertexIndex == 1) {
        outUV = vec2(2.0, 0.0);
        gl_Position = vec4(3.0, -1.0, 0.0, 1.0);
    } else { // gl_VertexIndex == 2
        outUV = vec2(0.0, 2.0);
        gl_Position = vec4(-1.0, 3.0, 0.0, 1.0);
    }
}