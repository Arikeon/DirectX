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

		LoadRootNode(renderer, objectArray, scene);
	}


	void TFileIO::LoadRootNode(CRenderer* renderer, std::vector<TObject>& objectArray, const aiScene* scene)
	{
		aiMesh** meshRoot = scene->mMeshes;
		aiMaterial** materialRoot = scene->mMaterials;
		aiNode* nodeRoot = scene->mRootNode;
		aiTexture** textureRoot = scene->mTextures;

		float4x4 objectTransform;
		memcpy(&objectTransform, &nodeRoot->mTransformation, sizeof(float4x4));

		TObject object;
		object.CopyFromNode(nodeRoot);

		//TODO this does NOT count the total number of meshes
		const unsigned int numMesh = nodeRoot->mNumMeshes;
		const unsigned int numChildren = nodeRoot->mNumChildren;
		unsigned int meshIndex = 0;
		unsigned int childIndex = 0;

		aiNode** rootNodeChild = nodeRoot->mChildren;

		LoadMesh(renderer,
			object,
			rootNodeChild,
			meshRoot,
			numMesh,
			numChildren,
			meshIndex,
			childIndex);

		objectArray.push_back(object);
	}

	void TFileIO::LoadMesh(
		CRenderer* renderer,
		TObject& object,
		aiNode** nodeChildRoot,
		aiMesh** meshRoot,
		const unsigned int meshCount,
		const unsigned int childCount,
		unsigned int& meshIndex,
		unsigned int& childIndex)
	{
		//TODO does not account for a node having more than 1 mesh...

		if (childIndex >= childCount)
			return;

		//Where we started in the mesh array for this node
		aiNode* currChild = nodeChildRoot[childIndex++];

		if (currChild->mNumMeshes > 0)
		{
			TModel model;
			model.CopyFromNode(currChild);
			model.m_meshes.resize(currChild->mNumMeshes);

			unsigned int tMeshIndex = 0;
			const unsigned int meshStart = meshIndex;
			for (; meshIndex < meshStart + currChild->mNumMeshes;)
			{
				aiMesh* currAIMesh = meshRoot[meshIndex++];

				TMesh& currMesh = model.m_meshes[tMeshIndex];
				currMesh.m_vertexLayout = TVertexLayout{
					currAIMesh->HasPositions(),
					currAIMesh->HasNormals(),
					currAIMesh->HasTangentsAndBitangents(),
					currAIMesh->HasTextureCoords(meshIndex - 1), //since we incremented earlier
				};

				//TODO add support for other verticies
#if ENABLE_DEBUG
				check(
					sizeof(BasePassInVS) ==
					sizeof(aiVector3D) +	//Positions
					sizeof(aiVector3D) +	//Normals
					sizeof(aiVector3D) +	//Tangents
					sizeof(aiVector3D) +	//BiTangents
					sizeof(aiVector2D)		//UVs
				);
#endif

				std::vector<BasePassInVS> vertexArray;
				std::vector<unsigned int> indexArray;

				vertexArray.resize(currAIMesh->mNumVertices);
				for (unsigned int vertexIndex = 0; vertexIndex < (unsigned int)vertexArray.size(); ++vertexIndex)
				{
					BasePassInVS& currVertex = vertexArray[vertexIndex];

					check(currAIMesh->HasPositions());
					memcpy(&currVertex.position, &currAIMesh->mVertices[vertexIndex], sizeof(aiVector3D));

					if (currAIMesh->HasNormals())
					{
						memcpy(&currVertex.normal, &currAIMesh->mNormals[vertexIndex], sizeof(aiVector3D));
					}

					if (currAIMesh->HasTangentsAndBitangents())
					{
						memcpy(&currVertex.tangents, &currAIMesh->mTangents[vertexIndex], sizeof(aiVector3D));
						memcpy(&currVertex.bitangents, &currAIMesh->mBitangents[vertexIndex], sizeof(aiVector3D));
					}

					if (currAIMesh->HasTextureCoords(meshIndex - 1))
					{
						//mTextureCoords is saved as aiVector3D - convert to float2
						currVertex.texcoord =
							currAIMesh->mTextureCoords[meshIndex - 1] ?
							float2(currAIMesh->mTextureCoords[meshIndex - 1]->x, currAIMesh->mTextureCoords[meshIndex - 1]->y) :
							float2(0, 0);
					}
				}

				indexArray.resize(currAIMesh->mNumFaces * 3);
				for (unsigned int faceIndex = 0; faceIndex < currAIMesh->mNumFaces; ++faceIndex)
				{
					memcpy(&indexArray[faceIndex * 3], currAIMesh->mFaces[faceIndex].mIndices, sizeof(unsigned int) * 3);
				}

				currMesh.CreateMesh<BasePassInVS>(renderer, vertexArray, indexArray);
				check(currMesh.m_vertexkey != -1 && currMesh.m_indexkey != -1);
				object.m_models.push_back(model);
			}
		}

		LoadMesh(renderer,
			object,
			nodeChildRoot,
			meshRoot,
			meshCount,
			childCount,
			meshIndex,
			childIndex);
	}
}