#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	class CommandPool
	{
	public:
		using Ptr = std::shared_ptr<CommandPool>;
		static Ptr create(const Device::Ptr& device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlagBits flag) { 
			return std::make_shared<CommandPool>(device, queueFamilyIndex, flag); 
		}

		CommandPool(const Device::Ptr & device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlagBits flag);

		~CommandPool();

	private:
		VkCommandPool mCommandPool{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}