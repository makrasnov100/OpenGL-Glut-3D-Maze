#version 330
in  vec4 vPosition;
in  vec4 Normal;

uniform mat4 ModelView;
uniform mat4 Projection;

out vec3 R;

void main()
{
    gl_Position = Projection*ModelView*vPosition;

	// Option 1 - Cube is textured
	R = Normal.xyz;

	// Option 2 - Cube is a mirror and reflects a cube environment
    // vec4 eyePos  = ModelView*vPosition;
    // vec4 NN = ModelView*Normal;
    // vec3 N = NN.xyz;
    // R = reflect(eyePos.xyz, N);

}
