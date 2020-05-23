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

void SceneNodeUtility::SetFieldData(const SceneNodeUtility::FieldContext& fieldCtx, const void* data, int dataSize)
{
	auto& field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
	delete[] reinterpret_cast<uchar*>(field);
	field = new uchar[dataSize];
	memcpy(field, data, dataSize);
}

void SceneNodeUtility::SetFieldDataFromString(SceneNode* root, const SceneNodeUtility::FieldContext& fieldCtx, const QString& data, int idx)
{
	auto fieldType = fieldCtx.FieldInfo->FieldType->Type;

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::Uint8:    SetFieldData<uchar >(fieldCtx, idx, data.toUShort()        ); break;
		case Definitions::ENodeFieldType::Uint16:   SetFieldData<ushort>(fieldCtx, idx, data.toUShort()        ); break;
		case Definitions::ENodeFieldType::Uint16_3: SetFieldData<ushort>(fieldCtx, idx, data.toUShort()        ); break;
		case Definitions::ENodeFieldType::Uint32:   SetFieldData<uint  >(fieldCtx, idx, data.toUInt()          ); break;
		case Definitions::ENodeFieldType::Int8:     SetFieldData<char  >(fieldCtx, idx, data.toShort()         ); break;
		case Definitions::ENodeFieldType::Int16:    SetFieldData<short >(fieldCtx, idx, data.toShort()         ); break;
		case Definitions::ENodeFieldType::Int32:    SetFieldData<int   >(fieldCtx, idx, data.toInt()           ); break;
		case Definitions::ENodeFieldType::Hex8:     SetFieldData<uchar >(fieldCtx, idx, data.toUShort(null, 16)); break;
		case Definitions::ENodeFieldType::Hex16:    SetFieldData<ushort>(fieldCtx, idx, data.toUShort(null, 16)); break;
		case Definitions::ENodeFieldType::Hex32:    SetFieldData<uint  >(fieldCtx, idx, data.toUInt  (null, 16)); break;
		case Definitions::ENodeFieldType::Float:    SetFieldData<float >(fieldCtx, idx, data.toFloat()         ); break;
		case Definitions::ENodeFieldType::Float2:   SetFieldData<float >(fieldCtx, idx, data.toFloat()         ); break;
		case Definitions::ENodeFieldType::Float3:   SetFieldData<float >(fieldCtx, idx, data.toFloat()         ); break;
		case Definitions::ENodeFieldType::Float4:   SetFieldData<float >(fieldCtx, idx, data.toFloat()         ); break;
		case Definitions::ENodeFieldType::Color:    SetFieldData<float >(fieldCtx, idx, data.toFloat()         ); break;

		case Definitions::ENodeFieldType::String:
		{
			auto lastData       = GetFieldDataAsString(fieldCtx)[idx];
			auto lastDataSize   = lastData.length() + 1;
			auto dataSize       = data.length() + 1;
			auto dataSizeOffset = dataSize - lastDataSize;

			QVector<SceneNode*> path;
			auto result = GetNodePath(path, root, fieldCtx.Node);
			Debug::Assert(result) << "Node not found in tree";

			SetFieldData(fieldCtx, data.toLatin1().constData(), dataSize);
			ApplyNodeSizeOffset(path, dataSizeOffset);
		}

		default: break;
	}
}

bool SceneNodeUtility::GetNodePath(QVector<SceneNode*>& path, SceneNode* parent, const SceneNode* node)
{
	if (parent == node)
	{
		path << parent;
		return true;
	}

	auto childs = parent->Childs;
	foreach (child, childs)
	{
		auto found = GetNodePath(path, *child, node);
		if (found == true)
		{
			path << parent;
			return true;
		}
	}

	return false;
}

void SceneNodeUtility::ApplyNodeSizeOffset(QVector<SceneNode *>& path, int offset)
{
	foreach (node, path)
	{
		(*node)->Size += offset;
	}
}
