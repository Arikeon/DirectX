#pragma once
#include "Build.h"
#include "Window.h"
#include <unordered_map>
#include <sstream>

namespace Algorithm
{
	//Used in c++ library for iterative deletes
	template<typename Type>
	static INLINE bool DeleteElements(Type* _Element) { DeletePtr(_Element); return true; }

	template <typename Type>
	static INLINE bool DeleteElementsArray(Type* _Element) { DeletePtrArr(_Element); return true; }

	static INLINE std::wstring string_to_wstring(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

#pragma warning( push )
#pragma warning(disable:4244)
	static INLINE std::string wstring_to_string(const std::wstring& str)
	{
		std::string s(str.begin(), str.end());
		return s;
	}
#pragma warning( pop )

	static INLINE std::wstring GetExecutablePath()
	{
		char Filename[MAX_PATH]; //this is a char buffer
		GetModuleFileNameA(NULL, Filename, sizeof(Filename));

		return std::wstring(string_to_wstring(Filename));
	}

	static INLINE std::wstring ChopLast(std::wstring wStr, std::wstring Delim, uint32 iterations = 0)
	{
		const auto pos = wStr.find_last_of(Delim);
		std::wstring result(wStr.substr(0, pos));

		--iterations;
		if (iterations != -1)
			return ChopLast(result, Delim, iterations);
		else
			return result;
	}

	static INLINE void split(const std::string& _str, std::vector<std::string>& _container, char _delim = ' ')
	{
		std::stringstream ss(_str);
		std::string token;
		while (std::getline(ss, token, _delim)) {
			if (token != "") {
				_container.push_back(token);
			}
		}
	}

	//static INLINE void LookAt()
}