#include "FileIO.h"
#include "Algorithm.h"
#include "BufferStruct.h"
#include "Object.h"

#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>


//http://www.suodenjoki.dk/us/archive/2010/min-max.htm
#define NOMINMAX
//min max compile errors - undef them
//{
	#ifndef max
	#define max(a,b) (((a) > (b)) ? (a) : (b))
	#endif
	#ifndef min
	#define min(a,b) (((a) < (b)) ? (a) : (b))
	#endif
	#undef max
	#undef min
//}

//Assimp
#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\mesh.h>

//Vertex type
#include "DebugLinesStructs.h"
#include "BasePassStructs.h"

using namespace Assimp;

namespace IO
{
	void TFileIO::LoadAsset(CRenderer* renderer, std::vector<TObject>& objectArray, std::string Dir)
	{
		const unsigned assimpFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_MakeLeftHanded |
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_LimitBoneWeights |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder;

		std::wstring wAssetDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Assets\\";
		std::string FullDir = Algorithm::wstring_to_string(wAssetDir) + Dir;

		Importer importer;
		const aiScene* scene = importer.ReadFile(FullDir, assimpFlags);

		if (scene == nullptr || scene->mRootNode == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			CONSOLE_LOG(L"[ERROR] Unable to open file: [" + Algorithm::string_to_wstring(FullDir) + L"]");
			check(0);
		}

		LoadScene(renderer, objectArray, scene);
	}

	void TFileIO::LoadScene(CRenderer* renderer, std::vector<TObject>& objectArray, const aiScene* scene)
	{
		LoadNode(renderer, objectArray, scene->mRootNode, scene);
	}

	void TFileIO::LoadNode(CRenderer* renderer, std::vector<TObject>& objectArray, aiNode* node, const aiScene* scene)
	{

		aiMesh** meshRoot = scene->mMeshes;
		aiMaterial** materialRoot = scene->mMaterials;
		aiNode* nodeRoot = scene->mRootNode;
		aiTexture** textureRoot = scene->mTextures;

		TModel model;
		model.CopyFromNode(node); //initialize

		const unsigned int numMesh = node->mNumMeshes;
		for (unsigned int i = 0; i < numMesh; ++i)
		{
			aiNode* currNodeChild = node->mChildren[i];
			LoadMesh(renderer, model, currNodeChild);
		}
	}

	void TFileIO::LoadMesh(CRenderer* renderer, TModel& model, aiNode* node)
	{

	}
}