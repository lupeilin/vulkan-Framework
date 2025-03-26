#include "descriptorSet.h"

namespace FF::Wrapper {

	DescriptorSet::DescriptorSet(
		const Device::Ptr& device,
		const std::vector<UniformParameter::Ptr> params,
		const DescriptorSetLayout::Ptr& layout,
		const DescriptorPool::Ptr& pool,
		int frameCount
	) {
		mDevice = device;

		std::vector<VkDescriptorSetLayout> layouts(frameCount, layout->getLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool->getDescriptorPool();
		allocInfo.descriptorSetCount = frameCount;
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(frameCount);
		if (vkAllocateDescriptorSets(mDevice->getDevice(), &allocInfo, &mDescriptorSets[0]) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to allocate descriptor sets");
		}

		for (size_t i = 0; i < frameCount; i++)
		{
			//�������ĸ��£���д��VkWriteDescriptorSet֮��ȥ���£��Ϳ��԰�һ��mDescriptorSets���³���
			//��ÿһ��DescriptorSet ��������Ҫ��params�����������Ϣ��д������
			std::vector<VkWriteDescriptorSet> descriptorSetWrites{};
			for (const auto& param : params) {
				VkWriteDescriptorSet descriptorSetWrite{};
				descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorSetWrite.dstSet = mDescriptorSets[i]; //����׼��д��i��DescriptorSet
				descriptorSetWrite.dstArrayElement = 0;
				descriptorSetWrite.descriptorType = param->mDescriptorType;
				descriptorSetWrite.descriptorCount = param->mCount;
				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					descriptorSetWrite.pBufferInfo = &param->mBuffers[i]->getBufferInfo();
				}
				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
					//TODE:�������
				}
				descriptorSetWrites.push_back(descriptorSetWrite);
			}
			//����
			vkUpdateDescriptorSets(mDevice->getDevice(), static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
		}

	}

	DescriptorSet::~DescriptorSet(){}
}