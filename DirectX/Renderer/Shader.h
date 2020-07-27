#pragma once
#include <string>
#include "Algorithm.h"
#include <array>

#define EShaderListToString(name) #name

namespace EShaderList
{
	enum Type
	{
		//Base Pass
		Base = 0,

		eCount,
	};
}

namespace EShaderStage
{
	enum Type
	{
		//Vertex Shader
		eVS = 0,
		//Hull Shader
		eHS,
		//Domain Shader
		eDS,
		//Geometry Shader
		eGS,
		//Pixel Shader
		ePS,
		//Compute Shader
		eCS,
		eCount,
	};
}

struct TShader
{
	template<bool VS, bool HS, bool DS, bool GS, bool PS, bool CS>
	void Initialize(std::string shadername)
	{
		std::wstring wShaderDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Renderer\\";
		const std::string ShaderDir = Algorithm::wstring_to_string(wShaderDir);
		m_shaderdir = ShaderDir + shadername;
		
		m_shaderstages[EShaderStage::eVS] = VS;
		m_shaderstages[EShaderStage::eHS] = HS;
		m_shaderstages[EShaderStage::eDS] = DS;
		m_shaderstages[EShaderStage::eGS] = GS;
		m_shaderstages[EShaderStage::ePS] = PS;
		m_shaderstages[EShaderStage::eCS] = CS;
	}

	std::string m_shaderdir;
	std::array<bool, EShaderStage::eCount> m_shaderstages;
};
