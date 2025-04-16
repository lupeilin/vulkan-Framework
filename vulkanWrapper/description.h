#pragma once
#include "buffer.h"
#include "../texture/texture.h"
 
namespace FF::Wrapper {
	//我们需要知道，布局当中，到底有那些uniform，每一个多大，如何binding，每一个是什么类型
	struct UniformParameter
	{
		using Ptr = std::shared_ptr<UniformParameter>;
		static Ptr create() { return std::make_shared<UniformParameter>(); }
		size_t mSize{ 0 };
		uint32_t mBinding{ 0 };

		//对于每一个binding点，都有可能传入不止一个uniform（可能是一个matrix的数组，这个count就代表了一个数据的大小）
		//需要使用indexDescriptor类型
		uint32_t mCount{ 0 };
		VkDescriptorType mDescriptorType; //是uniform  还是 image texture
		VkShaderStageFlagBits mStage;

		std::vector<Buffer::Ptr> mBuffers{}; //描述信息对应的buffer
		Texture::Ptr mTexture{ nullptr };
	};
}