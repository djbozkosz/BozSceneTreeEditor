#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>

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

	connect(m_Ui->Menu_Open, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(m_Ui->Menu_Save, SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(m_Ui->Menu_Exit, SIGNAL(triggered()), this, SLOT(ExitApp()));
}

Window::~Window()
{
	disconnect(m_Ui->Menu_Open, SIGNAL(triggered()), this, SLOT(OpenFile()));
	disconnect(m_Ui->Menu_Save, SIGNAL(triggered()), this, SLOT(SaveFile()));
	disconnect(m_Ui->Menu_Exit, SIGNAL(triggered()), this, SLOT(ExitApp()));

	delete m_Ui;
}

void Window::AddDocument(Document* document, Scene::Definitions* definitions)
{
	m_Status->setText("Loaded. Refreshing interface...");

	auto tab = new DocumentWindow(document, definitions, m_Ui->Tabs);
	connect(tab, SIGNAL(ProgressChanged(float)), this, SLOT(UpdateProgress(float)));
	tab->SetupTree();
	disconnect(tab, SIGNAL(ProgressChanged(float)), this, SLOT(UpdateProgress(float)));

	auto tabs = m_Ui->Tabs;

	tabs->addTab(tab, document->GetFile());
	tabs->setCurrentIndex(tabs->count() - 1);

	m_Status->setText("Done.");
}

void Window::OpenFile()
{
#if 1
	auto file = QFileDialog::getOpenFileName(this, "Open file", "C:\\Hry\\Mafia\\missions", "scene2.bin");
	if (file.isEmpty() == true)
		return;

	emit FileOpened(file);
#else
	emit FileOpened("C:\\Hry\\Mafia\\missions\\00menu\\scene2.bin");
#endif
}

void Window::SaveFile()
{
	auto tab      = as(m_Ui->Tabs->currentWidget(), DocumentWindow*);
	auto document = tab->GetDocument();
	auto file     = document->GetFile();

	if (file.isEmpty() == true)
	{
		file = QFileDialog::getSaveFileName(this, "Save file", "C:\\Hry\\Mafia\\missions", "scene2.bin");
	}

	if (file.isEmpty() == true)
		return;

	m_Status->setText("Saving...");
	emit FileSaved(document, file);
	m_Status->setText("Saved.");
}

void Window::ExitApp()
{
	qApp->quit();
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
