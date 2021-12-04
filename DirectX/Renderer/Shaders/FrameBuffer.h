#include "HLSLGlue.h"
#ifndef __DEPENDENCY_HLSL__
#define __DEPENDENCY_HLSL__


#if CPP
#pragma once
#endif


//Whole Frame buffer
START_CBUFFER(FrameBuffer, b0)
float4 CameraDirection;
float4 CameraPosition;
float4x4 View;
float4x4 Projection;
float4x4 InverseView;
float4x4 InverseProjection;
#if CPP
bool bIsUpToDate = false;
bool IsUpToDate() { return bIsUpToDate; }
#endif
END_CBUFFER(FrameBuffer);


#endif // __DEPENDENCY_HLSL__
