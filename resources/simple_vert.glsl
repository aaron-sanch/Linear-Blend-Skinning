#version 120

uniform mat4 P;
uniform mat4 MV;
varying vec3 vColor;

void main()
{
	gl_Position = P * (MV * gl_Vertex);
	vColor = gl_Color.rgb;
}
