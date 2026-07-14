#include "ViewCamera.h"

#include <glm/ext.hpp>

namespace Comet
{

void ViewCamera::Initialize()
{
    Get();
    CalcViewMatrix();
    CalcProjMatrix();
}

void ViewCamera::Update()
{
    Get().m_Aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    CalcViewMatrix();
    CalcProjMatrix();
}

void ViewCamera::CalcViewMatrix()
{
    Get().m_ViewMatrix = glm::lookAt(Get().m_Position, Get().m_Position + Get().m_ForwardVector, Get().POSITIVE_Y);
}

void ViewCamera::CalcProjMatrix()
{
    Get().m_ProjMatrix = glm::perspective(Get().m_FOV, Get().m_Aspect, Get().m_Near, Get().m_Far);
}

::Camera3D ViewCamera::GetRaylibCamera()
{
    auto& self = Get();
    ::Camera3D cam = {0};
    cam.position = {self.m_Position.x, self.m_Position.y, self.m_Position.z};
    cam.target = {self.m_Position.x + self.m_ForwardVector.x, self.m_Position.y + self.m_ForwardVector.y, self.m_Position.z + self.m_ForwardVector.z};
    cam.up = {0.0f, 1.0f, 0.0f};
    cam.fovy = glm::degrees(self.m_FOV);
    cam.projection = CAMERA_PERSPECTIVE;
    return cam;
}

}