#include "Camera.h"

void Camera::Initialize()
{
    Camera &camera = Instance();

    CalcViewMatrix();
    CalcProjMatrix();
}

void Camera::CalcViewMatrix()
{
    Camera &camera = Instance();
    camera.m_ViewMatrix = glm::lookAt(camera.m_Position, camera.m_Position + camera.m_ForwardVector, camera.POSITIVE_Y);
}

void Camera::CalcProjMatrix()
{
    Camera &camera = Instance();
    camera.m_ProjMatrix = glm::perspective(camera.m_FOV, camera.m_Aspect, camera.m_Near, camera.m_Far);
}

void Camera::Update()
{
    CalcViewMatrix();
}
