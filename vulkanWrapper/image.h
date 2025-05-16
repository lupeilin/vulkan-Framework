#pragma once

#include "../base.h"
#include "device.h"
#include "commandPoll.h"

namespace FF::Wrapper {

	/*
	���������������Ҫ��һ�ű��������������ͼƬ��
	��ô����������Ҫ��undefinedLayout�任��ΪTransferDst��
	Ȼ�������ݿ������֮����ת����ΪShaderReadOnly
	*/
	class Image
	{
	public:
		using Ptr = std::shared_ptr<Image>;

		//���ߺ�������̬
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

		//image��ʽת�� old: mlayout -> newLayout
		//barriar Ҳ�������������ȥ��������������ҪΪ���barriarҲ����һЩ����
		void setImageLayout(
			VkImageLayout newLayout, 
			VkPipelineStageFlags srcStageMask, 
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresouceRange,  //��VkImageView����
			const CommandPool::Ptr & commandPool
			);

		void fillImageData(size_t size, void* pData, const CommandPool::Ptr& commandPool); //�Ѿ�����һ��ͼƬ����ô����

		static VkFormat finddepthFormat(const Device::Ptr& device);

		//������ѯ��Ȼ���image�ĸ�ʽ
		static VkFormat findSupportedFormat(
			const Device::Ptr& device,
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features);

		//���ǵ�format�Ƿ���stencil�ĳɷ�
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