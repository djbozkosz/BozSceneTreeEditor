#include <QDir>
#include <QFileInfo>
#include <QSettings>

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
	QCoreApplication::setOrganizationName("djbozkosz");
	QCoreApplication::setApplicationName("BozSceneTreeEditor");

	auto arguments    = QCoreApplication::arguments();
	auto argDocuments = QStringList();

	for (int idx = 1, count = arguments.size(); idx < count; idx++)
	{
		auto argument = arguments[idx];
		auto isFile   = QFileInfo(argument).isFile();

		if (isFile == true)
		{
			argDocuments.push_back(argument);
		}
	}

	if (argDocuments.isEmpty() == false)
	{
		// restore the working dir back to app path
		// when the app is started via file "opwn with" or file drag & drop
		QDir::setCurrent(QCoreApplication::applicationDirPath());
	}

	m_Settings    = new QSettings();
	m_Definitions = new Scene::Definitions();

	m_Window      = new GUI::Window(m_Settings, m_Definitions);
	m_Window->show();

	connect(m_Window, SIGNAL(FileCreated(int)),              this, SLOT(CreateDocument(int)));
	connect(m_Window, SIGNAL(FileOpened(QString)),           this, SLOT(LoadDocument(QString)));
	connect(m_Window, SIGNAL(FileReloaded(Document*)),       this, SLOT(ReloadDocument(Document*)));
	connect(m_Window, SIGNAL(FileSaved(Document*, QString)), this, SLOT(SaveDocument(Document*, QString)));
	connect(m_Window, SIGNAL(FileClosed(Document*)),         this, SLOT(CloseDocument(Document*)));

	foreach (document, argDocuments)
	{
		m_Window->OpenFile(*document);
	}
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
	delete m_Settings;
}

void Application::CreateDocument(int idx)
{
	auto document = new Document(this, idx);
	document->SetDirty();

	m_Documents.insert(document);
	m_Window->AddDocument(document);
}

void Application::LoadDocument(const QString& file)
{
	auto document = new Document(this);
	document->Load(file, *m_Definitions);

	m_Documents.insert(document);
	m_Window->AddDocument(document);
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
