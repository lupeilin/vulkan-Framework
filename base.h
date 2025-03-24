#pragma once 

#include<iostream>
#include<memory>
#include<vector>
#include<array>
#include<string>
#include<map>
#include<optional>
#include<set>
#include<fstream>


/// Map the specified object coordinates (obj.x, obj.y, obj.z) into window coordinates using default near and far clip planes definition.
/// To change default near and far clip planes definition use GLM_FORCE_DEPTH_ZERO_TO_ONE.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vulkan/vulkan.h>

const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
};

struct VPMatrix
{
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;
};

struct ObjectUniform
{
	glm::mat4 mModelMatrix;
};
