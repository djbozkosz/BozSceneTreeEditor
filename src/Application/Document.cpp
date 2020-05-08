#include "Application/Document.h"
#include "Scene/SceneTree.h"


using namespace Djbozkosz::Application;


Document::Document(const QString& file)
{
	m_Tree = new Scene::SceneTree(file);
}

Document::~Document()
{
	delete m_Tree;
}
