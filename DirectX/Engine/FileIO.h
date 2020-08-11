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
struct aiString;

namespace IO
{
	struct TFileIO
	{
	public:
		static void DebugOutTexture(CRenderer* renderer, ID3D11Texture2D* texture, std::wstring name);

		static void LoadAsset(CRenderer* renderer, std::vector<TObject>& objectArray, std::string assetDir);
	private:
		static void LoadRootNode(CRenderer* renderer, std::vector<TObject>& objectArray, std::string assetDir, const aiScene* scene);
		//recursion
		static void LoadNode(
			CRenderer* renderer,
			TObject& object,
			std::string assetDir,
			aiNode** nodeChildRoot,
			aiMesh** meshRoot,
			aiMaterial** materialRoot,
			const unsigned int childCount,
			unsigned int& meshIndex,
			unsigned int& childIndex);

		static MaterialID LoadMaterial(
			CRenderer* renderer,
			std::string assetDir,
			aiNode* nodeChild,
			aiMesh* meshChild,
			aiMaterial** materialRoot
		);

		static TextureID LoadTextureFromFile(CRenderer* renderer, std::string assetDir,aiString dir);
	};
}