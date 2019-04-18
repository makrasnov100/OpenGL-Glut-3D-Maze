#version 150 

in  vec4 color;
in vec2 vSR;

uniform sampler2D texMap;

out vec4 fColor;

void main() 
{ 
    vec4 tMap = texture(texMap, vSR);
    fColor = color + tMap;
} 
