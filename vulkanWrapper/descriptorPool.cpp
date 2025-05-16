#include "descriptorPool.h"

namespace FF::Wrapper {
	DescriptorPool::DescriptorPool(const Device::Ptr& device) {
		mDevice = device;
	}

	DescriptorPool::~DescriptorPool() {
		if (mPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(mDevice->getDevice(), mPool, nullptr);
		}
	}

	void DescriptorPool::build(std::vector<UniformParameter::Ptr>& params, const int& frameCount) {
		//descriptor
		//descriptorSet(descriptorA（buffer）, descriptorA（buffer）, descriptorB（buffer）)
		//descriptorSet * N 因为描述符集合当中，绑定了buffer,当前一帧提交的时候，其他的帧正在绘制当中，
			//即，uniformBuffer, 可能正在被修改，此时cpu端的下一个循环，却对其进行了数据的修改  
			//所以需要为每一帧创建一个descriptorSet(descriptorA（buffer）, descriptorA（buffer）, descriptorB（buffer)

		//统计信息，统计每一种uniform有多少个
		int uniformBufferCount = 0; 
		//TODO:纹理这种种类的uniform有多少个？
		int textureCount = 0;

		for (const auto& param : params) {
			if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				uniformBufferCount++;
			} 
			if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
				textureCount++;
			}
		}

		//描述每一种uniform都有多少个
		std::vector<VkDescriptorPoolSize> poolSize{};

		VkDescriptorPoolSize uniformBufferSize{};
		uniformBufferSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferSize.descriptorCount = uniformBufferCount * frameCount;
		poolSize.push_back(uniformBufferSize);

		VkDescriptorPoolSize textureSize{};
		textureSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureSize.descriptorCount = textureCount * frameCount;    //这边的size值得是有多少个描述符，
																	//虽然texture只有一个，但是描述符要每一帧都有一个descriptorSet
		poolSize.push_back(textureSize);

		//创建pool
		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
		createInfo.pPoolSizes = poolSize.data();
		createInfo.maxSets = static_cast<uint32_t>(frameCount);

		if (vkCreateDescriptorPool(mDevice->getDevice(), &createInfo, nullptr, &mPool) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create descriptor pool");
		}
	}
}