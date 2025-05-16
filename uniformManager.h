#pragma once
#include"vulkanWrapper/device.h"
#include "base.h"

#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/descriptorSetLayout.h"
#include "vulkanWrapper/descriptorPool.h"
#include "vulkanWrapper/descriptorSet.h"
#include "vulkanWrapper/description.h"
#include "vulkanWrapper/commandPoll.h"

namespace FF {
	class UniformManager
	{
	public:
		using Ptr = std::shared_ptr<UniformManager>;
		static Ptr create() { return std::make_shared<UniformManager>(); }
		UniformManager();

		~UniformManager();

		void init(const Wrapper::Device::Ptr& device, const Wrapper::CommandPool::Ptr& commandPool, int frameCount);

		//更新函数，更新mUniformParams里面的mBuffers，在本例子中，我们只更新VPMatrix和ObjectUniform这两个矩阵
		void updata(const VPMatrix& vpMatrix, const ObjectUniform& objectUniform, const int& frameCount);

		[[nodiscard]] auto getDescriptorSetLayout() const { return mDescriptorSetLayout; }
		[[nodiscard]] auto getmDescriptorSet(int frameCount) const { return mDescriptorSet->getDescriptorSet(frameCount); }//frameCount： 要get mDescriptorSet中的哪一个

		

	private:
		Wrapper::Device::Ptr mDevice{ nullptr };
		std::vector<Wrapper::UniformParameter::Ptr> mUniformParams{}; //所有有所关于uniform的描述信息

		Wrapper::DescriptorSetLayout::Ptr mDescriptorSetLayout{ nullptr };
		Wrapper::DescriptorPool::Ptr mDescriptorPool{ nullptr }; //用来分配descriptorSet
		Wrapper::DescriptorSet::Ptr mDescriptorSet{ nullptr };
	};

}