#include "Camera.h"
#include "Input.h"

CCamera::CCamera() :
	m_pitch(0.f),
	m_yaw(0.f),
	m_cameramatrix(float4x4{})
{
	matrix view = XMMatrixLookToLH(XMVectorSet(0, 0, -5, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&m_cameramatrix, view);
}

void CCamera::Update(float delta)
{
	//Camera rotation
	float2 mousedelta = InputInstance()->GetMouseDelta();
	m_pitch += cos(mousedelta.y) * delta;
	m_yaw	+= sin(mousedelta.x) * delta;

	float XPos = m_cameramatrix._41;
	float YPos = m_cameramatrix._42;
	float ZPos = m_cameramatrix._43;
	m_cameramatrix._41 = m_cameramatrix._42 = m_cameramatrix._43 = 0.f;

	//Camera Translation
	const float deltaTS = m_ms * delta;
	if (InputIsKeyDown('W'))
		ZPos -= deltaTS;
	if (InputIsKeyDown('S'))
		ZPos += deltaTS;
	
	if (InputIsKeyDown('D'))
		XPos -= deltaTS;
	if (InputIsKeyDown('A'))
		XPos += deltaTS;

	if (InputIsKeyDown(VK_SPACE))
		YPos -= deltaTS;
	if (InputIsKeyDown(VK_CONTROL))
		YPos += deltaTS;

	CONSOLE_LOG(std::to_wstring(m_pitch) + L", " + std::to_wstring(m_yaw));

	vector EyePos = { XPos, YPos, ZPos, 0.f };
	vector Dir = { m_pitch, m_yaw, 0.f, 0.f };
	const vector up = { 0.f, 1.f, 0.f, 0.f };

	XMStoreFloat4x4(&m_cameramatrix, XMMatrixLookToLH(EyePos, Dir, up));
}