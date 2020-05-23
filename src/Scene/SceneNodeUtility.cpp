#include "Scene/SceneNodeUtility.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


QString SceneNodeUtility::GetEnumValue(const Definitions* definitions, const FieldContext& fieldCtx, int data)
{
	auto fieldType = fieldCtx.FieldInfo->FieldType->Type;

	if (fieldType < Definitions::ENodeFieldType::Uint8 || fieldType > Definitions::ENodeFieldType::Hex32)
		return QString();

	auto enumMap = definitions->GetNodeFieldEnum(fieldCtx.Node->Type, fieldCtx.FieldIdx);
	if (enumMap == null)
		return QString();

	const auto& enumValue = enumMap->find(data);
	if (enumValue == enumMap->end())
		return QString();

	return enumValue.value();
}
