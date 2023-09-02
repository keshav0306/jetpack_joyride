#version 330 core
uniform vec3 color;
out vec4 FragColor;
uniform float ycoord;
uniform float space;
in vec3 position;
void main(){
vec3 pos_vec = position - vec3(-0.9, ycoord, 0.0);
float doty = dot(pos_vec, pos_vec);
FragColor = vec4(space + (1 - space) * 0.6, space + (1 - space) * 0.5, (1 - space) * 0.04, 1.0) * (space * exp(-doty * 40) + (1 - space));
}