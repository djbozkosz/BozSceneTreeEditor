#include "Application/Application.h"
#include "Application/Document.h"
#include "GUI/Window.h"
#include "Scene/Definitions.h"


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

	connect(m_Window, SIGNAL(FileOpened(QString)),           this, SLOT(LoadDocument(QString)));
	connect(m_Window, SIGNAL(FileSaved(Document*, QString)), this, SLOT(SaveDocument(Document*, QString)));

	m_Definitions = new Scene::Definitions();
}

Application::~Application()
{
	disconnect(m_Window, SIGNAL(FileOpened(QString)),           this, SLOT(LoadDocument(QString)));
	disconnect(m_Window, SIGNAL(FileSaved(Document*, QString)), this, SLOT(SaveDocument(Document*, QString)));
	delete m_Window;

	foreach (document, m_Documents)
	{
		(*document)->deleteLater();
	}
	m_Documents.clear();

	delete m_Definitions;
}

void Application::LoadDocument(const QString& file)
{
	auto document = new Document(this);
	document->Load(file, *m_Definitions);

	m_Documents[file] = document;
	m_Window->AddDocument(document, m_Definitions);
}

void Application::SaveDocument(Document* document, const QString& file)
{
	document->Save(file);
}

void Application::CloseDocument(const QString& file)
{
	auto document = m_Documents[file];
	m_Documents.remove(file);
	delete document;
}
