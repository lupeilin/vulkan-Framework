#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 outColor;

vec2 position[3] = vec2[](vec2(0.0, -1.0), vec2(0.5, 0.0), vec2(-0.5, 0.0));

vec3 color[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

void main(){
	gl_Position = vec4(position[gl_VertexIndex], 0.0, 1.0);
	outColor = color[gl_VertexIndex];
}