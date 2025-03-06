#pragma once 

#include<iostream>
#include<memory>
#include<vector>
#include<string>
#include<map>
#include<optional>
#include<set>
#include<fstream>

#include<glm/glm.hpp>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vulkan/vulkan.h>

const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
};