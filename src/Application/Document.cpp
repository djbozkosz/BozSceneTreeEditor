#include "Application/Document.h"
#include "Scene/SceneTree.h"


using namespace Djbozkosz::Application;


Document::Document(Scene::Definitions* definitions) :
	m_Tree(new Scene::SceneTree(definitions))
{
}

Document::~Document()
{
	delete m_Tree;
}

void Document::Load(const QString& file)
{
	m_File = file;
	m_Tree->Load(file);
}
