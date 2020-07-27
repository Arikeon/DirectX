//structs and buffers intended to be readable by hlsl and c++

#if __cplusplus
	//Buffer & Structs
	#include "D3DDefines.h"
	#define START_CBUFFER(CB, reg) struct CB : register(reg) {
	#define END_CBUFFER(CB) }
	#define SEMANTIC(SM);

	//Texture resources
	#define Texture2D //
	#define Texture3D //
	#define TextureCube //
	#define SamplerState //

#elif SHADER //defined during compile
	//Buffer & Structs
	#define START_CBUFFER(CB, reg) cbuffer CB : register(reg) {
	#define END_CBUFFER(CB) }
	#define SEMANTIC(SM) : SM;

	//Texture resources
	#define Texture2D Texture2D
	#define Texture3D Texture3D
	#define TextureCube TextureCube
	#define SamplerState SamplerState
#endif