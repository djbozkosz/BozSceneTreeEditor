#include <QFile>

#include "Application/Document.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneNodeSerializer.h"


using namespace Djbozkosz::Application;


Document::Document(QObject* parent) :
	QObject(parent),
	m_Root(null),
	m_IsDirty(false)
{
}

Document::~Document()
{
	delete m_Root;
	m_Root = null;
}

void Document::Load(const QString& file, const Scene::Definitions& definitions)
{
	m_File = file;

	QFile reader(file);

	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return;

	m_Root = Scene::SceneNodeSerializer::Deserialize(reader, null, definitions);
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

	Scene::SceneNodeSerializer::Serialize(writer, *m_Root);
	SetDirty(false);
}

void Document::SetDirty(bool isDirty)
{
	if (isDirty == m_IsDirty)
		return;

	m_IsDirty = isDirty;
	emit DirtyStateChanged(isDirty);
}
