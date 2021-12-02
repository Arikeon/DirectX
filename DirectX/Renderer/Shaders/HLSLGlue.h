//structs and buffers intended to be readable by hlsl and c++

#ifndef SHADER
	#define SHADER 0
#endif

#define CPU_SHADER_READABLE __cplusplus || SHADER
#define CPP __cplusplus


#define FRUSTRUM_FAR_PLANE 3.402823466e+38F //FLT_MAX

#if CPP
	//Buffer & Structs
	#include "D3DDefines.h"
	#define START_CBUFFER(CB, reg) struct CB {
	#define END_CBUFFER(CB) }
	#define SEMANTIC(SM)
	#define CENTROID
	#define NOINTERPOLATION
	#define NOPERSPECTIVE
	#define LINEAR
#elif SHADER //defined during compile
	//Buffer & Structs
	#define START_CBUFFER(CB, reg) cbuffer CB /*: register(reg)*/ {
	#define END_CBUFFER(CB) }
	#define SEMANTIC(SM) : SM
	#define CENTROID centroid
	#define NOINTERPOLATION nointerpolation
	#define NOPERSPECTIVE noperspective
	#define LINEAR linear
#endif
