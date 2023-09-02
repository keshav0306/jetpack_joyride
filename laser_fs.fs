#version 330 core
in vec3 position;
out vec4 FragColor;
uniform float coeff[3];
void main(){
float dist = abs(coeff[0] * position.y + coeff[1] * position.x + coeff[2]);
vec4 fragColor = vec4(0.0, 1.0, 0.0, 1.0) * exp(-dist * 50);
FragColor = vec4(fragColor.xyz, 1.0);
}