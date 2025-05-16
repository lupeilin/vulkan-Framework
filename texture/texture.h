#pragma once

#include "../base.h"
#include "../vulkanWrapper/image.h"
#include "../vulkanWrapper/sampler.h"
#include "../vulkanWrapper/device.h"
#include "../stb_image.h"
#include "../vulkanWrapper/commandPoll.h"


namespace FF {
	class Texture
	{
	public:
		using Ptr = std::shared_ptr<Texture>;
		static Ptr create(const Wrapper::Device::Ptr& device, 
			const Wrapper::CommandPool::Ptr& commandPool, 
			const std::string& imageFilePath) { 
			return std::make_shared<Texture>(device, commandPool, imageFilePath); 
		}

		Texture(const Wrapper::Device::Ptr& device, const Wrapper::CommandPool::Ptr& commandPool, const std::string &imageFilePath);

		~Texture();

		[[discard]] auto getImage() const{ return mImage; }
		[[discard]] auto getSampler() const { return mSampler; }
		[[nodiscard]] VkDescriptorImageInfo& getImageInfo() { return mImageInfo; } //������������ľ������

	private:
		Wrapper::Device::Ptr mDevice{ nullptr };

		Wrapper::Image::Ptr  mImage{ nullptr };
		Wrapper::Sampler::Ptr mSampler{ nullptr };

		//texture �������Ϊһ��������������ȥ��
		VkDescriptorImageInfo mImageInfo{};
	};
}