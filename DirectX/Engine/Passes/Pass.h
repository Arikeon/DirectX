#pragma once
#include "Build.h"
#include "BufferKeys.h"
#include "Model.h"
#include "Mesh.h"
#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"
#include "Window.h"
#include "D3D11Interface.h"

#include <vector>

struct TPass 
{
	static TObject m_ScreenQuad;
	static void Initialize(CRenderer* renderer)
	{
		const float size = 1.0f;

		std::vector<uint32> indicies;
		indicies.resize(6);
		indicies[0] = 0;
		indicies[1] = 1;
		indicies[2] = 2;

		indicies[3] = 0;
		indicies[4] = 2;
		indicies[5] = 3;


		/**
		1	2
		0	3
		**/

		std::vector<ScreenQuadInVS> verticies;
		verticies.resize(4);
		verticies[0] = ScreenQuadInVS(float3(-size, -size, 0.f), float2(0.f, 1.0f));
		verticies[1] = ScreenQuadInVS(float3(-size, size, 0.f), float2(0.f, 0.0f));
		verticies[2] = ScreenQuadInVS(float3(size, size, 0.f), float2(1.f, 0.0f));
		verticies[3] = ScreenQuadInVS(float3(size, -size, 0.f), float2(1.f, 1.0f));

		TModel ScreenQuadModel = {};
		ScreenQuadModel.m_name = "ScreenQuadModel";

		TMesh newMesh = {};
		newMesh.CreateMesh<ScreenQuadInVS>(renderer, verticies, indicies);
		check(newMesh.m_vertexkey != -1 && newMesh.m_indexkey != -1);

		ScreenQuadModel.m_meshes.push_back(newMesh);
		m_ScreenQuad.m_models.push_back(ScreenQuadModel);
	}

	static TObject& GetScreenQuad() { return m_ScreenQuad; }
};
