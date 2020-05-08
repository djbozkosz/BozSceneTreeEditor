#include "Scene/SceneNode.h"
#include "Scene/SceneNodeDefinitions.h"


using namespace Djbozkosz::Application::Scene;


SceneNode::SceneNode(SceneNodeDefinitions* nodeDefinitions) :
	Type(0),
	Size(0),
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

	auto endPos     = startPos + Size;
	auto definition = m_NodeDefinitions->GetDefinition(Type);

	if (definition == null)
	{
		auto size = Size - sizeof(Type) - sizeof(Size);
		auto data = new uchar[size];

		auto result = LoadData(reader, data, size);
		if (result == false)
			return false; // assert

		Fields.push_back(data);

		return true;
	}

	auto fields = definition->Fields;
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

	if (definition->HasChilds == false || reader.pos() == endPos)
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

	// assert (reader.pos() == endPos)

	return true;
}

bool SceneNode::Save(QFile& writer) const
{
	unused(writer);

	return false;
}
