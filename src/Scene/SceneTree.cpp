#include <QFile>

#include "Scene/SceneTree.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneNodeSerializer.h"


using namespace Djbozkosz::Application::Scene;


SceneTree::SceneTree() :
	Root(null)
{
}

SceneTree::~SceneTree()
{
	DeleteTree();
}

void SceneTree::DeleteTree()
{
	delete Root;
	Root = null;
}

void SceneTree::Load(QFile& reader, const Definitions& definitions)
{
	Root = SceneNodeSerializer::Deserialize(reader, null, definitions);
}

void SceneTree::Save(QFile& writer) const
{
	SceneNodeSerializer::Serialize(writer, *Root);
}
