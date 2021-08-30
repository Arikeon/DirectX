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

		static void LoadAsset(CRenderer* renderer, std::string assetDir, std::string assetName, TObject& Object);

		static void CreateCube(CRenderer* renderer,
			TObject& Object,
			float size,
			int32 TextureID);

		static void CreateSphere(CRenderer* renderer,
			TObject& Object,
			float lat,
			float lon,
			float scale,
			int32 TextureID);

		static TextureID LoadTextureFromFile(CRenderer* renderer,
			std::string assetDir,
			std::string assetName);

	private:
		static TextureID LoadTextureFromFile(
			CRenderer* renderer,
			struct currAIMaterial* material,
			std::string assetDir,
			aiString dir);

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
	};
}