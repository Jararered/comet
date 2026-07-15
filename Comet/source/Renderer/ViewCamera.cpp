#include "ViewCamera.h"

#include <glm/ext.hpp>

namespace Comet
{

void ViewCamera::Initialize()
{
    CalcViewMatrix();
    CalcProjMatrix();
}

void ViewCamera::Update()
{
    m_Aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    CalcViewMatrix();
    CalcProjMatrix();
}

void ViewCamera::CalcViewMatrix()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ForwardVector, POSITIVE_Y);
}

void ViewCamera::CalcProjMatrix()
{
    m_ProjMatrix = glm::perspective(m_FOV, m_Aspect, m_Near, m_Far);
}

::Camera3D ViewCamera::GetRaylibCamera() const
{
    ::Camera3D cam = {0};
    cam.position = {m_Position.x, m_Position.y, m_Position.z};
    cam.target = {m_Position.x + m_ForwardVector.x, m_Position.y + m_ForwardVector.y, m_Position.z + m_ForwardVector.z};
    cam.up = {0.0f, 1.0f, 0.0f};
    cam.fovy = glm::degrees(m_FOV);
    cam.projection = CAMERA_PERSPECTIVE;
    return cam;
}

}
