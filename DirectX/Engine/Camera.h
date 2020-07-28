#pragma once
#include "Build.h"
#include "D3DDefines.h"

class CCamera
{
public:
	CCamera();

	//checks input
	void Update(float delta);
	float m_pitch, yaw;
	float4x4 m_cameramatrix;
};