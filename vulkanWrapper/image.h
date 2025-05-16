#pragma once

#include "../base.h"
#include "device.h"
#include "commandPoll.h"

namespace FF::Wrapper {

	/*
	分析：如果我们需要做一张被用于纹理采样的图片，
	那么我们首先需要从undefinedLayout变换成为TransferDst，
	然后在数据拷贝完毕之后，再转换成为ShaderReadOnly
	*/
	class Image
	{
	public:
		using Ptr = std::shared_ptr<Image>;

		//工具函数：静态
		static Image::Ptr createDepthImage(
			const Device::Ptr& device, 
			const int& width,
			const int& height			
			);

	public:
		static Ptr create(const Device::Ptr& device,
			const int& width,
			const int& height,
			const VkFormat& format,
			const VkImageType& imageType,
			const VkImageTiling& tilling,
			const VkImageUsageFlags& usage, 
			const VkSampleCountFlagBits& samples,
			const VkMemoryPropertyFlags& properties, //memory
			const VkImageAspectFlags& aspectFlags //view
		) 
			{
			return std::make_shared<Image>(device, width, height, format, imageType,
				tilling, usage, samples, properties, aspectFlags); 
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

		//image格式转换 old: mlayout -> newLayout
		//barriar 也在这个函数里面去做，所以我们需要为这个barriar也传入一些参数
		void setImageLayout(
			VkImageLayout newLayout, 
			VkPipelineStageFlags srcStageMask, 
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresouceRange,  //和VkImageView很像
			const CommandPool::Ptr & commandPool
			);

		void fillImageData(size_t size, void* pData, const CommandPool::Ptr& commandPool); //已经读入一张图片，怎么传入

		static VkFormat finddepthFormat(const Device::Ptr& device);

		//用来查询深度缓存image的格式
		static VkFormat findSupportedFormat(
			const Device::Ptr& device,
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features);

		//我们的format是否有stencil的成分
		bool hasStencilComponent();

		[[nodiscard]] auto getImage() const { return mImage; }
		[[nodiscard]] auto getLayout() const { return mlayout; }
		[[nodiscard]] auto getWidth() const { return mWidth; }
		[[nodiscard]] auto getHeight() const { return mHeight; }
		[[nodiscard]] auto getImageView() const { return mImageView; }

	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		

	private:

		size_t			mWidth{ 0 };
		size_t			mHeight{ 0 };

		Device::Ptr		mDevice{ nullptr };
		VkImage			mImage{ VK_NULL_HANDLE };
		VkDeviceMemory  mImageMemory{ VK_NULL_HANDLE };
		VkImageView		mImageView{ VK_NULL_HANDLE };
		VkFormat		mFormat{ VK_FORMAT_UNDEFINED };

		VkImageLayout	mlayout{ VK_IMAGE_LAYOUT_UNDEFINED };
	};

}