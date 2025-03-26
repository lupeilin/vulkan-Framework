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
			//描述符的更新，填写好VkWriteDescriptorSet之后去更新，就可以把一堆mDescriptorSets更新出来
			//对每一个DescriptorSet ，我们需要把params里面的描述信息，写入其中
			std::vector<VkWriteDescriptorSet> descriptorSetWrites{};
			for (const auto& param : params) {
				VkWriteDescriptorSet descriptorSetWrite{};
				descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorSetWrite.dstSet = mDescriptorSets[i]; //我们准备写第i个DescriptorSet
				descriptorSetWrite.dstArrayElement = 0;
				descriptorSetWrite.descriptorType = param->mDescriptorType;
				descriptorSetWrite.descriptorCount = param->mCount;
				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
					descriptorSetWrite.pBufferInfo = &param->mBuffers[i]->getBufferInfo();
				}
				if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
					//TODE:纹理相关
				}
				descriptorSetWrites.push_back(descriptorSetWrite);
			}
			//更新
			vkUpdateDescriptorSets(mDevice->getDevice(), static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, nullptr);
		}

	}

	DescriptorSet::~DescriptorSet(){}
}