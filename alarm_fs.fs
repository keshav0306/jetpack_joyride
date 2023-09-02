#version 330 core
uniform vec3 color;
out vec4 FragColor;
uniform float ycoord;
uniform float space;
in vec3 position;
void main(){
FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}