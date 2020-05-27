#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneNode::SceneNode() :
	Type(0),
	Size(0),
	Definition(null)
{
}

SceneNode::~SceneNode()
{
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

	foreach (child, Childs)
	{
		delete *child;
	}

	Fields.clear();
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
