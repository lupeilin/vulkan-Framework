#pragma once
#include"vulkanWrapper/device.h"


#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/descriptorSetLayout.h"
#include "vulkanWrapper/descriptorPool.h"
#include "vulkanWrapper/descriptorSet.h"
#include "vulkanWrapper/description.h"

namespace FF {
	class UniformManager
	{
	public:
		using Ptr = std::shared_ptr<UniformManager>;
		static Ptr create() { return std::make_shared<UniformManager>(); }
		UniformManager();

		~UniformManager();

		void init(const Wrapper::Device::Ptr& device, int frameCount);

		[[nodiscard]] auto getDescriptorSetLayout() const { return mDescriptorSetLayout; }

	private:
		std::vector<Wrapper::UniformParameter::Ptr> mUniformParams{}; //������������uniform��������Ϣ

		Wrapper::DescriptorSetLayout::Ptr mDescriptorSetLayout{ nullptr };
		Wrapper::DescriptorPool::Ptr mDescriptorPool{ nullptr }; //��������descriptorSet
	};

}