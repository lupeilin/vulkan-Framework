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

	//��ȡ�۲����
	glm::mat4 GetViewMatrix();
	//����ƶ�
	void ProcessMouseMovement(float deltaX, float deltaY);
	//���������λ��
	void UpdataCameraPosition();

private:
	//����������Ƕ�
	void UpdataCameraVectors();

	glm::vec3 m_position;//�����λ��
	glm::vec3 m_forward;//���������
	glm::vec3 m_up;//����
	glm::vec3 m_speed;

	float m_pitch;//ŷ��_������
	float m_yaw;//ŷ��_ƫ����
	float m_sensitivity;

	float m_xpos;
	float m_ypos;
	float m_firstMove;

	glm::mat4 m_vMatrix;
	glm::mat4 m_pMatrix;
};


