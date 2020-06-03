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

	connect(m_Window, SIGNAL(FileCreated(int)),              this, SLOT(CreateDocument(int)));
	connect(m_Window, SIGNAL(FileOpened(QString)),           this, SLOT(LoadDocument(QString)));
	connect(m_Window, SIGNAL(FileReloaded(Document*)),       this, SLOT(ReloadDocument(Document*)));
	connect(m_Window, SIGNAL(FileSaved(Document*, QString)), this, SLOT(SaveDocument(Document*, QString)));
	connect(m_Window, SIGNAL(FileClosed(Document*)),         this, SLOT(CloseDocument(Document*)));

	m_Definitions = new Scene::Definitions();
}

Application::~Application()
{
	disconnect(m_Window, SIGNAL(FileCreated(int)),              this, SLOT(CreateDocument(int)));
	disconnect(m_Window, SIGNAL(FileOpened(QString)),           this, SLOT(LoadDocument(QString)));
	disconnect(m_Window, SIGNAL(FileReloaded(Document*)),       this, SLOT(ReloadDocument(Document*)));
	disconnect(m_Window, SIGNAL(FileSaved(Document*, QString)), this, SLOT(SaveDocument(Document*, QString)));
	disconnect(m_Window, SIGNAL(FileClosed(Document*)),         this, SLOT(CloseDocument(Document*)));

	delete m_Window;

	foreach (document, m_Documents)
	{
		delete *document;
	}
	m_Documents.clear();

	delete m_Definitions;
}

void Application::CreateDocument(int idx)
{
	auto document = new Document(this, idx);
	document->SetDirty(true);

	m_Documents.insert(document);
	m_Window->AddDocument(document, m_Definitions);
}

void Application::LoadDocument(const QString& file)
{
	auto document = new Document(this);
	document->Load(file, *m_Definitions);

	m_Documents.insert(document);
	m_Window->AddDocument(document, m_Definitions);
}

void Application::ReloadDocument(Document* document)
{
	document->Reload(*m_Definitions);
}

void Application::SaveDocument(Document* document, const QString& file)
{
	document->Save(file);
}

void Application::CloseDocument(Document* document)
{
	Debug::Assert(m_Documents.contains(document)) << "Document to close not found!";

	m_Documents.remove(document);
	delete document;
}
