#pragma once
#include "Build.h"
#include "D3DDefines.h"

class CCamera
{
public:
	CCamera();

	//checks input
	void SetStartingPosition(float3 offset);
	void SetStartingPosition(float ox, float oy, float oz) { SetStartingPosition(float3(ox, oy, oz)); };
	void Update(float delta);
	float m_pitch, m_yaw;
	vector m_pos = {};
	float4x4 m_cameramatrix;
	//move speed
	float m_ms = 15.f;
};