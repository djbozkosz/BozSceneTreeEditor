#include <QFile>

#include "Scene/SceneTree.h"


using namespace Djbozkosz::Application::Scene;


SceneTree::SceneTree(const QString& file)
{
	if (file.isEmpty() == false)
	{
		Load(file);
	}
}

SceneTree::~SceneTree()
{
}

bool SceneTree::Load(const QString& file)
{
	QFile reader(file);

	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return false;

	return true;
}

bool SceneTree::Save(const QString& file)
{
	QFile writer(file);

	auto result = writer.open(QIODevice::WriteOnly);
	if (result == false)
		return false;

	return true;
}
