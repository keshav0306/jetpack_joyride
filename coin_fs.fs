#version 330 core
in vec3 position;
out vec4 FragColor;
void main(){
vec3 see = normalize(-position + vec3(0.0, 0.0, 1.0));
vec3 normal = vec3(0.0, 0.0, 1.0);
vec3 reflect = reflect(-see, normal);
float spec = pow(max(dot(reflect, vec3(0.0, 0.0, 1.0)), 0.0f), 32);
vec3 Color = vec3(1.0) * vec3(1.0, 0.843, 0.0);
FragColor = vec4(Color, 1.0) * (see.z + spec);
}