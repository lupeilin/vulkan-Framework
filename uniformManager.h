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

		//���º���������mUniformParams�����mBuffers���ڱ������У�����ֻ����VPMatrix��ObjectUniform����������
		void updata(const VPMatrix& vpMatrix, const ObjectUniform& objectUniform, const int& frameCount);

		[[nodiscard]] auto getDescriptorSetLayout() const { return mDescriptorSetLayout; }
		[[nodiscard]] auto getmDescriptorSet(int frameCount) const { return mDescriptorSet->getDescriptorSet(frameCount); }//frameCount�� Ҫget mDescriptorSet�е���һ��

		

	private:
		Wrapper::Device::Ptr mDevice{ nullptr };
		std::vector<Wrapper::UniformParameter::Ptr> mUniformParams{}; //������������uniform��������Ϣ

		Wrapper::DescriptorSetLayout::Ptr mDescriptorSetLayout{ nullptr };
		Wrapper::DescriptorPool::Ptr mDescriptorPool{ nullptr }; //��������descriptorSet
		Wrapper::DescriptorSet::Ptr mDescriptorSet{ nullptr };
	};

}