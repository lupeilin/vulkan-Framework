#pragma once

#include "../base.h"
#include "instance.h"
#include"windowSurface.h"
namespace FF::Wrapper {

	const std::vector<const char*> deviceRequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE1_EXTENSION_NAME // 在vulkan中，y轴向下，openGL中y轴向上（标准的右手坐标系）
											//第二个扩展就是将坐标系变成一个标准的右手坐标系
	};

	class Device
	{
	public:
		using Ptr = std::shared_ptr<Device>;
		static Ptr create(Instance::Ptr instance,  WindowSurface::Ptr surface) { return std::make_shared<Device>(instance, surface); }

		Device(Instance::Ptr instance, WindowSurface::Ptr surface);

		~Device();

		void pickPhysicalDevice();
		int rateDevice(VkPhysicalDevice device);

		bool isDeviceSuitable(VkPhysicalDevice device);

		void initQueueFamilies(VkPhysicalDevice device);

		void createLogicalDevice();

		bool isQueueFamilyComplete();

		[[nodiscard]] auto getDevice() const { return mDvice; }
		[[nodiscard]] auto getPhysicalDevice() const { return mPhysicalDevice; }

		[[nodiscard]] auto getGraphicQueueFamily() const { return mGraphicQueueFamily; }
		[[nodiscard]] auto getPresentQueueFamily() const { return mPresentQueueFamily; }

		[[nodiscard]] auto getGraphicQueue() const { return mGraphicQueue; }
		[[nodiscard]] auto getPresentQueue() const { return mPresentQueue; }
	private:
		VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
		Instance::Ptr mInstance{ nullptr };
		WindowSurface::Ptr mSurface{ nullptr };

		//存储当前渲染任务队列簇的id
		std::optional<uint32_t> mGraphicQueueFamily;
		VkQueue  mGraphicQueue{ VK_NULL_HANDLE };
		std::optional<uint32_t> mPresentQueueFamily;
		VkQueue mPresentQueue{ VK_NULL_HANDLE };

		//逻辑设备
		VkDevice mDvice{ VK_NULL_HANDLE };
	};
}
