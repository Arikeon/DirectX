//structs and buffers intended to be readable by hlsl and c++

#if __cplusplus
	#include "D3DDefines.h"
	#define START_CBUFFER(CB, reg) struct CB : register(reg) {
	#define END_CBUFFER(CB) }
	#define SEMANTIC(SM);
#elif SHADER //defined during compile
	#define START_CBUFFER(CB, reg) cbuffer CB : register(reg) {
	#define END_CBUFFER(CB) }
	#define SEMANTIC(SM) : SM;
#endif