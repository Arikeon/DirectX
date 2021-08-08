#include "Camera.h"
#include "Input.h"

CCamera::CCamera() :
	m_pitch(0.f),
	m_yaw(0.f),
	m_cameramatrix(float4x4{})
{
}

void CCamera::SetStartingPosition(float3 offset)
{
	m_pos = XMVectorSet(offset.x, offset.y, offset.z, 0.f);
}

void CCamera::Update(float delta)
{
	//Camera rotation
	float2 mousedelta = InputInstance()->GetMouseDelta();
	m_pitch += mousedelta.y * delta * 3.f;
	m_yaw	+= mousedelta.x * delta * 3.f;

	float XPos = 0.f;
	float YPos = 0.f;
	float ZPos = 0.f;

	const float deltaTS = m_ms * delta;
	if (InputIsKeyDown('W'))
		ZPos += deltaTS;
	if (InputIsKeyDown('S'))
		ZPos -= deltaTS;
	
	if (InputIsKeyDown('D'))
		XPos += deltaTS;
	if (InputIsKeyDown('A'))
		XPos -= deltaTS;

	if (InputIsKeyDown(VK_SPACE))
		YPos += deltaTS;
	if (InputIsKeyDown(VK_CONTROL))
		YPos -= deltaTS;

	const float RAD90 = 89 * DEG2RAD;
	//TODO convert to degrees
	m_pitch = std::fmin(m_pitch, RAD90);
	m_pitch = std::fmax(m_pitch, -RAD90);

	const vector defaultRight = { 1, 0, 0, 0 };
	const vector defaultUp = { 0, 1, 0, 0 };
	const vector defaultForward = { 0, 0, 1, 0 };
	vector vtarget, camRight, camForward, camUp;
	matrix mrollpitchyaw, mrotateyaw;

	mrollpitchyaw = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f);
	vtarget = XMVector3Normalize(XMVector3TransformCoord(defaultForward, mrollpitchyaw));

	mrotateyaw = XMMatrixRotationY(m_yaw);

	camRight = XMVector3TransformCoord(defaultRight, mrotateyaw);
	camForward = XMVector3TransformCoord(defaultForward, mrotateyaw);
	camUp = XMVector3TransformCoord(defaultUp, mrotateyaw);

	m_pos += XMVectorScale(camRight, XPos);
	m_pos += XMVectorScale(camForward, ZPos);
	m_pos += XMVectorScale(camUp, YPos);

	vtarget = m_pos + vtarget;

	XMStoreFloat4x4(&m_cameramatrix, XMMatrixLookAtLH(m_pos, vtarget, defaultUp));
}