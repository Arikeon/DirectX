#include "Camera.h"
#include "Input.h"

CCamera::CCamera() :
	m_pitch(0.f),
	m_yaw(0.f),
	m_cameramatrix(float4x4{})
{
	matrix view = XMMatrixLookAtLH(XMVectorSet(0, 3, -6, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&m_cameramatrix, view);
}

void CCamera::Update(float delta)
{
	//Camera rotation
	//float2 mousedelta = InputInstance()->GetMouseDelta();
	//m_pitch += mousedelta.x * (XM_PI / 180) * delta;
	//m_yaw += mousedelta.y * (XM_PI / 180) * delta;
	float XPos = m_cameramatrix._41, YPos = m_cameramatrix._42, ZPos = m_cameramatrix._43;
	//XMStoreFloat4x4(&m_cameramatrix, XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f));

	//Camera Translation
	if (InputIsKeyDown('W'))
	{
		ZPos += translationspeed;
	}
	if (InputIsKeyDown('S'))
	{
		ZPos -= translationspeed;
	}

	if (InputIsKeyDown('D'))
	{
		XPos += translationspeed;
	}
	if (InputIsKeyDown('A'))
	{
		XPos -= translationspeed;
	}

	if (InputIsKeyDown(VK_SPACE))
	{
		YPos += translationspeed;
		CONSOLE_LOG(std::to_wstring(YPos));

	}
	if (InputIsKeyDown(VK_CONTROL))
	{
		YPos -= translationspeed;
	}

	m_cameramatrix._41 = XPos;
	m_cameramatrix._42 = YPos;
	m_cameramatrix._43 = ZPos;
}