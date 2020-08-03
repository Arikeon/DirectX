#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"

namespace IO
{
	struct TFileIO
	{
	public:
		static void LoadModelFromOBJ(std::vector<TModel> model, std::string filename);
	};
}