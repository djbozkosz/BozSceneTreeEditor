#include <QApplication>
#include <QFileDialog>

#include "ui_Window.h"

#include "GUI/Window.h"


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

void Window::OpenFile()
{
	auto file = QFileDialog::getOpenFileName(this, "Open file", "C:\\Hry\\Mafia\\missions", "scene2.bin");
	if (file.isEmpty() == true)
		return;

	emit FileOpened(file);
}

void Window::ExitApp()
{
	qApp->quit();
}
