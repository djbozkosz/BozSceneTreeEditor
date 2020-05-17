#include <QFile>

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

	QFile reader(file);

	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return;

	m_Tree->Load(reader);
}

void Document::Save(const QString& file)
{
#if 0
	if (file.isEmpty() == false)
	{
		m_File = file;
	}

	QFile writer(m_File);
#else
	unused(file);
	QFile writer("C:\\Users\\Tomáš\\Desktop\\a.bin");
#endif

	auto result = writer.open(QIODevice::WriteOnly);
	if (result == false)
		return;

	m_Tree->Save(writer);
}
