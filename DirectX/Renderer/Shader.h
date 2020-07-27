#pragma once
#include "Algorithm.h"
#include <array>
#include <string>
#include <d3d11shader.h>
#include <d3d11.h>

#define EShaderListToString(name) #name

namespace EShaderList
{
	enum Type
	{
		//Base Pass
		BasePass = 0,

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

struct TShaderInfo
{
	std::string m_name;
	int m_instructioncount;
};

struct TShaderStages
{
	ID3D11VertexShader* m_vs;
	ID3D11HullShader* m_hs;
	ID3D11DomainShader* m_ds;
	ID3D11GeometryShader* m_gs;
	ID3D11PixelShader* m_ps;
	ID3D11ComputeShader* m_cs;
};

struct D3DConstantBufferLayout
{
	D3D11_SHADER_BUFFER_DESC m_desc;
	std::vector<D3D11_SHADER_VARIABLE_DESC> m_variables;
	std::vector<D3D11_SHADER_TYPE_DESC> m_types;
	unsigned m_buffSize;
	EShaderStage::Type m_stage;
	unsigned  m_bufSlot;
};

union D3DBlobs
{
	ID3D10Blob* m_stageblob[EShaderStage::eCount] = {};
	struct
	{
		ID3D10Blob* m_vsBlob;
		ID3D10Blob* m_hsBlob;
		ID3D10Blob* m_dsBlob;
		ID3D10Blob* m_gsBlob;
		ID3D10Blob* m_psBlob;
		ID3D10Blob* m_csBlob;
	};
};

union D3DReflections
{
	ID3D11ShaderReflection* m_stagereflection[EShaderStage::eCount] = {};
	struct
	{
		ID3D11ShaderReflection* m_vsreflection;
		ID3D11ShaderReflection* m_hsreflection;
		ID3D11ShaderReflection* m_dsreflection;
		ID3D11ShaderReflection* m_gsreflection;
		ID3D11ShaderReflection* m_psreflection;
		ID3D11ShaderReflection* m_csreflection;
	};
};

struct TShader
{
	template<bool VS, bool HS, bool DS, bool GS, bool PS, bool CS>
	void Initialize(std::string shadername)
	{
		m_info.m_name = shadername;

		std::wstring wShaderDir = Algorithm::ChopLast(Algorithm::GetExecutablePath(), L"\\", 4) + L"\\DirectX\\Renderer\\Shaders\\";
		const std::string ShaderDir = Algorithm::wstring_to_string(wShaderDir);
		m_shaderdir = ShaderDir + shadername;
		
		m_usedshaderstages[EShaderStage::eVS] = VS;
		m_usedshaderstages[EShaderStage::eHS] = HS;
		m_usedshaderstages[EShaderStage::eDS] = DS;
		m_usedshaderstages[EShaderStage::eGS] = GS;
		m_usedshaderstages[EShaderStage::ePS] = PS;
		m_usedshaderstages[EShaderStage::eCS] = CS;

		m_shadermacros.push_back({"SHADER", "1"});
	}

	static std::string GetEntryPointName(int stage)
	{
		return TShader::GetEntryPointName((EShaderStage::Type)stage);
	}

	static std::string GetEntryPointName(EShaderStage::Type stage)
	{
		const std::array<std::string, EShaderStage::eCount> EntryPointNames =
		{
			"MainVS",
			"MainHS",
			"MainDS",
			"MainGS",
			"MainPS",
			"MainCS",
		};

		return EntryPointNames[stage];
	}

	static std::string GetFeatureLevel(int stage)
	{
		return TShader::GetFeatureLevel((EShaderStage::Type)stage);
	}

	static std::string GetFeatureLevel(EShaderStage::Type stage)
	{
		const std::array<std::string, EShaderStage::eCount> FeatureLevelNames =
		{
			"vs_5_0",
			"hs_5_0",
			"ds_5_0",
			"gs_5_0",
			"ps_5_0",
			"cs_5_0",
		};

		return FeatureLevelNames[stage];
	}

	std::string GetDirectoryFromStage(EShaderStage::Type stage)
	{
		switch (stage)
		{
		case EShaderStage::eVS:
			return m_shaderdir + "VS.hlsl";
			break;
		case EShaderStage::eHS:
			return m_shaderdir + "HS.hlsl";
			break;
		case EShaderStage::eDS:
			return m_shaderdir + "DS.hlsl";
			break;
		case EShaderStage::eGS:
			return m_shaderdir + "GS.hlsl";
			break;
		case EShaderStage::ePS:
			return m_shaderdir + "PS.hlsl";
			break;
		case EShaderStage::eCS:
			return m_shaderdir + "CS.hlsl";
			break;
		default:
			check(0);
			return std::string("NULL");
			break;
		}
	}


	~TShader()
	{
		DXRelease(m_shaderstages.m_vs);
		DXRelease(m_shaderstages.m_hs);
		DXRelease(m_shaderstages.m_ds);
		DXRelease(m_shaderstages.m_gs);
		DXRelease(m_shaderstages.m_ps);
		DXRelease(m_shaderstages.m_cs);
		DXRelease(m_inputlayout);

		for (int i = 0; i < EShaderStage::eCount; ++i)
		{
			DXRelease(m_shaderreflections.m_stagereflection[i]);
		}
	}

	std::array<bool, EShaderStage::eCount> m_usedshaderstages;
	std::vector<D3D10_SHADER_MACRO> m_shadermacros;
	D3DReflections m_shaderreflections;
	TShaderStages m_shaderstages;
	ID3D11InputLayout* m_inputlayout;
	std::string m_shaderdir;
	TShaderInfo m_info;
	};