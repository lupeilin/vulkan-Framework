#include"window.h"

namespace FF::Wrapper {
	//���ô��ڴ�С�仯�Ļص�����
	static void windowResized(GLFWwindow* window, int width, int height) {
		//��glfwSetWindowUserPointer(mWindow, this);�д����thisָ���ó����������½��͵�ǰָ����ָ����ڴ��
		auto pUserData = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pUserData->mWindowResized = true;
	}


	Window::Window(const int& width, const int& height) {
		mWidth = width;
		mHeight = height;
		glfwInit();

		//���û������ص�opengl API ���ҽ�ֹ���ڸı��С
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		mWindow = glfwCreateWindow(mWidth, mHeight, "vulkan window", nullptr, nullptr);
		if (!mWindow) {
			std::cerr << "Error : failed to create window" << std::endl;
		}

		//this����һ��userPointer���û����� ��������mWindow�����һ��������mWindow������һ��������ר�Ÿ��������������ݵģ�
		glfwSetWindowUserPointer(mWindow, this);
		//���ݻص�����
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