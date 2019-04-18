#version 330

in vec3 R;
uniform samplerCube texMap;
out vec4 FragColor;

void main()
{
    FragColor =  texture(texMap, R);
}
