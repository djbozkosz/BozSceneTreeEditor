#include <QApplication>
#include <QFileDialog>

#include "ui_Window.h"

#include "GUI/DocumentWindow.h"
#include "GUI/Window.h"
#include "Application/Document.h"


using namespace Djbozkosz::Application::GUI;


Window::Window() :
	QMainWindow(),
	m_Ui(new Ui::Window())
{
	m_Ui->setupUi(this);

	connect(m_Ui->Menu_Open, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(m_Ui->Menu_Exit, SIGNAL(triggered()), this, SLOT(ExitApp()));
}

Window::~Window()
{
	disconnect(m_Ui->Menu_Open, SIGNAL(triggered()), this, SLOT(OpenFile()));
	disconnect(m_Ui->Menu_Exit, SIGNAL(triggered()), this, SLOT(ExitApp()));

	delete m_Ui;
}

void Window::AddDocument(Document* document)
{
	auto tab = new DocumentWindow(document, m_Ui->Tabs);
	m_Ui->Tabs->addTab(tab, document->GetFile());
}

void Window::OpenFile()
{
	/*auto file = QFileDialog::getOpenFileName(this, "Open file", "C:\\Hry\\Mafia\\missions", "scene2.bin");
	if (file.isEmpty() == true)
		return;

	emit FileOpened(file);*/

	emit FileOpened("C:\\Hry\\Mafia\\missions\\00menu\\scene2.bin");
}

void Window::ExitApp()
{
	qApp->quit();
}
