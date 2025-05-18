#include "Camera.h"

//获取视图观察矩阵
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, WorldUp);
}

//更新摄像机角度
void Camera::UpdataCameraVectors()
{
	Forward.x = glm::cos(Pitch) * glm::sin(Yaw);
	Forward.y = glm::sin(Pitch);
	Forward.z = glm::cos(Pitch) * glm::cos(Yaw);
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}

//鼠标移动
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

//更新摄像机位置
void Camera::UpdataCameraPosition()
{
	//Position += glm::vec3(speedX, speedY,-speedZ) * 0.3f;
	Position += Forward * speedZ * 0.001f + Right * speedX * 0.001f + Up * speedY * 0.001f;
}
