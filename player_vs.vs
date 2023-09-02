#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
out vec3 position;
uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;
void main(){
gl_Position = projection * transform * view * vec4(pos, 1.0);
position = gl_Position.xyz;
}