#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"

namespace IO
{
	struct TFileIO
	{
		static void LoadMesh(std::vector<TModel>& model, std::string Dir);
	};
}

