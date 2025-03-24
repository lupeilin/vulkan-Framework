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
		static Ptr create(const Wrapper::Device::Ptr& device) { return std::make_shared<Model>(device); }

		Model(const Wrapper::Device::Ptr &device) {
			//mDatas = {
			//	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, //前三个是坐标，后三个是颜色
			//	{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			//	{{-0.5f, 0.5f, 0.0f},{0.0f, 0.0f, 1.0f}}
			//};
			mPositions = {
			0.0f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
			};
			mColors = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
			};

			//因为设置了  mPipeline->mRasterstate.frontFace = VK_FRONT_FACE_CLOCKWISE; // 顺时针为正面
			mIndexDatas = { 0, 1, 2 };

			//mVertexBuffer = Wrapper::Buffer::createVertexBuffer(device, mDatas.size() * sizeof(Vertex), mDatas.data());
			mPositionBuffer = Wrapper::Buffer::createVertexBuffer(device, mPositions.size() * sizeof(Vertex), mPositions.data());
			mColorBuffer = Wrapper::Buffer::createVertexBuffer(device, mColors.size() * sizeof(Vertex), mColors.data());
			mIndexBuffer = Wrapper::Buffer::createIndexBuffer(device, mIndexDatas.size() * sizeof(unsigned int), mIndexDatas.data());
			int a = 1;
		}
		~Model(){}

		//顶点数组buffer相关的信息, 在createPipeline中调用
		std::vector<VkVertexInputBindingDescription> getVertexInputBindingDescriptions() {
			std::vector<VkVertexInputBindingDescription> bingingDes{};
			//bingingDes.resize(1);
			//bingingDes[0].binding = 0;
			//bingingDes[0].stride = sizeof(Vertex);
			//bingingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //表示逐顶点处理数据, 区别于 VK_VERTEX_INPUT_RATE_INSTANCE

			bingingDes.resize(2);
			bingingDes[0].binding = 0;
			bingingDes[0].stride = sizeof(float) * 3;
			bingingDes[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //表示逐顶点处理数据, 区别于 VK_VERTEX_INPUT_RATE_INSTANCE

			bingingDes[1].binding = 1;
			bingingDes[1].stride = sizeof(float) * 3;
			bingingDes[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //表示逐顶点处理数据, 区别于 VK_VERTEX_INPUT_RATE_INSTANCE
			return bingingDes;
		}

		//attribute相关信息，与vertexShader里面的location相关，, 在createPipeline中调用
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDes{};
			attributeDes.resize(2);

			attributeDes[0].binding = 0;
			attributeDes[0].location = 0;
			attributeDes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			//attributeDes[0].offset = offsetof(Vertex, mPosition);
			attributeDes[0].offset = 0;

			// attributeDes[1].binding = 0;
			attributeDes[1].binding = 1;
			attributeDes[1].location = 1;
			attributeDes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			//attributeDes[1].offset = offsetof(Vertex, mColor);
			attributeDes[1].offset = 0;

			return attributeDes;
		}
		
		//[[nodiscard]] auto getVertexBuffer() { return mVertexBuffer; }
		[[nodiscard]] auto getVertexBuffers() { 
			std::vector<VkBuffer> buffers{ mPositionBuffer->getBuffer(), mColorBuffer->getBuffer()};
			return buffers;
		}
		[[nodiscard]] auto getIndexBuffer() { return mIndexBuffer; }

		[[nodiscard]] auto getIndexCount() { return mIndexDatas.size(); }

		[[nodiscard]] auto getUniform() { return mUniform; }

		void setModelMatrix(const glm::mat4 matrix) { mUniform.mModelMatrix = matrix;  }

	private:
		//std::vector<Vertex> mDatas{};
		std::vector<float> mPositions{};
		std::vector<float> mColors{};
		std::vector<unsigned int> mIndexDatas{};

		Wrapper::Device::Ptr mDevice{ nullptr };

		// Wrapper::Buffer::Ptr mVertexBuffer{ nullptr };
		Wrapper::Buffer::Ptr mPositionBuffer{ nullptr };
		Wrapper::Buffer::Ptr mColorBuffer{ nullptr };
		Wrapper::Buffer::Ptr mIndexBuffer{ nullptr };

		ObjectUniform mUniform{};
	};
}