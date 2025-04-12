#include "image.h"
#include "commandBuffer.h"
#include "buffer.h"

namespace FF::Wrapper {
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
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; //oldlayout��Ϊ��һ��dst������Ҫ�ȴ�д�����
		default:
			break;
		}

		switch (newLayout)
		{
			//���Ŀ���ǣ���ͼƬת����Ϊһ�����Ʋ�����Ŀ��ͼƬ/�ڴ棬��ô�������Ĳ���һ����һ��д�����
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			//���Ŀ���ǣ���ͼƬת����Ϊһ���ʺ���Ϊ����ĸ�ʽ����ô�������Ĳ���һ���ǣ���ȡ  
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		default:
			break;
		}

		//����layout
		mlayout = newLayout;

		//��Barrier�͵�����ȥִ��
		auto commandBuffer = CommandBuffer::create(mDevice, commandPool);
		//��ʽת����һ��һ����ִ�е�commandBuffer������β������֮�󲻻��������
		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		commandBuffer->transferImageLayout(imageMemoryBarrier, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
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