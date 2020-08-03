#include "FileIO.h"
#include "Algorithm.h"

#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>

namespace IO
{
	std::vector<std::string> ReadDir(std::string dir)
	{
		std::vector<std::string> filesindir;

		std::string pattern(dir);
		pattern.append("\\*");
		WIN32_FIND_DATA data;
		HANDLE hFind;
		if ((hFind = FindFirstFile(Algorithm::string_to_wstring(pattern).c_str(), &data)) != INVALID_HANDLE_VALUE) {
			do {
				filesindir.push_back(Algorithm::wstring_to_string(data.cFileName));
			} while (FindNextFile(hFind, &data) != 0);
			FindClose(hFind);
		}

		return filesindir;
	}

	void TFileIO::LoadModelFromOBJ(std::vector<TModel> model, std::string filename)
	{
		check(model.size() == 0);

		const std::wstring wAssetDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Assets\\";
		const std::string AssetDir = Algorithm::wstring_to_string(wAssetDir) + filename;

		std::vector<std::string> FilesInDir = ReadDir(AssetDir);
	}
}