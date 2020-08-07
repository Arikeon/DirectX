#include "Object.h"

//min max compile errors - undef them
//{
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#undef max
#undef min
//}

//assimp library
#include <assimp\scene.h>

void TObject::CopyFromNode(aiNode* node)
{
	m_name = node->mName.C_Str();
	float4x4 transformMatrix;
	memcpy(&transformMatrix, &node->mTransformation, sizeof(float4x4));
	m_transform = TTransform(transformMatrix);
}