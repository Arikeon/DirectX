#pragma once
#include "Build.h"
#include "Window.h"
#include <Windows.h>

class CD3DInterface;

class CRenderer
{
	friend class CEngine;
private:
	void Initialize(TWindow window);
	void Update(float delta);

	CD3DInterface* m_D3DInterface;
};