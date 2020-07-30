#include "Camera.h"
#include "Input.h"

CCamera::CCamera() :
	m_pitch(0.f),
	m_yaw(0.f),
	m_cameramatrix(float4x4{})
{

}

void CCamera::Update(float delta)
{
	//Camera rotation
	float2 mousedelta = InputInstance()->GetMouseDelta();
	m_pitch += mousedelta.x;
	m_yaw += mousedelta.y;
	XMStoreFloat4x4(&m_cameramatrix, XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f));


	float& XPos = m_cameramatrix._41, & YPos = m_cameramatrix._42, & ZPos = m_cameramatrix._43;

	//Camera Translation
	if (InputIsKeyUp('w')) { ZPos += translationspeed; }
	if (InputIsKeyUp('s')) { ZPos -= translationspeed; }

	if (InputIsKeyUp('d')) { XPos += translationspeed; }
	if (InputIsKeyUp('a')) { XPos -= translationspeed; }

	if (InputIsKeyUp(VK_SPACE)) { YPos += translationspeed; }
	if (InputIsKeyUp(VK_CONTROL)) { YPos -= translationspeed; }
}