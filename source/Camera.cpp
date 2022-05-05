#include "Camera.h"

void Camera::Initialize()
{
    Get();

    CalcViewMatrix();
    CalcProjMatrix();
}

void Camera::Update() { CalcViewMatrix(); }

void Camera::CalcViewMatrix()
{
    Get().m_ViewMatrix = glm::lookAt(Get().m_Position, Get().m_Position + Get().m_ForwardVector, Get().POSITIVE_Y);
}

void Camera::CalcProjMatrix()
{
    Get().m_ProjMatrix = glm::perspective(Get().m_FOV, Get().m_Aspect, Get().m_Near, Get().m_Far);
}
