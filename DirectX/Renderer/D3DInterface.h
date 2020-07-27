#pragma once
#include "Build.h"
#include "Window.h"
#include <Window.h>
#include <d3d11.h>
#include <dxgi.h>

class CD3DInterface
{
public:
	virtual ~CD3DInterface() {};
	virtual void InitializeD3D(TWindow window) = 0;
};