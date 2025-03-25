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
		����buffer���ݣ���ʽ�����֣�
		1��ͨ���ڴ�mapping����ʽ��ֱ�Ӷ��ڴ���и��ģ�������hostVisible���͵��ڴ档
		2��������ڴ���localOptional��cpu���ɼ�������ô�ͱ��봴���м��stageBuffer������һ��hostVisible���͵�buffer����
			 �ȸ��Ƶ�stageBuffer���ٿ�����Ŀ��buffer

			 ע���������ҪƵ���������ݵ�buffer������ʹ��hostVisible��buffer
		*/
		void updataBufferByMap(void *data, size_t size);

		void updataBufferByStage(void* data, size_t size);

		void copyBuffer(const VkBuffer& srcBuufer, const VkBuffer& dstBuffer, VkDeviceSize size);

		[[nodiscard]] auto getBuffer() { return mBuffer; }

	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkBuffer mBuffer{ VK_NULL_HANDLE };//���ﴴ����mBufferʵ������һ������cpu�˵��������š�
		VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE }; //�������������gpu�˴�����buffer
		Device::Ptr mDevice{ nullptr };
	};
}