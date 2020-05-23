#include "Scene/SceneNodeUtility.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


QStringList SceneNodeUtility::GetFieldDataAsString(const SceneNodeUtility::FieldContext& fieldCtx)
{
	QStringList data;

	auto field     = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
	auto fieldType = fieldCtx.FieldInfo->FieldType->Type;

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::Uint8:    GetFieldDataInt<uchar >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Uint16:   GetFieldDataInt<ushort>(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Uint16_3: GetFieldDataInt<ushort>(data, fieldCtx, 3); break;
		case Definitions::ENodeFieldType::Uint32:   GetFieldDataInt<uint  >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Int8:     GetFieldDataInt<char  >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Int16:    GetFieldDataInt<short >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Int32:    GetFieldDataInt<int   >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Hex8:     GetFieldDataHex<uchar >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Hex16:    GetFieldDataHex<ushort>(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Hex32:    GetFieldDataHex<uint  >(data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Float:    GetFieldDataFloat      (data, fieldCtx, 1); break;
		case Definitions::ENodeFieldType::Float2:   GetFieldDataFloat      (data, fieldCtx, 2); break;
		case Definitions::ENodeFieldType::Float3:   GetFieldDataFloat      (data, fieldCtx, 3); break;
		case Definitions::ENodeFieldType::Float4:   GetFieldDataFloat      (data, fieldCtx, 4); break;
		case Definitions::ENodeFieldType::Color:    GetFieldDataFloat      (data, fieldCtx, 3); break;

		case Definitions::ENodeFieldType::String:
		case Definitions::ENodeFieldType::StringFixed:
		{
			auto dataChar = reinterpret_cast<const char*>(field);
			data << QString::fromLatin1(dataChar);
			break;
		}

		case Definitions::ENodeFieldType::StringArray:
		case Definitions::ENodeFieldType::StringArray2:
		{
			auto dataChar      = reinterpret_cast<const char*>(field);
			auto dataUInt      = reinterpret_cast<const uint*>(field);
			auto sizeReduction = (fieldType == Definitions::ENodeFieldType::StringArray2) ? 1 : 0;
			data << QString::fromLatin1(&dataChar[4], dataUInt[0] - sizeReduction);
			break;
		}

		default:
			break;
	}

	return data;
}

QString SceneNodeUtility::GetFieldDataEnum(const Definitions* definitions, const FieldContext& fieldCtx, int idx)
{
	auto enumMap = definitions->GetNodeFieldEnum(fieldCtx.Node->Type, fieldCtx.FieldIdx);
	if (enumMap == null)
		return QString();

	auto fieldType = fieldCtx.FieldInfo->FieldType->Type;
	auto data      = 0;

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::Uint8:    data = (int)GetFieldData<uchar >(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Uint16:   data = (int)GetFieldData<ushort>(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Uint16_3: data = (int)GetFieldData<ushort>(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Uint32:   data = (int)GetFieldData<uint  >(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Int8:     data = (int)GetFieldData<char  >(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Int16:    data = (int)GetFieldData<short >(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Int32:    data = (int)GetFieldData<int   >(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Hex8:     data = (int)GetFieldData<uchar >(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Hex16:    data = (int)GetFieldData<ushort>(fieldCtx, idx); break;
		case Definitions::ENodeFieldType::Hex32:    data = (int)GetFieldData<uint  >(fieldCtx, idx); break;
		default: break;
	}

	const auto& enumValue = enumMap->find(data);
	if (enumValue == enumMap->end())
		return QString();

	return enumValue.value();
}
