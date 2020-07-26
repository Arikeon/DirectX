#pragma once
#include "Build.h"
#include "Window.h"
#include <Windows.h>

class CRenderer
{
public:
	void Initialize(TWindow window);
	void Update(float delta);
};