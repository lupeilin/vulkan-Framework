#pragma once

#include "../base.h"
#include "device.h"
#include "description.h"
#include "descriptorSetLayout.h"
#include "descriptorPool.h"

namespace FF::Wrapper {

	/*
	* 对于每一个模型的渲染，都需要绑定一个DescriptorSet，绑定的位置就在CommandBuffer
	* 每一个DescriptorSet里面，都对应着一个vp矩阵使用的buffer，一个model矩阵使用的buffer，等等。其中也包括，
	*  binding，size等的描述信息
	* 由于交换链的存在，多帧可能并行渲染，所以我们需要为每一个交换链的图片，对应生成一个DescriptorSet。
	*/
	
	//这个class最终会作为一个model类里面的对象
	class DescriptorSet
	{
	public:
		using Ptr = std::shared_ptr<DescriptorSet>;
		static Ptr create(
			const Device::Ptr& device,
			const std::vector<UniformParameter::Ptr> params,
			const DescriptorSetLayout::Ptr& layout,
			const DescriptorPool::Ptr& pool,
			int frameCount
		) {
			return std::make_shared<DescriptorSet>(device, params, layout, pool, frameCount);
		}

		DescriptorSet(
			const Device::Ptr &device,
			const std::vector<UniformParameter::Ptr> params,
			const DescriptorSetLayout::Ptr &layout,
			const DescriptorPool::Ptr &pool,
			int frameCount
		);

		~DescriptorSet();

	private:
		std::vector<VkDescriptorSet> mDescriptorSets{};
		Device::Ptr mDevice{ nullptr };
	};
}