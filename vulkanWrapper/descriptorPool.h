#pragma once

#include "../base.h"
#include "device.h"
#include "description.h"
namespace FF::Wrapper {
	class DescriptorPool
	{
	public:
		using Ptr = std::shared_ptr<DescriptorPool>;
		static Ptr create(const Device::Ptr& device) { return std::make_shared<DescriptorPool>(device); }

		DescriptorPool(const Device::Ptr& device);

		~DescriptorPool();
		//frameCount我们一共会有多少个swapchain的image并行渲染
		void build(std::vector<UniformParameter::Ptr> &params, const int &frameCount);

		[[nodiscard]] auto getDescriptorPool() const { return mPool; }

	private:
		//需要知道每一个uniform都有多少个，才能为其预留分配空间，所谓空间，并不是uniformbuffer的大小，
		//而是，根据每一种uniform不同，那么描述符就不同，所以空间指的是描述符的大小，蕴含在系统内部的
		VkDescriptorPool mPool{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}