#include "buffer.h"
#include "commandBuffer.h"
#include "commandPoll.h" 
namespace FF::Wrapper {

	Buffer::Buffer(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		mDevice = device;
		
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;//表述buffer的用途
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //专属的

		//这里创建的mBuffer实际上是一个，在cpu端的描述符号。
		if (vkCreateBuffer(mDevice->getDevice(), &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create buffer");
		}

		//创建现存空间
		VkMemoryRequirements memReq{};
		vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

		VkMemoryAllocateInfo alocateInfo{};
		alocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alocateInfo.allocationSize = memReq.size;

		//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT  : 只有gpu可见，cpu端不可见
		//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  : gpu、cpu均可见

		//符合我上述buffer需求的内存类型的ID们！  memReq.memoryTypeBits 是一堆ID进行或操作的结果。
		alocateInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice->getDevice(), &alocateInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to allocate memory");
		}

		//将cpu端的描述mBuffer，和gpu端真正的buffer：mBufferMemory， 绑定起来。
		vkBindBufferMemory(mDevice->getDevice(), mBuffer, mBufferMemory, 0);
	}

	Buffer::~Buffer(){
		if (mBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(mDevice->getDevice(), mBuffer, nullptr);
		}

		if (mBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(mDevice->getDevice(), mBufferMemory, nullptr);
		}
	}

	uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalDevice(), &memProps);

		// 假设 typeFilter = 0x001 | 0x100 = 0x101 , i = 0,   第 i个对应的类型就是 1 << i,
		for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
				return i; 
			}
		}

		throw std::runtime_error("Error : failed to find the property memory type!");
	}

	void  Buffer::updataBufferByMap(void* data, size_t size) {
		void* memPtr = nullptr; //这个就是等会要map的地址

		vkMapMemory(mDevice->getDevice(), mBufferMemory, 0, size, 0, &memPtr);//把 memPtr指向 mBufferMemory的起始地址。
		memcpy(data, memPtr, size);
		vkUnmapMemory(mDevice->getDevice(), mBufferMemory);

		//这里其实是把gpu的显存，划分到cpu的虚拟地址当中，当成了cpu的内存进行操作
	}

	void Buffer::updataBufferByStage(void* data, size_t size) {

		//首先创建一个中间承接的stageBuffer(cpu可见)
		//VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 表示这个buffer被用于传输数据，而且是数据传输的源头
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 表示memcpy之后，数据直接刷入gpu内存
		auto stageBuffer = Buffer::create(mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
		stageBuffer->updataBufferByMap(data, size);


	}

	void  Buffer::copyBuffer(const VkBuffer& srcBuufer, const VkBuffer& dstBuffer, VkDeviceSize size) {

	}
}