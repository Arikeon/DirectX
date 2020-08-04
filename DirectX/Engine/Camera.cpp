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

	m_pitch = min(m_pitch, RAD90);
	m_pitch = max(m_pitch, -RAD90);

	const vector defaultRight = { 1, 0, 0, 0 };
	const vector defaultUp = { 0, 1, 0, 0 };
	const vector defaultForward = { 0, 0, 1, 0 };
	static vector vposition = {};
	vector vtarget, camRight, camForward, camUp;
	matrix mrollpitchyaw, mrotateyaw;

	mrollpitchyaw = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f);
	vtarget = XMVector3Normalize(XMVector3TransformCoord(defaultForward, mrollpitchyaw));

	mrotateyaw = XMMatrixRotationY(m_yaw);

	camRight = XMVector3TransformCoord(defaultRight, mrotateyaw);
	camForward = XMVector3TransformCoord(defaultForward, mrotateyaw);
	camUp = XMVector3TransformCoord(defaultUp, mrotateyaw);

	vposition += XMVectorScale(camRight, XPos);
	vposition += XMVectorScale(camForward, ZPos);
	vposition += XMVectorScale(camUp, YPos);

	vtarget = vposition + vtarget;

	XMStoreFloat4x4(&m_cameramatrix, XMMatrixLookAtLH(vposition, vtarget, defaultUp));
}