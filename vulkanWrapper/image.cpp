#include "image.h"
#include "commandBuffer.h"
#include "buffer.h"

namespace FF::Wrapper {
	Image::Ptr Image::createDepthImage(
		const Device::Ptr& device,
		const int& width,
		const int& height
	) {
		std::vector<VkFormat> formats = {
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};
		VkFormat depthFormat = findSupportedFormat(device, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		return Image::create(device, width, height, depthFormat, VK_IMAGE_TYPE_2D,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_SAMPLE_COUNT_1_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);

	}
	Image::Image(const Device::Ptr& device,
		const int& width,
		const int& height,
		const VkFormat& format,
		const VkImageType& imageType,
		const VkImageTiling& tilling,
		const VkImageUsageFlags& usage,
		const VkSampleCountFlagBits& samples,
		const VkMemoryPropertyFlags& properties, //memory
		const VkImageAspectFlags& aspectFlags //view
	) {
		mDevice = device;
		mlayout = VK_IMAGE_LAYOUT_UNDEFINED;
		mWidth = width;
		mHeight = height;
		mFormat = format;

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.format = format;
		imageCreateInfo.imageType = imageType;
		imageCreateInfo.tiling = tilling;//��ƽ��ֱ������Ϊ�������Կ�Ϊ��
		imageCreateInfo.usage = usage;  //color or depth
		imageCreateInfo.samples = samples;//���ڿ���ݵ�ʱ�򣬱�ʾ����һ��bit������4��bit������8��bit
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;// ��ռ�ģ�ֻ�ܸ�һ������ʹ��

		if (vkCreateImage(mDevice->getDevice(), &imageCreateInfo, nullptr, &mImage) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create image");
		}

		//�����ڴ�ռ�
		VkMemoryRequirements memReq{};
		vkGetImageMemoryRequirements(mDevice->getDevice(), mImage, &memReq);

		VkMemoryAllocateInfo alocateInfo{};
		alocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alocateInfo.allocationSize = memReq.size;

		//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT  : ֻ��gpu�ɼ���cpu�˲��ɼ�
		//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  : gpu��cpu���ɼ�

		//����������buffer������ڴ����͵�ID�ǣ�  memReq.memoryTypeBits ��һ��ID���л�����Ľ����
		alocateInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice->getDevice(), &alocateInfo, nullptr, &mImageMemory) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to allocate memory");
		}

		//��cpu�˵�����mImage����gpu��������buffer��mImageMemory�� ��������
		vkBindImageMemory(mDevice->getDevice(), mImage, mImageMemory, 0);

		//����imageview
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = imageType == VK_IMAGE_TYPE_2D ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.image = mImage;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;//��Ϊ��ɫ������Ϊ���
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(mDevice->getDevice(), &imageViewCreateInfo, nullptr, &mImageView) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create image View");
		}
	}


	Image::~Image() {
		if (mImageView != VK_NULL_HANDLE) {
			vkDestroyImageView(mDevice->getDevice(), mImageView, nullptr);
		}
		if (mImageMemory != VK_NULL_HANDLE) {
			vkFreeMemory(mDevice->getDevice(), mImageMemory, nullptr);
		} 
		if (mImage != VK_NULL_HANDLE) {
			vkDestroyImage(mDevice->getDevice(), mImage, nullptr);
		}
	}

	uint32_t Image::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalDevice(), &memProps);

		// ���� typeFilter = 0x001 | 0x100 = 0x101 , i = 0,   �� i����Ӧ�����;��� 1 << i,
		for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
				return i;
			}
		}

		throw std::runtime_error("Error : failed to find the property memory type!");
	}

	VkFormat Image::finddepthFormat(const Device::Ptr& device) {
		std::vector<VkFormat> formats = {
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};
		VkFormat depthFormat = findSupportedFormat(device, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		return depthFormat;
	}

	VkFormat Image::findSupportedFormat(const Device::Ptr& device, const std::vector<VkFormat>& candidates, 
		VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (auto& format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice(), format, &props);
			if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("Error : failed to find supported format");
	}

	bool Image::hasStencilComponent() {
		return mFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || mFormat == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	//�˴�����һ����ݵ�д������װ�ԱȽϺã����ǿ��Զ�����Ϊһ�����ߺ�����
	// д��Tool��������
	void Image::setImageLayout(
		VkImageLayout newLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkImageSubresourceRange subresouceRange,  //��VkImageView����
		const CommandPool::Ptr& commandPool
	) {
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = mlayout;
		imageMemoryBarrier.newLayout = newLayout;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;//ת������ͼƬ���ڶ��е�����Ȩ
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = mImage;
		imageMemoryBarrier.subresourceRange = subresouceRange;

		switch (mlayout)
		{
			//������޶���layout��˵��ͼƬ�ձ��������Ϸ�һ��û���κβ����������Ϸ���һ�����������
			//���Բ�������һ���׶ε��κβ���
		case VK_IMAGE_LAYOUT_UNDEFINED:
			imageMemoryBarrier.srcAccessMask = 0; //0ָ���ǲ�����
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; //oldlayout��Ϊ��һ��dst������Ҫ�ȴ�д�����
			break;
		default:
			break;
		}

		switch (newLayout)
		{
			//���Ŀ���ǣ���ͼƬת����Ϊһ�����Ʋ�����Ŀ��ͼƬ/�ڴ棬��ô�������Ĳ���һ����һ��д�����
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
			//���Ŀ���ǣ���ͼƬת����Ϊһ���ʺ���Ϊ����ĸ�ʽ����ô�������Ĳ���һ���ǣ���ȡ  
			//�����Ϊtexture,��ô��Դֻ�����֣�һ����ͨ��map��cpu����������һ����ͨ��stageBuffer��������
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
			if (imageMemoryBarrier.srcAccessMask = 0) {
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT;
				}
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			imageMemoryBarrier.dstAccessMask =  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		default:
			break;
		}

		//����layout
		mlayout = newLayout;

		//��Barrier�͵�����ȥִ��
		auto commandBuffer = CommandBuffer::create(mDevice, commandPool);
		//��ʽת����һ��һ����ִ�е�commandBuffer������β������֮�󲻻��������
		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		commandBuffer->transferImageLayout(imageMemoryBarrier, srcStageMask, dstStageMask);
		commandBuffer->end();

		commandBuffer->submitSync(mDevice->getGraphicQueue());
	}

	void Image::fillImageData(size_t size, void* pData, const CommandPool::Ptr& commandPool) {
		assert(pData);
		assert(size);

		auto stageBuffer = Buffer::createstageBuffer(mDevice, size, pData);

		//��һ��buffer�е����ݿ�����image��ȥ
		auto commandBuffer = CommandBuffer::create(mDevice, commandPool);

		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		commandBuffer->copyBufferToImage(stageBuffer->getBuffer(), mImage, mlayout, mWidth, mHeight);
		commandBuffer->end();

		commandBuffer->submitSync(mDevice->getGraphicQueue());
	}
}