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

	connect(m_Window, SIGNAL(FileOpened(QString)), this, SLOT(CreateDocument(QString)));

	m_NodeDefinitions = new Scene::SceneNodeDefinitions();
}

Application::~Application()
{
	disconnect(m_Window, SIGNAL(FileOpened(QString)), this, SLOT(CreateDocument(QString)));

	delete m_Window;
	delete m_NodeDefinitions;
}

void Application::CreateDocument(const QString& file)
{
	m_Documents[file] = new Document(m_NodeDefinitions, file);
}

void Application::CloseDocument(const QString& file)
{
	auto document = m_Documents[file];
	m_Documents.remove(file);
	delete document;
}
