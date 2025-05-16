#include "uniformManager.h"

namespace FF {

	UniformManager::UniformManager() {

	}

	UniformManager::~UniformManager() {}

	void UniformManager::init(const Wrapper::Device::Ptr& device, const Wrapper::CommandPool::Ptr& commandPool, int frameCount) {
		mDevice = device;

		auto vpParam = Wrapper::UniformParameter::create();
		vpParam->mBinding = 0;
		vpParam->mCount = 1;
		vpParam->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		vpParam->mSize = sizeof(VPMatrix);
		vpParam->mStage = VK_SHADER_STAGE_VERTEX_BIT;
		
		for (size_t i = 0; i < frameCount; i++)
		{
			auto buffer = Wrapper::Buffer::createUniformBuffer(mDevice, vpParam->mSize, nullptr);
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
			auto buffer = Wrapper::Buffer::createUniformBuffer(mDevice, objectParam->mSize, nullptr);
			objectParam->mBuffers.push_back(buffer);
		} 

		mUniformParams.push_back(objectParam); //所有所关于uniform的描述信息


		auto textureParam = Wrapper::UniformParameter::create();
		textureParam->mBinding = 2;
		textureParam->mCount = 1;
		textureParam->mDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//textureParam->mSize = sizeof(ObjectUniform);
		textureParam->mStage = VK_SHADER_STAGE_FRAGMENT_BIT;
		textureParam->mTexture = Texture::create(mDevice, commandPool, "assets/1.jpeg");

		mUniformParams.push_back(textureParam); 


		mDescriptorSetLayout = Wrapper::DescriptorSetLayout::create(device);
		mDescriptorSetLayout->build(mUniformParams);

		mDescriptorPool = Wrapper::DescriptorPool::create(device);
		mDescriptorPool->build(mUniformParams, frameCount);

		mDescriptorSet = Wrapper::DescriptorSet::create(device, mUniformParams, mDescriptorSetLayout, mDescriptorPool, frameCount);

	}

	void  UniformManager::updata(const VPMatrix& vpMatrix, const ObjectUniform& objectUniform, const int& frameCount) {
		//updata vp matrix
		mUniformParams[0]->mBuffers[frameCount]->updataBufferByMap((void *)&vpMatrix, sizeof(VPMatrix));

		//updata objectUniform
		mUniformParams[1]->mBuffers[frameCount]->updataBufferByMap((void*)&objectUniform, sizeof(ObjectUniform));

	}
}