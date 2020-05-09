#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneNode::SceneNode(SceneNodeDefinitions* nodeDefinitions) :
	Type(0),
	Size(0),
	Definition(null),
	m_NodeDefinitions(nodeDefinitions)
{
}

SceneNode::~SceneNode()
{
	foreach (field, Fields)
	{
		delete[] reinterpret_cast<uchar*>(*field);
	}

	foreach (child, Childs)
	{
		delete *child;
	}

	Childs.clear();
}

bool SceneNode::Load(QFile& reader)
{
	auto startPos = reader.pos();

	LoadData(reader, Type);
	LoadData(reader, Size);

	auto endPos = startPos + Size;
	Definition  = m_NodeDefinitions->GetDefinition(Type);

	if (Definition == null || (Definition->Fields.isEmpty() == false && Definition->Fields[0].FieldType->Type == SceneNodeDefinitions::ENodeFieldType::Unknown))
	{
		auto size = Size - sizeof(Type) - sizeof(Size);
		auto data = new uchar[size];

		auto result = LoadData(reader, data, size);
		Debug::Assert(result == true) << "Cannot load raw data for node" << Type;

		Fields.push_back(data);

		return true;
	}

	auto fields = Definition->Fields;
	foreach (field, fields)
	{
		auto type = field->FieldType->Type;
		auto data = default_(uchar*);

		if (type == SceneNodeDefinitions::ENodeFieldType::String)
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
		}
		else
		{
			auto size = field->FieldType->Size;
			data = new uchar[size];
			LoadData(reader, data, size);
		}

		Fields.push_back(data);
	}

	if (Definition->HasChilds == false || reader.pos() == endPos)
		return true;

	while (reader.pos() < endPos)
	{
		auto child  = new SceneNode(m_NodeDefinitions);
		auto result = child->Load(reader);
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

	return false;
}
