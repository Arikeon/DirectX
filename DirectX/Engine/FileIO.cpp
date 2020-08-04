#include "FileIO.h"
#include "Algorithm.h"
#include "BufferStruct.h"

#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>

namespace IO
{
	TDirectory ReadDir(std::string dir)
	{
		TDirectory Dir;
		Dir.m_dir = dir + "\\";

		std::string pattern(dir);
		pattern.append("\\*");
		WIN32_FIND_DATA data;
		HANDLE hFind;
		if ((hFind = FindFirstFile(Algorithm::string_to_wstring(pattern).c_str(), &data)) != INVALID_HANDLE_VALUE) {
			do {
				std::wstring wfilename = data.cFileName;

				if (wfilename.compare(L".") == 0 || wfilename.compare(L"..") == 0)
					continue;

				TFile file;
				file.m_name = Algorithm::wstring_to_string(wfilename);

				std::vector<std::string> strList;

				Algorithm::split(file.m_name, strList, '.');

				if (strList[1] == "obj")
				{
					file.m_fileType = EFileType::eObj;
					Dir.m_objfiles.push_back(file);
				}
				else if (strList[1] == "mtl")
				{
					file.m_fileType = EFileType::eMtl;
					Dir.m_mtlfiles.push_back(file);
				}

			} while (FindNextFile(hFind, &data) != 0);
			FindClose(hFind);
		}

		return Dir;
	}

	void TFileIO::LoadModelFromOBJ(std::vector<TModel> model, std::string filename)
	{
		check(model.size() == 0);

		const std::wstring wAssetDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Assets\\";
		const std::string AssetDir = Algorithm::wstring_to_string(wAssetDir) + filename;

		TDirectory Directory = ReadDir(AssetDir);

		for (int i = 0; i < (int)Directory.m_objfiles.size(); ++i)
		{
			TFile file = Directory.m_objfiles[i];
			TModel model;

			std::string dir = Directory.m_dir + file.m_name;

			std::fstream stream(dir);

			if (!stream.is_open())
			{
				CONSOLE_LOG(L"Failed to load... " + Algorithm::string_to_wstring(dir));
				continue;
			}

			std::string CurrLine;
			std::vector<std::string> CurrLineSplit;
			std::string FL; //First Line

			while (true)
			{
				if (stream.eof())
					break;

				std::getline(stream, CurrLine);

				if (CurrLine == "" || CurrLine == " ")
					continue;

				CurrLineSplit.clear();
				Algorithm::split(CurrLine, CurrLineSplit, ' ');

				check(CurrLineSplit.size() > 0);
				FL = CurrLineSplit[0];

				if (FL == "#")
					continue;



			}
		}
	}
}