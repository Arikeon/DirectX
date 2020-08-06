#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"

class CRenderer;
struct TObject;
struct aiScene;
struct aiNode;
struct aiMaterial;
struct aiMesh;

namespace IO
{
	struct TFileIO
	{
	public:
		static void LoadAsset(CRenderer* renderer, std::vector<TObject>& objectArray, std::string Dir);
	private:
		static void LoadRootNode(CRenderer* renderer, std::vector<TObject>& objectArray, const aiScene* scene);
		//recursion
		static void LoadMesh(
			CRenderer* renderer,
			TObject& object,
			aiNode** nodeChildRoot,
			aiMesh** meshRoot,
			const unsigned int meshCount,
			const unsigned int childCount,
			unsigned int& meshIndex,
			unsigned int& childIndex);
	};
}