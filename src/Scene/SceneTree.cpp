#include <QFile>

#include "Scene/SceneTree.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneTree::SceneTree(Definitions* definitions) :
	Root(null),
	m_Definitions(definitions)
{
}

SceneTree::~SceneTree()
{
	DeleteTree();
}

void SceneTree::DeleteTree()
{
	if (Root == null)
		return;

	delete Root;
	Root = null;
}

bool SceneTree::Load(QFile& reader)
{
	Root = new SceneNode(m_Definitions);

	auto result = Root->Load(reader, null);
	if (result == false)
	{
		DeleteTree();
		return false;
	}

	return true;
}

bool SceneTree::Save(QFile& writer) const
{
	return Root->Save(writer);
}
