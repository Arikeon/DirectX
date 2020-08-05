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
		static void LoadScene(CRenderer* rendere, std::vector<TObject>& objectArray, const aiScene* scene);

		static void LoadNode(CRenderer* renderer, std::vector<TObject>& objectArray, aiNode* node, const aiScene* scene);
		//recursion
		static void LoadMesh(CRenderer* renderer, TModel& model, aiNode* node);
	};
}