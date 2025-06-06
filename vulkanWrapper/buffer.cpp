#include "buffer.h"
#include "commandBuffer.h"
#include "commandPoll.h" 
namespace FF::Wrapper {

	Buffer::Ptr Buffer::createVertexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(
			device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, //copy的dst  buffer ，是一个vertex buffer
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT//创建成localbuffer，因为模型数据一般不怎么修改
		);

		buffer->updataBufferByStage(pData, size);
		return buffer;
	}

	Buffer::Ptr Buffer::createIndexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(
			device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, //copy的dst  buffer ，是一个index buffer
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT//创建成localbuffer，因为模型数据一般不怎么修改
		);

		buffer->updataBufferByStage(pData, size);
		return buffer;
	}

	Buffer::Ptr Buffer::createUniformBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(
			device, size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  我们可以利用updataBufferByMap的形式对没存进行更新，更新之后能够立马显现
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  
		);
		if (pData != nullptr) {
			buffer->updataBufferByStage(pData, size);
		}
		return buffer;
	}

	Buffer::Ptr Buffer::createstageBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(
			device, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT  我们可以利用updataBufferByMap的形式对没存进行更新，更新之后能够立马显现
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		if (pData != nullptr) {
			buffer->updataBufferByMap(pData, size);
		}
		return buffer;
	}

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

		//填写bufferInfo
		mBufferInfo.buffer = mBuffer;
		mBufferInfo.offset = 0;
		mBufferInfo.range = size;

	}

	Buffer::~Buffer() {
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
		memcpy(memPtr, data, size);
		vkUnmapMemory(mDevice->getDevice(), mBufferMemory);

		//这里其实是把gpu的显存，划分到cpu的虚拟地址当中，当成了cpu的内存进行操作
	}

	void Buffer::updataBufferByStage(void* data, size_t size) {

		//首先创建一个中间承接的stageBuffer(cpu可见)
		//VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 表示这个buffer被用于传输数据，而且是数据传输的源头
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 表示memcpy之后，数据直接刷入gpu内存
		auto stageBuffer = Buffer::create(mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stageBuffer->updataBufferByMap(data, size);

		copyBuffer(stageBuffer->getBuffer(), mBuffer, static_cast<VkDeviceSize>(size));
	}

	//把数据从cpu可见的buffer拷贝到localBuffer
	void  Buffer::copyBuffer(const VkBuffer& srcBuufer, const VkBuffer& dstBuffer, VkDeviceSize size) {

		//queue有graphic queue、present queue、 transform queue，我们没有船舰transform queue，所以我们在这里用graphic queue代替使用
		auto commandPool = CommandPool::create(mDevice);
		auto commandBuffer = CommandBuffer::create(mDevice, commandPool);

		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);//这个命令只提交一次

		VkBufferCopy copyInfo{};
		copyInfo.size = size; 

		commandBuffer->copyBufferToBuffer(srcBuufer, dstBuffer, 1, { copyInfo });

		commandBuffer->end();

		commandBuffer->submitSync(mDevice->getGraphicQueue(), VK_NULL_HANDLE);
	}

	

}