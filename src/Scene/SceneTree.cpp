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

bool SceneTree::Load(const QString& file)
{
	QFile reader(file);

	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return false;

	Root = new SceneNode(m_Definitions);

	result = Root->Load(reader, null);
	if (result == false)
	{
		DeleteTree();
		return false;
	}

	return true;
}

bool SceneTree::Save(const QString& file) const
{
	QFile writer(file);

	auto result = writer.open(QIODevice::WriteOnly);
	if (result == false)
		return false;

	return true;
}
