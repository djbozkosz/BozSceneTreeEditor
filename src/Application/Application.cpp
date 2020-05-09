#include "Application/Application.h"
#include "Application/Document.h"
#include "GUI/Window.h"
#include "Scene/SceneNodeDefinitions.h"


using namespace Djbozkosz::Application;


int main(int argc, char* argv[])
{
	Application application(argc, argv);
	return application.exec();
}


Application::Application(int argc, char* argv[]) : QApplication(argc, argv)
{
	m_Window = new GUI::Window();
	m_Window->show();

	connect(m_Window, SIGNAL(FileOpened(QString)), this, SLOT(LoadDocument(QString)));

	m_NodeDefinitions = new Scene::SceneNodeDefinitions();
}

Application::~Application()
{
	disconnect(m_Window, SIGNAL(FileOpened(QString)), this, SLOT(LoadDocument(QString)));

	delete m_Window;
	delete m_NodeDefinitions;
}

void Application::LoadDocument(const QString& file)
{
	auto document     = new Document(m_NodeDefinitions);
	document->Load(file);

	m_Documents[file] = document;
	m_Window->AddDocument(document);
}

void Application::CloseDocument(const QString& file)
{
	auto document = m_Documents[file];
	m_Documents.remove(file);
	delete document;
}
