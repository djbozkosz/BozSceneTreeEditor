#include "Application/Document.h"
#include "Scene/SceneTree.h"


using namespace Djbozkosz::Application;


Document::Document(Scene::SceneNodeDefinitions* nodeDefinitions, const QString& file)
{
	m_Tree = new Scene::SceneTree(nodeDefinitions, file);
}

Document::~Document()
{
	delete m_Tree;
}
