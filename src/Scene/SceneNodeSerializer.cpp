#include "Scene/SceneNodeSerializer.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneNode* SceneNodeSerializer::Deserialize(QIODevice& reader, SceneNode* parent, const Definitions& definitions)
{
	ushort nodeType = 0;
	auto   startPos = reader.pos();
	auto   result   = DeserializeData(reader, nodeType);

	if (result == false)
		return null;

	auto  sceneNode  = new SceneNode();
	auto& type       = sceneNode->Type;
	auto& size       = sceneNode->Size;
	auto& fields     = sceneNode->Fields;
	auto& definition = sceneNode->Definition;

	type = nodeType;
	DeserializeData(reader, size);

	auto endPos     = startPos + size;
	auto parentType = (parent != null) ? parent->Type : 0;

	auto field = definitions.GetNodeField(type);
	if (field != null)
	{
		auto sibling = parent->GetChild(field->SiblingType);
		Debug::Assert(sibling != null) << "Sibling" << QString::number(field->SiblingType, 16) << "is not found for node" << QString::number(type, 16);

		auto dataType = *reinterpret_cast<const int*>(sibling->Fields[field->SiblingFieldIdx]);
		definition    = definitions.GetNodeFieldData(type, dataType);
	}

	if (definition == null)
	{
		definition = definitions.GetNode(parentType, type);
	}

	if (definition == null)
	{
		definition = definitions.GetNode(0, type);
	}

	if (definition == null || (definition->Fields.isEmpty() == false && definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown))
	{
		auto dataSize = size - sizeof(type) - sizeof(size);
		auto data     = new uchar[dataSize];

		auto result = DeserializeData(reader, data, dataSize);
		Debug::Assert(result == true) << "Cannot deserialize raw data for node" << QString::number(type, 16);

		fields.push_back(data);

		return sceneNode;
	}

	DeserializeFields(reader, fields, definition->Fields);

	if (definition->HasChilds == false || reader.pos() == endPos)
		return sceneNode;

	while (reader.pos() < endPos)
	{
		auto child = Deserialize(reader, sceneNode, definitions);
		sceneNode->Childs.push_back(child);
	}

	Debug::Assert(reader.pos() == endPos) << "Data mismatch while deserialize child nodes for node" << QString::number(type, 16);

	return sceneNode;
}

void SceneNodeSerializer::Serialize(QIODevice& writer, const SceneNode& sceneNode)
{
	auto& type       = sceneNode.Type;
	auto& size       = sceneNode.Size;
	auto& fields     = sceneNode.Fields;
	auto& definition = sceneNode.Definition;

	SerializeData(writer, type);
	SerializeData(writer, size);

	if (definition == null || (definition->Fields.isEmpty() == false && definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown))
	{
		auto dataSize = size - sizeof(type) - sizeof(size);
		SerializeData(writer, fields[0], dataSize);
		return;
	}

	SerializeFields(writer, fields, definition->Fields);

	auto& childs = sceneNode.Childs;
	foreach (child, childs)
	{
		Serialize(writer, **child);
	}
}

void SceneNodeSerializer::DeserializeFields(QIODevice& reader, QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos)
{
	foreach (field, fieldInfos)
	{
		auto fieldType = field->FieldType->Type;
		auto data      = default_(uchar*);

		switch (fieldType)
		{
			case Definitions::ENodeFieldType::String:
			{
				QVector<uchar> str;
				uchar          c;

				do
				{
					DeserializeData(reader, c);
					str.push_back(c);
				}
				while (c != 0);

				auto size = str.size();
				data = new uchar[size];
				memcpy(data, str.data(), size);
				break;
			}

			case Definitions::ENodeFieldType::StringArray:
			case Definitions::ENodeFieldType::StringArray2:
			{
				auto dataTermSize = (fieldType == Definitions::ENodeFieldType::StringArray2) ? 1 : 0;
				auto lengthSize   = field->FieldType->Size - dataTermSize;
				uint arraySize;
				DeserializeData(reader, &arraySize, lengthSize);

				data = new uchar[lengthSize + arraySize];
				memcpy(data, &arraySize, lengthSize);
				DeserializeData(reader, &data[lengthSize], arraySize);
				break;
			}

			case Definitions::ENodeFieldType::StringFixed:
			{
				auto size = field->Number;
				data = new uchar[size];
				DeserializeData(reader, data, size);
				break;
			}

			case Definitions::ENodeFieldType::Struct:
			{
				auto sizeIdx = field->Number;
				auto size    = *reinterpret_cast<const uint*>(fields[sizeIdx]);

				auto structArray = new Definitions::StructField();
				structArray->reserve(size);

				for (uint idx = 0; idx < size; idx++)
				{
					structArray->push_back(QVector<void*>());
					DeserializeFields(reader, structArray->back(), field->NestedField->Fields);
				}

				data = reinterpret_cast<uchar*>(structArray);
				break;
			}

			default:
			{
				auto size = field->FieldType->Size;
				data = new uchar[size];
				DeserializeData(reader, data, size);
				break;
			}
		}

		fields.push_back(data);
	}
}

void SceneNodeSerializer::SerializeFields(QIODevice& writer, const QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos)
{
	for (int idx = 0, count = fields.size(); idx < count; idx++)
	{
		const auto* field     = fields[idx];
		const auto& fieldInfo = fieldInfos[idx];
		const auto  fieldType = fieldInfo.FieldType->Type;

		switch (fieldType)
		{
			case Definitions::ENodeFieldType::String:
			{
				auto size = strlen(reinterpret_cast<const char*>(field));
				SerializeData(writer, field, size + 1);
				break;
			}

			case Definitions::ENodeFieldType::StringArray:
			case Definitions::ENodeFieldType::StringArray2:
			{
				auto data = *reinterpret_cast<const uint*>(field);
				SerializeData(writer, field, data + sizeof(uint));
				break;
			}

			case Definitions::ENodeFieldType::StringFixed:
			{
				SerializeData(writer, field, fieldInfo.Number);
				break;
			}

			case Definitions::ENodeFieldType::Struct:
			{
				const auto& structArray = *reinterpret_cast<const Definitions::StructField*>(field);
				for (int idx = 0, count = structArray.size(); idx < count; idx++)
				{
					SerializeFields(writer, structArray[idx], fieldInfo.NestedField->Fields);
				}
				break;
			}

			default:
			{
				auto size = fieldInfo.FieldType->Size;
				SerializeData(writer, field, size);
				break;
			}
		}
	}
}
