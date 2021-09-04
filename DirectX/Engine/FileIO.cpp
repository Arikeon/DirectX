#include "FileIO.h"
#include "Algorithm.h"
#include "BufferStruct.h"
#include "Object.h"
#include "Colors.h"

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
#include <assimp/pbrmaterial.h>

//DirectXTex
#include <DirectXTex.h>
#include <WICTextureLoader.h>
#include <ScreenGrab.h>

//Vertex type
#include "StructCollection.h"

using namespace Assimp;

namespace IO
{
	void TFileIO::DebugOutTexture(CRenderer* renderer, ID3D11Texture2D* texture, std::wstring name)
	{
		//Link ScreenGrab11.h
		//ID3D11DeviceContext* context = renderer->m_D3DInterface->m_context;
		//const std::wstring wAssetDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DebugTextures\\" + name.c_str() + L".dds";
		//DirectX::SaveDDSTextureToFile(context, texture, wAssetDir.c_str());

	}

	void TFileIO::LoadAsset(CRenderer* renderer, std::string assetDir, std::string assetName, TObject& Object)
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

		const std::wstring wAssetDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Assets\\";
		std::string FullDir = Algorithm::wstring_to_string(wAssetDir) + assetDir + "\\" + assetName;

		Importer importer;
		const aiScene* scene = importer.ReadFile(FullDir, assimpFlags);

		if (scene == nullptr || scene->mRootNode == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			CONSOLE_LOG(L"[ERROR] Unable to open file: [" + Algorithm::string_to_wstring(FullDir) + L"]");
			check(0);
		}


		aiNode* nodeRoot = scene->mRootNode;
		aiMesh** meshRoot = scene->mMeshes;
		aiMaterial** materialRoot = scene->mMaterials;
		aiTexture** textureRoot = scene->mTextures;

		//TODO
		float4x4 objectTransform = {};
		memcpy(&objectTransform, &nodeRoot->mTransformation, sizeof(float4x4));

		check(Object.m_name == "");
		Object = {};
		Object.CopyFromNode(nodeRoot);

		const unsigned int numChildren = nodeRoot->mNumChildren;
		unsigned int meshIndex = 0;
		unsigned int childIndex = 0;

		aiNode** rootNodeChild = nodeRoot->mChildren;

		LoadNode(renderer,
			Object,
			FullDir,
			rootNodeChild,
			meshRoot,
			materialRoot,
			numChildren,
			meshIndex,
			childIndex);
	}

	void TFileIO::LoadNode(
		CRenderer* renderer,
		TObject& object,
		std::string assetDir,
		aiNode** nodeChildRoot,
		aiMesh** meshRoot,
		aiMaterial** materialRoot,
		const unsigned int childCount,
		unsigned int& meshIndex,
		unsigned int& childIndex)
	{
		//TODO does not account for a node having more than 1 mesh...

		if (childIndex >= childCount)
			return;

		//Where we started in the mesh array for this node
		aiNode* currAINode = nodeChildRoot[childIndex++];

		if (currAINode->mNumMeshes > 0)
		{
			TModel model;
			model.CopyFromNode(currAINode);
			model.m_meshes.resize(currAINode->mNumMeshes);

			unsigned int tMeshIndex = 0;
			const unsigned int meshStart = meshIndex;
			for (; meshIndex < meshStart + currAINode->mNumMeshes;)
			{
				aiMesh* currAIMesh = meshRoot[meshIndex++];


				TMesh& currMesh = model.m_meshes[tMeshIndex];
				currMesh.m_materialKey = LoadMaterial(renderer, assetDir, currAINode, currAIMesh, materialRoot);
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
				std::vector<DepthPassInVS> vertexArrayDepth;
				std::vector<unsigned int> indexArray;

				vertexArray.resize(currAIMesh->mNumVertices);
				vertexArrayDepth.resize(currAIMesh->mNumVertices);

				for (unsigned int vertexIndex = 0; vertexIndex < (unsigned int)vertexArray.size(); ++vertexIndex)
				{
					BasePassInVS& currVertex = vertexArray[vertexIndex];
					DepthPassInVS& currVertexDepth = vertexArrayDepth[vertexIndex];

					check(currAIMesh->HasPositions());
					memcpy(&currVertex.position, &currAIMesh->mVertices[vertexIndex], sizeof(aiVector3D));
					memcpy(&currVertexDepth.position, &currAIMesh->mVertices[vertexIndex], sizeof(aiVector3D));

					//Vertex colors

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
						aiVector3D* TexCoorArr = currAIMesh->mTextureCoords[meshIndex - 1];

						currVertex.texcoord =
							TexCoorArr ?
							float2(TexCoorArr[vertexIndex].x, TexCoorArr[vertexIndex].y) :
							float2(0, 0);
					}
				}

				indexArray.resize(currAIMesh->mNumFaces * 3);
				for (unsigned int faceIndex = 0; faceIndex < currAIMesh->mNumFaces; ++faceIndex)
				{
					memcpy(&indexArray[faceIndex * 3], currAIMesh->mFaces[faceIndex].mIndices, sizeof(unsigned int) * 3);
				}

				currMesh.CreateMesh<BasePassInVS>(renderer, vertexArray, indexArray);
				currMesh.CreateMesh<DepthPassInVS>(renderer, vertexArrayDepth, indexArray);
				check(currMesh.m_vertexkey != -1 && currMesh.m_indexkey != -1);
				object.m_models.push_back(model);
			}
		}

		LoadNode(renderer,
			object,
			assetDir,
			nodeChildRoot,
			meshRoot,
			materialRoot,
			childCount,
			meshIndex,
			childIndex);
	}

	void TFileIO::CreateCube(CRenderer* renderer,
		TObject& Object,
		float size,
		int32 TextureID = -1)
	{
		static int32 numCubes = 0;
		TModel model = {};
		TMesh mesh = {};
		TMaterial material = {};

		model.m_name = "Skybox_" + std::to_string(numCubes++);

		float scale = size * 0.5f;
		{
			//TODO calc normals
			std::vector<BasePassInVS> vertexArray = {
				{ float3(-scale, scale, -scale), {}, {}, {},			{0, 0.666666f} },
				{ float3(-scale, -scale, -scale), {}, {}, {},			{0.25f, 0.666666f} },
				{ float3(scale, scale, -scale), {}, {}, {},				{0, 0.333333f} },
				{ float3(scale, -scale, -scale), {}, {}, {},			{0.25f, 0.333333f} },

				{ float3(-scale, -scale, scale), {}, {}, {},			{0.5f, 0.666666f} },
				{ float3(scale, -scale, scale), {}, {}, {},				{0.5f, 0.333333f} },
				{ float3(-scale, scale, scale), {}, {}, {},				{0.75f, 0.666666f} },
				{ float3(scale, scale, scale), {}, {},	{},				{0.75f, 0.333333f} },

				{ float3(-scale, scale, -scale), {}, {}, {},			{1.f, 0.666666f} },
				{ float3(scale, scale, -scale), {}, {}, {},				{1.f, 0.333333f} },

				{ float3(-scale, scale, -scale), {}, {}, {},			{0.25f, 1.f} },
				{ float3(-scale, scale, scale), {}, {}, {},				{0.5f, 1.f} },

				{ float3(scale, scale, -scale), {}, {}, {},				{0.25f, 0} },
				{ float3(scale, scale, scale), {}, {},	{},				{0.5f, 0} },
			
			};

			std::vector<unsigned int> indexArray = {
				0, 2, 1,	//front
				1, 2, 3,
				4, 5, 6,	//back
				5, 7, 6,
				6, 7, 8,	//top
				7, 9 ,8,
				1, 3, 4,	//bottom
				3, 5, 4,
				1, 11,10,	//left
				1, 4, 11,
				3, 12, 5,	//right
				5, 12, 13
			};

			mesh.CreateMesh<BasePassInVS>(renderer, vertexArray, indexArray);
		}
		

		material.m_textureDiffuse = TextureID;
		renderer->m_materials.push_back(material);
		mesh.m_materialKey = (int32)renderer->m_materials.size() - 1;

		mesh.m_bInitialized = true;
		model.m_meshes.push_back(mesh);

		Object.m_models.push_back(model);
	}

	void TFileIO::CreateSphere(CRenderer* renderer,
		TObject& Object,
		float lat,
		float lon,
		float scale,
		int32 TextureID = -1)
	{
		//do this or load in an .obj
		//https://medium.com/@oscarsc/four-ways-to-create-a-mesh-for-a-sphere-d7956b825db4
	}

	TextureID TFileIO::LoadTextureFromFile(CRenderer* renderer,
		std::string assetDir,
		std::string assetName)
	{
		check(assetDir != "" && assetName != "");

		ID3D11Device* device = renderer->GetD3DInterface()->GetDevice();
		ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

		const std::wstring wAssetDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Assets\\";
		std::string FullDir = Algorithm::wstring_to_string(wAssetDir) + assetDir + "\\" + assetName;

		ID3D11Resource* resource = nullptr;
		ID3D11ShaderResourceView* srv = nullptr;

		HRESULT r = {};
		r = DirectX::CreateWICTextureFromFile(device, Algorithm::string_to_wstring(FullDir).c_str(), &resource, &srv);
		checkhr(r);

		return renderer->CreateTexture(resource, srv);

		return -1;
	}

	TextureID TFileIO::LoadTextureFromFile(
		CRenderer* renderer,
		currAIMaterial* material,
		std::string assetDir,
		aiString dir)
	{
		ID3D11Device* device = renderer->GetD3DInterface()->GetDevice();
		ID3D11DeviceContext* context = renderer->GetD3DInterface()->GetContext();

		std::wstring wassetDir = Algorithm::string_to_wstring(assetDir);
		std::wstring wdir = Algorithm::ChopLast(wassetDir, L"\\") + L"\\" + Algorithm::string_to_wstring(dir.C_Str());


		ID3D11Resource* resource = nullptr;
		ID3D11ShaderResourceView* srv = nullptr;

		HRESULT r = {};
		r = DirectX::CreateWICTextureFromFile(device, wdir.c_str(), &resource, &srv);
		checkhr(r);

		return renderer->CreateTexture(resource, srv);

#if 0 //broken
		auto scratch = DirectX::ScratchImage{};

		HRESULT r = {};
		r = DirectX::LoadFromWICFile(wdir.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch);

		checkhr(r);


		auto image = scratch.GetMetadata();

		return renderer->CreateTexture(
			static_cast<unsigned int>(image.width),
			static_cast<unsigned int>(image.height),
			static_cast<unsigned int>(image.depth),
			static_cast<unsigned int>(image.arraySize),
			static_cast<unsigned int>(image.mipLevels),
			image.format);
#endif
		return -1;
	}

	MaterialID TFileIO::LoadMaterial(
		CRenderer* renderer,
		std::string assetDir,
		aiNode* nodeChild,
		aiMesh* meshChild,
		aiMaterial** materialRoot)
	{
		if (meshChild->mMaterialIndex == 0)
			return TextureID(-1);

		renderer->m_materials.push_back({});
		MaterialID ID = MaterialID(renderer->m_materials.size() - 1);
		TMaterial& currMaterial = renderer->GetMaterial(ID);

		aiMaterial* currAIMaterial = materialRoot[meshChild->mMaterialIndex];

#if ENABLE_CHECK
		check(
			TMaterial::GetConstantSize() ==
			sizeof(aiColor3D) +	//Diffuse
			sizeof(aiColor3D) +	//Specular
			sizeof(float) +		//Roughness
			sizeof(float) +		//Metallic
			sizeof(float)		//Opacity
		);
#endif
		aiString name;
		if (currAIMaterial->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
		{
			currMaterial.m_name = std::string(name.C_Str());
		}

		aiColor3D color;
		if (currAIMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
		{
			memcpy(&currMaterial.m_diffuseColor, &color, sizeof(aiColor3D));
		}

		if (currAIMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS)
		{
			memcpy(&currMaterial.m_specularColor, &color, sizeof(aiColor3D));
		}

		float scalar;
		if (currAIMaterial->Get(AI_MATKEY_OPACITY, scalar) == aiReturn_SUCCESS)
		{
			memcpy(&currMaterial.m_opacity, &scalar, sizeof(float));
		}

#if 1
		if (currAIMaterial->Get(AI_MATKEY_SHININESS, scalar) == aiReturn_SUCCESS)
		{
			//rough converssion
			// https://graphicscompendium.com/gamedev/15-pbr

			scalar = sqrtf(2.0f / (2.0f + scalar));
			memcpy(&currMaterial.m_roughness, &scalar, sizeof(float));
		}
#else //test this
		if (currAIMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, scalar) == aiReturn_SUCCESS)
		{
			memcpy(&currMaterial.m_roughness, &scalar, sizeof(float));
		}
#endif

		if (currAIMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, scalar) == aiReturn_SUCCESS)
		{
			memcpy(&currMaterial.m_metallic, &scalar, sizeof(float));
		}
		else
		{
			//no metallic param in non pbr mat. hard code some metall-ness for sake of rendering
			if (assetDir.find("Cornell") != std::string::npos && currMaterial.m_name.find("sphere") != std::string::npos)
			{
				currMaterial.m_metallic = 1.0f;
			}
		}

		aiString path;
		if (currAIMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
		{
			currMaterial.m_textureDiffuse = LoadTextureFromFile(renderer, nullptr, assetDir, path);
		}
		
		if (currAIMaterial->GetTexture(aiTextureType_NORMALS, 0, &path) == aiReturn_SUCCESS)
		{
			currMaterial.m_textureNormal = LoadTextureFromFile(renderer, nullptr, assetDir, path);
		}
#if 0

		if (currAIMaterial->GetTexture(aiTextureType_OPACITY, 0, &path) == aiReturn_SUCCESS)
		{
			currMaterial.m_textureMask = LoadTextureFromFile(renderer, assetDir, path);
		}

		if (currAIMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path) == aiReturn_SUCCESS)
		{
			currMaterial.m_textureSpecular = LoadTextureFromFile(renderer, assetDir, path);
		}
#endif

		return ID;
	}
}
