#pragma once
#include "Algorithm.h"
#include <array>
#include <string>
#include <d3d11shader.h>
#include <d3d11.h>
#include <iostream>
#include <algorithm>
#include <forward_list>

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
		eBasePass = 0,
		eDebugBasePass,
		eScreenQuad,
		eDeferredLighting,
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
	ID3D11ShaderReflection* m_stagerreflection[EShaderStage::eCount] = {};
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

//like TShaderPermutationKey but with 1 value entry
using PermutationValue = std::pair<std::string, int32>;
using PermutationArray = std::vector<std::vector<PermutationValue>>;

//assumes possible values are always in order
//assumes macros will apply to all shader stages available
struct TShaderPermutationKey
{
	template<typename ... int32>
	TShaderPermutationKey(std::string macro, int32&& ... values)
	{
		m_macro = macro;
		m_possibleValues = { values ... };
		std::sort(m_possibleValues.begin(), m_possibleValues.end());
	}

	bool operator==(TShaderPermutationKey k) { return strcmp(m_macro.c_str(), k.m_macro.c_str()) == 0; }
	bool operator==(PermutationValue k) { return strcmp(m_macro.c_str(), k.first.c_str()) == 0; }

	std::string m_macro;
	std::vector<int32> m_possibleValues;
};

static bool KeyComp(TShaderPermutationKey a, TShaderPermutationKey b) 
{ 
	check(strcmp(a.m_macro.c_str(), b.m_macro.c_str()) != 0);
	return a.m_macro < b.m_macro;
}
static bool KeyCompLookup(PermutationValue a, PermutationValue b) 
{ 
	check(strcmp(a.first.c_str(), b.first.c_str()) != 0);
	return a.first < b.first; 
}

//Not permuting:
//input layouts
//cbuffer
//resource bindings
struct TShaderPerumation
{
public:
	friend struct TShader;
	friend class CD3D11Interface;

	//MUST enter all macros & values
	int32 GetShaderWithPermutation(std::vector<PermutationValue> keys)
	{
		const PermutationValue Globals = { "SHADER", 1 };
		keys.push_back(Globals);

		check(keys.size() == m_keys.size());

		SortKeysInner(keys);

		int32 keyIndex = 0;
		int32 iterator = 0;

		std::vector <TShaderPermutationKey> uniqueKeys;
		std::vector<PermutationValue>		uniquePermutations;

		//Take out keys with 1 possible value
		for (int32 i = 0; i < (int32)m_keys.size(); ++i)
		{
			check(keys.size() == m_keys.size());

			if (m_keys[i].m_possibleValues.size() > 1)
			{
				uniqueKeys.push_back(m_keys[i]);
				uniquePermutations.push_back(keys[i]);
			}
		}

		int32 prevMax = 1;

		while(true)
		{
			if (iterator >= uniquePermutations.size())
				break;

			std::vector<int32> values = uniqueKeys[iterator].m_possibleValues;

			for (int32 i = 0; i < (int32)values.size(); ++i)
			{
				if (uniquePermutations[iterator].second == values[i])
				{
					if (i != 0)
					{
						keyIndex += prevMax * i;
					}

					prevMax *= (int32)values.size();
					break;
				}
			}

			++iterator;
		}

		return keyIndex;
	}

	bool bIsValid() { return m_bIsValid; }

	void SortKeys()
	{
		if (m_keys.size())
		{
			SortKeysInner(m_keys);
			m_bSorted = true;
			m_pArray = ComputePermutations();
		}
	}

private:
	void SortKeysInner(std::vector<TShaderPermutationKey>& keys)
	{
		check(keys.size() > 0);
		std::sort(keys.begin(), keys.end(), &KeyComp);
	}

	void SortKeysInner(std::vector<PermutationValue>& keys)
	{
		check(keys.size() > 0);
		std::sort(keys.begin(), keys.end(), &KeyCompLookup);
	}

	PermutationArray ComputePermutations()
	{
		check(m_bSorted);
		PermutationArray outPermutations;

		std::vector<PermutationValue> currPermutation;
		currPermutation.resize(m_keys.size());
		int32 numPermutations = 0;

		//im okay with double looping here. There should only be a handful of keys
		for (int32 i = 0; i < m_keys.size(); ++i)
		{
			TShaderPermutationKey& key = m_keys[i];

			numPermutations += (int32)key.m_possibleValues.size();

			//just to initialize
			currPermutation[i] = { key.m_macro, key.m_possibleValues[0] };
		}

		outPermutations.push_back(currPermutation);

		//just in case nothing went horribly wrong
		check(currPermutation.size() == m_keys.size());


		int32 iterator = 0;

		while (true)
		{
			if (iterator > currPermutation.size() - 1)
				break;

			const std::vector<int32>& keyValues = m_keys[iterator].m_possibleValues;

			++currPermutation[iterator].second;

			if (currPermutation[iterator].second > keyValues[keyValues.size() - 1])
			{
				currPermutation[iterator].second = keyValues[0];
				++iterator;
				continue;
			}

			outPermutations.push_back(currPermutation);

			if (iterator > 0)
			{
				iterator = 0;
				continue;
			}

		}

		m_bIsValid = true;

		return outPermutations;
	}

	PermutationArray m_pArray;
	int32 m_shaderIndexUpperLimit;
	std::vector<TShaderPermutationKey> m_keys;
	bool m_bIsValid = false;
	bool m_bSorted = false;
};

struct TShader
{
	template<bool VS, bool HS, bool DS, bool GS, bool PS, bool CS>
	void Initialize(std::string shadername, std::vector<TShaderPermutationKey> permutations = {})
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

		//Global Macros
		{
			permutations.push_back({ "SHADER", 1 });
		}
		m_permutations.m_keys = permutations;
		m_permutations.SortKeys();
	}

	void Reset()
	{
		//reset cbuffers
	};

	template <EShaderStage::Type E> void SetShaderStages(ID3D11DeviceContext* context, int32 permutationIndex = 0) {}
	template <> void SetShaderStages<EShaderStage::Type::eVS>(ID3D11DeviceContext* context, int32 permutationIndex) { context->VSSetShader(m_shaderstages[permutationIndex].m_vs, nullptr, 0); }
	template <> void SetShaderStages<EShaderStage::Type::eHS>(ID3D11DeviceContext* context, int32 permutationIndex) { context->HSSetShader(m_shaderstages[permutationIndex].m_hs, nullptr, 0); }
	template <> void SetShaderStages<EShaderStage::Type::eDS>(ID3D11DeviceContext* context, int32 permutationIndex) { context->DSSetShader(m_shaderstages[permutationIndex].m_ds, nullptr, 0); }
	template <> void SetShaderStages<EShaderStage::Type::eGS>(ID3D11DeviceContext* context, int32 permutationIndex) { context->GSSetShader(m_shaderstages[permutationIndex].m_gs, nullptr, 0); }
	template <> void SetShaderStages<EShaderStage::Type::ePS>(ID3D11DeviceContext* context, int32 permutationIndex) { context->PSSetShader(m_shaderstages[permutationIndex].m_ps, nullptr, 0); }
	template <> void SetShaderStages<EShaderStage::Type::eCS>(ID3D11DeviceContext* context, int32 permutationIndex) { context->CSSetShader(m_shaderstages[permutationIndex].m_cs, nullptr, 0); }

	void SetShaderStages(ID3D11DeviceContext* context, int32 permutationIndex = 0)
	{
		if (m_usedshaderstages[EShaderStage::eVS])
		{
			context->VSSetShader(m_shaderstages[permutationIndex].m_vs, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eHS])
		{
			context->HSSetShader(m_shaderstages[permutationIndex].m_hs, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eDS])
		{
			context->DSSetShader(m_shaderstages[permutationIndex].m_ds, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eGS])
		{
			context->GSSetShader(m_shaderstages[permutationIndex].m_gs, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::ePS])
		{
			context->PSSetShader(m_shaderstages[permutationIndex].m_ps, nullptr, 0);
		}

		if (m_usedshaderstages[EShaderStage::eCS])
		{
			context->CSSetShader(m_shaderstages[permutationIndex].m_cs, nullptr, 0);
		}
	}

	template<EShaderStage::Type stage, int32 StartSlot = 0>
	void SetShaderResource(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv)
	{
		check(srv != nullptr);

		if (stage == EShaderStage::eVS)
		{
			//TODO
		}
		if (stage == EShaderStage::ePS)
		{
			context->PSSetShaderResources(StartSlot, 1, &srv);
		}
	}

	template<EShaderStage::Type stage, int32 StartSlot = 0>
	void SetSamplerState(ID3D11DeviceContext* context, ID3D11SamplerState* sampler)
	{
		check(sampler != nullptr);
		//check(stage == EShaderStage::ePS || stage == EShaderStage::eCS);

		context->PSSetSamplers(StartSlot, 1, &sampler);
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
		for (int i = 0; i < m_permutations.m_pArray.size(); ++i)
		{
			DXRelease(m_shaderstages[i].m_vs);
			DXRelease(m_shaderstages[i].m_hs);
			DXRelease(m_shaderstages[i].m_ds);
			DXRelease(m_shaderstages[i].m_gs);
			DXRelease(m_shaderstages[i].m_ps);
			DXRelease(m_shaderstages[i].m_cs);
			DXRelease(m_inputlayout);

			for (int j = 0; j < EShaderStage::eCount; ++j)
			{
				DXRelease(m_shaderreflections[i].m_stagerreflection[j]);
			}
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
	}

	std::vector<TConstantBufferBinding>	m_constantbuffers;
	std::vector<TD3DConstantBufferLayout> m_constantbufferlayouts;
	std::vector<TCPUConstant> m_CPUconstantbuffers;
	std::vector<ConstantBufferMapping> m_constantbuffermap;

	std::array<bool, EShaderStage::eCount> m_usedshaderstages;
	std::string m_shaderdir;

	std::vector<TTextureBinding> m_texturebindings;
	std::vector<TSamplerBinding> m_samplerbindings;
	ShaderTextureLookup m_shadertexturelookup;
	ShaderSamplerLookup m_shadersamplerlookup;

	ID3D11InputLayout* m_inputlayout;
	TShaderInfo m_info;
	TShaderPerumation m_permutations;


	std::vector<TD3DReflections> m_shaderreflections;
	std::vector<TShaderStages> m_shaderstages;

	bool m_firstShaderInitialized = false;
	};