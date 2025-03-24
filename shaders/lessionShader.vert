#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(binding = 0) uniform VPMatrix{
	mat4 mViewMatrix;
	mat4 mProjectMatrix;
}vpUBO;

layout(binding = 1) uniform ObjectUniform{
	mat4 mModelMatrix;
}objectUBO;

//vec2 position[3] = vec2[](vec2(0.0, -1.0), vec2(0.5, 0.0), vec2(-0.5, 0.0));

//vec3 color[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

void main(){
	//gl_Position = vec4(position[gl_VertexIndex], 0.0, 1.0);
	//vulkan最后算出来是0到1
	gl_Position = vpUBO.mProjectMatrix * vpUBO.mViewMatrix * objectUBO.mModelMatrix * vec4(inPosition, 1.0);
	outColor = inColor;
}