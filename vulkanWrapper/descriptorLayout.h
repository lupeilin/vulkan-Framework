#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {

	//我们需要知道，布局当中，到底有那些uniform，每一个多大，如何binding，每一个是什么类型
	struct DescriptorBindingParameter
	{
		using Ptr = std::shared_ptr<DescriptorBindingParameter>;
		static Ptr create() { return std::make_shared<DescriptorBindingParameter>(); }
		size_t mSize{ 0 };
		uint32_t mBinding{ 0 };

		//对于每一个binding点，都有可能传入不止一个uniform（可能是一个matrix的数组，这个count就代表了一个数据的大小）
		//需要使用indexDescriptor类型
		uint32_t mCount{ 0 }; 
		VkDescriptorType mDescriptorType; //是uniform  还是 image texture
		VkShaderStageFlagBits mStage;
	};

	class DescriptorSetLayout
	{
	public:
		using Ptr = std::shared_ptr<DescriptorSetLayout>;
		static Ptr create(const Device::Ptr& device) { return std::make_shared<DescriptorSetLayout>(device); }


		DescriptorSetLayout(const Device::Ptr& device);

		~DescriptorSetLayout();

		void addUniformParam(const DescriptorBindingParameter::Ptr &param);

		void build();

		[[nodiscard]] auto getLayout() { return mLayout; }

	private:
		VkDescriptorSetLayout mLayout{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		std::vector<DescriptorBindingParameter::Ptr> mParams{};
	};

}