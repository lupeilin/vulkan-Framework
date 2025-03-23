#pragma once

#include "../base.h"

namespace FF::Wrapper {
	class Window
	{
	public:
		using Ptr = std::shared_ptr<Window>;
		static Ptr create(const int& width, const int& height) { return std::make_shared<Window>(width ,height); }

		Window(const int &width, const int &height);

		~Window();

		bool shouldClose();

		void pollEvents();

		[[nodiscard]]auto getWindow() { return mWindow; }

	public:
		//记录window窗口大小变化
		bool mWindowResized{ false };
	private:
		int mWidth{ 0 };
		int mHeight{ 0 }; 
		GLFWwindow* mWindow{ NULL };
	};
}