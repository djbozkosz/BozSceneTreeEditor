#include <QFile>

#include "Application/Document.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneNodeSerializer.h"


using namespace Djbozkosz::Application;


Document::Document(QObject* parent, int idx) :
	QObject(parent),
	m_Idx(idx),
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

void Document::Reload(const Scene::Definitions& definitions)
{
	delete m_Root;

	Load(m_File, definitions);
}

void Document::Save(const QString& file)
{
	if (file.isEmpty() == false)
	{
		m_File = file;
	}

#ifdef RELEASE_BUILD
	QFile writer(m_File);
#else
	QFile writer("C:\\Users\\Tomáš\\Desktop\\scene2.bin");
#endif

	auto result = writer.open(QIODevice::WriteOnly);
	if (result == false)
		return;

	if (m_Root != null)
	{
		Scene::SceneNodeSerializer::Serialize(writer, *m_Root);
	}

	SetDirty(false);
}

void Document::SetDirty(bool isDirty)
{
	if (isDirty == m_IsDirty)
		return;

	m_IsDirty = isDirty;
	emit DirtyStateChanged(isDirty);
}
