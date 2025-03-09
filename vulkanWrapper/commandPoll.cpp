#include "commandPoll.h"

namespace FF::Wrapper {
	
	CommandPool::CommandPool(const Device::Ptr& device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlagBits flag) {
		mDevice = device;

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = queueFamilyIndex; //本commandPool服从队列簇queueFamilyIndex的管理，向queueFamilyIndex去提交。

	}

	CommandPool::~CommandPool() {
		if (mCommandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(mDevice->getDevice(), mCommandPool, nullptr);
		}
	}
}