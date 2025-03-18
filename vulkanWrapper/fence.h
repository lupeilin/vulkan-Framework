#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	/*
	fence�ǿ���һ�ζ����ύ�ı�־����semaphore������
		semaphore���Ƶ�һ�����ύ��Ϣ�ڵĲ�ִͬ�н׶�֮���������ϵ��semaphore�޷��ֶ���apiȥ������
		fence����һ�����У����� GraphicQueue������һ�����ύ������ָ��ִ����ϣ���Ϊ����̬ �� �Ǽ���̬�����ҿ��Խ���api����Ŀ��ơ�

	
	*/
	class Fence
	{
	public:
		using Ptr = std::shared_ptr<Fence>;
		static Ptr create(const Device::Ptr& device, bool signaled = true) {
			return std::make_shared<Fence>(device, signaled); 
		}

		Fence(const Device::Ptr& device, bool signaled = true);
		~Fence();

		//��Ϊ�Ǽ���̬
		void resetFence();

		//���ô˺��������fenceû�б���������ô������������ȴ�����
		void block(uint64_t timeout = UINT64_MAX);

		[[nodiscard]] auto getFence() const { return mFence; }
	private:
		VkFence mFence{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}