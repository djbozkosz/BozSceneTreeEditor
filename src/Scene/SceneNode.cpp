#include "Scene/SceneNode.h"
#include "Scene/SceneNodeUtility.h"


using namespace Djbozkosz::Application::Scene;


SceneNode::SceneNode() :
	Type(0),
	Size(0),
	Definition(null)
{
}

SceneNode::~SceneNode()
{
	SceneNodeUtility::DestroyFieldsData(Fields, (Definition != null) ? &Definition->Fields : null);

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
