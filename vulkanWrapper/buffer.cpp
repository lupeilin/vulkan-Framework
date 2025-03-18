#include "buffer.h"
#include "commandBuffer.h"
#include "commandPoll.h" 
namespace FF::Wrapper {

	Buffer::Buffer(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		mDevice = device;
		
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;//����buffer����;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //ר����

		//���ﴴ����mBufferʵ������һ������cpu�˵��������š�
		if (vkCreateBuffer(mDevice->getDevice(), &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to create buffer");
		}

		//�����ִ�ռ�
		VkMemoryRequirements memReq{};
		vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

		VkMemoryAllocateInfo alocateInfo{};
		alocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alocateInfo.allocationSize = memReq.size;

		//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT  : ֻ��gpu�ɼ���cpu�˲��ɼ�
		//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  : gpu��cpu���ɼ�

		//����������buffer������ڴ����͵�ID�ǣ�  memReq.memoryTypeBits ��һ��ID���л�����Ľ����
		alocateInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice->getDevice(), &alocateInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Error : failed to allocate memory");
		}

		//��cpu�˵�����mBuffer����gpu��������buffer��mBufferMemory�� ��������
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

		// ���� typeFilter = 0x001 | 0x100 = 0x101 , i = 0,   �� i����Ӧ�����;��� 1 << i,
		for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
				return i; 
			}
		}

		throw std::runtime_error("Error : failed to find the property memory type!");
	}

	void  Buffer::updataBufferByMap(void* data, size_t size) {
		void* memPtr = nullptr; //������ǵȻ�Ҫmap�ĵ�ַ

		vkMapMemory(mDevice->getDevice(), mBufferMemory, 0, size, 0, &memPtr);//�� memPtrָ�� mBufferMemory����ʼ��ַ��
		memcpy(data, memPtr, size);
		vkUnmapMemory(mDevice->getDevice(), mBufferMemory);

		//������ʵ�ǰ�gpu���Դ棬���ֵ�cpu�������ַ���У�������cpu���ڴ���в���
	}

	void Buffer::updataBufferByStage(void* data, size_t size) {

		//���ȴ���һ���м�нӵ�stageBuffer(cpu�ɼ�)
		//VK_BUFFER_USAGE_TRANSFER_SRC_BIT : ��ʾ���buffer�����ڴ������ݣ����������ݴ����Դͷ
		//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : ��ʾmemcpy֮������ֱ��ˢ��gpu�ڴ�
		auto stageBuffer = Buffer::create(mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
		stageBuffer->updataBufferByMap(data, size);


	}

	void  Buffer::copyBuffer(const VkBuffer& srcBuufer, const VkBuffer& dstBuffer, VkDeviceSize size) {

	}
}