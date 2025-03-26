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
		//descriptorSet(descriptorA��buffer��, descriptorA��buffer��, descriptorB��buffer��)
		//descriptorSet * N ��Ϊ���������ϵ��У�����buffer,��ǰһ֡�ύ��ʱ��������֡���ڻ��Ƶ��У�
			//����uniformBuffer, �������ڱ��޸ģ���ʱcpu�˵���һ��ѭ����ȴ������������ݵ��޸�  
			//������ҪΪÿһ֡����һ��descriptorSet(descriptorA��buffer��, descriptorA��buffer��, descriptorB��buffer)

		//ͳ����Ϣ��ͳ��ÿһ��uniform�ж��ٸ�
		int uniformBufferCount = 0; 
		//TODO:�������������uniform�ж��ٸ���

		for (const auto& param : params) {
			if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
				uniformBufferCount++;
			} 
			//TODO
		}

		//����ÿһ��uniform���ж��ٸ�
		std::vector<VkDescriptorPoolSize> poolSize{};

		VkDescriptorPoolSize uniformBufferSize{};
		uniformBufferSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferSize.descriptorCount = uniformBufferCount * frameCount;
		poolSize.push_back(uniformBufferSize);

		//����pool
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