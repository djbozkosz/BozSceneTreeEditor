#include <QFile>

#include "Scene/SceneTree.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneTree::SceneTree(SceneNodeDefinitions* nodeDefinitions, const QString& file) :
	m_NodeDefinitions(nodeDefinitions),
	m_Root(null)
{
	if (file.isEmpty() == false)
	{
		Load(file);
	}
}

SceneTree::~SceneTree()
{
	DeleteTree();
}

void SceneTree::DeleteTree()
{
	if (m_Root == null)
		return;

	delete m_Root;
	m_Root = null;
}

bool SceneTree::Load(const QString& file)
{
	QFile reader(file);

	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return false;

	m_Root = new SceneNode(m_NodeDefinitions);

	result = m_Root->Load(reader);
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
