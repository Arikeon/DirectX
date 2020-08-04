#pragma once
#include "Build.h"
#include "Model.h"
#include "Mesh.h"

namespace IO
{
	struct TFileIO;
	struct TFile;
	struct TDirectory;

	namespace EFileType
	{
		enum Type
		{
			eObj = 0,	//mesh
			eMtl,		//material
			eUnreadable
		};
	}

	struct TDirectory
	{
		std::vector<TFile> m_objfiles;
		std::vector<TFile> m_mtlfiles;
		std::string m_dir;
	};

	struct TFile
	{
		std::string m_name;
		EFileType::Type m_fileType;
	};

	struct TFileIO
	{
	public:
		static void LoadModelFromOBJ(std::vector<TModel> model, std::string filename);
	};
}