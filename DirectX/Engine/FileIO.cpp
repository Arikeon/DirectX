#include "FileIO.h"
#include "Algorithm.h"
#include "BufferStruct.h"

#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>

//Assimp
#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>

using namespace Assimp;

namespace IO
{
	void TFileIO::LoadMesh(std::vector<TModel>& model, std::string Dir)
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

		Importer importer;
		const aiScene* scene = importer.ReadFile(Dir, assimpFlags);

		int lol = 123;
	}
}