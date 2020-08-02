#pragma once
#include "Algorithm.h"
#include <array>
#include <string>
#include <d3d11shader.h>
#include <d3d11.h>

#define EShaderListToString(name) #name

using CPUConstantID = int;
using GPU_ConstantBufferSlotIndex = int;
using ConstantBufferMapping = std::pair<GPU_ConstantBufferSlotIndex, CPUConstantID>;
using ShaderTextureLookup = std::unordered_map<std::string, int>;
using ShaderSamplerLookup = std::unordered_map<std::string, int>;

namespace EShaderList
{
	enum Type
	{
		//Base Pass
		BasePass = 0,
		DebugBasePass,

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

struct TD3DConstantBufferLayout
{
	D3D11_SHADER_BUFFER_DESC m_desc;
	std::vector<D3D11_SHADER_VARIABLE_DESC> m_variables;
	std::vector<D3D11_SHADER_TYPE_DESC> m_types;
	int m_bufferSize;
	EShaderStage::Type m_stage;
	int  m_bufferSlot;
};

union TD3DBlobs
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

union TD3DReflections
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

struct TConstantBufferBinding
{
	EShaderStage::Type m_shaderstage;
	int m_bufferslot;
	ID3D11Buffer* m_pdata;
	bool m_dirty;
};
struct TTextureBinding
{
	EShaderStage::Type m_shaderStage;
	int m_textureSlot;
};
struct TSamplerBinding
{
	EShaderStage::Type m_shaderStage;
	int m_samplerSlot;
};

struct TCPUConstant
{
	using CPUConstantRefIDPair = std::tuple<TCPUConstant&, CPUConstantID>;

	TCPUConstant() : m_name(), m_size(0), m_pdata(nullptr) {}
	std::string m_name;
	size_t		m_size;
	void* m_pdata;

	inline bool operator==(const TCPUConstant& c) const { return (((this->m_pdata == c.m_pdata) && this->m_size == c.m_size) && this->m_name == c.m_name); }
	inline bool operator!=(const TCPUConstant& c) const { return ((this->m_pdata != c.m_pdata) || this->m_size != c.m_size || this->m_name != c.m_name); }
};

struct TShader
{
	template<bool VS, bool HS, bool DS, bool GS, bool PS, bool CS>
	void Initialize(std::string shadername, std::vector<D3D10_SHADER_MACRO>* macros = nullptr)
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

		if (macros)
		{
			for (int i = 0; i < macros->size(); ++i)
			{
				m_shadermacros.push_back((*macros)[i]);
			}
		}
	}

	void Reset()
	{
		//reset cbuffers
	};

	void SetShaderStages(ID3D11DeviceContext* context)
	{
		if (m_usedshaderstages[EShaderStage::eVS])
		{
			context->VSSetShader(m_shaderstages.m_vs, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eHS])
		{
			context->HSSetShader(m_shaderstages.m_hs, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eDS])
		{
			context->DSSetShader(m_shaderstages.m_ds, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eGS])
		{
			context->GSSetShader(m_shaderstages.m_gs, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::ePS])
		{
			context->PSSetShader(m_shaderstages.m_ps, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eCS])
		{
			context->CSSetShader(m_shaderstages.m_cs, nullptr, 0);
		}
	}

	void WriteConstants(std::string name, void* data)
	{
		for (int i = 0; i < (int)m_constantbuffermap.size(); i++)
		{
			ConstantBufferMapping& buffID = m_constantbuffermap[i];

			CPUConstantID constantID = buffID.second;
			TCPUConstant& CPUConstant = m_CPUconstantbuffers[constantID];

			if (strcmp(CPUConstant.m_name.c_str(), name.c_str()) == 0)
			{
				memcpy(CPUConstant.m_pdata, data, CPUConstant.m_size);
				m_constantbuffers[buffID.first].m_dirty = true;
				return;
			}
		}

		std::wstring wname(Algorithm::string_to_wstring(name));

		CONSOLE_LOG(L"Unable to find: " + wname);
	}

	void BindData(ID3D11DeviceContext* context)
	{
		for (int i = 0; i < (int)m_constantbuffers.size(); ++i)
		{
			TConstantBufferBinding& binding = m_constantbuffers[i];

			if (binding.m_dirty)
			{
				ID3D11Buffer* pdata = binding.m_pdata;
				D3D11_MAPPED_SUBRESOURCE mappedResource;

				context->Map(pdata, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				char* buffpos = (char*)mappedResource.pData;

				for (ConstantBufferMapping& indexedPair : m_constantbuffermap)
				{
					if (indexedPair.first != i)
						continue;

					int slotIndex = indexedPair.first;
					CPUConstantID cpuid = indexedPair.second;
					check(cpuid < (int)m_CPUconstantbuffers.size());
					TCPUConstant& constant = m_CPUconstantbuffers[cpuid];
					memcpy(buffpos, constant.m_pdata, constant.m_size);
					buffpos += constant.m_size;
				}
				context->Unmap(pdata, 0);

				const auto GetStageSlot = [](ID3D11DeviceContext* context, EShaderStage::Type stage, int cbslot, ID3D11Buffer* pdata)
				{
					switch (stage)
					{
					case EShaderStage::eVS:
					{
						context->VSSetConstantBuffers(cbslot, 1, &pdata);
					}
					break;
					case EShaderStage::eHS:
					{
						context->HSSetConstantBuffers(cbslot, 1, &pdata);
					}
					break;
					case EShaderStage::eDS:
					{
						context->DSSetConstantBuffers(cbslot, 1, &pdata);
					}
					break;
					case EShaderStage::eGS:
					{
						context->GSSetConstantBuffers(cbslot, 1, &pdata);
					}
					break;
					case EShaderStage::ePS:
					{
						context->PSSetConstantBuffers(cbslot, 1, &pdata);
					}
					break;
					case EShaderStage::eCS:
					{
						context->CSSetConstantBuffers(cbslot, 1, &pdata);
					}
					break;
					default:
						check(0);
						break;
					}
				};

				GetStageSlot(context, binding.m_shaderstage, binding.m_bufferslot, pdata);
			}

			binding.m_dirty = false;
		}
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


	void Release()
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

		for (int i = 0; i < m_constantbuffers.size(); ++i)
		{
			DXRelease(m_constantbuffers[i].m_pdata);
		}
		m_constantbuffers.clear();

		for (int i = 0; i < m_CPUconstantbuffers.size(); ++i)
		{
			delete m_CPUconstantbuffers[i].m_pdata;
			m_CPUconstantbuffers[i].m_pdata = nullptr;
		}
		m_CPUconstantbuffers.clear();

		m_constantbufferlayouts.clear();
		m_constantbuffermap.clear();
		m_shadermacros.clear();
	}

	std::vector<TConstantBufferBinding>	m_constantbuffers;
	std::vector<TD3DConstantBufferLayout> m_constantbufferlayouts;
	std::vector<TCPUConstant> m_CPUconstantbuffers;
	std::vector<ConstantBufferMapping> m_constantbuffermap;

	std::array<bool, EShaderStage::eCount> m_usedshaderstages;
	std::vector<D3D10_SHADER_MACRO> m_shadermacros;
	TD3DReflections m_shaderreflections;
	TShaderStages m_shaderstages;
	std::string m_shaderdir;

	std::vector<TTextureBinding> m_texturebindings;
	std::vector<TSamplerBinding> m_samplerbindings;
	ShaderTextureLookup m_shadertexturelookup;
	ShaderSamplerLookup m_shadersamplerlookup;

	ID3D11InputLayout* m_inputlayout;
	TShaderInfo m_info;
	};