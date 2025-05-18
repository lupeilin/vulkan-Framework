#include "Camera.h"

//��ȡ��ͼ�۲����
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, WorldUp);
}

//����������Ƕ�
void Camera::UpdataCameraVectors()
{
	Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}

//����ƶ�
void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
	Pitch += deltaY * SenceX;
	Yaw += deltaX * SenceY;
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;
	UpdataCameraVectors();
}

//���������λ��
void Camera::UpdataCameraPosition()
{
	//Position += glm::vec3(speedX, speedY,-speedZ) * 0.3f;
	Position += Forward * speedZ * 0.001f + Right * speedX * 0.001f + Up * speedY * 0.001f;
}
