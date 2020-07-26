#pragma once
#include "D3DInterface.h"
#include "Window.h"
#include <Window.h>

class CD3D11Interface : public CD3DInterface
{
public:
	CD3D11Interface();
	~CD3D11Interface();
	void InitializeD3D(TWindow window);
};