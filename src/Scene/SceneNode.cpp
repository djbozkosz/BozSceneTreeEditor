#include <QFile>

#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::Scene;


SceneNode::SceneNode()
{
}

SceneNode::~SceneNode()
{
}

bool SceneNode::Load(const QFile& reader)
{
	unused(reader);

	return false;
}

bool SceneNode::Save(const QFile& writer)
{
	unused(writer);

	return false;
}
