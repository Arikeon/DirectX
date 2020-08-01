#pragma once
#include "Build.h"
#include "D3DDefines.h"

class CCamera
{
public:
	CCamera();

	//checks input
	void Update(float delta);
	float m_pitch, m_yaw;
	float4x4 m_cameramatrix;
	//move speed
	float m_ms = 3.0f;
};