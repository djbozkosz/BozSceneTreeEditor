#include "Scene/SceneNodeUtility.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


QStringList SceneNodeUtility::GetFieldDataAsString(const SceneNodeUtility::FieldContext& fieldCtx)
{
	QStringList data;

	auto field     = fieldCtx.GetField();
	auto fieldInfo = fieldCtx.GetFieldInfo();
	auto fieldType = fieldInfo->FieldType->Type;

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

	auto fieldInfo = fieldCtx.GetFieldInfo();
	auto fieldType = fieldInfo->FieldType->Type;
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

void* SceneNodeUtility::ResizeFieldData(const SceneNodeUtility::FieldContext& fieldCtx, int dataSize)
{
	auto& field = fieldCtx.GetField();
	delete[] reinterpret_cast<uchar*>(field);
	field = new uchar[dataSize];
	return field;
}

void SceneNodeUtility::SetFieldDataFromString(SceneNode* root, const SceneNodeUtility::FieldContext& fieldCtx, const QString& data, int idx)
{
	NodePath path;
	auto result = GetNodePath(path, root, fieldCtx.Node);
	Debug::Assert(result) << "Node not found in tree!";

	auto field      = fieldCtx.GetField();
	auto fieldInfo  = fieldCtx.GetFieldInfo();
	auto fieldType  = fieldInfo->FieldType->Type;
	auto lastNumber = -1;
	auto number     = -1;

	if (fieldType == Definitions::ENodeFieldType::Uint32)
	{
		lastNumber = GetFieldData<uint>(fieldCtx, 0);
		number     = data.toUInt();
	}

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
			auto lastDataSize = GetFieldSize(field, fieldInfo, false);
			auto dataSize     = data.length() + 1;
			auto newData      = ResizeFieldData(fieldCtx, dataSize);
			memcpy(newData, data.toLatin1().constData(), dataSize);

			ApplyNodeSizeOffset(path, dataSize - lastDataSize);
			break;
		}

		case Definitions::ENodeFieldType::StringFixed:
		{
			auto fixedSize = (int)GetFieldSize(field, fieldInfo, false);
			auto dataSize  = data.length();

			if (dataSize >= fixedSize)
			{
				dataSize = fixedSize - 1;
			}

			auto field = fieldCtx.GetField();
			memset(field, 0, fixedSize);
			memcpy(field, data.toLatin1().constData(), dataSize);
			break;
		}

		case Definitions::ENodeFieldType::StringArray:
		case Definitions::ENodeFieldType::StringArray2:
		{
			auto dataTermSize   = (fieldType == Definitions::ENodeFieldType::StringArray2) ? 1 : 0;
			auto lastDataSize   = GetFieldSize(field, fieldInfo, false);
			auto dataSize       = data.length() + dataTermSize;
			auto newData        = ResizeFieldData(fieldCtx, dataSize + sizeof(uint));
			auto newDataInt     = reinterpret_cast<uint*>(newData);
			memcpy(&newDataInt[0], &dataSize, sizeof(uint));
			memcpy(&newDataInt[1], data.toLatin1().constData(), dataSize);

			ApplyNodeSizeOffset(path, dataSize - lastDataSize);
			break;
		}

		default: break;
	}

	if (fieldType == Definitions::ENodeFieldType::Uint32 && number != lastNumber)
	{
		auto offset = number - lastNumber;

		for (int idx = 0, count = fieldCtx.FieldInfos->size(); idx < count; idx++)
		{
			auto fieldInf = fieldCtx.GetFieldInfo(idx);

			if (fieldInf->FieldType->Type != Definitions::ENodeFieldType::Struct ||
				fieldInf->Number != fieldCtx.FieldIdx)
				continue;

			auto  field           = fieldCtx.GetField(idx);
			auto  struktArray     = reinterpret_cast<Definitions::StructField*>(field);
			auto& nestedFieldInfo = fieldInf->NestedField->Fields;
			auto  size            = 0;

			if (offset >= 0)
			{
				for (int i = 0; i < offset; i++)
				{
					QVector<void*> fields;
					size += CreateFieldsData(fields, nestedFieldInfo);
					struktArray->push_back(fields);
				}
			}
			else
			{
				for (int i = 0; i < -offset; i++)
				{
					size -= GetFieldsSize(struktArray->back(), nestedFieldInfo);
					struktArray->pop_back();
				}
			}


			ApplyNodeSizeOffset(path, size);
		}
	}
}

uint SceneNodeUtility::CreateFieldsData(QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos)
{
	auto size = 0;

	for (int idx = 0, count = fieldInfos.size(); idx < count; idx++)
	{
		auto fieldInfo = fieldInfos[idx];
		auto fieldType = fieldInfo.FieldType->Type;

		switch (fieldType)
		{
			case Definitions::ENodeFieldType::Struct:
			{
				fields.push_back(new Definitions::StructField());
				break;
			}

			default:
			{
				auto fieldSize = (fieldType == Definitions::ENodeFieldType::StringFixed) ? fieldInfo.Number : fieldInfo.FieldType->Size;
				auto field     = new uchar[fieldSize];

				memset(field, 0, fieldSize);

				if (fieldType == Definitions::ENodeFieldType::StringArray2)
				{
					auto fieldInt = reinterpret_cast<uint*>(field);
					*fieldInt = 1;
				}

				fields.push_back(field);
				size += fieldSize;
				break;
			}
		}
	}

	return size;
}

void SceneNodeUtility::DestroyFieldsData(QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>* fieldInfos)
{
	for (int idx = 0, count = fields.size(); idx < count; idx++)
	{
		auto field        = fields[idx];
		auto structFields = (fieldInfos != null) ? (*fieldInfos)[idx].NestedField : null;

		if (structFields == null)
		{
			delete[] reinterpret_cast<uchar*>(field);
			continue;
		}
		else
		{
			auto structArray = reinterpret_cast<Definitions::StructField*>(field);
			foreach (strukt, *structArray)
			{
				foreach (field, *strukt)
				{
					delete[] reinterpret_cast<uchar*>(*field);
				}
			}
		}
	}
}

uint SceneNodeUtility::GetFieldSize(const void* field, const Definitions::NodeFieldInfo* fieldInfo, bool withHeader)
{
	auto size      = withHeader ? (sizeof(SceneNode::Type) + sizeof(SceneNode::Size)) : 0;
	auto fieldType = fieldInfo->FieldType->Type;

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::String:
		{
			auto dataChar = reinterpret_cast<const char*>(field);
			return size + strlen(dataChar) + 1;
		}

		case Definitions::ENodeFieldType::StringArray:
			return size + *reinterpret_cast<const uint*>(field);

		case Definitions::ENodeFieldType::StringArray2:
			return size + sizeof(uint) + *reinterpret_cast<const uint*>(field);

		case Definitions::ENodeFieldType::StringFixed:
			return size + fieldInfo->Number;

		case Definitions::ENodeFieldType::Struct:
		{
			auto  struktArray = reinterpret_cast<const Definitions::StructField*>(field);
			auto& fieldInfs   = fieldInfo->NestedField->Fields;

			for (int idx = 0, count = struktArray->size(); idx < count; idx++)
			{
				size += GetFieldsSize((*struktArray)[idx], fieldInfs);
			}

			return size;
		}

		default:
			return size + fieldInfo->FieldType->Size;
	}
}

uint SceneNodeUtility::GetFieldsSize(const QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos)
{
	auto size = 0;

	for (int idx = 0, count = fieldInfos.size(); idx < count; idx++)
	{
		auto field     = fields[idx];
		auto fieldInfo = &fieldInfos[idx];
		size += GetFieldSize(field, fieldInfo, false);
	}

	return size;
}

QString SceneNodeUtility::GetNodeName(SceneNode* node, const Definitions* definitions)
{
	if (node->Definition == null)
		return QString("Unknown node: 0x%1").arg(node->Type, 4, 16);

	auto nameInfos = definitions->GetNodeNames(node->Type);
	if (nameInfos == null)
		return node->Definition->Name;

	QStringList names;

	foreach (nameInfo, *nameInfos)
	{
		auto childType    = nameInfo->ChildType;
		auto nodeWithName = (childType > 0) ? node->GetChild(childType) : node;

		if (nodeWithName == null)
			continue;

		auto fieldCtx   = FieldContext(nodeWithName, nameInfo->FieldIdx, &nodeWithName->Fields, &nodeWithName->Definition->Fields);
		auto nameFields = GetFieldDataAsString(fieldCtx);

		for (int idx = 0, count = nameFields.size(); idx < count; idx++)
		{
			auto enumValue = GetFieldDataEnum(definitions, fieldCtx, idx);
			if (enumValue.isEmpty() == false)
			{
				nameFields[idx] += QString(" - %1").arg(enumValue);
			}
		}

		names.push_back(nameFields.join(", "));
	}

	return QString("%1 [%2]").arg(node->Definition->Name, names.join("; "));
}

bool SceneNodeUtility::MoveNode(SceneNode* node, SceneNode* root, SceneNode* parent, SceneNode* newParent, uint oldIdx, uint newIdx)
{
	if (parent == null || newParent == null)
		return false;

	auto parentDefinition = newParent->Definition;
	if (parentDefinition == null || parentDefinition->HasChilds == false)
		return false;

	NodePath path;
	GetNodePath(path, root, node);
	path.removeFirst();
	ApplyNodeSizeOffset(path, -node->Size);

	parent->Childs.removeAt(oldIdx);
	newParent->Childs.insert(newIdx, node);

	path.clear();
	GetNodePath(path, root, node);
	path.removeFirst();
	ApplyNodeSizeOffset(path, node->Size);

	return true;
}

bool SceneNodeUtility::GetNodePath(NodePath& path, SceneNode* parent, const SceneNode* node)
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
