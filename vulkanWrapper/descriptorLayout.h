#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {

	//������Ҫ֪�������ֵ��У���������Щuniform��ÿһ��������binding��ÿһ����ʲô����
	struct DescriptorBindingParameter
	{
		using Ptr = std::shared_ptr<DescriptorBindingParameter>;
		static Ptr create() { return std::make_shared<DescriptorBindingParameter>(); }
		size_t mSize{ 0 };
		uint32_t mBinding{ 0 };

		//����ÿһ��binding�㣬���п��ܴ��벻ֹһ��uniform��������һ��matrix�����飬���count�ʹ�����һ�����ݵĴ�С��
		//��Ҫʹ��indexDescriptor����
		uint32_t mCount{ 0 }; 
		VkDescriptorType mDescriptorType; //��uniform  ���� image texture
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