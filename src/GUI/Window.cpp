#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>
#include <QEvent>

#include "ui_Window.h"

#include "GUI/DocumentWindow.h"
#include "GUI/Window.h"
#include "Application/Document.h"


using namespace Djbozkosz::Application::GUI;


Window::Window() :
	QMainWindow(),
	m_Ui(new Ui::Window()),
	m_Status(null),
	m_Progress(null)
{
	m_Ui->setupUi(this);

	auto statusBar = m_Ui->StatusBar;

	m_Status   = new QLabel("Go to File... and open or create new file.");
	m_Progress = new QProgressBar();
	m_Progress->setMinimum(0);
	m_Progress->setMaximum(1000);
	m_Progress->setTextVisible(false);
	m_Progress->setVisible(false);

	statusBar->addWidget(m_Status, 2);
	statusBar->addWidget(m_Progress, 1);

	m_Ui->Tabs->installEventFilter(this);
	connect(m_Ui->Tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

	connect(m_Ui->Menu_New,    SIGNAL(triggered()), this, SLOT(NewFile()));
	connect(m_Ui->Menu_Open,   SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(m_Ui->Menu_Save,   SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(m_Ui->Menu_SaveAs, SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	connect(m_Ui->Menu_Close,  SIGNAL(triggered()), this, SLOT(CloseFile()));
	connect(m_Ui->Menu_Exit,   SIGNAL(triggered()), this, SLOT(ExitApp()));
	connect(m_Ui->Menu_About,  SIGNAL(triggered()), this, SLOT(ShowAbout()));
}

Window::~Window()
{
	disconnect(m_Ui->Menu_New,    SIGNAL(triggered()), this, SLOT(NewFile()));
	disconnect(m_Ui->Menu_Open,   SIGNAL(triggered()), this, SLOT(OpenFile()));
	disconnect(m_Ui->Menu_Save,   SIGNAL(triggered()), this, SLOT(SaveFile()));
	disconnect(m_Ui->Menu_SaveAs, SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	disconnect(m_Ui->Menu_Close,  SIGNAL(triggered()), this, SLOT(CloseFile()));
	disconnect(m_Ui->Menu_Exit,   SIGNAL(triggered()), this, SLOT(ExitApp()));
	disconnect(m_Ui->Menu_About,  SIGNAL(triggered()), this, SLOT(ShowAbout()));

	m_Ui->Tabs->removeEventFilter(this);
	disconnect(m_Ui->Tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

	delete m_Ui;
}

void Window::AddDocument(Document* document, Scene::Definitions* definitions)
{
	m_Status->setText("Loaded. Refreshing interface...");

	auto tab = new DocumentWindow(document, definitions, m_Ui->Tabs);

	connect(tab,      SIGNAL(ProgressChanged(float)), this, SLOT(UpdateProgress(float)));
	connect(document, SIGNAL(DirtyStateChanged(bool)), this, SLOT(UpdateDirtyState(bool)));

	tab->SetupTree();

	auto tabs = m_Ui->Tabs;

	tabs->addTab(tab, document->GetFile());
	tabs->setCurrentIndex(tabs->count() - 1);

	m_Ui->Menu_Save->setEnabled(true);
	m_Ui->Menu_SaveAs->setEnabled(true);
	m_Ui->Menu_Close->setEnabled(true);

	m_Status->setText("Done.");
}

void Window::RemoveDocumemt(Djbozkosz::Application::Document* document)
{
	auto tab    = default_(DocumentWindow*);
	auto tabIdx = -1;

	for (int idx = 0, count = m_Ui->Tabs->count(); idx < count; idx++)
	{
		auto tabToCheck = GetTab(idx);
		if (tabToCheck->GetDocument() == document)
		{
			tab    = tabToCheck;
			tabIdx = idx;
			break;
		}
	}

	Debug::Assert(tab != null) << "Tab to remove not found!";

	disconnect(tab,      SIGNAL(ProgressChanged(float)), this, SLOT(UpdateProgress(float)));
	disconnect(document, SIGNAL(DirtyStateChanged(bool)), this, SLOT(UpdateDirtyState(bool)));

	m_Ui->Tabs->removeTab(tabIdx);
	tab->deleteLater();
}

bool Window::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_Ui->Tabs && event->type() == QEvent::MouseButtonDblClick)
	{
		OpenFile();
	}

	return QObject::eventFilter(object, event);
}

void Window::NewFile()
{
	emit FileCreated();
}

void Window::OpenFile()
{
	auto file = QFileDialog::getOpenFileName(this, "Open file", "C:\\Hry\\Mafia\\missions");
	if (file.isEmpty() == true)
		return;

	emit FileOpened(file);
}

void Window::SaveFile()
{
	SaveDocument(true);
}

void Window::SaveAsFile()
{
	SaveDocument(false);
}

void Window::CloseFile()
{
	CloseFile(m_Ui->Tabs->currentIndex());
}

void Window::CloseFile(int idx)
{
	auto tab      = GetTab(idx);
	auto document = tab->GetDocument();

	RemoveDocumemt(document);
	emit FileClosed(document);
}

void Window::ExitApp()
{
	qApp->quit();
}

void Window::ShowAbout()
{
}

void Window::UpdateProgress(float value)
{
	auto wasVisible = m_Progress->isVisible();
	auto isVisible  = (value > 0.0f && value < 1.0f);

	if (wasVisible != isVisible)
	{
		m_Progress->setVisible(isVisible);
	}

	m_Progress->setValue(value * 1000.0f);
}

void Window::UpdateDirtyState(bool isDirty)
{
	auto tab      = GetCurrentTab();
	auto document = tab->GetDocument();
	auto file     = document->GetFile();

	auto tabs = m_Ui->Tabs;
	tabs->setTabText(tabs->currentIndex(), QString("%1%2").arg(file).arg(isDirty ? "*" : ""));
}

DocumentWindow* Window::GetTab(int idx) const
{
	return as(m_Ui->Tabs->widget(idx), DocumentWindow*);
}

DocumentWindow* Window::GetCurrentTab() const
{
	return GetTab(m_Ui->Tabs->currentIndex());
}

void Window::SaveDocument(bool replace)
{
	auto tab      = GetCurrentTab();
	auto document = tab->GetDocument();
	auto file     = document->GetFile();

	if (replace == false || file.isEmpty() == true)
	{
		file = QFileDialog::getSaveFileName(this, "Save file", "C:\\Hry\\Mafia\\missions");
	}

	if (file.isEmpty() == true)
		return;

	m_Status->setText("Saving...");
	emit FileSaved(document, file);
	m_Status->setText("Saved.");
}
