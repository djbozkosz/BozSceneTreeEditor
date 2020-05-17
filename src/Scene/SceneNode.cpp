#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneNode::SceneNode(Definitions* definitions) :
	Type(0),
	Size(0),
	Definition(null),
	m_Definitions(definitions)
{
}

SceneNode::~SceneNode()
{
	Debug::Assert(false) << "Proper struct cleanup";

	for (int idx = 0, count = Fields.size(); idx < count; idx++)
	{
		auto field        = Fields[idx];
		auto structFields = (Definition != null) ? Definition->Fields[idx].NestedField : null;

		if (structFields == null)
		{
			delete[] reinterpret_cast<uchar*>(field);
			continue;
		}

		while (structFields != null)
		{
			auto structArray = reinterpret_cast<QVector<QVector<void*> >*>(field);
			foreach (strukt, *structArray)
			{
				foreach (field, *strukt)
				{
					delete[] reinterpret_cast<uchar*>(*field);
				}
			}
		}
	}

	foreach (child, Childs)
	{
		delete *child;
	}

	Childs.clear();
}

SceneNode* SceneNode::GetChild(ushort type) const
{
	foreach (child, Childs)
	{
		if ((*child)->Type == type)
			return *child;
	}

	return null;
}

bool SceneNode::Load(QFile& reader, SceneNode* parent)
{
	auto startPos = reader.pos();

	LoadData(reader, Type);
	LoadData(reader, Size);

	auto endPos     = startPos + Size;
	auto parentType = (parent != null) ? parent->Type : 0;

	auto field = m_Definitions->GetNodeField(Type);
	if (field != null)
	{
		auto sibling = parent->GetChild(field->SiblingType);
		Debug::Assert(sibling != null) << "Sibling" << field->SiblingType << "is not found for node" << Type;

		auto dataType = *reinterpret_cast<const int*>(sibling->Fields[field->SiblingFieldIdx]);
		Definition    = m_Definitions->GetNodeFieldData(Type, dataType);
	}

	if (Definition == null)
	{
		Definition = m_Definitions->GetNode(parentType, Type);
	}

	if (Definition == null)
	{
		Definition = m_Definitions->GetNode(0, Type);
	}

	if (Definition == null || (Definition->Fields.isEmpty() == false && Definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown))
	{
		auto size = Size - sizeof(Type) - sizeof(Size);
		auto data = new uchar[size];

		auto result = LoadData(reader, data, size);
		Debug::Assert(result == true) << "Cannot load raw data for node" << Type;

		Fields.push_back(data);

		return true;
	}

	LoadFields(reader, Fields, Definition->Fields);

	if (Definition->HasChilds == false || reader.pos() == endPos)
		return true;

	while (reader.pos() < endPos)
	{
		auto child  = new SceneNode(m_Definitions);
		auto result = child->Load(reader, this);
		if (result == false)
		{
			delete child;
			return false;
		}

		Childs.push_back(child);
	}

	Debug::Assert(reader.pos() == endPos) << "Data mismatch while load child nodes for node" << Type;

	return true;
}

bool SceneNode::Save(QFile& writer) const
{
	unused(writer);

	SaveData(writer, Type);
	SaveData(writer, Size);

	if (Definition == null || (Definition->Fields.isEmpty() == false && Definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown))
	{
		auto size = Size - sizeof(Type) - sizeof(Size);
		SaveData(writer, Fields[0], size);
		return true;
	}

	SaveFields(writer, Fields, Definition->Fields);

	foreach (child, Childs)
	{
		auto result = (*child)->Save(writer);
		if (result == false)
			return false;
	}

	return true;
}

void SceneNode::LoadFields(QFile& reader, QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos)
{
	foreach (field, fieldInfos)
	{
		auto type = field->FieldType->Type;
		auto data = default_(uchar*);

		switch (type)
		{
			case Definitions::ENodeFieldType::String:
			{
				QVector<uchar> str;
				uchar          c;

				do
				{
					LoadData(reader, c);
					str.push_back(c);
				}
				while (c != 0);

				auto size = str.size();
				data = new uchar[size];
				memcpy(data, str.data(), size);
				break;
			}

			case Definitions::ENodeFieldType::StringArray:
			{
				auto lengthSize = field->FieldType->Size;
				uint arraySize;
				LoadData(reader, &arraySize, lengthSize);

				data = new uchar[lengthSize + arraySize];
				memcpy(data, &arraySize, lengthSize);
				LoadData(reader, &data[lengthSize], arraySize);
				break;
			}

			case Definitions::ENodeFieldType::StringFixed:
			{
				auto size = field->Number;
				data = new uchar[size];
				LoadData(reader, data, size);
				break;
			}

			case Definitions::ENodeFieldType::Struct:
			{
				auto sizeIdx = field->Number;
				auto size    = *reinterpret_cast<const uint*>(fields[sizeIdx]);

				auto structArray = new QVector<QVector<void*> >();
				structArray->reserve(size);

				for (uint idx = 0; idx < size; idx++)
				{
					structArray->push_back(QVector<void*>());
					LoadFields(reader, structArray->back(), field->NestedField->Fields);
				}

				data = reinterpret_cast<uchar*>(structArray);
				break;
			}

			default:
			{
				auto size = field->FieldType->Size;
				data = new uchar[size];
				LoadData(reader, data, size);
				break;
			}
		}

		fields.push_back(data);
	}
}

void SceneNode::SaveFields(QFile& writer, const QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos)
{
	for (int idx = 0, count = fields.size(); idx < count; idx++)
	{
		const auto* field     = fields[idx];
		const auto& fieldInfo = fieldInfos[idx];
		const auto  type      = fieldInfo.FieldType->Type;

		switch (type)
		{
			case Definitions::ENodeFieldType::String:
			{
				auto size = strlen(reinterpret_cast<const char*>(field));
				SaveData(writer, field, size + 1);
				break;
			}

			case Definitions::ENodeFieldType::StringArray:
			{
				auto data = *reinterpret_cast<const uint*>(field);
				SaveData(writer, field, data + sizeof(uint));
				break;
			}

			case Definitions::ENodeFieldType::StringFixed:
			{
				SaveData(writer, field, fieldInfo.Number);
				break;
			}

			case Definitions::ENodeFieldType::Struct:
			{
				const auto& structArray = *reinterpret_cast<const QVector<QVector<void*> >*>(field);
				for (int idx = 0, count = structArray.size(); idx < count; idx++)
				{
					SaveFields(writer, structArray[idx], fieldInfo.NestedField->Fields);
				}
				break;
			}

			default:
			{
				auto size = fieldInfo.FieldType->Size;
				SaveData(writer, field, size);
				break;
			}
		}
	}
}
