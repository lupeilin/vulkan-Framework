#pragma once

#include "../base.h"
#include "device.h"

namespace FF::Wrapper {
	/*
	1��attachment
	vkAttachmentDescription ����һ����ɫ�������ģ�帽�ŵ������ṹ����������һ�������ĸ��ţ�����һ������
	vkAttachmentDescription1 vkAttanchmentDescription2 vkAttanchmentDescription3 vkAttanchmentDescription4�����ģ�壩�����������Ҫʲô
	vkAttachmentRefrence ˵������subpass  ��Ҫ��attachment����һ������һ������������id�Ƕ��٣���һ�����ŵ�ͼƬ������ʽ��ʲô
	VkSubpassDescription ������дһ����pass�������ṹ
	vkSubpassDependency  ������ͬ��������֮���������ϵ
	*/

	class SubPass
	{
	public:

		SubPass();
		~SubPass();

		void addColorAttachmentRefrence(const VkAttachmentReference& ref);

		void addInputAttachmentRefrence(const VkAttachmentReference& ref);

		void addDepthStencilAttachmentRefrence(const VkAttachmentReference& ref);

		void setResolvedAttachmentRefrence(const VkAttachmentReference& ref);

		void buildSubpassDescription();

		[[nodiscard]] auto getSubPassDescription() const { return mSubPassDescription; }

	private:
		VkSubpassDescription mSubPassDescription{};
		std::vector<VkAttachmentReference> mColorAttachmentRefrences{};
		std::vector<VkAttachmentReference> mInputAttachmentRefrences{};
		VkAttachmentReference mDepthStencilAttachmentRefrence{};
		VkAttachmentReference mResolvedAttachmentRefrence{};
	};

	class RenderPass
	{
	public:
		using Ptr = std::shared_ptr<RenderPass>;

		static Ptr create(const Device::Ptr& device) { return std::make_shared<RenderPass>(device); }

		RenderPass(const Device::Ptr &device);
		~RenderPass();

		void addSubPass(const SubPass& subpass);

		void addDependency(const VkSubpassDependency& dependency);

		void addAttchment(const VkAttachmentDescription& attachmentDes);

		void buildRenderPass();

	public:
		[[nodiscard]] auto getRenderPass() const { return mRenderPass; }

	private:
		VkRenderPass mRenderPass{ VK_NULL_HANDLE };

		std::vector<SubPass> mSubPasses{};
		std::vector<VkSubpassDependency> mDependencies{};
		std::vector <VkAttachmentDescription> mAttachmentDescriptions{};

		Device::Ptr mDevice{ nullptr };
	};
}