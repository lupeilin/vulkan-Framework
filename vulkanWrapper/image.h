#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	class Image
	{
	public:
		using Ptr = std::shared_ptr<Image>;
		static Ptr create() {
			return std::make_shared<Image>();
		}

		Image(const Device::Ptr& device,
			 const int& width,
			const int& height,
			const VkFormat& format,
			const VkImageType& imageType,
			const VkImageTiling& tilling,
			const VkImageUsageFlags& usage,
			const VkSampleCountFlagBits& samples,
			const VkMemoryPropertyFlags& properties, //memory
			const VkImageAspectFlags& aspectFlags //view
			);   
		
		~Image();

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		Device::Ptr mDevice{ nullptr };
		VkImage			mImage{ VK_NULL_HANDLE };
		VkDeviceMemory  mImageMemory{ VK_NULL_HANDLE };
		VkImageView		mImageView{ VK_NULL_HANDLE };
	};

}