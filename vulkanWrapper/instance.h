#pragma once

#include "../base.h"

namespace FF::Wrapper {
	class Instance
	{
	public:
		using Ptr = std::shared_ptr<Instance>;
		static Ptr create(bool enableValidationlayer) { return std::make_shared<Instance>(enableValidationlayer); }

		Instance(bool enableValidationlayer);
		~Instance();

		void printAvailableExtensions();

		std::vector<const char*> getRequiredExtensions();

		//layers

		bool checkValidationLayersSupport();
		void setupDebugger();

		[[nodiscard]]VkInstance getInstance() const { return mInstance; }

		[[nodiscard]] bool getEnableValidationLayer() const { return mEnableValidationLayer; }

	private:
		VkInstance mInstance{ VK_NULL_HANDLE };
		bool mEnableValidationLayer{ false };
		VkDebugUtilsMessengerEXT mDebugger{ VK_NULL_HANDLE };
	};

}  