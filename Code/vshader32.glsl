#version 150

uniform float angle;
in vec4 vPosition;
in vec4 vColor;
in vec2 vMapCoord;
uniform mat4 ModelView;
uniform mat4 Projection;

out vec4 color;
out vec2 vSR;

void main() 
{
  float ca = cos(angle);
  float sa = sin(angle);

  mat4 rot = mat4( ca, 0, -sa, 0.0,
                   0.0, 1.0, 0.0, 0.0,
                   sa, 0, ca, 0,
				   0.0, 0.0, 0.0, 1.0 );
  gl_Position = Projection*ModelView*rot*vPosition;
  color = angle * vColor ;
  vSR = vMapCoord;
} 
