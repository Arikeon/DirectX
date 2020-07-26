#pragma once
#include "Window.h"
#include <Window.h>

class CD3DInterface
{
public:
	virtual void InitializeD3D(TWindow window) = 0;
};