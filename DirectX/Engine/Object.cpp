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
}

void TObject::TransformMeshes(float3 offset)
{
	for (int32 i = 0; i < m_models.size(); i++)
	{
		TModel& model = m_models[i];
		model.m_transform.m_position.x += offset.x;
		model.m_transform.m_position.y += offset.y;
		model.m_transform.m_position.z += offset.z;
	}
}

void TObject::RotateMeshes(float3 offset)
{
	for (int32 i = 0; i < m_models.size(); i++)
	{
		TModel& model = m_models[i];
		model.m_transform.m_rotation.x += offset.x;
		model.m_transform.m_rotation.y += offset.y;
		model.m_transform.m_rotation.z += offset.z;
	}
}

void TObject::ScaleMeshes(float3 scale)
{
	for (int32 i = 0; i < m_models.size(); i++)
	{
		TModel& model = m_models[i];
		model.m_transform.m_scale.x += scale.x;
		model.m_transform.m_scale.y += scale.y;
		model.m_transform.m_scale.z += scale.z;
	}
}