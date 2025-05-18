#pragma once
#include "base.h"

enum class CAMERA_MOVE { 
	MOVE_FORWARD, 
	MOVE_BACKWARD,  
	MOVE_LEFT, 
	MOVE_RIGHT 
};

class Camera
{
public:
	
	Camera() {
		m_position = glm::vec3(1.0f);
	}

	~Camera(){
	
	}

	//获取观察矩阵
	glm::mat4 GetViewMatrix();
	//鼠标移动
	void ProcessMouseMovement(float deltaX, float deltaY);
	//更新摄像机位置
	void UpdataCameraPosition();

private:
	//更新摄像机角度
	void UpdataCameraVectors();

	glm::vec3 m_position;//摄像机位置
	glm::vec3 m_forward;//摄像机方向
	glm::vec3 m_up;//上轴
	glm::vec3 m_speed;

	float m_pitch;//欧拉_俯仰角
	float m_yaw;//欧拉_偏航角
	float m_sensitivity;

	float m_xpos;
	float m_ypos;
	float m_firstMove;

	glm::mat4 m_vMatrix;
	glm::mat4 m_pMatrix;
};


