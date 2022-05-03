#include "Camera.h"

Camera::Camera()
{
    CalcViewMatrix();
    CalcProjMatrix();
}

Camera::~Camera() {}

void Camera::CalcViewMatrix() { m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ForwardVector, POSITIVE_Y); }

void Camera::CalcProjMatrix() { m_ProjMatrix = glm::perspective(m_FOV, m_Aspect, m_Near, m_Far); }

void Camera::Update() { CalcViewMatrix(); }
