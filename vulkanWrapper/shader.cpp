#include "shader.h"

namespace FF::Wrapper {

	std::vector<char> readBinary(const std::string& fileName) {
		std::ifstream file(fileName.c_str(), std::ios::ate | std::ios::binary | std::ios::in);

		if (!file) {
			throw std::runtime_error("Error : failed to open shader file");
		}

		const size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		return buffer;
	}

	Shader::Shader(Device::Ptr device, const std::string& fileName, VkShaderStageFlagBits shaderStage, const std::string& entryPoint) {
		mDevie = device;
		mShaderStage = shaderStage;
		mEbtryPoint = entryPoint;
		std::vector<char> codeBuffer = readBinary(fileName);
		VkShaderModuleCreateInfo shaderCreateInfo{};
		shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCreateInfo.codeSize = codeBuffer.size();
		shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(codeBuffer.data()); //重新解释这段内存空间，因为返回类型和接受类型不同

		if (vkCreateShaderModule(mDevie->getDevice(), &shaderCreateInfo, nullptr, &mShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create shader");
		}
	}
	Shader::~Shader() {
		if (mShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(mDevie->getDevice(), mShaderModule, nullptr);
		}
	}


}