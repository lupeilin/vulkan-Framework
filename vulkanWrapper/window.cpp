#include"window.h"

namespace FF::Wrapper {
	//设置窗口大小变化的回调函数
	static void windowResized(GLFWwindow* window, int width, int height) {
		//把glfwSetWindowUserPointer(mWindow, this);中传入的this指针拿出来。并重新解释当前指针所指向的内存块
		auto pUserData = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pUserData->mWindowResized = true;
	}


	Window::Window(const int& width, const int& height) {
		mWidth = width;
		mHeight = height;
		glfwInit();

		//设置环境，关掉opengl API 并且禁止窗口改变大小
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		mWindow = glfwCreateWindow(mWidth, mHeight, "vulkan window", nullptr, nullptr);
		if (!mWindow) {
			std::cerr << "Error : failed to create window" << std::endl;
		}

		//this会变成一个userPointer（用户对象） ，被塞到mWindow里面的一个变量（mWindow里面有一个变量是专门给我们用来传数据的）
		glfwSetWindowUserPointer(mWindow, this);
		//传递回调函数
		glfwSetFramebufferSizeCallback(mWindow, windowResized); 
	}
	Window::~Window() {
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	bool Window::shouldClose() {
		return glfwWindowShouldClose(mWindow);
	}

	void Window::pollEvents() {
		glfwPollEvents();
	}
}