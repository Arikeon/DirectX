#include "Camera.h"
#include "Input.h"

CCamera::CCamera() :
	m_pitch(0.f),
	m_yaw(0.f),
	m_cameramatrix(float4x4{})
{
	matrix view = XMMatrixLookAtLH(XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 0, -5, 0), XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&m_cameramatrix, view);
}

void CCamera::Update(float delta)
{
	//Camera rotation
	float2 mousedelta = InputInstance()->GetMouseDelta();
	m_pitch -= mousedelta.y * delta;
	m_yaw	-= mousedelta.x * delta;

	if (m_pitch > 90)
		m_pitch = 90;
	if (m_pitch < -90)
		m_pitch = -90;
	if (m_yaw > 360)
		m_yaw = 0;
	if (m_yaw < 0)
		m_yaw = 360;

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

	matrix currmatrix = XMLoadFloat4x4(&m_cameramatrix);

	const vector defaultRight = { 1, 0, 0, 0 };
	const vector defaultUp = { 0, 1, 0, 0 };
	const vector defaultForward = { 0, 0, 1, 0 };
	vector position, target, camRight, camForward;


	matrix rotation = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f);
	target = XMVector3Normalize(XMVector3TransformCoord(defaultForward, rotation));

	matrix rotateyaw = XMMatrixRotationY(m_yaw);

	camRight = XMVector3TransformCoord(defaultRight, rotateyaw);
	camForward = XMVector3TransformCoord(defaultForward, rotateyaw);

	position = { XPos, YPos, ZPos, 0 };

	target = position + target;

	currmatrix = XMMatrixLookAtLH(position, target, defaultUp);
	XMStoreFloat4x4(&m_cameramatrix, currmatrix);
}