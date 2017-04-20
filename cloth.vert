#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 Position;
out vec3 Normal;
out vec2 texUV;

//uniform float time;

uniform mat4 MV;
uniform mat4 MVP;

void main(){

	Position = vec3((MV * vec4(position, 1.0)).xyz);
	Normal = normal;
	texUV = uv;
        gl_Position = MVP * vec4(position, 1.0);
}
