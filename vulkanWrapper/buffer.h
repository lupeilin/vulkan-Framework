#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	class Buffer
	{
	public:
		using Ptr = std::shared_ptr<Buffer>;
		static Ptr create(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) { 
			return std::make_shared<Buffer>(device, size, usage, properties); 
		}
	public:
		static Ptr createVertexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData);

		static Ptr createIndexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData);
		static Ptr createUniformBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData = nullptr);

	public:
		Buffer(const Device::Ptr &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		~Buffer();

		/*
		更新buffer数据，方式有两种：
		1、通过内存mapping的形式，直接对内存进行更改，适用于hostVisible类型的内存。
		2、如果本内存是localOptional（cpu不可见），那么就必须创建中间的stageBuffer（这是一个hostVisible类型的buffer），
			 先复制到stageBuffer，再拷贝如目标buffer

			 注：如果是需要频繁更改数据的buffer，建议使用hostVisible的buffer
		*/
		void updataBufferByMap(void *data, size_t size);

		void updataBufferByStage(void* data, size_t size);

		void copyBuffer(const VkBuffer& srcBuufer, const VkBuffer& dstBuffer, VkDeviceSize size);

		[[nodiscard]] auto getBuffer() { return mBuffer; }

	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkBuffer mBuffer{ VK_NULL_HANDLE };//这里创建的mBuffer实际上是一个，在cpu端的描述符号。
		VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE }; //这个才是真正的gpu端创建的buffer
		Device::Ptr mDevice{ nullptr };
	};
}