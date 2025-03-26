#include "uniformManager.h"

namespace FF {

	UniformManager::UniformManager() {

	}

	UniformManager::~UniformManager() {}

	void UniformManager::init(const Wrapper::Device::Ptr& device, int frameCount) {
		auto vpParam = Wrapper::UniformParameter::create();
		vpParam->mBinding = 0;
		vpParam->mCount = 1;
		vpParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		vpParam->mSize = sizeof(VPMatrix);
		vpParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;
		
		for (size_t i = 0; i < frameCount; i++)
		{
			auto buffer = Wrapper::Buffer::createUniformBuffer(device, vpParam->mSize, nullptr);
			vpParam->mBuffers.push_back(buffer);
		}

		mUniformParams.push_back(vpParam);

		auto objectParam = Wrapper::UniformParameter::create();
		objectParam->mBinding = 1;
		objectParam->mCount = 1;
		objectParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		objectParam->mSize = sizeof(ObjectUniform);
		objectParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;

		for (size_t i = 0; i < frameCount; i++)
		{
			auto buffer = Wrapper::Buffer::createUniformBuffer(device, vpParam->mSize, nullptr);
			objectParam->mBuffers.push_back(buffer);
		}

		mUniformParams.push_back(objectParam); //所有所关于uniform的描述信息

		mDescriptorSetLayout = Wrapper::DescriptorSetLayout::create(device);
		mDescriptorSetLayout->build(mUniformParams);

		mDescriptorPool = Wrapper::DescriptorPool::create(device);
		mDescriptorPool->build(mUniformParams, frameCount);
	}

}