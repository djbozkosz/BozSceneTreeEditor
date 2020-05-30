#include <QEvent>
#include <QCloseEvent>
#include <QLabel>
#include <QProgressBar>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_Window.h"

#include "GUI/DocumentWindow.h"
#include "GUI/Window.h"
#include "Application/Document.h"


using namespace Djbozkosz::Application::GUI;


Window::Window() :
	QMainWindow(),
	m_NewFileCounter(1),
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

	auto tabs = m_Ui->Tabs;
	tabs->installEventFilter(this);
	connect(tabs, SIGNAL(currentChanged(int)),    this, SLOT(UpdateEditMenu(int)));
	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

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

	auto tabs = m_Ui->Tabs;
	tabs->removeEventFilter(this);
	disconnect(tabs, SIGNAL(currentChanged(int)),    this, SLOT(UpdateEditMenu(int)));
	disconnect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

	delete m_Ui;
}

void Window::AddDocument(Document* document, Scene::Definitions* definitions)
{
	m_Status->setText("Loaded. Refreshing interface...");

	auto tab = new DocumentWindow(document, definitions, m_Ui->Menu_Edit->menuAction()->menu(), m_Ui->Tabs);

	connect(tab,      SIGNAL(ProgressChanged(float)), this, SLOT(UpdateProgress(float)));
	connect(document, SIGNAL(DirtyStateChanged(bool)), this, SLOT(UpdateDirtyState(bool)));

	tab->SetupTree();

	auto tabs = m_Ui->Tabs;

	tabs->addTab(tab, GetFileName(document));
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

	auto tabs = m_Ui->Tabs;
	tabs->removeTab(tabIdx);
	tab->deleteLater();

	if (tabs->count() == 0)
	{
		m_Ui->Menu_Save->setEnabled(false);
		m_Ui->Menu_SaveAs->setEnabled(false);
		m_Ui->Menu_Close->setEnabled(false);

		auto actions = m_Ui->Menu_Edit->actions();
		foreach (action, actions)
		{
			(*action)->setEnabled(false);
		}
	}
}

bool Window::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_Ui->Tabs && event->type() == QEvent::MouseButtonDblClick)
	{
		OpenFile();
	}

	return QObject::eventFilter(object, event);
}

void Window::closeEvent(QCloseEvent* event)
{
	if (ExitApp() == true)
	{
		event->accept();
		QMainWindow::closeEvent(event);
	}
	else
	{
		event->ignore();
	}
}

void Window::NewFile()
{
	emit FileCreated(m_NewFileCounter++);
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
	SaveDocument(GetCurrentTab(), true);
}

void Window::SaveAsFile()
{
	SaveDocument(GetCurrentTab(), false);
}

bool Window::CloseFile()
{
	return CloseFile(m_Ui->Tabs->currentIndex());
}

bool Window::CloseFile(int idx)
{
	auto tab      = GetTab(idx);
	auto document = tab->GetDocument();
	auto result   = 1;

	if (document->IsDirty() == true)
	{
		result = QMessageBox::question(this, windowTitle(), QString("Save changed file \"%1\"?").arg(GetFileName(document, false)), "&Yes", "&No", "&Cancel", 0);
	}

	if (result == 2)
		return false;

	if (result == 0)
	{
		SaveDocument(tab, true);
	}

	RemoveDocumemt(document);
	emit FileClosed(document);

	return true;
}

bool Window::ExitApp()
{
	auto tabs = m_Ui->Tabs;
	while (tabs->count() > 0)
	{
		if (CloseFile() == false)
			return false;
	}

	qApp->quit();
	return true;
}

void Window::ShowAbout()
{
	QMessageBox::about(this, windowTitle(), "todo");
}

void Window::UpdateEditMenu(int tabIdx)
{
	if (tabIdx == -1)
		return;

	GetTab(tabIdx)->UpdateEditMenu();
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
	unused(isDirty);

	auto tab      = GetCurrentTab();
	auto document = tab->GetDocument();

	auto tabs = m_Ui->Tabs;
	tabs->setTabText(tabs->currentIndex(), GetFileName(document));
}

DocumentWindow* Window::GetTab(int idx) const
{
	return as(m_Ui->Tabs->widget(idx), DocumentWindow*);
}

DocumentWindow* Window::GetCurrentTab() const
{
	return GetTab(m_Ui->Tabs->currentIndex());
}

void Window::SaveDocument(DocumentWindow* tab, bool replace)
{
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

QString Window::GetFileName(Document* document, bool useDirtyState)
{
	auto fileName = document->GetFile();
	if (fileName.isEmpty() == true)
	{
		fileName = QString("new_file_%1").arg(document->GetIdx());
	}

	if (useDirtyState == false || document->IsDirty() == false)
		return fileName;

	return QString("%1*").arg(fileName);
}
