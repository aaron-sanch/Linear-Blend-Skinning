#version 120

attribute vec4 aPos;
attribute vec3 aNor;
attribute vec2 aTex;

uniform mat4 P;
uniform mat4 MV;
uniform mat3 T;

varying vec3 vPos;
varying vec3 vNor;
varying vec2 vTex;

void main()
{
	vec4 posCam = MV * aPos;
	vec3 norCam = (MV * vec4(aNor, 0.0)).xyz;
	gl_Position = P * posCam;
	vPos = posCam.xyz;
	vNor = norCam;
	vTex = vec2(T * vec3(aTex, 1.0));
}
