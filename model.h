#pragma once
#include "base.h"
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/device.h"
namespace FF {
	struct Vertex
	{
		glm::vec3 mPosition;
		glm::vec3 mColor;
	};

	class Model
	{
	public:
		using Ptr = std::shared_ptr<Model>;
		static Ptr create() { return std::make_shared<Model>(); }

		Model(const Wrapper::Device::Ptr &device) {
			mDatas = {
				{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, //前三个是坐标，后三个是颜色
				{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f, 0.0f},{0.0f, 0.0f, 1.0f}}
			};
			//因为设置了  mPipeline->mRasterstate.frontFace = VK_FRONT_FACE_CLOCKWISE; // 顺时针为正面
			mIndexDatas = { 0, 1, 2 };

			mVertexBuffer = Wrapper::Buffer::createVertexBuffer(device, mDatas.size(), mDatas.data());
			mIndexBuffer = Wrapper::Buffer::createIndexBuffer(device, mIndexDatas.size(), mIndexDatas.data());
		}
		~Model(){}

		//顶点数组buffer相关的信息
		std::vector<VkVertexInputBindingDescription> getVertexInputBindingDescriptions() {
			std::vector<VkVertexInputBindingDescription> bingingDes{};
			bingingDes.resize(1);
			bingingDes[0].binding = 0;
			bingingDes[0].stride = sizeof(Vertex);
			bingingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //表示逐顶点处理数据, 区别于 VK_VERTEX_INPUT_RATE_INSTANCE
			return bingingDes;
		}

		//attribute相关信息，与vertexShader里面的location相关
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDes{};
			attributeDes.resize(2);

			attributeDes[0].binding = 0;
			attributeDes[0].location = 0;
			attributeDes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDes[0].offset = offsetof(Vertex, mPosition);

			attributeDes[1].binding = 0;
			attributeDes[1].location = 1;
			attributeDes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDes[1].offset = offsetof(Vertex, mColor);

			return attributeDes;
		}
		
		[[nodiscard]] auto getVertexBuffer() { return mVertexBuffer; }
		[[nodiscard]] auto getIndexBuffer() { return mIndexBuffer; }

	private:
		std::vector<Vertex> mDatas{};
		std::vector<int> mIndexDatas{};

		Wrapper::Device::Ptr mDevice{ nullptr };

		Wrapper::Buffer::Ptr mVertexBuffer{ nullptr };
		Wrapper::Buffer::Ptr mIndexBuffer{ nullptr };
	};
}